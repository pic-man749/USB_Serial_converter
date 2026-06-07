/*
 * Callback_wrapper.cpp
 *
 *      Author: picman
 */
#include "Callback_wrapper.hpp"
#include "UsbCdc.hpp"

void UsbCdc_RxCallback(const uint8_t *data, uint32_t size){
  Driver::UsbCdc::ReceiveCallback(data, size);
}

void UsbCdc_TxCpltCallback(void){
  Driver::UsbCdc::SendCpltCallback();
}
