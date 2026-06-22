/*
 * StateSettingFormat.cpp
 *
 *      Author: picman
 */
#include "StateSettingFormat.hpp"
#include <memory>
#include <cstdio>
#include "BinaryGFX.hpp"

namespace App {

  StateSettingFormat::StateSettingFormat(AppConfig &config) :
      CursorMenuState(kItemCount), config_(config) {

  }

  void StateSettingFormat::Enter() {
    SetCursorIndex(static_cast<uint8_t>(config_.displayMode));
  }

  ExecuteResult StateSettingFormat::HandleSelection(const ButtonEvent &e) {
    if(e.button_id == Driver::ButtonType::Center) {
      // 確定: 設定を更新して StateSetting へ戻る
      config_.displayMode = static_cast<DisplayMode>(cursorIndex_);
      return ExecuteResult::transitionTo(StateId::Setting);
    }
    if(e.button_id == Driver::ButtonType::Left) {
      // キャンセル: 変更せずに戻る
      return ExecuteResult::transitionTo(StateId::Setting);
    }
    return ExecuteResult::None();
  }

  void StateSettingFormat::Render(const RenderContext &context) {
    drawScreen(*context.RightOled);
  }

  // ---------------------------------------------------------------------------
  void StateSettingFormat::drawScreen(BinaryGFX::BinaryGFX &oled) const {
    oled.removeAll();
    auto addText = [&oled](int16_t x, int16_t y, const char *text) {
      auto obj = std::make_unique<BinaryGFX::TextObject>(x, y, text, &BinaryGFX::BgfxFont_Ascii);
      obj->setCharSpacing(1U);
      oled.addObject(std::move(obj));
    };
    addText(0, 0, "DISP FORMAT");
    const char *labels[kItemCount] = { "HEX", "ASCII" };
    char lines[kItemCount][22];
    for(uint8_t i = 0U; i < kItemCount; ++i) {
      snprintf(lines[i], sizeof(lines[i]), "%c %s", (cursorIndex_ == i) ? '>' : ' ', labels[i]);
      addText(0, static_cast<int16_t>(16 + i * 8), lines[i]);
    }
    addText(0, 56, "[<]Back [o]OK");
    oled.update();
  }

} // namespace App
