#pragma once

#include "String.h"
#include "MakeEvent.hpp"
#include "Singleton.hpp"

class ConnAp {
public:
    MAKE_EVENT(WifiEvent, System_Event_t*);
    MAKE_EVENT(Connected);

    SINGLETON(ConnAp);

public:
    void connect(const String& ssid, const String& password);
};
