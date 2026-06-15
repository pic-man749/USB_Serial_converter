/*
 * ButtonType.hpp
 *
 *      Author: picman
 */

#ifndef DRIVER_BUTTON_BUTTONTYPE_HPP_
#define DRIVER_BUTTON_BUTTONTYPE_HPP_

#include <cstdint>

namespace Driver {

  /**
   * ボタンの識別子
   */
  enum class ButtonType : uint8_t {
    Left = 0,
    Right = 1,
    Top = 2,
    Bottom = 3,
    Center = 4,
    kCount = 5
  };

}

#endif /* DRIVER_BUTTON_BUTTONTYPE_HPP_ */
