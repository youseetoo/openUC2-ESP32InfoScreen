#pragma once
#include "Arduino.h"

namespace serial_controller
{
    enum serial_status
    {
        disconnected,
        connected,
    };

    void init();
    void loop();
    serial_status getStatus();
    void sendMessage(const String& message);
}; // namespace serial_controller