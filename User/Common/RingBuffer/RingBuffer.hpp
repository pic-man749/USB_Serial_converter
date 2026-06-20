/*
 * RingBuffer.hpp
 *
 *      Author: picman
 */

#ifndef COMMON_RINGBUFFER_RINGBUFFER_HPP_
#define COMMON_RINGBUFFER_RINGBUFFER_HPP_

#include <cstdint>
#include <memory>
#include <optional>

namespace Common {

  /**
   * @brief 循環バッファによる汎用キュークラス
   * @tparam T 要素型（コピー代入可能であること）
   */
  template<typename T>
  class RingBuffer {
    public:
      /**
       * @brief コンストラクタ
       * @param maxSize キューの最大保持要素数
       */
      explicit RingBuffer(uint32_t maxSize)
      :
          buffer_(std::make_unique<T[]>(maxSize + 1)),
          maxSize_(maxSize + 1),
          head_(0),
          tail_(0) {
      }

      /**
       * @brief デストラクタ
       */
      ~RingBuffer() = default;

      /**
       * @brief データをキューへ追加する
       * @details 空き容量が不足している場合は追加しない
       * @param item 追加するデータへのポインタ
       * @param size 追加する要素数
       */
      void push(const T *item, uint32_t size) {
        // 空き容量が不足している場合は追加しない
        if(size > maxSize_ - 1 - receivedSize()) {
          return;
        }

        for(uint32_t i = 0; i < size; i++) {
          buffer_[head_] = item[i];
          head_ = (head_ + 1) % maxSize_;
        }
      }

      /**
       * @brief データをキューへ1要素追加する
       * @details overwrite が false の場合、空き容量が不足していると追加しない。
       *          overwrite が true の場合、満杯時は最古の要素を破棄してから追加する。
       * @param item 追加する要素
       * @param overwrite 満杯時に最古の要素を上書きするかどうか
       */
      void push(const T &item, bool overwrite = false) {
        if(overwrite && full()) {
          pop();
        }
        push(&item, 1);
      }

      /**
       * @brief キューの先頭からデータを取り出す
       * @param item 取り出したデータの格納先バッファ
       * @param size 取り出す要素数
       * @return 成功時 true、データ不足時 false
       */
      bool pop(T *item, uint32_t size) {
        if(size > receivedSize()) {
          return false;
        }

        for(uint32_t i = 0; i < size; i++) {
          item[i] = buffer_[tail_];
          tail_ = (tail_ + 1) % maxSize_;
        }

        return true;
      }

      /**
       * @brief キューの先頭から1要素取り出す
       * @return 成功時は取り出した要素、データ不足時は std::nullopt
       */
      std::optional<T> pop() {
        if(receivedSize() == 0) {
          return std::nullopt;
        }

        std::optional<T> item(buffer_[tail_]);
        tail_ = (tail_ + 1) % maxSize_;
        return item;
      }

      /**
       * @brief キューの先頭データをコピーして読み出す（取り出しは行わない）
       * @param item コピー先バッファ
       * @param size 読み出す要素数
       * @return 成功時 true、データ不足時 false
       */
      bool peek(T *item, uint32_t size) const {
        if(size > receivedSize()) {
          return false;
        }

        uint32_t readPos = tail_;
        for(uint32_t i = 0; i < size; i++) {
          item[i] = buffer_[readPos];
          readPos = (readPos + 1) % maxSize_;
        }

        return true;
      }

      /**
       * @brief キューに格納されている要素数を返す
       * @return 格納要素数
       */
      uint32_t receivedSize() const {
        // head_ が tail_ を追い越して折り返した場合も正しく算出する
        return (head_ - tail_ + maxSize_) % maxSize_;
      }

      /**
       * @brief キューが空かどうかを返す
       * @return 空の場合 true
       */
      bool empty() const {
        return head_ == tail_;
      }

      /**
       * @brief キューが満杯かどうかを返す
       * @return 満杯の場合 true
       */
      bool full() const {
        return receivedSize() == maxSize_ - 1;
      }

      /**
       * @brief キュー内のデータをすべて破棄する
       */
      void clear() {
        head_ = 0;
        tail_ = 0;
      }

    private:
      std::unique_ptr<T[]> buffer_;
      uint32_t maxSize_;
      uint32_t head_;
      uint32_t tail_;
  };

} /* namespace Common */

#endif /* COMMON_RINGBUFFER_RINGBUFFER_HPP_ */
