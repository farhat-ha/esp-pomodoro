#include "esp_lv_adapter.h"
#include "esp_lv_adapter_display.h"
#include "graphics.h"
#include "esp_lcd_panel_dev.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#define H_RES 480
#define V_RES 320
#define BUFFER_SIZE (H_RES * V_RES / 10 * 2)

static const char *TAG = "graphics.c";
static esp_lv_adapter_config_t lv_config = ESP_LV_ADAPTER_DEFAULT_CONFIG();
static lv_display_t *display;
static lv_obj_t *lbl_timer;
static lv_obj_t *bar_timer;
static lv_obj_t *lbl_state;
static char time_buf[8];
static lv_color_t color_work;
static lv_color_t color_short_break;
static lv_color_t color_long_break;


LV_FONT_DECLARE(space_mono_bold_64);
LV_FONT_DECLARE(space_mono_20);

void graphics_init(esp_lcd_panel_handle_t panel_handle, esp_lcd_panel_io_handle_t io_handle) {
    ESP_ERROR_CHECK(
        esp_lv_adapter_init(&lv_config));

    esp_lv_adapter_display_config_t display_config = ESP_LV_ADAPTER_DISPLAY_SPI_WITHOUT_PSRAM_DEFAULT_CONFIG(
        panel_handle, // LCD panel handle
        io_handle, // LCD panel IO handle (can be NULL for some interfaces)
        H_RES, // Horizontal resolution
        V_RES, // Vertical resolution
        ESP_LV_ADAPTER_ROTATE_0 // Rotation
    );

    display = esp_lv_adapter_register_display(&display_config);
    if (display == NULL) {
        ESP_LOGE(TAG, "display failed to register");
    }

    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    lv_obj_set_style_text_color(screen, lv_color_white(), 0);
    /* INIT COLORS */
    color_work = lv_color_hex(0x0be4ff);
    color_short_break = lv_color_hex(0xffa50b);
    color_long_break = lv_color_hex(0x83ff0b);
    /* INIT TIMER LABEL*/
    lbl_timer = lv_label_create(screen);
    lv_obj_set_style_text_font(lbl_timer, &space_mono_bold_64, 0);
    lv_label_set_text(lbl_timer, "00:00");
    lv_obj_align(lbl_timer, LV_ALIGN_CENTER, 0, -20);
    /* INIT TIMER BAR*/
    bar_timer = lv_bar_create(screen);
    lv_obj_set_size(bar_timer, 190, 3);
    lv_obj_align_to(bar_timer, lbl_timer, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_bar_set_value(bar_timer, 0, LV_ANIM_OFF);
    /* INIT STATE LABEL */
    lbl_state = lv_label_create(screen);
    lv_obj_set_style_text_font(lbl_state, &space_mono_20, 0);
    lv_obj_set_style_text_color(lbl_state, lv_color_black(), 0);
    lv_obj_set_style_bg_color(lbl_state, color_work, 0);
    lv_obj_set_style_pad_hor(lbl_state, 10, 0);
    lv_obj_set_style_pad_ver(lbl_state, 4, 0);
    lv_obj_set_style_bg_opa(lbl_state, LV_OPA_COVER, 0);
    lv_label_set_text(lbl_state, "INIT");
    lv_obj_set_pos(lbl_state, 30, 100);
    esp_lv_adapter_start();
}

void graphics_bar_set_range(const uint16_t max_value) {
    if (esp_lv_adapter_lock(portMAX_DELAY) == ESP_OK) {
        lv_bar_set_range(bar_timer, 0, max_value);
        esp_lv_adapter_unlock();
    } else {
        ESP_LOGW(TAG, "Failed to acquire LVGL lock for bar max value update");
    }
}

void graphics_update_bar(const uint16_t value) {
    if (esp_lv_adapter_lock(portMAX_DELAY) == ESP_OK) {
        lv_bar_set_value(bar_timer, value, LV_ANIM_ON);
        esp_lv_adapter_unlock();
    } else {
        ESP_LOGW(TAG, "Failed to acquire LVGL lock for bar value update");
    }
}

void graphics_update_state(const char *next_state) {
    if (esp_lv_adapter_lock(portMAX_DELAY) == ESP_OK) {
        lv_label_set_text(lbl_state, next_state);
        esp_lv_adapter_unlock();
    } else {
        ESP_LOGW(TAG, "Failed to acquire LVGL lock for state update");
    }
}

void graphics_update_time(const uint16_t timer_seconds) {
    snprintf(time_buf, sizeof(time_buf), "%02d:%02d", timer_seconds / 60, timer_seconds % 60);
    if (esp_lv_adapter_lock(portMAX_DELAY) == ESP_OK) {
        lv_label_set_text_static(lbl_timer, time_buf);
        esp_lv_adapter_unlock();
    } else {
        ESP_LOGW(TAG, "Failed to acquire LVGL lock for timer update");
    }
}
