#include "display.h"
#include "esp_lcd_panel_dev.h"
#include "graphics.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "pomodoro.h"
#include <string.h>

static char text_buffer[100];
static pomodoro_t *pomodoro;

void on_pomodoro_tick(uint16_t current_timer, void *ctx)
{
    const uint16_t minutes = current_timer / 60;
    const uint16_t seconds = current_timer % 60;
    sprintf(text_buffer, "%d:%d", minutes, seconds);
    graphics_draw_text(text_buffer);
    ESP_LOGW("APP", "%d:%d", minutes, seconds);
}
void on_pomodoro_state(pomodoro_state_t next_state, void *ctx)
{
    ESP_LOGI("APP", "next state is called with value %d", next_state);
}

void app_main(void)
{

    static const pomodoro_config_t pomodoro_config = {
        .work_duration_seconds = 25 * 60,
        .long_break_duration_seconds = 10 * 60,
        .short_break_duration_seconds = 5 * 60,
        .tick_callback = on_pomodoro_tick,
        .state_transition_callback = on_pomodoro_state};

    pomodoro = pomodoro_init(&pomodoro_config);
    init_display();
    init_graphics(
        display_get_panel_handle(), display_get_io_handle());
    pomodoro_start(pomodoro);
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}