#include "app.h"

#include "esp_common.h"
#include "gpio.h"
#include "uart.h"

#define LOG_FOR_MCU
#include "../modules/LOG/log.h"

static void init_uart() {
    UART_WaitTxFifoEmpty(UART0);
    UART_ConfigTypeDef uartConfig;
    uartConfig.baud_rate = BIT_RATE_115200;
    uartConfig.data_bits = UART_WordLength_8b;
    uartConfig.parity    = USART_Parity_None;
    uartConfig.stop_bits = USART_StopBits_1;
    uartConfig.flow_ctrl = USART_HardwareFlowControl_None;
    uartConfig.UART_InverseMask = UART_None_Inverse;
    UART_ParamConfig(UART0, &uartConfig);
}

[[noreturn]] static void task_blink(void* ignore)
{
    for(;;) {
        static bool value;
        value = !value;
        LOGD("LED: %d", value);
        GPIO_OUTPUT_SET(2, value);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }

    vTaskDelete(nullptr);
}

void init_app() {
    init_uart();

    xTaskCreate(&task_blink, (const signed char*)"startup", 2048, NULL, 1, NULL);
    xTaskCreate([](void*) {
        wifi_set_event_handler_cb(
                [](System_Event_t *evt) {
                    LOGD("wifi_set_event_handler_cb: %d", evt->event_id);
                    return ;
                    switch(evt->event_id){
                        case EVENT_STAMODE_CONNECTED:
                            LOGD("Have got IP!");
                            break;
                        case EVENT_STAMODE_DISCONNECTED:
                            LOGD("Have got IP!");
                            break;
                        defaut:
                            LOGD("Have'n got IP, id=%d", evt->event_id);
                            break;
                    }
                });

        if (not wifi_set_opmode(STATION_MODE)) {
            LOGE("wifi_set_opmode");
        }

        for(;;) {
            static scan_config config;
            wifi_station_scan(&config, [](void *arg, STATUS status) {
                LOGD("wifi_station_scan: %d", config.channel);
            });

            {
                // connect
                station_config stationConfig{};
                strcpy((char*)stationConfig.ssid, "test_1");
                strcpy((char*)stationConfig.password, "12345678");
                wifi_station_set_config(&stationConfig);
            }

            auto status = wifi_station_get_connect_status();
            LOGD("status: %d", status);
            auto ret = wifi_station_connect();
            LOGD("wifi_station_connect: %d", ret);
            /**
                static scan_config config;
                wifi_station_scan(&config, [](void *arg, STATUS status) {
                    LOGD("scan: %d", config.channel);
                });
             */
            status = wifi_station_get_connect_status();
            LOGD("status: %d", status);

            if (STATION_CONNECTING == status) {
                vTaskDelay(10000 / portTICK_RATE_MS);
                for(;;) {
                    if (STATION_GOT_IP == status) {
                        ip_info info{};
                        auto r = wifi_get_ip_info(STATION_IF, &info);
                        LOGD("wifi_get_ip_info: %d, %d", r, info.ip.addr);
                    }
                }
            }
        }
    }, (const signed char*)"startup", 2048, NULL, 1, NULL);
}

