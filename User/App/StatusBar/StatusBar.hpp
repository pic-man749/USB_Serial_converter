/*
 * StatusBar.hpp
 */

#ifndef USER_APP_STATUSBAR_STATUSBAR_HPP_
#define USER_APP_STATUSBAR_STATUSBAR_HPP_

#include <cstdint>
#include "App/Settings.hpp"
#include "BinaryGFX.hpp"

namespace App {

/**
 * @brief 両 OLED の最下行にステータス情報を表示するクラス
 *
 * 表示内容: "U1 115200 ASC" 形式（チャンネル・ボーレート・表示形式）
 * 設定変更を差分検出し、変化があったときのみ文字列バッファを更新する。
 * BinaryGFX::BinaryGFX::update() の呼び出しは UserMain が行う。
 */
class StatusBar {
public:
    static constexpr int16_t STATUS_Y      = 56;
    static constexpr uint8_t LINE_BUF_SIZE = 22;  // 21文字 + '\0'

    StatusBar(BinaryGFX::BinaryGFX& oledTx,
              BinaryGFX::BinaryGFX& oledRx,
              const Settings&       settings);

    /**
     * @brief BinaryGFX へ TextObject を登録する（setup() 内で 1 回呼ぶ）
     */
    void init();

    /**
     * @brief 設定が変化していれば TextObject の文字列バッファを更新する
     */
    void render();

    /**
     * @brief OLED1 (TX) 側のステータスバー TextObject の表示状態を切り替える
     * @param visible true=表示, false=非表示（メニューモード時に使用）
     */
    void setTxVisible(bool visible);

private:
    BinaryGFX::BinaryGFX& oledTx_;
    BinaryGFX::BinaryGFX& oledRx_;
    const Settings&        settings_;

    char txStatusBuf_[LINE_BUF_SIZE];
    char rxStatusBuf_[LINE_BUF_SIZE];

    BinaryGFX::ObjectId txStatusId_;
    BinaryGFX::ObjectId rxStatusId_;

    // 差分検出用（前回の描画設定）
    Settings lastRendered_;

    // 現在の settings_ からステータス文字列を生成する
    void formatStatus(char* buf, uint8_t bufLen) const;
};

} // namespace App

#endif /* USER_APP_STATUSBAR_STATUSBAR_HPP_ */
