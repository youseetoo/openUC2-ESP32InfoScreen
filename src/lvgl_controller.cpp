
#include "lvgl_controller.h"
#include <esp_log.h>
#include <Arduino.h>
#include "uc2ui_controller.h"

namespace lvgl_controller
{
    static ESP_Panel *panel = NULL;
    /* Initialize LVGL buffers */
    static lv_disp_draw_buf_t draw_buf;
    /* Initialize the display device */
    static lv_disp_drv_t disp_drv;

    static lv_indev_drv_t indev_drv;
    static SemaphoreHandle_t lvgl_mux = NULL; // LVGL mutex
#if ESP_PANEL_USE_LCD_TOUCH
    /* Read the touchpad */
    void lvgl_port_tp_read(lv_indev_drv_t *indev, lv_indev_data_t *data)
    {
        panel->getLcdTouch()->readData();

        bool touched = panel->getLcdTouch()->getTouchState();
        if (!touched)
        {
            data->state = LV_INDEV_STATE_REL;
        }
        else
        {
            TouchPoint point = panel->getLcdTouch()->getPoint();

            data->state = LV_INDEV_STATE_PR;
            /*Set the coordinates*/
            data->point.x = point.x;
            data->point.y = point.y;
            //log_i("Touch point: x %d, y %d\n", point.x, point.y);
        }
    }
#endif

    void lvgl_port_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
    {
        panel->getLcd()->drawBitmap(area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_p);
        lv_disp_flush_ready(disp);
    }

    void lvgl_port_lock(int timeout_ms)
    {
        const TickType_t timeout_ticks = (timeout_ms < 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
        xSemaphoreTakeRecursive(lvgl_mux, timeout_ticks);
    }

    void lvgl_port_unlock(void)
    {
        xSemaphoreGiveRecursive(lvgl_mux);
    }

    void lvgl_port_task(void *arg)
    {
        Serial.println("Starting LVGL task");

        uint32_t task_delay_ms = LVGL_TASK_MAX_DELAY_MS;
        while (1)
        {
            // Lock the mutex due to the LVGL APIs are not thread-safe
            lvgl_port_lock(-1);
            task_delay_ms = lv_timer_handler();
            // Release the mutex
            lvgl_port_unlock();
            if (task_delay_ms > LVGL_TASK_MAX_DELAY_MS)
            {
                task_delay_ms = LVGL_TASK_MAX_DELAY_MS;
            }
            else if (task_delay_ms < LVGL_TASK_MIN_DELAY_MS)
            {
                task_delay_ms = LVGL_TASK_MIN_DELAY_MS;
            }
            vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
        }
    }

    void initlgvl()
    {
        Serial.println("Starting LVGL initialization...");
        log_i("initlgvl");
        /* Initialize LVGL core */
        panel = new ESP_Panel();
        Serial.println("ESP_Panel created");
        lv_init();
        Serial.println("LVGL core initialized");

        /* Using double buffers is more faster than single buffer */
        /* Using internal SRAM is more fast than PSRAM (Note: Memory allocated using `malloc` may be located in PSRAM.) */
        Serial.println("Creating display buffer and driver...");
        log_i("create buffer and driver");
        uint8_t *buf = (uint8_t *)heap_caps_calloc(1, LVGL_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_INTERNAL);
        //uint8_t *buf2 = (uint8_t *)heap_caps_calloc(1, LVGL_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_INTERNAL);
        assert(buf);
        //assert(buf2);
        Serial.println("Buffer allocated successfully");
        lv_disp_draw_buf_init(&draw_buf, buf, NULL, LVGL_BUF_SIZE);
        Serial.println("Display buffer initialized");

        lv_disp_drv_init(&disp_drv);
        /* Change the following line to your display resolution */
        disp_drv.hor_res = ESP_PANEL_LCD_H_RES;
        disp_drv.ver_res = ESP_PANEL_LCD_V_RES;
        disp_drv.flush_cb = lvgl_port_disp_flush;
        disp_drv.draw_buf = &draw_buf;
        Serial.println("Display driver configured");
        lv_disp_drv_register(&disp_drv);
        Serial.println("Display driver registered");

#if ESP_PANEL_USE_LCD_TOUCH
        /* Initialize the input device */
        log_i("init touch");
        
        lv_indev_drv_init(&indev_drv);
        indev_drv.type = LV_INDEV_TYPE_POINTER;
        indev_drv.read_cb = lvgl_port_tp_read;
        lv_indev_drv_register(&indev_drv);
#endif
        log_i("create esp panel");
        /* Initialize bus and device of panel */
        Serial.println("Initializing ESP Panel...");
        panel->init();
        Serial.println("ESP Panel initialized");

        /* Start panel */
        Serial.println("Starting ESP Panel...");
        panel->begin();
        Serial.println("ESP Panel started");

        /* Create a task to run the LVGL task periodically */
        Serial.println("Creating LVGL task...");
        lvgl_mux = xSemaphoreCreateRecursiveMutex();
        xTaskCreate(lvgl_port_task, "lvgl", LVGL_TASK_STACK_SIZE, NULL, LVGL_TASK_PRIORITY, NULL);
        Serial.println("LVGL task created");

        /* Lock the mutex due to the LVGL APIs are not thread-safe */
        lvgl_port_lock(-1);

        Serial.println("Initializing UC2 UI...");
        log_i("init uc2 ui");
        uc2ui_controller::initUi();
        // ui_init();
        Serial.println("UC2 UI initialized");

        /* Release the mutex */
        lvgl_port_unlock();
        Serial.println("LVGL initialization completed!");
        log_i("initlgvl done");
    }
}
