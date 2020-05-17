extern "C" {
#include "lwip/sockets.h"
}
#include "user_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tcp_client.h"
#include "log.h"

static int m_fd = -1;

void tcp_client_start(void)
{
    struct sockaddr_in server_addr;

    LOGD("TCP connect start");
    while (1) {
        m_fd = socket(PF_INET, SOCK_STREAM, 0);
        if (m_fd == -1) {
            LOGD("fail: sock");
            vTaskDelay(1000 / portTICK_RATE_MS);
            continue;
        }
        LOGD("socket ok");

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(TCP_SERVER_IPADDR);
        server_addr.sin_port = htons(TCP_SERVER_REMOTE_PORT);
        int ret = connect(m_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
        if (ret != 0) {
            LOGD("fail: connect(%d)\n", ret);
            close(m_fd);
            m_fd = -1;
            vTaskDelay(1000 / portTICK_RATE_MS);
            continue;
        }
        break;
    }
    LOGD("connect ok");
    write(m_fd, TCP_CLIENT_GREETING, sizeof(TCP_CLIENT_GREETING) - 1);

    char buf[128];
    ssize_t sz;
    while ((sz = read(m_fd, buf, sizeof(buf))) > 0) {
        write(m_fd, buf, sz);
    }

    close(m_fd);
    m_fd = -1;
    LOGD("close");
}
