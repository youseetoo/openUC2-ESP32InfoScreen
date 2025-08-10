#pragma once
#include "Arduino.h"

namespace SerialApi
{
    void init();
    void loop();
    void handleIncomingMessage(const String& message);
    void updateColors(int r, int g, int b);
    void driveMotorForever(int motor, int speed);
    void driveMotorXYForever(int speedX, int speedY);
    void setLedOn(bool enable, int r, int g, int b);
    void sendStatusUpdate();
    void requestObjectiveSlot();
    void setObjectiveSlot(int slot);
    void snapImage();
    void updateSamplePosition(float x, float y);
    void startSerialTask();
};