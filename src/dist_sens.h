#ifndef DIST_SENS_H
#define DIST_SENS_H

#include "main.h"

/* TODO: Fill these in with the actual PSoC pin names for the VL53L3CX board. */
#define DIST_XSHUT P12_6
#define DIST_GPIO1 P12_7
#define SDA_1 P6_5
#define SCL_1 P6_4

void proto_sens_gpio_init(void);

#endif // DIST_SENS_H