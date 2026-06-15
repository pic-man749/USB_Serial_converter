/*
 * UpdateContext.hpp
 *
 * 毎ループ IState::Update() に渡すコンテキスト。
 */
#ifndef APP_STATEMACHINE_UPDATECONTEXT_HPP_
#define APP_STATEMACHINE_UPDATECONTEXT_HPP_

#include <cstdint>

namespace App {

  struct UpdateContext {
      /** 現在のTick値[ms] */
      uint32_t tickMs;
  };

} // namespace App

#endif /* APP_STATEMACHINE_UPDATECONTEXT_HPP_ */
