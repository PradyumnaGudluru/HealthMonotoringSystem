/***************************************************************************//**

 * Date:        3-2-23
 * Author:      Pradyumna Gudluru
 * Description: This code has i2c functionality. TMP 117
 *
 * References: Slides from Lectures of Dave Sluiter
 ******************************************************************************/

#ifndef SRC_I2C_TMP_H_
#define SRC_I2C_TMP_H_

#include "sl_i2cspm.h"
#include "em_i2c.h"
//#include "gpio.h"
#include "src/driver/timers.h"

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
void I2CInit_TMP();

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
uint8_t I2CTransferInitWrapper_TMP(uint8_t* Data, uint8_t ReadWrite, uint8_t DataLen, uint8_t ReadLen);

/***************************************************************************//**
 * @name TMP117_get_temperature
 *
 * @brief
 *   Gets the Temperature value from the TMP sensor.
 *
 * @param[in] none
 *
 * @return void
 ******************************************************************************/
void TMP117_get_temperature();

/***************************************************************************//**
 * @name TMP117_set_oneshot_mode
 *
 * @brief
 *  Set to oneshot mode, Configuration register 0x0CA0
 *
 * @param[in] none
 *
 * @return void
 ******************************************************************************/
void TMP117_set_oneshot_mode();

/***************************************************************************//**
 * @name setShutdownModeTMP117
 *
 * @brief
 *   Sets to Shut Down Mode, Config register 0x0620
 *
 * @param[in] none
 *
 * @return void
 ******************************************************************************/
void TMP117_set_shutdown_mode();

/***************************************************************************//**
 * @name reportTemperatureTMP117
 *
 * @brief
 *   Logs the temperature from the TMP sensor.
 *
 * @param[in] none
 *
 * @return Calculated Temperature in F
 ******************************************************************************/
int TMP117_report_temperature();


#endif /* SRC_I2C_TMP_H_ */
