/*
 * i2c_max.h
 *
 *  Created on: Apr 1, 2023
 *      Author: rampr
 */

#ifndef SRC_I2C_MAX_H_
#define SRC_I2C_MAX_H_

#include "sl_i2cspm.h"
//#include "gpio.h"
#include "em_cmu.h"
#include <em_i2c.h>
#include "sl_i2cspm_instances.h"
#include "src/driver/timers.h"

#include <stddef.h>
#include <stdint.h>


#define MAX_30101_ADDRESS 0x57 // Address


typedef struct max_data{

  float spo2;
  int bpm;
}max_data_t;


/*****************************************************************************
 * This function fetches both the IR data and RED LED data from the sensor. The
 * IR data and the RED LED data will be used for heart rate and pulse oximetry
 * measurement.
 *
 * @param: None
 *
 * @return:[int] return code
 *
 *****************************************************************************/
int Max_30101_FIFO();
//bool Max_30101_FIFO_and_Finger_Detection();

/******************************************************************************
 * @name MAX_30101_Init
 *
 * @brief
 *   Initializes the module MAX30101 sensor.
 *
 * @param[in] none
 *
 * @return void
 ******************************************************************************/
void MAX_30101_Init();

/******************************************************************************
 * @name MAX_30101_Get_Reg_Val
 *
 * @brief
 *   For getting the register value. Not used yet.
 *
 * @param[in] none
 *
 * @return void
 ******************************************************************************/
void MAX_30101_Get_Reg_Val (uint8_t reg, uint8_t* read_data, size_t nbytes_read_data);

/******************************************************************************
 * @name MAX_30101_ShutDown
 *
 * @brief
 *   To change MAX to shutdown mode.
 *
 * @param[in] none
 *
 * @return void
 ******************************************************************************/
void MAX_30101_ShutDown();

/******************************************************************************
 * @name MAX_30101_PowerUp
 *
 * @brief
 *   To change MAX to power up mode.
 *
 * @param[in] none
 *
 * @return void
 ******************************************************************************/
void MAX_30101_PowerUp();

/******************************************************************************
 * @name MAX_30101_Reset
 *
 * @brief
 *   To reset MAX sensor.
 *
 * @param[in] none
 *
 * @return void
 ******************************************************************************/
void MAX_30101_Reset();

/*****************************************************************************
 * This function generates both the heart rate data as well as the SpO2 data using
 * a series of filters and an algorithm that is properly explained in the following
 * blog:
 * https://devxplained.eu/en/blog/max3010x-pulse-oximeter-modules-part-4
 *
 * @param: None
 *
 * @return:max_data_t
 *
 *****************************************************************************/
void Max_30101_process();

max_data_t Max_30101_latest_data();
void Max_30101_reset_latest_data();


#endif /* SRC_I2C_MAX_H_ */
