/*
 * Uart.cpp
 *
 *      Author: picman
 */

#include "Uart.hpp"

namespace Driver {

  Uart *Uart::instanceRegistry_[INSTANCE_REGISTRY_SIZE] = {};
  uint8_t Uart::instanceCount_ = 0;

  Uart::Uart(UART_HandleTypeDef *huart, const uint32_t queueSize) :
      huart_(huart), receiveBuffer_(queueSize, 0), readPos_(0),
      isSending_(false), sendBuffer_(nullptr), sendBufferSize_(0) {
    if (instanceCount_ < INSTANCE_REGISTRY_SIZE) {
      instanceRegistry_[instanceCount_++] = this;
    }
  }

  Uart::~Uart() {
    HAL_UART_DMAStop(huart_);
  }

  bool Uart::setbaudrate(uint32_t baudrate) {
    // 再初期化前に進行中の送信を中断する
    HAL_UART_AbortTransmit(huart_);
    isSending_ = false;
    HAL_UART_DeInit(huart_);
    huart_->Init.BaudRate = baudrate;
    return HAL_UART_Init(huart_) == HAL_OK;
  }

  void Uart::TxCpltCallback(UART_HandleTypeDef *huart) {
    for (uint8_t i = 0; i < instanceCount_; ++i) {
      if (instanceRegistry_[i]->huart_ == huart) {
        instanceRegistry_[i]->isSending_ = false;
        break;
      }
    }
  }

  void Uart::send(const char *data, uint32_t size) {
    // 前回送信が完了するまで待つ
    while (isSending_) {
      ;
    }
    isSending_ = true;
    // すでに確保済みの送信バッファサイズが要求サイズよりも小さかった場合のみ再確保する
    if (sendBuffer_ == nullptr || sendBufferSize_ < size) {
      sendBuffer_ = std::make_unique<uint8_t[]>(size);
      sendBufferSize_ = static_cast<uint16_t>(size);
    }
    for (uint32_t idx = 0; idx < size; ++idx) {
      sendBuffer_[idx] = static_cast<uint8_t>(data[idx]);
    }
    // 送信要求が失敗した場合はフラグをリセットする
    if (HAL_UART_Transmit_IT(huart_, sendBuffer_.get(), static_cast<uint16_t>(size)) != HAL_OK) {
      isSending_ = false;
    }
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

  void Uart::clearReceiveBuffer() {
    // 読み出し位置を現在の書き込み位置に揃えて破棄する
    const uint32_t bufSize = static_cast<uint32_t>(receiveBuffer_.size());
    readPos_ = bufSize - __HAL_DMA_GET_COUNTER(huart_->hdmarx);
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
