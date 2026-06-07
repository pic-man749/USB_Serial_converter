/*
 * SettingsMenu.hpp
 */

#ifndef USER_APP_MENU_SETTINGSMENU_HPP_
#define USER_APP_MENU_SETTINGSMENU_HPP_

#include <cstdint>
#include "App/Settings.hpp"
#include "App/Input/InputEvent.hpp"
#include "BinaryGFX.hpp"

namespace App {

/**
 * @brief 設定メニューの状態機械と OLED1 への描画を担うクラス
 *
 * メニュー構造:
 *   MAIN  → SUB_UART   : UART チャンネル選択
 *         → SUB_BAUD   : ボーレート選択
 *         → SUB_FORMAT : 表示形式選択
 *
 * 各サブメニューで CENTER_SHORT を押すと選択値を settings_ に即時反映し MAIN へ戻る。
 * MAIN から LEFT を押すとメニューを閉じる。
 * settingsChangedAndReset() で設定変更を検出し、UserMain が applySettings() を呼ぶ。
 *
 * render() は TextObject の文字列バッファを更新するのみ。
 * BinaryGFX::BinaryGFX::update() の呼び出しは UserMain が行う。
 */
class SettingsMenu {
public:
    static constexpr uint8_t MENU_LINES    = 8;
    static constexpr uint8_t LINE_BUF_SIZE = 22;  // 21文字 + '\0'

    SettingsMenu(BinaryGFX::BinaryGFX& oled, Settings& settings);

    /**
     * @brief BinaryGFX へ TextObject を登録する（setup() 内で 1 回呼ぶ）
     */
    void init();

    bool isOpen() const { return menuState_ != MenuState::CLOSED; }

    /**
     * @brief メニューを開く（センター長押し時に呼ぶ）
     */
    void open();

    /**
     * @brief 入力イベントを処理する
     */
    void handleInput(InputEvent ev);

    /**
     * @brief OLED1 へメニュー内容を描画する
     */
    void render();

    /**
     * @brief 設定が変更されたかを確認しフラグをリセットする
     * @return true=変更あり（UserMain は applySettings() を呼ぶこと）
     */
    bool settingsChangedAndReset();

    /**
     * @brief メニュー TextObject の表示状態を切り替える
     * @param visible true=表示, false=非表示
     */
    void setVisible(bool visible);

private:
    enum class MenuState : uint8_t {
        CLOSED,
        MAIN,
        SUB_UART,
        SUB_BAUD,
        SUB_FORMAT,
    };

    static constexpr uint8_t MAIN_ITEM_COUNT   = 3;
    static constexpr uint8_t UART_OPTION_COUNT = 3;  // USART1/2/6
    static constexpr uint8_t FORMAT_OPTION_COUNT = 2; // ASCII/HEX

    BinaryGFX::BinaryGFX& oled_;
    Settings&              settings_;

    MenuState menuState_      = MenuState::CLOSED;
    uint8_t   cursor_         = 0;  // MAIN: 0-2、SUB: 選択中インデックス
    uint8_t   tempValue_      = 0;  // サブメニュー編集中の一時選択値
    bool      settingsChanged_ = false;

    char menuLineBuffers_[MENU_LINES][LINE_BUF_SIZE];
    BinaryGFX::ObjectId menuLineIds_[MENU_LINES];

    void handleMain(InputEvent ev);
    void handleSub(InputEvent ev);

    // サブメニューの選択値を settings_ へ反映する
    void commitSub();

    // サブメニューの選択上限値を返す
    uint8_t subItemCount() const;

    // 各状態の描画処理
    void renderMain();
    void renderSub();

    // チャンネル設定 → インデックス変換
    static uint8_t channelToIndex(UartChannel ch);
    static UartChannel indexToChannel(uint8_t idx);
};

} // namespace App

#endif /* USER_APP_MENU_SETTINGSMENU_HPP_ */
