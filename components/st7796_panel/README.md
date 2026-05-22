# st7796_panel

Reusable ESP-IDF LCD panel driver component for ST7796 controllers over `esp_lcd_panel_io`.

## Public API

- `esp_lcd_new_panel_st7796_general(...)`
- `st7796_lcd_init_cmd_t`
- `st7796_vendor_config_t`

## Usage

```c
#include "esp_lcd_st7796.h"

esp_lcd_panel_dev_config_t panel_cfg = {
    .reset_gpio_num = GPIO_NUM_20,
    .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
    .bits_per_pixel = 16,
};

esp_lcd_panel_handle_t panel = NULL;
ESP_ERROR_CHECK(esp_lcd_new_panel_st7796_general(io_handle, &panel_cfg, &panel));
ESP_ERROR_CHECK(esp_lcd_panel_reset(panel));
ESP_ERROR_CHECK(esp_lcd_panel_init(panel));
```

## Custom init commands

Set `panel_cfg.vendor_config` to `st7796_vendor_config_t` to override default init sequence.

