#include "uc2ui_samplemappage.h"
#include "lvgl_helper.h"

namespace uc2ui_samplemappage {
    static sample_map_click_callback click_callback = nullptr;
    static lv_obj_t *map_container = nullptr;
    static lv_obj_t *position_indicator = nullptr;

    static void map_double_tap_cb(lv_event_t *e) {
        if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
            lv_obj_t *obj = lv_event_get_target(e);
            lv_indev_t *indev = lv_indev_get_act();
            lv_point_t point;
            lv_indev_get_point(indev, &point);
            
            // Convert to local coordinates relative to map container
            // Account for padding and border properly
            lv_area_t area;
            lv_obj_get_coords(map_container, &area);
            lv_coord_t pad_left = lv_obj_get_style_pad_left(map_container, LV_PART_MAIN);
            lv_coord_t pad_top = lv_obj_get_style_pad_top(map_container, LV_PART_MAIN);
            lv_coord_t border_width = lv_obj_get_style_border_width(map_container, LV_PART_MAIN);
            
            int local_x = point.x - area.x1 - pad_left - border_width;
            int local_y = point.y - area.y1 - pad_top - border_width;
            
            // Ensure coordinates are within bounds
            int map_width = lv_obj_get_width(map_container) - 2 * (pad_left + border_width);
            int map_height = lv_obj_get_height(map_container) - 2 * (pad_top + border_width);
            
            if (local_x < 0) local_x = 0;
            if (local_y < 0) local_y = 0;
            if (local_x >= map_width) local_x = map_width - 1;
            if (local_y >= map_height) local_y = map_height - 1;
            
            // Update position indicator (accounting for padding/border)
            lv_obj_set_pos(position_indicator, local_x + pad_left + border_width - 10, 
                          local_y + pad_top + border_width - 10);
            
            // Calculate sample number (assuming 8x6 grid)
            int grid_width = 8;
            int grid_height = 6;
            
            int sample_x = (local_x * grid_width) / map_width;
            int sample_y = (local_y * grid_height) / map_height;
            int sample_number = sample_y * grid_width + sample_x;
            
            // Ensure sample number is within bounds
            if (sample_number < 0) sample_number = 0;
            if (sample_number >= grid_width * grid_height) sample_number = grid_width * grid_height - 1;
            
            // Call callback with pixel coordinates and sample number
            if (click_callback) {
                click_callback(local_x, local_y, sample_number);
            }
        }
    }

    void initUI(lv_obj_t *parent) {
        // Create container for sample map with proper layout
        lv_obj_t *container = lv_obj_create(parent);
        lv_obj_set_size(container, lv_pct(100), lv_pct(100));
        lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_all(container, 10, 0);
        lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE); // Disable scrolling
        
        // Title
        lv_obj_t *title = lv_label_create(container);
        lv_label_set_text(title, "Sample Map");
        lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(title, lv_pct(100));
        
        // Sample map container with improved visibility
        map_container = lv_obj_create(container);
        lv_obj_set_width(map_container, lv_pct(95));
        lv_obj_set_flex_grow(map_container, 1); // Take all available space
        lv_obj_set_style_border_width(map_container, 2, 0);
        lv_obj_set_style_border_color(map_container, lv_color_hex(0x333333), 0);
        lv_obj_set_style_radius(map_container, 5, 0);
        lv_obj_set_style_bg_color(map_container, lv_color_hex(0xFFFFFF), 0); // Pure white background
        lv_obj_set_style_pad_all(map_container, 5, 0); // Minimal padding
        lv_obj_clear_flag(map_container, LV_OBJ_FLAG_SCROLLABLE); // Disable scrolling
        lv_obj_add_event_cb(map_container, map_double_tap_cb, LV_EVENT_CLICKED, NULL);
        
        // Red position indicator (centered initially)
        position_indicator = lv_obj_create(map_container);
        lv_obj_set_size(position_indicator, 20, 20);
        lv_obj_set_style_bg_color(position_indicator, lv_color_hex(0xFF0000), 0);
        lv_obj_set_style_radius(position_indicator, 10, 0);
        lv_obj_set_style_border_width(position_indicator, 0, 0);
        lv_obj_center(position_indicator);
        
        // Info text
        lv_obj_t *info = lv_label_create(container);
        lv_label_set_text(info, "Click to select sample position");
        lv_obj_set_style_text_font(info, &lv_font_montserrat_18, 0);
        lv_obj_set_style_text_align(info, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(info, lv_color_hex(0x666666), 0);
        lv_obj_set_width(info, lv_pct(100));
    }

    void setSampleMapClickCallback(sample_map_click_callback callback) {
        click_callback = callback;
    }

    void updatePosition(float x, float y) {
        if (!map_container || !position_indicator) return;
        
        // Convert normalized coordinates (0-1) to pixel coordinates
        lv_coord_t pad_left = lv_obj_get_style_pad_left(map_container, LV_PART_MAIN);
        lv_coord_t pad_top = lv_obj_get_style_pad_top(map_container, LV_PART_MAIN);
        lv_coord_t border_width = lv_obj_get_style_border_width(map_container, LV_PART_MAIN);
        
        int map_width = lv_obj_get_width(map_container) - 2 * (pad_left + border_width);
        int map_height = lv_obj_get_height(map_container) - 2 * (pad_top + border_width);
        
        int pixel_x = (int)(x * map_width);
        int pixel_y = (int)(y * map_height);
        
        // Update position indicator
        lv_obj_set_pos(position_indicator, pixel_x + pad_left + border_width - 10, 
                      pixel_y + pad_top + border_width - 10);
    }
}