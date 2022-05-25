#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "STD_TYPES.h"
#include "LCD.h"
#include "GPIO_INIT.h"

int main()
{
	init_lcd();
	while(1)
	{
		//lcd_cmd(0x80); //first line
		//lcd_write_string("Hello");
		lcd_cmd(0xC0);
		lcd_write_string("Welcome");
		delay_ms(1000);
	}
}
