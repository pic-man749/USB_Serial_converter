/*
 * Encoder.cpp
 *
 *      Author: picman
 */
#include "Encoder.hpp"

namespace Driver {

  Encoder::Encoder(TIM_HandleTypeDef *htim){
    htim_ = htim;
    lastCounterValue_ = __HAL_TIM_GET_COUNTER(htim_);
  }

  bool Encoder::Start(void){
    return ( HAL_TIM_Encoder_Start(htim_, TIM_CHANNEL_ALL) == HAL_OK );
  }

  bool Encoder::Stop(void){
    return ( HAL_TIM_Encoder_Stop(htim_, TIM_CHANNEL_ALL) == HAL_OK );
  }

  int Encoder::GetValue(void){
    const int nowCounterValue = __HAL_TIM_GET_COUNTER(htim_);
    const int delta = nowCounterValue - lastCounterValue_;
    lastCounterValue_ = nowCounterValue;
    return delta / 2;
  }

  void Encoder::SetValue(int value){
    __HAL_TIM_SET_COUNTER(htim_, (uint32_t)(value * 2));
  }

}
