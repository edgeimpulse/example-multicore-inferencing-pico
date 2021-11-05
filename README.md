# Edge Impulse Example: multicore inferencing (Raspberry Pi Pico)

This repository runs an exported impulse on the Raspberry Pi Pico / RP2040. See the documentation at [Running your impulse locally](https://docs.edgeimpulse.com/docs/running-your-impulse-locally-1). This repository is based on the [Edge Impulse Example: stand-alone inferencing (Raspberry Pi Pico)](https://github.com/edgeimpulse/example-standalone-inferencing-pico).

## Requirements

### Hardware

* [Raspberry Pi Pico](https://www.raspberrypi.org/products/raspberry-pi-pico/).
* [MPU-6050 Module](https://www.dfrobot.com/product-880.html)

### Software

* [Edge Impulse CLI](https://docs.edgeimpulse.com/docs/cli-installation).
* [GNU ARM Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads).
* [CMake](https://cmake.org/install/).
* Rasperry Pi Pico SDK:
   ```bash
   git clone -b master https://github.com/raspberrypi/pico-sdk.git
   cd pico-sdk
   git submodule update --init
   export PICO_SDK_PATH=`pwd`
   ```

## Building the application

### Get the Edge Impulse SDK

Unzip the deployed `C++ library` from your Edge Impulse project and copy only the folders to the root directory of this repository:

   ```
   example-multicore-inferencing-pico/
   ├─ edge-impulse-sdk/
   ├─ model-parameters/
   ├─ source/
   ├─ tflite-model/
   ├─ .gitignore
   ├─ CMakeLists.txt
   ├─ LICENSE
   ├─ README.md
   └─ pico_sdk_import.cmake
   ```

### Compile

1. Create the `build` folder:
   ```bash
   mkdir build && cd build
   ```
1. Compile:
   ```bash
   cmake .. && cmake --build . --parallel
   ```

### Flash

Connect the Raspberry Pi Pico to your computer using a micro-USB cable while pressing and holding the **BOOTSEL** button.

Drag and drop the `build/example_multicore.uf2` file to the **RPI-RP2** disk in your file explorer.

### Serial connection

Use screen or minicom to set up a serial connection over USB. The following UART settings are used: 115200 baud, 8N1.
