/**
 * @file service.h
 * @brief サービス
 * @author takap
 * @date Nov 30, 2025
 * @Version 0.00
 */

#ifndef __SERVICE_H___
#define __SERVICE_H___

#ifdef __cplusplus
extern "C" {
#endif

/*========VVVV Include Standard Header START VVVV============================*/

/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "eventmn/event_manager.h"
#include "i2cmn/i2c_manager.h"
#include "timedevent/timedevent.h"

/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Declaration START VVVV========================*/
/*========AAAA GLOBAL Variable Declaration END AAAA==========================*/

/*========VVVV GLOBAL Function Declaration START VVVV========================*/
/**
 * @brief サービス関数
 * @details 詳細な説明
 */
void service_loop(void);

/*========AAAA GLOBAL Function Declaration END AAAA==========================*/

#ifdef __cplusplus
}
#endif

#endif /* __SERVICE_H___ */
