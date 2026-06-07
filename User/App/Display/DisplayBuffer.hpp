/*
 * DisplayBuffer.hpp
 */

#ifndef USER_APP_DISPLAY_DISPLAYBUFFER_HPP_
#define USER_APP_DISPLAY_DISPLAYBUFFER_HPP_

#include <cstdint>
#include <memory>
#include "App/Settings.hpp"

namespace App {

/**
 * @brief 生バイト列のリングバッファ（表示履歴管理用）
 *
 * 固定容量のリングバッファとしてバイト列を保持し、
 * ASCII / HEX 形式での行単位アクセスを提供する。
 * 容量を超えた場合は最古のデータが上書きされる。
 *
 * 行の定義:
 *   - ASCII: BYTES_PER_LINE_ASCII (21) バイトを 1 行とする
 *   - HEX:   BYTES_PER_LINE_HEX  ( 7) バイトを 1 行とする
 */
class DisplayBuffer {
public:
    static constexpr uint8_t BYTES_PER_LINE_ASCII = 21;
    static constexpr uint8_t BYTES_PER_LINE_HEX   = 7;

    /**
     * @param capacity バッファ容量（バイト）
     */
    explicit DisplayBuffer(uint32_t capacity);

    /**
     * @brief バイト列をバッファに追加する
     * @details 容量を超えた分は最古のデータを上書きする
     */
    void push(const uint8_t* data, uint32_t len);

    /**
     * @brief バッファをクリアする
     */
    void clear();

    /**
     * @brief 現在の表示形式での総行数を返す
     * @param fmt 表示形式
     */
    uint32_t lineCount(DisplayFormat fmt) const;

    /**
     * @brief 指定行の文字列を buf へ書き込む
     *
     * @param lineIdx 行インデックス（0 = 最古行）
     * @param fmt     表示形式
     * @param buf     出力先バッファ
     * @param bufLen  出力先バッファサイズ（終端 '\0' を含む）
     * @return        書き込んだ文字数（'\0' を除く）。行が存在しない場合は 0
     */
    uint8_t getLine(uint32_t lineIdx, DisplayFormat fmt,
                    char* buf, uint8_t bufLen) const;

    uint32_t usedBytes() const { return used_; }

private:
    std::unique_ptr<uint8_t[]> buf_;
    uint32_t capacity_;
    uint32_t head_;   // 次の書き込み位置（0-origin, ラップアラウンド）
    uint32_t used_;   // 有効データ量（capacity_ で頭打ち）

    // 最古バイトのバッファインデックスを返す
    uint32_t oldestIdx() const;

    // fmt に対応する 1 行あたりのバイト数を返す
    static uint8_t bytesPerLine(DisplayFormat fmt);

    // HEX 変換用テーブル
    static constexpr char HEX_TABLE[] = "0123456789ABCDEF";
};

} // namespace App

#endif /* USER_APP_DISPLAY_DISPLAYBUFFER_HPP_ */
