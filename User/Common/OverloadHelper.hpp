/*
 * OverloadHelper.hpp
 *
 *      Author: picman
 */

#ifndef COMMON_OVERLOADHELPER_HPP_
#define COMMON_OVERLOADHELPER_HPP_

namespace Common {

  template<class ... Ts>
  struct overload: Ts... {
    using Ts::operator()...;
  };

  template<class ... Ts>
  overload(Ts...) -> overload<Ts...>;
}

#endif /* COMMON_OVERLOADHELPER_HPP_ */
