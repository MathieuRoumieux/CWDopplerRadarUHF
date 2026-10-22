#ifndef HT16K33_H
#define	HT16K33_H

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
#define I2C_DRV_ADDRESS 0b1110000
#define DP 0x80
#define DASH 0x40
    
enum {DIGIT1 = 0x00,
    DIGIT2 = 0x02,
    DIGIT3 = 0x04,
    DIGIT4 = 0x06};

void i2c7SegDriverDisplayOn();
void i2c7SegDriverDisplayOff();
void i2c7SegDriverRawWrite(uint8_t addr, uint16_t val);
void i2c7SegDriverDefaultWrite();
void i2c7SegDriverLowSig();
void i2c7SegDriverClear();
uint16_t i2c7SegDriverDigitConvert(uint8_t val);
bool i2c7SegDriverWrite(float val);
void i2c7SegDriverInit();

#ifdef	__cplusplus
}
#endif

#endif	/* HT16K33_H */