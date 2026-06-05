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

  ButtonType Button::GetPushedButton(){

    if(HAL_GPIO_ReadPin(BTN_L_GPIO_Port, BTN_L_Pin) == GPIO_PIN_SET){
      return ButtonType::LEFT;

    } else if(HAL_GPIO_ReadPin(BTN_D_GPIO_Port, BTN_D_Pin) == GPIO_PIN_SET){
      return ButtonType::BOTTOM;

    } else if(HAL_GPIO_ReadPin(BTN_U_GPIO_Port, BTN_U_Pin) == GPIO_PIN_SET){
      return ButtonType::TOP;

    } else if(HAL_GPIO_ReadPin(BTN_R_GPIO_Port, BTN_R_Pin) == GPIO_PIN_SET){
      return ButtonType::RIGHT;

    } else if(HAL_GPIO_ReadPin(BTN_C_GPIO_Port, BTN_C_Pin) == GPIO_PIN_SET){
      return ButtonType::CENTER;

    } else{
      return ButtonType::NONE;
    }

  }

  ButtonType Button::GetPushedButton(int32_t timeout_ms){

    uint32_t startTick = HAL_GetTick();

    bool infinity = (timeout_ms < 0);

    auto res = ButtonType::NONE;

    while(true) {

      res = Button::GetPushedButton();

      if(res != ButtonType::NONE){
        return res;
      }

      if(!infinity){

        uint32_t now = HAL_GetTick();
        // overflow
        if(now < startTick){
          if(now + (0xFFFFFFFF - startTick) + 1 >= static_cast<uint32_t>(timeout_ms)){
            break;
          }
        } else if(now - startTick >= static_cast<uint32_t>(timeout_ms)) {
          break;
        }
      }

    }

    return res;

  }

}
