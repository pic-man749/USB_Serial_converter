/*
 * EventQueue.cpp
 */
#include "EventQueue.hpp"

namespace App {

  EventQueue::EventQueue() :
      buffer_(kCapacity) {
  }

  void EventQueue::push(const Event &event) {
    // 満杯時は最古イベントを破棄して追加する
    buffer_.push(event, true);
  }

  std::optional<Event> EventQueue::pop() {
    return buffer_.pop();
  }

  bool EventQueue::empty() const {
    return buffer_.empty();
  }

  void EventQueue::clear() {
    buffer_.clear();
  }

} // namespace App
