#include <stdlib.h>
#include "pomodoro.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define STACK_SIZE 8192

void pomodoro_go_to_next_state(pomodoro_t *pomodoro);
struct pomodoro_t
{
    pomodoro_config_t config;
    pomodoro_state_t current_state;
    uint16_t current_timer;
    uint8_t completed_sessions;
};

static const char *TAG = "POMODORO";
static const char *POMODORO_TASK_NAME = "pomodoro_tick_task";

TaskHandle_t pomodoro_ticker_handle;

void pomodoro_tick_task(void *pvParameters)
{
    pomodoro_t *pomodoro = (pomodoro_t *)pvParameters;

    if (!pomodoro)
    {
        ESP_LOGE(TAG, "Invalid pomodoro timer");
        vTaskDelete(NULL);
    }
    // Ideally here we want to update our display;
    while (true)
    {

        pomodoro->current_timer -= 1;
        if (pomodoro->config.tick_callback)
        {
            pomodoro->config.tick_callback(pomodoro->current_timer, pomodoro->config.context);
        }

        if (pomodoro->current_timer == 0)
        {
            pomodoro->completed_sessions = (pomodoro->completed_sessions + 1) % pomodoro->config.sessions_before_long_break;
            pomodoro_go_to_next_state(pomodoro);
            vTaskDelete(NULL);
            // pomodoro_ticker_handle = NULL;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

pomodoro_t *pomodoro_init(const pomodoro_config_t *config)
{
    pomodoro_t *pomodoro = malloc(sizeof(pomodoro_t));
    if (!pomodoro)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for pomodoro timer");
        return NULL;
    }
    pomodoro->config = *config;
    pomodoro->current_state = POMODORO_STATE_IDLE;
    pomodoro->current_timer = pomodoro->config.work_duration_seconds;

    return pomodoro;
}

void pomdooro_destroy(pomodoro_t *pomodoro)
{
    if (!pomodoro)
    {
        return;
    }

    if (pomodoro->current_state != POMODORO_STATE_IDLE)
    {
        ESP_LOGE(TAG, "Cannot destroy pomodoro timer while it's running");
        return;
    }
    // TODO: will the config.context leak? should we free it here?
    free(pomodoro);
}

void pomodoro_set_state(pomodoro_t *pomodoro, pomodoro_state_t new_state)
{
    if (!pomodoro)
    {
        ESP_LOGE(TAG, "Invalid pomodoro timer");
        return;
    }

    pomodoro->current_state = new_state;

    if (pomodoro->config.state_transition_callback)
    {
        pomodoro->config.state_transition_callback(pomodoro->current_state, pomodoro->config.context);
    }

    switch (new_state)
    {
    case POMODORO_STATE_WORK:
        pomodoro->current_timer = pomodoro->config.work_duration_seconds;
        xTaskCreate(&pomodoro_tick_task, POMODORO_TASK_NAME, STACK_SIZE, pomodoro, tskIDLE_PRIORITY + 1, &pomodoro_ticker_handle);
        break;
    case POMODORO_STATE_SHORT_BREAK:
        pomodoro->current_timer = pomodoro->config.short_break_duration_seconds;
        xTaskCreate(&pomodoro_tick_task, POMODORO_TASK_NAME, STACK_SIZE, pomodoro, tskIDLE_PRIORITY + 1, &pomodoro_ticker_handle);
        break;
    case POMODORO_STATE_LONG_BREAK:
        pomodoro->current_timer = pomodoro->config.long_break_duration_seconds;
        xTaskCreate(&pomodoro_tick_task, POMODORO_TASK_NAME, STACK_SIZE, pomodoro, tskIDLE_PRIORITY + 1, &pomodoro_ticker_handle);
        break;
    case POMODORO_STATE_IDLE:
        pomodoro->current_timer = 0;
        vTaskDelete(pomodoro_ticker_handle);
        pomodoro_ticker_handle = NULL;
        break;
    default:
        break;
    }
}

void pomodoro_go_to_next_state(pomodoro_t *pomodoro)
{
    if (!pomodoro)
    {
        ESP_LOGE(TAG, "Invalid pomodoro timer");
        return;
    }
    switch (pomodoro->current_state)
    {
    case POMODORO_STATE_IDLE:
    case POMODORO_STATE_LONG_BREAK:
    case POMODORO_STATE_SHORT_BREAK:
        pomodoro_set_state(pomodoro, POMODORO_STATE_WORK);
        break;
    case POMODORO_STATE_WORK:
        // Check if next is long break or short break;
        if (pomodoro->completed_sessions == 0)
        {
            pomodoro_set_state(pomodoro, POMODORO_STATE_LONG_BREAK);
        }
        else
        {
            pomodoro_set_state(pomodoro, POMODORO_STATE_SHORT_BREAK);
        }
    default:
        break;
    }
}