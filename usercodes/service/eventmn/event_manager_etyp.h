/**
 * @file event_manager_eid.h
 * @brief イベントID
 * @author takap
 * @date Nov 30, 2025
 * @Version 0.00
 */

#ifndef __EVENT_MANAGER_ETYP_H__
#define __EVENT_MANAGER_ETYP_H__

#include "event_manager_config.h"
#include <test_and_debug.h>
#ifdef __cplusplus
extern "C" {
#endif

/*========VVVV Include Standard Header START VVVV============================*/
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/**
 * @brief EVENT_MANAGER_EID_t型の列挙体
 */
typedef enum {
    EVENT_MANAGER_ETYPE_NONEVENT,   //! イベントがない場合（初期値）
#if (SERVICE_DEBUGMODE == DEBUGMODE_EVENTQUEUE_TEST)
    EVENT_MANAGER_ETYPE_TEST1,      //! テスト用のイベント1
    EVENT_MANAGER_ETYPE_TEST2,      //! テスト用のイベント2
    EVENT_MANAGER_ETYPE_TEST3,      //! テスト用のイベント3
#endif /*SERVICE_DEBUGMODE == DEBUGMODE_EVENTQUEUE_TEST */
    EVENT_MANAGER_ETYPE_SCHEDULE    //! タイマーイベント
/*======= VVVV イベントを追加する VVVV ========*/
/*======= AAAA イベントを追加する AAAA ========*/
} EVENT_MANAGER_ETYPE_t;


/*========AAAA GLOBAL Variable Declaration END AAAA==========================*/

#ifdef __cplusplus
}
#endif

#endif /* __EVENT_MANAGER_ETYP_H__ */

