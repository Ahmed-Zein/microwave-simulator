#ifndef MY_PAUSE_RESUME_INTERRUPT_H
#define MY_PAUSE_RESUME_INTERRUPT_H

/* -----------------------          Include Files       --------------------- */

#include "..\DeviceHeader\tm4c123gh6pm.h"
#include <stdbool.h>
#include <stdint.h>

/* -----------------------           definitions        --------------------- */

#define PAUSE (GPIO_PORTF_MIS_R & 0x10)
#define RESUME (GPIO_PORTF_MIS_R & 0x01)

/* -----------------------      Global Variables        --------------------- */

int pressedBefore = 0;

/* -----------------------      Function Prototypes     --------------------- */

bool forcReturnToMain(void);

/* -----------------------      Function Definition     --------------------- */

void armPortF_Interruput()
{
	__disable_irq(); // make sure that golbal interrupts are disabled before intialization

	// PORTF CONFIGURATION
	SYSCTL_RCGCGPIO_R |= 0x20;
	GPIO_PORTF_LOCK_R = 0x4C4F434B;
	GPIO_PORTF_CR_R = 0X1F;
	GPIO_PORTF_AMSEL_R = 0X00;
	GPIO_PORTF_PCTL_R = 0X00;
	GPIO_PORTF_DIR_R = 0XE;
	GPIO_PORTF_DEN_R = 0X1F;
	GPIO_PORTF_PUR_R = 0X11;

	// PORTF INTERRUPT CONFIGURATION
	GPIO_PORTF_IS_R = ~0x11;	// edge sensitve
	GPIO_PORTF_IBE_R = ~0x11;	// interrupt on one edge only { 1: rising || 0: falling}
	GPIO_PORTF_IEV_R = ~0x11;	// select interrupt on falling
	GPIO_PORTF_IM_R = 0x11;		// arm interrupt to PORTF pF0, PF4 to trigger interrupt
	NVIC_PRI17_R |= 0x00200000; // set interrupt proitiy to 2
	NVIC_EN0_R |= 0x40000000;	// enable interrupt priority
}

void GPIOF_Handler()
{
	if (RESUME) // sw2 - resume
	{
		GPIO_PORTF_ICR_R |= 0x01;
		NVIC_ST_CTRL_R |= 0x05 | 0x00010000; // turn on systick
		pressedBefore = 0;
	}
	if (PAUSE) // SW1 - pause
	{
		GPIO_PORTF_ICR_R |= 0x10;
		pressedBefore++;
		if (pressedBefore <= 1)
		{
			NVIC_ST_CTRL_R = 0; // turn off systick
		}
		else
		{
			NVIC_ST_CTRL_R |= 0x05 | 0x00010000; // turn on systick
		}
	}
}
bool forcReturnToMain()
{
	if (pressedBefore >= 2)
	{
		return 1;
	}
	else
		return 0;
}
#endif
