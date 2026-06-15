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
                   StateId initialStateId);

      /**
       * @brief ステートマシン実行：現在のステートのUpdateを呼び出す。遷移先があれば遷移する。
       *
       * @param updateCtx 
       * @return true: 描画要求あり、false: 描画要求なし
       */
      bool ExecuteUpdate(const UpdateContext &updateCtx);

      /**
       * @brief ステートマシン実行：現在のステートのEventを呼び出す。遷移先があれば遷移する。
       *
       * @param event
       * @return true: 描画要求あり、false: 描画要求なし
       */
      bool ExecuteEvent(const Event &event);

      /**
       * @brief ステートマシン実行：現在のステートのRenderを呼び出す。
       *
       * @param renderContext
       */
      void ExecuteRender(const RenderContext &renderContext);

    private:
      StateArray stateArray_;
      StateId currentStateId_;

      /**
       * @brief 状態遷移
       *
       * @param newState 遷移先の状態ID
       */
      void TransitionToState(StateId newState);

      /**
       * @brief 現在のStateを取得する
       *
       * @return 現在のStateへのポインタ
       */
      IState* GetCurrentState() const;

  };

}

#endif /* APP_STATEMACHINE_STATEMACHINE_HPP_ */
