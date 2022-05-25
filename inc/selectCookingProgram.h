/********************************
 * A :     popcorn              *
 * B :     beef                 *
 * C :     chicken              *
 * D :     userDefined          *
 ********************************/

/* ----------------------           Include Files       ---------------------- */

#include "microwaveFuntions.h"
#include "systickTimers.h"

/* -----------------------      Function Prototypes     --------------------- */

void SelectCookingProgram(void);

/* ----------------------          State Enumeration     ---------------------- */

enum State
{
    Popcorn = 'A',
    Beef = 'B',
    Chicken = 'C',
    UserDefined = 'D',
};

/* -----------------------      Function Definition     --------------------- */

void SelectCookingProgram(void)
{
    while (1)
    {
        char *key = keypad_getkey();

        switch (*key)
        {
        case Popcorn:
            popcorn();
            return;
        case Beef:
            BeefOrChicken("Beef", BeefRate);
            return;
        case Chicken:
            BeefOrChicken("Chicken", ChickenRate);
            return;
        case UserDefined:
            userDefinedTime();
            return;
        default:
            break;
        }
    }
}
