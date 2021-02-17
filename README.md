# LDC1312-Arduino
Arduino library for TI's LDC1312/1314 inductance-to-digital converters.

This library has not been fully tested (yet). I have used most of the functions in my projects, but I can't guarantee that everything will work as it should.

This library is written for the Arduino Pro Micro board, but can probably be reused for other Arduino boards with minimal changes.

This library is set to only use Wire. Not Wire1, Wire2, etc. You can easily change this by adding '1', '2', etc to all instances of the word "Wire".

This library works for both the LDC1312 and LDC1314 sensors - if you're using an LDC1312, just avoid using channels 2 and 3 because the LDC1312 doesn't have those.

Most of the channel-specific functions default to Channel 0 if they're given an invalid channel argument.

LDC1312 datasheet: https://www.ti.com/lit/ds/symlink/ldc1312.pdf

An overview with all the functions will follow shortly.
