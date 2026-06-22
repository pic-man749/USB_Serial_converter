/*
 * CursorMenuState.hpp
 *
 * エンコーダ回転および上下ボタン操作でカーソルを移動する、
 * 一覧選択系ステートの共通基底クラス。
 */
#ifndef APP_STATEMACHINE_STATES_BASE_CURSORMENUSTATE_HPP_
#define APP_STATEMACHINE_STATES_BASE_CURSORMENUSTATE_HPP_
#include <cstdint>
#include "../../IState.hpp"

namespace App {

  class CursorMenuState: public IState {

    public:
      void Exit() override;
      ExecuteResult HandleEvent(const Event &event) final;

    protected:
      explicit CursorMenuState(uint8_t itemCount);
      virtual ~CursorMenuState() = default;

      /** @brief Top/Bottom・エンコーダ以外のボタン押下（Center/Left等）に対する派生クラス固有の処理 */
      virtual ExecuteResult HandleSelection(const ButtonEvent &event) = 0;

      /** @brief カーソル位置を [0, itemCount-1] にクランプして設定する */
      void SetCursorIndex(uint8_t index);

      uint8_t cursorIndex_;

    private:
      void MoveCursor(int32_t delta);

      const uint8_t itemCount_;
  };

} // namespace App

#endif /* APP_STATEMACHINE_STATES_BASE_CURSORMENUSTATE_HPP_ */
