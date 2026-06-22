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
#include "../Base/CursorMenuState.hpp"
#include "../../../AppConfig/AppConfig.hpp"
#include "../../../IConfigApplicable.hpp"

namespace App {

  class StateSettingBaudRate: public CursorMenuState {

    public:
      StateSettingBaudRate(AppConfig &config, IConfigApplicable &applicable);
      virtual ~StateSettingBaudRate() = default;
      void Enter() override;
      void Render(const RenderContext &context) override;

    protected:
      ExecuteResult HandleSelection(const ButtonEvent &event) override;

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
      void drawScreen(BinaryGFX::BinaryGFX &oled) const;
  };

} // namespace App

#endif /* APP_STATEMACHINE_STATES_SETTING_STATESETTINGBAUDRATE_HPP_ */
