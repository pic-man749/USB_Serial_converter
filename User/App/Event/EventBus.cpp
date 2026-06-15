/*
 * EventBus.cpp
 */
#include "EventBus.hpp"

namespace App {

  EventBus::EventBus(std::initializer_list<IEventSource*> sources) :
      sources_ { }, sourceCount_(0U), queue_() {
    for(IEventSource *src : sources) {
      if(sourceCount_ < kMaxSources) {
        sources_[sourceCount_++] = src;
      }
    }
  }

  void EventBus::Update() {
    for(size_t i = 0U; i < sourceCount_; ++i) {
      sources_[i]->Update(queue_);
    }
  }

  EventQueue& EventBus::getQueue() {
    return queue_;
  }

} // namespace App
