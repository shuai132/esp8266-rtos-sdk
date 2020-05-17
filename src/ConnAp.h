#pragma once

#include "String.h"
#include "MakeEvent.hpp"

class ConnAp {
public:
    MAKE_EVENT(WifiEvent, System_Event_t*);
    MAKE_EVENT(Connected);

public:
    explicit ConnAp(ConnectedCb_t cb = nullptr);
    ~ConnAp();
    void connect(const String& ssid, const String& password);
};
