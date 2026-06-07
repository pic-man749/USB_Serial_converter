/*
 * DisplayBuffer.cpp
 */

#include "App/Display/DisplayBuffer.hpp"
#include <algorithm>

namespace App {

// HEX 変換テーブルの定義
constexpr char DisplayBuffer::HEX_TABLE[];

DisplayBuffer::DisplayBuffer(uint32_t capacity)
    : buf_(std::make_unique<uint8_t[]>(capacity)),
      capacity_(capacity),
      head_(0),
      used_(0)
{
}

void DisplayBuffer::push(const uint8_t* data, uint32_t len) {
    for (uint32_t i = 0; i < len; ++i) {
        buf_[head_] = data[i];
        head_ = (head_ + 1) % capacity_;
        if (used_ < capacity_) {
            ++used_;
        }
        // used_ == capacity_ の場合、head_ が最古データを上書きする
    }
}

void DisplayBuffer::clear() {
    head_ = 0;
    used_ = 0;
}

uint32_t DisplayBuffer::lineCount(DisplayFormat fmt) const {
    if (used_ == 0) return 0;
    const uint8_t bpl = bytesPerLine(fmt);
    return (used_ + bpl - 1) / bpl;
}

uint8_t DisplayBuffer::getLine(uint32_t lineIdx, DisplayFormat fmt,
                                char* buf, uint8_t bufLen) const {
    if (bufLen == 0) return 0;

    const uint8_t  bpl       = bytesPerLine(fmt);
    const uint32_t byteStart = lineIdx * bpl;

    if (byteStart >= used_) {
        buf[0] = '\0';
        return 0;
    }

    const uint32_t byteEnd = std::min(byteStart + static_cast<uint32_t>(bpl), used_);
    const uint32_t oldest  = oldestIdx();
    uint8_t written = 0;

    for (uint32_t i = byteStart; i < byteEnd; ++i) {
        const uint8_t b = buf_[(oldest + i) % capacity_];

        if (fmt == DisplayFormat::ASCII) {
            // 印字可能文字はそのまま、非印字文字は中点（U+00B7）の Latin-1 相当に置換
            if (written < bufLen - 1) {
                buf[written++] = (b >= 0x20u && b <= 0x7Eu)
                                 ? static_cast<char>(b)
                                 : static_cast<char>(0xB7u);  // ·
            }
        } else {
            // HEX 形式: "XX " を 7 バイト分書き込む（合計 21 文字）
            if (written + 2 < bufLen) {
                buf[written++] = HEX_TABLE[(b >> 4) & 0x0Fu];
                buf[written++] = HEX_TABLE[ b       & 0x0Fu];
                if (written < bufLen - 1) {
                    buf[written++] = ' ';
                }
            }
        }
    }

    buf[written] = '\0';
    return written;
}

uint32_t DisplayBuffer::oldestIdx() const {
    // used_ <= capacity_ が保証されているため underflow しない
    return (head_ + capacity_ - used_) % capacity_;
}

uint8_t DisplayBuffer::bytesPerLine(DisplayFormat fmt) {
    return (fmt == DisplayFormat::ASCII)
           ? BYTES_PER_LINE_ASCII
           : BYTES_PER_LINE_HEX;
}

} // namespace App
