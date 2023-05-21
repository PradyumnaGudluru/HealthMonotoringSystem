/***************************************************************************//**

 * Date:        19-09-2021
 * Author:      Pradyumna Gudluru
 * Description: This code has scheduler functionality.
 *              It is used  for ECEN 5823 "IoT Embedded Firmware".
 *              Scheduler is initialized, and app action is set, with sate change for underflow interrupt.
 *              The scheduler event is set and received.
 * References: Slides from Lectures of Dave Sluiter.
 ******************************************************************************/
#include "em_core.h"
#include "app.h"
#include "ble.h"

void scheduler_init();

void schedulerSetEvent_LETIMER0_UF();
void schedulerSetEvent_Max_Int();
void schedulerSetI2C_IRQ_Event();
uint32_t getNextEvent();

void system_state_machine();
#define event_count 1
