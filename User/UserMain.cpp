/*
 * UserMain.cpp
 */

#include "UserMain.hpp"

#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "stm32f4xx_hal.h"

#include "BinaryGFX.hpp"

#include "App/Settings.hpp"
#include "App/Bridge/UartBridge.hpp"
#include "App/Display/DisplayManager.hpp"
#include "App/Input/InputManager.hpp"
#include "App/Menu/SettingsMenu.hpp"
#include "App/StatusBar/StatusBar.hpp"

#include "Driver/Button/Button.hpp"
#include "Driver/Encoder/Encoder.hpp"
#include "Driver/Uart/Uart.hpp"
#include "Driver/USB_CDC/UsbCdc.hpp"

#include <memory>

namespace {

// --- 定数 ---
constexpr uint8_t  OLED_ADDR          = 0x3C;  // SSD1306 I2C アドレス（7bit）
constexpr uint32_t I2C_TIMEOUT_MS     = 100;
constexpr uint32_t UART_DMA_BUF_SIZE  = 512;
constexpr uint32_t DISPLAY_INTERVAL_MS = 40;    // OLED 更新間隔（約 25 Hz）

// --- HAL レベルのドライバ ---
// グローバル構築は安全（コンストラクタは HAL 関数を呼ばずポインタを格納するのみ）
Driver::Button  btn;
Driver::Encoder enc{ &htim5 };
Driver::Uart    uart1{ &huart1, UART_DMA_BUF_SIZE };
Driver::Uart    uart2{ &huart2, UART_DMA_BUF_SIZE };
Driver::Uart    uart6{ &huart6, UART_DMA_BUF_SIZE };

// --- アプリケーション設定 ---
App::Settings settings;

// --- BinaryGFX インスタンス（setup() で生成） ---
std::unique_ptr<BinaryGFX::BinaryGFX> oledTx;  // OLED1: TX データ表示
std::unique_ptr<BinaryGFX::BinaryGFX> oledRx;  // OLED2: RX データ表示

// --- アプリケーションコンポーネント（setup() で生成） ---
std::unique_ptr<App::UartBridge>     uartBridge;
std::unique_ptr<App::InputManager>   inputManager;
std::unique_ptr<App::DisplayManager> displayManager;
std::unique_ptr<App::SettingsMenu>   settingsMenu;
std::unique_ptr<App::StatusBar>      statusBar;

// --- ループ管理 ---
uint32_t lastDisplayMs = 0;

} // namespace

// ---------------------------------------------------------------------------
//  setup(): HAL 初期化完了後に 1 回だけ呼ばれる
// ---------------------------------------------------------------------------
void setup(void) {

    // --- 初期ボーレートを設定してから DMA 受信を開始する ---
    // setbaudrate() は HAL_UART_DeInit/Init を伴い DMA を再初期化するため、
    // startReceive() より先に呼ばなければならない。
    // 逆順だと HAL_DMA_DeInit() が NDTR を 0 にリセットし、
    // getReceiveDataSize() が誤って 512 を返し続ける。
    uart1.setbaudrate(settings.baudRate());

    // --- UART DMA 受信開始 ---
    uart1.startReceive();
    uart2.startReceive();
    uart6.startReceive();

    // --- エンコーダ開始 ---
    enc.Start();

    // --- BinaryGFX / OLED 初期化 ---
    // OLED1 (TX 側): I2C1
    oledTx = std::make_unique<BinaryGFX::BinaryGFX>(
        std::make_unique<BinaryGFX::Driver::Ssd1306Driver>(
            std::make_unique<BinaryGFX::Hal::Stm32I2cDma>(&hi2c1, I2C_TIMEOUT_MS),
            OLED_ADDR, 128, 64));
    oledTx->init();

    // OLED2 (RX 側): I2C2
    oledRx = std::make_unique<BinaryGFX::BinaryGFX>(
        std::make_unique<BinaryGFX::Driver::Ssd1306Driver>(
            std::make_unique<BinaryGFX::Hal::Stm32I2cDma>(&hi2c2, I2C_TIMEOUT_MS),
            OLED_ADDR, 128, 64));
    oledRx->init();

    // --- アプリケーションコンポーネント構築 ---
    // DisplayManager を先に構築し、UartBridge の IDataObserver として渡す
    displayManager = std::make_unique<App::DisplayManager>(*oledTx, *oledRx, settings);
    displayManager->init();

    uartBridge = std::make_unique<App::UartBridge>(
        Driver::UsbCdc::getInstance(),
        uart1, uart2, uart6,
        *displayManager);

    inputManager = std::make_unique<App::InputManager>(btn, enc);

    settingsMenu = std::make_unique<App::SettingsMenu>(*oledTx, settings);
    settingsMenu->init();

    statusBar = std::make_unique<App::StatusBar>(*oledTx, *oledRx, settings);
    statusBar->init();

    // --- 初期画面を転送 ---
    oledTx->update();
    oledRx->update();
}

// ---------------------------------------------------------------------------
//  loop(): setup() 後に無限ループで呼ばれる
// ---------------------------------------------------------------------------
void loop(void) {

    // --- 毎ループ: USB↔UART パススルー ---
    uartBridge->process();

    // --- 毎ループ: 入力処理 ---
    const App::InputEvent ev = inputManager->update();

    // --- 入力をメニューまたは表示管理へ振り分け ---
    if (settingsMenu->isOpen()) {
        settingsMenu->handleInput(ev);
    } else {
        displayManager->handleInput(ev);

        // センター長押しでメニューを開く
        if (ev == App::InputEvent::BTN_CENTER_LONG) {
            settingsMenu->open();
            // OLED1 のデータ行・ステータスバーを非表示にしてメニューを前面に出す
            displayManager->setTxVisible(false);
            statusBar->setTxVisible(false);
        }

        // 左長押しで TX / RX 両バッファをクリア
        if (ev == App::InputEvent::BTN_LEFT_LONG) {
            displayManager->clearBuffers();
        }
    }

    // --- 設定変更の検出: SettingsMenu が確定したら UART に反映 ---
    if (settingsMenu->settingsChangedAndReset()) {
        uartBridge->applySettings(settings);
    }

    // --- メニューが閉じた瞬間の復帰処理 ---
    // isOpen() が false になったら TX 側の表示を元に戻す
    // （閉じた直後のみ実行されるよう前回の開閉状態を追跡）
    static bool wasMenuOpen = false;
    const bool  isMenuOpen  = settingsMenu->isOpen();
    if (wasMenuOpen && !isMenuOpen) {
        // メニューが閉じた: TX 側の表示を復元
        settingsMenu->setVisible(false);
        displayManager->setTxVisible(true);
        statusBar->setTxVisible(true);
    }
    wasMenuOpen = isMenuOpen;

    // --- 定期: OLED 描画（40 ms 間隔）---
    const uint32_t now = HAL_GetTick();
    if (now - lastDisplayMs < DISPLAY_INTERVAL_MS) return;
    lastDisplayMs = now;

    if (settingsMenu->isOpen()) {
        settingsMenu->render();
        displayManager->renderRx();  // OLED2 は RX データを継続表示
    } else {
        displayManager->render();
    }

    statusBar->render();

    // 両 OLED へ転送（I2C DMA が並行動作）
    oledTx->update();
    oledRx->update();
}
