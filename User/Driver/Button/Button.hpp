/*
 * Button.hpp
 *
 *      Author: picman
 */

#ifndef DRIVER_BUTTON_BUTTON_HPP_
#define DRIVER_BUTTON_BUTTON_HPP_

#include <cstdint>
#include "ButtonState.hpp"

namespace Driver {

  class Button {

    public:
      Button();

      ButtonState GetButtonState();

    private:

  };

}

#endif /* DRIVER_BUTTON_BUTTON_HPP_ */
