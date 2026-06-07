/*
 * Queue.cpp
 *
 *      Author: picman
 */

#include "Queue.hpp"

namespace Common {

  Queue::Queue(uint32_t maxSize)
      : buffer_(std::make_unique<uint8_t[]>(maxSize + 1)),
        maxSize_(maxSize + 1),
        head_(0),
        tail_(0) {
  }

  Queue::~Queue() {
  }

  void Queue::push(const uint8_t *item, uint32_t size) {
    // 空き容量が不足している場合は追加しない
    if (size > maxSize_ - 1 - Queue::receivedSize()) {
      return;
    }

    for (uint32_t i = 0; i < size; i++) {
      buffer_[head_] = item[i];
      head_ = (head_ + 1) % maxSize_;
    }
  }

  bool Queue::pop(uint8_t *item, uint32_t size) {
    if (size > Queue::receivedSize()) {
      return false;
    }

    for (uint32_t i = 0; i < size; i++) {
      item[i] = buffer_[tail_];
      tail_ = (tail_ + 1) % maxSize_;
    }

    return true;
  }

  bool Queue::peek(uint8_t *item, uint32_t size) const {
    if (size > Queue::receivedSize()) {
      return false;
    }

    uint32_t readPos = tail_;
    for (uint32_t i = 0; i < size; i++) {
      item[i] = buffer_[readPos];
      readPos = (readPos + 1) % maxSize_;
    }

    return true;
  }

  uint32_t Queue::receivedSize() const {
    // head_ が tail_ を追い越して折り返した場合も正しく算出する
    return (head_ - tail_ + maxSize_) % maxSize_;
  }

} /* namespace Common */
