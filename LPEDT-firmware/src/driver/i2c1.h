/*
 * i2c1.h
 *
 *  Created on: Apr 10, 2023
 *      Author: rampr
 */

/***************************************************************************//**

 * Date:        19-09-2021
 * Author:      Pradyumna Gudluru
 * Description: This code has i2c functionality.
 *              It is used  for ECEN 5823 "IoT Embedded Firmware".
 *              I2C is initialized, the transfer function is set with udating the sequence typedef.
 *
 ******************************************************************************/

#ifndef SRC_DRIVER_I2C1_H_
#define SRC_DRIVER_I2C1_H_


#include "sl_i2cspm.h"
#include "app.h"
#include "timers.h"
#include "scheduler.h"
#include "gpio.h"

/**************************************************************************//**
 * This function initialises the I2C transfer. Sets the appropriate pins and
 * port numbers to perform I2C
 *
 * Refer to the data sheet for the details listed below
 *****************************************************************************/
void i2c1_Init();

/**************************************************************************//**
 * This function sends a command to the bus with the address of the slave
 * and also sends a command that needs to be performed by the slave
 *
 * TrasnferSequence struct is used to make the data in the apt format to send
 * via bus
 *
 * TransferReturn struct is used to initialize the transfer and also store the
 * status of the transfer.
 *****************************************************************************/

int i2c1_Write(uint8_t addr, uint8_t* wr_buff, uint8_t wr_buff_len);

/**************************************************************************//**
 * This function receives the data sent by the slave and stores in a an array
 * this array is then converted into a number based on the number of bits
 * required that is passed
 *
 * TrasnferSequence struct is used to make the data in the apt format to send
 * and receive via bus
 *
 * TransferReturn struct is used to initialize the transfer and also store the
 * status of the transfer.
 *
 * The slave sends out a 14 bit data for the command sent above. the first byte
 * received is the MS Byte and the second in the LS Byte
 *
 * @param:
 *
 *****************************************************************************/

int i2c1_Read(uint8_t addr, uint8_t* rd_buff, uint8_t rd_buff_len);


/**************************************************************************//**
 * This function sends a command to the bus with the address of the slave
 * and also sends the register to read data from. This also holds a pointer
 * to store the returning data.
 *
 * TrasnferSequence struct is used to make the data in the apt format to send
 * via bus
 *
 * TransferReturn struct is used to initialize the transfer and also store the
 * status of the transfer.
 *****************************************************************************/

int i2c1_Write_Read (uint8_t addr, uint8_t reg, uint8_t* read_data, size_t nbytes_read_data);

/**************************************************************************//**
 * This function sends a command to the bus with the address of the slave
 * and also sends a command that needs to be performed by the slave
 *
 * TrasnferSequence struct is used to make the data in the apt format to send
 * via bus
 *
 * TransferReturn struct is used to initialize the transfer and also store the
 * status of the transfer.
 *****************************************************************************/

int i2c1_Write_Write (uint8_t addr, uint8_t reg, uint8_t* write_data, size_t nbytes_write_data);

#endif /* SRC_DRIVER_I2C1_H_ */
