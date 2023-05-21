/***************************************************************************//**

 * Date:        19-09-2021
 * Author:      Pradyumna Gudluru
 * Description: This code has scheduler functionality.
 *              It is used  for ECEN 5823 "IoT Embedded Firmware".
 *              Scheduler is initialized, and app action is set, with sate change for underflow interrupt.
 *              The scheduler event is set and received.
 * References: Slides from Lectures of Dave Sluiter.
 ******************************************************************************/
#include "scheduler.h"
#define INCLUDE_LOG_DEBUG         1
#include "src/driver/log.h"

typedef enum {
  state_sched_time,
  state_idle,
  state_req_tmp,
  state_rcv_tmp,
  state_req_mpr,
  state_rcv_mpr,
  state_req_max,
  state_rcv_max,
  state_data_display
}State_I2C_t;



#define MAX_TRYOUT          (200)
#define PERSONAL            1
#define HOSPITAL            2
#define USER_CASE           HOSPITAL

static State_I2C_t system_state = state_sched_time;
static max_data_t latest_data;
static uint32_t timer_UF_count = 0;
static uint8_t scheduled_use_time = 0;
static uint32_t COUNT_REQUIRED = 0;
/***************************************************************************//**
  //void scheduler_init();
 * Description: scheduler_init is used to initialize scheduler parameters.
 ******************************************************************************/
void scheduler_init(){

#if USER_CASE == PERSONAL
  system_state = state_sched_time; //state_req_max;  // Setting default state to IDLE
#else
  system_state = state_idle;
  scheduled_use_time = 5;
  COUNT_REQUIRED = (scheduled_use_time * 1000) / LETIMER_PERIOD_MS;
#endif

}

// scheduler routine to set a scheduler event
/***************************************************************************//**
  //void system_state_machine();
 * Description: system_state_machine is used to read sensor data from state machine.
 ******************************************************************************/

void system_state_machine(sl_bt_msg_t *evt)
{
 // idea: use a timer that can be configured on the mobile app to initiate the data transfers

  static int temp_F = 0;
  static float pressure_mmHg = 0;
  static int max30101_try_out = 0;

  if(is_ble_connected()){
    // State chart with five states as per the enum State_I2C_t
    switch(system_state)
      {

        case state_sched_time:
        {
          read_data_time_ble(&scheduled_use_time);
          timerWaitUs(1000);
          if(scheduled_use_time){
              system_state = state_idle;
              LOG_INFO("scheduled_use_time = %d\r\n", scheduled_use_time);
              COUNT_REQUIRED = (scheduled_use_time * 1000) / LETIMER_PERIOD_MS;
              sl_power_manager_add_em_requirement(LOWEST_ENERGY_MODE);
          }

          break;
        }
        case state_idle:
          {

            if(evt->data.evt_system_external_signal.extsignals == evtLETIMER0_UF){

                timer_UF_count++;
                if(timer_UF_count == COUNT_REQUIRED){
                    timer_UF_count = 0;
                    sl_power_manager_remove_em_requirement(LOWEST_ENERGY_MODE);
                    sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
                    system_state = state_req_tmp;
                }
            }

            break;
          }

        case state_req_tmp:
          {
            TMP117_set_oneshot_mode();
            timerWaitUs(1500);
            TMP117_get_temperature();
            TMP117_set_shutdown_mode();
            system_state = state_rcv_tmp;
            break;
          }
        case state_rcv_tmp:
          {
             temp_F = TMP117_report_temperature();
             LOG_INFO("Temperature = %dF\r\n", temp_F);
             system_state = state_req_mpr;
            break;
          }

          //Reading MPR data
        case state_req_mpr:
          {
            MPR_read_pressure();
            system_state = state_rcv_mpr;
            break;
          }
        case state_rcv_mpr:
          {
            pressure_mmHg = MPR_report_pressure_in_Hg();
            LOG_INFO("Pressure = %f mmHg\r\n", pressure_mmHg);
//            system_state = state_idle;
            MAX_30101_PowerUp();
            system_state = state_req_max;
            break;
          }
        //Reading MAX30101 Data
        case state_req_max:
        {
          if(max30101_try_out > MAX_TRYOUT){
              MAX_30101_ShutDown();
              max30101_try_out = 0;
              sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
              sl_power_manager_add_em_requirement(LOWEST_ENERGY_MODE);
              system_state = state_idle;//state_data_display;
              break;
           }

          max30101_try_out++;
          if(Max_30101_FIFO()){
              MAX_30101_Reset();
              MAX_30101_Init();
              MAX_30101_PowerUp();
              system_state = state_req_max;
          }
          else{
              system_state = state_rcv_max;
          }

          break;
        }
      case state_rcv_max:
        {
          system_state = state_req_max;
           Max_30101_process();
           latest_data = Max_30101_latest_data();
           if(latest_data.bpm != 0 && latest_data.spo2 != 0){

               LOG_INFO("Heart Rate : %d bpm\r\n", latest_data.bpm);
               LOG_INFO("SpO2 = %f\r\n", latest_data.spo2);
               Max_30101_reset_latest_data();
               max30101_try_out = 0;
               MAX_30101_ShutDown();
               system_state = state_data_display;
           }
           break;
        }

        case state_data_display:
          {
            report_data_tmp_ble(temp_F);
            report_data_mpr_ble(pressure_mmHg);
            report_data_bpm_ble(latest_data.bpm);
            report_data_SpO2_ble(latest_data.spo2);
            system_state = state_idle;
            sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
            sl_power_manager_add_em_requirement(LOWEST_ENERGY_MODE);
            break;

          }
      } // switch
  }
} // app_process_action()
/***************************************************************************//**
  //void schedulerSetEvent_readtemp();
 * Description: schedulerSetEvent_readtemp is used to set interrupt for UF.
 ******************************************************************************/
void schedulerSetEvent_LETIMER0_UF() {

  CORE_DECLARE_IRQ_STATE;
  // enter the critical section
  CORE_ENTER_CRITICAL();
  // mask the COMP1 bit
  sl_bt_external_signal(evtLETIMER0_UF);
  // exit the critical section
  CORE_EXIT_CRITICAL();

}
/***************************************************************************//**
  //void schedulerSetI2C_IRQ_Event();
 * Description: schedulerSetI2C_IRQ_Event is used to set interrupt for I2C_IRQ.
 ******************************************************************************/
void schedulerSetI2C_IRQ_Event() {

  CORE_DECLARE_IRQ_STATE;
  // enter the critical section
  CORE_ENTER_CRITICAL();
  // mask the COMP1 bit
  sl_bt_external_signal(evtI2C_IRQACK);
  // exit the critical section
  CORE_EXIT_CRITICAL();

}
