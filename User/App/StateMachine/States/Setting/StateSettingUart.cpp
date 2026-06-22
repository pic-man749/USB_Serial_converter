/*
 * StateSettingUart.cpp
 *
 *      Author: picman
 */
#include "StateSettingUart.hpp"
#include <memory>
#include <cstdio>
#include "BinaryGFX.hpp"

namespace App {

  StateSettingUart::StateSettingUart(AppConfig &config, IConfigApplicable &applicable) :
      CursorMenuState(kItemCount), config_(config), applicable_(applicable) {

  }

  void StateSettingUart::Enter() {
    SetCursorIndex(static_cast<uint8_t>(config_.selectedUart));
  }

  ExecuteResult StateSettingUart::HandleSelection(const ButtonEvent &e) {
    if(e.button_id == Driver::ButtonType::Center) {
      // 確定: 設定を更新して StateSetting へ戻る
      config_.selectedUart = static_cast<UartChannel>(cursorIndex_);
      applicable_.setChannel(config_.selectedUart);
      applicable_.setBaudRate(config_.baudRate);
      return ExecuteResult::transitionTo(StateId::Setting);
    }
    if(e.button_id == Driver::ButtonType::Left) {
      // キャンセル: 変更せずに戻る
      return ExecuteResult::transitionTo(StateId::Setting);
    }
    return ExecuteResult::None();
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
