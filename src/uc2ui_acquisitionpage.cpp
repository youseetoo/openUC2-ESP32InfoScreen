#include "uc2ui_acquisitionpage.h"
#include "lvgl_helper.h"

namespace uc2ui_acquisitionpage {
    void initUI(lv_obj_t *parent) {
        // Create container for acquisition controls
        lv_obj_t *container = lv_obj_create(parent);
        lv_obj_set_size(container, lv_pct(100), lv_pct(100));
        lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_all(container, 20, 0);
        
        // Title
        lv_obj_t *title = lv_label_create(container);
        lv_label_set_text(title, "Image Acquisition");
        lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(title, lv_pct(100));
        
        // Capture button
        lv_obj_t *snap_btn = lv_btn_create(container);
        lv_obj_set_size(snap_btn, 200, 60);
        lv_obj_t *snap_label = lv_label_create(snap_btn);
        lv_label_set_text(snap_label, "CAPTURE IMAGE");
        lv_obj_set_style_text_font(snap_label, &lv_font_montserrat_18, 0);
        lv_obj_center(snap_label);
        
        // Settings placeholder
        lv_obj_t *settings_container = lv_obj_create(container);
        lv_obj_set_size(settings_container, lv_pct(80), 200);
        lv_obj_set_flex_flow(settings_container, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(settings_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_style_pad_all(settings_container, 15, 0);
        
        lv_obj_t *settings_title = lv_label_create(settings_container);
        lv_label_set_text(settings_title, "Acquisition Settings");
        lv_obj_set_style_text_font(settings_title, &lv_font_montserrat_18, 0);
        
        lv_obj_t *exposure_label = lv_label_create(settings_container);
        lv_label_set_text(exposure_label, "Exposure: Auto");
        
        lv_obj_t *format_label = lv_label_create(settings_container);
        lv_label_set_text(format_label, "Format: JPEG");
        
        lv_obj_t *resolution_label = lv_label_create(settings_container);
        lv_label_set_text(resolution_label, "Resolution: 1920x1080");
        
        // Info text
        lv_obj_t *info = lv_label_create(container);
        lv_label_set_text(info, "Configure capture settings and trigger image acquisition");
        lv_obj_set_style_text_font(info, &lv_font_montserrat_18, 0);
        lv_obj_set_style_text_align(info, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(info, lv_color_hex(0x666666), 0);
        lv_obj_set_width(info, lv_pct(100));
    }
    
    void setCaptureButtonCallback(capture_button_callback callback) {
        capture_callback = callback;
    }
    
    void triggerCapture() {
        // Programmatically trigger the capture callback
        if (capture_callback) {
            capture_callback();
        }
    }
}