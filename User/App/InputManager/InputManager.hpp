/*
 * InputManager.hpp
 *
 *      Author: picman
 */

#ifndef APP_INPUTMANAGER_INPUTMANAGER_HPP_
#define APP_INPUTMANAGER_INPUTMANAGER_HPP_

#include <array>
#include <cstdint>
#include <variant>
#include "../Event.hpp"
#include "../../Driver/Button/Button.hpp"
#include "../../Driver/Encoder/Encoder.hpp"

namespace App {

  /**
   * @brief 入力イベントを管理するクラス
   *
   * Button・Encoderの入力状態を監視し、 InputEvent としてキューに蓄積する。
   * ボタンイベントは押下、リリース、長押しを区別して発行する。
   * エンコーダイベントは回転量を delta として発行する。
   * Update() をメインループで定期的に呼び出すこと。
   */
  class InputManager {
    public:
      /** 長押し判定閾値（ミリ秒） */
      static constexpr uint32_t kLongPressThresholdMs = 1000U;
      /** イベントキューの最大保持件数 */
      static constexpr std::size_t kEventQueueSize = 32U;

      InputManager();

      /**
       * @brief 入力状態を更新する。メインループで定期的に呼び出すこと。
       */
      void Update();

      /**
       * @brief イベントキューから1件取得する
       * @return 取得したイベント。キューが空の場合は NoneEvent を返す
       */
      Event PopEvent();

    private:
      Driver::Button button_;
      Driver::Encoder encoder_;

      std::array<bool, static_cast<std::size_t>(Driver::ButtonType::kCount)> is_pressed_;
      std::array<bool, static_cast<std::size_t>(Driver::ButtonType::kCount)> is_long_pressed_;
      std::array<uint32_t, static_cast<std::size_t>(Driver::ButtonType::kCount)> press_start_ms_;

      std::array<Event, kEventQueueSize> event_queue_;
      std::size_t queue_head_;
      std::size_t queue_tail_;

      int prev_encoder_value_;

      /**
       * @brief イベントキューにイベントを追加する。満杯の場合は最古イベントを破棄する。
       */
      void PushEvent(Event event);

      /**
       * @brief ButtonStateから指定ボタンの押下状態を返す
       */
      static bool GetButtonPressed(const Driver::ButtonState &state, Driver::ButtonType id);
  };

} // namespace App

#endif /* APP_INPUTMANAGER_INPUTMANAGER_HPP_ */
