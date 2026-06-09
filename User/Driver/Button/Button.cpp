/*
 * Button.cpp
 *
 *      Author: picman
 */
#include "Button.hpp"
#include "gpio.h"

namespace Driver {

  Button::Button(){
    ;
  }

  ButtonState Button::GetButtonState(){

    ButtonState state;
    state.left   = (HAL_GPIO_ReadPin(BTN_L_GPIO_Port, BTN_L_Pin) == GPIO_PIN_SET);
    state.right  = (HAL_GPIO_ReadPin(BTN_R_GPIO_Port, BTN_R_Pin) == GPIO_PIN_SET);
    state.top    = (HAL_GPIO_ReadPin(BTN_U_GPIO_Port, BTN_U_Pin) == GPIO_PIN_SET);
    state.bottom = (HAL_GPIO_ReadPin(BTN_D_GPIO_Port, BTN_D_Pin) == GPIO_PIN_SET);
    state.center = (HAL_GPIO_ReadPin(BTN_C_GPIO_Port, BTN_C_Pin) == GPIO_PIN_SET);
    return state;

  }

}
