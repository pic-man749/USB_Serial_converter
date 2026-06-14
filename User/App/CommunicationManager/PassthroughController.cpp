/*
 * PassthroughController.cpp
 *
 *      Author: picman
 */

#include "PassthroughController.hpp"
#include <algorithm>

namespace App {

  PassthroughController::PassthroughController(
      Driver::Uart &uart1, Driver::Uart &uart2, Driver::Uart &uart3,
      Driver::UsbCdc &usbCdc,
      UartChannel initialChannel, uint32_t initialBaudRate) :
      uarts_ { &uart1, &uart2, &uart3 }, usbCdc_(usbCdc),
      currentChannel_(initialChannel), currentBaudRate_(initialBaudRate),
      running_(false) {
  }

  void PassthroughController::start() {
    if(running_) {
      return;
    }
    getActiveUart().startReceive();
    running_ = true;
  }

  void PassthroughController::stop() {
    if(!running_) {
      return;
    }
    getActiveUart().stopReceive();
    running_ = false;
  }

  void PassthroughController::setChannel(UartChannel channel) {
    if(channel == currentChannel_) {
      return;
    }
    if(running_) {
      getActiveUart().stopReceive();
    }
    currentChannel_ = channel;
    if(running_) {
      getActiveUart().startReceive();
    }
  }

  void PassthroughController::setBaudRate(uint32_t baudRate) {
    if(baudRate == currentBaudRate_) {
      return;
    }
    currentBaudRate_ = baudRate;
    if(running_) {
      // DeInit/Init で DMA が停止するため、前後に受信停止・再開を行い readPos_ をリセットする
      getActiveUart().stopReceive();
      getActiveUart().setbaudrate(currentBaudRate_);
      getActiveUart().startReceive();
    }
  }

  bool PassthroughController::transfer(DisplayBuffer &uartToUsb, DisplayBuffer &usbToUart) {
    if(!running_) {
      return false;
    }

    uint8_t buf[kTransferBufSize];
    bool newDataReceived = false;

    // USB → UART 方向の転送
    uint32_t usbRxSize = usbCdc_.getReceiveDataSize();
    while(usbRxSize > 0U) {
      const uint32_t chunk = std::min(usbRxSize, kTransferBufSize);
      if(!usbCdc_.getReceiveData(buf, chunk)) {
        break;
      }
      getActiveUart().send(reinterpret_cast<const char*>(buf), chunk);
      usbToUart.push(buf, chunk);
      newDataReceived = true;
      usbRxSize -= chunk;
    }

    // UART → USB 方向の転送
    uint32_t uartRxSize = getActiveUart().getReceiveDataSize();
    while(uartRxSize > 0U) {
      const uint32_t chunk = std::min(uartRxSize, kTransferBufSize);
      const uint32_t read = getActiveUart().getReceiveData(buf, chunk);
      if(read == 0U) {
        break;
      }
      usbCdc_.send(buf, static_cast<uint16_t>(read));
      uartToUsb.push(buf, read);
      newDataReceived = true;
      uartRxSize -= read;
    }

    return newDataReceived;
  }

  Driver::Uart& PassthroughController::getActiveUart() {
    return *uarts_[static_cast<size_t>(currentChannel_)];
  }

} // namespace App
