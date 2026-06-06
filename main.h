/**
 * @file main.h
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-07-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef __MAIN_H__
#define __MAIN_H__

#include "cyhal.h"
#include "cybsp.h"
#include "cyhal_hw_types.h"
#include "cyhal_gpio.h"
#include "cyhal_timer.h"
#include "cyhal_pwm.h"
#include "cyhal_adc.h"
#include "cyhal_uart.h"
#include "cyhal_i2c.h"
#include "cyhal_spi.h"
#include "cy_retarget_io.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* Uncomment the line below to enable FreeRTOS in your applications*/
#define ECE353_FREERTOS

#if defined(ECE353_FREERTOS)
// /* FreeRTOS Includes */
#include <FreeRTOS.h>
// #include <event_groups.h>
// #include <queue.h>
// #include <semphr.h>
// #include <task.h> MS ECE do for masters.
#endif

/* This macro is used to determine if we are building an executable for example code or ICE code.
 * Only a single line should be uncommented at one time.  
*/
// #define HW06
#define MOTOR_TEST
//#define MOTOR_PRIM

extern char NAME[];
extern char APP_DESCRIPTION[];

/**
 * @brief 
 * This function will initialize all of the hardware resources for
 * the ICE.  
 * 
 * This function is implemented in the iceXX.c file for the ICE you are
 * working on.
 */
void app_init_hw(void);

/*****************************************************************************/
/* Application Code                                                          */
/*****************************************************************************/
/**
 * @brief 
 * This function implements the behavioral requirements for the ICE
 * 
 * This function is implemented in the iceXX.c file for the ICE you are 
 * working on.
 */
void app_main(void);

#endif
