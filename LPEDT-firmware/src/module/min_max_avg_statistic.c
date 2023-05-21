#include "min_max_avg_statistic.h"
#define max_value(x,y) (((x) > (y)) ? (x) : (y))
#define min_value(x,y) (((x) < (y)) ? (x) : (y))


/**
 * @brief Initialize the statistic instance
 */
void init_MinMaxAvgStatistic(float min, float max, int sum, int count,
                             MinMaxAvgStatistic_t* mmas)
{
  mmas->min_ = min;
  mmas->max_ = max;
  mmas->sum_ = sum;
  mmas->count_ = count;

}

/**
 * @brief Add value to the statistic
 */
void process_MinMaxAvgStatistic(float value, MinMaxAvgStatistic_t* mmas) {
  mmas->min_ = min_value(mmas->min_, value);
  mmas->max_ = max_value(mmas->max_, value);
  mmas->sum_ += value;
  mmas->count_++;
}

/**
 * @brief Resets the stored values
 */
void reset_MinMaxAvgStatistic(MinMaxAvgStatistic_t* mmas) {
  mmas->min_ = 0;
  mmas->max_ = 0;
  mmas->sum_ = 0;
  mmas->count_ = 0;
}

/**
 * @brief Get Minimum
 * @return Minimum Value
 */
float minimum(MinMaxAvgStatistic_t const* mmas) {
  return  mmas->min_;
}

/**
 * @brief Get Maximum
 * @return Maximum Value
 */
float maximum(MinMaxAvgStatistic_t const* mmas) {
  return mmas->max_;
}

/**
 * @brief Get Average
 * @return Average Value
 */
float average(MinMaxAvgStatistic_t const* mmas) {
  return mmas->sum_/mmas->count_;
}
