#pragma once
#include "lvgl.h"
#include "WString.h"

namespace uc2ui_controller
{
    void initUi();
    void showMicroscopePage(bool show);
    bool displayImage(const String& tabName, int width, int height, const String& format, const String& imageData);
    int decodeBase64(const String& input, uint8_t* output, int maxLen);
};