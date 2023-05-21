#include "high_pass_filter.h"

/**
 * @brief Initialize the High Pass Filter
 * @param samples Number of samples until decay to 36.8 %
 * @remark Sample number is an RC time-constant equivalent
 */
void init_HighPassFilter_with_samples(float samples, HighPassFilter_t* hpf)
{
  hpf->kX = exp(-1/samples);
  hpf->kA0 = (1+hpf->kX)/2;
  hpf->kA1 = -(hpf->kA0);
  hpf->kB1 = hpf->kX;
  hpf->last_filter_value_ = 0;
  hpf->last_raw_value_ = 0;
}

/**
 * @brief Initialize the High Pass Filter
 * @param cutoff Cutoff frequency
 * @pram sampling_frequency Sampling frequency
 */
void init_HighPassFilter_with_cutoff(float cutoff, float sampling_frequency,
                                     HighPassFilter_t* hpf)
{
  init_HighPassFilter_with_samples(sampling_frequency/(cutoff*2*PI), hpf);
}

/**
 * @brief Applies the high pass filter
 */
float process_HighPassFilter(float value, HighPassFilter_t* hpf) {

  hpf->last_filter_value_ = hpf->kA0 * value + hpf->kA1 * hpf->last_raw_value_
              + hpf->kB1 * hpf->last_filter_value_;

  hpf->last_raw_value_ = value;
  return hpf->last_filter_value_;
}

/**
 * @brief Resets the stored values
 */
void reset_HighPassFilter(HighPassFilter_t* hpf) {
  hpf->last_raw_value_ = 0;
  hpf->last_filter_value_ = 0;
}
