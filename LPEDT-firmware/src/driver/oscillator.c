/***************************************************************************//**

 * Date:        11-09-2021
 * Author:      Pradyumna Gudluru
 * Description: This code has oscillator functionality for low power energy mode
 *              operation.
 *              It is used  for ECEN 5823 "IoT Embedded Firmware".
 *              Oscillator is enabled, the clock is set with set division and enabled.
 *
 ******************************************************************************/
#include "Oscillator.h"
/***************************************************************************//**
  //void oscillatorinit();
 * Description: oscillatorinit is used to initialize oscillator clock parameters.
 ******************************************************************************/
void oscillator_init(sl_power_manager_em_t energy_mode){

  // enable HFXO for EM0
//  CMU_OscillatorEnable(cmuSelect_HFXO , true, true);
//  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO );

  //For EM3 Ultra Low Frequency oscillator and clock used
  if (energy_mode == SL_POWER_MANAGER_EM3){
      CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);
      CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
      CMU_ClockDivSet(cmuClock_LETIMER0, cmuClkDiv_1);
  }
  else{
      CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
      CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
      CMU_ClockDivSet(cmuClock_LETIMER0, cmuClkDiv_4);
  }
  CMU_ClockEnable(cmuClock_LETIMER0, true);
}
