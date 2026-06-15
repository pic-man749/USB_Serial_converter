/*
 * InputManager.hpp
 *
 * ボタンとエンコーダの入力状態を監視し、イベントを EventQueue に push するクラス。
 * IEventSource を実装し、EventBus に登録して使用する。
 */

#ifndef APP_INPUTMANAGER_INPUTMANAGER_HPP_
#define APP_INPUTMANAGER_INPUTMANAGER_HPP_

#include <array>
#include <cstdint>
#include "../IEventSource.hpp"
#include "../../Driver/Button/Button.hpp"
#include "../../Driver/Encoder/Encoder.hpp"

namespace App {

  class InputManager: public IEventSource {
    public:
      /** 長押し判定閾値（ミリ秒） */
      static constexpr uint32_t kLongPressThresholdMs = 1000U;

      InputManager();

      /**
       * @brief ドライバを初期化する。HAL 初期化後、Update() 呼び出し前に一度だけ呼び出すこと。
       */
      void init();

      /**
       * @brief 入力状態を読み取り、発生したイベントを queue に push する。
       *        EventBus::Update() から呼び出される。
       */
      void Update(EventQueue &queue) override;

    private:
      Driver::Button button_;
      Driver::Encoder encoder_;

      std::array<bool, static_cast<size_t>(Driver::ButtonType::kCount)> is_pressed_;
      std::array<bool, static_cast<size_t>(Driver::ButtonType::kCount)> is_long_pressed_;
      std::array<uint32_t, static_cast<size_t>(Driver::ButtonType::kCount)> press_start_ms_;

      int prev_encoder_value_;

      static bool GetButtonPressed(const Driver::ButtonState &state, Driver::ButtonType id);
  };

} // namespace App

#endif /* APP_INPUTMANAGER_INPUTMANAGER_HPP_ */
