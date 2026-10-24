/**
 * @file event_manager_config.h
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 30, 2025
 * @Version 0.00
 */

#ifndef __EVENT_MANAGER_CONFIG_H__
#define __EVENT_MANAGER_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/*========VVVV Include Standard Header START VVVV============================*/
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
/**
 * @brief イベントキュー用の確保領域
 */
/* # define MAX_NANKA 256 */
#define EVENT_MANAGER_QUESIZE   16

/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Declaration START VVVV========================*/
/*========AAAA GLOBAL Variable Declaration END AAAA==========================*/


#ifdef __cplusplus
}
#endif

#endif /* __EVENT_MANAGER_CONFIG_H__ */