#ifndef MOVING_AVERAGE_FILTER_H__
#define MOVING_AVERAGE_FILTER_H__

#define BUFF_SIZE   5

/**
 * @brief MovingAverageFilter
 * @tparam buffer_size Number of samples to average over
 */
typedef struct MovingAverageFilter {
  int index_;
  int count_;
  float values_[BUFF_SIZE];
}MovingAverageFilter_t;

/**
 * @brief Initialize moving average filter
 */
void init_MovingAverageFilter(MovingAverageFilter_t* maf);

/**
 * @brief Applies the moving average filter
 */
float process_MovingAverageFilter(float value, MovingAverageFilter_t* maf);

/**
 * @brief Resets the stored values
 */
void reset_MovingAverageFilter(MovingAverageFilter_t* maf);

/**
 * @brief Get number of samples
 * @return Number of stored samples
 */
int count(const MovingAverageFilter_t* maf);


#endif /* MOVING_AVERAGE_FILTER_H__ */
