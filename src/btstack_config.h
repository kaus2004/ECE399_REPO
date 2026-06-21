
#ifndef BTSTACK_CONFIG_H
#define BTSTACK_CONFIG_H

#include "main.h"
#include "cybt_platform_config.h"
#include "wiced_bt_stack.h"
#include "wiced_bt_gatt.h"
#include "wiced_memory.h"
#include "wiced_bt_dev.h"
#include "wiced_bt_ble.h"
#include "motor_prim.h"

extern const wiced_bt_cfg_settings_t settings_cfg;
void ble_comm_start(void);

wiced_bt_gatt_status_t app_bt_gatt_callback(
    wiced_bt_gatt_evt_t event,
    wiced_bt_gatt_event_data_t *p_event_data
);

void ble_start_advertising(void);
// wiced_result_t app_bt_management_callback()

#endif /* BTSTACK_CONFIG_H */