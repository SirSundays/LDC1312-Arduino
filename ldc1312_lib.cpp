#include "ldc1312_lib.h"

void LDC_setRecommendedSettings(uint8_t address, int channel)
{
    uint8_t rcount_address = 0;
    uint8_t settlecount_address = 0;
    uint8_t clockdividers_address = 0;
    uint8_t drivecurrent_address = 0;
    uint16_t config_code = 0;

    /* CONFIG settings for active channel */
    switch(channel)
    {
        case 0:
            config_code = 0x1601; //0001 0110  0000 0001
            break;
        case 1:
            config_code = 0x5601; //0101 0110  0000 0001
            break;
        case 2:
            config_code = 0x9601; //1001 0110  0000 0001
            break;
        case 3:
            config_code = 0xD601; //1101 0110  0000 0001
            break;
        default:
            config_code = 0x1601; //defaults to channel 0 on invalid argument
    };

    //configure settings
    LDC_setConversionTime(address, channel, 0x04D6);
    LDC_setSettleTime(address, channel, 0x000A);
    LDC_setClockDividers(address, channel, 0x1002);
    LDC_setErrorConfig(address, 0x0000);
    LDC_setDriveCurrent(address, channel, 0x9000);
    LDC_setConfig(address, config_code);
}

uint16_t LDC_readRegister(uint8_t address, uint8_t register_address)
{
    uint16_t data = 0;

    Wire.beginTransmission(address); //I2C request code by Matthew Post
    Wire.write(register_address); //Data register for specified channel
    Wire.endTransmission();
    Wire.requestFrom(address, 2); //request 2 bytes
    while(Wire.available()) {     //read bytes
      data = data << 8;             //shift MSB to left 8 bits
      data = data | Wire.read();    //stop at LSB
    }

    return data;
}

void LDC_setRegister(uint8_t address, uint8_t register_address, uint16_t contents)
{
    uint8_t byte1 = (contents >> 8) & 0xFF; //MSB
    uint8_t byte0 = contents & 0xFF; //LSB
    Wire.beginTransmission(address);
    Wire.write(register_address);
    Wire.write(byte1);
    Wire.write(byte0);
    Wire.endTransmission();
}

uint16_t LDC_readData(uint8_t address, int channel)
{
    uint8_t register_address = 0;
    
    switch(channel)
    {
        case 0:
            register_address = LDC_DATA0;
            break;
        case 1:
            register_address = LDC_DATA1;
            break;
        case 2:
            register_address = LDC_DATA2;
            break;
        case 3:
            register_address = LDC_DATA3;
            break;
        default:
            register_address = LDC_DATA0; //defaults to DATA0 on invalid argument
    };

    uint16_t data = 0;

    Wire.beginTransmission(address); //I2C request code by Matthew Post
    Wire.write(register_address); //Data register for specified channel
    Wire.endTransmission();
    Wire.requestFrom(address, 2); //request 2 bytes
    while(Wire.available()) {     //read bytes
      data = data << 8;             //shift MSB to left 8 bits
      data = data | Wire.read();    //stop at LSB
    }

    return data;
}

uint16_t LDC_readManufacturerID(uint8_t address)
{
    uint8_t data = 0;
    
    Wire.beginTransmission(address);
    Wire.write(LDC_MANUFACTURER_ID); //Manufacturer ID register
    Wire.endTransmission();
    Wire.requestFrom(address, 2); //request 2 bytes
    while(Wire.available()) {     //read bytes
      data = data << 8;             //shift MSB to left 8 bits
      data = data | Wire.read();    //stop at LSB
    }

    return data;
}

uint16_t LDC_readDeviceID(uint8_t address)
{
    uint8_t data = 0;
    
    Wire.beginTransmission(address);
    Wire.write(LDC_DEVICE_ID); //Device ID register
    Wire.endTransmission();
    Wire.requestFrom(address, 2); //request 2 bytes
    while(Wire.available()) {     //read bytes
      data = data << 8;             //shift MSB to left 8 bits
      data = data | Wire.read();    //stop at LSB
    }

    return data;
}

uint16_t LDC_readStatus(uint8_t address)
{
    uint16_t data = 0;

    Wire.beginTransmission(address);
    Wire.write(LDC_STATUS); //Data register for Status bytes
    Wire.endTransmission();
    Wire.requestFrom(address, 2); //request 2 bytes
    while(Wire.available()) {     //read bytes
      data = data << 8;             //shift MSB to left 8 bits
      data = data | Wire.read();    //stop at LSB
    }

    return data;
}

bool LDC_hasNewData(int channel, uint16_t status_result)
{
    switch(channel)
    {
        case 0:
            return ((status_result & 0x0008) == 0x0008); //0000 0000  0000 1000 - UNREADCONV0 bit
        case 1:
            return ((status_result & 0x0004) == 0x0004); //0000 0000  0000 0100 - UNREADCONV1 bit
        case 2:
            return ((status_result & 0x0002) == 0x0002); //0000 0000  0000 0010 - UNREADCONV2 bit
        case 3:
            return ((status_result & 0x0001) == 0x0001); //0000 0000  0000 0001 - UNREADCONV3 bit
        default:
            return ((status_result & 0x0008) == 0x0008); //0000 0000  0000 1000 - UNREADCONV0 bit
                                                         //defaults to channel 0 on invalid argument
    };
}

bool LDC_hasNewData(uint8_t address, int channel)
{
    return LDC_hasNewData(channel, LDC_readStatus(address));
}

bool LDC_hasConversionErrors(uint16_t status_result)
{ 
    return (status_result & 0x3F00) != 0x0000; //error bits: 0011 1111 0000 0000
}

uint16_t LDC_getChannelWithErrors(uint16_t status_result)
{
    return (status_result >> 14) & 0x0003; //shift right 14 bits and & w/ 0000 0000 0000 0011
}

void LDC_resetLDC(uint8_t address)
{ //Writes b1 to MSB of RESET_DEV
    Wire.beginTransmission(address);
    Wire.write(LDC_RESET_DEV);
    Wire.write(0x80); //1000 0000
    Wire.write(0x00);
    Wire.endTransmission();
}

bool LDC_setGain(uint8_t address, int gain)
{ //writes to OUTPUT_GAIN [ 10:9 ] in RESET_DEV register (the rest is reserved)
    uint8_t out_byte = 0;
    switch(gain)
    {
        case 0: //1x gain
            out_byte = 0x00; //00000 00 0
            break;
        case 1: //4x gain
            out_byte = 0x02; //00000 01 0
            break;
        case 2: //8x gain
            out_byte = 0x04; //00000 10 0
            break;
        case 3: //16x gain
            out_byte = 0x06; //00000 11 0 
            break;
        default:
            return false; //return false on invalid gain argument
    }
    Wire.beginTransmission(address);
    Wire.write(LDC_RESET_DEV);
    Wire.write(out_byte);
    Wire.endTransmission();
    return true;
}

uint16_t LDC_readGain(uint8_t address)
{
    uint16_t data = 0;

    Wire.beginTransmission(address);
    Wire.write(LDC_RESET_DEV); //register for specified channel
    Wire.endTransmission();
    Wire.requestFrom(address, 2); //request 2 bytes
    while(Wire.available()) {     //read bytes
      data = data << 8;             //shift MSB to left 8 bits
      data = data | Wire.read();    //stop at LSB
    }

    return (data >> 9) & 0x0003;//shift right 9 bits and & w/ 0000 0000  0000 0011
}

uint16_t LDC_readResetDev(uint8_t address)
{
    uint16_t data = 0;

    Wire.beginTransmission(address);
    Wire.write(LDC_RESET_DEV); //register for specified channel
    Wire.endTransmission();
    Wire.requestFrom(address, 2); //request 2 bytes
    while(Wire.available()) {     //read bytes
      data = data << 8;             //shift MSB to left 8 bits
      data = data | Wire.read();    //stop at LSB
    }

    return data;
}

void LDC_setResetDev(uint8_t address, uint16_t config_code)
{
    uint8_t byte1 = (config_code >> 8) & 0xFF; //MSB
    uint8_t byte0 = config_code & 0xFF; //LSB
    Wire.beginTransmission(address);
    Wire.write(LDC_RESET_DEV);
    Wire.write(byte1);
    Wire.write(byte0);
    Wire.endTransmission();
}

void LDC_setOffset(uint8_t address, int channel, uint16_t offset)
{
    uint8_t register_address = 0;

    switch(channel)
    {
        case 0:
            register_address = LDC_OFFSET0;
            break;
        case 1:
            register_address = LDC_OFFSET1;
            break;
        case 2:
            register_address = LDC_OFFSET2;
            break;
        case 3:
            register_address = LDC_OFFSET3;
            break;
        default:
            register_address = LDC_OFFSET0; //defaults to channel 0 on invalid argument
    };

    uint8_t byte1 = (offset >> 8) & 0xFF; //MSB
    uint8_t byte0 = offset & 0xFF; //LSB
    Wire.beginTransmission(address);
    Wire.write(register_address);
    Wire.write(byte1);
    Wire.write(byte0);
    Wire.endTransmission();
}

uint16_t LDC_readOffset(uint8_t address, int channel)
{
    uint8_t register_address = 0;

    switch(channel)
    {
        case 0:
            register_address = LDC_OFFSET0;
            break;
        case 1:
            register_address = LDC_OFFSET1;
            break;
        case 2:
            register_address = LDC_OFFSET2;
            break;
        case 3:
            register_address = LDC_OFFSET3;
            break;
        default:
            register_address = LDC_OFFSET0; //defaults to channel 0 on invalid argument
    };

    uint16_t data = 0;

    Wire.beginTransmission(address);
    Wire.write(register_address); //register for specified channel
    Wire.endTransmission();
    Wire.requestFrom(address, 2); //request 2 bytes
    while(Wire.available()) {     //read bytes
      data = data << 8;             //shift MSB to left 8 bits
      data = data | Wire.read();    //stop at LSB
    }

    return data;
}

void LDC_setConversionTime(uint8_t address, int channel, uint16_t conversion_count)
{
    uint8_t register_address = 0;

    switch(channel)
    {
        case 0:
            register_address = LDC_RCOUNT0;
            break;
        case 1:
            register_address = LDC_RCOUNT1;
            break;
        case 2:
            register_address = LDC_RCOUNT2;
            break;
        case 3:
            register_address = LDC_RCOUNT3;
            break;
        default:
            register_address = LDC_RCOUNT0; //defaults to channel 0 on invalid argument
    };

    uint8_t byte1 = (conversion_count >> 8) & 0xFF; //MSB
    uint8_t byte0 = conversion_count & 0xFF; //LSB
    Wire.beginTransmission(address);
    Wire.write(register_address);
    Wire.write(byte1);
    Wire.write(byte0);
    Wire.endTransmission();
}

uint16_t LDC_readConversionTime(uint8_t address, int channel)
{
    uint8_t register_address = 0;

    switch(channel)
    {
        case 0:
            register_address = LDC_RCOUNT0;
            break;
        case 1:
            register_address = LDC_RCOUNT1;
            break;
        case 2:
            register_address = LDC_RCOUNT2;
            break;
        case 3:
            register_address = LDC_RCOUNT3;
            break;
        default:
            register_address = LDC_RCOUNT0; //defaults to channel 0 on invalid argument
    };

    uint16_t data = 0;

    Wire.beginTransmission(address);
    Wire.write(register_address); //register for specified channel
    Wire.endTransmission();
    Wire.requestFrom(address, 2); //request 2 bytes
    while(Wire.available()) {     //read bytes
      data = data << 8;             //shift MSB to left 8 bits
      data = data | Wire.read();    //stop at LSB
    }

    return data;
}

void LDC_setRCount(uint8_t address, int channel, uint16_t conversion_count)
{
    uint8_t register_address = 0;

    switch(channel)
    {
        case 0:
            register_address = LDC_RCOUNT0;
            break;
        case 1:
            register_address = LDC_RCOUNT1;
            break;
        case 2:
            register_address = LDC_RCOUNT2;
            break;
        case 3:
            register_address = LDC_RCOUNT3;
            break;
        default:
            register_address = LDC_RCOUNT0; //defaults to channel 0 on invalid argument
    };

    uint8_t byte1 = (conversion_count >> 8) & 0xFF; //MSB
    uint8_t byte0 = conversion_count & 0xFF; //LSB
    Wire.beginTransmission(address);
    Wire.write(register_address);
    Wire.write(byte1);
    Wire.write(byte0);
    Wire.endTransmission();
}

uint16_t LDC_readRCount(uint8_t address, int channel)
{
    uint8_t register_address = 0;

    switch(channel)
    {
        case 0:
            register_address = LDC_RCOUNT0;
            break;
        case 1:
            register_address = LDC_RCOUNT1;
            break;
        case 2:
            register_address = LDC_RCOUNT2;
            break;
        case 3:
            register_address = LDC_RCOUNT3;
            break;
        default:
            register_address = LDC_RCOUNT0; //defaults to channel 0 on invalid argument
    };

    uint16_t data = 0;

    Wire.beginTransmission(address);
    Wire.write(register_address); //register for specified channel
    Wire.endTransmission();
    Wire.requestFrom(address, 2); //request 2 bytes
    while(Wire.available()) {     //read bytes
      data = data << 8;             //shift MSB to left 8 bits
      data = data | Wire.read();    //stop at LSB
    }

    return data;
}

void LDC_setClockDividers(uint8_t address, int channel, uint16_t config_code)
{
    uint8_t register_address = 0;

    switch(channel)
    {
        case 0:
            register_address = LDC_CLOCK_DIVIDERS0;
            break;
        case 1:
            register_address = LDC_CLOCK_DIVIDERS1;
            break;
        case 2:
            register_address = LDC_CLOCK_DIVIDERS2;
            break;
        case 3:
            register_address = LDC_CLOCK_DIVIDERS3;
            break;
        default:
            register_address = LDC_CLOCK_DIVIDERS0; //defaults to channel 0 on invalid argument
    };

    uint8_t byte1 = (config_code >> 8) & 0xFF; //MSB
    uint8_t byte0 = config_code & 0xFF; //LSB
    Wire.beginTransmission(address);
    Wire.write(register_address);
    Wire.write(byte1);
    Wire.write(byte0);
    Wire.endTransmission();
}

void LDC_setClockDividers(uint8_t address, int channel, uint16_t input_divider, uint16_t reference_divider)
{
    input_divider = (input_divider << 11) & 0xF000; //shift to the left and & w/ 1111 0000  0000 0000
    reference_divider = reference_divider & 0x03FF; // & w/ 0000 0011  1111 1111

    uint16_t config_code = input_divider | reference_divider; //combine arguments

    LDC_setClockDividers(address, channel, config_code); //send to LDC1312
}

uint16_t LDC_readClockDividers(uint8_t address, int channel)
{
    uint8_t register_address = 0;

    switch(channel)
    {
        case 0:
            register_address = LDC_CLOCK_DIVIDERS0;
            break;
        case 1:
            register_address = LDC_CLOCK_DIVIDERS1;
            break;
        case 2:
            register_address = LDC_CLOCK_DIVIDERS2;
            break;
        case 3:
            register_address = LDC_CLOCK_DIVIDERS3;
            break;
        default:
            register_address = LDC_CLOCK_DIVIDERS0; //defaults to channel 0 on invalid argument
    };

    uint16_t data = 0;

    Wire.beginTransmission(address);
    Wire.write(register_address); //CLOCK_DIVIDERSx register for specified channel
    Wire.endTransmission();
    Wire.requestFrom(address, 2); //request 2 bytes
    while(Wire.available()) {     //read bytes
      data = data << 8;             //shift MSB to left 8 bits
      data = data | Wire.read();    //stop at LSB
    }

    return data;
}

uint16_t LDC_getInputDivider(uint16_t config_code)
{
    return (config_code >> 12) & 0x000F; //shift right 12 bits and & w/ 0000 0000  0000 1111
}

uint16_t LDC_getReferenceDivider(uint16_t config_code)
{
    return config_code & 0x03FF; //return bits 9:0 0000 0011 1111 1111
}

void LDC_setSettleTime(uint8_t address, int channel, uint16_t settle_count)
{
    uint8_t register_address = 0;

    switch(channel)
    {
        case 0:
            register_address = LDC_SETTLECOUNT0;
            break;
        case 1:
            register_address = LDC_SETTLECOUNT1;
            break;
        case 2:
            register_address = LDC_SETTLECOUNT2;
            break;
        case 3:
            register_address = LDC_SETTLECOUNT3;
            break;
        default:
            register_address = LDC_SETTLECOUNT0; //defaults to channel 0 on invalid argument
    };

    uint8_t byte1 = (settle_count >> 8) & 0xFF; //MSB
    uint8_t byte0 = settle_count & 0xFF; //LSB
    Wire.beginTransmission(address);
    Wire.write(register_address);
    Wire.write(byte1);
    Wire.write(byte0);
    Wire.endTransmission();
}

uint16_t LDC_readSettleTime(uint8_t address, int channel)
{
    uint8_t register_address = 0;

    switch(channel)
    {
        case 0:
            register_address = LDC_SETTLECOUNT0;
            break;
        case 1:
            register_address = LDC_SETTLECOUNT1;
            break;
        case 2:
            register_address = LDC_SETTLECOUNT2;
            break;
        case 3:
            register_address = LDC_SETTLECOUNT3;
            break;
        default:
            register_address = LDC_SETTLECOUNT0; //defaults to channel 0 on invalid argument
    };

    uint16_t data = 0;

    Wire.beginTransmission(address);
    Wire.write(register_address); //register for specified channel
    Wire.endTransmission();
    Wire.requestFrom(address, 2); //request 2 bytes
    while(Wire.available()) {     //read bytes
      data = data << 8;             //shift MSB to left 8 bits
      data = data | Wire.read();    //stop at LSB
    }

    return data;
}

void LDC_setConfig(uint8_t address, uint16_t config_code)
{
    uint8_t byte1 = (config_code >> 8) & 0xFF; //MSB
    uint8_t byte0 = config_code & 0xFF; //LSB
    Wire.beginTransmission(address);
    Wire.write(LDC_CONFIG);
    Wire.write(byte1);
    Wire.write(byte0);
    Wire.endTransmission();
}

uint16_t LDC_readConfig(uint8_t address)
{
    uint16_t data = 0;

    Wire.beginTransmission(address);
    Wire.write(LDC_CONFIG); //Data register for Status bytes
    Wire.endTransmission();
    Wire.requestFrom(address, 2); //request 2 bytes
    while(Wire.available()) {     //read bytes
      data = data << 8;             //shift MSB to left 8 bits
      data = data | Wire.read();    //stop at LSB
    }

    return data;
}

int LDC_getActiveChannel(uint16_t config_code)
{
    return (config_code >> 14) & 0x0003; //shift right 14 bits and & w/ 0000 0000  0000 0011
                                         //Active channel bits @ 15:14
}

bool LDC_isSleeping(uint16_t config_code)
{
    return (config_code & 0x2000) == 0x2000; //0010 0000  0000 0000
}

bool LDC_isCurrentOverrideEnabled(uint16_t config_code)
{
    return (config_code & 0x1000) == 0x1000; //0001 0000  0000 0000
}

bool LDC_isLowPowerModeEnabled(uint16_t config_code)
{
    return (config_code & 0x0800) == 0x0800; //0000 1000  0000 0000
}

bool LDC_isAutoAmplitudeDisabled(uint16_t config_code)
{
    return (config_code & 0x0400) == 0x0400; //0000 0100  0000 0000
}

bool LDC_hasExternalOscillator(uint16_t config_code)
{
    return (config_code & 0x0200) == 0x0200; //0000 0010  0000 0000
}

bool LDC_isInterruptDisabled(uint16_t config_code)
{
    return (config_code & 0x0080) == 0x0080; //0000 0000  1000 0000
}

bool LDC_isHighCurrentDriveEnabled(uint16_t config_code)
{
    return (config_code & 0x0020) == 0x0020; //0000 0000  0010 0000
}

void LDC_setErrorConfig(uint8_t address, uint16_t config_code)
{
    uint8_t byte1 = (config_code >> 8) & 0xFF; //MSB
    uint8_t byte0 = config_code & 0xFF; //LSB
    Wire.beginTransmission(address);
    Wire.write(LDC_ERROR_CONFIG);
    Wire.write(byte1);
    Wire.write(byte0);
    Wire.endTransmission();
}

uint16_t LDC_readErrorConfig(uint8_t address)
{
    uint16_t data = 0;

    Wire.beginTransmission(address);
    Wire.write(LDC_ERROR_CONFIG); //Data register for Status bytes
    Wire.endTransmission();
    Wire.requestFrom(address, 2); //request 2 bytes
    while(Wire.available()) {     //read bytes
      data = data << 8;             //shift MSB to left 8 bits
      data = data | Wire.read();    //stop at LSB
    }

    return data;
}

void LDC_setMUXConfig(uint8_t address, uint16_t config_code)
{
    uint8_t byte1 = (config_code >> 8) & 0xFF; //MSB
    uint8_t byte0 = config_code & 0xFF; //LSB
    Wire.beginTransmission(address);
    Wire.write(LDC_MUX_CONFIG);
    Wire.write(byte1);
    Wire.write(byte0);
    Wire.endTransmission();
}

uint16_t LDC_readMUXConfig(uint8_t address)
{
    uint16_t data = 0;

    Wire.beginTransmission(address);
    Wire.write(LDC_MUX_CONFIG);
    Wire.endTransmission();
    Wire.requestFrom(address, 2); //request 2 bytes
    while(Wire.available()) {     //read bytes
      data = data << 8;             //shift MSB to left 8 bits
      data = data | Wire.read();    //stop at LSB
    }

    return data;
}

bool LDC_isAutoscanEnabled(uint16_t config_code)
{
    return (config_code & 0x8000) == 0x8000; //1000 0000  0000 0000
}

uint16_t LDC_getAutoscanSequence(uint16_t config_code)
{
    return (config_code >> 13) & 0x0003; //shift right 13 bits and & w/ 0000 0000  0000 0011
}

uint16_t LDC_getDeglitchBandwidth(uint16_t config_code)
{
    return config_code & 0x0007; //return last 3 bits 0000 0000  0000 0111
}

void LDC_setDriveCurrent(uint8_t address, int channel, uint16_t drive_current)
{
    uint8_t register_address = 0;

    switch(channel)
    {
        case 0:
            register_address = LDC_DRIVE_CURRENT0;
            break;
        case 1:
            register_address = LDC_DRIVE_CURRENT1;
            break;
        case 2:
            register_address = LDC_DRIVE_CURRENT2;
            break;
        case 3:
            register_address = LDC_DRIVE_CURRENT3;
            break;
        default:
            register_address = LDC_DRIVE_CURRENT0; //defaults to channel 0 on invalid argument
    };

    uint8_t byte1 = (drive_current >> 8) & 0xFF; //MSB
    uint8_t byte0 = drive_current & 0xFF; //LSB

    Wire.beginTransmission(address);
    Wire.write(register_address);
    Wire.write(byte1);
    Wire.write(byte0);
    Wire.endTransmission();
}

uint16_t LDC_readDriveCurrent(uint8_t address, int channel)
{
    uint8_t register_address = 0;

    switch(channel)
    {
        case 0:
            register_address = LDC_DRIVE_CURRENT0;
            break;
        case 1:
            register_address = LDC_DRIVE_CURRENT1;
            break;
        case 2:
            register_address = LDC_DRIVE_CURRENT2;
            break;
        case 3:
            register_address = LDC_DRIVE_CURRENT3;
            break;
        default:
            register_address = LDC_DRIVE_CURRENT0; //defaults to channel 0 on invalid argument
    };

    uint16_t data = 0;

    Wire.beginTransmission(address);
    Wire.write(register_address);
    Wire.endTransmission();
    Wire.requestFrom(address, 2); //request 2 bytes
    while(Wire.available()) {     //read bytes
      data = data << 8;             //shift MSB to left 8 bits
      data = data | Wire.read();    //stop at LSB
    }

    return data;
}

uint16_t LDC_getDriveCurrent(uint8_t address, int channel)
{
    return (LDC_readDriveCurrent(address, channel) >> 11) & 0x001F; //shift right 11 bits and & w/ 0000 0000 0001 1111
}

uint16_t LDC_getInitialDriveCurrent(uint8_t address, int channel)
{
    return (LDC_readDriveCurrent(address, channel) >> 6) & 0x001F; //shift right 6 bits and & w/ 0000 0000 0001 1111
}