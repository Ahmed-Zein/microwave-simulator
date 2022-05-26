/********************************
 * sw1 :     Pause              *
 * SW2 :     Start              *
 * SW3 :     Door_State         *
 ********************************/
#ifndef MY_MICROWAVE_FUNCTION_DEFINTION_H
#define MY_MICROWAVE_FUNCTION_DEFINTION_H
/* -----------------------          Include Files       --------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "DeviceHeader\tm4c123gh6pm.h"
#include "lcd_keypad.h"
#include "systickTimers.h"
#include "hardwareFunctions.h"

/* -----------------------           definitions        --------------------- */

#define BeefRate 30     // converted from 0.5 minutes per kilo => 30 sec per kilo
#define ChickenRate 12  // converted from 0.5 minutes per kilo => 12 sec per kilo
#define StartCooking 1  // switch2 is pressed & the door is closed
#define CloseTheDoor 10 // switch 2 is pressed BUT the door is Open
#define Idel 0          // switch 2 is not pressed yet
#define operationStart(sw2, sw3) (sw2 != Sw2press && (sw3 != DoorIsOpen)) ? (StartCooking) : ((sw2 != Sw2press && (sw3 == DoorIsOpen)) ? (CloseTheDoor) : Idel)

/* -----------------------      Function Prototypes     --------------------- */

void popcorn(void);
void BeefOrChicken(char *cookingElement, int cookingRate);
void userDefinedTime(void);
void endCooking(void);
bool getTime(void);
void getWeight(char *x);
int calculateCookingTime(int t);
void printFormatedTime(int time);
void promptCloseTheDoor(void);

/* -----------------------      Global Variables        --------------------- */

char sw1;
char sw2;
char sw3;
char *weight;
int weightValue = 100;
volatile int inTimeVal = 0;
volatile bool screenUpdateFlage = 1; //prvents continuously refreshing the lcd while {pressing sw2 and the door is open}

/* -----------------------      Function Definition     --------------------- */

void popcorn(void)
{
    initAllSwitches();
    LCD_Cmd(clear_display);
    LCD_String("Popcorn");
    LCD_Cmd(lcd_next_line);
    LCD_String("   Press Start  ");
    screenUpdateFlage = 1; // makesure that the screen update flage is set to 1
    while (1)
    {
        sw2 = sw2_input();
        sw3 = sw3_input(); // door Status
        switch (operationStart(sw2, sw3))
        {
        case StartCooking:
            turnOnLEDS();
            LCD_Cmd(clear_display);
            LCD_String("Cooking Popcorn");
            DelayMin(1);
            endCooking();
            return;
        case CloseTheDoor:
            promptCloseTheDoor();
            break;
        default:
            continue;
        }
    }
}

void BeefOrChicken(char *cookingElement, int cookingRate)
{
    initAllSwitches();
    getWeight(cookingElement);
    LCD_Cmd(clear_display);
    LCD_String("  Press Start  ");
    screenUpdateFlage = 1; // makesure that the screen update flage is set to 1
    while (1)
    {
        sw2 = sw2_input();
        sw3 = sw3_input(); // door Status
        switch (operationStart(sw2, sw3))
        {
        case StartCooking:
            LCD_Cmd(clear_display);
            LCD_String("cooking ");
            LCD_String(cookingElement);
            turnOnLEDS();
            DelaySec(weightValue * cookingRate);
            endCooking();
            return;
        case CloseTheDoor:
            promptCloseTheDoor();
            break;
        default:
            continue;
        }
    }
}

void userDefinedTime(void)
{
    initAllSwitches();
    LCD_Cmd(clear_display);
    LCD_String("Cooking Time?");
    waitSec(2);
    LCD_Cmd(clear_display);
    LCD_String("00:00");
    if (getTime())
    {
        // runs if the user pressed start in getTime();
        turnOnLEDS();
        DelaySec(calculateCookingTime(inTimeVal)); // add the seconds u got
        turnOffLEDS();
        endCooking();
        return;
    }
    else
    {
        // runs if the user entered the 4-digits only in getTime();
        LCD_Cmd(lcd_next_line);
        LCD_String("   Press Start  ");
        while (1)
        {
            sw2 = sw2_input();
            sw3 = sw3_input(); // door Status
            switch (operationStart(sw2, sw3))
            {
            case StartCooking:
                turnOnLEDS();
                DelaySec(calculateCookingTime(inTimeVal)); // add the seconds u got
                turnOffLEDS();
                endCooking();
                return;
            case CloseTheDoor:
                if (screenUpdateFlage)
                {
                    // this if block is to ensure that the screen won't keep updating if
                    // the user holds the sw2 btn
                    printFormatedTime(inTimeVal);
                    LCD_Cmd(lcd_next_line);
                    LCD_String(" Close The door");
                    screenUpdateFlage = 0;
                }
                break;
            default:
                continue;
            }
        }
    }
}

/**
 * Function: End Cooking
 * \return none
 * \note prints "Done" on the lcd for 2sec,
 *       then turns on the buzzer and flash the leds for 3sec
 * \note used in all cooking functions
 **/
void endCooking(void)
{
    volatile uint8_t flash;
    if (forcReturnToMain())
    {
        turnOffLEDS();
        __disable_irq();
        pressedBefore = 0;
        return;
    }
    LCD_Cmd(clear_display);
    LCD_String("Done");
    ringBuzzerInit();
    flash = 3;
    Buzzer |= BuzzerON;
    while (flash--)
    {
        turnOnLEDS();
        waitSec(1);
        turnOffLEDS();
        waitSec(1);
    }
    Buzzer &= BuzzerOFF;
}

/**
 * Function: Get Weight
 * \param charPointer name of the item weight you need
 * \note getWight prints on the lcd the weight of the item in kg
 * \note used in BeefOrChicken function
 **/
void getWeight(char *x)
{
    LCD_Cmd(clear_display);
    LCD_String(x);
    LCD_String(" weight?");
    LCD_Cmd(lcd_next_line);
    delayMs(100);
    while (1)
    {
        weight = keypad_getkey();

        if (weight != 0)
        {
            weightValue = atoi(weight);
            if (weightValue <= 9 && weightValue > 0)
            {
                LCD_Cmd(clear_display);
                LCD_String(x);
                LCD_String(" weight :");
                LCD_Cmd(lcd_next_line);
                LCD_String(weight);
                LCD_String("Kg");
                waitSec(2);
                break;
            }
            else
            {
                LCD_Cmd(clear_display);
                LCD_String("Err ...");
                waitSec(2);
                LCD_Cmd(clear_display);
                LCD_String(x);
                LCD_String(" weight?");
            }
        }
    }
}

/**
 * gets the time for cooking from user.
 * \returns \b 1 if user pressed on start(sw2) directly
 *        & \b 0 if user entered full 4 digits.
 * \note used in userDefinendTime function
 **/
bool getTime(void)
{
    volatile int temp;
    char *inTime;
    volatile int j = 4;
    inTimeVal = 0;
    __disable_irq();
    for (j = 4; j > 0; j--)
    {
        *inTime = NULL;
        while (1)
        {
            inTime = keypad_getkey();
            if (strcmp(inTime, "A") == 0 || strcmp(inTime, "B") == 0 || strcmp(inTime, "C") == 0 ||
                strcmp(inTime, "D") == 0 || strcmp(inTime, "*") == 0 || strcmp(inTime, "#") == 0)
            {
                // makes sure that the user input is only a Valid Numbers
                continue;
            }
            else if (strcmp(inTime, "0") == 0 && inTimeVal == 0)
            {
                // ignores all zeros the user input before any number greater than zero is entered
                // also makes sure that if the user enters 4-Zeros the function won't terminate
                continue;
            }
            else if (inTime != 0 && *inTime != NULL)
            {
                temp = atoi(inTime);
                inTimeVal = inTimeVal * 10 + temp;
                printFormatedTime(inTimeVal);
                if (inTimeVal > 3000)
                {
                    /*
                     * this block checks if the user time input didn't exceed 30min
                     */
                    LCD_Cmd(lcd_next_line);
                    LCD_String("max time 30min!");
                    waitSec(2);
                    printFormatedTime(10000); // prints 00:00 on the lcd
                    j = 5;
                    inTimeVal = 0;
                }
                break;
            }

            sw1 = sw1_input();
            if (sw1 != 0x10 && inTimeVal != 0) // reset the input time if switch 1 is pressed
            {
                printFormatedTime(10000); // prints 00:00 on the lcd
                j = 4;
                inTimeVal = 0;
            }

            if (inTimeVal == 0) // don't check the state until user input
                continue;

            sw2 = sw2_input(); // Starts operation
            sw3 = sw3_input(); // door status
            switch (operationStart(sw2, sw3))
            {
            case StartCooking:
                return 1;
            case CloseTheDoor:
                LCD_Cmd(lcd_next_line);
                LCD_String(" Close The door");
                break;
            default:
                continue;
            }
        }
        /*
        100 milli seconds delay in order to clear the value
        on the keypad buffer after pressing.
        */
        delayMs(100);
    }
    return 0; // prompts for pressing SW2 to start
}

/**
 * Calculate Cooking Time
 * \param int the time we get from getTime() function.
 * \return (int) cooking time in sec.
 * \note used in userDefinendTime function
 **/
int calculateCookingTime(int t)
{
    int res;
    res = t % 100;
    res += (t / 100) * 60;
    return res;
}

/**
 * print formated time
 * \param int entered time by user
 * \note prints on the lcd as 00:00
 * \note used in userDefinendTime function
 **/
void printFormatedTime(int time)
{
    volatile int i = 1;
    char formatedTime[5];
    time += 10000;
    LCD_Cmd(clear_display);
    sprintf(formatedTime, "%d", time);
    for (; i <= 4; i++)
    {
        if (i == 3)
        {
            LCD_String(":");
        }
        LCD4bits_Data(*(formatedTime + i));
    }
}

/**
 * prompts the user to close the door
 * \note used in all functions
 * */
void promptCloseTheDoor(void)
{
    if (screenUpdateFlage)
    {
        LCD_Cmd(clear_display);
        LCD_String(" Close The door");
        screenUpdateFlage = 0;
    }
}
#endif
