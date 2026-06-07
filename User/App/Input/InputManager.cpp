/*
 * InputManager.cpp
 */

#include "App/Input/InputManager.hpp"
#include "stm32f4xx_hal.h"

namespace App {

InputManager::InputManager(Driver::Button& btn, Driver::Encoder& enc)
    : btn_(btn),
      enc_(enc),
      lastEncValue_(enc.GetValue()),
      lastBtn_(Driver::ButtonType::NONE),
      leftHeld_(false),
      leftLongEmitted_(false),
      leftPressStartMs_(0),
      centerHeld_(false),
      centerLongEmitted_(false),
      centerPressStartMs_(0)
{
}

InputEvent InputManager::update() {
    const uint32_t now = HAL_GetTick();
    const Driver::ButtonType btn = btn_.GetPushedButton();

    // --- 左ボタン長押し判定 ---
    if (btn == Driver::ButtonType::LEFT) {
        if (!leftHeld_) {
            leftHeld_         = true;
            leftLongEmitted_  = false;
            leftPressStartMs_ = now;
        } else if (!leftLongEmitted_ &&
                   (now - leftPressStartMs_ >= LONG_PRESS_MS)) {
            leftLongEmitted_ = true;
            return InputEvent::BTN_LEFT_LONG;
        }
        return InputEvent::NONE;
    }

    // 左ボタンが離された瞬間: 短押しイベントを発行
    if (leftHeld_) {
        leftHeld_ = false;
        lastBtn_  = Driver::ButtonType::NONE;
        if (!leftLongEmitted_) {
            return InputEvent::BTN_LEFT;
        }
        return InputEvent::NONE;
    }

    // --- センターボタン長押し判定 ---
    if (btn == Driver::ButtonType::CENTER) {
        if (!centerHeld_) {
            centerHeld_         = true;
            centerLongEmitted_  = false;
            centerPressStartMs_ = now;
        } else if (!centerLongEmitted_ &&
                   (now - centerPressStartMs_ >= LONG_PRESS_MS)) {
            centerLongEmitted_ = true;
            return InputEvent::BTN_CENTER_LONG;
        }
        return InputEvent::NONE;
    }

    // センターが離された瞬間: 短押しイベントを発行
    if (centerHeld_) {
        centerHeld_ = false;
        if (!centerLongEmitted_) {
            return InputEvent::BTN_CENTER_SHORT;
        }
        return InputEvent::NONE;
    }

    // --- エンコーダ差分検出 ---
    const int curEnc = enc_.GetValue();
    const int delta  = curEnc - lastEncValue_;
    lastEncValue_    = curEnc;
    if (delta > 0) return InputEvent::SCROLL_DOWN;
    if (delta < 0) return InputEvent::SCROLL_UP;

    // --- その他ボタン（エッジ検出）---
    if (btn != lastBtn_) {
        lastBtn_ = btn;
        switch (btn) {
            case Driver::ButtonType::RIGHT:  return InputEvent::BTN_RIGHT;
            case Driver::ButtonType::TOP:    return InputEvent::SCROLL_UP;
            case Driver::ButtonType::BOTTOM: return InputEvent::SCROLL_DOWN;
            default: break;
        }
    }

    return InputEvent::NONE;
}

} // namespace App
