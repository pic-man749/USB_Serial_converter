/*
 * StateSettingFormat.hpp
 *
 * 表示形式（HEX / ASCII）を選択する設定ステート。
 */
#ifndef APP_STATEMACHINE_STATES_SETTING_STATESETTINGFORMAT_HPP_
#define APP_STATEMACHINE_STATES_SETTING_STATESETTINGFORMAT_HPP_
#include <cstdint>
#include "../Base/CursorMenuState.hpp"
#include "../../../AppConfig/AppConfig.hpp"

namespace App {

  class StateSettingFormat: public CursorMenuState {

    public:
      explicit StateSettingFormat(AppConfig &config);
      virtual ~StateSettingFormat() = default;
      void Enter() override;
      void Render(const RenderContext &context) override;

    protected:
      ExecuteResult HandleSelection(const ButtonEvent &event) override;

    private:
      static constexpr uint8_t kItemCount = 2U;
      AppConfig &config_;
      void drawScreen(BinaryGFX::BinaryGFX &oled) const;
  };

} // namespace App

#endif /* APP_STATEMACHINE_STATES_SETTING_STATESETTINGFORMAT_HPP_ */
