/*
 * StateBoot.cpp
 *
 *      Author: picman
 */
#include "StateBoot.hpp"
#include <variant>
#include "Common/OverloadHelper.hpp"
#include "BinaryGFX.hpp"

namespace App {

  StateBoot::StateBoot() :
      IState(), enterTick_(0U) {

  }
  void StateBoot::Enter() {
    saveEnterTick_ = false;
  }

  void StateBoot::Exit() {
  }

  ExecuteResult StateBoot::Update(const UpdateContext &ctx) {
    // 初回ならTickをセーブしておく
    if(!saveEnterTick_) {
      enterTick_ = ctx.tickMs;
      saveEnterTick_ = true;
      return ExecuteResult::executed(true);
    }
    // Tickが表示時間を超えたら遷移する
    if(ctx.tickMs - enterTick_ >= kBootDisplayMs) {
      return ExecuteResult::transitionTo(StateId::MonitorCommunication);
    }
    // 念のため
    return ExecuteResult::None();
  }

  ExecuteResult StateBoot::HandleEvent(const Event &event) {
    return std::visit(Common::overload {
        [](const ButtonEvent &e) -> ExecuteResult {
          if(e.type == ButtonEventType::kPress) {
            return ExecuteResult::transitionTo(StateId::MonitorCommunication);
          }
          return ExecuteResult::None();
        },
        [](const auto&) -> ExecuteResult {
          return ExecuteResult::None();
        }
    }, event);
  }

  void StateBoot::Render(const RenderContext &context) {
    for(auto *oled : { context.LeftOled.get(), context.RightOled.get() }) {
      oled->removeAll();
      BinaryGFX::createText(*oled, 40, 24, "USB-UART");
      BinaryGFX::createText(*oled, 37, 40, "CONVERTER");
      oled->update();
    }
  }
} // namespace App
