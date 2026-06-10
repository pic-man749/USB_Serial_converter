/*
 * InputEvent.hpp
 *
 *      Author: picman
 */

#ifndef APP_INPUTMANAGER_INPUTEVENT_HPP_
#define APP_INPUTMANAGER_INPUTEVENT_HPP_

#include "../../Driver/Button/ButtonState.hpp"
#include "../../Driver/Button/ButtonType.hpp"

namespace App {
  /** イベントなし */
  struct NoneEvent {};

  /** ボタン単押しイベント */
  struct ButtonPressEvent {
      Driver::ButtonType button_id;
  };

  /** ボタン長押しイベント */
  struct ButtonLongPressEvent {
      Driver::ButtonType button_id;
  };

  /** ボタンReleaseイベント */
  struct ButtonReleaseEvent {
      Driver::ButtonType button_id;
  };

  /**
   * エンコーダ回転イベント
   * delta > 0: 時計回り、delta < 0: 反時計回り
   */
  struct EncoderRotateEvent {
      int32_t delta;
  };

  /** 統一入力イベント型 */
  using InputEvent = std::variant<NoneEvent, ButtonPressEvent, ButtonLongPressEvent, ButtonReleaseEvent, EncoderRotateEvent>;
}

#endif /* APP_INPUTMANAGER_INPUTEVENT_HPP_ */
