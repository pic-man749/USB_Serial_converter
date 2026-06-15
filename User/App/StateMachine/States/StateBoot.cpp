/*
 * StateBoot.cpp
 *
 *      Author: picman
 */
#include "StateBoot.hpp"
#include <variant>
#include <memory>
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
    if(!saveEnterTick_){
      enterTick_ = ctx.tickMs;
    }
    // Tickが表示時間を超えたら遷移する
    if(ctx.tickMs - enterTick_ >= kBootDisplayMs) {
      return ExecuteResult::transitionTo(StateId::MonitorCommunication);
    }
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
      auto title = std::make_unique<BinaryGFX::TextObject>(20, 24, "USB-UART",
          &BinaryGFX::BgfxFont_Ascii);
      title->setCharSpacing(1U);
      oled->addObject(std::move(title));
      auto sub = std::make_unique<BinaryGFX::TextObject>(13, 40, "CONVERTER",
          &BinaryGFX::BgfxFont_Ascii);
      sub->setCharSpacing(1U);
      oled->addObject(std::move(sub));
      oled->update();
    }
  }
} // namespace App
