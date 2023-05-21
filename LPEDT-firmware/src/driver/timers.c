/***************************************************************************//**

 * Date:        11-09-2021
 * Author:      Pradyumna Gudluru
 * Description: This code has timer functionality for low power energy mode
 *              operation.
 *              It is used  for ECEN 5823 "IoT Embedded Firmware".
 *              Timer is initiated with the parameters, comparator values are set
 *              to specific registers, Interrupt flags are enabled and finally
 *              the timer is enabled with the appropriate arguments.
 * References and Support: 1) Lectures by Dave Sluiter
 *                         2) Discussion and analysis from SA.
 ******************************************************************************/
#include "timers.h"
#include "app.h"

#define INCLUDE_LOG_DEBUG 1
#include "src/driver/log.h"


#define PRESCALER_VALUE     4
#define LFXO_FREQ           32768
#define ULFRCO_FREQ         1000
#define LETIMER0_PERIOD_MS   3000


LETIMER_Init_TypeDef init;

static volatile uint32_t max_count = 0;

/***************************************************************************//**
  //void letimerinit();
 * Description: letimerinit is used to initialize parameters.
 ******************************************************************************/
void letimerinit(){
  init.comp0Top = true;
  init.bufTop = false;
  init.enable = false;

  LETIMER_Init(LETIMER0, &init);
  LETIMER_CompareSet(LETIMER0, 0, VALUE_TO_LOAD); // Counter value updated to COMP0
  //LETIMER_CompareSet(LETIMER0, 1, VALUE_TO_COMP); // Interrupt count updated to COMP1
  LETIMER_CounterSet(LETIMER0, VALUE_TO_LOAD);
  LETIMER_IntEnable(LETIMER0, LETIMER_IEN_UF); //Interrupt enabled at Underflow and COMP1
  NVIC_ClearPendingIRQ(LETIMER0_IRQn);
  NVIC_EnableIRQ(LETIMER0_IRQn);
  LETIMER_Enable(LETIMER0, true);

}


void initLETIMER0(uint32_t period, sl_power_manager_em_t energy_mode)
{

  //configure the oscillator for LETIMER0 based on the energy mode
  oscillator_init(energy_mode);

  //enable the clock
//  CMU_ClockEnable(cmuClock_LETIMER0, true);
  //set the top value according to period
  uint32_t top_value = (period * ACTUAL_CLK_FREQ) / 1000;

  //set letimer0 run in repeatFree mode
  //set comp0 as the top value each time letimer0 wraps around
  LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT;
  letimerInit.enable = false;
  letimerInit.comp0Top = true;
  letimerInit.repMode = letimerRepeatFree;
  letimerInit.topValue = top_value;

  //set the repeat mode
  LETIMER_RepeatSet(LETIMER0, 0, 1);

  //set the comp0 register with top_value
  LETIMER_CompareSet(LETIMER0, 0, top_value);

  //enable LETIMER0 interrupt for COMP1 and UF
//  LETIMER0->IEN |= LETIMER_IEN_UF;
  LETIMER0->IEN |= LETIMER_IEN_COMP1 | LETIMER_IEN_UF;

  //initialize and enable LETIMER0
  LETIMER_Init(LETIMER0, &letimerInit);

  //enable LETIMER0 IRQ in NVIC
  NVIC_ClearPendingIRQ(LETIMER0_IRQn);
  NVIC_EnableIRQ(LETIMER0_IRQn);

  //enable LETIMER0
  LETIMER_Enable(LETIMER0,true);

} //initLETIMER0


/***************************************************************************//**
  //void timerWaitUs();
 * Description: timerWaitUs is interrupt based timer for required period.
 ******************************************************************************/
int timerWaitUs(uint32_t us_wait){
  /* You should add a new function timerWaitUs(uint32_t us_wait) which blocks
(polls) at least us_wait microseconds, using LETIMER0 tick counts as a reference,
and which supports waits as long as those needed by the Load Power
Management and I2C steps needed below. Add code to range check the input
parameter to ensure that the requested delay is not longer than the routine is
capable of providing, and choose a way to handle this exception.*/

  //To hold the program at I2C until transfer at one space.

  volatile uint32_t i;
  if((us_wait > (LETIMER_PERIOD_MS*1000)) || (us_wait <= (1000/ACTUAL_CLK_FREQ)))
    {
      //LOG_ERROR("Cannot have timer wait time greater than period");
    }
  else{

      uint32_t ms_wait = us_wait/1000;
      uint32_t us_wait_count = (ms_wait*ACTUAL_CLK_FREQ)/1000;
      max_count = ((LETIMER_PERIOD_MS*ACTUAL_CLK_FREQ)/1000); // To check the max count for maximum number of ticks
      if(LETIMER_CounterGet(LETIMER0) >= us_wait_count){           // To check the count to be done if rollover
          i = (LETIMER_CounterGet(LETIMER0) - us_wait_count);
      }
      else{
           i = (max_count) -(us_wait_count - LETIMER_CounterGet(LETIMER0));
      }
      LETIMER_IntClear(LETIMER0, LETIMER_IFC_COMP1);
      LETIMER_CompareSet(LETIMER0, 1, i); // Interrupt count updated to COMP1
      LETIMER_IntEnable(LETIMER0, LETIMER_IEN_COMP1);

  }
 // break;
return 0;
}


uint32_t getLETIMER0TOP()
{
  return max_count;
}
