/*
 * UartBridge.hpp
 */

#ifndef USER_APP_BRIDGE_UARTBRIDGE_HPP_
#define USER_APP_BRIDGE_UARTBRIDGE_HPP_

#include <cstdint>
#include "App/Settings.hpp"
#include "Driver/USB_CDC/UsbCdc.hpp"
#include "Driver/Uart/Uart.hpp"

namespace App {

/**
 * @brief 転送データの通知を受け取るオブザーバインタフェース
 *
 * UartBridge が USB↔UART 間でデータを転送した際に呼び出される。
 * DisplayManager が実装し、表示バッファへの追記に使用する。
 */
class IDataObserver {
public:
    /**
     * @brief USB→UART 方向のデータが転送されたときに呼ばれる
     * @param data 転送されたデータへのポインタ
     * @param len  転送バイト数
     */
    virtual void onTxData(const uint8_t* data, uint32_t len) = 0;

    /**
     * @brief UART→USB 方向のデータが転送されたときに呼ばれる
     * @param data 転送されたデータへのポインタ
     * @param len  転送バイト数
     */
    virtual void onRxData(const uint8_t* data, uint32_t len) = 0;

    virtual ~IDataObserver() = default;
};

/**
 * @brief USB CDC ↔ 選択中 UART の双方向パススルーを管理するクラス
 *
 * - process() を毎ループ呼び出すことで双方向の転送を行う
 * - applySettings() で UART チャンネル・ボーレートを切り替える
 * - 転送データは IDataObserver 経由で DisplayManager へ通知される
 */
class UartBridge {
public:
    /**
     * @param usb      USB CDC ドライバ（シングルトン参照）
     * @param uart1    USART1 ドライバ
     * @param uart2    USART2 ドライバ
     * @param uart6    USART6 ドライバ
     * @param observer 転送通知先（DisplayManager）
     */
    UartBridge(Driver::UsbCdc&   usb,
               Driver::Uart&     uart1,
               Driver::Uart&     uart2,
               Driver::Uart&     uart6,
               IDataObserver&    observer);

    /**
     * @brief 双方向のデータ転送を 1 ループ分処理する
     * @details 毎ループ呼び出すこと。1 回あたり最大 TRANSFER_BUF_SIZE バイトを処理する。
     */
    void process();

    /**
     * @brief 設定を適用してアクティブ UART を切り替える
     * @details 旧 UART の受信停止 → バッファクリア → 新 UART のボーレート設定 → 受信再開
     * @param s 新しい設定
     */
    void applySettings(const Settings& s);

private:
    Driver::UsbCdc&  usb_;
    Driver::Uart*    uarts_[3];   // インデックス: UartChannel の値と対応
    Driver::Uart*    activeUart_;
    IDataObserver&   observer_;

    // 1 ループあたりの最大転送バイト数
    static constexpr uint32_t TRANSFER_BUF_SIZE = 256;
    uint8_t txBuf_[TRANSFER_BUF_SIZE];
    uint8_t rxBuf_[TRANSFER_BUF_SIZE];

    Driver::Uart* channelToUart(UartChannel ch);
};

} // namespace App

#endif /* USER_APP_BRIDGE_UARTBRIDGE_HPP_ */
