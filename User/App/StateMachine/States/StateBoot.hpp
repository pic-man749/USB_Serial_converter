/*
 * StateBoot.hpp
 *
 * 起動ロゴを表示し、一定時間経過またはボタン押下で
 * StateMonitorCommunication へ遷移する起動ステート。
 */
#ifndef APP_STATEMACHINE_STATES_STATEBOOT_HPP_
#define APP_STATEMACHINE_STATES_STATEBOOT_HPP_
#include <cstdint>
#include "../IState.hpp"

namespace App {

  class StateBoot: public IState {

    public:
      StateBoot();
      virtual ~StateBoot() = default;
      void Enter(const UpdateContext &context) override;
      void Exit() override;
      ProcessResult Update(const UpdateContext &ctx) override;
      ProcessResult ProcessEvent(const Event &event) override;
      void Render(const RenderContext &context) override;

    private:
      /** ロゴ表示最短時間（ms） */
      static constexpr uint32_t kBootDisplayMs = 2000U;
      uint32_t enterTime_;
  };
} // namespace App

#endif /* APP_STATEMACHINE_STATES_STATEBOOT_HPP_ */
