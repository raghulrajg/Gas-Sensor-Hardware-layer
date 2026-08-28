#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_SETTING = 2,
    SCREEN_ID_SHOW_DATA = 3,
    SCREEN_ID_ABOUT = 4,
    SCREEN_ID_SENSOR_LIST = 5,
    SCREEN_ID_STARTUP = 6,
    _SCREEN_ID_LAST = 6
};

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *setting;
    lv_obj_t *show_data;
    lv_obj_t *about;
    lv_obj_t *sensor_list;
    lv_obj_t *startup;
    lv_obj_t *panel_header;
    lv_obj_t *title;
    lv_obj_t *about_button;
    lv_obj_t *setting_label;
    lv_obj_t *show_data_label;
    lv_obj_t *about_label;
    lv_obj_t *setting_button;
    lv_obj_t *show_data_button;
    lv_obj_t *panel_header_1;
    lv_obj_t *title_1;
    lv_obj_t *back_button_1;
    lv_obj_t *back_1;
    lv_obj_t *obj0;
    lv_obj_t *tgs_825;
    lv_obj_t *tgs_2602;
    lv_obj_t *mq_3;
    lv_obj_t *mq_138;
    lv_obj_t *mq_137;
    lv_obj_t *wsp_2110;
    lv_obj_t *tgs825_label;
    lv_obj_t *tgs2602_label;
    lv_obj_t *mq3_label;
    lv_obj_t *mq138_label;
    lv_obj_t *mq137_label;
    lv_obj_t *wsp2110_label;
    lv_obj_t *panel_header_2;
    lv_obj_t *title_2;
    lv_obj_t *back_button;
    lv_obj_t *back;
    lv_obj_t *graph;
    lv_obj_t *calibration_bar;
    lv_obj_t *calibration_data;
    lv_obj_t *obj1;
    lv_obj_t *temperature;
    lv_obj_t *humidity;
    lv_obj_t *obj2;
    lv_obj_t *panel_header_3;
    lv_obj_t *title_3;
    lv_obj_t *back_button_2;
    lv_obj_t *back_2;
    lv_obj_t *obj3;
    lv_obj_t *panel_header_5;
    lv_obj_t *title_5;
    lv_obj_t *button_panel;
    lv_obj_t *tgs825_button;
    lv_obj_t *tgs2602_button;
    lv_obj_t *wsp2110_button;
    lv_obj_t *mq138_button;
    lv_obj_t *tgs1820_button;
    lv_obj_t *mq3_button;
    lv_obj_t *mq137_button;
    lv_obj_t *tgs825;
    lv_obj_t *tgs2602;
    lv_obj_t *tgs1820;
    lv_obj_t *mq137;
    lv_obj_t *mq3;
    lv_obj_t *mq138;
    lv_obj_t *wsp2110;
    lv_obj_t *obj4;
    lv_obj_t *boot_logo;
} objects_t;

extern objects_t objects;

void create_screen_main();
void tick_screen_main();

void create_screen_setting();
void tick_screen_setting();

void create_screen_show_data();
void tick_screen_show_data();

void create_screen_about();
void tick_screen_about();

void create_screen_sensor_list();
void tick_screen_sensor_list();

void create_screen_startup();
void tick_screen_startup();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/