/*
 * StateSettingBaudRateCustom.cpp
 *
 *      Author: picman
 */
#include "StateSettingBaudRateCustom.hpp"
#include <variant>
#include <algorithm>
#include <cstdio>
#include "Common/OverloadHelper.hpp"
#include "BinaryGFX.hpp"

namespace App {

  StateSettingBaudRateCustom::StateSettingBaudRateCustom(AppConfig &config, IConfigApplicable &applicable) :
      IState(), config_(config), applicable_(applicable), digits_ { }, cursorDigit_(0U), valueError_(false), isSettingFailed_(false) {

  }

  void StateSettingBaudRateCustom::Enter() {
    valueError_ = false;
    isSettingFailed_ = false;
    // 現在のボーレートを kMaxDigits 桁で展開する（上位桁は 0 パディング）
    uint32_t value = config_.baudRate;
    uint32_t divisor = 1U;
    for(uint8_t i = 0U; i < kMaxDigits - 1U; ++i) {
      divisor *= 10U;
    }
    for(uint8_t i = 0U; i < kMaxDigits; ++i) {
      digits_[i] = static_cast<uint8_t>(value / divisor);
      value %= divisor;
      if(divisor > 1U) {
        divisor /= 10U;
      }
    }
    cursorDigit_ = 0U;
  }

  void StateSettingBaudRateCustom::Exit() {
  }

  ExecuteResult StateSettingBaudRateCustom::HandleEvent(const Event &event) {
    return std::visit(Common::overload {
        [](const NoneEvent&) -> ExecuteResult {
          return ExecuteResult::None();
        },
        [this](const EncoderRotateEvent &e) -> ExecuteResult {
          // 選択中の桁の値を 0〜9 の範囲で変更する（桁上がりなし）
          const int32_t newVal = static_cast<int32_t>(digits_[cursorDigit_]) + e.delta;
          digits_[cursorDigit_] = static_cast<uint8_t>(std::max(static_cast<int32_t>(0), std::min(static_cast<int32_t>(9), newVal)));
          valueError_ = false;
          isSettingFailed_ = false;
          return ExecuteResult::executed(true);
        },
        [this](const ButtonEvent &e) -> ExecuteResult {
          if(e.type != ButtonEventType::kPress) {
            return ExecuteResult::None();
          }
          if(e.button_id == Driver::ButtonType::Right) {
            // 下位桁へ移動する
            if(cursorDigit_ < kMaxDigits - 1U) {
              ++cursorDigit_;
            }
            return ExecuteResult::executed(true);
          }
          if(e.button_id == Driver::ButtonType::Left) {
            if(cursorDigit_ == 0U) {
              // 最上位桁でキャンセル → StateSettingBaudRate へ戻る
              return ExecuteResult::transitionTo(StateId::SettingBaudRate);
            }
            --cursorDigit_;
            return ExecuteResult::executed(true);
          }
          if(e.button_id == Driver::ButtonType::Center) {
            // 決定
            const uint32_t value = digitsToValue();
            if(value >= kMinBaud && value <= kMaxBaud) {
              if(applicable_.setBaudRate(value)){
                config_.baudRate = value;
                return ExecuteResult::transitionTo(StateId::Setting);
              } else {
                // HALでエラーがかえってきた場合
                isSettingFailed_ = true;
                return ExecuteResult::executed(true);
              }
            }
            // 範囲外の場合はエラー表示して入力を継続する
            valueError_ = true;
            return ExecuteResult::executed(true);
          }
          return ExecuteResult::None();
        },
        [](const auto&) -> ExecuteResult {
          return ExecuteResult::None();
        }
      }, event);
    }

  void StateSettingBaudRateCustom::Render(const RenderContext &context) {
    drawScreen(*context.RightOled);
  }

  // ---------------------------------------------------------------------------
  uint32_t StateSettingBaudRateCustom::digitsToValue() const {
    uint32_t result = 0U;
    for(uint8_t i = 0U; i < kMaxDigits; ++i) {
      result = result * 10U + static_cast<uint32_t>(digits_[i]);
    }
    return result;
  }

  void StateSettingBaudRateCustom::drawScreen(BinaryGFX::BinaryGFX &oled) const {
    oled.removeAll();

    BinaryGFX::createText(oled, 0, 0, "BAUD CUSTOM");
    // 桁列を構築する（形式: "D D D D D D D D"）
    char digitStr[kMaxDigits * 2U];
    size_t pos = 0U;
    for(uint8_t i = 0U; i < kMaxDigits; ++i) {
      if(i > 0U) {
        digitStr[pos++] = ' ';
      }
      digitStr[pos++] = static_cast<char>('0' + digits_[i]);
    }
    digitStr[pos] = '\0';
    BinaryGFX::createText(oled, 0, 16, digitStr);
    // カーソル行（選択中の桁の直下に '^' を表示する）
    char cursorStr[kMaxDigits * 2U];
    for(uint8_t i = 0U; i < kMaxDigits * 2U - 1U; ++i) {
      cursorStr[i] = (i == cursorDigit_ * 2U) ? '^' : ' ';
    }
    cursorStr[kMaxDigits * 2U - 1U] = '\0';
    BinaryGFX::createText(oled, 0, 24, cursorStr);
    // 有効範囲の表示
    BinaryGFX::createText(oled, 0, 32, "2565-10500000");
    // エラーメッセージ
    if(valueError_) {
      BinaryGFX::createText(oled, 0, 40, "OUT OF RANGE");
    }
    // 失敗
    if(isSettingFailed_){
      BinaryGFX::createText(oled, 0, 40, "INVALID VALUE");
    }
    BinaryGFX::createText(oled, 0, 56, "[<]Back [o]OK");
    oled.update();
  }

} // namespace App
