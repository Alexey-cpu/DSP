#ifndef RTW_HEADER_DZG_v3_acc_private_h_
#define RTW_HEADER_DZG_v3_acc_private_h_
#include "rtwtypes.h"
#include "multiword_types.h"
#include "DZG_v3_acc.h"
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
boolean_T DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( int_T * bufSzPtr , int_T
* tailPtr , int_T * headPtr , int_T * lastPtr , real_T tMinusDelay , real_T *
* tBufPtr , real_T * * uBufPtr , real_T * * xBufPtr , boolean_T isfixedbuf ,
boolean_T istransportdelay , int_T * maxNewBufSzPtr ) ; real_T
DZG_v3_acc_rt_TDelayInterpolate ( real_T tMinusDelay , real_T tStart , real_T
* tBuf , real_T * uBuf , int_T bufSz , int_T * lastIdx , int_T oldestIdx ,
int_T newIdx , real_T initOutput , boolean_T discrete , boolean_T
minorStepAndTAtLastMajorOutput ) ; real_T
DZG_v3_acc_rt_VTDelayfindtDInterpolate ( real_T x , real_T * tBuf , real_T *
uBuf , real_T * xBuf , int_T bufSz , int_T head , int_T tail , int_T * pLast
, real_T t , real_T tStart , boolean_T discrete , boolean_T
minorStepAndTAtLastMajorOutput , real_T initOutput , real_T * appliedDelay )
; void DZG_v3_TriggeredSubsystem_Init ( SimStruct * S ,
B_TriggeredSubsystem_DZG_v3_T * localB , P_TriggeredSubsystem_DZG_v3_T *
localP ) ; void DZG_v3_TriggeredSubsystem ( SimStruct * S , boolean_T
rtu_Trigger , real_T rtu_In1 , B_TriggeredSubsystem_DZG_v3_T * localB ,
DW_TriggeredSubsystem_DZG_v3_T * localDW , ZCE_TriggeredSubsystem_DZG_v3_T *
localZCE ) ; void DZG_v3_NEGATIVEEdge_Init ( SimStruct * S ,
B_NEGATIVEEdge_DZG_v3_T * localB , P_NEGATIVEEdge_DZG_v3_T * localP ) ; void
DZG_v3_NEGATIVEEdge_Disable ( SimStruct * S , DW_NEGATIVEEdge_DZG_v3_T *
localDW ) ; void DZG_v3_NEGATIVEEdge ( SimStruct * S , real_T rtu_Enable ,
boolean_T rtu_IN , boolean_T rtu_INprevious , B_NEGATIVEEdge_DZG_v3_T *
localB , DW_NEGATIVEEdge_DZG_v3_T * localDW ) ; void DZG_v3_POSITIVEEdge_Init
( SimStruct * S , B_POSITIVEEdge_DZG_v3_T * localB , P_POSITIVEEdge_DZG_v3_T
* localP ) ; void DZG_v3_POSITIVEEdge_Disable ( SimStruct * S ,
DW_POSITIVEEdge_DZG_v3_T * localDW ) ; void DZG_v3_POSITIVEEdge ( SimStruct *
S , real_T rtu_Enable , boolean_T rtu_IN , boolean_T rtu_INprevious ,
B_POSITIVEEdge_DZG_v3_T * localB , DW_POSITIVEEdge_DZG_v3_T * localDW ) ;
void DZG_v3_OFFDelay_Init ( SimStruct * S , B_OFFDelay_DZG_v3_T * localB ,
DW_OFFDelay_DZG_v3_T * localDW , P_OFFDelay_DZG_v3_T * localP ) ; void
DZG_v3_OFFDelay_Disable ( SimStruct * S , DW_OFFDelay_DZG_v3_T * localDW ) ;
void DZG_v3_OFFDelay_Update ( SimStruct * S , B_OFFDelay_DZG_v3_T * localB ,
DW_OFFDelay_DZG_v3_T * localDW ) ; void DZG_v3_OFFDelay ( SimStruct * S ,
boolean_T rtu_Enable , boolean_T rtu_IN , real_T rtu_clock , real_T rtu_DELAY
, B_OFFDelay_DZG_v3_T * localB , DW_OFFDelay_DZG_v3_T * localDW ,
ZCE_OFFDelay_DZG_v3_T * localZCE ) ; void DZG_v3_OFFDelayTID2 ( SimStruct * S
, B_OFFDelay_DZG_v3_T * localB , DW_OFFDelay_DZG_v3_T * localDW ,
P_OFFDelay_DZG_v3_T * localP ) ; void DZG_v3_ONDelay_Init ( SimStruct * S ,
B_ONDelay_DZG_v3_T * localB , DW_ONDelay_DZG_v3_T * localDW ,
P_ONDelay_DZG_v3_T * localP ) ; void DZG_v3_ONDelay_Disable ( SimStruct * S ,
DW_ONDelay_DZG_v3_T * localDW ) ; void DZG_v3_ONDelay_Update ( SimStruct * S
, B_ONDelay_DZG_v3_T * localB , DW_ONDelay_DZG_v3_T * localDW ) ; void
DZG_v3_ONDelay ( SimStruct * S , boolean_T rtu_Enable , boolean_T rtu_IN ,
real_T rtu_clock , real_T rtu_DELAY , B_ONDelay_DZG_v3_T * localB ,
DW_ONDelay_DZG_v3_T * localDW , ZCE_ONDelay_DZG_v3_T * localZCE ) ; void
DZG_v3_ONDelayTID2 ( SimStruct * S , B_ONDelay_DZG_v3_T * localB ,
DW_ONDelay_DZG_v3_T * localDW , P_ONDelay_DZG_v3_T * localP ) ; void
DZG_v3_Subsystempi2delay_Init ( SimStruct * S , B_Subsystempi2delay_DZG_v3_T
* localB , P_Subsystempi2delay_DZG_v3_T * localP ) ; void
DZG_v3_Subsystempi2delay_Disable ( SimStruct * S ,
DW_Subsystempi2delay_DZG_v3_T * localDW ) ; void DZG_v3_Subsystempi2delay (
SimStruct * S , uint8_T rtu_Enable , const real_T rtu_alpha_beta [ 2 ] ,
real_T rtu_wt , B_Subsystempi2delay_DZG_v3_T * localB ,
DW_Subsystempi2delay_DZG_v3_T * localDW ) ; void DZG_v3_Subsystem1_Init (
SimStruct * S , B_Subsystem1_DZG_v3_T * localB , P_Subsystem1_DZG_v3_T *
localP ) ; void DZG_v3_Subsystem1_Disable ( SimStruct * S ,
DW_Subsystem1_DZG_v3_T * localDW ) ; void DZG_v3_Subsystem1 ( SimStruct * S ,
uint8_T rtu_Enable , const real_T rtu_alpha_beta [ 2 ] , real_T rtu_wt ,
B_Subsystem1_DZG_v3_T * localB , DW_Subsystem1_DZG_v3_T * localDW ) ; void
DZG_v3_NegSeqComputation_Init ( SimStruct * S , B_NegSeqComputation_DZG_v3_T
* localB , P_NegSeqComputation_DZG_v3_T * localP ) ; void
DZG_v3_NegSeqComputation_Disable ( SimStruct * S ,
DW_NegSeqComputation_DZG_v3_T * localDW ) ; void DZG_v3_NegSeqComputation (
SimStruct * S , real_T rtu_Enable , creal_T rtu_In , creal_T rtu_In_a ,
creal_T rtu_In_d , B_NegSeqComputation_DZG_v3_T * localB ,
DW_NegSeqComputation_DZG_v3_T * localDW , P_NegSeqComputation_DZG_v3_T *
localP ) ; void DZG_v3_ZeroSeqComputation_Init ( SimStruct * S ,
B_ZeroSeqComputation_DZG_v3_T * localB , P_ZeroSeqComputation_DZG_v3_T *
localP ) ; void DZG_v3_ZeroSeqComputation_Disable ( SimStruct * S ,
DW_ZeroSeqComputation_DZG_v3_T * localDW ) ; void DZG_v3_ZeroSeqComputation (
SimStruct * S , real_T rtu_Enable , creal_T rtu_In , creal_T rtu_In_p ,
creal_T rtu_In_j , B_ZeroSeqComputation_DZG_v3_T * localB ,
DW_ZeroSeqComputation_DZG_v3_T * localDW , P_ZeroSeqComputation_DZG_v3_T *
localP ) ;
#endif
