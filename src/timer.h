#ifndef TIMER_H
#define	TIMER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
#define SATURATION_DELAY 1 //time it takes for the LF amp to exist saturation regime in seconds

extern volatile bool saturationTimerFlag;
extern volatile uint16_t saturationTimerCounter;
extern volatile bool inhibTimerCallbackFlag;
extern volatile uint16_t inhibTimerCounter;
extern volatile bool isTxInhibited;

void saturationTimerStart();
void saturationTimerStop();
void saturationTimerCallback(uint32_t status, uintptr_t context);
void inhibTimerStart();
void inhibTimerStop();
void inhibTimerCallback(uint32_t status, uintptr_t context);
void timerInit();

#ifdef	__cplusplus
}
#endif

#endif	/* TIMER_H */