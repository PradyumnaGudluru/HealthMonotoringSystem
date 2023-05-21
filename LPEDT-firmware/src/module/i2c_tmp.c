/***************************************************************************//**

 * Date:        3-2-23
 * Author:      Pradyumna Gudluru
 * Description: This code has i2c functionality. TMP 117
 *
 * References: Slides from Lectures of Dave Sluiter
 ******************************************************************************/

#include <src/driver/i2c0.h>
#include "i2c_tmp.h"
#define INCLUDE_LOG_DEBUG 1
#include "src/driver/log.h"


#define TMP117_I2CADDR    0x48
#define TMP117_EEPROM_UL  0x04
#define TMP117_TEMP       0x00
#define TMP117_CONFIG     0x01

//Shutdown mode
#define TMP117_SD         0x620
#define TMP117_SD_MSB     ((TMP117_SD & 0xFF00) >> 8)
#define TMP117_SD_LSB     (TMP117_SD & 0xFF)

//One shot mode
#define TMP117_OS         0xCA0
#define TMP117_OS_MSB     ((TMP117_OS & 0xFF00) >> 8)
#define TMP117_OS_LSB     (TMP117_OS & 0xFF)

I2CSPM_Init_TypeDef init_TMP;
I2C_TransferSeq_TypeDef TransferSeq_TMP;

/* Transfer Data */
static volatile uint8_t SD_DataBuffer[2] = {TMP117_SD_MSB, TMP117_SD_LSB};
static volatile uint8_t OS_DataBuffer[2] = {TMP117_OS_MSB, TMP117_OS_LSB};
static volatile uint8_t TP_DataBuffer[3] = {0, 0, 0};


void TMP117_set_shutdown_mode()
{
  i2c0_Write_Write(TMP117_I2CADDR, TMP117_CONFIG, SD_DataBuffer, sizeof(SD_DataBuffer));
}

void TMP117_set_oneshot_mode()
{
   if(i2c0_Write_Write(TMP117_I2CADDR, TMP117_CONFIG, OS_DataBuffer, sizeof(OS_DataBuffer))){
       LOG_INFO("error in configuring the TMP117\r\n");
   }
}

void TMP117_get_temperature()
{
  // Integrated Code
  for(int i=0;i<sizeof(TP_DataBuffer)/sizeof(uint8_t);i++){
      TP_DataBuffer[i] = 0;
  }

  if(i2c0_Write_Read(TMP117_I2CADDR, TMP117_TEMP, TP_DataBuffer, sizeof(TP_DataBuffer))){
      LOG_INFO("error in getting temperature value\r\n");
  }
}

int TMP117_report_temperature()
{
  int Calc_Temp;
  int Calc_Temp_F;
  uint16_t Read_Temp;

  /* Arrange MSB and LSB into Read_Temp variable */
  Read_Temp = ((TP_DataBuffer[0] << 8) | TP_DataBuffer[1]);

  /* Convert received value to degrees C and F */
  Calc_Temp = Read_Temp/0x80;
  Calc_Temp_F = ((Calc_Temp * 9)/5) + 32;

  return Calc_Temp_F;
}

