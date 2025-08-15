#pragma once
#include <lvgl.h>

namespace uc2ui_laserspage {
    void initUI(lv_obj_t *parent);
    
    // Callback function types
    typedef void (*pwm_update_callback)(int channel, int value);
    
    // Set callback for PWM updates
    void setPwmUpdateCallback(pwm_update_callback callback);
    void updatePwmSlider(int channel, int value);
}