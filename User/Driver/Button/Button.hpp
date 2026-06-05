/*
 * Button.hpp
 *
 *      Author: picman
 */

#ifndef DRIVER_BUTTON_BUTTON_HPP_
#define DRIVER_BUTTON_BUTTON_HPP_

#include <cstdint>
#include "ButtonType.hpp"

namespace Driver {

  class Button {

    public:
      Button();

      ButtonType GetPushedButton();

      ButtonType GetPushedButton(int32_t timeout);

    private:

  };

}

#endif /* DRIVER_BUTTON_BUTTON_HPP_ */
