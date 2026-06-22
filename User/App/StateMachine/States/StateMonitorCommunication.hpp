/*
 * StateMonitorCommunication.hpp
 *
 * USB-UART 通信のパススルーを監視し、左右 OLED に通信内容を表示するステート。
 * 左 OLED: UART→USB 方向、右 OLED: USB→UART 方向。
 */
#ifndef APP_STATEMACHINE_STATES_STATEMONITORCOMMUNICATION_HPP_
#define APP_STATEMACHINE_STATES_STATEMONITORCOMMUNICATION_HPP_
#include <cstdint>
#include "../IState.hpp"
#include "../../AppConfig/AppConfig.hpp"
#include "../../CommunicationManager/CommunicationManager.hpp"

namespace App {

  class StateMonitorCommunication: public IState {

    public:
      StateMonitorCommunication(CommunicationManager &commMgr, const AppConfig &config);
      virtual ~StateMonitorCommunication() = default;
      void Enter() override;
      void Exit() override;
      ExecuteResult HandleEvent(const Event &event) override;
      void Render(const RenderContext &context) override;

    private:
      /** データ表示行数（ヘッダ行を除く） */
      static constexpr uint8_t kDataRows = 7U;
      /** HEX モード: 1行あたりのバイト数 */
      static constexpr uint8_t kBytesPerHexRow = 7U;
      /** ASCII モード: 1行あたりの文字数 */
      static constexpr uint8_t kCharsPerAsciiRow = 21U;
      /** フォント高さ（ピクセル） */
      static constexpr uint8_t kFontHeight = 8U;
      /** 1行の最大文字数（null 終端を除く） */
      static constexpr uint8_t kMaxLineLen = 21U;
      CommunicationManager &commMgr_;
      const AppConfig &config_;
      /** スクロールオフセット（0 = 最新データ、正方向 = 過去方向） */
      int32_t scrollOffset_;
      /** 最大スクロール数 */
      int32_t maxScroll_;
      /** 左画面ヘッダ */
      char headerLeft_[22];
      /** 右画面ヘッダ */
      char headerRight_[22];

      /**
       * @brief OLED 1面分を描画する
       * @param oled   描画対象の BinaryGFX インスタンス
       * @param buf    表示対象の DisplayBuffer
       * @param header ヘッダ行文字列
       */
      void renderOled(BinaryGFX::BinaryGFX &oled, const DisplayBuffer &buf, const char *header);
      /** @brief HEX 形式の1行文字列を生成する */
      static void buildHexLine(char *out, const DisplayBuffer &buf, size_t byteIndex);
      /**
       * @brief ASCII 形式の1行文字列を生成する。LF/CR で行終端とする。
       */
      static void buildAsciiLine(char *out, const DisplayBuffer &buf, size_t byteIndex);
      /** @brief 4ビット値を HEX 文字に変換する */
      static char nibbleToHex(uint8_t n);
      /**
       * @brief ASCII モードでの総行数を返す。
       *        LF で改行、kCharsPerAsciiRow 文字で折り返しとして計算する。
       */
      static size_t countAsciiRows(const DisplayBuffer &buf);
      /**
       * @brief ASCII モードで rowIndex 行目の開始バイトインデックスを返す。
       */
      static size_t getAsciiRowStart(const DisplayBuffer &buf, size_t rowIndex);
  };

} // namespace App

#endif /* APP_STATEMACHINE_STATES_STATEMONITORCOMMUNICATION_HPP_ */
