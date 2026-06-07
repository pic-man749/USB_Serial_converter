/*
 * UsbCdc.hpp
 *
 *      Author: picman
 */

#ifndef DRIVER_USB_CDC_USBCDC_HPP_
#define DRIVER_USB_CDC_USBCDC_HPP_

#include "Common/Queue/Queue.hpp"
#include <cstdint>
#include <memory>

namespace Driver {

  /**
   * @brief USB CDCドライバクラス（シングルトン）
   */
  class UsbCdc {
    public:
      /**
       * @brief 唯一のインスタンスを返す
       * @return UsbCdc インスタンスの参照
       */
      static UsbCdc& getInstance();

      UsbCdc(const UsbCdc&) = delete;
      UsbCdc& operator=(const UsbCdc&) = delete;
      UsbCdc(UsbCdc&&) = delete;
      UsbCdc& operator=(UsbCdc&&) = delete;

      /**
       * @brief USB CDC受信コールバック
       * @details usbd_cdc_if.c の CDC_Receive_FS から呼び出す
       * @param data 受信データへのポインタ
       * @param size 受信バイト数
       */
      static void ReceiveCallback(const uint8_t *data, uint32_t size);

      /**
       * @brief USB CDC送信完了コールバック
       * @details usbd_cdc_if.c の CDC_TransmitCplt_FS から呼び出す
       */
      static void SendCpltCallback();

      /**
       * @brief データを送信する（ブロッキング）
       * @param data 送信データへのポインタ
       * @param size 送信バイト数
       */
      void send(uint8_t *data, uint16_t size);

      /**
       * @brief 受信キューに格納されているバイト数を返す
       * @return 読み出し可能なバイト数
       */
      uint32_t getReceiveDataSize() const;

      /**
       * @brief 受信キューからデータを取り出す
       * @param data 取り出したデータの格納先バッファ
       * @param size 取り出すバイト数
       * @return 成功時 true、データ不足時 false
       */
      bool getReceiveData(uint8_t *data, uint32_t size);

    private:
      UsbCdc() = default;
      ~UsbCdc() = default;

      static constexpr uint32_t RECEIVE_QUEUE_SIZE = 1024;

      static Common::Queue receiveQueue_;
      static bool isSending_;
      static std::unique_ptr<uint8_t[]> sendBuffer_;
      static uint16_t sendBufferSize_;
  };

} /* namespace Driver */

#endif /* DRIVER_USB_CDC_USBCDC_HPP_ */
