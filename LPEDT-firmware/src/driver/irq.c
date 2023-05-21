/***************************************************************************//**

 * Date:        11-09-2021
 * Author:      Pradyumna Gudluru
 * Description: This code has interrupt handling functionality for low power
 *              energy mode operation.
 *              It is used  for ECEN 5823 "IoT Embedded Firmware".
 *              Reference from Lecture slides and videos.
 ******************************************************************************/
#include "irq.h"
#include "src/driver/timers.h"
#define INCLUDE_LOG_DEBUG 1
#include "src/driver/log.h"



/* Keeps count of time passed since the system startup */
static volatile uint32_t milliseconds;
/***************************************************************************//**
  //void LETIMER0_IRQHandler();
 * Description: LETIMER0_IRQHandler is used to enable the IRQ for LETIMER0.
 ******************************************************************************/
void LETIMER0_IRQHandler()
{

  /* Check which IF is set */
  uint32_t flags = LETIMER_IntGetEnabled(LETIMER0);


  /* Set the UF Event */
  if(flags & LETIMER_IEN_UF)
  {
      schedulerSetEvent_LETIMER0_UF();
  }

    /* 3 seconds have passed */
    CORE_CRITICAL_SECTION(milliseconds += LETIMER_PERIOD_MS;);


//  /* Set the COMP1 Event */
//  if(flags == LETIMER_IEN_COMP1)
//    {
//      LETIMER_IntDisable(LETIMER0, LETIMER_IEN_COMP1);
//      schedulerSetEvent_COMP1();
//    }

    /* Clear the interrupt */
      LETIMER_IntClear(LETIMER0, flags);

}

/***************************************************************************//**
  //int letimerMilliseconds();
 * Description: letimerMilliseconds is used to transfer time count for logging.
 ******************************************************************************/

uint32_t letimerMilliseconds(){

  uint32_t ctrmilliseconds;
  ctrmilliseconds = (LETIMER_CTR_VAL - LETIMER_CounterGet(LETIMER0));
  ctrmilliseconds = (ctrmilliseconds*LETIMER_PERIOD_MS)/ACTUAL_CLK_FREQ;
  return (milliseconds + ctrmilliseconds);
}

/***************************************************************************//**
  //void I2C0_IRQHandler();
 * Description: I2C0_IRQHandler is used to enable the IRQ for I2C.
 ******************************************************************************/
void I2C0_IRQHandler(void) {
// this can be locally defined
  uint32_t flags = I2C_IntGetEnabled(I2C0);

  I2C_TransferReturn_TypeDef trans_ret = I2C_Transfer(I2C0);

  if(trans_ret == i2cTransferDone){
      schedulerSetI2C_IRQ_Event();
  }

  if(trans_ret < 0)
    LOG_ERROR("I2C Transfer Failed with status = %d\r\n", trans_ret);

  I2C_IntClear(I2C0,flags);

} // I2C0_IRQHandler()


/***************************************************************************//**
 * @name GPIO_EVEN_IRQHandler
 *
 * @brief
 *   Interrupt handler which sets events based on the GPIO odd pin interrupt.
 *
 * @param[in] osc
 *   none
 *
 * @return void
 ******************************************************************************/
void GPIO_EVEN_IRQHandler()
{
  /* Check which IF is set */
  uint32_t flags = GPIO_IntGetEnabled();

  /* Set the button release event */
  if(flags == (1 << 8))
    {
      /* Clear the interrupt */
      GPIO_IntClear(flags);
    //  schedulerSetEvent_Max_Int();
    }

}

