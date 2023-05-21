/*
 * i2c_mpr.c
 *
 *  Created on: Mar 3, 2023
 *      Author: rampr
 *
 * Description: This code has i2c functionality. MPR sensor
 *
 * References: Slides from Lectures of Dave Sluiter
 */

#include "i2c_mpr.h"
#include "src/driver/i2c0.h"
#define INCLUDE_LOG_DEBUG 1
#include "src/driver/log.h"

#define MPR_I2CADDR       0x18

#define MPR_OPM           0xAA0000
#define MPR_OPM_0         ((MPR_OPM & 0xFF0000) >> 16)
#define MPR_OPM_1         ((MPR_OPM & 0x00FF00) >> 8)
#define MPR_OPM_2         (MPR_OPM & 0xFF)

#define MPR_BUSY_FLAG     0X20

#define OUTPUT_MAX        0xE66666
#define OUTPUT_MIN        0x19999A
#define MAXIMUM_PSI       25
#define MINIMUM_PSI       0
#define CALIB_MMHG        620


/* Transfer Data - MPR */
uint8_t OPM_DataBuffer[3] = {MPR_OPM_0, MPR_OPM_1, MPR_OPM_2};
uint8_t status_DataBuffer = 1;
uint8_t RP_DataBuffer[4] = {0, 0, 0, 0};


void MPR_read_pressure(){

  /* writing 0xAA 0x00 0x00 */

  i2c0_Write(MPR_I2CADDR, OPM_DataBuffer, sizeof(OPM_DataBuffer));

    /* Checking Status - Reading status register of 1 bit */
    i2c0_Read(MPR_I2CADDR, &status_DataBuffer, sizeof(status_DataBuffer));
  while ((status_DataBuffer & MPR_BUSY_FLAG) && (status_DataBuffer != 0xFF))
  {
      i2c0_Read(MPR_I2CADDR, &status_DataBuffer, sizeof(status_DataBuffer));
  }

  /* Read data*/
  i2c0_Read(MPR_I2CADDR, RP_DataBuffer, sizeof(RP_DataBuffer));

}

float MPR_report_pressure_in_Hg()
{
  uint32_t pressure_int = 0;
  float pressure_InHg = 0;
  float pressure_psi = 0;
  float pressure_float = 0;
  float pressure_mmHg = 0;

  /* Arrange MSB and LSB into pressure_int variable */
  pressure_int = ((RP_DataBuffer[1] << 16) | RP_DataBuffer[2] << 8 | RP_DataBuffer[3]);
  /*typecast to float*/
  pressure_float = (float) pressure_int;
  /*Pressure Calculation - Refer section 8.0 of data sheet*/
  pressure_psi = (pressure_float - OUTPUT_MIN) * (MAXIMUM_PSI - MINIMUM_PSI);
  pressure_psi = (pressure_psi / (OUTPUT_MAX - OUTPUT_MIN)) + MINIMUM_PSI;

  /*Conversion of PSI to mmHg*/
  pressure_mmHg = pressure_psi * 2.03602 * 25.4 - CALIB_MMHG;

//  LOG_INFO("Pressure = %fpsi\r\n", pressure_psi);
//  LOG_INFO("Pressure = %fInHg\r\n", pressure_InHg);
//  LOG_INFO("Pressure = %fInHg\r\n", pressure_mmHg);
  //displayPrintf(DISPLAY_ROW_4, "BP =  %d mmHg", (int)pressure_InHg);
  return pressure_mmHg;
}

