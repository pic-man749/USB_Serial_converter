/*
 * InputManager.cpp
 *
 *      Author: picman
 */

#include "InputManager.hpp"
#include "../../Driver/Time/Time.hpp"

namespace App {

  InputManager::InputManager() :
      button_(), encoder_(),
      is_pressed_ {false}, press_start_ms_ {0u}, event_queue_ { }, 
      queue_head_(0), queue_tail_(0), prev_encoder_value_(encoder_.GetValue()) {
  }

  void InputManager::Update() {

    // ボタンのイベント発行
    Driver::ButtonState curr = button_.GetButtonState();
    uint32_t now = Driver::GetTick();

    constexpr std::size_t kButtonCount = static_cast<std::size_t>(Driver::ButtonType::kCount);
    for(std::size_t i = 0; i < kButtonCount; ++i) {
      Driver::ButtonType id = static_cast<Driver::ButtonType>(i);
      bool curr_pressed = GetButtonPressed(curr, id);

      if(curr_pressed && !is_pressed_[i]) {
        // 押下開始を記録し、押下イベント発行
        press_start_ms_[i] = now;
        PushEvent(ButtonPressEvent { id });
      } else if(!curr_pressed && is_pressed_[i]) {
        // リリースイベントを発行
        PushEvent(ButtonReleaseEvent { id });
      } else if(curr_pressed && !is_long_pressed_[i]
          && (now - press_start_ms_[i] >= kLongPressThresholdMs)) {
        // 長押しイベントを発行
        is_long_pressed_[i] = true;
        PushEvent(ButtonLongPressEvent { id });
      }

      is_pressed_[i] = curr_pressed;
    }

    // エンコーダのイベント発行
    int curr_encoder = encoder_.GetValue();
    int32_t delta = static_cast<int32_t>(curr_encoder - prev_encoder_value_);
    if(delta != 0) {
      PushEvent(EncoderRotateEvent { delta });
      prev_encoder_value_ = curr_encoder;
    }
  }

  bool InputManager::PopEvent(InputEvent &event) {
    if(queue_head_ == queue_tail_) {
      return false;
    }
    event = event_queue_[queue_head_];
    queue_head_ = (queue_head_ + 1) % kEventQueueSize;
    return true;
  }

  void InputManager::PushEvent(InputEvent event) {
    std::size_t next_tail = (queue_tail_ + 1) % kEventQueueSize;
    if(next_tail == queue_head_) {
      // キューが満杯なら最も古いイベントを破棄して空きを確保
      queue_head_ = (queue_head_ + 1) % kEventQueueSize;
    }
    event_queue_[queue_tail_] = std::move(event);
    queue_tail_ = next_tail;
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
