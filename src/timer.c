#include "timer.h"
#include "utils.h"
#include "definitions.h"

volatile bool saturationTimerFlag = false;
volatile uint16_t saturationTimerCounter = 0;
volatile bool inhibTimerCallbackFlag = false;
volatile uint16_t inhibTimerCounter = 0;
volatile bool isTxInhibited = false;

void saturationTimerStart()
{
    TCC0_TimerStart();
}

void saturationTimerStop()
{
    TCC0_TimerStop();
}

void saturationTimerCallback(uint32_t status, uintptr_t context)
{
    saturationTimerCounter++;
    
    if(saturationTimerCounter >= SATURATION_DELAY)
    {
        saturationTimerStop();
        saturationTimerCounter = 0;
        saturationTimerFlag = true;
    }
}

void inhibTimerStart()
{
    isTxInhibited = true;
    TCC2_TimerStart();
}

void inhibTimerStop()
{
    TCC2_TimerStop();
}

void inhibTimerCallback(uint32_t status, uintptr_t context)
{
    inhibTimerCallbackFlag = true;
    inhibTimerCounter++;
    
    if(inhibTimerCounter >= (uint8_t)(SATURATION_DELAY+(NSAMPLE/FS)+1)*10) //inhibits TX to follow <10% duty cycle regulation
    {
        inhibTimerStop();
        inhibTimerCounter = 0;
        isTxInhibited = false;
    }
}

void timerInit()
{
    TCC0_TimerCallbackRegister(saturationTimerCallback, 0);
    TCC2_TimerCallbackRegister(inhibTimerCallback, 0);
}