#include <stdint.h>
#include "pomodoro.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define STACK_DEPTH 8192
#define TASK_PRIO tskIDLE_PRIORITY + 1
#define TASK_NAME "pomodoro_task_tick"
#define TAG "POMODORO"

bool state_is_active(pomodoro_t *pom);

struct pomodoro_t
{
    pomodoro_config_t config;
    pomodoro_state_t current_state;
    pomodoro_state_t previous_state;
    uint16_t current_timer;
    uint8_t completed_sessions;
};

TaskHandle_t task_handle;

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
    pomodoro->previous_state = POMODORO_STATE_INIT;
    pomodoro->current_timer = pomodoro->config.work_duration_seconds;
    pomodoro->completed_sessions = 0;

    return pomodoro;
}

void pomodoro_destroy(pomodoro_t *pomodoro)
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
    free(pomodoro);
}

void pomodoro_tick_task(void *pvParameters)
{
    pomodoro_t *pomodoro = (pomodoro_t *)pvParameters;
    
    if (!pomodoro)
    {
        ESP_LOGE(TAG, "Invalid pomodoro timer");
        vTaskDelete(NULL);
    }
    ESP_LOGW(TAG, "PRECONDITION MET!!");

    while (pomodoro->current_timer > 0)
    {
        ESP_LOGW(TAG, "IN LOOP");
        pomodoro->current_timer -= 1;
        if (pomodoro->config.tick_callback)
            pomodoro->config.tick_callback(pomodoro->current_timer, NULL);

        if (pomodoro->current_timer == 0)
        {
            if (pomodoro->current_state == POMODORO_STATE_WORK)
                pomodoro->completed_sessions = (pomodoro->completed_sessions + 1) % pomodoro->config.sessions_before_long_break;
            return pomodoro_stop(pomodoro);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL);
}

void pomodoro_stop(pomodoro_t *pom)
{
    if (!pom)
    {
        ESP_LOGE(TAG, "go_to_idle(): invalid pomodoro");
        return;
    }
    if (task_handle)
    {
        vTaskDelete(task_handle);
        task_handle = NULL;
    }
    pom->previous_state = pom->current_state;
    pom->current_state = POMODORO_STATE_IDLE;
    if (pom->config.state_transition_callback)
        pom->config.state_transition_callback(POMODORO_STATE_IDLE, NULL);
}

void pomodoro_start(pomodoro_t *pom)
{

    if (!pom)
    {
        ESP_LOGE(TAG, "pomodoro_start(): invalid pomodoro");
        return;
    }
    if (state_is_active(pom))
    {
        ESP_LOGE(TAG, "pomodoro_start(): already in active state %s", pom->current_state);
        return;
    }

    uint16_t next_timer = pom->current_timer;
    pomodoro_state_t next_state = POMODORO_STATE_WORK;
    switch (pom->previous_state)
    {
    case POMODORO_STATE_WORK:
        next_timer = pom->config.work_duration_seconds;
        next_state = POMODORO_STATE_WORK;
        break;
    case POMODORO_STATE_SHORT_BREAK:
        next_timer = pom->config.short_break_duration_seconds;
        next_state = POMODORO_STATE_SHORT_BREAK;
        break;
    case POMODORO_STATE_LONG_BREAK:
        next_timer = pom->config.long_break_duration_seconds;
        next_state = POMODORO_STATE_LONG_BREAK;
        break;
    case POMODORO_STATE_INIT:
        next_timer = pom->config.work_duration_seconds;
        next_state = POMODORO_STATE_WORK;
        break;
    case POMODORO_STATE_PAUSE:
        next_timer = pom->current_timer;
        next_state = POMODORO_STATE_WORK;
        break;
    default:
        ESP_LOGE(TAG, "pomodoro_start(): invalid previous state");
        break;
    }

    pom->current_timer = next_timer;
    pom->current_state = next_state;

    xTaskCreate(pomodoro_tick_task, TASK_NAME, STACK_DEPTH, (void *)pom, TASK_PRIO, &task_handle);
    if (pom->config.state_transition_callback)
        pom->config.state_transition_callback(next_state, NULL);
}

void pomodoro_reset(pomodoro_t *pom)
{
    if (!pom)
    {
        ESP_LOGE(TAG, "pomodoro_stop(): invalid pomodoro");
        return;
    }
    if (pom->current_state != POMODORO_STATE_IDLE)
    {
        ESP_LOGE(TAG, "pomodoro_stop(): pomodoro is not idle");
        return;
    }

    pom->previous_state = POMODORO_STATE_IDLE;
    pom->current_state = POMODORO_STATE_IDLE;
    if (pom->config.state_transition_callback)
        pom->config.state_transition_callback(POMODORO_STATE_IDLE, NULL);
}

bool state_is_active(pomodoro_t *pom)
{
    switch (pom->current_state)
    {
    case POMODORO_STATE_WORK:
    case POMODORO_STATE_SHORT_BREAK:
    case POMODORO_STATE_LONG_BREAK:
        return true;
    default:
        return false;
    }
}

char *state_to_string(pomodoro_state_t state)
{
    switch (state)
    {
    case POMODORO_STATE_INIT:
        return "POMODORO_STATE_INIT";
    case POMODORO_STATE_IDLE:
        return "POMODORO_STATE_IDLE";

    case POMODORO_STATE_WORK:
        return "POMODORO_STATE_WORK";
    case POMODORO_STATE_SHORT_BREAK:
        return "POMODORO_STATE_SHORT_BREAK";
    case POMODORO_STATE_LONG_BREAK:
        return "POMODORO_STATE_LONG_BREAK";
    case POMODORO_STATE_PAUSE:
        return "POMODORO_STATE_PAUSE";
    default:
        return "INVALID STATE!";
    }
}