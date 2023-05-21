/*
 * i2c_max.c
 *
 *  Created on: Apr 1, 2023
 *      Author: Shuran Xu & Pradyumna Gudluru
 * 
 * @Author: Shuran Xu & Pradyumna Gudluru
 * @ref:
 * 1. MAX3010x Pulse Oximeter algorithm:
 * https://devxplained.eu/en/blog/max3010x-pulse-oximeter-modules-part-4
 * 2. SpO2 constant reference values:
 * https://www.analog.com/en/technical-articles/guidelines-for-spo2-measurement--maxim-integrated.html
 * 
 */

#include <src/driver/i2c1.h>
#include <src/driver/i2c0.h>
#include "i2c_max.h"
#include <stdbool.h>
#define INCLUDE_LOG_DEBUG         1
#include "src/driver/log.h"
#include "src/module/high_pass_filter.h"
#include "src/module/low_pass_filter.h"
#include "src/module/moving_average_filter.h"
#include "src/module/min_max_avg_statistic.h"
#include "src/driver/irq.h"
#include "src/module/differentiator.h"

/*****************************************************************************
 * Macro definitions for i2c_max.c
 *****************************************************************************/

#define MAX_BUFFER_SIZE           (128)
#define FIFO_CONFIG_REG           0x08
#define MODE_CONFIG_REG           0x09
#define RED_LED_PULSE_AMP         0x0C
#define IR_LED_PULSE_AMP          0x0D
#define MULTI_LED_MODE_CONTROL    0x11
#define SPO2_CONFIG_REG           0x0A

/*****************************************************************************
 * Constant values used for heart rate and pulse oximetry measurement
 *****************************************************************************/

const float kSamplingFrequency = 400.0;

// Finger Detection Threshold and Cool-down
const unsigned long kFingerThreshold = 10000;
const unsigned long kFingerCooldownMs = 500;

// Edge Detection Threshold
const float kEdgeThreshold = -2000.0;

// R value to SpO2 calibration factors
// See https://www.maximintegrated.com/en/design/technical-documents/app-notes/6/6845.html
//const float kSpO2_A = 1.5958422;
//const float kSpO2_B = -34.6596622;
////const float kSpO2_C = 112.6898759;
//const float kSpO2_C = 121.6898759;


const float kSpO2_A = 1.5958422;
const float kSpO2_B = -16.3068;
const float kSpO2_C = 110.6898;


// Timestamp of the last heartbeat
static volatile uint32_t last_heartbeat = 0;

// Timestamp for finger detection
static volatile uint32_t finger_timestamp = 0;
static volatile bool finger_detected = false;

// Last diff to detect zero crossing
static volatile float last_diff = 0;
static volatile bool crossed = false;
static volatile uint32_t crossed_time = 0;
static max_data_t latest_max_data = {0.0,0};
/*****************************************************************************
 * User-defined structs
 *****************************************************************************/

typedef struct{
  uint32_t buff[MAX_BUFFER_SIZE];
  uint32_t iter;
}max_buff_t;

typedef struct{
  max_data_t buff[MAX_BUFFER_SIZE];
  uint32_t iter;
}max_data_buff_t;

static volatile max_buff_t ir_buffer;
static volatile max_buff_t red_buffer;
static volatile max_data_buff_t max_data_buffer;

// Differentiator instance
static Differentiator_t diff;

// Statistic for pulse oximetry
static MinMaxAvgStatistic_t stat_red;
static MinMaxAvgStatistic_t stat_ir;

/****************************************************************************
 * This function initializes the sensor registers and initiates the sensor to
 * start taking readings
 *
 * @param:
 *      no params
 * @return:
 *      no params
 *****************************************************************************/

void MAX_30101_Init()
{

  init_Dfifferentiator(kSamplingFrequency, &diff);
  init_MinMaxAvgStatistic(0, 0, 0, 0, &stat_red);
  init_MinMaxAvgStatistic(0, 0, 0, 0, &stat_ir);

  // Checking initial values
  uint8_t write = 0x00;
  i2c1_Write_Write(MAX_30101_ADDRESS, 0x08, &write, sizeof(write));
  i2c1_Write_Write(MAX_30101_ADDRESS, 0x09, &write, sizeof(write));
  i2c1_Write_Write(MAX_30101_ADDRESS, 0x0C, &write, sizeof(write));
  i2c1_Write_Write(MAX_30101_ADDRESS, 0x11, &write, sizeof(write));
  i2c1_Write_Write(MAX_30101_ADDRESS, 0x00, &write, sizeof(write));
  i2c1_Write_Write(MAX_30101_ADDRESS, 0x04, &write, sizeof(write));
  i2c1_Write_Write(MAX_30101_ADDRESS, 0x06, &write, sizeof(write));


  // FIFO Config - 0x08
  // setting: Sample average = 8
  // enable rollover
  // 32 samples to be read
  write = 0x50;  // averaging 4 samples //00010000 no averaging
  i2c1_Write_Write(MAX_30101_ADDRESS, FIFO_CONFIG_REG, &write, sizeof(write));

  // LED Pulse Amp - RED - 0x0C
  write = 0xF;
  i2c1_Write_Write(MAX_30101_ADDRESS, RED_LED_PULSE_AMP, &write, sizeof(write));

  // LED Pulse Amp - IR - 0x0D
  write = 0xF;
  i2c1_Write_Write(MAX_30101_ADDRESS, IR_LED_PULSE_AMP, &write, sizeof(write));

  // LED Pulse Amp - MULTI LED - 0X11
  // SLOT1: enable for RED LED
  // SLOT2: enable for IR LED
  write = 0x21;
  i2c1_Write_Write(MAX_30101_ADDRESS, MULTI_LED_MODE_CONTROL, &write, sizeof(write));

  // SPO2 - 0x0A
  // set the sampling rate 400
  // LED ADC resolution of 18 bits
  write = 0x0F;  //  00001011 for 200 sampling rate
  i2c1_Write_Write(MAX_30101_ADDRESS, SPO2_CONFIG_REG, &write, sizeof(write));

  // Checking final values
//  i2c1_Write_Read(MAX_30101_ADDRESS, 0x08, &read, sizeof(read));
//  i2c1_Write_Read(MAX_30101_ADDRESS, 0x09, &read, sizeof(read));
//  i2c1_Write_Read(MAX_30101_ADDRESS, 0x0C, &read, sizeof(read));
//  i2c1_Write_Read(MAX_30101_ADDRESS, 0x11, &read, sizeof(read));

  // Reset the FIFO_RD_PTR to 0
  write = 0;
  i2c1_Write_Write(MAX_30101_ADDRESS, 0x06, &write, sizeof(write));
  // Reset the FIFO_WR_PTR to 0
  i2c1_Write_Write(MAX_30101_ADDRESS, 0x04, &write, sizeof(write));

  // Start the sensor
//  i2c1_Write_Read (MAX_30101_ADDRESS, MODE_CONFIG_REG, &read, sizeof(read));
//  write = read & 0x7F;
//  i2c1_Write_Write(MAX_30101_ADDRESS, MODE_CONFIG_REG, &write, sizeof(write));
//
  // Mode Config - 0x09
  //mode: SpO2 mode
  // shut down the sensor
  write = 0x83; //0x03;
  i2c1_Write_Write(MAX_30101_ADDRESS, MODE_CONFIG_REG, &write, sizeof(write));

}


/*****************************************************************************
 * This function gets the value that is stored in a particular register in the
 * MAX30101 sensor
 *
 * @param:
 *      reg:              The register that you wish to read the value from
 *      read_data:        Pointer to the data that is read from the sensor
 *      nbytes_read_data: Number of bytes that is read from the sensor with
 *                        read_data as the base pointer
 *
 * @return:
 *      no params
 *****************************************************************************/
void MAX_30101_Get_Reg_Val (uint8_t reg, uint8_t* read_data, size_t nbytes_read_data)
{
  for (int i = 0; i < (int)nbytes_read_data; i++)
    {
      printf("Reg 0x%02x :: Read %d:0x%02x\t", reg, i+1, *(read_data+i));
    }
  printf("\n\n");
}


/*****************************************************************************
 * This function shuts the sensor down. This functions will retain the register
 * values while just saving power. Upon MAX_30101_PowerUp() the sensor will
 * start taking readings with the same configurations
 *
 * @param:
 *      no params
 *
 * @return:
 *      no params
 *****************************************************************************/
void MAX_30101_ShutDown()
{
  uint8_t write = 0x83; // Shutdown
  i2c1_Write_Write(MAX_30101_ADDRESS, MODE_CONFIG_REG, &write, sizeof(write));
}

/*****************************************************************************
 * This function powers the sensor up. This functions will retain the register
 * values while just saving power. Upon MAX_30101_PowerUp() the sensor will
 * start taking readings with the same configurations
 *
 * @param:
 *      no params
 *
 * @return:
 *      no params
 *****************************************************************************/
void MAX_30101_PowerUp()
{
  uint8_t write = 0x03; // Power Up
  i2c1_Write_Write(MAX_30101_ADDRESS, MODE_CONFIG_REG, &write, sizeof(write));
}

/*****************************************************************************
 * This function resets the sensor and sets all the registers back to its
 * default values.
 *
 * @param:
 *      no params
 *
 * @return:
 *      no params
 *****************************************************************************/
void MAX_30101_Reset()
{
  uint8_t write = 0xC2; // Reset
  i2c1_Write_Write(MAX_30101_ADDRESS, 0x09, &write, sizeof(write));
  //timerWaitUs(5000);
}

/*
 * This function is a temporary function to read the values of Max30101.
 * As per the pseudo code given by data sheet
 *
 */
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

//bool Max_30101_FIFO_and_Finger_Detection(){
//
//  // Reset values if the finger is removed
//  reset_Differentiator(&diff);
//  reset_MinMaxAvgStatistic(&stat_red);
//  reset_MinMaxAvgStatistic(&stat_ir);
//
//  uint8_t write_ptr, read_ptr;
//  uint32_t reading = 0;
//  int8_t sample_numbers;
//
//  // Get the FIFO_RD_PTR:
//  I2C_TransferReturn_TypeDef rc = i2c1_Write_Read(MAX_30101_ADDRESS, 0x06, &read_ptr, sizeof(read_ptr));
//  if(rc != 0){
//      LOG_INFO("rd ptr failed, rc = %d\r\n", rc);
//      return false;
//  }
//
//  // Get the FIFO_WR_PTR:
//  rc = i2c1_Write_Read(MAX_30101_ADDRESS, 0x04, &write_ptr, sizeof(write_ptr));
//  if(rc != 0){
//      LOG_INFO("wr ptr failed, rc = %d\r\n", rc);
//      return false;
//  }
//
//  // The central processor evaluates the number of samples to be read from the FIFO
//  sample_numbers = abs(16 + ((write_ptr - read_ptr)%16));
//
//  // reset iterators for ir_buffer and red_buffer
//  ir_buffer.iter = 0;
//  red_buffer.iter = 0;
//
//  uint8_t data = 0x7;
//  rc = i2c1_Write(MAX_30101_ADDRESS, &data, sizeof(data));
//  if(rc != 0){
//      LOG_INFO("wr failed, rc = %d\r\n", rc);
//      return false;
//  }
//
//  //Read NUM_SAMPLES_TO_READ samples from the FIFO:
//  for (int i = 0; i<sample_numbers; ++i)
//  {
//      uint8_t temp[6] = {0};
//      rc = i2c1_Read(MAX_30101_ADDRESS, temp, sizeof(temp));
//      if(rc != 0){
//          LOG_INFO("rd failed, rc = %d\r\n", rc);
//          return false;
//      }
//
//      reading = (((uint32_t)temp[0]) << 16 | ((uint32_t)temp[1]) << 8 | (uint32_t)temp[2]);
//      red_buffer.buff[red_buffer.iter++] = reading;
//      reading = (((uint32_t)temp[3]) << 16 | ((uint32_t)temp[4]) << 8 | (uint32_t)temp[5]);
//      ir_buffer.buff[ir_buffer.iter++] = reading;
//
//      // Detect Finger using raw sensor value
//      if(red_buffer.buff[red_buffer.iter-1] > kFingerThreshold) {
//
//        if(letimerMilliseconds() - finger_timestamp > kFingerCooldownMs) {
//            return true;
//        }
//      }
//
//      finger_timestamp = letimerMilliseconds();
//   }
//
//  return false;
//}


int Max_30101_FIFO(){

  uint8_t write_ptr, read_ptr;
  uint32_t reading = 0;
  int8_t sample_numbers;

  // Get the FIFO_RD_PTR:
  I2C_TransferReturn_TypeDef rc = i2c1_Write_Read(MAX_30101_ADDRESS, 0x06, &read_ptr, sizeof(read_ptr));
  if(rc != 0){
      LOG_INFO("rd ptr failed, rc = %d\r\n", rc);
      return 1;
  }

  // Get the FIFO_WR_PTR:
  rc = i2c1_Write_Read(MAX_30101_ADDRESS, 0x04, &write_ptr, sizeof(write_ptr));
  if(rc != 0){
      LOG_INFO("wr ptr failed, rc = %d\r\n", rc);
      return 1;
  }

  // The central processor evaluates the number of samples to be read from the FIFO
  sample_numbers = abs(16 + ((write_ptr - read_ptr)%16));

  // reset iterators for ir_buffer and red_buffer
  ir_buffer.iter = 0;
  red_buffer.iter = 0;

  uint8_t data = 0x7;
  rc = i2c1_Write(MAX_30101_ADDRESS, &data, sizeof(data));
  if(rc != 0){
      LOG_INFO("wr failed, rc = %d\r\n", rc);
      return 1;
  }

  //Read NUM_SAMPLES_TO_READ samples from the FIFO:
  for (int i = 0; i<sample_numbers; ++i)
  {
      uint8_t temp[6] = {0};
      rc = i2c1_Read(MAX_30101_ADDRESS, temp, sizeof(temp));
      if(rc != 0){
          LOG_INFO("rd failed, rc = %d\r\n", rc);
          return 1;
      }

      reading = (((uint32_t)temp[0]) << 16 | ((uint32_t)temp[1]) << 8 | (uint32_t)temp[2]);
      red_buffer.buff[red_buffer.iter++] = reading;
      reading = (((uint32_t)temp[3]) << 16 | ((uint32_t)temp[4]) << 8 | (uint32_t)temp[5]);
      ir_buffer.buff[ir_buffer.iter++] = reading;
   }

  return 0;
}

/*****************************************************************************
 * This function generates both the heart rate data as well as the SpO2 data using
 * a series of filters and an algorithm that is properly explained in the following
 * blog:
 * https://devxplained.eu/en/blog/max3010x-pulse-oximeter-modules-part-4
 *
 * @param: None
 *
 * @return: None
 *
 *****************************************************************************/

max_data_t Max_30101_latest_data(){
  return latest_max_data;
}


void Max_30101_reset_latest_data()
{
  latest_max_data.bpm = 0;
  latest_max_data.spo2 = 0;
}


void Max_30101_process(){

  // reset max_data_buffer's iterator
  max_data_buffer.iter = 0;

  float current_value_red;
  float current_value_ir;
  const int length = ir_buffer.iter;
  float sum_spo2 = 0;
  int sum_bpm = 0;

  for(int i = 0; i < length; i++){

      current_value_red = red_buffer.buff[i];
      current_value_ir = ir_buffer.buff[i];

      // Detect Finger using raw sensor value
      if(red_buffer.buff[i] > kFingerThreshold) {

        if(letimerMilliseconds() - finger_timestamp > kFingerCooldownMs) {
          finger_detected = true;
        }
      }
      else {

          // Reset values if the finger is removed
          reset_Differentiator(&diff);
          reset_MinMaxAvgStatistic(&stat_red);
          reset_MinMaxAvgStatistic(&stat_ir);

          finger_detected = false;
          finger_timestamp = letimerMilliseconds();
      }

      if(finger_detected) {

            // Statistics for pulse oximetry
            process_MinMaxAvgStatistic(current_value_red, &stat_red);
            process_MinMaxAvgStatistic(current_value_ir, &stat_ir);

            // Heart beat detection using value for red LED
            float current_diff = process_Differentiator(current_value_red, &diff);

            // Detect Heartbeat - Zero-Crossing
            if((last_diff > 0 && current_diff < 0)){
              crossed = true;
              crossed_time = letimerMilliseconds();
            }

            if(current_diff > 0) {
                crossed = false;
              }

            // Detect Heartbeat - Falling Edge Threshold
            if(crossed && current_diff < kEdgeThreshold) {
              if((last_heartbeat != 0) && (crossed_time - last_heartbeat > 300)) {

                // Show Results
                int bpm = 60000/(crossed_time - last_heartbeat);
                float rred = (maximum(&stat_red) - minimum(&stat_red)) / average(&stat_red);
                float rir = (maximum(&stat_ir) - minimum(&stat_ir)) / average(&stat_ir);
                float r = rred/rir;
                float spo2 = kSpO2_A * r * r + kSpO2_B * r + kSpO2_C;

                if(bpm > 50 && bpm < 200) {

                    max_data_buffer.buff[max_data_buffer.iter].bpm = bpm;
                    max_data_buffer.buff[max_data_buffer.iter].spo2 = spo2;
                    max_data_buffer.iter++;

                    sum_bpm += bpm;
                    sum_spo2 += spo2;
                }

                // Reset statistic
                reset_MinMaxAvgStatistic(&stat_red);
                reset_MinMaxAvgStatistic(&stat_ir);
              }

              crossed = false;
              last_heartbeat = crossed_time;
            }

          last_diff = current_diff;
        }
     }

  if(sum_bpm != 0 && sum_spo2 != 0){
      latest_max_data.spo2 = sum_spo2 / max_data_buffer.iter;
      latest_max_data.bpm = sum_bpm / max_data_buffer.iter;
  }

  return;
}


//void Max_30101_process(){
//
//  // reset max_data_buffer's iterator
//  max_data_buffer.iter = 0;
//
//  float current_value_red;
//  float current_value_ir;
//  const int length = ir_buffer.iter;
//  float sum_spo2 = 0;
//  int sum_bpm = 0;
//
//  for(int i = 0; i < length; i++){
//
//      current_value_red = red_buffer.buff[i];
//      current_value_ir = ir_buffer.buff[i];
//
//      // Statistics for pulse oximetry
//      process_MinMaxAvgStatistic(current_value_red, &stat_red);
//      process_MinMaxAvgStatistic(current_value_ir, &stat_ir);
//
//      // Heart beat detection using value for red LED
//      float current_diff = process_Differentiator(current_value_red, &diff);
//
//      // Detect Heartbeat - Zero-Crossing
//      if((last_diff > 0 && current_diff < 0)){
//        crossed = true;
//        crossed_time = letimerMilliseconds();
//      }
//
//      if(current_diff > 0) {
//          crossed = false;
//        }
//
//      // Detect Heartbeat - Falling Edge Threshold
//      if(crossed && current_diff < kEdgeThreshold) {
//        if((last_heartbeat != 0) && (crossed_time - last_heartbeat > 300)) {
//
//          // Show Results
//          int bpm = 60000/(crossed_time - last_heartbeat);
//          float rred = (maximum(&stat_red) - minimum(&stat_red)) / average(&stat_red);
//          float rir = (maximum(&stat_ir) - minimum(&stat_ir)) / average(&stat_ir);
//          float r = rred/rir;
//          float spo2 = kSpO2_A * r * r + kSpO2_B * r + kSpO2_C;
//
//          if(bpm > 50 && bpm < 200) {
//
//              max_data_buffer.buff[max_data_buffer.iter].bpm = bpm;
//              max_data_buffer.buff[max_data_buffer.iter].spo2 = spo2;
//              max_data_buffer.iter++;
//
//              sum_bpm += bpm;
//              sum_spo2 += spo2;
//          }
//
//          // Reset statistic
//          reset_MinMaxAvgStatistic(&stat_red);
//          reset_MinMaxAvgStatistic(&stat_ir);
//        }
//
//        crossed = false;
//        last_heartbeat = crossed_time;
//      }
//
//    last_diff = current_diff;
//  }
//
//
//  if(sum_bpm != 0 && sum_spo2 != 0){
//      latest_max_data.spo2 = sum_spo2 / max_data_buffer.iter;
//      latest_max_data.bpm = sum_bpm / max_data_buffer.iter;
//  }
//
//  return;
//}

