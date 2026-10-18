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
cyhal_i2c_t *dist_sens_get_i2c_handle(void);
cy_rslt_t i2c_write(cyhal_i2c_t *obj, uint16_t reg_index, const uint8_t *p_data, uint16_t len);
cy_rslt_t i2c_read(cyhal_i2c_t *obj, uint16_t reg_index, uint8_t *p_dest_buffer, uint16_t len);
bool vl53l3cx_test_connection(cyhal_i2c_t *obj);
bool dist_sens_st_init(void);
void dist_sens_st_shutdown(void);
bool dist_sens_st_configure_gpio1_interrupt(void);
bool dist_sens_st_try_read_distance(uint16_t *distance_mm);
bool dist_sens_st_read_distance_from_interrupt(uint16_t *distance_mm);
bool dist_sens_st_poll_distance_mm(uint16_t *distance_mm);
bool dist_sens_st_identity_test(void);

#endif // DIST_SENS_H