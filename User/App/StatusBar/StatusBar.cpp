/*
 * StatusBar.cpp
 */

#include "App/StatusBar/StatusBar.hpp"
#include <cstring>
#include <cstdio>

namespace App {

StatusBar::StatusBar(BinaryGFX::BinaryGFX& oledTx,
                     BinaryGFX::BinaryGFX& oledRx,
                     const Settings&       settings)
    : oledTx_(oledTx),
      oledRx_(oledRx),
      settings_(settings),
      txStatusId_(BinaryGFX::InvalidObjectId),
      rxStatusId_(BinaryGFX::InvalidObjectId)
{
    txStatusBuf_[0] = '\0';
    rxStatusBuf_[0] = '\0';
}

void StatusBar::init() {
    // 初期文字列を生成してバッファに書き込む
    formatStatus(txStatusBuf_, LINE_BUF_SIZE);
    strncpy(rxStatusBuf_, txStatusBuf_, LINE_BUF_SIZE);

    // OLED1 (TX 側) へ登録（Z=10 でデータ行より前面に表示）
    txStatusId_ = oledTx_.addObject(
        std::make_unique<BinaryGFX::TextObject>(
            0, STATUS_Y,
            txStatusBuf_,
            &BinaryGFX::BgfxFont_Ascii,
            BinaryGFX::PixelState::On,
            10));

    // OLED2 (RX 側) へ登録
    rxStatusId_ = oledRx_.addObject(
        std::make_unique<BinaryGFX::TextObject>(
            0, STATUS_Y,
            rxStatusBuf_,
            &BinaryGFX::BgfxFont_Ascii,
            BinaryGFX::PixelState::On,
            10));

    lastRendered_ = settings_;
}

void StatusBar::render() {
    // 設定が変化していなければスキップ
    if (settings_.channel     == lastRendered_.channel     &&
        settings_.baudRateIdx == lastRendered_.baudRateIdx &&
        settings_.format      == lastRendered_.format) {
        return;
    }

    formatStatus(txStatusBuf_, LINE_BUF_SIZE);
    strncpy(rxStatusBuf_, txStatusBuf_, LINE_BUF_SIZE);
    lastRendered_ = settings_;
    // TextObject は txStatusBuf_ / rxStatusBuf_ を直接参照しているため
    // バッファ更新のみで次回 BinaryGFX::update() 時に反映される
}

void StatusBar::setTxVisible(bool visible) {
    auto* obj = oledTx_.getObjectById<BinaryGFX::TextObject>(txStatusId_);
    if (obj) {
        obj->setPixelState(visible
            ? BinaryGFX::PixelState::On
            : BinaryGFX::PixelState::Off);
    }
}

void StatusBar::formatStatus(char* buf, uint8_t bufLen) const {
    const char* chStr  = (settings_.channel == UartChannel::UART1) ? "U1"
                       : (settings_.channel == UartChannel::UART2) ? "U2"
                                                                     : "U6";
    const char* fmtStr = (settings_.format == DisplayFormat::ASCII) ? "ASC" : "HEX";

    snprintf(buf, bufLen, "%s %lu %s",
             chStr,
             static_cast<unsigned long>(settings_.baudRate()),
             fmtStr);
}

} // namespace App
