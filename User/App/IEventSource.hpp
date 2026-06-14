/*
 * IEventSource.hpp
 *
 * イベントを生成するソースが実装すべきインターフェース。
 */
#ifndef APP_IEVENTSOURCE_HPP_
#define APP_IEVENTSOURCE_HPP_

#include "Event/EventQueue.hpp"

namespace App {

  class IEventSource {
    public:
      virtual ~IEventSource() = default;

      /** 入力状態を読み取り、発生したイベントを queue に push する。 */
      virtual void Update(EventQueue &queue) = 0;
  };

} // namespace App

#endif /* APP_IEVENTSOURCE_HPP_ */
