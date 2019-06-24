# LDC1312-Arduino
Arduino library for TI's LDC1312/1314 inductance-to-digital converters.

This library has not been fully tested (yet). I have used most of the functions in my projects, but I can't guarantee that everything will work as it should. I have *only* ever used this library with [Visual Studio Code](https://code.visualstudio.com/) using [PlatformIO](https://docs.platformio.org/en/latest/ide/vscode.html).

This library is written specifically for the Teensy 3.5 Arduino board, but can probably be reused for other Arduino boards with minimal changes. The main compatibility issue with non-Teensy boards is this library's usage of [i2c_t3](https://github.com/nox771/i2c_t3). I might come back and make this library more friendly to other Arduino devices later (probably not).

This library is set to only use Wire. Not Wire1, Wire2, etc. You can easily change this by adding '1', '2', etc to all instances of the word "Wire".

This library works for both the LDC1312 and LDC1314 sensors - if you're using an LDC1312, just avoid using channels 2 and 3 because the LDC1312 doesn't have those.

This library does not configure the Arduino's I2C functions for you. You will need to use something like:
```
Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000);
```

Most of the channel-specific functions default to Channel 0 if they're given an invalid channel argument.

Created by Darius Fieschko.

LDC1312 datasheet: https://www.ti.com/lit/ds/symlink/ldc1312.pdf
