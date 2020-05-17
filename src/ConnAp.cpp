#include "esp_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ConnAp.h"
#include "log.h"

static ConnAp* instance = nullptr;

static void wifi_handle_event_cb(System_Event_t *evt)
{
    LOG("wifi event: %x", evt->event_id);
    if (instance)
        instance->onWifiEvent(evt);

    switch (evt->event_id) {
    case EVENT_STAMODE_CONNECTED:
        LOG("connect to ssid %s, channel %d",
               evt->event_info.connected.ssid,
               evt->event_info.connected.channel);
        break;
    case EVENT_STAMODE_DISCONNECTED:
        LOG("disconnect from ssid %s, reason %d",
               evt->event_info.disconnected.ssid,
               evt->event_info.disconnected.reason);
        break;
    case EVENT_STAMODE_AUTHMODE_CHANGE:
        LOG("mode: %d -> %d",
               evt->event_info.auth_change.old_mode,
               evt->event_info.auth_change.new_mode);
        break;
    case EVENT_STAMODE_GOT_IP:
        LOG("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR,
               IP2STR(&evt->event_info.got_ip.ip),
               IP2STR(&evt->event_info.got_ip.mask),
               IP2STR(&evt->event_info.got_ip.gw));
        break;
    case EVENT_SOFTAPMODE_STACONNECTED:
        LOG("station: " MACSTR "join, AID = %d",
               MAC2STR(evt->event_info.sta_connected.mac),
               evt->event_info.sta_connected.aid);
        break;
    case EVENT_SOFTAPMODE_STADISCONNECTED:
        LOG("station: " MACSTR "leave, AID = %d",
               MAC2STR(evt->event_info.sta_disconnected.mac),
               evt->event_info.sta_disconnected.aid);
        break;
    default:
        break;
    }
}

static void wifi_connect_wait()
{
    uint8 con_status = wifi_station_get_connect_status();
    while(con_status != STATION_GOT_IP) {
        con_status = wifi_station_get_connect_status();
        LOGD("Connect ap: %s", con_status == STATION_IDLE ? "IDLE" : con_status == STATION_CONNECTING ? \
               "connecting" : con_status == STATION_WRONG_PASSWORD ? \
               "wrong password" : con_status == STATION_NO_AP_FOUND ? \
               "no ap found" : con_status == STATION_CONNECT_FAIL ? "fail" : "got ip");
        vTaskDelay(1000 / portTICK_RATE_MS);
    }

    LOG("wifi connected");
}

void ConnAp::connect(const String& ssid, const String& password) {
    wifi_set_opmode(STATION_MODE);

    station_config config{};
    strcpy((char*)config.ssid, ssid.c_str());
    strcpy((char*)config.password, password.c_str());
    wifi_station_set_config(&config);

    wifi_set_event_handler_cb(wifi_handle_event_cb);
    wifi_station_connect();

    wifi_connect_wait();

    onConnected();
}

ConnAp::ConnAp(ConnectedCb_t cb) {
    instance = this;
    setConnectedCb(cb);
}

ConnAp::~ConnAp() {
    instance = nullptr;
}
