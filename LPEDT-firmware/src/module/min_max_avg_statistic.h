#ifndef STATS_H__
#define STATS_H__

/**
 * @brief Statistic block for min/nax/avg
 */
typedef struct MinMaxAvgStatistic {
  float min_;
  float max_;
  float sum_;
  int count_;
}MinMaxAvgStatistic_t;

/**
 * @brief Initialize the statistic instance
 */
void init_MinMaxAvgStatistic(float min, float max, int sum, int count,
                             MinMaxAvgStatistic_t* mmas);

/**
 * @brief Add value to the statistic
 */
void process_MinMaxAvgStatistic(float value, MinMaxAvgStatistic_t* mmas);

/**
 * @brief Resets the stored values
 */
void reset_MinMaxAvgStatistic(MinMaxAvgStatistic_t* mmas);

/**
 * @brief Get Minimum
 * @return Minimum Value
 */
float minimum(MinMaxAvgStatistic_t const* mmas);

/**
 * @brief Get Maximum
 * @return Maximum Value
 */
float maximum(MinMaxAvgStatistic_t const* mmas);

/**
 * @brief Get Average
 * @return Average Value
 */
float average(MinMaxAvgStatistic_t const* mmas);


#endif // STATS_H__
