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
