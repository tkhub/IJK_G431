/*
 * uisw_const.c
 *
 *  Created on: Oct 27, 2025
 *      Author: takap
 */

#include "orgtypedef.h"

const uint16_t UISW_HOLD_10MS = 200; // 2秒ぐらい
const uint16_t UISW_OFFHOLD_10MS = 10; // 0.1秒ぐらい

// NOSW:4005
// sw2:3415
// sw1:3120
// sw12:2750
const uint16_t UISW_ADC_ERR_SW12_TH = 2658; // 2750 - ((2935 - 2750) / 2)
const uint16_t UISW_ADC_SW12_SW1_TH = 2935; // (3120 - 2750) / 2
const uint16_t UISW_ADC_SW1_SW2_TH  = 3268; // (3415 + 3120) / 2
const uint16_t UISW_ADC_SW2_NON_TH  = 3710; // (4005 + 3415) / 2
