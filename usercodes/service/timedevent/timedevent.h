/**
 * @file timedevent.h
 * @brief 指定時間後に一度だけイベントを発行する
 */

#ifndef __TIMEDEVENT_H__
#define __TIMEDEVENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <orgtypedef.h>
#include "../eventmn/event_manager.h"
#include "../systime/systime.h"

/**
 * @brief タイマー設定を識別するID
 */
typedef uint16_t timedevent_id_t;

/**
 * @brief タイマー管理を初期化する
 */
void timedevent_Init(void);

/**
 * @brief 指定した遅延時間後にスケジュールイベントを一度だけ発行する
 * @param delay_ms 登録時点からの遅延時間[ms]。0は指定不可
 * @param timer_id 登録したタイマーIDの出力先
 * @return true 空き枠へ登録できた場合
 * @return false 引数不正の場合、または満杯時に最古の設定を破棄して登録した場合
 * @details falseの場合も新しいタイマーは登録されるが、古いタイマーが1件破棄されたことを示す。
 *          発行されるイベントタイプはEVENT_MANAGER_ETYPE_SCHEDULEである。
 */
bool timedevent_Register(uint32_t delay_ms, timedevent_id_t* timer_id);

/**
 * @brief 登録済みタイマーを期限到達判定する
 * @details 1ms周期の割り込みから呼び出す。期限到達後は登録を無効化して一度だけ発行する。
 */
void timedevent_1ms(void);

/**
 * @brief 登録中のタイマー数を取得する
 * @return 登録中のタイマー数
 */
uint8_t timedevent_BufferSize(void);


#if (SERVICE_DEBUGMODE == DEBUGMODE_TIMEDEVENT_TEST)
/**
 * @brief タイマー機能テスト用の関数
 * 
 * @param strBuffer 送信用データのバッファポインタ
 * @param maxBufferSize 最大バッファサイズ
 * @return uint8_t 戻り値(0)
 */
uint8_t timedevent_test(char* strBuffer, uint8_t maxBufferSize);
#endif /* SERVICE_DEBUGMODE == DEBUGMODE_TIMEDEVENT_TEST */

#ifdef __cplusplus
}
#endif

#endif /* __TIMEDEVENT_H__ */
