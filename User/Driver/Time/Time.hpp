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

}

#endif /* DRIVER_TIME_TIME_HPP_ */
