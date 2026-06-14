/*
 * EventBus.hpp
 *
 * IEventSource のリストと EventQueue を保持する。
 * Update() で全ソースを更新し、StateMachine には getQueue() でキューを公開する。
 */
#ifndef APP_EVENT_EVENTBUS_HPP_
#define APP_EVENT_EVENTBUS_HPP_

#include <array>
#include <initializer_list>
#include <cstddef>
#include "../IEventSource.hpp"
#include "EventQueue.hpp"

namespace App {

  class EventBus {
    public:
      static constexpr size_t kMaxSources = 4U;

      explicit EventBus(std::initializer_list<IEventSource*> sources);

      /** 登録された全ソースの Update() を呼び出し、イベントをキューに積む。 */
      void Update();

      /** StateMachine に渡すキューへの参照を返す。 */
      EventQueue& getQueue();

    private:
      std::array<IEventSource*, kMaxSources> sources_;
      size_t sourceCount_;
      EventQueue queue_;
  };

} // namespace App

#endif /* APP_EVENT_EVENTBUS_HPP_ */
