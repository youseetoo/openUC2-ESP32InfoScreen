#pragma once
#include <lvgl.h>

namespace uc2ui_acquisitionpage {
    typedef void (*capture_button_callback)();
    
    void initUI(lv_obj_t *parent);
    void setCaptureButtonCallback(capture_button_callback callback);
}