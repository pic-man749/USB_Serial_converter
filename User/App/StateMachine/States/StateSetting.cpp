/*
 * StateSetting.cpp
 *
 *      Author: picman
 */
#include "StateSetting.hpp"
#include <memory>
#include <cstdio>
#include "BinaryGFX.hpp"

namespace App {

      StateSetting::StateSetting(const AppConfig& config) :
          CursorMenuState(kItemCount), config_(config) {

  }

  void StateSetting::Enter() {
    SetCursorIndex(0U);
  }

  ExecuteResult StateSetting::HandleSelection(const ButtonEvent& e) {
    if(e.button_id == Driver::ButtonType::Center) {
      switch(cursorIndex_) {
        case 0U: return ExecuteResult::transitionTo(StateId::SettingUart);
        case 1U: return ExecuteResult::transitionTo(StateId::SettingBaudRate);
        case 2U: return ExecuteResult::transitionTo(StateId::SettingFormat);
        default: break;
      }
    }
    if(e.button_id == Driver::ButtonType::Left) {
      return ExecuteResult::transitionTo(StateId::MonitorCommunication);
    }
    return ExecuteResult::None();
  }

  void StateSetting::Render(const RenderContext& context) {
    context.LeftOled->removeAll();
    context.LeftOled->update();
    drawMenu(*context.RightOled);
  }

  // ---------------------------------------------------------------------------
  void StateSetting::drawMenu(BinaryGFX::BinaryGFX& oled) const {
    oled.removeAll();
    auto addText = [&oled](int16_t x, int16_t y, const char* text) {
      auto obj = std::make_unique<BinaryGFX::TextObject>(
          x, y, text, &BinaryGFX::BgfxFont_Ascii);
      obj->setCharSpacing(1U);
      oled.addObject(std::move(obj));
    };
    addText(0, 0, "SETTINGS");
    // UART 項目
    const char* uartStr = App::GetUartChannelStr(config_.selectedUart);
    char line0[22];
    snprintf(line0, sizeof(line0), "%c UART : %s",
        (cursorIndex_ == 0U) ? '>' : ' ', uartStr);
    addText(0, 8, line0);
    // BaudRate 項目
    char baudStr[9];
    snprintf(baudStr, sizeof(baudStr), "%u",
        static_cast<unsigned int>(config_.baudRate));
    char line1[22];
    snprintf(line1, sizeof(line1), "%c BAUD : %s",
        (cursorIndex_ == 1U) ? '>' : ' ', baudStr);
    addText(0, 16, line1);
    // Format 項目
    const char* modeStr =
        (config_.displayMode == DisplayMode::Hex) ? "HEX" : "ASCII";
    char line2[22];
    snprintf(line2, sizeof(line2), "%c MODE : %s",
        (cursorIndex_ == 2U) ? '>' : ' ', modeStr);
    addText(0, 24, line2);
    // フッタ行
    addText(0, 56, "[<]Back [o]Enter");
    oled.update();
  }

} // namespace App
