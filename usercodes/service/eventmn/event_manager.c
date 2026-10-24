/**
 * @file event_manager.c
 * @brief イベント管理モジュール
 */

/*========VVVV Include Standard Header START VVVV============================*/
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "event_manager.h"
#include "event_manager_config.h"
#include "event_manager_etyp.h"
#include "../systime/systime.h"
#include "orgtypedef.h"
#include "stm32g4xx_hal.h"
#include <stdint.h>

#if (SERVICE_DEBUGMODE == DEBUGMODE_EVENTQUEUE_TEST)
    #include <xprintf.h>
    #include <string.h>
    #include <stdlib.h>
#endif /* SERVICE_DEBUGMODE == DEBUGMODE_EVENTQUEUE_TEST */

/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/**
 * @brief キューに格納するイベントの内容
 */
typedef struct {
    EVENT_MANAGER_ETYPE_t etp;
    systime_t time;
    uint16_t source_id;
    uint8_t* message;
    uint8_t message_size;
} event_t;

/**
 * @brief イベントを格納する双方向リストのノード
 */
typedef struct st_event_node {
    event_t event;
    struct st_event_node* pre;
    struct st_event_node* next;
} event_node_t;

/**
 * @brief イベントノードの管理情報
 * @details nodesは固定バッファの先頭、headとtailは使用中リストの先頭と末尾を示す。
 */
typedef struct {
    event_node_t* nodes;
    event_node_t* head;
    event_node_t* tail;
} event_node_manager_t;
/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Definition START VVVV=========================*/
/*========AAAA GLOBAL Variable Definition END AAAA===========================*/

/*========VVVV Private Variable Definition START VVVV========================*/
/**
 * @brief イベントノードを確保する固定長バッファ
 */
static event_node_t event_manager_buffer[EVENT_MANAGER_QUESIZE];

/**
 * @brief イベントノードリストの管理情報
 */
static event_node_manager_t event_nodes;
#if (SERVICE_DEBUGMODE == DEBUGMODE_EVENTQUEUE_TEST)
    uint8_t eventmanager_loopcnt;
    static systime_t test_last_enqueued_time;
#endif /* SERVICE_DEBUGMODE == DEBUGMODE_EVENTQUEUE_TEST */

/*========AAAA Private Variable Definition END AAAA==========================*/

/*========VVVV Private Function Prototype Declaration START VVVV=============*/
/**
 * @brief 固定長バッファから未使用ノードを探す
 * @return 未使用ノード。空きがない場合はNULL
 */
static event_node_t* event_manager_AllocateFree(void);

/**
 * @brief イベントをリストの末尾へ追加する
 * @param content 追加するイベント
 * @param node 追加先として確保済みのノード
 */
static void event_manager_InsertTail(event_t content, event_node_t* node);

/**
 * @brief ノードに格納されたイベントを読み出す
 * @param node 読み出し対象のノード
 * @return ノードに格納されたイベント
 */
static event_t event_manager_ReadNode(event_node_t* node);

/**
 * @brief ノードをリストから外し、未使用状態に戻す
 * @param node 削除対象のノード
 * @return 再利用可能になったノード。nodeがNULLの場合はNULL
 */
static event_node_t* event_manager_RemoveAndAllocate(event_node_t* node);

/**
 * @brief 指定イベントタイプをリストの先頭から検索する
 * @param etp 検索対象のイベントタイプ
 * @return 最初に見つかったノード。見つからない場合はNULL
 */
static event_node_t* event_manager_SearchNodeForward(EVENT_MANAGER_ETYPE_t etp);

/**
 * @brief イベント内容を出力引数へ設定する
 * @param content 出力するイベント
 * @param et イベントタイプの出力先。NULL可
 * @param time 発生時刻の出力先。NULL可
 * @param message メッセージポインタの出力先。NULL可
 * @param message_size メッセージサイズの出力先。NULL可
 */
static void event_manager_SetOutput(event_t content, EVENT_MANAGER_ETYPE_t* et,
                                    systime_t* time, uint16_t* source_id,
                                    uint8_t** message,
                                    uint8_t* message_size);
/*========AAAA Private Function Prototype Declaration END AAAA===============*/

/*========VVVV GLOBAL Function Definition START VVVV=========================*/
/**
 * @brief イベントマネージャを初期化する
 * @details 固定長バッファを未使用状態にし、イベントリストを空にする。
 */
void event_manager_Init(void) {
    int cnt;

    /* 全ノードを未使用状態に戻し、リンクも初期化する。 */
    for (cnt = 0; cnt < EVENT_MANAGER_QUESIZE; cnt++) {
        event_manager_buffer[cnt].event.etp = EVENT_MANAGER_ETYPE_NONEVENT;
        event_manager_buffer[cnt].event.time = 0;
        event_manager_buffer[cnt].event.source_id = 0;
        event_manager_buffer[cnt].event.message = NULL;
        event_manager_buffer[cnt].event.message_size = 0;
        event_manager_buffer[cnt].pre = NULL;
        event_manager_buffer[cnt].next = NULL;
    }
    /* 初期状態では使用中リストにノードが存在しない。 */
    event_nodes.nodes = event_manager_buffer;
    event_nodes.head = NULL;
    event_nodes.tail = NULL;
}

/**
 * @brief イベントをキューの末尾へ格納する
 * @param et イベントタイプ
 * @param message イベントに付随するメッセージへのポインタ
 * @param message_size メッセージサイズ
 * @return true 空きノードへ格納できた場合
 * @return false 満杯のため最古のイベントを破棄して格納した場合、またはNONEVENTの場合
 * @details リスト更新中は割り込みを停止する。メッセージ本体はコピーせず、ポインタを保持する。
 */
bool event_manager_Enque(EVENT_MANAGER_ETYPE_t et, uint16_t source_id,
                         uint8_t* message, uint8_t message_size) {
    event_t content = {et, systime_now_ms(), source_id, message, message_size};
    event_node_t* node;
    bool result = true;

    if (et == EVENT_MANAGER_ETYPE_NONEVENT) {
        return false;
    }

#if (SERVICE_DEBUGMODE == DEBUGMODE_EVENTQUEUE_TEST)
    test_last_enqueued_time = content.time;
#endif /* SERVICE_DEBUGMODE == DEBUGMODE_EVENTQUEUE_TEST */

    /* 割り込み側とmain側が同じリストを操作するため、更新中は排他する。 */
    __disable_irq();
    node = event_manager_AllocateFree();
    if (node == NULL) {
        /* 満杯時は最古のheadを外し、その領域を新しいtailに再利用する。 */
        node = event_manager_RemoveAndAllocate(event_nodes.head);
        result = false;
    }
    event_manager_InsertTail(content, node);
    __enable_irq();
    return result;
}

/**
 * @brief キューの先頭イベントを取り出して削除する
 * @param et イベントタイプの出力先
 * @param time 発生時刻の出力先
 * @param message メッセージポインタの出力先
 * @param message_size メッセージサイズの出力先
 * @return true イベントを取り出せた場合
 * @return false キューが空の場合
 */
bool event_manager_Deque(EVENT_MANAGER_ETYPE_t* et, systime_t* time,
                         uint16_t* source_id, uint8_t** message,
                         uint8_t* message_size) {
    event_node_t* node;
    event_t content;

    if (source_id == NULL) {
        return false;
    }

    __disable_irq();
    node = event_nodes.head;
    if (node == NULL) {
        /* 空キューではイベントがないことを出力して終了する。 */
        event_manager_SetOutput((event_t){EVENT_MANAGER_ETYPE_NONEVENT, 0, 0,
                          NULL, 0},
                    et, time, source_id, message, message_size);
        __enable_irq();
        return false;
    }
    /* 内容を先に退避してから、headノードをリストから外す。 */
    content = event_manager_ReadNode(node);
    event_manager_SetOutput(content, et, time, source_id, message, message_size);
    event_manager_RemoveAndAllocate(node);
    __enable_irq();
    return true;
}

/**
 * @brief 指定タイプの最古イベントを検索して取り出す
 * @param et 検索対象のイベントタイプ
 * @param time 発生時刻の出力先
 * @param message メッセージポインタの出力先
 * @param message_size メッセージサイズの出力先
 * @return true 対象イベントを取り出せた場合
 * @return false 対象イベントが存在しない場合
 */
bool event_manager_FindAndRemove(EVENT_MANAGER_ETYPE_t et, systime_t* time,
                                 uint8_t** message, uint8_t* message_size) {
    event_node_t* node;
    event_t content;

    __disable_irq();
    /* headから順に探すことで、同じタイプが複数あっても最古を選ぶ。 */
    node = event_manager_SearchNodeForward(et);
    if (node == NULL) {
        __enable_irq();
        return false;
    }
    /* 検索したノードはheadとは限らないため、リンクをつなぎ直して削除する。 */
    content = event_manager_ReadNode(node);
    event_manager_SetOutput(content, NULL, time, NULL, message, message_size);
    event_manager_RemoveAndAllocate(node);
    __enable_irq();
    return true;
}

/**
 * @brief キューに格納されているイベント数を取得する
 * @return 格納イベント数
 */
uint8_t event_manager_BufferSize(void) {
    uint8_t count = 0;
    event_node_t* node;

    __disable_irq();
    /* リストを走査して使用中ノード数を数える。 */
    for (node = event_nodes.head; node != NULL; node = node->next) {
        count++;
    }
    __enable_irq();
    return count;
}

/**
 * @brief 指定タイプのイベント数を取得する
 * @param et 検索対象のイベントタイプ
 * @return 指定タイプの格納数
 */
uint8_t event_manager_Find(EVENT_MANAGER_ETYPE_t et) {
    uint8_t count = 0;
    event_node_t* node;

    __disable_irq();
    /* 対象タイプだけを数え、イベント自体は変更しない。 */
    for (node = event_nodes.head; node != NULL; node = node->next) {
        if (node->event.etp == et) {
            count++;
        }
    }
    __enable_irq();
    return count;
}

#if (SERVICE_DEBUGMODE == DEBUGMODE_EVENTQUEUE_TEST)
uint8_t event_test(char* strBuffer, uint8_t maxBufferSize) {
    static uint8_t phase;
    static uint8_t enqueue_count;
    EVENT_MANAGER_ETYPE_t event_type;
    EVENT_MANAGER_ETYPE_t dequeued_type;
    uint16_t source_id;
    systime_t enqueued_time;
    systime_t dequeued_time;
    bool result;
    uint8_t queue_size;

    switch (phase) {
    case 0:
        /* テスト開始時にキューを空にし、容量分の投入へ進む。 */
        event_manager_Init();
        enqueue_count = 0;
        phase = 1;
        xsnprintf(strBuffer, maxBufferSize,
              "INIT size=%d", event_manager_BufferSize());
        return 0;

    case 1:
        /* TEST1〜TEST3を繰り返し、キューをちょうど満杯にする。 */
        event_type = (EVENT_MANAGER_ETYPE_t)(EVENT_MANAGER_ETYPE_TEST1 +
                                             (enqueue_count % 3));
        result = event_manager_Enque(event_type, 0, NULL, 0);
        enqueued_time = test_last_enqueued_time;
        queue_size = event_manager_BufferSize();
        enqueue_count++;
        if (enqueue_count == EVENT_MANAGER_QUESIZE) {
            phase = 2;
        }
        xsnprintf(strBuffer, maxBufferSize,
                  "ENQ TEST%d time=%lu ok=%d size=%d",
                  event_type - EVENT_MANAGER_ETYPE_TEST1 + 1,
                  (unsigned long)enqueued_time,
                  result, queue_size);
        return 0;

    case 2:
        /* 満杯のキューへ追加し、最古のイベントが破棄されることを確認する。 */
        result = event_manager_Enque(EVENT_MANAGER_ETYPE_TEST1, 0, NULL, 0);
        enqueued_time = test_last_enqueued_time;
        phase = 3;
        xsnprintf(strBuffer, maxBufferSize,
              "FULL ENQ TEST1 time=%lu ok=%d size=%d",
              (unsigned long)enqueued_time,
              result, event_manager_BufferSize());
        return 0;

    case 3:
        /* TEST3だけを検索して取り出し、FindAndRemoveを確認する。 */
        result = event_manager_FindAndRemove(EVENT_MANAGER_ETYPE_TEST3,
                                   &dequeued_time, NULL, NULL);
        phase = 4;
        xsnprintf(strBuffer, maxBufferSize,
              "FIND TEST3 event_time=%lu ok=%d size=%d",
              (unsigned long)dequeued_time, result, event_manager_BufferSize());
        return 0;

    case 4:
        /* 残ったイベントを先頭から1件ずつ取り出す。 */
        result = event_manager_Deque(&dequeued_type, &dequeued_time,
                         &source_id, NULL, NULL);
        if (!result) {
            phase = 5;
            xsnprintf(strBuffer, maxBufferSize, "DRAIN size=0");
            return 0;
        }
        xsnprintf(strBuffer, maxBufferSize,
                  "DEQ TEST%d event_time=%lu size=%d",
                  dequeued_type - EVENT_MANAGER_ETYPE_TEST1 + 1,
                  (unsigned long)dequeued_time,
                  event_manager_BufferSize());
        return 0;

    default:
        /* 空キューからのデキューを確認し、次の周期を開始する。 */
        result = event_manager_Deque(&dequeued_type, NULL, &source_id,
                         NULL, NULL);
        phase = 0;
        xsnprintf(strBuffer, maxBufferSize,
              "EMPTY DEQ type=%d ok=%d size=%d",
              dequeued_type, result, event_manager_BufferSize());
        return 0;
    }
}
#endif /* SERVICE_DEBUGMODE == DEBUGMODE_EVENTQUEUE_TEST */

/*========AAAA GLOBAL Function Definition END AAAA===========================*/

/*========VVVV Private Function Definition START VVVV========================*/
/**
 * @brief 固定長バッファから未使用ノードを探す
 * @return 未使用ノード。空きがない場合はNULL
 */
static event_node_t* event_manager_AllocateFree(void) {
    int cnt;

    /* NONEVENTを未使用ノードの印として使う。 */
    for (cnt = 0; cnt < EVENT_MANAGER_QUESIZE; cnt++) {
        if (event_manager_buffer[cnt].event.etp == EVENT_MANAGER_ETYPE_NONEVENT) {
            return &event_manager_buffer[cnt];
        }
    }
    return NULL;
}

/**
 * @brief イベントを双方向リストの末尾へ追加する
 * @param content 追加するイベント
 * @param node 追加先のノード
 */
static void event_manager_InsertTail(event_t content, event_node_t* node) {
    /* 新ノードを末尾につなぎ、空リストならheadにも設定する。 */
    node->event = content;
    node->pre = event_nodes.tail;
    node->next = NULL;
    if (event_nodes.tail != NULL) {
        event_nodes.tail->next = node;
    } else {
        event_nodes.head = node;
    }
    event_nodes.tail = node;
}

/**
 * @brief ノードのイベント内容を読み出す
 * @param node 読み出し対象のノード
 * @return ノードのイベント内容
 */
static event_t event_manager_ReadNode(event_node_t* node) {
    return node->event;
}

/**
 * @brief ノードを双方向リストから削除して再利用可能にする
 * @param node 削除対象のノード
 * @return 再利用可能になったノード。nodeがNULLの場合はNULL
 */
static event_node_t* event_manager_RemoveAndAllocate(event_node_t* node) {
    if (node == NULL) {
        return NULL;
    }

    /* 削除対象の前後を直接つなぎ、head/tailも必要に応じて更新する。 */
    if (node->pre != NULL) {
        node->pre->next = node->next;
    } else {
        event_nodes.head = node->next;
    }
    if (node->next != NULL) {
        node->next->pre = node->pre;
    } else {
        event_nodes.tail = node->pre;
    }

    /* 次回の空きノード探索で再利用できる状態に戻す。 */
    node->event.etp = EVENT_MANAGER_ETYPE_NONEVENT;
    node->event.time = 0;
    node->event.source_id = 0;
    node->event.message = NULL;
    node->event.message_size = 0;
    node->pre = NULL;
    node->next = NULL;
    return node;
}

/**
 * @brief 双方向リストを先頭から検索する
 * @param etp 検索対象のイベントタイプ
 * @return 最初に見つかったノード。見つからない場合はNULL
 */
static event_node_t* event_manager_SearchNodeForward(EVENT_MANAGER_ETYPE_t etp) {
    event_node_t* node;

    for (node = event_nodes.head; node != NULL; node = node->next) {
        if (node->event.etp == etp) {
            return node;
        }
    }
    return NULL;
}

/**
 * @brief イベント内容を指定された出力先へ設定する
 * @param content 出力するイベント
 * @param et イベントタイプの出力先。NULL可
 * @param time 発生時刻の出力先。NULL可
 * @param message メッセージポインタの出力先。NULL可
 * @param message_size メッセージサイズの出力先。NULL可
 */
static void event_manager_SetOutput(event_t content, EVENT_MANAGER_ETYPE_t* et,
                                    systime_t* time, uint16_t* source_id,
                                    uint8_t** message,
                                    uint8_t* message_size) {
    if (et != NULL) {
        *et = content.etp;
    }
    if (time != NULL) {
        *time = content.time;
    }
    if (source_id != NULL) {
        *source_id = content.source_id;
    }
    if (message != NULL) {
        *message = content.message;
    }
    if (message_size != NULL) {
        *message_size = content.message_size;
    }
}
/*========AAAA Private Function Definition END AAAA==========================*/
