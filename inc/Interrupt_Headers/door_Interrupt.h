#ifndef MY_DOOR_INTERRUPT_H
#define MY_DOOR_INTERRUPT_H

#include <stdint.h>
#include "..\DeviceHeader\tm4c123gh6pm.h"

#define PA6 0x40
void armPortA_Interruput()
{
    __disable_irq(); // make sure that golbal interrupts are disabled before intialization

    // PORTA CONFIGURATION
    SYSCTL_RCGCGPIO_R |= 0x01;
    GPIO_PORTA_LOCK_R = 0x4C4F434B;
    GPIO_PORTA_CR_R |= 0x40;
    GPIO_PORTA_AMSEL_R &= ~0x40;
    GPIO_PORTA_PCTL_R &= ~0x0F000000;
    GPIO_PORTA_DEN_R |= 0x40;
    GPIO_PORTA_AFSEL_R &= ~0x40;
    GPIO_PORTA_DIR_R &= ~0x40;
    GPIO_PORTA_PUR_R |= 0x40;

    // PORTA INTERRUPT CONFIGURATION
    GPIO_PORTA_IS_R = PA6;     // level sensitve
    GPIO_PORTA_IBE_R = ~PA6;   // select level sensitivity frim GPIOIEV
    GPIO_PORTA_IEV_R = PA6;    // select zero level
    GPIO_PORTA_IM_R = PA6;     // arm interrupt to PORTF pF0, PF4 to trigger interrupt
    NVIC_PRI0_R |= 0x00000060; // set interrupt proitiy to 3
    NVIC_EN0_R |= 0x00000001;  // enable interrupt priority
}

void GPIOA_Handler()
{
    // the the door is open the program will remain in this functoin
    // until the the door is closed.
    // this is due to the zero level sensitivty of this interrupt
    GPIO_PORTA_ICR_R |= PA6;
}
#endif
