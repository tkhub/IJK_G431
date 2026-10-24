/**
 * @file event_manager.h
 * @brief イベントの発生と取得を管理する
 * @author takap
 * @date Nov 30, 2025
 * @Version 0.00
 */

#ifndef __EVENT_MANAGER_H__
#define __EVENT_MANAGER_H__

#include "eventmn/event_manager.h"
#ifdef __cplusplus
extern "C" {
#endif

/*========VVVV Include Standard Header START VVVV============================*/
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "event_manager_etyp.h"
#include "../service/systime/systime.h"
#include <orgtypedef.h>
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Declaration START VVVV========================*/
/*========AAAA GLOBAL Variable Declaration END AAAA==========================*/

/*========VVVV GLOBAL Function Declaration START VVVV========================*/

/**
 * @brief eventマネージャ初期化関数
 */
void event_manager_Init(void);

/**
 * @brief バッファへのイベント格納
 * 
 * @param et イベントタイプ
 * @param [in] message メッセージ（オプション：message_sizeがゼロなら読まない）領域へのポインタ
 * @param message_size メッセージサイズ
 * @param source_id イベント発行元を識別するID
 * @return true バッファ格納成功
 * @return false バッファ格納失敗。古いイベントから消去された。
 */
bool event_manager_Enque(EVENT_MANAGER_ETYPE_t et, uint16_t source_id,
						 uint8_t* message, uint8_t message_size);

/**
 * @brief バッファからのイベント取り出し
 * 
 * @param [out] et バッファの先頭に格納されているイベントタイプ(バッファが空の場合、EVENT_MANAGER_ETYPE_NONEVENTを格納する)
 * @param [out] time イベント発生時刻を返すポインタ
 * @param [out] message イベントに付与されるメッセージポインタを返す領域
 * @param [out] source_id イベント発行元を返すポインタ
 * @param [out] message_size イベントに付与されているメッセージの格納サイズ(メッセージが付与されない場合は)
 * @return true バッファ読み出し成功
 * @return false バッファ読み出し失敗。バッファにイベントが無かった。
 */
bool event_manager_Deque(EVENT_MANAGER_ETYPE_t* et, systime_t* time,
						 uint16_t* source_id, uint8_t** message,
						 uint8_t* message_size);

/**
 * @brief バッファから任意のイベントタイプを探して取り出す
 * 
 * @param et 探す対象のイベントタイプ
 * @param [out] time イベント発生時刻を返すポインタ
 * @param [out] message イベントに付与されるメッセージポインタを返す領域
 * @param [out] message_size イベントに付与されているメッセージの格納サイズ(メッセージが付与されない場合は)
 * @return true バッファ読み出し成功
 * @return false バッファ読み出し失敗。バッファに対象のイベントが無かった。
 * @details バッファからイベントタイプを探して取り出す。複数対象が存在する場合、古い順から最初に見つけたものを取り出す。取り出した後はバッファから消去される
 */
bool event_manager_FindAndRemove(EVENT_MANAGER_ETYPE_t et, systime_t* time, uint8_t** message, uint8_t* message_size);

/**
 * @brief バッファに格納されているイベントサイズを返す
 * 
 * @return uint8_t バッファに格納されているイベントサイズ
 */
uint8_t event_manager_BufferSize(void);

/**
 * @brief バッファ内にイベントがあるか探す
 * 
 * @param et 対象イベントタイプ
 * @return uint8_t 発見個数
 */
uint8_t event_manager_Find(EVENT_MANAGER_ETYPE_t et);

#if (SERVICE_DEBUGMODE == DEBUGMODE_EVENTQUEUE_TEST)
/**
 * @brief イベントテスト用関数
 * 
 * @param [out] strBuffer printfデバッグ用の文字列格納先
 * @param [in] maxBufferSize 文字列の最大格納数
 * @return 戻り値はゼロ
 */
uint8_t event_test(char* strBuffer, uint8_t maxBufferSize);
#endif /* SERVICE_DEBUGMODE == DEBUGMODE_EVENTQUEUE_TEST */

/*========AAAA GLOBAL Function Declaration END AAAA==========================*/

#ifdef __cplusplus
}
#endif

#endif /* __EVENT_MANAGER_H__ */

