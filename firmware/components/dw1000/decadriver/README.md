# Decawave DW1000 driver

This directory is for the DW1000 device driver from Decawave (now Qorvo).

This repository does not contain the driver files. The Decawave license permits
use and modification, but it does not clearly permit redistribution. You must
get the files and put them here before you build the firmware.

## Get the archive

1. Go to the DW1000 product page: <https://www.qorvo.com/products/p/DW1000>.
2. Open the "Design Resources" section, then "Software".
3. Download the DW1000 API software, revision 2.14. The file name is
   `dw1000_api_rev2p14.zip`.
4. Put the file in this directory.
5. Make sure that the file is correct:

```sh
sha256sum dw1000_api_rev2p14.zip
```

The result must be:

```text
f7baf8b3b113caafb3db65c3214a95a0b80fa0b98461b8b45dc6d90fbf9593ba
```

## Extract the driver

Do these commands in this directory:

```sh
unzip -j dw1000_api_rev2p14.zip dw1000_api_rev2p14_stsw.zip
unzip -j dw1000_api_rev2p14_stsw.zip 'dw1000_api_rev2p14_stsw/Src/decadriver/*'
rm dw1000_api_rev2p14_stsw.zip
```

This directory must then contain these 7 files:

- `deca_device.c`
- `deca_device_api.h`
- `deca_param_types.h`
- `deca_params_init.c`
- `deca_regs.h`
- `deca_types.h`
- `deca_version.h`

## Rules

- Use only the `stsw` archive. Do not use the `coocox` archive.
- Do not change the driver files. Do not format them. They have CRLF line ends.
- Do not add the driver files or the archive to Git. The `.gitignore` file
  ignores all files in this directory except this document.
- The license is in the archive: `dw1000_api_rev2p14_stsw/Src/disclaimer.txt`.
