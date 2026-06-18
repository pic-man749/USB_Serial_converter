/*
 * Time.hpp
 *
 *      Author: picman
 */

#ifndef DRIVER_TIME_TIME_HPP_
#define DRIVER_TIME_TIME_HPP_

#include <cstdint>

namespace Driver {

  /**
   * @brief Get the current tick count.
   *
   * @return The current tick count as a 32-bit unsigned integer.
   */
  uint32_t GetTick();

  /**
   * @brief Delay execution for a specified number of milliseconds.
   *
   * @param ms The number of milliseconds to delay.
   */
  void DelayMs(uint32_t ms);

}

#endif /* DRIVER_TIME_TIME_HPP_ */
