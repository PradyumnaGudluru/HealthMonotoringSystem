/***************************************************************************//**

 * Date:        11-09-2021
 * Author:      Pradyumna Gudluru
 * Description: This code has oscillator functionality for low power energy mode
 *              operation.
 *              It is used  for ECEN 5823 "IoT Embedded Firmware".
 *              Oscillator is enabled, the clock is set with set division and enabled.
 *
 ******************************************************************************/
#include "em_cmu.h"
#include "app.h"


void oscillatorinit();
