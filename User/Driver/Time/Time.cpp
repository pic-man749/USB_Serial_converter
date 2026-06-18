/*
 * Time.cpp
 *
 *      Author: picman
 */
#include "Time.hpp"
#include "stm32f4xx_hal.h"

namespace Driver {

  uint32_t GetTick() {
    return HAL_GetTick();
  }

  void DelayMs(uint32_t ms){
    HAL_Delay(ms);
  }

}
