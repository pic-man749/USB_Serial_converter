/*
 * Callback_wrapper.cpp
 *
 *      Author: picman
 */
#include "Callback_wrapper.hpp"
#include "Uart.hpp"

void Uart_TxCpltCallback(UART_HandleTypeDef *huart) {
  Driver::Uart::TxCpltCallback(huart);
}
