#include "display.h"
#include "driver/gpio.h"
#include "esp_heap_caps.h"
#include "driver/spi_master.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_st7796.h"

#define LINES_PER_CHUNK 32   // tune this


spi_bus_config_t bus_config = {
    .sclk_io_num = SPI_CLK,
    .mosi_io_num = SPI_MOSI,
    .miso_io_num = -1,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = SCREEN_W * 80 * sizeof(uint16_t)
};
esp_lcd_panel_io_spi_config_t panel_io_config = {
    .cs_gpio_num = SPI_CS,
    .dc_gpio_num = SPI_DC,
    .spi_mode = 0,
    .pclk_hz = 40 * 1000 * 1000,
    .trans_queue_depth = 10,
    .lcd_cmd_bits = 8,
    .lcd_param_bits = 8
};

esp_lcd_panel_dev_config_t panel_config = {
    .reset_gpio_num = SPI_RST,
    .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
    .bits_per_pixel = 16,
};

esp_lcd_panel_io_handle_t panel_io = nullptr;
esp_lcd_panel_handle_t panel = nullptr;
uint16_t *frame_buffer = nullptr;


void display_backlight(const bool state) {
    gpio_set_direction(SPI_BL, GPIO_MODE_OUTPUT);
    gpio_set_level(SPI_BL, state);
}

void display_init(void) {
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(SPI2_HOST,&panel_io_config, &panel_io));
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7796_general(panel_io, &panel_config, &panel));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel, true));
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel, true));
    display_backlight(true);
}


void display_destroy(void) {
    free(frame_buffer);
    frame_buffer = nullptr;
}


esp_lcd_panel_handle_t display_get_panel_handle(void) {
    return panel;
}

esp_lcd_panel_io_handle_t display_get_io_handle(void) {
    return panel_io;
}
