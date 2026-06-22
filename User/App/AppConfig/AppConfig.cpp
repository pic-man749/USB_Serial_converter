/*
 * AppConfig.cpp
 *
 *      Author: picman
 */
#include "AppConfig.hpp"

namespace App {

  const char* GetUartChannelStr(UartChannel ch) {
    switch(ch) {
      case UartChannel::UART1:
        return reinterpret_cast<const char*>("UART1");
      case UartChannel::UART2:
        return reinterpret_cast<const char*>("UART2");
      case UartChannel::UART3:
        return reinterpret_cast<const char*>("UART3");
    }
    return reinterpret_cast<const char*>("UNKNOWN");
  }

  const char* GetDisplayModeStr(DisplayMode mode) {
    switch(mode) {
      case DisplayMode::Ascii:
        return reinterpret_cast<const char*>("ASCII");
      case DisplayMode::Hex:
        return reinterpret_cast<const char*>("HEX");
    }
    return reinterpret_cast<const char*>("UNKNOWN");
  }

}
