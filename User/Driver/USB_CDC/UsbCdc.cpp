/*
 * UsbCdc.cpp
 *
 *      Author: picman
 */

#include "UsbCdc.hpp"
#include "usbd_cdc_if.h"

namespace Driver {

  Common::Queue UsbCdc::receiveQueue_(RECEIVE_QUEUE_SIZE);
  bool UsbCdc::isSending_ = false;
  std::unique_ptr<uint8_t[]> UsbCdc::sendBuffer_ = nullptr;
  uint16_t UsbCdc::sendBufferSize_ = 0;

  UsbCdc& UsbCdc::getInstance() {
    static UsbCdc instance;
    return instance;
  }

  void UsbCdc::ReceiveCallback(const uint8_t *data, uint32_t size) {
    receiveQueue_.push(data, size);
  }

  void UsbCdc::SendCpltCallback() {
    isSending_ = false;
  }

  void UsbCdc::send(uint8_t *data, uint16_t size) {
    // 前回データを送信中なら完了まで待つ
    while (isSending_) {
      ;
    }
    isSending_ = true;
    // すでに確保済みの送信バッファサイズが要求サイズよりも小さかった場合のみ再確保する
    if (sendBuffer_ == nullptr || sendBufferSize_ < size) {
      sendBuffer_ = std::make_unique<uint8_t[]>(size);
      sendBufferSize_ = size;
    }
    for (uint16_t idx = 0; idx < size; ++idx) {
      sendBuffer_[idx] = data[idx];
    }
    CDC_Transmit_FS(sendBuffer_.get(), size);
  }

  uint32_t UsbCdc::getReceiveDataSize() const {
    return receiveQueue_.receivedSize();
  }

  bool UsbCdc::getReceiveData(uint8_t *data, uint32_t size) {
    return receiveQueue_.pop(data, size);
  }

} /* namespace Driver */
