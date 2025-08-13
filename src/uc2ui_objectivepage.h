#pragma once
#include "lvgl.h"

namespace uc2ui_objectivepage
{
    void initUI(lv_obj_t * container);
    void setCurrentSlot(int slot);
    void setOnSlotChangeListener(void slotChangeListener(int slot));

}; // namespace uc2ui_objectivepage