/*
 * Callback_wrapper.cpp
 *
 *      Author: picman
 */
#include "Callback_wrapper.hpp"
#include "Uart.hpp"

extern "C" {

  void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    Driver::Uart::TxCpltCallback(huart);
  }

  void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    Driver::Uart::ErrorCallback(huart);
  }

}
