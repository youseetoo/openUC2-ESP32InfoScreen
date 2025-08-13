#pragma once
#include "lvgl.h"
namespace uc2ui_motorpage
{
    void initUI(lv_obj_t *container);
    void initJoystickUI(lv_obj_t *container);
    void initStepUI(lv_obj_t *container);
    void step_button_cb(lv_event_t *e);
    void setDriveXYMotorListner(void driveXYMotorListner(int speedX, int speedY));
    void setMotorModule(bool enable);
    void setMotorX(bool enable);
    void setMotorY(bool enable);
    void setMotorZ(bool enable);
    void setMotorA(bool enable);
    void setUpdateMotorSpeedListner(void updateMotorSpeed(int motor, int speed));
    void setMoveMotorStepsListner(void moveMotorSteps(int motor, int steps));
    void applyCurrentStates(); // Apply motor states after UI initialization
}; // namespace uc2ui_motorpage
