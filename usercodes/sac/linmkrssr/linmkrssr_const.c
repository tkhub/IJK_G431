/**
 * @file linmkrssr_const.c
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 15, 2025
 * @Version 0.00
 */

/*========VVVV Include Standard Header START VVVV============================*/
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "linmkrssr_const.h"
#include "linmkrssr_private.h"
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Definition START VVVV=========================*/


const float     LLSSR_OFFSET        = 33.0f;    // +33.0[mm]
const uint16_t  LLSSR_DEFAULT_WHITE = 2000;
const uint16_t  LLSSR_DEFAULT_BLACK = 150;

const float     LCSSR_OFFSET        = 12.0f;    // +12.0[mm]
const uint16_t  LCSSR_DEFAULT_WHITE = 2500;
const uint16_t  LCSSR_DEFAULT_BLACK = 60;


const float     RCSSR_OFFSET        = -12.0f;   // -12.0[mm]
const uint16_t  RCSSR_DEFAULT_WHITE = 2300;
const uint16_t  RCSSR_DEFAULT_BLACK = 50;

const float     RRSSR_OFFSET        = -33.0f;   // +33.0[mm]
const uint16_t  RRSSR_DEFAULT_WHITE = 2000;
const uint16_t  RRSSR_DEFAULT_BLACK = 35;


const float      LINESENSOR_THRESHOLD   = 0.35f;
const float      LINESENSOR_HYSTERESIS  = 0.1f;
const float      LINESENSOR_IIRLPG      = 0.7f;

const uint16_t   MKLSSR_DEFAULT_WHITE   = 400;
const uint16_t   MKLSSR_DEFAULT_BLACK   = 60;

const uint16_t   MKRSSR_DEFAULT_WHITE   = 400;
const uint16_t   MKRSSR_DEFAULT_BLACK   = 60;

const float      MARKERSENSOR_THRESHOLD = 0.65f;
const float      MARKERSENSOR_HYSTERESIS= 0.20f;

/*========AAAA GLOBAL Variable Definition END AAAA===========================*/
