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
        if (!serialMutex) {
            serialMutex = xSemaphoreCreateMutex();
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


    static void writeLine(const String& line) {
    if (serialMutex) xSemaphoreTake(serialMutex, portMAX_DELAY);
    // Write entire line atomically
    Serial.write((const uint8_t*)line.c_str(), line.length());
    Serial.write('\n');
    Serial.flush();          // Ensure it is pushed to USB CDC
    // Give USB task time to drain (important on S3/TinyUSB)
    vTaskDelay(10);           // yield 1 tick
    if (serialMutex) xSemaphoreGive(serialMutex);
}

    void sendMessage(const String& message)
    {
        writeLine(message);
        /*
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
            */
    }
}; // namespace serial_controller