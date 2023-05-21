#ifndef LOW_PASS_FILTER_H__
#define LOW_PASS_FILTER_H__

#include "defines.h"
/**
 * @brief Low Pass Filter
 */
typedef struct LowPassFilter {
  float kX;
  float kA0;
  float kB1;
  float last_value_;
}LowPassFilter_t;


/**
 * @brief Initialize the Low Pass Filter
 * @param samples Number of samples until decay to 36.8 %
 * @remark Sample number is an RC time-constant equivalent
 */
void init_LowPassFilter_with_samples(float samples, LowPassFilter_t* lpf);
/**
 * @brief Initialize the Low Pass Filter
 * @param cutoff Cutoff frequency
 * @pram sampling_frequency Sampling frequency
 */
void init_LowPassFilter_with_cutoff(float cutoff, float sampling_frequency,
                                    LowPassFilter_t* lpf);
/**
 * @brief Applies the low pass filter
 */
float process_LowPassFilter(float value, LowPassFilter_t* lpf);

/**
 * @brief Resets the stored values
 */
void reset_LowPassFilter(LowPassFilter_t* lpf);


#endif /* LOW_PASS_FILTER_H__ */
