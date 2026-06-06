/*
 * Uart.cpp
 *
 *      Author: picman
 */

#include "Uart.hpp"

namespace Driver {

  Uart::Uart(UART_HandleTypeDef *huart, const uint32_t queueSize) :
      huart_(huart), receiveBuffer_(queueSize, 0), readPos_(0) {
  }

  Uart::~Uart() {
    HAL_UART_DMAStop(huart_);
  }

  bool Uart::setbaudrate(uint32_t baudrate) {
    HAL_UART_DeInit(huart_);
    huart_->Init.BaudRate = baudrate;
    return HAL_UART_Init(huart_) == HAL_OK;
  }

  void Uart::send(const char *data, uint32_t size) {
    HAL_UART_Transmit(huart_, reinterpret_cast<const uint8_t*>(data), static_cast<uint16_t>(size),
        HAL_MAX_DELAY);
  }

  void Uart::startReceive() {
    readPos_ = 0;
    HAL_UART_Receive_DMA(huart_, receiveBuffer_.data(),
        static_cast<uint16_t>(receiveBuffer_.size()));
  }

  void Uart::stopReceive() {
    HAL_UART_DMAStop(huart_);
    readPos_ = 0;
  }

  uint32_t Uart::getReceiveDataSize() {
    // DMAカウンタは残り転送数を示すため、バッファサイズから引いて書き込み位置を算出する
    const uint32_t bufSize = static_cast<uint32_t>(receiveBuffer_.size());
    const uint32_t writePos = bufSize - __HAL_DMA_GET_COUNTER(huart_->hdmarx);

    if(writePos >= readPos_) {
      return writePos - readPos_;
    }

    // 書き込み位置が読み出し位置を追い越して折り返した場合
    return bufSize - readPos_ + writePos;
  }

  uint32_t Uart::getReceiveData(uint8_t *data, uint32_t size) {
    const uint32_t available = getReceiveDataSize();
    const uint32_t readSize = (size < available) ? size : available;
    const uint32_t bufSize = static_cast<uint32_t>(receiveBuffer_.size());

    for(uint32_t i = 0; i < readSize; i++) {
      data[i] = receiveBuffer_[readPos_];
      readPos_ = (readPos_ + 1) % bufSize;
    }

    return readSize;
  }

} // namespace Driver
