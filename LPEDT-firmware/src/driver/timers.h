/***************************************************************************//**

 * Date:        11-09-2021
 * Author:      Pradyumna Gudluru
 * Description: This code has timer functionality for low power energy mode
 *              operation.
 *              It is used  for ECEN 5823 "IoT Embedded Firmware".
 *              Timer is initiated with the parameters, comparator values are set
 *              to specific registers, Interrupt flags are enabled and finally
 *              the timer is enabled with the appropriate arguments.
 ******************************************************************************/
#include "em_letimer.h"
#include "app.h"

//void letimerinit();
void initLETIMER0(uint32_t period, sl_power_manager_em_t energy_mode);

int timerWaitUs();

uint32_t getLETIMER0TOP();
