#ifndef __APP_CONST_H__
#define __APP_CONST_H__

#define MAX_PARAM_NUM 8
#define RUN_PARAM_COUNT MAX_PARAM_NUM
extern const float RUN_POWERS[MAX_PARAM_NUM];
extern const float RUN_BREAK[MAX_PARAM_NUM];

//! マーカー・クロスラインなどの最小間隔[mm]
extern const float RUN_MARKER_MIN_INTERVAL_MM ;
extern const float RUN_GOAL_PASS_LENGH_MM;

//! マーカー・クロスラインなどの最小間隔ゲイン
extern const float RUN_RPS2LENGTH_GAIN;

#endif /* __APP_CONST_H__ */
