/*
 * Callback_wrapper.hpp
 *
 *      Author: picman
 */

#ifndef DRIVER_USB_CDC_CALLBACK_WRAPPER_HPP_
#define DRIVER_USB_CDC_CALLBACK_WRAPPER_HPP_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void UsbCdc_RxCallback(const uint8_t *data, uint32_t size);
void UsbCdc_TxCpltCallback(void);


#ifdef __cplusplus
}
#endif

#endif /* DRIVER_USB_CDC_CALLBACK_WRAPPER_HPP_ */
