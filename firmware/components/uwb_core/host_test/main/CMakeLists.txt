idf_component_register(SRCS "test_main.c" "test_spi_unskew.c"
                       REQUIRES unity uwb_core
                       WHOLE_ARCHIVE)
