/*
 * DisplayManager.cpp
 */

#include "App/Display/DisplayManager.hpp"
#include <algorithm>
#include <cstring>

namespace App {

DisplayManager::DisplayManager(BinaryGFX::BinaryGFX& oledTx,
                               BinaryGFX::BinaryGFX& oledRx,
                               const Settings&       settings)
    : oledTx_(oledTx),
      oledRx_(oledRx),
      settings_(settings),
      txBuf_(1024),
      rxBuf_(1024)
{
    // 文字列バッファをゼロ初期化（TextObject が空文字列を参照する状態にする）
    memset(txLineBuffers_, 0, sizeof(txLineBuffers_));
    memset(rxLineBuffers_, 0, sizeof(rxLineBuffers_));
}

void DisplayManager::init() {
    // TX 側: OLED1 へ TextObject を登録
    for (uint8_t i = 0; i < DATA_LINES; ++i) {
        txLineIds_[i] = oledTx_.addObject(
            std::make_unique<BinaryGFX::TextObject>(
                0, static_cast<int16_t>(i * 8),
                txLineBuffers_[i],
                &BinaryGFX::BgfxFont_Ascii));
    }

    // RX 側: OLED2 へ TextObject を登録
    for (uint8_t i = 0; i < DATA_LINES; ++i) {
        rxLineIds_[i] = oledRx_.addObject(
            std::make_unique<BinaryGFX::TextObject>(
                0, static_cast<int16_t>(i * 8),
                rxLineBuffers_[i],
                &BinaryGFX::BgfxFont_Ascii));
    }
}

void DisplayManager::onTxData(const uint8_t* data, uint32_t len) {
    txBuf_.push(data, len);
    updateAutoScroll();
}

void DisplayManager::onRxData(const uint8_t* data, uint32_t len) {
    rxBuf_.push(data, len);
    updateAutoScroll();
}

void DisplayManager::handleInput(InputEvent ev) {
    const uint32_t txTotal = txBuf_.lineCount(settings_.format);
    const uint32_t rxTotal = rxBuf_.lineCount(settings_.format);
    const uint32_t total   = std::max(txTotal, rxTotal);

    const uint32_t maxTopLine = (total > DATA_LINES)
                                ? (total - DATA_LINES)
                                : 0u;

    switch (ev) {
        case InputEvent::SCROLL_UP:
            if (topLine_ > 0) {
                --topLine_;
                autoScroll_ = false;
            }
            break;

        case InputEvent::SCROLL_DOWN:
            if (topLine_ < maxTopLine) {
                ++topLine_;
            }
            // 最新行に到達したらオートスクロールを再開
            autoScroll_ = (topLine_ >= maxTopLine);
            break;

        default:
            break;
    }
}

void DisplayManager::render() {
    // 表示形式が変わった場合はオートスクロール位置をリセット
    if (settings_.format != lastFormat_) {
        lastFormat_ = settings_.format;
        autoScroll_ = true;
    }
    updateAutoScroll();

    renderDisplay(oledTx_, txBuf_, txLineBuffers_);
    renderDisplay(oledRx_, rxBuf_, rxLineBuffers_);
}

void DisplayManager::renderRx() {
    updateAutoScroll();
    renderDisplay(oledRx_, rxBuf_, rxLineBuffers_);
}

void DisplayManager::clearBuffers() {
    txBuf_.clear();
    rxBuf_.clear();
    topLine_    = 0;
    autoScroll_ = true;
}

void DisplayManager::setTxVisible(bool visible) {
    const BinaryGFX::PixelState ps = visible
        ? BinaryGFX::PixelState::On
        : BinaryGFX::PixelState::Off;

    for (uint8_t i = 0; i < DATA_LINES; ++i) {
        auto* obj = oledTx_.getObjectById<BinaryGFX::TextObject>(txLineIds_[i]);
        if (obj) obj->setPixelState(ps);
    }
}

void DisplayManager::updateAutoScroll() {
    if (!autoScroll_) return;

    const uint32_t txTotal = txBuf_.lineCount(settings_.format);
    const uint32_t rxTotal = rxBuf_.lineCount(settings_.format);
    const uint32_t total   = std::max(txTotal, rxTotal);

    topLine_ = (total > DATA_LINES) ? (total - DATA_LINES) : 0u;
}

void DisplayManager::renderDisplay(BinaryGFX::BinaryGFX& oled,
                                   DisplayBuffer&        buf,
                                   char                  lines[][LINE_BUF_SIZE]) {
    for (uint8_t i = 0; i < DATA_LINES; ++i) {
        buf.getLine(topLine_ + i, settings_.format, lines[i], LINE_BUF_SIZE);
        // TextObject は lines[i] のアドレスを保持しているため
        // バッファ内容を更新するだけで次回 update() 時に反映される
    }
    (void)oled;  // TextObject はすでに oled に登録済み。update() は UserMain が呼ぶ
}

} // namespace App
