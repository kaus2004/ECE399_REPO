#include "main.h"
#include "motor_test.h"

#if defined(BLE_COMM)

#include "wiced_bt_stack.h"
#include "wiced_bt_dev.h"
#include "wiced_bt_rfcomm.h"
#include "wiced_bt_adv_scan_legacy.h"
#include "wiced_bt_sdp_defs.h"
#include "cybt_platform_config.h"

extern const wiced_bt_cfg_settings_t wiced_bt_cfg_settings;

static uint16_t s_rfcomm_handle = 0;
static char s_rfcomm_rx_fifo[256];

static wiced_result_t bt_management_cb(wiced_bt_management_evt_t event,
                                       wiced_bt_management_evt_data_t *p_event_data);
static void rfcomm_mgmt_cb(wiced_bt_rfcomm_result_t code, uint16_t port_handle);
static void rfcomm_port_event_cb(wiced_bt_rfcomm_port_event_t event, uint16_t port_handle);

static const cybt_platform_config_t bt_platform_cfg = {
    .hci_config = {
        .hci_transport = CYBT_HCI_IPC
    }
};

static void rfcomm_server_start(void)
{
    wiced_bt_rfcomm_result_t rc;

    rc = wiced_bt_rfcomm_create_connection(UUID_SERVCLASS_SERIAL_PORT,
                                           1,
                                           1,
                                           0,
                                           NULL,
                                           &s_rfcomm_handle,
                                           rfcomm_mgmt_cb);
    if (rc != WICED_BT_RFCOMM_SUCCESS) {
        printf("[BT] RFCOMM create failed: %d\r\n", rc);
        return;
    }

    (void)wiced_bt_rfcomm_set_rx_fifo(s_rfcomm_handle, s_rfcomm_rx_fifo, sizeof(s_rfcomm_rx_fifo));
    (void)wiced_bt_rfcomm_set_event_callback(s_rfcomm_handle, rfcomm_port_event_cb, NULL);
    (void)wiced_bt_rfcomm_set_event_mask(s_rfcomm_handle, PORT_EV_RXCHAR | PORT_EV_CONNECTED | PORT_EV_CONNECT_ERR);
}

void ble_comm_start(void)
{
    cybt_platform_config_init(&bt_platform_cfg);

    if (wiced_bt_stack_init(bt_management_cb, &wiced_bt_cfg_settings) != WICED_BT_SUCCESS) {
        printf("[BT] stack init failed\r\n");
    }
}

static wiced_result_t bt_management_cb(wiced_bt_management_evt_t event,
                                       wiced_bt_management_evt_data_t *p_event_data)
{
    (void)p_event_data;

    if (event == BTM_ENABLED_EVT) {
        printf("[BT] stack enabled\r\n");
        (void)wiced_bt_start_advertisements(BTM_BLE_ADVERT_UNDIRECTED_HIGH, BLE_ADDR_PUBLIC, NULL);
        rfcomm_server_start();
    }

    return WICED_BT_SUCCESS;
}

static void rfcomm_mgmt_cb(wiced_bt_rfcomm_result_t code, uint16_t port_handle)
{
    printf("[BT] RFCOMM mgmt code=%d handle=%u\r\n", code, port_handle);
}

static void rfcomm_port_event_cb(wiced_bt_rfcomm_port_event_t event, uint16_t port_handle)
{
    if (event & PORT_EV_RXCHAR) {
        char rx[32];
        uint16_t read_len = 0;

        do {
            if (wiced_bt_rfcomm_read_data(port_handle, rx, sizeof(rx), &read_len) != WICED_BT_RFCOMM_SUCCESS) {
                break;
            }

            for (uint16_t i = 0; i < read_len; i++) {
                rover_execute_command(rx[i]);
            }
        } while (read_len > 0);
    }

    if (event & PORT_EV_CONNECTED) {
        printf("[BT] RFCOMM connected\r\n");
    }
    if (event & PORT_EV_CONNECT_ERR) {
        printf("[BT] RFCOMM disconnected/error\r\n");
    }
}

#endif