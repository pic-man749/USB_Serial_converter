/*
 * InputManager.cpp
 */
#include "InputManager.hpp"
#include "../../Driver/Time/Time.hpp"

namespace App {

  InputManager::InputManager() :
          button_(), encoder_(), is_pressed_ { }, is_long_pressed_ { }, press_start_ms_ { },
          prev_encoder_value_(0) {
  }

  void InputManager::init() {
    encoder_.init();
    prev_encoder_value_ = encoder_.GetValue();
  }

  void InputManager::Update(EventQueue &queue) {
    Driver::ButtonState curr = button_.GetButtonState();
    uint32_t now = Driver::GetTick();

    constexpr size_t kButtonCount = static_cast<size_t>(Driver::ButtonType::kCount);
    for(size_t i = 0U; i < kButtonCount; ++i) {
      Driver::ButtonType id = static_cast<Driver::ButtonType>(i);
      bool curr_pressed = GetButtonPressed(curr, id);

      if(curr_pressed && !is_pressed_[i]) {
        press_start_ms_[i] = now;
        queue.push(ButtonEvent { id, ButtonEventType::kPress });
      } else if(!curr_pressed && is_pressed_[i]) {
        queue.push(ButtonEvent { id, ButtonEventType::kRelease });
      } else if(curr_pressed
          && !is_long_pressed_[i]
          && (now - press_start_ms_[i] >= kLongPressThresholdMs)) {
        is_long_pressed_[i] = true;
        queue.push(ButtonEvent { id, ButtonEventType::kLongPress });
      }

      if(!curr_pressed) {
        is_long_pressed_[i] = false;
      }
      is_pressed_[i] = curr_pressed;
    }

    int curr_encoder = encoder_.GetValue();
    int32_t delta = static_cast<int32_t>(curr_encoder - prev_encoder_value_);
    if(delta != 0) {
      queue.push(EncoderRotateEvent { delta });
      prev_encoder_value_ = curr_encoder;
    }
  }

  bool InputManager::GetButtonPressed(const Driver::ButtonState &state, Driver::ButtonType id) {
    switch(id) {
      case Driver::ButtonType::Left:
        return state.left;
      case Driver::ButtonType::Right:
        return state.right;
      case Driver::ButtonType::Top:
        return state.top;
      case Driver::ButtonType::Bottom:
        return state.bottom;
      case Driver::ButtonType::Center:
        return state.center;
      default:
        return false;
    }
  }

} // namespace App
