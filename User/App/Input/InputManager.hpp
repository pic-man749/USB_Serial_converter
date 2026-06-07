/*
 * InputManager.hpp
 */

#ifndef USER_APP_INPUT_INPUTMANAGER_HPP_
#define USER_APP_INPUT_INPUTMANAGER_HPP_

#include <cstdint>
#include "App/Input/InputEvent.hpp"
#include "Driver/Button/Button.hpp"
#include "Driver/Button/ButtonType.hpp"
#include "Driver/Encoder/Encoder.hpp"

namespace App {

/**
 * @brief ボタン・エンコーダ入力を InputEvent へ変換する
 *
 * - エンコーダ: 差分を毎フレーム検出し SCROLL_UP / SCROLL_DOWN を生成
 * - 上下ボタン: エッジ検出（押下時のみ）で SCROLL_UP / SCROLL_DOWN を生成
 * - 左ボタン:   長押し（LONG_PRESS_MS 以上）で BTN_LEFT_LONG を一度だけ生成
 *               短押し（離した時点で閾値未満）で BTN_LEFT を生成
 * - 右ボタン:   エッジ検出で BTN_RIGHT を生成
 * - センター:   長押し（LONG_PRESS_MS 以上）で BTN_CENTER_LONG を一度だけ生成
 *               短押し（離した時点で閾値未満）で BTN_CENTER_SHORT を生成
 *
 * update() は毎ループ 1 回呼び出すこと。1 呼び出しにつき最大 1 イベントを返す。
 */
class InputManager {
public:
    InputManager(Driver::Button& btn, Driver::Encoder& enc);

    InputEvent update();

private:
    Driver::Button&  btn_;
    Driver::Encoder& enc_;

    int                  lastEncValue_;
    Driver::ButtonType   lastBtn_;

    bool     leftHeld_;
    bool     leftLongEmitted_;
    uint32_t leftPressStartMs_;

    bool     centerHeld_;
    bool     centerLongEmitted_;
    uint32_t centerPressStartMs_;

    static constexpr uint32_t LONG_PRESS_MS = 1000;
};

} // namespace App

#endif /* USER_APP_INPUT_INPUTMANAGER_HPP_ */
