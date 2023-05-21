/*
 * i2c_mpr.h
 *
 *  Created on: Mar 3, 2023
 *      Author: rampr
 *
 * Description: This code has i2c functionality. MPR sensor
 *
 * References: Slides from Lectures of Dave Sluiter
 */

#ifndef SRC_I2C_MPR_H_
#define SRC_I2C_MPR_H_

#include "sl_i2cspm.h"
#include "em_i2c.h"


/***************************************************************************//**
 * @name I2C_Init
 *
 * @brief
 *   Initializes the I2C peripheral for the TMP117 temperature sensor.
 *
 * @param[in] none
 *
 * @return void
 ******************************************************************************/
void I2CInit_MPR();

/***************************************************************************//**
 * @name I2CTransferInitWrapper
 *
 * @brief
 *   Sets up the transfer sequence structure for the I2CSPM_Transfer call
 *   based on the parameters.
 *
 * @param[in/out] Data - I2C command or pointer to buffer to send/receive.
 * @param[in/out] ReadWrite - Read or Write I2C bit. Refer to I2CSPM_Transfer flags.
 * @param[in/out] DataLen - sizeof(Data)
 *
 * @return status - Transfer successful/unsuccesful
 ******************************************************************************/
uint8_t I2CTransferInitWrapper_MPR(uint8_t* Data, uint8_t ReadWrite, uint8_t DataLen, uint8_t ReadLen);

/***************************************************************************//**
 * @name readPressure
 *
 * @brief
 *   Reads the pressure from sensor.
 *
 * @param[in] none
 *
 * @return void
 ******************************************************************************/
void MPR_read_pressure();

/***************************************************************************//**
 * @name reportPressureInHg
 *
 * @brief
 *   Logs the pressure in InHg.
 *
 * @param[in] none
 *
 * @return pressure in Hg
 ******************************************************************************/
float MPR_report_pressure_in_Hg();

#endif /* SRC_I2C_MPR_H_ */
