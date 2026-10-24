/**
 * @file systime.c
 * @brief システム時刻を計測・公開する
 * @author takap
 * @date Aug 09, 2026
 * @Version 0.00
 */

/*========VVVV Include Standard Header START VVVV============================*/
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "systime.h"
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Definition START VVVV=========================*/
/*========AAAA GLOBAL Variable Definition END AAAA===========================*/

/*========VVVV Private Variable Definition START VVVV========================*/
//! 1msでカウントアップして時刻を記録する変数
volatile static systime_t systime_count1ms;

/*========AAAA Private Variable Definition END AAAA==========================*/

/*========VVVV Private Function Prototype Declaration START VVVV=============*/
/*========AAAA Private Function Prototype Declaration END AAAA===============*/

/*========VVVV GLOBAL Function Definition START VVVV=========================*/
void systime_Init(void) {
    systime_count1ms = 0;
}

void systime_handler1ms(void) {
    systime_count1ms++;
}

systime_t systime_now_ms(void) {
    return systime_count1ms;
}

/*========AAAA GLOBAL Function Definition END AAAA===========================*/

/*========VVVV Private Function Definition START VVVV========================*/
/*========AAAA Private Function Definition END AAAA==========================*/

