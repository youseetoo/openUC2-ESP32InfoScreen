#pragma once
#include "lvgl.h"

namespace uc2ui_ledpage
{
    void setLedModule(bool enable);
    void setLedOn(bool on);
    void setLedCount(int count);
    void setColorChangedListner(void colorchangedlistner(int r, int g, int b));
    void setenableLedListner(void ledlistner(bool enable,int r, int g, int b));
    void initUI(lv_obj_t * container);
    void initRGBUI(lv_obj_t *container);
    void initPatternUI(lv_obj_t *container);
    void pattern_button_cb(lv_event_t *e);
    void applyCurrentStates(); // Apply LED states after UI initialization
};