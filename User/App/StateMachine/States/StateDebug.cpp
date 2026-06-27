/*
 * StateDebug.cpp
 *
 *      Author: picman
 */
#include "StateDebug.hpp"
#include "Common/OverloadHelper.hpp"
#include <cstdint>

namespace App {

  StateDebug::StateDebug(const AppConfig & config) :
    config_(config), isObjectCreated_(false){

  }

  void StateDebug::Enter() {
    isObjectCreated_ = false;
    encDeltaSum_ = 0;
    isPressedBtnT_ = false;
    isPressedBtnL_ = false;
    isPressedBtnR_ = false;
    isPressedBtnB_ = false;
    isPressedBtnC_ = false;
  }

  void StateDebug::Exit(){

  }

  ExecuteResult StateDebug::HandleEvent(const Event &event) {
    return std::visit(Common::overload {

      [this](const EncoderRotateEvent &e) -> ExecuteResult {
        encDeltaSum_ += e.delta;
        return ExecuteResult::executed(true);
      },

      [this](const ButtonEvent &e) -> ExecuteResult {
        // pressとlongPressはtrueにしたいけど長くなるのでReleaseで判定して反転する
        if(e.button_id == Driver::ButtonType::Top) {
          isPressedBtnT_ = !(e.type == ButtonEventType::kRelease);
        } else if(e.button_id == Driver::ButtonType::Left) {
          isPressedBtnL_ = !(e.type == ButtonEventType::kRelease);
        } else if(e.button_id == Driver::ButtonType::Right) {
          isPressedBtnR_ = !(e.type == ButtonEventType::kRelease);
        } else if(e.button_id == Driver::ButtonType::Bottom) {
          isPressedBtnB_ = !(e.type == ButtonEventType::kRelease);
        } else if(e.button_id == Driver::ButtonType::Center) {
          isPressedBtnC_ = !(e.type == ButtonEventType::kRelease);
        }

        // 左長押しでreturn
        if(e.button_id == Driver::ButtonType::Left && e.type == ButtonEventType::kLongPress) {;
          return ExecuteResult::transitionTo(StateId::Setting);
        }
        return ExecuteResult::executed(true);
      },

      [](const auto&) -> ExecuteResult {
        return ExecuteResult::None();
      }
    }, event);
  }

  ExecuteResult StateDebug::Update(const UpdateContext &context){
    auto now = context.tickMs;
    span_ = now - lastTick_;
    lastTick_ = now;
    return ExecuteResult::executed(true);
  }

  void StateDebug::Render(const RenderContext &context) {

    if(!isObjectCreated_){
      // init
      context.LeftOled->removeAll();
      context.RightOled->removeAll();
      // create
      CreateObject(context);
      context.RightOled->update();
      isObjectCreated_ = true;
    }

    // ボタン状態更新
    auto cirObj = context.LeftOled->getObjectById(oidBtnT_);
    cirObj->setFilled(isPressedBtnT_);
    cirObj = context.LeftOled->getObjectById(oidBtnL_);
    cirObj->setFilled(isPressedBtnL_);
    cirObj = context.LeftOled->getObjectById(oidBtnR_);
    cirObj->setFilled(isPressedBtnR_);
    cirObj = context.LeftOled->getObjectById(oidBtnB_);
    cirObj->setFilled(isPressedBtnB_);
    cirObj = context.LeftOled->getObjectById(oidBtnC_);
    cirObj->setFilled(isPressedBtnC_);

    // エンコーダ積分値更新
    auto strObj = context.LeftOled->getObjectById(oidEncStr_);
    static const size_t kBufSize = 16;
    char textbuf[kBufSize] = {0};
    snprintf(textbuf, kBufSize, "%ld", encDeltaSum_);
    strObj->getText().assign(textbuf);

    // FPS更新
    if(span_ != 0){
      auto fpsStrObj = context.LeftOled->getObjectById(oidFps_);
      snprintf(textbuf, kBufSize, "FPS:%lu", 1000 / span_);
      fpsStrObj->getText().assign(textbuf);
    }


    context.LeftOled->update();

  }

  void StateDebug::CreateObject(const RenderContext &context) {

    static const size_t kBufSize = 32;
    char textbuf[kBufSize] = {0};

    // RightOled : show settings
    BinaryGFX::createString(*context.RightOled, 0, 0, "SETTINGS");
    // baudrate
    snprintf(textbuf, kBufSize, "baudrate:%lu", config_.baudRate);
    BinaryGFX::createString(*context.RightOled, 5, 16, textbuf);
    // display mode
    snprintf(textbuf, kBufSize,  "display mode:%s", GetDisplayModeStr(config_.displayMode));
    BinaryGFX::createString(*context.RightOled, 5, 24, textbuf);
    // uart
    snprintf(textbuf, kBufSize,  "uart:%s", GetUartChannelStr(config_.selectedUart));
    BinaryGFX::createString(*context.RightOled, 5, 32, textbuf);

    // navi
    BinaryGFX::createString(*context.RightOled, 0, 56, "[<<]Back");

    // LeftOled : FPS & show button & encoder state
    // show FPS
    snprintf(textbuf, kBufSize,  "FPS:%lu", 1000 / span_);
    oidFps_ = BinaryGFX::createString(*context.LeftOled, 10, 8, "");

    // button
    static const uint16_t radius = 5;
    oidBtnC_ = BinaryGFX::createCircle(*context.LeftOled, 80, 32, radius);
    oidBtnL_ = BinaryGFX::createCircle(*context.LeftOled, 80-radius*3, 32, radius);
    oidBtnR_ = BinaryGFX::createCircle(*context.LeftOled, 80+radius*3, 32, radius);
    oidBtnT_ = BinaryGFX::createCircle(*context.LeftOled, 80, 32-radius*3, radius);
    oidBtnB_ = BinaryGFX::createCircle(*context.LeftOled, 80, 32+radius*3, radius);

    // encoder
    BinaryGFX::createCircle(*context.LeftOled, 40, 32, radius*2);
    snprintf(textbuf, kBufSize, "%ld", encDeltaSum_);
    oidEncStr_ = BinaryGFX::createString(*context.LeftOled, 30, 50, textbuf);

  }

}

