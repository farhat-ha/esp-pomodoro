#include "display.h"
#include "esp_log.h"
#include "esp_lcd_panel_io_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_gc9a01.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"

#define H_RES 240
#define V_RES 240
#define BPP 16
#define MAX_TRANSACTION_SIZE H_RES * 80 * BPP / 8

// PINS
#define CLK_PIN GPIO_NUM_9
#define DATA_PIN GPIO_NUM_18
#define RESET_PIN GPIO_NUM_19
#define DC_PIN GPIO_NUM_20
#define CS_PIN GPIO_NUM_21
#define BLK_PIN GPIO_NUM_7

static const char *TAG = "display.c";
static const spi_bus_config_t bus_config = GC9A01_PANEL_BUS_SPI_CONFIG(
    CLK_PIN,
    DATA_PIN,
    MAX_TRANSACTION_SIZE);

static const esp_lcd_panel_io_spi_config_t spi_panel_config = GC9A01_PANEL_IO_SPI_CONFIG(
    CS_PIN, DC_PIN, NULL, NULL);
static const esp_lcd_panel_dev_config_t panel_config = {
    .bits_per_pixel = BPP,
    .reset_gpio_num = RESET_PIN,
    .rgb_endian = COLOR_RGB_ELEMENT_ORDER_RGB,
};

static esp_lcd_panel_handle_t panel_handle;
static esp_lcd_panel_io_handle_t io_handle;

void init_display()
{
    ESP_ERROR_CHECK(
        spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(
        esp_lcd_new_panel_io_spi(
            (esp_lcd_spi_bus_handle_t)SPI2_HOST, &spi_panel_config, &io_handle));
    ESP_ERROR_CHECK(
        esp_lcd_new_panel_gc9a01(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(
        esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(
        esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(
        esp_lcd_panel_invert_color(panel_handle, true));
    ESP_ERROR_CHECK(
        esp_lcd_panel_mirror(panel_handle, true, false));

    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, false));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    ESP_ERROR_CHECK(gpio_set_direction(BLK_PIN, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_level(BLK_PIN, 1));

    ESP_LOGI(TAG, "All good :+1:");
}

esp_lcd_panel_handle_t display_get_panel_handle()
{
    return panel_handle;
}

esp_lcd_panel_io_handle_t display_get_io_handle()
{
    return io_handle;
}