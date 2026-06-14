/*
 * UserMain.cpp
 *
 * アプリケーション全体のセットアップとメインループを担うエントリポイント。
 */
#include "UserMain.hpp"
#include <memory>
// アプリケーション層
#include "App/AppConfig/AppConfig.hpp"
#include "App/Event/EventBus.hpp"
#include "App/InputManager/InputManager.hpp"
#include "App/CommunicationManager/CommunicationManager.hpp"
#include "App/StateMachine/StateMachine.hpp"
#include "App/StateMachine/RenderContext.hpp"
#include "App/StateMachine/States/StateBoot.hpp"
#include "App/StateMachine/States/StateMonitorCommunication.hpp"
#include "App/StateMachine/States/StateSetting.hpp"
#include "App/StateMachine/States/Setting/StateSettingUart.hpp"
#include "App/StateMachine/States/Setting/StateSettingBaudRate.hpp"
#include "App/StateMachine/States/Setting/StateSettingBaudRateCustom.hpp"
#include "App/StateMachine/States/Setting/StateSettingFormat.hpp"
// ドライバ層
#include "Driver/Uart/Uart.hpp"
#include "Driver/USB_CDC/UsbCdc.hpp"
// BinaryGFX
#include "BinaryGFX.hpp"
// ドライバ共通
#include "Driver/Time/Time.hpp"
// STM32 HAL ハンドル
#include "usart.h"
#include "i2c.h"

namespace {
  // アプリケーション設定
  App::AppConfig g_config;

  // UART ドライバ（HAL 初期化後に setup() で生成する）
  std::unique_ptr<Driver::Uart> g_uart1;
  std::unique_ptr<Driver::Uart> g_uart2;
  std::unique_ptr<Driver::Uart> g_uart3;

  // CommunicationManager
  std::unique_ptr<App::CommunicationManager> g_commMgr;

  // OLED ディスプレイ（左: UART→USB、右: USB→UART）
  std::unique_ptr<BinaryGFX::BinaryGFX> g_leftOled;
  std::unique_ptr<BinaryGFX::BinaryGFX> g_rightOled;

  // ステートマシン
  std::unique_ptr<App::StateMachine> g_stateMachine;

  // 入力マネージャ（HAL 初期化後に setup() で生成する）
  std::unique_ptr<App::InputManager> g_inputManager;

  // イベントバス
  std::unique_ptr<App::EventBus> g_eventBus;

  // SSD1306 I2C デバイスアドレス（7ビット値）, その他定数
  static constexpr uint8_t kOledAddr = 0x3CU;
  static constexpr uint16_t kOledWidth = 128U;
  static constexpr uint16_t kOledHeight = 64U;
  static constexpr uint32_t kI2cTimeout = 100U;

  // UART DMA 受信バッファサイズ */
  static constexpr uint32_t kUartBufSize = 256U;

  // StateId とステートインスタンスの対応を明示的に設定する
  App::StateMachine::StateArray buildStateArray() {
    // StateId が追加された場合にコンパイルエラーで buildStateArray() の更新漏れを検出する
    static_assert(static_cast<size_t>(App::StateId::Count) == 7U,
        "StateId が追加されました。buildStateArray() を更新してください。");

    App::StateMachine::StateArray arr { };

    arr[static_cast<size_t>(App::StateId::Boot)] =
        std::make_unique<App::StateBoot>();
    arr[static_cast<size_t>(App::StateId::MonitorCommunication)] =
        std::make_unique<App::StateMonitorCommunication>(*g_commMgr, g_config);
    arr[static_cast<size_t>(App::StateId::Setting)] =
        std::make_unique<App::StateSetting>(g_config);
    arr[static_cast<size_t>(App::StateId::SettingUart)] =
        std::make_unique<App::StateSettingUart>(g_config, *g_commMgr);
    arr[static_cast<size_t>(App::StateId::SettingBaudRate)] =
        std::make_unique<App::StateSettingBaudRate>(g_config, *g_commMgr);
    arr[static_cast<size_t>(App::StateId::SettingBaudRateCustom)] =
        std::make_unique<App::StateSettingBaudRateCustom>(g_config, *g_commMgr);
    arr[static_cast<size_t>(App::StateId::SettingFormat)] =
        std::make_unique<App::StateSettingFormat>(g_config);

    return arr;
  }
} // namespace
// ----------------------------------------------------------------------------
void setup(void) {
  // 1. UART ドライバを生成する
  g_uart1 = std::make_unique<Driver::Uart>(&huart1, kUartBufSize);
  g_uart2 = std::make_unique<Driver::Uart>(&huart2, kUartBufSize);
  g_uart3 = std::make_unique<Driver::Uart>(&huart6, kUartBufSize);

  // 2. CommunicationManager を生成する
  g_commMgr = std::make_unique<App::CommunicationManager>(*g_uart1, *g_uart2, *g_uart3,
      Driver::UsbCdc::getInstance(), g_config.selectedUart, g_config.baudRate);

  // 3. OLED ディスプレイを初期化する
  //    左 OLED: I2C2 / 右 OLED: I2C1
  {
    auto leftHal = std::make_unique<BinaryGFX::Hal::Stm32I2cDma>(&hi2c2, kI2cTimeout);
    auto leftDriver = std::make_unique<BinaryGFX::Driver::Ssd1306Driver>(std::move(leftHal),
        kOledAddr, kOledWidth, kOledHeight);
    g_leftOled = std::make_unique<BinaryGFX::BinaryGFX>(std::move(leftDriver));
    g_leftOled->init();

    auto rightHal = std::make_unique<BinaryGFX::Hal::Stm32I2cDma>(&hi2c1, kI2cTimeout);
    auto rightDriver = std::make_unique<BinaryGFX::Driver::Ssd1306Driver>(std::move(rightHal),
        kOledAddr, kOledWidth, kOledHeight);
    g_rightOled = std::make_unique<BinaryGFX::BinaryGFX>(std::move(rightDriver));
    g_rightOled->init();
  }

  // 4. Inputマネージャを生成・初期化する
  g_inputManager = std::make_unique<App::InputManager>();
  g_inputManager->init();

  // 5. EventBus を生成し、入力とCommunicationManagerをソースとして登録する
  //    登録順がイベントのキュー投入順（= 優先度）になる
  g_eventBus = std::make_unique<App::EventBus>(
      std::initializer_list<App::IEventSource*> { g_inputManager.get(), g_commMgr.get() });

  // 6. StateMachine を生成して初期ステートの Enter()/Render() を実行する
  //    buildStateArray() でステートの生成と StateMachine への所有権移譲を行う
  g_stateMachine = std::make_unique<App::StateMachine>(
      buildStateArray(),
      App::StateId::Boot,
      App::UpdateContext { Driver::GetTick() },
      App::RenderContext { g_rightOled, g_leftOled });
}

void loop(void) {
  // 全イベントソースを更新し、発生したイベントをキューに積む
  g_eventBus->Update();
  // ステートマシンを実行する
  App::UpdateContext uctx { Driver::GetTick() };
  App::RenderContext rctx { g_rightOled, g_leftOled };
  g_stateMachine->Execute(uctx, g_eventBus->getQueue(), rctx);
}
