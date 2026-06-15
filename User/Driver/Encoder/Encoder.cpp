/*
 * Encoder.cpp
 *
 *      Author: picman
 */
#include "Encoder.hpp"

namespace Driver {

  Encoder::Encoder() {
    htim_ = &htim5;
  }

  void Encoder::init(void) {
    Start();
  }

  bool Encoder::Start(void){
    return ( HAL_TIM_Encoder_Start(htim_, TIM_CHANNEL_ALL) == HAL_OK );
  }

  bool Encoder::Stop(void){
    return ( HAL_TIM_Encoder_Stop(htim_, TIM_CHANNEL_ALL) == HAL_OK );
  }

  int Encoder::GetValue(void){
    // uint32_t のまま除算すると、カウンタが 0 を跨いで折り返したときバグるので
    // int32_t にキャストしてから除算することで折り返しを符号付きで扱う。
    return static_cast<int32_t>(__HAL_TIM_GET_COUNTER(htim_)) / 2;
  }

  void Encoder::SetValue(int value){
    __HAL_TIM_SET_COUNTER(htim_, static_cast<uint32_t>(value * 2));
  }

}
