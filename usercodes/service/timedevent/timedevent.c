/**
 * @file timedevent.c
 * @brief 指定時間後に一度だけイベントを発行する
 */

/*========VVVV Include Standard Header START VVVV============================*/
#include <orgtypedef.h>
#include <stdint.h>
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "../eventmn/event_manager.h"
#include "timedevent.h"
#include "systime.h"
#include "timedevent_config.h"
#include "stm32g4xx_hal.h"
#if (SERVICE_DEBUGMODE == DEBUGMODE_TIMEDEVENT_TEST)
    #include <xprintf.h>
    #include <string.h>
    #include <stdlib.h>
#endif /* _ENABLE_SERVICE_TEST_ */
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/**
 * @brief 発行待ちタイマーの内容
 */
typedef struct {
    bool active;
    systime_t deadline;
    timedevent_id_t timer_id;
} timedevent_entry_t;
/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV Private Variable Definition START VVVV========================*/
/**
 * @brief 発行待ちタイマーを保持する固定長バッファ
 */
static timedevent_entry_t timedevent_buffer[TIMEDEVENT_MAX_ENTRIES];

/**
 * @brief 次に発行するタイマーID
 */
static timedevent_id_t timedevent_next_id;
#if (SERVICE_DEBUGMODE == DEBUGMODE_TIMEDEVENT_TEST)
    uint16_t timedevent_loopcnt;
    static bool test_running;
    static bool test_started;
    static uint8_t test_registered_count;
    static uint8_t test_fired_count;
    static timedevent_id_t test_timer_ids[TIMEDEVENT_MAX_ENTRIES];
#endif /* _ENABLE_SERVICE_TEST_ */
/*========AAAA Private Variable Definition END AAAA==========================*/

/*========VVVV Private Function Prototype Declaration START VVVV=============*/
/**
 * @brief 未使用のタイマー登録枠を探す
 * @return 空き枠。空きがない場合はNULL
 */
static timedevent_entry_t* timedevent_FindFreeEntry(void);

/**
 * @brief 最も古い発行待ちタイマーを探す
 * @return 最も早い期限のタイマー。登録がない場合はNULL
 */
static timedevent_entry_t* timedevent_FindOldestEntry(void);

/**
 * @brief 次のタイマーIDを発行する
 * @return 0以外のタイマーID
 */
static timedevent_id_t timedevent_AllocateId(void);

/**
 * @brief タイマーが期限に到達したか判定する
 * @param now 現在時刻
 * @param deadline タイマー期限
 * @return true 期限到達済み
 */
static bool timedevent_IsExpired(systime_t now, systime_t deadline);
/*========AAAA Private Function Prototype Declaration END AAAA===============*/

/*========VVVV GLOBAL Function Definition START VVVV=========================*/
void timedevent_Init(void) {
    uint8_t index;

    __disable_irq();
    for (index = 0; index < TIMEDEVENT_MAX_ENTRIES; index++) {
        timedevent_buffer[index].active = false;
        timedevent_buffer[index].deadline = 0;
        timedevent_buffer[index].timer_id = 0;
    }
    timedevent_next_id = 1;
#if (SERVICE_DEBUGMODE == DEBUGMODE_TIMEDEVENT_TEST)
    timedevent_loopcnt = 0;
    test_running = false;
    test_started = false;
    test_registered_count = 0;
    test_fired_count = 0;
#endif /* SERVICE_DEBUGMODE == DEBUGMODE_TIMEDEVENT_TEST */
    __enable_irq();
}

bool timedevent_Register(uint32_t delay_ms, timedevent_id_t* timer_id) {
    timedevent_entry_t* entry;
    systime_t now;
    timedevent_id_t allocated_id;
    bool result = true;

    if ((delay_ms == 0U) || (TIMEDEVENT_MAX_DELAY_MS < delay_ms) ||
        (timer_id == NULL)) {
        return false;
    }

    __disable_irq();
    entry = timedevent_FindFreeEntry();
    if (entry == NULL) {
        /* 満杯時は最古のタイマーを破棄して新しい設定に再利用する。 */
        entry = timedevent_FindOldestEntry();
        if (entry == NULL) {
            __enable_irq();
            return false;
        }
        result = false;
    }

    now = systime_now_ms();
    allocated_id = timedevent_AllocateId();
    entry->active = true;
    entry->deadline = now + (systime_t)delay_ms;
    entry->timer_id = allocated_id;
    *timer_id = allocated_id;
    __enable_irq();
    return result;
}

void timedevent_1ms(void) {
    uint8_t index;
    systime_t now = systime_now_ms();
    timedevent_id_t timer_id;
    bool expired;

    for (index = 0; index < TIMEDEVENT_MAX_ENTRIES; index++) {
        expired = false;
        __disable_irq();
        if (timedevent_buffer[index].active &&
            timedevent_IsExpired(now, timedevent_buffer[index].deadline)) {
            /* 先に無効化し、同じタイマーが再発行されないようにする。 */
            timedevent_buffer[index].active = false;
            timer_id = timedevent_buffer[index].timer_id;
            expired = true;
        }
        __enable_irq();

        if (expired) {
            event_manager_Enque(EVENT_MANAGER_ETYPE_SCHEDULE,
                                timer_id, NULL, 0);
        }
    }
}

uint8_t timedevent_BufferSize(void) {
    uint8_t index;
    uint8_t count = 0;

    __disable_irq();
    for (index = 0; index < TIMEDEVENT_MAX_ENTRIES; index++) {
        if (timedevent_buffer[index].active) {
            count++;
        }
    }
    __enable_irq();
    return count;
}

#if (SERVICE_DEBUGMODE == DEBUGMODE_TIMEDEVENT_TEST)
uint8_t timedevent_test(char* strBuffer, uint8_t maxBufferSize) {
    uint8_t cnt;
    uint8_t timer_index;
    bool result;
    bool set_timer = false;
    uint16_t set_trigger_count = 0;
    EVENT_MANAGER_ETYPE_t dequeued_type;
    systime_t dequeued_time;
    uint16_t source_id;

    if (!test_running) {
        if (!test_started) {
            set_timer = true;
            set_trigger_count = 0;
            test_started = true;
        }
        else {
            timedevent_loopcnt++;
            if (200U <= timedevent_loopcnt) {
                set_timer = true;
                set_trigger_count = timedevent_loopcnt;
            }
        }

        if (!set_timer) {
            xsnprintf(strBuffer, maxBufferSize,
                      "TIMER WAIT count=%u", (unsigned)timedevent_loopcnt);
            return 0;
        }

        test_running = true;
        test_registered_count = 0;
        test_fired_count = 0;
        for (cnt = 0; cnt < TIMEDEVENT_MAX_ENTRIES; cnt++) {
            result = timedevent_Register(1000U * (cnt + 1U),
                                         &test_timer_ids[cnt]);
            if (result) {
                test_registered_count++;
            }
        }
        timedevent_loopcnt = 0;
        xsnprintf(strBuffer, maxBufferSize,
                  "TIMER SET trigger=%u count=%u active=%u",
                  (unsigned)set_trigger_count,
                  (unsigned)test_registered_count,
                  (unsigned)timedevent_BufferSize());
        return 0;
    }

    if (event_manager_Find(EVENT_MANAGER_ETYPE_SCHEDULE) == 0U) {
        xsnprintf(strBuffer, maxBufferSize,
                  "TIMER WAIT fired=%u active=%u",
                  (unsigned)test_fired_count,
                  (unsigned)timedevent_BufferSize());
        return 0;
    }

    result = event_manager_Deque(&dequeued_type, &dequeued_time,
                                 &source_id, NULL, NULL);
    if (!result) {
        xsnprintf(strBuffer, maxBufferSize, "TIMER ERROR dequeue");
        return 0;
    }

    if (dequeued_type != EVENT_MANAGER_ETYPE_SCHEDULE) {
        xsnprintf(strBuffer, maxBufferSize,
                  "TIMER ERROR type=%d source_id=%u",
                  dequeued_type, (unsigned)source_id);
        return 0;
    }

    for (timer_index = 0; timer_index < test_registered_count;
         timer_index++) {
        if (test_timer_ids[timer_index] == source_id) {
            test_fired_count++;
            break;
        }
    }

    if (timer_index == test_registered_count) {
        xsnprintf(strBuffer, maxBufferSize,
                  "TIMER ERROR unknown_source=%u event_time=%lu",
                  (unsigned)source_id, (unsigned long)dequeued_time);
        return 0;
    }

    xsnprintf(strBuffer, maxBufferSize,
              "TIMER OK source_id=%u event_time=%lu fired=%u/%u",
              (unsigned)source_id, (unsigned long)dequeued_time,
              (unsigned)test_fired_count, (unsigned)test_registered_count);

    if (test_fired_count == test_registered_count) {
        test_running = false;
    }
    return 0;
}
#endif /* (SERVICE_DEBUGMODE == DEBUGMODE_TIMEDEVENT_TEST) */
/*========AAAA GLOBAL Function Definition END AAAA===========================*/

/*========VVVV Private Function Definition START VVVV========================*/
static timedevent_entry_t* timedevent_FindFreeEntry(void) {
    uint8_t index;

    for (index = 0; index < TIMEDEVENT_MAX_ENTRIES; index++) {
        if (!timedevent_buffer[index].active) {
            return &timedevent_buffer[index];
        }
    }
    return NULL;
}

static timedevent_entry_t* timedevent_FindOldestEntry(void) {
    uint8_t index;
    timedevent_entry_t* oldest = NULL;

    for (index = 0; index < TIMEDEVENT_MAX_ENTRIES; index++) {
        if (timedevent_buffer[index].active &&
            ((oldest == NULL) ||
             ((int32_t)(timedevent_buffer[index].deadline -
                        oldest->deadline) < 0))) {
            oldest = &timedevent_buffer[index];
        }
    }
    return oldest;
}

static timedevent_id_t timedevent_AllocateId(void) {
    timedevent_id_t allocated_id = timedevent_next_id;

    timedevent_next_id++;
    if (timedevent_next_id == 0) {
        timedevent_next_id = 1;
    }
    if (allocated_id == 0) {
        allocated_id = timedevent_next_id;
        timedevent_next_id++;
        if (timedevent_next_id == 0) {
            timedevent_next_id = 1;
        }
    }
    return allocated_id;
}

static bool timedevent_IsExpired(systime_t now, systime_t deadline) {
    return ((int32_t)(now - deadline) >= 0);
}
/*========AAAA Private Function Definition END AAAA==========================*/
