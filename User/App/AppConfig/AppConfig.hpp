/*
 * AppConfig.hpp
 *
 * アプリケーション全体で共有する設定値。
 * 参照渡しで各クラスに共有する。
 */

#ifndef APP_APPCONFIG_APPCONFIG_HPP_
#define APP_APPCONFIG_APPCONFIG_HPP_

#include <cstdint>

namespace App {

  /**
   * システム上の UART チャンネル。
   * UART1/2/3 はそれぞれペリフェラルの USART1/2/6 に対応する。
   * この対応関係は CommunicationManager のコンストラクタ引数の順序で定義する。
   */
  enum class UartChannel : uint8_t {
    UART1 = 0,
    UART2,
    UART3
  };

  const char* GetUartChannelStr(UartChannel ch);

  /** OLED の表示形式 */
  enum class DisplayMode : uint8_t {
    Hex,
    Ascii
  };

  const char* GetDisplayModeStr(DisplayMode mode);

  struct AppConfig {
      UartChannel selectedUart = UartChannel::UART1;
      uint32_t baudRate = 115200U;
      DisplayMode displayMode = DisplayMode::Hex;
  };

} // namespace App

#endif /* APP_APPCONFIG_APPCONFIG_HPP_ */
