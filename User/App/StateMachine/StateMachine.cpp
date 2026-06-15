/*
 * StateMachine.cpp
 *
 *      Author: picman
 */
#include "StateMachine.hpp"

namespace App {

  StateMachine::StateMachine(StateArray stateArray,
                             StateId initialStateId) :
      stateArray_(std::move(stateArray)), currentStateId_(initialStateId) {
    GetCurrentState()->Enter();
  }

  bool StateMachine::ExecuteUpdate(const UpdateContext &updateCtx) {

    auto ret = GetCurrentState()->Update(updateCtx);

    if(ret.transition.nextState.has_value()) {
      TransitionToState(ret.transition.nextState.value());
      return true;
    }
    return ret.renderRequested;
  }

  bool StateMachine::ExecuteEvent(const Event &event) {

    auto ret = GetCurrentState()->HandleEvent(event);

    if(ret.transition.nextState.has_value()) {
      TransitionToState(ret.transition.nextState.value());
      return true;
    }
    return ret.renderRequested;
  }

  void StateMachine::ExecuteRender(const RenderContext &renderContext) {
    GetCurrentState()->Render(renderContext);
  }

  void StateMachine::TransitionToState(StateId newState) {
    GetCurrentState()->Exit();
    currentStateId_ = newState;
    GetCurrentState()->Enter();
  }

  IState* StateMachine::GetCurrentState() const {
    return stateArray_[static_cast<size_t>(currentStateId_)].get();
  }

}
