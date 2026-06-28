#include <os_task.h>
#include "Cpu.h"

volatile int exit_code = 0;

int main(void)
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  #ifdef PEX_RTOS_INIT
    PEX_RTOS_INIT();                   /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of Processor Expert internal initialization.                    ***/


  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif

  for(;;) 
  {
    if(exit_code != 0) 
    {
      break;
    }
  }
  return exit_code;

} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/
