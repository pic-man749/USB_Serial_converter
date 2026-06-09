/*
 * Callback_wrapper.hpp
 *
 *      Author: picman
 */

#ifndef DRIVER_UART_CALLBACK_WRAPPER_HPP_
#define DRIVER_UART_CALLBACK_WRAPPER_HPP_

#include "usart.h"

#ifdef __cplusplus
extern "C" {
#endif

void Uart_TxCpltCallback(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

#endif /* DRIVER_UART_CALLBACK_WRAPPER_HPP_ */
