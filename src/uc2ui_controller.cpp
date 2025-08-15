#include "uc2ui_controller.h"
#include "uc2ui_wifipage.h"
#include "Arduino.h"
#include "uc2ui_microscopepage.h"
#include "uc2ui_motorpage.h"
#include "uc2ui_ledpage.h"
#include "uc2ui_objectivepage.h"
#include "uc2ui_laserspage.h"
#include "uc2ui_samplemappage.h"
#include "uc2ui_acquisitionpage.h"
#include "lvgl_helper.h"

namespace uc2ui_controller
{
    lv_obj_t *mainScreen;
    lv_obj_t *keyboard;
    lv_obj_t *ui_MainTabView;

    lv_obj_t *wifiPage;
    lv_obj_t *microscopepage;
    
    // Remember state before UI is initialized
    static bool microscope_page_should_be_shown = true;


    void on_textarea_focus_event(lv_event_t *e)
    {
        //log_i("on_textarea_focus_event");
        lv_event_code_t event_code = lv_event_get_code(e);
        lv_obj_t *target = lv_event_get_target(e);
        if (event_code == LV_EVENT_FOCUSED)
        {
            lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
            lv_keyboard_set_textarea(keyboard, target);
            lv_obj_update_layout(mainScreen);
            lv_obj_scroll_to_y(mainScreen, LV_COORD_MAX , LV_ANIM_ON);
        }
        if (event_code == LV_EVENT_DEFOCUSED)
        {
            lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
            lv_obj_update_layout(mainScreen);
            lv_obj_scroll_to_y(mainScreen, LV_COORD_MAX , LV_ANIM_ON);
        }
    }

    void initUi()
    {
        Serial.println("Starting UC2 UI initialization...");
        lv_disp_t *display = lv_disp_get_default();
        Serial.println("Got default display");
        lv_theme_t *theme = lv_theme_default_init(display, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                                  true, LV_FONT_DEFAULT);
        Serial.println("Theme initialized");
        lv_disp_set_theme(display, theme);
        // create mainscreen
        mainScreen = lv_obj_create(NULL);
        //lv_obj_clear_flag(mainScreen, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_flex_flow(mainScreen, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(mainScreen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

        ui_MainTabView = lv_tabview_create(mainScreen, LV_DIR_TOP, 40);
        lv_obj_set_width(ui_MainTabView, lv_pct(100));
        lv_obj_set_height(ui_MainTabView, lv_pct(100));
        lv_obj_set_align(ui_MainTabView, LV_ALIGN_CENTER);
        lv_obj_clear_flag(lv_tabview_get_content(ui_MainTabView), LV_OBJ_FLAG_SCROLLABLE);

        keyboard = lv_keyboard_create(mainScreen);
        lv_obj_set_width(keyboard, 800);
        lv_obj_set_height(keyboard, 240);
        lv_obj_set_align(keyboard, LV_ALIGN_BOTTOM_MID);
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

        // WiFi tab disabled for serial interface mode
        // wifiPage = lv_tabview_add_tab(ui_MainTabView, "Wifi");
        // uc2ui_wifipage::init_ui(wifiPage, on_textarea_focus_event);

        // Individual tabs replacing the nested Microscope tab structure
        lv_obj_t *motorpage = lv_tabview_add_tab(ui_MainTabView, "Motor");
        uc2ui_motorpage::initUI(motorpage);

        lv_obj_t *ledpage = lv_tabview_add_tab(ui_MainTabView, "LED");
        uc2ui_ledpage::initUI(ledpage);

        lv_obj_t *objectivepage = lv_tabview_add_tab(ui_MainTabView, "Objective");
        lv_obj_clear_flag(objectivepage, LV_OBJ_FLAG_SCROLLABLE);
        uc2ui_objectivepage::initUI(objectivepage);

        // TODO: Implement additional tabs as requested
        lv_obj_t *sampleMappage = lv_tabview_add_tab(ui_MainTabView, "Sample Map");
        uc2ui_samplemappage::initUI(sampleMappage);

        lv_obj_t *laserspage = lv_tabview_add_tab(ui_MainTabView, "Lasers");
        uc2ui_laserspage::initUI(laserspage);

        lv_obj_t *acquisitionpage = lv_tabview_add_tab(ui_MainTabView, "Acquisition");
        uc2ui_acquisitionpage::initUI(acquisitionpage);

        lv_disp_load_scr(mainScreen);
        
        // Apply any deferred state changes
        if (microscope_page_should_be_shown) {
            showMicroscopePage(true);
        }
    }

    void showMicroscopePage(bool show)
    {
        // Remember the desired state
        microscope_page_should_be_shown = show;
        
        // Only update visibility if UI has been initialized
        if (microscopepage != nullptr) {
            lvgl_helper::setVisibility(microscopepage, show);
        }
        // If UI not initialized yet, the state will be applied when initUi() is called
    }

}