/*
 * Queue.hpp
 *
 *      Author: picman
 */

#ifndef COMMON_QUEUE_QUEUE_HPP_
#define COMMON_QUEUE_QUEUE_HPP_

#include <cstdint>
#include <memory>

namespace Common {

  /**
   * @brief 循環バッファによるバイトキュークラス
   */
  class Queue {
    public:
      /**
       * @brief コンストラクタ
       * @param maxSize キューの最大保持バイト数
       */
      Queue(uint32_t maxSize);

      /**
       * @brief デストラクタ
       */
      ~Queue();

      /**
       * @brief データをキューへ追加する
       * @details 空き容量が不足している場合は追加しない
       * @param item 追加するデータへのポインタ
       * @param size 追加するバイト数
       */
      void push(const uint8_t *item, uint32_t size);

      /**
       * @brief キューの先頭からデータを取り出す
       * @param item 取り出したデータの格納先バッファ
       * @param size 取り出すバイト数
       * @return 成功時 true、データ不足時 false
       */
      bool pop(uint8_t *item, uint32_t size);

      /**
       * @brief キューの先頭データをコピーして読み出す（取り出しは行わない）
       * @param item コピー先バッファ
       * @param size 読み出すバイト数
       * @return 成功時 true、データ不足時 false
       */
      bool peek(uint8_t *item, uint32_t size) const;

      /**
       * @brief キューに格納されているバイト数を返す
       * @return 格納バイト数
       */
      uint32_t receivedSize() const;

    private:
      std::unique_ptr<uint8_t[]> buffer_;
      uint32_t maxSize_;
      uint32_t head_;
      uint32_t tail_;
  };

} /* namespace Common */

#endif /* COMMON_QUEUE_QUEUE_HPP_ */
