/*
 * EventQueue.hpp
 *
 * イベントを蓄積する固定サイズリングバッファ。
 * 容量超過時は最古のイベントを上書きする。
 */
#ifndef APP_EVENT_EVENTQUEUE_HPP_
#define APP_EVENT_EVENTQUEUE_HPP_

#include <optional>
#include "Event.hpp"
#include "Common/RingBuffer/RingBuffer.hpp"

namespace App {

  class EventQueue {
    public:
      static constexpr uint32_t kCapacity = 16U;

      EventQueue();

      /** イベントをキューの末尾に追加する。満杯の場合は最古のイベントを上書きする。 */
      void push(const Event &event);

      /** キューの先頭からイベントを取り出す。空の場合は nullopt を返す。 */
      std::optional<Event> pop();

      bool empty() const;

      /** キュー内の全イベントを破棄する。 */
      void clear();

    private:
      Common::RingBuffer<Event> buffer_;
  };

} // namespace App

#endif /* APP_EVENT_EVENTQUEUE_HPP_ */
