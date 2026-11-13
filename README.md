# uav_uwb

UWB indoor positioning for PX4 vehicles.

Anchors and a tag measure ranges with DW1000 radios. A solver calculates the tag
position and sends it to PX4 EKF2.

The firmware uses C with ESP-IDF. The host tools use Python.

## Setup

Install the tools:

```bash
eim install -i v6.1 -t esp32s3
sudo apt install clang-format clang-tidy clangd ccache lcov
uv tool install prek
```

Install the Git hooks:

```sh
prek install
```

## Build and run

Get the DW1000 driver first. Refer to
[`firmware/components/dw1000/decadriver/README.md`](firmware/components/dw1000/decadriver/README.md).

Start a shell with the ESP-IDF environment:

```sh
eim shell
cd firmware
```

Connect a board. Find its port name and set `PORT`:

```sh
ls /dev/serial/by-id/
PORT=/dev/serial/by-id/<port name>
```

Build the firmware, write it to the board, and show the output:

```sh
idf.py -p $PORT flash
idf.py -p $PORT monitor --no-reset
```

Push `Ctrl+]` to stop the monitor.

If the monitor shows no output, refer to
[`docs/troubleshooting.md`](docs/troubleshooting.md).
