/*
 * StateSettingFormat.cpp
 *
 *      Author: picman
 */
#include "StateSettingFormat.hpp"
#include <variant>
#include <memory>
#include <algorithm>
#include <cstdio>
#include "Common/OverloadHelper.hpp"
#include "BinaryGFX.hpp"

namespace App {

  StateSettingFormat::StateSettingFormat(AppConfig &config) :
      IState(), config_(config), cursorIndex_(0U) {

  }

  void StateSettingFormat::Enter(const UpdateContext&) {
    cursorIndex_ = static_cast<uint8_t>(config_.displayMode);
  }

  void StateSettingFormat::Exit() {
  }

  ProcessResult StateSettingFormat::ProcessEvent(const Event &event) {
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
          config_.displayMode = static_cast<DisplayMode>(cursorIndex_);
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
