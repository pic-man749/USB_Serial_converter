/*
 * StateSettingBaudRate.cpp
 *
 *      Author: picman
 */
#include "StateSettingBaudRate.hpp"
#include <cstdio>
#include "BinaryGFX.hpp"
namespace App {

  const std::array<uint32_t, StateSettingBaudRate::kPresetCount> StateSettingBaudRate::kPresets = {
      9600U, 38400U, 115200U, 921600U, 3000000U };
  StateSettingBaudRate::StateSettingBaudRate(AppConfig &config, IConfigApplicable &applicable) :
      CursorMenuState(kItemCount), config_(config), applicable_(applicable) {

  }

  void StateSettingBaudRate::Enter() {
    // 現在のボーレートがプリセットに一致すればその位置、なければ Custom... を初期カーソルにする
    uint8_t initialIndex = kCustomIndex;
    for(uint8_t i = 0U; i < kPresetCount; ++i) {
      if(config_.baudRate == kPresets[i]) {
        initialIndex = i;
        break;
      }
    }
    SetCursorIndex(initialIndex);
  }

  ExecuteResult StateSettingBaudRate::HandleSelection(const ButtonEvent &e) {
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
  }

  void StateSettingBaudRate::Render(const RenderContext &context) {
    drawScreen(*context.RightOled);
  }

  // ---------------------------------------------------------------------------
  void StateSettingBaudRate::drawScreen(BinaryGFX::BinaryGFX &oled) const {
    oled.removeAll();

    BinaryGFX::createText(oled, 0, 0, "BAUD RATE");
    // プリセット行
    char lines[kPresetCount][22];
    for(uint8_t i = 0U; i < kPresetCount; ++i) {
      snprintf(lines[i], sizeof(lines[i]), "%c %u", (cursorIndex_ == i) ? '>' : ' ',
          static_cast<unsigned int>(kPresets[i]));
      BinaryGFX::createText(oled, 0, static_cast<int16_t>(8 + i * 8), lines[i]);
    }
    // Custom... 行
    char customLine[22];
    snprintf(customLine, sizeof(customLine), "%c Custom...",
        (cursorIndex_ == kCustomIndex) ? '>' : ' ');
    BinaryGFX::createText(oled, 0, static_cast<int16_t>(8 + kPresetCount * 8), customLine);
    BinaryGFX::createText(oled, 0, 56, "[<]Back [o]OK");
    oled.update();
  }

} // namespace App
