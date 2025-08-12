#include "serial_controller.h"
#include "SerialApi.h"
#include "uc2ui_controller.h"

namespace serial_controller
{
    serial_status current_status = serial_status::disconnected;
    String inputBuffer = "";
    static SemaphoreHandle_t serialMutex = nullptr;

    void init()
    {
        // Serial is already initialized in main.ino
        current_status = serial_status::connected;
        uc2ui_controller::showMicroscopePage(true);
        inputBuffer.reserve(1024); // Reserve space for incoming messages
        
        // Create mutex for serial communication synchronization
        serialMutex = xSemaphoreCreateMutex();
        if (serialMutex == NULL) {
            log_e("Failed to create serial mutex");
        }
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
        // Use mutex to prevent serial message corruption from multiple tasks
        if (serialMutex != nullptr && xSemaphoreTake(serialMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            Serial.println(message);
            Serial.flush();
            xSemaphoreGive(serialMutex);
        } else {
            // Fallback without mutex if acquisition fails
            Serial.println(message);
            Serial.flush();
        }
    }
}; // namespace serial_controller