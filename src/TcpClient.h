#pragma once

#include "String.h"
#include "MakeEvent.hpp"
#include "Singleton.hpp"

class TcpClient {
public:
    using byte = unsigned char;

    SINGLETON(TcpClient);

public:
    void connectTo(const String& ssid
            , const String& password
            , const String& serverIp
            , uint16_t serverPort
            );

    void sendMsg(const String& msg) const;

    void sendMsg(const void *data, size_t size) const;

public:
    MAKE_EVENT(Connect);
    MAKE_EVENT(Disconnect);

    MAKE_EVENT(Receive, byte*, ssize_t);

    MAKE_EVENT(Close);

private:
    int _fd = -1;
};
