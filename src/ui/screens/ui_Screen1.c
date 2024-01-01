// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.3.6
// Project name: SquareLine_Project

#include "../ui.h"

void ui_Screen1_screen_init(void)
{
ui_Screen1 = lv_obj_create(NULL);
lv_obj_clear_flag( ui_Screen1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_TabView1 = lv_tabview_create(ui_Screen1, LV_DIR_TOP, 50);
lv_obj_set_width( ui_TabView1, 785);
lv_obj_set_height( ui_TabView1, 395);
lv_obj_set_x( ui_TabView1, 0 );
lv_obj_set_y( ui_TabView1, -36 );
lv_obj_set_align( ui_TabView1, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_TabView1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags


ui_TabPage1 = lv_tabview_add_tab(ui_TabView1, "Title 1");

ui_Button1 = lv_btn_create(ui_TabPage1);
lv_obj_set_width( ui_Button1, 140);
lv_obj_set_height( ui_Button1, 50);
lv_obj_set_x( ui_Button1, 314 );
lv_obj_set_y( ui_Button1, -108 );
lv_obj_set_align( ui_Button1, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_Button1, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_Button1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_a = lv_btn_create(ui_TabPage1);
lv_obj_set_width( ui_a, 135);
lv_obj_set_height( ui_a, 50);
lv_obj_set_x( ui_a, -252 );
lv_obj_set_y( ui_a, -105 );
lv_obj_set_align( ui_a, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_a, LV_OBJ_FLAG_FLOATING | LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_a, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_outline_color(ui_a, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_outline_opa(ui_a, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_outline_width(ui_a, 2, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_outline_pad(ui_a, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_color(ui_a, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_a, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_letter_space(ui_a, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_line_space(ui_a, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_a, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_TabPage2 = lv_tabview_add_tab(ui_TabView1, "Title 2");

ui_Button2 = lv_btn_create(ui_TabPage2);
lv_obj_set_width( ui_Button2, 140);
lv_obj_set_height( ui_Button2, 50);
lv_obj_set_x( ui_Button2, 314 );
lv_obj_set_y( ui_Button2, -108 );
lv_obj_set_align( ui_Button2, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_Button2, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_Button2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_TabPage3 = lv_tabview_add_tab(ui_TabView1, "Title 3");

ui_a1 = lv_btn_create(ui_TabPage3);
lv_obj_set_width( ui_a1, 135);
lv_obj_set_height( ui_a1, 50);
lv_obj_set_x( ui_a1, -252 );
lv_obj_set_y( ui_a1, -105 );
lv_obj_set_align( ui_a1, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_a1, LV_OBJ_FLAG_FLOATING | LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_a1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_outline_color(ui_a1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_outline_opa(ui_a1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_outline_width(ui_a1, 2, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_outline_pad(ui_a1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_color(ui_a1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_a1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_letter_space(ui_a1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_line_space(ui_a1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_a1, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN| LV_STATE_DEFAULT);

}
