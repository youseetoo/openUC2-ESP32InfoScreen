#include "uc2ui_laserspage.h"
#include "lvgl_helper.h"

namespace uc2ui_laserspage {
    // PWM sliders for channels 1-4
    static lv_obj_t *pwm_sliders[4];
    static lv_obj_t *pwm_labels[4];
    static lv_obj_t *pwm_value_labels[4];
    static lv_obj_t *pwm_switches[4];  // On/off switches for each channel
    static bool laser_enabled[4] = {false, false, false, false};  // Track enabled state
    
    // Callback for PWM updates
    static pwm_update_callback pwm_callback = nullptr;

    static void switch_event_cb(lv_event_t *e) {
        lv_obj_t *sw = lv_event_get_target(e);
        
        // Find which channel this switch belongs to
        int channel = -1;
        for (int i = 0; i < 4; i++) {
            if (pwm_switches[i] == sw) {
                channel = i + 1; // Channels are 1-4
                break;
            }
        }
        
        if (channel > 0) {
            laser_enabled[channel-1] = lv_obj_has_state(sw, LV_STATE_CHECKED);
            
            // If disabled, send 0 value; if enabled, send current slider value
            int value = 0;
            if (laser_enabled[channel-1]) {
                value = lv_slider_get_value(pwm_sliders[channel-1]);
            }
            
            // Call the callback with the appropriate value
            if (pwm_callback) {
                pwm_callback(channel, value);
            }
        }
    }

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
            
            // Only send value if laser is enabled
            int actual_value = laser_enabled[channel-1] ? value : 0;
            
            // Call the callback if set
            if (pwm_callback) {
                pwm_callback(channel, actual_value);
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
        lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
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
            
            // On/off switch
            pwm_switches[i] = lv_switch_create(channel_container);
            lv_obj_set_size(pwm_switches[i], 50, 25);
            lv_obj_add_event_cb(pwm_switches[i], switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
            
            // Channel label
            pwm_labels[i] = lv_label_create(channel_container);
            lv_label_set_text_fmt(pwm_labels[i], "PWM %d:", i + 1);
            lv_obj_set_style_text_font(pwm_labels[i], &lv_font_montserrat_18, 0);
            lv_obj_set_width(pwm_labels[i], 80);
            
            // Slider
            pwm_sliders[i] = lv_slider_create(channel_container);
            lv_obj_set_size(pwm_sliders[i], 250, 30);
            lv_slider_set_range(pwm_sliders[i], 0, 1024);
            lv_slider_set_value(pwm_sliders[i], 0, LV_ANIM_OFF);
            lv_obj_add_event_cb(pwm_sliders[i], slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
            
            // Value label  
            pwm_value_labels[i] = lv_label_create(channel_container);
            lv_label_set_text(pwm_value_labels[i], "0");
            lv_obj_set_style_text_font(pwm_value_labels[i], &lv_font_montserrat_18, 0);
            lv_obj_set_width(pwm_value_labels[i], 50);
            lv_obj_set_style_text_align(pwm_value_labels[i], LV_TEXT_ALIGN_CENTER, 0);
        }
        
        // Info text
        lv_obj_t *info = lv_label_create(container);
        lv_label_set_text(info, "Toggle switches to enable lasers\nPWM values range from 0 to 1024");
        lv_obj_set_style_text_font(info, &lv_font_montserrat_18, 0);
        lv_obj_set_style_text_align(info, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(info, lv_color_hex(0x666666), 0);
        lv_obj_set_width(info, lv_pct(100));
    }
    
    void setPwmUpdateCallback(pwm_update_callback callback) {
        pwm_callback = callback;
    }
}