#include "dist_sens.h"
#include "vl53lx_platform.h"
#include "vl53lx_platform_init.h"
#include "vl53lx_api.h"

#define VL53_I2C_ADDR_7BIT     (0x29)   // PSoC HAL automatically handles write (0x52) / read (0x53)
#define VL53_REG_MODEL_ID      (0x010F) // Expected: 0xEA
#define VL53_REG_MODULE_TYPE   (0x0110) // Expected: 0xAA

static cyhal_i2c_t *g_dist_i2c_handle = NULL;
static VL53LX_Dev_t g_vl53_dev;
static bool g_vl53_dev_initialized = false;
static bool g_vl53_dev_data_initialized = false;
static volatile bool g_dist_data_ready = false;

static void dist_sens_gpio1_isr(void *callback_arg, cyhal_gpio_event_t event)
{
    (void)callback_arg;
    (void)event;
    g_dist_data_ready = true;
}

static const cyhal_i2c_cfg_t vl53_i2c_config = {
    .is_slave        = false,
    .address         = 0,
    .frequencyhal_hz = 400000 // 400 kHz Fast Mode
};

// 1. Hardware Bus Initialization
cyhal_i2c_t *proto_i2c_init(cyhal_gpio_t sda, cyhal_gpio_t scl)
{
    static cyhal_i2c_t i2c_obj;
    cy_rslt_t result;

    result = cyhal_i2c_init(&i2c_obj, sda, scl, NULL);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
        return NULL;
    }

    result = cyhal_i2c_configure(&i2c_obj, &vl53_i2c_config);
    if (result != CY_RSLT_SUCCESS)
    {
        cyhal_i2c_free(&i2c_obj);
        CY_ASSERT(0);
        return NULL;
    }

    g_dist_i2c_handle = &i2c_obj;
    return &i2c_obj;
}

cyhal_i2c_t *dist_sens_get_i2c_handle(void)
{
    return g_dist_i2c_handle;
}

// 2. Write Data to a 16-bit Register Index
cy_rslt_t i2c_write(cyhal_i2c_t *obj, uint16_t reg_index, const uint8_t *p_data, uint16_t len)
{
    uint8_t tx_buf[2 + len];

    // Split 16-bit register index into 2 High/Low bytes
    tx_buf[0] = (uint8_t)(reg_index >> 8);
    tx_buf[1] = (uint8_t)(reg_index & 0xFF);

    // Copy payload data into buffer after index
    for (uint16_t i = 0; i < len; i++)
    {
        tx_buf[2 + i] = p_data[i];
    }

    // Perform I2C Write
    cy_rslt_t result = cyhal_i2c_master_write(obj, VL53_I2C_ADDR_7BIT, tx_buf, 2 + len, pdMS_TO_TICKS(500), true);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
    return result;
}

// 3. Read Data from a 16-bit Register Index
cy_rslt_t i2c_read(cyhal_i2c_t *obj, uint16_t reg_index, uint8_t *p_dest_buffer, uint16_t len)
{
    uint8_t reg_buf[2];
    reg_buf[0] = (uint8_t)(reg_index >> 8);
    reg_buf[1] = (uint8_t)(reg_index & 0xFF);

    // Step A: Send 16-bit register index (send_stop = false for Repeated START)
    cy_rslt_t result = cyhal_i2c_master_write(obj, VL53_I2C_ADDR_7BIT, reg_buf, 2, pdMS_TO_TICKS(500), false);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
        return result;
    }

    // Step B: Read length bytes into the destination buffer
    result = cyhal_i2c_master_read(obj, VL53_I2C_ADDR_7BIT, p_dest_buffer, len, pdMS_TO_TICKS(500), true);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    return result;
}

// 4. Test Function: Reads real values from the Model ID and Module Type registers
bool vl53l3cx_test_connection(cyhal_i2c_t *obj)
{
    uint8_t model_id = 0;
    uint8_t module_type = 0;

    // Read Model ID (1 byte from 0x010F)
    cy_rslt_t res1 = i2c_read(obj, VL53_REG_MODEL_ID, &model_id, 1);
    
    // Read Module Type (1 byte from 0x0110)
    cy_rslt_t res2 = i2c_read(obj, VL53_REG_MODULE_TYPE, &module_type, 1);

    if (res1 == CY_RSLT_SUCCESS && res2 == CY_RSLT_SUCCESS)
    {
        printf("Read Model ID: 0x%02X (Expected: 0xEA)\r\n", model_id);
        printf("Read Module Type: 0x%02X (Expected: 0xAA)\r\n", module_type);

        if (model_id == 0xEA && module_type == 0xAA)
        {
            printf("Hardware Test PASSED! Sensor is working.\r\n");
            return true;
        }
    }

    printf("Hardware Test FAILED! Check power and wiring.\r\n");
    return false;
}

bool dist_sens_st_init(void)
{
    VL53LX_Error status;

    if (g_vl53_dev_initialized)
    {
        return true;
    }

    memset(&g_vl53_dev, 0, sizeof(g_vl53_dev));
    g_vl53_dev.i2c_slave_address = VL53_I2C_ADDR_7BIT;
    g_vl53_dev.comms_type = 0;
    g_vl53_dev.comms_speed_khz = 400;

    status = VL53LX_platform_init(&g_vl53_dev,
                                 g_vl53_dev.i2c_slave_address,
                                 g_vl53_dev.comms_type,
                                 g_vl53_dev.comms_speed_khz);
    if (status != VL53LX_ERROR_NONE)
    {
        printf("ST platform init failed: %d\r\n", (int)status);
        VL53LX_platform_terminate(&g_vl53_dev);
        return false;
    }

    printf("ST platform initialized.\r\n");

    status = VL53LX_WaitDeviceBooted(&g_vl53_dev);
    if (status != VL53LX_ERROR_NONE)
    {
        printf("ST WaitDeviceBooted failed: %d\r\n", (int)status);
        VL53LX_platform_terminate(&g_vl53_dev);
        return false;
    }

    printf("ST WaitDeviceBooted PASSED.\r\n");
    g_vl53_dev_initialized = true;
    g_vl53_dev_data_initialized = false;
    return true;
}

void dist_sens_st_shutdown(void)
{
    if (!g_vl53_dev_initialized)
    {
        return;
    }

    VL53LX_platform_terminate(&g_vl53_dev);
    g_vl53_dev_initialized = false;
    g_vl53_dev_data_initialized = false;
}

bool dist_sens_st_poll_distance_mm(uint16_t *distance_mm)
{
    VL53LX_MultiRangingData_t ranging_data;
    uint8_t data_ready = 0;
    VL53LX_Error status;
    uint8_t attempt;

    if (distance_mm == NULL)
    {
        return false;
    }

    if (!dist_sens_st_init())
    {
        return false;
    }

    if (!g_vl53_dev_data_initialized)
    {
        status = VL53LX_DataInit(&g_vl53_dev);
        if (status != VL53LX_ERROR_NONE)
        {
            printf("ST DataInit failed: %d\r\n", (int)status);
            dist_sens_st_shutdown();
            return false;
        }

        status = VL53LX_SetDistanceMode(&g_vl53_dev, VL53LX_DISTANCEMODE_SHORT);
        if (status != VL53LX_ERROR_NONE)
        {
            printf("ST SetDistanceMode failed: %d\r\n", (int)status);
            dist_sens_st_shutdown();
            return false;
        }

        status = VL53LX_SetMeasurementTimingBudgetMicroSeconds(&g_vl53_dev, 20000U);
        if (status != VL53LX_ERROR_NONE)
        {
            printf("ST timing budget failed: %d\r\n", (int)status);
            dist_sens_st_shutdown();
            return false;
        }

        g_vl53_dev_data_initialized = true;
        printf("ST ranging initialized.\r\n");
    }

    status = VL53LX_ClearInterruptAndStartMeasurement(&g_vl53_dev);
    if (status != VL53LX_ERROR_NONE)
    {
        printf("ST StartMeasurement failed: %d\r\n", (int)status);
        return false;
    }

    for (attempt = 0; attempt < 50U; attempt++)
    {
        status = VL53LX_GetMeasurementDataReady(&g_vl53_dev, &data_ready);
        if (status == VL53LX_ERROR_NONE && data_ready != 0U)
        {
            break;
        }

        cyhal_system_delay_ms(10U);
    }

    if (status != VL53LX_ERROR_NONE || data_ready == 0U)
    {
        printf("ST distance poll timeout: status=%d data_ready=%d\r\n", (int)status, (int)data_ready);
        return false;
    }

    status = VL53LX_GetMultiRangingData(&g_vl53_dev, &ranging_data);
    if (status != VL53LX_ERROR_NONE)
    {
        printf("ST GetMultiRangingData failed: %d\r\n", (int)status);
        return false;
    }

    if (ranging_data.NumberOfObjectsFound == 0U)
    {
        *distance_mm = 0U;
        return false;
    }

    if (ranging_data.RangeData[0].RangeStatus != VL53LX_RANGESTATUS_RANGE_VALID &&
        ranging_data.RangeData[0].RangeStatus != VL53LX_RANGESTATUS_RANGE_VALID_MIN_RANGE_CLIPPED &&
        ranging_data.RangeData[0].RangeStatus != VL53LX_RANGESTATUS_RANGE_VALID_MERGED_PULSE)
    {
        printf("ST invalid range status: %u\r\n", ranging_data.RangeData[0].RangeStatus);
        *distance_mm = 0U;
        return false;
    }

    *distance_mm = (uint16_t)ranging_data.RangeData[0].RangeMilliMeter;
    return true;
}

bool dist_sens_st_configure_gpio1_interrupt(void)
{
    cy_rslt_t result;

    if (!dist_sens_st_init())
    {
        return false;
    }

    if (!g_vl53_dev_data_initialized)
    {
        VL53LX_Error status = VL53LX_DataInit(&g_vl53_dev);
        if (status != VL53LX_ERROR_NONE)
        {
            printf("ST DataInit failed: %d\r\n", (int)status);
            dist_sens_st_shutdown();
            return false;
        }

        status = VL53LX_SetDistanceMode(&g_vl53_dev, VL53LX_DISTANCEMODE_SHORT);
        if (status != VL53LX_ERROR_NONE)
        {
            printf("ST SetDistanceMode failed: %d\r\n", (int)status);
            dist_sens_st_shutdown();
            return false;
        }

        status = VL53LX_SetMeasurementTimingBudgetMicroSeconds(&g_vl53_dev, 20000U);
        if (status != VL53LX_ERROR_NONE)
        {
            printf("ST timing budget failed: %d\r\n", (int)status);
            dist_sens_st_shutdown();
            return false;
        }

        g_vl53_dev_data_initialized = true;
    }

    g_dist_data_ready = false;

    result = cyhal_gpio_init(DIST_GPIO1,
                             CYHAL_GPIO_DIR_INPUT,
                             CYHAL_GPIO_DRIVE_NONE,
                             false);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("GPIO1 init failed: %lu\r\n", (unsigned long)result);
        return false;
    }

    cyhal_gpio_callback_data_t gpio1_cb = {
        .callback = dist_sens_gpio1_isr,
        .callback_arg = NULL,
    };

    cyhal_gpio_register_callback(DIST_GPIO1, &gpio1_cb);
    cyhal_gpio_enable_event(DIST_GPIO1,
                            CYHAL_GPIO_IRQ_FALL,
                            CYHAL_ISR_PRIORITY_DEFAULT,
                            true);

    return VL53LX_ClearInterruptAndStartMeasurement(&g_vl53_dev) == VL53LX_ERROR_NONE;
}

bool dist_sens_st_read_distance_from_interrupt(uint16_t *distance_mm)
{
    VL53LX_MultiRangingData_t ranging_data;
    VL53LX_Error status;

    if (distance_mm == NULL)
    {
        return false;
    }

    if (!g_vl53_dev_initialized)
    {
        return false;
    }

    while (!g_dist_data_ready)
    {
        __WFI();
    }

    g_dist_data_ready = false;

    status = VL53LX_GetMultiRangingData(&g_vl53_dev, &ranging_data);
    if (status != VL53LX_ERROR_NONE)
    {
        printf("ST GetMultiRangingData failed: %d\r\n", (int)status);
        return false;
    }

    if (ranging_data.NumberOfObjectsFound == 0U)
    {
        *distance_mm = 0U;
        return false;
    }

    if (ranging_data.RangeData[0].RangeStatus != VL53LX_RANGESTATUS_RANGE_VALID &&
        ranging_data.RangeData[0].RangeStatus != VL53LX_RANGESTATUS_RANGE_VALID_MIN_RANGE_CLIPPED &&
        ranging_data.RangeData[0].RangeStatus != VL53LX_RANGESTATUS_RANGE_VALID_MERGED_PULSE)
    {
        printf("ST invalid range status: %u\r\n", ranging_data.RangeData[0].RangeStatus);
        *distance_mm = 0U;
        return false;
    }

    *distance_mm = (uint16_t)ranging_data.RangeData[0].RangeMilliMeter;

    status = VL53LX_ClearInterruptAndStartMeasurement(&g_vl53_dev);
    if (status != VL53LX_ERROR_NONE)
    {
        printf("ST re-arm failed: %d\r\n", (int)status);
        return false;
    }

    return true;
}

bool dist_sens_st_identity_test(void)
{
    uint8_t model_id = 0;
    uint8_t module_type = 0;
    VL53LX_Error status;

    if (!dist_sens_st_init())
    {
        return false;
    }

    status = VL53LX_RdByte(&g_vl53_dev, VL53_REG_MODEL_ID, &model_id);
    if (status != VL53LX_ERROR_NONE)
    {
        printf("ST model ID read failed: %d\r\n", (int)status);
        dist_sens_st_shutdown();
        return false;
    }

    status = VL53LX_RdByte(&g_vl53_dev, VL53_REG_MODULE_TYPE, &module_type);
    if (status != VL53LX_ERROR_NONE)
    {
        printf("ST module type read failed: %d\r\n", (int)status);
        dist_sens_st_shutdown();
        return false;
    }

    printf("ST path: Model ID 0x%02X (expected 0xEA)\r\n", model_id);
    printf("ST path: Module Type 0x%02X (expected 0xAA)\r\n", module_type);

    if (model_id == 0xEA && module_type == 0xAA)
    {
        printf("ST platform identity test PASSED.\r\n");
        return true;
    }

    printf("ST platform identity test FAILED.\r\n");
    dist_sens_st_shutdown();
    return false;
}
// bool check_sensor_alive(void)
// {
//     uint8_t dummy_buf = 0;
    
//     /* Attempt a zero-byte or single-byte transaction to 7-bit address 0x29 */
//     cy_rslt_t result = cyhal_i2c_master_write(&dist_i2c, 0x29, &dummy_buf, 0, 100, false);

//     /* CY_RSLT_SUCCESS indicates an ACK was received from the sensor */
//     return (result == CY_RSLT_SUCCESS);
// }
// ```<ElicitationsGroup message="How would you like to proceed with testing or driver development?">

//   <Elicitation label="Connect this to ST's official ULD API layer" query="How do I hook these i2c_read and i2c_write functions into ST's VL53L3CX driver files?"/>

//   <Elicitation label="Write a complete main.c startup sequence" query="Can you show a complete main.c example that calls init, tests the connection, and starts reading distance data?"/>

// </ElicitationsGroup>