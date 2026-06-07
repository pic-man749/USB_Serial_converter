/*
 * InputEvent.hpp
 */

#ifndef USER_APP_INPUT_INPUTEVENT_HPP_
#define USER_APP_INPUT_INPUTEVENT_HPP_

#include <cstdint>

namespace App {

enum class InputEvent : uint8_t {
    NONE,
    SCROLL_UP,         // エンコーダ CCW または上ボタン
    SCROLL_DOWN,       // エンコーダ CW  または下ボタン
    BTN_LEFT,          // 左ボタン短押し（1000 ms 未満）
    BTN_LEFT_LONG,     // 左ボタン長押し（1000 ms 以上）
    BTN_RIGHT,
    BTN_CENTER_SHORT,  // センターボタン短押し（1000 ms 未満）
    BTN_CENTER_LONG,   // センターボタン長押し（1000 ms 以上）
};

} // namespace App

#endif /* USER_APP_INPUT_INPUTEVENT_HPP_ */
