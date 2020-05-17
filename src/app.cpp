#include "app.h"

#include "esp_common.h"
#include "gpio.h"
#include "uart.h"

#include "log.h"
#include "ConnAp.h"
#include "TcpClient.h"
#include "user_config.h"

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

[[noreturn]] static void task_blink(void* ignore) {
    for(;;) {
        static bool value;
        value = !value;
        LOGD("LED: %d", value);
        GPIO_OUTPUT_SET(2, value);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }

    vTaskDelete(nullptr);
}

[[noreturn]] static void task_tcp(void* ignore) {
    for(;;) {
        LOGD("tcp_task...");

        static ConnAp connAp([]{
            static TcpClient client;
            client.connectTo(AP_SSID
                    , AP_PASSWORD
                    , TCP_SERVER_ADDR
                    , TCP_SERVER_PORT
            );

            client.setConnectCb([] {
                LOGD("client connected cb!");
                client.sendMsg("hello!!!");
            });

            client.setReceiveCb([](TcpClient::byte* data, ssize_t size) {
                LOGD("client receive cb: %d, %s", size, String((char*)data, size).c_str());
                // echo
                client.sendMsg(data, size);
            });
        });
        connAp.connect(AP_SSID, AP_PASSWORD);
    }
}

void init_app() {
    init_uart();

    LOG("SDK version:%s", system_get_sdk_version());
    LOG("ESP8266 chip ID:0x%x", system_get_chip_id());

    xTaskCreate(&task_blink, (const signed char*)"task_blink", 2048, NULL, 1, NULL);
    xTaskCreate(&task_tcp, (const signed char*)"task_tcp", 2048, NULL, 2, NULL);
}

void std::__throw_length_error(const char* str) {
    FATAL("throw_length_error: %s", str);
}
