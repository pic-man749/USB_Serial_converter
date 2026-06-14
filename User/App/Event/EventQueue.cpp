/*
 * EventQueue.cpp
 */
#include "EventQueue.hpp"

namespace App {

  EventQueue::EventQueue() :
      buffer_ { }, head_(0U), tail_(0U), count_(0U) {
  }

  void EventQueue::push(const Event &event) {
    if(count_ == kCapacity) {
      // 最古イベントを破棄して空きを確保する
      head_ = (head_ + 1U) % kCapacity;
      --count_;
    }
    buffer_[tail_] = event;
    tail_ = (tail_ + 1U) % kCapacity;
    ++count_;
  }

  std::optional<Event> EventQueue::pop() {
    if(count_ == 0U) {
      return std::nullopt;
    }
    Event event = buffer_[head_];
    head_ = (head_ + 1U) % kCapacity;
    --count_;
    return event;
  }

  bool EventQueue::empty() const {
    return count_ == 0U;
  }

  void EventQueue::clear() {
    head_ = 0U;
    tail_ = 0U;
    count_ = 0U;
  }

} // namespace App
