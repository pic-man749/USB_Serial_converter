/*
 * StateSettingBaudRate.hpp
 *
 * ボーレートをプリセット一覧または任意値入力から選択する設定ステート。
 * プリセット: 9600 / 38400 / 115200 / 921600 / 3000000
 * 末尾の Custom... を選ぶと StateSettingBaudRateCustom へ遷移する。
 */
#ifndef APP_STATEMACHINE_STATES_SETTING_STATESETTINGBAUDRATE_HPP_
#define APP_STATEMACHINE_STATES_SETTING_STATESETTINGBAUDRATE_HPP_
#include <cstdint>
#include <array>
#include "../../IState.hpp"
#include "../../../AppConfig/AppConfig.hpp"
#include "../../../IConfigApplicable.hpp"

namespace App {

  class StateSettingBaudRate: public IState {

    public:
      StateSettingBaudRate(AppConfig &config, IConfigApplicable &applicable);
      virtual ~StateSettingBaudRate() = default;
      void Enter(const UpdateContext &context) override;
      void Exit() override;
      ProcessResult ProcessEvent(const Event &event) override;
      void Render(const RenderContext &context) override;

    private:
      /** プリセット数（Custom... を除く） */
      static constexpr uint8_t kPresetCount = 5U;
      /** 全選択肢数（プリセット + Custom...） */
      static constexpr uint8_t kItemCount = kPresetCount + 1U;
      /** Custom... のインデックス */
      static constexpr uint8_t kCustomIndex = kPresetCount;
      static const std::array<uint32_t, kPresetCount> kPresets;
      AppConfig &config_;
      IConfigApplicable &applicable_;
      uint8_t cursorIndex_;
      void drawScreen(BinaryGFX::BinaryGFX &oled) const;
  };

} // namespace App

#endif /* APP_STATEMACHINE_STATES_SETTING_STATESETTINGBAUDRATE_HPP_ */
