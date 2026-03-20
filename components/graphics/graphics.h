#pragma once

void init_graphics(esp_lcd_panel_handle_t panel_handle, esp_lcd_panel_io_handle_t io_handle);
void graphics_draw_text(char *text);