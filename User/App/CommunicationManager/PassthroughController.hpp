/*
 * PassthroughController.hpp
 *
 * USB CDC ↔ UART 間のデータ転送と、チャンネル・ボーレートの管理のみを担う。
 * DisplayBuffer への記録およびイベント発行は行わない。
 *
 * ## ライフサイクル
 * - `start()` を呼び出すと DMA 受信が開始され、`transfer()` でデータを転送できる。
 * - `start()` / `stop()` はべき等であり、重複呼び出しは無視される。
 * - 動作中に `setChannel()` / `setBaudRate()` を呼び出した場合、内部で受信を
 *   一時停止してから設定を変更し、再開する。呼び出し元は事前に `stop()` を
 *   呼び出す必要はない。
 */

#ifndef APP_COMMUNICATIONMANAGER_PASSTHROUGHCONTROLLER_HPP_
#define APP_COMMUNICATIONMANAGER_PASSTHROUGHCONTROLLER_HPP_

#include <array>
#include <cstdint>
#include "../AppConfig/AppConfig.hpp"
#include "DisplayBuffer.hpp"
#include "../../Driver/Uart/Uart.hpp"
#include "../../Driver/USB_CDC/UsbCdc.hpp"

namespace App {

  class PassthroughController {
    public:
      /**
       * @param uart1         USART1 に対応する Uart インスタンス
       * @param uart2         USART2 に対応する Uart インスタンス
       * @param uart3         USART6 に対応する Uart インスタンス
       * @param usbCdc        UsbCdc シングルトンインスタンス
       * @param initialChannel  起動時のアクティブ UART チャンネル
       * @param initialBaudRate 起動時のボーレート
       */
      PassthroughController(Driver::Uart &uart1,
                            Driver::Uart &uart2, Driver::Uart &uart3,
                            Driver::UsbCdc &usbCdc,
                            UartChannel initialChannel, uint32_t initialBaudRate);

      /** @brief パススルーを開始する。アクティブ UART の DMA 受信を開始する。 */
      void start();

      /** @brief パススルーを停止する。アクティブ UART の DMA 受信を停止する。 */
      void stop();

      /**
       * @brief UARTチャンネルを切り替える。
       *        動作中の場合は旧 UART の受信を停止してから新 UART の受信を開始する。
       */
      void setChannel(UartChannel channel);

      /**
       * @brief ボーレートを変更する。
       *        動作中の場合は受信を停止し、ボーレートを設定した後に受信を再開する。
       */
      void setBaudRate(uint32_t baudRate);

      /**
       * @brief USB↔UART のデータ転送を1回実行する。
       *        転送したデータを uartToUsb / usbToUart に書き込む。
       * @param uartToUsb  UART→USB 方向の表示用バッファ
       * @param usbToUart  USB→UART 方向の表示用バッファ
       * @return 今回の呼び出しで新着データがあれば true
       */
      bool transfer(DisplayBuffer &uartToUsb, DisplayBuffer &usbToUart);

    private:
      static constexpr uint32_t kTransferBufSize = 256U;

      std::array<Driver::Uart*, 3> uarts_;
      Driver::UsbCdc &usbCdc_;
      UartChannel currentChannel_;
      uint32_t currentBaudRate_;
      bool running_;

      Driver::Uart& getActiveUart();
  };

} // namespace App

#endif /* APP_COMMUNICATIONMANAGER_PASSTHROUGHCONTROLLER_HPP_ */
