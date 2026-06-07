/*
 * DisplayManager.hpp
 */

#ifndef USER_APP_DISPLAY_DISPLAYMANAGER_HPP_
#define USER_APP_DISPLAY_DISPLAYMANAGER_HPP_

#include <cstdint>
#include "App/Settings.hpp"
#include "App/Input/InputEvent.hpp"
#include "App/Bridge/UartBridge.hpp"
#include "App/Display/DisplayBuffer.hpp"
#include "BinaryGFX.hpp"

namespace App {

/**
 * @brief TX / RX の表示履歴を管理し、両 OLED へ描画するクラス
 *
 * IDataObserver を実装し、UartBridge からデータ通知を受け取る。
 * render() は TextObject の文字列バッファを更新するのみ。
 * BinaryGFX::BinaryGFX::update() の呼び出しは UserMain が行う。
 *
 * - OLED TX 側: USB→UART 方向のデータを表示
 * - OLED RX 側: UART→USB 方向のデータを表示
 * - topLine_: 両 OLED の表示先頭行（同期スクロール）
 */
class DisplayManager : public IDataObserver {
public:
    static constexpr uint8_t DATA_LINES    = 7;   // ステータスバー除く表示行数
    static constexpr uint8_t LINE_BUF_SIZE = 22;  // 21文字 + '\0'

    DisplayManager(BinaryGFX::BinaryGFX& oledTx,
                   BinaryGFX::BinaryGFX& oledRx,
                   const Settings&       settings);

    /**
     * @brief BinaryGFX へ TextObject を登録する（setup() 内で 1 回呼ぶ）
     */
    void init();

    // --- IDataObserver ---
    void onTxData(const uint8_t* data, uint32_t len) override;
    void onRxData(const uint8_t* data, uint32_t len) override;

    /**
     * @brief 入力イベントをスクロール制御へ変換する（毎ループ呼ぶ）
     */
    void handleInput(InputEvent ev);

    /**
     * @brief 両 OLED の TextObject を更新する（通常モード）
     */
    void render();

    /**
     * @brief OLED2 (RX) の TextObject のみ更新する（メニューモード）
     */
    void renderRx();

    /**
     * @brief TX / RX 両バッファの内容を消去し、画面を空にする
     */
    void clearBuffers();

    /**
     * @brief OLED1 (TX) 側の TextObject の表示状態を切り替える
     * @param visible true=表示, false=非表示（メニューモード切り替え時に使用）
     */
    void setTxVisible(bool visible);

private:
    BinaryGFX::BinaryGFX& oledTx_;
    BinaryGFX::BinaryGFX& oledRx_;
    const Settings&        settings_;

    DisplayBuffer txBuf_;
    DisplayBuffer rxBuf_;

    uint32_t      topLine_    = 0;
    bool          autoScroll_ = true;
    DisplayFormat lastFormat_ = DisplayFormat::ASCII;

    char txLineBuffers_[DATA_LINES][LINE_BUF_SIZE];
    char rxLineBuffers_[DATA_LINES][LINE_BUF_SIZE];

    BinaryGFX::ObjectId txLineIds_[DATA_LINES];
    BinaryGFX::ObjectId rxLineIds_[DATA_LINES];

    // フォーマット・バッファ状態からオートスクロール位置を更新する
    void updateAutoScroll();

    // 指定 OLED の TextObject を buf の内容で更新する
    void renderDisplay(BinaryGFX::BinaryGFX& oled,
                       DisplayBuffer&        buf,
                       char                  lines[][LINE_BUF_SIZE]);
};

} // namespace App

#endif /* USER_APP_DISPLAY_DISPLAYMANAGER_HPP_ */
