/*
 * CommunicationManager.hpp
 *
 * USB CDC と UART 間のパススルー処理と、表示用データ記録を担うクラス。
 * 通信方向別に独立した DisplayBuffer を保持する。
 */

#ifndef APP_COMMUNICATIONMANAGER_COMMUNICATIONMANAGER_HPP_
#define APP_COMMUNICATIONMANAGER_COMMUNICATIONMANAGER_HPP_

#include <array>
#include <cstdint>
#include "DisplayBuffer.hpp"
#include "../AppConfig.hpp"
#include "../../Driver/Uart/Uart.hpp"
#include "../../Driver/USB_CDC/UsbCdc.hpp"

namespace App {

  /**
   * USB CDC ↔ UART 間のパススルー処理と方向別 DisplayBuffer への記録を担う。
   *
   * - AppConfig.selectedUart の変更を update() 内で自動検知してUARTを切り替える。
   * - AppConfig.baudRate の変更を update() 内で自動検知してボーレートを変更する。
   * - start() 呼び出し後からパススルーを開始し、stop() で停止する。
   */
  class CommunicationManager {
    public:
      /**
       * @param uart1  USART1 に対応する Uart インスタンス
       * @param uart2  USART2 に対応する Uart インスタンス
       * @param uart3  USART6 に対応する Uart インスタンス
       * @param usbCdc UsbCdc シングルトンインスタンス
       * @param config アプリ設定（参照）
       */
      CommunicationManager(
          Driver::Uart&   uart1,
          Driver::Uart&   uart2,
          Driver::Uart&   uart3,
          Driver::UsbCdc& usbCdc,
          AppConfig&      config
      );

      /** @brief パススルーを開始する。アクティブ UART の DMA 受信を開始する。 */
      void start();

      /** @brief パススルーを停止する。アクティブ UART の DMA 受信を停止する。 */
      void stop();

      /**
       * @brief パススルー処理を実行する。メインループから毎回呼び出すこと。
       *
       * UART切り替えおよびボーレート変更の自動検知も本メソッド内で行う。
       */
      void update();

      /** @brief UART→USB 方向の DisplayBuffer への const 参照を返す */
      const DisplayBuffer& getUartToUsbBuffer() const;

      /** @brief USB→UART 方向の DisplayBuffer への const 参照を返す */
      const DisplayBuffer& getUsbToUartBuffer() const;

      /** @brief いずれかの DisplayBuffer に新着データがあれば true を返す */
      bool hasNewData() const;

      /** @brief 両 DisplayBuffer の新着フラグをクリアする */
      void clearNewDataFlag();

    private:
      Driver::Uart& getActiveUart();

      static constexpr uint32_t kTransferBufSize = 256U;

      std::array<Driver::Uart*, 3> uarts_;
      Driver::UsbCdc& usbCdc_;
      AppConfig&      config_;
      UartChannel     currentChannel_;
      uint32_t        currentBaudRate_;
      DisplayBuffer   uartToUsbBuffer_;
      DisplayBuffer   usbToUartBuffer_;
      bool            running_;
  };

} // namespace App

#endif /* APP_COMMUNICATIONMANAGER_COMMUNICATIONMANAGER_HPP_ */
