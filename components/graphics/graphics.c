
#include "esp_lv_adapter.h"
#include "esp_lv_adapter_display.h"
#include "graphics.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_dev.h"
#include "esp_log.h"
#define H_RES 240
#define V_RES 240

static const char *TAG = "graphics.c";
static esp_lv_adapter_config_t lv_config = ESP_LV_ADAPTER_DEFAULT_CONFIG();
static lv_display_t *display;
void init_graphics(esp_lcd_panel_handle_t panel_handle, esp_lcd_panel_io_handle_t io_handle)
{
    ESP_ERROR_CHECK(
        esp_lv_adapter_init(&lv_config));

    esp_lv_adapter_display_config_t display_config = ESP_LV_ADAPTER_DISPLAY_SPI_WITHOUT_PSRAM_DEFAULT_CONFIG(
        panel_handle,           // LCD panel handle
        io_handle,              // LCD panel IO handle (can be NULL for some interfaces)
        H_RES,                  // Horizontal resolution
        V_RES,                  // Vertical resolution
        ESP_LV_ADAPTER_ROTATE_0 // Rotation
    );

    display = esp_lv_adapter_register_display(&display_config);
    if (display == NULL)
    {
        ESP_LOGE(TAG, "display failed to register");
    }

    esp_lv_adapter_start();
}

void graphics_draw_text(char *text)
{
    if (esp_lv_adapter_lock(-1) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to lock LVGL");
        return;
    }

    lv_obj_clean(lv_scr_act()); // clear previous content

    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, text);
    lv_obj_center(label);

    esp_lv_adapter_unlock();
}