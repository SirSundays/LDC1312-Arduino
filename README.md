# LDC1312-Arduino
Arduino library for TI's LDC1312/1314 inductance-to-digital converters.

This library has not been fully tested (yet). I have used most of the functions in my projects, but I can't guarantee that everything will work as it should.

This library is written specifically for the Teensy 3.5 Arduino board, but can probably be reused for other Arduino boards with minimal changes.

This library works for both the LDC1312 and LDC1314 sensors - if you're using an LDC1312, just avoid using channels 2 and 3 because the LDC1312 doesn't have those.

This library does not configure the Arduino's I2C functions for you. You will need to use something like:
```
Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000);
```

Most of the channel-specific functions default to Channel 0 if they're given an invalid channel argument.

Created by Darius Fieschko.

LDC1312 datasheet: https://www.ti.com/lit/ds/symlink/ldc1312.pdf
