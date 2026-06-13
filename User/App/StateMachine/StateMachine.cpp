/*
 * StateMachine.cpp
 *
 *      Author: picman
 */
#include "StateMachine.hpp"

namespace App {

  StateMachine::StateMachine(StateArray stateArray, StateId initialStateId) :
      stateArray_(stateArray), currentStateId_(initialStateId) {
    GetCurrentState()->Enter();
  }

  void StateMachine::Execute(const InputEvent &event, const RenderContext &renderContext) {

    const ProcessResult result = GetCurrentState()->ProcessEvent(event);

    if(result.transition.nextState.has_value()) {
      TransitionToState(result.transition.nextState.value());
    }

    if(result.renderRequested) {
      GetCurrentState()->Render(renderContext);
    }
  }

  void StateMachine::TransitionToState(StateId newState) {
    GetCurrentState()->Exit();
    currentStateId_ = newState;
    GetCurrentState()->Enter();
  }

  IState* StateMachine::GetCurrentState() const {
    return stateArray_[static_cast<size_t>(currentStateId_)];
  }

}
