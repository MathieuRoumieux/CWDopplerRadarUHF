#ifndef IRQ_H
#define	IRQ_H

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
extern volatile bool irqCallbackFlag;

void USR_SW_Callback(uintptr_t context);
void irqInit();

#ifdef	__cplusplus
}
#endif

#endif	/* IRQ_H */
