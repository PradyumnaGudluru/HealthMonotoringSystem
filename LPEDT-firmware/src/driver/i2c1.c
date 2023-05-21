/*
 * i2c1.c
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
 * References: Slides from Lectures of Dave Sluiter
 ******************************************************************************/
#include "i2c1.h"
#include "src/driver/log.h"

/**************************************************************************//**
 * GLOBAL Variable Declarations
 *****************************************************************************/
static I2C_TransferReturn_TypeDef transferStatus_i2c1; // A struct that keeps track of the transfer status
static I2C_TransferSeq_TypeDef transferSequence_i2c1; // A struct that stores the transfer sequence that the data/command has to be transfered


/**************************************************************************//**
 * This function initialises the I2C transfer. Sets the appropriate pins and
 * port numbers to perform I2C
 *
 * Refer to the data sheet for the details listed below
 *****************************************************************************/
void i2c1_Init()
{
//  I2CSPM_Init_TypeDef I2C_Config;
//  I2C_Config.port = I2C1,
//  I2C_Config.sclPort = gpioPortC,
//  I2C_Config.sclPin = 10,
//  I2C_Config.sdaPort = gpioPortC,
//  I2C_Config.sdaPin = 11,
//  I2C_Config.portLocationScl = 18,
//  I2C_Config.portLocationSda = 20,
//  I2C_Config.i2cRefFreq = 0,
//  I2C_Config.i2cMaxFreq = I2C_FREQ_FAST_MAX,
//  I2C_Config.i2cClhr = i2cClockHLRFast;
//
//  //  Passing the struct to the initialization function
//  I2CSPM_Init(&I2C_Config);

  I2CSPM_Init_TypeDef I2C_Config;
    I2C_Config.port = I2C1,
    I2C_Config.sdaPort = gpioPortC,
    I2C_Config.sdaPin = 10,
    I2C_Config.sclPort = gpioPortC,
    I2C_Config.sclPin = 11,
    I2C_Config.portLocationScl = 19,
    I2C_Config.portLocationSda = 19,
    I2C_Config.i2cRefFreq = 0,
    I2C_Config.i2cMaxFreq = I2C_FREQ_FAST_MAX,
    I2C_Config.i2cClhr = i2cClockHLRFast;

    //  Passing the struct to the initialization function
    I2CSPM_Init(&I2C_Config);

}

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

int i2c1_Write(uint8_t addr, uint8_t* wr_buff, uint8_t wr_buff_len)
{
  transferSequence_i2c1.flags = I2C_FLAG_WRITE, // Write command
  transferSequence_i2c1.addr = (addr<<1), // Slave address needs to be left shift by one bit
  transferSequence_i2c1.buf[0].data = wr_buff, // Passing the pointer that has the command data stored
  transferSequence_i2c1.buf[0].len = wr_buff_len; // Length of the command data

//  if(!NVIC_GetEnableIRQ(I2C1_IRQn)){
//        NVIC_ClearPendingIRQ(I2C1_IRQn);
//        NVIC_EnableIRQ(I2C1_IRQn);
//    }

     // This will initialize the write command on to the bus
    I2C_TransferReturn_TypeDef trans_ret = I2C_TransferInit(I2C1,&transferSequence_i2c1);

    // Sending data
    while (trans_ret == i2cTransferInProgress)
    {
        trans_ret = I2C_Transfer(I2C1);
    }

    return trans_ret == i2cTransferDone ? 0 : trans_ret;
}
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
 *      len is the number of bytes that is expected to be received from the
 *      slave.
 *
 * @return:
 *      returns the temperature in degree celsius
 *****************************************************************************/
int i2c1_Read(uint8_t addr, uint8_t* rd_buff, uint8_t rd_buff_len)
{
  transferSequence_i2c1.flags = I2C_FLAG_READ, // Read command
  transferSequence_i2c1.addr = (addr<<1), // Slave address needs to be left shift by one bit
  transferSequence_i2c1.buf[0].data = rd_buff, // Passing the array that will store the incoming data
  transferSequence_i2c1.buf[0].len = rd_buff_len; // Passing the size of the array

//  if(!NVIC_GetEnableIRQ(I2C1_IRQn)){
//        NVIC_ClearPendingIRQ(I2C1_IRQn);
//        NVIC_EnableIRQ(I2C1_IRQn);
//    }

    // This will initialize the write command on to the bus
    I2C_TransferReturn_TypeDef trans_ret = I2C_TransferInit(I2C1,&transferSequence_i2c1);

    // Sending data
    while (trans_ret == i2cTransferInProgress)
    {
        trans_ret = I2C_Transfer(I2C1);
    }

    return trans_ret == i2cTransferDone ? 0 : trans_ret;
}

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

int i2c1_Write_Read (uint8_t addr, uint8_t reg, uint8_t* read_data, size_t nbytes_read_data)
{
  transferSequence_i2c1.flags = I2C_FLAG_WRITE_READ, // Write command
  transferSequence_i2c1.addr = (addr<<1), // Slave address needs to be left shift by one bit
  transferSequence_i2c1.buf[0].data = &reg, // Passing the pointer that has the command data stored
  transferSequence_i2c1.buf[0].len = sizeof(reg); // Length of the command data
  transferSequence_i2c1.buf[1].data = read_data, // Passing the pointer that has the command data stored
  transferSequence_i2c1.buf[1].len = nbytes_read_data;

//  if(!NVIC_GetEnableIRQ(I2C1_IRQn)){
//        NVIC_ClearPendingIRQ(I2C1_IRQn);
//        NVIC_EnableIRQ(I2C1_IRQn);
//    }

    // This will initialize the write command on to the bus
    I2C_TransferReturn_TypeDef trans_ret = I2C_TransferInit(I2C1,&transferSequence_i2c1);

    // Sending data
    while (trans_ret == i2cTransferInProgress)
    {
        trans_ret = I2C_Transfer(I2C1);
    }

    return trans_ret == i2cTransferDone ? 0 : trans_ret;
}

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
int i2c1_Write_Write (uint8_t addr, uint8_t reg, uint8_t* write_data, size_t nbytes_write_data)
{
  transferSequence_i2c1.flags = I2C_FLAG_WRITE_WRITE, // Write command
  transferSequence_i2c1.addr = (addr<<1), // Slave address needs to be left shift by one bit
  transferSequence_i2c1.buf[0].data = &reg, // Passing the pointer that has the command data stored
  transferSequence_i2c1.buf[0].len = sizeof(reg); // Length of the command data
  transferSequence_i2c1.buf[1].data = write_data, // Passing the pointer that has the command data stored
  transferSequence_i2c1.buf[1].len = nbytes_write_data;
//
//  if(!NVIC_GetEnableIRQ(I2C1_IRQn)){
//      NVIC_ClearPendingIRQ(I2C1_IRQn);
//      NVIC_EnableIRQ(I2C1_IRQn);
//  }

  // This will initialize the write command on to the bus
  I2C_TransferReturn_TypeDef trans_ret = I2C_TransferInit(I2C1,&transferSequence_i2c1);

  // Sending data
  while (trans_ret == i2cTransferInProgress)
  {
      trans_ret = I2C_Transfer(I2C1);
  }

  return trans_ret == i2cTransferDone ? 0 : trans_ret;
}
