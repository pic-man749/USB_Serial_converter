/*
 * StateId.hpp
 *
 *      Author: picman
 */

#ifndef APP_STATEMACHINE_STATEID_HPP_
#define APP_STATEMACHINE_STATEID_HPP_

namespace App {
  enum class StateId {
    Boot = 0,
    MonitorCommunication,
    Setting,
    SettingUart,
    SettingBaudRate,
    SettingBaudRateCustom,
    SettingFormat,
    Debug,
    Count
  };
}

#endif /* APP_STATEMACHINE_STATEID_HPP_ */
