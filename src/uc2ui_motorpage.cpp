#include "uc2ui_motorpage.h"
#include "lvgl_helper.h"
#include <Preferences.h>

namespace uc2ui_motorpage
{

    // Prevent scroll and gesture bubbling on joystick-related objects
    static void prevent_scroll_events(lv_event_t * e)
    {
        lv_event_code_t code = lv_event_get_code(e);
        if (code == LV_EVENT_GESTURE ||
            code == LV_EVENT_SCROLL_BEGIN ||
            code == LV_EVENT_SCROLL ||
            code == LV_EVENT_SCROLL_END) {
            // Stop this event from bubbling up and from being processed further
            lv_event_stop_bubbling(e);
            lv_event_stop_processing(e);
        }
    }

    bool motor_module = false;
    bool a_motor = false;
    bool x_motor = false;
    bool y_motor = false;
    bool z_motor = false;
    lv_obj_t *motorPanel = nullptr;
    lv_obj_t *motorASlider = nullptr;
    lv_obj_t *motorXSlider = nullptr;
    lv_obj_t *motorYSlider = nullptr;
    lv_obj_t *motorZSlider = nullptr;
    lv_obj_t *LabelA = nullptr;
    lv_obj_t *LabelX = nullptr;
    lv_obj_t *LabelY = nullptr;
    lv_obj_t *LabelZ = nullptr;

    lv_obj_t *joyContainer = nullptr;
    lv_obj_t *joyButton = nullptr;
    const int centerx = 400;
    const int centery = 240;
    const int halfsizecontainer = 175;
    int last_x_speed = 0;
    int last_y_speed = 0;
    int joybutton_real_x_pos;

    void (*updateMotorSpeedListner)(int motor, int speed);
    void (*moveMotorStepsListner)(int motor, int steps);

    // Position storage variables
    static Preferences preferences;
    static float currentX = 0.0f;
    static float currentY = 0.0f;
    static float currentZ = 0.0f;
    static lv_obj_t *positionsContainer = nullptr;
    static lv_obj_t *positionsList = nullptr;
    static lv_obj_t *currentPosLabel = nullptr;
    static void (*goToPositionListener)(float x, float y, float z) = nullptr;
    
    typedef struct {
        float x;
        float y; 
        float z;
        char name[32];
    } SavedPosition;
    
    const int MAX_POSITIONS = 20;

    void setUpdateMotorSpeedListner(void updateMotorSpeed(int motor, int speed))
    {
        updateMotorSpeedListner = updateMotorSpeed;
    }

    void setMoveMotorStepsListner(void moveMotorSteps(int motor, int steps))
    {
        moveMotorStepsListner = moveMotorSteps;
    }

    // Handle external step commands (e.g., from SerialApi)
    void handleStepCommand(int motor, int steps)
    {
        // Call the registered listener if available
        if (moveMotorStepsListner != nullptr) {
            moveMotorStepsListner(motor, steps);
        }
    }

    void (*driveXYMotor)(int speedX, int speedY);

    void setDriveXYMotorListner(void driveXYMotorListner(int speedX, int speedY))
    {
        driveXYMotor = driveXYMotorListner;
    }


    void onMotorSpeedChanged(lv_event_t *e, int motor)
    {
        lv_event_code_t event_code = lv_event_get_code(e);
        lv_obj_t *target = lv_event_get_target(e);
        if (event_code == LV_EVENT_VALUE_CHANGED)
        {
            int speed = lv_slider_get_value(target);
            if (updateMotorSpeedListner != nullptr)
                updateMotorSpeedListner(motor, speed);
        }
        if (event_code == LV_EVENT_RELEASED)
        {
            lv_slider_set_value(target, 17, LV_ANIM_OFF);
            if (updateMotorSpeedListner != nullptr)
                updateMotorSpeedListner(motor, 17);
        }
    }

    void onMotorAchanged(lv_event_t *e)
    {
        onMotorSpeedChanged(e, 0);
    }

    void onMotorXchanged(lv_event_t *e)
    {
        onMotorSpeedChanged(e, 1);
    }

    void onMotorYchanged(lv_event_t *e)
    {
        onMotorSpeedChanged(e, 2);
    }

    void onMotorZchanged(lv_event_t *e)
    {
        onMotorSpeedChanged(e, 3);
    }

    void hideAllMotorViews()
    {
        lvgl_helper::setVisibility(motorASlider, false);
        lvgl_helper::setVisibility(motorXSlider, false);
        lvgl_helper::setVisibility(motorYSlider, false);
        lvgl_helper::setVisibility(motorZSlider, false);

        lvgl_helper::setVisibility(LabelA, false);
        lvgl_helper::setVisibility(LabelX, false);
        lvgl_helper::setVisibility(LabelY, false);
        lvgl_helper::setVisibility(LabelZ, false);
    }

    void setMotorModule(bool enable)
    {
        motor_module = enable;
        // Only update visibility if UI has been initialized
        if (motorPanel != nullptr) {
            lvgl_helper::setVisibility(motorPanel, enable);
        }
    }

    void setMotorX(bool enable)
    {
        x_motor = enable;
        // Only update visibility if UI has been initialized
        if (motorXSlider != nullptr && LabelX != nullptr) {
            lvgl_helper::setVisibility(motorXSlider, enable);
            lvgl_helper::setVisibility(LabelX, enable);
            if (y_motor && x_motor && joyContainer != nullptr)
                lvgl_helper::setVisibility(joyContainer, enable);
        }
    }

    void setMotorY(bool enable)
    {
        y_motor = enable;
        // Only update visibility if UI has been initialized
        if (motorYSlider != nullptr && LabelY != nullptr) {
            lvgl_helper::setVisibility(motorYSlider, enable);
            lvgl_helper::setVisibility(LabelY, enable);
            if (y_motor && x_motor && joyContainer != nullptr)
                lvgl_helper::setVisibility(joyContainer, enable);
        }
    }

    void setMotorZ(bool enable)
    {
        z_motor = enable;
        // Only update visibility if UI has been initialized
        if (motorZSlider != nullptr && LabelZ != nullptr) {
            lvgl_helper::setVisibility(motorZSlider, enable);
            lvgl_helper::setVisibility(LabelZ, enable);
        }
    }

    void setMotorA(bool enable)
    {
        a_motor = enable;
        // Only update visibility if UI has been initialized
        if (motorASlider != nullptr && LabelA != nullptr) {
            lvgl_helper::setVisibility(motorASlider, enable);
            lvgl_helper::setVisibility(LabelA, enable);
        }
    }

void joyButtonEventListner(lv_event_t *e)
    {
        lv_event_code_t event_code = lv_event_get_code(e);
        lv_obj_t *target = lv_event_get_target(e);
        if (event_code == LV_EVENT_PRESSING)
        {
            lv_indev_t *indev = lv_indev_get_act();
            lv_point_t p;
            lv_indev_get_point(indev, &p);

            int transXtoCenter = p.x - (centerx +175);
            int transYtoCenter = p.y - centery;
            if (transXtoCenter > halfsizecontainer - 30)
                transXtoCenter = halfsizecontainer - 30;
            if (transXtoCenter < -(halfsizecontainer - 30))
                transXtoCenter = -(halfsizecontainer - 30);
            if (transYtoCenter > halfsizecontainer - 30)
                transYtoCenter = halfsizecontainer - 30;
            if (transYtoCenter < -(halfsizecontainer - 30))
                transYtoCenter = -(halfsizecontainer - 30);

            lv_obj_set_x(joyButton, transXtoCenter);
            lv_obj_set_y(joyButton, transYtoCenter);
            if (driveXYMotor != nullptr)
            {
                int x = transXtoCenter * 10;
                int y = transYtoCenter * 10;
                if (last_x_speed != x && last_y_speed != y)
                {
                    last_x_speed = x;
                    last_y_speed = y;
                    driveXYMotor(transXtoCenter * 10, transYtoCenter * 10);
                }
            }
        }
        if (event_code == LV_EVENT_RELEASED)
        {
            lv_obj_set_x(joyButton, 0);
            lv_obj_set_y(joyButton, 0);
            last_x_speed = 0;
            last_y_speed = 0;
            if (driveXYMotor != nullptr)
                driveXYMotor(0, 0);
        }
    }

    void initUI(lv_obj_t *container)
    {
        // Create a tabview for motor controls
        lv_obj_t *motor_tabview = lv_tabview_create(container, LV_DIR_TOP, 40);
    // Disable any scroll/gesture behavior on tabview and its content
    lv_obj_t *tv_content = lv_tabview_get_content(motor_tabview);
    lv_obj_clear_flag(tv_content, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(tv_content, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_clear_flag(tv_content, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(motor_tabview, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(motor_tabview, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_clear_flag(motor_tabview, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        
        // Tab 1: Joystick control
        lv_obj_t *joystick_tab = lv_tabview_add_tab(motor_tabview, "Joystick");
        initJoystickUI(joystick_tab);
        
        // Tab 2: Step movements
        lv_obj_t *step_tab = lv_tabview_add_tab(motor_tabview, "Steps");
        initStepUI(step_tab);
        
        // Tab 3: Position storage
        lv_obj_t *positions_tab = lv_tabview_add_tab(motor_tabview, "Positions");
        initPositionsUI(positions_tab);
    }

    void initJoystickUI(lv_obj_t *container)
    {
    // Ensure the tab container itself doesn't scroll or bubble gestures
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(container, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_event_cb(container, prevent_scroll_events, LV_EVENT_ALL, NULL);

        motorPanel = lv_obj_create(container);
        lv_obj_set_height(motorPanel, 350);
        lv_obj_set_width(motorPanel, lv_pct(50));
        lv_obj_clear_flag(motorPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(motorPanel, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_clear_flag(motorPanel, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_event_cb(motorPanel, prevent_scroll_events, LV_EVENT_ALL, NULL);
        lvgl_helper::setVisibility(motorPanel, false);

        motorASlider = lv_slider_create(motorPanel);
        lv_obj_set_width(motorASlider, 20);
        lv_obj_set_height(motorASlider, 250);
        lv_obj_set_x(motorASlider, -130);
        lv_obj_set_y(motorASlider, 40);
        lv_obj_set_align(motorASlider, LV_ALIGN_TOP_MID);
        lv_slider_set_range(motorASlider, 0, 35);
        lv_slider_set_value(motorASlider, 17, LV_ANIM_OFF);
        if (lv_slider_get_mode(motorASlider) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(motorASlider, 0, LV_ANIM_OFF);

        motorXSlider = lv_slider_create(motorPanel);
        lv_obj_set_width(motorXSlider, 20);
        lv_obj_set_height(motorXSlider, 250);
        lv_obj_set_x(motorXSlider, -50);
        lv_obj_set_y(motorXSlider, 40);
        lv_obj_set_align(motorXSlider, LV_ALIGN_TOP_MID);
        lv_slider_set_range(motorXSlider, 0, 35);
        lv_slider_set_value(motorXSlider, 17, LV_ANIM_OFF);
        if (lv_slider_get_mode(motorXSlider) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(motorXSlider, 0, LV_ANIM_OFF);

        motorYSlider = lv_slider_create(motorPanel);
        lv_obj_set_width(motorYSlider, 20);
        lv_obj_set_height(motorYSlider, 250);
        lv_obj_set_x(motorYSlider, 50);
        lv_obj_set_y(motorYSlider, 40);
        lv_obj_set_align(motorYSlider, LV_ALIGN_TOP_MID);
        lv_slider_set_range(motorYSlider, 0, 35);
        lv_slider_set_value(motorYSlider, 17, LV_ANIM_OFF);
        if (lv_slider_get_mode(motorYSlider) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(motorYSlider, 0, LV_ANIM_OFF);

        motorZSlider = lv_slider_create(motorPanel);
        lv_obj_set_width(motorZSlider, 20);
        lv_obj_set_height(motorZSlider, 250);
        lv_obj_set_x(motorZSlider, 130);
        lv_obj_set_y(motorZSlider, 40);
        lv_obj_set_align(motorZSlider, LV_ALIGN_TOP_MID);
        lv_slider_set_range(motorZSlider, 0, 35);
        lv_slider_set_value(motorZSlider, 17, LV_ANIM_OFF);
        if (lv_slider_get_mode(motorZSlider) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(motorZSlider, 0, LV_ANIM_OFF);

        LabelA = lv_label_create(motorPanel);
        lv_obj_set_width(LabelA, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelA, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelA, -130);
        lv_obj_set_y(LabelA, 10);
        lv_obj_set_align(LabelA, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelA, "A");

        LabelX = lv_label_create(motorPanel);
        lv_obj_set_width(LabelX, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelX, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelX, -50);
        lv_obj_set_y(LabelX, 10);
        lv_obj_set_align(LabelX, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelX, "X");

        LabelY = lv_label_create(motorPanel);
        lv_obj_set_width(LabelY, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelY, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelY, 50);
        lv_obj_set_y(LabelY, 10);
        lv_obj_set_align(LabelY, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelY, "Y");

        LabelZ = lv_label_create(motorPanel);
        lv_obj_set_width(LabelZ, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelZ, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelZ, 130);
        lv_obj_set_y(LabelZ, 10);
        lv_obj_set_align(LabelZ, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelZ, "Z");

        lv_obj_add_event_cb(motorASlider, onMotorAchanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(motorXSlider, onMotorXchanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(motorYSlider, onMotorYchanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(motorZSlider, onMotorZchanged, LV_EVENT_ALL, NULL);

    joyContainer = lv_obj_create(container);
        lv_obj_remove_style_all(joyContainer);
        lv_obj_set_width(joyContainer, 350);
        lv_obj_set_height(joyContainer, 350);
        lv_obj_set_align(joyContainer, LV_ALIGN_TOP_RIGHT);
    lv_obj_clear_flag(joyContainer, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(joyContainer, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_clear_flag(joyContainer, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_event_cb(joyContainer, prevent_scroll_events, LV_EVENT_ALL, NULL);
        lv_obj_set_style_radius(joyContainer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(joyContainer, lv_color_hex(0x7E7E7E), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(joyContainer, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_bg_color(joyContainer, lv_color_hex(0xFFFFFF), LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(joyContainer, 255, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);

    joyButton = lv_btn_create(joyContainer);
        lv_obj_set_width(joyButton, 50);
        lv_obj_set_height(joyButton, 50);
        lv_obj_set_align(joyButton, LV_ALIGN_CENTER);
        lv_obj_set_x(joyButton, 0);
        lv_obj_set_y(joyButton, 0);

        lv_area_t  c;
        lv_obj_get_coords(joyButton,&c);
        joybutton_real_x_pos = c.x1 + (c.x1 - c.x2 / 2);
        //lv_obj_add_flag(joyButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(joyButton, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(joyButton, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_clear_flag(joyButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_event_cb(joyButton, prevent_scroll_events, LV_EVENT_ALL, NULL);
        lv_obj_set_style_radius(joyButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(joyButton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(joyButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_add_event_cb(joyButton, joyButtonEventListner, LV_EVENT_ALL, NULL);

        // Apply current motor states after UI initialization
        applyCurrentStates();
        
        // hideAllMotorViews();
    }

    void applyCurrentStates()
    {
        // Apply the current motor states that were set before UI initialization
        setMotorModule(motor_module);
        setMotorA(a_motor);
        setMotorX(x_motor);
        setMotorY(y_motor);
        setMotorZ(z_motor);
    }

    void initStepUI(lv_obj_t *container)
    {
        // Create container for step movement buttons
        lv_obj_t *step_container = lv_obj_create(container);
        lv_obj_set_size(step_container, lv_pct(100), lv_pct(100));
        lv_obj_set_flex_flow(step_container, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(step_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_all(step_container, 20, 0);
        
        // Title
        lv_obj_t *title = lv_label_create(step_container);
        lv_label_set_text(title, "Step Movement Control");
        lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(title, lv_pct(100));
        
        // X-axis controls
        lv_obj_t *x_container = lv_obj_create(step_container);
        lv_obj_set_size(x_container, lv_pct(90), 80);
        lv_obj_set_flex_flow(x_container, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(x_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        
        lv_obj_t *x_label = lv_label_create(x_container);
        lv_label_set_text(x_label, "X:");
        lv_obj_set_style_text_font(x_label, &lv_font_montserrat_18, 0);
        
        lv_obj_t *x_minus_1000 = lv_btn_create(x_container);
        lv_obj_set_size(x_minus_1000, 80, 40);
        lv_obj_t *x_minus_1000_label = lv_label_create(x_minus_1000);
        lv_label_set_text(x_minus_1000_label, "-1000");
        lv_obj_center(x_minus_1000_label);
        lv_obj_add_event_cb(x_minus_1000, step_button_cb, LV_EVENT_CLICKED, (void*)0); // X motor, -1000
        
        lv_obj_t *x_minus_10 = lv_btn_create(x_container);
        lv_obj_set_size(x_minus_10, 60, 40);
        lv_obj_t *x_minus_10_label = lv_label_create(x_minus_10);
        lv_label_set_text(x_minus_10_label, "-10");
        lv_obj_center(x_minus_10_label);
        lv_obj_add_event_cb(x_minus_10, step_button_cb, LV_EVENT_CLICKED, (void*)1); // X motor, -10
        
        lv_obj_t *x_plus_10 = lv_btn_create(x_container);
        lv_obj_set_size(x_plus_10, 60, 40);
        lv_obj_t *x_plus_10_label = lv_label_create(x_plus_10);
        lv_label_set_text(x_plus_10_label, "+10");
        lv_obj_center(x_plus_10_label);
        lv_obj_add_event_cb(x_plus_10, step_button_cb, LV_EVENT_CLICKED, (void*)2); // X motor, +10
        
        lv_obj_t *x_plus_1000 = lv_btn_create(x_container);
        lv_obj_set_size(x_plus_1000, 80, 40);
        lv_obj_t *x_plus_1000_label = lv_label_create(x_plus_1000);
        lv_label_set_text(x_plus_1000_label, "+1000");
        lv_obj_center(x_plus_1000_label);
        lv_obj_add_event_cb(x_plus_1000, step_button_cb, LV_EVENT_CLICKED, (void*)3); // X motor, +1000
        
        // Y-axis controls
        lv_obj_t *y_container = lv_obj_create(step_container);
        lv_obj_set_size(y_container, lv_pct(90), 80);
        lv_obj_set_flex_flow(y_container, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(y_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        
        lv_obj_t *y_label = lv_label_create(y_container);
        lv_label_set_text(y_label, "Y:");
        lv_obj_set_style_text_font(y_label, &lv_font_montserrat_18, 0);
        
        lv_obj_t *y_minus_1000 = lv_btn_create(y_container);
        lv_obj_set_size(y_minus_1000, 80, 40);
        lv_obj_t *y_minus_1000_label = lv_label_create(y_minus_1000);
        lv_label_set_text(y_minus_1000_label, "-1000");
        lv_obj_center(y_minus_1000_label);
        lv_obj_add_event_cb(y_minus_1000, step_button_cb, LV_EVENT_CLICKED, (void*)4); // Y motor, -1000
        
        lv_obj_t *y_minus_10 = lv_btn_create(y_container);
        lv_obj_set_size(y_minus_10, 60, 40);
        lv_obj_t *y_minus_10_label = lv_label_create(y_minus_10);
        lv_label_set_text(y_minus_10_label, "-10");
        lv_obj_center(y_minus_10_label);
        lv_obj_add_event_cb(y_minus_10, step_button_cb, LV_EVENT_CLICKED, (void*)5); // Y motor, -10
        
        lv_obj_t *y_plus_10 = lv_btn_create(y_container);
        lv_obj_set_size(y_plus_10, 60, 40);
        lv_obj_t *y_plus_10_label = lv_label_create(y_plus_10);
        lv_label_set_text(y_plus_10_label, "+10");
        lv_obj_center(y_plus_10_label);
        lv_obj_add_event_cb(y_plus_10, step_button_cb, LV_EVENT_CLICKED, (void*)6); // Y motor, +10
        
        lv_obj_t *y_plus_1000 = lv_btn_create(y_container);
        lv_obj_set_size(y_plus_1000, 80, 40);
        lv_obj_t *y_plus_1000_label = lv_label_create(y_plus_1000);
        lv_label_set_text(y_plus_1000_label, "+1000");
        lv_obj_center(y_plus_1000_label);
        lv_obj_add_event_cb(y_plus_1000, step_button_cb, LV_EVENT_CLICKED, (void*)7); // Y motor, +1000
        
        // Z-axis controls
        lv_obj_t *z_container = lv_obj_create(step_container);
        lv_obj_set_size(z_container, lv_pct(90), 80);
        lv_obj_set_flex_flow(z_container, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(z_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        
        lv_obj_t *z_label = lv_label_create(z_container);
        lv_label_set_text(z_label, "Z:");
        lv_obj_set_style_text_font(z_label, &lv_font_montserrat_18, 0);
        
        lv_obj_t *z_minus_1000 = lv_btn_create(z_container);
        lv_obj_set_size(z_minus_1000, 80, 40);
        lv_obj_t *z_minus_1000_label = lv_label_create(z_minus_1000);
        lv_label_set_text(z_minus_1000_label, "-1000");
        lv_obj_center(z_minus_1000_label);
        lv_obj_add_event_cb(z_minus_1000, step_button_cb, LV_EVENT_CLICKED, (void*)8); // Z motor, -1000
        
        lv_obj_t *z_minus_10 = lv_btn_create(z_container);
        lv_obj_set_size(z_minus_10, 60, 40);
        lv_obj_t *z_minus_10_label = lv_label_create(z_minus_10);
        lv_label_set_text(z_minus_10_label, "-10");
        lv_obj_center(z_minus_10_label);
        lv_obj_add_event_cb(z_minus_10, step_button_cb, LV_EVENT_CLICKED, (void*)9); // Z motor, -10
        
        lv_obj_t *z_plus_10 = lv_btn_create(z_container);
        lv_obj_set_size(z_plus_10, 60, 40);
        lv_obj_t *z_plus_10_label = lv_label_create(z_plus_10);
        lv_label_set_text(z_plus_10_label, "+10");
        lv_obj_center(z_plus_10_label);
        lv_obj_add_event_cb(z_plus_10, step_button_cb, LV_EVENT_CLICKED, (void*)10); // Z motor, +10
        
        lv_obj_t *z_plus_1000 = lv_btn_create(z_container);
        lv_obj_set_size(z_plus_1000, 80, 40);
        lv_obj_t *z_plus_1000_label = lv_label_create(z_plus_1000);
        lv_label_set_text(z_plus_1000_label, "+1000");
        lv_obj_center(z_plus_1000_label);
        lv_obj_add_event_cb(z_plus_1000, step_button_cb, LV_EVENT_CLICKED, (void*)11); // Z motor, +1000
    }

    void step_button_cb(lv_event_t *e)
    {
        int button_id = (int)lv_event_get_user_data(e);
        
        // Map button ID to motor and step
        int motor = 0;
        int step = 0;
        
        switch(button_id) {
            case 0: motor = 1; step = -1000; break; // X motor, -1000
            case 1: motor = 1; step = -10; break;   // X motor, -10  
            case 2: motor = 1; step = 10; break;    // X motor, +10
            case 3: motor = 1; step = 1000; break;  // X motor, +1000
            case 4: motor = 2; step = -1000; break; // Y motor, -1000
            case 5: motor = 2; step = -10; break;   // Y motor, -10
            case 6: motor = 2; step = 10; break;    // Y motor, +10
            case 7: motor = 2; step = 1000; break;  // Y motor, +1000
            case 8: motor = 3; step = -1000; break; // Z motor, -1000
            case 9: motor = 3; step = -10; break;   // Z motor, -10
            case 10: motor = 3; step = 10; break;   // Z motor, +10
            case 11: motor = 3; step = 1000; break; // Z motor, +1000
        }
        
        // Send step command via serial (positions, not speeds)
        if (moveMotorStepsListner != nullptr) {
            moveMotorStepsListner(motor, step);
        }
    }

    // Position management functions
    void setGoToPositionListener(void goToPosition(float x, float y, float z))
    {
        goToPositionListener = goToPosition;
    }

    void updateCurrentPosition(float x, float y, float z)
    {
        currentX = x;
        currentY = y;
        currentZ = z;
        
        // Update current position display
        if (currentPosLabel != nullptr) {
            char posText[64];
            snprintf(posText, sizeof(posText), "Current: X:%.2f Y:%.2f Z:%.2f", x, y, z);
            lv_label_set_text(currentPosLabel, posText);
        }
    }

    void refreshPositionsList()
    {
        if (positionsList == nullptr) return;
        
        // Clear existing list items
        lv_obj_clean(positionsList);
        
        preferences.begin("positions", false);
        int count = preferences.getInt("count", 0);
        
        for (int i = 0; i < count && i < MAX_POSITIONS; i++) {
            char key[16];
            snprintf(key, sizeof(key), "pos_%d", i);
            
            size_t size = preferences.getBytesLength(key);
            if (size == sizeof(SavedPosition)) {
                SavedPosition pos;
                preferences.getBytes(key, &pos, sizeof(SavedPosition));
                
                // Create list item
                lv_obj_t *item = lv_obj_create(positionsList);
                lv_obj_set_size(item, lv_pct(95), 60);
                lv_obj_set_flex_flow(item, LV_FLEX_FLOW_ROW);
                lv_obj_set_flex_align(item, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
                lv_obj_set_style_pad_all(item, 5, 0);
                
                // Position info label
                lv_obj_t *info_label = lv_label_create(item);
                char info_text[64];
                snprintf(info_text, sizeof(info_text), "%s\nX:%.2f Y:%.2f Z:%.2f", pos.name, pos.x, pos.y, pos.z);
                lv_label_set_text(info_label, info_text);
                lv_obj_set_style_text_font(info_label, &lv_font_montserrat_12, 0);
                
                // Button container
                lv_obj_t *btn_container = lv_obj_create(item);
                lv_obj_set_size(btn_container, 120, 50);
                lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_ROW);
                lv_obj_set_flex_align(btn_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
                lv_obj_set_style_border_width(btn_container, 0, 0);
                lv_obj_set_style_bg_opa(btn_container, 0, 0);
                lv_obj_set_style_pad_all(btn_container, 2, 0);
                
                // Go To button
                lv_obj_t *goto_btn = lv_btn_create(btn_container);
                lv_obj_set_size(goto_btn, 50, 40);
                lv_obj_t *goto_label = lv_label_create(goto_btn);
                lv_label_set_text(goto_label, LV_SYMBOL_PLAY);
                lv_obj_center(goto_label);
                lv_obj_add_event_cb(goto_btn, [](lv_event_t *e) {
                    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
                        int index = (int)(intptr_t)lv_event_get_user_data(e);
                        goToPosition(index);
                    }
                }, LV_EVENT_CLICKED, (void*)(intptr_t)i);
                
                // Delete button
                lv_obj_t *delete_btn = lv_btn_create(btn_container);
                lv_obj_set_size(delete_btn, 50, 40);
                lv_obj_set_style_bg_color(delete_btn, lv_color_hex(0xFF4444), 0);
                lv_obj_t *delete_label = lv_label_create(delete_btn);
                lv_label_set_text(delete_label, LV_SYMBOL_TRASH);
                lv_obj_center(delete_label);
                lv_obj_add_event_cb(delete_btn, [](lv_event_t *e) {
                    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
                        int index = (int)(intptr_t)lv_event_get_user_data(e);
                        deletePosition(index);
                    }
                }, LV_EVENT_CLICKED, (void*)(intptr_t)i);
            }
        }
        preferences.end();
    }

    void saveCurrentPosition()
    {
        preferences.begin("positions", false);
        int count = preferences.getInt("count", 0);
        
        if (count >= MAX_POSITIONS) {
            preferences.end();
            return; // List is full
        }
        
        SavedPosition pos;
        pos.x = currentX;
        pos.y = currentY;
        pos.z = currentZ;
        snprintf(pos.name, sizeof(pos.name), "Pos %d", count + 1);
        
        char key[16];
        snprintf(key, sizeof(key), "pos_%d", count);
        preferences.putBytes(key, &pos, sizeof(SavedPosition));
        preferences.putInt("count", count + 1);
        preferences.end();
        
        refreshPositionsList();
    }

    void deletePosition(int index)
    {
        preferences.begin("positions", false);
        int count = preferences.getInt("count", 0);
        
        if (index >= 0 && index < count) {
            // Shift all positions after the deleted one
            for (int i = index; i < count - 1; i++) {
                char oldKey[16], newKey[16];
                snprintf(oldKey, sizeof(oldKey), "pos_%d", i + 1);
                snprintf(newKey, sizeof(newKey), "pos_%d", i);
                
                size_t size = preferences.getBytesLength(oldKey);
                if (size == sizeof(SavedPosition)) {
                    SavedPosition pos;
                    preferences.getBytes(oldKey, &pos, sizeof(SavedPosition));
                    preferences.putBytes(newKey, &pos, sizeof(SavedPosition));
                }
            }
            
            // Remove the last position and update count
            char lastKey[16];
            snprintf(lastKey, sizeof(lastKey), "pos_%d", count - 1);
            preferences.remove(lastKey);
            preferences.putInt("count", count - 1);
        }
        preferences.end();
        
        refreshPositionsList();
    }

    void deleteAllPositions()
    {
        preferences.begin("positions", false);
        int count = preferences.getInt("count", 0);
        
        // Remove all position entries
        for (int i = 0; i < count; i++) {
            char key[16];
            snprintf(key, sizeof(key), "pos_%d", i);
            preferences.remove(key);
        }
        preferences.putInt("count", 0);
        preferences.end();
        
        refreshPositionsList();
    }

    void goToPosition(int index)
    {
        preferences.begin("positions", false);
        char key[16];
        snprintf(key, sizeof(key), "pos_%d", index);
        
        size_t size = preferences.getBytesLength(key);
        if (size == sizeof(SavedPosition)) {
            SavedPosition pos;
            preferences.getBytes(key, &pos, sizeof(SavedPosition));
            
            if (goToPositionListener != nullptr) {
                goToPositionListener(pos.x, pos.y, pos.z);
            }
        }
        preferences.end();
    }

    void initPositionsUI(lv_obj_t *container)
    {
        positionsContainer = lv_obj_create(container);
        lv_obj_set_size(positionsContainer, lv_pct(100), lv_pct(100));
        lv_obj_set_flex_flow(positionsContainer, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(positionsContainer, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_all(positionsContainer, 10, 0);
        
        // Title
        lv_obj_t *title = lv_label_create(positionsContainer);
        lv_label_set_text(title, "Position Storage");
        lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(title, lv_pct(100));
        
        // Current position display
        currentPosLabel = lv_label_create(positionsContainer);
        lv_label_set_text(currentPosLabel, "Current: X:0.00 Y:0.00 Z:0.00");
        lv_obj_set_style_text_font(currentPosLabel, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_align(currentPosLabel, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(currentPosLabel, lv_pct(100));
        lv_obj_set_style_bg_color(currentPosLabel, lv_color_hex(0x444444), 0);
        lv_obj_set_style_bg_opa(currentPosLabel, 255, 0);
        lv_obj_set_style_text_color(currentPosLabel, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_pad_all(currentPosLabel, 5, 0);
        lv_obj_set_style_radius(currentPosLabel, 5, 0);
        
        // Control buttons container
        lv_obj_t *btn_container = lv_obj_create(positionsContainer);
        lv_obj_set_size(btn_container, lv_pct(90), 50);
        lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(btn_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_border_width(btn_container, 0, 0);
        lv_obj_set_style_bg_opa(btn_container, 0, 0);
        
        // Save Current button
        lv_obj_t *save_btn = lv_btn_create(btn_container);
        lv_obj_set_size(save_btn, 120, 40);
        lv_obj_set_style_bg_color(save_btn, lv_color_hex(0x44AA44), 0);
        lv_obj_t *save_label = lv_label_create(save_btn);
        lv_label_set_text(save_label, "Save Current");
        lv_obj_center(save_label);
        lv_obj_add_event_cb(save_btn, [](lv_event_t *e) {
            if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
                saveCurrentPosition();
            }
        }, LV_EVENT_CLICKED, nullptr);
        
        // Delete All button
        lv_obj_t *delete_all_btn = lv_btn_create(btn_container);
        lv_obj_set_size(delete_all_btn, 120, 40);
        lv_obj_set_style_bg_color(delete_all_btn, lv_color_hex(0xAA4444), 0);
        lv_obj_t *delete_all_label = lv_label_create(delete_all_btn);
        lv_label_set_text(delete_all_label, "Delete All");
        lv_obj_center(delete_all_label);
        lv_obj_add_event_cb(delete_all_btn, [](lv_event_t *e) {
            if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
                deleteAllPositions();
            }
        }, LV_EVENT_CLICKED, nullptr);
        
        // Positions list
        positionsList = lv_obj_create(positionsContainer);
        lv_obj_set_size(positionsList, lv_pct(95), lv_pct(60));
        lv_obj_set_flex_flow(positionsList, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(positionsList, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_all(positionsList, 5, 0);
        lv_obj_add_flag(positionsList, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_scroll_dir(positionsList, LV_DIR_VER);
        
        // Load and display existing positions
        refreshPositionsList();
    }
}