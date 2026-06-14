/*
 * CommunicationManager.cpp
 *
 *      Author: picman
 */

#include "CommunicationManager.hpp"
#include "../Event/Event.hpp"

namespace App {

  CommunicationManager::CommunicationManager(Driver::Uart &uart1,
                                             Driver::Uart &uart2,
                                             Driver::Uart &uart3,
                                             Driver::UsbCdc &usbCdc,
                                             UartChannel initialChannel, uint32_t initialBaudRate) :
      passthrough_(uart1, uart2, uart3, usbCdc, initialChannel, initialBaudRate),
      uartToUsbBuffer_(),
      usbToUartBuffer_() {
  }

  void CommunicationManager::start() {
    passthrough_.start();
  }

  void CommunicationManager::stop() {
    passthrough_.stop();
  }

  void CommunicationManager::Update(EventQueue &queue) {
    const bool newData = passthrough_.transfer(uartToUsbBuffer_, usbToUartBuffer_);
    if(newData) {
      queue.push(CommunicationDataEvent { });
    }
  }

  void CommunicationManager::setChannel(UartChannel channel) {
    passthrough_.setChannel(channel);
  }

  void CommunicationManager::setBaudRate(uint32_t baudRate) {
    passthrough_.setBaudRate(baudRate);
  }

  const DisplayBuffer& CommunicationManager::getUartToUsbBuffer() const {
    return uartToUsbBuffer_;
  }

  const DisplayBuffer& CommunicationManager::getUsbToUartBuffer() const {
    return usbToUartBuffer_;
  }

} // namespace App
