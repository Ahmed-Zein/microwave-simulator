#ifndef MY_SYSTIC_H
#define MY_SYSTIC_H
/* -----------------------          Include Files       --------------------- */

#include <stdint.h>
#include <stdio.h>
#include "DeviceHeader\tm4c123gh6pm.h"
#include "lcd_Keypad.h"
#include "Interrupt_Headers/door_Interrupt.h"
#include "Interrupt_Headers/pause_resume_interrupt.h"

/* -----------------------      Function Prototypes     --------------------- */

void waitSec(volatile uint32_t w);
void DelaySec(volatile uint32_t s);
void DelayMin(volatile uint32_t m);
void printRemaingSec(uint32_t i);

/* -----------------------      Function Definition     --------------------- */

/**
 * Delay in n seconds
 * \param uint32_t  n number of seconds
 * \return none
 * @note SysTick timer is used to generate the delay
 * @note This assumes 16 MHz system clock
 * @note one iteration in the inner loop make 1 mellie second delay
 * **/
void waitSec(volatile uint32_t w)
{
    while (w--)
    {
        volatile uint32_t i = 1000; // looping 1000 time make 1sec delay
        while (i--)
        {
            // 1000 iteration makes 1 second delay
            NVIC_ST_CTRL_R = 0;
            NVIC_ST_RELOAD_R = 0x3E7F;
            NVIC_ST_CURRENT_R = 0;
            NVIC_ST_CTRL_R = 0x5;
            while ((NVIC_ST_CTRL_R & 0x00010000) == 0)
                ;
            NVIC_ST_CTRL_R = 0;
        }
    }
}

/**
 * Delay in n minutes and prints the remaining seconds
 * \param uint32_t n number of minutes
 * \return none
 * @note SysTick timer is used to generate the delay
 * **/
void DelayMin(volatile uint32_t m)
{
    DelaySec(m * 60);
}

/**
 * Delay in n seconds and prints the remaining seconds
 * \param uint32_t  n number of seconds
 * \return none
 * @note SysTick timer is used to generate the delay
 * @note This assumes 16 MHz system clock
 * @note one iteration in the inner loop make 1 mellie second delay
 * **/
void DelaySec(volatile uint32_t s)
{
    volatile uint32_t i;
    armPortF_Interruput();
    armPortA_Interruput();
    __enable_irq();
    while (s--)
    {
        i = 1000;
        while (i--)
        {
            // 1000 iteration makes 1 second delay
            NVIC_ST_CTRL_R = 0;
            NVIC_ST_RELOAD_R = 0x3E7F;
            NVIC_ST_CURRENT_R = 0;
            NVIC_ST_CTRL_R = 0x5;
            while ((NVIC_ST_CTRL_R & 0x00010000) == 0)
                ;
            NVIC_ST_CTRL_R = 0;

            if (forcReturnToMain())
                return;
        }
        printRemaingSec(s);
    }
    __disable_irq();
}

/**
 *  clears the lcd and prints the remaining seconds
 * \param int  n number of remaing seconds
 * \return none
 * **/
void printRemaingSec(uint32_t s)
{
    char buf[3];
    LCD_Cmd(clear_display);
    sprintf(buf, "%d", s);
    LCD_String(buf);
    LCD_String(" sec");
}

#endif
