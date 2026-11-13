# decadriver/ holds the Decawave driver, which is not in this repository
# (see decadriver/README.md). port/ is the platform layer that the driver calls.
if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/decadriver/deca_device.c")
    message(FATAL_ERROR "DW1000 driver not found. Read ${CMAKE_CURRENT_LIST_DIR}/decadriver/README.md")
endif()

idf_component_register(
    SRCS "decadriver/deca_device.c"
         "decadriver/deca_params_init.c"
         "port/dw1000_port.c"
    INCLUDE_DIRS "decadriver" "port"
    PRIV_REQUIRES esp_driver_spi esp_driver_gpio)
