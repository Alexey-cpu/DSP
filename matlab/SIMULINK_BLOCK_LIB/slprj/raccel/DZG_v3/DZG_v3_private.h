#ifndef RTW_HEADER_DZG_v3_private_h_
#define RTW_HEADER_DZG_v3_private_h_
#include "rtwtypes.h"
#include "builtin_typeid_types.h"
#include "multiword_types.h"
#include <math.h>
#include <stdlib.h>
#include "DZG_v3.h"
#if !defined(rt_VALIDATE_MEMORY)
#define rt_VALIDATE_MEMORY(S, ptr)   if(!(ptr)) {\
  ssSetErrorStatus(rtS, RT_MEMORY_ALLOCATION_ERROR);\
  }
#endif
#if !defined(rt_FREE)
#if !defined(_WIN32)
#define rt_FREE(ptr)   if((ptr) != (NULL)) {\
  free((ptr));\
  (ptr) = (NULL);\
  }
#else
#define rt_FREE(ptr)   if((ptr) != (NULL)) {\
  free((void *)(ptr));\
  (ptr) = (NULL);\
  }
#endif
#endif
#ifndef CodeFormat
#define CodeFormat   S-Function
#else
#undef CodeFormat
#define CodeFormat   S-Function
#endif
#ifndef S_FUNCTION_NAME
#define S_FUNCTION_NAME   simulink_only_sfcn
#else
#undef S_FUNCTION_NAME
#define S_FUNCTION_NAME   simulink_only_sfcn
#endif
#ifndef S_FUNCTION_LEVEL
#define S_FUNCTION_LEVEL  2
#else
#undef S_FUNCTION_LEVEL
#define S_FUNCTION_LEVEL  2
#endif
#ifndef RTW_GENERATED_S_FUNCTION
#define RTW_GENERATED_S_FUNCTION
#endif
#ifndef rtmGetDataMapInfo
#define rtmGetDataMapInfo(rtm)        NULL
#endif
#ifndef rtmSetDataMapInfo
#define rtmSetDataMapInfo(rtm, val)
#endif
#if !defined(RTW_SFUNCTION_DEFINES)
#define RTW_SFUNCTION_DEFINES
#ifndef _RTW_COMMON_DEFINES_
#define _RTW_COMMON_DEFINES_
#endif
#endif
#ifndef __RTW_UTFREE__
extern void * utMalloc ( size_t ) ; extern void utFree ( void * ) ;
#endif
void * rt_TDelayCreateBuf ( int_T numBuffer , int_T bufSz , int_T elemSz ) ;
boolean_T rt_TDelayUpdateTailOrGrowBuf ( int_T * bufSzPtr , int_T * tailPtr ,
int_T * headPtr , int_T * lastPtr , real_T tMinusDelay , real_T * * tBufPtr ,
real_T * * uBufPtr , real_T * * xBufPtr , boolean_T isfixedbuf , boolean_T
istransportdelay , int_T * maxNewBufSzPtr ) ; real_T rt_TDelayInterpolate (
real_T tMinusDelay , real_T tStart , real_T * tBuf , real_T * uBuf , int_T
bufSz , int_T * lastIdx , int_T oldestIdx , int_T newIdx , real_T initOutput
, boolean_T discrete , boolean_T minorStepAndTAtLastMajorOutput ) ; real_T
rt_VTDelayfindtDInterpolate ( real_T x , real_T * tBuf , real_T * uBuf ,
real_T * xBuf , int_T bufSz , int_T head , int_T tail , int_T * pLast ,
real_T t , real_T tStart , boolean_T discrete , boolean_T
minorStepAndTAtLastMajorOutput , real_T initOutput , real_T * appliedDelay )
; void rt_TDelayFreeBuf ( void * buf ) ; extern void ora1wlqeti ( nenimphfvv
* localB , dq1pbrolhq * localP ) ; extern void fx2esogudm ( SimStruct * rtS_e
, boolean_T fflxnsiu0g , real_T orjpri2rnl , nenimphfvv * localB , c2yel4glz3
* localDW , kfjh0rdg4y * localZCE ) ; extern void h4dgpi453n ( pfuoywnvt4 *
localB , dm4sxbd4wc * localP ) ; extern void a5f3eefi2x ( SimStruct * rtS_i ,
fittvluxdk * localDW ) ; extern void m3qjgqfo54 ( fittvluxdk * localDW ) ;
extern void cgvj5xjnjk ( SimStruct * rtS_i , real_T k4grt3yuge , boolean_T
fyzi5wcstb , boolean_T h5w3yyafqq , pfuoywnvt4 * localB , fittvluxdk *
localDW ) ; extern void h33j1jr1zm ( p4n5izx35p * localB , mknwhmtwd0 *
localP ) ; extern void kwn5re5v52 ( SimStruct * rtS_g , alhepkh51m * localDW
) ; extern void f3mc3li2yn ( alhepkh51m * localDW ) ; extern void itcd43cpff
( SimStruct * rtS_j , real_T ivz1lgh0sc , boolean_T nqdkubdyhn , boolean_T
jqq1z5jyzp , p4n5izx35p * localB , alhepkh51m * localDW ) ; extern void
p5hgyyqss4 ( jk5bp5fzdn * localB , mrvrl4is5b * localDW , aynkghgu2e * localP
, boolean_T rtp_ic ) ; extern void fovxs4153s ( SimStruct * rtS_m ,
mrvrl4is5b * localDW ) ; extern void n0lbj4yv0e ( mrvrl4is5b * localDW ) ;
extern void ny353qchui ( SimStruct * rtS_b , jk5bp5fzdn * localB , mrvrl4is5b
* localDW ) ; extern void bhc3qgnuic ( SimStruct * rtS_k , boolean_T
lupksbzqrb , boolean_T mqwzmjfrx3 , real_T fuppb0sv3g , real_T imqbiz5qjw ,
jk5bp5fzdn * localB , mrvrl4is5b * localDW , aynkghgu2e * localP , lshrplgrom
* localZCE ) ; extern void bhc3qgnuicTID2 ( SimStruct * rtS_c , jk5bp5fzdn *
localB , mrvrl4is5b * localDW , aynkghgu2e * localP ) ; extern void
mbb3rj5v0g ( ozzv521fhb * localB , mrj3zmuhdd * localDW , cpv1hiufg3 * localP
, boolean_T rtp_ic ) ; extern void ei2bq4gymn ( SimStruct * rtS_g ,
mrj3zmuhdd * localDW ) ; extern void cgjlc55t5n ( mrj3zmuhdd * localDW ) ;
extern void lwcs3jh3k2 ( SimStruct * rtS_l , ozzv521fhb * localB , mrj3zmuhdd
* localDW ) ; extern void on20slldft ( SimStruct * rtS_d , boolean_T
f2dowwvjdu , boolean_T dkh4fhrrog , real_T aupbitltac , real_T p4i2dovx34 ,
ozzv521fhb * localB , mrj3zmuhdd * localDW , cpv1hiufg3 * localP , lkx3rpkald
* localZCE ) ; extern void on20slldftTID2 ( SimStruct * rtS_d , ozzv521fhb *
localB , mrj3zmuhdd * localDW , cpv1hiufg3 * localP ) ; extern void
pzffuxhmmn ( k0g5xdyqr2 * localB , hfic5lwxhq * localP ) ; extern void
hjgrwuhwvr ( SimStruct * rtS_hn , c4a0p3nf0r * localDW ) ; extern void
cl3avcjyrj ( c4a0p3nf0r * localDW ) ; extern void kseqhf442u ( SimStruct *
rtS_pj , uint8_T ij3w1ygrqh , const real_T e1wis5d0jt [ 2 ] , real_T
bcr5qqte0s , k0g5xdyqr2 * localB , c4a0p3nf0r * localDW ) ; extern void
oitgzbgdnq ( cdef0al4wh * localB , nzb40xcmsy * localP ) ; extern void
jztd2wvlzd ( SimStruct * rtS_pk , jevkkrp5kd * localDW ) ; extern void
c5hqhunjdy ( jevkkrp5kd * localDW ) ; extern void cgsqxgexc2 ( SimStruct *
rtS_kh , uint8_T akf3t4vetg , const real_T kijsznk4qm [ 2 ] , real_T
bq2hzajal0 , cdef0al4wh * localB , jevkkrp5kd * localDW ) ; extern void
azcfs245nz ( icyonxkqj1 * localB , ocqjbneyfv * localP ) ; extern void
kqrkri2pgl ( SimStruct * rtS_fv , fqje1r52jc * localDW ) ; extern void
b5dtpmhnl0 ( fqje1r52jc * localDW ) ; extern void jvoeh2x3fa ( SimStruct *
rtS_nf , real_T p3nbfeyjc2 , creal_T h0x051a543 , creal_T f2xovsyrtp ,
creal_T e1cbmi2xac , icyonxkqj1 * localB , fqje1r52jc * localDW , ocqjbneyfv
* localP ) ; extern void hnxt2e2yod ( cb5z2tiqbz * localB , cbyz3vd5ik *
localP ) ; extern void f3onajihkb ( SimStruct * rtS_b0 , mtg0lhh3xi * localDW
) ; extern void ptuajwmchn ( mtg0lhh3xi * localDW ) ; extern void czm0w3hmwh
( SimStruct * rtS_gw , real_T cf3sa0t5du , creal_T jpkpejfffg , creal_T
m0jbhpsmo3 , creal_T mzwlt5f1ag , cb5z2tiqbz * localB , mtg0lhh3xi * localDW
, cbyz3vd5ik * localP ) ;
#if defined(MULTITASKING)
#error Model (DZG_v3) was built in \SingleTasking solver mode, however the MULTITASKING define is \present. If you have multitasking (e.g. -DMT or -DMULTITASKING) \defined on the Code Generation page of Simulation parameter dialog, please \remove it and on the Solver page, select solver mode \MultiTasking. If the Simulation parameter dialog is configured \correctly, please verify that your template makefile is \configured correctly.
#endif
#endif
