#include <stdlib.h>

#include "unity.h"
#include "unity_test_runner.h"

// Runs all tests one time. The exit status is the number of failed tests.
void app_main(void) {
    UNITY_BEGIN();
    unity_run_all_tests();
    exit(UNITY_END());
}
