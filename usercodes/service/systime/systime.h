/**
 * @file systime.h
 * @brief 簡単な説明  
 * @author takap
 * @date Aug 09, 2026
 * @Version 0.00
 */

#ifndef __SYSTIME_H__
#define __SYSTIME_H__

#ifdef __cplusplus
extern "C" {
#endif

/*========VVVV Include Standard Header START VVVV============================*/
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include <orgtypedef.h>
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
typedef uint32_t systime_t;
/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Declaration START VVVV========================*/
/*========AAAA GLOBAL Variable Declaration END AAAA==========================*/

/*========VVVV GLOBAL Function Declaration START VVVV========================*/

/**
 * @brief systime初期化
 * 
 */
void systime_Init(void);

/**
 * @brief systimeの1msハンドラ
 */
void systime_handler1ms(void);

/**
 * @brief systimeの取得
 * 
 * @return systime_t 取得したシステム時間[ms]
 */
systime_t systime_now_ms(void);


/*========AAAA GLOBAL Function Declaration END AAAA==========================*/

#ifdef __cplusplus
}
#endif

#endif /* __SYSTIME_H__ */

