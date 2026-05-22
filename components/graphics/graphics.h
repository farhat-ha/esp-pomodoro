#pragma once

void graphics_init(esp_lcd_panel_handle_t panel_handle, esp_lcd_panel_io_handle_t io_handle);

void graphics_destroy(esp_lcd_panel_handle_t panel_handle);

void graphics_draw();

void graphics_update_time(uint16_t timer_seconds);

void graphics_bar_set_range(uint16_t max);


void graphics_update_bar(uint16_t value);

void graphics_update_state(const char *next_state);
