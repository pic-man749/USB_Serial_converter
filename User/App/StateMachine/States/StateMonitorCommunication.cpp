/*
 * StateMonitorCommunication.cpp
 *
 *      Author: picman
 */
#include "StateMonitorCommunication.hpp"
#include <variant>
#include <memory>
#include <algorithm>
#include "Common/OverloadHelper.hpp"
#include "BinaryGFX.hpp"

namespace App {

  StateMonitorCommunication::StateMonitorCommunication(CommunicationManager &commMgr, const AppConfig &config) :
      IState(), commMgr_(commMgr), config_(config), scrollOffset_(0) {

  }

  void StateMonitorCommunication::Enter() {
    commMgr_.start();
    scrollOffset_ = 0;
  }

  void StateMonitorCommunication::Exit() {
    // パススルーは設定画面への遷移中も継続させるため stop() を呼ばない。
    // チャンネルやボーレートの変更は Setting 系ステートが IConfigApplicable 経由で
    // 直接 CommunicationManager に通知するため、停止・再開は不要。
  }

  ExecuteResult StateMonitorCommunication::HandleEvent(const Event &event) {
    return std::visit(Common::overload {
        [](const NoneEvent&) -> ExecuteResult {
          return ExecuteResult::None();
        },
        [](const CommunicationDataEvent&) -> ExecuteResult {
          // 新着データあり: 再描画を要求する
          return ExecuteResult::executed(true);
        },
        [this](const EncoderRotateEvent &e) -> ExecuteResult {
          // スクロールオフセットを更新する（0 以上でクランプ）
          scrollOffset_ = std::max(static_cast<int32_t>(0), scrollOffset_ + e.delta);
          return ExecuteResult::executed(true);
        },
        [](const ButtonEvent &e) -> ExecuteResult {
          if(e.button_id == Driver::ButtonType::Center && e.type == ButtonEventType::kPress) {
            return ExecuteResult::transitionTo(StateId::Setting);
          } else if(e.button_id == Driver::ButtonType::Left && e.type == ButtonEventType::kLongPress) {
            commMgr_.clearDisplayBuffers();
            return ExecuteResult::executed(true);
          }
          return ExecuteResult::None();
        }
    }, event);
  }

  void StateMonitorCommunication::Render(const RenderContext &context) {
    renderOled(*context.LeftOled, commMgr_.getUartToUsbBuffer(), "UART->USB");
    renderOled(*context.RightOled, commMgr_.getUsbToUartBuffer(), "USB->UART");
  }

  // ---------------------------------------------------------------------------
  void StateMonitorCommunication::renderOled(BinaryGFX::BinaryGFX &oled, const DisplayBuffer &buf,
                                             const char *header) const {
    oled.removeAll();

    // TextObject 生成ヘルパー（charSpacing=1 で 6px ピッチ）
    auto addText = [&oled](int16_t x, int16_t y, const char *text) {
      auto obj = std::make_unique<BinaryGFX::TextObject>(x, y, text, &BinaryGFX::BgfxFont_Ascii);
      obj->setCharSpacing(1U);
      oled.addObject(std::move(obj));
    };

    addText(0, 0, header);
    const size_t bufSize = buf.size();
    if(bufSize > 0U) {
      const bool isHex = (config_.displayMode == DisplayMode::Hex);
      // HEX: 固定バイト幅で行数を計算する。ASCII: LF/折り返しを考慮して行数を計算する。
      const size_t totalRows = isHex
          ? (bufSize + kBytesPerHexRow - 1U) / kBytesPerHexRow
          : countAsciiRows(buf);
      const int32_t maxScroll = (totalRows > kDataRows) ? static_cast<int32_t>(totalRows - kDataRows) : 0;
      const int32_t clampedOffset = std::min(scrollOffset_, maxScroll);
      // clampedOffset=0 のとき最新データが最下行に並ぶ
      const int32_t startRow = static_cast<int32_t>(totalRows) - static_cast<int32_t>(kDataRows) - clampedOffset;
      char lines[kDataRows][kMaxLineLen + 1U];
      for(uint8_t row = 0U; row < kDataRows; ++row) {
        lines[row][0] = '\0';
        const int32_t bufRow = startRow + static_cast<int32_t>(row);
        if(bufRow >= 0 && static_cast<size_t>(bufRow) < totalRows) {
          const size_t byteIndex = isHex
              ? static_cast<size_t>(bufRow) * kBytesPerHexRow
              : getAsciiRowStart(buf, static_cast<size_t>(bufRow));
          if(isHex) {
            buildHexLine(lines[row], buf, byteIndex);
          } else {
            buildAsciiLine(lines[row], buf, byteIndex);
          }
        }
        addText(0, static_cast<int16_t>((1U + row) * kFontHeight), lines[row]);
      }
    }
    oled.update();
  }

  void StateMonitorCommunication::buildHexLine(char *out, const DisplayBuffer &buf, size_t byteIndex) {
    size_t pos = 0U;
    for(uint8_t col = 0U; col < kBytesPerHexRow; ++col) {
      const size_t idx = byteIndex + static_cast<size_t>(col);
      if(idx >= buf.size()) {
        break;
      }
      if(col > 0U) {
        out[pos++] = ' ';
      }
      const uint8_t b = buf.at(idx);
      out[pos++] = nibbleToHex(b >> 4U);
      out[pos++] = nibbleToHex(b & 0x0FU);
    }
    out[pos] = '\0';
  }

  void StateMonitorCommunication::buildAsciiLine(char *out, const DisplayBuffer &buf, size_t byteIndex) {
    size_t pos = 0U;
    for(uint8_t col = 0U; col < kCharsPerAsciiRow; ++col) {
      const size_t idx = byteIndex + static_cast<size_t>(col);
      if(idx >= buf.size()) {
        break;
      }
      const uint8_t b = buf.at(idx);
      // LF/CR は行終端として扱い、表示しない
      if(b == '\n' || b == '\r') {
        break;
      }
      out[pos++] = (b >= 0x20U && b <= 0x7EU) ? static_cast<char>(b) : '.';
    }
    out[pos] = '\0';
  }

  size_t StateMonitorCommunication::countAsciiRows(const DisplayBuffer &buf) {
    if(buf.size() == 0U) {
      return 0U;
    }
    size_t rows = 1U;
    size_t colInRow = 0U;
    for(size_t i = 0U; i < buf.size(); ++i) {
      const uint8_t b = buf.at(i);
      if(b == '\n') {
        ++rows;
        colInRow = 0U;
      } else if(b != '\r') {
        if(++colInRow == kCharsPerAsciiRow) {
          ++rows;
          colInRow = 0U;
        }
      }
    }
    return rows;
  }

  size_t StateMonitorCommunication::getAsciiRowStart(const DisplayBuffer &buf, size_t rowIndex) {
    if(rowIndex == 0U) {
      return 0U;
    }
    size_t row = 0U;
    size_t colInRow = 0U;
    for(size_t i = 0U; i < buf.size(); ++i) {
      const uint8_t b = buf.at(i);
      bool newRow = false;
      if(b == '\n') {
        newRow = true;
        colInRow = 0U;
      } else if(b != '\r') {
        if(++colInRow == kCharsPerAsciiRow) {
          newRow = true;
          colInRow = 0U;
        }
      }
      if(newRow && ++row == rowIndex) {
        return i + 1U;
      }
    }
    return buf.size();
  }

  char StateMonitorCommunication::nibbleToHex(uint8_t n) {
    return (n < 10U) ? static_cast<char>('0' + n) : static_cast<char>('A' + n - 10U);
  }
} // namespace App
