/*
  gpio.c
 
   Created on: Dec 12, 2018
       Author: Dan Walkes
   Updated by Dave Sluiter Dec 31, 2020. Minor edits with #defines.

   March 17
   Dave Sluiter: Use this file to define functions that set up or control GPIOs.

 */




#include "gpio.h"
#include "em_cmu.h"




// Set GPIO drive strengths and modes of operation
void gpioInit()
{

  // enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
	//GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, false);

	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
	//GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, false);

	GPIO_DriveStrengthSet(SNSR_EN_port, gpioDriveStrengthStrongAlternateStrong);
	//GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(SNSR_EN_port, SNSR_EN_pin, gpioModePushPull, false);

	GPIO_DriveStrengthSet(EXT_COM_IN_port, gpioDriveStrengthStrongAlternateStrong);
	//GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(EXT_COM_IN_port, EXT_COM_IN_pin, gpioModePushPull, false);

	// Using PC10 (SDA) and PC11 (SCL)

	GPIO_PinModeSet(gpioPortC, 10, gpioModeWiredAndPullUp, 1);
	GPIO_PinModeSet(gpioPortC, 11, gpioModeWiredAndPullUp, 1);

	// Using PC6 (SCL) and PC7 (SDA)

	  GPIO_PinModeSet(gpioPortC, 6, gpioModeWiredAndPullUp, 1);
	  GPIO_PinModeSet(gpioPortC, 7, gpioModeWiredAndPullUp, 1);

	  // PC9 to pull-up, used as MAX30101 reset pin
	  GPIO_PinModeSet(gpioPortC, 9, gpioModeWiredAndPullUp, 0);

  // Using PC8 for MAX30101 INT
//	GPIO_PinModeSet(gpioPortC, 8, gpioModeWiredAndPullUpFilter, 1);
//	GPIO_PinModeSet(gpioPortC, 8, gpioModeInput, 1);
//	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
//	GPIO_IntConfig(gpioPortC, 8, true, false, true);

} // gpioInit()


void gpioLed0SetOn()
{
	GPIO_PinOutSet(LED0_port,LED0_pin);
}


void gpioLed0SetOff()
{
	GPIO_PinOutClear(LED0_port,LED0_pin);
}


void gpioLed1SetOn()
{
	GPIO_PinOutSet(LED1_port,LED1_pin);
}


void gpioLed1SetOff()
{
	GPIO_PinOutClear(LED1_port,LED1_pin);
}

void gpioSensorEnSetOn(){
  GPIO_PinOutSet(SNSR_EN_port, SNSR_EN_pin);
}

/*Connection for GPIO Set Display for External Communication Input*/
void gpioSetDisplayExtcomin(bool value){

  if(value == 1){
      gpioextcominOn();
  }
  else if(value == 0){
      gpioextcominOff();
  }
  else{
      //No Action
  }
}

/* GPIO function ON for External Communication Input ON */
void gpioextcominOn(){
  GPIO_PinOutSet(EXT_COM_IN_port, EXT_COM_IN_pin);

}

/* GPIO function OFF for External Communication Input ON */
void gpioextcominOff(){
  GPIO_PinOutClear(EXT_COM_IN_port, EXT_COM_IN_pin);

}



