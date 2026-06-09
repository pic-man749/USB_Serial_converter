/*
 * Uart.hpp
 *
 *      Author: picman
 */

#ifndef DRIVER_UART_UART_HPP_
#define DRIVER_UART_UART_HPP_

#include "usart.h"
#include <cstdint>
#include <memory>
#include <vector>

namespace Driver {
  /**
   * @brief UARTドライバクラス
   * @details 受信はDMAサーキュラーモード、送信は割込み方式で動作する
   */
  class Uart {
    public:
      /**
       * @brief コンストラクタ
       * @param huart_    STM32 HAL UARTハンドル
       * @param queueSize DMAサーキュラーバッファのサイズ（バイト）
       */
      Uart(UART_HandleTypeDef *huart_, const uint32_t queueSize);

      /**
       * @brief デストラクタ。DMA転送を停止する
       */
      ~Uart();

      /**
       * @brief ボーレートを変更する
       * @details 内部でUARTの再初期化を行う
       * @param baudrate 設定するボーレート（bps）
       * @return 初期化成功時 true、失敗時 false
       */
      bool setbaudrate(uint32_t baudrate);

      /**
       * @brief UART送信完了コールバック（静的ディスパッチ）
       * @details Callback_wrapper の Uart_TxCpltCallback から呼び出す
       * @param huart 送信完了した UART ハンドル
       */
      static void TxCpltCallback(UART_HandleTypeDef *huart);

      /**
       * @brief データを送信する（前回送信完了まで待機後、割込みで非同期送信）
       * @details 内部バッファにコピーしてから HAL_UART_Transmit_IT を呼び出す
       * @param data 送信データへのポインタ
       * @param size 送信バイト数
       */
      void send(const char *data, uint32_t size);

      /**
       * @brief DMAサーキュラーモードで受信を開始する
       * @details 呼び出し時に読み出し位置をリセットする
       */
      void startReceive();

      /**
       * @brief DMA受信を停止する
       * @details 呼び出し時に読み出し位置をリセットする
       */
      void stopReceive();

      /**
       * @brief 未読の受信データサイズを返す
       * @return 読み出し可能なバイト数
       */
      uint32_t getReceiveDataSize();

      /**
       * @brief 受信バッファからデータを読み出す
       * @param data 読み出しデータの格納先バッファ
       * @param size 読み出しバイト数（納先バッファのサイズ）
       * @return 実際に読み出したバイト数
       */
      uint32_t getReceiveData(uint8_t *data, uint32_t size);

      /**
       * @brief 受信バッファをクリアする
       */
      void clearReceiveBuffer();

    private:
      UART_HandleTypeDef *huart_;
      std::vector<uint8_t> receiveBuffer_;
      uint32_t readPos_;
      bool isSending_;
      std::unique_ptr<uint8_t[]> sendBuffer_;
      uint16_t sendBufferSize_;

      // インスタンスのディスパッチ用レジストリ
      static constexpr uint8_t INSTANCE_REGISTRY_SIZE = 4;
      static Uart *instanceRegistry_[INSTANCE_REGISTRY_SIZE];
      static uint8_t instanceCount_;
  };
}

#endif /* DRIVER_UART_UART_HPP_ */
