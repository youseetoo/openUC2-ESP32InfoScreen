#include "uc2ui_objectivepage.h"

namespace uc2ui_objectivepage
{
    // UI Components
    lv_obj_t * currentSlotLabel;
    lv_obj_t * slot1Button;
    lv_obj_t * slot2Button;
    lv_obj_t * snapButton;
    lv_obj_t * sampleMapCanvas;
    lv_obj_t * positionDot;

    // State
    int current_slot = 1;
    float current_x = 0.0f;
    float current_y = 0.0f;

    // Callbacks
    void (*slotChangeCallback)(int slot) = nullptr;
    void (*snapButtonCallback)() = nullptr;

    // Event handlers
    static void slot1_button_event_handler(lv_event_t * e)
    {
        lv_event_code_t code = lv_event_get_code(e);
        if (code == LV_EVENT_CLICKED) {
            if (slotChangeCallback != nullptr) {
                slotChangeCallback(1);
            }
        }
    }

    static void slot2_button_event_handler(lv_event_t * e)
    {
        lv_event_code_t code = lv_event_get_code(e);
        if (code == LV_EVENT_CLICKED) {
            if (slotChangeCallback != nullptr) {
                slotChangeCallback(2);
            }
        }
    }

    static void snap_button_event_handler(lv_event_t * e)
    {
        lv_event_code_t code = lv_event_get_code(e);
        if (code == LV_EVENT_CLICKED) {
            if (snapButtonCallback != nullptr) {
                snapButtonCallback();
            }
        }
    }

    void initUI(lv_obj_t * container)
    {
        // Create main container
        lv_obj_t * main_container = lv_obj_create(container);
        lv_obj_set_width(main_container, lv_pct(100));
        lv_obj_set_height(main_container, lv_pct(100));
        lv_obj_set_align(main_container, LV_ALIGN_CENTER);
        lv_obj_clear_flag(main_container, LV_OBJ_FLAG_SCROLLABLE);

        // Current slot display
        lv_obj_t * slot_container = lv_obj_create(main_container);
        lv_obj_set_width(slot_container, lv_pct(100));
        lv_obj_set_height(slot_container, 60);
        lv_obj_set_x(slot_container, 0);
        lv_obj_set_y(slot_container, 10);
        lv_obj_set_align(slot_container, LV_ALIGN_TOP_MID);

        lv_obj_t * slot_label = lv_label_create(slot_container);
        lv_label_set_text(slot_label, "Current Objective Slot:");
        lv_obj_set_align(slot_label, LV_ALIGN_TOP_LEFT);

        currentSlotLabel = lv_label_create(slot_container);
        lv_label_set_text(currentSlotLabel, "Slot 1");
        lv_obj_set_x(currentSlotLabel, 0);
        lv_obj_set_y(currentSlotLabel, 25);
        lv_obj_set_align(currentSlotLabel, LV_ALIGN_TOP_LEFT);
        
        // Style current slot label
        static lv_style_t slot_style;
        lv_style_init(&slot_style);
        // Set font to montserrat 18, since montserrat 16 is not available
        lv_style_set_text_font(&slot_style, &lv_font_montserrat_18);
        lv_style_set_text_color(&slot_style, lv_color_hex(0x0099ff));
        lv_obj_add_style(currentSlotLabel, &slot_style, 0);

        // Objective slot buttons
        lv_obj_t * button_container = lv_obj_create(main_container);
        lv_obj_set_width(button_container, lv_pct(100));
        lv_obj_set_height(button_container, 60);
        lv_obj_set_x(button_container, 0);
        lv_obj_set_y(button_container, 80);
        lv_obj_set_align(button_container, LV_ALIGN_TOP_MID);

        slot1Button = lv_btn_create(button_container);
        lv_obj_set_width(slot1Button, 80);
        lv_obj_set_height(slot1Button, 40);
        lv_obj_set_x(slot1Button, -50);
        lv_obj_set_align(slot1Button, LV_ALIGN_CENTER);
        lv_obj_add_event_cb(slot1Button, slot1_button_event_handler, LV_EVENT_ALL, NULL);

        lv_obj_t * slot1_label = lv_label_create(slot1Button);
        lv_label_set_text(slot1_label, "Slot 1");
        lv_obj_set_align(slot1_label, LV_ALIGN_CENTER);

        slot2Button = lv_btn_create(button_container);
        lv_obj_set_width(slot2Button, 80);
        lv_obj_set_height(slot2Button, 40);
        lv_obj_set_x(slot2Button, 50);
        lv_obj_set_align(slot2Button, LV_ALIGN_CENTER);
        lv_obj_add_event_cb(slot2Button, slot2_button_event_handler, LV_EVENT_ALL, NULL);

        lv_obj_t * slot2_label = lv_label_create(slot2Button);
        lv_label_set_text(slot2_label, "Slot 2");
        lv_obj_set_align(slot2_label, LV_ALIGN_CENTER);

        // Snap button
        lv_obj_t * snap_container = lv_obj_create(main_container);
        lv_obj_set_width(snap_container, lv_pct(100));
        lv_obj_set_height(snap_container, 60);
        lv_obj_set_x(snap_container, 0);
        lv_obj_set_y(snap_container, 150);
        lv_obj_set_align(snap_container, LV_ALIGN_TOP_MID);

        snapButton = lv_btn_create(snap_container);
        lv_obj_set_width(snapButton, 120);
        lv_obj_set_height(snapButton, 50);
        lv_obj_set_align(snapButton, LV_ALIGN_CENTER);
        lv_obj_add_event_cb(snapButton, snap_button_event_handler, LV_EVENT_ALL, NULL);

        // Style snap button
        static lv_style_t snap_style;
        lv_style_init(&snap_style);
        lv_style_set_bg_color(&snap_style, lv_color_hex(0x00aa00));
        lv_obj_add_style(snapButton, &snap_style, 0);

        lv_obj_t * snap_label = lv_label_create(snapButton);
        lv_label_set_text(snap_label, "SNAP");
        lv_obj_set_align(snap_label, LV_ALIGN_CENTER);

        // Sample map display
        lv_obj_t * map_container = lv_obj_create(main_container);
        lv_obj_set_width(map_container, lv_pct(100));
        lv_obj_set_height(map_container, 120);
        lv_obj_set_x(map_container, 0);
        lv_obj_set_y(map_container, 220);
        lv_obj_set_align(map_container, LV_ALIGN_TOP_MID);

        lv_obj_t * map_label = lv_label_create(map_container);
        lv_label_set_text(map_label, "Sample Map:");
        lv_obj_set_align(map_label, LV_ALIGN_TOP_LEFT);

        // Create a simple sample map representation
        sampleMapCanvas = lv_obj_create(map_container);
        lv_obj_set_width(sampleMapCanvas, 200);
        lv_obj_set_height(sampleMapCanvas, 80);
        lv_obj_set_x(sampleMapCanvas, 0);
        lv_obj_set_y(sampleMapCanvas, 25);
        lv_obj_set_align(sampleMapCanvas, LV_ALIGN_TOP_MID);
        
        // Style map canvas
        static lv_style_t map_style;
        lv_style_init(&map_style);
        lv_style_set_bg_color(&map_style, lv_color_hex(0x333333));
        lv_style_set_border_color(&map_style, lv_color_hex(0x666666));
        lv_style_set_border_width(&map_style, 2);
        lv_obj_add_style(sampleMapCanvas, &map_style, 0);

        // Position indicator dot
        positionDot = lv_obj_create(sampleMapCanvas);
        lv_obj_set_width(positionDot, 8);
        lv_obj_set_height(positionDot, 8);
        lv_obj_set_align(positionDot, LV_ALIGN_CENTER);
        
        // Style position dot
        static lv_style_t dot_style;
        lv_style_init(&dot_style);
        lv_style_set_bg_color(&dot_style, lv_color_hex(0xff0000)); // Red dot
        lv_style_set_radius(&dot_style, LV_RADIUS_CIRCLE);
        lv_style_set_border_width(&dot_style, 0);
        lv_obj_add_style(positionDot, &dot_style, 0);

        // Set initial slot
        setCurrentSlot(1);
    }

    void setCurrentSlot(int slot)
    {
        current_slot = slot;
        if (currentSlotLabel != nullptr) {
            lv_label_set_text_fmt(currentSlotLabel, "Slot %d", slot);
        }
        
        // Update button styles to show current selection
        if (slot1Button != nullptr && slot2Button != nullptr) {
            static lv_style_t active_style;
            lv_style_init(&active_style);
            lv_style_set_bg_color(&active_style, lv_color_hex(0x0099ff));
            
            static lv_style_t inactive_style;
            lv_style_init(&inactive_style);
            lv_style_set_bg_color(&inactive_style, lv_color_hex(0x555555));
            
            if (slot == 1) {
                lv_obj_add_style(slot1Button, &active_style, 0);
                lv_obj_add_style(slot2Button, &inactive_style, 0);
            } else {
                lv_obj_add_style(slot1Button, &inactive_style, 0);
                lv_obj_add_style(slot2Button, &active_style, 0);
            }
        }
    }

    void setOnSlotChangeListener(void slotChangeListener(int slot))
    {
        slotChangeCallback = slotChangeListener;
    }

    void setOnSnapButtonListener(void snapButtonListener())
    {
        snapButtonCallback = snapButtonListener;
    }

    void updateSampleMap(float x, float y)
    {
        current_x = x;
        current_y = y;
        
        if (positionDot != nullptr && sampleMapCanvas != nullptr) {
            // Map position to canvas coordinates (assuming normalized coordinates 0-1)
            int canvas_width = lv_obj_get_width(sampleMapCanvas);
            int canvas_height = lv_obj_get_height(sampleMapCanvas);
            
            int dot_x = (int)(x * canvas_width);
            int dot_y = (int)(y * canvas_height);
            
            // Clamp to canvas boundaries
            dot_x = dot_x < 4 ? 4 : (dot_x > canvas_width - 4 ? canvas_width - 4 : dot_x);
            dot_y = dot_y < 4 ? 4 : (dot_y > canvas_height - 4 ? canvas_height - 4 : dot_y);
            
            lv_obj_set_x(positionDot, dot_x - 4); // Center the dot
            lv_obj_set_y(positionDot, dot_y - 4);
        }
    }
}; // namespace uc2ui_objectivepage