/*
 * UsbCdc.cpp
 *
 *      Author: picman
 */

#include "UsbCdc.hpp"
#include "usbd_cdc_if.h"

extern "C" {
  extern USBD_HandleTypeDef hUsbDeviceFS;
}

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

  bool UsbCdc::isConnected() {
    return hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED;
  }

  void UsbCdc::send(uint8_t *data, uint16_t size) {
    // 未接続時は送信しない
    if (!isConnected()) {
      return;
    }
    // 前回データを送信中の場合は、完了または切断まで待つ
    while (isSending_ && isConnected()) {
      ;
    }
    if (!isConnected()) {
      isSending_ = false;
      return;
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
    // 送信要求が失敗した場合はフラグをリセットする
    if (CDC_Transmit_FS(sendBuffer_.get(), size) != USBD_OK) {
      isSending_ = false;
    }
  }

  uint32_t UsbCdc::getReceiveDataSize() const {
    return receiveQueue_.receivedSize();
  }

  bool UsbCdc::getReceiveData(uint8_t *data, uint32_t size) {
    return receiveQueue_.pop(data, size);
  }

} /* namespace Driver */
