/*
 * IConfigApplicable.hpp
 *
 * パススルー設定（UARTチャンネル・ボーレート）の変更を受け付けるインターフェース。
 */

#ifndef APP_ICONFIGAPPLICABLE_HPP_
#define APP_ICONFIGAPPLICABLE_HPP_

#include <cstdint>
#include "AppConfig/AppConfig.hpp"

namespace App {

  class IConfigApplicable {
    public:
      virtual ~IConfigApplicable() = default;
      virtual void setChannel(UartChannel channel) = 0;
      virtual void setBaudRate(uint32_t baudRate) = 0;
  };

} // namespace App

#endif /* APP_ICONFIGAPPLICABLE_HPP_ */
