/*
 * StateMachine.hpp
 *
 *      Author: picman
 */

#ifndef APP_STATEMACHINE_STATEMACHINE_HPP_
#define APP_STATEMACHINE_STATEMACHINE_HPP_

#include "IState.hpp"
#include <array>
#include "StateId.hpp"

namespace App {

  class StateMachine {
    public:
      using StateArray = std::array<IState*, static_cast<size_t>(StateId::Count)>;

      StateMachine(StateArray stateArray, StateId initialStateId);

      void Execute(const Event &event, const RenderContext &renderContext);

    private:
      std::array<IState*, static_cast<size_t>(StateId::Count)> stateArray_;
      StateId currentStateId_;

      void TransitionToState(StateId newState);

      IState* GetCurrentState() const;

  };

}

#endif /* APP_STATEMACHINE_STATEMACHINE_HPP_ */
