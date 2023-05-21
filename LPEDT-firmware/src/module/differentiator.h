#ifndef DIFFERENTIATOR_H__
#define DIFFERENTIATOR_H__

/**
 * @brief Differentiator
 */
typedef struct Differentiator {
  float kSamplingFrequency;
  float last_value_;
}Differentiator_t;


/**
 * @brief Initializes the differentiator
 */
void init_Dfifferentiator(float sampling_frequency, Differentiator_t* difftr);

/**
 * @brief Applies the differentiator
 */
float process_Differentiator(float value, Differentiator_t* difftr);

/**
 * @brief Resets the stored values
 */
void reset_Differentiator(Differentiator_t* difftr);

#endif /* DIFFERENTIATOR_H__ */
