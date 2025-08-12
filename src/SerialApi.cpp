#include "SerialApi.h"
#include "serial_controller.h"
#include "uc2ui_ledpage.h"
#include "uc2ui_motorpage.h"
#include "uc2ui_controller.h"
#include <ArduinoJson.h>

namespace SerialApi
{
    typedef struct
    {
        int r, g, b;
    } update_led_t;

    typedef struct
    {
        int motor, speed;
    } updateMotorForever_t;

    typedef struct
    {
        int speedX, speedY;
    } updateMotorXYForever_t;

    static QueueHandle_t updateLedColorQueue;
    static QueueHandle_t driveMotorForeverQueue;
    static QueueHandle_t driveMotorXYForeverQueue;
    const int QueueElementSize = 2;
    xTaskHandle xHandle;

    // Speed mapping array (same as RestApi)
    int speeds[] = {-80000, -40000, -8000, -4000, -2000, -1000, -500, -200, -100, -50, -20, -10, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 4000, 8000, 40000, 80000};

    void init()
    {
        updateLedColorQueue = xQueueCreate(QueueElementSize, sizeof(update_led_t));
        if (updateLedColorQueue == 0)
            log_e("Failed to create LED queue");
        
        driveMotorForeverQueue = xQueueCreate(QueueElementSize, sizeof(updateMotorForever_t));
        if (driveMotorForeverQueue == 0)
            log_e("Failed to create motor queue");
        
        driveMotorXYForeverQueue = xQueueCreate(QueueElementSize, sizeof(updateMotorXYForever_t));
        if (driveMotorXYForeverQueue == 0)
            log_e("Failed to create XY motor queue");
        
        // Enable motor and LED modules by default for serial communication
        uc2ui_motorpage::setMotorModule(true);
        uc2ui_motorpage::setMotorX(true);
        uc2ui_motorpage::setMotorY(true);
        uc2ui_motorpage::setMotorZ(true);
        uc2ui_motorpage::setMotorA(true);
        
        uc2ui_ledpage::setLedModule(true);
        uc2ui_ledpage::setLedCount(1);
    }

    void loop()
    {
        // Process queued commands
        delay(1);
    }

    void handleIncomingMessage(const String& message)
    {
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, message);
        
        if (error) {
            log_e("Failed to parse JSON: %s", error.c_str());
            return;
        }

        // Handle different message types
        if (doc.containsKey("type")) {
            String type = doc["type"];
            
            if (type == "status_request") {
                sendStatusUpdate();
            }
            else if (type == "led_update" && doc.containsKey("data")) {
                JsonObject data = doc["data"];
                if (data.containsKey("enabled")) {
                    bool enabled = data["enabled"];
                    int r = data.containsKey("r") ? data["r"] : 0;
                    int g = data.containsKey("g") ? data["g"] : 0; 
                    int b = data.containsKey("b") ? data["b"] : 0;
                    uc2ui_ledpage::setLedOn(enabled);
                }
            }
            else if (type == "motor_update" && doc.containsKey("data")) {
                JsonObject data = doc["data"];
                if (data.containsKey("positions")) {
                    JsonObject positions = data["positions"];
                    // Update motor positions in UI if needed
                }
            }
            else if (type == "objective_slot" && doc.containsKey("data")) {
                JsonObject data = doc["data"];
                if (data.containsKey("current_slot")) {
                    int slot = data["current_slot"];
                    // Update objective slot display
                }
            }
            else if (type == "sample_position" && doc.containsKey("data")) {
                JsonObject data = doc["data"];
                if (data.containsKey("x") && data.containsKey("y")) {
                    float x = data["x"];
                    float y = data["y"];
                    // Update sample map position
                }
            }
            else if (type == "pwm_command" && doc.containsKey("data")) {
                JsonObject data = doc["data"];
                if (data.containsKey("channel") && data.containsKey("value")) {
                    int channel = data["channel"];
                    int value = data["value"];
                    // Handle PWM setting - implement hardware PWM control here
                    // For now, just acknowledge the command
                    DynamicJsonDocument response(256);
                    response["type"] = "pwm_update";
                    response["data"]["channel"] = channel;
                    response["data"]["value"] = value;
                    sendMessage(response);
                }
            }
        }
    }

    void sendMessage(const DynamicJsonDocument& doc)
    {
        String message;
        serializeJson(doc, message);
        serial_controller::sendMessage(message);
    }

    void updateColors(int r, int g, int b)
    {
        // Note: Debug logging disabled to prevent JSON parsing interference  
        update_led_t led;
        led.r = r;
        led.g = g;
        led.b = b;
        xQueueSend(updateLedColorQueue, (void *)&led, 0);
    }

    void setLedOn(bool enable, int r, int g, int b)
    {
        DynamicJsonDocument doc(512);
        doc["type"] = "led_command";
        doc["data"]["enabled"] = enable;
        doc["data"]["r"] = r;
        doc["data"]["g"] = g;
        doc["data"]["b"] = b;
        sendMessage(doc);
    }

    void driveMotorForever(int motor, int speed)
    {
        // Note: Debug logging disabled to prevent JSON parsing interference  
        updateMotorForever_t m;
        
        if (uxQueueMessagesWaiting(driveMotorForeverQueue) == QueueElementSize - 1)
            xQueueReceive(driveMotorForeverQueue, (void *)&m, 0);
        
        m.speed = speeds[speed];
        m.motor = motor;
        xQueueSend(driveMotorForeverQueue, (void *)&m, 0);
    }

    void driveMotorXYForever(int speedX, int speedY)
    {
        // Note: Debug logging disabled to prevent JSON parsing interference
        updateMotorXYForever_t o;
        
        if (uxQueueMessagesWaiting(driveMotorXYForeverQueue) == QueueElementSize - 1)
            xQueueReceive(driveMotorXYForeverQueue, (void *)&o, 0);
        
        o.speedX = speedX;
        o.speedY = speedY;
        xQueueSend(driveMotorXYForeverQueue, (void *)&o, 0);
    }

    void sendSerialMsg(void *pvParameters)
    {
        for (;;) {
            // Process LED color updates
            if (updateLedColorQueue != NULL && uxQueueMessagesWaiting(updateLedColorQueue) > 0) {
                update_led_t led;
                BaseType_t ret = xQueueReceive(updateLedColorQueue, (void *)&led, 0);
                if (ret == pdTRUE) {
                    DynamicJsonDocument doc(512);
                    doc["type"] = "led_command";
                    doc["data"]["enabled"] = true;
                    doc["data"]["r"] = led.r;
                    doc["data"]["g"] = led.g;
                    doc["data"]["b"] = led.b;
                    sendMessage(doc);
                }
            }

            // Process motor updates
            if (driveMotorForeverQueue != NULL && uxQueueMessagesWaiting(driveMotorForeverQueue) > 0) {
                updateMotorForever_t m;
                BaseType_t ret = xQueueReceive(driveMotorForeverQueue, &m, 0);
                if (ret == pdTRUE) {
                    DynamicJsonDocument doc(512);
                    doc["type"] = "motor_command";
                    doc["data"]["motor"] = m.motor;
                    doc["data"]["speed"] = m.speed;
                    sendMessage(doc);
                }
            }

            // Process XY motor updates
            if (driveMotorXYForeverQueue != NULL && uxQueueMessagesWaiting(driveMotorXYForeverQueue) > 0) {
                updateMotorXYForever_t m;
                BaseType_t ret = xQueueReceive(driveMotorXYForeverQueue, &m, 0);
                if (ret == pdTRUE) {
                    DynamicJsonDocument doc(512);
                    doc["type"] = "motor_xy_command";
                    doc["data"]["speedX"] = m.speedX;
                    doc["data"]["speedY"] = m.speedY;
                    sendMessage(doc);
                }
            }

            delay(10);
        }
    }

    void sendStatusUpdate()
    {
        DynamicJsonDocument doc(512);
        doc["type"] = "status_update";
        doc["data"]["connected"] = true;
        doc["data"]["modules"]["motor"] = true;
        doc["data"]["modules"]["led"] = true;
        sendMessage(doc);
    }

    void requestObjectiveSlot()
    {
        DynamicJsonDocument doc(256);
        doc["type"] = "objective_slot_request";
        sendMessage(doc);
    }

    void setObjectiveSlot(int slot)
    {
        DynamicJsonDocument doc(256);
        doc["type"] = "objective_slot_command";
        doc["data"]["slot"] = slot;
        sendMessage(doc);
    }

    void snapImage()
    {
        DynamicJsonDocument doc(256);
        doc["type"] = "snap_image_command";
        sendMessage(doc);
    }

    void updateSamplePosition(float x, float y)
    {
        DynamicJsonDocument doc(256);
        doc["type"] = "sample_position_update";
        doc["data"]["x"] = x;
        doc["data"]["y"] = y;
        sendMessage(doc);
    }

    void setPwmValue(int channel, int value)
    {
        DynamicJsonDocument doc(256);
        doc["type"] = "pwm_command";
        doc["data"]["channel"] = channel;
        doc["data"]["value"] = value;
        sendMessage(doc);
    }

    // Initialize the serial message task
    void startSerialTask()
    {
        if (xHandle != nullptr)
            vTaskDelete(xHandle);
        xTaskCreate(sendSerialMsg, "sendSerialmsg", 4 * 1024, NULL, 1, &xHandle);
    }
}