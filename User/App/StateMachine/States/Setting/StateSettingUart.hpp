/*
 * StateSettingUart.hpp
 *
 * UART チャンネル（UART1 / UART2 / UART3）を選択する設定ステート。
 */
#ifndef APP_STATEMACHINE_STATES_SETTING_STATESETTINGUART_HPP_
#define APP_STATEMACHINE_STATES_SETTING_STATESETTINGUART_HPP_
#include <cstdint>
#include "../../IState.hpp"
#include "../../../AppConfig/AppConfig.hpp"
#include "../../../IConfigApplicable.hpp"

namespace App {

  class StateSettingUart: public IState {

    public:
      StateSettingUart(AppConfig &config, IConfigApplicable &applicable);
      virtual ~StateSettingUart() = default;
      void Enter() override;
      void Exit() override;
      ExecuteResult HandleEvent(const Event &event) override;
      void Render(const RenderContext &context) override;

    private:
      static constexpr uint8_t kItemCount = 3U;
      AppConfig &config_;
      IConfigApplicable &applicable_;
      uint8_t cursorIndex_;
      void drawScreen(BinaryGFX::BinaryGFX &oled) const;
  };
} // namespace App

#endif /* APP_STATEMACHINE_STATES_SETTING_STATESETTINGUART_HPP_ */
