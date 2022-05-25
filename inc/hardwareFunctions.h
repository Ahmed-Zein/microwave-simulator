#ifndef MY_HARDWARE_FUNCTIONS_H
#define MY_HARDWARE_FUNCTIONS_H
/* -----------------------          Include Files       --------------------- */

#include <stdint.h>
#include "DeviceHeader\tm4c123gh6pm.h"
#include "lcd_Keypad.h"

/* -----------------------           definitions        --------------------- */

#define Buzzer GPIO_PORTA_DATA_R
#define BuzzerON 0x80
#define BuzzerOFF ~0x80
#define LED GPIO_PORTF_DATA_R
#define LEDsON 0xE
#define LEDsOFF ~0xE
#define DoorIsOpen 0x40
#define Sw2press 0x01

/* -----------------------      Function Prototypes     --------------------- */

void initializeUserIO(void);
void turnOnLEDS(void);
void turnOffLEDS(void);
void ringBuzzer(void);
void initialize_SW1(void);
void initialize_SW2(void);
void initialize_SW3(void);
void initAllSwitches(void);
unsigned char sw1_input(void);
unsigned char sw2_input(void);
unsigned char sw3_input(void);

/* -----------------------      Function Definition     --------------------- */

/**
 * initializes keyPad {input} and lcd {output}
 */
void initializeUserIO(void)
{
    LCD_init();             // initialize 16x2 LCD
    LCD_init();             // initialize 16x2 LCD
    keypad_Init();          // Initialize keypad and TM4C123 GPIO pins
    LCD_Cmd(clear_display); // Clear LCD screen
}

/**
 * initializes switch 1, 2, 3
 */
void initAllSwitches(void)
{
    initialize_SW1();
    initialize_SW2();
    initialize_SW3();
}

/**
 * turn ON the 3 leds @portF
 * \param pin PF1 -> 1
 * \param pin PF2 -> 1
 * \param pin PF3 -> 1
 **/
void turnOnLEDS(void)
{
    SYSCTL_RCGCGPIO_R |= 0x20; // intialize the clk for PortF
    while ((SYSCTL_PRGPIO_R & 0x20) == 0)
        ;                             // delay
    GPIO_PORTF_LOCK_R = 0x4C4F434B;   // unlocking PortF
    GPIO_PORTF_CR_R |= 0x0E;          // Allow changing in pin 3, 2, 1 in portF
    GPIO_PORTF_AMSEL_R &= ~0x0E;      // disable the analog function
    GPIO_PORTF_PCTL_R &= ~0x0000FFF0; // disable alternative functions
    GPIO_PORTF_AFSEL_R &= ~0x0E;      // disable the alternate function
    GPIO_PORTF_DIR_R |= 0x0E;         // setting Pin 1, 2, 3 as output
    GPIO_PORTF_DEN_R |= 0x0E;         // digital enable  for Pin 1, 2, 3
    LED |= LEDsON;                    // intialize pins 1,2,3 to be On
}

/**
 * turn OFF the 3 leds @portF
 * \param pin PF1 -> 0
 * \param pin PF2 -> 0
 * \param pin PF3 -> 0
 **/
void turnOffLEDS(void)
{
    SYSCTL_RCGCGPIO_R |= 0x20; // intialize the clk for PortF
    while ((SYSCTL_PRGPIO_R & 0x20) == 0)
        ;                             // delay
    GPIO_PORTF_LOCK_R = 0x4C4F434B;   // unlocking PortF
    GPIO_PORTF_CR_R |= 0x0E;          // Allow changing in pin 3, 2, 1 in portF
    GPIO_PORTF_AMSEL_R &= ~0x0E;      // disable the analog function
    GPIO_PORTF_PCTL_R &= ~0x0000FFF0; // disable alternative functions
    GPIO_PORTF_AFSEL_R &= ~0x0E;      // disable the alternate function
    GPIO_PORTF_DIR_R |= 0x0E;         // setting Pin 1, 2, 3 as output
    GPIO_PORTF_DEN_R |= 0x0E;         // digital enable  for Pin 1, 2, 3
    LED &= LEDsOFF;                   // intialize pins 1,2,3 to be On
}

/**
 * intializes switch 1 in portF
 * \param pin PF4
 **/
void initialize_SW1(void)
{
    SYSCTL_RCGCGPIO_R |= 0x20; // intialize the clk for PortF
    while ((SYSCTL_PRGPIO_R & 0x20) == 0)
        ;                             // delay
    GPIO_PORTF_LOCK_R = 0x4C4F434B;   // Constant value
    GPIO_PORTF_CR_R |= 0x10;          // setting pin pF4 to change
    GPIO_PORTF_AMSEL_R &= ~0x10;      // disable analog function 		-off by default
    GPIO_PORTF_PCTL_R &= ~0x000F0000; // enable digital function
    GPIO_PORTF_DEN_R |= 0x10;         // Enable digital for sw1
    GPIO_PORTF_AFSEL_R &= ~0x10;      // disable alterante function
    GPIO_PORTF_DIR_R &= ~0x10;        // DIR--> 0->input & 1->output
    GPIO_PORTF_PUR_R |= 0x10;         // active low
}

/**
 * intializes switch 2 in portF
 * \param pin PF0
 **/
void initialize_SW2(void)
{
    SYSCTL_RCGCGPIO_R |= 0x20; // intialize the clk for PortF
    while ((SYSCTL_PRGPIO_R & 0x20) == 0)
        ;                           // delay
    GPIO_PORTF_LOCK_R = 0x4C4F434B; // Constant value
    GPIO_PORTF_CR_R |= 0x01;        // setting pin PF0 to change
    GPIO_PORTF_AMSEL_R &= ~0x01;    // disable analog function 		-off by default
    GPIO_PORTF_PCTL_R &= ~0x0F;     // enable digital function
    GPIO_PORTF_DEN_R |= 0x01;       // Enable digital for sw2
    GPIO_PORTF_AFSEL_R &= ~0x10;    // disable alterante function
    GPIO_PORTF_DIR_R &= ~0x01;      // DIR--> 0->input & 1->output
    GPIO_PORTF_PUR_R |= 0x01;       // active low
}

/**
 * intializes switch 3 in portA
 * \param pin PA0
 **/
void initialize_SW3(void)
{
    SYSCTL_RCGCGPIO_R |= 0x01; // intialize the clk for PortA
    while ((SYSCTL_PRGPIO_R & 0x01) == 0)
        ;                             // delay
    GPIO_PORTA_LOCK_R = 0x4C4F434B;   // Constant value
    GPIO_PORTA_CR_R |= 0x40;          // setting pin pA7 to change
    GPIO_PORTA_AMSEL_R &= ~0x40;      // disable analog function 		-off by default
    GPIO_PORTA_PCTL_R &= ~0x0F000000; // enable digital function
    GPIO_PORTA_DEN_R |= 0x40;         // Enable digital for pA6
    GPIO_PORTA_AFSEL_R &= ~0x40;      // disable alterante function
    GPIO_PORTA_DIR_R &= ~0x40;        // DIR--> 0->input & 1->output     -setting A6 to 1
    GPIO_PORTA_PUR_R |= 0x40;         // active low
}

/**
 * gets input from switch 1 @PF4
 * \return [0x01 OR 0x00]
 **/
unsigned char sw1_input(void)
{
    return GPIO_PORTF_DATA_R & 0x10; // 0001 0000
}

/**
 * gets input from switch 1 @PF0
 * \return [0x01 OR 0x00]
 **/
unsigned char sw2_input(void)
{
    return GPIO_PORTF_DATA_R & 0x01; // 0000 0001
}

/**
 *  gets the status of the sw3{door}
 * \return [0x01 OR 0x00]
 **/
unsigned char sw3_input(void)
{
    return GPIO_PORTA_DATA_R & 0x40; // 0100 0000
}

/**
 * intializes the buzzer in portA
 * \param pin PA7
 **/
void ringBuzzerInit(void)
{
    SYSCTL_RCGCGPIO_R |= 0x01; // intialize the clk for PortA
    while ((SYSCTL_PRGPIO_R & 0x01) == 0)
        ;                             // delay
    GPIO_PORTA_LOCK_R = 0x4C4F434B;   // Constant value
    GPIO_PORTA_CR_R |= 0x80;          // setting pin pA7 to change
    GPIO_PORTA_AMSEL_R &= ~0x80;      // disable analog function 		-off by default
    GPIO_PORTA_PCTL_R &= ~0xF0000000; // enable digital function
    GPIO_PORTA_DEN_R |= 0x80;         // Enable digital for pA7
    GPIO_PORTA_AFSEL_R &= ~0x80;      // disable alterante function
    GPIO_PORTA_DIR_R |= 0x80;         // DIR--> 0->input & 1->output     -setting A7 to 1
    GPIO_PORTA_PUR_R |= 0x80;         // active low
}

#endif
