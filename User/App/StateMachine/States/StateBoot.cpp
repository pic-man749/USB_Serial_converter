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
      IState(), enterTime_(0U) {

  }
  void StateBoot::Enter(const UpdateContext &context) {
    enterTime_ = context.tickMs;
  }

  void StateBoot::Exit() {
  }

  ProcessResult StateBoot::Update(const UpdateContext &ctx) {
    if(ctx.tickMs - enterTime_ >= kBootDisplayMs) {
      return ProcessResult::transitionTo(StateId::MonitorCommunication);
    }
    return ProcessResult::None();
  }

  ProcessResult StateBoot::ProcessEvent(const Event &event) {
    return std::visit(Common::overload {
        [](const ButtonEvent &e) -> ProcessResult {
          if(e.type == ButtonEventType::kPress) {
            return ProcessResult::transitionTo(StateId::MonitorCommunication);
          }
          return ProcessResult::None();
        },
        [](const auto&) -> ProcessResult {
          return ProcessResult::None();
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
