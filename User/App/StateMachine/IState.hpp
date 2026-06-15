/*
 * IState.hpp
 *
 *      Author: picman
 */

#ifndef APP_STATEMACHINE_ISTATE_HPP_
#define APP_STATEMACHINE_ISTATE_HPP_
#include <array>
#include <optional>
#include "StateId.hpp"
#include "UpdateContext.hpp"
#include "../Event/Event.hpp"
#include "RenderContext.hpp"

namespace App {

  struct Transition {
      std::optional<StateId> nextState = std::nullopt;

      static const Transition None() {
        return {std::nullopt};
      }

      static const Transition To(StateId sid) {
        return {sid};
      }
  };

  struct ExecuteResult {

      Transition transition = Transition::None();
      bool renderRequested = false;

      static const ExecuteResult None() {
        return {Transition::None(), false};
      }

      static const ExecuteResult executed(bool renderRequested = false) {
        return {Transition::None(), renderRequested};
      }

      static const ExecuteResult transitionTo(StateId sid, bool renderRequested = false) {
        return {Transition::To(sid), renderRequested};
      }
  };

  class IState {

    public:
      IState() = default;
      virtual ~IState() = default;

      virtual void Enter() = 0;
      virtual void Exit() = 0;
      virtual ExecuteResult Update(const UpdateContext&) {
        return ExecuteResult::None();
      }
      virtual ExecuteResult HandleEvent(const Event &event) = 0;
      virtual void Render(const RenderContext &context) = 0;

    private:

  };

}

#endif /* APP_STATEMACHINE_ISTATE_HPP_ */
