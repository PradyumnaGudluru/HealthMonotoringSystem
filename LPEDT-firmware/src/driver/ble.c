/***************************************************************************//**

 * Date:        19-09-2021
 * Author:      Pradyumna Gudluru
 * Description: This code has i2c functionality.
 *              It is used  for ECEN 5823 "IoT Embedded Firmware".
 *              BLE handle event is updated according to the change in connection handles.
 * References: Slides from Lectures of Dave Sluiter
 *              SoC example from demo code
 ******************************************************************************/
#include "ble.h"

#define INCLUDE_LOG_DEBUG 1
#include "src/driver/log.h"

// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;
static volatile bool connect_status = false;

static uint8_t client_config_flag = 0;
static uint8_t status_flags = 0;
static uint8_t openedConnection = 0;

/***************************************************************************//**
  void handle_ble_event(sl_bt_msg_t *evt);
 * Description: void handle_ble_event is called from sl_bt_on_event for every cycle.
 *              This works on the states of events of bluetooth.
 ******************************************************************************/
bool is_ble_connected()
{
  return connect_status;
}

void handle_ble_event(sl_bt_msg_t *evt) {

  sl_status_t sc;
    bd_addr address;
    uint8_t address_type;
    uint8_t system_id[8];

    // Handle stack events
    switch (SL_BT_MSG_ID(evt->header)) {

      // ******************************************************
      // Events common to both Servers and Clients
      // ******************************************************

      // Boot Event
      case sl_bt_evt_system_boot_id:

        displayInit(); // LCD application A6 instruction
        LOG_INFO("Bluetooth stack booted: v%d.%d.%d-b%d\r\n",
                     evt->data.evt_system_boot.major,
                     evt->data.evt_system_boot.minor,
                     evt->data.evt_system_boot.patch,
                     evt->data.evt_system_boot.build);

        // Extract unique ID from BT Address.
        sc = sl_bt_system_get_identity_address(&address, &address_type);
        app_assert_status(sc);

        //bt_addr = gecko_cmd_system_get_bt_address();
        displayPrintf(DISPLAY_ROW_1, "M WIZ");

        displayPrintf(DISPLAY_ROW_ASSIGNMENT, "EMBEDDED WIZARDS");

        // Pad and reverse unique ID to get System ID.
        system_id[0] = address.addr[5];
        system_id[1] = address.addr[4];
        system_id[2] = address.addr[3];
        system_id[3] = 0xFF;
        system_id[4] = 0xFE;
        system_id[5] = address.addr[2];
        system_id[6] = address.addr[1];
        system_id[7] = address.addr[0];

        sc = sl_bt_gatt_server_write_attribute_value(gattdb_system_id,
                                                     0,
                                                     sizeof(system_id),
                                                     system_id);
        app_assert_status(sc);

        LOG_INFO("Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                     address_type ? "static random" : "public device",
                     address.addr[5],
                     address.addr[4],
                     address.addr[3],
                     address.addr[2],
                     address.addr[1],
                     address.addr[0]);

        // Create an advertising set.
        sc = sl_bt_advertiser_create_set(&advertising_set_handle);
        app_assert_status(sc);

        // Set advertising interval to 250ms.
        sc = sl_bt_advertiser_set_timing(
          advertising_set_handle, // advertising set handle
          400, // min. adv. interval (milliseconds * 1.6)
          400, // max. adv. interval (milliseconds * 1.6)
          0,   // adv. duration
          0);  // max. num. adv. events
        app_assert_status(sc);

        // Start general advertising and enable connections.
        sc = sl_bt_advertiser_start(
          advertising_set_handle,
          sl_bt_advertiser_general_discoverable,
          sl_bt_advertiser_connectable_scannable);
        app_assert_status(sc);
        LOG_INFO("Started advertising\r\n");

        displayPrintf(DISPLAY_ROW_CONNECTION, "Advertising");
        break;

      // -------------------------------
      // This event indicates that a new connection was opened.
      case sl_bt_evt_connection_opened_id:
        LOG_INFO("Connection opened\r\n");
        displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");


        sc = sl_bt_advertiser_stop(advertising_set_handle);

        openedConnection = evt->data.evt_connection_opened.connection;
        app_assert_status(sc);

        // Connection Set parameters
        sc = sl_bt_connection_set_parameters(evt->data.evt_connection_opened.connection,
                                        60,                     /*Time = Value x 1.25 ms  Required time = 75ms*/
                                        60,                     /*Time = Value x 1.25 ms  Required time = 75ms*/
                                        4,                      /*latency = Total latency time/ connection interval - 1 : 300ms/75ms - 1 = 3*/
                                        76,                     /*timeout = (1+latency) * (max_connection_interval)*2 : (1+4)*75*2 = 750ms; value = 750/10 = 75ms */
                                        0,
                                        0xffff);

        app_assert_status(sc);
        connect_status = true;
        break;

      // Connection Closed event
      case sl_bt_evt_connection_closed_id:
        LOG_INFO("Connection closed\r\n");
        displayPrintf(DISPLAY_ROW_BP, "");
        displayPrintf(DISPLAY_ROW_HR, "");
        displayPrintf(DISPLAY_ROW_SPo2, "");
        displayPrintf(DISPLAY_ROW_TMP, "");

        /* Clear the flag bits */
        client_config_flag = 0;
        status_flags = 0;

        // Begin Advertising
        sc = sl_bt_advertiser_start(
          advertising_set_handle,
          sl_bt_advertiser_general_discoverable,
          sl_bt_advertiser_connectable_scannable);
        app_assert_status(sc);
        LOG_INFO("Started advertising\r\n");
        displayPrintf(DISPLAY_ROW_CONNECTION, "Advertising");
        connect_status = false;
        break;

        // Connection Parameters event
      case sl_bt_evt_connection_parameters_id:
        /* Informational. Triggered whenever the connection parameters are changed and at any time a connection is established */
//        LOG_INFO("Connection Parameter -> Interval = %d\r\n",(int)((evt->data.evt_connection_parameters.interval)*1.25));
//        LOG_INFO("Connection Parameter -> Latency = %d\r\n",evt->data.evt_connection_parameters.latency);
//        LOG_INFO("Connection Parameter -> timeout = %d\r\n",(evt->data.evt_connection_parameters.timeout)*10);

        break;

        // ******************************************************
        // Events for Servers/Slaves
        // ******************************************************
        // GATT server characteristic status event
      case sl_bt_evt_gatt_server_characteristic_status_id:
        /* Indicates either that a local Client Characteristic Configuration descriptor (CCCD) was changed by the remote GATT client,
         * or that a confirmation from the remote GATT client was received upon a successful reception of the indication
         */{
           /*LOG_INFO("evt->data.evt_gatt_server_characteristic_status.characteristic = %d",evt->data.evt_gatt_server_characteristic_status.characteristic);
           LOG_INFO("evt->data.evt_gatt_server_characteristic_status.client_config_flags = %d",evt->data.evt_gatt_server_characteristic_status.client_config_flags);
           LOG_INFO("evt->data.evt_gatt_server_characteristic_status.status_flags = %d",evt->data.evt_gatt_server_characteristic_status.status_flags);
           LOG_INFO("evt->data.evt_gatt_server_characteristic_status.client_config = %d",evt->data.evt_gatt_server_characteristic_status.client_config);
           LOG_INFO("evt->data.evt_gatt_server_characteristic_status.connection = %d",evt->data.evt_gatt_server_characteristic_status.connection);*/

           client_config_flag = evt->data.evt_gatt_server_characteristic_status.client_config_flags;
           status_flags = evt->data.evt_gatt_server_characteristic_status.status_flags;

         }

        break;

      case sl_bt_evt_system_soft_timer_id:
        {
          displayUpdate();
        }
        break;

      // Default event handler.
      default:
        break;


    }
}
/***************************************************************************//**
  void report_data_tmp_ble(float temp_c);
 * Description: void report_data_ble is called from I2C.c while everytime the
 *              I2C reports the temperature data. The condition for GATT DB transfer
 *              in case sl_bt_evt_gatt_server_characteristic_status_id is obtained
 *              and checked for the temperature process as per functionality.
 ******************************************************************************/
void report_data_tmp_ble(int temp_F)
   {
      sl_status_t sc;
      if(openedConnection && temp_F > 0){
        /*Temperature transfer using Gatt server write attribute*/
        uint8_t htm_temperature_buffer[5];
        uint8_t *p = htm_temperature_buffer;
        uint32_t htm_temperature_flt;
        uint8_t flags = 0x00;

        UINT8_TO_BITSTREAM(p, flags);
        htm_temperature_flt = UINT32_TO_FLOAT(temp_F*1000, -3);
        // Convert temperature to bitstream and place it in the htm_temperature_buffer
        UINT32_TO_BITSTREAM(p, htm_temperature_flt);
        sc = sl_bt_gatt_server_write_attribute_value(gattdb_temperature_measurement, // Attribute from gatt_db.h
                                                   0,                             // Offset set to 0
                                                   5,                             // Size of the data transmitted = array length of 5.
                                                   htm_temperature_buffer);       // Passing the address of the value
        app_assert_status(sc);

        displayPrintf(DISPLAY_ROW_TMP, "Temp = %d F", temp_F);
    }
}

/***************************************************************************//**
  void report_data_ble(float temp_c);
 * Description: void report_data_ble is called from I2C.c while everytime the
 *              I2C reports the temperature data. The condition for GATT DB transfer
 *              in case sl_bt_evt_gatt_server_characteristic_status_id is obtained
 *              and checked for the temperature process as per functionality.
 ******************************************************************************/
void report_data_mpr_ble(float pressure)
{
    sl_status_t sc;
    if(openedConnection && pressure > 0){

        uint16_t p_int = (uint16_t)pressure;
        p_int = (p_int << 8) | (p_int >> 8);
        /*Temperature transfer using Gatt server write attribute*/
        uint8_t pressure_buffer[5];
        uint8_t *p = pressure_buffer;
        uint32_t pressure_flt;
        uint8_t flags = 0x00;

        UINT8_TO_BITSTREAM(p, flags);
        pressure_flt = UINT32_TO_FLOAT(p_int*1000, -3);
        // Convert temperature to bitstream and place it in the htm_temperature_buffer
        UINT32_TO_BITSTREAM(p, pressure_flt);


        sc = sl_bt_gatt_server_write_attribute_value(gattdb_BP_Measurement, // Attribute from gatt_db.h
                                                   0,                             // Offset set to 0
                                                   sizeof(p_int),                             // Size of the data transmitted = array length of 5.
                                                  (const uint8_t*)&p_int);     // Passing the address of the value
        app_assert_status(sc);

        displayPrintf(DISPLAY_ROW_BP, "BP = %d mmHg", (int)pressure);
    }
}

/***************************************************************************//**
  void report_data_SpO2_ble(float max_val);
 * Description: void report_data_ble is called from I2C.c while everytime the
 *              I2C reports the temperature data. The condition for GATT DB transfer
 *              in case sl_bt_evt_gatt_server_characteristic_status_id is obtained
 *              and checked for the temperature process as per functionality.
 ******************************************************************************/

void report_data_SpO2_ble(float spo2)
{
     sl_status_t sc;
     if(openedConnection && spo2 > 0){

          uint8_t spo2_short = (uint8_t)spo2;
          sc = sl_bt_gatt_server_write_attribute_value(gattdb_SPO2, // Attribute from gatt_db.h
                                                     0,                             // Offset set to 0
                                                     1,                             // Size of the data transmitted = array length of 5.
                                                     &spo2_short);       // Passing the address of the value
          app_assert_status(sc);
          displayPrintf(DISPLAY_ROW_SPo2, "spo2 = %d", spo2_short);
    }
}
/***************************************************************************//**
  void report_data_bpm_ble(int bpm);
 * Description: void report_data_ble is called from I2C.c while everytime the
 *              I2C reports the temperature data. The condition for GATT DB transfer
 *              in case sl_bt_evt_gatt_server_characteristic_status_id is obtained
 *              and checked for the temperature process as per functionality.
 ******************************************************************************/
void report_data_bpm_ble(int bpm)
{
     sl_status_t sc;
     if(openedConnection && bpm > 0){

            uint8_t bpm_short = (uint8_t)bpm;
            sc = sl_bt_gatt_server_write_attribute_value(gattdb_HR_measurement, // Attribute from gatt_db.h
                                                       0,                             // Offset set to 0
                                                       1,                             // Size of the data transmitted = array length of 5.
                                                       &bpm_short);       // Passing the address of the value
            app_assert_status(sc);

          displayPrintf(DISPLAY_ROW_HR, "HR = %d bpm", bpm);
    }
}
/***************************************************************************//**
  void read_data_time_ble();
 * Description: void report_data_ble is called from I2C.c while everytime the
 *              I2C reports the temperature data. The condition for GATT DB transfer
 *              in case sl_bt_evt_gatt_server_characteristic_status_id is obtained
 *              and checked for the temperature process as per functionality.
 ******************************************************************************/
void read_data_time_ble(uint8_t* time)
{
     sl_status_t sc;
     if (openedConnection) {

         size_t value_len;
         uint8_t value;
          sc = sl_bt_gatt_server_read_attribute_value(gattdb_read_time_sec, // Attribute from gatt_db.h
                                                                  0,                                // Offset set to 0
                                                                  1,
                                                                  &value_len,
                                                                  &value);
          app_assert_status(sc);

          displayPrintf(DISPLAY_ROW_TIME, "Time = %d", value);

          *time = value;
    }
     else{
         displayPrintf(DISPLAY_ROW_TIME, "");
     }
}

