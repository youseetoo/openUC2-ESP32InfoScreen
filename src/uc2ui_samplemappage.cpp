#include "uc2ui_samplemappage.h"
#include "lvgl_helper.h"

namespace uc2ui_samplemappage {
    void initUI(lv_obj_t *parent) {
        // Create container for sample map
        lv_obj_t *container = lv_obj_create(parent);
        lv_obj_set_size(container, lv_pct(100), lv_pct(100));
        lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_all(container, 20, 0);
        
        // Title
        lv_obj_t *title = lv_label_create(container);
        lv_label_set_text(title, "Sample Map");
        lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(title, lv_pct(100));
        
        // Placeholder for sample map grid
        lv_obj_t *map_container = lv_obj_create(container);
        lv_obj_set_size(map_container, 400, 300);
        lv_obj_set_style_border_width(map_container, 2, 0);
        lv_obj_set_style_border_color(map_container, lv_color_hex(0x333333), 0);
        lv_obj_set_style_radius(map_container, 10, 0);
        lv_obj_set_style_bg_color(map_container, lv_color_hex(0xF0F0F0), 0);
        
        // Red position indicator (centered initially)
        lv_obj_t *position_indicator = lv_obj_create(map_container);
        lv_obj_set_size(position_indicator, 20, 20);
        lv_obj_set_style_bg_color(position_indicator, lv_color_hex(0xFF0000), 0);
        lv_obj_set_style_radius(position_indicator, 10, 0);
        lv_obj_set_style_border_width(position_indicator, 0, 0);
        lv_obj_center(position_indicator);
        
        // Info text
        lv_obj_t *info = lv_label_create(container);
        lv_label_set_text(info, "Red dot shows current sample position\nClick to move to different positions");
        lv_obj_set_style_text_font(info, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_align(info, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(info, lv_color_hex(0x666666), 0);
        lv_obj_set_width(info, lv_pct(100));
    }
}