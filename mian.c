/* ----------------------           Include Files       ---------------------- */

#include "inc/selectCookingProgram.h"

/* ----------------------            Main Program       ---------------------- */

int main()
{
  initializeUserIO();

  while (1)
  {
    LCD_Cmd(clear_display);
    LCD_String("choose a meal");
    SelectCookingProgram();
  }
}
