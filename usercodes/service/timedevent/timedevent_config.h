/**
 * @file timedevent_config.h
 * @brief 時刻到達イベントの設定
 */

#ifndef __TIMEDEVENT_CONFIG_H__
#define __TIMEDEVENT_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 同時に登録できるタイマー設定数
 */
#define TIMEDEVENT_MAX_ENTRIES 8U

/**
 * @brief 登録可能な最大遅延時間[ms]
 * @details 時刻差分によるオーバーフロー判定を成立させるため、符号付き32bit整数の最大値に制限する。
 */
#define TIMEDEVENT_MAX_DELAY_MS 0x7FFFFFFFUL

#ifdef __cplusplus
}
#endif

#endif /* __TIMEDEVENT_CONFIG_H__ */
