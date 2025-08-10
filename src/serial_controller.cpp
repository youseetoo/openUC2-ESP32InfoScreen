#include "serial_controller.h"
#include "SerialApi.h"
#include "uc2ui_controller.h"

namespace serial_controller
{
    serial_status current_status = serial_status::disconnected;
    String inputBuffer = "";

    void init()
    {
        // Serial is already initialized in main.ino
        current_status = serial_status::connected;
        uc2ui_controller::showMicroscopePage(true);
        inputBuffer.reserve(1024); // Reserve space for incoming messages
    }

    void loop()
    {
        // Read incoming serial messages
        while (Serial.available())
        {
            char c = Serial.read();
            if (c == '\n' || c == '\r')
            {
                if (inputBuffer.length() > 0)
                {
                    SerialApi::handleIncomingMessage(inputBuffer);
                    inputBuffer = "";
                }
            }
            else
            {
                inputBuffer += c;
            }
        }
    }

    serial_status getStatus()
    {
        return current_status;
    }

    void sendMessage(const String& message)
    {
        Serial.println(message);
        Serial.flush();
    }
}; // namespace serial_controller