/*
 * CommunicationManager.hpp
 *
 * PassthroughController への委譲、DisplayBuffer の管理、
 * IEventSource および IConfigApplicable の実装を担うクラス。
 */

#ifndef APP_COMMUNICATIONMANAGER_COMMUNICATIONMANAGER_HPP_
#define APP_COMMUNICATIONMANAGER_COMMUNICATIONMANAGER_HPP_

#include <cstdint>
#include "DisplayBuffer.hpp"
#include "PassthroughController.hpp"
#include "../AppConfig/AppConfig.hpp"
#include "../IEventSource.hpp"
#include "../IConfigApplicable.hpp"

namespace App {

  class CommunicationManager: public IEventSource, public IConfigApplicable {
    public:
      /**
       * @param uart1         USART1 に対応する Uart インスタンス
       * @param uart2         USART2 に対応する Uart インスタンス
       * @param uart3         USART6 に対応する Uart インスタンス
       * @param usbCdc        UsbCdc シングルトンインスタンス
       * @param initialChannel  起動時のアクティブ UART チャンネル
       * @param initialBaudRate 起動時のボーレート
       */
      CommunicationManager(Driver::Uart &uart1,
                           Driver::Uart &uart2, Driver::Uart &uart3,
                           Driver::UsbCdc &usbCdc,
                           UartChannel initialChannel = UartChannel::UART1,
                           uint32_t initialBaudRate = 115200U);

      /**
       * @brief パススルーを開始する。
       *        StateMonitorCommunication::Enter() から呼び出される。
       *        設定画面への遷移中は stop() を呼ばないため、パススルーは継続される。
       *        べき等であり、重複呼び出しは無視される。
       */
      void start();

      /**
       * @brief パススルーを停止する。
       *        べき等であり、重複呼び出しは無視される。
       *        現時点では通常フローで呼び出されない（パススルーは起動後継続）。
       */
      void stop();

      /**
       * @brief データ転送を実行し、新着データがあれば CommunicationDataEvent を push する。
       *        EventBus::Update() から呼び出される。
       */
      void Update(EventQueue &queue) override;

      /** @brief UARTチャンネルを切り替える。IConfigApplicable 実装。*/
      void setChannel(UartChannel channel) override;

      /** @brief ボーレートを変更する。IConfigApplicable 実装。*/
      bool setBaudRate(uint32_t baudRate) override;

      /** @brief UART→USB 方向の DisplayBuffer への const 参照を返す */
      const DisplayBuffer& getUartToUsbBuffer() const;

      /** @brief USB→UART 方向の DisplayBuffer への const 参照を返す */
      const DisplayBuffer& getUsbToUartBuffer() const;

      /** @brief 両方向の表示バッファを消去する */
      void clearDisplayBuffers();

    private:
      PassthroughController passthrough_;
      DisplayBuffer uartToUsbBuffer_;
      DisplayBuffer usbToUartBuffer_;
  };

} // namespace App

#endif /* APP_COMMUNICATIONMANAGER_COMMUNICATIONMANAGER_HPP_ */
