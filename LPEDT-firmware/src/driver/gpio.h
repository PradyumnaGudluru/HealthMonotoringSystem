/*
   gpio.h
  
    Created on: Dec 12, 2018
        Author: Dan Walkes

    Updated by Dave Sluiter Sept 7, 2020. moved #defines from .c to .h file.
    Updated by Dave Sluiter Dec 31, 2020. Minor edits with #defines.

 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_
#include <stdbool.h>
#include "em_gpio.h"
#include <string.h>


// Student Edit: Define these, 0's are placeholder values.
// See the radio board user guide at https://www.silabs.com/documents/login/user-guides/ug279-brd4104a-user-guide.pdf
// and GPIO documentation at https://siliconlabs.github.io/Gecko_SDK_Doc/efm32g/html/group__GPIO.html
// to determine the correct values for these.

#define	LED0_port  gpioPortF // change to correct ports and pins
#define LED0_pin   4
#define LED1_port  gpioPortF
#define LED1_pin   5

#define SNSR_EN_port gpioPortA // Sensor Enable port set to PortD
#define SNSR_EN_pin 0 // Sensor Enable pin set to 15

#define EXT_COM_IN_port gpioPortA // EXT COM INPUT Enable port set to PortD
#define EXT_COM_IN_pin 1 // EXT COM INPUT Enabke pin set to 13


// Function prototypes
void gpioInit();
void gpioLed0SetOn();
void gpioLed0SetOff();
void gpioLed1SetOn();
void gpioLed1SetOff();
void gpioSensorEnSetOn();
void gpioSetDisplayExtcomin(bool value);

void gpioextcominOn();
void gpioextcominOff();


#endif /* SRC_GPIO_H_ */
