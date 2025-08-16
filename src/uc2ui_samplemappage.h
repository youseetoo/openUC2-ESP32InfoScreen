#pragma once
#include <lvgl.h>

namespace uc2ui_samplemappage {
    typedef void (*sample_map_click_callback)(int pixel_x, int pixel_y, int sample_number);
    
    void initUI(lv_obj_t *parent);
    void setSampleMapClickCallback(sample_map_click_callback callback);
    void updatePosition(float x, float y);
}