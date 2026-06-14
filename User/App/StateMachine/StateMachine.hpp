/*
 * StateMachine.hpp
 *
 *      Author: picman
 */

#ifndef APP_STATEMACHINE_STATEMACHINE_HPP_
#define APP_STATEMACHINE_STATEMACHINE_HPP_

#include "IState.hpp"
#include <array>
#include <memory>
#include "StateId.hpp"
#include "../Event/EventQueue.hpp"

namespace App {

  class StateMachine {
    public:
      using StateArray = std::array<std::unique_ptr<IState>, static_cast<size_t>(StateId::Count)>;

      StateMachine(StateArray stateArray,
                   StateId initialStateId,
                   const UpdateContext &updateCtx,
                   const RenderContext &renderCtx);

      void Execute(const UpdateContext &updateCtx,
                   EventQueue &queue,
                   const RenderContext &renderContext);

    private:
      StateArray stateArray_;
      StateId currentStateId_;

      void TransitionToState(StateId newState,
                             const UpdateContext &updateCtx,
                             const RenderContext &renderContext);

      // result を処理し、遷移が発生した場合は true を返す。描画が必要な場合は Render() を呼ぶ。
      bool handleResult(const ProcessResult &result,
                        const UpdateContext &updateCtx,
                        const RenderContext &renderContext);

      IState* GetCurrentState() const;

  };

}

#endif /* APP_STATEMACHINE_STATEMACHINE_HPP_ */
