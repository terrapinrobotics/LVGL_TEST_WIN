#include <stdlib.h>
#include "lvgl/lvgl.h"
#include "lv_drivers/win32drv/win32drv.h"
#include <windows.h>

static void hal_init(void);
static int tick_thread(void *data);

bool init_display(int w, int h) {
    if (!lv_win32_init(
                       GetModuleHandleW(NULL),
                       SW_SHOW,
                       w,
                       h,
                       LoadIconW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(1))))
    {
        return false;
    }

    lv_win32_add_all_input_devices_to_group(NULL);

    return true;
}

static lv_style_t style_title_lg;
static lv_style_t style_title;
static lv_style_t style_bar;
static lv_style_t style_text_underline;
static lv_style_t style_button;
static lv_style_t style_button_pressed;

// our primary accent color
static lv_color_t accent_color;

// init screen
static lv_style_t style_text;

static void clear_screen() {
    lv_obj_clean(lv_scr_act());
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x23282D), LV_PART_MAIN);
}

static void style_init() {
    // set the screen bg color
    clear_screen();

    accent_color = lv_color_hex(0xf5c116);

    // the title label
    lv_style_init(&style_title_lg);
    lv_style_set_text_color(&style_title_lg, lv_color_hex(0xFFFFFF));
    lv_style_set_text_font(&style_title_lg, &lv_font_montserrat_32);
    lv_style_set_text_align(&style_title_lg, LV_TEXT_ALIGN_CENTER);

    // the normal title label
    lv_style_init(&style_title);
    lv_style_set_text_color(&style_title, lv_color_hex(0xFFFFFF));
    lv_style_set_text_font(&style_title, &lv_font_montserrat_20);
    lv_style_set_text_align(&style_title, LV_TEXT_ALIGN_CENTER);

    // the bar
    lv_style_init(&style_bar);
    lv_style_set_bg_opa(&style_bar, LV_OPA_COVER);
    lv_style_set_bg_color(&style_bar, accent_color);
    lv_style_set_radius(&style_bar, 10);

    // the text
    lv_style_init(&style_text);
    lv_style_set_text_color(&style_text, lv_color_hex(0xFFFFFF));
    lv_style_set_text_font(&style_text, &lv_font_montserrat_14);
    lv_style_set_text_align(&style_text, LV_TEXT_ALIGN_CENTER);

    // the text underline
    lv_style_init(&style_text_underline);
    lv_style_set_text_decor(&style_text_underline, LV_TEXT_DECOR_UNDERLINE);

    // the button
    lv_style_init(&style_button);
    lv_style_set_outline_width(&style_button, 0);

    // the button pressed
    lv_style_init(&style_button_pressed);
    lv_style_set_bg_color(&style_button_pressed, accent_color);
}

void set_bar_percent(void *bar, int32_t percent) {
    lv_bar_set_value(bar, percent, LV_ANIM_ON);
}

void bar_anim(lv_obj_t *bar) {
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_bar_percent);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_var(&a, bar);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_start(&a);
}

const int TITLE_INIT_OFFSET = 25;

void display_init_screen() {
    style_init();

    // create a title label
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -TITLE_INIT_OFFSET);
    lv_label_set_text(label, "Initializing");
    lv_obj_add_style(label, &style_title_lg, 0);

    // create a small label in the bottom left
    lv_obj_t *label2 = lv_label_create(lv_scr_act());
    lv_obj_align(label2, LV_ALIGN_BOTTOM_LEFT, 5, -5);
    lv_label_set_text(label2, "ReAuto v1.0.0");
    lv_obj_add_style(label2, &style_text, 0);

    // now a small label in the bottom right
    /*lv_obj_t *label3 = lv_label_create(lv_scr_act());
    lv_obj_align(label3, LV_ALIGN_BOTTOM_RIGHT, -5, -5);
    lv_label_set_text(label3, "Davis S. | 727G");
    lv_obj_add_style(label3, &style_text, 0);*/

    // create a bar!
    lv_obj_t *bar = lv_bar_create(lv_scr_act());
    lv_obj_set_size(bar, 220, 20);
    lv_obj_align(bar, LV_ALIGN_CENTER, 0, TITLE_INIT_OFFSET);

    // style the bar
    lv_obj_add_style(bar, &style_bar, LV_PART_INDICATOR);

    // animate the bar
    bar_anim(bar);
}

void display_auton_selector() {
    // clear the screen
    clear_screen();

    // create a title label
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 30, 25);
    lv_label_set_text(label, "Select Auto");
    lv_obj_add_style(label, &style_title, 0);
    lv_obj_add_style(label, &style_text_underline, 0);

    // create a button
    lv_obj_t *autoA = lv_btn_create(lv_scr_act());
    lv_obj_set_size(autoA, 200, 50);
    lv_obj_align(autoA, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(autoA, &style_button, 0);
    lv_obj_add_style(autoA, &style_button_pressed, LV_STATE_CHECKED);
    lv_obj_add_flag(autoA, LV_OBJ_FLAG_CHECKABLE);

    // create a label for the button
    lv_obj_t *labelA = lv_label_create(autoA);
    lv_label_set_text(labelA, "Auto A");
    lv_obj_add_style(labelA, &style_text, 0);
}

int main() {
    lv_init();
    if (!init_display(480, 272)) return -1;

    display_init_screen();

    // track current time
    long tick = 0;

    while (!lv_win32_quit_signal) {
        lv_task_handler();

        // check if it's been 2000 ms
        if (tick == 000) {
            display_auton_selector();
        }

        Sleep(1);
        tick++;
    }

    return 0;
}
