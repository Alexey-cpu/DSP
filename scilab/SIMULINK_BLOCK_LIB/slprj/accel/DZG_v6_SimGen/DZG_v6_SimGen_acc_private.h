#ifndef RTW_HEADER_DZG_v6_SimGen_acc_private_h_
#define RTW_HEADER_DZG_v6_SimGen_acc_private_h_
#include "rtwtypes.h"
#include "multiword_types.h"
#include "DZG_v6_SimGen_acc.h"
#if !defined(ss_VALIDATE_MEMORY)
#define ss_VALIDATE_MEMORY(S, ptr)   if(!(ptr)) {\
  ssSetErrorStatus(S, RT_MEMORY_ALLOCATION_ERROR);\
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
#ifndef __RTW_UTFREE__
extern void * utMalloc ( size_t ) ; extern void utFree ( void * ) ;
#endif
extern void rt_invd6x6_snf ( const real_T u [ 36 ] , real_T y [ 36 ] ) ;
boolean_T DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( int_T * bufSzPtr ,
int_T * tailPtr , int_T * headPtr , int_T * lastPtr , real_T tMinusDelay ,
real_T * * tBufPtr , real_T * * uBufPtr , real_T * * xBufPtr , boolean_T
isfixedbuf , boolean_T istransportdelay , int_T * maxNewBufSzPtr ) ; real_T
DZG_v6_SimGen_acc_rt_TDelayInterpolate ( real_T tMinusDelay , real_T tStart ,
real_T * tBuf , real_T * uBuf , int_T bufSz , int_T * lastIdx , int_T
oldestIdx , int_T newIdx , real_T initOutput , boolean_T discrete , boolean_T
minorStepAndTAtLastMajorOutput ) ; extern real_T look1_pbinlxpw ( real_T u0 ,
const real_T bp0 [ ] , const real_T table [ ] , uint32_T prevIndex [ ] ,
uint32_T maxIndex ) ; void DZG_v6_SimGen_NegSeqComputation_Init ( SimStruct *
S , B_NegSeqComputation_DZG_v6_SimGen_T * localB ,
P_NegSeqComputation_DZG_v6_SimGen_T * localP ) ; void
DZG_v6_SimGen_NegSeqComputation_Disable ( SimStruct * S ,
DW_NegSeqComputation_DZG_v6_SimGen_T * localDW ) ; void
DZG_v6_SimGen_NegSeqComputation ( SimStruct * S , real_T rtu_Enable , creal_T
rtu_In , creal_T rtu_In_b , creal_T rtu_In_e ,
B_NegSeqComputation_DZG_v6_SimGen_T * localB ,
DW_NegSeqComputation_DZG_v6_SimGen_T * localDW ,
P_NegSeqComputation_DZG_v6_SimGen_T * localP ) ; void
DZG_v6_SimGen_ZeroSeqComputation_Init ( SimStruct * S ,
B_ZeroSeqComputation_DZG_v6_SimGen_T * localB ,
P_ZeroSeqComputation_DZG_v6_SimGen_T * localP ) ; void
DZG_v6_SimGen_ZeroSeqComputation_Disable ( SimStruct * S ,
DW_ZeroSeqComputation_DZG_v6_SimGen_T * localDW ) ; void
DZG_v6_SimGen_ZeroSeqComputation ( SimStruct * S , real_T rtu_Enable ,
creal_T rtu_In , creal_T rtu_In_j , creal_T rtu_In_b ,
B_ZeroSeqComputation_DZG_v6_SimGen_T * localB ,
DW_ZeroSeqComputation_DZG_v6_SimGen_T * localDW ,
P_ZeroSeqComputation_DZG_v6_SimGen_T * localP ) ; void
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer_Init ( SimStruct * S ,
B_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T * localB ,
P_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T * localP ) ; void
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer_Update ( SimStruct * S ,
real_T rtu_Enable ) ; void
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer ( SimStruct * S , real_T
rtu_Enable , real_T rtu_In1 ,
B_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T * localB ,
DW_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T * localDW ,
P_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T * localP ) ;
#endif
