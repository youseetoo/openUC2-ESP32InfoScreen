#pragma once
#include "Arduino.h"
namespace RestApi
{
    void init();
    void connectTo(String url);
    void websocket_updateColors(int r, int g, int b);
    void driveMotorForever(int motor, int speed);
    void driveMotorXYForever(int speedX, int speedY);
    void loop();
    void setLedOn(bool enable,int r,int g,int b);
};