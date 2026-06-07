/*
 * SettingsMenu.cpp
 */

#include "App/Menu/SettingsMenu.hpp"
#include <cstring>
#include <cstdio>

namespace App {

// チャンネル名テーブル
static constexpr const char* CHANNEL_NAMES[] = { "USART1", "USART2", "USART6" };

// MAIN メニューの項目名テーブル
static constexpr const char* MAIN_LABELS[] = { "UART", "Baud", "Fmt " };

// 表示形式名テーブル
static constexpr const char* FORMAT_NAMES[] = { "ASCII", "HEX" };

SettingsMenu::SettingsMenu(BinaryGFX::BinaryGFX& oled, Settings& settings)
    : oled_(oled), settings_(settings)
{
    memset(menuLineBuffers_, 0, sizeof(menuLineBuffers_));
}

void SettingsMenu::init() {
    for (uint8_t i = 0; i < MENU_LINES; ++i) {
        // メニューは初期非表示（PixelState::Off）
        menuLineIds_[i] = oled_.addObject(
            std::make_unique<BinaryGFX::TextObject>(
                0, static_cast<int16_t>(i * 8),
                menuLineBuffers_[i],
                &BinaryGFX::BgfxFont_Ascii,
                BinaryGFX::PixelState::Off,
                20));  // Z=20 でステータスバー(Z=10)より前面
    }
}

void SettingsMenu::open() {
    cursor_    = 0;
    menuState_ = MenuState::MAIN;
    setVisible(true);
}

void SettingsMenu::handleInput(InputEvent ev) {
    switch (menuState_) {
        case MenuState::MAIN:       handleMain(ev); break;
        case MenuState::SUB_UART:
        case MenuState::SUB_BAUD:
        case MenuState::SUB_FORMAT: handleSub(ev);  break;
        default: break;
    }
}

void SettingsMenu::render() {
    switch (menuState_) {
        case MenuState::MAIN:       renderMain(); break;
        case MenuState::SUB_UART:
        case MenuState::SUB_BAUD:
        case MenuState::SUB_FORMAT: renderSub();  break;
        default: break;
    }
}

bool SettingsMenu::settingsChangedAndReset() {
    if (settingsChanged_) {
        settingsChanged_ = false;
        return true;
    }
    return false;
}

void SettingsMenu::setVisible(bool visible) {
    const BinaryGFX::PixelState ps = visible
        ? BinaryGFX::PixelState::On
        : BinaryGFX::PixelState::Off;
    for (uint8_t i = 0; i < MENU_LINES; ++i) {
        auto* obj = oled_.getObjectById<BinaryGFX::TextObject>(menuLineIds_[i]);
        if (obj) obj->setPixelState(ps);
        // 非表示時はバッファを空にする。
        // PixelState::Off の TextObject はグリフのビットをゼロクリアするため、
        // 低 Z の On オブジェクトが描いたピクセルに "穴" を開けてしまう。
        // バッファを空にすることでグリフ描画自体を抑止する。
        if (!visible) {
            menuLineBuffers_[i][0] = '\0';
        }
    }
}

// -----------------------------------------------------------------------
//  MAIN メニューの入力処理
// -----------------------------------------------------------------------
void SettingsMenu::handleMain(InputEvent ev) {
    switch (ev) {
        case InputEvent::SCROLL_UP:
            if (cursor_ > 0) --cursor_;
            break;

        case InputEvent::SCROLL_DOWN:
            if (cursor_ < MAIN_ITEM_COUNT - 1) ++cursor_;
            break;

        case InputEvent::BTN_RIGHT:
        case InputEvent::BTN_CENTER_SHORT:
            // カーソル位置に対応するサブメニューへ遷移
            tempValue_ = (cursor_ == 0) ? channelToIndex(settings_.channel)
                       : (cursor_ == 1) ? settings_.baudRateIdx
                                        : static_cast<uint8_t>(settings_.format);
            menuState_ = (cursor_ == 0) ? MenuState::SUB_UART
                       : (cursor_ == 1) ? MenuState::SUB_BAUD
                                        : MenuState::SUB_FORMAT;
            cursor_    = tempValue_;  // サブメニュー内カーソルを現在値に合わせる
            break;

        case InputEvent::BTN_LEFT:
            // メニューを閉じる
            menuState_ = MenuState::CLOSED;
            setVisible(false);
            break;

        default:
            break;
    }
}

// -----------------------------------------------------------------------
//  サブメニューの入力処理
// -----------------------------------------------------------------------
void SettingsMenu::handleSub(InputEvent ev) {
    const uint8_t maxIdx = subItemCount() - 1;

    switch (ev) {
        case InputEvent::SCROLL_UP:
            if (cursor_ > 0) --cursor_;
            break;

        case InputEvent::SCROLL_DOWN:
            if (cursor_ < maxIdx) ++cursor_;
            break;

        case InputEvent::BTN_CENTER_SHORT:
            // 選択値を設定に反映して MAIN へ戻る
            commitSub();
            menuState_ = MenuState::MAIN;
            cursor_    = 0;
            break;

        case InputEvent::BTN_LEFT:
            // 変更を破棄して MAIN へ戻る
            menuState_ = MenuState::MAIN;
            cursor_    = 0;
            break;

        default:
            break;
    }
}

// -----------------------------------------------------------------------
//  サブメニューの選択値を settings_ へ反映
// -----------------------------------------------------------------------
void SettingsMenu::commitSub() {
    switch (menuState_) {
        case MenuState::SUB_UART:
            settings_.channel = indexToChannel(cursor_);
            break;
        case MenuState::SUB_BAUD:
            settings_.baudRateIdx = cursor_;
            break;
        case MenuState::SUB_FORMAT:
            settings_.format = static_cast<DisplayFormat>(cursor_);
            break;
        default:
            break;
    }
    settingsChanged_ = true;
}

uint8_t SettingsMenu::subItemCount() const {
    switch (menuState_) {
        case MenuState::SUB_UART:   return UART_OPTION_COUNT;
        case MenuState::SUB_BAUD:   return BAUD_RATE_COUNT;
        case MenuState::SUB_FORMAT: return FORMAT_OPTION_COUNT;
        default:                    return 0;
    }
}

// -----------------------------------------------------------------------
//  MAIN メニューの描画
// -----------------------------------------------------------------------
void SettingsMenu::renderMain() {
    // Line 0: ヘッダ
    snprintf(menuLineBuffers_[0], LINE_BUF_SIZE, " [SETTINGS]");
    // Line 1: 空行
    menuLineBuffers_[1][0] = '\0';

    // Lines 2-4: 設定項目（カーソル位置に '>' を表示）
    const char* curFmtStr = (settings_.format == DisplayFormat::ASCII) ? "ASCII" : "HEX";
    const char* items[MAIN_ITEM_COUNT] = {
        CHANNEL_NAMES[channelToIndex(settings_.channel)],
        nullptr,
        curFmtStr,
    };

    for (uint8_t i = 0; i < MAIN_ITEM_COUNT; ++i) {
        const char prefix = (cursor_ == i) ? '>' : ' ';
        if (i == 1) {
            // ボーレートは数値で表示
            snprintf(menuLineBuffers_[2 + i], LINE_BUF_SIZE,
                     "%c %s: %lu", prefix, MAIN_LABELS[i],
                     static_cast<unsigned long>(settings_.baudRate()));
        } else {
            snprintf(menuLineBuffers_[2 + i], LINE_BUF_SIZE,
                     "%c %s: %s", prefix, MAIN_LABELS[i], items[i]);
        }
    }

    // Lines 5-6: 空行
    menuLineBuffers_[5][0] = '\0';
    menuLineBuffers_[6][0] = '\0';

    // Line 7: ヒント
    snprintf(menuLineBuffers_[7], LINE_BUF_SIZE, " [L]:Cancel");
}

// -----------------------------------------------------------------------
//  サブメニューの描画
// -----------------------------------------------------------------------
void SettingsMenu::renderSub() {
    const uint8_t count = subItemCount();

    // Line 0: サブメニューのタイトル
    switch (menuState_) {
        case MenuState::SUB_UART:
            snprintf(menuLineBuffers_[0], LINE_BUF_SIZE, " UART Channel");
            break;
        case MenuState::SUB_BAUD:
            snprintf(menuLineBuffers_[0], LINE_BUF_SIZE, " Baud Rate");
            break;
        case MenuState::SUB_FORMAT:
            snprintf(menuLineBuffers_[0], LINE_BUF_SIZE, " Display Fmt");
            break;
        default:
            break;
    }

    // Lines 1-7: 選択肢（最大6個 + ヒント行）
    // ボーレートは 6 選択肢が lines 1-6 に収まる。UART/Fmt は空行で埋める。
    for (uint8_t i = 0; i < 6; ++i) {
        const uint8_t lineIdx = 1 + i;
        if (i < count) {
            const char prefix = (cursor_ == i) ? '>' : ' ';
            if (menuState_ == MenuState::SUB_UART) {
                snprintf(menuLineBuffers_[lineIdx], LINE_BUF_SIZE,
                         "%c %s", prefix, CHANNEL_NAMES[i]);
            } else if (menuState_ == MenuState::SUB_BAUD) {
                snprintf(menuLineBuffers_[lineIdx], LINE_BUF_SIZE,
                         "%c %lu", prefix,
                         static_cast<unsigned long>(BAUD_RATES[i]));
            } else {
                snprintf(menuLineBuffers_[lineIdx], LINE_BUF_SIZE,
                         "%c %s", prefix, FORMAT_NAMES[i]);
            }
        } else {
            menuLineBuffers_[lineIdx][0] = '\0';
        }
    }

    // Line 7: ヒント
    snprintf(menuLineBuffers_[7], LINE_BUF_SIZE, " [L]:Back [C]:Set");
}

// -----------------------------------------------------------------------
//  ヘルパー関数
// -----------------------------------------------------------------------
uint8_t SettingsMenu::channelToIndex(UartChannel ch) {
    switch (ch) {
        case UartChannel::UART1: return 0;
        case UartChannel::UART2: return 1;
        case UartChannel::UART6: return 2;
        default:                  return 0;
    }
}

UartChannel SettingsMenu::indexToChannel(uint8_t idx) {
    switch (idx) {
        case 0: return UartChannel::UART1;
        case 1: return UartChannel::UART2;
        case 2: return UartChannel::UART6;
        default: return UartChannel::UART1;
    }
}

} // namespace App
