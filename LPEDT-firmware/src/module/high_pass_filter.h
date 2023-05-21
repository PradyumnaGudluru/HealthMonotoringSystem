#ifndef HIGH_PASS_FILTER_H__
#define HIGH_PASS_FILTER_H__

#include "defines.h"

/**
 * @brief High Pass Filter
 */
typedef struct HighPassFilter {
  float kX;
  float kA0;
  float kA1;
  float kB1;
  float last_filter_value_;
  float last_raw_value_;
}HighPassFilter_t;



/**
 * @brief Initialize the High Pass Filter
 * @param samples Number of samples until decay to 36.8 %
 * @remark Sample number is an RC time-constant equivalent
 */
void init_HighPassFilter_with_samples(float samples, HighPassFilter_t* hpf);

/**
 * @brief Initialize the High Pass Filter
 * @param cutoff Cutoff frequency
 * @pram sampling_frequency Sampling frequency
 */
void init_HighPassFilter_with_cutoff(float cutoff, float sampling_frequency,
                                     HighPassFilter_t* hpf);

/**
 * @brief Applies the high pass filter
 */
float process_HighPassFilter(float value, HighPassFilter_t* hpf);

/**
 * @brief Resets the stored values
 */
void reset_HighPassFilter(HighPassFilter_t* hpf);

#endif /* HIGH_PASS_FILTER_H__ */
