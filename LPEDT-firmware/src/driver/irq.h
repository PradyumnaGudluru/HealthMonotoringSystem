/***************************************************************************//**

 * Date:        11-09-2021
 * Author:      Pradyumna Gudluru
 * Description: This code has interrupt handling functionality for low power
 *              energy mode operation.
 *              It is used  for ECEN 5823 "IoT Embedded Firmware".
 *              Reference from Lecture slides and videos.
 ******************************************************************************/
#include "em_letimer.h"
#include "system_efr32bg13p.h"
#include "app.h"
#include "scheduler.h"

uint32_t letimerMilliseconds();

//Testing

#define LETIMER_CTR_VAL   ((LETIMER_PERIOD_MS * ACTUAL_CLK_FREQ)/1000)

