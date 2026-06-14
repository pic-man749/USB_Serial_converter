/*
 * StateSettingBaudRateCustom.hpp
 *
 * エンコーダとボタンを使って任意のボーレートを桁単位で入力する設定ステート。
 * 入力範囲: 2565 〜 10500000 bps
 */
#ifndef APP_STATEMACHINE_STATES_SETTING_STATESETTINGBAUDRATECUSTOM_HPP_
#define APP_STATEMACHINE_STATES_SETTING_STATESETTINGBAUDRATECUSTOM_HPP_
#include <cstdint>
#include <array>
#include "../../IState.hpp"
#include "../../../AppConfig/AppConfig.hpp"
#include "../../../IConfigApplicable.hpp"

namespace App {

  class StateSettingBaudRateCustom: public IState {

    public:
      StateSettingBaudRateCustom(AppConfig &config, IConfigApplicable &applicable);
      virtual ~StateSettingBaudRateCustom() = default;
      void Enter(const UpdateContext &context) override;
      void Exit() override;
      ProcessResult ProcessEvent(const Event &event) override;
      void Render(const RenderContext &context) override;

    private:
      /** 入力可能な最小ボーレート */
      static constexpr uint32_t kMinBaud = 2565U;
      /** 入力可能な最大ボーレート */
      static constexpr uint32_t kMaxBaud = 10500000U;
      /** digits_ の最大桁数（kMaxBaud の桁数） */
      static constexpr uint8_t kMaxDigits = 8U;
      AppConfig &config_;
      IConfigApplicable &applicable_;
      /** 各桁の値（インデックス 0 が最上位桁） */
      std::array<uint8_t, kMaxDigits> digits_;
      /** 編集中の桁インデックス（0 が最上位） */
      uint8_t cursorDigit_;
      /** 範囲外エラーフラグ */
      bool valueError_;
      /** @brief digits_ の内容を数値に変換する */
      uint32_t digitsToValue() const;
      /** @brief 右 OLED に桁入力画面を描画する */
      void drawScreen(BinaryGFX::BinaryGFX &oled) const;
  };

} // namespace App

#endif /* APP_STATEMACHINE_STATES_SETTING_STATESETTINGBAUDRATECUSTOM_HPP_ */
