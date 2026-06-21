#include "btstack_config.h"
#if defined(BLE_COMM)

static const wiced_bt_cfg_gatt_t gatt_cfg =
{
    .max_db_service_modules = 1,
    .max_eatt_bearers = 0
};

static const wiced_bt_cfg_ble_scan_settings_t ble_scan_cfg =
{
    .scan_mode = BTM_BLE_SCAN_MODE_PASSIVE,

    .high_duty_scan_interval = 0x30,
    .high_duty_scan_window = 0x30,
    .high_duty_scan_duration = 0,

    .low_duty_scan_interval = 0x30,
    .low_duty_scan_window = 0x30,
    .low_duty_scan_duration = 0,

    .high_duty_conn_scan_interval = 0x30,
    .high_duty_conn_scan_window = 0x30,
    .high_duty_conn_duration = 30,

    .low_duty_conn_scan_interval = 0x30,
    .low_duty_conn_scan_window = 0x30,
    .low_duty_conn_duration = 60,

    .conn_min_interval = 24,
    .conn_max_interval = 40,
    .conn_latency = 0,
    .conn_supervision_timeout = 200
};

static const wiced_bt_cfg_ble_advert_settings_t ble_advert_cfg =
{
    .channel_map = BTM_BLE_ADVERT_CHNL_37 |
                   BTM_BLE_ADVERT_CHNL_38 |
                   BTM_BLE_ADVERT_CHNL_39,

    .high_duty_min_interval = 0x30,
    .high_duty_max_interval = 0x30,
    .high_duty_duration = 0,

    .low_duty_min_interval = 0x80,
    .low_duty_max_interval = 0x80,
    .low_duty_duration = 0,

    .high_duty_directed_min_interval = 0x30,
    .high_duty_directed_max_interval = 0x30,

    .low_duty_directed_min_interval = 0x80,
    .low_duty_directed_max_interval = 0x80,
    .low_duty_directed_duration = 0,

    .high_duty_nonconn_min_interval = 0x30,
    .high_duty_nonconn_max_interval = 0x30,
    .high_duty_nonconn_duration = 0,

    .low_duty_nonconn_min_interval = 0x80,
    .low_duty_nonconn_max_interval = 0x80,
    .low_duty_nonconn_duration = 0
};

static const wiced_bt_cfg_ble_t ble_cfg =
{
    .ble_max_simultaneous_links = 1,
    .ble_max_rx_pdu_size = 65,
    .appearance = 0,
    .rpa_refresh_timeout = 0,
    .host_addr_resolution_db_size = 0,
    .p_ble_scan_cfg = &ble_scan_cfg,
    .p_ble_advert_cfg = &ble_advert_cfg,
    .default_ble_power_level = 0
};

const wiced_bt_cfg_settings_t settings_cfg =
{
    .device_name = (uint8_t *)"WALL-E Rover",
    .security_required = BTM_SEC_BEST_EFFORT,
    .p_br_cfg = NULL,
    .p_ble_cfg = &ble_cfg,
    .p_gatt_cfg = &gatt_cfg,
    .p_isoc_cfg = NULL,
    .p_l2cap_app_cfg = NULL
};
wiced_result_t p_bt_management_cback(wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data){
    //write task console printf later
    printf("[BT] Event = %d\r\n", event);
    switch(event){
        case BTM_ENABLED_EVT:
            printf("[BT] Bluetooth stack enabled\n");
            if (p_event_data->enabled.status == WICED_BT_SUCCESS)
            {
                printf("[BT] Bluetooth stack enabled successfully\n");
            }
            else
            {
                printf("[BT] Bluetooth stack failed to enable, status: %d\n", p_event_data->enabled.status);
            }
            // return WICED_BT_SUCCESS;
            wiced_bt_gatt_register(app_bt_gatt_callback);
            ble_start_advertising();
            break;
        case BTM_DISABLED_EVT:
            printf("[BT] Bluetooth stack disabled\n");
            break;
        default:
            printf("[BT] Unhandled event: %d\n", event);
            break;
    }
    return WICED_BT_SUCCESS;

}

void ble_comm_start(void)
{
    printf("[BT] ble_comm_start ENTERED\r\n");
    cybt_platform_config_t bt_platform_cfg =
    {
        .hci_config =
        {
            .hci_transport = CYBT_HCI_IPC
        }
    };

    cybt_platform_config_init(&bt_platform_cfg);

    printf("[BT] Platform config initialized using IPC\r\n");

    /*
     * NEXT:
     * wiced_bt_stack_init(app_bt_management_callback, &settings_cfg);
     *
     * But app_bt_management_callback still needs to be defined.
     */
    printf("[BT] Platform config initialized using IPC\r\n");
    printf("[BT] before wiced_bt_stack_init\r\n");
    wiced_result_t result = wiced_bt_stack_init(p_bt_management_cback,&settings_cfg);
    printf("[BT] wiced_bt_stack_init result=%d\r\n", result);
    if(result == WICED_BT_SUCCESS)
    {
        printf("[BT] Bluetooth stack initialization successful\r\n");
        // testing only ble_start_advertising();
    }
    else
    {
        printf("[BT] Bluetooth stack initialization failed, result: %d\r\n", result);
    }
    
}

wiced_bt_gatt_status_t app_bt_gatt_callback(wiced_bt_gatt_evt_t event, wiced_bt_gatt_event_data_t *p_event_data)
{
    //p event_data must have rx buffer, tx buffer
    // Handle GATT events here (e.g., connection, disconnection, read/write requests), operation complete and connection status.
    (void)p_event_data;
    switch (event)
    {
        case GATT_CONNECTION_STATUS_EVT:
            printf("[BT] Connection status changed\n");
            break;
        case GATT_OPERATION_CPLT_EVT:
            printf("[BT] GATT operation complete\n");
            break;
        case GATT_DISCOVERY_RESULT_EVT:
            printf("[BT] GATT discovery result\n");
            break;
        case GATT_DISCOVERY_CPLT_EVT:
            printf("[BT] GATT discovery complete\n");
            break;
        case GATT_ATTRIBUTE_REQUEST_EVT:
            printf("[BT] Attribute request received\n");
            wiced_bt_gatt_attribute_request_t *p_attr_req = &p_event_data->attribute_request;

            //
            //
            if(p_attr_req->opcode == GATT_REQ_WRITE)
            {
                printf("[BT] Write request received\n");
                //taking a,w,s,d as input from the write request and go to motor_prim.c prim_motor_execute_command
                switch(p_attr_req->data.write_req.p_val[0])
                {
                    case 'w':
                        printf("[BT] Move forward command received\n");
                        prim_motor_forward();
                        break;
                    case 'a':
                        printf("[BT] Move left command received\n");
                        prim_motor_turn_left();
                        break;
                    case 's':
                        printf("[BT] Move backward command received\n");
                        prim_motor_backward();
                        break;
                    case 'd':
                        printf("[BT] Move right command received\n");
                        prim_motor_turn_right();
                        break;
                    default:
                        printf("[BT] Unknown command received: %c\n", p_attr_req->data.write_req.p_val[0]);
                        break;
                   
                }
            }
            break;
        case GATT_CONGESTION_EVT:
            printf("[BT] GATT congestion status changed\n");
            break;
        case GATT_GET_RESPONSE_BUFFER_EVT:
            printf("[BT] GATT response buffer requested\n");
            break;
        case GATT_APP_BUFFER_TRANSMITTED_EVT:
            printf("[BT] GATT app buffer transmitted\n");
            break;
        default:
            printf("[BT] Unhandled GATT event: %d\n", event);
            break;
    }
    return WICED_BT_GATT_SUCCESS;
}
void ble_start_advertising(void)
{
    printf("[BT] ble_start_advertising ENTERED\r\n");
    wiced_result_t result;

    wiced_bt_ble_advert_elem_t adv_elem[] =
    {
        {
            .advert_type = BTM_BLE_ADVERT_TYPE_FLAG,
            .len = 1,
            .p_data = (uint8_t *)"\x06"
        },
        {
            .advert_type = BTM_BLE_ADVERT_TYPE_NAME_COMPLETE,
            .len = sizeof("WALL-E Rover") - 1,
            .p_data = (uint8_t *)"WALL-E Rover"
        }
    };

    result = wiced_bt_ble_set_raw_advertisement_data(
        sizeof(adv_elem) / sizeof(adv_elem[0]),
        adv_elem
    );

    printf("[BT] Set adv data result = %d\r\n", result);

    result = wiced_bt_start_advertisements(
        BTM_BLE_ADVERT_UNDIRECTED_HIGH,
        0,
        NULL
    );

    printf("[BT] Start advertising result = %d\r\n", result);
}



#endif