# Test

The tests for the code that does not use hardware run on the computer.

Install the library that the ESP-IDF linux target uses:

```sh
sudo apt install libbsd-dev
```

Build and run the tests in the ESP-IDF shell:

```sh
cd firmware/components/uwb_core/host_test
idf.py build
build/uwb_core_host_test.elf
```

The last lines show the number of tests and the number of failures.
