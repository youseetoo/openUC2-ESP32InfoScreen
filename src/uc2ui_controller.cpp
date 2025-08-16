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
#include "esp_heap_caps.h"

namespace uc2ui_controller
{
    lv_obj_t *mainScreen;
    lv_obj_t *keyboard;
    lv_obj_t *ui_MainTabView;

    lv_obj_t *wifiPage;
    lv_obj_t *microscopepage;


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
        lv_disp_t *display = lv_disp_get_default();
        lv_theme_t *theme = lv_theme_default_init(display, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                                  true, LV_FONT_DEFAULT);
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
        
        // Apply any deferred state that was set before UI was ready
        showMicroscopePage(true);
        
    }

    // Structure to hold image resources for cleanup
    typedef struct {
        lv_img_dsc_t* imgDsc;
        uint8_t* imageBuffer;
    } image_resources_t;
    
    // Event handler for close button
    static void close_btn_event_handler(lv_event_t * e)
    {
        lv_event_code_t code = lv_event_get_code(e);
        if (code == LV_EVENT_CLICKED) {
            lv_obj_t * btn = lv_event_get_target(e);
            
            // Get the image resources from user data
            image_resources_t* resources = (image_resources_t*)lv_obj_get_user_data(btn);
            if (resources) {
                // Free allocated memory
                if (resources->imageBuffer) {
                    heap_caps_free(resources->imageBuffer);
                    log_i("Freed image buffer");
                }
                if (resources->imgDsc) {
                    heap_caps_free(resources->imgDsc);
                    log_i("Freed image descriptor");
                }
                heap_caps_free(resources);
            }
            
            // Remove the tab (this will also delete all child objects)
            lv_obj_t* tab = lv_obj_get_parent(btn);
            lv_obj_del(tab);
            log_i("Closed image tab and cleaned up resources");
        }
    }

    static bool microscope_page_should_be_shown = false;

    void showMicroscopePage(bool show)
    {
        if (microscopepage == nullptr) {
            // UI not initialized yet, remember the state for later
            microscope_page_should_be_shown = show;
            return;
        }
        lvgl_helper::setVisibility(microscopepage, show);
        microscope_page_should_be_shown = false;
    }
    
    bool displayImage(const String& tabName, int width, int height, const String& format, const String& imageData)
    {
        // Check if UI is initialized
        if (ui_MainTabView == nullptr) {
            log_e("UI not initialized, cannot display image");
            return false;
        }
        
        // Validate input parameters
        if (tabName.length() == 0 || width <= 0 || height <= 0 || imageData.length() == 0) {
            log_e("Invalid image parameters");
            return false;
        }
        
        // Check image size limits (ESP32 memory constraints)
        const int max_pixels = 240 * 160;  // Reasonable limit for ESP32
        if (width * height > max_pixels) {
            log_e("Image too large: %dx%d exceeds %d pixels", width, height, max_pixels);
            return false;
        }
        
        // Only support RGB565 format for now
        if (format != "rgb565") {
            log_e("Unsupported image format: %s", format.c_str());
            return false;
        }
        
        try {
            // Log image parameters for debugging
            log_i("Displaying image: %s (%dx%d), data length: %d", 
                  tabName.c_str(), width, height, imageData.length());
            
            // Create new tab for the image
            lv_obj_t *imageTab = lv_tabview_add_tab(ui_MainTabView, tabName.c_str());
            if (imageTab == nullptr) {
                log_e("Failed to create image tab");
                return false;
            }
            
            // Set tab to be scrollable for large images
            lv_obj_clear_flag(imageTab, LV_OBJ_FLAG_SCROLLABLE);
            
            // Calculate expected decoded length more precisely
            int expectedLen = width * height * 2; // RGB565 = 2 bytes per pixel
            int decodedLen = (imageData.length() * 3) / 4 + 4; // Add some padding
            
            log_i("Expected decoded length: %d, buffer size: %d", expectedLen, decodedLen);
            
            uint8_t* imageBuffer = (uint8_t*)heap_caps_malloc(decodedLen, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
            if (imageBuffer == nullptr) {
                // Try regular heap if SPIRAM allocation fails
                imageBuffer = (uint8_t*)heap_caps_malloc(decodedLen, MALLOC_CAP_8BIT);
                if (imageBuffer == nullptr) {
                    log_e("Failed to allocate image buffer (%d bytes)", decodedLen);
                    return false;
                }
                log_i("Using regular heap for image buffer");
            } else {
                log_i("Using SPIRAM for image buffer");
            }
            
            // Decode base64 image data
            int actualLen = decodeBase64(imageData, imageBuffer, decodedLen);
            log_i("Decoded %d bytes from base64 data", actualLen);
            
            if (actualLen <= 0 || actualLen < expectedLen) {
                heap_caps_free(imageBuffer);
                log_e("Failed to decode base64 image data (got %d, expected %d)", actualLen, expectedLen);
                return false;
            }
            
            // Create LVGL image descriptor
            lv_img_dsc_t* imgDsc = (lv_img_dsc_t*)heap_caps_malloc(sizeof(lv_img_dsc_t), MALLOC_CAP_8BIT);
            if (imgDsc == nullptr) {
                heap_caps_free(imageBuffer);
                log_e("Failed to allocate image descriptor");
                return false;
            }
            
            // Set up image descriptor for RGB565
            imgDsc->header.always_zero = 0;
            imgDsc->header.w = width;
            imgDsc->header.h = height;
            imgDsc->header.cf = LV_IMG_CF_TRUE_COLOR;  // RGB565
            imgDsc->data_size = width * height * 2;     // 2 bytes per pixel
            imgDsc->data = imageBuffer;
            
            // Create image object
            lv_obj_t* imgObj = lv_img_create(imageTab);
            if (imgObj == nullptr) {
                heap_caps_free(imageBuffer);
                heap_caps_free(imgDsc);
                log_e("Failed to create image object");
                return false;
            }
            
            // Set image source and position
            lv_img_set_src(imgObj, imgDsc);
            lv_obj_center(imgObj);
            
            // Add close button for the tab with proper cleanup
            lv_obj_t* closeBtn = lv_btn_create(imageTab);
            lv_obj_set_size(closeBtn, 60, 30);
            lv_obj_align(closeBtn, LV_ALIGN_TOP_RIGHT, -10, 10);
            
            lv_obj_t* closeLbl = lv_label_create(closeBtn);
            lv_label_set_text(closeLbl, "X");
            lv_obj_center(closeLbl);
            
            // Store image resources for cleanup when tab is closed
            image_resources_t* resources = (image_resources_t*)heap_caps_malloc(sizeof(image_resources_t), MALLOC_CAP_8BIT);
            if (resources) {
                resources->imgDsc = imgDsc;
                resources->imageBuffer = imageBuffer;
                lv_obj_set_user_data(closeBtn, resources);
                lv_obj_add_event_cb(closeBtn, close_btn_event_handler, LV_EVENT_CLICKED, NULL);
            }
            
            log_i("Successfully displayed image: %s (%dx%d), decoded %d bytes", 
                  tabName.c_str(), width, height, actualLen);
            return true;
            
        } catch (...) {
            log_e("Exception while creating image display");
            return false;
        }
    }
    
    // Improved base64 decoder implementation
    int decodeBase64(const String& input, uint8_t* output, int maxLen) 
    {
        const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        int len = input.length();
        int outLen = 0;
        
        // Remove any padding characters for length calculation
        while (len > 0 && input[len - 1] == '=') {
            len--;
        }
        
        for (int i = 0; i < len && outLen < maxLen - 3; i += 4) {
            uint8_t a = 0, b = 0, c = 0, d = 0;
            
            // Find character positions in base64 alphabet
            if (i < len) {
                for (int j = 0; j < 64; j++) {
                    if (input[i] == chars[j]) {
                        a = j;
                        break;
                    }
                }
            }
            
            if (i + 1 < input.length()) {
                for (int j = 0; j < 64; j++) {
                    if (input[i + 1] == chars[j]) {
                        b = j;
                        break;
                    }
                }
            }
            
            if (i + 2 < input.length() && input[i + 2] != '=') {
                for (int j = 0; j < 64; j++) {
                    if (input[i + 2] == chars[j]) {
                        c = j;
                        break;
                    }
                }
            }
            
            if (i + 3 < input.length() && input[i + 3] != '=') {
                for (int j = 0; j < 64; j++) {
                    if (input[i + 3] == chars[j]) {
                        d = j;
                        break;
                    }
                }
            }
            
            // Decode 4 characters to up to 3 bytes
            uint32_t combined = (a << 18) | (b << 12) | (c << 6) | d;
            
            // Always output first byte
            output[outLen++] = (combined >> 16) & 0xFF;
            
            // Output second byte if not padding
            if (i + 2 < input.length() && input[i + 2] != '=') {
                if (outLen < maxLen) output[outLen++] = (combined >> 8) & 0xFF;
            }
            
            // Output third byte if not padding
            if (i + 3 < input.length() && input[i + 3] != '=') {
                if (outLen < maxLen) output[outLen++] = combined & 0xFF;
            }
        }
        
        return outLen;
    }

}