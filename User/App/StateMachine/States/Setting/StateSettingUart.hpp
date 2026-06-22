/*
 * StateSettingUart.hpp
 *
 * UART チャンネル（UART1 / UART2 / UART3）を選択する設定ステート。
 */
#ifndef APP_STATEMACHINE_STATES_SETTING_STATESETTINGUART_HPP_
#define APP_STATEMACHINE_STATES_SETTING_STATESETTINGUART_HPP_
#include <cstdint>
#include "../Base/CursorMenuState.hpp"
#include "../../../AppConfig/AppConfig.hpp"
#include "../../../IConfigApplicable.hpp"

namespace App {

  class StateSettingUart: public CursorMenuState {

    public:
      StateSettingUart(AppConfig &config, IConfigApplicable &applicable);
      virtual ~StateSettingUart() = default;
      void Enter() override;
      void Render(const RenderContext &context) override;

    protected:
      ExecuteResult HandleSelection(const ButtonEvent &event) override;

    private:
      static constexpr uint8_t kItemCount = 3U;
      AppConfig &config_;
      IConfigApplicable &applicable_;
      void drawScreen(BinaryGFX::BinaryGFX &oled) const;
  };
} // namespace App

#endif /* APP_STATEMACHINE_STATES_SETTING_STATESETTINGUART_HPP_ */
