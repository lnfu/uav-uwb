# Logic that does not use hardware. It also builds for the linux target, so
# its tests run on the host (see host_test/).
idf_component_register(SRCS "spi_unskew.c"
                       INCLUDE_DIRS "include")
