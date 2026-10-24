/**
 * @file hex2bindec.h
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 15, 2025
 * @Version 0.00
 */

#ifndef __COMMON_HEX2BINDEC_H___
#define __COMMON_HEX2BINDEC_H___

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
 * @brief 数字を２進数変換するマクロ
 * @Warning 変換結果は２進数のように見えるが、実際は10進数である。
 */
#define HEX_TO_BIN_DEC(x) \
    ((x) == 0x0 ? 0 : \
     (x) == 0x1 ? 1 : \
     (x) == 0x2 ? 10 : \
     (x) == 0x3 ? 11 : \
     (x) == 0x4 ? 100 : \
     (x) == 0x5 ? 101 : \
     (x) == 0x6 ? 110 : \
     (x) == 0x7 ? 111 : \
     (x) == 0x8 ? 1000 : \
     (x) == 0x9 ? 1001 : \
     (x) == 0xA ? 1010 : \
     (x) == 0xB ? 1011 : \
     (x) == 0xC ? 1100 : \
     (x) == 0xD ? 1101 : \
     (x) == 0xE ? 1110 : \
     (x) == 0xF ? 1111 : -1)
/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Declaration START VVVV========================*/
/*========AAAA GLOBAL Variable Declaration END AAAA==========================*/

/*========VVVV GLOBAL Function Declaration START VVVV========================*/
/*========AAAA GLOBAL Function Declaration END AAAA==========================*/

#ifdef __cplusplus
}
#endif

#endif /* __COMMON_HEX2BINDEC_H___ */
