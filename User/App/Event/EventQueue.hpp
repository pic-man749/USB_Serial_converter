/*
 * EventQueue.hpp
 *
 * イベントを蓄積する固定サイズリングバッファ。
 * 容量超過時は最古のイベントを上書きする。
 */
#ifndef APP_EVENT_EVENTQUEUE_HPP_
#define APP_EVENT_EVENTQUEUE_HPP_

#include <array>
#include <optional>
#include "Event.hpp"

namespace App {

  class EventQueue {
    public:
      static constexpr size_t kCapacity = 16U;

      EventQueue();

      /** イベントをキューの末尾に追加する。満杯の場合は最古のイベントを上書きする。 */
      void push(const Event &event);

      /** キューの先頭からイベントを取り出す。空の場合は nullopt を返す。 */
      std::optional<Event> pop();

      bool empty() const;

      /** キュー内の全イベントを破棄する。 */
      void clear();

    private:
      std::array<Event, kCapacity> buffer_;
      size_t head_;
      size_t tail_;
      size_t count_;
  };

} // namespace App

#endif /* APP_EVENT_EVENTQUEUE_HPP_ */
