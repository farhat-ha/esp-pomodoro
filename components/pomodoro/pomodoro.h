#pragma once

#include <stdint.h>

typedef enum
{
    POMODORO_STATE_IDLE,
    POMODORO_STATE_WORK,
    POMODORO_STATE_SHORT_BREAK,
    POMODORO_STATE_LONG_BREAK,
} pomodoro_state_t;

typedef struct pomodoro_t pomodoro_t;
typedef void (*pomodoro_tick_cb)(uint16_t current_timer_seconds, void *context);
typedef void (*pomodoro_state_transition_cb)(pomodoro_state_t new_state, void *context);

typedef struct
{
    void *context;
    pomodoro_tick_cb tick_callback;
    pomodoro_state_transition_cb state_transition_callback;
    uint16_t work_duration_seconds;
    uint16_t short_break_duration_seconds;
    uint16_t long_break_duration_seconds;
    uint8_t sessions_before_long_break;

} pomodoro_config_t;

pomodoro_t *pomodoro_init(const pomodoro_config_t *config);
void pomodoro_destroy(pomodoro_t *pomodoro);
void pomodoro_set_state(pomodoro_t *pomodoro, pomodoro_state_t new_state);