#include "utils.h"
#include "definitions.h"

char txBuff[256];
volatile uint8_t currentState = STANDBY;

void startTX()
{
    TX_EN_Clear();
    USR_LED_Set();
}

void stopTX()
{
    TX_EN_Set();
    USR_LED_Clear();
}

void dirtyDelay_ms(uint16_t ms)
{
    for(uint16_t i = 0; i < ms; i++)
    {
        for(uint32_t j = 0; j < 12000; j++)
        {
            __NOP();
        }
    }
}

float getSpeed(float freq) //returns speed from doppler shift in km/h
{
    return (float)(3.6*CELERITY*freq/(2*FCARRIER));
}

uint16_t getMaxSpeedIndex() //returns index of the maximum speed in the fft buffer, allows some degree of optimization
{
    return (uint16_t)((2*MAXSPEED*FCARRIER/(CELERITY*3.6))*((float)NSAMPLE/FS));
}

uint16_t getBufferMaxIndex(uint16_t* buf, uint16_t len)
{
    uint16_t max = 0;
    uint16_t res = 0;
    
    for(uint16_t i = 1; i < len; i++) //starts at 1 to discard dc
    {
        if(buf[i] > max)
        {
            max = buf[i];
            res = i;
        }
    }
    
    return res;
}

uint16_t getMax(uint16_t* buf, uint16_t len)
{
    uint16_t max = 0;
    
    for(uint16_t i = 1; i < len; i++) //starts at 1 to discard dc
    {
        if(buf[i] > max)
        {
            max = buf[i];
        }
    }
    
    return max;
}

uint16_t getMean(uint16_t* buf, uint16_t len)
{
    uint64_t res = 0;
    
    for(uint16_t i = 0; i < len; i++)
    {
        res += buf[i];
    }
    
    return (uint32_t)(res/len);
}

void printUart(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(txBuff, sizeof(txBuff), format, args);
    va_end(args);
    
    SERCOM2_USART_Write(txBuff, strlen(txBuff));
    while(SERCOM2_USART_WriteIsBusy());
}

void printUartIntBuffer(uint16_t* buf, uint16_t len)
{
    printUart("BUFFER CONTENTS :\r\n");
    for(uint16_t i=0; i<len; i++)
    {
        printUart("%d,", buf[i]);
        dirtyDelay_ms(1);
    }
    printUart("\r\n");
}

void uartInit()
{
    SERCOM2_USART_Enable();
    SERCOM2_USART_TransmitterEnable();
}