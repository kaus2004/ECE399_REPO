#include "dist_sens.h"

void proto_sens_gpio_init(void)
{
    cy_rslt_t result;

    /* XSHUT: output, active-low control for sensor enable */
    result = cyhal_gpio_init(DIST_XSHUT,
                            CYHAL_GPIO_DIR_OUTPUT,
                            CYHAL_GPIO_DRIVE_STRONG,
                            false);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* GPIO1: sensor interrupt/status pin (placeholder until actual board mapping is known) */
    result = cyhal_gpio_init(DIST_GPIO1,
                            CYHAL_GPIO_DIR_INPUT,
                            CYHAL_GPIO_DRIVE_PULLUP,
                            false);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* I2C data lines: placeholders for later real SDA/SCL assignment */
    result = cyhal_gpio_init(SDA_1,
                            CYHAL_GPIO_DIR_OUTPUT,
                            CYHAL_GPIO_DRIVE_STRONG,
                            false);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    result = cyhal_gpio_init(SCL_1,
                            CYHAL_GPIO_DIR_OUTPUT,
                            CYHAL_GPIO_DRIVE_STRONG,
                            false);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
}
