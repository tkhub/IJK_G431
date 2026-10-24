/**
 * @file service.c
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 30, 2025
 * @Version 0.00
 */

#include <test_and_debug.h>
/*========VVVV Include Standard Header START VVVV============================*/
#include  <xprintf.h>
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "service.h"
#include "eventmn/event_manager.h"
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Definition START VVVV=========================*/
#ifdef _ENABLE_SERVICE_TEST_
static int testcnt;
#endif /* _ENABLE_SAC_TEST_ */
/*========AAAA GLOBAL Variable Definition END AAAA===========================*/

/*========VVVV Private Variable Definition START VVVV========================*/
/*========AAAA Private Variable Definition END AAAA==========================*/

/*========VVVV Private Function Prototype Declaration START VVVV=============*/
/*========AAAA Private Function Prototype Declaration END AAAA===============*/

/*========VVVV GLOBAL Function Definition START VVVV=========================*/
void service_loop(void) {
#ifdef _ENABLE_SERVICE_TEST_
    static char teststr[200];
    #if (SERVICE_DEBUGMODE == DEBUGMODE_EVENTQUEUE_TEST)
        event_test(teststr, 180);
    #elif (SERVICE_DEBUGMODE == DEBUGMODE_TIMEDEVENT_TEST)
        timedevent_test(teststr, 180);
    #endif /*  */
    xprintf("%d\t%s\r\n", testcnt, teststr);
    testcnt++;
#endif /* _ENABLE_SERVICE_TEST_ */
}
/*========AAAA GLOBAL Function Definition END AAAA===========================*/

/*========VVVV Private Function Definition START VVVV========================*/
/*========AAAA Private Function Definition END AAAA==========================*/
