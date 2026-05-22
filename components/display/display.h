#pragma once

#include "esp_lcd_panel_dev.h"

#define SCREEN_W 480
#define SCREEN_H 320


#define SPI_CLK GPIO_NUM_6
#define SPI_MOSI GPIO_NUM_7
#define SPI_RST GPIO_NUM_3
#define SPI_DC GPIO_NUM_11
#define SPI_CS GPIO_NUM_10
#define SPI_BL GPIO_NUM_1


void display_backlight(bool state);

void display_init();

void display_destroy();

esp_lcd_panel_handle_t display_get_panel_handle();

esp_lcd_panel_io_handle_t display_get_io_handle();
