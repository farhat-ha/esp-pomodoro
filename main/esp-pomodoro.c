#include "display.h"
#include "esp_log.h"
#include "graphics.h"
#include "pomodoro.h"
#include "freertos/FreeRTOS.h"
#define WORK_DURATION_SECONDS (1 * 10)
#define SHORT_BREAK_DURATION_SECONDS (1 * 60)
#define LONG_BREAK_DURATION_SECONDS (1 * 60)
#define SESSIONS_BEFORE_LONG_BREAK 6

static void on_tick(const uint16_t current_timer_seconds, void *context) {
    graphics_update_time(current_timer_seconds);
    graphics_update_bar(WORK_DURATION_SECONDS - current_timer_seconds);
}

static void on_state_changed(const pomodoro_state_t new_state, void *context) {
    ESP_LOGI("TAG", "State changed to %s", state_to_string(new_state));
    graphics_update_state(state_to_string(new_state));
}

static pomodoro_config_t pomodoro_config = {
    .work_duration_seconds = WORK_DURATION_SECONDS,
    .long_break_duration_seconds = LONG_BREAK_DURATION_SECONDS,
    .sessions_before_long_break = SESSIONS_BEFORE_LONG_BREAK,
    .short_break_duration_seconds = SHORT_BREAK_DURATION_SECONDS,
    .tick_callback = on_tick,
    .state_transition_callback = on_state_changed,
};


void app_main(void) {
    display_init();
    graphics_init(
        display_get_panel_handle(), display_get_io_handle());
    graphics_bar_set_range(WORK_DURATION_SECONDS);
    pomodoro_start(
        pomodoro_init(&pomodoro_config)
    );

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
