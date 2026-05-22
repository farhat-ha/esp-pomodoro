/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_lcd_panel_dev.h"
#include "esp_lcd_st7796_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_LCD_ST7796_VER_MAJOR 1
#define ESP_LCD_ST7796_VER_MINOR 0
#define ESP_LCD_ST7796_VER_PATCH 0

esp_err_t esp_lcd_new_panel_st7796_general(const esp_lcd_panel_io_handle_t io,
                                           const esp_lcd_panel_dev_config_t *panel_dev_config,
                                           esp_lcd_panel_handle_t *ret_panel);

#ifdef __cplusplus
}
#endif

