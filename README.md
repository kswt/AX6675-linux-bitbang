# MAX6675-linux-bitbang
GNU/Linux application to read temperature from thermocouple and MAX6675 chip connected to GPIO pins. It uses new UAPI kernel interface and works without hardware SPI support.
## Compilation
1. Install `libgpiod` package
2. Edit `MAX6675-linux-bitbang.c` to set GPIO chip and GPIO pins that are connected to your MAX6675 device. Default values corresponds to Raspbeery Pi 3B SPI pins.
2. Compile it via `cc -lgpiod MAX6675-linux-bitbang.c`
