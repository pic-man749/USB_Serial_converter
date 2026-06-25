/*
 * StateDebug.cpp
 *
 *      Author: picman
 */
#include "StateDebug.hpp"
#include "Common/OverloadHelper.hpp"
#include <memory>
#include <string>
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
    auto cirObj = context.LeftOled->getObjectById<BinaryGFX::CircleObject>(oidBtnT_);
    cirObj->setFilled(isPressedBtnT_);
    cirObj = context.LeftOled->getObjectById<BinaryGFX::CircleObject>(oidBtnL_);
    cirObj->setFilled(isPressedBtnL_);
    cirObj = context.LeftOled->getObjectById<BinaryGFX::CircleObject>(oidBtnR_);
    cirObj->setFilled(isPressedBtnR_);
    cirObj = context.LeftOled->getObjectById<BinaryGFX::CircleObject>(oidBtnB_);
    cirObj->setFilled(isPressedBtnB_);
    cirObj = context.LeftOled->getObjectById<BinaryGFX::CircleObject>(oidBtnC_);
    cirObj->setFilled(isPressedBtnC_);

    // エンコーダ積分値更新
    auto strObj = context.LeftOled->getObjectById<BinaryGFX::StringObject>(oidEncStr_);
    static const size_t kBufSize = 16;
    char textbuf[kBufSize] = {0};
    snprintf(textbuf, kBufSize, "%ld", encDeltaSum_);
    strObj->getText().assign(textbuf);

    context.LeftOled->update();

  }

  void StateDebug::CreateObject(const RenderContext &context) {

    // string object creator
    auto CreateStrObj = [](BinaryGFX::BinaryGFX &oled, int16_t x, int16_t y, const char *text) -> BinaryGFX::ObjectId{
      std::string str(text);
      auto obj = std::make_unique<BinaryGFX::StringObject>(x, y, str, &BinaryGFX::BgfxFont_Ascii);
      return oled.addObject(std::move(obj));
    };
    static const size_t kBufSize = 32;
    char textbuf[kBufSize] = {0};

    // RightOled : show settings
    CreateStrObj(*context.RightOled, 0, 0, "SETTINGS");
    // baudrate
    snprintf(textbuf, kBufSize, "baudrate:%lu", config_.baudRate);
    CreateStrObj(*context.RightOled, 5, 16, textbuf);
    // display mode
    snprintf(textbuf, kBufSize,  "display mode:%s", GetDisplayModeStr(config_.displayMode));
    CreateStrObj(*context.RightOled, 5, 24, textbuf);
    // uart
    snprintf(textbuf, kBufSize,  "uart:%s", GetUartChannelStr(config_.selectedUart));
    CreateStrObj(*context.RightOled, 5, 32, textbuf);

    // navi
    CreateStrObj(*context.RightOled, 0, 56, "[<<]Back");

    // LeftOled : show button & encoder state
    // button
    static const uint16_t radius = 5;
    oidBtnC_ = context.LeftOled->addObject(std::make_unique<BinaryGFX::CircleObject>(80, 32, radius));
    oidBtnL_ = context.LeftOled->addObject(std::make_unique<BinaryGFX::CircleObject>(80-radius*3, 32, radius));
    oidBtnR_ = context.LeftOled->addObject(std::make_unique<BinaryGFX::CircleObject>(80+radius*3, 32, radius));
    oidBtnT_ = context.LeftOled->addObject(std::make_unique<BinaryGFX::CircleObject>(80, 32-radius*3, radius));
    oidBtnB_ = context.LeftOled->addObject(std::make_unique<BinaryGFX::CircleObject>(80, 32+radius*3, radius));

    // encoder
    context.LeftOled->addObject(std::make_unique<BinaryGFX::CircleObject>(40, 32, radius*2));
    snprintf(textbuf, kBufSize, "%ld", encDeltaSum_);
    oidEncStr_ = CreateStrObj(*context.LeftOled, 30, 50, textbuf);

  }

}

