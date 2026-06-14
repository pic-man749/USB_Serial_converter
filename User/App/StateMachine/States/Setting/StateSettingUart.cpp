/*
 * StateSettingUart.cpp
 *
 *      Author: picman
 */
#include "StateSettingUart.hpp"
#include <variant>
#include <memory>
#include <algorithm>
#include <cstdio>
#include "Common/OverloadHelper.hpp"
#include "BinaryGFX.hpp"

namespace App {

  StateSettingUart::StateSettingUart(AppConfig &config, IConfigApplicable &applicable) :
      IState(), config_(config), applicable_(applicable), cursorIndex_(0U) {

  }

  void StateSettingUart::Enter(const UpdateContext&) {
    cursorIndex_ = static_cast<uint8_t>(config_.selectedUart);
  }

  void StateSettingUart::Exit() {
  }

  ProcessResult StateSettingUart::ProcessEvent(const Event &event) {
    return std::visit(Common::overload {
      [](const NoneEvent&) -> ProcessResult {
        return ProcessResult::None();
      },
      [this](const EncoderRotateEvent &e) -> ProcessResult {
        const int32_t next = static_cast<int32_t>(cursorIndex_) + e.delta;
        cursorIndex_ = static_cast<uint8_t>(std::max(static_cast<int32_t>(0), std::min(static_cast<int32_t>(kItemCount - 1U), next)));
        return ProcessResult::executed(true);
      },
      [this](const ButtonEvent& e) -> ProcessResult {
        if(e.type != ButtonEventType::kPress) {
          return ProcessResult::None();
        }
        if(e.button_id == Driver::ButtonType::Center) {
          // 確定: 設定を更新して StateSetting へ戻る
          config_.selectedUart = static_cast<UartChannel>(cursorIndex_);
          applicable_.setChannel(config_.selectedUart);
          return ProcessResult::transitionTo(StateId::Setting);
        }
        if(e.button_id == Driver::ButtonType::Left) {
          // キャンセル: 変更せずに戻る
          return ProcessResult::transitionTo(StateId::Setting);
        }
        return ProcessResult::None();
      },
      [](const auto&) -> ProcessResult {
        return ProcessResult::None();
      }
    }, event);
  }

  void StateSettingUart::Render(const RenderContext &context) {
    drawScreen(*context.RightOled);
  }

  // ---------------------------------------------------------------------------
  void StateSettingUart::drawScreen(BinaryGFX::BinaryGFX &oled) const {
    oled.removeAll();
    auto addText = [&oled](int16_t x, int16_t y, const char *text) {
      auto obj = std::make_unique<BinaryGFX::TextObject>(x, y, text, &BinaryGFX::BgfxFont_Ascii);
      obj->setCharSpacing(1U);
      oled.addObject(std::move(obj));
    };
    addText(0, 0, "UART SELECT");
    const char *labels[kItemCount] = { "UART1", "UART2", "UART3" };
    char lines[kItemCount][22];
    for(uint8_t i = 0U; i < kItemCount; ++i) {
      snprintf(lines[i], sizeof(lines[i]), "%c %s", (cursorIndex_ == i) ? '>' : ' ', labels[i]);
      addText(0, static_cast<int16_t>(16 + i * 8), lines[i]);
    }
    addText(0, 56, "[<]Back [o]OK");
    oled.update();
  }

} // namespace App
