#ifndef MY_LCD_KEYPAD_H
#define MY_LCD_KEYPAD_H

/* -----------------------          Include Files       --------------------- */

#include "string.h"
#include "TM4C123.h" // Device header
#include "DeviceHeader\tm4c123gh6pm.h"

/* -----------------------           definitions        --------------------- */

#define LCD GPIOB		   // LCD port with Tiva C
#define RS 0x01			   // RS -> PB0 (0x01)
#define RW 0x02			   // RW -> PB1 (0x02)
#define EN 0x04			   // EN -> PB2 (0x04)
#define clear_display 0x01 // clears lcd display
#define lcd_next_line 0xC0 // moves the curser to the next line

/* Defines the size of rows and columns of keypad */
#define RowsSize 4
#define ColsSize 4

/* -----------------------      Global Variables        --------------------- */

unsigned int ncols = 0;
unsigned int nrows = 0;

/* -----------------------      Function Prototypes     --------------------- */

void delayUs(int);								   // Delay in Micro Seconds
void delayMs(int);								   // Delay in Milli Seconds
void LCD_init(void);							   // Initialization of LCD Dispaly
void LCD_Write4bits(unsigned char, unsigned char); // Write data as (4 bits) on LCD
void LCD_String(char *);						   // Write a string on LCD
void LCD_Cmd(unsigned char);					   // Write command
void LCD4bits_Data(unsigned char);				   // Write a character
char *keypad_getkey(void);
void keypad_Init(void);

/* -----------------------      Function Definition     --------------------- */

void LCD_init(void)
{
	SYSCTL->RCGCGPIO |= 0x02; // enable clock for PORTB
	while ((SYSCTL_PRGPIO_R & 0x02) == 0)
		;			 // delay to enable the clock of PORTB
	LCD->DIR = 0xFF; // let PORTB as output pins
	LCD->DEN = 0xFF; // enable PORTB digital IO pins
	LCD_Cmd(0x28);	 // 2 lines and 5x7 character (4-bit data, D4 to D7)
	LCD_Cmd(0x06);	 // Automatic Increment cursor (shift cursor to right)
	LCD_Cmd(0x01);	 // Clear display screen
	LCD_Cmd(0x0F);	 // Display on, cursor blinking
}

void LCD_Write4bits(unsigned char data, unsigned char control)
{
	data &= 0xF0;					 // clear lower nibble for control
	control &= 0x0F;				 // clear upper nibble for data
	LCD->DATA = data | control;		 // Include RS value (command or data ) with data
	LCD->DATA = data | control | EN; // pulse EN
	delayUs(0);						 // delay for pulsing EN
	LCD->DATA = data | control;		 // Turn off the pulse EN
	LCD->DATA = 0;					 // Clear the Data
}

void LCD_String(char *str)
{
	volatile int i = 0; // volatile is important

	while (*(str + i) != '\0') // until the end of the string
	{
		LCD4bits_Data(*(str + i)); // Write each character of string
		i++;					   // increment for next character
	}
}

void LCD_Cmd(unsigned char command)
{
	LCD_Write4bits(command & 0xF0, 0); // upper nibble first
	LCD_Write4bits(command << 4, 0);   // then lower nibble

	if (command < 4)
		delayMs(2); // commands 1 and 2 need up to 1.64ms
	else
		delayUs(40); // all others 40 us
}

void LCD4bits_Data(unsigned char data)
{
	LCD_Write4bits(data & 0xF0, RS); // upper nibble first
	LCD_Write4bits(data << 4, RS);	 // then lower nibble
	delayUs(40);					 // delay for LCD (MCU is faster than LCD)
}

void delayMs(int n)
{
	volatile int i, j; // volatile is important for variables incremented in code
	for (i = 0; i < n; i++)
		for (j = 0; j < 3180; j++) // delay for 1 msec
		{
		}
}

void delayUs(int n)
{
	volatile int i, j; // volatile is important for variables incremented in code
	for (i = 0; i < n; i++)
		for (j = 0; j < 3; j++) // delay for 1 micro second
		{
		}
}

void keypad_Init(void)
{
	SYSCTL->RCGCGPIO |= 0x04; /* enable clock to GPIOC */
	SYSCTL->RCGCGPIO |= 0x10; /* enable clock to GPIOE */

	GPIOE->DIR |= 0x0F; /* set row pins 3-0 as output */
	GPIOE->DEN |= 0x0F; /* set row pins 3-0 as digital pins */
	GPIOE->ODR |= 0x0F; /* set row pins 3-0 as open drain */

	GPIOC->DIR &= ~0xF0; /* set column pin 7-4 as input */
	GPIOC->DEN |= 0xF0;	 /* set column pin 7-4 as digital pins */
	GPIOC->PUR |= 0xF0;	 /* enable pull-ups for pin 7-4 */
}

char *keypad_getkey(void)
{
	char *keymap[4][4] = {
		{"1", "2", "3", "A"},
		{"4", "5", "6", "B"},
		{"7", "8", "9", "C"},
		{"*", "0", "#", "D"},
	};

	int row, col;

	/* check to see any key pressed first */
	GPIOE->DATA = 0;		  /* enable all rows */
	col = GPIOC->DATA & 0xF0; /* read all columns */
	if (col == 0xF0)
		return 0; /* no key pressed */

	/* If a key is pressed, it gets here to find out which key. */
	/* Although it is written as an infinite loop, it will take one of the breaks or return in one pass.*/
	while (1)
	{
		row = 0;
		GPIOE->DATA = 0x0E; /* enable row 0 */
		delayUs(2);			/* wait for signal to settle */
		col = GPIOC->DATA & 0xF0;
		if (col != 0xF0)
			break;

		row = 1;
		GPIOE->DATA = 0x0D; /* enable row 1 */
		delayUs(2);			/* wait for signal to settle */
		col = GPIOC->DATA & 0xF0;
		if (col != 0xF0)
			break;

		row = 2;
		GPIOE->DATA = 0x0B; /* enable row 2 */
		delayUs(2);			/* wait for signal to settle */
		col = GPIOC->DATA & 0xF0;
		if (col != 0xF0)
			break;

		row = 3;
		GPIOE->DATA = 0x07; /* enable row 3 */
		delayUs(2);			/* wait for signal to settle */
		col = GPIOC->DATA & 0xF0;
		if (col != 0xF0)
			break;

		return 0; /* if no key is pressed */
	}

	/* gets here when one of the rows has key pressed */
	if (col == 0xE0)
		return keymap[row][0]; /* key in column 0 */
	if (col == 0xD0)
		return keymap[row][1]; /* key in column 1 */
	if (col == 0xB0)
		return keymap[row][2]; /* key in column 2 */
	if (col == 0x70)
		return keymap[row][3]; /* key in column 3 */
	return 0;				   /* just to be safe */
}
#endif
