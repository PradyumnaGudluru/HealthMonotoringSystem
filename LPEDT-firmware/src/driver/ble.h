/***************************************************************************//**

 * Date:        19-09-2021
 * Author:      Pradyumna Gudluru
 * Description: This code has i2c functionality.
 *              It is used  for ECEN 5823 "IoT Embedded Firmware".
 *              BLE handle event is updated according to the change in connection handles.
 * References: Slides from Lectures of Dave Sluiter
 *              SoC example from demo code
 ******************************************************************************/
#include "sl_bt_api.h"

#include "gatt_db.h"
#include "app_log.h"
#include "app_assert.h"

#include "stdbool.h"
#include "lcd.h"

void handle_ble_event(sl_bt_msg_t *evt);

bool is_ble_connected();
void report_data_tmp_ble(int temp_c);
//void report_data_mpr_ble(sl_bt_msg_t *evt, float pressure);
void report_data_mpr_ble(float pressure);
void report_data_bpm_ble(int bpm);
void report_data_SpO2_ble(float spo2);
void read_data_time_ble(uint8_t* time);

#define UINT8_TO_BITSTREAM(p, n) { *(p)++ = (uint8_t)(n); }
#define UINT32_TO_BITSTREAM(p, n) { *(p)++ = (uint8_t)(n); *(p)++ = (uint8_t)((n) >> 8); \
*(p)++ = (uint8_t)((n) >> 16); *(p)++ = (uint8_t)((n) >> 24); }
#define UINT32_TO_FLOAT(m, e) (((uint32_t)(m) & 0x00FFFFFFU) | (uint32_t)((int32_t)(e) << 24))

