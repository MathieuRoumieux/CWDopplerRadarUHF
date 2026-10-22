#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <math.h>

#include "definitions.h"                // SYS function prototypes
#include "fft.h"
#include "utils.h"
#include "ads1015.h"
#include "ht16k33.h"
#include "timer.h"
#include "irq.h"

int sampleBuffer[NSAMPLE];
volatile uint16_t sampleIndex = 0;
double complex fftBufferRaw[NSAMPLE];
uint16_t fftBuffer[(uint16_t)(NSAMPLE/2)];
float bufMaxIndex = 0.0f;
float bufMax = 0.0f;

int main ( void )
{
    //init
    SYS_Initialize ( NULL );
    stopTX();
    uartInit();
    i2c7SegDriverInit();
    i2cAdcInit();
    timerInit();
    irqInit();
    
    //init message
    printUart("RST\r\n");

    //main while loop
    while ( true )
    {
        SYS_Tasks ( );
        
        //state machine
        switch(currentState)
        {
            //waits for user to press push button and starts TX timer (allows LF amp to exit saturation regime)
            case STANDBY:
                if(irqCallbackFlag == true && isTxInhibited == false)
                {
                    printUart("Push button pressed\r\n");
                    irqCallbackFlag = false;

                    startTX();
                    saturationTimerStart();
                    i2c7SegDriverDefaultWrite();
                    
                    currentState = SAMPLING_START;
                }
                break;
                
            //enables adc once timer overflows
            case SAMPLING_START:
                if(saturationTimerFlag == true)
                {
                    printUart("Start sampling\r\n");
                    saturationTimerFlag = false;

                    i2cAdcEnable();
                    
                    currentState = SAMPLING_ONGOING;
                }
                break;
            
            //adc sampling is done here
            case SAMPLING_ONGOING:
                if(adcCallbackFlag == true) //enters here everytime a new sample has been gathered by the adc and stores it in an array
                {
                    adcCallbackFlag = false;

                    sampleBuffer[sampleIndex] = i2cAdcRead();
                    sampleIndex++;

                    if(sampleIndex%(uint16_t)(NSAMPLE/20) == 0) //blinks LED
                    {
                        USR_LED_Toggle();
                    }

                    if(sampleIndex >= NSAMPLE)
                    {
                        i2cAdcDisable();
                        sampleIndex = 0;
                        
                        currentState = DATA_PROCESSING;
                    }
                }
                break;
                
            //stops TX and computes FFT
            case DATA_PROCESSING:
                printUart("Stop sampling\r\n");
                stopTX();

                //fft computation
                fft(sampleBuffer, fftBufferRaw, NSAMPLE); //computes fft, radix2
                for(uint16_t i = 0; i < getMaxSpeedIndex(); i++) //discards DC, negative frequencies and out of reach speeds
                {
                    fftBuffer[i] = (uint16_t)cabs(fftBufferRaw[i+1]/NSAMPLE); //gets magnitude and casts to int
                }
                bufMaxIndex = (getBufferMaxIndex(fftBuffer, getMaxSpeedIndex())+1)*((float)FS/NSAMPLE);
                bufMax = getMax(fftBuffer, getMaxSpeedIndex());

                //prints max frequency found in the fft
                i2c7SegDriverWrite(getSpeed(bufMaxIndex));
                printUart("%.2f Hz = %.1f km/h\r\n", bufMaxIndex, getSpeed(bufMaxIndex));

                currentState = INHIB_STATE;
                break;
                
            //inhibits TX to follow <10% duty cycle regulation
            case INHIB_STATE:
                inhibTimerStart();
                USR_LED_Set();
                while(isTxInhibited)
                {
                    //if signal is too faint, flashes a "Low Signal" warning on the 7seg display while TX is inhibited
                    if(bufMax <= LOW_LVL && inhibTimerCallbackFlag == true)
                    {
                        inhibTimerCallbackFlag = false;
                        if(inhibTimerCounter%2 == 0)
                        {
                            i2c7SegDriverLowSig();
                        }
                        else
                        {
                            i2c7SegDriverWrite(getSpeed(bufMaxIndex));
                        }
                    }
                    irqCallbackFlag = false; //callback flag is asynchronous, without this line it'll immediatly resample if the push button is pressed during the inhib_state
                }
                USR_LED_Clear();
                i2c7SegDriverDefaultWrite();
                
                currentState = STANDBY;
                break;
                
            default:
                break;
        }
    }
    return ( EXIT_FAILURE );
}

/*#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <math.h>

#include "definitions.h"                // SYS function prototypes
#include "fft.h"
#include "utils.h"
#include "ads1015.h"
#include "ht16k33.h"
#include "timer.h"
#include "irq.h"

volatile bool adcFlag = false;
int sampleBuffer[NSAMPLE];
volatile uint16_t sampleIndex = 0;
double complex fftBufferRaw[NSAMPLE];
uint16_t fftBuffer[(uint16_t)(NSAMPLE/2)];
float bufMaxIndex = 0.0f;
float bufMax = 0.0f;

int main ( void )
{
    SYS_Initialize ( NULL );
    stopTX();
    uartInit();
    i2c7SegDriverInit();
    i2cAdcInit();
    timerInit();
    irqInit();
    
    printUart("RST\r\n");

    while ( true )
    {
        SYS_Tasks ( );
        
        //waits for user to press push button and starts TX timer (allows LF amp to exit saturation regime)
        if(irqCallbackFlag == true && isTxInhibited == false)
        {
            printUart("Push button pressed\r\n");
            irqCallbackFlag = false;
            
            startTX();
            TCC0_TimerStart();
            i2c7SegDriverDefaultWrite();
        }
        
        //enables adc once timer overflows
        if(saturationTimerFlag == true)
        {
            printUart("Start sampling\r\n");
            saturationTimerFlag = false;
            
            i2cAdcEnable();
        }
        
        //enters here everytime a new sample has been gathered by the adc and stores it in an array
        if(adcCallbackFlag == true)
        {
            adcCallbackFlag = false;
            
            sampleBuffer[sampleIndex] = i2cAdcRead();
            sampleIndex++;
            
            if(sampleIndex%(uint16_t)(NSAMPLE/20) == 0) //blinks LED
            {
                USR_LED_Toggle();
            }

            if(sampleIndex >= NSAMPLE)
            {
                i2cAdcDisable();
                sampleIndex = 0;
                adcFlag = true;
            }
        }
        
        //adc array is full, proceeds to data processing
        if(adcFlag == true)
        {
            printUart("Stop sampling\r\n");
            adcFlag = false;
            
            stopTX();
            
            //fft computation
            fft(sampleBuffer, fftBufferRaw, NSAMPLE); //computes fft, radix2
            for(uint16_t i = 0; i < getMaxSpeedIndex(); i++) //discards DC, negative frequencies and out of reach speeds
            {
                fftBuffer[i] = (uint16_t)cabs(fftBufferRaw[i+1]/NSAMPLE); //gets magnitude and casts to int
            }
            bufMaxIndex = (getBufferMaxIndex(fftBuffer, getMaxSpeedIndex())+1)*((float)FS/NSAMPLE);
            bufMax = getMax(fftBuffer, getMaxSpeedIndex());
            
            //prints max frequency found in the fft
            i2c7SegDriverWrite(getSpeed(bufMaxIndex));
            printUart("%.2f Hz = %.1f km/h\r\n", bufMaxIndex, getSpeed(bufMaxIndex));
            
            //inhibits TX to follow <10% duty cycle regulation
            inhibTimerStart();
            while(isTxInhibited)
            {
                //if signal is too faint, flashes a "Low Signal" warning on the 7seg display while TX is inhibited
                if(bufMax <= LOW_LVL && inhibTimerCallbackFlag == true)
                {
                    inhibTimerCallbackFlag = false;
                    if(inhibTimerCounter%5 == 0)
                    {
                        i2c7SegDriverLowSig();
                    }
                    else
                    {
                        i2c7SegDriverWrite(getSpeed(bufMaxIndex));
                    }
                }
            }
        }
    }
    return ( EXIT_FAILURE );
}*/