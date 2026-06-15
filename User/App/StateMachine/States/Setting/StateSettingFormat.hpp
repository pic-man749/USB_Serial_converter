/*
 * StateSettingFormat.hpp
 *
 * 表示形式（HEX / ASCII）を選択する設定ステート。
 */
#ifndef APP_STATEMACHINE_STATES_SETTING_STATESETTINGFORMAT_HPP_
#define APP_STATEMACHINE_STATES_SETTING_STATESETTINGFORMAT_HPP_
#include <cstdint>
#include "../../IState.hpp"
#include "../../../AppConfig/AppConfig.hpp"

namespace App {

  class StateSettingFormat: public IState {

    public:
      explicit StateSettingFormat(AppConfig &config);
      virtual ~StateSettingFormat() = default;
      void Enter() override;
      void Exit() override;
      ExecuteResult HandleEvent(const Event &event) override;
      void Render(const RenderContext &context) override;
    private:
      static constexpr uint8_t kItemCount = 2U;
      AppConfig &config_;
      uint8_t cursorIndex_;
      void drawScreen(BinaryGFX::BinaryGFX &oled) const;
  };

} // namespace App

#endif /* APP_STATEMACHINE_STATES_SETTING_STATESETTINGFORMAT_HPP_ */
