#include "ads1015.h"
#include "definitions.h"

volatile bool adcCallbackFlag = false;

void i2cAdcEnable()
{
    EIC_InterruptEnable(EIC_PIN_9);
}

void i2cAdcDisable()
{
    EIC_InterruptDisable(EIC_PIN_9);
}

int16_t i2cAdcRead()
{
    uint8_t conversionReg[2];
    uint16_t conversionRes = 0;
    
    //reads conversion register and returns 12bit data in 2s complement format
    SERCOM0_I2C_Read(I2C_ADC_ADDRESS, conversionReg, 2);
    while(SERCOM0_I2C_IsBusy());
    
    conversionRes = ((uint16_t)conversionReg[0] << 8) | conversionReg[1];
    
    return (int16_t)conversionRes; //result in float = [INT_RES]*(2^B/(2*FSR))
}

void i2cAdcCallback(uintptr_t context)
{
    adcCallbackFlag = true;
}

void i2cAdcInit()
{
    //hiThresh MSB set to 1 and loTresh MSB set to 0 to enable RDY feature
    uint8_t hiThreshReg[3] = {0x03, 0x80, 0x00};
    uint8_t loThreshReg[3] = {0x02, 0x00, 0x00};
    
    //N/A ; N/A ; FSR=+-2.048V ; MODE=Continuous ; FS = 3300Hz ; N/A ; RDY active low ; N/A ; RDY enabled
    uint8_t configReg[3] = {0x01, 0x04, 0xE0};
    
    //address of the conversion register : during a read operation, the adc returns the content of the last written register.
    //writing to it only once since it'll only be read from hereafter 
    uint8_t conversionRegAddr = 0x00;
    
    //sends config to i2c bus
    SERCOM0_I2C_Write(I2C_ADC_ADDRESS, hiThreshReg, 3);
    while(SERCOM0_I2C_IsBusy());  
    SERCOM0_I2C_Write(I2C_ADC_ADDRESS, loThreshReg, 3);
    while(SERCOM0_I2C_IsBusy());
    SERCOM0_I2C_Write(I2C_ADC_ADDRESS, configReg, 3);
    while(SERCOM0_I2C_IsBusy());
    SERCOM0_I2C_Write(I2C_ADC_ADDRESS, &conversionRegAddr, 1);
    while(SERCOM0_I2C_IsBusy());
    
    //defines IRQ on RDY pin
    EIC_CallbackRegister(EIC_PIN_9, i2cAdcCallback, 0);
    i2cAdcDisable();
}