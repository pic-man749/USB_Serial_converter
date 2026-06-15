/*
 * DisplayBuffer.cpp
 *
 *      Author: picman
 */

#include "DisplayBuffer.hpp"

namespace App {

  DisplayBuffer::DisplayBuffer() :
      buffer_ { }, head_(0U), count_(0U), newDataFlag_(false) {
  }

  void DisplayBuffer::push(const uint8_t *data, size_t size) {
    for(size_t i = 0; i < size; ++i) {
      buffer_[head_] = data[i];
      head_ = (head_ + 1U) % kCapacity;
      if(count_ < kCapacity) {
        ++count_;
      }
    }
    if(size > 0U) {
      newDataFlag_ = true;
    }
  }

  size_t DisplayBuffer::size() const {
    return count_;
  }

  uint8_t DisplayBuffer::at(size_t index) const {
    // 最古データの位置を求め、そこから index 分進めた位置を返す
    const size_t oldest = (head_ - count_ + kCapacity) % kCapacity;
    return buffer_[(oldest + index) % kCapacity];
  }

  bool DisplayBuffer::hasNewData() const {
    return newDataFlag_;
  }

  void DisplayBuffer::clearNewDataFlag() {
    newDataFlag_ = false;
  }

  void DisplayBuffer::clear() {
    head_ = 0U;
    count_ = 0U;
    newDataFlag_ = false;
  }

} // namespace App
