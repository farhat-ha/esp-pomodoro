#include "input.h"
#include "button_gpio.h"
#include "iot_button.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

#define BTN_IO_LEVEL 0

static const button_gpio_config_t button_1_gpio_config = {
    .active_level = BTN_IO_LEVEL,
    .gpio_num = GPIO_NUM_20,
    .disable_pull = false
};

static const button_config_t default_button_config = {.short_press_time = 100, .long_press_time = 1500};
static button_handle_t button_1_handle = NULL;

void input_init(const input_button_config_t *config) {

    gpio_set_pull_mode(GPIO_NUM_20, GPIO_PULLUP_ONLY);
    ESP_ERROR_CHECK(iot_button_new_gpio_device(&default_button_config, &button_1_gpio_config, &button_1_handle));

    iot_button_register_cb(button_1_handle,BUTTON_PRESS_DOWN,NULL, config->btn_1_press_cb, (void*) "DOWN");
    iot_button_register_cb(button_1_handle,BUTTON_PRESS_UP,NULL, config->btn_1_press_cb, (void*) "UP");
}
