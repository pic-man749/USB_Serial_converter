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
#include "../InputManager/InputEvent.hpp"
#include "RenderContext.hpp"

namespace App {

  struct Transition {
    std::optional<StateId> nextState = std::nullopt;

    static const Transition None(){
      return { std::nullopt };
    }

    static const Transition To(StateId sid){
      return { sid };
    }
  };

  struct ProcessResult {
    
    Transition transition = Transition::None();
    bool renderRequested = false;

    static const ProcessResult None() {
      return { Transition::None(), false };
    }

    static const ProcessResult executed(bool renderRequested = false) {
      return { Transition::None(), renderRequested };
    }

    static const ProcessResult transitionTo(StateId sid, bool renderRequested = false) {
      return { Transition::To(sid), renderRequested };
    }
  };

  class IState {

    public:
      IState() = default;
      virtual ~IState() = default;

      virtual void Enter() = 0;
      virtual void Exit() = 0;
      virtual ProcessResult ProcessEvent(const InputEvent &event) = 0;
      virtual void Render(const RenderContext &context) = 0;

    private:

  };

}

#endif /* APP_STATEMACHINE_ISTATE_HPP_ */
