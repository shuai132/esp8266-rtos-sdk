extern "C" {
#include "lwip/sockets.h"
}
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "TcpClient.h"

//#define LOG_SHOW_VERBOSE
#include "log.h"

void TcpClient::connectTo(const String& ssid
        , const String& password
        , const String& serverIp
        , uint16_t serverPort
) {
    LOGD("TCP connect start");
    while (true) {
        _fd = socket(PF_INET, SOCK_STREAM, 0);
        if (_fd == -1) {
            LOGD("fail: sock");
            vTaskDelay(1000 / portTICK_RATE_MS);
            continue;
        }
        LOGD("socket ok");

        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(serverIp.c_str());
        server_addr.sin_port = htons(serverPort);
        int ret = connect(_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
        if (ret != 0) {
            LOGD("fail: connect(%d)", ret);
            close(_fd);
            _fd = -1;
            vTaskDelay(1000 / portTICK_RATE_MS);
            continue;
        }
        break;
    }
    LOGD("connect ok");
    onConnect();

    byte buf[128];
    ssize_t sz;
    while ((sz = read(_fd, buf, sizeof(buf))) > 0) {
        onReceive(buf, sz);
    }

    onDisconnect();

    close(_fd);
    _fd = -1;
    LOGD("close");

    onClose();
}

void TcpClient::sendMsg(const String& msg) const {
    sendMsg(msg.c_str(), msg.length());
}

void TcpClient::sendMsg(const void* data, size_t size) const {
    size_t sendSize = 0;
    while (size > 0) {
        sendSize = write(_fd, (byte*)data + sendSize, size);
        if (sendSize < 0) {
            LOGE("send error");
        }
        else {
            size -= sendSize;
            LOGV("sendSize:%u", sendSize);
        }
    }
}
