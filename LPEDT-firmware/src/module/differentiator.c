#include "differentiator.h"

/**
 * @brief Initializes the differentiator
 */
void init_Dfifferentiator(float sampling_frequency, Differentiator_t* difftr)
{
  difftr->kSamplingFrequency = sampling_frequency;
  difftr->last_value_ = 0;
}

/**
 * @brief Applies the differentiator
 */
float process_Differentiator(float value, Differentiator_t* difftr)
{
    float diff = (value - difftr->last_value_) * difftr->kSamplingFrequency;
    difftr->last_value_ = value;
    return diff;
}

/**
 * @brief Resets the stored values
 */
void reset_Differentiator(Differentiator_t* difftr)
{
  difftr->last_value_ = 0;
}
