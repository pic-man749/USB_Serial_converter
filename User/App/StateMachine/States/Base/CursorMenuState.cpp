/*
 * CursorMenuState.cpp
 *
 *      Author: picman
 */
#include "CursorMenuState.hpp"
#include <variant>
#include <algorithm>
#include "Common/OverloadHelper.hpp"

namespace App {

  CursorMenuState::CursorMenuState(uint8_t itemCount) :
      IState(), cursorIndex_(0U), itemCount_(itemCount) {

  }

  void CursorMenuState::Exit() {
  }

  void CursorMenuState::SetCursorIndex(uint8_t index) {
    cursorIndex_ = std::min(index, static_cast<uint8_t>(itemCount_ - 1U));
  }

  void CursorMenuState::MoveCursor(int32_t delta) {
    const int32_t next = static_cast<int32_t>(cursorIndex_) + delta;
    cursorIndex_ = static_cast<uint8_t>(
        std::max(static_cast<int32_t>(0), std::min(static_cast<int32_t>(itemCount_ - 1U), next)));
  }

  ExecuteResult CursorMenuState::HandleEvent(const Event &event) {
    return std::visit(Common::overload {
        [](const NoneEvent&) -> ExecuteResult {
          return ExecuteResult::None();
        },
        [this](const EncoderRotateEvent &e) -> ExecuteResult {
          MoveCursor(e.delta);
          return ExecuteResult::executed(true);
        },
        [this](const ButtonEvent &e) -> ExecuteResult {
          if(e.type != ButtonEventType::kPress) {
            return ExecuteResult::None();
          }
          if(e.button_id == Driver::ButtonType::Top) {
            MoveCursor(-1);
            return ExecuteResult::executed(true);
          }
          if(e.button_id == Driver::ButtonType::Bottom) {
            MoveCursor(1);
            return ExecuteResult::executed(true);
          }
          return HandleSelection(e);
        },
        [](const auto&) -> ExecuteResult {
          return ExecuteResult::None();
        }
    }, event);
  }

} // namespace App
