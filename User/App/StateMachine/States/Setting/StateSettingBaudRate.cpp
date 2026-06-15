/*
 * StateSettingBaudRate.cpp
 *
 *      Author: picman
 */
#include "StateSettingBaudRate.hpp"
#include <variant>
#include <memory>
#include <algorithm>
#include <cstdio>
#include "Common/OverloadHelper.hpp"
#include "BinaryGFX.hpp"
namespace App {

  const std::array<uint32_t, StateSettingBaudRate::kPresetCount> StateSettingBaudRate::kPresets = {
      9600U, 38400U, 115200U, 921600U, 3000000U };
  StateSettingBaudRate::StateSettingBaudRate(AppConfig &config, IConfigApplicable &applicable) :
      IState(), config_(config), applicable_(applicable), cursorIndex_(0U) {

  }

  void StateSettingBaudRate::Enter() {
    // 現在のボーレートがプリセットに一致すればその位置、なければ Custom... を初期カーソルにする
    cursorIndex_ = kCustomIndex;
    for(uint8_t i = 0U; i < kPresetCount; ++i) {
      if(config_.baudRate == kPresets[i]) {
        cursorIndex_ = i;
        break;
      }
    }
  }

  void StateSettingBaudRate::Exit() {
  }

  ExecuteResult StateSettingBaudRate::HandleEvent(const Event &event) {
    return std::visit(Common::overload {
        [](const NoneEvent&) -> ExecuteResult {
          return ExecuteResult::None();
        },
        [this](const EncoderRotateEvent &e) -> ExecuteResult {
          const int32_t next = static_cast<int32_t>(cursorIndex_) + e.delta;
          cursorIndex_ = static_cast<uint8_t>(std::max(static_cast<int32_t>(0), std::min(static_cast<int32_t>(kItemCount - 1U), next)));
          return ExecuteResult::executed(true);
        },
        [this](const ButtonEvent& e) -> ExecuteResult {
          if(e.type != ButtonEventType::kPress) {
            return ExecuteResult::None();
          }
          if(e.button_id == Driver::ButtonType::Center) {
            if(cursorIndex_ == kCustomIndex) {
              // 任意値入力へ遷移する
              return ExecuteResult::transitionTo(StateId::SettingBaudRateCustom);
            }
            // プリセット確定
            config_.baudRate = kPresets[cursorIndex_];
            applicable_.setBaudRate(config_.baudRate);
            return ExecuteResult::transitionTo(StateId::Setting);
          }
          if(e.button_id == Driver::ButtonType::Left) {
            return ExecuteResult::transitionTo(StateId::Setting);
          }
          return ExecuteResult::None();
        },
        [](const auto&) -> ExecuteResult {
          return ExecuteResult::None();
        }
      }, event);
    }

  void StateSettingBaudRate::Render(const RenderContext &context) {
    drawScreen(*context.RightOled);
  }

  // ---------------------------------------------------------------------------
  void StateSettingBaudRate::drawScreen(BinaryGFX::BinaryGFX &oled) const {
    oled.removeAll();
    auto addText = [&oled](int16_t x, int16_t y, const char *text) {
      auto obj = std::make_unique<BinaryGFX::TextObject>(x, y, text, &BinaryGFX::BgfxFont_Ascii);
      obj->setCharSpacing(1U);
      oled.addObject(std::move(obj));
    };
    addText(0, 0, "BAUD RATE");
    // プリセット行
    char lines[kPresetCount][22];
    for(uint8_t i = 0U; i < kPresetCount; ++i) {
      snprintf(lines[i], sizeof(lines[i]), "%c %u", (cursorIndex_ == i) ? '>' : ' ',
          static_cast<unsigned int>(kPresets[i]));
      addText(0, static_cast<int16_t>(8 + i * 8), lines[i]);
    }
    // Custom... 行
    char customLine[22];
    snprintf(customLine, sizeof(customLine), "%c Custom...",
        (cursorIndex_ == kCustomIndex) ? '>' : ' ');
    addText(0, static_cast<int16_t>(8 + kPresetCount * 8), customLine);
    addText(0, 56, "[<]Back [o]OK");
    oled.update();
  }

} // namespace App
