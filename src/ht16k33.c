#include "ht16k33.h"
#include "definitions.h"

void i2c7SegDriverDisplayOn()
{
    uint8_t displaySetupReg = 0x81;
    SERCOM1_I2C_Write(I2C_DRV_ADDRESS, &displaySetupReg, 1);
    while(SERCOM1_I2C_IsBusy());
}

void i2c7SegDriverDisplayOff()
{
    uint8_t displaySetupReg = 0x80;
    SERCOM1_I2C_Write(I2C_DRV_ADDRESS, &displaySetupReg, 1);
    while(SERCOM1_I2C_IsBusy());
}

//raw write for a single com (single 7 segment digit, tho it'd work with 16 segments total) as per dataseet page 14
void i2c7SegDriverRawWrite(uint8_t addr, uint16_t val)
{
    //from my understanding of the datasheet, to write data to the ram, you first have to send the address pointer
    //and then the corresponding value such that [slave address] -> [address pointer] -> [data]
    //each digit is 16bits but the data is 8bits, so splitting in two
    uint8_t ramRegLsb[2] = {addr, (uint8_t)(val & 0xFF)};
    uint8_t ramRegMsb[2] = {addr+1, (uint8_t)(val >> 8)};
    
    //sends config to i2c bus
    SERCOM1_I2C_Write(I2C_DRV_ADDRESS, ramRegLsb, 2);
    while(SERCOM1_I2C_IsBusy());
    SERCOM1_I2C_Write(I2C_DRV_ADDRESS, ramRegMsb, 2);
    while(SERCOM1_I2C_IsBusy());
}

//writes 4 dashes on the 7segment display at startup
void i2c7SegDriverDefaultWrite()
{
    i2c7SegDriverRawWrite(DIGIT1, DASH);
    i2c7SegDriverRawWrite(DIGIT2, DASH);
    i2c7SegDriverRawWrite(DIGIT3, DASH);
    i2c7SegDriverRawWrite(DIGIT4, DASH);
    
    //turns display on
    i2c7SegDriverDisplayOn();
}

void i2c7SegDriverLowSig()
{
    i2c7SegDriverRawWrite(DIGIT1, 0b00111000);
    i2c7SegDriverRawWrite(DIGIT2, 0b11011100);
    i2c7SegDriverRawWrite(DIGIT3, 0x00);
    i2c7SegDriverRawWrite(DIGIT4, 0b01101101);
    
    //turns display on
    i2c7SegDriverDisplayOn();
}

//clears whole display's ram
void i2c7SegDriverClear()
{
    for(uint8_t i = 0; i < 0x0F; i=i+2) //max address = 0x0F
    {
        i2c7SegDriverRawWrite(i, 0x0000);
    }
}

//converts a given digit (ie : 2) into a 16bit value corresponding to the display ROW routing
uint16_t i2c7SegDriverDigitConvert(uint8_t val)
{
    uint16_t digitArr[10] = {0x3F,  //0
                            0x06,   //1
                            0x5B,   //2
                            0x4F,   //3
                            0x66,   //4
                            0x6D,   //5
                            0x7D,   //6
                            0x07,   //7
                            0x7F,   //8
                            0x6F};  //9
    
    return digitArr[val];
}

bool i2c7SegDriverWrite(float val)
{
    uint16_t valBuff = (uint16_t)(val*10);
    uint8_t digitArr[4] = {0};
    
    //exits function if the value is more than 4 digits long
    if(val > 999.9)
    {
        i2c7SegDriverDefaultWrite();
        return false;
    }

    //extracts each digits into digitArr
    for(uint8_t i = 0; i < 4; i++)
    {
        digitArr[i] = valBuff % 10;
        valBuff /= 10;
    }
    
    //converts each digit into its 16bit ROW representation and writes it to the display
    i2c7SegDriverRawWrite(DIGIT1, i2c7SegDriverDigitConvert(digitArr[3]));
    i2c7SegDriverRawWrite(DIGIT2, i2c7SegDriverDigitConvert(digitArr[2]));
    i2c7SegDriverRawWrite(DIGIT3, i2c7SegDriverDigitConvert(digitArr[1]) | DP);
    i2c7SegDriverRawWrite(DIGIT4, i2c7SegDriverDigitConvert(digitArr[0]));
    
    //turns display on
    i2c7SegDriverDisplayOn();
    
    return true;
}

void i2c7SegDriverInit()
{
    //config as per the flowchart on the datasheet page 29
    
    //from my understanding of the datasheet, for the overall configuration of the driver, there's no register address such that a write command looks like [slave address] -> [register address] -> [register contents]
    //but rather a command system such that a write command looks like [slave address] -> [command code]
    uint8_t systemSetupReg = 0x21; //enables driver's system oscillator
    uint8_t rowIntReg = 0xA0; //sets INT/ROW output pin to ROW driver output
    uint8_t dimmingReg = 0xE7; //sets 7segment dimming to half value
    uint8_t displaySetupReg = 0x80; //keeps display off for the moment and disables blinking
    
    //sends config to i2c bus
    SERCOM1_I2C_Write(I2C_DRV_ADDRESS, &systemSetupReg, 1);
    while(SERCOM1_I2C_IsBusy());
    SERCOM1_I2C_Write(I2C_DRV_ADDRESS, &rowIntReg, 1);
    while(SERCOM1_I2C_IsBusy());
    SERCOM1_I2C_Write(I2C_DRV_ADDRESS, &dimmingReg, 1);
    while(SERCOM1_I2C_IsBusy());
    SERCOM1_I2C_Write(I2C_DRV_ADDRESS, &displaySetupReg, 1);
    while(SERCOM1_I2C_IsBusy());
    
    //clears whole display's ram
    i2c7SegDriverClear();
    
    //displays default value
    i2c7SegDriverDefaultWrite();
}