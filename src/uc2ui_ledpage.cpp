#include "uc2ui_ledpage.h"
#include "lvgl_helper.h"

namespace uc2ui_ledpage
{
    bool led_module = false;
    int led_count;
    bool led_on = false;

    lv_obj_t *ledPanel;
    lv_obj_t *LedOnCheckBox;
    lv_obj_t *LabelBlue;
    lv_obj_t *LabelGreen;
    lv_obj_t *LabelRed;
    lv_obj_t *SliderBlue;
    lv_obj_t *SliderGreen;
    lv_obj_t *SliderRed;

    void (*updatedColorsListner)(int r, int g, int b);

    void setColorChangedListner(void colorchangedlistner(int r, int g, int b))
    {
        updatedColorsListner = colorchangedlistner;
    }

    void (*enableLedListner)(bool enable, int r, int g, int b);

    void setenableLedListner(void ledlistner(bool enable, int r, int g, int b))
    {
        enableLedListner = ledlistner;
    }

    void setLedModule(bool enable)
    {
        led_module = enable;
        // Only update visibility if UI has been initialized
        if (ledPanel != nullptr) {
            lvgl_helper::setVisibility(ledPanel, enable);
        }
    }

    void setLedOn(bool on)
    {
        led_on = on;
        // Only update state if UI has been initialized
        if (LedOnCheckBox != nullptr) {
            if (on)
                lv_obj_add_state(LedOnCheckBox, LV_STATE_CHECKED);
            else
                lv_obj_clear_state(LedOnCheckBox, LV_STATE_CHECKED);
        }
    }

    void setLedCount(int count)
    {
        led_count = count;
    }

    void onColorSliderChanged(lv_event_t *e)
    {
        lv_event_code_t event_code = lv_event_get_code(e);
        lv_obj_t *target = lv_event_get_target(e);
        if (event_code == LV_EVENT_VALUE_CHANGED)
        {
            int r = lv_slider_get_value(SliderRed);
            int g = lv_slider_get_value(SliderGreen);
            int b = lv_slider_get_value(SliderBlue);
            if (updatedColorsListner != nullptr)
                updatedColorsListner(r, g, b);
        }
    }

    void ledCheckBox_event_handler(lv_event_t *e)
    {
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t *obj = lv_event_get_target(e);
        if (code == LV_EVENT_VALUE_CHANGED)
        {
            int r = lv_slider_get_value(SliderRed);
            int g = lv_slider_get_value(SliderGreen);
            int b = lv_slider_get_value(SliderBlue);
            bool state = lv_obj_get_state(obj) & LV_STATE_CHECKED;
            if (enableLedListner != nullptr)
                enableLedListner(state, r, g, b);
        }
    }

    void initLedPanel()
    {

        LedOnCheckBox = lv_checkbox_create(ledPanel);
        lv_checkbox_set_text(LedOnCheckBox, "Led On");
        lv_obj_set_width(LedOnCheckBox, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LedOnCheckBox, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LedOnCheckBox, -130);
        lv_obj_set_y(LedOnCheckBox, 30);
        lv_obj_set_align(LedOnCheckBox, LV_ALIGN_TOP_MID);
        lv_obj_add_flag(LedOnCheckBox, LV_OBJ_FLAG_SCROLL_ON_FOCUS); /// Flags

        LabelRed = lv_label_create(ledPanel);
        lv_obj_set_width(LabelRed, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelRed, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelRed, 0);
        lv_obj_set_y(LabelRed, 70);
        lv_obj_set_align(LabelRed, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelRed, "Red");

        SliderRed = lv_slider_create(ledPanel);
        lv_slider_set_range(SliderRed, 0, 255);
        lv_slider_set_value(SliderRed, 255, LV_ANIM_OFF);
        if (lv_slider_get_mode(SliderRed) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(SliderRed, 0, LV_ANIM_OFF);
        lv_obj_set_width(SliderRed, lv_pct(100));
        lv_obj_set_height(SliderRed, 20);
        lv_obj_set_x(SliderRed, 0);
        lv_obj_set_y(SliderRed, 90);
        lv_obj_set_align(SliderRed, LV_ALIGN_TOP_MID);
        lv_obj_set_style_bg_color(SliderRed, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderRed, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_bg_color(SliderRed, lv_color_hex(0xFF0000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderRed, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

        LabelGreen = lv_label_create(ledPanel);
        lv_obj_set_width(LabelGreen, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelGreen, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelGreen, 0);
        lv_obj_set_y(LabelGreen, 120);
        lv_obj_set_align(LabelGreen, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelGreen, "Green");

        SliderGreen = lv_slider_create(ledPanel);
        lv_slider_set_range(SliderGreen, 0, 255);
        lv_slider_set_value(SliderGreen, 255, LV_ANIM_OFF);
        if (lv_slider_get_mode(SliderGreen) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(SliderGreen, 0, LV_ANIM_OFF);
        lv_obj_set_width(SliderGreen, lv_pct(100));
        lv_obj_set_height(SliderGreen, 20);
        lv_obj_set_x(SliderGreen, 0);
        lv_obj_set_y(SliderGreen, 140);
        lv_obj_set_align(SliderGreen, LV_ALIGN_TOP_MID);
        lv_obj_set_style_bg_color(SliderGreen, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderGreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_bg_color(SliderGreen, lv_color_hex(0x00FF00), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderGreen, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

        LabelBlue = lv_label_create(ledPanel);
        lv_obj_set_width(LabelBlue, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelBlue, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelBlue, 0);
        lv_obj_set_y(LabelBlue, 170);
        lv_obj_set_align(LabelBlue, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelBlue, "Blue");

        SliderBlue = lv_slider_create(ledPanel);
        lv_slider_set_range(SliderBlue, 0, 255);
        lv_slider_set_value(SliderBlue, 255, LV_ANIM_OFF);
        if (lv_slider_get_mode(SliderBlue) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(SliderBlue, 0, LV_ANIM_OFF);
        lv_obj_set_width(SliderBlue, lv_pct(100));
        lv_obj_set_height(SliderBlue, 20);
        lv_obj_set_x(SliderBlue, 0);
        lv_obj_set_y(SliderBlue, 190);
        lv_obj_set_align(SliderBlue, LV_ALIGN_TOP_MID);
        lv_obj_set_style_bg_color(SliderBlue, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderBlue, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_bg_color(SliderBlue, lv_color_hex(0x0000FF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderBlue, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

        lv_obj_add_event_cb(SliderRed, onColorSliderChanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(SliderGreen, onColorSliderChanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(SliderBlue, onColorSliderChanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(LedOnCheckBox, ledCheckBox_event_handler, LV_EVENT_ALL, NULL);
    }

    void initUI(lv_obj_t *container)
    {
        // Create a tabview for LED controls
        lv_obj_t *led_tabview = lv_tabview_create(container, LV_DIR_TOP, 40);
        lv_obj_clear_flag(lv_tabview_get_content(led_tabview), LV_OBJ_FLAG_SCROLLABLE);
        
        // Tab 1: RGB color control
        lv_obj_t *color_tab = lv_tabview_add_tab(led_tabview, "RGB");
        initRGBUI(color_tab);
        
        // Tab 2: LED patterns
        lv_obj_t *pattern_tab = lv_tabview_add_tab(led_tabview, "Patterns");
        initPatternUI(pattern_tab);
        
        // Apply current LED states after UI initialization
        applyCurrentStates();
    }

    void initRGBUI(lv_obj_t *container)
    {
        ledPanel = lv_obj_create(container);
        lv_obj_set_height(ledPanel, lv_pct(100));
        lv_obj_set_width(ledPanel, lv_pct(100));
        lv_obj_set_flex_flow(ledPanel, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(ledPanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_clear_flag(ledPanel, LV_OBJ_FLAG_SCROLLABLE);
        lvgl_helper::setVisibility(ledPanel, false);

        initLedPanel();
    }

    void applyCurrentStates()
    {
        // Apply the current LED states that were set before UI initialization
        setLedModule(led_module);
        setLedOn(led_on);
        // LED count doesn't need UI update, just store the value
    }

    void initPatternUI(lv_obj_t *container)
    {
        // Create container for LED pattern buttons
        lv_obj_t *pattern_container = lv_obj_create(container);
        lv_obj_set_size(pattern_container, lv_pct(100), lv_pct(100));
        lv_obj_set_flex_flow(pattern_container, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(pattern_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_all(pattern_container, 20, 0);
        
        // Title
        lv_obj_t *title = lv_label_create(pattern_container);
        lv_label_set_text(title, "LED Pattern Control");
        lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(title, lv_pct(100));
        
        // Directional buttons row
        lv_obj_t *direction_container = lv_obj_create(pattern_container);
        lv_obj_set_size(direction_container, lv_pct(90), 80);
        lv_obj_set_flex_flow(direction_container, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(direction_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        
        // Top button
        lv_obj_t *top_btn = lv_btn_create(direction_container);
        lv_obj_set_size(top_btn, 80, 50);
        lv_obj_t *top_label = lv_label_create(top_btn);
        lv_label_set_text(top_label, "Top");
        lv_obj_center(top_label);
        lv_obj_add_event_cb(top_btn, pattern_button_cb, LV_EVENT_CLICKED, (void*)0);
        
        // Bottom button
        lv_obj_t *bottom_btn = lv_btn_create(direction_container);
        lv_obj_set_size(bottom_btn, 80, 50);
        lv_obj_t *bottom_label = lv_label_create(bottom_btn);
        lv_label_set_text(bottom_label, "Bottom");
        lv_obj_center(bottom_label);
        lv_obj_add_event_cb(bottom_btn, pattern_button_cb, LV_EVENT_CLICKED, (void*)1);
        
        // Left button
        lv_obj_t *left_btn = lv_btn_create(direction_container);
        lv_obj_set_size(left_btn, 80, 50);
        lv_obj_t *left_label = lv_label_create(left_btn);
        lv_label_set_text(left_label, "Left");
        lv_obj_center(left_label);
        lv_obj_add_event_cb(left_btn, pattern_button_cb, LV_EVENT_CLICKED, (void*)2);
        
        // Right button
        lv_obj_t *right_btn = lv_btn_create(direction_container);
        lv_obj_set_size(right_btn, 80, 50);
        lv_obj_t *right_label = lv_label_create(right_btn);
        lv_label_set_text(right_label, "Right");
        lv_obj_center(right_label);
        lv_obj_add_event_cb(right_btn, pattern_button_cb, LV_EVENT_CLICKED, (void*)3);
        
        // Ring buttons row
        lv_obj_t *ring_container = lv_obj_create(pattern_container);
        lv_obj_set_size(ring_container, lv_pct(90), 80);
        lv_obj_set_flex_flow(ring_container, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(ring_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        
        // Ring 1 button
        lv_obj_t *ring1_btn = lv_btn_create(ring_container);
        lv_obj_set_size(ring1_btn, 80, 50);
        lv_obj_t *ring1_label = lv_label_create(ring1_btn);
        lv_label_set_text(ring1_label, "Ring 1");
        lv_obj_center(ring1_label);
        lv_obj_add_event_cb(ring1_btn, pattern_button_cb, LV_EVENT_CLICKED, (void*)4);
        
        // Ring 2 button
        lv_obj_t *ring2_btn = lv_btn_create(ring_container);
        lv_obj_set_size(ring2_btn, 80, 50);
        lv_obj_t *ring2_label = lv_label_create(ring2_btn);
        lv_label_set_text(ring2_label, "Ring 2");
        lv_obj_center(ring2_label);
        lv_obj_add_event_cb(ring2_btn, pattern_button_cb, LV_EVENT_CLICKED, (void*)5);
        
        // Ring 3 button
        lv_obj_t *ring3_btn = lv_btn_create(ring_container);
        lv_obj_set_size(ring3_btn, 80, 50);
        lv_obj_t *ring3_label = lv_label_create(ring3_btn);
        lv_label_set_text(ring3_label, "Ring 3");
        lv_obj_center(ring3_label);
        lv_obj_add_event_cb(ring3_btn, pattern_button_cb, LV_EVENT_CLICKED, (void*)6);
        
        // All off button
        lv_obj_t *off_btn = lv_btn_create(pattern_container);
        lv_obj_set_size(off_btn, 120, 50);
        lv_obj_t *off_label = lv_label_create(off_btn);
        lv_label_set_text(off_label, "All Off");
        lv_obj_center(off_label);
        lv_obj_add_event_cb(off_btn, pattern_button_cb, LV_EVENT_CLICKED, (void*)7);
        lv_obj_set_style_bg_color(off_btn, lv_color_hex(0xFF0000), 0);
    }

    void pattern_button_cb(lv_event_t *e)
    {
        int pattern_id = (int)lv_event_get_user_data(e);
        
        // Pattern mapping:
        // 0 = Top, 1 = Bottom, 2 = Left, 3 = Right
        // 4 = Ring 1, 5 = Ring 2, 6 = Ring 3, 7 = All Off
        
        // For now, we'll send predefined RGB values for each pattern
        // This can be customized based on hardware implementation
        int r = 255, g = 255, b = 255;
        bool enable = true;
        
        switch(pattern_id) {
            case 0: // Top - White
                r = 255; g = 255; b = 255;
                break;
            case 1: // Bottom - Blue
                r = 0; g = 0; b = 255;
                break;
            case 2: // Left - Green
                r = 0; g = 255; b = 0;
                break;
            case 3: // Right - Red
                r = 255; g = 0; b = 0;
                break;
            case 4: // Ring 1 - Cyan
                r = 0; g = 255; b = 255;
                break;
            case 5: // Ring 2 - Magenta
                r = 255; g = 0; b = 255;
                break;
            case 6: // Ring 3 - Yellow
                r = 255; g = 255; b = 0;
                break;
            case 7: // All Off
                r = 0; g = 0; b = 0;
                enable = false;
                break;
        }
        
        // Send pattern command via the LED listener
        if (enableLedListner != nullptr) {
            enableLedListner(enable, r, g, b);
        }
    }
}
    