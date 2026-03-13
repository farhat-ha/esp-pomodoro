#pragma once

#include "iot_button.h"

typedef struct {
    button_cb_t btn_1_press_cb;
    button_cb_t btn_2_press_cb;
} input_button_config_t;

void input_init(const input_button_config_t* config);