#include "low_pass_filter.h"

/**
 * @brief Initialize the Low Pass Filter
 * @param samples Number of samples until decay to 36.8 %
 * @remark Sample number is an RC time-constant equivalent
 */
void init_LowPassFilter_with_samples(float samples, LowPassFilter_t* lpf)
{

  lpf->kX = (exp(-1/samples));
  lpf->kA0 = 1 -lpf->kX;
  lpf->kB1 = lpf->kX;
  lpf->last_value_ = 0;
}

/**
 * @brief Initialize the Low Pass Filter
 * @param cutoff Cutoff frequency
 * @pram sampling_frequency Sampling frequency
 */
void init_LowPassFilter_with_cutoff(float cutoff, float sampling_frequency,
                                    LowPassFilter_t* lpf)
{
  init_LowPassFilter_with_samples(sampling_frequency/(cutoff*2*PI), lpf);
}

/**
 * @brief Applies the low pass filter
 */
float process_LowPassFilter(float value, LowPassFilter_t* lpf) {

  if(lpf->last_value_ == 0){
      lpf->last_value_ = value;
  }
  else{
      lpf->last_value_ = lpf->kA0 * value + lpf->kB1 * lpf->last_value_;
  }

  return lpf->last_value_;
}

/**
 * @brief Resets the stored values
 */
void reset_LowPassFilter(LowPassFilter_t* lpf) {
  lpf->last_value_ = 0;
}
