idf_component_register(
    SRCS "display.c"
    INCLUDE_DIRS "."
    REQUIRES esp_lcd esp_driver_gpio st7796_panel
)