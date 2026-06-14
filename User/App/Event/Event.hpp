/*
 * Event.hpp
 *
 * アプリケーション全体で使用するイベント型定義。
 * 入力イベント（ボタン・エンコーダ）と通信データイベントを単一の variant で表現する。
 */

#ifndef APP_EVENT_EVENT_HPP_
#define APP_EVENT_EVENT_HPP_

#include <variant>
#include <cstdint>
#include "../../Driver/Button/ButtonType.hpp"

namespace App {

  /** イベントなし */
  struct NoneEvent {
  };

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

  /** DisplayBuffer に新着データがあることを示すイベント */
  struct CommunicationDataEvent {
  };

  /** アプリケーションイベント型 */
  using Event = std::variant<
  NoneEvent,
  ButtonEvent,
  EncoderRotateEvent,
  CommunicationDataEvent
  >;

} // namespace App

#endif /* APP_EVENT_EVENT_HPP_ */
