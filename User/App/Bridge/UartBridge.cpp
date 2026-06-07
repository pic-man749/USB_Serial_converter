/*
 * UartBridge.cpp
 */

#include "App/Bridge/UartBridge.hpp"
#include <algorithm>

namespace App {

UartBridge::UartBridge(Driver::UsbCdc&  usb,
                       Driver::Uart&    uart1,
                       Driver::Uart&    uart2,
                       Driver::Uart&    uart6,
                       IDataObserver&   observer)
    : usb_(usb),
      uarts_{ &uart1, &uart2, &uart6 },
      activeUart_(&uart1),
      observer_(observer)
{
}

void UartBridge::process() {
    // --- USB → UART 転送 ---
    const uint32_t usbRxSize = usb_.getReceiveDataSize();
    if (usbRxSize > 0) {
        const uint32_t readLen = std::min(usbRxSize, TRANSFER_BUF_SIZE);
        if (usb_.getReceiveData(txBuf_, readLen)) {
            activeUart_->send(reinterpret_cast<const char*>(txBuf_), readLen);
            observer_.onTxData(txBuf_, readLen);
        }
    }

    // --- UART → USB 転送 ---
    const uint32_t uartRxSize = activeUart_->getReceiveDataSize();
    if (uartRxSize > 0) {
        const uint32_t readLen = std::min(uartRxSize, TRANSFER_BUF_SIZE);
        const uint32_t actual  = activeUart_->getReceiveData(rxBuf_, readLen);
        if (actual > 0) {
            usb_.send(rxBuf_, static_cast<uint16_t>(actual));
            observer_.onRxData(rxBuf_, actual);
        }
    }
}

void UartBridge::applySettings(const Settings& s) {
    Driver::Uart* nextUart = channelToUart(s.channel);

    // 現在のアクティブ UART を停止してバッファをクリア
    activeUart_->stopReceive();
    activeUart_->clearReceiveBuffer();

    // USB 受信キューをドレイン（切り替え前のデータを破棄）
    while (usb_.getReceiveDataSize() > 0) {
        uint8_t drain[TRANSFER_BUF_SIZE];
        const uint32_t len = std::min(usb_.getReceiveDataSize(), TRANSFER_BUF_SIZE);
        usb_.getReceiveData(drain, len);
    }

    // 新しい UART にボーレートを設定して受信を開始
    activeUart_ = nextUart;
    activeUart_->setbaudrate(s.baudRate());
    activeUart_->startReceive();
}

Driver::Uart* UartBridge::channelToUart(UartChannel ch) {
    return uarts_[static_cast<uint8_t>(ch)];
}

} // namespace App
