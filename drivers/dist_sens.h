#ifndef DIST_SENS_H
#define DIST_SENS_H

#include "main.h"
#include "cyhal_gpio.h"
#include "cyhal_i2c.h"


/* TODO: Fill these in with the actual PSoC pin names for the VL53L3CX board. */
#define DIST_XSHUT P12_6
#define DIST_GPIO1 P12_7
#define SDA_1 P6_5
#define SCL_1 P6_4

cyhal_i2c_t *proto_i2c_init(cyhal_gpio_t sda, cyhal_gpio_t scl);
cy_rslt_t i2c_write(cyhal_i2c_t *obj, uint16_t reg_index, const uint8_t *p_data, uint16_t len);
cy_rslt_t i2c_read(cyhal_i2c_t *obj, uint16_t reg_index, uint8_t *p_dest_buffer, uint16_t len);
bool vl53l3cx_test_connection(cyhal_i2c_t *obj);

#endif // DIST_SENS_H