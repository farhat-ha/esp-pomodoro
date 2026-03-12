#include <stdio.h>
#include "esp_log.h"
#include "pomodoro.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void graphics_update_time(uint16_t current_time, void *context)
{
    ESP_LOGI("TICK_CALLBACK", "Current timer: %d seconds", current_time);
}

void pomodoro_on_state_transition(pomodoro_state_t new_state, void *context)
{
    ESP_LOGI("ON_STATE_TRANSITON_CB", "Transitioned to state: %d", new_state);
}

static const pomodoro_config_t pomodoro_config = {
    .context = NULL,
    .tick_callback = graphics_update_time,
    .state_transition_callback = pomodoro_on_state_transition,
    .work_duration_seconds = 5,        // 25 minutes
    .short_break_duration_seconds = 2, // 5 minutes
    .long_break_duration_seconds = 3,  // 15 minutes
    .sessions_before_long_break = 4};

pomodoro_t *pomodoro;

void app_main(void)
{
    pomodoro = pomodoro_init(&pomodoro_config);
    ESP_LOGI("ESP-Pomodoro", "Hello, ESP-Pomodoro!");
    vTaskDelay(pdMS_TO_TICKS(2000)); // Wait for 2 seconds before starting the timer
    pomodoro_set_state(pomodoro, POMODORO_STATE_WORK);
}
