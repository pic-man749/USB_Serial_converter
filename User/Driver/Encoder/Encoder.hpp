/*
 * Encoder.hpp
 *
 *      Author: picman
 */

#ifndef DRIVER_ENCODER_ENCODER_HPP_
#define DRIVER_ENCODER_ENCODER_HPP_

#include "tim.h"

namespace Driver {

  class Encoder {

    public:
      Encoder(TIM_HandleTypeDef *htim);

      bool Start(void);

      bool Stop(void);

      int GetValue(void);

      /**
       * @brief エンコーダカウンタ値を設定する
       *
       * @param value 設定値（内部カウンタへは value * 2 を書き込む）
       */
      void SetValue(int value);

    private:

      TIM_HandleTypeDef *htim_;

      int32_t lastCounterValue_;

  };
}

#endif /* DRIVER_ENCODER_ENCODER_HPP_ */
