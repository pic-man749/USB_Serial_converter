/*
 * StateMachine.cpp
 *
 *      Author: picman
 */
#include "StateMachine.hpp"

namespace App {

  StateMachine::StateMachine(StateArray stateArray,
                             StateId initialStateId,
                             const UpdateContext &updateCtx,
                             const RenderContext &renderCtx) :
      stateArray_(std::move(stateArray)), currentStateId_(initialStateId) {
    GetCurrentState()->Enter(updateCtx);
    GetCurrentState()->Render(renderCtx);
  }

  void StateMachine::Execute(const UpdateContext &updateCtx,
                             EventQueue &queue,
                             const RenderContext &renderContext) {
    // Updateを実行して、状態遷移があれば遷移する
    if(handleResult(GetCurrentState()->Update(updateCtx), updateCtx, renderContext)) {
      queue.clear();
      return;
    }
    // イベントキューを処理する
    while(!queue.empty()) {
      if(handleResult(GetCurrentState()->ProcessEvent(*queue.pop()), updateCtx, renderContext)) {
        queue.clear();
        return;
      }
    }
  }

  bool StateMachine::handleResult(const ProcessResult &result,
                                  const UpdateContext &updateCtx,
                                  const RenderContext &renderContext) {
    if(result.transition.nextState.has_value()) {
      TransitionToState(result.transition.nextState.value(), updateCtx, renderContext);
      return true;
    }
    if(result.renderRequested) {
      GetCurrentState()->Render(renderContext);
    }
    return false;
  }

  void StateMachine::TransitionToState(StateId newState,
                                       const UpdateContext &updateCtx,
                                       const RenderContext &renderContext) {
    GetCurrentState()->Exit();
    currentStateId_ = newState;
    GetCurrentState()->Enter(updateCtx);
    GetCurrentState()->Render(renderContext);
  }

  IState* StateMachine::GetCurrentState() const {
    return stateArray_[static_cast<size_t>(currentStateId_)].get();
  }

}
