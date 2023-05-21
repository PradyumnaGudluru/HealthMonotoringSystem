#include "moving_average_filter.h"

/**
 * @brief Initialize moving average filter
 */
void init_MovingAverageFilter(MovingAverageFilter_t* maf)
{
  maf->index_ = 0;
  maf->count_ = 0;
}

/**
 * @brief Applies the moving average filter
 */
float process_MovingAverageFilter(float value, MovingAverageFilter_t* maf)
{
  // Add value
  maf->values_[maf->index_] = value;

  // Increase index and count
  maf->index_ = (maf->index_ + 1) % BUFF_SIZE;
  if(maf->count_ < BUFF_SIZE) {
      maf->count_++;
  }

  // Calculate sum
  float sum = 0.0;
  for(int i = 0; i < maf->count_; i++) {
      sum += maf->values_[i];
  }

  // Calculate average
  return sum/maf->count_;
}

/**
 * @brief Resets the stored values
 */
void reset_MovingAverageFilter(MovingAverageFilter_t* maf)
{
  maf->index_ = 0;
  maf->count_ = 0;
}

/**
 * @brief Get number of samples
 * @return Number of stored samples
 */
int count(const MovingAverageFilter_t* maf)
{
  return maf->count_;
}
