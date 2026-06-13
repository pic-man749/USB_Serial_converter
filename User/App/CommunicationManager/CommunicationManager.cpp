/*
 * CommunicationManager.cpp
 *
 *      Author: picman
 */

#include "CommunicationManager.hpp"
#include <algorithm>

namespace App {

  CommunicationManager::CommunicationManager(
      Driver::Uart&   uart1,
      Driver::Uart&   uart2,
      Driver::Uart&   uart3,
      Driver::UsbCdc& usbCdc,
      AppConfig&      config
  ) :
      uarts_ { &uart1, &uart2, &uart3 },
      usbCdc_(usbCdc),
      config_(config),
      currentChannel_(config.selectedUart),
      currentBaudRate_(config.baudRate),
      uartToUsbBuffer_(),
      usbToUartBuffer_(),
      running_(false) {
  }

  void CommunicationManager::start() {
    if(running_) {
      return;
    }
    currentChannel_  = config_.selectedUart;
    currentBaudRate_ = config_.baudRate;
    getActiveUart().startReceive();
    running_ = true;
  }

  void CommunicationManager::stop() {
    if(!running_) {
      return;
    }
    getActiveUart().stopReceive();
    running_ = false;
  }

  void CommunicationManager::update() {
    if(!running_) {
      return;
    }

    // UARTチャンネル切り替えの自動検知
    if(config_.selectedUart != currentChannel_) {
      getActiveUart().stopReceive();
      currentChannel_ = config_.selectedUart;
      getActiveUart().startReceive();
    }

    // ボーレート変更の自動検知
    if(config_.baudRate != currentBaudRate_) {
      currentBaudRate_ = config_.baudRate;
      getActiveUart().setbaudrate(currentBaudRate_);
    }

    uint8_t buf[kTransferBufSize];

    // USB → UART 方向の転送
    uint32_t usbRxSize = usbCdc_.getReceiveDataSize();
    while(usbRxSize > 0U) {
      const uint32_t chunk = std::min(usbRxSize, kTransferBufSize);
      if(!usbCdc_.getReceiveData(buf, chunk)) {
        break;
      }
      getActiveUart().send(reinterpret_cast<const char*>(buf), chunk);
      usbToUartBuffer_.push(buf, chunk);
      usbRxSize -= chunk;
    }

    // UART → USB 方向の転送
    uint32_t uartRxSize = getActiveUart().getReceiveDataSize();
    while(uartRxSize > 0U) {
      const uint32_t chunk = std::min(uartRxSize, kTransferBufSize);
      const uint32_t read  = getActiveUart().getReceiveData(buf, chunk);
      if(read == 0U) {
        break;
      }
      usbCdc_.send(buf, static_cast<uint16_t>(read));
      uartToUsbBuffer_.push(buf, read);
      uartRxSize -= read;
    }
  }

  const DisplayBuffer& CommunicationManager::getUartToUsbBuffer() const {
    return uartToUsbBuffer_;
  }

  const DisplayBuffer& CommunicationManager::getUsbToUartBuffer() const {
    return usbToUartBuffer_;
  }

  bool CommunicationManager::hasNewData() const {
    return uartToUsbBuffer_.hasNewData() || usbToUartBuffer_.hasNewData();
  }

  void CommunicationManager::clearNewDataFlag() {
    uartToUsbBuffer_.clearNewDataFlag();
    usbToUartBuffer_.clearNewDataFlag();
  }

  Driver::Uart& CommunicationManager::getActiveUart() {
    return *uarts_[static_cast<size_t>(currentChannel_)];
  }

} // namespace App
