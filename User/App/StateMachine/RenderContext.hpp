/*
 * RenderContext.hpp
 *
 *      Author: picman
 */

#ifndef APP_STATEMACHINE_RENDERCONTEXT_HPP_
#define APP_STATEMACHINE_RENDERCONTEXT_HPP_

#include "BinaryGFX.hpp"

namespace App {

  struct RenderContext {
    std::unique_ptr<BinaryGFX::BinaryGFX>& RightOled;
    std::unique_ptr<BinaryGFX::BinaryGFX>& LeftOled;
  };

}

#endif /* APP_STATEMACHINE_RENDERCONTEXT_HPP_ */
