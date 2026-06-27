/*
 * StateDebug.hpp
 *
 *      Author: picman
 */

#ifndef APP_STATEMACHINE_STATES_STATEDEBUG_HPP_
#define APP_STATEMACHINE_STATES_STATEDEBUG_HPP_

#include <cstdint>
#include "../IState.hpp"
#include "../../AppConfig/AppConfig.hpp"
#include "BinaryGFX.hpp"

namespace App {

  class StateDebug : public IState{

    public:
      explicit StateDebug(const AppConfig &config);
      virtual ~StateDebug() = default;
      void Enter() override;
      void Exit() override;
      ExecuteResult HandleEvent(const Event &event) override;
      ExecuteResult Update(const UpdateContext &context) override;
      void Render(const RenderContext &context) override;

    private:
      const AppConfig &config_;

      bool isObjectCreated_;

      void CreateObject(const RenderContext &context);

      // obj ids
      BinaryGFX::TypedObjectId<BinaryGFX::StringObject> oidFps_;
      BinaryGFX::TypedObjectId<BinaryGFX::CircleObject> oidBtnT_;
      BinaryGFX::TypedObjectId<BinaryGFX::CircleObject> oidBtnL_;
      BinaryGFX::TypedObjectId<BinaryGFX::CircleObject> oidBtnR_;
      BinaryGFX::TypedObjectId<BinaryGFX::CircleObject> oidBtnB_;
      BinaryGFX::TypedObjectId<BinaryGFX::CircleObject> oidBtnC_;
      BinaryGFX::TypedObjectId<BinaryGFX::StringObject> oidEncStr_;

      bool isPressedBtnT_ = false;
      bool isPressedBtnL_ = false;
      bool isPressedBtnR_ = false;
      bool isPressedBtnB_ = false;
      bool isPressedBtnC_ = false;

      // encoder変化量の合計
      int32_t encDeltaSum_ = 0;

      // 前回のTick値
      uint32_t lastTick_ = 0;
      // 前回～今回までの経過Tick値
      uint32_t span_ = 0;

  };

} // namespace App

#endif /* APP_STATEMACHINE_STATES_STATEDEBUG_HPP_ */
