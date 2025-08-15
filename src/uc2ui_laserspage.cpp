#include "uc2ui_laserspage.h"
#include "lvgl_helper.h"

namespace uc2ui_laserspage {
    // PWM sliders for channels 1-4
    static lv_obj_t *pwm_sliders[4];
    static lv_obj_t *pwm_labels[4];
    static lv_obj_t *pwm_value_labels[4];
    
    // Callback for PWM updates
    static pwm_update_callback pwm_callback = nullptr;

    static void slider_event_cb(lv_event_t *e) {
        lv_obj_t *slider = lv_event_get_target(e);
        int value = lv_slider_get_value(slider);
        
        // Find which channel this slider belongs to
        int channel = -1;
        for (int i = 0; i < 4; i++) {
            if (pwm_sliders[i] == slider) {
                channel = i + 1; // Channels are 1-4
                break;
            }
        }
        
        if (channel > 0) {
            // Update the value label
            lv_label_set_text_fmt(pwm_value_labels[channel-1], "%d", value);
            
            // Call the callback if set
            if (pwm_callback) {
                pwm_callback(channel, value);
            }
        }
    }

    void initUI(lv_obj_t *parent) {
        // Create container for PWM controls
        lv_obj_t *container = lv_obj_create(parent);
        lv_obj_set_size(container, lv_pct(100), lv_pct(100));
        lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_all(container, 20, 0);
        
        // Title
        lv_obj_t *title = lv_label_create(container);
        lv_label_set_text(title, "PWM Laser Control");
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(title, lv_pct(100));
        
        // Create PWM sliders for channels 1-4
        for (int i = 0; i < 4; i++) {
            // Channel container
            lv_obj_t *channel_container = lv_obj_create(container);
            lv_obj_set_size(channel_container, lv_pct(90), 80);
            lv_obj_set_flex_flow(channel_container, LV_FLEX_FLOW_ROW);
            lv_obj_set_flex_align(channel_container, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
            lv_obj_set_style_border_width(channel_container, 1, 0);
            lv_obj_set_style_border_color(channel_container, lv_color_hex(0x888888), 0);
            lv_obj_set_style_radius(channel_container, 10, 0);
            
            // Channel label
            pwm_labels[i] = lv_label_create(channel_container);
            lv_label_set_text_fmt(pwm_labels[i], "PWM %d:", i + 1);
            lv_obj_set_width(pwm_labels[i], 80);
            
            // Slider
            pwm_sliders[i] = lv_slider_create(channel_container);
            lv_obj_set_size(pwm_sliders[i], 300, 30);
            lv_slider_set_range(pwm_sliders[i], 0, 1024);
            lv_slider_set_value(pwm_sliders[i], 0, LV_ANIM_OFF);
            lv_obj_add_event_cb(pwm_sliders[i], slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
            
            // Value label  
            pwm_value_labels[i] = lv_label_create(channel_container);
            lv_label_set_text(pwm_value_labels[i], "0");
            lv_obj_set_width(pwm_value_labels[i], 50);
            lv_obj_set_style_text_align(pwm_value_labels[i], LV_TEXT_ALIGN_CENTER, 0);
        }
        
        // Info text
        lv_obj_t *info = lv_label_create(container);
        lv_label_set_text(info, "PWM values range from 0 to 1024\nUse sliders to control laser intensity");
        lv_obj_set_style_text_align(info, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(info, lv_color_hex(0x666666), 0);
        lv_obj_set_width(info, lv_pct(100));
    }
    
    void setPwmUpdateCallback(pwm_update_callback callback) {
        pwm_callback = callback;
    }
    
    void updatePwmSlider(int channel, int value) {
        if (channel >= 1 && channel <= 4) {
            int index = channel - 1; // Convert to 0-based index
            if (pwm_sliders[index] != nullptr && pwm_value_labels[index] != nullptr) {
                lv_slider_set_value(pwm_sliders[index], value, LV_ANIM_OFF);
                lv_label_set_text_fmt(pwm_value_labels[index], "%d", value);
            }
        }
    }
}