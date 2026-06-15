/*
 * StateSetting.hpp
 *
 * UART チャンネル・ボーレート・表示形式を一覧表示する設定トップメニュー。
 * 右 OLED にメニューを表示し、左 OLED はクリアする。
 */
#ifndef APP_STATEMACHINE_STATES_STATESETTING_HPP_
#define APP_STATEMACHINE_STATES_STATESETTING_HPP_
#include <cstdint>
#include "../IState.hpp"
#include "../../AppConfig/AppConfig.hpp"

namespace App {

  class StateSetting: public IState {

    public:
      explicit StateSetting(const AppConfig &config);
      virtual ~StateSetting() = default;
      void Enter() override;
      void Exit() override;
      ExecuteResult HandleEvent(const Event &event) override;
      void Render(const RenderContext &context) override;

    private:
      /** メニュー項目数 */
      static constexpr uint8_t kItemCount = 3U;
      const AppConfig &config_;
      /** 選択中の項目インデックス（0: UART / 1: BaudRate / 2: Format） */
      uint8_t cursorIndex_;
      /** @brief 右 OLED にメニューを描画する */
      void drawMenu(BinaryGFX::BinaryGFX &oled) const;
  };

} // namespace App

#endif /* APP_STATEMACHINE_STATES_STATESETTING_HPP_ */
