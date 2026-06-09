/*
 * ButtonState.hpp
 *
 *      Author: picman
 */

#ifndef DRIVER_BUTTON_BUTTONSTATE_HPP_
#define DRIVER_BUTTON_BUTTONSTATE_HPP_

namespace Driver {

  struct ButtonState {
    bool left   = false;
    bool right  = false;
    bool top    = false;
    bool bottom = false;
    bool center = false;
  };

}

#endif /* DRIVER_BUTTON_BUTTONSTATE_HPP_ */
