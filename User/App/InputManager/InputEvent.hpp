/*
 * InputEvent.hpp
 *
 *      Author: picman
 */

#ifndef APP_INPUTMANAGER_INPUTEVENT_HPP_
#define APP_INPUTMANAGER_INPUTEVENT_HPP_

#include <variant>
#include "../../Driver/Button/ButtonState.hpp"
#include "../../Driver/Button/ButtonType.hpp"

namespace App {
  /** イベントなし */
  struct NoneEvent {};

  enum class ButtonEventType {
    kPress,
    kRelease,
    kLongPress
  };

  /** ボタンイベント */
  struct ButtonEvent {
      Driver::ButtonType button_id;
      ButtonEventType type;
  };

  /**
   * エンコーダ回転イベント
   * delta > 0: 時計回り、delta < 0: 反時計回り
   */
  struct EncoderRotateEvent {
      int32_t delta;
  };

  /** 入力イベント型 */
  using InputEvent = std::variant<NoneEvent, ButtonEvent, EncoderRotateEvent>;
}

#endif /* APP_INPUTMANAGER_INPUTEVENT_HPP_ */
