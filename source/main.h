/**
 * @file main.h
 * @author Kaustubh Bharadwaj (kbharadwaj@wisc.edu )
 * @brief 
 * @version 0.1
 * @date 2026-07-11
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef __MAIN_H__
#define __MAIN_H__

#include "app_flash_common.h"
#include "app_bt_bonding.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "mtb_kvstore.h"
#include <FreeRTOS.h>
#include <task.h>
#include "cycfg_bt_settings.h"
#include "wiced_bt_stack.h"
#include "cybsp_bt_config.h"
#include "cybt_platform_config.h"
#include "app_bt_event_handler.h"
#include "app_bt_gatt_handler.h"
#include "app_hw_device.h"
#include "app_bt_utils.h"

// #include <stdbool.h>
// #include <stdint.h>
// #include <stdio.h>
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

#endif // __MAIN_H__
