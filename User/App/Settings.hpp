/*
 * Settings.hpp
 */

#ifndef USER_APP_SETTINGS_HPP_
#define USER_APP_SETTINGS_HPP_

#include <cstdint>

namespace App {

// HAL ヘッダが USART1/USART2/USART6 をマクロ定義するため、衝突を避けて UART1/2/6 を使用する
enum class UartChannel : uint8_t {
    UART1 = 0,
    UART2 = 1,
    UART6 = 2,
};

enum class DisplayFormat : uint8_t {
    ASCII = 0,
    HEX   = 1,
};

constexpr uint32_t BAUD_RATES[]     = { 9600, 19200, 38400, 57600, 115200, 230400 };
constexpr uint8_t  BAUD_RATE_COUNT  = 6;
constexpr uint8_t  DEFAULT_BAUD_IDX = 4;  // 115200 bps

struct Settings {
    UartChannel   channel     = UartChannel::UART1;
    uint8_t       baudRateIdx = DEFAULT_BAUD_IDX;
    DisplayFormat format      = DisplayFormat::ASCII;

    uint32_t baudRate() const { return BAUD_RATES[baudRateIdx]; }
};

} // namespace App

#endif /* USER_APP_SETTINGS_HPP_ */
