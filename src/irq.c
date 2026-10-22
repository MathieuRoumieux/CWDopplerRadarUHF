#include "irq.h"
#include "definitions.h"
#include "utils.h"
#include "timer.h"

volatile bool irqCallbackFlag = false;

void USR_SW_Callback(uintptr_t context)
{
    irqCallbackFlag = true;
}

void irqInit()
{
    EIC_CallbackRegister(EIC_PIN_7, USR_SW_Callback, 0);
    EIC_InterruptEnable(EIC_PIN_7);
}