#ifndef ADS1015_H
#define	ADS1015_H

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define I2C_ADC_ADDRESS 0b1001000
    
extern volatile bool adcCallbackFlag;

void i2cAdcEnable();
void i2cAdcDisable();
int16_t i2cAdcRead();
void i2cAdcCallback(uintptr_t context);
void i2cAdcInit();


#ifdef	__cplusplus
}
#endif

#endif	/* ADS1015_H */