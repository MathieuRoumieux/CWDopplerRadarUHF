#ifndef UTILS_H
#define	UTILS_H

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
#define CELERITY 300 //celerity of light in vacuum in megam/s
#define FCARRIER 433.92 //radar carrier frequency in MHz
#define FS 3300 //sampling frequency in Hz
#define NSAMPLE 4096 //number of adc samples
#define MAXSPEED 300 //max measurable speed in km/h
#define LOW_LVL 2000 //threshold of low level signal, found empirically that no signals max~= 800-1200, slow moving max~>2000

enum{
    STANDBY = 0,
    SAMPLING_START = 1,
    SAMPLING_ONGOING = 2,
    DATA_PROCESSING = 3,
    INHIB_STATE = 4
};    

extern char txBuff[256];
extern volatile uint8_t currentState;

void startTX();
void stopTX();
void dirtyDelay_ms(uint16_t ms);
float getSpeed(float freq);
uint16_t getMaxSpeedIndex();
uint16_t getBufferMaxIndex(uint16_t* buf, uint16_t len);
uint16_t getMax(uint16_t* buf, uint16_t len);
uint16_t getMean(uint16_t* buf, uint16_t len);
void printUart(const char* format, ...);
void printUartIntBuffer(uint16_t* buf, uint16_t len);
void uartInit();


#ifdef	__cplusplus
}
#endif

#endif	/* UTILS_H */