/*
 * DisplayBuffer.hpp
 *
 * 1方向分の通信データをバイト列として保持する固定サイズ循環バッファ。
 * 方向の区別はインスタンス自体が担うため、エントリに方向フラグを持たない。
 * 表示用途専用であり、パススルーのデータコピーは行わない。
 */

#ifndef APP_COMMUNICATIONMANAGER_DISPLAYBUFFER_HPP_
#define APP_COMMUNICATIONMANAGER_DISPLAYBUFFER_HPP_

#include <array>
#include <cstddef>
#include <cstdint>

namespace App {

  /**
   * 1方向分の通信データを保持する循環バッファ。
   * 容量超過時は最古データを上書きする。
   */
  class DisplayBuffer {
    public:
      /** バッファ容量（バイト） */
      static constexpr size_t kCapacity = 512U;

      DisplayBuffer();

      /**
       * @brief データをバッファに追加する
       * @param data データへのポインタ
       * @param size データサイズ（バイト）
       */
      void push(const uint8_t *data, size_t size);

      /** @brief 有効バイト数を返す */
      size_t size() const;

      /**
       * @brief 絶対インデックスでバイトを取得する
       * @param index インデックス（0 が最古）
       * @return バイト値
       */
      uint8_t at(size_t index) const;

      /** @brief 新着データフラグを参照する */
      bool hasNewData() const;

      /** @brief 新着データフラグをクリアする */
      void clearNewDataFlag();

      /** @brief 全データを削除する */
      void clear();

    private:
      std::array<uint8_t, kCapacity> buffer_;
      size_t head_;
      size_t count_;
      bool newDataFlag_;
  };

} // namespace App

#endif /* APP_COMMUNICATIONMANAGER_DISPLAYBUFFER_HPP_ */
