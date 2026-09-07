#include "dist_sens.h"

#define VL53_I2C_ADDR_7BIT     (0x29)   // PSoC HAL automatically handles write (0x52) / read (0x53)
#define VL53_REG_MODEL_ID      (0x010F) // Expected: 0xEA
#define VL53_REG_MODULE_TYPE   (0x0110) // Expected: 0xAA

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

    return &i2c_obj;
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