#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;

static const char *screen_names[] = { "Main", "Setting", "Show Data", "About", "Sensor List", "startup", "file_saver", "FIle Manager" };
static const char *object_names[] = { "main", "setting", "show_data", "about", "sensor_list", "startup", "file_saver", "file_manager", "panel_header", "title", "obj0", "panel_live_data", "show_data_label_3", "panel_manger", "file_manager_label", "panel_about", "about_label_3", "panel_setting", "setting_label_3", "panel_header_1", "title_1", "back_button_1", "back_1", "obj1", "tgs_825", "tgs_2602", "mq_3", "mq_138", "mq_137", "wsp_2110", "tgs825_label", "tgs2602_label", "mq3_label", "mq138_label", "mq137_label", "wsp2110_label", "recording_button", "record", "panel_header_2", "title_2", "back_button", "back", "graph", "calibration_bar", "calibration_data", "obj2", "temperature", "humidity", "obj3", "panel_header_3", "title_3", "back_button_2", "back_2", "obj4", "panel_header_5", "title_5", "button_panel", "tgs825_button", "tgs2602_button", "wsp2110_button", "mq138_button", "tgs1820_button", "mq3_button", "mq137_button", "tgs825", "tgs2602", "tgs1820", "mq137", "mq3", "mq138", "wsp2110", "back_button_5", "back_5", "obj5", "boot_logo", "panel_header_6", "title_6", "back_button_3", "back_3", "panel_header_7", "title_7", "back_button_4", "back_4" };

//
// Event handlers
//

lv_obj_t *tick_value_change_obj;

static void event_handler_cb_main_panel_live_data(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 4, 0, e);
    }
}

static void event_handler_cb_main_panel_manger(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 7, 0, e);
    }
}

static void event_handler_cb_main_panel_about(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 10, 0, e);
    }
}

static void event_handler_cb_main_panel_setting(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 13, 0, e);
    }
}

static void event_handler_cb_setting_back_button_1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 3, 0, e);
    }
}

static void event_handler_cb_show_data_back_button(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 3, 0, e);
    }
}

static void event_handler_cb_about_back_button_2(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 3, 0, e);
    }
}

static void event_handler_cb_sensor_list_tgs825_button(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 4, 0, e);
    }
}

static void event_handler_cb_sensor_list_tgs2602_button(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 5, 0, e);
    }
}

static void event_handler_cb_sensor_list_wsp2110_button(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 6, 0, e);
    }
}

static void event_handler_cb_sensor_list_mq138_button(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 7, 0, e);
    }
}

static void event_handler_cb_sensor_list_tgs1820_button(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 8, 0, e);
    }
}

static void event_handler_cb_sensor_list_mq3_button(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 9, 0, e);
    }
}

static void event_handler_cb_sensor_list_mq137_button(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 10, 0, e);
    }
}

static void event_handler_cb_sensor_list_back_button_5(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 18, 0, e);
    }
}

static void event_handler_cb_file_saver_back_button_3(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 3, 0, e);
    }
}

static void event_handler_cb_file_manager_back_button_4(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 3, 0, e);
    }
}

//
// Screens
//

void create_screen_main() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    {
        lv_obj_t *parent_obj = obj;
        {
            // panel_header
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_header = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 480, 48);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8bd4d4), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // Title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.title = obj;
                    lv_obj_set_pos(obj, 160, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff020202), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "RT-DiagNose");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 30, 54);
            lv_obj_set_size(obj, 419, 258);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffe6e8ea), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // panel_live_data
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.panel_live_data = obj;
                    lv_obj_set_pos(obj, 236, -4);
                    lv_obj_set_size(obj, 111, 106);
                    lv_obj_add_event_cb(obj, event_handler_cb_main_panel_live_data, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // Show_Data_Label_3
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.show_data_label_3 = obj;
                            lv_obj_set_pos(obj, -10, 63);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Live Data");
                        }
                        {
                            lv_obj_t *obj = lv_image_create(parent_obj);
                            lv_obj_set_pos(obj, 7, -3);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_image_set_src(obj, &img_show_data);
                        }
                    }
                }
                {
                    // panel_manger
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.panel_manger = obj;
                    lv_obj_set_pos(obj, 36, 116);
                    lv_obj_set_size(obj, 111, 106);
                    lv_obj_add_event_cb(obj, event_handler_cb_main_panel_manger, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // file_manager_label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.file_manager_label = obj;
                            lv_obj_set_pos(obj, 13, 55);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Files");
                        }
                        {
                            lv_obj_t *obj = lv_image_create(parent_obj);
                            lv_obj_set_pos(obj, 6, -9);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_image_set_src(obj, &img_file_manager);
                        }
                    }
                }
                {
                    // panel_about
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.panel_about = obj;
                    lv_obj_set_pos(obj, 237, 114);
                    lv_obj_set_size(obj, 111, 106);
                    lv_obj_add_event_cb(obj, event_handler_cb_main_panel_about, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // About_Label_3
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.about_label_3 = obj;
                            lv_obj_set_pos(obj, 6, 57);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "About");
                        }
                        {
                            lv_obj_t *obj = lv_image_create(parent_obj);
                            lv_obj_set_pos(obj, 6, -7);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_image_set_src(obj, &img_about);
                        }
                    }
                }
                {
                    // panel_setting
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.panel_setting = obj;
                    lv_obj_set_pos(obj, 36, -4);
                    lv_obj_set_size(obj, 111, 106);
                    lv_obj_add_event_cb(obj, event_handler_cb_main_panel_setting, LV_EVENT_ALL, flowState);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // Setting_Label_3
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.setting_label_3 = obj;
                            lv_obj_set_pos(obj, -4, 63);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Settings");
                        }
                        {
                            lv_obj_t *obj = lv_image_create(parent_obj);
                            lv_obj_set_pos(obj, 6, -3);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_image_set_src(obj, &img_setting_button);
                        }
                    }
                }
            }
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
}

void create_screen_setting() {
    void *flowState = getFlowState(0, 1);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.setting = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff5f5f5), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // panel_header_1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_header_1 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 480, 48);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8bd4d4), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // Title_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.title_1 = obj;
                    lv_obj_set_pos(obj, 158, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff020202), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "RT-DiagNose");
                }
            }
        }
        {
            // Back_Button_1
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.back_button_1 = obj;
            lv_obj_set_pos(obj, 373, 57);
            lv_obj_set_size(obj, 76, 31);
            lv_obj_add_event_cb(obj, event_handler_cb_setting_back_button_1, LV_EVENT_ALL, flowState);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_opa(obj, 127, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // Back_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.back_1 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Back");
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 203, 57);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Setting");
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj1 = obj;
            lv_obj_set_pos(obj, 28, 101);
            lv_obj_set_size(obj, 425, 302);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffe5ebe5), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 158, -7);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "TGS 825");
                }
                {
                    // TGS_825
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.tgs_825 = obj;
                    lv_obj_set_pos(obj, -3, 16);
                    lv_obj_set_size(obj, 333, 9);
                    lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                }
                {
                    // TGS_2602
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.tgs_2602 = obj;
                    lv_obj_set_pos(obj, -3, 62);
                    lv_obj_set_size(obj, 333, 9);
                    lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 154, 38);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "TGS 2602");
                }
                {
                    // MQ_3
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.mq_3 = obj;
                    lv_obj_set_pos(obj, -3, 102);
                    lv_obj_set_size(obj, 333, 9);
                    lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 165, 80);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "MQ 3");
                }
                {
                    // MQ_138
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.mq_138 = obj;
                    lv_obj_set_pos(obj, -3, 144);
                    lv_obj_set_size(obj, 333, 9);
                    lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 165, 122);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "MQ 138");
                }
                {
                    // MQ_137
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.mq_137 = obj;
                    lv_obj_set_pos(obj, -2, 186);
                    lv_obj_set_size(obj, 332, 9);
                    lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 165, 165);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "MQ 137");
                }
                {
                    // WSP_2110
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.wsp_2110 = obj;
                    lv_obj_set_pos(obj, -2, 231);
                    lv_obj_set_size(obj, 332, 9);
                    lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 158, 208);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "WSP 2110");
                }
                {
                    // TGS825_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.tgs825_label = obj;
                    lv_obj_set_pos(obj, 338, 15);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "25");
                }
                {
                    // TGS2602_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.tgs2602_label = obj;
                    lv_obj_set_pos(obj, 338, 61);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "25");
                }
                {
                    // MQ3_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.mq3_label = obj;
                    lv_obj_set_pos(obj, 338, 101);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "25");
                }
                {
                    // MQ138_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.mq138_label = obj;
                    lv_obj_set_pos(obj, 338, 143);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "25");
                }
                {
                    // MQ137_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.mq137_label = obj;
                    lv_obj_set_pos(obj, 338, 178);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "25");
                }
                {
                    // WSP2110_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.wsp2110_label = obj;
                    lv_obj_set_pos(obj, 338, 223);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "25");
                }
            }
        }
        {
            // Recording_Button
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.recording_button = obj;
            lv_obj_set_pos(obj, 28, 57);
            lv_obj_set_size(obj, 90, 31);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_opa(obj, 127, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // record
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.record = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Record");
                }
            }
        }
    }
    
    tick_screen_setting();
}

void tick_screen_setting() {
    void *flowState = getFlowState(0, 1);
    (void)flowState;
}

void create_screen_show_data() {
    void *flowState = getFlowState(0, 2);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.show_data = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    {
        lv_obj_t *parent_obj = obj;
        {
            // panel_header_2
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_header_2 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 480, 48);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8bd4d4), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // Title_2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.title_2 = obj;
                    lv_obj_set_pos(obj, 158, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff020202), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "RT-DiagNose");
                }
            }
        }
        {
            // Back_Button
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.back_button = obj;
            lv_obj_set_pos(obj, 396, 54);
            lv_obj_set_size(obj, 75, 28);
            lv_obj_add_event_cb(obj, event_handler_cb_show_data_back_button, LV_EVENT_ALL, flowState);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_opa(obj, 127, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // Back
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.back = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Back");
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj3 = obj;
            lv_obj_set_pos(obj, 137, 56);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "");
        }
        {
            // graph
            lv_obj_t *obj = lv_chart_create(parent_obj);
            objects.graph = obj;
            lv_obj_set_pos(obj, 22, 82);
            lv_obj_set_size(obj, 337, 191);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_ACTIVE);
        }
        {
            // Calibration_Bar
            lv_obj_t *obj = lv_slider_create(parent_obj);
            objects.calibration_bar = obj;
            lv_obj_set_pos(obj, 22, 294);
            lv_obj_set_size(obj, 291, 9);
            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
        }
        {
            // calibration_Data
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.calibration_data = obj;
            lv_obj_set_pos(obj, 328, 289);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "25");
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj2 = obj;
            lv_obj_set_pos(obj, 366, 90);
            lv_obj_set_size(obj, 105, 191);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff0f2f3), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 371, 114);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Temperature");
        }
        {
            // temperature
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.temperature = obj;
            lv_obj_set_pos(obj, 391, 148);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "100 C");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 385, 196);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Humidity");
        }
        {
            // humidity
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.humidity = obj;
            lv_obj_set_pos(obj, 376, 232);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "100 RH");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 160, 273);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Time(s)");
        }
    }
    
    tick_screen_show_data();
}

void tick_screen_show_data() {
    void *flowState = getFlowState(0, 2);
    (void)flowState;
    {
        const char *new_val = evalTextProperty(flowState, 5, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj3);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj3;
            lv_label_set_text(objects.obj3, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_about() {
    void *flowState = getFlowState(0, 3);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.about = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    {
        lv_obj_t *parent_obj = obj;
        {
            // panel_header_3
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_header_3 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 480, 48);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8bd4d4), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // Title_3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.title_3 = obj;
                    lv_obj_set_pos(obj, 158, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff020202), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "RT-DiagNose");
                }
            }
        }
        {
            // Back_Button_2
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.back_button_2 = obj;
            lv_obj_set_pos(obj, 384, 62);
            lv_obj_set_size(obj, 81, 31);
            lv_obj_add_event_cb(obj, event_handler_cb_about_back_button_2, LV_EVENT_ALL, flowState);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_opa(obj, 127, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // Back_2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.back_2 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Back");
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 208, 55);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "About");
        }
        {
            lv_obj_t *obj = lv_qrcode_create(parent_obj);
            objects.obj4 = obj;
            lv_obj_set_pos(obj, 140, 93);
            lv_obj_set_size(obj, 200, 190);
            lv_qrcode_set_size(obj, 190);
            lv_qrcode_set_dark_color(obj, lv_color_hex(0xff000000));
            lv_qrcode_set_light_color(obj, lv_color_hex(0xffffffff));
            lv_qrcode_update(obj, "https://drive.google.com/file/d/1RPgtJLkGu8JaOYKYgd5qDAHwTfCgFFrN/view?usp=sharing", 82);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        }
    }
    
    tick_screen_about();
}

void tick_screen_about() {
    void *flowState = getFlowState(0, 3);
    (void)flowState;
}

void create_screen_sensor_list() {
    void *flowState = getFlowState(0, 4);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.sensor_list = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    {
        lv_obj_t *parent_obj = obj;
        {
            // panel_header_5
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_header_5 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 480, 48);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8bd4d4), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // Title_5
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.title_5 = obj;
                    lv_obj_set_pos(obj, 158, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff020202), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "RT-DiagNose");
                }
            }
        }
        {
            // button_panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.button_panel = obj;
            lv_obj_set_pos(obj, 32, 95);
            lv_obj_set_size(obj, 415, 204);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffe8f1f2), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TGS825_button
                    lv_obj_t *obj = lv_imagebutton_create(parent_obj);
                    objects.tgs825_button = obj;
                    lv_obj_set_pos(obj, -5, -6);
                    lv_obj_set_size(obj, 63, 65);
                    lv_imagebutton_set_src(obj, LV_IMAGEBUTTON_STATE_RELEASED, NULL, &img_gas_sensor, NULL);
                    lv_obj_add_event_cb(obj, event_handler_cb_sensor_list_tgs825_button, LV_EVENT_ALL, flowState);
                }
                {
                    // TGS2602_button
                    lv_obj_t *obj = lv_imagebutton_create(parent_obj);
                    objects.tgs2602_button = obj;
                    lv_obj_set_pos(obj, 198, -6);
                    lv_obj_set_size(obj, 65, 65);
                    lv_imagebutton_set_src(obj, LV_IMAGEBUTTON_STATE_RELEASED, NULL, &img_gas_sensor, NULL);
                    lv_obj_add_event_cb(obj, event_handler_cb_sensor_list_tgs2602_button, LV_EVENT_ALL, flowState);
                }
                {
                    // WSP2110_button
                    lv_obj_t *obj = lv_imagebutton_create(parent_obj);
                    objects.wsp2110_button = obj;
                    lv_obj_set_pos(obj, 197, 95);
                    lv_obj_set_size(obj, 65, 65);
                    lv_imagebutton_set_src(obj, LV_IMAGEBUTTON_STATE_RELEASED, NULL, &img_gas_sensor, NULL);
                    lv_obj_add_event_cb(obj, event_handler_cb_sensor_list_wsp2110_button, LV_EVENT_ALL, flowState);
                }
                {
                    // MQ138_button
                    lv_obj_t *obj = lv_imagebutton_create(parent_obj);
                    objects.mq138_button = obj;
                    lv_obj_set_pos(obj, -5, 99);
                    lv_obj_set_size(obj, 65, 65);
                    lv_imagebutton_set_src(obj, LV_IMAGEBUTTON_STATE_RELEASED, NULL, &img_gas_sensor, NULL);
                    lv_obj_add_event_cb(obj, event_handler_cb_sensor_list_mq138_button, LV_EVENT_ALL, flowState);
                }
                {
                    // TGS1820_button
                    lv_obj_t *obj = lv_imagebutton_create(parent_obj);
                    objects.tgs1820_button = obj;
                    lv_obj_set_pos(obj, 315, -6);
                    lv_obj_set_size(obj, 65, 65);
                    lv_imagebutton_set_src(obj, LV_IMAGEBUTTON_STATE_RELEASED, NULL, &img_gas_sensor, NULL);
                    lv_obj_add_event_cb(obj, event_handler_cb_sensor_list_tgs1820_button, LV_EVENT_ALL, flowState);
                }
                {
                    // MQ3_button
                    lv_obj_t *obj = lv_imagebutton_create(parent_obj);
                    objects.mq3_button = obj;
                    lv_obj_set_pos(obj, 315, 99);
                    lv_obj_set_size(obj, 65, 65);
                    lv_imagebutton_set_src(obj, LV_IMAGEBUTTON_STATE_RELEASED, NULL, &img_gas_sensor, NULL);
                    lv_obj_add_event_cb(obj, event_handler_cb_sensor_list_mq3_button, LV_EVENT_ALL, flowState);
                }
                {
                    // MQ137_button
                    lv_obj_t *obj = lv_imagebutton_create(parent_obj);
                    objects.mq137_button = obj;
                    lv_obj_set_pos(obj, 92, 54);
                    lv_obj_set_size(obj, 65, 65);
                    lv_imagebutton_set_src(obj, LV_IMAGEBUTTON_STATE_RELEASED, NULL, &img_gas_sensor, NULL);
                    lv_obj_add_event_cb(obj, event_handler_cb_sensor_list_mq137_button, LV_EVENT_ALL, flowState);
                }
                {
                    // TGS825
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.tgs825 = obj;
                    lv_obj_set_pos(obj, 1, 51);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "TGS825");
                }
                {
                    // TGS2602
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.tgs2602 = obj;
                    lv_obj_set_pos(obj, 200, 51);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "TGS2602");
                }
                {
                    // TGS1820
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.tgs1820 = obj;
                    lv_obj_set_pos(obj, 318, 59);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "TGS1820");
                }
                {
                    // MQ137
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.mq137 = obj;
                    lv_obj_set_pos(obj, 102, 111);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "MQ137");
                }
                {
                    // MQ3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.mq3 = obj;
                    lv_obj_set_pos(obj, 331, 156);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "MQ3");
                }
                {
                    // MQ138
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.mq138 = obj;
                    lv_obj_set_pos(obj, 4, 156);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "MQ138");
                }
                {
                    // WSP2110
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.wsp2110 = obj;
                    lv_obj_set_pos(obj, 199, 152);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "WSP2110");
                }
            }
        }
        {
            // Back_Button_5
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.back_button_5 = obj;
            lv_obj_set_pos(obj, 377, 53);
            lv_obj_set_size(obj, 76, 31);
            lv_obj_add_event_cb(obj, event_handler_cb_sensor_list_back_button_5, LV_EVENT_ALL, flowState);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_opa(obj, 127, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // Back_5
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.back_5 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Back");
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 155, 60);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Sensors Network");
        }
    }
    
    tick_screen_sensor_list();
}

void tick_screen_sensor_list() {
    void *flowState = getFlowState(0, 4);
    (void)flowState;
}

void create_screen_startup() {
    void *flowState = getFlowState(0, 5);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.startup = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj5 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 480, 320);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // boot logo
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.boot_logo = obj;
            lv_obj_set_pos(obj, 190, 114);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_boot_logo);
        }
    }
    
    tick_screen_startup();
}

void tick_screen_startup() {
    void *flowState = getFlowState(0, 5);
    (void)flowState;
}

void create_screen_file_saver() {
    void *flowState = getFlowState(0, 6);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.file_saver = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    {
        lv_obj_t *parent_obj = obj;
        {
            // panel_header_6
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_header_6 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 480, 48);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8bd4d4), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // Title_6
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.title_6 = obj;
                    lv_obj_set_pos(obj, 158, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff020202), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "RT-DiagNose");
                }
            }
        }
        {
            // Back_Button_3
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.back_button_3 = obj;
            lv_obj_set_pos(obj, 384, 62);
            lv_obj_set_size(obj, 81, 31);
            lv_obj_add_event_cb(obj, event_handler_cb_file_saver_back_button_3, LV_EVENT_ALL, flowState);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_opa(obj, 127, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // Back_3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.back_3 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Back");
                }
            }
        }
        {
            lv_obj_t *obj = lv_keyboard_create(parent_obj);
            lv_obj_set_pos(obj, 0, 113);
            lv_obj_set_size(obj, 480, 207);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            lv_obj_set_pos(obj, 129, 70);
            lv_obj_set_size(obj, 201, 38);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_one_line(obj, false);
            lv_textarea_set_password_mode(obj, false);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 17, 78);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "File Name");
        }
    }
    
    tick_screen_file_saver();
}

void tick_screen_file_saver() {
    void *flowState = getFlowState(0, 6);
    (void)flowState;
}

void create_screen_file_manager() {
    void *flowState = getFlowState(0, 7);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.file_manager = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    {
        lv_obj_t *parent_obj = obj;
        {
            // panel_header_7
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_header_7 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 480, 48);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8bd4d4), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // Title_7
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.title_7 = obj;
                    lv_obj_set_pos(obj, 158, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff020202), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "RT-DiagNose");
                }
            }
        }
        {
            // Back_Button_4
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.back_button_4 = obj;
            lv_obj_set_pos(obj, 384, 62);
            lv_obj_set_size(obj, 81, 31);
            lv_obj_add_event_cb(obj, event_handler_cb_file_manager_back_button_4, LV_EVENT_ALL, flowState);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_opa(obj, 127, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // Back_4
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.back_4 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Back");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 23, 106);
            lv_obj_set_size(obj, 434, 200);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 174, 69);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "File Manager");
        }
    }
    
    tick_screen_file_manager();
}

void tick_screen_file_manager() {
    void *flowState = getFlowState(0, 7);
    (void)flowState;
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_setting,
    tick_screen_show_data,
    tick_screen_about,
    tick_screen_sensor_list,
    tick_screen_startup,
    tick_screen_file_saver,
    tick_screen_file_manager,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

//
// Fonts
//

ext_font_desc_t fonts[] = {
#if LV_FONT_MONTSERRAT_8
    { "MONTSERRAT_8", &lv_font_montserrat_8 },
#endif
#if LV_FONT_MONTSERRAT_10
    { "MONTSERRAT_10", &lv_font_montserrat_10 },
#endif
#if LV_FONT_MONTSERRAT_12
    { "MONTSERRAT_12", &lv_font_montserrat_12 },
#endif
#if LV_FONT_MONTSERRAT_14
    { "MONTSERRAT_14", &lv_font_montserrat_14 },
#endif
#if LV_FONT_MONTSERRAT_16
    { "MONTSERRAT_16", &lv_font_montserrat_16 },
#endif
#if LV_FONT_MONTSERRAT_18
    { "MONTSERRAT_18", &lv_font_montserrat_18 },
#endif
#if LV_FONT_MONTSERRAT_20
    { "MONTSERRAT_20", &lv_font_montserrat_20 },
#endif
#if LV_FONT_MONTSERRAT_22
    { "MONTSERRAT_22", &lv_font_montserrat_22 },
#endif
#if LV_FONT_MONTSERRAT_24
    { "MONTSERRAT_24", &lv_font_montserrat_24 },
#endif
#if LV_FONT_MONTSERRAT_26
    { "MONTSERRAT_26", &lv_font_montserrat_26 },
#endif
#if LV_FONT_MONTSERRAT_28
    { "MONTSERRAT_28", &lv_font_montserrat_28 },
#endif
#if LV_FONT_MONTSERRAT_30
    { "MONTSERRAT_30", &lv_font_montserrat_30 },
#endif
#if LV_FONT_MONTSERRAT_32
    { "MONTSERRAT_32", &lv_font_montserrat_32 },
#endif
#if LV_FONT_MONTSERRAT_34
    { "MONTSERRAT_34", &lv_font_montserrat_34 },
#endif
#if LV_FONT_MONTSERRAT_36
    { "MONTSERRAT_36", &lv_font_montserrat_36 },
#endif
#if LV_FONT_MONTSERRAT_38
    { "MONTSERRAT_38", &lv_font_montserrat_38 },
#endif
#if LV_FONT_MONTSERRAT_40
    { "MONTSERRAT_40", &lv_font_montserrat_40 },
#endif
#if LV_FONT_MONTSERRAT_42
    { "MONTSERRAT_42", &lv_font_montserrat_42 },
#endif
#if LV_FONT_MONTSERRAT_44
    { "MONTSERRAT_44", &lv_font_montserrat_44 },
#endif
#if LV_FONT_MONTSERRAT_46
    { "MONTSERRAT_46", &lv_font_montserrat_46 },
#endif
#if LV_FONT_MONTSERRAT_48
    { "MONTSERRAT_48", &lv_font_montserrat_48 },
#endif
};

//
//
//

void create_screens() {
    
    eez_flow_init_fonts(fonts, sizeof(fonts) / sizeof(ext_font_desc_t));

// Set default LVGL theme
    lv_display_t *dispp = lv_display_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_display_set_theme(dispp, theme);
    
    // Initialize screens
    eez_flow_init_screen_names(screen_names, sizeof(screen_names) / sizeof(const char *));
    eez_flow_init_object_names(object_names, sizeof(object_names) / sizeof(const char *));
    
    // Create screens
    create_screen_main();
    create_screen_setting();
    create_screen_show_data();
    create_screen_about();
    create_screen_sensor_list();
    create_screen_startup();
    create_screen_file_saver();
    create_screen_file_manager();
}