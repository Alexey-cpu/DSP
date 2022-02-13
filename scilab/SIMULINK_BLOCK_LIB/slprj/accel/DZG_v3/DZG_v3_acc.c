#include <math.h>
#include "DZG_v3_acc.h"
#include "DZG_v3_acc_private.h"
#include <stdio.h>
#include "slexec_vm_simstruct_bridge.h"
#include "slexec_vm_zc_functions.h"
#include "slexec_vm_lookup_functions.h"
#include "slsv_diagnostic_codegen_c_api.h"
#include "simtarget/slSimTgtMdlrefSfcnBridge.h"
#include "simstruc.h"
#include "fixedpoint.h"
#define CodeFormat S-Function
#define AccDefine1 Accelerator_S-Function
#include "simtarget/slAccSfcnBridge.h"
#ifndef __RTW_UTFREE__  
extern void * utMalloc ( size_t ) ; extern void utFree ( void * ) ;
#endif
boolean_T DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( int_T * bufSzPtr , int_T
* tailPtr , int_T * headPtr , int_T * lastPtr , real_T tMinusDelay , real_T *
* tBufPtr , real_T * * uBufPtr , real_T * * xBufPtr , boolean_T isfixedbuf ,
boolean_T istransportdelay , int_T * maxNewBufSzPtr ) { int_T testIdx ; int_T
tail = * tailPtr ; int_T bufSz = * bufSzPtr ; real_T * tBuf = * tBufPtr ;
real_T * xBuf = ( NULL ) ; int_T numBuffer = 2 ; if ( istransportdelay ) {
numBuffer = 3 ; xBuf = * xBufPtr ; } testIdx = ( tail < ( bufSz - 1 ) ) ? (
tail + 1 ) : 0 ; if ( ( tMinusDelay <= tBuf [ testIdx ] ) && ! isfixedbuf ) {
int_T j ; real_T * tempT ; real_T * tempU ; real_T * tempX = ( NULL ) ;
real_T * uBuf = * uBufPtr ; int_T newBufSz = bufSz + 1024 ; if ( newBufSz > *
maxNewBufSzPtr ) { * maxNewBufSzPtr = newBufSz ; } tempU = ( real_T * )
utMalloc ( numBuffer * newBufSz * sizeof ( real_T ) ) ; if ( tempU == ( NULL
) ) { return ( false ) ; } tempT = tempU + newBufSz ; if ( istransportdelay )
tempX = tempT + newBufSz ; for ( j = tail ; j < bufSz ; j ++ ) { tempT [ j -
tail ] = tBuf [ j ] ; tempU [ j - tail ] = uBuf [ j ] ; if ( istransportdelay
) tempX [ j - tail ] = xBuf [ j ] ; } for ( j = 0 ; j < tail ; j ++ ) { tempT
[ j + bufSz - tail ] = tBuf [ j ] ; tempU [ j + bufSz - tail ] = uBuf [ j ] ;
if ( istransportdelay ) tempX [ j + bufSz - tail ] = xBuf [ j ] ; } if ( *
lastPtr > tail ) { * lastPtr -= tail ; } else { * lastPtr += ( bufSz - tail )
; } * tailPtr = 0 ; * headPtr = bufSz ; utFree ( uBuf ) ; * bufSzPtr =
newBufSz ; * tBufPtr = tempT ; * uBufPtr = tempU ; if ( istransportdelay ) *
xBufPtr = tempX ; } else { * tailPtr = testIdx ; } return ( true ) ; } real_T
DZG_v3_acc_rt_TDelayInterpolate ( real_T tMinusDelay , real_T tStart , real_T
* tBuf , real_T * uBuf , int_T bufSz , int_T * lastIdx , int_T oldestIdx ,
int_T newIdx , real_T initOutput , boolean_T discrete , boolean_T
minorStepAndTAtLastMajorOutput ) { int_T i ; real_T yout , t1 , t2 , u1 , u2
; if ( ( newIdx == 0 ) && ( oldestIdx == 0 ) && ( tMinusDelay > tStart ) )
return initOutput ; if ( tMinusDelay <= tStart ) return initOutput ; if ( (
tMinusDelay <= tBuf [ oldestIdx ] ) ) { if ( discrete ) { return ( uBuf [
oldestIdx ] ) ; } else { int_T tempIdx = oldestIdx + 1 ; if ( oldestIdx ==
bufSz - 1 ) tempIdx = 0 ; t1 = tBuf [ oldestIdx ] ; t2 = tBuf [ tempIdx ] ;
u1 = uBuf [ oldestIdx ] ; u2 = uBuf [ tempIdx ] ; if ( t2 == t1 ) { if (
tMinusDelay >= t2 ) { yout = u2 ; } else { yout = u1 ; } } else { real_T f1 =
( t2 - tMinusDelay ) / ( t2 - t1 ) ; real_T f2 = 1.0 - f1 ; yout = f1 * u1 +
f2 * u2 ; } return yout ; } } if ( minorStepAndTAtLastMajorOutput ) { if (
newIdx != 0 ) { if ( * lastIdx == newIdx ) { ( * lastIdx ) -- ; } newIdx -- ;
} else { if ( * lastIdx == newIdx ) { * lastIdx = bufSz - 1 ; } newIdx =
bufSz - 1 ; } } i = * lastIdx ; if ( tBuf [ i ] < tMinusDelay ) { while (
tBuf [ i ] < tMinusDelay ) { if ( i == newIdx ) break ; i = ( i < ( bufSz - 1
) ) ? ( i + 1 ) : 0 ; } } else { while ( tBuf [ i ] >= tMinusDelay ) { i = (
i > 0 ) ? i - 1 : ( bufSz - 1 ) ; } i = ( i < ( bufSz - 1 ) ) ? ( i + 1 ) : 0
; } * lastIdx = i ; if ( discrete ) { double tempEps = ( DBL_EPSILON ) *
128.0 ; double localEps = tempEps * muDoubleScalarAbs ( tBuf [ i ] ) ; if (
tempEps > localEps ) { localEps = tempEps ; } localEps = localEps / 2.0 ; if
( tMinusDelay >= ( tBuf [ i ] - localEps ) ) { yout = uBuf [ i ] ; } else {
if ( i == 0 ) { yout = uBuf [ bufSz - 1 ] ; } else { yout = uBuf [ i - 1 ] ;
} } } else { if ( i == 0 ) { t1 = tBuf [ bufSz - 1 ] ; u1 = uBuf [ bufSz - 1
] ; } else { t1 = tBuf [ i - 1 ] ; u1 = uBuf [ i - 1 ] ; } t2 = tBuf [ i ] ;
u2 = uBuf [ i ] ; if ( t2 == t1 ) { if ( tMinusDelay >= t2 ) { yout = u2 ; }
else { yout = u1 ; } } else { real_T f1 = ( t2 - tMinusDelay ) / ( t2 - t1 )
; real_T f2 = 1.0 - f1 ; yout = f1 * u1 + f2 * u2 ; } } return ( yout ) ; }
real_T DZG_v3_acc_rt_VTDelayfindtDInterpolate ( real_T x , real_T * tBuf ,
real_T * uBuf , real_T * xBuf , int_T bufSz , int_T head , int_T tail , int_T
* pLast , real_T t , real_T tStart , boolean_T discrete , boolean_T
minorStepAndTAtLastMajorOutput , real_T initOutput , real_T * appliedDelay )
{ int_T n , k ; real_T f ; int_T kp1 ; real_T tminustD , tL , tR , uD , uL ,
uR , fU ; if ( minorStepAndTAtLastMajorOutput ) { if ( * pLast == head ) { *
pLast = ( * pLast == 0 ) ? bufSz - 1 : * pLast - 1 ; } head = ( head == 0 ) ?
bufSz - 1 : head - 1 ; } if ( x <= 1 ) { return initOutput ; } k = * pLast ;
n = 0 ; for ( ; ; ) { n ++ ; if ( n > bufSz ) break ; if ( x - xBuf [ k ] >
1.0 ) { if ( k == head ) { int_T km1 ; f = ( x - 1.0 - xBuf [ k ] ) / ( x -
xBuf [ k ] ) ; tminustD = ( 1.0 - f ) * tBuf [ k ] + f * t ; km1 = k - 1 ; if
( km1 < 0 ) km1 = bufSz - 1 ; tL = tBuf [ km1 ] ; tR = tBuf [ k ] ; uL = uBuf
[ km1 ] ; uR = uBuf [ k ] ; break ; } kp1 = k + 1 ; if ( kp1 == bufSz ) kp1 =
0 ; if ( x - xBuf [ kp1 ] <= 1.0 ) { f = ( x - 1.0 - xBuf [ k ] ) / ( xBuf [
kp1 ] - xBuf [ k ] ) ; tL = tBuf [ k ] ; tR = tBuf [ kp1 ] ; uL = uBuf [ k ]
; uR = uBuf [ kp1 ] ; tminustD = ( 1.0 - f ) * tL + f * tR ; break ; } k =
kp1 ; } else { if ( k == tail ) { f = ( x - 1.0 ) / xBuf [ k ] ; if (
discrete ) { return ( uBuf [ tail ] ) ; } kp1 = k + 1 ; if ( kp1 == bufSz )
kp1 = 0 ; tminustD = ( 1 - f ) * tStart + f * tBuf [ k ] ; tL = tBuf [ k ] ;
tR = tBuf [ kp1 ] ; uL = uBuf [ k ] ; uR = uBuf [ kp1 ] ; break ; } k = k - 1
; if ( k < 0 ) k = bufSz - 1 ; } } * pLast = k ; if ( tR == tL ) { fU = 1.0 ;
} else { fU = ( tminustD - tL ) / ( tR - tL ) ; } if ( discrete ) { uD = ( fU
> ( 1.0 - fU ) ) ? uR : uL ; } else { uD = ( 1.0 - fU ) * uL + fU * uR ; } *
appliedDelay = t - tminustD ; return uD ; } void rt_ssGetBlockPath (
SimStruct * S , int_T sysIdx , int_T blkIdx , char_T * * path ) {
_ssGetBlockPath ( S , sysIdx , blkIdx , path ) ; } void rt_ssSet_slErrMsg (
SimStruct * S , void * diag ) { if ( ! _ssIsErrorStatusAslErrMsg ( S ) ) {
_ssSet_slErrMsg ( S , diag ) ; } else { _ssDiscardDiagnostic ( S , diag ) ; }
} void rt_ssReportDiagnosticAsWarning ( SimStruct * S , void * diag ) {
_ssReportDiagnosticAsWarning ( S , diag ) ; } void
DZG_v3_TriggeredSubsystem_Init ( SimStruct * S ,
B_TriggeredSubsystem_DZG_v3_T * localB , P_TriggeredSubsystem_DZG_v3_T *
localP ) { localB -> B_83_0_0 = localP -> P_0 ; } void
DZG_v3_TriggeredSubsystem ( SimStruct * S , boolean_T rtu_Trigger , real_T
rtu_In1 , B_TriggeredSubsystem_DZG_v3_T * localB ,
DW_TriggeredSubsystem_DZG_v3_T * localDW , ZCE_TriggeredSubsystem_DZG_v3_T *
localZCE ) { boolean_T zcEvent ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
zcEvent = ( rtu_Trigger && ( localZCE -> TriggeredSubsystem_Trig_ZCE !=
POS_ZCSIG ) ) ; if ( zcEvent ) { localB -> B_83_0_0 = rtu_In1 ; localDW ->
TriggeredSubsystem_SubsysRanBC = 4 ; } localZCE ->
TriggeredSubsystem_Trig_ZCE = rtu_Trigger ; } } void
DZG_v3_TriggeredSubsystem_Term ( SimStruct * const S ) { } void
DZG_v3_NEGATIVEEdge_Init ( SimStruct * S , B_NEGATIVEEdge_DZG_v3_T * localB ,
P_NEGATIVEEdge_DZG_v3_T * localP ) { localB -> B_84_0_0 = localP -> P_0 ; }
void DZG_v3_NEGATIVEEdge_Disable ( SimStruct * S , DW_NEGATIVEEdge_DZG_v3_T *
localDW ) { localDW -> NEGATIVEEdge_MODE = false ; } void DZG_v3_NEGATIVEEdge
( SimStruct * S , real_T rtu_Enable , boolean_T rtu_IN , boolean_T
rtu_INprevious , B_NEGATIVEEdge_DZG_v3_T * localB , DW_NEGATIVEEdge_DZG_v3_T
* localDW ) { int32_T isHit ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( (
isHit != 0 ) && ( ssIsMajorTimeStep ( S ) != 0 ) ) { if ( rtu_Enable > 0.0 )
{ if ( ! localDW -> NEGATIVEEdge_MODE ) { if ( ssGetTaskTime ( S , 1 ) !=
ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; }
localDW -> NEGATIVEEdge_MODE = true ; } } else { if ( localDW ->
NEGATIVEEdge_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
DZG_v3_NEGATIVEEdge_Disable ( S , localDW ) ; } } } if ( localDW ->
NEGATIVEEdge_MODE ) { localB -> B_84_0_0 = ( ( int32_T ) rtu_INprevious > (
int32_T ) rtu_IN ) ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC (
localDW -> NEGATIVEEdge_SubsysRanBC ) ; } } } void DZG_v3_NEGATIVEEdge_Term (
SimStruct * const S ) { } void DZG_v3_POSITIVEEdge_Init ( SimStruct * S ,
B_POSITIVEEdge_DZG_v3_T * localB , P_POSITIVEEdge_DZG_v3_T * localP ) {
localB -> B_85_0_0 = localP -> P_0 ; } void DZG_v3_POSITIVEEdge_Disable (
SimStruct * S , DW_POSITIVEEdge_DZG_v3_T * localDW ) { localDW ->
POSITIVEEdge_MODE = false ; } void DZG_v3_POSITIVEEdge ( SimStruct * S ,
real_T rtu_Enable , boolean_T rtu_IN , boolean_T rtu_INprevious ,
B_POSITIVEEdge_DZG_v3_T * localB , DW_POSITIVEEdge_DZG_v3_T * localDW ) {
int32_T isHit ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) &&
( ssIsMajorTimeStep ( S ) != 0 ) ) { if ( rtu_Enable > 0.0 ) { if ( ! localDW
-> POSITIVEEdge_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) )
{ ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
POSITIVEEdge_MODE = true ; } } else { if ( localDW -> POSITIVEEdge_MODE ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
DZG_v3_POSITIVEEdge_Disable ( S , localDW ) ; } } } if ( localDW ->
POSITIVEEdge_MODE ) { localB -> B_85_0_0 = ( ( int32_T ) rtu_INprevious < (
int32_T ) rtu_IN ) ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC (
localDW -> POSITIVEEdge_SubsysRanBC ) ; } } } void DZG_v3_POSITIVEEdge_Term (
SimStruct * const S ) { } void DZG_v3_OFFDelay_Init ( SimStruct * S ,
B_OFFDelay_DZG_v3_T * localB , DW_OFFDelay_DZG_v3_T * localDW ,
P_OFFDelay_DZG_v3_T * localP ) { localDW -> Memory_PreviousInput = localP ->
P_7 ; localDW -> ICic_PreviousInput = localP -> P_0 ;
DZG_v3_POSITIVEEdge_Init ( S , & localB -> POSITIVEEdge , & localP ->
POSITIVEEdge ) ; DZG_v3_NEGATIVEEdge_Init ( S , & localB -> NEGATIVEEdge , &
localP -> NEGATIVEEdge ) ; DZG_v3_TriggeredSubsystem_Init ( S , & localB ->
TriggeredSubsystem , & localP -> TriggeredSubsystem ) ; localB -> B_86_19_0 =
localP -> P_6 ; } void DZG_v3_OFFDelay_Disable ( SimStruct * S ,
DW_OFFDelay_DZG_v3_T * localDW ) { if ( localDW -> POSITIVEEdge .
POSITIVEEdge_MODE ) { DZG_v3_POSITIVEEdge_Disable ( S , & localDW ->
POSITIVEEdge ) ; } if ( localDW -> NEGATIVEEdge . NEGATIVEEdge_MODE ) {
DZG_v3_NEGATIVEEdge_Disable ( S , & localDW -> NEGATIVEEdge ) ; } localDW ->
OFFDelay_MODE = false ; } void DZG_v3_OFFDelay ( SimStruct * S , boolean_T
rtu_Enable , boolean_T rtu_IN , real_T rtu_clock , real_T rtu_DELAY ,
B_OFFDelay_DZG_v3_T * localB , DW_OFFDelay_DZG_v3_T * localDW ,
ZCE_OFFDelay_DZG_v3_T * localZCE ) { int32_T isHit ; isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S ) != 0 ) ) { if
( rtu_Enable ) { if ( ! localDW -> OFFDelay_MODE ) { if ( ssGetTaskTime ( S ,
1 ) != ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S )
; } localDW -> OFFDelay_MODE = true ; } } else { if ( localDW ->
OFFDelay_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
DZG_v3_OFFDelay_Disable ( S , localDW ) ; } } } if ( localDW -> OFFDelay_MODE
) { localB -> B_86_0_0 = ssGetT ( S ) ; localB -> B_86_1_0_c = rtu_IN ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { localB -> B_86_2_0_b =
localDW -> Memory_PreviousInput ; localB -> B_86_3_0 = localB -> B_86_5_0_k [
0 ] ; } DZG_v3_POSITIVEEdge ( S , localB -> B_86_3_0 , localB -> B_86_1_0_c ,
localB -> B_86_2_0_b , & localB -> POSITIVEEdge , & localDW -> POSITIVEEdge )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { localB ->
B_86_5_0 = localB -> B_86_5_0_k [ 1 ] ; } DZG_v3_NEGATIVEEdge ( S , localB ->
B_86_5_0 , localB -> B_86_1_0_c , localB -> B_86_2_0_b , & localB ->
NEGATIVEEdge , & localDW -> NEGATIVEEdge ) ; localB -> B_86_7_0 = ( localB ->
POSITIVEEdge . B_85_0_0 || localB -> NEGATIVEEdge . B_84_0_0 ) ; localB ->
B_86_8_0 = localB -> B_86_7_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { DZG_v3_TriggeredSubsystem ( S , localB -> B_86_8_0 , localB ->
B_86_0_0 , & localB -> TriggeredSubsystem , & localDW -> TriggeredSubsystem ,
& localZCE -> TriggeredSubsystem ) ; localB -> B_86_10_0 = localB ->
TriggeredSubsystem . B_83_0_0 + localB -> B_86_0_0_m ; } localB -> B_86_11_0
= ( localB -> B_86_10_0 > localB -> B_86_0_0 ) ; localB -> B_86_12_0 = !
rtu_IN ; localB -> B_86_13_0 = localB -> B_86_7_0 ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { localB -> B_86_14_0 = localDW ->
ICic_PreviousInput ; } if ( localB -> B_86_13_0 ) { localB -> B_86_15_0 =
rtu_clock ; } else { localB -> B_86_15_0 = localB -> B_86_14_0 ; } localB ->
B_86_16_0 = localB -> B_86_15_0 + rtu_DELAY ; localB -> B_86_17_0 = (
rtu_clock >= localB -> B_86_16_0 ) ; localB -> B_86_18_0 = ( localB ->
B_86_17_0 && localB -> B_86_12_0 ) ; localB -> B_86_19_0 = ! localB ->
B_86_18_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
OFFDelay_SubsysRanBC ) ; } } } void DZG_v3_OFFDelayTID2 ( SimStruct * S ,
B_OFFDelay_DZG_v3_T * localB , DW_OFFDelay_DZG_v3_T * localDW ,
P_OFFDelay_DZG_v3_T * localP ) { localB -> B_86_0_0_m = localP -> P_1 ;
localB -> B_86_1_0 = localP -> P_2 ; localB -> B_86_2_0 [ 0 ] = localP -> P_3
[ 0 ] ; localB -> B_86_3_0_c [ 0 ] = localP -> P_4 [ 0 ] ; localB -> B_86_4_0
[ 0 ] = localP -> P_5 [ 0 ] ; localB -> B_86_2_0 [ 1 ] = localP -> P_3 [ 1 ]
; localB -> B_86_3_0_c [ 1 ] = localP -> P_4 [ 1 ] ; localB -> B_86_4_0 [ 1 ]
= localP -> P_5 [ 1 ] ; switch ( ( int32_T ) localB -> B_86_1_0 ) { case 1 :
localB -> B_86_5_0_k [ 0 ] = localB -> B_86_2_0 [ 0 ] ; localB -> B_86_5_0_k
[ 1 ] = localB -> B_86_2_0 [ 1 ] ; break ; case 2 : localB -> B_86_5_0_k [ 0
] = localB -> B_86_3_0_c [ 0 ] ; localB -> B_86_5_0_k [ 1 ] = localB ->
B_86_3_0_c [ 1 ] ; break ; default : localB -> B_86_5_0_k [ 0 ] = localB ->
B_86_4_0 [ 0 ] ; localB -> B_86_5_0_k [ 1 ] = localB -> B_86_4_0 [ 1 ] ;
break ; } if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
OFFDelay_SubsysRanBC ) ; } } void DZG_v3_OFFDelay_Update ( SimStruct * S ,
B_OFFDelay_DZG_v3_T * localB , DW_OFFDelay_DZG_v3_T * localDW ) { int32_T
isHit ; if ( localDW -> OFFDelay_MODE ) { isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { localDW -> Memory_PreviousInput = localB -> B_86_1_0_c
; localDW -> ICic_PreviousInput = localB -> B_86_15_0 ; } } } void
DZG_v3_OFFDelay_Term ( SimStruct * const S ) { } void DZG_v3_ONDelay_Init (
SimStruct * S , B_ONDelay_DZG_v3_T * localB , DW_ONDelay_DZG_v3_T * localDW ,
P_ONDelay_DZG_v3_T * localP ) { localDW -> Memory_PreviousInput = localP ->
P_7 ; localDW -> ICic_PreviousInput = localP -> P_0 ;
DZG_v3_POSITIVEEdge_Init ( S , & localB -> POSITIVEEdge , & localP ->
POSITIVEEdge ) ; DZG_v3_NEGATIVEEdge_Init ( S , & localB -> NEGATIVEEdge , &
localP -> NEGATIVEEdge ) ; DZG_v3_TriggeredSubsystem_Init ( S , & localB ->
TriggeredSubsystem , & localP -> TriggeredSubsystem ) ; localB -> B_90_17_0 =
localP -> P_6 ; } void DZG_v3_ONDelay_Disable ( SimStruct * S ,
DW_ONDelay_DZG_v3_T * localDW ) { if ( localDW -> POSITIVEEdge .
POSITIVEEdge_MODE ) { DZG_v3_POSITIVEEdge_Disable ( S , & localDW ->
POSITIVEEdge ) ; } if ( localDW -> NEGATIVEEdge . NEGATIVEEdge_MODE ) {
DZG_v3_NEGATIVEEdge_Disable ( S , & localDW -> NEGATIVEEdge ) ; } localDW ->
ONDelay_MODE = false ; } void DZG_v3_ONDelay ( SimStruct * S , boolean_T
rtu_Enable , boolean_T rtu_IN , real_T rtu_clock , real_T rtu_DELAY ,
B_ONDelay_DZG_v3_T * localB , DW_ONDelay_DZG_v3_T * localDW ,
ZCE_ONDelay_DZG_v3_T * localZCE ) { int32_T isHit ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S ) != 0 ) ) { if (
rtu_Enable ) { if ( ! localDW -> ONDelay_MODE ) { if ( ssGetTaskTime ( S , 1
) != ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
} localDW -> ONDelay_MODE = true ; } } else { if ( localDW -> ONDelay_MODE )
{ ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; DZG_v3_ONDelay_Disable (
S , localDW ) ; } } } if ( localDW -> ONDelay_MODE ) { localB -> B_90_0_0 =
ssGetT ( S ) ; localB -> B_90_1_0_c = rtu_IN ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { localB -> B_90_2_0_b = localDW ->
Memory_PreviousInput ; localB -> B_90_3_0 = localB -> B_90_5_0_k [ 0 ] ; }
DZG_v3_POSITIVEEdge ( S , localB -> B_90_3_0 , localB -> B_90_1_0_c , localB
-> B_90_2_0_b , & localB -> POSITIVEEdge , & localDW -> POSITIVEEdge ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { localB -> B_90_5_0
= localB -> B_90_5_0_k [ 1 ] ; } DZG_v3_NEGATIVEEdge ( S , localB -> B_90_5_0
, localB -> B_90_1_0_c , localB -> B_90_2_0_b , & localB -> NEGATIVEEdge , &
localDW -> NEGATIVEEdge ) ; localB -> B_90_7_0 = ( localB -> POSITIVEEdge .
B_85_0_0 || localB -> NEGATIVEEdge . B_84_0_0 ) ; localB -> B_90_8_0 = localB
-> B_90_7_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
DZG_v3_TriggeredSubsystem ( S , localB -> B_90_8_0 , localB -> B_90_0_0 , &
localB -> TriggeredSubsystem , & localDW -> TriggeredSubsystem , & localZCE
-> TriggeredSubsystem ) ; localB -> B_90_10_0 = localB -> TriggeredSubsystem
. B_83_0_0 + localB -> B_90_0_0_m ; } localB -> B_90_11_0 = ( localB ->
B_90_10_0 > localB -> B_90_0_0 ) ; localB -> B_90_12_0 = localB -> B_90_7_0 ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { localB -> B_90_13_0
= localDW -> ICic_PreviousInput ; } if ( localB -> B_90_12_0 ) { localB ->
B_90_14_0 = rtu_clock ; } else { localB -> B_90_14_0 = localB -> B_90_13_0 ;
} localB -> B_90_15_0 = localB -> B_90_14_0 + rtu_DELAY ; localB -> B_90_16_0
= ( rtu_clock >= localB -> B_90_15_0 ) ; localB -> B_90_17_0 = ( localB ->
B_90_16_0 && rtu_IN ) ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC (
localDW -> ONDelay_SubsysRanBC ) ; } } } void DZG_v3_ONDelayTID2 ( SimStruct
* S , B_ONDelay_DZG_v3_T * localB , DW_ONDelay_DZG_v3_T * localDW ,
P_ONDelay_DZG_v3_T * localP ) { localB -> B_90_0_0_m = localP -> P_1 ; localB
-> B_90_1_0 = localP -> P_2 ; localB -> B_90_2_0 [ 0 ] = localP -> P_3 [ 0 ]
; localB -> B_90_3_0_c [ 0 ] = localP -> P_4 [ 0 ] ; localB -> B_90_4_0 [ 0 ]
= localP -> P_5 [ 0 ] ; localB -> B_90_2_0 [ 1 ] = localP -> P_3 [ 1 ] ;
localB -> B_90_3_0_c [ 1 ] = localP -> P_4 [ 1 ] ; localB -> B_90_4_0 [ 1 ] =
localP -> P_5 [ 1 ] ; switch ( ( int32_T ) localB -> B_90_1_0 ) { case 1 :
localB -> B_90_5_0_k [ 0 ] = localB -> B_90_2_0 [ 0 ] ; localB -> B_90_5_0_k
[ 1 ] = localB -> B_90_2_0 [ 1 ] ; break ; case 2 : localB -> B_90_5_0_k [ 0
] = localB -> B_90_3_0_c [ 0 ] ; localB -> B_90_5_0_k [ 1 ] = localB ->
B_90_3_0_c [ 1 ] ; break ; default : localB -> B_90_5_0_k [ 0 ] = localB ->
B_90_4_0 [ 0 ] ; localB -> B_90_5_0_k [ 1 ] = localB -> B_90_4_0 [ 1 ] ;
break ; } if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
ONDelay_SubsysRanBC ) ; } } void DZG_v3_ONDelay_Update ( SimStruct * S ,
B_ONDelay_DZG_v3_T * localB , DW_ONDelay_DZG_v3_T * localDW ) { int32_T isHit
; if ( localDW -> ONDelay_MODE ) { isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { localDW -> Memory_PreviousInput = localB -> B_90_1_0_c ;
localDW -> ICic_PreviousInput = localB -> B_90_14_0 ; } } } void
DZG_v3_ONDelay_Term ( SimStruct * const S ) { } void
DZG_v3_Subsystempi2delay_Init ( SimStruct * S , B_Subsystempi2delay_DZG_v3_T
* localB , P_Subsystempi2delay_DZG_v3_T * localP ) { localB -> B_942_0_0 =
localP -> P_0 [ 0 ] ; localB -> B_942_1_0 = localP -> P_0 [ 1 ] ; } void
DZG_v3_Subsystempi2delay_Disable ( SimStruct * S ,
DW_Subsystempi2delay_DZG_v3_T * localDW ) { localDW -> Subsystempi2delay_MODE
= false ; } void DZG_v3_Subsystempi2delay ( SimStruct * S , uint8_T
rtu_Enable , const real_T rtu_alpha_beta [ 2 ] , real_T rtu_wt ,
B_Subsystempi2delay_DZG_v3_T * localB , DW_Subsystempi2delay_DZG_v3_T *
localDW ) { int32_T isHit ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( (
isHit != 0 ) && ( ssIsMajorTimeStep ( S ) != 0 ) ) { if ( rtu_Enable > 0 ) {
if ( ! localDW -> Subsystempi2delay_MODE ) { if ( ssGetTaskTime ( S , 1 ) !=
ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; }
localDW -> Subsystempi2delay_MODE = true ; } } else { if ( localDW ->
Subsystempi2delay_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
DZG_v3_Subsystempi2delay_Disable ( S , localDW ) ; } } } if ( localDW ->
Subsystempi2delay_MODE ) { localB -> B_942_0_0 = rtu_alpha_beta [ 0 ] *
muDoubleScalarSin ( rtu_wt ) - rtu_alpha_beta [ 1 ] * muDoubleScalarCos (
rtu_wt ) ; localB -> B_942_1_0 = rtu_alpha_beta [ 0 ] * muDoubleScalarCos (
rtu_wt ) + rtu_alpha_beta [ 1 ] * muDoubleScalarSin ( rtu_wt ) ; if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
Subsystempi2delay_SubsysRanBC ) ; } } } void DZG_v3_Subsystempi2delay_Term (
SimStruct * const S ) { } void DZG_v3_Subsystem1_Init ( SimStruct * S ,
B_Subsystem1_DZG_v3_T * localB , P_Subsystem1_DZG_v3_T * localP ) { localB ->
B_943_0_0 = localP -> P_0 [ 0 ] ; localB -> B_943_1_0 = localP -> P_0 [ 1 ] ;
} void DZG_v3_Subsystem1_Disable ( SimStruct * S , DW_Subsystem1_DZG_v3_T *
localDW ) { localDW -> Subsystem1_MODE = false ; } void DZG_v3_Subsystem1 (
SimStruct * S , uint8_T rtu_Enable , const real_T rtu_alpha_beta [ 2 ] ,
real_T rtu_wt , B_Subsystem1_DZG_v3_T * localB , DW_Subsystem1_DZG_v3_T *
localDW ) { int32_T isHit ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( (
isHit != 0 ) && ( ssIsMajorTimeStep ( S ) != 0 ) ) { if ( rtu_Enable > 0 ) {
if ( ! localDW -> Subsystem1_MODE ) { if ( ssGetTaskTime ( S , 1 ) !=
ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; }
localDW -> Subsystem1_MODE = true ; } } else { if ( localDW ->
Subsystem1_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
DZG_v3_Subsystem1_Disable ( S , localDW ) ; } } } if ( localDW ->
Subsystem1_MODE ) { localB -> B_943_0_0 = rtu_alpha_beta [ 0 ] *
muDoubleScalarCos ( rtu_wt ) + rtu_alpha_beta [ 1 ] * muDoubleScalarSin (
rtu_wt ) ; localB -> B_943_1_0 = - rtu_alpha_beta [ 0 ] * muDoubleScalarSin (
rtu_wt ) + rtu_alpha_beta [ 1 ] * muDoubleScalarCos ( rtu_wt ) ; if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
Subsystem1_SubsysRanBC ) ; } } } void DZG_v3_Subsystem1_Term ( SimStruct *
const S ) { } void DZG_v3_NegSeqComputation_Init ( SimStruct * S ,
B_NegSeqComputation_DZG_v3_T * localB , P_NegSeqComputation_DZG_v3_T * localP
) { localB -> B_982_2_0 . re = localP -> P_0 ; localB -> B_982_2_0 . im = 0.0
; } void DZG_v3_NegSeqComputation_Disable ( SimStruct * S ,
DW_NegSeqComputation_DZG_v3_T * localDW ) { localDW -> NegSeqComputation_MODE
= false ; } void DZG_v3_NegSeqComputation ( SimStruct * S , real_T rtu_Enable
, creal_T rtu_In , creal_T rtu_In_a , creal_T rtu_In_d ,
B_NegSeqComputation_DZG_v3_T * localB , DW_NegSeqComputation_DZG_v3_T *
localDW , P_NegSeqComputation_DZG_v3_T * localP ) { real_T re ; real_T im ;
int32_T isHit ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) &&
( ssIsMajorTimeStep ( S ) != 0 ) ) { if ( rtu_Enable > 0.0 ) { if ( ! localDW
-> NegSeqComputation_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S
) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
NegSeqComputation_MODE = true ; } } else { if ( localDW ->
NegSeqComputation_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
DZG_v3_NegSeqComputation_Disable ( S , localDW ) ; } } } if ( localDW ->
NegSeqComputation_MODE ) { re = localP -> P_2 [ 0 ] . re * rtu_In . re -
localP -> P_2 [ 0 ] . im * rtu_In . im ; im = localP -> P_2 [ 0 ] . re *
rtu_In . im + localP -> P_2 [ 0 ] . im * rtu_In . re ; localB -> B_982_0_0 [
0 ] . re = re ; localB -> B_982_0_0 [ 0 ] . im = im ; re = localP -> P_2 [ 1
] . re * rtu_In_a . re - localP -> P_2 [ 1 ] . im * rtu_In_a . im ; im =
localP -> P_2 [ 1 ] . re * rtu_In_a . im + localP -> P_2 [ 1 ] . im *
rtu_In_a . re ; localB -> B_982_0_0 [ 1 ] . re = re ; localB -> B_982_0_0 [ 1
] . im = im ; re = localP -> P_2 [ 2 ] . re * rtu_In_d . re - localP -> P_2 [
2 ] . im * rtu_In_d . im ; im = localP -> P_2 [ 2 ] . re * rtu_In_d . im +
localP -> P_2 [ 2 ] . im * rtu_In_d . re ; localB -> B_982_0_0 [ 2 ] . re =
re ; localB -> B_982_0_0 [ 2 ] . im = im ; re = localB -> B_982_0_0 [ 0 ] .
re ; im = localB -> B_982_0_0 [ 0 ] . im ; re += localB -> B_982_0_0 [ 1 ] .
re ; im += localB -> B_982_0_0 [ 1 ] . im ; re += localB -> B_982_0_0 [ 2 ] .
re ; im += localB -> B_982_0_0 [ 2 ] . im ; localB -> B_982_1_0 . re = re ;
localB -> B_982_1_0 . im = im ; localB -> B_982_2_0 . re = localP -> P_1 *
localB -> B_982_1_0 . re ; localB -> B_982_2_0 . im = localP -> P_1 * localB
-> B_982_1_0 . im ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC (
localDW -> NegSeqComputation_SubsysRanBC ) ; } } } void
DZG_v3_NegSeqComputation_Term ( SimStruct * const S ) { } void
DZG_v3_ZeroSeqComputation_Init ( SimStruct * S ,
B_ZeroSeqComputation_DZG_v3_T * localB , P_ZeroSeqComputation_DZG_v3_T *
localP ) { localB -> B_984_1_0 . re = localP -> P_0 ; localB -> B_984_1_0 .
im = 0.0 ; } void DZG_v3_ZeroSeqComputation_Disable ( SimStruct * S ,
DW_ZeroSeqComputation_DZG_v3_T * localDW ) { localDW ->
ZeroSeqComputation_MODE = false ; } void DZG_v3_ZeroSeqComputation (
SimStruct * S , real_T rtu_Enable , creal_T rtu_In , creal_T rtu_In_p ,
creal_T rtu_In_j , B_ZeroSeqComputation_DZG_v3_T * localB ,
DW_ZeroSeqComputation_DZG_v3_T * localDW , P_ZeroSeqComputation_DZG_v3_T *
localP ) { real_T tmpForInput_idx_0_re ; real_T tmpForInput_idx_0_im ; real_T
tmpForInput_idx_1_re ; real_T tmpForInput_idx_1_im ; real_T
tmpForInput_idx_2_re ; real_T tmpForInput_idx_2_im ; int32_T isHit ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S
) != 0 ) ) { if ( rtu_Enable > 0.0 ) { if ( ! localDW ->
ZeroSeqComputation_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S )
) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
ZeroSeqComputation_MODE = true ; } } else { if ( localDW ->
ZeroSeqComputation_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S )
; DZG_v3_ZeroSeqComputation_Disable ( S , localDW ) ; } } } if ( localDW ->
ZeroSeqComputation_MODE ) { tmpForInput_idx_0_re = rtu_In . re ;
tmpForInput_idx_0_im = rtu_In . im ; tmpForInput_idx_1_re = rtu_In_p . re ;
tmpForInput_idx_1_im = rtu_In_p . im ; tmpForInput_idx_2_re = rtu_In_j . re ;
tmpForInput_idx_2_im = rtu_In_j . im ; tmpForInput_idx_0_re +=
tmpForInput_idx_1_re ; tmpForInput_idx_0_im += tmpForInput_idx_1_im ;
tmpForInput_idx_0_re += tmpForInput_idx_2_re ; tmpForInput_idx_0_im +=
tmpForInput_idx_2_im ; localB -> B_984_0_0 . re = tmpForInput_idx_0_re ;
localB -> B_984_0_0 . im = tmpForInput_idx_0_im ; localB -> B_984_1_0 . re =
localP -> P_1 * localB -> B_984_0_0 . re ; localB -> B_984_1_0 . im = localP
-> P_1 * localB -> B_984_0_0 . im ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
srUpdateBC ( localDW -> ZeroSeqComputation_SubsysRanBC ) ; } } } void
DZG_v3_ZeroSeqComputation_Term ( SimStruct * const S ) { } static void
mdlOutputs ( SimStruct * S , int_T tid ) { real_T maxV ; boolean_T
didZcEventOccur ; real_T riseValLimit ; int32_T isHit ; real_T u0 ; real_T u2
; B_DZG_v3_T * _rtB ; P_DZG_v3_T * _rtP ; X_DZG_v3_T * _rtX ;
PrevZCX_DZG_v3_T * _rtZCE ; DW_DZG_v3_T * _rtDW ; _rtDW = ( ( DW_DZG_v3_T * )
ssGetRootDWork ( S ) ) ; _rtZCE = ( ( PrevZCX_DZG_v3_T * )
_ssGetPrevZCSigState ( S ) ) ; _rtX = ( ( X_DZG_v3_T * ) ssGetContStates ( S
) ) ; _rtP = ( ( P_DZG_v3_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( ( B_DZG_v3_T
* ) _ssGetModelBlockIO ( S ) ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { ssCallAccelRunBlock ( S , 1036 , 0 , SS_CALL_MDL_OUTPUTS ) ;
if ( _rtDW -> systemEnable != 0 ) { _rtDW -> lastSin = muDoubleScalarSin (
_rtP -> P_135 * ssGetTaskTime ( S , 1 ) ) ; _rtDW -> lastCos =
muDoubleScalarCos ( _rtP -> P_135 * ssGetTaskTime ( S , 1 ) ) ; _rtDW ->
systemEnable = 0 ; } _rtB -> B_1036_1_0 = ( ( _rtDW -> lastSin * _rtP ->
P_139 + _rtDW -> lastCos * _rtP -> P_138 ) * _rtP -> P_137 + ( _rtDW ->
lastCos * _rtP -> P_139 - _rtDW -> lastSin * _rtP -> P_138 ) * _rtP -> P_136
) * _rtP -> P_133 + _rtP -> P_134 ; if ( _rtDW -> systemEnable_b != 0 ) {
_rtDW -> lastSin_a = muDoubleScalarSin ( _rtP -> P_142 * ssGetTaskTime ( S ,
1 ) ) ; _rtDW -> lastCos_m = muDoubleScalarCos ( _rtP -> P_142 *
ssGetTaskTime ( S , 1 ) ) ; _rtDW -> systemEnable_b = 0 ; } _rtB ->
B_1036_2_0 = ( ( _rtDW -> lastSin_a * _rtP -> P_146 + _rtDW -> lastCos_m *
_rtP -> P_145 ) * _rtP -> P_144 + ( _rtDW -> lastCos_m * _rtP -> P_146 -
_rtDW -> lastSin_a * _rtP -> P_145 ) * _rtP -> P_143 ) * _rtP -> P_140 + _rtP
-> P_141 ; if ( _rtDW -> systemEnable_h != 0 ) { _rtDW -> lastSin_k =
muDoubleScalarSin ( _rtP -> P_149 * ssGetTaskTime ( S , 1 ) ) ; _rtDW ->
lastCos_k = muDoubleScalarCos ( _rtP -> P_149 * ssGetTaskTime ( S , 1 ) ) ;
_rtDW -> systemEnable_h = 0 ; } _rtB -> B_1036_3_0 = ( ( _rtDW -> lastSin_k *
_rtP -> P_153 + _rtDW -> lastCos_k * _rtP -> P_152 ) * _rtP -> P_151 + (
_rtDW -> lastCos_k * _rtP -> P_153 - _rtDW -> lastSin_k * _rtP -> P_152 ) *
_rtP -> P_150 ) * _rtP -> P_147 + _rtP -> P_148 ; if ( _rtDW ->
systemEnable_h1 != 0 ) { _rtDW -> lastSin_f = muDoubleScalarSin ( _rtP ->
P_156 * ssGetTaskTime ( S , 1 ) ) ; _rtDW -> lastCos_g = muDoubleScalarCos (
_rtP -> P_156 * ssGetTaskTime ( S , 1 ) ) ; _rtDW -> systemEnable_h1 = 0 ; }
_rtB -> B_1036_4_0 = ( ( _rtDW -> lastSin_f * _rtP -> P_160 + _rtDW ->
lastCos_g * _rtP -> P_159 ) * _rtP -> P_158 + ( _rtDW -> lastCos_g * _rtP ->
P_160 - _rtDW -> lastSin_f * _rtP -> P_159 ) * _rtP -> P_157 ) * _rtP ->
P_154 + _rtP -> P_155 ; if ( _rtDW -> systemEnable_f != 0 ) { _rtDW ->
lastSin_j = muDoubleScalarSin ( _rtP -> P_163 * ssGetTaskTime ( S , 1 ) ) ;
_rtDW -> lastCos_m2 = muDoubleScalarCos ( _rtP -> P_163 * ssGetTaskTime ( S ,
1 ) ) ; _rtDW -> systemEnable_f = 0 ; } _rtB -> B_1036_5_0 = ( ( _rtDW ->
lastSin_j * _rtP -> P_167 + _rtDW -> lastCos_m2 * _rtP -> P_166 ) * _rtP ->
P_165 + ( _rtDW -> lastCos_m2 * _rtP -> P_167 - _rtDW -> lastSin_j * _rtP ->
P_166 ) * _rtP -> P_164 ) * _rtP -> P_161 + _rtP -> P_162 ; if ( _rtDW ->
systemEnable_m != 0 ) { _rtDW -> lastSin_l = muDoubleScalarSin ( _rtP ->
P_170 * ssGetTaskTime ( S , 1 ) ) ; _rtDW -> lastCos_n = muDoubleScalarCos (
_rtP -> P_170 * ssGetTaskTime ( S , 1 ) ) ; _rtDW -> systemEnable_m = 0 ; }
_rtB -> B_1036_6_0 = ( ( _rtDW -> lastSin_l * _rtP -> P_174 + _rtDW ->
lastCos_n * _rtP -> P_173 ) * _rtP -> P_172 + ( _rtDW -> lastCos_n * _rtP ->
P_174 - _rtDW -> lastSin_l * _rtP -> P_173 ) * _rtP -> P_171 ) * _rtP ->
P_168 + _rtP -> P_169 ; ssCallAccelRunBlock ( S , 1036 , 7 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_8_0 = _rtX -> integrator_CSTATE ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_186 ; _rtB -> B_1036_9_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK . CircularBufSize , & _rtDW ->
TransportDelay_IWORK . Last , _rtDW -> TransportDelay_IWORK . Tail , _rtDW ->
TransportDelay_IWORK . Head , _rtP -> P_187 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_10_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_11_0 = _rtP -> P_188 ; } _rtB ->
B_1036_12_0_h = ( _rtB -> B_1036_10_0 >= _rtB -> B_1036_11_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_13_0 = _rtDW
-> Memory_PreviousInput ; } if ( _rtB -> B_1036_12_0_h ) { _rtB -> B_959_0_0
= _rtB -> B_1036_8_0 - _rtB -> B_1036_9_0 ; _rtB -> B_959_1_0 = _rtP -> P_66
* _rtB -> B_959_0_0 ; _rtB -> B_1036_15_0 = _rtB -> B_959_1_0 ; } else { _rtB
-> B_1036_15_0 = _rtB -> B_1036_13_0 ; } _rtB -> B_1036_16_0 = _rtX ->
integrator_CSTATE_c ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_a . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_a . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_191 ; _rtB ->
B_1036_17_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer
, * uBuffer , _rtDW -> TransportDelay_IWORK_p . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_p . Last , _rtDW -> TransportDelay_IWORK_p . Tail ,
_rtDW -> TransportDelay_IWORK_p . Head , _rtP -> P_192 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_18_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_19_0 = _rtP -> P_193 ; } _rtB ->
B_1036_20_0_j = ( _rtB -> B_1036_18_0 >= _rtB -> B_1036_19_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_21_0 = _rtDW
-> Memory_PreviousInput_i ; } if ( _rtB -> B_1036_20_0_j ) { _rtB ->
B_958_0_0 = _rtB -> B_1036_16_0 - _rtB -> B_1036_17_0 ; _rtB -> B_958_1_0 =
_rtP -> P_65 * _rtB -> B_958_0_0 ; _rtB -> B_1036_23_0 = _rtB -> B_958_1_0 ;
} else { _rtB -> B_1036_23_0 = _rtB -> B_1036_21_0 ; } _rtB -> B_1036_24_0 .
re = _rtB -> B_1036_15_0 ; _rtB -> B_1036_24_0 . im = _rtB -> B_1036_23_0 ;
_rtB -> B_1036_25_0 = muDoubleScalarHypot ( _rtB -> B_1036_24_0 . re , _rtB
-> B_1036_24_0 . im ) ; _rtB -> B_1036_25_1 = muDoubleScalarAtan2 ( _rtB ->
B_1036_24_0 . im , _rtB -> B_1036_24_0 . re ) ; _rtB -> B_1036_26_0 = _rtX ->
integrator_CSTATE_o ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_f . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_f . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_196 ; _rtB ->
B_1036_27_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer
, * uBuffer , _rtDW -> TransportDelay_IWORK_o . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_o . Last , _rtDW -> TransportDelay_IWORK_o . Tail ,
_rtDW -> TransportDelay_IWORK_o . Head , _rtP -> P_197 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_28_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_29_0 = _rtP -> P_198 ; } _rtB ->
B_1036_30_0_b = ( _rtB -> B_1036_28_0 >= _rtB -> B_1036_29_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_31_0 = _rtDW
-> Memory_PreviousInput_b ; } if ( _rtB -> B_1036_30_0_b ) { _rtB ->
B_965_0_0 = _rtB -> B_1036_26_0 - _rtB -> B_1036_27_0 ; _rtB -> B_965_1_0 =
_rtP -> P_72 * _rtB -> B_965_0_0 ; _rtB -> B_1036_33_0 = _rtB -> B_965_1_0 ;
} else { _rtB -> B_1036_33_0 = _rtB -> B_1036_31_0 ; } _rtB -> B_1036_34_0 =
_rtX -> integrator_CSTATE_p ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_b . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_b . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_201 ; _rtB
-> B_1036_35_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_g . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_g . Last , _rtDW -> TransportDelay_IWORK_g .
Tail , _rtDW -> TransportDelay_IWORK_g . Head , _rtP -> P_202 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_36_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_37_0 = _rtP
-> P_203 ; } _rtB -> B_1036_38_0_k = ( _rtB -> B_1036_36_0 >= _rtB ->
B_1036_37_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_39_0 = _rtDW -> Memory_PreviousInput_k ; } if ( _rtB ->
B_1036_38_0_k ) { _rtB -> B_964_0_0 = _rtB -> B_1036_34_0 - _rtB ->
B_1036_35_0 ; _rtB -> B_964_1_0 = _rtP -> P_71 * _rtB -> B_964_0_0 ; _rtB ->
B_1036_41_0 = _rtB -> B_964_1_0 ; } else { _rtB -> B_1036_41_0 = _rtB ->
B_1036_39_0 ; } _rtB -> B_1036_42_0 . re = _rtB -> B_1036_33_0 ; _rtB ->
B_1036_42_0 . im = _rtB -> B_1036_41_0 ; _rtB -> B_1036_43_0 =
muDoubleScalarHypot ( _rtB -> B_1036_42_0 . re , _rtB -> B_1036_42_0 . im ) ;
_rtB -> B_1036_43_1 = muDoubleScalarAtan2 ( _rtB -> B_1036_42_0 . im , _rtB
-> B_1036_42_0 . re ) ; _rtB -> B_1036_44_0 = _rtX -> integrator_CSTATE_i ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fr .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fr . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_206 ; _rtB -> B_1036_45_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_a . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_a . Last , _rtDW -> TransportDelay_IWORK_a . Tail ,
_rtDW -> TransportDelay_IWORK_a . Head , _rtP -> P_207 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_46_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_47_0 = _rtP -> P_208 ; } _rtB ->
B_1036_48_0_h = ( _rtB -> B_1036_46_0 >= _rtB -> B_1036_47_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_49_0 = _rtDW
-> Memory_PreviousInput_bv ; } if ( _rtB -> B_1036_48_0_h ) { _rtB ->
B_971_0_0 = _rtB -> B_1036_44_0 - _rtB -> B_1036_45_0 ; _rtB -> B_971_1_0 =
_rtP -> P_78 * _rtB -> B_971_0_0 ; _rtB -> B_1036_51_0 = _rtB -> B_971_1_0 ;
} else { _rtB -> B_1036_51_0 = _rtB -> B_1036_49_0 ; } _rtB -> B_1036_52_0 =
_rtX -> integrator_CSTATE_e ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_o . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_o . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_211 ; _rtB
-> B_1036_53_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_e . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_e . Last , _rtDW -> TransportDelay_IWORK_e .
Tail , _rtDW -> TransportDelay_IWORK_e . Head , _rtP -> P_212 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_54_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_55_0 = _rtP
-> P_213 ; } _rtB -> B_1036_56_0_d = ( _rtB -> B_1036_54_0 >= _rtB ->
B_1036_55_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_57_0 = _rtDW -> Memory_PreviousInput_d ; } if ( _rtB ->
B_1036_56_0_d ) { _rtB -> B_970_0_0 = _rtB -> B_1036_52_0 - _rtB ->
B_1036_53_0 ; _rtB -> B_970_1_0 = _rtP -> P_77 * _rtB -> B_970_0_0 ; _rtB ->
B_1036_59_0 = _rtB -> B_970_1_0 ; } else { _rtB -> B_1036_59_0 = _rtB ->
B_1036_57_0 ; } _rtB -> B_1036_60_0 . re = _rtB -> B_1036_51_0 ; _rtB ->
B_1036_60_0 . im = _rtB -> B_1036_59_0 ; _rtB -> B_1036_61_0 =
muDoubleScalarHypot ( _rtB -> B_1036_60_0 . re , _rtB -> B_1036_60_0 . im ) ;
_rtB -> B_1036_61_1 = muDoubleScalarAtan2 ( _rtB -> B_1036_60_0 . im , _rtB
-> B_1036_60_0 . re ) ; _rtB -> B_1036_62_0 = _rtX -> integrator_CSTATE_f ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_m . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_216 ; _rtB -> B_1036_63_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_eo . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_eo . Last , _rtDW -> TransportDelay_IWORK_eo . Tail ,
_rtDW -> TransportDelay_IWORK_eo . Head , _rtP -> P_217 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_64_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_65_0 = _rtP -> P_218 ; } _rtB ->
B_1036_66_0_o = ( _rtB -> B_1036_64_0 >= _rtB -> B_1036_65_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_67_0 = _rtDW
-> Memory_PreviousInput_kb ; } if ( _rtB -> B_1036_66_0_o ) { _rtB ->
B_949_0_0 = _rtB -> B_1036_62_0 - _rtB -> B_1036_63_0 ; _rtB -> B_949_1_0 =
_rtP -> P_56 * _rtB -> B_949_0_0 ; _rtB -> B_1036_69_0 = _rtB -> B_949_1_0 ;
} else { _rtB -> B_1036_69_0 = _rtB -> B_1036_67_0 ; } _rtB -> B_1036_70_0 =
_rtX -> integrator_CSTATE_g ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_mc . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mc . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_221 ; _rtB
-> B_1036_71_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_gp . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_gp . Last , _rtDW -> TransportDelay_IWORK_gp .
Tail , _rtDW -> TransportDelay_IWORK_gp . Head , _rtP -> P_222 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_72_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_73_0 = _rtP
-> P_223 ; } _rtB -> B_1036_74_0_k = ( _rtB -> B_1036_72_0 >= _rtB ->
B_1036_73_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_75_0 = _rtDW -> Memory_PreviousInput_g ; } if ( _rtB ->
B_1036_74_0_k ) { _rtB -> B_948_0_0 = _rtB -> B_1036_70_0 - _rtB ->
B_1036_71_0 ; _rtB -> B_948_1_0 = _rtP -> P_55 * _rtB -> B_948_0_0 ; _rtB ->
B_1036_77_0 = _rtB -> B_948_1_0 ; } else { _rtB -> B_1036_77_0 = _rtB ->
B_1036_75_0 ; } _rtB -> B_1036_78_0 . re = _rtB -> B_1036_69_0 ; _rtB ->
B_1036_78_0 . im = _rtB -> B_1036_77_0 ; _rtB -> B_1036_79_0 =
muDoubleScalarHypot ( _rtB -> B_1036_78_0 . re , _rtB -> B_1036_78_0 . im ) ;
_rtB -> B_1036_79_1 = muDoubleScalarAtan2 ( _rtB -> B_1036_78_0 . im , _rtB
-> B_1036_78_0 . re ) ; _rtB -> B_1036_80_0 = _rtX -> integrator_CSTATE_k ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_l .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_l . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_226 ; _rtB -> B_1036_81_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_gk . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_gk . Last , _rtDW -> TransportDelay_IWORK_gk . Tail ,
_rtDW -> TransportDelay_IWORK_gk . Head , _rtP -> P_227 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_82_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_83_0 = _rtP -> P_228 ; } _rtB ->
B_1036_84_0_k = ( _rtB -> B_1036_82_0 >= _rtB -> B_1036_83_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_85_0 = _rtDW
-> Memory_PreviousInput_j ; } if ( _rtB -> B_1036_84_0_k ) { _rtB ->
B_951_0_0 = _rtB -> B_1036_80_0 - _rtB -> B_1036_81_0 ; _rtB -> B_951_1_0 =
_rtP -> P_58 * _rtB -> B_951_0_0 ; _rtB -> B_1036_87_0 = _rtB -> B_951_1_0 ;
} else { _rtB -> B_1036_87_0 = _rtB -> B_1036_85_0 ; } _rtB -> B_1036_88_0 =
_rtX -> integrator_CSTATE_e3 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_be . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_be . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_231 ; _rtB
-> B_1036_89_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_b . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_b . Last , _rtDW -> TransportDelay_IWORK_b .
Tail , _rtDW -> TransportDelay_IWORK_b . Head , _rtP -> P_232 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_90_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_91_0 = _rtP
-> P_233 ; } _rtB -> B_1036_92_0_o = ( _rtB -> B_1036_90_0 >= _rtB ->
B_1036_91_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_93_0 = _rtDW -> Memory_PreviousInput_je ; } if ( _rtB ->
B_1036_92_0_o ) { _rtB -> B_950_0_0 = _rtB -> B_1036_88_0 - _rtB ->
B_1036_89_0 ; _rtB -> B_950_1_0 = _rtP -> P_57 * _rtB -> B_950_0_0 ; _rtB ->
B_1036_95_0 = _rtB -> B_950_1_0 ; } else { _rtB -> B_1036_95_0 = _rtB ->
B_1036_93_0 ; } _rtB -> B_1036_96_0 . re = _rtB -> B_1036_87_0 ; _rtB ->
B_1036_96_0 . im = _rtB -> B_1036_95_0 ; _rtB -> B_1036_97_0 =
muDoubleScalarHypot ( _rtB -> B_1036_96_0 . re , _rtB -> B_1036_96_0 . im ) ;
_rtB -> B_1036_97_1 = muDoubleScalarAtan2 ( _rtB -> B_1036_96_0 . im , _rtB
-> B_1036_96_0 . re ) ; _rtB -> B_1036_98_0 = _rtX -> integrator_CSTATE_a ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_k . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_236 ; _rtB -> B_1036_99_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_k . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_k . Last , _rtDW -> TransportDelay_IWORK_k . Tail ,
_rtDW -> TransportDelay_IWORK_k . Head , _rtP -> P_237 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_100_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_101_0 = _rtP -> P_238 ; } _rtB ->
B_1036_102_0_d = ( _rtB -> B_1036_100_0 >= _rtB -> B_1036_101_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_103_0 =
_rtDW -> Memory_PreviousInput_n ; } if ( _rtB -> B_1036_102_0_d ) { _rtB ->
B_953_0_0 = _rtB -> B_1036_98_0 - _rtB -> B_1036_99_0 ; _rtB -> B_953_1_0 =
_rtP -> P_60 * _rtB -> B_953_0_0 ; _rtB -> B_1036_105_0 = _rtB -> B_953_1_0 ;
} else { _rtB -> B_1036_105_0 = _rtB -> B_1036_103_0 ; } _rtB -> B_1036_106_0
= _rtX -> integrator_CSTATE_gp ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_g . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_g . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_241 ; _rtB
-> B_1036_107_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_pj . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_pj . Last , _rtDW -> TransportDelay_IWORK_pj .
Tail , _rtDW -> TransportDelay_IWORK_pj . Head , _rtP -> P_242 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_108_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_109_0 = _rtP
-> P_243 ; } _rtB -> B_1036_110_0_l = ( _rtB -> B_1036_108_0 >= _rtB ->
B_1036_109_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_111_0 = _rtDW -> Memory_PreviousInput_o ; } if ( _rtB ->
B_1036_110_0_l ) { _rtB -> B_952_0_0 = _rtB -> B_1036_106_0 - _rtB ->
B_1036_107_0 ; _rtB -> B_952_1_0 = _rtP -> P_59 * _rtB -> B_952_0_0 ; _rtB ->
B_1036_113_0 = _rtB -> B_952_1_0 ; } else { _rtB -> B_1036_113_0 = _rtB ->
B_1036_111_0 ; } _rtB -> B_1036_114_0 . re = _rtB -> B_1036_105_0 ; _rtB ->
B_1036_114_0 . im = _rtB -> B_1036_113_0 ; _rtB -> B_1036_115_0 =
muDoubleScalarHypot ( _rtB -> B_1036_114_0 . re , _rtB -> B_1036_114_0 . im )
; _rtB -> B_1036_115_1 = muDoubleScalarAtan2 ( _rtB -> B_1036_114_0 . im ,
_rtB -> B_1036_114_0 . re ) ; _rtB -> B_1036_116_0 = _rtX ->
integrator_CSTATE_ac ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ba . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ba . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_246 ; _rtB
-> B_1036_117_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ga . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_ga . Last , _rtDW -> TransportDelay_IWORK_ga .
Tail , _rtDW -> TransportDelay_IWORK_ga . Head , _rtP -> P_247 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_118_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_119_0 = _rtP
-> P_248 ; } _rtB -> B_1036_120_0_m = ( _rtB -> B_1036_118_0 >= _rtB ->
B_1036_119_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_121_0 = _rtDW -> Memory_PreviousInput_a ; } if ( _rtB ->
B_1036_120_0_m ) { _rtB -> B_955_0_0 = _rtB -> B_1036_116_0 - _rtB ->
B_1036_117_0 ; _rtB -> B_955_1_0 = _rtP -> P_62 * _rtB -> B_955_0_0 ; _rtB ->
B_1036_123_0 = _rtB -> B_955_1_0 ; } else { _rtB -> B_1036_123_0 = _rtB ->
B_1036_121_0 ; } _rtB -> B_1036_124_0 = _rtX -> integrator_CSTATE_b ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bh .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_bh . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_251 ; _rtB -> B_1036_125_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_j . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_j . Last , _rtDW -> TransportDelay_IWORK_j . Tail ,
_rtDW -> TransportDelay_IWORK_j . Head , _rtP -> P_252 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_126_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_127_0 = _rtP -> P_253 ; } _rtB ->
B_1036_128_0_o = ( _rtB -> B_1036_126_0 >= _rtB -> B_1036_127_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_129_0 =
_rtDW -> Memory_PreviousInput_ay ; } if ( _rtB -> B_1036_128_0_o ) { _rtB ->
B_954_0_0 = _rtB -> B_1036_124_0 - _rtB -> B_1036_125_0 ; _rtB -> B_954_1_0 =
_rtP -> P_61 * _rtB -> B_954_0_0 ; _rtB -> B_1036_131_0 = _rtB -> B_954_1_0 ;
} else { _rtB -> B_1036_131_0 = _rtB -> B_1036_129_0 ; } _rtB -> B_1036_132_0
. re = _rtB -> B_1036_123_0 ; _rtB -> B_1036_132_0 . im = _rtB ->
B_1036_131_0 ; _rtB -> B_1036_133_0 = muDoubleScalarHypot ( _rtB ->
B_1036_132_0 . re , _rtB -> B_1036_132_0 . im ) ; _rtB -> B_1036_133_1 =
muDoubleScalarAtan2 ( _rtB -> B_1036_132_0 . im , _rtB -> B_1036_132_0 . re )
; _rtB -> B_1036_134_0 = _rtX -> integrator_CSTATE_n ; { real_T * * uBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_bv . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bv .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_256 ; _rtB -> B_1036_135_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_jd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jd . Last , _rtDW -> TransportDelay_IWORK_jd . Tail ,
_rtDW -> TransportDelay_IWORK_jd . Head , _rtP -> P_257 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_136_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_137_0 = _rtP -> P_258 ; } _rtB ->
B_1036_138_0_a = ( _rtB -> B_1036_136_0 >= _rtB -> B_1036_137_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_139_0 =
_rtDW -> Memory_PreviousInput_bc ; } if ( _rtB -> B_1036_138_0_a ) { _rtB ->
B_963_0_0 = _rtB -> B_1036_134_0 - _rtB -> B_1036_135_0 ; _rtB -> B_963_1_0 =
_rtP -> P_70 * _rtB -> B_963_0_0 ; _rtB -> B_1036_141_0 = _rtB -> B_963_1_0 ;
} else { _rtB -> B_1036_141_0 = _rtB -> B_1036_139_0 ; } _rtB -> B_1036_142_0
= _rtX -> integrator_CSTATE_l ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_bw . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_bw . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_261 ; _rtB
-> B_1036_143_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_p1 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_p1 . Last , _rtDW -> TransportDelay_IWORK_p1 .
Tail , _rtDW -> TransportDelay_IWORK_p1 . Head , _rtP -> P_262 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_144_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_145_0 = _rtP
-> P_263 ; } _rtB -> B_1036_146_0_i = ( _rtB -> B_1036_144_0 >= _rtB ->
B_1036_145_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_147_0 = _rtDW -> Memory_PreviousInput_o2 ; } if ( _rtB ->
B_1036_146_0_i ) { _rtB -> B_962_0_0 = _rtB -> B_1036_142_0 - _rtB ->
B_1036_143_0 ; _rtB -> B_962_1_0 = _rtP -> P_69 * _rtB -> B_962_0_0 ; _rtB ->
B_1036_149_0 = _rtB -> B_962_1_0 ; } else { _rtB -> B_1036_149_0 = _rtB ->
B_1036_147_0 ; } _rtB -> B_1036_150_0 . re = _rtB -> B_1036_141_0 ; _rtB ->
B_1036_150_0 . im = _rtB -> B_1036_149_0 ; _rtB -> B_1036_151_0 =
muDoubleScalarHypot ( _rtB -> B_1036_150_0 . re , _rtB -> B_1036_150_0 . im )
; _rtB -> B_1036_151_1 = muDoubleScalarAtan2 ( _rtB -> B_1036_150_0 . im ,
_rtB -> B_1036_150_0 . re ) ; _rtB -> B_1036_152_0 = _rtX ->
integrator_CSTATE_m ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ly . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ly . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_266 ; _rtB
-> B_1036_153_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_k4 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_k4 . Last , _rtDW -> TransportDelay_IWORK_k4 .
Tail , _rtDW -> TransportDelay_IWORK_k4 . Head , _rtP -> P_267 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_154_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_155_0 = _rtP
-> P_268 ; } _rtB -> B_1036_156_0_f = ( _rtB -> B_1036_154_0 >= _rtB ->
B_1036_155_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_157_0 = _rtDW -> Memory_PreviousInput_e ; } if ( _rtB ->
B_1036_156_0_f ) { _rtB -> B_969_0_0 = _rtB -> B_1036_152_0 - _rtB ->
B_1036_153_0 ; _rtB -> B_969_1_0 = _rtP -> P_76 * _rtB -> B_969_0_0 ; _rtB ->
B_1036_159_0 = _rtB -> B_969_1_0 ; } else { _rtB -> B_1036_159_0 = _rtB ->
B_1036_157_0 ; } _rtB -> B_1036_160_0 = _rtX -> integrator_CSTATE_po ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ow .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ow . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_271 ; _rtB -> B_1036_161_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_og . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_og . Last , _rtDW -> TransportDelay_IWORK_og . Tail ,
_rtDW -> TransportDelay_IWORK_og . Head , _rtP -> P_272 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_162_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_163_0 = _rtP -> P_273 ; } _rtB ->
B_1036_164_0 = ( _rtB -> B_1036_162_0 >= _rtB -> B_1036_163_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_165_0 =
_rtDW -> Memory_PreviousInput_k3 ; } if ( _rtB -> B_1036_164_0 ) { _rtB ->
B_968_0_0 = _rtB -> B_1036_160_0 - _rtB -> B_1036_161_0 ; _rtB -> B_968_1_0 =
_rtP -> P_75 * _rtB -> B_968_0_0 ; _rtB -> B_1036_167_0 = _rtB -> B_968_1_0 ;
} else { _rtB -> B_1036_167_0 = _rtB -> B_1036_165_0 ; } _rtB -> B_1036_168_0
. re = _rtB -> B_1036_159_0 ; _rtB -> B_1036_168_0 . im = _rtB ->
B_1036_167_0 ; _rtB -> B_1036_169_0 = muDoubleScalarHypot ( _rtB ->
B_1036_168_0 . re , _rtB -> B_1036_168_0 . im ) ; _rtB -> B_1036_169_1 =
muDoubleScalarAtan2 ( _rtB -> B_1036_168_0 . im , _rtB -> B_1036_168_0 . re )
; _rtB -> B_1036_170_0 = _rtX -> integrator_CSTATE_ch ; { real_T * * uBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK_gm . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_gm .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_276 ; _rtB -> B_1036_171_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_i . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_i . Last , _rtDW -> TransportDelay_IWORK_i . Tail ,
_rtDW -> TransportDelay_IWORK_i . Head , _rtP -> P_277 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_172_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_173_0 = _rtP -> P_278 ; } _rtB ->
B_1036_174_0 = ( _rtB -> B_1036_172_0 >= _rtB -> B_1036_173_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_175_0 =
_rtDW -> Memory_PreviousInput_di ; } if ( _rtB -> B_1036_174_0 ) { _rtB ->
B_975_0_0 = _rtB -> B_1036_170_0 - _rtB -> B_1036_171_0 ; _rtB -> B_975_1_0 =
_rtP -> P_82 * _rtB -> B_975_0_0 ; _rtB -> B_1036_177_0 = _rtB -> B_975_1_0 ;
} else { _rtB -> B_1036_177_0 = _rtB -> B_1036_175_0 ; } _rtB -> B_1036_178_0
= _rtX -> integrator_CSTATE_l0 ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_g3 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_g3 . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_281
; _rtB -> B_1036_179_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0
, * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_pi . CircularBufSize
, & _rtDW -> TransportDelay_IWORK_pi . Last , _rtDW ->
TransportDelay_IWORK_pi . Tail , _rtDW -> TransportDelay_IWORK_pi . Head ,
_rtP -> P_282 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_180_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_181_0 = _rtP -> P_283 ; } _rtB -> B_1036_182_0_a = ( _rtB ->
B_1036_180_0 >= _rtB -> B_1036_181_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_183_0 = _rtDW -> Memory_PreviousInput_b1
; } if ( _rtB -> B_1036_182_0_a ) { _rtB -> B_974_0_0 = _rtB -> B_1036_178_0
- _rtB -> B_1036_179_0 ; _rtB -> B_974_1_0 = _rtP -> P_81 * _rtB -> B_974_0_0
; _rtB -> B_1036_185_0 = _rtB -> B_974_1_0 ; } else { _rtB -> B_1036_185_0 =
_rtB -> B_1036_183_0 ; } _rtB -> B_1036_186_0 . re = _rtB -> B_1036_177_0 ;
_rtB -> B_1036_186_0 . im = _rtB -> B_1036_185_0 ; _rtB -> B_1036_187_0 =
muDoubleScalarHypot ( _rtB -> B_1036_186_0 . re , _rtB -> B_1036_186_0 . im )
; _rtB -> B_1036_187_1 = muDoubleScalarAtan2 ( _rtB -> B_1036_186_0 . im ,
_rtB -> B_1036_186_0 . re ) ; _rtB -> B_1036_188_0 = _rtX ->
integrator_CSTATE_pf ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_kq . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_kq . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_286 ; _rtB
-> B_1036_189_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_p4 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_p4 . Last , _rtDW -> TransportDelay_IWORK_p4 .
Tail , _rtDW -> TransportDelay_IWORK_p4 . Head , _rtP -> P_287 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_190_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_191_0 = _rtP
-> P_288 ; } _rtB -> B_1036_192_0 = ( _rtB -> B_1036_190_0 >= _rtB ->
B_1036_191_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_193_0 = _rtDW -> Memory_PreviousInput_os ; } if ( _rtB ->
B_1036_192_0 ) { _rtB -> B_957_0_0 = _rtB -> B_1036_188_0 - _rtB ->
B_1036_189_0 ; _rtB -> B_957_1_0 = _rtP -> P_64 * _rtB -> B_957_0_0 ; _rtB ->
B_1036_195_0 = _rtB -> B_957_1_0 ; } else { _rtB -> B_1036_195_0 = _rtB ->
B_1036_193_0 ; } _rtB -> B_1036_196_0 = _rtX -> integrator_CSTATE_ly ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_g4 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_g4 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_291 ; _rtB -> B_1036_197_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_aj . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_aj . Last , _rtDW -> TransportDelay_IWORK_aj . Tail ,
_rtDW -> TransportDelay_IWORK_aj . Head , _rtP -> P_292 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_198_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_199_0 = _rtP -> P_293 ; } _rtB ->
B_1036_200_0_g = ( _rtB -> B_1036_198_0 >= _rtB -> B_1036_199_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_201_0 =
_rtDW -> Memory_PreviousInput_ki ; } if ( _rtB -> B_1036_200_0_g ) { _rtB ->
B_956_0_0 = _rtB -> B_1036_196_0 - _rtB -> B_1036_197_0 ; _rtB -> B_956_1_0 =
_rtP -> P_63 * _rtB -> B_956_0_0 ; _rtB -> B_1036_203_0 = _rtB -> B_956_1_0 ;
} else { _rtB -> B_1036_203_0 = _rtB -> B_1036_201_0 ; } _rtB -> B_1036_204_0
. re = _rtB -> B_1036_195_0 ; _rtB -> B_1036_204_0 . im = _rtB ->
B_1036_203_0 ; _rtB -> B_1036_205_0 = muDoubleScalarHypot ( _rtB ->
B_1036_204_0 . re , _rtB -> B_1036_204_0 . im ) ; _rtB -> B_1036_205_1 =
muDoubleScalarAtan2 ( _rtB -> B_1036_204_0 . im , _rtB -> B_1036_204_0 . re )
; _rtB -> B_1036_206_0 = _rtX -> integrator_CSTATE_ah ; { real_T * * uBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK_p . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_p .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_296 ; _rtB -> B_1036_207_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_f . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_f . Last , _rtDW -> TransportDelay_IWORK_f . Tail ,
_rtDW -> TransportDelay_IWORK_f . Head , _rtP -> P_297 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_208_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_209_0 = _rtP -> P_298 ; } _rtB ->
B_1036_210_0 = ( _rtB -> B_1036_208_0 >= _rtB -> B_1036_209_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_211_0 =
_rtDW -> Memory_PreviousInput_o4 ; } if ( _rtB -> B_1036_210_0 ) { _rtB ->
B_961_0_0 = _rtB -> B_1036_206_0 - _rtB -> B_1036_207_0 ; _rtB -> B_961_1_0 =
_rtP -> P_68 * _rtB -> B_961_0_0 ; _rtB -> B_1036_213_0 = _rtB -> B_961_1_0 ;
} else { _rtB -> B_1036_213_0 = _rtB -> B_1036_211_0 ; } _rtB -> B_1036_214_0
= _rtX -> integrator_CSTATE_d ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_i . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_i . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_301 ; _rtB
-> B_1036_215_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_c . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_c . Last , _rtDW -> TransportDelay_IWORK_c .
Tail , _rtDW -> TransportDelay_IWORK_c . Head , _rtP -> P_302 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_216_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_217_0 = _rtP
-> P_303 ; } _rtB -> B_1036_218_0 = ( _rtB -> B_1036_216_0 >= _rtB ->
B_1036_217_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_219_0 = _rtDW -> Memory_PreviousInput_l ; } if ( _rtB ->
B_1036_218_0 ) { _rtB -> B_960_0_0 = _rtB -> B_1036_214_0 - _rtB ->
B_1036_215_0 ; _rtB -> B_960_1_0 = _rtP -> P_67 * _rtB -> B_960_0_0 ; _rtB ->
B_1036_221_0 = _rtB -> B_960_1_0 ; } else { _rtB -> B_1036_221_0 = _rtB ->
B_1036_219_0 ; } _rtB -> B_1036_222_0 . re = _rtB -> B_1036_213_0 ; _rtB ->
B_1036_222_0 . im = _rtB -> B_1036_221_0 ; _rtB -> B_1036_223_0 =
muDoubleScalarHypot ( _rtB -> B_1036_222_0 . re , _rtB -> B_1036_222_0 . im )
; _rtB -> B_1036_223_1 = muDoubleScalarAtan2 ( _rtB -> B_1036_222_0 . im ,
_rtB -> B_1036_222_0 . re ) ; _rtB -> B_1036_224_0 = _rtX ->
integrator_CSTATE_ff ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_k4 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_k4 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_306 ; _rtB
-> B_1036_225_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_m . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_m . Last , _rtDW -> TransportDelay_IWORK_m .
Tail , _rtDW -> TransportDelay_IWORK_m . Head , _rtP -> P_307 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_226_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_227_0 = _rtP
-> P_308 ; } _rtB -> B_1036_228_0 = ( _rtB -> B_1036_226_0 >= _rtB ->
B_1036_227_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_229_0 = _rtDW -> Memory_PreviousInput_d3 ; } if ( _rtB ->
B_1036_228_0 ) { _rtB -> B_967_0_0 = _rtB -> B_1036_224_0 - _rtB ->
B_1036_225_0 ; _rtB -> B_967_1_0 = _rtP -> P_74 * _rtB -> B_967_0_0 ; _rtB ->
B_1036_231_0 = _rtB -> B_967_1_0 ; } else { _rtB -> B_1036_231_0 = _rtB ->
B_1036_229_0 ; } _rtB -> B_1036_232_0 = _rtX -> integrator_CSTATE_kc ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_l4 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_l4 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_311 ; _rtB -> B_1036_233_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_d . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_d . Last , _rtDW -> TransportDelay_IWORK_d . Tail ,
_rtDW -> TransportDelay_IWORK_d . Head , _rtP -> P_312 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_234_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_235_0 = _rtP -> P_313 ; } _rtB ->
B_1036_236_0_k = ( _rtB -> B_1036_234_0 >= _rtB -> B_1036_235_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_237_0 =
_rtDW -> Memory_PreviousInput_bf ; } if ( _rtB -> B_1036_236_0_k ) { _rtB ->
B_966_0_0 = _rtB -> B_1036_232_0 - _rtB -> B_1036_233_0 ; _rtB -> B_966_1_0 =
_rtP -> P_73 * _rtB -> B_966_0_0 ; _rtB -> B_1036_239_0 = _rtB -> B_966_1_0 ;
} else { _rtB -> B_1036_239_0 = _rtB -> B_1036_237_0 ; } _rtB -> B_1036_240_0
. re = _rtB -> B_1036_231_0 ; _rtB -> B_1036_240_0 . im = _rtB ->
B_1036_239_0 ; _rtB -> B_1036_241_0 = muDoubleScalarHypot ( _rtB ->
B_1036_240_0 . re , _rtB -> B_1036_240_0 . im ) ; _rtB -> B_1036_241_1 =
muDoubleScalarAtan2 ( _rtB -> B_1036_240_0 . im , _rtB -> B_1036_240_0 . re )
; _rtB -> B_1036_242_0 = _rtX -> integrator_CSTATE_pt ; { real_T * * uBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_316 ; _rtB -> B_1036_243_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_n . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_n . Last , _rtDW -> TransportDelay_IWORK_n . Tail ,
_rtDW -> TransportDelay_IWORK_n . Head , _rtP -> P_317 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_244_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_245_0 = _rtP -> P_318 ; } _rtB ->
B_1036_246_0_l = ( _rtB -> B_1036_244_0 >= _rtB -> B_1036_245_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_247_0 =
_rtDW -> Memory_PreviousInput_lx ; } if ( _rtB -> B_1036_246_0_l ) { _rtB ->
B_973_0_0 = _rtB -> B_1036_242_0 - _rtB -> B_1036_243_0 ; _rtB -> B_973_1_0 =
_rtP -> P_80 * _rtB -> B_973_0_0 ; _rtB -> B_1036_249_0 = _rtB -> B_973_1_0 ;
} else { _rtB -> B_1036_249_0 = _rtB -> B_1036_247_0 ; } _rtB -> B_1036_250_0
= _rtX -> integrator_CSTATE_d1 ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_ij . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_ij . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_321
; _rtB -> B_1036_251_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0
, * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_nz . CircularBufSize
, & _rtDW -> TransportDelay_IWORK_nz . Last , _rtDW ->
TransportDelay_IWORK_nz . Tail , _rtDW -> TransportDelay_IWORK_nz . Head ,
_rtP -> P_322 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_252_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_253_0 = _rtP -> P_323 ; } _rtB -> B_1036_254_0_e = ( _rtB ->
B_1036_252_0 >= _rtB -> B_1036_253_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_255_0 = _rtDW -> Memory_PreviousInput_db
; } if ( _rtB -> B_1036_254_0_e ) { _rtB -> B_972_0_0 = _rtB -> B_1036_250_0
- _rtB -> B_1036_251_0 ; _rtB -> B_972_1_0 = _rtP -> P_79 * _rtB -> B_972_0_0
; _rtB -> B_1036_257_0 = _rtB -> B_972_1_0 ; } else { _rtB -> B_1036_257_0 =
_rtB -> B_1036_255_0 ; } _rtB -> B_1036_258_0 . re = _rtB -> B_1036_249_0 ;
_rtB -> B_1036_258_0 . im = _rtB -> B_1036_257_0 ; _rtB -> B_1036_259_0 =
muDoubleScalarHypot ( _rtB -> B_1036_258_0 . re , _rtB -> B_1036_258_0 . im )
; _rtB -> B_1036_259_1 = muDoubleScalarAtan2 ( _rtB -> B_1036_258_0 . im ,
_rtB -> B_1036_258_0 . re ) ; _rtB -> B_1036_260_0 [ 0 ] = _rtP -> P_325 *
_rtB -> B_1036_25_0 ; _rtB -> B_1036_260_0 [ 1 ] = _rtP -> P_325 * _rtB ->
B_1036_43_0 ; _rtB -> B_1036_260_0 [ 2 ] = _rtP -> P_325 * _rtB ->
B_1036_61_0 ; _rtB -> B_1036_260_0 [ 3 ] = _rtP -> P_325 * _rtB ->
B_1036_79_0 ; _rtB -> B_1036_260_0 [ 4 ] = _rtP -> P_325 * _rtB ->
B_1036_97_0 ; _rtB -> B_1036_260_0 [ 5 ] = _rtP -> P_325 * _rtB ->
B_1036_115_0 ; _rtB -> B_1036_260_0 [ 6 ] = _rtP -> P_325 * _rtB ->
B_1036_133_0 ; _rtB -> B_1036_260_0 [ 7 ] = _rtP -> P_325 * _rtB ->
B_1036_151_0 ; _rtB -> B_1036_260_0 [ 8 ] = _rtP -> P_325 * _rtB ->
B_1036_169_0 ; _rtB -> B_1036_260_0 [ 9 ] = _rtP -> P_325 * _rtB ->
B_1036_187_0 ; _rtB -> B_1036_260_0 [ 10 ] = _rtP -> P_325 * _rtB ->
B_1036_205_0 ; _rtB -> B_1036_260_0 [ 11 ] = _rtP -> P_325 * _rtB ->
B_1036_223_0 ; _rtB -> B_1036_260_0 [ 12 ] = _rtP -> P_325 * _rtB ->
B_1036_241_0 ; _rtB -> B_1036_260_0 [ 13 ] = _rtP -> P_325 * _rtB ->
B_1036_259_0 ; _rtB -> B_1036_261_0 = _rtP -> P_326 * _rtB -> B_1036_25_1 ;
_rtB -> B_1036_262_0 = _rtP -> P_327 * _rtB -> B_1036_43_1 ; _rtB ->
B_1036_263_0 = _rtP -> P_328 * _rtB -> B_1036_61_1 ; _rtB -> B_1036_264_0 =
_rtP -> P_329 * _rtB -> B_1036_79_1 ; _rtB -> B_1036_265_0 = _rtP -> P_330 *
_rtB -> B_1036_97_1 ; _rtB -> B_1036_266_0 = _rtP -> P_331 * _rtB ->
B_1036_115_1 ; _rtB -> B_1036_267_0 = _rtP -> P_332 * _rtB -> B_1036_133_1 ;
_rtB -> B_1036_268_0 = _rtP -> P_333 * _rtB -> B_1036_151_1 ; _rtB ->
B_1036_269_0 = _rtP -> P_334 * _rtB -> B_1036_169_1 ; _rtB -> B_1036_270_0 =
_rtP -> P_335 * _rtB -> B_1036_187_1 ; _rtB -> B_1036_271_0 = _rtP -> P_336 *
_rtB -> B_1036_205_1 ; _rtB -> B_1036_272_0 = _rtP -> P_337 * _rtB ->
B_1036_223_1 ; _rtB -> B_1036_273_0 = _rtP -> P_338 * _rtB -> B_1036_241_1 ;
_rtB -> B_1036_274_0 = _rtP -> P_339 * _rtB -> B_1036_259_1 ; _rtB ->
B_1036_275_0 [ 0 ] = _rtP -> P_340 * _rtB -> B_1036_261_0 ; _rtB ->
B_1036_275_0 [ 1 ] = _rtP -> P_340 * _rtB -> B_1036_262_0 ; _rtB ->
B_1036_275_0 [ 2 ] = _rtP -> P_340 * _rtB -> B_1036_263_0 ; _rtB ->
B_1036_275_0 [ 3 ] = _rtP -> P_340 * _rtB -> B_1036_264_0 ; _rtB ->
B_1036_275_0 [ 4 ] = _rtP -> P_340 * _rtB -> B_1036_265_0 ; _rtB ->
B_1036_275_0 [ 5 ] = _rtP -> P_340 * _rtB -> B_1036_266_0 ; _rtB ->
B_1036_275_0 [ 6 ] = _rtP -> P_340 * _rtB -> B_1036_267_0 ; _rtB ->
B_1036_275_0 [ 7 ] = _rtP -> P_340 * _rtB -> B_1036_268_0 ; _rtB ->
B_1036_275_0 [ 8 ] = _rtP -> P_340 * _rtB -> B_1036_269_0 ; _rtB ->
B_1036_275_0 [ 9 ] = _rtP -> P_340 * _rtB -> B_1036_270_0 ; _rtB ->
B_1036_275_0 [ 10 ] = _rtP -> P_340 * _rtB -> B_1036_271_0 ; _rtB ->
B_1036_275_0 [ 11 ] = _rtP -> P_340 * _rtB -> B_1036_272_0 ; _rtB ->
B_1036_275_0 [ 12 ] = _rtP -> P_340 * _rtB -> B_1036_273_0 ; _rtB ->
B_1036_275_0 [ 13 ] = _rtP -> P_340 * _rtB -> B_1036_274_0 ; _rtB ->
B_1036_276_0 = muDoubleScalarCos ( _rtB -> B_1036_275_0 [ 3 ] ) ; _rtB ->
B_1036_277_0 = _rtB -> B_1036_260_0 [ 3 ] * _rtB -> B_1036_276_0 ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_278_0 =
_rtDW -> u4_PreviousInput ; } _rtB -> B_1036_279_0 = ( _rtB -> B_1036_459_0_j
!= 0.0 ) ; _rtB -> B_1036_280_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_281_0_e = _rtB -> B_1036_519_0_p
; } DZG_v3_ONDelay ( S , _rtB -> B_1036_281_0_e , _rtB -> B_1036_279_0 , _rtB
-> B_1036_280_0 , _rtB -> B_1036_517_0_n , & _rtB -> ONDelay_kb , & _rtDW ->
ONDelay_kb , & _rtZCE -> ONDelay_kb ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_283_0 = _rtB -> B_1036_520_0 ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_283_0 , _rtB -> B_1036_279_0 , _rtB ->
B_1036_280_0 , _rtB -> B_1036_517_0_n , & _rtB -> OFFDelay_mh , & _rtDW ->
OFFDelay_mh , & _rtZCE -> OFFDelay_mh ) ; _rtB -> B_1036_285_0 = ( _rtB ->
ONDelay_kb . B_90_17_0 || _rtB -> OFFDelay_mh . B_86_19_0 ) ; _rtB ->
B_1036_286_0 = ( _rtB -> B_1036_278_0 || _rtB -> B_1036_285_0 ) ; _rtB ->
B_1036_287_0_f = _rtB -> B_1036_460_0_e ; _rtB -> B_1036_288_0 = ssGetT ( S )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_289_0_d = _rtB -> B_1036_526_0_f ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_289_0_d , _rtB -> B_1036_287_0_f , _rtB -> B_1036_288_0 , _rtB ->
B_1036_524_0_k , & _rtB -> ONDelay_m4 , & _rtDW -> ONDelay_m4 , & _rtZCE ->
ONDelay_m4 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_291_0 = _rtB -> B_1036_527_0_h ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_291_0 , _rtB -> B_1036_287_0_f , _rtB -> B_1036_288_0 , _rtB ->
B_1036_524_0_k , & _rtB -> OFFDelay_ic , & _rtDW -> OFFDelay_ic , & _rtZCE ->
OFFDelay_ic ) ; _rtB -> B_1036_293_0 = ( _rtB -> ONDelay_m4 . B_90_17_0 ||
_rtB -> OFFDelay_ic . B_86_19_0 ) ; _rtB -> B_1036_294_0_b = ! _rtB ->
B_1036_293_0 ; _rtB -> B_1036_295_0_o = ( _rtB -> B_1036_286_0 && _rtB ->
B_1036_294_0_b ) ; if ( _rtB -> B_1036_295_0_o ) { _rtB -> B_1036_296_0 =
_rtB -> B_1036_474_0 ; } else { _rtB -> B_1036_296_0 = _rtB -> B_1036_500_0 ;
} _rtB -> B_1036_297_0 = _rtB -> B_1036_277_0 * _rtB -> B_1036_296_0 ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_298_0 =
_rtDW -> u4_PreviousInput_g ; } _rtB -> B_1036_299_0 = false ; _rtB ->
B_1036_300_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_301_0_p = _rtB -> B_1036_533_0_h ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_301_0_p , _rtB -> B_1036_299_0 , _rtB ->
B_1036_300_0 , _rtB -> B_1036_531_0_h , & _rtB -> ONDelay_c , & _rtDW ->
ONDelay_c , & _rtZCE -> ONDelay_c ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_303_0_l = _rtB -> B_1036_534_0_b ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_303_0_l , _rtB -> B_1036_299_0 , _rtB ->
B_1036_300_0 , _rtB -> B_1036_531_0_h , & _rtB -> OFFDelay_ig , & _rtDW ->
OFFDelay_ig , & _rtZCE -> OFFDelay_ig ) ; _rtB -> B_1036_305_0 = ( _rtB ->
ONDelay_c . B_90_17_0 || _rtB -> OFFDelay_ig . B_86_19_0 ) ; _rtB ->
B_1036_306_0 = ( _rtB -> B_1036_298_0 || _rtB -> B_1036_305_0 ) ; _rtB ->
B_1036_307_0 = _rtB -> B_1036_463_0_d ; _rtB -> B_1036_308_0 = ssGetT ( S ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_309_0_d = _rtB -> B_1036_540_0_n ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_309_0_d , _rtB -> B_1036_307_0 , _rtB -> B_1036_308_0 , _rtB ->
B_1036_538_0 , & _rtB -> ONDelay_jz , & _rtDW -> ONDelay_jz , & _rtZCE ->
ONDelay_jz ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_311_0 = _rtB -> B_1036_541_0_e ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_311_0 , _rtB -> B_1036_307_0 , _rtB -> B_1036_308_0 , _rtB ->
B_1036_538_0 , & _rtB -> OFFDelay_fp , & _rtDW -> OFFDelay_fp , & _rtZCE ->
OFFDelay_fp ) ; _rtB -> B_1036_313_0 = ( _rtB -> ONDelay_jz . B_90_17_0 ||
_rtB -> OFFDelay_fp . B_86_19_0 ) ; _rtB -> B_1036_314_0 = ! _rtB ->
B_1036_313_0 ; _rtB -> B_1036_315_0_l = ( _rtB -> B_1036_306_0 && _rtB ->
B_1036_314_0 ) ; if ( _rtB -> B_1036_315_0_l ) { _rtB -> B_1036_316_0 = _rtB
-> B_1036_482_0 ; } else { _rtB -> B_1036_316_0 = _rtB -> B_1036_501_0_h ; }
_rtB -> B_1036_317_0 = 0.0 * _rtB -> B_1036_316_0 ; _rtB -> B_1036_318_0 =
_rtB -> B_1036_297_0 + _rtB -> B_1036_317_0 ; _rtB -> B_1036_319_0 = _rtB ->
B_1036_318_0 * _rtB -> B_1036_318_0 ; _rtB -> B_1036_320_0 =
muDoubleScalarSin ( _rtB -> B_1036_275_0 [ 3 ] ) ; _rtB -> B_1036_321_0 =
_rtB -> B_1036_320_0 * _rtB -> B_1036_260_0 [ 3 ] ; _rtB -> B_1036_322_0 =
_rtB -> B_1036_321_0 * _rtB -> B_1036_296_0 ; _rtB -> B_1036_323_0 = 0.0 *
_rtB -> B_1036_316_0 ; _rtB -> B_1036_324_0 = _rtB -> B_1036_322_0 + _rtB ->
B_1036_323_0 ; _rtB -> B_1036_325_0 = _rtB -> B_1036_324_0 * _rtB ->
B_1036_324_0 ; _rtB -> B_1036_326_0 = _rtB -> B_1036_319_0 + _rtB ->
B_1036_325_0 ; _rtB -> B_1036_327_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_326_0 ) ; _rtB -> B_1036_328_0 = ( _rtB -> B_1036_327_0 < 0.0 ) ; _rtB
-> B_1036_329_0_a = ! _rtB -> B_1036_328_0 ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_330_0 = _rtDW -> u_PreviousInput ; }
_rtB -> B_1036_331_0 = _rtB -> B_1036_318_0 - _rtB -> B_1036_330_0 ; _rtB ->
B_1036_332_0 = _rtB -> B_1036_331_0 * _rtB -> B_1036_331_0 ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_333_0 =
_rtDW -> u_PreviousInput_p ; } _rtB -> B_1036_334_0 = _rtB -> B_1036_324_0 -
_rtB -> B_1036_333_0 ; _rtB -> B_1036_335_0 = _rtB -> B_1036_334_0 * _rtB ->
B_1036_334_0 ; _rtB -> B_1036_336_0 = _rtB -> B_1036_332_0 + _rtB ->
B_1036_335_0 ; _rtB -> B_1036_337_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_336_0 ) ; _rtB -> B_1036_338_0 = _rtB -> B_1036_318_0 * _rtB ->
B_1036_318_0 ; _rtB -> B_1036_339_0 = _rtB -> B_1036_324_0 * _rtB ->
B_1036_324_0 ; _rtB -> B_1036_340_0 = _rtB -> B_1036_338_0 + _rtB ->
B_1036_339_0 ; _rtB -> B_1036_341_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_340_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_342_0 = _rtB -> B_1036_330_0 * _rtB -> B_1036_330_0 ; _rtB ->
B_1036_343_0 = _rtB -> B_1036_333_0 * _rtB -> B_1036_333_0 ; _rtB ->
B_1036_344_0 = _rtB -> B_1036_342_0 + _rtB -> B_1036_343_0 ; _rtB ->
B_1036_345_0 = muDoubleScalarSqrt ( _rtB -> B_1036_344_0 ) ; } maxV =
muDoubleScalarMax ( _rtB -> B_1036_341_0 , _rtB -> B_1036_345_0 ) ; _rtB ->
B_1036_346_0 = maxV ; _rtB -> B_1036_347_0_n = ( _rtB -> B_1036_346_0 == _rtB
-> B_1036_385_0 ) ; if ( _rtB -> B_1036_347_0_n ) { _rtB -> B_1036_348_0 =
_rtB -> B_1036_386_0 ; } else { _rtB -> B_1036_348_0 = _rtB -> B_1036_346_0 ;
} _rtB -> B_1036_349_0 = _rtB -> B_1036_337_0 / _rtB -> B_1036_348_0 ; _rtB
-> B_1036_350_0_e = ( _rtB -> B_1036_349_0 > _rtB -> B_1036_368_0 ) ; _rtB ->
B_1036_351_0_n = ( _rtB -> B_1036_329_0_a && _rtB -> B_1036_350_0_e ) ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_352_0 =
_rtDW -> u_PreviousInput_k ; _rtB -> B_1036_353_0 = _rtDW ->
u_PreviousInput_e ; _rtB -> B_1036_354_0 = _rtDW -> u_PreviousInput_pi ; _rtB
-> B_1036_355_0 = ( _rtB -> B_1036_352_0 + _rtB -> B_1036_353_0 ) + _rtB ->
B_1036_354_0 ; _rtB -> B_1036_356_0 = _rtB -> B_1036_355_0 * _rtB ->
B_1036_382_0 ; } _rtB -> B_1036_357_0 = _rtB -> B_1036_337_0 - _rtB ->
B_1036_356_0 ; _rtB -> B_1036_358_0 = muDoubleScalarAbs ( _rtB ->
B_1036_357_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_359_0_h = ( _rtB -> B_1036_356_0 == _rtB -> B_1036_383_0 ) ;
if ( _rtB -> B_1036_359_0_h ) { _rtB -> B_1036_360_0 = _rtB -> B_1036_384_0 ;
} else { _rtB -> B_1036_360_0 = _rtB -> B_1036_356_0 ; } } _rtB ->
B_1036_361_0 = _rtB -> B_1036_358_0 / _rtB -> B_1036_360_0 ; _rtB ->
B_1036_362_0_a = ( _rtB -> B_1036_361_0 > _rtB -> B_1036_367_0 ) ; _rtB ->
B_1036_363_0 = ( _rtB -> B_1036_337_0 < _rtB -> B_1036_365_0 ) ; _rtB ->
B_1036_364_0 = ! _rtB -> B_1036_363_0 ; _rtB -> B_1036_365_0_b = ( _rtB ->
B_1036_362_0_a && _rtB -> B_1036_364_0 ) ; _rtB -> B_1036_366_0 = ( _rtB ->
B_1036_351_0_n && _rtB -> B_1036_365_0_b ) ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_367_0_k = _rtDW ->
u4_PreviousInput_n ; _rtB -> B_1036_368_0_a = _rtDW -> u1_PreviousInput ;
_rtB -> B_1036_369_0 = ! _rtB -> B_1036_368_0_a ; _rtB -> B_1036_370_0 = (
_rtB -> B_1036_367_0_k && _rtB -> B_1036_369_0 ) ; } _rtB -> B_1036_371_0 = (
_rtB -> B_1036_366_0 || _rtB -> B_1036_370_0 ) ; _rtB -> B_1036_372_0 = _rtB
-> B_1036_371_0 ; _rtB -> B_1036_373_0 = ssGetT ( S ) ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_374_0 = _rtB ->
B_1036_378_0_a ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_374_0 , _rtB ->
B_1036_372_0 , _rtB -> B_1036_373_0 , _rtB -> B_1036_376_0 , & _rtB ->
ONDelay_i , & _rtDW -> ONDelay_i , & _rtZCE -> ONDelay_i ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_376_0_k =
_rtB -> B_1036_379_0_o ; } DZG_v3_OFFDelay ( S , _rtB -> B_1036_376_0_k ,
_rtB -> B_1036_372_0 , _rtB -> B_1036_373_0 , _rtB -> B_1036_376_0 , & _rtB
-> OFFDelay_fu , & _rtDW -> OFFDelay_fu , & _rtZCE -> OFFDelay_fu ) ; _rtB ->
B_1036_378_0 = ( _rtB -> ONDelay_i . B_90_17_0 || _rtB -> OFFDelay_fu .
B_86_19_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_379_0 = _rtDW -> Memory_PreviousInput_kk ; _rtB -> B_1036_380_0 = !
_rtB -> B_1036_379_0 ; } _rtB -> B_1036_381_0 = ( _rtB -> B_1036_378_0 &&
_rtB -> B_1036_380_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_1036_382_0_p = _rtDW -> u4_PreviousInput_m ; } _rtB ->
B_1036_383_0_o = ! _rtB -> B_1036_351_0_n ; _rtB -> B_1036_384_0_c = ! _rtB
-> B_1036_383_0_o ; _rtB -> B_1036_385_0_k = ( _rtB -> B_1036_382_0_p && _rtB
-> B_1036_384_0_c ) ; _rtB -> B_1036_386_0_h = ( _rtB -> B_1036_381_0 || _rtB
-> B_1036_385_0_k ) ; _rtB -> B_1036_387_0_b = ( _rtB -> B_1036_386_0_h &&
_rtB -> B_1036_371_0_l ) ; _rtB -> B_1036_388_0_e = ( _rtB -> B_1036_381_0 &&
_rtB -> B_1036_372_0_o ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_1036_389_0_c = _rtDW -> Memory_PreviousInput_pu ; _rtB ->
B_1036_390_0 = ! _rtB -> B_1036_389_0_c ; } _rtB -> B_1036_391_0 = ( _rtB ->
B_1036_366_0 && _rtB -> B_1036_390_0 ) ; _rtB -> B_1036_392_0 = ( _rtB ->
B_1036_391_0 && _rtB -> B_1036_373_0_i ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S , 1036 , 393 ,
SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_1036_394_0_o = _rtDW -> u4_PreviousInput_o
; } _rtB -> B_1036_395_0 = _rtP -> P_346 * _rtB -> B_1036_318_0 ; _rtB ->
B_1036_396_0 = _rtB -> B_1036_395_0 * _rtB -> B_1036_1574_0 ; _rtB ->
B_1036_397_0 = muDoubleScalarCos ( _rtB -> B_1036_275_0 [ 4 ] ) ; _rtB ->
B_1036_398_0 = _rtB -> B_1036_260_0 [ 4 ] * _rtB -> B_1036_397_0 ; _rtB ->
B_1036_399_0 = _rtB -> B_1036_398_0 * _rtB -> B_1036_296_0 ; _rtB ->
B_1036_400_0 = 0.0 * _rtB -> B_1036_316_0 ; _rtB -> B_1036_401_0 = _rtB ->
B_1036_399_0 + _rtB -> B_1036_400_0 ; _rtB -> B_1036_402_0 = _rtP -> P_347 *
_rtB -> B_1036_401_0 ; _rtB -> B_1036_403_0 = muDoubleScalarCos ( _rtB ->
B_1036_275_0 [ 5 ] ) ; _rtB -> B_1036_404_0 = _rtB -> B_1036_260_0 [ 5 ] *
_rtB -> B_1036_403_0 ; _rtB -> B_1036_405_0 = _rtB -> B_1036_404_0 * _rtB ->
B_1036_296_0 ; _rtB -> B_1036_406_0 = 0.0 * _rtB -> B_1036_316_0 ; _rtB ->
B_1036_407_0 = _rtB -> B_1036_405_0 + _rtB -> B_1036_406_0 ; _rtB ->
B_1036_408_0 = _rtP -> P_348 * _rtB -> B_1036_407_0 ; _rtB -> B_1036_409_0 =
_rtB -> B_1036_402_0 + _rtB -> B_1036_408_0 ; _rtB -> B_1036_410_0 = _rtB ->
B_1036_409_0 + _rtB -> B_1036_395_0 ; _rtB -> B_1036_411_0 = _rtB ->
B_1036_410_0 * _rtB -> B_1036_1733_0_g ; if ( _rtB -> B_1036_1190_0 > _rtP ->
P_349 ) { _rtB -> B_674_0_0 = _rtB -> B_1036_411_0 * _rtB -> B_1036_1574_0 ;
_rtB -> B_674_1_0 = _rtB -> B_1036_396_0 - _rtB -> B_674_0_0 ; _rtB ->
B_1036_413_0 = _rtB -> B_674_1_0 ; } else { _rtB -> B_1036_413_0 = _rtB ->
B_1036_396_0 ; } _rtB -> B_1036_414_0 = _rtB -> B_1036_413_0 * _rtB ->
B_1036_1576_0 ; _rtB -> B_1036_415_0 = _rtP -> P_350 * _rtB -> B_1036_324_0 ;
_rtB -> B_1036_416_0 = _rtB -> B_1036_415_0 * _rtB -> B_1036_1574_0 ; _rtB ->
B_1036_417_0 = muDoubleScalarSin ( _rtB -> B_1036_275_0 [ 4 ] ) ; _rtB ->
B_1036_418_0 = _rtB -> B_1036_417_0 * _rtB -> B_1036_260_0 [ 4 ] ; _rtB ->
B_1036_419_0 = _rtB -> B_1036_418_0 * _rtB -> B_1036_296_0 ; _rtB ->
B_1036_420_0 = 0.0 * _rtB -> B_1036_316_0 ; _rtB -> B_1036_421_0 = _rtB ->
B_1036_419_0 + _rtB -> B_1036_420_0 ; _rtB -> B_1036_422_0 = _rtP -> P_351 *
_rtB -> B_1036_421_0 ; _rtB -> B_1036_423_0 = muDoubleScalarSin ( _rtB ->
B_1036_275_0 [ 5 ] ) ; _rtB -> B_1036_424_0 = _rtB -> B_1036_423_0 * _rtB ->
B_1036_260_0 [ 5 ] ; _rtB -> B_1036_425_0 = _rtB -> B_1036_424_0 * _rtB ->
B_1036_296_0 ; _rtB -> B_1036_426_0 = 0.0 * _rtB -> B_1036_316_0 ; _rtB ->
B_1036_427_0 = _rtB -> B_1036_425_0 + _rtB -> B_1036_426_0 ; _rtB ->
B_1036_428_0 = _rtP -> P_352 * _rtB -> B_1036_427_0 ; _rtB -> B_1036_429_0 =
_rtB -> B_1036_422_0 + _rtB -> B_1036_428_0 ; _rtB -> B_1036_430_0 = _rtB ->
B_1036_429_0 + _rtB -> B_1036_415_0 ; _rtB -> B_1036_431_0 = _rtB ->
B_1036_430_0 * _rtB -> B_1036_1733_0_g ; if ( _rtB -> B_1036_1190_0 > _rtP ->
P_353 ) { _rtB -> B_675_0_0 = _rtB -> B_1036_431_0 * _rtB -> B_1036_1574_0 ;
_rtB -> B_675_1_0 = _rtB -> B_1036_416_0 - _rtB -> B_675_0_0 ; _rtB ->
B_1036_433_0 = _rtB -> B_675_1_0 ; } else { _rtB -> B_1036_433_0 = _rtB ->
B_1036_416_0 ; } _rtB -> B_1036_434_0 = _rtB -> B_1036_433_0 * _rtB ->
B_1036_1575_0 ; _rtB -> B_1036_435_0 = _rtB -> B_1036_414_0 - _rtB ->
B_1036_434_0 ; _rtB -> B_1036_436_0_a = ! _rtB -> B_1036_295_0_o ; _rtB ->
B_1036_437_0_p = ! _rtB -> B_1036_315_0_l ; _rtB -> B_1036_438_0 = ( _rtB ->
B_1036_436_0_a && _rtB -> B_1036_437_0_p ) ; _rtB -> B_1036_439_0 = _rtB ->
B_1036_277_0 * _rtB -> B_1036_277_0 ; _rtB -> B_1036_440_0 = _rtB ->
B_1036_321_0 * _rtB -> B_1036_321_0 ; _rtB -> B_1036_441_0 = _rtB ->
B_1036_439_0 + _rtB -> B_1036_440_0 ; _rtB -> B_1036_442_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_441_0 ) ; _rtB -> B_1036_443_0_o = ( _rtB
-> B_1036_442_0 > _rtB -> B_1036_465_0_n ) ; _rtB -> B_1036_444_0_l = ( _rtB
-> B_1036_443_0_o && _rtB -> B_1036_295_0_o ) ; _rtB -> B_1036_445_0_f = (
_rtB -> B_1036_488_0_m && _rtB -> B_1036_315_0_l ) ; _rtB -> B_1036_446_0_p =
( _rtB -> B_1036_295_0_o && _rtB -> B_1036_315_0_l ) ; if ( _rtB ->
B_1036_438_0 ) { _rtB -> B_1036_448_0 = _rtB -> B_1036_502_0 ; } else { if (
_rtB -> B_1036_446_0_p ) { _rtB -> B_260_0_0 = ( _rtB -> B_1036_444_0_l ||
_rtB -> B_1036_445_0_f ) ; _rtB -> B_260_1_0 = ( _rtB -> B_260_0_0 && _rtB ->
B_1036_446_0_p ) ; _rtB -> B_261_2_0 = _rtB -> B_260_1_0 ; } else { if ( _rtB
-> B_1036_295_0_o ) { _rtB -> B_259_0_0 = _rtB -> B_1036_444_0_l ; } else {
_rtB -> B_259_0_0 = _rtB -> B_1036_445_0_f ; } _rtB -> B_261_2_0 = _rtB ->
B_259_0_0 ; } _rtB -> B_1036_448_0 = _rtB -> B_261_2_0 ; } if ( _rtB ->
B_1036_1568_0 > _rtP -> P_354 ) { if ( _rtB -> B_1036_448_0 > _rtP -> P_24 )
{ _rtB -> B_677_0_0 = _rtB -> B_1036_435_0 ; } else { _rtB -> B_677_0_0 =
_rtB -> B_1036_1579_0 ; } _rtB -> B_1036_450_0 = _rtB -> B_677_0_0 ; } else {
_rtB -> B_1036_450_0 = _rtB -> B_1036_1579_0 ; } _rtB -> B_1036_451_0 = _rtB
-> B_1036_1801_0_b + _rtB -> B_1036_450_0 ; _rtB -> B_1036_452_0 = _rtX ->
integrator_CSTATE_gv ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_is . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_is . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_356 ; _rtB
-> B_1036_453_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_mn . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_mn . Last , _rtDW -> TransportDelay_IWORK_mn .
Tail , _rtDW -> TransportDelay_IWORK_mn . Head , _rtP -> P_357 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_454_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_455_0 = _rtP
-> P_358 ; } _rtB -> B_1036_456_0_p = ( _rtB -> B_1036_454_0 >= _rtB ->
B_1036_455_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_457_0 = _rtDW -> Memory_PreviousInput_gt ; } if ( _rtB ->
B_1036_456_0_p ) { _rtB -> B_1005_0_0 = _rtB -> B_1036_452_0 - _rtB ->
B_1036_453_0 ; _rtB -> B_1005_1_0 = _rtP -> P_106 * _rtB -> B_1005_0_0 ; _rtB
-> B_1036_459_0 = _rtB -> B_1005_1_0 ; } else { _rtB -> B_1036_459_0 = _rtB
-> B_1036_457_0 ; } _rtB -> B_1036_460_0 = _rtX -> integrator_CSTATE_d3 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_g2 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_g2 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_361 ; _rtB -> B_1036_461_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_ay . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ay . Last , _rtDW -> TransportDelay_IWORK_ay . Tail ,
_rtDW -> TransportDelay_IWORK_ay . Head , _rtP -> P_362 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_462_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_463_0 = _rtP -> P_363 ; } _rtB ->
B_1036_464_0_b = ( _rtB -> B_1036_462_0 >= _rtB -> B_1036_463_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_465_0 =
_rtDW -> Memory_PreviousInput_ii ; } if ( _rtB -> B_1036_464_0_b ) { _rtB ->
B_1004_0_0 = _rtB -> B_1036_460_0 - _rtB -> B_1036_461_0 ; _rtB -> B_1004_1_0
= _rtP -> P_105 * _rtB -> B_1004_0_0 ; _rtB -> B_1036_467_0 = _rtB ->
B_1004_1_0 ; } else { _rtB -> B_1036_467_0 = _rtB -> B_1036_465_0 ; } _rtB ->
B_1036_468_0 . re = _rtB -> B_1036_459_0 ; _rtB -> B_1036_468_0 . im = _rtB
-> B_1036_467_0 ; _rtB -> B_1036_469_0 = muDoubleScalarHypot ( _rtB ->
B_1036_468_0 . re , _rtB -> B_1036_468_0 . im ) ; _rtB -> B_1036_469_1 =
muDoubleScalarAtan2 ( _rtB -> B_1036_468_0 . im , _rtB -> B_1036_468_0 . re )
; _rtB -> B_1036_470_0 = _rtX -> integrator_CSTATE_lg ; { real_T * * uBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mv . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mv .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_366 ; _rtB -> B_1036_471_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_nq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_nq . Last , _rtDW -> TransportDelay_IWORK_nq . Tail ,
_rtDW -> TransportDelay_IWORK_nq . Head , _rtP -> P_367 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_472_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_473_0 = _rtP -> P_368 ; } _rtB ->
B_1036_474_0_k = ( _rtB -> B_1036_472_0 >= _rtB -> B_1036_473_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_475_0 =
_rtDW -> Memory_PreviousInput_p ; } if ( _rtB -> B_1036_474_0_k ) { _rtB ->
B_1009_0_0 = _rtB -> B_1036_470_0 - _rtB -> B_1036_471_0 ; _rtB -> B_1009_1_0
= _rtP -> P_110 * _rtB -> B_1009_0_0 ; _rtB -> B_1036_477_0 = _rtB ->
B_1009_1_0 ; } else { _rtB -> B_1036_477_0 = _rtB -> B_1036_475_0 ; } _rtB ->
B_1036_478_0 = _rtX -> integrator_CSTATE_fo ; { real_T * * uBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_k3 . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k3 . TUbufferPtrs [
1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP ->
P_371 ; _rtB -> B_1036_479_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ni .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ni . Last , _rtDW ->
TransportDelay_IWORK_ni . Tail , _rtDW -> TransportDelay_IWORK_ni . Head ,
_rtP -> P_372 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_480_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_481_0 = _rtP -> P_373 ; } _rtB -> B_1036_482_0_m = ( _rtB ->
B_1036_480_0 >= _rtB -> B_1036_481_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_483_0 = _rtDW -> Memory_PreviousInput_dc
; } if ( _rtB -> B_1036_482_0_m ) { _rtB -> B_1008_0_0 = _rtB -> B_1036_478_0
- _rtB -> B_1036_479_0 ; _rtB -> B_1008_1_0 = _rtP -> P_109 * _rtB ->
B_1008_0_0 ; _rtB -> B_1036_485_0 = _rtB -> B_1008_1_0 ; } else { _rtB ->
B_1036_485_0 = _rtB -> B_1036_483_0 ; } _rtB -> B_1036_486_0 . re = _rtB ->
B_1036_477_0 ; _rtB -> B_1036_486_0 . im = _rtB -> B_1036_485_0 ; _rtB ->
B_1036_487_0 = muDoubleScalarHypot ( _rtB -> B_1036_486_0 . re , _rtB ->
B_1036_486_0 . im ) ; _rtB -> B_1036_487_1 = muDoubleScalarAtan2 ( _rtB ->
B_1036_486_0 . im , _rtB -> B_1036_486_0 . re ) ; _rtB -> B_1036_488_0 = _rtX
-> integrator_CSTATE_h ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fs . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_fs . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_376 ; _rtB
-> B_1036_489_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_mo . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_mo . Last , _rtDW -> TransportDelay_IWORK_mo .
Tail , _rtDW -> TransportDelay_IWORK_mo . Head , _rtP -> P_377 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_490_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_491_0 = _rtP
-> P_378 ; } _rtB -> B_1036_492_0_g = ( _rtB -> B_1036_490_0 >= _rtB ->
B_1036_491_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_493_0 = _rtDW -> Memory_PreviousInput_f ; } if ( _rtB ->
B_1036_492_0_g ) { _rtB -> B_1013_0_0 = _rtB -> B_1036_488_0 - _rtB ->
B_1036_489_0 ; _rtB -> B_1013_1_0 = _rtP -> P_114 * _rtB -> B_1013_0_0 ; _rtB
-> B_1036_495_0 = _rtB -> B_1013_1_0 ; } else { _rtB -> B_1036_495_0 = _rtB
-> B_1036_493_0 ; } _rtB -> B_1036_496_0 = _rtX -> integrator_CSTATE_gi ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_d .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_d . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_381 ; _rtB -> B_1036_497_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_k2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_k2 . Last , _rtDW -> TransportDelay_IWORK_k2 . Tail ,
_rtDW -> TransportDelay_IWORK_k2 . Head , _rtP -> P_382 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_498_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_499_0 = _rtP -> P_383 ; } _rtB ->
B_1036_500_0_b = ( _rtB -> B_1036_498_0 >= _rtB -> B_1036_499_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_501_0 =
_rtDW -> Memory_PreviousInput_p5 ; } if ( _rtB -> B_1036_500_0_b ) { _rtB ->
B_1012_0_0 = _rtB -> B_1036_496_0 - _rtB -> B_1036_497_0 ; _rtB -> B_1012_1_0
= _rtP -> P_113 * _rtB -> B_1012_0_0 ; _rtB -> B_1036_503_0 = _rtB ->
B_1012_1_0 ; } else { _rtB -> B_1036_503_0 = _rtB -> B_1036_501_0 ; } _rtB ->
B_1036_504_0 . re = _rtB -> B_1036_495_0 ; _rtB -> B_1036_504_0 . im = _rtB
-> B_1036_503_0 ; _rtB -> B_1036_505_0 = muDoubleScalarHypot ( _rtB ->
B_1036_504_0 . re , _rtB -> B_1036_504_0 . im ) ; _rtB -> B_1036_505_1 =
muDoubleScalarAtan2 ( _rtB -> B_1036_504_0 . im , _rtB -> B_1036_504_0 . re )
; _rtB -> B_1036_506_0 = _rtX -> integrator_CSTATE_df ; { real_T * * uBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK_p4 . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_p4 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_386 ; _rtB -> B_1036_507_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_or . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_or . Last , _rtDW -> TransportDelay_IWORK_or . Tail ,
_rtDW -> TransportDelay_IWORK_or . Head , _rtP -> P_387 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_508_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_509_0 = _rtP -> P_388 ; } _rtB ->
B_1036_510_0 = ( _rtB -> B_1036_508_0 >= _rtB -> B_1036_509_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_511_0 =
_rtDW -> Memory_PreviousInput_eu ; } if ( _rtB -> B_1036_510_0 ) { _rtB ->
B_995_0_0 = _rtB -> B_1036_506_0 - _rtB -> B_1036_507_0 ; _rtB -> B_995_1_0 =
_rtP -> P_96 * _rtB -> B_995_0_0 ; _rtB -> B_1036_513_0 = _rtB -> B_995_1_0 ;
} else { _rtB -> B_1036_513_0 = _rtB -> B_1036_511_0 ; } _rtB -> B_1036_514_0
= _rtX -> integrator_CSTATE_cv ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_ao . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_ao . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_391
; _rtB -> B_1036_515_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0
, * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_jy . CircularBufSize
, & _rtDW -> TransportDelay_IWORK_jy . Last , _rtDW ->
TransportDelay_IWORK_jy . Tail , _rtDW -> TransportDelay_IWORK_jy . Head ,
_rtP -> P_392 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_516_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_517_0 = _rtP -> P_393 ; } _rtB -> B_1036_518_0_i = ( _rtB ->
B_1036_516_0 >= _rtB -> B_1036_517_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_519_0 = _rtDW -> Memory_PreviousInput_c
; } if ( _rtB -> B_1036_518_0_i ) { _rtB -> B_994_0_0 = _rtB -> B_1036_514_0
- _rtB -> B_1036_515_0 ; _rtB -> B_994_1_0 = _rtP -> P_95 * _rtB -> B_994_0_0
; _rtB -> B_1036_521_0 = _rtB -> B_994_1_0 ; } else { _rtB -> B_1036_521_0 =
_rtB -> B_1036_519_0 ; } _rtB -> B_1036_522_0 . re = _rtB -> B_1036_513_0 ;
_rtB -> B_1036_522_0 . im = _rtB -> B_1036_521_0 ; _rtB -> B_1036_523_0 =
muDoubleScalarHypot ( _rtB -> B_1036_522_0 . re , _rtB -> B_1036_522_0 . im )
; _rtB -> B_1036_523_1 = muDoubleScalarAtan2 ( _rtB -> B_1036_522_0 . im ,
_rtB -> B_1036_522_0 . re ) ; _rtB -> B_1036_524_0 = _rtX ->
integrator_CSTATE_lz ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_oz . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_oz . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_396 ; _rtB
-> B_1036_525_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_il . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_il . Last , _rtDW -> TransportDelay_IWORK_il .
Tail , _rtDW -> TransportDelay_IWORK_il . Head , _rtP -> P_397 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_526_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_527_0 = _rtP
-> P_398 ; } _rtB -> B_1036_528_0 = ( _rtB -> B_1036_526_0 >= _rtB ->
B_1036_527_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_529_0 = _rtDW -> Memory_PreviousInput_jx ; } if ( _rtB ->
B_1036_528_0 ) { _rtB -> B_997_0_0 = _rtB -> B_1036_524_0 - _rtB ->
B_1036_525_0 ; _rtB -> B_997_1_0 = _rtP -> P_98 * _rtB -> B_997_0_0 ; _rtB ->
B_1036_531_0 = _rtB -> B_997_1_0 ; } else { _rtB -> B_1036_531_0 = _rtB ->
B_1036_529_0 ; } _rtB -> B_1036_532_0 = _rtX -> integrator_CSTATE_kk ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_od .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_od . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_401 ; _rtB -> B_1036_533_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_g2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_g2 . Last , _rtDW -> TransportDelay_IWORK_g2 . Tail ,
_rtDW -> TransportDelay_IWORK_g2 . Head , _rtP -> P_402 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_534_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_535_0 = _rtP -> P_403 ; } _rtB ->
B_1036_536_0 = ( _rtB -> B_1036_534_0 >= _rtB -> B_1036_535_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_537_0 =
_rtDW -> Memory_PreviousInput_m ; } if ( _rtB -> B_1036_536_0 ) { _rtB ->
B_996_0_0 = _rtB -> B_1036_532_0 - _rtB -> B_1036_533_0 ; _rtB -> B_996_1_0 =
_rtP -> P_97 * _rtB -> B_996_0_0 ; _rtB -> B_1036_539_0 = _rtB -> B_996_1_0 ;
} else { _rtB -> B_1036_539_0 = _rtB -> B_1036_537_0 ; } _rtB -> B_1036_540_0
. re = _rtB -> B_1036_531_0 ; _rtB -> B_1036_540_0 . im = _rtB ->
B_1036_539_0 ; _rtB -> B_1036_541_0 = muDoubleScalarHypot ( _rtB ->
B_1036_540_0 . re , _rtB -> B_1036_540_0 . im ) ; _rtB -> B_1036_541_1 =
muDoubleScalarAtan2 ( _rtB -> B_1036_540_0 . im , _rtB -> B_1036_540_0 . re )
; _rtB -> B_1036_542_0 = _rtX -> integrator_CSTATE_gx ; { real_T * * uBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_406 ; _rtB -> B_1036_543_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_nf . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_nf . Last , _rtDW -> TransportDelay_IWORK_nf . Tail ,
_rtDW -> TransportDelay_IWORK_nf . Head , _rtP -> P_407 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_544_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_545_0 = _rtP -> P_408 ; } _rtB ->
B_1036_546_0_b = ( _rtB -> B_1036_544_0 >= _rtB -> B_1036_545_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_547_0 =
_rtDW -> Memory_PreviousInput_fv ; } if ( _rtB -> B_1036_546_0_b ) { _rtB ->
B_999_0_0 = _rtB -> B_1036_542_0 - _rtB -> B_1036_543_0 ; _rtB -> B_999_1_0 =
_rtP -> P_100 * _rtB -> B_999_0_0 ; _rtB -> B_1036_549_0 = _rtB -> B_999_1_0
; } else { _rtB -> B_1036_549_0 = _rtB -> B_1036_547_0 ; } _rtB ->
B_1036_550_0 = _rtX -> integrator_CSTATE_fc ; { real_T * * uBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_hz . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hz . TUbufferPtrs [
1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP ->
P_411 ; _rtB -> B_1036_551_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_oo .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_oo . Last , _rtDW ->
TransportDelay_IWORK_oo . Tail , _rtDW -> TransportDelay_IWORK_oo . Head ,
_rtP -> P_412 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_552_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_553_0 = _rtP -> P_413 ; } _rtB -> B_1036_554_0 = ( _rtB ->
B_1036_552_0 >= _rtB -> B_1036_553_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_555_0 = _rtDW -> Memory_PreviousInput_d1
; } if ( _rtB -> B_1036_554_0 ) { _rtB -> B_998_0_0 = _rtB -> B_1036_550_0 -
_rtB -> B_1036_551_0 ; _rtB -> B_998_1_0 = _rtP -> P_99 * _rtB -> B_998_0_0 ;
_rtB -> B_1036_557_0 = _rtB -> B_998_1_0 ; } else { _rtB -> B_1036_557_0 =
_rtB -> B_1036_555_0 ; } _rtB -> B_1036_558_0 . re = _rtB -> B_1036_549_0 ;
_rtB -> B_1036_558_0 . im = _rtB -> B_1036_557_0 ; _rtB -> B_1036_559_0 =
muDoubleScalarHypot ( _rtB -> B_1036_558_0 . re , _rtB -> B_1036_558_0 . im )
; _rtB -> B_1036_559_1 = muDoubleScalarAtan2 ( _rtB -> B_1036_558_0 . im ,
_rtB -> B_1036_558_0 . re ) ; _rtB -> B_1036_560_0 = _rtX ->
integrator_CSTATE_bg ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_j . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_j . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_416 ; _rtB ->
B_1036_561_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ji . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_ji . Last , _rtDW -> TransportDelay_IWORK_ji .
Tail , _rtDW -> TransportDelay_IWORK_ji . Head , _rtP -> P_417 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_562_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_563_0 = _rtP
-> P_418 ; } _rtB -> B_1036_564_0 = ( _rtB -> B_1036_562_0 >= _rtB ->
B_1036_563_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_565_0 = _rtDW -> Memory_PreviousInput_l0 ; } if ( _rtB ->
B_1036_564_0 ) { _rtB -> B_1001_0_0 = _rtB -> B_1036_560_0 - _rtB ->
B_1036_561_0 ; _rtB -> B_1001_1_0 = _rtP -> P_102 * _rtB -> B_1001_0_0 ; _rtB
-> B_1036_567_0 = _rtB -> B_1001_1_0 ; } else { _rtB -> B_1036_567_0 = _rtB
-> B_1036_565_0 ; } _rtB -> B_1036_568_0 = _rtX -> integrator_CSTATE_g1 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h4 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_h4 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_421 ; _rtB -> B_1036_569_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_nb . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_nb . Last , _rtDW -> TransportDelay_IWORK_nb . Tail ,
_rtDW -> TransportDelay_IWORK_nb . Head , _rtP -> P_422 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_570_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_571_0 = _rtP -> P_423 ; } _rtB ->
B_1036_572_0 = ( _rtB -> B_1036_570_0 >= _rtB -> B_1036_571_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_573_0 =
_rtDW -> Memory_PreviousInput_bfe ; } if ( _rtB -> B_1036_572_0 ) { _rtB ->
B_1000_0_0 = _rtB -> B_1036_568_0 - _rtB -> B_1036_569_0 ; _rtB -> B_1000_1_0
= _rtP -> P_101 * _rtB -> B_1000_0_0 ; _rtB -> B_1036_575_0 = _rtB ->
B_1000_1_0 ; } else { _rtB -> B_1036_575_0 = _rtB -> B_1036_573_0 ; } _rtB ->
B_1036_576_0 . re = _rtB -> B_1036_567_0 ; _rtB -> B_1036_576_0 . im = _rtB
-> B_1036_575_0 ; _rtB -> B_1036_577_0 = muDoubleScalarHypot ( _rtB ->
B_1036_576_0 . re , _rtB -> B_1036_576_0 . im ) ; _rtB -> B_1036_577_1 =
muDoubleScalarAtan2 ( _rtB -> B_1036_576_0 . im , _rtB -> B_1036_576_0 . re )
; _rtB -> B_1036_578_0 = _rtX -> integrator_CSTATE_n3 ; { real_T * * uBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ax . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ax .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_426 ; _rtB -> B_1036_579_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_ca . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ca . Last , _rtDW -> TransportDelay_IWORK_ca . Tail ,
_rtDW -> TransportDelay_IWORK_ca . Head , _rtP -> P_427 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_580_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_581_0 = _rtP -> P_428 ; } _rtB ->
B_1036_582_0_h = ( _rtB -> B_1036_580_0 >= _rtB -> B_1036_581_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_583_0 =
_rtDW -> Memory_PreviousInput_df ; } if ( _rtB -> B_1036_582_0_h ) { _rtB ->
B_1007_0_0 = _rtB -> B_1036_578_0 - _rtB -> B_1036_579_0 ; _rtB -> B_1007_1_0
= _rtP -> P_108 * _rtB -> B_1007_0_0 ; _rtB -> B_1036_585_0 = _rtB ->
B_1007_1_0 ; } else { _rtB -> B_1036_585_0 = _rtB -> B_1036_583_0 ; } _rtB ->
B_1036_586_0 = _rtX -> integrator_CSTATE_cb ; { real_T * * uBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_h5 . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h5 . TUbufferPtrs [
1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP ->
P_431 ; _rtB -> B_1036_587_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_pd .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_pd . Last , _rtDW ->
TransportDelay_IWORK_pd . Tail , _rtDW -> TransportDelay_IWORK_pd . Head ,
_rtP -> P_432 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_588_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_589_0 = _rtP -> P_433 ; } _rtB -> B_1036_590_0_d = ( _rtB ->
B_1036_588_0 >= _rtB -> B_1036_589_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_591_0 = _rtDW -> Memory_PreviousInput_no
; } if ( _rtB -> B_1036_590_0_d ) { _rtB -> B_1006_0_0 = _rtB -> B_1036_586_0
- _rtB -> B_1036_587_0 ; _rtB -> B_1006_1_0 = _rtP -> P_107 * _rtB ->
B_1006_0_0 ; _rtB -> B_1036_593_0 = _rtB -> B_1006_1_0 ; } else { _rtB ->
B_1036_593_0 = _rtB -> B_1036_591_0 ; } _rtB -> B_1036_594_0 . re = _rtB ->
B_1036_585_0 ; _rtB -> B_1036_594_0 . im = _rtB -> B_1036_593_0 ; _rtB ->
B_1036_595_0 = muDoubleScalarHypot ( _rtB -> B_1036_594_0 . re , _rtB ->
B_1036_594_0 . im ) ; _rtB -> B_1036_595_1 = muDoubleScalarAtan2 ( _rtB ->
B_1036_594_0 . im , _rtB -> B_1036_594_0 . re ) ; _rtB -> B_1036_596_0 = _rtX
-> integrator_CSTATE_bx ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_pu . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_pu . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_436 ; _rtB
-> B_1036_597_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_a5 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_a5 . Last , _rtDW -> TransportDelay_IWORK_a5 .
Tail , _rtDW -> TransportDelay_IWORK_a5 . Head , _rtP -> P_437 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_598_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_599_0 = _rtP
-> P_438 ; } _rtB -> B_1036_600_0_f = ( _rtB -> B_1036_598_0 >= _rtB ->
B_1036_599_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_601_0 = _rtDW -> Memory_PreviousInput_et ; } if ( _rtB ->
B_1036_600_0_f ) { _rtB -> B_1011_0_0 = _rtB -> B_1036_596_0 - _rtB ->
B_1036_597_0 ; _rtB -> B_1011_1_0 = _rtP -> P_112 * _rtB -> B_1011_0_0 ; _rtB
-> B_1036_603_0 = _rtB -> B_1011_1_0 ; } else { _rtB -> B_1036_603_0 = _rtB
-> B_1036_601_0 ; } _rtB -> B_1036_604_0 = _rtX -> integrator_CSTATE_di ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_n .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_n . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_441 ; _rtB -> B_1036_605_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_o2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_o2 . Last , _rtDW -> TransportDelay_IWORK_o2 . Tail ,
_rtDW -> TransportDelay_IWORK_o2 . Head , _rtP -> P_442 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_606_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_607_0 = _rtP -> P_443 ; } _rtB ->
B_1036_608_0_d = ( _rtB -> B_1036_606_0 >= _rtB -> B_1036_607_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_609_0 =
_rtDW -> Memory_PreviousInput_pc ; } if ( _rtB -> B_1036_608_0_d ) { _rtB ->
B_1010_0_0 = _rtB -> B_1036_604_0 - _rtB -> B_1036_605_0 ; _rtB -> B_1010_1_0
= _rtP -> P_111 * _rtB -> B_1010_0_0 ; _rtB -> B_1036_611_0 = _rtB ->
B_1010_1_0 ; } else { _rtB -> B_1036_611_0 = _rtB -> B_1036_609_0 ; } _rtB ->
B_1036_612_0 . re = _rtB -> B_1036_603_0 ; _rtB -> B_1036_612_0 . im = _rtB
-> B_1036_611_0 ; _rtB -> B_1036_613_0 = muDoubleScalarHypot ( _rtB ->
B_1036_612_0 . re , _rtB -> B_1036_612_0 . im ) ; _rtB -> B_1036_613_1 =
muDoubleScalarAtan2 ( _rtB -> B_1036_612_0 . im , _rtB -> B_1036_612_0 . re )
; _rtB -> B_1036_614_0 = _rtX -> integrator_CSTATE_o3 ; { real_T * * uBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ml . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ml .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_446 ; _rtB -> B_1036_615_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_c0 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_c0 . Last , _rtDW -> TransportDelay_IWORK_c0 . Tail ,
_rtDW -> TransportDelay_IWORK_c0 . Head , _rtP -> P_447 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_616_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_617_0 = _rtP -> P_448 ; } _rtB ->
B_1036_618_0 = ( _rtB -> B_1036_616_0 >= _rtB -> B_1036_617_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_619_0 =
_rtDW -> Memory_PreviousInput_h ; } if ( _rtB -> B_1036_618_0 ) { _rtB ->
B_1015_0_0 = _rtB -> B_1036_614_0 - _rtB -> B_1036_615_0 ; _rtB -> B_1015_1_0
= _rtP -> P_116 * _rtB -> B_1015_0_0 ; _rtB -> B_1036_621_0 = _rtB ->
B_1015_1_0 ; } else { _rtB -> B_1036_621_0 = _rtB -> B_1036_619_0 ; } _rtB ->
B_1036_622_0 = _rtX -> integrator_CSTATE_mn ; { real_T * * uBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_om . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_om . TUbufferPtrs [
1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP ->
P_451 ; _rtB -> B_1036_623_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dq .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_dq . Last , _rtDW ->
TransportDelay_IWORK_dq . Tail , _rtDW -> TransportDelay_IWORK_dq . Head ,
_rtP -> P_452 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_624_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_625_0 = _rtP -> P_453 ; } _rtB -> B_1036_626_0 = ( _rtB ->
B_1036_624_0 >= _rtB -> B_1036_625_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_627_0 = _rtDW -> Memory_PreviousInput_h4
; } if ( _rtB -> B_1036_626_0 ) { _rtB -> B_1014_0_0 = _rtB -> B_1036_622_0 -
_rtB -> B_1036_623_0 ; _rtB -> B_1014_1_0 = _rtP -> P_115 * _rtB ->
B_1014_0_0 ; _rtB -> B_1036_629_0 = _rtB -> B_1014_1_0 ; } else { _rtB ->
B_1036_629_0 = _rtB -> B_1036_627_0 ; } _rtB -> B_1036_630_0 . re = _rtB ->
B_1036_621_0 ; _rtB -> B_1036_630_0 . im = _rtB -> B_1036_629_0 ; _rtB ->
B_1036_631_0 = muDoubleScalarHypot ( _rtB -> B_1036_630_0 . re , _rtB ->
B_1036_630_0 . im ) ; _rtB -> B_1036_631_1 = muDoubleScalarAtan2 ( _rtB ->
B_1036_630_0 . im , _rtB -> B_1036_630_0 . re ) ; _rtB -> B_1036_632_0 = _rtX
-> integrator_CSTATE_bi ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_gx . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_gx . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_456 ; _rtB
-> B_1036_633_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ny . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_ny . Last , _rtDW -> TransportDelay_IWORK_ny .
Tail , _rtDW -> TransportDelay_IWORK_ny . Head , _rtP -> P_457 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_634_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_635_0 = _rtP
-> P_458 ; } _rtB -> B_1036_636_0 = ( _rtB -> B_1036_634_0 >= _rtB ->
B_1036_635_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_637_0 = _rtDW -> Memory_PreviousInput_j3 ; } if ( _rtB ->
B_1036_636_0 ) { _rtB -> B_1003_0_0 = _rtB -> B_1036_632_0 - _rtB ->
B_1036_633_0 ; _rtB -> B_1003_1_0 = _rtP -> P_104 * _rtB -> B_1003_0_0 ; _rtB
-> B_1036_639_0 = _rtB -> B_1003_1_0 ; } else { _rtB -> B_1036_639_0 = _rtB
-> B_1036_637_0 ; } _rtB -> B_1036_640_0 = _rtX -> integrator_CSTATE_j ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k1 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_k1 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_461 ; _rtB -> B_1036_641_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_gd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_gd . Last , _rtDW -> TransportDelay_IWORK_gd . Tail ,
_rtDW -> TransportDelay_IWORK_gd . Head , _rtP -> P_462 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_642_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_643_0 = _rtP -> P_463 ; } _rtB ->
B_1036_644_0_p = ( _rtB -> B_1036_642_0 >= _rtB -> B_1036_643_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_645_0 =
_rtDW -> Memory_PreviousInput_ls ; } if ( _rtB -> B_1036_644_0_p ) { _rtB ->
B_1002_0_0 = _rtB -> B_1036_640_0 - _rtB -> B_1036_641_0 ; _rtB -> B_1002_1_0
= _rtP -> P_103 * _rtB -> B_1002_0_0 ; _rtB -> B_1036_647_0 = _rtB ->
B_1002_1_0 ; } else { _rtB -> B_1036_647_0 = _rtB -> B_1036_645_0 ; } _rtB ->
B_1036_648_0 . re = _rtB -> B_1036_639_0 ; _rtB -> B_1036_648_0 . im = _rtB
-> B_1036_647_0 ; _rtB -> B_1036_649_0 = muDoubleScalarHypot ( _rtB ->
B_1036_648_0 . re , _rtB -> B_1036_648_0 . im ) ; _rtB -> B_1036_649_1 =
muDoubleScalarAtan2 ( _rtB -> B_1036_648_0 . im , _rtB -> B_1036_648_0 . re )
; _rtB -> B_1036_650_0 [ 0 ] = _rtP -> P_465 * _rtB -> B_1036_469_0 ; _rtB ->
B_1036_650_0 [ 1 ] = _rtP -> P_465 * _rtB -> B_1036_487_0 ; _rtB ->
B_1036_650_0 [ 2 ] = _rtP -> P_465 * _rtB -> B_1036_505_0 ; _rtB ->
B_1036_650_0 [ 3 ] = _rtP -> P_465 * _rtB -> B_1036_523_0 ; _rtB ->
B_1036_650_0 [ 4 ] = _rtP -> P_465 * _rtB -> B_1036_541_0 ; _rtB ->
B_1036_650_0 [ 5 ] = _rtP -> P_465 * _rtB -> B_1036_559_0 ; _rtB ->
B_1036_650_0 [ 6 ] = _rtP -> P_465 * _rtB -> B_1036_577_0 ; _rtB ->
B_1036_650_0 [ 7 ] = _rtP -> P_465 * _rtB -> B_1036_595_0 ; _rtB ->
B_1036_650_0 [ 8 ] = _rtP -> P_465 * _rtB -> B_1036_613_0 ; _rtB ->
B_1036_650_0 [ 9 ] = _rtP -> P_465 * _rtB -> B_1036_631_0 ; _rtB ->
B_1036_650_0 [ 10 ] = _rtP -> P_465 * _rtB -> B_1036_649_0 ; _rtB ->
B_1036_651_0 = _rtP -> P_466 * _rtB -> B_1036_469_1 ; _rtB -> B_1036_652_0 =
_rtP -> P_467 * _rtB -> B_1036_487_1 ; _rtB -> B_1036_653_0 = _rtP -> P_468 *
_rtB -> B_1036_505_1 ; _rtB -> B_1036_654_0 = _rtP -> P_469 * _rtB ->
B_1036_523_1 ; _rtB -> B_1036_655_0 = _rtP -> P_470 * _rtB -> B_1036_541_1 ;
_rtB -> B_1036_656_0 = _rtP -> P_471 * _rtB -> B_1036_559_1 ; _rtB ->
B_1036_657_0 = _rtP -> P_472 * _rtB -> B_1036_577_1 ; _rtB -> B_1036_658_0 =
_rtP -> P_473 * _rtB -> B_1036_595_1 ; _rtB -> B_1036_659_0 = _rtP -> P_474 *
_rtB -> B_1036_613_1 ; _rtB -> B_1036_660_0 = _rtP -> P_475 * _rtB ->
B_1036_631_1 ; _rtB -> B_1036_661_0 = _rtP -> P_476 * _rtB -> B_1036_649_1 ;
_rtB -> B_1036_662_0 [ 0 ] = _rtP -> P_477 * _rtB -> B_1036_651_0 ; _rtB ->
B_1036_662_0 [ 1 ] = _rtP -> P_477 * _rtB -> B_1036_652_0 ; _rtB ->
B_1036_662_0 [ 2 ] = _rtP -> P_477 * _rtB -> B_1036_653_0 ; _rtB ->
B_1036_662_0 [ 3 ] = _rtP -> P_477 * _rtB -> B_1036_654_0 ; _rtB ->
B_1036_662_0 [ 4 ] = _rtP -> P_477 * _rtB -> B_1036_655_0 ; _rtB ->
B_1036_662_0 [ 5 ] = _rtP -> P_477 * _rtB -> B_1036_656_0 ; _rtB ->
B_1036_662_0 [ 6 ] = _rtP -> P_477 * _rtB -> B_1036_657_0 ; _rtB ->
B_1036_662_0 [ 7 ] = _rtP -> P_477 * _rtB -> B_1036_658_0 ; _rtB ->
B_1036_662_0 [ 8 ] = _rtP -> P_477 * _rtB -> B_1036_659_0 ; _rtB ->
B_1036_662_0 [ 9 ] = _rtP -> P_477 * _rtB -> B_1036_660_0 ; _rtB ->
B_1036_662_0 [ 10 ] = _rtP -> P_477 * _rtB -> B_1036_661_0 ; _rtB ->
B_1036_663_0 = muDoubleScalarCos ( _rtB -> B_1036_662_0 [ 3 ] ) ; _rtB ->
B_1036_664_0 = _rtB -> B_1036_650_0 [ 3 ] * _rtB -> B_1036_663_0 ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_665_0_g =
_rtDW -> u4_PreviousInput_b ; } _rtB -> B_1036_666_0_f = false ; _rtB ->
B_1036_667_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_668_0 = _rtB -> B_1036_603_0_m ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_668_0 , _rtB -> B_1036_666_0_f , _rtB ->
B_1036_667_0 , _rtB -> B_1036_601_0_o , & _rtB -> ONDelay_f3 , & _rtDW ->
ONDelay_f3 , & _rtZCE -> ONDelay_f3 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_670_0_o = _rtB -> B_1036_604_0_k ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_670_0_o , _rtB -> B_1036_666_0_f , _rtB
-> B_1036_667_0 , _rtB -> B_1036_601_0_o , & _rtB -> OFFDelay_ek , & _rtDW ->
OFFDelay_ek , & _rtZCE -> OFFDelay_ek ) ; _rtB -> B_1036_672_0_f = ( _rtB ->
ONDelay_f3 . B_90_17_0 || _rtB -> OFFDelay_ek . B_86_19_0 ) ; _rtB ->
B_1036_673_0_o = ( _rtB -> B_1036_665_0_g || _rtB -> B_1036_672_0_f ) ; _rtB
-> B_1036_674_0_l = _rtB -> B_1036_544_0_l ; _rtB -> B_1036_675_0 = ssGetT (
S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_676_0_e = _rtB -> B_1036_610_0 ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_676_0_e , _rtB -> B_1036_674_0_l , _rtB -> B_1036_675_0 , _rtB ->
B_1036_608_0 , & _rtB -> ONDelay_jx , & _rtDW -> ONDelay_jx , & _rtZCE ->
ONDelay_jx ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_678_0_g = _rtB -> B_1036_611_0_h ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_678_0_g , _rtB -> B_1036_674_0_l , _rtB -> B_1036_675_0 , _rtB ->
B_1036_608_0 , & _rtB -> OFFDelay_o5 , & _rtDW -> OFFDelay_o5 , & _rtZCE ->
OFFDelay_o5 ) ; _rtB -> B_1036_680_0_b = ( _rtB -> ONDelay_jx . B_90_17_0 ||
_rtB -> OFFDelay_o5 . B_86_19_0 ) ; _rtB -> B_1036_681_0_j = ! _rtB ->
B_1036_680_0_b ; _rtB -> B_1036_682_0_f = ( _rtB -> B_1036_673_0_o && _rtB ->
B_1036_681_0_j ) ; if ( _rtB -> B_1036_682_0_f ) { _rtB -> B_1036_683_0 =
_rtB -> B_1036_558_0_n ; } else { _rtB -> B_1036_683_0 = _rtB -> B_1036_584_0
; } _rtB -> B_1036_684_0 = _rtB -> B_1036_664_0 * _rtB -> B_1036_683_0 ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_685_0_c = _rtDW -> u4_PreviousInput_k ; } _rtB -> B_1036_686_0_f =
false ; _rtB -> B_1036_687_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_688_0 = _rtB -> B_1036_617_0_b ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_688_0 , _rtB -> B_1036_686_0_f , _rtB ->
B_1036_687_0 , _rtB -> B_1036_615_0_o , & _rtB -> ONDelay_jn , & _rtDW ->
ONDelay_jn , & _rtZCE -> ONDelay_jn ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_690_0_b = _rtB -> B_1036_618_0_f ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_690_0_b , _rtB -> B_1036_686_0_f , _rtB
-> B_1036_687_0 , _rtB -> B_1036_615_0_o , & _rtB -> OFFDelay_d4 , & _rtDW ->
OFFDelay_d4 , & _rtZCE -> OFFDelay_d4 ) ; _rtB -> B_1036_692_0_d = ( _rtB ->
ONDelay_jn . B_90_17_0 || _rtB -> OFFDelay_d4 . B_86_19_0 ) ; _rtB ->
B_1036_693_0 = ( _rtB -> B_1036_685_0_c || _rtB -> B_1036_692_0_d ) ; _rtB ->
B_1036_694_0_j = _rtB -> B_1036_547_0_j ; _rtB -> B_1036_695_0 = ssGetT ( S )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_696_0_o = _rtB -> B_1036_624_0_b ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_696_0_o , _rtB -> B_1036_694_0_j , _rtB -> B_1036_695_0 , _rtB ->
B_1036_622_0_a , & _rtB -> ONDelay_fa , & _rtDW -> ONDelay_fa , & _rtZCE ->
ONDelay_fa ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_698_0_l = _rtB -> B_1036_625_0_i ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_698_0_l , _rtB -> B_1036_694_0_j , _rtB -> B_1036_695_0 , _rtB ->
B_1036_622_0_a , & _rtB -> OFFDelay_nq , & _rtDW -> OFFDelay_nq , & _rtZCE ->
OFFDelay_nq ) ; _rtB -> B_1036_700_0 = ( _rtB -> ONDelay_fa . B_90_17_0 ||
_rtB -> OFFDelay_nq . B_86_19_0 ) ; _rtB -> B_1036_701_0_g = ! _rtB ->
B_1036_700_0 ; _rtB -> B_1036_702_0_m = ( _rtB -> B_1036_693_0 && _rtB ->
B_1036_701_0_g ) ; if ( _rtB -> B_1036_702_0_m ) { _rtB -> B_1036_703_0 =
_rtB -> B_1036_566_0 ; } else { _rtB -> B_1036_703_0 = _rtB -> B_1036_585_0_j
; } _rtB -> B_1036_704_0 = 0.0 * _rtB -> B_1036_703_0 ; _rtB -> B_1036_705_0
= _rtB -> B_1036_684_0 + _rtB -> B_1036_704_0 ; _rtB -> B_1036_706_0 = _rtB
-> B_1036_705_0 * _rtB -> B_1036_1743_0_d ; _rtB -> B_1036_707_0 =
muDoubleScalarCos ( _rtB -> B_1036_662_0 [ 4 ] ) ; _rtB -> B_1036_708_0 =
_rtB -> B_1036_650_0 [ 4 ] * _rtB -> B_1036_707_0 ; _rtB -> B_1036_709_0 =
_rtB -> B_1036_708_0 * _rtB -> B_1036_683_0 ; _rtB -> B_1036_710_0 = 0.0 *
_rtB -> B_1036_703_0 ; _rtB -> B_1036_711_0 = _rtB -> B_1036_709_0 + _rtB ->
B_1036_710_0 ; _rtB -> B_1036_712_0 = muDoubleScalarCos ( _rtB ->
B_1036_662_0 [ 5 ] ) ; _rtB -> B_1036_713_0 = _rtB -> B_1036_650_0 [ 5 ] *
_rtB -> B_1036_712_0 ; _rtB -> B_1036_714_0 = _rtB -> B_1036_713_0 * _rtB ->
B_1036_683_0 ; _rtB -> B_1036_715_0 = 0.0 * _rtB -> B_1036_703_0 ; _rtB ->
B_1036_716_0 = _rtB -> B_1036_714_0 + _rtB -> B_1036_715_0 ; _rtB ->
B_1036_717_0 = _rtB -> B_1036_711_0 + _rtB -> B_1036_716_0 ; _rtB ->
B_1036_718_0 = _rtB -> B_1036_717_0 + _rtB -> B_1036_705_0 ; _rtB ->
B_1036_719_0 = _rtB -> B_1036_718_0 * _rtB -> B_1036_1772_0_l ; if ( _rtB ->
B_1036_1180_0 > _rtP -> P_478 ) { _rtB -> B_687_0_0 = _rtB -> B_1036_719_0 *
_rtB -> B_1036_1743_0_d ; _rtB -> B_687_1_0 = _rtB -> B_1036_706_0 - _rtB ->
B_687_0_0 ; _rtB -> B_1036_721_0 = _rtB -> B_687_1_0 ; } else { _rtB ->
B_1036_721_0 = _rtB -> B_1036_706_0 ; } _rtB -> B_1036_722_0 = _rtB ->
B_1036_721_0 * _rtB -> B_1036_1745_0_a ; _rtB -> B_1036_723_0 =
muDoubleScalarSin ( _rtB -> B_1036_662_0 [ 3 ] ) ; _rtB -> B_1036_724_0 =
_rtB -> B_1036_723_0 * _rtB -> B_1036_650_0 [ 3 ] ; _rtB -> B_1036_725_0 =
_rtB -> B_1036_724_0 * _rtB -> B_1036_683_0 ; _rtB -> B_1036_726_0 = 0.0 *
_rtB -> B_1036_703_0 ; _rtB -> B_1036_727_0 = _rtB -> B_1036_725_0 + _rtB ->
B_1036_726_0 ; _rtB -> B_1036_728_0 = _rtB -> B_1036_727_0 * _rtB ->
B_1036_1743_0_d ; _rtB -> B_1036_729_0 = muDoubleScalarSin ( _rtB ->
B_1036_662_0 [ 4 ] ) ; _rtB -> B_1036_730_0 = _rtB -> B_1036_729_0 * _rtB ->
B_1036_650_0 [ 4 ] ; _rtB -> B_1036_731_0 = _rtB -> B_1036_730_0 * _rtB ->
B_1036_683_0 ; _rtB -> B_1036_732_0 = 0.0 * _rtB -> B_1036_703_0 ; _rtB ->
B_1036_733_0 = _rtB -> B_1036_731_0 + _rtB -> B_1036_732_0 ; _rtB ->
B_1036_734_0 = muDoubleScalarSin ( _rtB -> B_1036_662_0 [ 5 ] ) ; _rtB ->
B_1036_735_0 = _rtB -> B_1036_734_0 * _rtB -> B_1036_650_0 [ 5 ] ; _rtB ->
B_1036_736_0 = _rtB -> B_1036_735_0 * _rtB -> B_1036_683_0 ; _rtB ->
B_1036_737_0 = 0.0 * _rtB -> B_1036_703_0 ; _rtB -> B_1036_738_0 = _rtB ->
B_1036_736_0 + _rtB -> B_1036_737_0 ; _rtB -> B_1036_739_0 = _rtB ->
B_1036_733_0 + _rtB -> B_1036_738_0 ; _rtB -> B_1036_740_0 = _rtB ->
B_1036_739_0 + _rtB -> B_1036_727_0 ; _rtB -> B_1036_741_0 = _rtB ->
B_1036_740_0 * _rtB -> B_1036_1772_0_l ; if ( _rtB -> B_1036_1180_0 > _rtP ->
P_479 ) { _rtB -> B_688_0_0 = _rtB -> B_1036_741_0 * _rtB -> B_1036_1743_0_d
; _rtB -> B_688_1_0 = _rtB -> B_1036_728_0 - _rtB -> B_688_0_0 ; _rtB ->
B_1036_743_0 = _rtB -> B_688_1_0 ; } else { _rtB -> B_1036_743_0 = _rtB ->
B_1036_728_0 ; } _rtB -> B_1036_744_0 = _rtB -> B_1036_743_0 * _rtB ->
B_1036_1744_0_f ; _rtB -> B_1036_745_0 = _rtB -> B_1036_722_0 - _rtB ->
B_1036_744_0 ; _rtB -> B_1036_746_0_g = ! _rtB -> B_1036_682_0_f ; _rtB ->
B_1036_747_0_a = ! _rtB -> B_1036_702_0_m ; _rtB -> B_1036_748_0_e = ( _rtB
-> B_1036_746_0_g && _rtB -> B_1036_747_0_a ) ; _rtB -> B_1036_749_0 = _rtB
-> B_1036_664_0 * _rtB -> B_1036_664_0 ; _rtB -> B_1036_750_0 = _rtB ->
B_1036_724_0 * _rtB -> B_1036_724_0 ; _rtB -> B_1036_751_0 = _rtB ->
B_1036_749_0 + _rtB -> B_1036_750_0 ; _rtB -> B_1036_752_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_751_0 ) ; _rtB -> B_1036_753_0_b = ( _rtB
-> B_1036_752_0 > _rtB -> B_1036_549_0_i ) ; _rtB -> B_1036_754_0_e = ( _rtB
-> B_1036_753_0_b && _rtB -> B_1036_682_0_f ) ; _rtB -> B_1036_755_0_m = (
_rtB -> B_1036_572_0_n && _rtB -> B_1036_702_0_m ) ; _rtB -> B_1036_756_0_h =
( _rtB -> B_1036_682_0_f && _rtB -> B_1036_702_0_m ) ; if ( _rtB ->
B_1036_748_0_e ) { _rtB -> B_1036_758_0 = _rtB -> B_1036_586_0_g ; } else {
if ( _rtB -> B_1036_756_0_h ) { _rtB -> B_305_0_0 = ( _rtB -> B_1036_754_0_e
|| _rtB -> B_1036_755_0_m ) ; _rtB -> B_305_1_0 = ( _rtB -> B_305_0_0 && _rtB
-> B_1036_756_0_h ) ; _rtB -> B_306_2_0 = _rtB -> B_305_1_0 ; } else { if (
_rtB -> B_1036_682_0_f ) { _rtB -> B_304_0_0 = _rtB -> B_1036_754_0_e ; }
else { _rtB -> B_304_0_0 = _rtB -> B_1036_755_0_m ; } _rtB -> B_306_2_0 =
_rtB -> B_304_0_0 ; } _rtB -> B_1036_758_0 = _rtB -> B_306_2_0 ; } if ( _rtB
-> B_1036_1737_0_j > _rtP -> P_480 ) { if ( _rtB -> B_1036_758_0 > _rtP ->
P_28 ) { _rtB -> B_690_0_0 = _rtB -> B_1036_745_0 ; } else { _rtB ->
B_690_0_0 = _rtB -> B_1036_1748_0_e ; } _rtB -> B_1036_760_0 = _rtB ->
B_690_0_0 ; } else { _rtB -> B_1036_760_0 = _rtB -> B_1036_1748_0_e ; } _rtB
-> B_1036_761_0 = _rtB -> B_1036_451_0 + _rtB -> B_1036_760_0 ; _rtB ->
B_1036_762_0 = _rtB -> B_1036_761_0 * _rtB -> B_1036_761_0 ; _rtB ->
B_1036_763_0 = _rtB -> B_1036_433_0 * _rtB -> B_1036_1576_0 ; _rtB ->
B_1036_764_0 = _rtB -> B_1036_413_0 * _rtB -> B_1036_1575_0 ; _rtB ->
B_1036_765_0 = _rtB -> B_1036_763_0 + _rtB -> B_1036_764_0 ; if ( _rtB ->
B_1036_1568_0 > _rtP -> P_481 ) { if ( _rtB -> B_1036_448_0 > _rtP -> P_25 )
{ _rtB -> B_678_0_0 = _rtB -> B_1036_765_0 ; } else { _rtB -> B_678_0_0 =
_rtB -> B_1036_1579_0 ; } _rtB -> B_1036_767_0 = _rtB -> B_678_0_0 ; } else {
_rtB -> B_1036_767_0 = _rtB -> B_1036_1579_0 ; } _rtB -> B_1036_768_0 = _rtB
-> B_1036_1800_0_j + _rtB -> B_1036_767_0 ; _rtB -> B_1036_769_0 = _rtB ->
B_1036_743_0 * _rtB -> B_1036_1745_0_a ; _rtB -> B_1036_770_0 = _rtB ->
B_1036_721_0 * _rtB -> B_1036_1744_0_f ; _rtB -> B_1036_771_0 = _rtB ->
B_1036_769_0 + _rtB -> B_1036_770_0 ; if ( _rtB -> B_1036_1737_0_j > _rtP ->
P_482 ) { if ( _rtB -> B_1036_758_0 > _rtP -> P_29 ) { _rtB -> B_691_0_0 =
_rtB -> B_1036_771_0 ; } else { _rtB -> B_691_0_0 = _rtB -> B_1036_1748_0_e ;
} _rtB -> B_1036_773_0 = _rtB -> B_691_0_0 ; } else { _rtB -> B_1036_773_0 =
_rtB -> B_1036_1748_0_e ; } _rtB -> B_1036_774_0 = _rtB -> B_1036_768_0 +
_rtB -> B_1036_773_0 ; _rtB -> B_1036_775_0 = _rtB -> B_1036_774_0 * _rtB ->
B_1036_774_0 ; _rtB -> B_1036_776_0 = _rtB -> B_1036_762_0 + _rtB ->
B_1036_775_0 ; _rtB -> B_1036_777_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_776_0 ) ; _rtB -> B_1036_778_0 = _rtB -> B_1036_450_0 * _rtB ->
B_1036_450_0 ; _rtB -> B_1036_779_0 = _rtB -> B_1036_767_0 * _rtB ->
B_1036_767_0 ; _rtB -> B_1036_780_0 = _rtB -> B_1036_778_0 + _rtB ->
B_1036_779_0 ; _rtB -> B_1036_781_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_780_0 ) ; _rtB -> B_1036_782_0_b = ( _rtB -> B_1036_1629_0_n > _rtB ->
B_1036_781_0 ) ; if ( _rtB -> B_1036_782_0_b ) { _rtB -> B_1036_783_0 = _rtB
-> B_1036_1624_0_j ; _rtB -> B_1036_785_0 = _rtB -> B_1036_1626_0_p ; } else
{ _rtB -> B_1036_783_0 = _rtB -> B_1036_450_0 ; _rtB -> B_1036_785_0 = _rtB
-> B_1036_767_0 ; } _rtB -> B_1036_784_0 = _rtB -> B_1036_783_0 * _rtB ->
B_1036_783_0 ; _rtB -> B_1036_786_0 = _rtB -> B_1036_785_0 * _rtB ->
B_1036_785_0 ; _rtB -> B_1036_787_0 = _rtB -> B_1036_784_0 + _rtB ->
B_1036_786_0 ; _rtB -> B_1036_788_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_787_0 ) ; _rtB -> B_1036_789_0 = _rtB -> B_1036_760_0 * _rtB ->
B_1036_760_0 ; _rtB -> B_1036_790_0 = _rtB -> B_1036_773_0 * _rtB ->
B_1036_773_0 ; _rtB -> B_1036_791_0 = _rtB -> B_1036_789_0 + _rtB ->
B_1036_790_0 ; _rtB -> B_1036_792_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_791_0 ) ; _rtB -> B_1036_793_0_d = ( _rtB -> B_1036_788_0 > _rtB ->
B_1036_792_0 ) ; if ( _rtB -> B_1036_793_0_d ) { _rtB -> B_1036_794_0 = _rtB
-> B_1036_783_0 ; _rtB -> B_1036_795_0 = _rtB -> B_1036_785_0 ; } else { _rtB
-> B_1036_794_0 = _rtB -> B_1036_760_0 ; _rtB -> B_1036_795_0 = _rtB ->
B_1036_773_0 ; } _rtB -> B_1036_796_0 = _rtB -> B_1036_794_0 - _rtB ->
B_1036_761_0 ; _rtB -> B_1036_797_0 = _rtB -> B_1036_795_0 - _rtB ->
B_1036_774_0 ; _rtB -> B_1036_798_0 = _rtB -> B_1036_794_0 * _rtB ->
B_1036_794_0 ; _rtB -> B_1036_799_0 = _rtB -> B_1036_795_0 * _rtB ->
B_1036_795_0 ; _rtB -> B_1036_800_0 = _rtB -> B_1036_798_0 + _rtB ->
B_1036_799_0 ; _rtB -> B_1036_801_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_800_0 ) ; _rtB -> B_1036_802_0 = _rtB -> B_1036_794_0 / _rtB ->
B_1036_801_0 ; _rtB -> B_1036_803_0 = _rtB -> B_1036_796_0 * _rtB ->
B_1036_796_0 ; _rtB -> B_1036_804_0 = _rtB -> B_1036_797_0 * _rtB ->
B_1036_797_0 ; _rtB -> B_1036_805_0 = _rtB -> B_1036_803_0 + _rtB ->
B_1036_804_0 ; _rtB -> B_1036_806_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_805_0 ) ; _rtB -> B_1036_807_0 = _rtB -> B_1036_796_0 / _rtB ->
B_1036_806_0 ; _rtB -> B_1036_808_0 = _rtB -> B_1036_802_0 - _rtB ->
B_1036_807_0 ; _rtB -> B_1036_809_0 = _rtB -> B_1036_808_0 * _rtB ->
B_1036_808_0 ; _rtB -> B_1036_810_0 = _rtB -> B_1036_795_0 / _rtB ->
B_1036_801_0 ; _rtB -> B_1036_811_0 = _rtB -> B_1036_797_0 / _rtB ->
B_1036_806_0 ; _rtB -> B_1036_812_0 = _rtB -> B_1036_810_0 - _rtB ->
B_1036_811_0 ; _rtB -> B_1036_813_0 = _rtB -> B_1036_812_0 * _rtB ->
B_1036_812_0 ; _rtB -> B_1036_814_0 = _rtB -> B_1036_809_0 + _rtB ->
B_1036_813_0 ; _rtB -> B_1036_815_0 = _rtB -> B_1036_814_0 * _rtB ->
B_1036_1558_0 ; _rtB -> B_1036_816_0 = _rtB -> B_1036_1557_0 - _rtB ->
B_1036_815_0 ; u0 = _rtB -> B_1036_816_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB -> B_1036_817_0 =
muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_818_0 = _rtB -> B_1036_810_0 -
_rtB -> B_1036_807_0 ; _rtB -> B_1036_819_0 = _rtB -> B_1036_818_0 * _rtB ->
B_1036_818_0 ; _rtB -> B_1036_820_0 = _rtB -> B_1036_802_0 + _rtB ->
B_1036_811_0 ; _rtB -> B_1036_821_0 = _rtB -> B_1036_820_0 * _rtB ->
B_1036_820_0 ; _rtB -> B_1036_822_0 = _rtB -> B_1036_819_0 + _rtB ->
B_1036_821_0 ; _rtB -> B_1036_823_0 = ( _rtB -> B_1036_822_0 > _rtB ->
B_1036_1555_0 ) ; if ( _rtB -> B_1036_823_0 ) { _rtB -> B_671_0_0 = _rtB ->
B_1036_817_0 * _rtB -> B_1036_1556_0 ; _rtB -> B_1036_825_0 = _rtB ->
B_671_0_0 ; } else { _rtB -> B_1036_825_0 = _rtB -> B_1036_817_0 ; } _rtB ->
B_1036_826_0 = muDoubleScalarCos ( _rtB -> B_1036_825_0 ) ; _rtB ->
B_1036_827_0_o = ( _rtB -> B_1036_826_0 >= _rtB -> B_1036_1554_0 ) ; if (
_rtB -> B_1036_827_0_o ) { _rtB -> B_668_0_0 = _rtB -> B_1036_797_0 * _rtB ->
B_1036_797_0 ; _rtB -> B_668_1_0 = _rtB -> B_1036_796_0 * _rtB ->
B_1036_796_0 ; _rtB -> B_668_2_0 = _rtB -> B_668_1_0 + _rtB -> B_668_0_0 ;
_rtB -> B_668_3_0 = muDoubleScalarSqrt ( _rtB -> B_668_2_0 ) ; _rtB ->
B_668_4_0 = _rtB -> B_1036_795_0 * _rtB -> B_1036_795_0 ; _rtB -> B_668_5_0 =
_rtB -> B_1036_794_0 * _rtB -> B_1036_794_0 ; _rtB -> B_668_6_0 = _rtB ->
B_668_5_0 + _rtB -> B_668_4_0 ; _rtB -> B_668_7_0 = muDoubleScalarSqrt ( _rtB
-> B_668_6_0 ) ; _rtB -> B_668_8_0 = _rtB -> B_668_7_0 * _rtB -> B_668_3_0 *
_rtB -> B_1036_826_0 ; _rtB -> B_668_9_0 = muDoubleScalarSqrt ( _rtB ->
B_668_8_0 ) ; _rtB -> B_1036_829_0 = _rtB -> B_668_9_0 ; } else { _rtB ->
B_1036_829_0 = _rtB -> B_1036_1553_0 ; } if ( _rtB -> B_1036_1217_0_i ) {
_rtB -> B_1036_831_0 = _rtB -> B_1036_1193_0_d ; } else { _rtB -> B_612_0_0 =
( _rtB -> B_1036_829_0 > _rtB -> B_1036_1208_0_l ) ; if ( _rtB -> B_612_0_0 )
{ if ( _rtB -> B_1036_1205_0_f ) { _rtB -> B_610_0_0 = _rtB ->
B_1036_1197_0_g * _rtB -> B_1036_829_0 ; _rtB -> B_610_1_0 = _rtB ->
B_610_0_0 + _rtB -> B_1036_1222_0_l ; _rtB -> B_611_1_0 = _rtB -> B_610_1_0 ;
} else { _rtB -> B_611_1_0 = _rtB -> B_1036_1189_0 ; } _rtB -> B_612_3_0 =
_rtB -> B_611_1_0 ; } else { _rtB -> B_609_0_0 = ( _rtB -> B_1036_829_0 <=
_rtB -> B_1036_1208_0_l ) ; _rtB -> B_609_1_0 = ( _rtB -> B_1036_829_0 > _rtB
-> B_1036_1192_0_c ) ; _rtB -> B_609_2_0 = ( _rtB -> B_609_1_0 && _rtB ->
B_609_0_0 ) ; if ( _rtB -> B_609_2_0 ) { _rtB -> B_608_0_0 = _rtB ->
B_1036_829_0 * _rtB -> B_1036_1197_0_g ; _rtB -> B_608_1_0 = _rtB ->
B_608_0_0 + _rtB -> B_1036_1222_0_l ; _rtB -> B_609_4_0 = _rtB -> B_608_1_0 ;
} else { _rtB -> B_609_4_0 = _rtB -> B_1036_1193_0_d ; } _rtB -> B_612_3_0 =
_rtB -> B_609_4_0 ; } _rtB -> B_1036_831_0 = _rtB -> B_612_3_0 ; } if ( _rtB
-> B_1036_1246_0_p ) { _rtB -> B_1036_833_0 = _rtB -> B_1036_831_0 ; } else {
_rtB -> B_617_0_0 = ( _rtB -> B_1036_829_0 > _rtB -> B_1036_1237_0 ) ; if (
_rtB -> B_617_0_0 ) { if ( _rtB -> B_1036_1234_0_b ) { _rtB -> B_615_0_0 =
_rtB -> B_1036_1226_0 * _rtB -> B_1036_829_0 ; _rtB -> B_615_1_0 = _rtB ->
B_615_0_0 + _rtB -> B_1036_1251_0_k ; _rtB -> B_616_1_0 = _rtB -> B_615_1_0 ;
} else { _rtB -> B_616_1_0 = _rtB -> B_1036_1189_0 ; } _rtB -> B_617_3_0 =
_rtB -> B_616_1_0 ; } else { _rtB -> B_614_0_0 = ( _rtB -> B_1036_829_0 <=
_rtB -> B_1036_1237_0 ) ; _rtB -> B_614_1_0 = ( _rtB -> B_1036_829_0 > _rtB
-> B_1036_1172_0 ) ; _rtB -> B_614_2_0 = ( _rtB -> B_614_1_0 && _rtB ->
B_614_0_0 ) ; if ( _rtB -> B_614_2_0 ) { _rtB -> B_613_0_0 = _rtB ->
B_1036_829_0 * _rtB -> B_1036_1226_0 ; _rtB -> B_613_1_0 = _rtB -> B_613_0_0
+ _rtB -> B_1036_1251_0_k ; _rtB -> B_614_4_0 = _rtB -> B_613_1_0 ; } else {
_rtB -> B_614_4_0 = _rtB -> B_1036_831_0 ; } _rtB -> B_617_3_0 = _rtB ->
B_614_4_0 ; } _rtB -> B_1036_833_0 = _rtB -> B_617_3_0 ; } if ( _rtB ->
B_1036_1275_0_l ) { _rtB -> B_1036_835_0 = _rtB -> B_1036_833_0 ; } else {
_rtB -> B_622_0_0 = ( _rtB -> B_1036_829_0 > _rtB -> B_1036_1266_0_i ) ; if (
_rtB -> B_622_0_0 ) { if ( _rtB -> B_1036_1263_0_a ) { _rtB -> B_620_0_0 =
_rtB -> B_1036_1255_0 * _rtB -> B_1036_829_0 ; _rtB -> B_620_1_0 = _rtB ->
B_620_0_0 + _rtB -> B_1036_1280_0_c ; _rtB -> B_621_1_0 = _rtB -> B_620_1_0 ;
} else { _rtB -> B_621_1_0 = _rtB -> B_1036_1189_0 ; } _rtB -> B_622_3_0 =
_rtB -> B_621_1_0 ; } else { _rtB -> B_619_0_0 = ( _rtB -> B_1036_829_0 <=
_rtB -> B_1036_1266_0_i ) ; _rtB -> B_619_1_0 = ( _rtB -> B_1036_829_0 > _rtB
-> B_1036_1174_0 ) ; _rtB -> B_619_2_0 = ( _rtB -> B_619_1_0 && _rtB ->
B_619_0_0 ) ; if ( _rtB -> B_619_2_0 ) { _rtB -> B_618_0_0 = _rtB ->
B_1036_829_0 * _rtB -> B_1036_1255_0 ; _rtB -> B_618_1_0 = _rtB -> B_618_0_0
+ _rtB -> B_1036_1280_0_c ; _rtB -> B_619_4_0 = _rtB -> B_618_1_0 ; } else {
_rtB -> B_619_4_0 = _rtB -> B_1036_833_0 ; } _rtB -> B_622_3_0 = _rtB ->
B_619_4_0 ; } _rtB -> B_1036_835_0 = _rtB -> B_622_3_0 ; } if ( _rtB ->
B_1036_1304_0_c ) { _rtB -> B_1036_837_0 = _rtB -> B_1036_835_0 ; } else {
_rtB -> B_627_0_0 = ( _rtB -> B_1036_829_0 > _rtB -> B_1036_1295_0 ) ; if (
_rtB -> B_627_0_0 ) { if ( _rtB -> B_1036_1292_0_a ) { _rtB -> B_625_0_0 =
_rtB -> B_1036_1284_0_e * _rtB -> B_1036_829_0 ; _rtB -> B_625_1_0 = _rtB ->
B_625_0_0 + _rtB -> B_1036_1309_0 ; _rtB -> B_626_1_0 = _rtB -> B_625_1_0 ; }
else { _rtB -> B_626_1_0 = _rtB -> B_1036_1189_0 ; } _rtB -> B_627_3_0 = _rtB
-> B_626_1_0 ; } else { _rtB -> B_624_0_0 = ( _rtB -> B_1036_829_0 <= _rtB ->
B_1036_1295_0 ) ; _rtB -> B_624_1_0 = ( _rtB -> B_1036_829_0 > _rtB ->
B_1036_1176_0 ) ; _rtB -> B_624_2_0 = ( _rtB -> B_624_1_0 && _rtB ->
B_624_0_0 ) ; if ( _rtB -> B_624_2_0 ) { _rtB -> B_623_0_0 = _rtB ->
B_1036_829_0 * _rtB -> B_1036_1284_0_e ; _rtB -> B_623_1_0 = _rtB ->
B_623_0_0 + _rtB -> B_1036_1309_0 ; _rtB -> B_624_4_0 = _rtB -> B_623_1_0 ; }
else { _rtB -> B_624_4_0 = _rtB -> B_1036_835_0 ; } _rtB -> B_627_3_0 = _rtB
-> B_624_4_0 ; } _rtB -> B_1036_837_0 = _rtB -> B_627_3_0 ; } _rtB ->
B_1036_838_0 = ( _rtB -> B_1036_777_0 > _rtB -> B_1036_837_0 ) ; _rtB ->
B_1036_839_0 = ( _rtB -> B_1036_837_0 == _rtB -> B_1036_1544_0 ) ; _rtB ->
B_1036_840_0_k = ! _rtB -> B_1036_839_0 ; _rtB -> B_1036_841_0_m = ( _rtB ->
B_1036_838_0 && _rtB -> B_1036_840_0_k ) ; _rtB -> B_1036_842_0_f = ( _rtB ->
B_1036_777_0 > _rtB -> B_1036_1189_0 ) ; _rtB -> B_1036_843_0 = ( _rtB ->
B_1036_842_0_f && _rtB -> B_1036_1546_0_f ) ; _rtB -> B_1036_844_0 = ( _rtB
-> B_1036_841_0_m || _rtB -> B_1036_843_0 ) ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_845_0 = _rtDW ->
Memory_PreviousInput_kv ; _rtB -> B_1036_846_0 = ! _rtB -> B_1036_845_0 ; }
if ( _rtB -> B_1036_1181_0 > _rtP -> P_483 ) { _rtB -> B_724_0_0 = ( _rtB ->
B_1036_844_0 && _rtB -> B_1036_846_0 ) ; _rtB -> B_1036_848_0_o = _rtB ->
B_724_0_0 ; } else { _rtB -> B_1036_848_0_o = _rtB -> B_1036_844_0 ; } _rtB
-> B_1036_849_0 = _rtB -> B_1036_705_0 * _rtB -> B_1036_705_0 ; _rtB ->
B_1036_850_0 = _rtB -> B_1036_727_0 * _rtB -> B_1036_727_0 ; _rtB ->
B_1036_851_0 = _rtB -> B_1036_849_0 + _rtB -> B_1036_850_0 ; _rtB ->
B_1036_852_0 = muDoubleScalarSqrt ( _rtB -> B_1036_851_0 ) ; _rtB ->
B_1036_853_0 = ( _rtB -> B_1036_852_0 < 0.0 ) ; _rtB -> B_1036_854_0_i = !
_rtB -> B_1036_853_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_1036_855_0 = _rtDW -> u_PreviousInput_pk ; } _rtB ->
B_1036_856_0 = _rtB -> B_1036_705_0 - _rtB -> B_1036_855_0 ; _rtB ->
B_1036_857_0 = _rtB -> B_1036_856_0 * _rtB -> B_1036_856_0 ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_858_0 =
_rtDW -> u_PreviousInput_l ; } _rtB -> B_1036_859_0 = _rtB -> B_1036_727_0 -
_rtB -> B_1036_858_0 ; _rtB -> B_1036_860_0 = _rtB -> B_1036_859_0 * _rtB ->
B_1036_859_0 ; _rtB -> B_1036_861_0 = _rtB -> B_1036_857_0 + _rtB ->
B_1036_860_0 ; _rtB -> B_1036_862_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_861_0 ) ; _rtB -> B_1036_863_0 = _rtB -> B_1036_705_0 * _rtB ->
B_1036_705_0 ; _rtB -> B_1036_864_0 = _rtB -> B_1036_727_0 * _rtB ->
B_1036_727_0 ; _rtB -> B_1036_865_0 = _rtB -> B_1036_863_0 + _rtB ->
B_1036_864_0 ; _rtB -> B_1036_866_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_865_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_867_0 = _rtB -> B_1036_855_0 * _rtB -> B_1036_855_0 ; _rtB ->
B_1036_868_0 = _rtB -> B_1036_858_0 * _rtB -> B_1036_858_0 ; _rtB ->
B_1036_869_0 = _rtB -> B_1036_867_0 + _rtB -> B_1036_868_0 ; _rtB ->
B_1036_870_0 = muDoubleScalarSqrt ( _rtB -> B_1036_869_0 ) ; } maxV =
muDoubleScalarMax ( _rtB -> B_1036_866_0 , _rtB -> B_1036_870_0 ) ; _rtB ->
B_1036_871_0 = maxV ; _rtB -> B_1036_872_0_k = ( _rtB -> B_1036_871_0 == _rtB
-> B_1036_433_0_o ) ; if ( _rtB -> B_1036_872_0_k ) { _rtB -> B_1036_873_0 =
_rtB -> B_1036_434_0_l ; } else { _rtB -> B_1036_873_0 = _rtB -> B_1036_871_0
; } _rtB -> B_1036_874_0 = _rtB -> B_1036_862_0 / _rtB -> B_1036_873_0 ; _rtB
-> B_1036_875_0 = ( _rtB -> B_1036_874_0 > _rtB -> B_1036_416_0_a ) ; _rtB ->
B_1036_876_0 = ( _rtB -> B_1036_854_0_i && _rtB -> B_1036_875_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_877_0 =
_rtDW -> u_PreviousInput_l1 ; _rtB -> B_1036_878_0 = _rtDW ->
u_PreviousInput_i ; _rtB -> B_1036_879_0 = _rtDW -> u_PreviousInput_a ; _rtB
-> B_1036_880_0 = ( _rtB -> B_1036_877_0 + _rtB -> B_1036_878_0 ) + _rtB ->
B_1036_879_0 ; _rtB -> B_1036_881_0 = _rtB -> B_1036_880_0 * _rtB ->
B_1036_430_0_o ; } _rtB -> B_1036_882_0 = _rtB -> B_1036_862_0 - _rtB ->
B_1036_881_0 ; _rtB -> B_1036_883_0 = muDoubleScalarAbs ( _rtB ->
B_1036_882_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_884_0 = ( _rtB -> B_1036_881_0 == _rtB -> B_1036_431_0_f ) ;
if ( _rtB -> B_1036_884_0 ) { _rtB -> B_1036_885_0 = _rtB -> B_1036_432_0 ; }
else { _rtB -> B_1036_885_0 = _rtB -> B_1036_881_0 ; } } _rtB -> B_1036_886_0
= _rtB -> B_1036_883_0 / _rtB -> B_1036_885_0 ; _rtB -> B_1036_887_0_k = (
_rtB -> B_1036_886_0 > _rtB -> B_1036_415_0_b ) ; _rtB -> B_1036_888_0_n = (
_rtB -> B_1036_862_0 < _rtB -> B_1036_413_0_e ) ; _rtB -> B_1036_889_0 = !
_rtB -> B_1036_888_0_n ; _rtB -> B_1036_890_0 = ( _rtB -> B_1036_887_0_k &&
_rtB -> B_1036_889_0 ) ; _rtB -> B_1036_891_0 = ( _rtB -> B_1036_876_0 &&
_rtB -> B_1036_890_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_1036_892_0 = _rtDW -> u4_PreviousInput_gb ; _rtB ->
B_1036_893_0_f = _rtDW -> u1_PreviousInput_d ; _rtB -> B_1036_894_0_m = !
_rtB -> B_1036_893_0_f ; _rtB -> B_1036_895_0_i = ( _rtB -> B_1036_892_0 &&
_rtB -> B_1036_894_0_m ) ; } _rtB -> B_1036_896_0 = ( _rtB -> B_1036_891_0 ||
_rtB -> B_1036_895_0_i ) ; _rtB -> B_1036_897_0 = _rtB -> B_1036_896_0 ; _rtB
-> B_1036_898_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_899_0 = _rtB -> B_1036_426_0_d ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_899_0 , _rtB -> B_1036_897_0 , _rtB ->
B_1036_898_0 , _rtB -> B_1036_424_0_f , & _rtB -> ONDelay_hb , & _rtDW ->
ONDelay_hb , & _rtZCE -> ONDelay_hb ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_901_0_i = _rtB -> B_1036_427_0_j ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_901_0_i , _rtB -> B_1036_897_0 , _rtB ->
B_1036_898_0 , _rtB -> B_1036_424_0_f , & _rtB -> OFFDelay_a , & _rtDW ->
OFFDelay_a , & _rtZCE -> OFFDelay_a ) ; _rtB -> B_1036_903_0_b = ( _rtB ->
ONDelay_hb . B_90_17_0 || _rtB -> OFFDelay_a . B_86_19_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_904_0_n =
_rtDW -> Memory_PreviousInput_pt ; _rtB -> B_1036_905_0_i = ! _rtB ->
B_1036_904_0_n ; } _rtB -> B_1036_906_0_n = ( _rtB -> B_1036_903_0_b && _rtB
-> B_1036_905_0_i ) ; _rtB -> B_1036_907_0 = ( _rtB -> B_1036_906_0_n && _rtB
-> B_1036_420_0_m ) ; _rtB -> B_1036_908_0_m = ( _rtB -> B_1036_388_0_e ||
_rtB -> B_1036_907_0 ) ; _rtB -> B_1036_909_0 = _rtB -> B_1036_908_0_m ; _rtB
-> B_1036_910_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_911_0_p = _rtB -> B_1036_1911_0_h ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_911_0_p , _rtB -> B_1036_909_0 , _rtB ->
B_1036_910_0 , _rtB -> B_1036_1909_0_d , & _rtB -> ONDelay_d , & _rtDW ->
ONDelay_d , & _rtZCE -> ONDelay_d ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_913_0_k = _rtB -> B_1036_1912_0_h ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_913_0_k , _rtB -> B_1036_909_0 , _rtB ->
B_1036_910_0 , _rtB -> B_1036_1909_0_d , & _rtB -> OFFDelay_ht , & _rtDW ->
OFFDelay_ht , & _rtZCE -> OFFDelay_ht ) ; _rtB -> B_1036_915_0 = ( _rtB ->
ONDelay_d . B_90_17_0 || _rtB -> OFFDelay_ht . B_86_19_0 ) ; _rtB ->
B_1036_916_0 = ( _rtB -> B_1036_848_0_o && _rtB -> B_1036_915_0 ) ; _rtB ->
B_1036_917_0 = ( _rtB -> B_1036_394_0_o || _rtB -> B_1036_916_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_918_0 =
_rtDW -> u2_PreviousInput ; } _rtB -> B_1036_919_0 = ! _rtB -> B_1036_844_0 ;
_rtB -> B_1036_920_0 = ( _rtB -> B_1036_918_0 && _rtB -> B_1036_919_0 ) ;
_rtB -> B_1036_921_0_p = _rtB -> B_1036_920_0 ; _rtB -> B_1036_922_0 = ssGetT
( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_923_0_e = _rtB -> B_1036_1918_0_f ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_923_0_e , _rtB -> B_1036_921_0_p , _rtB -> B_1036_922_0 , _rtB ->
B_1036_1916_0_c , & _rtB -> ONDelay_n0 , & _rtDW -> ONDelay_n0 , & _rtZCE ->
ONDelay_n0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_925_0_k = _rtB -> B_1036_1919_0_k ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_925_0_k , _rtB -> B_1036_921_0_p , _rtB -> B_1036_922_0 , _rtB ->
B_1036_1916_0_c , & _rtB -> OFFDelay_hp , & _rtDW -> OFFDelay_hp , & _rtZCE
-> OFFDelay_hp ) ; _rtB -> B_1036_927_0_b = ( _rtB -> ONDelay_n0 . B_90_17_0
|| _rtB -> OFFDelay_hp . B_86_19_0 ) ; _rtB -> B_1036_928_0_l = ! _rtB ->
B_1036_927_0_b ; _rtB -> B_1036_929_0 = ( _rtB -> B_1036_917_0 && _rtB ->
B_1036_928_0_l ) ; _rtB -> B_1036_930_0 = ( _rtB -> B_1036_777_0 > _rtB ->
B_1036_1179_0_n ) ; _rtB -> B_1036_931_0 = ! ( _rtB -> B_1036_448_0 != 0.0 )
; _rtB -> B_1036_932_0 = ( _rtB -> B_1036_931_0 && ( _rtB -> B_1036_1568_0 !=
0.0 ) ) ; _rtB -> B_1036_933_0 = ( _rtB -> B_1036_1835_0_l || _rtB ->
B_1036_932_0 ) ; _rtB -> B_1036_934_0 = ! ( _rtB -> B_1036_758_0 != 0.0 ) ;
_rtB -> B_1036_935_0_j = ( _rtB -> B_1036_934_0 && ( _rtB -> B_1036_1737_0_j
!= 0.0 ) ) ; _rtB -> B_1036_936_0_j = ( _rtB -> B_1036_933_0 || _rtB ->
B_1036_935_0_j ) ; _rtB -> B_1036_937_0_k = ( _rtB -> B_1036_930_0 && _rtB ->
B_1036_936_0_j ) ; _rtB -> B_1036_938_0_p = _rtB -> B_1036_937_0_k ; _rtB ->
B_1036_939_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_940_0_p = _rtB -> B_1036_1890_0_l ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_940_0_p , _rtB -> B_1036_938_0_p , _rtB
-> B_1036_939_0 , _rtB -> B_1036_1888_0_d , & _rtB -> ONDelay_os , & _rtDW ->
ONDelay_os , & _rtZCE -> ONDelay_os ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_942_0 = _rtB -> B_1036_1891_0_o ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_942_0 , _rtB -> B_1036_938_0_p , _rtB ->
B_1036_939_0 , _rtB -> B_1036_1888_0_d , & _rtB -> OFFDelay_c2 , & _rtDW ->
OFFDelay_c2 , & _rtZCE -> OFFDelay_c2 ) ; _rtB -> B_1036_944_0 = ( _rtB ->
ONDelay_os . B_90_17_0 || _rtB -> OFFDelay_c2 . B_86_19_0 ) ; _rtB ->
B_1036_945_0 = _rtB -> B_1036_402_0 * _rtB -> B_1036_1633_0_f ; if ( _rtB ->
B_1036_1190_0 > _rtP -> P_489 ) { _rtB -> B_679_0_0 = _rtB -> B_1036_411_0 *
_rtB -> B_1036_1633_0_f ; _rtB -> B_679_1_0 = _rtB -> B_1036_945_0 - _rtB ->
B_679_0_0 ; _rtB -> B_1036_947_0 = _rtB -> B_679_1_0 ; } else { _rtB ->
B_1036_947_0 = _rtB -> B_1036_945_0 ; } _rtB -> B_1036_948_0 = _rtB ->
B_1036_947_0 * _rtB -> B_1036_1635_0_g ; _rtB -> B_1036_949_0 = _rtB ->
B_1036_422_0 * _rtB -> B_1036_1633_0_f ; if ( _rtB -> B_1036_1190_0 > _rtP ->
P_490 ) { _rtB -> B_680_0_0 = _rtB -> B_1036_431_0 * _rtB -> B_1036_1633_0_f
; _rtB -> B_680_1_0 = _rtB -> B_1036_949_0 - _rtB -> B_680_0_0 ; _rtB ->
B_1036_951_0 = _rtB -> B_680_1_0 ; } else { _rtB -> B_1036_951_0 = _rtB ->
B_1036_949_0 ; } _rtB -> B_1036_952_0 = _rtB -> B_1036_951_0 * _rtB ->
B_1036_1634_0_j ; _rtB -> B_1036_953_0 = _rtB -> B_1036_948_0 - _rtB ->
B_1036_952_0 ; _rtB -> B_1036_954_0 = _rtB -> B_1036_398_0 * _rtB ->
B_1036_398_0 ; _rtB -> B_1036_955_0 = _rtB -> B_1036_418_0 * _rtB ->
B_1036_418_0 ; _rtB -> B_1036_956_0 = _rtB -> B_1036_954_0 + _rtB ->
B_1036_955_0 ; _rtB -> B_1036_957_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_956_0 ) ; _rtB -> B_1036_958_0 = ( _rtB -> B_1036_957_0 > _rtB ->
B_1036_465_0_n ) ; _rtB -> B_1036_959_0 = ( _rtB -> B_1036_958_0 && _rtB ->
B_1036_295_0_o ) ; _rtB -> B_1036_960_0 = ( _rtB -> B_1036_494_0 && _rtB ->
B_1036_315_0_l ) ; if ( _rtB -> B_1036_438_0 ) { _rtB -> B_1036_962_0 = _rtB
-> B_1036_502_0 ; } else { if ( _rtB -> B_1036_446_0_p ) { _rtB -> B_257_0_0
= ( _rtB -> B_1036_959_0 || _rtB -> B_1036_960_0 ) ; _rtB -> B_257_1_0 = (
_rtB -> B_257_0_0 && _rtB -> B_1036_446_0_p ) ; _rtB -> B_258_2_0 = _rtB ->
B_257_1_0 ; } else { if ( _rtB -> B_1036_295_0_o ) { _rtB -> B_256_0_0 = _rtB
-> B_1036_959_0 ; } else { _rtB -> B_256_0_0 = _rtB -> B_1036_960_0 ; } _rtB
-> B_258_2_0 = _rtB -> B_256_0_0 ; } _rtB -> B_1036_962_0 = _rtB -> B_258_2_0
; } if ( _rtB -> B_1036_962_0 > _rtP -> P_491 ) { _rtB -> B_1036_963_0 = _rtB
-> B_1036_953_0 ; } else { _rtB -> B_1036_963_0 = _rtB -> B_1036_1638_0_p ; }
if ( _rtB -> B_1036_1568_0 > _rtP -> P_492 ) { _rtB -> B_1036_964_0 = _rtB ->
B_1036_963_0 ; } else { _rtB -> B_1036_964_0 = _rtB -> B_1036_1638_0_p ; }
_rtB -> B_1036_965_0 = _rtB -> B_1036_1815_0 + _rtB -> B_1036_964_0 ; _rtB ->
B_1036_966_0 = _rtB -> B_1036_711_0 * _rtB -> B_1036_1752_0_c ; if ( _rtB ->
B_1036_1180_0 > _rtP -> P_493 ) { _rtB -> B_692_0_0 = _rtB -> B_1036_719_0 *
_rtB -> B_1036_1752_0_c ; _rtB -> B_692_1_0 = _rtB -> B_1036_966_0 - _rtB ->
B_692_0_0 ; _rtB -> B_1036_968_0 = _rtB -> B_692_1_0 ; } else { _rtB ->
B_1036_968_0 = _rtB -> B_1036_966_0 ; } _rtB -> B_1036_969_0 = _rtB ->
B_1036_968_0 * _rtB -> B_1036_1754_0_o ; _rtB -> B_1036_970_0 = _rtB ->
B_1036_733_0 * _rtB -> B_1036_1752_0_c ; if ( _rtB -> B_1036_1180_0 > _rtP ->
P_494 ) { _rtB -> B_693_0_0 = _rtB -> B_1036_741_0 * _rtB -> B_1036_1752_0_c
; _rtB -> B_693_1_0 = _rtB -> B_1036_970_0 - _rtB -> B_693_0_0 ; _rtB ->
B_1036_972_0 = _rtB -> B_693_1_0 ; } else { _rtB -> B_1036_972_0 = _rtB ->
B_1036_970_0 ; } _rtB -> B_1036_973_0 = _rtB -> B_1036_972_0 * _rtB ->
B_1036_1753_0_m ; _rtB -> B_1036_974_0 = _rtB -> B_1036_969_0 - _rtB ->
B_1036_973_0 ; _rtB -> B_1036_975_0 = _rtB -> B_1036_708_0 * _rtB ->
B_1036_708_0 ; _rtB -> B_1036_976_0 = _rtB -> B_1036_730_0 * _rtB ->
B_1036_730_0 ; _rtB -> B_1036_977_0 = _rtB -> B_1036_975_0 + _rtB ->
B_1036_976_0 ; _rtB -> B_1036_978_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_977_0 ) ; _rtB -> B_1036_979_0 = ( _rtB -> B_1036_978_0 > _rtB ->
B_1036_549_0_i ) ; _rtB -> B_1036_980_0 = ( _rtB -> B_1036_979_0 && _rtB ->
B_1036_682_0_f ) ; _rtB -> B_1036_981_0 = ( _rtB -> B_1036_578_0_g && _rtB ->
B_1036_702_0_m ) ; if ( _rtB -> B_1036_748_0_e ) { _rtB -> B_1036_983_0 =
_rtB -> B_1036_586_0_g ; } else { if ( _rtB -> B_1036_756_0_h ) { _rtB ->
B_302_0_0 = ( _rtB -> B_1036_980_0 || _rtB -> B_1036_981_0 ) ; _rtB ->
B_302_1_0 = ( _rtB -> B_302_0_0 && _rtB -> B_1036_756_0_h ) ; _rtB ->
B_303_2_0 = _rtB -> B_302_1_0 ; } else { if ( _rtB -> B_1036_682_0_f ) { _rtB
-> B_301_0_0 = _rtB -> B_1036_980_0 ; } else { _rtB -> B_301_0_0 = _rtB ->
B_1036_981_0 ; } _rtB -> B_303_2_0 = _rtB -> B_301_0_0 ; } _rtB ->
B_1036_983_0 = _rtB -> B_303_2_0 ; } if ( _rtB -> B_1036_983_0 > _rtP ->
P_495 ) { _rtB -> B_1036_984_0 = _rtB -> B_1036_974_0 ; } else { _rtB ->
B_1036_984_0 = _rtB -> B_1036_1757_0_i ; } if ( _rtB -> B_1036_1737_0_j >
_rtP -> P_496 ) { _rtB -> B_1036_985_0 = _rtB -> B_1036_984_0 ; } else { _rtB
-> B_1036_985_0 = _rtB -> B_1036_1757_0_i ; } _rtB -> B_1036_986_0 = _rtB ->
B_1036_965_0 + _rtB -> B_1036_985_0 ; _rtB -> B_1036_987_0 = _rtB ->
B_1036_986_0 * _rtB -> B_1036_986_0 ; _rtB -> B_1036_988_0 = _rtB ->
B_1036_951_0 * _rtB -> B_1036_1635_0_g ; _rtB -> B_1036_989_0 = _rtB ->
B_1036_947_0 * _rtB -> B_1036_1634_0_j ; _rtB -> B_1036_990_0 = _rtB ->
B_1036_988_0 + _rtB -> B_1036_989_0 ; if ( _rtB -> B_1036_962_0 > _rtP ->
P_497 ) { _rtB -> B_1036_991_0 = _rtB -> B_1036_990_0 ; } else { _rtB ->
B_1036_991_0 = _rtB -> B_1036_1638_0_p ; } if ( _rtB -> B_1036_1568_0 > _rtP
-> P_498 ) { _rtB -> B_1036_992_0 = _rtB -> B_1036_991_0 ; } else { _rtB ->
B_1036_992_0 = _rtB -> B_1036_1638_0_p ; } _rtB -> B_1036_993_0 = _rtB ->
B_1036_1814_0_g + _rtB -> B_1036_992_0 ; _rtB -> B_1036_994_0 = _rtB ->
B_1036_972_0 * _rtB -> B_1036_1754_0_o ; _rtB -> B_1036_995_0 = _rtB ->
B_1036_968_0 * _rtB -> B_1036_1753_0_m ; _rtB -> B_1036_996_0 = _rtB ->
B_1036_994_0 + _rtB -> B_1036_995_0 ; if ( _rtB -> B_1036_983_0 > _rtP ->
P_499 ) { _rtB -> B_1036_997_0 = _rtB -> B_1036_996_0 ; } else { _rtB ->
B_1036_997_0 = _rtB -> B_1036_1757_0_i ; } if ( _rtB -> B_1036_1737_0_j >
_rtP -> P_500 ) { _rtB -> B_1036_998_0 = _rtB -> B_1036_997_0 ; } else { _rtB
-> B_1036_998_0 = _rtB -> B_1036_1757_0_i ; } _rtB -> B_1036_999_0 = _rtB ->
B_1036_993_0 + _rtB -> B_1036_998_0 ; _rtB -> B_1036_1000_0 = _rtB ->
B_1036_999_0 * _rtB -> B_1036_999_0 ; _rtB -> B_1036_1001_0 = _rtB ->
B_1036_987_0 + _rtB -> B_1036_1000_0 ; _rtB -> B_1036_1002_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1001_0 ) ; _rtB -> B_1036_1003_0 = ( _rtB
-> B_1036_1002_0 > _rtB -> B_1036_1179_0_n ) ; _rtB -> B_1036_1004_0 = ! (
_rtB -> B_1036_962_0 != 0.0 ) ; _rtB -> B_1036_1005_0 = ( _rtB ->
B_1036_1004_0 && ( _rtB -> B_1036_1568_0 != 0.0 ) ) ; _rtB -> B_1036_1006_0 =
( _rtB -> B_1036_1836_0_f || _rtB -> B_1036_1005_0 ) ; _rtB -> B_1036_1007_0
= ! ( _rtB -> B_1036_983_0 != 0.0 ) ; _rtB -> B_1036_1008_0 = ( _rtB ->
B_1036_1007_0 && ( _rtB -> B_1036_1737_0_j != 0.0 ) ) ; _rtB ->
B_1036_1009_0_e = ( _rtB -> B_1036_1006_0 || _rtB -> B_1036_1008_0 ) ; _rtB
-> B_1036_1010_0_h = ( _rtB -> B_1036_1003_0 && _rtB -> B_1036_1009_0_e ) ;
_rtB -> B_1036_1011_0_i = _rtB -> B_1036_1010_0_h ; _rtB -> B_1036_1012_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_1013_0_j = _rtB -> B_1036_1897_0_a ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_1013_0_j , _rtB -> B_1036_1011_0_i , _rtB -> B_1036_1012_0 , _rtB ->
B_1036_1895_0_f , & _rtB -> ONDelay_h2 , & _rtDW -> ONDelay_h2 , & _rtZCE ->
ONDelay_h2 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_1015_0_a = _rtB -> B_1036_1898_0_j ; } DZG_v3_OFFDelay ( S , _rtB
-> B_1036_1015_0_a , _rtB -> B_1036_1011_0_i , _rtB -> B_1036_1012_0 , _rtB
-> B_1036_1895_0_f , & _rtB -> OFFDelay_l4 , & _rtDW -> OFFDelay_l4 , &
_rtZCE -> OFFDelay_l4 ) ; _rtB -> B_1036_1017_0_j = ( _rtB -> ONDelay_h2 .
B_90_17_0 || _rtB -> OFFDelay_l4 . B_86_19_0 ) ; _rtB -> B_1036_1018_0 = _rtB
-> B_1036_408_0 * _rtB -> B_1036_1682_0_d ; if ( _rtB -> B_1036_1190_0 > _rtP
-> P_501 ) { _rtB -> B_682_0_0 = _rtB -> B_1036_411_0 * _rtB ->
B_1036_1682_0_d ; _rtB -> B_682_1_0 = _rtB -> B_1036_1018_0 - _rtB ->
B_682_0_0 ; _rtB -> B_1036_1020_0 = _rtB -> B_682_1_0 ; } else { _rtB ->
B_1036_1020_0 = _rtB -> B_1036_1018_0 ; } _rtB -> B_1036_1021_0 = _rtB ->
B_1036_1020_0 * _rtB -> B_1036_1684_0 ; _rtB -> B_1036_1022_0 = _rtB ->
B_1036_428_0 * _rtB -> B_1036_1682_0_d ; if ( _rtB -> B_1036_1190_0 > _rtP ->
P_502 ) { _rtB -> B_683_0_0 = _rtB -> B_1036_431_0 * _rtB -> B_1036_1682_0_d
; _rtB -> B_683_1_0 = _rtB -> B_1036_1022_0 - _rtB -> B_683_0_0 ; _rtB ->
B_1036_1024_0 = _rtB -> B_683_1_0 ; } else { _rtB -> B_1036_1024_0 = _rtB ->
B_1036_1022_0 ; } _rtB -> B_1036_1025_0 = _rtB -> B_1036_1024_0 * _rtB ->
B_1036_1683_0_l ; _rtB -> B_1036_1026_0 = _rtB -> B_1036_1021_0 - _rtB ->
B_1036_1025_0 ; _rtB -> B_1036_1027_0 = _rtB -> B_1036_404_0 * _rtB ->
B_1036_404_0 ; _rtB -> B_1036_1028_0 = _rtB -> B_1036_424_0 * _rtB ->
B_1036_424_0 ; _rtB -> B_1036_1029_0 = _rtB -> B_1036_1027_0 + _rtB ->
B_1036_1028_0 ; _rtB -> B_1036_1030_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1029_0 ) ; _rtB -> B_1036_1031_0_g = ( _rtB -> B_1036_1030_0 > _rtB ->
B_1036_465_0_n ) ; _rtB -> B_1036_1032_0_n = ( _rtB -> B_1036_1031_0_g &&
_rtB -> B_1036_295_0_o ) ; _rtB -> B_1036_1033_0_l = ( _rtB -> B_1036_499_0_g
&& _rtB -> B_1036_315_0_l ) ; if ( _rtB -> B_1036_438_0 ) { _rtB ->
B_1036_1035_0 = _rtB -> B_1036_502_0 ; } else { if ( _rtB -> B_1036_446_0_p )
{ _rtB -> B_254_0_0 = ( _rtB -> B_1036_1032_0_n || _rtB -> B_1036_1033_0_l )
; _rtB -> B_254_1_0 = ( _rtB -> B_254_0_0 && _rtB -> B_1036_446_0_p ) ; _rtB
-> B_255_2_0 = _rtB -> B_254_1_0 ; } else { if ( _rtB -> B_1036_295_0_o ) {
_rtB -> B_253_0_0 = _rtB -> B_1036_1032_0_n ; } else { _rtB -> B_253_0_0 =
_rtB -> B_1036_1033_0_l ; } _rtB -> B_255_2_0 = _rtB -> B_253_0_0 ; } _rtB ->
B_1036_1035_0 = _rtB -> B_255_2_0 ; } if ( _rtB -> B_1036_1568_0 > _rtP ->
P_503 ) { if ( _rtB -> B_1036_1035_0 > _rtP -> P_26 ) { _rtB -> B_685_0_0 =
_rtB -> B_1036_1026_0 ; } else { _rtB -> B_685_0_0 = _rtB -> B_1036_1687_0_l
; } _rtB -> B_1036_1037_0 = _rtB -> B_685_0_0 ; } else { _rtB ->
B_1036_1037_0 = _rtB -> B_1036_1687_0_l ; } _rtB -> B_1036_1038_0 = _rtB ->
B_1036_1828_0_f + _rtB -> B_1036_1037_0 ; _rtB -> B_1036_1039_0 = _rtB ->
B_1036_716_0 * _rtB -> B_1036_1761_0_n ; if ( _rtB -> B_1036_1180_0 > _rtP ->
P_504 ) { _rtB -> B_695_0_0 = _rtB -> B_1036_719_0 * _rtB -> B_1036_1761_0_n
; _rtB -> B_695_1_0 = _rtB -> B_1036_1039_0 - _rtB -> B_695_0_0 ; _rtB ->
B_1036_1041_0 = _rtB -> B_695_1_0 ; } else { _rtB -> B_1036_1041_0 = _rtB ->
B_1036_1039_0 ; } _rtB -> B_1036_1042_0 = _rtB -> B_1036_1041_0 * _rtB ->
B_1036_1763_0_j ; _rtB -> B_1036_1043_0 = _rtB -> B_1036_738_0 * _rtB ->
B_1036_1761_0_n ; if ( _rtB -> B_1036_1180_0 > _rtP -> P_505 ) { _rtB ->
B_696_0_0 = _rtB -> B_1036_741_0 * _rtB -> B_1036_1761_0_n ; _rtB ->
B_696_1_0 = _rtB -> B_1036_1043_0 - _rtB -> B_696_0_0 ; _rtB -> B_1036_1045_0
= _rtB -> B_696_1_0 ; } else { _rtB -> B_1036_1045_0 = _rtB -> B_1036_1043_0
; } _rtB -> B_1036_1046_0 = _rtB -> B_1036_1045_0 * _rtB -> B_1036_1762_0_c ;
_rtB -> B_1036_1047_0 = _rtB -> B_1036_1042_0 - _rtB -> B_1036_1046_0 ; _rtB
-> B_1036_1048_0 = _rtB -> B_1036_713_0 * _rtB -> B_1036_713_0 ; _rtB ->
B_1036_1049_0 = _rtB -> B_1036_735_0 * _rtB -> B_1036_735_0 ; _rtB ->
B_1036_1050_0 = _rtB -> B_1036_1048_0 + _rtB -> B_1036_1049_0 ; _rtB ->
B_1036_1051_0 = muDoubleScalarSqrt ( _rtB -> B_1036_1050_0 ) ; _rtB ->
B_1036_1052_0 = ( _rtB -> B_1036_1051_0 > _rtB -> B_1036_549_0_i ) ; _rtB ->
B_1036_1053_0 = ( _rtB -> B_1036_1052_0 && _rtB -> B_1036_682_0_f ) ; _rtB ->
B_1036_1054_0_e = ( _rtB -> B_1036_583_0_i && _rtB -> B_1036_702_0_m ) ; if (
_rtB -> B_1036_748_0_e ) { _rtB -> B_1036_1056_0 = _rtB -> B_1036_586_0_g ; }
else { if ( _rtB -> B_1036_756_0_h ) { _rtB -> B_299_0_0 = ( _rtB ->
B_1036_1053_0 || _rtB -> B_1036_1054_0_e ) ; _rtB -> B_299_1_0 = ( _rtB ->
B_299_0_0 && _rtB -> B_1036_756_0_h ) ; _rtB -> B_300_2_0 = _rtB -> B_299_1_0
; } else { if ( _rtB -> B_1036_682_0_f ) { _rtB -> B_298_0_0 = _rtB ->
B_1036_1053_0 ; } else { _rtB -> B_298_0_0 = _rtB -> B_1036_1054_0_e ; } _rtB
-> B_300_2_0 = _rtB -> B_298_0_0 ; } _rtB -> B_1036_1056_0 = _rtB ->
B_300_2_0 ; } if ( _rtB -> B_1036_1737_0_j > _rtP -> P_506 ) { if ( _rtB ->
B_1036_1056_0 > _rtP -> P_30 ) { _rtB -> B_698_0_0 = _rtB -> B_1036_1047_0 ;
} else { _rtB -> B_698_0_0 = _rtB -> B_1036_1766_0_k ; } _rtB ->
B_1036_1058_0 = _rtB -> B_698_0_0 ; } else { _rtB -> B_1036_1058_0 = _rtB ->
B_1036_1766_0_k ; } _rtB -> B_1036_1059_0 = _rtB -> B_1036_1038_0 + _rtB ->
B_1036_1058_0 ; _rtB -> B_1036_1060_0 = _rtB -> B_1036_1059_0 * _rtB ->
B_1036_1059_0 ; _rtB -> B_1036_1061_0 = _rtB -> B_1036_1024_0 * _rtB ->
B_1036_1684_0 ; _rtB -> B_1036_1062_0 = _rtB -> B_1036_1020_0 * _rtB ->
B_1036_1683_0_l ; _rtB -> B_1036_1063_0 = _rtB -> B_1036_1061_0 + _rtB ->
B_1036_1062_0 ; if ( _rtB -> B_1036_1568_0 > _rtP -> P_507 ) { if ( _rtB ->
B_1036_1035_0 > _rtP -> P_27 ) { _rtB -> B_686_0_0 = _rtB -> B_1036_1063_0 ;
} else { _rtB -> B_686_0_0 = _rtB -> B_1036_1687_0_l ; } _rtB ->
B_1036_1065_0 = _rtB -> B_686_0_0 ; } else { _rtB -> B_1036_1065_0 = _rtB ->
B_1036_1687_0_l ; } _rtB -> B_1036_1066_0 = _rtB -> B_1036_1827_0_d + _rtB ->
B_1036_1065_0 ; _rtB -> B_1036_1067_0 = _rtB -> B_1036_1045_0 * _rtB ->
B_1036_1763_0_j ; _rtB -> B_1036_1068_0 = _rtB -> B_1036_1041_0 * _rtB ->
B_1036_1762_0_c ; _rtB -> B_1036_1069_0 = _rtB -> B_1036_1067_0 + _rtB ->
B_1036_1068_0 ; if ( _rtB -> B_1036_1737_0_j > _rtP -> P_508 ) { if ( _rtB ->
B_1036_1056_0 > _rtP -> P_31 ) { _rtB -> B_699_0_0 = _rtB -> B_1036_1069_0 ;
} else { _rtB -> B_699_0_0 = _rtB -> B_1036_1766_0_k ; } _rtB ->
B_1036_1071_0 = _rtB -> B_699_0_0 ; } else { _rtB -> B_1036_1071_0 = _rtB ->
B_1036_1766_0_k ; } _rtB -> B_1036_1072_0 = _rtB -> B_1036_1066_0 + _rtB ->
B_1036_1071_0 ; _rtB -> B_1036_1073_0 = _rtB -> B_1036_1072_0 * _rtB ->
B_1036_1072_0 ; _rtB -> B_1036_1074_0 = _rtB -> B_1036_1060_0 + _rtB ->
B_1036_1073_0 ; _rtB -> B_1036_1075_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1074_0 ) ; _rtB -> B_1036_1076_0 = ( _rtB -> B_1036_1075_0 > _rtB ->
B_1036_1179_0_n ) ; _rtB -> B_1036_1077_0 = ! ( _rtB -> B_1036_1035_0 != 0.0
) ; _rtB -> B_1036_1078_0 = ( _rtB -> B_1036_1077_0 && ( _rtB ->
B_1036_1568_0 != 0.0 ) ) ; _rtB -> B_1036_1079_0_c = ( _rtB ->
B_1036_1837_0_i || _rtB -> B_1036_1078_0 ) ; _rtB -> B_1036_1080_0_h = ! (
_rtB -> B_1036_1056_0 != 0.0 ) ; _rtB -> B_1036_1081_0_o = ( _rtB ->
B_1036_1080_0_h && ( _rtB -> B_1036_1737_0_j != 0.0 ) ) ; _rtB ->
B_1036_1082_0_d = ( _rtB -> B_1036_1079_0_c || _rtB -> B_1036_1081_0_o ) ;
_rtB -> B_1036_1083_0 = ( _rtB -> B_1036_1076_0 && _rtB -> B_1036_1082_0_d )
; _rtB -> B_1036_1084_0 = _rtB -> B_1036_1083_0 ; _rtB -> B_1036_1085_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_1086_0 = _rtB -> B_1036_1904_0_b ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_1086_0 , _rtB -> B_1036_1084_0 , _rtB -> B_1036_1085_0 , _rtB ->
B_1036_1902_0_n , & _rtB -> ONDelay_k1 , & _rtDW -> ONDelay_k1 , & _rtZCE ->
ONDelay_k1 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_1088_0 = _rtB -> B_1036_1905_0_l ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_1088_0 , _rtB -> B_1036_1084_0 , _rtB -> B_1036_1085_0 , _rtB ->
B_1036_1902_0_n , & _rtB -> OFFDelay_gk , & _rtDW -> OFFDelay_gk , & _rtZCE
-> OFFDelay_gk ) ; _rtB -> B_1036_1090_0 = ( _rtB -> ONDelay_k1 . B_90_17_0
|| _rtB -> OFFDelay_gk . B_86_19_0 ) ; _rtB -> B_1036_1091_0 = ( _rtB ->
B_1036_944_0 || _rtB -> B_1036_1017_0_j || _rtB -> B_1036_1090_0 ) ; _rtB ->
B_1036_1092_0 = ( _rtB -> B_1036_1885_0_d || _rtB -> B_1036_1091_0 ) ; _rtB
-> B_1036_1093_0_d = ( _rtB -> B_1036_1092_0 && _rtB -> B_1036_1886_0_n ) ;
_rtB -> B_1036_1094_0_j = ( _rtB -> B_1036_1184_0_m || _rtB ->
B_1036_1093_0_d ) ; _rtB -> B_1036_1095_0_f = ! _rtB -> B_1036_1094_0_j ;
_rtB -> B_1036_1096_0_k = ( _rtB -> B_1036_929_0 && _rtB -> B_1036_1095_0_f )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_1097_0 = _rtDW -> Memory_PreviousInput_j3e ; _rtB -> B_1036_1098_0 = !
_rtB -> B_1036_1097_0 ; } _rtB -> B_1036_1099_0 = ( _rtB -> B_1036_1096_0_k
&& _rtB -> B_1036_1098_0 ) ; _rtB -> B_1036_1100_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1101_0 =
_rtDW -> Memory_PreviousInput_od ; _rtB -> B_1036_1102_0 = ! _rtB ->
B_1036_1101_0 ; } _rtB -> B_1036_1103_0 = _rtB -> B_1036_1096_0_k ; _rtB ->
B_1036_1104_0 = _rtB -> B_1036_1103_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_1105_0 = _rtDW -> Memory_PreviousInput_a3
; _rtB -> B_1036_1106_0 = _rtB -> B_1036_1955_0_d [ 0 ] ; }
DZG_v3_POSITIVEEdge ( S , _rtB -> B_1036_1106_0 , _rtB -> B_1036_1104_0 ,
_rtB -> B_1036_1105_0 , & _rtB -> POSITIVEEdge_cc , & _rtDW ->
POSITIVEEdge_cc ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_1108_0 = _rtB -> B_1036_1955_0_d [ 1 ] ; } DZG_v3_NEGATIVEEdge
( S , _rtB -> B_1036_1108_0 , _rtB -> B_1036_1104_0 , _rtB -> B_1036_1105_0 ,
& _rtB -> NEGATIVEEdge_b , & _rtDW -> NEGATIVEEdge_b ) ; _rtB ->
B_1036_1110_0 = ( _rtB -> POSITIVEEdge_cc . B_85_0_0 || _rtB ->
NEGATIVEEdge_b . B_84_0_0 ) ; _rtB -> B_1036_1111_0 = ( _rtB -> B_1036_1102_0
&& _rtB -> B_1036_1110_0 ) ; _rtB -> B_1036_1112_0 = _rtB -> B_1036_1111_0 ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_1113_0 = _rtDW -> ICic_PreviousInput ; } if ( _rtB -> B_1036_1112_0 )
{ _rtB -> B_1036_1114_0 = _rtB -> B_1036_1100_0 ; } else { _rtB ->
B_1036_1114_0 = _rtB -> B_1036_1113_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_1115_0 = _rtP -> P_510 ; } _rtB ->
B_1036_1116_0 = _rtB -> B_1036_1114_0 + _rtB -> B_1036_1115_0 ; _rtB ->
B_1036_1117_0 = ( _rtB -> B_1036_1116_0 > _rtB -> B_1036_1100_0 ) ; _rtB ->
B_1036_1118_0 = ( _rtB -> B_1036_1099_0 || _rtB -> B_1036_1117_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1119_0_n =
_rtDW -> u4_PreviousInput_d ; } _rtB -> B_1036_1120_0 = _rtB -> B_1036_964_0
* _rtB -> B_1036_964_0 ; _rtB -> B_1036_1121_0 = _rtB -> B_1036_992_0 * _rtB
-> B_1036_992_0 ; _rtB -> B_1036_1122_0 = _rtB -> B_1036_1120_0 + _rtB ->
B_1036_1121_0 ; _rtB -> B_1036_1123_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1122_0 ) ; _rtB -> B_1036_1124_0 = ( _rtB -> B_1036_1678_0_h > _rtB ->
B_1036_1123_0 ) ; if ( _rtB -> B_1036_1124_0 ) { _rtB -> B_1036_1125_0 = _rtB
-> B_1036_1673_0_j ; _rtB -> B_1036_1127_0 = _rtB -> B_1036_1675_0_j ; } else
{ _rtB -> B_1036_1125_0 = _rtB -> B_1036_964_0 ; _rtB -> B_1036_1127_0 = _rtB
-> B_1036_992_0 ; } _rtB -> B_1036_1126_0 = _rtB -> B_1036_1125_0 * _rtB ->
B_1036_1125_0 ; _rtB -> B_1036_1128_0 = _rtB -> B_1036_1127_0 * _rtB ->
B_1036_1127_0 ; _rtB -> B_1036_1129_0 = _rtB -> B_1036_1126_0 + _rtB ->
B_1036_1128_0 ; _rtB -> B_1036_1130_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1129_0 ) ; _rtB -> B_1036_1131_0 = _rtB -> B_1036_985_0 * _rtB ->
B_1036_985_0 ; _rtB -> B_1036_1132_0 = _rtB -> B_1036_998_0 * _rtB ->
B_1036_998_0 ; _rtB -> B_1036_1133_0 = _rtB -> B_1036_1131_0 + _rtB ->
B_1036_1132_0 ; _rtB -> B_1036_1134_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1133_0 ) ; _rtB -> B_1036_1135_0_n = ( _rtB -> B_1036_1130_0 > _rtB ->
B_1036_1134_0 ) ; if ( _rtB -> B_1036_1135_0_n ) { _rtB -> B_1036_1136_0 =
_rtB -> B_1036_1125_0 ; _rtB -> B_1036_1137_0 = _rtB -> B_1036_1127_0 ; }
else { _rtB -> B_1036_1136_0 = _rtB -> B_1036_985_0 ; _rtB -> B_1036_1137_0 =
_rtB -> B_1036_998_0 ; } _rtB -> B_1036_1138_0 = _rtB -> B_1036_1136_0 - _rtB
-> B_1036_986_0 ; _rtB -> B_1036_1139_0 = _rtB -> B_1036_1137_0 - _rtB ->
B_1036_999_0 ; _rtB -> B_1036_1140_0 = _rtB -> B_1036_1136_0 * _rtB ->
B_1036_1136_0 ; _rtB -> B_1036_1141_0 = _rtB -> B_1036_1137_0 * _rtB ->
B_1036_1137_0 ; _rtB -> B_1036_1142_0 = _rtB -> B_1036_1140_0 + _rtB ->
B_1036_1141_0 ; _rtB -> B_1036_1143_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1142_0 ) ; _rtB -> B_1036_1144_0 = _rtB -> B_1036_1136_0 / _rtB ->
B_1036_1143_0 ; _rtB -> B_1036_1145_0 = _rtB -> B_1036_1138_0 * _rtB ->
B_1036_1138_0 ; _rtB -> B_1036_1146_0 = _rtB -> B_1036_1139_0 * _rtB ->
B_1036_1139_0 ; _rtB -> B_1036_1147_0 = _rtB -> B_1036_1145_0 + _rtB ->
B_1036_1146_0 ; _rtB -> B_1036_1148_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1147_0 ) ; _rtB -> B_1036_1149_0 = _rtB -> B_1036_1138_0 / _rtB ->
B_1036_1148_0 ; _rtB -> B_1036_1150_0 = _rtB -> B_1036_1144_0 - _rtB ->
B_1036_1149_0 ; _rtB -> B_1036_1151_0 = _rtB -> B_1036_1150_0 * _rtB ->
B_1036_1150_0 ; _rtB -> B_1036_1152_0 = _rtB -> B_1036_1137_0 / _rtB ->
B_1036_1143_0 ; _rtB -> B_1036_1153_0 = _rtB -> B_1036_1139_0 / _rtB ->
B_1036_1148_0 ; _rtB -> B_1036_1154_0 = _rtB -> B_1036_1152_0 - _rtB ->
B_1036_1153_0 ; _rtB -> B_1036_1155_0 = _rtB -> B_1036_1154_0 * _rtB ->
B_1036_1154_0 ; _rtB -> B_1036_1156_0 = _rtB -> B_1036_1151_0 + _rtB ->
B_1036_1155_0 ; _rtB -> B_1036_1157_0 = _rtB -> B_1036_1156_0 * _rtB ->
B_1036_1562_0 ; _rtB -> B_1036_1158_0 = _rtB -> B_1036_1561_0 - _rtB ->
B_1036_1157_0 ; u0 = _rtB -> B_1036_1158_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB -> B_1036_1159_0 =
muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_1160_0 = _rtB -> B_1036_1152_0 -
_rtB -> B_1036_1149_0 ; _rtB -> B_1036_1161_0 = _rtB -> B_1036_1160_0 * _rtB
-> B_1036_1160_0 ; _rtB -> B_1036_1162_0 = _rtB -> B_1036_1144_0 + _rtB ->
B_1036_1153_0 ; _rtB -> B_1036_1163_0 = _rtB -> B_1036_1162_0 * _rtB ->
B_1036_1162_0 ; _rtB -> B_1036_1164_0 = _rtB -> B_1036_1161_0 + _rtB ->
B_1036_1163_0 ; _rtB -> B_1036_1165_0_i = ( _rtB -> B_1036_1164_0 > _rtB ->
B_1036_1559_0_p ) ; if ( _rtB -> B_1036_1165_0_i ) { _rtB -> B_672_0_0 = _rtB
-> B_1036_1159_0 * _rtB -> B_1036_1560_0 ; _rtB -> B_1036_1167_0 = _rtB ->
B_672_0_0 ; } else { _rtB -> B_1036_1167_0 = _rtB -> B_1036_1159_0 ; } _rtB
-> B_1036_1168_0 = muDoubleScalarCos ( _rtB -> B_1036_1167_0 ) ; _rtB ->
B_1036_1169_0_e = ( _rtB -> B_1036_1168_0 >= _rtB -> B_1036_1554_0 ) ; if (
_rtB -> B_1036_1169_0_e ) { _rtB -> B_670_0_0 = _rtB -> B_1036_1139_0 * _rtB
-> B_1036_1139_0 ; _rtB -> B_670_1_0 = _rtB -> B_1036_1138_0 * _rtB ->
B_1036_1138_0 ; _rtB -> B_670_2_0 = _rtB -> B_670_1_0 + _rtB -> B_670_0_0 ;
_rtB -> B_670_3_0 = muDoubleScalarSqrt ( _rtB -> B_670_2_0 ) ; _rtB ->
B_670_4_0 = _rtB -> B_1036_1137_0 * _rtB -> B_1036_1137_0 ; _rtB -> B_670_5_0
= _rtB -> B_1036_1136_0 * _rtB -> B_1036_1136_0 ; _rtB -> B_670_6_0 = _rtB ->
B_670_5_0 + _rtB -> B_670_4_0 ; _rtB -> B_670_7_0 = muDoubleScalarSqrt ( _rtB
-> B_670_6_0 ) ; _rtB -> B_670_8_0 = _rtB -> B_670_7_0 * _rtB -> B_670_3_0 *
_rtB -> B_1036_1168_0 ; _rtB -> B_670_9_0 = muDoubleScalarSqrt ( _rtB ->
B_670_8_0 ) ; _rtB -> B_1036_1171_0 = _rtB -> B_670_9_0 ; } else { _rtB ->
B_1036_1171_0 = _rtB -> B_1036_1553_0 ; } if ( _rtB -> B_1036_1334_0_p ) {
_rtB -> B_1036_1173_0 = _rtB -> B_1036_1310_0 ; } else { _rtB -> B_632_0_0 =
( _rtB -> B_1036_1171_0 > _rtB -> B_1036_1325_0 ) ; if ( _rtB -> B_632_0_0 )
{ if ( _rtB -> B_1036_1322_0_g ) { _rtB -> B_630_0_0 = _rtB -> B_1036_1314_0
* _rtB -> B_1036_1171_0 ; _rtB -> B_630_1_0 = _rtB -> B_630_0_0 + _rtB ->
B_1036_1339_0 ; _rtB -> B_631_1_0 = _rtB -> B_630_1_0 ; } else { _rtB ->
B_631_1_0 = _rtB -> B_1036_1189_0 ; } _rtB -> B_632_3_0 = _rtB -> B_631_1_0 ;
} else { _rtB -> B_629_0_0 = ( _rtB -> B_1036_1171_0 <= _rtB -> B_1036_1325_0
) ; _rtB -> B_629_1_0 = ( _rtB -> B_1036_1171_0 > _rtB -> B_1036_1192_0_c ) ;
_rtB -> B_629_2_0 = ( _rtB -> B_629_1_0 && _rtB -> B_629_0_0 ) ; if ( _rtB ->
B_629_2_0 ) { _rtB -> B_628_0_0 = _rtB -> B_1036_1171_0 * _rtB ->
B_1036_1314_0 ; _rtB -> B_628_1_0 = _rtB -> B_628_0_0 + _rtB -> B_1036_1339_0
; _rtB -> B_629_4_0 = _rtB -> B_628_1_0 ; } else { _rtB -> B_629_4_0 = _rtB
-> B_1036_1310_0 ; } _rtB -> B_632_3_0 = _rtB -> B_629_4_0 ; } _rtB ->
B_1036_1173_0 = _rtB -> B_632_3_0 ; } if ( _rtB -> B_1036_1363_0_p ) { _rtB
-> B_1036_1175_0 = _rtB -> B_1036_1173_0 ; } else { _rtB -> B_637_0_0 = (
_rtB -> B_1036_1171_0 > _rtB -> B_1036_1354_0 ) ; if ( _rtB -> B_637_0_0 ) {
if ( _rtB -> B_1036_1351_0_i ) { _rtB -> B_635_0_0 = _rtB -> B_1036_1343_0 *
_rtB -> B_1036_1171_0 ; _rtB -> B_635_1_0 = _rtB -> B_635_0_0 + _rtB ->
B_1036_1368_0_b ; _rtB -> B_636_1_0 = _rtB -> B_635_1_0 ; } else { _rtB ->
B_636_1_0 = _rtB -> B_1036_1189_0 ; } _rtB -> B_637_3_0 = _rtB -> B_636_1_0 ;
} else { _rtB -> B_634_0_0 = ( _rtB -> B_1036_1171_0 <= _rtB -> B_1036_1354_0
) ; _rtB -> B_634_1_0 = ( _rtB -> B_1036_1171_0 > _rtB -> B_1036_1172_0 ) ;
_rtB -> B_634_2_0 = ( _rtB -> B_634_1_0 && _rtB -> B_634_0_0 ) ; if ( _rtB ->
B_634_2_0 ) { _rtB -> B_633_0_0 = _rtB -> B_1036_1171_0 * _rtB ->
B_1036_1343_0 ; _rtB -> B_633_1_0 = _rtB -> B_633_0_0 + _rtB ->
B_1036_1368_0_b ; _rtB -> B_634_4_0 = _rtB -> B_633_1_0 ; } else { _rtB ->
B_634_4_0 = _rtB -> B_1036_1173_0 ; } _rtB -> B_637_3_0 = _rtB -> B_634_4_0 ;
} _rtB -> B_1036_1175_0 = _rtB -> B_637_3_0 ; } if ( _rtB -> B_1036_1392_0_o
) { _rtB -> B_1036_1177_0 = _rtB -> B_1036_1175_0 ; } else { _rtB ->
B_642_0_0 = ( _rtB -> B_1036_1171_0 > _rtB -> B_1036_1383_0_g ) ; if ( _rtB
-> B_642_0_0 ) { if ( _rtB -> B_1036_1380_0_c ) { _rtB -> B_640_0_0 = _rtB ->
B_1036_1372_0_n * _rtB -> B_1036_1171_0 ; _rtB -> B_640_1_0 = _rtB ->
B_640_0_0 + _rtB -> B_1036_1397_0_n ; _rtB -> B_641_1_0 = _rtB -> B_640_1_0 ;
} else { _rtB -> B_641_1_0 = _rtB -> B_1036_1189_0 ; } _rtB -> B_642_3_0 =
_rtB -> B_641_1_0 ; } else { _rtB -> B_639_0_0 = ( _rtB -> B_1036_1171_0 <=
_rtB -> B_1036_1383_0_g ) ; _rtB -> B_639_1_0 = ( _rtB -> B_1036_1171_0 >
_rtB -> B_1036_1174_0 ) ; _rtB -> B_639_2_0 = ( _rtB -> B_639_1_0 && _rtB ->
B_639_0_0 ) ; if ( _rtB -> B_639_2_0 ) { _rtB -> B_638_0_0 = _rtB ->
B_1036_1171_0 * _rtB -> B_1036_1372_0_n ; _rtB -> B_638_1_0 = _rtB ->
B_638_0_0 + _rtB -> B_1036_1397_0_n ; _rtB -> B_639_4_0 = _rtB -> B_638_1_0 ;
} else { _rtB -> B_639_4_0 = _rtB -> B_1036_1175_0 ; } _rtB -> B_642_3_0 =
_rtB -> B_639_4_0 ; } _rtB -> B_1036_1177_0 = _rtB -> B_642_3_0 ; } if ( _rtB
-> B_1036_1421_0_c ) { _rtB -> B_1036_1179_0 = _rtB -> B_1036_1177_0 ; } else
{ _rtB -> B_647_0_0 = ( _rtB -> B_1036_1171_0 > _rtB -> B_1036_1412_0 ) ; if
( _rtB -> B_647_0_0 ) { if ( _rtB -> B_1036_1409_0_b ) { _rtB -> B_645_0_0 =
_rtB -> B_1036_1401_0 * _rtB -> B_1036_1171_0 ; _rtB -> B_645_1_0 = _rtB ->
B_645_0_0 + _rtB -> B_1036_1426_0 ; _rtB -> B_646_1_0 = _rtB -> B_645_1_0 ; }
else { _rtB -> B_646_1_0 = _rtB -> B_1036_1189_0 ; } _rtB -> B_647_3_0 = _rtB
-> B_646_1_0 ; } else { _rtB -> B_644_0_0 = ( _rtB -> B_1036_1171_0 <= _rtB
-> B_1036_1412_0 ) ; _rtB -> B_644_1_0 = ( _rtB -> B_1036_1171_0 > _rtB ->
B_1036_1176_0 ) ; _rtB -> B_644_2_0 = ( _rtB -> B_644_1_0 && _rtB ->
B_644_0_0 ) ; if ( _rtB -> B_644_2_0 ) { _rtB -> B_643_0_0 = _rtB ->
B_1036_1171_0 * _rtB -> B_1036_1401_0 ; _rtB -> B_643_1_0 = _rtB -> B_643_0_0
+ _rtB -> B_1036_1426_0 ; _rtB -> B_644_4_0 = _rtB -> B_643_1_0 ; } else {
_rtB -> B_644_4_0 = _rtB -> B_1036_1177_0 ; } _rtB -> B_647_3_0 = _rtB ->
B_644_4_0 ; } _rtB -> B_1036_1179_0 = _rtB -> B_647_3_0 ; } _rtB ->
B_1036_1180_0_k = ( _rtB -> B_1036_1002_0 > _rtB -> B_1036_1179_0 ) ; _rtB ->
B_1036_1181_0_k = ( _rtB -> B_1036_1179_0 == _rtB -> B_1036_1547_0_e ) ; _rtB
-> B_1036_1182_0_p = ! _rtB -> B_1036_1181_0_k ; _rtB -> B_1036_1183_0_k = (
_rtB -> B_1036_1180_0_k && _rtB -> B_1036_1182_0_p ) ; _rtB -> B_1036_1184_0
= ( _rtB -> B_1036_1002_0 > _rtB -> B_1036_1189_0 ) ; _rtB -> B_1036_1185_0_p
= ( _rtB -> B_1036_1184_0 && _rtB -> B_1036_1549_0 ) ; _rtB ->
B_1036_1186_0_p = ( _rtB -> B_1036_1183_0_k || _rtB -> B_1036_1185_0_p ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_1187_0_b = _rtDW -> Memory_PreviousInput_j1 ; _rtB -> B_1036_1188_0_c
= ! _rtB -> B_1036_1187_0_b ; } if ( _rtB -> B_1036_1181_0 > _rtP -> P_511 )
{ _rtB -> B_741_0_0 = ( _rtB -> B_1036_1186_0_p && _rtB -> B_1036_1188_0_c )
; _rtB -> B_1036_1190_0_d = _rtB -> B_741_0_0 ; } else { _rtB ->
B_1036_1190_0_d = _rtB -> B_1036_1186_0_p ; } _rtB -> B_1036_1191_0 = _rtB ->
B_1036_401_0 * _rtB -> B_1036_401_0 ; _rtB -> B_1036_1192_0 = _rtB ->
B_1036_421_0 * _rtB -> B_1036_421_0 ; _rtB -> B_1036_1193_0 = _rtB ->
B_1036_1191_0 + _rtB -> B_1036_1192_0 ; _rtB -> B_1036_1194_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1193_0 ) ; _rtB -> B_1036_1195_0_c = (
_rtB -> B_1036_1194_0 < 0.0 ) ; _rtB -> B_1036_1196_0_a = ! _rtB ->
B_1036_1195_0_c ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_1197_0 = _rtDW -> u_PreviousInput_n ; } _rtB -> B_1036_1198_0
= _rtB -> B_1036_401_0 - _rtB -> B_1036_1197_0 ; _rtB -> B_1036_1199_0 = _rtB
-> B_1036_1198_0 * _rtB -> B_1036_1198_0 ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_1200_0 = _rtDW -> u_PreviousInput_io ;
} _rtB -> B_1036_1201_0 = _rtB -> B_1036_421_0 - _rtB -> B_1036_1200_0 ; _rtB
-> B_1036_1202_0 = _rtB -> B_1036_1201_0 * _rtB -> B_1036_1201_0 ; _rtB ->
B_1036_1203_0 = _rtB -> B_1036_1199_0 + _rtB -> B_1036_1202_0 ; _rtB ->
B_1036_1204_0 = muDoubleScalarSqrt ( _rtB -> B_1036_1203_0 ) ; _rtB ->
B_1036_1205_0 = _rtB -> B_1036_401_0 * _rtB -> B_1036_401_0 ; _rtB ->
B_1036_1206_0 = _rtB -> B_1036_421_0 * _rtB -> B_1036_421_0 ; _rtB ->
B_1036_1207_0 = _rtB -> B_1036_1205_0 + _rtB -> B_1036_1206_0 ; _rtB ->
B_1036_1208_0 = muDoubleScalarSqrt ( _rtB -> B_1036_1207_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1209_0 =
_rtB -> B_1036_1197_0 * _rtB -> B_1036_1197_0 ; _rtB -> B_1036_1210_0 = _rtB
-> B_1036_1200_0 * _rtB -> B_1036_1200_0 ; _rtB -> B_1036_1211_0 = _rtB ->
B_1036_1209_0 + _rtB -> B_1036_1210_0 ; _rtB -> B_1036_1212_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1211_0 ) ; } maxV = muDoubleScalarMax (
_rtB -> B_1036_1208_0 , _rtB -> B_1036_1212_0 ) ; _rtB -> B_1036_1213_0 =
maxV ; _rtB -> B_1036_1214_0 = ( _rtB -> B_1036_1213_0 == _rtB ->
B_1036_397_0_f ) ; if ( _rtB -> B_1036_1214_0 ) { _rtB -> B_1036_1215_0 =
_rtB -> B_1036_398_0_n ; } else { _rtB -> B_1036_1215_0 = _rtB ->
B_1036_1213_0 ; } _rtB -> B_1036_1216_0 = _rtB -> B_1036_1204_0 / _rtB ->
B_1036_1215_0 ; _rtB -> B_1036_1217_0 = ( _rtB -> B_1036_1216_0 > _rtB ->
B_1036_368_0 ) ; _rtB -> B_1036_1218_0_i = ( _rtB -> B_1036_1196_0_a && _rtB
-> B_1036_1217_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_1036_1219_0 = _rtDW -> u_PreviousInput_n5 ; _rtB -> B_1036_1220_0
= _rtDW -> u_PreviousInput_pl ; _rtB -> B_1036_1221_0 = _rtDW ->
u_PreviousInput_d ; _rtB -> B_1036_1222_0 = ( _rtB -> B_1036_1219_0 + _rtB ->
B_1036_1220_0 ) + _rtB -> B_1036_1221_0 ; _rtB -> B_1036_1223_0 = _rtB ->
B_1036_1222_0 * _rtB -> B_1036_394_0 ; } _rtB -> B_1036_1224_0 = _rtB ->
B_1036_1204_0 - _rtB -> B_1036_1223_0 ; _rtB -> B_1036_1225_0 =
muDoubleScalarAbs ( _rtB -> B_1036_1224_0 ) ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1226_0_d = ( _rtB -> B_1036_1223_0
== _rtB -> B_1036_395_0_f ) ; if ( _rtB -> B_1036_1226_0_d ) { _rtB ->
B_1036_1227_0 = _rtB -> B_1036_396_0_c ; } else { _rtB -> B_1036_1227_0 =
_rtB -> B_1036_1223_0 ; } } _rtB -> B_1036_1228_0 = _rtB -> B_1036_1225_0 /
_rtB -> B_1036_1227_0 ; _rtB -> B_1036_1229_0_f = ( _rtB -> B_1036_1228_0 >
_rtB -> B_1036_367_0 ) ; _rtB -> B_1036_1230_0_n = ( _rtB -> B_1036_1204_0 <
_rtB -> B_1036_365_0 ) ; _rtB -> B_1036_1231_0 = ! _rtB -> B_1036_1230_0_n ;
_rtB -> B_1036_1232_0 = ( _rtB -> B_1036_1229_0_f && _rtB -> B_1036_1231_0 )
; _rtB -> B_1036_1233_0 = ( _rtB -> B_1036_1218_0_i && _rtB -> B_1036_1232_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_1234_0 = _rtDW -> u4_PreviousInput_a ; _rtB -> B_1036_1235_0 = _rtDW
-> u1_PreviousInput_f ; _rtB -> B_1036_1236_0 = ! _rtB -> B_1036_1235_0 ;
_rtB -> B_1036_1237_0_g = ( _rtB -> B_1036_1234_0 && _rtB -> B_1036_1236_0 )
; } _rtB -> B_1036_1238_0_o = ( _rtB -> B_1036_1233_0 || _rtB ->
B_1036_1237_0_g ) ; _rtB -> B_1036_1239_0 = _rtB -> B_1036_1238_0_o ; _rtB ->
B_1036_1240_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_1241_0 = _rtB -> B_1036_390_0_i ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_1241_0 , _rtB -> B_1036_1239_0 , _rtB ->
B_1036_1240_0 , _rtB -> B_1036_388_0 , & _rtB -> ONDelay_pu , & _rtDW ->
ONDelay_pu , & _rtZCE -> ONDelay_pu ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_1243_0 = _rtB -> B_1036_391_0_m ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_1243_0 , _rtB -> B_1036_1239_0 , _rtB ->
B_1036_1240_0 , _rtB -> B_1036_388_0 , & _rtB -> OFFDelay_b , & _rtDW ->
OFFDelay_b , & _rtZCE -> OFFDelay_b ) ; _rtB -> B_1036_1245_0 = ( _rtB ->
ONDelay_pu . B_90_17_0 || _rtB -> OFFDelay_b . B_86_19_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1246_0 =
_rtDW -> Memory_PreviousInput_og ; _rtB -> B_1036_1247_0_m = ! _rtB ->
B_1036_1246_0 ; } _rtB -> B_1036_1248_0_h = ( _rtB -> B_1036_1245_0 && _rtB
-> B_1036_1247_0_m ) ; _rtB -> B_1036_1249_0_e = ( _rtB -> B_1036_1248_0_h &&
_rtB -> B_1036_363_0_i ) ; _rtB -> B_1036_1250_0 = _rtB -> B_1036_711_0 *
_rtB -> B_1036_711_0 ; _rtB -> B_1036_1251_0 = _rtB -> B_1036_733_0 * _rtB ->
B_1036_733_0 ; _rtB -> B_1036_1252_0 = _rtB -> B_1036_1250_0 + _rtB ->
B_1036_1251_0 ; _rtB -> B_1036_1253_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1252_0 ) ; _rtB -> B_1036_1254_0_m = ( _rtB -> B_1036_1253_0 < 0.0 ) ;
_rtB -> B_1036_1255_0_i = ! _rtB -> B_1036_1254_0_m ; isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1256_0 = _rtDW ->
u_PreviousInput_c ; } _rtB -> B_1036_1257_0 = _rtB -> B_1036_711_0 - _rtB ->
B_1036_1256_0 ; _rtB -> B_1036_1258_0 = _rtB -> B_1036_1257_0 * _rtB ->
B_1036_1257_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_1259_0 = _rtDW -> u_PreviousInput_i4 ; } _rtB -> B_1036_1260_0
= _rtB -> B_1036_733_0 - _rtB -> B_1036_1259_0 ; _rtB -> B_1036_1261_0 = _rtB
-> B_1036_1260_0 * _rtB -> B_1036_1260_0 ; _rtB -> B_1036_1262_0 = _rtB ->
B_1036_1258_0 + _rtB -> B_1036_1261_0 ; _rtB -> B_1036_1263_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1262_0 ) ; _rtB -> B_1036_1264_0 = _rtB
-> B_1036_711_0 * _rtB -> B_1036_711_0 ; _rtB -> B_1036_1265_0 = _rtB ->
B_1036_733_0 * _rtB -> B_1036_733_0 ; _rtB -> B_1036_1266_0 = _rtB ->
B_1036_1264_0 + _rtB -> B_1036_1265_0 ; _rtB -> B_1036_1267_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1266_0 ) ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1268_0 = _rtB -> B_1036_1256_0 *
_rtB -> B_1036_1256_0 ; _rtB -> B_1036_1269_0 = _rtB -> B_1036_1259_0 * _rtB
-> B_1036_1259_0 ; _rtB -> B_1036_1270_0 = _rtB -> B_1036_1268_0 + _rtB ->
B_1036_1269_0 ; _rtB -> B_1036_1271_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1270_0 ) ; } maxV = muDoubleScalarMax ( _rtB -> B_1036_1267_0 , _rtB
-> B_1036_1271_0 ) ; _rtB -> B_1036_1272_0 = maxV ; _rtB -> B_1036_1273_0 = (
_rtB -> B_1036_1272_0 == _rtB -> B_1036_445_0 ) ; if ( _rtB -> B_1036_1273_0
) { _rtB -> B_1036_1274_0 = _rtB -> B_1036_446_0 ; } else { _rtB ->
B_1036_1274_0 = _rtB -> B_1036_1272_0 ; } _rtB -> B_1036_1275_0 = _rtB ->
B_1036_1263_0 / _rtB -> B_1036_1274_0 ; _rtB -> B_1036_1276_0_h = ( _rtB ->
B_1036_1275_0 > _rtB -> B_1036_416_0_a ) ; _rtB -> B_1036_1277_0_n = ( _rtB
-> B_1036_1255_0_i && _rtB -> B_1036_1276_0_h ) ; isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1278_0 = _rtDW ->
u_PreviousInput_j ; _rtB -> B_1036_1279_0 = _rtDW -> u_PreviousInput_h ; _rtB
-> B_1036_1280_0 = _rtDW -> u_PreviousInput_pg ; _rtB -> B_1036_1281_0 = (
_rtB -> B_1036_1278_0 + _rtB -> B_1036_1279_0 ) + _rtB -> B_1036_1280_0 ;
_rtB -> B_1036_1282_0 = _rtB -> B_1036_1281_0 * _rtB -> B_1036_442_0_g ; }
_rtB -> B_1036_1283_0 = _rtB -> B_1036_1263_0 - _rtB -> B_1036_1282_0 ; _rtB
-> B_1036_1284_0 = muDoubleScalarAbs ( _rtB -> B_1036_1283_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1285_0_e = (
_rtB -> B_1036_1282_0 == _rtB -> B_1036_443_0 ) ; if ( _rtB ->
B_1036_1285_0_e ) { _rtB -> B_1036_1286_0 = _rtB -> B_1036_444_0 ; } else {
_rtB -> B_1036_1286_0 = _rtB -> B_1036_1282_0 ; } } _rtB -> B_1036_1287_0 =
_rtB -> B_1036_1284_0 / _rtB -> B_1036_1286_0 ; _rtB -> B_1036_1288_0_e = (
_rtB -> B_1036_1287_0 > _rtB -> B_1036_415_0_b ) ; _rtB -> B_1036_1289_0 = (
_rtB -> B_1036_1263_0 < _rtB -> B_1036_413_0_e ) ; _rtB -> B_1036_1290_0 = !
_rtB -> B_1036_1289_0 ; _rtB -> B_1036_1291_0 = ( _rtB -> B_1036_1288_0_e &&
_rtB -> B_1036_1290_0 ) ; _rtB -> B_1036_1292_0 = ( _rtB -> B_1036_1277_0_n
&& _rtB -> B_1036_1291_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_1036_1293_0 = _rtDW -> u4_PreviousInput_c ; _rtB ->
B_1036_1294_0 = _rtDW -> u1_PreviousInput_i ; _rtB -> B_1036_1295_0_o = !
_rtB -> B_1036_1294_0 ; _rtB -> B_1036_1296_0_o = ( _rtB -> B_1036_1293_0 &&
_rtB -> B_1036_1295_0_o ) ; } _rtB -> B_1036_1297_0 = ( _rtB -> B_1036_1292_0
|| _rtB -> B_1036_1296_0_o ) ; _rtB -> B_1036_1298_0 = _rtB -> B_1036_1297_0
; _rtB -> B_1036_1299_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_1300_0_f = _rtB -> B_1036_438_0_j ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_1300_0_f , _rtB -> B_1036_1298_0 , _rtB
-> B_1036_1299_0 , _rtB -> B_1036_436_0 , & _rtB -> ONDelay_ja , & _rtDW ->
ONDelay_ja , & _rtZCE -> ONDelay_ja ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_1302_0 = _rtB -> B_1036_439_0_f ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_1302_0 , _rtB -> B_1036_1298_0 , _rtB ->
B_1036_1299_0 , _rtB -> B_1036_436_0 , & _rtB -> OFFDelay_m , & _rtDW ->
OFFDelay_m , & _rtZCE -> OFFDelay_m ) ; _rtB -> B_1036_1304_0 = ( _rtB ->
ONDelay_ja . B_90_17_0 || _rtB -> OFFDelay_m . B_86_19_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1305_0_m =
_rtDW -> Memory_PreviousInput_bd ; _rtB -> B_1036_1306_0_g = ! _rtB ->
B_1036_1305_0_m ; } _rtB -> B_1036_1307_0_c = ( _rtB -> B_1036_1304_0 && _rtB
-> B_1036_1306_0_g ) ; _rtB -> B_1036_1308_0_k = ( _rtB -> B_1036_1307_0_c &&
_rtB -> B_1036_411_0_a ) ; _rtB -> B_1036_1309_0_d = ( _rtB ->
B_1036_1249_0_e || _rtB -> B_1036_1308_0_k ) ; _rtB -> B_1036_1310_0_c = _rtB
-> B_1036_1309_0_d ; _rtB -> B_1036_1311_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1312_0_k =
_rtB -> B_1036_1925_0_f ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_1312_0_k ,
_rtB -> B_1036_1310_0_c , _rtB -> B_1036_1311_0 , _rtB -> B_1036_1923_0 , &
_rtB -> ONDelay_fna , & _rtDW -> ONDelay_fna , & _rtZCE -> ONDelay_fna ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_1314_0_k = _rtB -> B_1036_1926_0_i ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_1314_0_k , _rtB -> B_1036_1310_0_c , _rtB -> B_1036_1311_0 , _rtB ->
B_1036_1923_0 , & _rtB -> OFFDelay_kf , & _rtDW -> OFFDelay_kf , & _rtZCE ->
OFFDelay_kf ) ; _rtB -> B_1036_1316_0_f = ( _rtB -> ONDelay_fna . B_90_17_0
|| _rtB -> OFFDelay_kf . B_86_19_0 ) ; _rtB -> B_1036_1317_0_g = ( _rtB ->
B_1036_1190_0_d && _rtB -> B_1036_1316_0_f ) ; _rtB -> B_1036_1318_0_k = (
_rtB -> B_1036_1119_0_n || _rtB -> B_1036_1317_0_g ) ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1319_0 = _rtDW ->
u2_PreviousInput_l ; } _rtB -> B_1036_1320_0 = ! _rtB -> B_1036_1186_0_p ;
_rtB -> B_1036_1321_0 = ( _rtB -> B_1036_1319_0 && _rtB -> B_1036_1320_0 ) ;
_rtB -> B_1036_1322_0 = _rtB -> B_1036_1321_0 ; _rtB -> B_1036_1323_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_1324_0 = _rtB -> B_1036_1932_0_p ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_1324_0 , _rtB -> B_1036_1322_0 , _rtB -> B_1036_1323_0 , _rtB ->
B_1036_1930_0 , & _rtB -> ONDelay_mt , & _rtDW -> ONDelay_mt , & _rtZCE ->
ONDelay_mt ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_1326_0_n = _rtB -> B_1036_1933_0_d ; } DZG_v3_OFFDelay ( S , _rtB
-> B_1036_1326_0_n , _rtB -> B_1036_1322_0 , _rtB -> B_1036_1323_0 , _rtB ->
B_1036_1930_0 , & _rtB -> OFFDelay_ec , & _rtDW -> OFFDelay_ec , & _rtZCE ->
OFFDelay_ec ) ; _rtB -> B_1036_1328_0 = ( _rtB -> ONDelay_mt . B_90_17_0 ||
_rtB -> OFFDelay_ec . B_86_19_0 ) ; _rtB -> B_1036_1329_0 = ! _rtB ->
B_1036_1328_0 ; _rtB -> B_1036_1330_0_p = ( _rtB -> B_1036_1318_0_k && _rtB
-> B_1036_1329_0 ) ; _rtB -> B_1036_1331_0 = ! _rtB -> B_1036_1094_0_j ; _rtB
-> B_1036_1332_0 = ( _rtB -> B_1036_1330_0_p && _rtB -> B_1036_1331_0 ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_1333_0 = _rtDW -> Memory_PreviousInput_oy ; _rtB -> B_1036_1334_0 = !
_rtB -> B_1036_1333_0 ; } _rtB -> B_1036_1335_0_e = ( _rtB -> B_1036_1332_0
&& _rtB -> B_1036_1334_0 ) ; _rtB -> B_1036_1336_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1337_0_l =
_rtDW -> Memory_PreviousInput_ku ; _rtB -> B_1036_1338_0_a = ! _rtB ->
B_1036_1337_0_l ; } _rtB -> B_1036_1339_0_e = _rtB -> B_1036_1332_0 ; _rtB ->
B_1036_1340_0_g = _rtB -> B_1036_1339_0_e ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_1341_0_f = _rtDW ->
Memory_PreviousInput_by ; _rtB -> B_1036_1342_0 = _rtB -> B_1036_1961_0_a [ 0
] ; } DZG_v3_POSITIVEEdge ( S , _rtB -> B_1036_1342_0 , _rtB ->
B_1036_1340_0_g , _rtB -> B_1036_1341_0_f , & _rtB -> POSITIVEEdge_bc , &
_rtDW -> POSITIVEEdge_bc ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_1036_1344_0 = _rtB -> B_1036_1961_0_a [ 1 ] ; }
DZG_v3_NEGATIVEEdge ( S , _rtB -> B_1036_1344_0 , _rtB -> B_1036_1340_0_g ,
_rtB -> B_1036_1341_0_f , & _rtB -> NEGATIVEEdge_ht5 , & _rtDW ->
NEGATIVEEdge_ht5 ) ; _rtB -> B_1036_1346_0_e = ( _rtB -> POSITIVEEdge_bc .
B_85_0_0 || _rtB -> NEGATIVEEdge_ht5 . B_84_0_0 ) ; _rtB -> B_1036_1347_0_n =
( _rtB -> B_1036_1338_0_a && _rtB -> B_1036_1346_0_e ) ; _rtB ->
B_1036_1348_0 = _rtB -> B_1036_1347_0_n ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_1349_0 = _rtDW -> ICic_PreviousInput_j ;
} if ( _rtB -> B_1036_1348_0 ) { _rtB -> B_1036_1350_0 = _rtB ->
B_1036_1336_0 ; } else { _rtB -> B_1036_1350_0 = _rtB -> B_1036_1349_0 ; }
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_1351_0 = _rtP -> P_523 ; } _rtB -> B_1036_1352_0 = _rtB ->
B_1036_1350_0 + _rtB -> B_1036_1351_0 ; _rtB -> B_1036_1353_0 = ( _rtB ->
B_1036_1352_0 > _rtB -> B_1036_1336_0 ) ; _rtB -> B_1036_1354_0_m = ( _rtB ->
B_1036_1335_0_e || _rtB -> B_1036_1353_0 ) ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1355_0_b = _rtDW ->
u4_PreviousInput_e ; } _rtB -> B_1036_1356_0 = _rtB -> B_1036_1037_0 * _rtB
-> B_1036_1037_0 ; _rtB -> B_1036_1357_0 = _rtB -> B_1036_1065_0 * _rtB ->
B_1036_1065_0 ; _rtB -> B_1036_1358_0 = _rtB -> B_1036_1356_0 + _rtB ->
B_1036_1357_0 ; _rtB -> B_1036_1359_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1358_0 ) ; _rtB -> B_1036_1360_0 = ( _rtB -> B_1036_1727_0_c > _rtB ->
B_1036_1359_0 ) ; if ( _rtB -> B_1036_1360_0 ) { _rtB -> B_1036_1361_0 = _rtB
-> B_1036_1722_0_m ; _rtB -> B_1036_1363_0 = _rtB -> B_1036_1724_0_m ; } else
{ _rtB -> B_1036_1361_0 = _rtB -> B_1036_1037_0 ; _rtB -> B_1036_1363_0 =
_rtB -> B_1036_1065_0 ; } _rtB -> B_1036_1362_0 = _rtB -> B_1036_1361_0 *
_rtB -> B_1036_1361_0 ; _rtB -> B_1036_1364_0 = _rtB -> B_1036_1363_0 * _rtB
-> B_1036_1363_0 ; _rtB -> B_1036_1365_0 = _rtB -> B_1036_1362_0 + _rtB ->
B_1036_1364_0 ; _rtB -> B_1036_1366_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1365_0 ) ; _rtB -> B_1036_1367_0 = _rtB -> B_1036_1058_0 * _rtB ->
B_1036_1058_0 ; _rtB -> B_1036_1368_0 = _rtB -> B_1036_1071_0 * _rtB ->
B_1036_1071_0 ; _rtB -> B_1036_1369_0 = _rtB -> B_1036_1367_0 + _rtB ->
B_1036_1368_0 ; _rtB -> B_1036_1370_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1369_0 ) ; _rtB -> B_1036_1371_0_n = ( _rtB -> B_1036_1366_0 > _rtB ->
B_1036_1370_0 ) ; if ( _rtB -> B_1036_1371_0_n ) { _rtB -> B_1036_1372_0 =
_rtB -> B_1036_1361_0 ; _rtB -> B_1036_1373_0 = _rtB -> B_1036_1363_0 ; }
else { _rtB -> B_1036_1372_0 = _rtB -> B_1036_1058_0 ; _rtB -> B_1036_1373_0
= _rtB -> B_1036_1071_0 ; } _rtB -> B_1036_1374_0 = _rtB -> B_1036_1372_0 -
_rtB -> B_1036_1059_0 ; _rtB -> B_1036_1375_0 = _rtB -> B_1036_1373_0 - _rtB
-> B_1036_1072_0 ; _rtB -> B_1036_1376_0 = _rtB -> B_1036_1372_0 * _rtB ->
B_1036_1372_0 ; _rtB -> B_1036_1377_0 = _rtB -> B_1036_1373_0 * _rtB ->
B_1036_1373_0 ; _rtB -> B_1036_1378_0 = _rtB -> B_1036_1376_0 + _rtB ->
B_1036_1377_0 ; _rtB -> B_1036_1379_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1378_0 ) ; _rtB -> B_1036_1380_0 = _rtB -> B_1036_1372_0 / _rtB ->
B_1036_1379_0 ; _rtB -> B_1036_1381_0 = _rtB -> B_1036_1374_0 * _rtB ->
B_1036_1374_0 ; _rtB -> B_1036_1382_0 = _rtB -> B_1036_1375_0 * _rtB ->
B_1036_1375_0 ; _rtB -> B_1036_1383_0 = _rtB -> B_1036_1381_0 + _rtB ->
B_1036_1382_0 ; _rtB -> B_1036_1384_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1383_0 ) ; _rtB -> B_1036_1385_0 = _rtB -> B_1036_1374_0 / _rtB ->
B_1036_1384_0 ; _rtB -> B_1036_1386_0 = _rtB -> B_1036_1380_0 - _rtB ->
B_1036_1385_0 ; _rtB -> B_1036_1387_0 = _rtB -> B_1036_1386_0 * _rtB ->
B_1036_1386_0 ; _rtB -> B_1036_1388_0 = _rtB -> B_1036_1373_0 / _rtB ->
B_1036_1379_0 ; _rtB -> B_1036_1389_0 = _rtB -> B_1036_1375_0 / _rtB ->
B_1036_1384_0 ; _rtB -> B_1036_1390_0 = _rtB -> B_1036_1388_0 - _rtB ->
B_1036_1389_0 ; _rtB -> B_1036_1391_0 = _rtB -> B_1036_1390_0 * _rtB ->
B_1036_1390_0 ; _rtB -> B_1036_1392_0 = _rtB -> B_1036_1387_0 + _rtB ->
B_1036_1391_0 ; _rtB -> B_1036_1393_0 = _rtB -> B_1036_1392_0 * _rtB ->
B_1036_1566_0 ; _rtB -> B_1036_1394_0 = _rtB -> B_1036_1565_0 - _rtB ->
B_1036_1393_0 ; u0 = _rtB -> B_1036_1394_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB -> B_1036_1395_0 =
muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_1396_0 = _rtB -> B_1036_1388_0 -
_rtB -> B_1036_1385_0 ; _rtB -> B_1036_1397_0 = _rtB -> B_1036_1396_0 * _rtB
-> B_1036_1396_0 ; _rtB -> B_1036_1398_0 = _rtB -> B_1036_1380_0 + _rtB ->
B_1036_1389_0 ; _rtB -> B_1036_1399_0 = _rtB -> B_1036_1398_0 * _rtB ->
B_1036_1398_0 ; _rtB -> B_1036_1400_0 = _rtB -> B_1036_1397_0 + _rtB ->
B_1036_1399_0 ; _rtB -> B_1036_1401_0_c = ( _rtB -> B_1036_1400_0 > _rtB ->
B_1036_1563_0 ) ; if ( _rtB -> B_1036_1401_0_c ) { _rtB -> B_673_0_0 = _rtB
-> B_1036_1395_0 * _rtB -> B_1036_1564_0 ; _rtB -> B_1036_1403_0 = _rtB ->
B_673_0_0 ; } else { _rtB -> B_1036_1403_0 = _rtB -> B_1036_1395_0 ; } _rtB
-> B_1036_1404_0 = muDoubleScalarCos ( _rtB -> B_1036_1403_0 ) ; _rtB ->
B_1036_1405_0_m = ( _rtB -> B_1036_1404_0 >= _rtB -> B_1036_1554_0 ) ; if (
_rtB -> B_1036_1405_0_m ) { _rtB -> B_669_0_0 = _rtB -> B_1036_1375_0 * _rtB
-> B_1036_1375_0 ; _rtB -> B_669_1_0 = _rtB -> B_1036_1374_0 * _rtB ->
B_1036_1374_0 ; _rtB -> B_669_2_0 = _rtB -> B_669_1_0 + _rtB -> B_669_0_0 ;
_rtB -> B_669_3_0 = muDoubleScalarSqrt ( _rtB -> B_669_2_0 ) ; _rtB ->
B_669_4_0 = _rtB -> B_1036_1373_0 * _rtB -> B_1036_1373_0 ; _rtB -> B_669_5_0
= _rtB -> B_1036_1372_0 * _rtB -> B_1036_1372_0 ; _rtB -> B_669_6_0 = _rtB ->
B_669_5_0 + _rtB -> B_669_4_0 ; _rtB -> B_669_7_0 = muDoubleScalarSqrt ( _rtB
-> B_669_6_0 ) ; _rtB -> B_669_8_0 = _rtB -> B_669_7_0 * _rtB -> B_669_3_0 *
_rtB -> B_1036_1404_0 ; _rtB -> B_669_9_0 = muDoubleScalarSqrt ( _rtB ->
B_669_8_0 ) ; _rtB -> B_1036_1407_0 = _rtB -> B_669_9_0 ; } else { _rtB ->
B_1036_1407_0 = _rtB -> B_1036_1553_0 ; } if ( _rtB -> B_1036_1451_0_m ) {
_rtB -> B_1036_1409_0 = _rtB -> B_1036_1427_0_p ; } else { _rtB -> B_652_0_0
= ( _rtB -> B_1036_1407_0 > _rtB -> B_1036_1442_0_c ) ; if ( _rtB ->
B_652_0_0 ) { if ( _rtB -> B_1036_1439_0_a ) { _rtB -> B_650_0_0 = _rtB ->
B_1036_1431_0 * _rtB -> B_1036_1407_0 ; _rtB -> B_650_1_0 = _rtB -> B_650_0_0
+ _rtB -> B_1036_1456_0_g ; _rtB -> B_651_1_0 = _rtB -> B_650_1_0 ; } else {
_rtB -> B_651_1_0 = _rtB -> B_1036_1189_0 ; } _rtB -> B_652_3_0 = _rtB ->
B_651_1_0 ; } else { _rtB -> B_649_0_0 = ( _rtB -> B_1036_1407_0 <= _rtB ->
B_1036_1442_0_c ) ; _rtB -> B_649_1_0 = ( _rtB -> B_1036_1407_0 > _rtB ->
B_1036_1192_0_c ) ; _rtB -> B_649_2_0 = ( _rtB -> B_649_1_0 && _rtB ->
B_649_0_0 ) ; if ( _rtB -> B_649_2_0 ) { _rtB -> B_648_0_0 = _rtB ->
B_1036_1407_0 * _rtB -> B_1036_1431_0 ; _rtB -> B_648_1_0 = _rtB -> B_648_0_0
+ _rtB -> B_1036_1456_0_g ; _rtB -> B_649_4_0 = _rtB -> B_648_1_0 ; } else {
_rtB -> B_649_4_0 = _rtB -> B_1036_1427_0_p ; } _rtB -> B_652_3_0 = _rtB ->
B_649_4_0 ; } _rtB -> B_1036_1409_0 = _rtB -> B_652_3_0 ; } if ( _rtB ->
B_1036_1480_0 ) { _rtB -> B_1036_1411_0 = _rtB -> B_1036_1409_0 ; } else {
_rtB -> B_657_0_0 = ( _rtB -> B_1036_1407_0 > _rtB -> B_1036_1471_0 ) ; if (
_rtB -> B_657_0_0 ) { if ( _rtB -> B_1036_1468_0_b ) { _rtB -> B_655_0_0 =
_rtB -> B_1036_1460_0_e * _rtB -> B_1036_1407_0 ; _rtB -> B_655_1_0 = _rtB ->
B_655_0_0 + _rtB -> B_1036_1485_0 ; _rtB -> B_656_1_0 = _rtB -> B_655_1_0 ; }
else { _rtB -> B_656_1_0 = _rtB -> B_1036_1189_0 ; } _rtB -> B_657_3_0 = _rtB
-> B_656_1_0 ; } else { _rtB -> B_654_0_0 = ( _rtB -> B_1036_1407_0 <= _rtB
-> B_1036_1471_0 ) ; _rtB -> B_654_1_0 = ( _rtB -> B_1036_1407_0 > _rtB ->
B_1036_1172_0 ) ; _rtB -> B_654_2_0 = ( _rtB -> B_654_1_0 && _rtB ->
B_654_0_0 ) ; if ( _rtB -> B_654_2_0 ) { _rtB -> B_653_0_0 = _rtB ->
B_1036_1407_0 * _rtB -> B_1036_1460_0_e ; _rtB -> B_653_1_0 = _rtB ->
B_653_0_0 + _rtB -> B_1036_1485_0 ; _rtB -> B_654_4_0 = _rtB -> B_653_1_0 ; }
else { _rtB -> B_654_4_0 = _rtB -> B_1036_1409_0 ; } _rtB -> B_657_3_0 = _rtB
-> B_654_4_0 ; } _rtB -> B_1036_1411_0 = _rtB -> B_657_3_0 ; } if ( _rtB ->
B_1036_1509_0_j ) { _rtB -> B_1036_1413_0 = _rtB -> B_1036_1411_0 ; } else {
_rtB -> B_662_0_0 = ( _rtB -> B_1036_1407_0 > _rtB -> B_1036_1500_0_a ) ; if
( _rtB -> B_662_0_0 ) { if ( _rtB -> B_1036_1497_0_o ) { _rtB -> B_660_0_0 =
_rtB -> B_1036_1489_0_l * _rtB -> B_1036_1407_0 ; _rtB -> B_660_1_0 = _rtB ->
B_660_0_0 + _rtB -> B_1036_1514_0_i ; _rtB -> B_661_1_0 = _rtB -> B_660_1_0 ;
} else { _rtB -> B_661_1_0 = _rtB -> B_1036_1189_0 ; } _rtB -> B_662_3_0 =
_rtB -> B_661_1_0 ; } else { _rtB -> B_659_0_0 = ( _rtB -> B_1036_1407_0 <=
_rtB -> B_1036_1500_0_a ) ; _rtB -> B_659_1_0 = ( _rtB -> B_1036_1407_0 >
_rtB -> B_1036_1174_0 ) ; _rtB -> B_659_2_0 = ( _rtB -> B_659_1_0 && _rtB ->
B_659_0_0 ) ; if ( _rtB -> B_659_2_0 ) { _rtB -> B_658_0_0 = _rtB ->
B_1036_1407_0 * _rtB -> B_1036_1489_0_l ; _rtB -> B_658_1_0 = _rtB ->
B_658_0_0 + _rtB -> B_1036_1514_0_i ; _rtB -> B_659_4_0 = _rtB -> B_658_1_0 ;
} else { _rtB -> B_659_4_0 = _rtB -> B_1036_1411_0 ; } _rtB -> B_662_3_0 =
_rtB -> B_659_4_0 ; } _rtB -> B_1036_1413_0 = _rtB -> B_662_3_0 ; } if ( _rtB
-> B_1036_1538_0_l ) { _rtB -> B_1036_1415_0 = _rtB -> B_1036_1413_0 ; } else
{ _rtB -> B_667_0_0 = ( _rtB -> B_1036_1407_0 > _rtB -> B_1036_1529_0 ) ; if
( _rtB -> B_667_0_0 ) { if ( _rtB -> B_1036_1526_0_c ) { _rtB -> B_665_0_0 =
_rtB -> B_1036_1518_0_c * _rtB -> B_1036_1407_0 ; _rtB -> B_665_1_0 = _rtB ->
B_665_0_0 + _rtB -> B_1036_1543_0 ; _rtB -> B_666_1_0 = _rtB -> B_665_1_0 ; }
else { _rtB -> B_666_1_0 = _rtB -> B_1036_1189_0 ; } _rtB -> B_667_3_0 = _rtB
-> B_666_1_0 ; } else { _rtB -> B_664_0_0 = ( _rtB -> B_1036_1407_0 <= _rtB
-> B_1036_1529_0 ) ; _rtB -> B_664_1_0 = ( _rtB -> B_1036_1407_0 > _rtB ->
B_1036_1176_0 ) ; _rtB -> B_664_2_0 = ( _rtB -> B_664_1_0 && _rtB ->
B_664_0_0 ) ; if ( _rtB -> B_664_2_0 ) { _rtB -> B_663_0_0 = _rtB ->
B_1036_1407_0 * _rtB -> B_1036_1518_0_c ; _rtB -> B_663_1_0 = _rtB ->
B_663_0_0 + _rtB -> B_1036_1543_0 ; _rtB -> B_664_4_0 = _rtB -> B_663_1_0 ; }
else { _rtB -> B_664_4_0 = _rtB -> B_1036_1413_0 ; } _rtB -> B_667_3_0 = _rtB
-> B_664_4_0 ; } _rtB -> B_1036_1415_0 = _rtB -> B_667_3_0 ; } _rtB ->
B_1036_1416_0 = ( _rtB -> B_1036_1075_0 > _rtB -> B_1036_1415_0 ) ; _rtB ->
B_1036_1417_0_j = ( _rtB -> B_1036_1415_0 == _rtB -> B_1036_1550_0 ) ; _rtB
-> B_1036_1418_0 = ! _rtB -> B_1036_1417_0_j ; _rtB -> B_1036_1419_0 = ( _rtB
-> B_1036_1416_0 && _rtB -> B_1036_1418_0 ) ; _rtB -> B_1036_1420_0 = ( _rtB
-> B_1036_1075_0 > _rtB -> B_1036_1189_0 ) ; _rtB -> B_1036_1421_0 = ( _rtB
-> B_1036_1420_0 && _rtB -> B_1036_1552_0_h ) ; _rtB -> B_1036_1422_0_h = (
_rtB -> B_1036_1419_0 || _rtB -> B_1036_1421_0 ) ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1423_0_c = _rtDW ->
Memory_PreviousInput_bl ; _rtB -> B_1036_1424_0_m = ! _rtB -> B_1036_1423_0_c
; } if ( _rtB -> B_1036_1181_0 > _rtP -> P_524 ) { _rtB -> B_758_0_0 = ( _rtB
-> B_1036_1422_0_h && _rtB -> B_1036_1424_0_m ) ; _rtB -> B_1036_1426_0_n =
_rtB -> B_758_0_0 ; } else { _rtB -> B_1036_1426_0_n = _rtB ->
B_1036_1422_0_h ; } _rtB -> B_1036_1427_0 = _rtB -> B_1036_407_0 * _rtB ->
B_1036_407_0 ; _rtB -> B_1036_1428_0 = _rtB -> B_1036_427_0 * _rtB ->
B_1036_427_0 ; _rtB -> B_1036_1429_0 = _rtB -> B_1036_1427_0 + _rtB ->
B_1036_1428_0 ; _rtB -> B_1036_1430_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1429_0 ) ; _rtB -> B_1036_1431_0_e = ( _rtB -> B_1036_1430_0 < 0.0 ) ;
_rtB -> B_1036_1432_0_c = ! _rtB -> B_1036_1431_0_e ; isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1433_0 = _rtDW ->
u_PreviousInput_h2 ; } _rtB -> B_1036_1434_0 = _rtB -> B_1036_407_0 - _rtB ->
B_1036_1433_0 ; _rtB -> B_1036_1435_0 = _rtB -> B_1036_1434_0 * _rtB ->
B_1036_1434_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_1436_0 = _rtDW -> u_PreviousInput_ab ; } _rtB -> B_1036_1437_0
= _rtB -> B_1036_427_0 - _rtB -> B_1036_1436_0 ; _rtB -> B_1036_1438_0 = _rtB
-> B_1036_1437_0 * _rtB -> B_1036_1437_0 ; _rtB -> B_1036_1439_0 = _rtB ->
B_1036_1435_0 + _rtB -> B_1036_1438_0 ; _rtB -> B_1036_1440_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1439_0 ) ; _rtB -> B_1036_1441_0 = _rtB
-> B_1036_407_0 * _rtB -> B_1036_407_0 ; _rtB -> B_1036_1442_0 = _rtB ->
B_1036_427_0 * _rtB -> B_1036_427_0 ; _rtB -> B_1036_1443_0 = _rtB ->
B_1036_1441_0 + _rtB -> B_1036_1442_0 ; _rtB -> B_1036_1444_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1443_0 ) ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1445_0 = _rtB -> B_1036_1433_0 *
_rtB -> B_1036_1433_0 ; _rtB -> B_1036_1446_0 = _rtB -> B_1036_1436_0 * _rtB
-> B_1036_1436_0 ; _rtB -> B_1036_1447_0 = _rtB -> B_1036_1445_0 + _rtB ->
B_1036_1446_0 ; _rtB -> B_1036_1448_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1447_0 ) ; } maxV = muDoubleScalarMax ( _rtB -> B_1036_1444_0 , _rtB
-> B_1036_1448_0 ) ; _rtB -> B_1036_1449_0 = maxV ; _rtB -> B_1036_1450_0 = (
_rtB -> B_1036_1449_0 == _rtB -> B_1036_409_0_d ) ; if ( _rtB ->
B_1036_1450_0 ) { _rtB -> B_1036_1451_0 = _rtB -> B_1036_410_0_e ; } else {
_rtB -> B_1036_1451_0 = _rtB -> B_1036_1449_0 ; } _rtB -> B_1036_1452_0 =
_rtB -> B_1036_1440_0 / _rtB -> B_1036_1451_0 ; _rtB -> B_1036_1453_0_b = (
_rtB -> B_1036_1452_0 > _rtB -> B_1036_368_0 ) ; _rtB -> B_1036_1454_0_p = (
_rtB -> B_1036_1432_0_c && _rtB -> B_1036_1453_0_b ) ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1455_0 = _rtDW ->
u_PreviousInput_js ; _rtB -> B_1036_1456_0 = _rtDW -> u_PreviousInput_m ;
_rtB -> B_1036_1457_0 = _rtDW -> u_PreviousInput_b ; _rtB -> B_1036_1458_0 =
( _rtB -> B_1036_1455_0 + _rtB -> B_1036_1456_0 ) + _rtB -> B_1036_1457_0 ;
_rtB -> B_1036_1459_0 = _rtB -> B_1036_1458_0 * _rtB -> B_1036_406_0_k ; }
_rtB -> B_1036_1460_0 = _rtB -> B_1036_1440_0 - _rtB -> B_1036_1459_0 ; _rtB
-> B_1036_1461_0 = muDoubleScalarAbs ( _rtB -> B_1036_1460_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1462_0_i = (
_rtB -> B_1036_1459_0 == _rtB -> B_1036_407_0_f ) ; if ( _rtB ->
B_1036_1462_0_i ) { _rtB -> B_1036_1463_0 = _rtB -> B_1036_408_0_a ; } else {
_rtB -> B_1036_1463_0 = _rtB -> B_1036_1459_0 ; } } _rtB -> B_1036_1464_0 =
_rtB -> B_1036_1461_0 / _rtB -> B_1036_1463_0 ; _rtB -> B_1036_1465_0 = (
_rtB -> B_1036_1464_0 > _rtB -> B_1036_367_0 ) ; _rtB -> B_1036_1466_0 = (
_rtB -> B_1036_1440_0 < _rtB -> B_1036_365_0 ) ; _rtB -> B_1036_1467_0 = !
_rtB -> B_1036_1466_0 ; _rtB -> B_1036_1468_0 = ( _rtB -> B_1036_1465_0 &&
_rtB -> B_1036_1467_0 ) ; _rtB -> B_1036_1469_0 = ( _rtB -> B_1036_1454_0_p
&& _rtB -> B_1036_1468_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_1036_1470_0 = _rtDW -> u4_PreviousInput_gl ; _rtB ->
B_1036_1471_0_o = _rtDW -> u1_PreviousInput_c ; _rtB -> B_1036_1472_0_n = !
_rtB -> B_1036_1471_0_o ; _rtB -> B_1036_1473_0 = ( _rtB -> B_1036_1470_0 &&
_rtB -> B_1036_1472_0_n ) ; } _rtB -> B_1036_1474_0 = ( _rtB -> B_1036_1469_0
|| _rtB -> B_1036_1473_0 ) ; _rtB -> B_1036_1475_0 = _rtB -> B_1036_1474_0 ;
_rtB -> B_1036_1476_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_1477_0 = _rtB -> B_1036_402_0_h ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_1477_0 , _rtB -> B_1036_1475_0 , _rtB ->
B_1036_1476_0 , _rtB -> B_1036_400_0_l , & _rtB -> ONDelay_ih , & _rtDW ->
ONDelay_ih , & _rtZCE -> ONDelay_ih ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_1479_0 = _rtB -> B_1036_403_0_l ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_1479_0 , _rtB -> B_1036_1475_0 , _rtB ->
B_1036_1476_0 , _rtB -> B_1036_400_0_l , & _rtB -> OFFDelay_nd , & _rtDW ->
OFFDelay_nd , & _rtZCE -> OFFDelay_nd ) ; _rtB -> B_1036_1481_0_m = ( _rtB ->
ONDelay_ih . B_90_17_0 || _rtB -> OFFDelay_nd . B_86_19_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1482_0_m =
_rtDW -> Memory_PreviousInput_g5 ; _rtB -> B_1036_1483_0_d = ! _rtB ->
B_1036_1482_0_m ; } _rtB -> B_1036_1484_0_i = ( _rtB -> B_1036_1481_0_m &&
_rtB -> B_1036_1483_0_d ) ; _rtB -> B_1036_1485_0_e = ( _rtB ->
B_1036_1484_0_i && _rtB -> B_1036_369_0_f ) ; _rtB -> B_1036_1486_0 = _rtB ->
B_1036_716_0 * _rtB -> B_1036_716_0 ; _rtB -> B_1036_1487_0 = _rtB ->
B_1036_738_0 * _rtB -> B_1036_738_0 ; _rtB -> B_1036_1488_0 = _rtB ->
B_1036_1486_0 + _rtB -> B_1036_1487_0 ; _rtB -> B_1036_1489_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1488_0 ) ; _rtB -> B_1036_1490_0_k = (
_rtB -> B_1036_1489_0 < 0.0 ) ; _rtB -> B_1036_1491_0_p = ! _rtB ->
B_1036_1490_0_k ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_1492_0 = _rtDW -> u_PreviousInput_in ; } _rtB -> B_1036_1493_0
= _rtB -> B_1036_716_0 - _rtB -> B_1036_1492_0 ; _rtB -> B_1036_1494_0 = _rtB
-> B_1036_1493_0 * _rtB -> B_1036_1493_0 ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_1495_0 = _rtDW -> u_PreviousInput_kw ;
} _rtB -> B_1036_1496_0 = _rtB -> B_1036_738_0 - _rtB -> B_1036_1495_0 ; _rtB
-> B_1036_1497_0 = _rtB -> B_1036_1496_0 * _rtB -> B_1036_1496_0 ; _rtB ->
B_1036_1498_0 = _rtB -> B_1036_1494_0 + _rtB -> B_1036_1497_0 ; _rtB ->
B_1036_1499_0 = muDoubleScalarSqrt ( _rtB -> B_1036_1498_0 ) ; _rtB ->
B_1036_1500_0 = _rtB -> B_1036_716_0 * _rtB -> B_1036_716_0 ; _rtB ->
B_1036_1501_0 = _rtB -> B_1036_738_0 * _rtB -> B_1036_738_0 ; _rtB ->
B_1036_1502_0 = _rtB -> B_1036_1500_0 + _rtB -> B_1036_1501_0 ; _rtB ->
B_1036_1503_0 = muDoubleScalarSqrt ( _rtB -> B_1036_1502_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1504_0 =
_rtB -> B_1036_1492_0 * _rtB -> B_1036_1492_0 ; _rtB -> B_1036_1505_0 = _rtB
-> B_1036_1495_0 * _rtB -> B_1036_1495_0 ; _rtB -> B_1036_1506_0 = _rtB ->
B_1036_1504_0 + _rtB -> B_1036_1505_0 ; _rtB -> B_1036_1507_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1506_0 ) ; } maxV = muDoubleScalarMax (
_rtB -> B_1036_1503_0 , _rtB -> B_1036_1507_0 ) ; _rtB -> B_1036_1508_0 =
maxV ; _rtB -> B_1036_1509_0 = ( _rtB -> B_1036_1508_0 == _rtB ->
B_1036_457_0_f ) ; if ( _rtB -> B_1036_1509_0 ) { _rtB -> B_1036_1510_0 =
_rtB -> B_1036_458_0 ; } else { _rtB -> B_1036_1510_0 = _rtB -> B_1036_1508_0
; } _rtB -> B_1036_1511_0 = _rtB -> B_1036_1499_0 / _rtB -> B_1036_1510_0 ;
_rtB -> B_1036_1512_0_j = ( _rtB -> B_1036_1511_0 > _rtB -> B_1036_416_0_a )
; _rtB -> B_1036_1513_0_c = ( _rtB -> B_1036_1491_0_p && _rtB ->
B_1036_1512_0_j ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_1514_0 = _rtDW -> u_PreviousInput_k2 ; _rtB -> B_1036_1515_0 =
_rtDW -> u_PreviousInput_g ; _rtB -> B_1036_1516_0 = _rtDW ->
u_PreviousInput_iw ; _rtB -> B_1036_1517_0 = ( _rtB -> B_1036_1514_0 + _rtB
-> B_1036_1515_0 ) + _rtB -> B_1036_1516_0 ; _rtB -> B_1036_1518_0 = _rtB ->
B_1036_1517_0 * _rtB -> B_1036_454_0_d ; } _rtB -> B_1036_1519_0 = _rtB ->
B_1036_1499_0 - _rtB -> B_1036_1518_0 ; _rtB -> B_1036_1520_0 =
muDoubleScalarAbs ( _rtB -> B_1036_1519_0 ) ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1521_0_b = ( _rtB -> B_1036_1518_0
== _rtB -> B_1036_455_0_j ) ; if ( _rtB -> B_1036_1521_0_b ) { _rtB ->
B_1036_1522_0 = _rtB -> B_1036_456_0 ; } else { _rtB -> B_1036_1522_0 = _rtB
-> B_1036_1518_0 ; } } _rtB -> B_1036_1523_0 = _rtB -> B_1036_1520_0 / _rtB
-> B_1036_1522_0 ; _rtB -> B_1036_1524_0 = ( _rtB -> B_1036_1523_0 > _rtB ->
B_1036_415_0_b ) ; _rtB -> B_1036_1525_0 = ( _rtB -> B_1036_1499_0 < _rtB ->
B_1036_413_0_e ) ; _rtB -> B_1036_1526_0 = ! _rtB -> B_1036_1525_0 ; _rtB ->
B_1036_1527_0 = ( _rtB -> B_1036_1524_0 && _rtB -> B_1036_1526_0 ) ; _rtB ->
B_1036_1528_0 = ( _rtB -> B_1036_1513_0_c && _rtB -> B_1036_1527_0 ) ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1529_0_l =
_rtDW -> u4_PreviousInput_i ; _rtB -> B_1036_1530_0_n = _rtDW ->
u1_PreviousInput_b ; _rtB -> B_1036_1531_0 = ! _rtB -> B_1036_1530_0_n ; _rtB
-> B_1036_1532_0 = ( _rtB -> B_1036_1529_0_l && _rtB -> B_1036_1531_0 ) ; }
_rtB -> B_1036_1533_0 = ( _rtB -> B_1036_1528_0 || _rtB -> B_1036_1532_0 ) ;
_rtB -> B_1036_1534_0_a = _rtB -> B_1036_1533_0 ; _rtB -> B_1036_1535_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_1536_0 = _rtB -> B_1036_450_0_g ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_1536_0 , _rtB -> B_1036_1534_0_a , _rtB -> B_1036_1535_0 , _rtB ->
B_1036_448_0_d , & _rtB -> ONDelay_m , & _rtDW -> ONDelay_m , & _rtZCE ->
ONDelay_m ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_1538_0 = _rtB -> B_1036_451_0_n ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_1538_0 , _rtB -> B_1036_1534_0_a , _rtB -> B_1036_1535_0 , _rtB ->
B_1036_448_0_d , & _rtB -> OFFDelay_er , & _rtDW -> OFFDelay_er , & _rtZCE ->
OFFDelay_er ) ; _rtB -> B_1036_1540_0_n = ( _rtB -> ONDelay_m . B_90_17_0 ||
_rtB -> OFFDelay_er . B_86_19_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if
( isHit != 0 ) { _rtB -> B_1036_1541_0_d = _rtDW -> Memory_PreviousInput_n3 ;
_rtB -> B_1036_1542_0_g = ! _rtB -> B_1036_1541_0_d ; } _rtB ->
B_1036_1543_0_f = ( _rtB -> B_1036_1540_0_n && _rtB -> B_1036_1542_0_g ) ;
_rtB -> B_1036_1544_0_g = ( _rtB -> B_1036_1543_0_f && _rtB -> B_1036_417_0_j
) ; _rtB -> B_1036_1545_0 = ( _rtB -> B_1036_1485_0_e || _rtB ->
B_1036_1544_0_g ) ; _rtB -> B_1036_1546_0 = _rtB -> B_1036_1545_0 ; _rtB ->
B_1036_1547_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_1548_0 = _rtB -> B_1036_1939_0_o ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_1548_0 , _rtB -> B_1036_1546_0 , _rtB ->
B_1036_1547_0 , _rtB -> B_1036_1937_0_p , & _rtB -> ONDelay_ixo , & _rtDW ->
ONDelay_ixo , & _rtZCE -> ONDelay_ixo ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_1550_0_l = _rtB -> B_1036_1940_0_g ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_1550_0_l , _rtB -> B_1036_1546_0 , _rtB
-> B_1036_1547_0 , _rtB -> B_1036_1937_0_p , & _rtB -> OFFDelay_os , & _rtDW
-> OFFDelay_os , & _rtZCE -> OFFDelay_os ) ; _rtB -> B_1036_1552_0 = ( _rtB
-> ONDelay_ixo . B_90_17_0 || _rtB -> OFFDelay_os . B_86_19_0 ) ; _rtB ->
B_1036_1553_0_l = ( _rtB -> B_1036_1426_0_n && _rtB -> B_1036_1552_0 ) ; _rtB
-> B_1036_1554_0_o = ( _rtB -> B_1036_1355_0_b || _rtB -> B_1036_1553_0_l ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_1555_0_k = _rtDW -> u2_PreviousInput_g ; } _rtB -> B_1036_1556_0_g = !
_rtB -> B_1036_1422_0_h ; _rtB -> B_1036_1557_0_k = ( _rtB -> B_1036_1555_0_k
&& _rtB -> B_1036_1556_0_g ) ; _rtB -> B_1036_1558_0_j = _rtB ->
B_1036_1557_0_k ; _rtB -> B_1036_1559_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1560_0_k =
_rtB -> B_1036_1946_0_n ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_1560_0_k ,
_rtB -> B_1036_1558_0_j , _rtB -> B_1036_1559_0 , _rtB -> B_1036_1944_0_n , &
_rtB -> ONDelay_cs , & _rtDW -> ONDelay_cs , & _rtZCE -> ONDelay_cs ) ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1562_0_g =
_rtB -> B_1036_1947_0_g ; } DZG_v3_OFFDelay ( S , _rtB -> B_1036_1562_0_g ,
_rtB -> B_1036_1558_0_j , _rtB -> B_1036_1559_0 , _rtB -> B_1036_1944_0_n , &
_rtB -> OFFDelay_dh , & _rtDW -> OFFDelay_dh , & _rtZCE -> OFFDelay_dh ) ;
_rtB -> B_1036_1564_0_h = ( _rtB -> ONDelay_cs . B_90_17_0 || _rtB ->
OFFDelay_dh . B_86_19_0 ) ; _rtB -> B_1036_1565_0_m = ! _rtB ->
B_1036_1564_0_h ; _rtB -> B_1036_1566_0_k = ( _rtB -> B_1036_1554_0_o && _rtB
-> B_1036_1565_0_m ) ; _rtB -> B_1036_1567_0_c = ! _rtB -> B_1036_1094_0_j ;
_rtB -> B_1036_1568_0_h = ( _rtB -> B_1036_1566_0_k && _rtB ->
B_1036_1567_0_c ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_1569_0_h = _rtDW -> Memory_PreviousInput_fh ; _rtB ->
B_1036_1570_0_d = ! _rtB -> B_1036_1569_0_h ; } _rtB -> B_1036_1571_0_b = (
_rtB -> B_1036_1568_0_h && _rtB -> B_1036_1570_0_d ) ; _rtB -> B_1036_1572_0
= ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_1573_0_p = _rtDW -> Memory_PreviousInput_foz ; _rtB ->
B_1036_1574_0_c = ! _rtB -> B_1036_1573_0_p ; } _rtB -> B_1036_1575_0_b =
_rtB -> B_1036_1568_0_h ; _rtB -> B_1036_1576_0_l = _rtB -> B_1036_1575_0_b ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_1577_0_o = _rtDW -> Memory_PreviousInput_ij ; _rtB -> B_1036_1578_0 =
_rtB -> B_1036_1967_0_k [ 0 ] ; } DZG_v3_POSITIVEEdge ( S , _rtB ->
B_1036_1578_0 , _rtB -> B_1036_1576_0_l , _rtB -> B_1036_1577_0_o , & _rtB ->
POSITIVEEdge_da , & _rtDW -> POSITIVEEdge_da ) ; isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1580_0 = _rtB -> B_1036_1967_0_k
[ 1 ] ; } DZG_v3_NEGATIVEEdge ( S , _rtB -> B_1036_1580_0 , _rtB ->
B_1036_1576_0_l , _rtB -> B_1036_1577_0_o , & _rtB -> NEGATIVEEdge_by , &
_rtDW -> NEGATIVEEdge_by ) ; _rtB -> B_1036_1582_0_d = ( _rtB ->
POSITIVEEdge_da . B_85_0_0 || _rtB -> NEGATIVEEdge_by . B_84_0_0 ) ; _rtB ->
B_1036_1583_0_m = ( _rtB -> B_1036_1574_0_c && _rtB -> B_1036_1582_0_d ) ;
_rtB -> B_1036_1584_0_n = _rtB -> B_1036_1583_0_m ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_1585_0 = _rtDW ->
ICic_PreviousInput_n ; } if ( _rtB -> B_1036_1584_0_n ) { _rtB ->
B_1036_1586_0 = _rtB -> B_1036_1572_0 ; } else { _rtB -> B_1036_1586_0 = _rtB
-> B_1036_1585_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_1036_1587_0 = _rtP -> P_536 ; } _rtB -> B_1036_1588_0 = _rtB ->
B_1036_1586_0 + _rtB -> B_1036_1587_0 ; _rtB -> B_1036_1589_0_m = ( _rtB ->
B_1036_1588_0 > _rtB -> B_1036_1572_0 ) ; _rtB -> B_1036_1590_0_i = ( _rtB ->
B_1036_1571_0_b || _rtB -> B_1036_1589_0_m ) ; _rtB -> B_1036_1591_0_o = (
_rtB -> B_1036_1118_0 || _rtB -> B_1036_1354_0_m || _rtB -> B_1036_1590_0_i )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
ssCallAccelRunBlock ( S , 1036 , 1592 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 1036 , 1593 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 1036 , 1594 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 1036 , 1595 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 1036 , 1596 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_1036_1597_0 = _rtP -> P_537 * _rtB -> B_1036_7_0 [ 45 ] ; _rtB ->
B_1036_1598_0 = _rtP -> P_538 * _rtB -> B_1036_7_0 [ 46 ] ; _rtB ->
B_1036_1599_0 = _rtP -> P_539 * _rtB -> B_1036_7_0 [ 47 ] ; _rtB ->
B_1036_1600_0 [ 0 ] = _rtP -> P_540 * _rtB -> B_1036_1597_0 ; _rtB ->
B_1036_1600_0 [ 1 ] = _rtP -> P_540 * _rtB -> B_1036_1598_0 ; _rtB ->
B_1036_1600_0 [ 2 ] = _rtP -> P_540 * _rtB -> B_1036_1599_0 ; _rtB ->
B_1036_1601_0 [ 0 ] = _rtP -> P_541 * _rtB -> B_1036_1600_0 [ 0 ] ; _rtB ->
B_1036_1601_0 [ 1 ] = _rtP -> P_541 * _rtB -> B_1036_1600_0 [ 1 ] ; _rtB ->
B_1036_1601_0 [ 2 ] = _rtP -> P_541 * _rtB -> B_1036_1600_0 [ 2 ] ; _rtB ->
B_1036_1602_0 = _rtP -> P_542 * _rtB -> B_1036_7_0 [ 24 ] ; _rtB ->
B_1036_1603_0 = _rtP -> P_543 * _rtB -> B_1036_7_0 [ 25 ] ; _rtB ->
B_1036_1604_0 = _rtP -> P_544 * _rtB -> B_1036_7_0 [ 26 ] ; _rtB ->
B_1036_1605_0 [ 0 ] = _rtP -> P_545 * _rtB -> B_1036_1602_0 ; _rtB ->
B_1036_1605_0 [ 1 ] = _rtP -> P_545 * _rtB -> B_1036_1603_0 ; _rtB ->
B_1036_1605_0 [ 2 ] = _rtP -> P_545 * _rtB -> B_1036_1604_0 ; _rtB ->
B_1036_1606_0 [ 0 ] = _rtP -> P_546 * _rtB -> B_1036_1605_0 [ 0 ] ; _rtB ->
B_1036_1606_0 [ 1 ] = _rtP -> P_546 * _rtB -> B_1036_1605_0 [ 1 ] ; _rtB ->
B_1036_1606_0 [ 2 ] = _rtP -> P_546 * _rtB -> B_1036_1605_0 [ 2 ] ; _rtB ->
B_1036_1607_0 = _rtP -> P_547 * _rtB -> B_1036_7_0 [ 33 ] ; _rtB ->
B_1036_1608_0 = _rtP -> P_548 * _rtB -> B_1036_7_0 [ 34 ] ; _rtB ->
B_1036_1609_0 = _rtP -> P_549 * _rtB -> B_1036_7_0 [ 35 ] ; _rtB ->
B_1036_1610_0 [ 0 ] = _rtP -> P_550 * _rtB -> B_1036_1607_0 ; _rtB ->
B_1036_1610_0 [ 1 ] = _rtP -> P_550 * _rtB -> B_1036_1608_0 ; _rtB ->
B_1036_1610_0 [ 2 ] = _rtP -> P_550 * _rtB -> B_1036_1609_0 ; _rtB ->
B_1036_1611_0 [ 0 ] = _rtP -> P_551 * _rtB -> B_1036_1610_0 [ 0 ] ; _rtB ->
B_1036_1611_0 [ 1 ] = _rtP -> P_551 * _rtB -> B_1036_1610_0 [ 1 ] ; _rtB ->
B_1036_1611_0 [ 2 ] = _rtP -> P_551 * _rtB -> B_1036_1610_0 [ 2 ] ; } _rtB ->
B_1036_1612_0 = muDoubleScalarCos ( _rtB -> B_1036_275_0 [ 0 ] ) ; _rtB ->
B_1036_1613_0 = _rtB -> B_1036_260_0 [ 0 ] * _rtB -> B_1036_1612_0 ; _rtB ->
B_1036_1614_0 = _rtB -> B_1036_1613_0 * _rtB -> B_1036_639_0_k ; _rtB ->
B_1036_1615_0 = _rtB -> B_1036_1614_0 * _rtB -> B_1036_1614_0 ; _rtB ->
B_1036_1616_0 = muDoubleScalarSin ( _rtB -> B_1036_275_0 [ 0 ] ) ; _rtB ->
B_1036_1617_0 = _rtB -> B_1036_1616_0 * _rtB -> B_1036_260_0 [ 0 ] ; _rtB ->
B_1036_1618_0 = _rtB -> B_1036_1617_0 * _rtB -> B_1036_639_0_k ; _rtB ->
B_1036_1619_0 = _rtB -> B_1036_1618_0 * _rtB -> B_1036_1618_0 ; _rtB ->
B_1036_1620_0 = _rtB -> B_1036_1615_0 + _rtB -> B_1036_1619_0 ; _rtB ->
B_1036_1621_0 = muDoubleScalarSqrt ( _rtB -> B_1036_1620_0 ) ; _rtB ->
B_1036_1622_0 = _rtB -> B_1036_1614_0 / _rtB -> B_1036_1621_0 ; _rtB ->
B_1036_1623_0 = muDoubleScalarCos ( _rtB -> B_1036_275_0 [ 1 ] ) ; _rtB ->
B_1036_1624_0 = _rtB -> B_1036_260_0 [ 1 ] * _rtB -> B_1036_1623_0 ; _rtB ->
B_1036_1625_0 = _rtB -> B_1036_1624_0 * _rtB -> B_1036_639_0_k ; _rtB ->
B_1036_1626_0 = _rtB -> B_1036_1625_0 * _rtB -> B_1036_1625_0 ; _rtB ->
B_1036_1627_0 = muDoubleScalarSin ( _rtB -> B_1036_275_0 [ 1 ] ) ; _rtB ->
B_1036_1628_0 = _rtB -> B_1036_1627_0 * _rtB -> B_1036_260_0 [ 1 ] ; _rtB ->
B_1036_1629_0 = _rtB -> B_1036_1628_0 * _rtB -> B_1036_639_0_k ; _rtB ->
B_1036_1630_0 = _rtB -> B_1036_1629_0 * _rtB -> B_1036_1629_0 ; _rtB ->
B_1036_1631_0 = _rtB -> B_1036_1626_0 + _rtB -> B_1036_1630_0 ; _rtB ->
B_1036_1632_0 = muDoubleScalarSqrt ( _rtB -> B_1036_1631_0 ) ; _rtB ->
B_1036_1633_0 = _rtB -> B_1036_1625_0 / _rtB -> B_1036_1632_0 ; _rtB ->
B_1036_1634_0 = _rtB -> B_1036_1622_0 - _rtB -> B_1036_1633_0 ; _rtB ->
B_1036_1635_0 = _rtB -> B_1036_1634_0 * _rtB -> B_1036_1634_0 ; _rtB ->
B_1036_1636_0 = _rtB -> B_1036_1618_0 / _rtB -> B_1036_1621_0 ; _rtB ->
B_1036_1637_0 = _rtB -> B_1036_1629_0 / _rtB -> B_1036_1632_0 ; _rtB ->
B_1036_1638_0 = _rtB -> B_1036_1636_0 - _rtB -> B_1036_1637_0 ; _rtB ->
B_1036_1639_0 = _rtB -> B_1036_1638_0 * _rtB -> B_1036_1638_0 ; _rtB ->
B_1036_1640_0 = _rtB -> B_1036_1635_0 + _rtB -> B_1036_1639_0 ; _rtB ->
B_1036_1641_0 = _rtB -> B_1036_1640_0 * _rtB -> B_1036_120_0 ; _rtB ->
B_1036_1642_0 = _rtB -> B_1036_119_0_e - _rtB -> B_1036_1641_0 ; u0 = _rtB ->
B_1036_1642_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0
= - 1.0 ; } } _rtB -> B_1036_1643_0 = muDoubleScalarAcos ( u0 ) ; _rtB ->
B_1036_1644_0 = _rtB -> B_1036_1636_0 - _rtB -> B_1036_1633_0 ; _rtB ->
B_1036_1645_0 = _rtB -> B_1036_1644_0 * _rtB -> B_1036_1644_0 ; _rtB ->
B_1036_1646_0 = _rtB -> B_1036_1622_0 + _rtB -> B_1036_1637_0 ; _rtB ->
B_1036_1647_0 = _rtB -> B_1036_1646_0 * _rtB -> B_1036_1646_0 ; _rtB ->
B_1036_1648_0 = _rtB -> B_1036_1645_0 + _rtB -> B_1036_1647_0 ; _rtB ->
B_1036_1649_0_a = ( _rtB -> B_1036_1648_0 > _rtB -> B_1036_117_0_f ) ; if (
_rtB -> B_1036_1649_0_a ) { _rtB -> B_57_0_0 = _rtB -> B_1036_1643_0 * _rtB
-> B_1036_118_0_h ; _rtB -> B_1036_1651_0 = _rtB -> B_57_0_0 ; } else { _rtB
-> B_1036_1651_0 = _rtB -> B_1036_1643_0 ; } _rtB -> B_1036_1652_0 =
muDoubleScalarAbs ( _rtB -> B_1036_1651_0 ) ; _rtB -> B_1036_1653_0 = _rtB ->
B_1036_1625_0 * _rtB -> B_1036_1625_0 ; _rtB -> B_1036_1654_0 = _rtB ->
B_1036_1629_0 * _rtB -> B_1036_1629_0 ; _rtB -> B_1036_1655_0 = _rtB ->
B_1036_1653_0 + _rtB -> B_1036_1654_0 ; _rtB -> B_1036_1656_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1655_0 ) ; _rtB -> B_1036_1657_0 = _rtB
-> B_1036_1625_0 / _rtB -> B_1036_1656_0 ; _rtB -> B_1036_1658_0 =
muDoubleScalarCos ( _rtB -> B_1036_275_0 [ 2 ] ) ; _rtB -> B_1036_1659_0 =
_rtB -> B_1036_260_0 [ 2 ] * _rtB -> B_1036_1658_0 ; _rtB -> B_1036_1660_0 =
_rtB -> B_1036_1659_0 * _rtB -> B_1036_639_0_k ; _rtB -> B_1036_1661_0 = _rtB
-> B_1036_1660_0 * _rtB -> B_1036_1660_0 ; _rtB -> B_1036_1662_0 =
muDoubleScalarSin ( _rtB -> B_1036_275_0 [ 2 ] ) ; _rtB -> B_1036_1663_0 =
_rtB -> B_1036_1662_0 * _rtB -> B_1036_260_0 [ 2 ] ; _rtB -> B_1036_1664_0 =
_rtB -> B_1036_1663_0 * _rtB -> B_1036_639_0_k ; _rtB -> B_1036_1665_0 = _rtB
-> B_1036_1664_0 * _rtB -> B_1036_1664_0 ; _rtB -> B_1036_1666_0 = _rtB ->
B_1036_1661_0 + _rtB -> B_1036_1665_0 ; _rtB -> B_1036_1667_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1666_0 ) ; _rtB -> B_1036_1668_0 = _rtB
-> B_1036_1660_0 / _rtB -> B_1036_1667_0 ; _rtB -> B_1036_1669_0 = _rtB ->
B_1036_1657_0 - _rtB -> B_1036_1668_0 ; _rtB -> B_1036_1670_0 = _rtB ->
B_1036_1669_0 * _rtB -> B_1036_1669_0 ; _rtB -> B_1036_1671_0 = _rtB ->
B_1036_1629_0 / _rtB -> B_1036_1656_0 ; _rtB -> B_1036_1672_0 = _rtB ->
B_1036_1664_0 / _rtB -> B_1036_1667_0 ; _rtB -> B_1036_1673_0 = _rtB ->
B_1036_1671_0 - _rtB -> B_1036_1672_0 ; _rtB -> B_1036_1674_0 = _rtB ->
B_1036_1673_0 * _rtB -> B_1036_1673_0 ; _rtB -> B_1036_1675_0 = _rtB ->
B_1036_1670_0 + _rtB -> B_1036_1674_0 ; _rtB -> B_1036_1676_0 = _rtB ->
B_1036_1675_0 * _rtB -> B_1036_124_0_d ; _rtB -> B_1036_1677_0 = _rtB ->
B_1036_123_0_a - _rtB -> B_1036_1676_0 ; u0 = _rtB -> B_1036_1677_0 ; if ( u0
> 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB ->
B_1036_1678_0 = muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_1679_0 = _rtB ->
B_1036_1671_0 - _rtB -> B_1036_1668_0 ; _rtB -> B_1036_1680_0 = _rtB ->
B_1036_1679_0 * _rtB -> B_1036_1679_0 ; _rtB -> B_1036_1681_0 = _rtB ->
B_1036_1657_0 + _rtB -> B_1036_1672_0 ; _rtB -> B_1036_1682_0 = _rtB ->
B_1036_1681_0 * _rtB -> B_1036_1681_0 ; _rtB -> B_1036_1683_0 = _rtB ->
B_1036_1680_0 + _rtB -> B_1036_1682_0 ; _rtB -> B_1036_1684_0_k = ( _rtB ->
B_1036_1683_0 > _rtB -> B_1036_121_0_c ) ; if ( _rtB -> B_1036_1684_0_k ) {
_rtB -> B_58_0_0 = _rtB -> B_1036_1678_0 * _rtB -> B_1036_122_0 ; _rtB ->
B_1036_1686_0 = _rtB -> B_58_0_0 ; } else { _rtB -> B_1036_1686_0 = _rtB ->
B_1036_1678_0 ; } _rtB -> B_1036_1687_0 = muDoubleScalarAbs ( _rtB ->
B_1036_1686_0 ) ; _rtB -> B_1036_1688_0 = _rtB -> B_1036_1660_0 * _rtB ->
B_1036_1660_0 ; _rtB -> B_1036_1689_0 = _rtB -> B_1036_1664_0 * _rtB ->
B_1036_1664_0 ; _rtB -> B_1036_1690_0 = _rtB -> B_1036_1688_0 + _rtB ->
B_1036_1689_0 ; _rtB -> B_1036_1691_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1690_0 ) ; _rtB -> B_1036_1692_0 = _rtB -> B_1036_1660_0 / _rtB ->
B_1036_1691_0 ; _rtB -> B_1036_1693_0 = _rtB -> B_1036_1614_0 * _rtB ->
B_1036_1614_0 ; _rtB -> B_1036_1694_0 = _rtB -> B_1036_1618_0 * _rtB ->
B_1036_1618_0 ; _rtB -> B_1036_1695_0 = _rtB -> B_1036_1693_0 + _rtB ->
B_1036_1694_0 ; _rtB -> B_1036_1696_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1695_0 ) ; _rtB -> B_1036_1697_0 = _rtB -> B_1036_1614_0 / _rtB ->
B_1036_1696_0 ; _rtB -> B_1036_1698_0 = _rtB -> B_1036_1692_0 - _rtB ->
B_1036_1697_0 ; _rtB -> B_1036_1699_0 = _rtB -> B_1036_1698_0 * _rtB ->
B_1036_1698_0 ; _rtB -> B_1036_1700_0 = _rtB -> B_1036_1664_0 / _rtB ->
B_1036_1691_0 ; _rtB -> B_1036_1701_0 = _rtB -> B_1036_1618_0 / _rtB ->
B_1036_1696_0 ; _rtB -> B_1036_1702_0 = _rtB -> B_1036_1700_0 - _rtB ->
B_1036_1701_0 ; _rtB -> B_1036_1703_0 = _rtB -> B_1036_1702_0 * _rtB ->
B_1036_1702_0 ; _rtB -> B_1036_1704_0 = _rtB -> B_1036_1699_0 + _rtB ->
B_1036_1703_0 ; _rtB -> B_1036_1705_0 = _rtB -> B_1036_1704_0 * _rtB ->
B_1036_128_0 ; _rtB -> B_1036_1706_0 = _rtB -> B_1036_127_0_m - _rtB ->
B_1036_1705_0 ; u0 = _rtB -> B_1036_1706_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB -> B_1036_1707_0 =
muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_1708_0 = _rtB -> B_1036_1700_0 -
_rtB -> B_1036_1697_0 ; _rtB -> B_1036_1709_0 = _rtB -> B_1036_1708_0 * _rtB
-> B_1036_1708_0 ; _rtB -> B_1036_1710_0 = _rtB -> B_1036_1692_0 + _rtB ->
B_1036_1701_0 ; _rtB -> B_1036_1711_0 = _rtB -> B_1036_1710_0 * _rtB ->
B_1036_1710_0 ; _rtB -> B_1036_1712_0 = _rtB -> B_1036_1709_0 + _rtB ->
B_1036_1711_0 ; _rtB -> B_1036_1713_0_m = ( _rtB -> B_1036_1712_0 > _rtB ->
B_1036_125_0_a ) ; if ( _rtB -> B_1036_1713_0_m ) { _rtB -> B_59_0_0 = _rtB
-> B_1036_1707_0 * _rtB -> B_1036_126_0_p ; _rtB -> B_1036_1715_0 = _rtB ->
B_59_0_0 ; } else { _rtB -> B_1036_1715_0 = _rtB -> B_1036_1707_0 ; } _rtB ->
B_1036_1716_0 = muDoubleScalarAbs ( _rtB -> B_1036_1715_0 ) ; _rtB ->
B_1036_1717_0 = _rtP -> P_552 * _rtB -> B_1036_318_0 ; _rtB -> B_1036_1718_0
= _rtB -> B_1036_1717_0 * _rtB -> B_1036_1717_0 ; _rtB -> B_1036_1719_0 =
_rtP -> P_553 * _rtB -> B_1036_324_0 ; _rtB -> B_1036_1720_0 = _rtB ->
B_1036_1719_0 * _rtB -> B_1036_1719_0 ; _rtB -> B_1036_1721_0 = _rtB ->
B_1036_1718_0 + _rtB -> B_1036_1720_0 ; _rtB -> B_1036_1722_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1721_0 ) ; _rtB -> B_1036_1723_0 = _rtB
-> B_1036_1717_0 / _rtB -> B_1036_1722_0 ; _rtB -> B_1036_1724_0 = _rtP ->
P_554 * _rtB -> B_1036_401_0 ; _rtB -> B_1036_1725_0 = _rtB -> B_1036_1724_0
* _rtB -> B_1036_1724_0 ; _rtB -> B_1036_1726_0 = _rtP -> P_555 * _rtB ->
B_1036_421_0 ; _rtB -> B_1036_1727_0 = _rtB -> B_1036_1726_0 * _rtB ->
B_1036_1726_0 ; _rtB -> B_1036_1728_0 = _rtB -> B_1036_1725_0 + _rtB ->
B_1036_1727_0 ; _rtB -> B_1036_1729_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1728_0 ) ; _rtB -> B_1036_1730_0 = _rtB -> B_1036_1724_0 / _rtB ->
B_1036_1729_0 ; _rtB -> B_1036_1731_0 = _rtB -> B_1036_1723_0 - _rtB ->
B_1036_1730_0 ; _rtB -> B_1036_1732_0 = _rtB -> B_1036_1731_0 * _rtB ->
B_1036_1731_0 ; _rtB -> B_1036_1733_0 = _rtB -> B_1036_1719_0 / _rtB ->
B_1036_1722_0 ; _rtB -> B_1036_1734_0 = _rtB -> B_1036_1726_0 / _rtB ->
B_1036_1729_0 ; _rtB -> B_1036_1735_0 = _rtB -> B_1036_1733_0 - _rtB ->
B_1036_1734_0 ; _rtB -> B_1036_1736_0 = _rtB -> B_1036_1735_0 * _rtB ->
B_1036_1735_0 ; _rtB -> B_1036_1737_0 = _rtB -> B_1036_1732_0 + _rtB ->
B_1036_1736_0 ; _rtB -> B_1036_1738_0 = _rtB -> B_1036_1737_0 * _rtB ->
B_1036_108_0_j ; _rtB -> B_1036_1739_0 = _rtB -> B_1036_107_0_a - _rtB ->
B_1036_1738_0 ; u0 = _rtB -> B_1036_1739_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB -> B_1036_1740_0 =
muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_1741_0 = _rtB -> B_1036_1733_0 -
_rtB -> B_1036_1730_0 ; _rtB -> B_1036_1742_0 = _rtB -> B_1036_1741_0 * _rtB
-> B_1036_1741_0 ; _rtB -> B_1036_1743_0 = _rtB -> B_1036_1723_0 + _rtB ->
B_1036_1734_0 ; _rtB -> B_1036_1744_0 = _rtB -> B_1036_1743_0 * _rtB ->
B_1036_1743_0 ; _rtB -> B_1036_1745_0 = _rtB -> B_1036_1742_0 + _rtB ->
B_1036_1744_0 ; _rtB -> B_1036_1746_0_g = ( _rtB -> B_1036_1745_0 > _rtB ->
B_1036_105_0_p ) ; if ( _rtB -> B_1036_1746_0_g ) { _rtB -> B_54_0_0 = _rtB
-> B_1036_1740_0 * _rtB -> B_1036_106_0_p ; _rtB -> B_1036_1748_0 = _rtB ->
B_54_0_0 ; } else { _rtB -> B_1036_1748_0 = _rtB -> B_1036_1740_0 ; } _rtB ->
B_1036_1749_0 = muDoubleScalarAbs ( _rtB -> B_1036_1748_0 ) ; _rtB ->
B_1036_1750_0 = _rtB -> B_1036_1724_0 * _rtB -> B_1036_1724_0 ; _rtB ->
B_1036_1751_0 = _rtB -> B_1036_1726_0 * _rtB -> B_1036_1726_0 ; _rtB ->
B_1036_1752_0 = _rtB -> B_1036_1750_0 + _rtB -> B_1036_1751_0 ; _rtB ->
B_1036_1753_0 = muDoubleScalarSqrt ( _rtB -> B_1036_1752_0 ) ; _rtB ->
B_1036_1754_0 = _rtB -> B_1036_1724_0 / _rtB -> B_1036_1753_0 ; _rtB ->
B_1036_1755_0 = _rtP -> P_556 * _rtB -> B_1036_407_0 ; _rtB -> B_1036_1756_0
= _rtB -> B_1036_1755_0 * _rtB -> B_1036_1755_0 ; _rtB -> B_1036_1757_0 =
_rtP -> P_557 * _rtB -> B_1036_427_0 ; _rtB -> B_1036_1758_0 = _rtB ->
B_1036_1757_0 * _rtB -> B_1036_1757_0 ; _rtB -> B_1036_1759_0 = _rtB ->
B_1036_1756_0 + _rtB -> B_1036_1758_0 ; _rtB -> B_1036_1760_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1759_0 ) ; _rtB -> B_1036_1761_0 = _rtB
-> B_1036_1755_0 / _rtB -> B_1036_1760_0 ; _rtB -> B_1036_1762_0 = _rtB ->
B_1036_1754_0 - _rtB -> B_1036_1761_0 ; _rtB -> B_1036_1763_0 = _rtB ->
B_1036_1762_0 * _rtB -> B_1036_1762_0 ; _rtB -> B_1036_1764_0 = _rtB ->
B_1036_1726_0 / _rtB -> B_1036_1753_0 ; _rtB -> B_1036_1765_0 = _rtB ->
B_1036_1757_0 / _rtB -> B_1036_1760_0 ; _rtB -> B_1036_1766_0 = _rtB ->
B_1036_1764_0 - _rtB -> B_1036_1765_0 ; _rtB -> B_1036_1767_0 = _rtB ->
B_1036_1766_0 * _rtB -> B_1036_1766_0 ; _rtB -> B_1036_1768_0 = _rtB ->
B_1036_1763_0 + _rtB -> B_1036_1767_0 ; _rtB -> B_1036_1769_0 = _rtB ->
B_1036_1768_0 * _rtB -> B_1036_112_0 ; _rtB -> B_1036_1770_0 = _rtB ->
B_1036_111_0_o - _rtB -> B_1036_1769_0 ; u0 = _rtB -> B_1036_1770_0 ; if ( u0
> 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB ->
B_1036_1771_0 = muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_1772_0 = _rtB ->
B_1036_1764_0 - _rtB -> B_1036_1761_0 ; _rtB -> B_1036_1773_0 = _rtB ->
B_1036_1772_0 * _rtB -> B_1036_1772_0 ; _rtB -> B_1036_1774_0 = _rtB ->
B_1036_1754_0 + _rtB -> B_1036_1765_0 ; _rtB -> B_1036_1775_0 = _rtB ->
B_1036_1774_0 * _rtB -> B_1036_1774_0 ; _rtB -> B_1036_1776_0 = _rtB ->
B_1036_1773_0 + _rtB -> B_1036_1775_0 ; _rtB -> B_1036_1777_0_n = ( _rtB ->
B_1036_1776_0 > _rtB -> B_1036_109_0_e ) ; if ( _rtB -> B_1036_1777_0_n ) {
_rtB -> B_55_0_0 = _rtB -> B_1036_1771_0 * _rtB -> B_1036_110_0 ; _rtB ->
B_1036_1779_0 = _rtB -> B_55_0_0 ; } else { _rtB -> B_1036_1779_0 = _rtB ->
B_1036_1771_0 ; } _rtB -> B_1036_1780_0 = muDoubleScalarAbs ( _rtB ->
B_1036_1779_0 ) ; _rtB -> B_1036_1781_0 = _rtB -> B_1036_1755_0 * _rtB ->
B_1036_1755_0 ; _rtB -> B_1036_1782_0 = _rtB -> B_1036_1757_0 * _rtB ->
B_1036_1757_0 ; _rtB -> B_1036_1783_0 = _rtB -> B_1036_1781_0 + _rtB ->
B_1036_1782_0 ; _rtB -> B_1036_1784_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1783_0 ) ; _rtB -> B_1036_1785_0 = _rtB -> B_1036_1755_0 / _rtB ->
B_1036_1784_0 ; _rtB -> B_1036_1786_0 = _rtB -> B_1036_1717_0 * _rtB ->
B_1036_1717_0 ; _rtB -> B_1036_1787_0 = _rtB -> B_1036_1719_0 * _rtB ->
B_1036_1719_0 ; _rtB -> B_1036_1788_0 = _rtB -> B_1036_1786_0 + _rtB ->
B_1036_1787_0 ; _rtB -> B_1036_1789_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1788_0 ) ; _rtB -> B_1036_1790_0 = _rtB -> B_1036_1717_0 / _rtB ->
B_1036_1789_0 ; _rtB -> B_1036_1791_0 = _rtB -> B_1036_1785_0 - _rtB ->
B_1036_1790_0 ; _rtB -> B_1036_1792_0 = _rtB -> B_1036_1791_0 * _rtB ->
B_1036_1791_0 ; _rtB -> B_1036_1793_0 = _rtB -> B_1036_1757_0 / _rtB ->
B_1036_1784_0 ; _rtB -> B_1036_1794_0 = _rtB -> B_1036_1719_0 / _rtB ->
B_1036_1789_0 ; _rtB -> B_1036_1795_0 = _rtB -> B_1036_1793_0 - _rtB ->
B_1036_1794_0 ; _rtB -> B_1036_1796_0 = _rtB -> B_1036_1795_0 * _rtB ->
B_1036_1795_0 ; _rtB -> B_1036_1797_0 = _rtB -> B_1036_1792_0 + _rtB ->
B_1036_1796_0 ; _rtB -> B_1036_1798_0 = _rtB -> B_1036_1797_0 * _rtB ->
B_1036_116_0_e ; _rtB -> B_1036_1799_0 = _rtB -> B_1036_115_0_g - _rtB ->
B_1036_1798_0 ; u0 = _rtB -> B_1036_1799_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB -> B_1036_1800_0 =
muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_1801_0 = _rtB -> B_1036_1793_0 -
_rtB -> B_1036_1790_0 ; _rtB -> B_1036_1802_0 = _rtB -> B_1036_1801_0 * _rtB
-> B_1036_1801_0 ; _rtB -> B_1036_1803_0 = _rtB -> B_1036_1785_0 + _rtB ->
B_1036_1794_0 ; _rtB -> B_1036_1804_0 = _rtB -> B_1036_1803_0 * _rtB ->
B_1036_1803_0 ; _rtB -> B_1036_1805_0 = _rtB -> B_1036_1802_0 + _rtB ->
B_1036_1804_0 ; _rtB -> B_1036_1806_0_i = ( _rtB -> B_1036_1805_0 > _rtB ->
B_1036_113_0_b ) ; if ( _rtB -> B_1036_1806_0_i ) { _rtB -> B_56_0_0 = _rtB
-> B_1036_1800_0 * _rtB -> B_1036_114_0_a ; _rtB -> B_1036_1808_0 = _rtB ->
B_56_0_0 ; } else { _rtB -> B_1036_1808_0 = _rtB -> B_1036_1800_0 ; } _rtB ->
B_1036_1809_0 = muDoubleScalarAbs ( _rtB -> B_1036_1808_0 ) ; _rtB ->
B_1036_1810_0 = ! ( _rtB -> B_1036_448_0 != 0.0 ) ; _rtB -> B_1036_1811_0 =
_rtB -> B_1036_1613_0 * _rtB -> B_1036_1613_0 ; _rtB -> B_1036_1812_0 = _rtB
-> B_1036_1617_0 * _rtB -> B_1036_1617_0 ; _rtB -> B_1036_1813_0 = _rtB ->
B_1036_1811_0 + _rtB -> B_1036_1812_0 ; _rtB -> B_1036_1814_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1813_0 ) ; _rtB -> B_1036_1815_0_e = (
_rtB -> B_1036_1814_0 > _rtB -> B_1036_630_0_h ) ; _rtB -> B_1036_1816_0_e =
! _rtB -> B_1036_1815_0_e ; _rtB -> B_1036_1817_0_b = ( _rtB -> B_1036_1810_0
&& _rtB -> B_1036_1816_0_e ) ; _rtB -> B_1036_1818_0_i = ( _rtB ->
B_1036_1687_0 < _rtB -> B_1036_19_0_l ) ; _rtB -> B_1036_1819_0_k = ( _rtB ->
B_1036_1780_0 > _rtB -> B_1036_20_0 ) ; _rtB -> B_1036_1820_0_d = ( _rtB ->
B_1036_1818_0_i && _rtB -> B_1036_1819_0_k ) ; _rtB -> B_1036_1821_0_k = (
_rtB -> B_1036_1716_0 < _rtB -> B_1036_19_0_l ) ; _rtB -> B_1036_1822_0_a = (
_rtB -> B_1036_1809_0 > _rtB -> B_1036_20_0 ) ; _rtB -> B_1036_1823_0_i = (
_rtB -> B_1036_1821_0_k && _rtB -> B_1036_1822_0_a ) ; _rtB -> B_1036_1824_0
= _rtB -> B_1036_1625_0 * _rtB -> B_1036_1724_0 ; _rtB -> B_1036_1825_0 =
_rtB -> B_1036_1629_0 * _rtB -> B_1036_1726_0 ; _rtB -> B_1036_1826_0 = _rtB
-> B_1036_1824_0 + _rtB -> B_1036_1825_0 ; _rtB -> B_1036_1827_0 = _rtB ->
B_1036_1724_0 * _rtB -> B_1036_1724_0 ; _rtB -> B_1036_1828_0 = _rtB ->
B_1036_1726_0 * _rtB -> B_1036_1726_0 ; _rtB -> B_1036_1829_0 = _rtB ->
B_1036_1827_0 + _rtB -> B_1036_1828_0 ; _rtB -> B_1036_1830_0 = ( _rtB ->
B_1036_1829_0 < _rtB -> B_1036_84_0 ) ; if ( _rtB -> B_1036_1830_0 ) { _rtB
-> B_1036_1831_0 = _rtB -> B_1036_84_0 ; } else { _rtB -> B_1036_1831_0 =
_rtB -> B_1036_1829_0 ; } _rtB -> B_1036_1832_0 = _rtB -> B_1036_1826_0 /
_rtB -> B_1036_1831_0 ; _rtB -> B_1036_1833_0 = _rtB -> B_1036_1660_0 * _rtB
-> B_1036_1832_0 ; _rtB -> B_1036_1834_0 = _rtB -> B_1036_1629_0 * _rtB ->
B_1036_1724_0 ; _rtB -> B_1036_1835_0 = _rtB -> B_1036_1625_0 * _rtB ->
B_1036_1726_0 ; _rtB -> B_1036_1836_0 = _rtB -> B_1036_1834_0 - _rtB ->
B_1036_1835_0 ; _rtB -> B_1036_1837_0 = _rtB -> B_1036_1836_0 / _rtB ->
B_1036_1831_0 ; _rtB -> B_1036_1838_0 = _rtB -> B_1036_1664_0 * _rtB ->
B_1036_1837_0 ; _rtB -> B_1036_1839_0 = _rtB -> B_1036_1833_0 + _rtB ->
B_1036_1838_0 ; _rtB -> B_1036_1840_0 = _rtB -> B_1036_1832_0 * _rtB ->
B_1036_1832_0 ; _rtB -> B_1036_1841_0 = _rtB -> B_1036_1837_0 * _rtB ->
B_1036_1837_0 ; _rtB -> B_1036_1842_0 = _rtB -> B_1036_1840_0 + _rtB ->
B_1036_1841_0 ; _rtB -> B_1036_1843_0_b = ( _rtB -> B_1036_1842_0 < _rtB ->
B_1036_82_0_l ) ; if ( _rtB -> B_1036_1843_0_b ) { _rtB -> B_1036_1844_0 =
_rtB -> B_1036_82_0_l ; } else { _rtB -> B_1036_1844_0 = _rtB ->
B_1036_1842_0 ; } _rtB -> B_1036_1845_0 = _rtB -> B_1036_1839_0 / _rtB ->
B_1036_1844_0 ; _rtB -> B_1036_1846_0 = _rtB -> B_1036_1755_0 - _rtB ->
B_1036_1845_0 ; _rtB -> B_1036_1847_0 = _rtB -> B_1036_1846_0 * _rtB ->
B_1036_1846_0 ; _rtB -> B_1036_1848_0 = _rtB -> B_1036_1664_0 * _rtB ->
B_1036_1832_0 ; _rtB -> B_1036_1849_0 = _rtB -> B_1036_1660_0 * _rtB ->
B_1036_1837_0 ; _rtB -> B_1036_1850_0 = _rtB -> B_1036_1848_0 - _rtB ->
B_1036_1849_0 ; _rtB -> B_1036_1851_0 = _rtB -> B_1036_1850_0 / _rtB ->
B_1036_1844_0 ; _rtB -> B_1036_1852_0 = _rtB -> B_1036_1757_0 - _rtB ->
B_1036_1851_0 ; _rtB -> B_1036_1853_0 = _rtB -> B_1036_1852_0 * _rtB ->
B_1036_1852_0 ; _rtB -> B_1036_1854_0 = _rtB -> B_1036_1847_0 + _rtB ->
B_1036_1853_0 ; _rtB -> B_1036_1855_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1854_0 ) ; _rtB -> B_1036_1856_0 = _rtB -> B_1036_1845_0 * _rtB ->
B_1036_1845_0 ; _rtB -> B_1036_1857_0 = _rtB -> B_1036_1851_0 * _rtB ->
B_1036_1851_0 ; _rtB -> B_1036_1858_0 = _rtB -> B_1036_1856_0 + _rtB ->
B_1036_1857_0 ; _rtB -> B_1036_1859_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1858_0 ) ; _rtB -> B_1036_1860_0_e = ( _rtB -> B_1036_1855_0 > _rtB ->
B_1036_1859_0 ) ; _rtB -> B_1036_1861_0 = _rtB -> B_1036_1614_0 * _rtB ->
B_1036_1832_0 ; _rtB -> B_1036_1862_0 = _rtB -> B_1036_1618_0 * _rtB ->
B_1036_1837_0 ; _rtB -> B_1036_1863_0 = _rtB -> B_1036_1861_0 + _rtB ->
B_1036_1862_0 ; _rtB -> B_1036_1864_0 = _rtB -> B_1036_1832_0 * _rtB ->
B_1036_1832_0 ; _rtB -> B_1036_1865_0 = _rtB -> B_1036_1837_0 * _rtB ->
B_1036_1837_0 ; _rtB -> B_1036_1866_0 = _rtB -> B_1036_1864_0 + _rtB ->
B_1036_1865_0 ; _rtB -> B_1036_1867_0_l = ( _rtB -> B_1036_1866_0 < _rtB ->
B_1036_83_0_m ) ; if ( _rtB -> B_1036_1867_0_l ) { _rtB -> B_1036_1868_0 =
_rtB -> B_1036_83_0_m ; } else { _rtB -> B_1036_1868_0 = _rtB ->
B_1036_1866_0 ; } _rtB -> B_1036_1869_0 = _rtB -> B_1036_1863_0 / _rtB ->
B_1036_1868_0 ; _rtB -> B_1036_1870_0 = _rtB -> B_1036_1717_0 - _rtB ->
B_1036_1869_0 ; _rtB -> B_1036_1871_0 = _rtB -> B_1036_1870_0 * _rtB ->
B_1036_1870_0 ; _rtB -> B_1036_1872_0 = _rtB -> B_1036_1618_0 * _rtB ->
B_1036_1832_0 ; _rtB -> B_1036_1873_0 = _rtB -> B_1036_1614_0 * _rtB ->
B_1036_1837_0 ; _rtB -> B_1036_1874_0 = _rtB -> B_1036_1872_0 - _rtB ->
B_1036_1873_0 ; _rtB -> B_1036_1875_0 = _rtB -> B_1036_1874_0 / _rtB ->
B_1036_1868_0 ; _rtB -> B_1036_1876_0 = _rtB -> B_1036_1719_0 - _rtB ->
B_1036_1875_0 ; _rtB -> B_1036_1877_0 = _rtB -> B_1036_1876_0 * _rtB ->
B_1036_1876_0 ; _rtB -> B_1036_1878_0 = _rtB -> B_1036_1871_0 + _rtB ->
B_1036_1877_0 ; _rtB -> B_1036_1879_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1878_0 ) ; _rtB -> B_1036_1880_0 = _rtB -> B_1036_1869_0 * _rtB ->
B_1036_1869_0 ; _rtB -> B_1036_1881_0 = _rtB -> B_1036_1875_0 * _rtB ->
B_1036_1875_0 ; _rtB -> B_1036_1882_0 = _rtB -> B_1036_1880_0 + _rtB ->
B_1036_1881_0 ; _rtB -> B_1036_1883_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1882_0 ) ; _rtB -> B_1036_1884_0 = ( _rtB -> B_1036_1879_0 > _rtB ->
B_1036_1883_0 ) ; _rtB -> B_1036_1885_0 = ( _rtB -> B_1036_1860_0_e && _rtB
-> B_1036_1884_0 ) ; _rtB -> B_1036_1886_0 = ( _rtB -> B_1036_1885_0 && _rtB
-> B_1036_1823_0_i ) ; _rtB -> B_1036_1887_0 = _rtB -> B_1036_1614_0 * _rtB
-> B_1036_1717_0 ; _rtB -> B_1036_1888_0 = _rtB -> B_1036_1618_0 * _rtB ->
B_1036_1719_0 ; _rtB -> B_1036_1889_0 = _rtB -> B_1036_1887_0 + _rtB ->
B_1036_1888_0 ; _rtB -> B_1036_1890_0 = _rtB -> B_1036_1717_0 * _rtB ->
B_1036_1717_0 ; _rtB -> B_1036_1891_0 = _rtB -> B_1036_1719_0 * _rtB ->
B_1036_1719_0 ; _rtB -> B_1036_1892_0 = _rtB -> B_1036_1890_0 + _rtB ->
B_1036_1891_0 ; _rtB -> B_1036_1893_0 = ( _rtB -> B_1036_1892_0 < _rtB ->
B_1036_73_0_i ) ; if ( _rtB -> B_1036_1893_0 ) { _rtB -> B_1036_1894_0 = _rtB
-> B_1036_73_0_i ; } else { _rtB -> B_1036_1894_0 = _rtB -> B_1036_1892_0 ; }
_rtB -> B_1036_1895_0 = _rtB -> B_1036_1889_0 / _rtB -> B_1036_1894_0 ; _rtB
-> B_1036_1896_0 = _rtB -> B_1036_1625_0 * _rtB -> B_1036_1895_0 ; _rtB ->
B_1036_1897_0 = _rtB -> B_1036_1618_0 * _rtB -> B_1036_1717_0 ; _rtB ->
B_1036_1898_0 = _rtB -> B_1036_1614_0 * _rtB -> B_1036_1719_0 ; _rtB ->
B_1036_1899_0 = _rtB -> B_1036_1897_0 - _rtB -> B_1036_1898_0 ; _rtB ->
B_1036_1900_0 = _rtB -> B_1036_1899_0 / _rtB -> B_1036_1894_0 ; _rtB ->
B_1036_1901_0 = _rtB -> B_1036_1629_0 * _rtB -> B_1036_1900_0 ; _rtB ->
B_1036_1902_0 = _rtB -> B_1036_1896_0 + _rtB -> B_1036_1901_0 ; _rtB ->
B_1036_1903_0 = _rtB -> B_1036_1895_0 * _rtB -> B_1036_1895_0 ; _rtB ->
B_1036_1904_0 = _rtB -> B_1036_1900_0 * _rtB -> B_1036_1900_0 ; _rtB ->
B_1036_1905_0 = _rtB -> B_1036_1903_0 + _rtB -> B_1036_1904_0 ; _rtB ->
B_1036_1906_0 = ( _rtB -> B_1036_1905_0 < _rtB -> B_1036_71_0_o ) ; if ( _rtB
-> B_1036_1906_0 ) { _rtB -> B_1036_1907_0 = _rtB -> B_1036_71_0_o ; } else {
_rtB -> B_1036_1907_0 = _rtB -> B_1036_1905_0 ; } _rtB -> B_1036_1908_0 =
_rtB -> B_1036_1902_0 / _rtB -> B_1036_1907_0 ; _rtB -> B_1036_1909_0 = _rtB
-> B_1036_1724_0 - _rtB -> B_1036_1908_0 ; _rtB -> B_1036_1910_0 = _rtB ->
B_1036_1909_0 * _rtB -> B_1036_1909_0 ; _rtB -> B_1036_1911_0 = _rtB ->
B_1036_1629_0 * _rtB -> B_1036_1895_0 ; _rtB -> B_1036_1912_0 = _rtB ->
B_1036_1625_0 * _rtB -> B_1036_1900_0 ; _rtB -> B_1036_1913_0 = _rtB ->
B_1036_1911_0 - _rtB -> B_1036_1912_0 ; _rtB -> B_1036_1914_0 = _rtB ->
B_1036_1913_0 / _rtB -> B_1036_1907_0 ; _rtB -> B_1036_1915_0 = _rtB ->
B_1036_1726_0 - _rtB -> B_1036_1914_0 ; _rtB -> B_1036_1916_0 = _rtB ->
B_1036_1915_0 * _rtB -> B_1036_1915_0 ; _rtB -> B_1036_1917_0 = _rtB ->
B_1036_1910_0 + _rtB -> B_1036_1916_0 ; _rtB -> B_1036_1918_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1917_0 ) ; _rtB -> B_1036_1919_0 = _rtB
-> B_1036_1908_0 * _rtB -> B_1036_1908_0 ; _rtB -> B_1036_1920_0 = _rtB ->
B_1036_1914_0 * _rtB -> B_1036_1914_0 ; _rtB -> B_1036_1921_0 = _rtB ->
B_1036_1919_0 + _rtB -> B_1036_1920_0 ; _rtB -> B_1036_1922_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1921_0 ) ; _rtB -> B_1036_1923_0_i = (
_rtB -> B_1036_1918_0 > _rtB -> B_1036_1922_0 ) ; _rtB -> B_1036_1924_0 =
_rtB -> B_1036_1660_0 * _rtB -> B_1036_1895_0 ; _rtB -> B_1036_1925_0 = _rtB
-> B_1036_1664_0 * _rtB -> B_1036_1900_0 ; _rtB -> B_1036_1926_0 = _rtB ->
B_1036_1924_0 + _rtB -> B_1036_1925_0 ; _rtB -> B_1036_1927_0 = _rtB ->
B_1036_1895_0 * _rtB -> B_1036_1895_0 ; _rtB -> B_1036_1928_0 = _rtB ->
B_1036_1900_0 * _rtB -> B_1036_1900_0 ; _rtB -> B_1036_1929_0 = _rtB ->
B_1036_1927_0 + _rtB -> B_1036_1928_0 ; _rtB -> B_1036_1930_0_b = ( _rtB ->
B_1036_1929_0 < _rtB -> B_1036_72_0_o ) ; if ( _rtB -> B_1036_1930_0_b ) {
_rtB -> B_1036_1931_0 = _rtB -> B_1036_72_0_o ; } else { _rtB ->
B_1036_1931_0 = _rtB -> B_1036_1929_0 ; } _rtB -> B_1036_1932_0 = _rtB ->
B_1036_1926_0 / _rtB -> B_1036_1931_0 ; _rtB -> B_1036_1933_0 = _rtB ->
B_1036_1755_0 - _rtB -> B_1036_1932_0 ; _rtB -> B_1036_1934_0 = _rtB ->
B_1036_1933_0 * _rtB -> B_1036_1933_0 ; _rtB -> B_1036_1935_0 = _rtB ->
B_1036_1664_0 * _rtB -> B_1036_1895_0 ; _rtB -> B_1036_1936_0 = _rtB ->
B_1036_1660_0 * _rtB -> B_1036_1900_0 ; _rtB -> B_1036_1937_0 = _rtB ->
B_1036_1935_0 - _rtB -> B_1036_1936_0 ; _rtB -> B_1036_1938_0 = _rtB ->
B_1036_1937_0 / _rtB -> B_1036_1931_0 ; _rtB -> B_1036_1939_0 = _rtB ->
B_1036_1757_0 - _rtB -> B_1036_1938_0 ; _rtB -> B_1036_1940_0 = _rtB ->
B_1036_1939_0 * _rtB -> B_1036_1939_0 ; _rtB -> B_1036_1941_0 = _rtB ->
B_1036_1934_0 + _rtB -> B_1036_1940_0 ; _rtB -> B_1036_1942_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1941_0 ) ; _rtB -> B_1036_1943_0 = _rtB
-> B_1036_1932_0 * _rtB -> B_1036_1932_0 ; _rtB -> B_1036_1944_0 = _rtB ->
B_1036_1938_0 * _rtB -> B_1036_1938_0 ; _rtB -> B_1036_1945_0 = _rtB ->
B_1036_1943_0 + _rtB -> B_1036_1944_0 ; _rtB -> B_1036_1946_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1945_0 ) ; _rtB -> B_1036_1947_0 = ( _rtB
-> B_1036_1942_0 > _rtB -> B_1036_1946_0 ) ; _rtB -> B_1036_1948_0 = ( _rtB
-> B_1036_1923_0_i && _rtB -> B_1036_1947_0 ) ; _rtB -> B_1036_1949_0 = (
_rtB -> B_1036_1948_0 && _rtB -> B_1036_1820_0_d ) ; _rtB -> B_1036_1950_0 =
_rtB -> B_1036_1660_0 * _rtB -> B_1036_1755_0 ; _rtB -> B_1036_1951_0 = _rtB
-> B_1036_1664_0 * _rtB -> B_1036_1757_0 ; _rtB -> B_1036_1952_0 = _rtB ->
B_1036_1950_0 + _rtB -> B_1036_1951_0 ; _rtB -> B_1036_1953_0 = _rtB ->
B_1036_1755_0 * _rtB -> B_1036_1755_0 ; _rtB -> B_1036_1954_0 = _rtB ->
B_1036_1757_0 * _rtB -> B_1036_1757_0 ; _rtB -> B_1036_1955_0 = _rtB ->
B_1036_1953_0 + _rtB -> B_1036_1954_0 ; _rtB -> B_1036_1956_0_e = ( _rtB ->
B_1036_1955_0 < _rtB -> B_1036_62_0_p ) ; if ( _rtB -> B_1036_1956_0_e ) {
_rtB -> B_1036_1957_0 = _rtB -> B_1036_62_0_p ; } else { _rtB ->
B_1036_1957_0 = _rtB -> B_1036_1955_0 ; } _rtB -> B_1036_1958_0 = _rtB ->
B_1036_1952_0 / _rtB -> B_1036_1957_0 ; _rtB -> B_1036_1959_0 = _rtB ->
B_1036_1614_0 * _rtB -> B_1036_1958_0 ; _rtB -> B_1036_1960_0 = _rtB ->
B_1036_1664_0 * _rtB -> B_1036_1755_0 ; _rtB -> B_1036_1961_0 = _rtB ->
B_1036_1660_0 * _rtB -> B_1036_1757_0 ; _rtB -> B_1036_1962_0 = _rtB ->
B_1036_1960_0 - _rtB -> B_1036_1961_0 ; _rtB -> B_1036_1963_0 = _rtB ->
B_1036_1962_0 / _rtB -> B_1036_1957_0 ; _rtB -> B_1036_1964_0 = _rtB ->
B_1036_1618_0 * _rtB -> B_1036_1963_0 ; _rtB -> B_1036_1965_0 = _rtB ->
B_1036_1959_0 + _rtB -> B_1036_1964_0 ; _rtB -> B_1036_1966_0 = _rtB ->
B_1036_1958_0 * _rtB -> B_1036_1958_0 ; _rtB -> B_1036_1967_0 = _rtB ->
B_1036_1963_0 * _rtB -> B_1036_1963_0 ; _rtB -> B_1036_1968_0 = _rtB ->
B_1036_1966_0 + _rtB -> B_1036_1967_0 ; _rtB -> B_1036_1969_0_b = ( _rtB ->
B_1036_1968_0 < _rtB -> B_1036_60_0_c ) ; if ( _rtB -> B_1036_1969_0_b ) {
_rtB -> B_1036_1970_0 = _rtB -> B_1036_60_0_c ; } else { _rtB ->
B_1036_1970_0 = _rtB -> B_1036_1968_0 ; } _rtB -> B_1036_1971_0 = _rtB ->
B_1036_1965_0 / _rtB -> B_1036_1970_0 ; _rtB -> B_1036_1972_0 = _rtB ->
B_1036_1717_0 - _rtB -> B_1036_1971_0 ; _rtB -> B_1036_1973_0 = _rtB ->
B_1036_1972_0 * _rtB -> B_1036_1972_0 ; _rtB -> B_1036_1974_0 = _rtB ->
B_1036_1618_0 * _rtB -> B_1036_1958_0 ; _rtB -> B_1036_1975_0 = _rtB ->
B_1036_1614_0 * _rtB -> B_1036_1963_0 ; _rtB -> B_1036_1976_0 = _rtB ->
B_1036_1974_0 - _rtB -> B_1036_1975_0 ; _rtB -> B_1036_1977_0 = _rtB ->
B_1036_1976_0 / _rtB -> B_1036_1970_0 ; _rtB -> B_1036_1978_0 = _rtB ->
B_1036_1719_0 - _rtB -> B_1036_1977_0 ; _rtB -> B_1036_1979_0 = _rtB ->
B_1036_1978_0 * _rtB -> B_1036_1978_0 ; _rtB -> B_1036_1980_0 = _rtB ->
B_1036_1973_0 + _rtB -> B_1036_1979_0 ; _rtB -> B_1036_1981_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1980_0 ) ; _rtB -> B_1036_1982_0 = _rtB
-> B_1036_1971_0 * _rtB -> B_1036_1971_0 ; _rtB -> B_1036_1983_0 = _rtB ->
B_1036_1977_0 * _rtB -> B_1036_1977_0 ; _rtB -> B_1036_1984_0 = _rtB ->
B_1036_1982_0 + _rtB -> B_1036_1983_0 ; _rtB -> B_1036_1985_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1984_0 ) ; _rtB -> B_1036_1986_0_i = (
_rtB -> B_1036_1981_0 > _rtB -> B_1036_1985_0 ) ; _rtB -> B_1036_1987_0 =
_rtB -> B_1036_1625_0 * _rtB -> B_1036_1958_0 ; _rtB -> B_1036_1988_0 = _rtB
-> B_1036_1629_0 * _rtB -> B_1036_1963_0 ; _rtB -> B_1036_1989_0 = _rtB ->
B_1036_1987_0 + _rtB -> B_1036_1988_0 ; _rtB -> B_1036_1990_0 = _rtB ->
B_1036_1958_0 * _rtB -> B_1036_1958_0 ; _rtB -> B_1036_1991_0 = _rtB ->
B_1036_1963_0 * _rtB -> B_1036_1963_0 ; _rtB -> B_1036_1992_0 = _rtB ->
B_1036_1990_0 + _rtB -> B_1036_1991_0 ; _rtB -> B_1036_1993_0 = ( _rtB ->
B_1036_1992_0 < _rtB -> B_1036_61_0_p ) ; if ( _rtB -> B_1036_1993_0 ) { _rtB
-> B_1036_1994_0 = _rtB -> B_1036_61_0_p ; } else { _rtB -> B_1036_1994_0 =
_rtB -> B_1036_1992_0 ; } _rtB -> B_1036_1995_0 = _rtB -> B_1036_1989_0 /
_rtB -> B_1036_1994_0 ; _rtB -> B_1036_1996_0 = _rtB -> B_1036_1724_0 - _rtB
-> B_1036_1995_0 ; _rtB -> B_1036_1997_0 = _rtB -> B_1036_1996_0 * _rtB ->
B_1036_1996_0 ; _rtB -> B_1036_1998_0 = _rtB -> B_1036_1629_0 * _rtB ->
B_1036_1958_0 ; _rtB -> B_1036_1999_0 = _rtB -> B_1036_1625_0 * _rtB ->
B_1036_1963_0 ; _rtB -> B_1036_2000_0 = _rtB -> B_1036_1998_0 - _rtB ->
B_1036_1999_0 ; _rtB -> B_1036_2001_0 = _rtB -> B_1036_2000_0 / _rtB ->
B_1036_1994_0 ; _rtB -> B_1036_2002_0 = _rtB -> B_1036_1726_0 - _rtB ->
B_1036_2001_0 ; _rtB -> B_1036_2003_0 = _rtB -> B_1036_2002_0 * _rtB ->
B_1036_2002_0 ; _rtB -> B_1036_2004_0 = _rtB -> B_1036_1997_0 + _rtB ->
B_1036_2003_0 ; _rtB -> B_1036_2005_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_2004_0 ) ; _rtB -> B_1036_2006_0 = _rtB -> B_1036_1995_0 * _rtB ->
B_1036_1995_0 ; _rtB -> B_1036_2007_0 = _rtB -> B_1036_2001_0 * _rtB ->
B_1036_2001_0 ; _rtB -> B_1036_2008_0 = _rtB -> B_1036_2006_0 + _rtB ->
B_1036_2007_0 ; _rtB -> B_1036_2009_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_2008_0 ) ; _rtB -> B_1036_2010_0_m = ( _rtB -> B_1036_2005_0 > _rtB ->
B_1036_2009_0 ) ; _rtB -> B_1036_2011_0_l = ( _rtB -> B_1036_1986_0_i && _rtB
-> B_1036_2010_0_m ) ; _rtB -> B_1036_2012_0_o = ( _rtB -> B_1036_1652_0 <
_rtB -> B_1036_19_0_l ) ; _rtB -> B_1036_2013_0_n = ( _rtB -> B_1036_1749_0 >
_rtB -> B_1036_20_0 ) ; _rtB -> B_1036_2014_0_i = ( _rtB -> B_1036_2012_0_o
&& _rtB -> B_1036_2013_0_n ) ; _rtB -> B_1036_2015_0_o = ( _rtB ->
B_1036_2011_0_l && _rtB -> B_1036_2014_0_i ) ; _rtB -> B_1036_2016_0_m = ! (
_rtB -> B_1036_962_0 != 0.0 ) ; _rtB -> B_1036_2017_0 = _rtB -> B_1036_1624_0
* _rtB -> B_1036_1624_0 ; _rtB -> B_1036_2018_0 = _rtB -> B_1036_1628_0 *
_rtB -> B_1036_1628_0 ; _rtB -> B_1036_2019_0 = _rtB -> B_1036_2017_0 + _rtB
-> B_1036_2018_0 ; _rtB -> B_1036_2020_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_2019_0 ) ; _rtB -> B_1036_2021_0_a = ( _rtB -> B_1036_2020_0 > _rtB ->
B_1036_630_0_h ) ; _rtB -> B_1036_2022_0_c = ! _rtB -> B_1036_2021_0_a ; _rtB
-> B_1036_2023_0_d = ( _rtB -> B_1036_2016_0_m && _rtB -> B_1036_2022_0_c ) ;
_rtB -> B_1036_2024_0_g = ! ( _rtB -> B_1036_1035_0 != 0.0 ) ; _rtB ->
B_1036_2025_0 = _rtB -> B_1036_1659_0 * _rtB -> B_1036_1659_0 ; _rtB ->
B_1036_2026_0 = _rtB -> B_1036_1663_0 * _rtB -> B_1036_1663_0 ; _rtB ->
B_1036_2027_0 = _rtB -> B_1036_2025_0 + _rtB -> B_1036_2026_0 ; _rtB ->
B_1036_2028_0 = muDoubleScalarSqrt ( _rtB -> B_1036_2027_0 ) ; _rtB ->
B_1036_2029_0_h = ( _rtB -> B_1036_2028_0 > _rtB -> B_1036_630_0_h ) ; _rtB
-> B_1036_2030_0_b = ! _rtB -> B_1036_2029_0_h ; _rtB -> B_1036_2031_0_d = (
_rtB -> B_1036_2024_0_g && _rtB -> B_1036_2030_0_b ) ; _rtB -> B_1036_2032_0
= _rtB -> B_1036_1614_0 * _rtB -> B_1036_1614_0 ; _rtB -> B_1036_2033_0 =
_rtB -> B_1036_1618_0 * _rtB -> B_1036_1618_0 ; _rtB -> B_1036_2034_0 = _rtB
-> B_1036_2032_0 + _rtB -> B_1036_2033_0 ; _rtB -> B_1036_2035_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_2034_0 ) ; _rtB -> B_1036_2036_0 = _rtB
-> B_1036_1625_0 * _rtB -> B_1036_1625_0 ; _rtB -> B_1036_2037_0 = _rtB ->
B_1036_1629_0 * _rtB -> B_1036_1629_0 ; _rtB -> B_1036_2038_0 = _rtB ->
B_1036_2036_0 + _rtB -> B_1036_2037_0 ; _rtB -> B_1036_2039_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_2038_0 ) ; _rtB -> B_1036_2040_0 = _rtB
-> B_1036_1660_0 * _rtB -> B_1036_1660_0 ; _rtB -> B_1036_2041_0 = _rtB ->
B_1036_1664_0 * _rtB -> B_1036_1664_0 ; _rtB -> B_1036_2042_0 = _rtB ->
B_1036_2040_0 + _rtB -> B_1036_2041_0 ; _rtB -> B_1036_2043_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_2042_0 ) ; _rtB -> B_1036_2044_0_c = (
_rtB -> B_1036_2035_0 < _rtB -> B_1036_14_0 ) ; _rtB -> B_1036_2045_0_h = (
_rtB -> B_1036_2039_0 < _rtB -> B_1036_14_0 ) ; _rtB -> B_1036_2046_0_f = (
_rtB -> B_1036_2043_0 < _rtB -> B_1036_14_0 ) ; _rtB -> B_1036_2047_0 = _rtB
-> B_1036_1719_0 * _rtB -> B_1036_21_0_d ; _rtB -> B_1036_2048_0 = _rtB ->
B_1036_1614_0 * _rtB -> B_1036_1717_0 ; _rtB -> B_1036_2049_0 = _rtB ->
B_1036_1618_0 * _rtB -> B_1036_2047_0 ; _rtB -> B_1036_2050_0 = _rtB ->
B_1036_2048_0 - _rtB -> B_1036_2049_0 ; _rtB -> B_1036_2051_0 = _rtB ->
B_1036_1618_0 * _rtB -> B_1036_1717_0 ; _rtB -> B_1036_2052_0 = _rtB ->
B_1036_1614_0 * _rtB -> B_1036_2047_0 ; _rtB -> B_1036_2053_0 = _rtB ->
B_1036_2051_0 + _rtB -> B_1036_2052_0 ; _rtB -> B_1036_2054_0 = _rtB ->
B_1036_2053_0 * _rtB -> B_1036_26_0_b ; _rtB -> B_1036_2055_0 = _rtB ->
B_1036_2050_0 * _rtB -> B_1036_28_0_b ; _rtB -> B_1036_2056_0 = _rtB ->
B_1036_2054_0 + _rtB -> B_1036_2055_0 ; _rtB -> B_1036_2057_0 = _rtB ->
B_1036_1726_0 * _rtB -> B_1036_30_0 ; _rtB -> B_1036_2058_0 = _rtB ->
B_1036_1625_0 * _rtB -> B_1036_1724_0 ; _rtB -> B_1036_2059_0 = _rtB ->
B_1036_1629_0 * _rtB -> B_1036_2057_0 ; _rtB -> B_1036_2060_0 = _rtB ->
B_1036_2058_0 - _rtB -> B_1036_2059_0 ; _rtB -> B_1036_2061_0 = _rtB ->
B_1036_1629_0 * _rtB -> B_1036_1724_0 ; _rtB -> B_1036_2062_0 = _rtB ->
B_1036_1625_0 * _rtB -> B_1036_2057_0 ; _rtB -> B_1036_2063_0 = _rtB ->
B_1036_2061_0 + _rtB -> B_1036_2062_0 ; _rtB -> B_1036_2064_0 = _rtB ->
B_1036_2063_0 * _rtB -> B_1036_35_0_e ; _rtB -> B_1036_2065_0 = _rtB ->
B_1036_2060_0 * _rtB -> B_1036_37_0_j ; _rtB -> B_1036_2066_0 = _rtB ->
B_1036_2064_0 + _rtB -> B_1036_2065_0 ; _rtB -> B_1036_2067_0 = _rtB ->
B_1036_1757_0 * _rtB -> B_1036_39_0_f ; _rtB -> B_1036_2068_0 = _rtB ->
B_1036_1660_0 * _rtB -> B_1036_1755_0 ; _rtB -> B_1036_2069_0 = _rtB ->
B_1036_1664_0 * _rtB -> B_1036_2067_0 ; _rtB -> B_1036_2070_0 = _rtB ->
B_1036_2068_0 - _rtB -> B_1036_2069_0 ; _rtB -> B_1036_2071_0 = _rtB ->
B_1036_1664_0 * _rtB -> B_1036_1755_0 ; _rtB -> B_1036_2072_0 = _rtB ->
B_1036_1660_0 * _rtB -> B_1036_2067_0 ; _rtB -> B_1036_2073_0 = _rtB ->
B_1036_2071_0 + _rtB -> B_1036_2072_0 ; _rtB -> B_1036_2074_0 = _rtB ->
B_1036_2073_0 * _rtB -> B_1036_44_0_o ; _rtB -> B_1036_2075_0 = _rtB ->
B_1036_2070_0 * _rtB -> B_1036_46_0_i ; _rtB -> B_1036_2076_0 = _rtB ->
B_1036_2074_0 + _rtB -> B_1036_2075_0 ; _rtB -> B_1036_2077_0 = _rtB ->
B_1036_1660_0 * _rtB -> B_1036_1846_0 ; _rtB -> B_1036_2078_0 = _rtB ->
B_1036_1852_0 * _rtB -> B_1036_101_0_k ; _rtB -> B_1036_2079_0 = _rtB ->
B_1036_1664_0 * _rtB -> B_1036_2078_0 ; _rtB -> B_1036_2080_0 = _rtB ->
B_1036_2077_0 - _rtB -> B_1036_2079_0 ; _rtB -> B_1036_2081_0_n = ( _rtB ->
B_1036_2080_0 > _rtB -> B_1036_103_0_p ) ; _rtB -> B_1036_2082_0 = _rtB ->
B_1036_2080_0 * _rtB -> B_1036_2080_0 ; _rtB -> B_1036_2083_0 = _rtB ->
B_1036_1664_0 * _rtB -> B_1036_1846_0 ; _rtB -> B_1036_2084_0 = _rtB ->
B_1036_1660_0 * _rtB -> B_1036_2078_0 ; _rtB -> B_1036_2085_0 = _rtB ->
B_1036_2083_0 + _rtB -> B_1036_2084_0 ; _rtB -> B_1036_2086_0 = _rtB ->
B_1036_2085_0 * _rtB -> B_1036_102_0 ; _rtB -> B_1036_2087_0 = _rtB ->
B_1036_2085_0 * _rtB -> B_1036_2086_0 ; _rtB -> B_1036_2088_0 = _rtB ->
B_1036_2082_0 + _rtB -> B_1036_2087_0 ; _rtB -> B_1036_2089_0_d = ( _rtB ->
B_1036_2088_0 > _rtB -> B_1036_104_0 ) ; if ( _rtB -> B_1036_18_0_g > _rtP ->
P_558 ) { _rtB -> B_1036_2090_0 = _rtB -> B_1036_2089_0_d ; } else { _rtB ->
B_1036_2090_0 = _rtB -> B_1036_11_0_n ; } _rtB -> B_1036_2091_0_e = ( _rtB ->
B_1036_2081_0_n && ( _rtB -> B_1036_2090_0 != 0.0 ) ) ; _rtB ->
B_1036_2092_0_e = ( _rtB -> B_1036_2085_0 > _rtB -> B_1036_103_0_p ) ; if (
_rtB -> B_1036_18_0_g > _rtP -> P_559 ) { _rtB -> B_53_0_0 = ! _rtB ->
B_1036_2089_0_d ; _rtB -> B_1036_2094_0 = _rtB -> B_53_0_0 ; } else { _rtB ->
B_1036_2094_0 = _rtB -> B_1036_18_0_g ; } _rtB -> B_1036_2095_0_p = ( _rtB ->
B_1036_2092_0_e && ( _rtB -> B_1036_2094_0 != 0.0 ) ) ; _rtB ->
B_1036_2096_0_n = ( _rtB -> B_1036_2091_0_e || _rtB -> B_1036_2095_0_p ) ;
_rtB -> B_1036_2097_0 = _rtB -> B_1036_1660_0 * _rtB -> B_1036_1755_0 ; _rtB
-> B_1036_2098_0 = _rtB -> B_1036_1757_0 * _rtB -> B_1036_56_0 ; _rtB ->
B_1036_2099_0 = _rtB -> B_1036_1664_0 * _rtB -> B_1036_2098_0 ; _rtB ->
B_1036_2100_0 = _rtB -> B_1036_2097_0 - _rtB -> B_1036_2099_0 ; _rtB ->
B_1036_2101_0_f = ( _rtB -> B_1036_2100_0 > _rtB -> B_1036_58_0 ) ; _rtB ->
B_1036_2102_0 = _rtB -> B_1036_2100_0 * _rtB -> B_1036_2100_0 ; _rtB ->
B_1036_2103_0 = _rtB -> B_1036_1664_0 * _rtB -> B_1036_1755_0 ; _rtB ->
B_1036_2104_0 = _rtB -> B_1036_1660_0 * _rtB -> B_1036_2098_0 ; _rtB ->
B_1036_2105_0 = _rtB -> B_1036_2103_0 + _rtB -> B_1036_2104_0 ; _rtB ->
B_1036_2106_0 = _rtB -> B_1036_2105_0 * _rtB -> B_1036_57_0_h ; _rtB ->
B_1036_2107_0 = _rtB -> B_1036_2105_0 * _rtB -> B_1036_2106_0 ; _rtB ->
B_1036_2108_0 = _rtB -> B_1036_2102_0 + _rtB -> B_1036_2107_0 ; _rtB ->
B_1036_2109_0_e = ( _rtB -> B_1036_2108_0 > _rtB -> B_1036_59_0_c ) ; if (
_rtB -> B_1036_2046_0_f ) { _rtB -> B_1036_2110_0 = _rtB -> B_1036_2109_0_e ;
_rtB -> B_38_0_0 = ! _rtB -> B_1036_2109_0_e ; _rtB -> B_1036_2114_0 = _rtB
-> B_38_0_0 ; } else { _rtB -> B_1036_2110_0 = _rtB -> B_1036_15_0_l ; _rtB
-> B_1036_2114_0 = _rtB -> B_1036_16_0_j ; } _rtB -> B_1036_2111_0_k = ( _rtB
-> B_1036_2101_0_f && ( _rtB -> B_1036_2110_0 != 0.0 ) ) ; _rtB ->
B_1036_2112_0_f = ( _rtB -> B_1036_2105_0 > _rtB -> B_1036_58_0 ) ; _rtB ->
B_1036_2115_0_k = ( _rtB -> B_1036_2112_0_f && ( _rtB -> B_1036_2114_0 != 0.0
) ) ; _rtB -> B_1036_2116_0_p = ( _rtB -> B_1036_2111_0_k || _rtB ->
B_1036_2115_0_k ) ; _rtB -> B_1036_2117_0 = _rtB -> B_1036_1625_0 * _rtB ->
B_1036_1724_0 ; _rtB -> B_1036_2118_0 = _rtB -> B_1036_1726_0 * _rtB ->
B_1036_52_0_c ; _rtB -> B_1036_2119_0 = _rtB -> B_1036_1629_0 * _rtB ->
B_1036_2118_0 ; _rtB -> B_1036_2120_0 = _rtB -> B_1036_2117_0 - _rtB ->
B_1036_2119_0 ; _rtB -> B_1036_2121_0_h = ( _rtB -> B_1036_2120_0 > _rtB ->
B_1036_54_0_m ) ; _rtB -> B_1036_2122_0 = _rtB -> B_1036_2120_0 * _rtB ->
B_1036_2120_0 ; _rtB -> B_1036_2123_0 = _rtB -> B_1036_1629_0 * _rtB ->
B_1036_1724_0 ; _rtB -> B_1036_2124_0 = _rtB -> B_1036_1625_0 * _rtB ->
B_1036_2118_0 ; _rtB -> B_1036_2125_0 = _rtB -> B_1036_2123_0 + _rtB ->
B_1036_2124_0 ; _rtB -> B_1036_2126_0 = _rtB -> B_1036_2125_0 * _rtB ->
B_1036_53_0_m ; _rtB -> B_1036_2127_0 = _rtB -> B_1036_2125_0 * _rtB ->
B_1036_2126_0 ; _rtB -> B_1036_2128_0 = _rtB -> B_1036_2122_0 + _rtB ->
B_1036_2127_0 ; _rtB -> B_1036_2129_0_d = ( _rtB -> B_1036_2128_0 > _rtB ->
B_1036_55_0_j ) ; if ( _rtB -> B_1036_2045_0_h ) { _rtB -> B_1036_2130_0 =
_rtB -> B_1036_2129_0_d ; _rtB -> B_37_0_0 = ! _rtB -> B_1036_2129_0_d ; _rtB
-> B_1036_2134_0 = _rtB -> B_37_0_0 ; } else { _rtB -> B_1036_2130_0 = _rtB
-> B_1036_15_0_l ; _rtB -> B_1036_2134_0 = _rtB -> B_1036_16_0_j ; } _rtB ->
B_1036_2131_0_d = ( _rtB -> B_1036_2121_0_h && ( _rtB -> B_1036_2130_0 != 0.0
) ) ; _rtB -> B_1036_2132_0_d = ( _rtB -> B_1036_2125_0 > _rtB ->
B_1036_54_0_m ) ; _rtB -> B_1036_2135_0_j = ( _rtB -> B_1036_2132_0_d && (
_rtB -> B_1036_2134_0 != 0.0 ) ) ; _rtB -> B_1036_2136_0_a = ( _rtB ->
B_1036_2131_0_d || _rtB -> B_1036_2135_0_j ) ; _rtB -> B_1036_2137_0 = _rtB
-> B_1036_1614_0 * _rtB -> B_1036_1972_0 ; _rtB -> B_1036_2138_0 = _rtB ->
B_1036_1978_0 * _rtB -> B_1036_93_0_h ; _rtB -> B_1036_2139_0 = _rtB ->
B_1036_1618_0 * _rtB -> B_1036_2138_0 ; _rtB -> B_1036_2140_0 = _rtB ->
B_1036_2137_0 - _rtB -> B_1036_2139_0 ; _rtB -> B_1036_2141_0_a = ( _rtB ->
B_1036_2140_0 > _rtB -> B_1036_95_0_l ) ; _rtB -> B_1036_2142_0 = _rtB ->
B_1036_2140_0 * _rtB -> B_1036_2140_0 ; _rtB -> B_1036_2143_0 = _rtB ->
B_1036_1618_0 * _rtB -> B_1036_1972_0 ; _rtB -> B_1036_2144_0 = _rtB ->
B_1036_1614_0 * _rtB -> B_1036_2138_0 ; _rtB -> B_1036_2145_0 = _rtB ->
B_1036_2143_0 + _rtB -> B_1036_2144_0 ; _rtB -> B_1036_2146_0 = _rtB ->
B_1036_2145_0 * _rtB -> B_1036_94_0 ; _rtB -> B_1036_2147_0 = _rtB ->
B_1036_2145_0 * _rtB -> B_1036_2146_0 ; _rtB -> B_1036_2148_0 = _rtB ->
B_1036_2142_0 + _rtB -> B_1036_2147_0 ; _rtB -> B_1036_2149_0_a = ( _rtB ->
B_1036_2148_0 > _rtB -> B_1036_96_0_h ) ; if ( _rtB -> B_1036_18_0_g > _rtP
-> P_560 ) { _rtB -> B_1036_2150_0 = _rtB -> B_1036_2149_0_a ; } else { _rtB
-> B_1036_2150_0 = _rtB -> B_1036_11_0_n ; } _rtB -> B_1036_2151_0_l = ( _rtB
-> B_1036_2141_0_a && ( _rtB -> B_1036_2150_0 != 0.0 ) ) ; _rtB ->
B_1036_2152_0_i = ( _rtB -> B_1036_2145_0 > _rtB -> B_1036_95_0_l ) ; if (
_rtB -> B_1036_18_0_g > _rtP -> P_561 ) { _rtB -> B_51_0_0 = ! _rtB ->
B_1036_2149_0_a ; _rtB -> B_1036_2154_0 = _rtB -> B_51_0_0 ; } else { _rtB ->
B_1036_2154_0 = _rtB -> B_1036_18_0_g ; } _rtB -> B_1036_2155_0_n = ( _rtB ->
B_1036_2152_0_i && ( _rtB -> B_1036_2154_0 != 0.0 ) ) ; _rtB ->
B_1036_2156_0_b = ( _rtB -> B_1036_2151_0_l || _rtB -> B_1036_2155_0_n ) ;
_rtB -> B_1036_2157_0 = _rtB -> B_1036_1625_0 * _rtB -> B_1036_1909_0 ; _rtB
-> B_1036_2158_0 = _rtB -> B_1036_1915_0 * _rtB -> B_1036_97_0_m ; _rtB ->
B_1036_2159_0 = _rtB -> B_1036_1629_0 * _rtB -> B_1036_2158_0 ; _rtB ->
B_1036_2160_0 = _rtB -> B_1036_2157_0 - _rtB -> B_1036_2159_0 ; _rtB ->
B_1036_2161_0_h = ( _rtB -> B_1036_2160_0 > _rtB -> B_1036_99_0_h ) ; _rtB ->
B_1036_2162_0 = _rtB -> B_1036_2160_0 * _rtB -> B_1036_2160_0 ; _rtB ->
B_1036_2163_0 = _rtB -> B_1036_1629_0 * _rtB -> B_1036_1909_0 ; _rtB ->
B_1036_2164_0 = _rtB -> B_1036_1625_0 * _rtB -> B_1036_2158_0 ; _rtB ->
B_1036_2165_0 = _rtB -> B_1036_2163_0 + _rtB -> B_1036_2164_0 ; _rtB ->
B_1036_2166_0 = _rtB -> B_1036_2165_0 * _rtB -> B_1036_98_0_m ; _rtB ->
B_1036_2167_0 = _rtB -> B_1036_2165_0 * _rtB -> B_1036_2166_0 ; _rtB ->
B_1036_2168_0 = _rtB -> B_1036_2162_0 + _rtB -> B_1036_2167_0 ; _rtB ->
B_1036_2169_0_b = ( _rtB -> B_1036_2168_0 > _rtB -> B_1036_100_0_c ) ; if (
_rtB -> B_1036_18_0_g > _rtP -> P_562 ) { _rtB -> B_1036_2170_0 = _rtB ->
B_1036_2169_0_b ; } else { _rtB -> B_1036_2170_0 = _rtB -> B_1036_11_0_n ; }
_rtB -> B_1036_2171_0_a = ( _rtB -> B_1036_2161_0_h && ( _rtB ->
B_1036_2170_0 != 0.0 ) ) ; _rtB -> B_1036_2172_0_b = ( _rtB -> B_1036_2165_0
> _rtB -> B_1036_99_0_h ) ; if ( _rtB -> B_1036_18_0_g > _rtP -> P_563 ) {
_rtB -> B_52_0_0 = ! _rtB -> B_1036_2169_0_b ; _rtB -> B_1036_2174_0 = _rtB
-> B_52_0_0 ; } else { _rtB -> B_1036_2174_0 = _rtB -> B_1036_18_0_g ; } _rtB
-> B_1036_2175_0_l = ( _rtB -> B_1036_2172_0_b && ( _rtB -> B_1036_2174_0 !=
0.0 ) ) ; _rtB -> B_1036_2176_0 = ( _rtB -> B_1036_2171_0_a || _rtB ->
B_1036_2175_0_l ) ; _rtB -> B_1036_2177_0 = _rtB -> B_1036_1614_0 * _rtB ->
B_1036_1717_0 ; _rtB -> B_1036_2178_0 = _rtB -> B_1036_1719_0 * _rtB ->
B_1036_48_0 ; _rtB -> B_1036_2179_0 = _rtB -> B_1036_1618_0 * _rtB ->
B_1036_2178_0 ; _rtB -> B_1036_2180_0 = _rtB -> B_1036_2177_0 - _rtB ->
B_1036_2179_0 ; _rtB -> B_1036_2181_0 = ( _rtB -> B_1036_2180_0 > _rtB ->
B_1036_50_0 ) ; _rtB -> B_1036_2182_0 = _rtB -> B_1036_2180_0 * _rtB ->
B_1036_2180_0 ; _rtB -> B_1036_2183_0 = _rtB -> B_1036_1618_0 * _rtB ->
B_1036_1717_0 ; _rtB -> B_1036_2184_0 = _rtB -> B_1036_1614_0 * _rtB ->
B_1036_2178_0 ; _rtB -> B_1036_2185_0 = _rtB -> B_1036_2183_0 + _rtB ->
B_1036_2184_0 ; _rtB -> B_1036_2186_0 = _rtB -> B_1036_2185_0 * _rtB ->
B_1036_49_0_n ; _rtB -> B_1036_2187_0 = _rtB -> B_1036_2185_0 * _rtB ->
B_1036_2186_0 ; _rtB -> B_1036_2188_0 = _rtB -> B_1036_2182_0 + _rtB ->
B_1036_2187_0 ; _rtB -> B_1036_2189_0_h = ( _rtB -> B_1036_2188_0 > _rtB ->
B_1036_51_0_m ) ; if ( _rtB -> B_1036_2044_0_c ) { _rtB -> B_1036_2190_0 =
_rtB -> B_1036_2189_0_h ; _rtB -> B_36_0_0 = ! _rtB -> B_1036_2189_0_h ; _rtB
-> B_1036_2194_0 = _rtB -> B_36_0_0 ; } else { _rtB -> B_1036_2190_0 = _rtB
-> B_1036_15_0_l ; _rtB -> B_1036_2194_0 = _rtB -> B_1036_16_0_j ; } _rtB ->
B_1036_2191_0_d = ( _rtB -> B_1036_2181_0 && ( _rtB -> B_1036_2190_0 != 0.0 )
) ; _rtB -> B_1036_2192_0_g = ( _rtB -> B_1036_2185_0 > _rtB -> B_1036_50_0 )
; _rtB -> B_1036_2195_0_j = ( _rtB -> B_1036_2192_0_g && ( _rtB ->
B_1036_2194_0 != 0.0 ) ) ; _rtB -> B_1036_2196_0_m = ( _rtB ->
B_1036_2191_0_d || _rtB -> B_1036_2195_0_j ) ; if ( _rtB -> B_1036_2031_0_d )
{ _rtB -> B_1036_2198_0 = _rtB -> B_1036_12_0 ; } else { if ( _rtB ->
B_1036_9_0_g > _rtP -> P_23 ) { _rtB -> B_34_0_0 = _rtB -> B_1036_2073_0 *
_rtB -> B_1036_46_0_i ; _rtB -> B_34_1_0 = _rtB -> B_1036_2070_0 * _rtB ->
B_1036_44_0_o ; _rtB -> B_34_2_0 = _rtB -> B_34_1_0 - _rtB -> B_34_0_0 ; _rtB
-> B_34_3_0 = ( _rtB -> B_34_2_0 > _rtB -> B_1036_13_0_p ) ; _rtB -> B_34_4_0
= ! _rtB -> B_34_3_0 ; if ( _rtB -> B_34_4_0 ) { _rtB -> B_33_0_0 = ( _rtB ->
B_34_2_0 < _rtB -> B_1036_47_0_o ) ; _rtB -> B_33_1_0 = ! _rtB -> B_33_0_0 ;
_rtB -> B_34_6_0 = _rtB -> B_33_1_0 ; } else { _rtB -> B_34_6_0 = _rtB ->
B_34_3_0 ; } _rtB -> B_35_2_0 = _rtB -> B_34_6_0 ; } else { if ( _rtB ->
B_1036_2015_0_o ) { _rtB -> B_32_1_0 = _rtB -> B_1036_2116_0_p ; } else { if
( _rtB -> B_1036_1949_0 ) { _rtB -> B_31_1_0 = _rtB -> B_1036_2176_0 ; } else
{ if ( _rtB -> B_1036_1886_0 ) { _rtB -> B_30_0_0 = _rtB -> B_1036_2096_0_n ;
} else { _rtB -> B_30_0_0 = _rtB -> B_1036_2116_0_p ; } _rtB -> B_31_1_0 =
_rtB -> B_30_0_0 ; } _rtB -> B_32_1_0 = _rtB -> B_31_1_0 ; } _rtB -> B_35_2_0
= _rtB -> B_32_1_0 ; } _rtB -> B_1036_2198_0 = _rtB -> B_35_2_0 ; } if ( _rtB
-> B_1036_2023_0_d ) { _rtB -> B_1036_2200_0 = _rtB -> B_1036_12_0 ; } else {
if ( _rtB -> B_1036_9_0_g > _rtP -> P_22 ) { _rtB -> B_28_0_0 = _rtB ->
B_1036_2063_0 * _rtB -> B_1036_37_0_j ; _rtB -> B_28_1_0 = _rtB ->
B_1036_2060_0 * _rtB -> B_1036_35_0_e ; _rtB -> B_28_2_0 = _rtB -> B_28_1_0 -
_rtB -> B_28_0_0 ; _rtB -> B_28_3_0 = ( _rtB -> B_28_2_0 > _rtB ->
B_1036_13_0_p ) ; _rtB -> B_28_4_0 = ! _rtB -> B_28_3_0 ; if ( _rtB ->
B_28_4_0 ) { _rtB -> B_27_0_0 = ( _rtB -> B_28_2_0 < _rtB -> B_1036_38_0 ) ;
_rtB -> B_27_1_0 = ! _rtB -> B_27_0_0 ; _rtB -> B_28_6_0 = _rtB -> B_27_1_0 ;
} else { _rtB -> B_28_6_0 = _rtB -> B_28_3_0 ; } _rtB -> B_29_2_0 = _rtB ->
B_28_6_0 ; } else { if ( _rtB -> B_1036_2015_0_o ) { _rtB -> B_26_1_0 = _rtB
-> B_1036_2156_0_b ; } else { if ( _rtB -> B_1036_1949_0 ) { _rtB -> B_25_1_0
= _rtB -> B_1036_2176_0 ; } else { _rtB -> B_24_0_0 = _rtB -> B_1036_2136_0_a
; _rtB -> B_25_1_0 = _rtB -> B_24_0_0 ; } _rtB -> B_26_1_0 = _rtB -> B_25_1_0
; } _rtB -> B_29_2_0 = _rtB -> B_26_1_0 ; } _rtB -> B_1036_2200_0 = _rtB ->
B_29_2_0 ; } if ( _rtB -> B_1036_1817_0_b ) { _rtB -> B_1036_2202_0 = _rtB ->
B_1036_12_0 ; } else { if ( _rtB -> B_1036_9_0_g > _rtP -> P_21 ) { _rtB ->
B_22_0_0 = _rtB -> B_1036_2053_0 * _rtB -> B_1036_28_0_b ; _rtB -> B_22_1_0 =
_rtB -> B_1036_2050_0 * _rtB -> B_1036_26_0_b ; _rtB -> B_22_2_0 = _rtB ->
B_22_1_0 - _rtB -> B_22_0_0 ; _rtB -> B_22_3_0 = ( _rtB -> B_22_2_0 > _rtB ->
B_1036_13_0_p ) ; _rtB -> B_22_4_0 = ! _rtB -> B_22_3_0 ; if ( _rtB ->
B_22_4_0 ) { _rtB -> B_21_0_0 = ( _rtB -> B_22_2_0 < _rtB -> B_1036_29_0_l )
; _rtB -> B_21_1_0 = ! _rtB -> B_21_0_0 ; _rtB -> B_22_6_0 = _rtB -> B_21_1_0
; } else { _rtB -> B_22_6_0 = _rtB -> B_22_3_0 ; } _rtB -> B_23_2_0 = _rtB ->
B_22_6_0 ; } else { if ( _rtB -> B_1036_2015_0_o ) { _rtB -> B_20_1_0 = _rtB
-> B_1036_2156_0_b ; } else { if ( _rtB -> B_1036_1949_0 ) { _rtB -> B_19_1_0
= _rtB -> B_1036_2196_0_m ; } else { if ( _rtB -> B_1036_1886_0 ) { _rtB ->
B_18_0_0 = _rtB -> B_1036_2096_0_n ; } else { _rtB -> B_18_0_0 = _rtB ->
B_1036_2196_0_m ; } _rtB -> B_19_1_0 = _rtB -> B_18_0_0 ; } _rtB -> B_20_1_0
= _rtB -> B_19_1_0 ; } _rtB -> B_23_2_0 = _rtB -> B_20_1_0 ; } _rtB ->
B_1036_2202_0 = _rtB -> B_23_2_0 ; } _rtB -> B_1036_2203_0 = _rtB ->
B_1036_1972_0 * _rtB -> B_1036_1972_0 ; _rtB -> B_1036_2204_0 = _rtB ->
B_1036_1978_0 * _rtB -> B_1036_1978_0 ; _rtB -> B_1036_2205_0 = _rtB ->
B_1036_2203_0 + _rtB -> B_1036_2204_0 ; _rtB -> B_1036_2206_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_2205_0 ) ; _rtB -> B_1036_2207_0 = _rtB
-> B_1036_1972_0 / _rtB -> B_1036_2206_0 ; u0 = _rtB -> B_1036_2207_0 ; if (
u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB ->
B_1036_2208_0 = muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_2209_0_l = ( _rtB
-> B_1036_1978_0 > _rtB -> B_1036_64_0_e ) ; if ( _rtB -> B_1036_2209_0_l ) {
_rtB -> B_1036_2211_0 = _rtB -> B_1036_2208_0 ; } else { _rtB -> B_39_0_0 =
_rtB -> B_1036_2208_0 * _rtB -> B_1036_63_0_a ; _rtB -> B_1036_2211_0 = _rtB
-> B_39_0_0 ; } _rtB -> B_1036_2212_0 = _rtB -> B_1036_1996_0 * _rtB ->
B_1036_1996_0 ; _rtB -> B_1036_2213_0 = _rtB -> B_1036_2002_0 * _rtB ->
B_1036_2002_0 ; _rtB -> B_1036_2214_0 = _rtB -> B_1036_2212_0 + _rtB ->
B_1036_2213_0 ; _rtB -> B_1036_2215_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_2214_0 ) ; _rtB -> B_1036_2216_0 = _rtB -> B_1036_1996_0 / _rtB ->
B_1036_2215_0 ; u0 = _rtB -> B_1036_2216_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB -> B_1036_2217_0 =
muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_2218_0_o = ( _rtB -> B_1036_2002_0
> _rtB -> B_1036_66_0 ) ; if ( _rtB -> B_1036_2218_0_o ) { _rtB ->
B_1036_2220_0 = _rtB -> B_1036_2217_0 ; } else { _rtB -> B_40_0_0 = _rtB ->
B_1036_2217_0 * _rtB -> B_1036_65_0_a ; _rtB -> B_1036_2220_0 = _rtB ->
B_40_0_0 ; } _rtB -> B_1036_2221_0 = _rtB -> B_1036_1971_0 * _rtB ->
B_1036_1971_0 ; _rtB -> B_1036_2222_0 = _rtB -> B_1036_1977_0 * _rtB ->
B_1036_1977_0 ; _rtB -> B_1036_2223_0 = _rtB -> B_1036_2221_0 + _rtB ->
B_1036_2222_0 ; _rtB -> B_1036_2224_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_2223_0 ) ; _rtB -> B_1036_2225_0 = _rtB -> B_1036_1971_0 / _rtB ->
B_1036_2224_0 ; u0 = _rtB -> B_1036_2225_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB -> B_1036_2226_0 =
muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_2227_0_p = ( _rtB -> B_1036_1977_0
> _rtB -> B_1036_68_0 ) ; if ( _rtB -> B_1036_2227_0_p ) { _rtB ->
B_1036_2229_0 = _rtB -> B_1036_2226_0 ; } else { _rtB -> B_41_0_0 = _rtB ->
B_1036_2226_0 * _rtB -> B_1036_67_0_a ; _rtB -> B_1036_2229_0 = _rtB ->
B_41_0_0 ; } _rtB -> B_1036_2230_0 = _rtB -> B_1036_1995_0 * _rtB ->
B_1036_1995_0 ; _rtB -> B_1036_2231_0 = _rtB -> B_1036_2001_0 * _rtB ->
B_1036_2001_0 ; _rtB -> B_1036_2232_0 = _rtB -> B_1036_2230_0 + _rtB ->
B_1036_2231_0 ; _rtB -> B_1036_2233_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_2232_0 ) ; _rtB -> B_1036_2234_0 = _rtB -> B_1036_1995_0 / _rtB ->
B_1036_2233_0 ; u0 = _rtB -> B_1036_2234_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB -> B_1036_2235_0 =
muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_2236_0 = ( _rtB -> B_1036_2001_0 >
_rtB -> B_1036_70_0_l ) ; if ( _rtB -> B_1036_2236_0 ) { _rtB ->
B_1036_2238_0 = _rtB -> B_1036_2235_0 ; } else { _rtB -> B_42_0_0 = _rtB ->
B_1036_2235_0 * _rtB -> B_1036_69_0_i ; _rtB -> B_1036_2238_0 = _rtB ->
B_42_0_0 ; } _rtB -> B_1036_2239_0 = _rtB -> B_1036_1909_0 * _rtB ->
B_1036_1909_0 ; _rtB -> B_1036_2240_0 = _rtB -> B_1036_1915_0 * _rtB ->
B_1036_1915_0 ; _rtB -> B_1036_2241_0 = _rtB -> B_1036_2239_0 + _rtB ->
B_1036_2240_0 ; _rtB -> B_1036_2242_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_2241_0 ) ; _rtB -> B_1036_2243_0 = _rtB -> B_1036_1909_0 / _rtB ->
B_1036_2242_0 ; u0 = _rtB -> B_1036_2243_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB -> B_1036_2244_0 =
muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_2245_0 = ( _rtB -> B_1036_1915_0 >
_rtB -> B_1036_75_0_f ) ; if ( _rtB -> B_1036_2245_0 ) { _rtB ->
B_1036_2247_0 = _rtB -> B_1036_2244_0 ; } else { _rtB -> B_43_0_0 = _rtB ->
B_1036_2244_0 * _rtB -> B_1036_74_0 ; _rtB -> B_1036_2247_0 = _rtB ->
B_43_0_0 ; } _rtB -> B_1036_2248_0 = _rtB -> B_1036_1933_0 * _rtB ->
B_1036_1933_0 ; _rtB -> B_1036_2249_0 = _rtB -> B_1036_1939_0 * _rtB ->
B_1036_1939_0 ; _rtB -> B_1036_2250_0 = _rtB -> B_1036_2248_0 + _rtB ->
B_1036_2249_0 ; _rtB -> B_1036_2251_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_2250_0 ) ; _rtB -> B_1036_2252_0 = _rtB -> B_1036_1933_0 / _rtB ->
B_1036_2251_0 ; u0 = _rtB -> B_1036_2252_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB -> B_1036_2253_0 =
muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_2254_0_d = ( _rtB -> B_1036_1939_0
> _rtB -> B_1036_77_0_i ) ; if ( _rtB -> B_1036_2254_0_d ) { _rtB ->
B_1036_2256_0 = _rtB -> B_1036_2253_0 ; } else { _rtB -> B_44_0_0 = _rtB ->
B_1036_2253_0 * _rtB -> B_1036_76_0 ; _rtB -> B_1036_2256_0 = _rtB ->
B_44_0_0 ; } _rtB -> B_1036_2257_0 = _rtB -> B_1036_1908_0 * _rtB ->
B_1036_1908_0 ; _rtB -> B_1036_2258_0 = _rtB -> B_1036_1914_0 * _rtB ->
B_1036_1914_0 ; _rtB -> B_1036_2259_0 = _rtB -> B_1036_2257_0 + _rtB ->
B_1036_2258_0 ; _rtB -> B_1036_2260_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_2259_0 ) ; _rtB -> B_1036_2261_0 = _rtB -> B_1036_1908_0 / _rtB ->
B_1036_2260_0 ; u0 = _rtB -> B_1036_2261_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB -> B_1036_2262_0 =
muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_2263_0_g = ( _rtB -> B_1036_1914_0
> _rtB -> B_1036_79_0_g ) ; if ( _rtB -> B_1036_2263_0_g ) { _rtB ->
B_1036_2265_0 = _rtB -> B_1036_2262_0 ; } else { _rtB -> B_45_0_0 = _rtB ->
B_1036_2262_0 * _rtB -> B_1036_78_0_f ; _rtB -> B_1036_2265_0 = _rtB ->
B_45_0_0 ; } _rtB -> B_1036_2266_0 = _rtB -> B_1036_1932_0 * _rtB ->
B_1036_1932_0 ; _rtB -> B_1036_2267_0 = _rtB -> B_1036_1938_0 * _rtB ->
B_1036_1938_0 ; _rtB -> B_1036_2268_0 = _rtB -> B_1036_2266_0 + _rtB ->
B_1036_2267_0 ; _rtB -> B_1036_2269_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_2268_0 ) ; _rtB -> B_1036_2270_0 = _rtB -> B_1036_1932_0 / _rtB ->
B_1036_2269_0 ; u0 = _rtB -> B_1036_2270_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB -> B_1036_2271_0 =
muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_2272_0 = ( _rtB -> B_1036_1938_0 >
_rtB -> B_1036_81_0_o ) ; if ( _rtB -> B_1036_2272_0 ) { _rtB ->
B_1036_2274_0 = _rtB -> B_1036_2271_0 ; } else { _rtB -> B_46_0_0 = _rtB ->
B_1036_2271_0 * _rtB -> B_1036_80_0_c ; _rtB -> B_1036_2274_0 = _rtB ->
B_46_0_0 ; } _rtB -> B_1036_2275_0 = _rtB -> B_1036_1846_0 * _rtB ->
B_1036_1846_0 ; _rtB -> B_1036_2276_0 = _rtB -> B_1036_1852_0 * _rtB ->
B_1036_1852_0 ; _rtB -> B_1036_2277_0 = _rtB -> B_1036_2275_0 + _rtB ->
B_1036_2276_0 ; _rtB -> B_1036_2278_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_2277_0 ) ; _rtB -> B_1036_2279_0 = _rtB -> B_1036_1846_0 / _rtB ->
B_1036_2278_0 ; u0 = _rtB -> B_1036_2279_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB -> B_1036_2280_0 =
muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_2281_0 = ( _rtB -> B_1036_1852_0 >
_rtB -> B_1036_86_0 ) ; if ( _rtB -> B_1036_2281_0 ) { _rtB -> B_1036_2283_0
= _rtB -> B_1036_2280_0 ; } else { _rtB -> B_47_0_0 = _rtB -> B_1036_2280_0 *
_rtB -> B_1036_85_0_m ; _rtB -> B_1036_2283_0 = _rtB -> B_47_0_0 ; } _rtB ->
B_1036_2284_0 = _rtB -> B_1036_1870_0 * _rtB -> B_1036_1870_0 ; _rtB ->
B_1036_2285_0 = _rtB -> B_1036_1876_0 * _rtB -> B_1036_1876_0 ; _rtB ->
B_1036_2286_0 = _rtB -> B_1036_2284_0 + _rtB -> B_1036_2285_0 ; _rtB ->
B_1036_2287_0 = muDoubleScalarSqrt ( _rtB -> B_1036_2286_0 ) ; _rtB ->
B_1036_2288_0 = _rtB -> B_1036_1870_0 / _rtB -> B_1036_2287_0 ; u0 = _rtB ->
B_1036_2288_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0
= - 1.0 ; } } _rtB -> B_1036_2289_0 = muDoubleScalarAcos ( u0 ) ; _rtB ->
B_1036_2290_0_m = ( _rtB -> B_1036_1876_0 > _rtB -> B_1036_88_0_f ) ; if (
_rtB -> B_1036_2290_0_m ) { _rtB -> B_1036_2292_0 = _rtB -> B_1036_2289_0 ; }
else { _rtB -> B_48_0_0 = _rtB -> B_1036_2289_0 * _rtB -> B_1036_87_0_c ;
_rtB -> B_1036_2292_0 = _rtB -> B_48_0_0 ; } _rtB -> B_1036_2293_0 = _rtB ->
B_1036_1845_0 * _rtB -> B_1036_1845_0 ; _rtB -> B_1036_2294_0 = _rtB ->
B_1036_1851_0 * _rtB -> B_1036_1851_0 ; _rtB -> B_1036_2295_0 = _rtB ->
B_1036_2293_0 + _rtB -> B_1036_2294_0 ; _rtB -> B_1036_2296_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_2295_0 ) ; _rtB -> B_1036_2297_0 = _rtB
-> B_1036_1845_0 / _rtB -> B_1036_2296_0 ; u0 = _rtB -> B_1036_2297_0 ; if (
u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB ->
B_1036_2298_0 = muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_2299_0 = ( _rtB ->
B_1036_1851_0 > _rtB -> B_1036_90_0_e ) ; if ( _rtB -> B_1036_2299_0 ) { _rtB
-> B_1036_2301_0 = _rtB -> B_1036_2298_0 ; } else { _rtB -> B_49_0_0 = _rtB
-> B_1036_2298_0 * _rtB -> B_1036_89_0_p ; _rtB -> B_1036_2301_0 = _rtB ->
B_49_0_0 ; } _rtB -> B_1036_2302_0 = _rtB -> B_1036_1869_0 * _rtB ->
B_1036_1869_0 ; _rtB -> B_1036_2303_0 = _rtB -> B_1036_1875_0 * _rtB ->
B_1036_1875_0 ; _rtB -> B_1036_2304_0 = _rtB -> B_1036_2302_0 + _rtB ->
B_1036_2303_0 ; _rtB -> B_1036_2305_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_2304_0 ) ; _rtB -> B_1036_2306_0 = _rtB -> B_1036_1869_0 / _rtB ->
B_1036_2305_0 ; u0 = _rtB -> B_1036_2306_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB -> B_1036_2307_0 =
muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_2308_0 = ( _rtB -> B_1036_1875_0 >
_rtB -> B_1036_92_0 ) ; if ( _rtB -> B_1036_2308_0 ) { _rtB -> B_1036_2310_0
= _rtB -> B_1036_2307_0 ; } else { _rtB -> B_50_0_0 = _rtB -> B_1036_2307_0 *
_rtB -> B_1036_91_0_o ; _rtB -> B_1036_2310_0 = _rtB -> B_50_0_0 ; } _rtB ->
B_1036_2311_0 = muDoubleScalarCos ( _rtB -> B_1036_1748_0 ) ; _rtB ->
B_1036_2312_0 = muDoubleScalarSin ( _rtB -> B_1036_1748_0 ) ; _rtB ->
B_1036_2313_0 = muDoubleScalarCos ( _rtB -> B_1036_1779_0 ) ; _rtB ->
B_1036_2314_0 = muDoubleScalarSin ( _rtB -> B_1036_1779_0 ) ; _rtB ->
B_1036_2315_0 = muDoubleScalarCos ( _rtB -> B_1036_1808_0 ) ; _rtB ->
B_1036_2316_0 = muDoubleScalarSin ( _rtB -> B_1036_1808_0 ) ; _rtB ->
B_1036_2317_0 = muDoubleScalarCos ( _rtB -> B_1036_1651_0 ) ; _rtB ->
B_1036_2318_0 = muDoubleScalarSin ( _rtB -> B_1036_1651_0 ) ; _rtB ->
B_1036_2319_0 = muDoubleScalarCos ( _rtB -> B_1036_1686_0 ) ; _rtB ->
B_1036_2320_0 = muDoubleScalarSin ( _rtB -> B_1036_1686_0 ) ; _rtB ->
B_1036_2321_0 = muDoubleScalarCos ( _rtB -> B_1036_1715_0 ) ; _rtB ->
B_1036_2322_0 = muDoubleScalarSin ( _rtB -> B_1036_1715_0 ) ; _rtB ->
B_1036_2323_0 = _rtB -> B_1036_1614_0 - _rtB -> B_1036_146_0 ; _rtB ->
B_1036_2324_0 = _rtB -> B_1036_2323_0 * _rtB -> B_1036_2323_0 ; _rtB ->
B_1036_2325_0 = _rtB -> B_1036_1618_0 - _rtB -> B_1036_152_0_c ; _rtB ->
B_1036_2326_0 = _rtB -> B_1036_2325_0 * _rtB -> B_1036_2325_0 ; _rtB ->
B_1036_2327_0 = _rtB -> B_1036_2324_0 + _rtB -> B_1036_2326_0 ; _rtB ->
B_1036_2328_0 = muDoubleScalarSqrt ( _rtB -> B_1036_2327_0 ) ; _rtB ->
B_1036_2329_0 = _rtB -> B_1036_1614_0 * _rtB -> B_1036_1614_0 ; _rtB ->
B_1036_2330_0 = _rtB -> B_1036_1618_0 * _rtB -> B_1036_1618_0 ; _rtB ->
B_1036_2331_0 = _rtB -> B_1036_2329_0 + _rtB -> B_1036_2330_0 ; _rtB ->
B_1036_2332_0 = muDoubleScalarSqrt ( _rtB -> B_1036_2331_0 ) ; maxV =
muDoubleScalarMax ( _rtB -> B_1036_2332_0 , _rtB -> B_1036_182_0 ) ; _rtB ->
B_1036_2333_0 = maxV ; _rtB -> B_1036_2334_0 = _rtB -> B_1036_2328_0 / _rtB
-> B_1036_2333_0 ; _rtB -> B_1036_2335_0 = ( _rtB -> B_1036_2334_0 > _rtB ->
B_1036_129_0_o ) ; _rtB -> B_1036_2336_0 = ( _rtB -> B_1036_1815_0_e && _rtB
-> B_1036_2021_0_a ) ; _rtB -> B_1036_2337_0 = ! _rtB -> B_1036_2336_0 ; _rtB
-> B_1036_2338_0_o = ( _rtB -> B_1036_2335_0 && _rtB -> B_1036_2337_0 ) ;
_rtB -> B_1036_2339_0_f = ( _rtB -> B_1036_2332_0 < _rtB -> B_1036_182_0 ) ;
_rtB -> B_1036_2340_0_i = ( _rtB -> B_1036_2338_0_o && _rtB ->
B_1036_2339_0_f ) ; _rtB -> B_1036_2341_0 = ! _rtB -> B_1036_2339_0_f ; _rtB
-> B_1036_2342_0 = ( _rtB -> B_1036_2338_0_o && _rtB -> B_1036_2341_0 ) ;
_rtB -> B_1036_2343_0 = ( _rtB -> B_1036_2340_0_i || _rtB -> B_1036_2342_0 )
; _rtB -> B_1036_2344_0 = ! _rtB -> B_1036_2343_0 ; _rtB -> B_1036_2345_0 =
_rtB -> B_1036_1625_0 - _rtB -> B_1036_167_0_n ; _rtB -> B_1036_2346_0 = _rtB
-> B_1036_2345_0 * _rtB -> B_1036_2345_0 ; _rtB -> B_1036_2347_0 = _rtB ->
B_1036_1629_0 - _rtB -> B_1036_169_0_m ; _rtB -> B_1036_2348_0 = _rtB ->
B_1036_2347_0 * _rtB -> B_1036_2347_0 ; _rtB -> B_1036_2349_0 = _rtB ->
B_1036_2346_0 + _rtB -> B_1036_2348_0 ; _rtB -> B_1036_2350_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_2349_0 ) ; _rtB -> B_1036_2351_0 = _rtB
-> B_1036_1625_0 * _rtB -> B_1036_1625_0 ; _rtB -> B_1036_2352_0 = _rtB ->
B_1036_1629_0 * _rtB -> B_1036_1629_0 ; _rtB -> B_1036_2353_0 = _rtB ->
B_1036_2351_0 + _rtB -> B_1036_2352_0 ; _rtB -> B_1036_2354_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_2353_0 ) ; maxV = muDoubleScalarMax (
_rtB -> B_1036_2354_0 , _rtB -> B_1036_186_0_d ) ; _rtB -> B_1036_2355_0 =
maxV ; _rtB -> B_1036_2356_0 = _rtB -> B_1036_2350_0 / _rtB -> B_1036_2355_0
; _rtB -> B_1036_2357_0 = ( _rtB -> B_1036_2356_0 > _rtB -> B_1036_129_0_o )
; _rtB -> B_1036_2358_0 = ( _rtB -> B_1036_2029_0_h && _rtB -> B_1036_174_0_i
) ; _rtB -> B_1036_2359_0_d = ! _rtB -> B_1036_2358_0 ; _rtB ->
B_1036_2360_0_f = ( _rtB -> B_1036_2357_0 && _rtB -> B_1036_2359_0_d ) ; _rtB
-> B_1036_2361_0_n = ( _rtB -> B_1036_2354_0 < _rtB -> B_1036_186_0_d ) ;
_rtB -> B_1036_2362_0 = ( _rtB -> B_1036_2360_0_f && _rtB -> B_1036_2361_0_n
) ; _rtB -> B_1036_2363_0 = ! _rtB -> B_1036_2361_0_n ; _rtB -> B_1036_2364_0
= ( _rtB -> B_1036_2360_0_f && _rtB -> B_1036_2363_0 ) ; _rtB ->
B_1036_2365_0 = ( _rtB -> B_1036_2362_0 || _rtB -> B_1036_2364_0 ) ; _rtB ->
B_1036_2366_0 = ! _rtB -> B_1036_2365_0 ; _rtB -> B_1036_2367_0 = _rtB ->
B_1036_1660_0 - _rtB -> B_1036_148_0 ; _rtB -> B_1036_2368_0 = _rtB ->
B_1036_2367_0 * _rtB -> B_1036_2367_0 ; _rtB -> B_1036_2369_0 = _rtB ->
B_1036_1664_0 - _rtB -> B_1036_150_0_o ; _rtB -> B_1036_2370_0 = _rtB ->
B_1036_2369_0 * _rtB -> B_1036_2369_0 ; _rtB -> B_1036_2371_0 = _rtB ->
B_1036_2368_0 + _rtB -> B_1036_2370_0 ; _rtB -> B_1036_2372_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_2371_0 ) ; _rtB -> B_1036_2373_0 = _rtB
-> B_1036_1660_0 * _rtB -> B_1036_1660_0 ; _rtB -> B_1036_2374_0 = _rtB ->
B_1036_1664_0 * _rtB -> B_1036_1664_0 ; _rtB -> B_1036_2375_0 = _rtB ->
B_1036_2373_0 + _rtB -> B_1036_2374_0 ; _rtB -> B_1036_2376_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_2375_0 ) ; maxV = muDoubleScalarMax (
_rtB -> B_1036_2376_0 , _rtB -> B_1036_190_0_p ) ; _rtB -> B_1036_2377_0 =
maxV ; _rtB -> B_1036_2378_0 = _rtB -> B_1036_2372_0 / _rtB -> B_1036_2377_0
; _rtB -> B_1036_2379_0_d = ( _rtB -> B_1036_2378_0 > _rtB -> B_1036_129_0_o
) ; _rtB -> B_1036_2380_0_c = ( _rtB -> B_1036_2379_0_d && _rtB ->
B_1036_165_0_j ) ; _rtB -> B_1036_2381_0_b = ( _rtB -> B_1036_2376_0 < _rtB
-> B_1036_190_0_p ) ; _rtB -> B_1036_2382_0_b = ( _rtB -> B_1036_2380_0_c &&
_rtB -> B_1036_2381_0_b ) ; _rtB -> B_1036_2383_0_h = ! _rtB ->
B_1036_2381_0_b ; _rtB -> B_1036_2384_0_g = ( _rtB -> B_1036_2380_0_c && _rtB
-> B_1036_2383_0_h ) ; _rtB -> B_1036_2385_0_c = ( _rtB -> B_1036_2382_0_b ||
_rtB -> B_1036_2384_0_g ) ; _rtB -> B_1036_2386_0_d = ! _rtB ->
B_1036_2385_0_c ; _rtB -> B_1036_2387_0_a = ( _rtB -> B_1036_2344_0 && _rtB
-> B_1036_2366_0 && _rtB -> B_1036_2386_0_d ) ; _rtB -> B_1036_2388_0_h = !
_rtB -> B_1036_1815_0_e ; _rtB -> B_1036_2389_0_c = ! _rtB -> B_1036_2021_0_a
; _rtB -> B_1036_2390_0_c = ! _rtB -> B_1036_2029_0_h ; _rtB ->
B_1036_2391_0_m = ( _rtB -> B_1036_2388_0_h && _rtB -> B_1036_2389_0_c &&
_rtB -> B_1036_2390_0_c ) ; _rtB -> B_1036_2392_0 = ( _rtB -> B_1036_2391_0_m
&& _rtB -> B_1036_178_0_a ) ; _rtB -> B_1036_2393_0_o = ( _rtB ->
B_1036_2387_0_a && _rtB -> B_1036_2392_0 ) ; _rtB -> B_1036_2394_0_e = ( _rtB
-> B_1036_132_0_e && _rtB -> B_1036_2393_0_o ) ; _rtB -> B_1036_2395_0_j = !
_rtB -> B_1036_2393_0_o ; _rtB -> B_1036_2396_0_l = false ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2397_0_n =
_rtDW -> u4_PreviousInput_el ; } _rtB -> B_1036_2398_0_e = ( _rtB ->
B_1036_2343_0 && _rtB -> B_1036_2365_0 && _rtB -> B_1036_2385_0_c ) ; _rtB ->
B_1036_2399_0 = _rtB -> B_1036_2323_0 - _rtB -> B_1036_2345_0 ; _rtB ->
B_1036_2400_0 = _rtB -> B_1036_2399_0 * _rtB -> B_1036_2399_0 ; _rtB ->
B_1036_2401_0 = _rtB -> B_1036_2325_0 - _rtB -> B_1036_2347_0 ; _rtB ->
B_1036_2402_0 = _rtB -> B_1036_2401_0 * _rtB -> B_1036_2401_0 ; _rtB ->
B_1036_2403_0 = _rtB -> B_1036_2400_0 + _rtB -> B_1036_2402_0 ; _rtB ->
B_1036_2404_0 = muDoubleScalarSqrt ( _rtB -> B_1036_2403_0 ) ; maxV =
muDoubleScalarMax ( _rtB -> B_1036_2328_0 , _rtB -> B_1036_2350_0 ) ; maxV =
muDoubleScalarMax ( maxV , _rtB -> B_1036_2372_0 ) ; _rtB -> B_1036_2405_0 =
maxV ; _rtB -> B_1036_2406_0 = _rtB -> B_1036_2404_0 / _rtB -> B_1036_2405_0
; _rtB -> B_1036_2407_0_c = ( _rtB -> B_1036_2406_0 < _rtB -> B_1036_131_0_n
) ; _rtB -> B_1036_2408_0 = _rtB -> B_1036_2345_0 - _rtB -> B_1036_2367_0 ;
_rtB -> B_1036_2409_0 = _rtB -> B_1036_2408_0 * _rtB -> B_1036_2408_0 ; _rtB
-> B_1036_2410_0 = _rtB -> B_1036_2347_0 - _rtB -> B_1036_2369_0 ; _rtB ->
B_1036_2411_0 = _rtB -> B_1036_2410_0 * _rtB -> B_1036_2410_0 ; _rtB ->
B_1036_2412_0 = _rtB -> B_1036_2409_0 + _rtB -> B_1036_2411_0 ; _rtB ->
B_1036_2413_0 = muDoubleScalarSqrt ( _rtB -> B_1036_2412_0 ) ; _rtB ->
B_1036_2414_0 = _rtB -> B_1036_2413_0 / _rtB -> B_1036_2405_0 ; _rtB ->
B_1036_2415_0_j = ( _rtB -> B_1036_2414_0 < _rtB -> B_1036_131_0_n ) ; _rtB
-> B_1036_2416_0 = _rtB -> B_1036_2367_0 - _rtB -> B_1036_2323_0 ; _rtB ->
B_1036_2417_0 = _rtB -> B_1036_2416_0 * _rtB -> B_1036_2416_0 ; _rtB ->
B_1036_2418_0 = _rtB -> B_1036_2369_0 - _rtB -> B_1036_2325_0 ; _rtB ->
B_1036_2419_0 = _rtB -> B_1036_2418_0 * _rtB -> B_1036_2418_0 ; _rtB ->
B_1036_2420_0 = _rtB -> B_1036_2417_0 + _rtB -> B_1036_2419_0 ; _rtB ->
B_1036_2421_0 = muDoubleScalarSqrt ( _rtB -> B_1036_2420_0 ) ; _rtB ->
B_1036_2422_0 = _rtB -> B_1036_2421_0 / _rtB -> B_1036_2405_0 ; _rtB ->
B_1036_2423_0 = ( _rtB -> B_1036_2422_0 < _rtB -> B_1036_131_0_n ) ; _rtB ->
B_1036_2424_0 = ( _rtB -> B_1036_2407_0_c && _rtB -> B_1036_2415_0_j && _rtB
-> B_1036_2423_0 ) ; _rtB -> B_1036_2425_0_m = ( _rtB -> B_1036_2398_0_e &&
_rtB -> B_1036_2424_0 ) ; _rtB -> B_1036_2426_0_e = ( _rtB -> B_1036_2334_0 >
_rtB -> B_1036_130_0 ) ; _rtB -> B_1036_2427_0 = ! _rtB -> B_1036_2425_0_m ;
_rtB -> B_1036_2428_0_h = ( _rtB -> B_1036_2426_0_e && _rtB -> B_1036_2427_0
) ; _rtB -> B_1036_2429_0_c = ( _rtB -> B_1036_2356_0 > _rtB -> B_1036_130_0
) ; _rtB -> B_1036_2430_0_j = ! _rtB -> B_1036_2425_0_m ; _rtB ->
B_1036_2431_0_a = ( _rtB -> B_1036_2429_0_c && _rtB -> B_1036_2430_0_j ) ;
_rtB -> B_1036_2432_0_d = ( _rtB -> B_1036_2378_0 > _rtB -> B_1036_130_0 ) ;
_rtB -> B_1036_2433_0 = ! _rtB -> B_1036_2425_0_m ; _rtB -> B_1036_2434_0_b =
( _rtB -> B_1036_2432_0_d && _rtB -> B_1036_2433_0 ) ; _rtB ->
B_1036_2435_0_c = ( _rtB -> B_1036_2425_0_m || _rtB -> B_1036_2428_0_h ||
_rtB -> B_1036_2431_0_a || _rtB -> B_1036_2434_0_b ) ; _rtB ->
B_1036_2436_0_a = false ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_1036_2437_0_i = _rtDW -> Memory_PreviousInput_b0 ; _rtB ->
B_1036_2438_0 = ! _rtB -> B_1036_2437_0_i ; } _rtB -> B_1036_2439_0 = ( _rtB
-> B_1036_2436_0_a && _rtB -> B_1036_2438_0 ) ; _rtB -> B_1036_2440_0 = (
_rtB -> B_1036_2397_0_n || _rtB -> B_1036_2439_0 ) ; isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2441_0_p = _rtDW ->
Memory_PreviousInput_gc ; _rtB -> B_1036_2442_0_m = ! _rtB -> B_1036_2441_0_p
; } _rtB -> B_1036_2443_0_b = ( _rtB -> B_1036_2393_0_o && _rtB ->
B_1036_2442_0_m ) ; _rtB -> B_1036_2444_0_e = ! _rtB -> B_1036_2443_0_b ;
_rtB -> B_1036_2445_0_f = ( _rtB -> B_1036_2440_0 && _rtB -> B_1036_2444_0_e
) ; _rtB -> B_1036_2446_0 = ( _rtB -> B_1036_2445_0_f || _rtB ->
B_1036_134_0_m || _rtB -> B_1036_2396_0_l ) ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2447_0 = _rtDW ->
Memory_PreviousInput_op ; _rtB -> B_1036_2448_0 = ! _rtB -> B_1036_2447_0 ; }
_rtB -> B_1036_2449_0_m = ( _rtB -> B_1036_2445_0_f && _rtB -> B_1036_2448_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_2450_0_a = _rtDW -> u4_PreviousInput_ay ; } _rtB -> B_1036_2451_0_o =
! _rtB -> B_1036_2364_0 ; _rtB -> B_1036_2452_0_l = ( _rtB -> B_1036_2342_0
&& _rtB -> B_1036_2451_0_o && _rtB -> B_1036_2384_0_g ) ; _rtB ->
B_1036_2453_0_p = ! _rtB -> B_1036_2342_0 ; _rtB -> B_1036_2454_0_d = ( _rtB
-> B_1036_2453_0_p && _rtB -> B_1036_2364_0 && _rtB -> B_1036_2384_0_g ) ;
_rtB -> B_1036_2455_0_b = ( _rtB -> B_1036_2452_0_l || _rtB ->
B_1036_2454_0_d ) ; _rtB -> B_1036_2456_0 = ! _rtB -> B_1036_2425_0_m ; _rtB
-> B_1036_2457_0_d = ( _rtB -> B_1036_2426_0_e && _rtB -> B_1036_2456_0 ) ;
_rtB -> B_1036_2458_0_d = ! _rtB -> B_1036_2455_0_b ; _rtB -> B_1036_2459_0_e
= ( _rtB -> B_1036_2457_0_d && _rtB -> B_1036_2458_0_d ) ; _rtB ->
B_1036_2460_0_l = ! _rtB -> B_1036_2425_0_m ; _rtB -> B_1036_2461_0_k = (
_rtB -> B_1036_2429_0_c && _rtB -> B_1036_2460_0_l ) ; _rtB -> B_1036_2462_0
= ! _rtB -> B_1036_2455_0_b ; _rtB -> B_1036_2463_0_l = ( _rtB ->
B_1036_2461_0_k && _rtB -> B_1036_2462_0 ) ; _rtB -> B_1036_2464_0_j = ! _rtB
-> B_1036_2425_0_m ; _rtB -> B_1036_2465_0_h = ( _rtB -> B_1036_2432_0_d &&
_rtB -> B_1036_2464_0_j ) ; _rtB -> B_1036_2466_0_k = ! _rtB ->
B_1036_2455_0_b ; _rtB -> B_1036_2467_0 = ( _rtB -> B_1036_2465_0_h && _rtB
-> B_1036_2466_0_k ) ; _rtB -> B_1036_2468_0_e = ( _rtB -> B_1036_2455_0_b ||
_rtB -> B_1036_2459_0_e || _rtB -> B_1036_2463_0_l || _rtB -> B_1036_2467_0 )
; _rtB -> B_1036_2469_0_g = false ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if
( isHit != 0 ) { _rtB -> B_1036_2470_0_c = _rtDW -> Memory_PreviousInput_dm ;
_rtB -> B_1036_2471_0_o = ! _rtB -> B_1036_2470_0_c ; } _rtB ->
B_1036_2472_0_l = ( _rtB -> B_1036_2469_0_g && _rtB -> B_1036_2471_0_o ) ;
_rtB -> B_1036_2473_0_e = ( _rtB -> B_1036_2450_0_a || _rtB ->
B_1036_2472_0_l ) ; _rtB -> B_1036_2474_0_m = ! _rtB -> B_1036_2443_0_b ;
_rtB -> B_1036_2475_0_o = ( _rtB -> B_1036_2473_0_e && _rtB ->
B_1036_2474_0_m ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_2476_0_e = _rtDW -> Memory_PreviousInput_ll ; _rtB ->
B_1036_2477_0_m = ! _rtB -> B_1036_2476_0_e ; } _rtB -> B_1036_2478_0 = (
_rtB -> B_1036_2475_0_o && _rtB -> B_1036_2477_0_m ) ; _rtB ->
B_1036_2479_0_j = ( _rtB -> B_1036_2449_0_m || _rtB -> B_1036_2478_0 ) ; _rtB
-> B_1036_2480_0_h = ( _rtB -> B_1036_2394_0_e || _rtB -> B_1036_135_0_d ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_2481_0_n = _rtDW -> u4_PreviousInput_j ; } _rtB -> B_1036_2482_0_j = (
_rtB -> B_1036_2481_0_n || _rtB -> B_1036_2446_0 ) ; _rtB -> B_1036_2483_0 =
! _rtB -> B_1036_2480_0_h ; _rtB -> B_1036_2484_0_d = ( _rtB ->
B_1036_2482_0_j && _rtB -> B_1036_2483_0 ) ; _rtB -> B_1036_2485_0 = _rtB ->
B_1036_711_0 * _rtB -> B_1036_711_0 ; _rtB -> B_1036_2486_0 = _rtB ->
B_1036_733_0 * _rtB -> B_1036_733_0 ; _rtB -> B_1036_2487_0 = _rtB ->
B_1036_2485_0 + _rtB -> B_1036_2486_0 ; _rtB -> B_1036_2488_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_2487_0 ) ; if ( _rtB -> B_1036_197_0_k >
_rtP -> P_564 ) { _rtB -> B_70_0_0 = ( _rtB -> B_1036_2488_0 < _rtB ->
B_1036_200_0 ) ; _rtB -> B_1036_2491_0_j = _rtB -> B_70_0_0 ; } else { _rtB
-> B_69_0_0 = ( _rtB -> B_1036_2488_0 > _rtB -> B_1036_200_0 ) ; _rtB ->
B_1036_2491_0_j = _rtB -> B_69_0_0 ; } if ( _rtB -> B_1036_358_0_l > _rtP ->
P_565 ) { _rtB -> B_61_0_0 = _rtB -> B_1036_711_0 + _rtB -> B_1036_716_0 ;
_rtB -> B_61_1_0 = _rtB -> B_61_0_0 + _rtB -> B_1036_705_0 ; _rtB -> B_61_2_0
= _rtB -> B_61_1_0 * _rtB -> B_1036_360_0_k ; _rtB -> B_1036_2493_0 = _rtB ->
B_61_2_0 ; } else { _rtB -> B_1036_2493_0 = 0.0 ; } _rtB -> B_1036_2494_0 =
_rtB -> B_1036_2493_0 * _rtB -> B_1036_2493_0 ; if ( _rtB -> B_1036_358_0_l >
_rtP -> P_566 ) { _rtB -> B_60_0_0 = _rtB -> B_1036_733_0 + _rtB ->
B_1036_738_0 ; _rtB -> B_60_1_0 = _rtB -> B_60_0_0 + _rtB -> B_1036_727_0 ;
_rtB -> B_60_2_0 = _rtB -> B_60_1_0 * _rtB -> B_1036_360_0_k ; _rtB ->
B_1036_2496_0 = _rtB -> B_60_2_0 ; } else { _rtB -> B_1036_2496_0 = 0.0 ; }
_rtB -> B_1036_2497_0 = _rtB -> B_1036_2496_0 * _rtB -> B_1036_2496_0 ; _rtB
-> B_1036_2498_0 = _rtB -> B_1036_2494_0 + _rtB -> B_1036_2497_0 ; _rtB ->
B_1036_2499_0 = muDoubleScalarSqrt ( _rtB -> B_1036_2498_0 ) ; if ( _rtB ->
B_1036_223_0_c > _rtP -> P_567 ) { _rtB -> B_73_0_0 = ( _rtB -> B_1036_2499_0
< _rtB -> B_1036_226_0_k ) ; _rtB -> B_1036_2502_0 = _rtB -> B_73_0_0 ; }
else { _rtB -> B_72_0_0 = ( _rtB -> B_1036_2499_0 > _rtB -> B_1036_226_0_k )
; _rtB -> B_1036_2502_0 = _rtB -> B_72_0_0 ; } _rtB -> B_1036_2503_0 = ( _rtB
-> B_1036_2502_0 && _rtB -> B_1036_228_0_l ) ; if ( _rtB -> B_1036_220_0 ) {
_rtB -> B_1036_2505_0_k = _rtB -> B_1036_2503_0 ; } else { _rtB -> B_71_0_0 =
( _rtB -> B_1036_2503_0 && _rtB -> B_1036_232_0_i ) ; _rtB -> B_1036_2505_0_k
= _rtB -> B_71_0_0 ; } _rtB -> B_1036_2506_0_b = ( _rtB -> B_1036_2505_0_k ||
( _rtB -> B_1036_193_0_p != 0.0 ) || ( _rtB -> B_1036_193_0_p != 0.0 ) || (
_rtB -> B_1036_193_0_p != 0.0 ) ) ; _rtB -> B_1036_2507_0 = _rtB ->
B_1036_705_0 * _rtB -> B_1036_705_0 ; _rtB -> B_1036_2508_0 = _rtB ->
B_1036_727_0 * _rtB -> B_1036_727_0 ; _rtB -> B_1036_2509_0 = _rtB ->
B_1036_2507_0 + _rtB -> B_1036_2508_0 ; _rtB -> B_1036_2510_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_2509_0 ) ; if ( _rtB -> B_1036_197_0_k >
_rtP -> P_568 ) { _rtB -> B_68_0_0 = ( _rtB -> B_1036_2510_0 < _rtB ->
B_1036_200_0 ) ; _rtB -> B_1036_2513_0_l = _rtB -> B_68_0_0 ; } else { _rtB
-> B_67_0_0 = ( _rtB -> B_1036_2510_0 > _rtB -> B_1036_200_0 ) ; _rtB ->
B_1036_2513_0_l = _rtB -> B_67_0_0 ; } _rtB -> B_1036_2514_0 = _rtB ->
B_1036_716_0 * _rtB -> B_1036_716_0 ; _rtB -> B_1036_2515_0 = _rtB ->
B_1036_738_0 * _rtB -> B_1036_738_0 ; _rtB -> B_1036_2516_0 = _rtB ->
B_1036_2514_0 + _rtB -> B_1036_2515_0 ; _rtB -> B_1036_2517_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_2516_0 ) ; if ( _rtB -> B_1036_197_0_k >
_rtP -> P_569 ) { _rtB -> B_66_0_0 = ( _rtB -> B_1036_2517_0 < _rtB ->
B_1036_200_0 ) ; _rtB -> B_1036_2520_0_j = _rtB -> B_66_0_0 ; } else { _rtB
-> B_65_0_0 = ( _rtB -> B_1036_2517_0 > _rtB -> B_1036_200_0 ) ; _rtB ->
B_1036_2520_0_j = _rtB -> B_65_0_0 ; } _rtB -> B_1036_2521_0_n = ( _rtB ->
B_1036_2491_0_j && _rtB -> B_1036_215_0_e ) ; _rtB -> B_1036_2522_0 = ( _rtB
-> B_1036_2506_0_b && _rtB -> B_1036_201_0_n ) ; _rtB -> B_1036_2523_0 = (
_rtB -> B_1036_2521_0_n && _rtB -> B_1036_2522_0 ) ; _rtB -> B_1036_2526_0 =
( _rtB -> B_1036_2513_0_l && _rtB -> B_1036_214_0_a ) ; _rtB -> B_1036_2527_0
= ( _rtB -> B_1036_2526_0 && _rtB -> B_1036_2522_0 ) ; if ( _rtB ->
B_1036_196_0_k ) { _rtB -> B_1036_2525_0 = _rtB -> B_1036_2523_0 ; } else {
_rtB -> B_64_0_0 = ( _rtB -> B_1036_2523_0 && _rtB -> B_1036_211_0_p ) ; _rtB
-> B_1036_2525_0 = _rtB -> B_64_0_0 ; } _rtB -> B_1036_2530_0_d = ( _rtB ->
B_1036_2520_0_j && _rtB -> B_1036_216_0_m ) ; _rtB -> B_1036_2531_0 = ( _rtB
-> B_1036_2530_0_d && _rtB -> B_1036_2522_0 ) ; if ( _rtB -> B_1036_196_0_k )
{ _rtB -> B_1036_2529_0_g = _rtB -> B_1036_2527_0 ; _rtB -> B_1036_2533_0 =
_rtB -> B_1036_2531_0 ; } else { _rtB -> B_63_0_0 = ( _rtB -> B_1036_2527_0
&& _rtB -> B_1036_207_0_o ) ; _rtB -> B_1036_2529_0_g = _rtB -> B_63_0_0 ;
_rtB -> B_62_0_0 = ( _rtB -> B_1036_2531_0 && _rtB -> B_1036_213_0_d ) ; _rtB
-> B_1036_2533_0 = _rtB -> B_62_0_0 ; } _rtB -> B_1036_2534_0 = ( _rtB ->
B_1036_2529_0_g || _rtB -> B_1036_2525_0 || _rtB -> B_1036_2533_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2535_0_o =
_rtDW -> Memory_PreviousInput_m4 ; _rtB -> B_1036_2536_0_a = ! _rtB ->
B_1036_2535_0_o ; } _rtB -> B_1036_2537_0_e = ( _rtB -> B_1036_2534_0 && _rtB
-> B_1036_2536_0_a ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_1036_2538_0 = _rtDW -> Memory_PreviousInput_nn ; _rtB ->
B_1036_2539_0 = ! _rtB -> B_1036_2538_0 ; } _rtB -> B_1036_2540_0 = ( _rtB ->
B_1036_2505_0_k && _rtB -> B_1036_2539_0 ) ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2541_0 = _rtDW -> u4_PreviousInput_p
; _rtB -> B_1036_2542_0 = _rtDW -> Memory_PreviousInput_bx ; _rtB ->
B_1036_2543_0 = ! _rtB -> B_1036_2542_0 ; } if ( _rtB -> B_1036_238_0 > _rtP
-> P_570 ) { _rtB -> B_139_0_0 = ( _rtB -> B_1036_2529_0_g && _rtB ->
B_1036_2543_0 ) ; _rtB -> B_1036_2545_0 = _rtB -> B_139_0_0 ; } else { _rtB
-> B_1036_2545_0 = _rtB -> B_1036_2529_0_g ; } _rtB -> B_1036_2546_0 = _rtB
-> B_1036_907_0 ; _rtB -> B_1036_2547_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2548_0 =
_rtB -> B_1036_297_0_l ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_2548_0 , _rtB
-> B_1036_2546_0 , _rtB -> B_1036_2547_0 , _rtB -> B_1036_295_0 , & _rtB ->
ONDelay_p , & _rtDW -> ONDelay_p , & _rtZCE -> ONDelay_p ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2550_0 =
_rtB -> B_1036_298_0_p ; } DZG_v3_OFFDelay ( S , _rtB -> B_1036_2550_0 , _rtB
-> B_1036_2546_0 , _rtB -> B_1036_2547_0 , _rtB -> B_1036_295_0 , & _rtB ->
OFFDelay_oq , & _rtDW -> OFFDelay_oq , & _rtZCE -> OFFDelay_oq ) ; _rtB ->
B_1036_2552_0 = ( _rtB -> ONDelay_p . B_90_17_0 || _rtB -> OFFDelay_oq .
B_86_19_0 ) ; _rtB -> B_1036_2553_0 = ( _rtB -> B_1036_2545_0 && _rtB ->
B_1036_2552_0 ) ; _rtB -> B_1036_2554_0 = ( _rtB -> B_1036_2541_0 || _rtB ->
B_1036_2553_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_2555_0 = _rtDW -> u2_PreviousInput_k ; } _rtB -> B_1036_2556_0
= ! _rtB -> B_1036_2529_0_g ; _rtB -> B_1036_2557_0 = ( _rtB -> B_1036_2555_0
&& _rtB -> B_1036_2556_0 ) ; _rtB -> B_1036_2558_0 = _rtB -> B_1036_2557_0 ;
_rtB -> B_1036_2559_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_2560_0 = _rtB -> B_1036_304_0 ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_2560_0 , _rtB -> B_1036_2558_0 , _rtB ->
B_1036_2559_0 , _rtB -> B_1036_302_0 , & _rtB -> ONDelay_b , & _rtDW ->
ONDelay_b , & _rtZCE -> ONDelay_b ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_2562_0 = _rtB -> B_1036_305_0_l ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_2562_0 , _rtB -> B_1036_2558_0 , _rtB ->
B_1036_2559_0 , _rtB -> B_1036_302_0 , & _rtB -> OFFDelay_p , & _rtDW ->
OFFDelay_p , & _rtZCE -> OFFDelay_p ) ; _rtB -> B_1036_2564_0 = ( _rtB ->
ONDelay_b . B_90_17_0 || _rtB -> OFFDelay_p . B_86_19_0 ) ; _rtB ->
B_1036_2565_0 = ! _rtB -> B_1036_2564_0 ; _rtB -> B_1036_2566_0 = ( _rtB ->
B_1036_2554_0 && _rtB -> B_1036_2565_0 ) ; _rtB -> B_1036_2567_0 = ( _rtB ->
B_1036_242_0_f && _rtB -> B_1036_2566_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_2568_0 = _rtDW -> u4_PreviousInput_mm
; _rtB -> B_1036_2569_0 = _rtDW -> Memory_PreviousInput_cp ; _rtB ->
B_1036_2570_0 = ! _rtB -> B_1036_2569_0 ; } if ( _rtB -> B_1036_238_0 > _rtP
-> P_571 ) { _rtB -> B_156_0_0 = ( _rtB -> B_1036_2525_0 && _rtB ->
B_1036_2570_0 ) ; _rtB -> B_1036_2572_0 = _rtB -> B_156_0_0 ; } else { _rtB
-> B_1036_2572_0 = _rtB -> B_1036_2525_0 ; } _rtB -> B_1036_2573_0 = _rtB ->
B_1036_1308_0_k ; _rtB -> B_1036_2574_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2575_0 =
_rtB -> B_1036_311_0_p ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_2575_0 , _rtB
-> B_1036_2573_0 , _rtB -> B_1036_2574_0 , _rtB -> B_1036_309_0 , & _rtB ->
ONDelay_f , & _rtDW -> ONDelay_f , & _rtZCE -> ONDelay_f ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2577_0 =
_rtB -> B_1036_312_0 ; } DZG_v3_OFFDelay ( S , _rtB -> B_1036_2577_0 , _rtB
-> B_1036_2573_0 , _rtB -> B_1036_2574_0 , _rtB -> B_1036_309_0 , & _rtB ->
OFFDelay_e , & _rtDW -> OFFDelay_e , & _rtZCE -> OFFDelay_e ) ; _rtB ->
B_1036_2579_0 = ( _rtB -> ONDelay_f . B_90_17_0 || _rtB -> OFFDelay_e .
B_86_19_0 ) ; _rtB -> B_1036_2580_0 = ( _rtB -> B_1036_2572_0 && _rtB ->
B_1036_2579_0 ) ; _rtB -> B_1036_2581_0 = ( _rtB -> B_1036_2568_0 || _rtB ->
B_1036_2580_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_2582_0 = _rtDW -> u2_PreviousInput_i ; } _rtB -> B_1036_2583_0
= ! _rtB -> B_1036_2525_0 ; _rtB -> B_1036_2584_0 = ( _rtB -> B_1036_2582_0
&& _rtB -> B_1036_2583_0 ) ; _rtB -> B_1036_2585_0 = _rtB -> B_1036_2584_0 ;
_rtB -> B_1036_2586_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_2587_0 = _rtB -> B_1036_318_0_m ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_2587_0 , _rtB -> B_1036_2585_0 , _rtB ->
B_1036_2586_0 , _rtB -> B_1036_316_0_d , & _rtB -> ONDelay_e5 , & _rtDW ->
ONDelay_e5 , & _rtZCE -> ONDelay_e5 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_2589_0 = _rtB -> B_1036_319_0_c ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_2589_0 , _rtB -> B_1036_2585_0 , _rtB ->
B_1036_2586_0 , _rtB -> B_1036_316_0_d , & _rtB -> OFFDelay_ip , & _rtDW ->
OFFDelay_ip , & _rtZCE -> OFFDelay_ip ) ; _rtB -> B_1036_2591_0 = ( _rtB ->
ONDelay_e5 . B_90_17_0 || _rtB -> OFFDelay_ip . B_86_19_0 ) ; _rtB ->
B_1036_2592_0 = ! _rtB -> B_1036_2591_0 ; _rtB -> B_1036_2593_0 = ( _rtB ->
B_1036_2581_0 && _rtB -> B_1036_2592_0 ) ; _rtB -> B_1036_2594_0 = ( _rtB ->
B_1036_243_0_d && _rtB -> B_1036_2593_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_2595_0 = _rtDW -> u4_PreviousInput_h ;
_rtB -> B_1036_2596_0_o = _rtDW -> Memory_PreviousInput_kl ; _rtB ->
B_1036_2597_0_i = ! _rtB -> B_1036_2596_0_o ; } if ( _rtB -> B_1036_238_0 >
_rtP -> P_572 ) { _rtB -> B_173_0_0 = ( _rtB -> B_1036_2533_0 && _rtB ->
B_1036_2597_0_i ) ; _rtB -> B_1036_2599_0_b = _rtB -> B_173_0_0 ; } else {
_rtB -> B_1036_2599_0_b = _rtB -> B_1036_2533_0 ; } _rtB -> B_1036_2600_0_a =
_rtB -> B_1036_1544_0_g ; _rtB -> B_1036_2601_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2602_0_j =
_rtB -> B_1036_325_0_a ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_2602_0_j ,
_rtB -> B_1036_2600_0_a , _rtB -> B_1036_2601_0 , _rtB -> B_1036_323_0_n , &
_rtB -> ONDelay_n , & _rtDW -> ONDelay_n , & _rtZCE -> ONDelay_n ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2604_0_a =
_rtB -> B_1036_326_0_g ; } DZG_v3_OFFDelay ( S , _rtB -> B_1036_2604_0_a ,
_rtB -> B_1036_2600_0_a , _rtB -> B_1036_2601_0 , _rtB -> B_1036_323_0_n , &
_rtB -> OFFDelay_dc , & _rtDW -> OFFDelay_dc , & _rtZCE -> OFFDelay_dc ) ;
_rtB -> B_1036_2606_0_g = ( _rtB -> ONDelay_n . B_90_17_0 || _rtB ->
OFFDelay_dc . B_86_19_0 ) ; _rtB -> B_1036_2607_0_o = ( _rtB ->
B_1036_2599_0_b && _rtB -> B_1036_2606_0_g ) ; _rtB -> B_1036_2608_0_a = (
_rtB -> B_1036_2595_0 || _rtB -> B_1036_2607_0_o ) ; isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2609_0_c = _rtDW ->
u2_PreviousInput_d ; } _rtB -> B_1036_2610_0_d = ! _rtB -> B_1036_2533_0 ;
_rtB -> B_1036_2611_0_d = ( _rtB -> B_1036_2609_0_c && _rtB ->
B_1036_2610_0_d ) ; _rtB -> B_1036_2612_0_l = _rtB -> B_1036_2611_0_d ; _rtB
-> B_1036_2613_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_2614_0_d = _rtB -> B_1036_332_0_c ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_2614_0_d , _rtB -> B_1036_2612_0_l , _rtB
-> B_1036_2613_0 , _rtB -> B_1036_330_0_c , & _rtB -> ONDelay_j , & _rtDW ->
ONDelay_j , & _rtZCE -> ONDelay_j ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_2616_0_j = _rtB -> B_1036_333_0_c ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_2616_0_j , _rtB -> B_1036_2612_0_l ,
_rtB -> B_1036_2613_0 , _rtB -> B_1036_330_0_c , & _rtB -> OFFDelay_n , &
_rtDW -> OFFDelay_n , & _rtZCE -> OFFDelay_n ) ; _rtB -> B_1036_2618_0_k = (
_rtB -> ONDelay_j . B_90_17_0 || _rtB -> OFFDelay_n . B_86_19_0 ) ; _rtB ->
B_1036_2619_0_d = ! _rtB -> B_1036_2618_0_k ; _rtB -> B_1036_2620_0_c = (
_rtB -> B_1036_2608_0_a && _rtB -> B_1036_2619_0_d ) ; _rtB ->
B_1036_2621_0_k = ( _rtB -> B_1036_244_0_i && _rtB -> B_1036_2620_0_c ) ;
_rtB -> B_1036_2622_0_p = ( _rtB -> B_1036_2567_0 || _rtB -> B_1036_2594_0 ||
_rtB -> B_1036_2621_0_k ) ; if ( _rtB -> B_1036_339_0_b ) { _rtB -> B_191_0_0
= ( _rtB -> B_1036_2567_0 || _rtB -> B_1036_2594_0 || _rtB -> B_1036_2621_0_k
) ; _rtB -> B_1036_2625_0_e = _rtB -> B_191_0_0 ; } else { _rtB -> B_190_0_0
= ( _rtB -> B_1036_2567_0 && _rtB -> B_1036_2594_0 && _rtB -> B_1036_2621_0_k
) ; _rtB -> B_190_1_0 = ( ( _rtB -> B_1036_236_0 != 0.0 ) && _rtB ->
B_190_0_0 ) ; _rtB -> B_190_2_0 = ! _rtB -> B_1036_2621_0_k ; _rtB ->
B_190_3_0 = ( _rtB -> B_1036_2567_0 && _rtB -> B_1036_2594_0 && _rtB ->
B_190_2_0 ) ; _rtB -> B_190_4_0 = ! _rtB -> B_1036_2594_0 ; _rtB -> B_190_5_0
= ( _rtB -> B_1036_2567_0 && _rtB -> B_190_4_0 && _rtB -> B_1036_2621_0_k ) ;
_rtB -> B_190_6_0 = ! _rtB -> B_1036_2567_0 ; _rtB -> B_190_7_0 = ( _rtB ->
B_190_6_0 && _rtB -> B_1036_2594_0 && _rtB -> B_1036_2621_0_k ) ; _rtB ->
B_190_8_0 = ( _rtB -> B_190_7_0 || _rtB -> B_190_5_0 || _rtB -> B_190_3_0 ) ;
_rtB -> B_190_9_0 = ( ( _rtB -> B_1036_235_0_p != 0.0 ) && _rtB -> B_190_8_0
) ; _rtB -> B_190_10_0 = ! _rtB -> B_1036_2594_0 ; _rtB -> B_190_11_0 = !
_rtB -> B_1036_2567_0 ; _rtB -> B_190_12_0 = ( _rtB -> B_190_11_0 && _rtB ->
B_190_10_0 && _rtB -> B_1036_2621_0_k ) ; _rtB -> B_190_13_0 = ! _rtB ->
B_1036_2621_0_k ; _rtB -> B_190_14_0 = ! _rtB -> B_1036_2567_0 ; _rtB ->
B_190_15_0 = ( _rtB -> B_190_14_0 && _rtB -> B_1036_2594_0 && _rtB ->
B_190_13_0 ) ; _rtB -> B_190_16_0 = ! _rtB -> B_1036_2621_0_k ; _rtB ->
B_190_17_0 = ! _rtB -> B_1036_2594_0 ; _rtB -> B_190_18_0 = ( _rtB ->
B_1036_2567_0 && _rtB -> B_190_17_0 && _rtB -> B_190_16_0 ) ; _rtB ->
B_190_19_0 = ( _rtB -> B_190_18_0 || _rtB -> B_190_15_0 || _rtB -> B_190_12_0
) ; _rtB -> B_190_20_0 = ( ( _rtB -> B_1036_237_0_d != 0.0 ) && _rtB ->
B_190_19_0 ) ; _rtB -> B_190_21_0 = ( _rtB -> B_190_20_0 || _rtB -> B_190_9_0
|| _rtB -> B_190_1_0 ) ; _rtB -> B_1036_2625_0_e = _rtB -> B_190_21_0 ; }
_rtB -> B_1036_2626_0_i = ( _rtB -> B_1036_2625_0_e && _rtB ->
B_1036_2621_0_k ) ; _rtB -> B_1036_2627_0_b = ( _rtB -> B_1036_2626_0_i || (
_rtB -> B_1036_233_0_m != 0.0 ) ) ; _rtB -> B_1036_2628_0_i = _rtB ->
B_1036_2627_0_b ; _rtB -> B_1036_2629_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2630_0_h =
_rtB -> B_1036_269_0_l ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_2630_0_h ,
_rtB -> B_1036_2628_0_i , _rtB -> B_1036_2629_0 , _rtB -> B_1036_267_0_d , &
_rtB -> ONDelay_eu , & _rtDW -> ONDelay_eu , & _rtZCE -> ONDelay_eu ) ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2632_0_d =
_rtB -> B_1036_270_0_g ; } DZG_v3_OFFDelay ( S , _rtB -> B_1036_2632_0_d ,
_rtB -> B_1036_2628_0_i , _rtB -> B_1036_2629_0 , _rtB -> B_1036_267_0_d , &
_rtB -> OFFDelay_i , & _rtDW -> OFFDelay_i , & _rtZCE -> OFFDelay_i ) ; _rtB
-> B_1036_2634_0_g = _rtB -> B_1036_240_0_p ; _rtB -> B_1036_2635_0 = ssGetT
( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_2636_0_m = _rtB -> B_1036_290_0 ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_2636_0_m , _rtB -> B_1036_2634_0_g , _rtB -> B_1036_2635_0 , _rtB ->
B_1036_288_0_c , & _rtB -> ONDelay_k , & _rtDW -> ONDelay_k , & _rtZCE ->
ONDelay_k ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_2638_0_g = _rtB -> B_1036_291_0_n ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_2638_0_g , _rtB -> B_1036_2634_0_g , _rtB -> B_1036_2635_0 , _rtB ->
B_1036_288_0_c , & _rtB -> OFFDelay_c , & _rtDW -> OFFDelay_c , & _rtZCE ->
OFFDelay_c ) ; _rtB -> B_1036_2640_0_h = ( _rtB -> ONDelay_k . B_90_17_0 ||
_rtB -> OFFDelay_c . B_86_19_0 ) ; _rtB -> B_1036_2641_0_a = _rtB ->
B_1036_2627_0_b ; _rtB -> B_1036_2642_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2643_0_e =
_rtB -> B_1036_255_0_h ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_2643_0_e ,
_rtB -> B_1036_2641_0_a , _rtB -> B_1036_2642_0 , _rtB -> B_1036_253_0_i , &
_rtB -> ONDelay_o , & _rtDW -> ONDelay_o , & _rtZCE -> ONDelay_o ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2645_0_i =
_rtB -> B_1036_256_0 ; } DZG_v3_OFFDelay ( S , _rtB -> B_1036_2645_0_i , _rtB
-> B_1036_2641_0_a , _rtB -> B_1036_2642_0 , _rtB -> B_1036_253_0_i , & _rtB
-> OFFDelay_d , & _rtDW -> OFFDelay_d , & _rtZCE -> OFFDelay_d ) ; if ( _rtB
-> B_1036_241_0_o ) { _rtB -> B_1036_2648_0 = _rtB -> B_1036_2627_0_b ; }
else { if ( _rtB -> B_1036_2640_0_h ) { _rtB -> B_78_0_0 = ( _rtB ->
ONDelay_eu . B_90_17_0 || _rtB -> OFFDelay_i . B_86_19_0 ) ; _rtB -> B_79_2_0
= _rtB -> B_78_0_0 ; } else { _rtB -> B_77_0_0 = ( _rtB -> ONDelay_o .
B_90_17_0 || _rtB -> OFFDelay_d . B_86_19_0 ) ; _rtB -> B_79_2_0 = _rtB ->
B_77_0_0 ; } _rtB -> B_1036_2648_0 = _rtB -> B_79_2_0 ; } _rtB ->
B_1036_2649_0 = ( _rtB -> B_1036_2533_0 && _rtB -> B_1036_2648_0 && _rtB ->
B_1036_2625_0_e ) ; _rtB -> B_1036_2650_0 = ( _rtB -> B_1036_2625_0_e && _rtB
-> B_1036_2567_0 ) ; _rtB -> B_1036_2651_0 = ( _rtB -> B_1036_2625_0_e &&
_rtB -> B_1036_2594_0 ) ; _rtB -> B_1036_2652_0 = ( _rtB -> B_1036_2650_0 ||
( _rtB -> B_1036_233_0_m != 0.0 ) ) ; _rtB -> B_1036_2653_0 = ( _rtB ->
B_1036_2651_0 || ( _rtB -> B_1036_233_0_m != 0.0 ) ) ; _rtB -> B_1036_2654_0
= _rtB -> B_1036_2652_0 ; _rtB -> B_1036_2655_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2656_0 =
_rtB -> B_1036_283_0_n ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_2656_0 , _rtB
-> B_1036_2654_0 , _rtB -> B_1036_2655_0 , _rtB -> B_1036_281_0 , & _rtB ->
ONDelay_h , & _rtDW -> ONDelay_h , & _rtZCE -> ONDelay_h ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2658_0_h =
_rtB -> B_1036_284_0 ; } DZG_v3_OFFDelay ( S , _rtB -> B_1036_2658_0_h , _rtB
-> B_1036_2654_0 , _rtB -> B_1036_2655_0 , _rtB -> B_1036_281_0 , & _rtB ->
OFFDelay_g , & _rtDW -> OFFDelay_g , & _rtZCE -> OFFDelay_g ) ; _rtB ->
B_1036_2660_0_k = _rtB -> B_1036_2652_0 ; _rtB -> B_1036_2661_0 = ssGetT ( S
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_2662_0 = _rtB -> B_1036_262_0_f ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_2662_0 , _rtB -> B_1036_2660_0_k , _rtB -> B_1036_2661_0 , _rtB ->
B_1036_260_0_g , & _rtB -> ONDelay_e , & _rtDW -> ONDelay_e , & _rtZCE ->
ONDelay_e ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_2664_0_f = _rtB -> B_1036_263_0_h ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_2664_0_f , _rtB -> B_1036_2660_0_k , _rtB -> B_1036_2661_0 , _rtB ->
B_1036_260_0_g , & _rtB -> OFFDelay_o , & _rtDW -> OFFDelay_o , & _rtZCE ->
OFFDelay_o ) ; if ( _rtB -> B_1036_241_0_o ) { _rtB -> B_1036_2667_0_l = _rtB
-> B_1036_2652_0 ; } else { if ( _rtB -> B_1036_2640_0_h ) { _rtB -> B_75_0_0
= ( _rtB -> ONDelay_h . B_90_17_0 || _rtB -> OFFDelay_g . B_86_19_0 ) ; _rtB
-> B_76_2_0 = _rtB -> B_75_0_0 ; } else { _rtB -> B_74_0_0 = ( _rtB ->
ONDelay_e . B_90_17_0 || _rtB -> OFFDelay_o . B_86_19_0 ) ; _rtB -> B_76_2_0
= _rtB -> B_74_0_0 ; } _rtB -> B_1036_2667_0_l = _rtB -> B_76_2_0 ; } _rtB ->
B_1036_2668_0_o = ( _rtB -> B_1036_2529_0_g && _rtB -> B_1036_2667_0_l &&
_rtB -> B_1036_2625_0_e ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_1036_2669_0 = _rtDW -> Memory_PreviousInput_av ; _rtB ->
B_1036_2670_0 = ! _rtB -> B_1036_2669_0 ; } _rtB -> B_1036_2671_0 = ( _rtB ->
B_1036_2668_0_o && _rtB -> B_1036_2670_0 ) ; _rtB -> B_1036_2672_0 = ssGetT (
S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_2673_0_l = _rtDW -> Memory_PreviousInput_dw ; _rtB -> B_1036_2674_0_d
= ! _rtB -> B_1036_2673_0_l ; } _rtB -> B_1036_2675_0_a = _rtB ->
B_1036_2668_0_o ; _rtB -> B_1036_2676_0_j = _rtB -> B_1036_2675_0_a ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2677_0_o =
_rtDW -> Memory_PreviousInput_pa ; _rtB -> B_1036_2678_0 = _rtB ->
B_1036_345_0_h [ 0 ] ; } DZG_v3_POSITIVEEdge ( S , _rtB -> B_1036_2678_0 ,
_rtB -> B_1036_2676_0_j , _rtB -> B_1036_2677_0_o , & _rtB -> POSITIVEEdge ,
& _rtDW -> POSITIVEEdge ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_1036_2680_0 = _rtB -> B_1036_345_0_h [ 1 ] ; }
DZG_v3_NEGATIVEEdge ( S , _rtB -> B_1036_2680_0 , _rtB -> B_1036_2676_0_j ,
_rtB -> B_1036_2677_0_o , & _rtB -> NEGATIVEEdge , & _rtDW -> NEGATIVEEdge )
; _rtB -> B_1036_2682_0 = ( _rtB -> POSITIVEEdge . B_85_0_0 || _rtB ->
NEGATIVEEdge . B_84_0_0 ) ; _rtB -> B_1036_2683_0 = ( _rtB -> B_1036_2674_0_d
&& _rtB -> B_1036_2682_0 ) ; _rtB -> B_1036_2684_0 = _rtB -> B_1036_2683_0 ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_2685_0 = _rtDW -> ICic_PreviousInput_g ; } if ( _rtB -> B_1036_2684_0
) { _rtB -> B_1036_2686_0 = _rtB -> B_1036_2672_0 ; } else { _rtB ->
B_1036_2686_0 = _rtB -> B_1036_2685_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_2687_0 = _rtP -> P_574 ; } _rtB ->
B_1036_2688_0 = _rtB -> B_1036_2686_0 + _rtB -> B_1036_2687_0 ; _rtB ->
B_1036_2689_0 = ( _rtB -> B_1036_2688_0 > _rtB -> B_1036_2672_0 ) ; _rtB ->
B_1036_2690_0 = ( _rtB -> B_1036_2671_0 || _rtB -> B_1036_2689_0 ) ; _rtB ->
B_1036_2691_0 = _rtB -> B_1036_2653_0 ; _rtB -> B_1036_2692_0 = ssGetT ( S )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_2693_0 = _rtB -> B_1036_276_0_l ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_2693_0 , _rtB -> B_1036_2691_0 , _rtB -> B_1036_2692_0 , _rtB ->
B_1036_274_0_h , & _rtB -> ONDelay_l , & _rtDW -> ONDelay_l , & _rtZCE ->
ONDelay_l ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_2695_0 = _rtB -> B_1036_277_0_m ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_2695_0 , _rtB -> B_1036_2691_0 , _rtB -> B_1036_2692_0 , _rtB ->
B_1036_274_0_h , & _rtB -> OFFDelay_f , & _rtDW -> OFFDelay_f , & _rtZCE ->
OFFDelay_f ) ; _rtB -> B_1036_2697_0 = _rtB -> B_1036_2653_0 ; _rtB ->
B_1036_2698_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_2699_0 = _rtB -> B_1036_248_0 ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_2699_0 , _rtB -> B_1036_2697_0 , _rtB ->
B_1036_2698_0 , _rtB -> B_1036_246_0 , & _rtB -> ONDelay , & _rtDW -> ONDelay
, & _rtZCE -> ONDelay ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_1036_2701_0 = _rtB -> B_1036_249_0_i ; } DZG_v3_OFFDelay ( S
, _rtB -> B_1036_2701_0 , _rtB -> B_1036_2697_0 , _rtB -> B_1036_2698_0 ,
_rtB -> B_1036_246_0 , & _rtB -> OFFDelay , & _rtDW -> OFFDelay , & _rtZCE ->
OFFDelay ) ; if ( _rtB -> B_1036_241_0_o ) { _rtB -> B_1036_2704_0 = _rtB ->
B_1036_2653_0 ; } else { if ( _rtB -> B_1036_2640_0_h ) { _rtB -> B_81_0_0 =
( _rtB -> ONDelay_l . B_90_17_0 || _rtB -> OFFDelay_f . B_86_19_0 ) ; _rtB ->
B_82_2_0 = _rtB -> B_81_0_0 ; } else { _rtB -> B_80_0_0 = ( _rtB -> ONDelay .
B_90_17_0 || _rtB -> OFFDelay . B_86_19_0 ) ; _rtB -> B_82_2_0 = _rtB ->
B_80_0_0 ; } _rtB -> B_1036_2704_0 = _rtB -> B_82_2_0 ; } _rtB ->
B_1036_2705_0 = ( _rtB -> B_1036_2525_0 && _rtB -> B_1036_2704_0 && _rtB ->
B_1036_2625_0_e ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_2706_0 = _rtDW -> Memory_PreviousInput_e5 ; _rtB ->
B_1036_2707_0 = ! _rtB -> B_1036_2706_0 ; } _rtB -> B_1036_2708_0 = ( _rtB ->
B_1036_2705_0 && _rtB -> B_1036_2707_0 ) ; _rtB -> B_1036_2709_0 = ssGetT ( S
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_2710_0 = _rtDW -> Memory_PreviousInput_am ; _rtB -> B_1036_2711_0 = !
_rtB -> B_1036_2710_0 ; } _rtB -> B_1036_2712_0 = _rtB -> B_1036_2705_0 ;
_rtB -> B_1036_2713_0 = _rtB -> B_1036_2712_0 ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2714_0 = _rtDW ->
Memory_PreviousInput_ijz ; _rtB -> B_1036_2715_0 = _rtB -> B_1036_351_0 [ 0 ]
; } DZG_v3_POSITIVEEdge ( S , _rtB -> B_1036_2715_0 , _rtB -> B_1036_2713_0 ,
_rtB -> B_1036_2714_0 , & _rtB -> POSITIVEEdge_h , & _rtDW -> POSITIVEEdge_h
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_2717_0 = _rtB -> B_1036_351_0 [ 1 ] ; } DZG_v3_NEGATIVEEdge ( S , _rtB
-> B_1036_2717_0 , _rtB -> B_1036_2713_0 , _rtB -> B_1036_2714_0 , & _rtB ->
NEGATIVEEdge_o , & _rtDW -> NEGATIVEEdge_o ) ; _rtB -> B_1036_2719_0 = ( _rtB
-> POSITIVEEdge_h . B_85_0_0 || _rtB -> NEGATIVEEdge_o . B_84_0_0 ) ; _rtB ->
B_1036_2720_0 = ( _rtB -> B_1036_2711_0 && _rtB -> B_1036_2719_0 ) ; _rtB ->
B_1036_2721_0 = _rtB -> B_1036_2720_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_2722_0 = _rtDW -> ICic_PreviousInput_b ; }
if ( _rtB -> B_1036_2721_0 ) { _rtB -> B_1036_2723_0 = _rtB -> B_1036_2709_0
; } else { _rtB -> B_1036_2723_0 = _rtB -> B_1036_2722_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2724_0 =
_rtP -> P_576 ; } _rtB -> B_1036_2725_0 = _rtB -> B_1036_2723_0 + _rtB ->
B_1036_2724_0 ; _rtB -> B_1036_2726_0 = ( _rtB -> B_1036_2725_0 > _rtB ->
B_1036_2709_0 ) ; _rtB -> B_1036_2727_0 = ( _rtB -> B_1036_2708_0 || _rtB ->
B_1036_2726_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_2728_0 = _rtDW -> Memory_PreviousInput_gj ; _rtB ->
B_1036_2729_0 = ! _rtB -> B_1036_2728_0 ; } _rtB -> B_1036_2730_0 = ( _rtB ->
B_1036_2649_0 && _rtB -> B_1036_2729_0 ) ; _rtB -> B_1036_2731_0 = ssGetT ( S
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_2732_0 = _rtDW -> Memory_PreviousInput_j4 ; _rtB -> B_1036_2733_0 = !
_rtB -> B_1036_2732_0 ; } _rtB -> B_1036_2734_0 = _rtB -> B_1036_2649_0 ;
_rtB -> B_1036_2735_0 = _rtB -> B_1036_2734_0 ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2736_0 = _rtDW ->
Memory_PreviousInput_ai ; _rtB -> B_1036_2737_0 = _rtB -> B_1036_357_0_o [ 0
] ; } DZG_v3_POSITIVEEdge ( S , _rtB -> B_1036_2737_0 , _rtB -> B_1036_2735_0
, _rtB -> B_1036_2736_0 , & _rtB -> POSITIVEEdge_b , & _rtDW ->
POSITIVEEdge_b ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_2739_0 = _rtB -> B_1036_357_0_o [ 1 ] ; } DZG_v3_NEGATIVEEdge
( S , _rtB -> B_1036_2739_0 , _rtB -> B_1036_2735_0 , _rtB -> B_1036_2736_0 ,
& _rtB -> NEGATIVEEdge_g , & _rtDW -> NEGATIVEEdge_g ) ; _rtB ->
B_1036_2741_0 = ( _rtB -> POSITIVEEdge_b . B_85_0_0 || _rtB -> NEGATIVEEdge_g
. B_84_0_0 ) ; _rtB -> B_1036_2742_0 = ( _rtB -> B_1036_2733_0 && _rtB ->
B_1036_2741_0 ) ; _rtB -> B_1036_2743_0 = _rtB -> B_1036_2742_0 ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2744_0 =
_rtDW -> ICic_PreviousInput_o ; } if ( _rtB -> B_1036_2743_0 ) { _rtB ->
B_1036_2745_0 = _rtB -> B_1036_2731_0 ; } else { _rtB -> B_1036_2745_0 = _rtB
-> B_1036_2744_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_1036_2746_0 = _rtP -> P_578 ; } _rtB -> B_1036_2747_0 = _rtB ->
B_1036_2745_0 + _rtB -> B_1036_2746_0 ; _rtB -> B_1036_2748_0 = ( _rtB ->
B_1036_2747_0 > _rtB -> B_1036_2731_0 ) ; _rtB -> B_1036_2749_0 = ( _rtB ->
B_1036_2730_0 || _rtB -> B_1036_2748_0 ) ; _rtB -> B_1036_2750_0 = ( _rtB ->
B_1036_2690_0 || _rtB -> B_1036_2727_0 || _rtB -> B_1036_2749_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2751_0 =
_rtDW -> Memory_PreviousInput_aq ; _rtB -> B_1036_2752_0 = ! _rtB ->
B_1036_2751_0 ; } _rtB -> B_1036_2753_0 = ( _rtB -> B_1036_2750_0 && _rtB ->
B_1036_2752_0 ) ; _rtB -> B_1036_2754_0 = ssGetT ( S ) ; _rtB ->
B_1036_2755_0 = _rtB -> B_1036_2682_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { DZG_v3_TriggeredSubsystem ( S , _rtB -> B_1036_2755_0 ,
_rtB -> B_1036_2754_0 , & _rtB -> TriggeredSubsystem , & _rtDW ->
TriggeredSubsystem , & _rtZCE -> TriggeredSubsystem ) ; _rtB -> B_1036_2757_0
= _rtB -> TriggeredSubsystem . B_83_0_0 + _rtB -> B_1036_340_0_p ; } _rtB ->
B_1036_2758_0 = ( _rtB -> B_1036_2757_0 > _rtB -> B_1036_2754_0 ) ; _rtB ->
B_1036_2759_0 = ssGetT ( S ) ; _rtB -> B_1036_2760_0 = _rtB -> B_1036_2719_0
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
DZG_v3_TriggeredSubsystem ( S , _rtB -> B_1036_2760_0 , _rtB -> B_1036_2759_0
, & _rtB -> TriggeredSubsystem_d , & _rtDW -> TriggeredSubsystem_d , & _rtZCE
-> TriggeredSubsystem_d ) ; _rtB -> B_1036_2762_0 = _rtB ->
TriggeredSubsystem_d . B_83_0_0 + _rtB -> B_1036_346_0_d ; } _rtB ->
B_1036_2763_0 = ( _rtB -> B_1036_2762_0 > _rtB -> B_1036_2759_0 ) ; _rtB ->
B_1036_2764_0 = ssGetT ( S ) ; _rtB -> B_1036_2765_0 = _rtB -> B_1036_2741_0
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
DZG_v3_TriggeredSubsystem ( S , _rtB -> B_1036_2765_0 , _rtB -> B_1036_2764_0
, & _rtB -> TriggeredSubsystem_e , & _rtDW -> TriggeredSubsystem_e , & _rtZCE
-> TriggeredSubsystem_e ) ; _rtB -> B_1036_2767_0 = _rtB ->
TriggeredSubsystem_e . B_83_0_0 + _rtB -> B_1036_352_0_p ; } _rtB ->
B_1036_2768_0 = ( _rtB -> B_1036_2767_0 > _rtB -> B_1036_2764_0 ) ; _rtB ->
B_1036_2769_0 = _rtB -> B_1036_733_0 * _rtB -> B_1036_359_0 ; _rtB ->
B_1036_2770_0 = _rtB -> B_1036_711_0 * _rtB -> B_1036_362_0 ; _rtB ->
B_1036_2771_0 = _rtB -> B_1036_2769_0 + _rtB -> B_1036_2770_0 ; _rtB ->
B_1036_2772_0 = _rtB -> B_1036_738_0 * _rtB -> B_1036_359_0 ; _rtB ->
B_1036_2773_0 = _rtB -> B_1036_716_0 * _rtB -> B_1036_361_0_j ; _rtB ->
B_1036_2774_0 = _rtB -> B_1036_2772_0 + _rtB -> B_1036_2773_0 ; _rtB ->
B_1036_2775_0 = _rtB -> B_1036_2771_0 + _rtB -> B_1036_2774_0 ; _rtB ->
B_1036_2776_0 = _rtB -> B_1036_727_0 + _rtB -> B_1036_2775_0 ; _rtB ->
B_1036_2777_0 = _rtB -> B_1036_733_0 * _rtB -> B_1036_359_0 ; _rtB ->
B_1036_2778_0 = _rtB -> B_1036_711_0 * _rtB -> B_1036_361_0_j ; _rtB ->
B_1036_2779_0 = _rtB -> B_1036_2777_0 + _rtB -> B_1036_2778_0 ; _rtB ->
B_1036_2780_0 = _rtB -> B_1036_738_0 * _rtB -> B_1036_359_0 ; _rtB ->
B_1036_2781_0 = _rtB -> B_1036_716_0 * _rtB -> B_1036_362_0 ; _rtB ->
B_1036_2782_0 = _rtB -> B_1036_2780_0 + _rtB -> B_1036_2781_0 ; _rtB ->
B_1036_2783_0 = _rtB -> B_1036_2779_0 + _rtB -> B_1036_2782_0 ; _rtB ->
B_1036_2784_0 = _rtB -> B_1036_727_0 + _rtB -> B_1036_2783_0 ; _rtB ->
B_1036_2785_0 = _rtB -> B_1036_711_0 * _rtB -> B_1036_359_0 ; _rtB ->
B_1036_2786_0 = _rtB -> B_1036_733_0 * _rtB -> B_1036_361_0_j ; _rtB ->
B_1036_2787_0 = _rtB -> B_1036_2785_0 - _rtB -> B_1036_2786_0 ; _rtB ->
B_1036_2788_0 = _rtB -> B_1036_716_0 * _rtB -> B_1036_359_0 ; _rtB ->
B_1036_2789_0 = _rtB -> B_1036_738_0 * _rtB -> B_1036_362_0 ; _rtB ->
B_1036_2790_0 = _rtB -> B_1036_2788_0 - _rtB -> B_1036_2789_0 ; _rtB ->
B_1036_2791_0 = _rtB -> B_1036_2787_0 + _rtB -> B_1036_2790_0 ; _rtB ->
B_1036_2792_0 = _rtB -> B_1036_705_0 + _rtB -> B_1036_2791_0 ; _rtB ->
B_1036_2793_0 = _rtB -> B_1036_2792_0 * _rtB -> B_1036_360_0_k ; _rtB ->
B_1036_2794_0 = _rtB -> B_1036_711_0 * _rtB -> B_1036_359_0 ; _rtB ->
B_1036_2795_0 = _rtB -> B_1036_733_0 * _rtB -> B_1036_362_0 ; _rtB ->
B_1036_2796_0 = _rtB -> B_1036_2794_0 - _rtB -> B_1036_2795_0 ; _rtB ->
B_1036_2797_0 = _rtB -> B_1036_716_0 * _rtB -> B_1036_359_0 ; _rtB ->
B_1036_2798_0 = _rtB -> B_1036_738_0 * _rtB -> B_1036_361_0_j ; _rtB ->
B_1036_2799_0 = _rtB -> B_1036_2797_0 - _rtB -> B_1036_2798_0 ; _rtB ->
B_1036_2800_0 = _rtB -> B_1036_2796_0 + _rtB -> B_1036_2799_0 ; _rtB ->
B_1036_2801_0 = _rtB -> B_1036_705_0 + _rtB -> B_1036_2800_0 ; _rtB ->
B_1036_2802_0 = _rtB -> B_1036_2801_0 * _rtB -> B_1036_360_0_k ; _rtB ->
B_1036_2803_0 = _rtB -> B_1036_2776_0 * _rtB -> B_1036_360_0_k ; _rtB ->
B_1036_2804_0 = _rtB -> B_1036_2784_0 * _rtB -> B_1036_360_0_k ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2805_0 =
_rtDW -> u4_PreviousInput_e3 ; } _rtB -> B_1036_2806_0 = ! _rtB ->
B_1036_1218_0_i ; _rtB -> B_1036_2807_0 = ! _rtB -> B_1036_2806_0 ; _rtB ->
B_1036_2808_0 = ( _rtB -> B_1036_2805_0 && _rtB -> B_1036_2807_0 ) ; _rtB ->
B_1036_2809_0 = ( _rtB -> B_1036_1248_0_h || _rtB -> B_1036_2808_0 ) ; _rtB
-> B_1036_2810_0 = ( _rtB -> B_1036_2809_0 && _rtB -> B_1036_374_0_d ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_2811_0 = _rtDW -> Memory_PreviousInput_pb ; _rtB -> B_1036_2812_0 = !
_rtB -> B_1036_2811_0 ; } _rtB -> B_1036_2813_0 = ( _rtB -> B_1036_1233_0 &&
_rtB -> B_1036_2812_0 ) ; _rtB -> B_1036_2814_0 = ( _rtB -> B_1036_2813_0 &&
_rtB -> B_1036_364_0_l ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_1036_2815_0 = _rtDW -> u4_PreviousInput_gl3 ; } _rtB ->
B_1036_2816_0 = ! _rtB -> B_1036_1454_0_p ; _rtB -> B_1036_2817_0 = ! _rtB ->
B_1036_2816_0 ; _rtB -> B_1036_2818_0 = ( _rtB -> B_1036_2815_0 && _rtB ->
B_1036_2817_0 ) ; _rtB -> B_1036_2819_0 = ( _rtB -> B_1036_1484_0_i || _rtB
-> B_1036_2818_0 ) ; _rtB -> B_1036_2820_0 = ( _rtB -> B_1036_2819_0 && _rtB
-> B_1036_366_0_d ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_1036_2821_0 = _rtDW -> Memory_PreviousInput_pr ; _rtB ->
B_1036_2822_0 = ! _rtB -> B_1036_2821_0 ; } _rtB -> B_1036_2823_0 = ( _rtB ->
B_1036_1469_0 && _rtB -> B_1036_2822_0 ) ; _rtB -> B_1036_2824_0 = ( _rtB ->
B_1036_2823_0 && _rtB -> B_1036_370_0_g ) ; _rtB -> B_1036_2825_0 = ( _rtB ->
B_1036_387_0_b || _rtB -> B_1036_2810_0 || _rtB -> B_1036_2820_0 ) ; _rtB ->
B_1036_2826_0 = ( _rtB -> B_1036_388_0_e || _rtB -> B_1036_1249_0_e || _rtB
-> B_1036_1485_0_e ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { ssCallAccelRunBlock ( S , 1036 , 2827 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 1036 , 2828 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_1036_2829_0 = _rtDW -> u_PreviousInput_abb ; _rtB -> B_1036_2830_0 = _rtDW
-> u_PreviousInput_px ; _rtB -> B_1036_2831_0 = _rtDW -> u_PreviousInput_ij ;
_rtB -> B_1036_2832_0 = _rtDW -> u_PreviousInput_o ; ssCallAccelRunBlock ( S
, 1036 , 2833 , SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_2834_0 = ( _rtB ->
B_1036_341_0 == _rtB -> B_1036_346_0 ) ; _rtB -> B_1036_2835_0 = ( _rtB ->
B_1036_346_0 == _rtB -> B_1036_345_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_2836_0 = _rtDW -> u_PreviousInput_ka ;
_rtB -> B_1036_2837_0 = _rtDW -> u_PreviousInput_ey ; _rtB -> B_1036_2838_0 =
_rtDW -> u_PreviousInput_iu ; _rtB -> B_1036_2839_0 = _rtDW ->
u_PreviousInput_cp ; _rtB -> B_1036_2840_0 = _rtDW -> u_PreviousInput_mw ;
_rtB -> B_1036_2841_0 = _rtDW -> u_PreviousInput_dl ; _rtB -> B_1036_2842_0 =
_rtDW -> u_PreviousInput_lp ; _rtB -> B_1036_2843_0 = _rtDW ->
u_PreviousInput_bl ; _rtB -> B_1036_2844_0 = _rtDW -> u_PreviousInput_os ;
_rtB -> B_1036_2845_0 = _rtDW -> u_PreviousInput_k4 ; _rtB -> B_1036_2846_0 =
_rtDW -> u_PreviousInput_lb ; _rtB -> B_1036_2847_0 = _rtDW ->
u_PreviousInput_e2 ; ssCallAccelRunBlock ( S , 1036 , 2848 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 2849 ,
SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_1036_2850_0 = _rtDW -> u_PreviousInput_hd ;
_rtB -> B_1036_2851_0 = _rtDW -> u_PreviousInput_f ; _rtB -> B_1036_2852_0 =
_rtDW -> u_PreviousInput_ob ; _rtB -> B_1036_2853_0 = _rtDW ->
u_PreviousInput_jm ; ssCallAccelRunBlock ( S , 1036 , 2854 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_2855_0 = ( _rtB -> B_1036_1208_0 ==
_rtB -> B_1036_1213_0 ) ; _rtB -> B_1036_2856_0 = ( _rtB -> B_1036_1213_0 ==
_rtB -> B_1036_1212_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_1036_2857_0 = _rtDW -> u_PreviousInput_lz ; _rtB ->
B_1036_2858_0 = _rtDW -> u_PreviousInput_fc ; _rtB -> B_1036_2859_0 = _rtDW
-> u_PreviousInput_ii ; _rtB -> B_1036_2860_0 = _rtDW -> u_PreviousInput_db ;
_rtB -> B_1036_2861_0 = _rtDW -> u_PreviousInput_oe ; _rtB -> B_1036_2862_0 =
_rtDW -> u_PreviousInput_f1 ; _rtB -> B_1036_2863_0 = _rtDW ->
u_PreviousInput_dt ; _rtB -> B_1036_2864_0 = _rtDW -> u_PreviousInput_bs ;
_rtB -> B_1036_2865_0 = _rtDW -> u_PreviousInput_pv ; _rtB -> B_1036_2866_0 =
_rtDW -> u_PreviousInput_c4 ; _rtB -> B_1036_2867_0 = _rtDW ->
u_PreviousInput_go ; _rtB -> B_1036_2868_0 = _rtDW -> u_PreviousInput_oc ;
ssCallAccelRunBlock ( S , 1036 , 2869 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 1036 , 2870 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_1036_2871_0 = _rtDW -> u_PreviousInput_gw ; _rtB -> B_1036_2872_0 = _rtDW
-> u_PreviousInput_es ; _rtB -> B_1036_2873_0 = _rtDW -> u_PreviousInput_jmi
; _rtB -> B_1036_2874_0 = _rtDW -> u_PreviousInput_cq ; ssCallAccelRunBlock (
S , 1036 , 2875 , SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_2876_0 = ( _rtB ->
B_1036_1444_0 == _rtB -> B_1036_1449_0 ) ; _rtB -> B_1036_2877_0 = ( _rtB ->
B_1036_1449_0 == _rtB -> B_1036_1448_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_2878_0 = _rtDW -> u_PreviousInput_gx ;
_rtB -> B_1036_2879_0 = _rtDW -> u_PreviousInput_bt ; _rtB -> B_1036_2880_0 =
_rtDW -> u_PreviousInput_iv ; _rtB -> B_1036_2881_0 = _rtDW ->
u_PreviousInput_lj ; _rtB -> B_1036_2882_0 = _rtDW -> u_PreviousInput_pvb ;
_rtB -> B_1036_2883_0 = _rtDW -> u_PreviousInput_ba ; _rtB -> B_1036_2884_0 =
_rtDW -> u_PreviousInput_dr ; _rtB -> B_1036_2885_0 = _rtDW ->
u_PreviousInput_fh ; _rtB -> B_1036_2886_0 = _rtDW -> u_PreviousInput_hj ;
_rtB -> B_1036_2887_0 = _rtDW -> u_PreviousInput_l1q ; _rtB -> B_1036_2888_0
= _rtDW -> u_PreviousInput_nm ; _rtB -> B_1036_2889_0 = _rtDW ->
u_PreviousInput_fq ; _rtB -> B_1036_2890_0 = _rtDW -> Memory_PreviousInput_be
; _rtB -> B_1036_2891_0 = ! _rtB -> B_1036_2890_0 ; } _rtB -> B_1036_2892_0 =
( _rtB -> B_1036_2826_0 && _rtB -> B_1036_2891_0 ) ; isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2893_0 = _rtDW ->
u4_PreviousInput_jw ; } _rtB -> B_1036_2894_0 = ! _rtB -> B_1036_1277_0_n ;
_rtB -> B_1036_2895_0 = ! _rtB -> B_1036_2894_0 ; _rtB -> B_1036_2896_0 = (
_rtB -> B_1036_2893_0 && _rtB -> B_1036_2895_0 ) ; _rtB -> B_1036_2897_0 = (
_rtB -> B_1036_1307_0_c || _rtB -> B_1036_2896_0 ) ; _rtB -> B_1036_2898_0 =
( _rtB -> B_1036_2897_0 && _rtB -> B_1036_422_0_l ) ; isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2899_0 = _rtDW ->
Memory_PreviousInput_bde ; _rtB -> B_1036_2900_0 = ! _rtB -> B_1036_2899_0 ;
} _rtB -> B_1036_2901_0 = ( _rtB -> B_1036_1292_0 && _rtB -> B_1036_2900_0 )
; _rtB -> B_1036_2902_0 = ( _rtB -> B_1036_2901_0 && _rtB -> B_1036_412_0 ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_2903_0 = _rtDW -> u4_PreviousInput_hz ; } _rtB -> B_1036_2904_0 = !
_rtB -> B_1036_1513_0_c ; _rtB -> B_1036_2905_0 = ! _rtB -> B_1036_2904_0 ;
_rtB -> B_1036_2906_0 = ( _rtB -> B_1036_2903_0 && _rtB -> B_1036_2905_0 ) ;
_rtB -> B_1036_2907_0 = ( _rtB -> B_1036_1543_0_f || _rtB -> B_1036_2906_0 )
; _rtB -> B_1036_2908_0 = ( _rtB -> B_1036_2907_0 && _rtB -> B_1036_414_0_n )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_2909_0 = _rtDW -> u4_PreviousInput_pn ; } _rtB -> B_1036_2910_0 = !
_rtB -> B_1036_876_0 ; _rtB -> B_1036_2911_0 = ! _rtB -> B_1036_2910_0 ; _rtB
-> B_1036_2912_0 = ( _rtB -> B_1036_2909_0 && _rtB -> B_1036_2911_0 ) ; _rtB
-> B_1036_2913_0 = ( _rtB -> B_1036_906_0_n || _rtB -> B_1036_2912_0 ) ; _rtB
-> B_1036_2914_0 = ( _rtB -> B_1036_2913_0 && _rtB -> B_1036_419_0_g ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_2915_0 = _rtDW -> Memory_PreviousInput_oh ; _rtB -> B_1036_2916_0 = !
_rtB -> B_1036_2915_0 ; } _rtB -> B_1036_2917_0 = ( _rtB -> B_1036_1528_0 &&
_rtB -> B_1036_2916_0 ) ; _rtB -> B_1036_2918_0 = ( _rtB -> B_1036_2917_0 &&
_rtB -> B_1036_418_0_l ) ; _rtB -> B_1036_2919_0 = ( _rtB -> B_1036_2914_0 ||
_rtB -> B_1036_2898_0 || _rtB -> B_1036_2908_0 ) ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2920_0 = _rtDW ->
Memory_PreviousInput_f0 ; _rtB -> B_1036_2921_0 = ! _rtB -> B_1036_2920_0 ; }
_rtB -> B_1036_2922_0 = ( _rtB -> B_1036_891_0 && _rtB -> B_1036_2921_0 ) ;
_rtB -> B_1036_2923_0 = ( _rtB -> B_1036_2922_0 && _rtB -> B_1036_421_0_d ) ;
_rtB -> B_1036_2924_0 = ( _rtB -> B_1036_907_0 || _rtB -> B_1036_1308_0_k ||
_rtB -> B_1036_1544_0_g ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { ssCallAccelRunBlock ( S , 1036 , 2925 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 1036 , 2926 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_1036_2927_0 = _rtDW -> u_PreviousInput_mh ; _rtB -> B_1036_2928_0 = _rtDW
-> u_PreviousInput_na ; _rtB -> B_1036_2929_0 = _rtDW -> u_PreviousInput_mo ;
_rtB -> B_1036_2930_0 = _rtDW -> u_PreviousInput_n2 ; ssCallAccelRunBlock ( S
, 1036 , 2931 , SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_2932_0 = ( _rtB ->
B_1036_866_0 == _rtB -> B_1036_871_0 ) ; _rtB -> B_1036_2933_0 = ( _rtB ->
B_1036_871_0 == _rtB -> B_1036_870_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_2934_0 = _rtDW -> u_PreviousInput_kn ;
_rtB -> B_1036_2935_0 = _rtDW -> u_PreviousInput_de ; _rtB -> B_1036_2936_0 =
_rtDW -> u_PreviousInput_lx ; _rtB -> B_1036_2937_0 = _rtDW ->
u_PreviousInput_n4 ; _rtB -> B_1036_2938_0 = _rtDW -> u_PreviousInput_ll ;
_rtB -> B_1036_2939_0 = _rtDW -> u_PreviousInput_hh ; _rtB -> B_1036_2940_0 =
_rtDW -> u_PreviousInput_c4c ; _rtB -> B_1036_2941_0 = _rtDW ->
u_PreviousInput_m4 ; _rtB -> B_1036_2942_0 = _rtDW -> u_PreviousInput_ol ;
_rtB -> B_1036_2943_0 = _rtDW -> u_PreviousInput_el ; _rtB -> B_1036_2944_0 =
_rtDW -> u_PreviousInput_mv ; _rtB -> B_1036_2945_0 = _rtDW ->
u_PreviousInput_pu ; ssCallAccelRunBlock ( S , 1036 , 2946 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 2947 ,
SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_1036_2948_0 = _rtDW -> u_PreviousInput_nap
; _rtB -> B_1036_2949_0 = _rtDW -> u_PreviousInput_jh ; _rtB -> B_1036_2950_0
= _rtDW -> u_PreviousInput_hb ; _rtB -> B_1036_2951_0 = _rtDW ->
u_PreviousInput_av ; ssCallAccelRunBlock ( S , 1036 , 2952 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_2953_0 = ( _rtB -> B_1036_1267_0 ==
_rtB -> B_1036_1272_0 ) ; _rtB -> B_1036_2954_0 = ( _rtB -> B_1036_1272_0 ==
_rtB -> B_1036_1271_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_1036_2955_0 = _rtDW -> u_PreviousInput_lx0 ; _rtB ->
B_1036_2956_0 = _rtDW -> u_PreviousInput_iim ; _rtB -> B_1036_2957_0 = _rtDW
-> u_PreviousInput_hi ; _rtB -> B_1036_2958_0 = _rtDW -> u_PreviousInput_jg ;
_rtB -> B_1036_2959_0 = _rtDW -> u_PreviousInput_jx ; _rtB -> B_1036_2960_0 =
_rtDW -> u_PreviousInput_bi ; _rtB -> B_1036_2961_0 = _rtDW ->
u_PreviousInput_ioi ; _rtB -> B_1036_2962_0 = _rtDW -> u_PreviousInput_ni ;
_rtB -> B_1036_2963_0 = _rtDW -> u_PreviousInput_mho ; _rtB -> B_1036_2964_0
= _rtDW -> u_PreviousInput_baq ; _rtB -> B_1036_2965_0 = _rtDW ->
u_PreviousInput_piy ; _rtB -> B_1036_2966_0 = _rtDW -> u_PreviousInput_gz ;
ssCallAccelRunBlock ( S , 1036 , 2967 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 1036 , 2968 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_1036_2969_0 = _rtDW -> u_PreviousInput_be ; _rtB -> B_1036_2970_0 = _rtDW
-> u_PreviousInput_llk ; _rtB -> B_1036_2971_0 = _rtDW -> u_PreviousInput_bm
; _rtB -> B_1036_2972_0 = _rtDW -> u_PreviousInput_iwj ; ssCallAccelRunBlock
( S , 1036 , 2973 , SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_2974_0 = ( _rtB
-> B_1036_1503_0 == _rtB -> B_1036_1508_0 ) ; _rtB -> B_1036_2975_0 = ( _rtB
-> B_1036_1508_0 == _rtB -> B_1036_1507_0 ) ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_2976_0 = _rtDW -> u_PreviousInput_i0
; _rtB -> B_1036_2977_0 = _rtDW -> u_PreviousInput_ci ; _rtB -> B_1036_2978_0
= _rtDW -> u_PreviousInput_cj ; _rtB -> B_1036_2979_0 = _rtDW ->
u_PreviousInput_ax ; _rtB -> B_1036_2980_0 = _rtDW -> u_PreviousInput_kc ;
_rtB -> B_1036_2981_0 = _rtDW -> u_PreviousInput_jn ; _rtB -> B_1036_2982_0 =
_rtDW -> u_PreviousInput_lg ; _rtB -> B_1036_2983_0 = _rtDW ->
u_PreviousInput_au ; _rtB -> B_1036_2984_0 = _rtDW -> u_PreviousInput_drc ;
_rtB -> B_1036_2985_0 = _rtDW -> u_PreviousInput_ljv ; _rtB -> B_1036_2986_0
= _rtDW -> u_PreviousInput_oh ; _rtB -> B_1036_2987_0 = _rtDW ->
u_PreviousInput_dh ; _rtB -> B_1036_2988_0 = _rtDW -> Memory_PreviousInput_cd
; _rtB -> B_1036_2989_0 = ! _rtB -> B_1036_2988_0 ; } _rtB -> B_1036_2990_0 =
( _rtB -> B_1036_2924_0 && _rtB -> B_1036_2989_0 ) ; if ( _rtB ->
B_1036_446_0_p ) { _rtB -> B_1036_2992_0 = _rtB -> B_1036_489_0_o ; } else {
if ( _rtB -> B_1036_295_0_o ) { _rtB -> B_252_0_0 = _rtB -> B_1036_475_0_k ;
} else { _rtB -> B_252_0_0 = _rtB -> B_1036_483_0_e ; } _rtB -> B_1036_2992_0
= _rtB -> B_252_0_0 ; } _rtB -> B_1036_2993_0 = ( ( _rtB -> B_1036_448_0 !=
0.0 ) && ( _rtB -> B_1036_962_0 != 0.0 ) && ( _rtB -> B_1036_1035_0 != 0.0 )
) ; _rtB -> B_1036_2994_0 = 0.0 * _rtB -> B_1036_296_0 ; _rtB ->
B_1036_2995_0 = _rtB -> B_1036_2994_0 / _rtB -> B_1036_503_0_h ; _rtB ->
B_1036_2996_0 = 0.0 * _rtB -> B_1036_296_0 ; _rtB -> B_1036_2997_0 = _rtB ->
B_1036_2996_0 / _rtB -> B_1036_503_0_h ; _rtB -> B_1036_2998_0 = 0.0 * _rtB
-> B_1036_316_0 ; _rtB -> B_1036_2999_0 = 0.0 * _rtB -> B_1036_316_0 ; _rtB
-> B_1036_3000_0 = _rtB -> B_1036_2994_0 + _rtB -> B_1036_2998_0 ; _rtB ->
B_1036_3001_0 = _rtB -> B_1036_3000_0 / _rtB -> B_1036_504_0_f ; _rtB ->
B_1036_3002_0 = _rtB -> B_1036_2996_0 + _rtB -> B_1036_2999_0 ; _rtB ->
B_1036_3003_0 = _rtB -> B_1036_3002_0 / _rtB -> B_1036_504_0_f ; _rtB ->
B_1036_3004_0 = _rtB -> B_1036_2998_0 / _rtB -> B_1036_505_0_i ; _rtB ->
B_1036_3005_0 = _rtB -> B_1036_2999_0 / _rtB -> B_1036_505_0_i ; _rtB ->
B_1036_3006_0 = ( _rtB -> B_1036_510_0_k && _rtB -> B_1036_295_0_o ) ; _rtB
-> B_1036_3007_0 = ( _rtB -> B_1036_515_0_g && _rtB -> B_1036_315_0_l ) ; if
( _rtB -> B_1036_438_0 ) { _rtB -> B_1036_3009_0 = _rtB -> B_1036_502_0 ; }
else { if ( _rtB -> B_1036_446_0_p ) { _rtB -> B_250_0_0 = ( _rtB ->
B_1036_3006_0 || _rtB -> B_1036_3007_0 ) ; _rtB -> B_250_1_0 = ( _rtB ->
B_250_0_0 && _rtB -> B_1036_446_0_p ) ; _rtB -> B_251_2_0 = _rtB -> B_250_1_0
; } else { if ( _rtB -> B_1036_295_0_o ) { _rtB -> B_249_0_0 = _rtB ->
B_1036_3006_0 ; } else { _rtB -> B_249_0_0 = _rtB -> B_1036_3007_0 ; } _rtB
-> B_251_2_0 = _rtB -> B_249_0_0 ; } _rtB -> B_1036_3009_0 = _rtB ->
B_251_2_0 ; } _rtB -> B_1036_3010_0 = ( _rtB -> B_1036_444_0_l && _rtB ->
B_1036_959_0 && _rtB -> B_1036_1032_0_n ) ; _rtB -> B_1036_3011_0 = ( _rtB ->
B_1036_445_0_f && _rtB -> B_1036_960_0 && _rtB -> B_1036_1033_0_l ) ; if (
_rtB -> B_1036_756_0_h ) { _rtB -> B_1036_3013_0 = _rtB -> B_1036_573_0_o ; }
else { if ( _rtB -> B_1036_682_0_f ) { _rtB -> B_297_0_0 = _rtB ->
B_1036_559_0_g ; } else { _rtB -> B_297_0_0 = _rtB -> B_1036_567_0_g ; } _rtB
-> B_1036_3013_0 = _rtB -> B_297_0_0 ; } _rtB -> B_1036_3014_0 = ( ( _rtB ->
B_1036_758_0 != 0.0 ) && ( _rtB -> B_1036_983_0 != 0.0 ) && ( _rtB ->
B_1036_1056_0 != 0.0 ) ) ; _rtB -> B_1036_3015_0 = 0.0 * _rtB -> B_1036_683_0
; _rtB -> B_1036_3016_0 = _rtB -> B_1036_3015_0 / _rtB -> B_1036_587_0_o ;
_rtB -> B_1036_3017_0 = 0.0 * _rtB -> B_1036_683_0 ; _rtB -> B_1036_3018_0 =
_rtB -> B_1036_3017_0 / _rtB -> B_1036_587_0_o ; _rtB -> B_1036_3019_0 = 0.0
* _rtB -> B_1036_703_0 ; _rtB -> B_1036_3020_0 = 0.0 * _rtB -> B_1036_703_0 ;
_rtB -> B_1036_3021_0 = _rtB -> B_1036_3015_0 + _rtB -> B_1036_3019_0 ; _rtB
-> B_1036_3022_0 = _rtB -> B_1036_3021_0 / _rtB -> B_1036_588_0_h ; _rtB ->
B_1036_3023_0 = _rtB -> B_1036_3017_0 + _rtB -> B_1036_3020_0 ; _rtB ->
B_1036_3024_0 = _rtB -> B_1036_3023_0 / _rtB -> B_1036_588_0_h ; _rtB ->
B_1036_3025_0 = _rtB -> B_1036_3019_0 / _rtB -> B_1036_589_0_c ; _rtB ->
B_1036_3026_0 = _rtB -> B_1036_3020_0 / _rtB -> B_1036_589_0_c ; _rtB ->
B_1036_3027_0 = ( _rtB -> B_1036_594_0_a && _rtB -> B_1036_682_0_f ) ; _rtB
-> B_1036_3028_0 = ( _rtB -> B_1036_599_0_p && _rtB -> B_1036_702_0_m ) ; if
( _rtB -> B_1036_748_0_e ) { _rtB -> B_1036_3030_0 = _rtB -> B_1036_586_0_g ;
} else { if ( _rtB -> B_1036_756_0_h ) { _rtB -> B_295_0_0 = ( _rtB ->
B_1036_3027_0 || _rtB -> B_1036_3028_0 ) ; _rtB -> B_295_1_0 = ( _rtB ->
B_295_0_0 && _rtB -> B_1036_756_0_h ) ; _rtB -> B_296_2_0 = _rtB -> B_295_1_0
; } else { if ( _rtB -> B_1036_682_0_f ) { _rtB -> B_294_0_0 = _rtB ->
B_1036_3027_0 ; } else { _rtB -> B_294_0_0 = _rtB -> B_1036_3028_0 ; } _rtB
-> B_296_2_0 = _rtB -> B_294_0_0 ; } _rtB -> B_1036_3030_0 = _rtB ->
B_296_2_0 ; } _rtB -> B_1036_3031_0 = ( _rtB -> B_1036_754_0_e && _rtB ->
B_1036_980_0 && _rtB -> B_1036_1053_0 ) ; _rtB -> B_1036_3032_0 = ( _rtB ->
B_1036_755_0_m && _rtB -> B_1036_981_0 && _rtB -> B_1036_1054_0_e ) ; _rtB ->
B_1036_3033_0 = ( _rtB -> B_1036_1815_0_e && _rtB -> B_1036_2021_0_a && _rtB
-> B_1036_2029_0_h ) ; _rtB -> B_1036_3034_0 = _rtB -> B_1036_1613_0 - _rtB
-> B_1036_1624_0 ; _rtB -> B_1036_3035_0 = _rtB -> B_1036_3034_0 * _rtB ->
B_1036_3034_0 ; _rtB -> B_1036_3036_0 = _rtB -> B_1036_1617_0 - _rtB ->
B_1036_1628_0 ; _rtB -> B_1036_3037_0 = _rtB -> B_1036_3036_0 * _rtB ->
B_1036_3036_0 ; _rtB -> B_1036_3038_0 = _rtB -> B_1036_3035_0 + _rtB ->
B_1036_3037_0 ; _rtB -> B_1036_3039_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_3038_0 ) ; _rtB -> B_1036_3040_0 = ( _rtB -> B_1036_3039_0 > _rtB ->
B_1036_632_0_g ) ; _rtB -> B_1036_3041_0 = _rtB -> B_1036_3034_0 * _rtB ->
B_1036_639_0_k ; _rtB -> B_1036_3042_0 = _rtB -> B_1036_3036_0 * _rtB ->
B_1036_639_0_k ; _rtB -> B_1036_3043_0 = _rtB -> B_1036_1624_0 - _rtB ->
B_1036_1659_0 ; _rtB -> B_1036_3044_0 = _rtB -> B_1036_3043_0 * _rtB ->
B_1036_639_0_k ; _rtB -> B_1036_3045_0 = _rtB -> B_1036_1628_0 - _rtB ->
B_1036_1663_0 ; _rtB -> B_1036_3046_0 = _rtB -> B_1036_3045_0 * _rtB ->
B_1036_639_0_k ; _rtB -> B_1036_3047_0 = _rtB -> B_1036_1659_0 - _rtB ->
B_1036_1613_0 ; _rtB -> B_1036_3048_0 = _rtB -> B_1036_3047_0 * _rtB ->
B_1036_639_0_k ; _rtB -> B_1036_3049_0 = _rtB -> B_1036_1663_0 - _rtB ->
B_1036_1617_0 ; _rtB -> B_1036_3050_0 = _rtB -> B_1036_3049_0 * _rtB ->
B_1036_639_0_k ; _rtB -> B_1036_3051_0 = _rtB -> B_1036_3043_0 * _rtB ->
B_1036_3043_0 ; _rtB -> B_1036_3052_0 = _rtB -> B_1036_3045_0 * _rtB ->
B_1036_3045_0 ; _rtB -> B_1036_3053_0 = _rtB -> B_1036_3051_0 + _rtB ->
B_1036_3052_0 ; _rtB -> B_1036_3054_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_3053_0 ) ; _rtB -> B_1036_3055_0 = ( _rtB -> B_1036_3054_0 > _rtB ->
B_1036_632_0_g ) ; _rtB -> B_1036_3056_0 = _rtB -> B_1036_3047_0 * _rtB ->
B_1036_3047_0 ; _rtB -> B_1036_3057_0 = _rtB -> B_1036_3049_0 * _rtB ->
B_1036_3049_0 ; _rtB -> B_1036_3058_0 = _rtB -> B_1036_3056_0 + _rtB ->
B_1036_3057_0 ; _rtB -> B_1036_3059_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_3058_0 ) ; _rtB -> B_1036_3060_0 = ( _rtB -> B_1036_3059_0 > _rtB ->
B_1036_632_0_g ) ; _rtB -> B_1036_3061_0 = muDoubleScalarCos ( _rtB ->
B_1036_275_0 [ 12 ] ) ; _rtB -> B_1036_3062_0 = _rtB -> B_1036_260_0 [ 12 ] *
_rtB -> B_1036_3061_0 ; _rtB -> B_1036_3063_0 = _rtB -> B_1036_3062_0 * _rtB
-> B_1036_3062_0 ; _rtB -> B_1036_3064_0 = muDoubleScalarSin ( _rtB ->
B_1036_275_0 [ 12 ] ) ; _rtB -> B_1036_3065_0 = _rtB -> B_1036_3064_0 * _rtB
-> B_1036_260_0 [ 12 ] ; _rtB -> B_1036_3066_0 = _rtB -> B_1036_3065_0 * _rtB
-> B_1036_3065_0 ; _rtB -> B_1036_3067_0 = _rtB -> B_1036_3063_0 + _rtB ->
B_1036_3066_0 ; _rtB -> B_1036_3068_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_3067_0 ) ; _rtB -> B_1036_3069_0 = ( _rtB -> B_1036_3068_0 > _rtB ->
B_1036_653_0_n ) ; _rtB -> B_1036_3070_0 = muDoubleScalarCos ( _rtB ->
B_1036_275_0 [ 11 ] ) ; _rtB -> B_1036_3071_0 = _rtB -> B_1036_260_0 [ 11 ] *
_rtB -> B_1036_3070_0 ; _rtB -> B_1036_3072_0 = _rtB -> B_1036_3071_0 * _rtB
-> B_1036_3071_0 ; _rtB -> B_1036_3073_0 = muDoubleScalarSin ( _rtB ->
B_1036_275_0 [ 11 ] ) ; _rtB -> B_1036_3074_0 = _rtB -> B_1036_3073_0 * _rtB
-> B_1036_260_0 [ 11 ] ; _rtB -> B_1036_3075_0 = _rtB -> B_1036_3074_0 * _rtB
-> B_1036_3074_0 ; _rtB -> B_1036_3076_0 = _rtB -> B_1036_3072_0 + _rtB ->
B_1036_3075_0 ; _rtB -> B_1036_3077_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_3076_0 ) ; _rtB -> B_1036_3078_0 = ( _rtB -> B_1036_3077_0 > _rtB ->
B_1036_653_0_n ) ; _rtB -> B_1036_3079_0 = muDoubleScalarCos ( _rtB ->
B_1036_275_0 [ 13 ] ) ; _rtB -> B_1036_3080_0 = _rtB -> B_1036_260_0 [ 13 ] *
_rtB -> B_1036_3079_0 ; _rtB -> B_1036_3081_0 = _rtB -> B_1036_3080_0 * _rtB
-> B_1036_3080_0 ; _rtB -> B_1036_3082_0 = muDoubleScalarSin ( _rtB ->
B_1036_275_0 [ 13 ] ) ; _rtB -> B_1036_3083_0 = _rtB -> B_1036_3082_0 * _rtB
-> B_1036_260_0 [ 13 ] ; _rtB -> B_1036_3084_0 = _rtB -> B_1036_3083_0 * _rtB
-> B_1036_3083_0 ; _rtB -> B_1036_3085_0 = _rtB -> B_1036_3081_0 + _rtB ->
B_1036_3084_0 ; _rtB -> B_1036_3086_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_3085_0 ) ; _rtB -> B_1036_3087_0 = ( _rtB -> B_1036_3086_0 > _rtB ->
B_1036_653_0_n ) ; _rtB -> B_1036_3088_0 = ( _rtB -> B_1036_3078_0 && _rtB ->
B_1036_3069_0 && _rtB -> B_1036_3087_0 ) ; _rtB -> B_1036_3089_0 = _rtB ->
B_1036_3071_0 - _rtB -> B_1036_3062_0 ; _rtB -> B_1036_3090_0 = _rtB ->
B_1036_3089_0 * _rtB -> B_1036_3089_0 ; _rtB -> B_1036_3091_0 = _rtB ->
B_1036_3074_0 - _rtB -> B_1036_3065_0 ; _rtB -> B_1036_3092_0 = _rtB ->
B_1036_3091_0 * _rtB -> B_1036_3091_0 ; _rtB -> B_1036_3093_0 = _rtB ->
B_1036_3090_0 + _rtB -> B_1036_3092_0 ; _rtB -> B_1036_3094_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_3093_0 ) ; _rtB -> B_1036_3095_0 = ( _rtB
-> B_1036_3094_0 > _rtB -> B_1036_656_0_b ) ; _rtB -> B_1036_3096_0 = _rtB ->
B_1036_3089_0 * _rtB -> B_1036_663_0_n ; _rtB -> B_1036_3097_0 = _rtB ->
B_1036_3091_0 * _rtB -> B_1036_663_0_n ; _rtB -> B_1036_3098_0 = _rtB ->
B_1036_3071_0 * _rtB -> B_1036_663_0_n ; _rtB -> B_1036_3099_0 = _rtB ->
B_1036_3074_0 * _rtB -> B_1036_663_0_n ; _rtB -> B_1036_3100_0 = _rtB ->
B_1036_3062_0 * _rtB -> B_1036_663_0_n ; _rtB -> B_1036_3101_0 = _rtB ->
B_1036_3065_0 * _rtB -> B_1036_663_0_n ; _rtB -> B_1036_3102_0 = _rtB ->
B_1036_3080_0 * _rtB -> B_1036_663_0_n ; _rtB -> B_1036_3103_0 = _rtB ->
B_1036_3083_0 * _rtB -> B_1036_663_0_n ; _rtB -> B_1036_3104_0 = _rtB ->
B_1036_3062_0 - _rtB -> B_1036_3080_0 ; _rtB -> B_1036_3105_0 = _rtB ->
B_1036_3104_0 * _rtB -> B_1036_663_0_n ; _rtB -> B_1036_3106_0 = _rtB ->
B_1036_3065_0 - _rtB -> B_1036_3083_0 ; _rtB -> B_1036_3107_0 = _rtB ->
B_1036_3106_0 * _rtB -> B_1036_663_0_n ; _rtB -> B_1036_3108_0 = _rtB ->
B_1036_3080_0 - _rtB -> B_1036_3071_0 ; _rtB -> B_1036_3109_0 = _rtB ->
B_1036_3108_0 * _rtB -> B_1036_663_0_n ; _rtB -> B_1036_3110_0 = _rtB ->
B_1036_3083_0 - _rtB -> B_1036_3074_0 ; _rtB -> B_1036_3111_0 = _rtB ->
B_1036_3110_0 * _rtB -> B_1036_663_0_n ; _rtB -> B_1036_3112_0 = _rtB ->
B_1036_3104_0 * _rtB -> B_1036_3104_0 ; _rtB -> B_1036_3113_0 = _rtB ->
B_1036_3106_0 * _rtB -> B_1036_3106_0 ; _rtB -> B_1036_3114_0 = _rtB ->
B_1036_3112_0 + _rtB -> B_1036_3113_0 ; _rtB -> B_1036_3115_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_3114_0 ) ; _rtB -> B_1036_3116_0 = ( _rtB
-> B_1036_3115_0 > _rtB -> B_1036_656_0_b ) ; _rtB -> B_1036_3117_0 = _rtB ->
B_1036_3108_0 * _rtB -> B_1036_3108_0 ; _rtB -> B_1036_3118_0 = _rtB ->
B_1036_3110_0 * _rtB -> B_1036_3110_0 ; _rtB -> B_1036_3119_0 = _rtB ->
B_1036_3117_0 + _rtB -> B_1036_3118_0 ; _rtB -> B_1036_3120_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_3119_0 ) ; _rtB -> B_1036_3121_0 = ( _rtB
-> B_1036_3120_0 > _rtB -> B_1036_656_0_b ) ; _rtB -> B_1036_3122_0 =
muDoubleScalarCos ( _rtB -> B_1036_275_0 [ 8 ] ) ; _rtB -> B_1036_3123_0 =
_rtB -> B_1036_260_0 [ 8 ] * _rtB -> B_1036_3122_0 ; _rtB -> B_1036_3124_0 =
_rtB -> B_1036_3123_0 * _rtB -> B_1036_3123_0 ; _rtB -> B_1036_3125_0 =
muDoubleScalarSin ( _rtB -> B_1036_275_0 [ 8 ] ) ; _rtB -> B_1036_3126_0 =
_rtB -> B_1036_3125_0 * _rtB -> B_1036_260_0 [ 8 ] ; _rtB -> B_1036_3127_0 =
_rtB -> B_1036_3126_0 * _rtB -> B_1036_3126_0 ; _rtB -> B_1036_3128_0 = _rtB
-> B_1036_3124_0 + _rtB -> B_1036_3127_0 ; _rtB -> B_1036_3129_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_3128_0 ) ; _rtB -> B_1036_3130_0 = ( _rtB
-> B_1036_3129_0 > _rtB -> B_1036_705_0_g ) ; _rtB -> B_1036_3131_0 =
muDoubleScalarCos ( _rtB -> B_1036_275_0 [ 7 ] ) ; _rtB -> B_1036_3132_0 =
_rtB -> B_1036_260_0 [ 7 ] * _rtB -> B_1036_3131_0 ; _rtB -> B_1036_3133_0 =
_rtB -> B_1036_3132_0 * _rtB -> B_1036_3132_0 ; _rtB -> B_1036_3134_0 =
muDoubleScalarSin ( _rtB -> B_1036_275_0 [ 7 ] ) ; _rtB -> B_1036_3135_0 =
_rtB -> B_1036_3134_0 * _rtB -> B_1036_260_0 [ 7 ] ; _rtB -> B_1036_3136_0 =
_rtB -> B_1036_3135_0 * _rtB -> B_1036_3135_0 ; _rtB -> B_1036_3137_0 = _rtB
-> B_1036_3133_0 + _rtB -> B_1036_3136_0 ; _rtB -> B_1036_3138_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_3137_0 ) ; _rtB -> B_1036_3139_0 = ( _rtB
-> B_1036_3138_0 > _rtB -> B_1036_705_0_g ) ; _rtB -> B_1036_3140_0 =
muDoubleScalarCos ( _rtB -> B_1036_275_0 [ 9 ] ) ; _rtB -> B_1036_3141_0 =
_rtB -> B_1036_260_0 [ 9 ] * _rtB -> B_1036_3140_0 ; _rtB -> B_1036_3142_0 =
_rtB -> B_1036_3141_0 * _rtB -> B_1036_3141_0 ; _rtB -> B_1036_3143_0 =
muDoubleScalarSin ( _rtB -> B_1036_275_0 [ 9 ] ) ; _rtB -> B_1036_3144_0 =
_rtB -> B_1036_3143_0 * _rtB -> B_1036_260_0 [ 9 ] ; _rtB -> B_1036_3145_0 =
_rtB -> B_1036_3144_0 * _rtB -> B_1036_3144_0 ; _rtB -> B_1036_3146_0 = _rtB
-> B_1036_3142_0 + _rtB -> B_1036_3145_0 ; _rtB -> B_1036_3147_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_3146_0 ) ; _rtB -> B_1036_3148_0 = ( _rtB
-> B_1036_3147_0 > _rtB -> B_1036_705_0_g ) ; _rtB -> B_1036_3149_0 = ( _rtB
-> B_1036_3139_0 && _rtB -> B_1036_3130_0 && _rtB -> B_1036_3148_0 ) ; _rtB
-> B_1036_3150_0 = _rtB -> B_1036_3132_0 - _rtB -> B_1036_3123_0 ; _rtB ->
B_1036_3151_0 = _rtB -> B_1036_3150_0 * _rtB -> B_1036_3150_0 ; _rtB ->
B_1036_3152_0 = _rtB -> B_1036_3135_0 - _rtB -> B_1036_3126_0 ; _rtB ->
B_1036_3153_0 = _rtB -> B_1036_3152_0 * _rtB -> B_1036_3152_0 ; _rtB ->
B_1036_3154_0 = _rtB -> B_1036_3151_0 + _rtB -> B_1036_3153_0 ; _rtB ->
B_1036_3155_0 = muDoubleScalarSqrt ( _rtB -> B_1036_3154_0 ) ; _rtB ->
B_1036_3156_0 = ( _rtB -> B_1036_3155_0 > _rtB -> B_1036_708_0_g ) ; _rtB ->
B_1036_3157_0 = _rtB -> B_1036_3150_0 * _rtB -> B_1036_715_0_k ; _rtB ->
B_1036_3158_0 = _rtB -> B_1036_3152_0 * _rtB -> B_1036_715_0_k ; _rtB ->
B_1036_3159_0 = _rtB -> B_1036_3132_0 * _rtB -> B_1036_715_0_k ; _rtB ->
B_1036_3160_0 = _rtB -> B_1036_3135_0 * _rtB -> B_1036_715_0_k ; _rtB ->
B_1036_3161_0 = _rtB -> B_1036_3123_0 * _rtB -> B_1036_715_0_k ; _rtB ->
B_1036_3162_0 = _rtB -> B_1036_3126_0 * _rtB -> B_1036_715_0_k ; _rtB ->
B_1036_3163_0 = _rtB -> B_1036_3141_0 * _rtB -> B_1036_715_0_k ; _rtB ->
B_1036_3164_0 = _rtB -> B_1036_3144_0 * _rtB -> B_1036_715_0_k ; _rtB ->
B_1036_3165_0 = _rtB -> B_1036_3123_0 - _rtB -> B_1036_3141_0 ; _rtB ->
B_1036_3166_0 = _rtB -> B_1036_3165_0 * _rtB -> B_1036_715_0_k ; _rtB ->
B_1036_3167_0 = _rtB -> B_1036_3126_0 - _rtB -> B_1036_3144_0 ; _rtB ->
B_1036_3168_0 = _rtB -> B_1036_3167_0 * _rtB -> B_1036_715_0_k ; _rtB ->
B_1036_3169_0 = _rtB -> B_1036_3141_0 - _rtB -> B_1036_3132_0 ; _rtB ->
B_1036_3170_0 = _rtB -> B_1036_3169_0 * _rtB -> B_1036_715_0_k ; _rtB ->
B_1036_3171_0 = _rtB -> B_1036_3144_0 - _rtB -> B_1036_3135_0 ; _rtB ->
B_1036_3172_0 = _rtB -> B_1036_3171_0 * _rtB -> B_1036_715_0_k ; _rtB ->
B_1036_3173_0 = _rtB -> B_1036_3165_0 * _rtB -> B_1036_3165_0 ; _rtB ->
B_1036_3174_0 = _rtB -> B_1036_3167_0 * _rtB -> B_1036_3167_0 ; _rtB ->
B_1036_3175_0 = _rtB -> B_1036_3173_0 + _rtB -> B_1036_3174_0 ; _rtB ->
B_1036_3176_0 = muDoubleScalarSqrt ( _rtB -> B_1036_3175_0 ) ; _rtB ->
B_1036_3177_0 = ( _rtB -> B_1036_3176_0 > _rtB -> B_1036_708_0_g ) ; _rtB ->
B_1036_3178_0 = _rtB -> B_1036_3169_0 * _rtB -> B_1036_3169_0 ; _rtB ->
B_1036_3179_0 = _rtB -> B_1036_3171_0 * _rtB -> B_1036_3171_0 ; _rtB ->
B_1036_3180_0 = _rtB -> B_1036_3178_0 + _rtB -> B_1036_3179_0 ; _rtB ->
B_1036_3181_0 = muDoubleScalarSqrt ( _rtB -> B_1036_3180_0 ) ; _rtB ->
B_1036_3182_0 = ( _rtB -> B_1036_3181_0 > _rtB -> B_1036_708_0_g ) ; _rtB ->
B_1036_3183_0 = muDoubleScalarCos ( _rtB -> B_1036_662_0 [ 10 ] ) ; _rtB ->
B_1036_3184_0 = _rtB -> B_1036_650_0 [ 10 ] * _rtB -> B_1036_3183_0 ; _rtB ->
B_1036_3185_0 = _rtB -> B_1036_3184_0 * _rtB -> B_1036_3184_0 ; _rtB ->
B_1036_3186_0 = muDoubleScalarSin ( _rtB -> B_1036_662_0 [ 10 ] ) ; _rtB ->
B_1036_3187_0 = _rtB -> B_1036_3186_0 * _rtB -> B_1036_650_0 [ 10 ] ; _rtB ->
B_1036_3188_0 = _rtB -> B_1036_3187_0 * _rtB -> B_1036_3187_0 ; _rtB ->
B_1036_3189_0 = _rtB -> B_1036_3185_0 + _rtB -> B_1036_3188_0 ; _rtB ->
B_1036_3190_0 = muDoubleScalarSqrt ( _rtB -> B_1036_3189_0 ) ; _rtB ->
B_1036_3191_0 = ( _rtB -> B_1036_3190_0 > _rtB -> B_1036_757_0 ) ; _rtB ->
B_1036_3192_0 = muDoubleScalarCos ( _rtB -> B_1036_662_0 [ 6 ] ) ; _rtB ->
B_1036_3193_0 = _rtB -> B_1036_650_0 [ 6 ] * _rtB -> B_1036_3192_0 ; _rtB ->
B_1036_3194_0 = _rtB -> B_1036_3193_0 * _rtB -> B_1036_3193_0 ; _rtB ->
B_1036_3195_0 = muDoubleScalarSin ( _rtB -> B_1036_662_0 [ 6 ] ) ; _rtB ->
B_1036_3196_0 = _rtB -> B_1036_3195_0 * _rtB -> B_1036_650_0 [ 6 ] ; _rtB ->
B_1036_3197_0 = _rtB -> B_1036_3196_0 * _rtB -> B_1036_3196_0 ; _rtB ->
B_1036_3198_0 = _rtB -> B_1036_3194_0 + _rtB -> B_1036_3197_0 ; _rtB ->
B_1036_3199_0 = muDoubleScalarSqrt ( _rtB -> B_1036_3198_0 ) ; _rtB ->
B_1036_3200_0 = ( _rtB -> B_1036_3199_0 > _rtB -> B_1036_757_0 ) ; _rtB ->
B_1036_3201_0 = ( _rtB -> B_1036_3200_0 && _rtB -> B_1036_3191_0 && _rtB ->
B_1036_762_0_p ) ; _rtB -> B_1036_3202_0 = _rtB -> B_1036_3193_0 - _rtB ->
B_1036_3184_0 ; _rtB -> B_1036_3203_0 = _rtB -> B_1036_3202_0 * _rtB ->
B_1036_3202_0 ; _rtB -> B_1036_3204_0 = _rtB -> B_1036_3196_0 - _rtB ->
B_1036_3187_0 ; _rtB -> B_1036_3205_0 = _rtB -> B_1036_3204_0 * _rtB ->
B_1036_3204_0 ; _rtB -> B_1036_3206_0 = _rtB -> B_1036_3203_0 + _rtB ->
B_1036_3205_0 ; _rtB -> B_1036_3207_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_3206_0 ) ; _rtB -> B_1036_3208_0 = ( _rtB -> B_1036_3207_0 > _rtB ->
B_1036_765_0_f ) ; _rtB -> B_1036_3209_0 = _rtB -> B_1036_3202_0 * _rtB ->
B_1036_772_0 ; _rtB -> B_1036_3210_0 = _rtB -> B_1036_3204_0 * _rtB ->
B_1036_772_0 ; _rtB -> B_1036_3211_0 = _rtB -> B_1036_3193_0 * _rtB ->
B_1036_772_0 ; _rtB -> B_1036_3212_0 = _rtB -> B_1036_3196_0 * _rtB ->
B_1036_772_0 ; _rtB -> B_1036_3213_0 = _rtB -> B_1036_3184_0 * _rtB ->
B_1036_772_0 ; _rtB -> B_1036_3214_0 = _rtB -> B_1036_3187_0 * _rtB ->
B_1036_772_0 ; _rtB -> B_1036_3215_0 = _rtB -> B_1036_3184_0 ; _rtB ->
B_1036_3216_0 = _rtB -> B_1036_3215_0 * _rtB -> B_1036_772_0 ; _rtB ->
B_1036_3217_0 = _rtB -> B_1036_3187_0 ; _rtB -> B_1036_3218_0 = _rtB ->
B_1036_3217_0 * _rtB -> B_1036_772_0 ; _rtB -> B_1036_3219_0 = 0.0 - _rtB ->
B_1036_3193_0 ; _rtB -> B_1036_3220_0 = _rtB -> B_1036_3219_0 * _rtB ->
B_1036_772_0 ; _rtB -> B_1036_3221_0 = 0.0 - _rtB -> B_1036_3196_0 ; _rtB ->
B_1036_3222_0 = _rtB -> B_1036_3221_0 * _rtB -> B_1036_772_0 ; _rtB ->
B_1036_3223_0 = _rtB -> B_1036_3215_0 * _rtB -> B_1036_3215_0 ; _rtB ->
B_1036_3224_0 = _rtB -> B_1036_3217_0 * _rtB -> B_1036_3217_0 ; _rtB ->
B_1036_3225_0 = _rtB -> B_1036_3223_0 + _rtB -> B_1036_3224_0 ; _rtB ->
B_1036_3226_0 = muDoubleScalarSqrt ( _rtB -> B_1036_3225_0 ) ; _rtB ->
B_1036_3227_0 = ( _rtB -> B_1036_3226_0 > _rtB -> B_1036_765_0_f ) ; _rtB ->
B_1036_3228_0 = _rtB -> B_1036_3219_0 * _rtB -> B_1036_3219_0 ; _rtB ->
B_1036_3229_0 = _rtB -> B_1036_3221_0 * _rtB -> B_1036_3221_0 ; _rtB ->
B_1036_3230_0 = _rtB -> B_1036_3228_0 + _rtB -> B_1036_3229_0 ; _rtB ->
B_1036_3231_0 = muDoubleScalarSqrt ( _rtB -> B_1036_3230_0 ) ; _rtB ->
B_1036_3232_0 = ( _rtB -> B_1036_3231_0 > _rtB -> B_1036_765_0_f ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3233_0 =
_rtDW -> u4_PreviousInput_eb ; } _rtB -> B_1036_3234_0 = _rtB ->
B_1036_3100_0 * _rtB -> B_1036_922_0_i ; _rtB -> B_1036_3235_0 = _rtB ->
B_1036_3101_0 * _rtB -> B_1036_924_0 ; _rtB -> B_1036_3236_0 = _rtB ->
B_1036_3234_0 - _rtB -> B_1036_3235_0 ; _rtB -> B_1036_3237_0 = _rtB ->
B_1036_3102_0 * _rtB -> B_1036_922_0_i ; _rtB -> B_1036_3238_0 = _rtB ->
B_1036_3103_0 * _rtB -> B_1036_925_0 ; _rtB -> B_1036_3239_0 = _rtB ->
B_1036_3237_0 - _rtB -> B_1036_3238_0 ; _rtB -> B_1036_3240_0 = _rtB ->
B_1036_3236_0 + _rtB -> B_1036_3239_0 ; _rtB -> B_1036_3241_0 = _rtB ->
B_1036_3098_0 + _rtB -> B_1036_3240_0 ; _rtB -> B_1036_3242_0 = _rtB ->
B_1036_3241_0 * _rtB -> B_1036_923_0 ; _rtB -> B_1036_3243_0 = _rtB ->
B_1036_3242_0 * _rtB -> B_1036_3242_0 ; _rtB -> B_1036_3244_0 = _rtB ->
B_1036_3101_0 * _rtB -> B_1036_922_0_i ; _rtB -> B_1036_3245_0 = _rtB ->
B_1036_3100_0 * _rtB -> B_1036_924_0 ; _rtB -> B_1036_3246_0 = _rtB ->
B_1036_3244_0 + _rtB -> B_1036_3245_0 ; _rtB -> B_1036_3247_0 = _rtB ->
B_1036_3103_0 * _rtB -> B_1036_922_0_i ; _rtB -> B_1036_3248_0 = _rtB ->
B_1036_3102_0 * _rtB -> B_1036_925_0 ; _rtB -> B_1036_3249_0 = _rtB ->
B_1036_3247_0 + _rtB -> B_1036_3248_0 ; _rtB -> B_1036_3250_0 = _rtB ->
B_1036_3246_0 + _rtB -> B_1036_3249_0 ; _rtB -> B_1036_3251_0 = _rtB ->
B_1036_3099_0 + _rtB -> B_1036_3250_0 ; _rtB -> B_1036_3252_0 = _rtB ->
B_1036_3251_0 * _rtB -> B_1036_923_0 ; _rtB -> B_1036_3253_0 = _rtB ->
B_1036_3252_0 * _rtB -> B_1036_3252_0 ; _rtB -> B_1036_3254_0 = _rtB ->
B_1036_3243_0 + _rtB -> B_1036_3253_0 ; _rtB -> B_1036_3255_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_3254_0 ) ; if ( _rtB -> B_1036_911_0 >
_rtP -> P_675 ) { _rtB -> B_480_0_0 = ( _rtB -> B_1036_3255_0 < _rtB ->
B_1036_914_0 ) ; _rtB -> B_1036_3258_0 = _rtB -> B_480_0_0 ; } else { _rtB ->
B_479_0_0 = ( _rtB -> B_1036_3255_0 > _rtB -> B_1036_914_0 ) ; _rtB ->
B_1036_3258_0 = _rtB -> B_479_0_0 ; } _rtB -> B_1036_3259_0 = ( _rtB ->
B_1036_3258_0 && _rtB -> B_1036_916_0_d ) ; if ( _rtB -> B_1036_909_0_o ) {
_rtB -> B_1036_3261_0 = _rtB -> B_1036_3259_0 ; } else { _rtB -> B_478_0_0 =
( _rtB -> B_1036_3259_0 && _rtB -> B_1036_920_0_d ) ; _rtB -> B_1036_3261_0 =
_rtB -> B_478_0_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_1036_3262_0 = _rtDW -> Memory_PreviousInput_j4j ; _rtB ->
B_1036_3263_0 = ! _rtB -> B_1036_3262_0 ; } if ( _rtB -> B_1036_815_0_n >
_rtP -> P_676 ) { _rtB -> B_411_0_0 = ( _rtB -> B_1036_3261_0 && _rtB ->
B_1036_3263_0 ) ; _rtB -> B_1036_3265_0 = _rtB -> B_411_0_0 ; } else { _rtB
-> B_1036_3265_0 = _rtB -> B_1036_3261_0 ; } _rtB -> B_1036_3266_0 = ( _rtB
-> B_1036_921_0 != 0.0 ) ; _rtB -> B_1036_3267_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3268_0 =
_rtB -> B_1036_843_0_h ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_3268_0 , _rtB
-> B_1036_3266_0 , _rtB -> B_1036_3267_0 , _rtB -> B_1036_841_0 , & _rtB ->
ONDelay_l5 , & _rtDW -> ONDelay_l5 , & _rtZCE -> ONDelay_l5 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3270_0 =
_rtB -> B_1036_844_0_p ; } DZG_v3_OFFDelay ( S , _rtB -> B_1036_3270_0 , _rtB
-> B_1036_3266_0 , _rtB -> B_1036_3267_0 , _rtB -> B_1036_841_0 , & _rtB ->
OFFDelay_du , & _rtDW -> OFFDelay_du , & _rtZCE -> OFFDelay_du ) ; _rtB ->
B_1036_3272_0 = ( _rtB -> ONDelay_l5 . B_90_17_0 || _rtB -> OFFDelay_du .
B_86_19_0 ) ; _rtB -> B_1036_3273_0 = ( _rtB -> B_1036_3265_0 && _rtB ->
B_1036_3272_0 ) ; _rtB -> B_1036_3274_0 = ( _rtB -> B_1036_3233_0 || _rtB ->
B_1036_3273_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_3275_0 = _rtDW -> u2_PreviousInput_e ; } _rtB -> B_1036_3276_0
= ! _rtB -> B_1036_3261_0 ; _rtB -> B_1036_3277_0 = ( _rtB -> B_1036_3275_0
&& _rtB -> B_1036_3276_0 ) ; _rtB -> B_1036_3278_0 = _rtB -> B_1036_3277_0 ;
_rtB -> B_1036_3279_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_3280_0 = _rtB -> B_1036_850_0_p ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_3280_0 , _rtB -> B_1036_3278_0 , _rtB ->
B_1036_3279_0 , _rtB -> B_1036_848_0 , & _rtB -> ONDelay_kl , & _rtDW ->
ONDelay_kl , & _rtZCE -> ONDelay_kl ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_3282_0 = _rtB -> B_1036_851_0_l ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_3282_0 , _rtB -> B_1036_3278_0 , _rtB ->
B_1036_3279_0 , _rtB -> B_1036_848_0 , & _rtB -> OFFDelay_l , & _rtDW ->
OFFDelay_l , & _rtZCE -> OFFDelay_l ) ; _rtB -> B_1036_3284_0 = ( _rtB ->
ONDelay_kl . B_90_17_0 || _rtB -> OFFDelay_l . B_86_19_0 ) ; _rtB ->
B_1036_3285_0 = ! _rtB -> B_1036_3284_0 ; _rtB -> B_1036_3286_0 = ( _rtB ->
B_1036_3274_0 && _rtB -> B_1036_3285_0 ) ; _rtB -> B_1036_3287_0 = ( _rtB ->
B_1036_818_0_j && _rtB -> B_1036_3286_0 ) ; _rtB -> B_1036_3288_0 = ( _rtB ->
B_1036_3287_0 || ( _rtB -> B_1036_813_0_c != 0.0 ) ) ; _rtB -> B_1036_3289_0
= _rtB -> B_1036_3288_0 ; _rtB -> B_1036_3290_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3291_0 =
_rtB -> B_1036_829_0_j ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_3291_0 , _rtB
-> B_1036_3289_0 , _rtB -> B_1036_3290_0 , _rtB -> B_1036_827_0 , & _rtB ->
ONDelay_k4 , & _rtDW -> ONDelay_k4 , & _rtZCE -> ONDelay_k4 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3293_0 =
_rtB -> B_1036_830_0 ; } DZG_v3_OFFDelay ( S , _rtB -> B_1036_3293_0 , _rtB
-> B_1036_3289_0 , _rtB -> B_1036_3290_0 , _rtB -> B_1036_827_0 , & _rtB ->
OFFDelay_cf , & _rtDW -> OFFDelay_cf , & _rtZCE -> OFFDelay_cf ) ; _rtB ->
B_1036_3295_0 = _rtB -> B_1036_816_0_p ; _rtB -> B_1036_3296_0 = ssGetT ( S )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_3297_0 = _rtB -> B_1036_836_0 ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_3297_0 , _rtB -> B_1036_3295_0 , _rtB -> B_1036_3296_0 , _rtB ->
B_1036_834_0 , & _rtB -> ONDelay_jn2 , & _rtDW -> ONDelay_jn2 , & _rtZCE ->
ONDelay_jn2 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_3299_0 = _rtB -> B_1036_837_0_j ; } DZG_v3_OFFDelay ( S , _rtB
-> B_1036_3299_0 , _rtB -> B_1036_3295_0 , _rtB -> B_1036_3296_0 , _rtB ->
B_1036_834_0 , & _rtB -> OFFDelay_ga , & _rtDW -> OFFDelay_ga , & _rtZCE ->
OFFDelay_ga ) ; _rtB -> B_1036_3301_0 = _rtB -> B_1036_3288_0 ; _rtB ->
B_1036_3302_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_3303_0 = _rtB -> B_1036_822_0_n ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_3303_0 , _rtB -> B_1036_3301_0 , _rtB ->
B_1036_3302_0 , _rtB -> B_1036_820_0_i , & _rtB -> ONDelay_b5 , & _rtDW ->
ONDelay_b5 , & _rtZCE -> ONDelay_b5 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_3305_0 = _rtB -> B_1036_823_0_n ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_3305_0 , _rtB -> B_1036_3301_0 , _rtB ->
B_1036_3302_0 , _rtB -> B_1036_820_0_i , & _rtB -> OFFDelay_k , & _rtDW ->
OFFDelay_k , & _rtZCE -> OFFDelay_k ) ; if ( _rtB -> B_1036_817_0_g ) { _rtB
-> B_1036_3308_0 = _rtB -> B_1036_3288_0 ; } else { _rtB -> B_386_0_0 = (
_rtB -> ONDelay_jn2 . B_90_17_0 || _rtB -> OFFDelay_ga . B_86_19_0 ) ; if (
_rtB -> B_386_0_0 ) { _rtB -> B_385_0_0 = ( _rtB -> ONDelay_k4 . B_90_17_0 ||
_rtB -> OFFDelay_cf . B_86_19_0 ) ; _rtB -> B_386_3_0 = _rtB -> B_385_0_0 ; }
else { _rtB -> B_384_0_0 = ( _rtB -> ONDelay_b5 . B_90_17_0 || _rtB ->
OFFDelay_k . B_86_19_0 ) ; _rtB -> B_386_3_0 = _rtB -> B_384_0_0 ; } _rtB ->
B_1036_3308_0 = _rtB -> B_386_3_0 ; } _rtB -> B_1036_3309_0 = ( _rtB ->
B_1036_3308_0 && _rtB -> B_1036_3261_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_3310_0 = _rtDW ->
Memory_PreviousInput_kub ; _rtB -> B_1036_3311_0 = ! _rtB -> B_1036_3310_0 ;
} _rtB -> B_1036_3312_0 = ( _rtB -> B_1036_3309_0 && _rtB -> B_1036_3311_0 )
; _rtB -> B_1036_3313_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_3314_0 = _rtDW ->
Memory_PreviousInput_bj ; _rtB -> B_1036_3315_0 = ! _rtB -> B_1036_3314_0 ; }
_rtB -> B_1036_3316_0 = _rtB -> B_1036_3309_0 ; _rtB -> B_1036_3317_0 = _rtB
-> B_1036_3316_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_3318_0 = _rtDW -> Memory_PreviousInput_nd ; _rtB ->
B_1036_3319_0 = _rtB -> B_1036_859_0_f [ 0 ] ; } DZG_v3_POSITIVEEdge ( S ,
_rtB -> B_1036_3319_0 , _rtB -> B_1036_3317_0 , _rtB -> B_1036_3318_0 , &
_rtB -> POSITIVEEdge_c , & _rtDW -> POSITIVEEdge_c ) ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3321_0 = _rtB ->
B_1036_859_0_f [ 1 ] ; } DZG_v3_NEGATIVEEdge ( S , _rtB -> B_1036_3321_0 ,
_rtB -> B_1036_3317_0 , _rtB -> B_1036_3318_0 , & _rtB -> NEGATIVEEdge_j , &
_rtDW -> NEGATIVEEdge_j ) ; _rtB -> B_1036_3323_0 = ( _rtB -> POSITIVEEdge_c
. B_85_0_0 || _rtB -> NEGATIVEEdge_j . B_84_0_0 ) ; _rtB -> B_1036_3324_0 = (
_rtB -> B_1036_3315_0 && _rtB -> B_1036_3323_0 ) ; _rtB -> B_1036_3325_0 =
_rtB -> B_1036_3324_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_1036_3326_0 = _rtDW -> ICic_PreviousInput_l ; } if ( _rtB ->
B_1036_3325_0 ) { _rtB -> B_1036_3327_0 = _rtB -> B_1036_3313_0 ; } else {
_rtB -> B_1036_3327_0 = _rtB -> B_1036_3326_0 ; } isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3328_0 = _rtP -> P_678 ; } _rtB
-> B_1036_3329_0 = _rtB -> B_1036_3327_0 + _rtB -> B_1036_3328_0 ; _rtB ->
B_1036_3330_0 = ( _rtB -> B_1036_3329_0 > _rtB -> B_1036_3313_0 ) ; _rtB ->
B_1036_3331_0 = ( _rtB -> B_1036_3312_0 || _rtB -> B_1036_3330_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3332_0 =
_rtDW -> Memory_PreviousInput_na ; _rtB -> B_1036_3333_0 = ! _rtB ->
B_1036_3332_0 ; } _rtB -> B_1036_3334_0 = ( _rtB -> B_1036_3331_0 && _rtB ->
B_1036_3333_0 ) ; _rtB -> B_1036_3335_0 = ssGetT ( S ) ; _rtB ->
B_1036_3336_0 = _rtB -> B_1036_3323_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { DZG_v3_TriggeredSubsystem ( S , _rtB -> B_1036_3336_0 ,
_rtB -> B_1036_3335_0 , & _rtB -> TriggeredSubsystem_k , & _rtDW ->
TriggeredSubsystem_k , & _rtZCE -> TriggeredSubsystem_k ) ; _rtB ->
B_1036_3338_0 = _rtB -> TriggeredSubsystem_k . B_83_0_0 + _rtB ->
B_1036_854_0 ; } _rtB -> B_1036_3339_0 = ( _rtB -> B_1036_3338_0 > _rtB ->
B_1036_3335_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_3340_0 = _rtDW -> u4_PreviousInput_ef ; } _rtB ->
B_1036_3341_0 = false ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_1036_3342_0 = _rtDW -> Memory_PreviousInput_bu ; _rtB ->
B_1036_3343_0 = ! _rtB -> B_1036_3342_0 ; } if ( _rtB -> B_1036_861_0_k >
_rtP -> P_679 ) { _rtB -> B_458_0_0 = ( _rtB -> B_1036_3341_0 && _rtB ->
B_1036_3343_0 ) ; _rtB -> B_1036_3345_0 = _rtB -> B_458_0_0 ; } else { _rtB
-> B_1036_3345_0 = _rtB -> B_1036_3341_0 ; } _rtB -> B_1036_3346_0 = ( _rtB
-> B_1036_921_0 != 0.0 ) ; _rtB -> B_1036_3347_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3348_0 =
_rtB -> B_1036_889_0_p ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_3348_0 , _rtB
-> B_1036_3346_0 , _rtB -> B_1036_3347_0 , _rtB -> B_1036_887_0 , & _rtB ->
ONDelay_hn , & _rtDW -> ONDelay_hn , & _rtZCE -> ONDelay_hn ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3350_0 =
_rtB -> B_1036_890_0_e ; } DZG_v3_OFFDelay ( S , _rtB -> B_1036_3350_0 , _rtB
-> B_1036_3346_0 , _rtB -> B_1036_3347_0 , _rtB -> B_1036_887_0 , & _rtB ->
OFFDelay_f4 , & _rtDW -> OFFDelay_f4 , & _rtZCE -> OFFDelay_f4 ) ; _rtB ->
B_1036_3352_0 = ( _rtB -> ONDelay_hn . B_90_17_0 || _rtB -> OFFDelay_f4 .
B_86_19_0 ) ; _rtB -> B_1036_3353_0 = ( _rtB -> B_1036_3345_0 && _rtB ->
B_1036_3352_0 ) ; _rtB -> B_1036_3354_0 = ( _rtB -> B_1036_3340_0 || _rtB ->
B_1036_3353_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_3355_0 = _rtDW -> u2_PreviousInput_gy ; } _rtB ->
B_1036_3356_0 = ! _rtB -> B_1036_3341_0 ; _rtB -> B_1036_3357_0 = ( _rtB ->
B_1036_3355_0 && _rtB -> B_1036_3356_0 ) ; _rtB -> B_1036_3358_0 = _rtB ->
B_1036_3357_0 ; _rtB -> B_1036_3359_0 = ssGetT ( S ) ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3360_0 = _rtB ->
B_1036_896_0_k ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_3360_0 , _rtB ->
B_1036_3358_0 , _rtB -> B_1036_3359_0 , _rtB -> B_1036_894_0 , & _rtB ->
ONDelay_nu , & _rtDW -> ONDelay_nu , & _rtZCE -> ONDelay_nu ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3362_0 =
_rtB -> B_1036_897_0_n ; } DZG_v3_OFFDelay ( S , _rtB -> B_1036_3362_0 , _rtB
-> B_1036_3358_0 , _rtB -> B_1036_3359_0 , _rtB -> B_1036_894_0 , & _rtB ->
OFFDelay_e4 , & _rtDW -> OFFDelay_e4 , & _rtZCE -> OFFDelay_e4 ) ; _rtB ->
B_1036_3364_0 = ( _rtB -> ONDelay_nu . B_90_17_0 || _rtB -> OFFDelay_e4 .
B_86_19_0 ) ; _rtB -> B_1036_3365_0 = ! _rtB -> B_1036_3364_0 ; _rtB ->
B_1036_3366_0 = ( _rtB -> B_1036_3354_0 && _rtB -> B_1036_3365_0 ) ; _rtB ->
B_1036_3367_0 = ( _rtB -> B_1036_864_0_l && _rtB -> B_1036_3366_0 ) ; _rtB ->
B_1036_3368_0 = ( _rtB -> B_1036_3367_0 || ( _rtB -> B_1036_813_0_c != 0.0 )
) ; _rtB -> B_1036_3369_0 = _rtB -> B_1036_3368_0 ; _rtB -> B_1036_3370_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_3371_0 = _rtB -> B_1036_875_0_g ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_3371_0 , _rtB -> B_1036_3369_0 , _rtB -> B_1036_3370_0 , _rtB ->
B_1036_873_0_a , & _rtB -> ONDelay_fr , & _rtDW -> ONDelay_fr , & _rtZCE ->
ONDelay_fr ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_3373_0 = _rtB -> B_1036_876_0_o ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_3373_0 , _rtB -> B_1036_3369_0 , _rtB -> B_1036_3370_0 , _rtB ->
B_1036_873_0_a , & _rtB -> OFFDelay_mj , & _rtDW -> OFFDelay_mj , & _rtZCE ->
OFFDelay_mj ) ; _rtB -> B_1036_3375_0 = _rtB -> B_1036_862_0_h ; _rtB ->
B_1036_3376_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_3377_0 = _rtB -> B_1036_882_0_k ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_3377_0 , _rtB -> B_1036_3375_0 , _rtB ->
B_1036_3376_0 , _rtB -> B_1036_880_0_b , & _rtB -> ONDelay_hl , & _rtDW ->
ONDelay_hl , & _rtZCE -> ONDelay_hl ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_3379_0 = _rtB -> B_1036_883_0_p ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_3379_0 , _rtB -> B_1036_3375_0 , _rtB ->
B_1036_3376_0 , _rtB -> B_1036_880_0_b , & _rtB -> OFFDelay_a0 , & _rtDW ->
OFFDelay_a0 , & _rtZCE -> OFFDelay_a0 ) ; _rtB -> B_1036_3381_0 = _rtB ->
B_1036_3368_0 ; _rtB -> B_1036_3382_0 = ssGetT ( S ) ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3383_0 = _rtB ->
B_1036_868_0_g ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_3383_0 , _rtB ->
B_1036_3381_0 , _rtB -> B_1036_3382_0 , _rtB -> B_1036_866_0_j , & _rtB ->
ONDelay_hs , & _rtDW -> ONDelay_hs , & _rtZCE -> ONDelay_hs ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3385_0 =
_rtB -> B_1036_869_0_g ; } DZG_v3_OFFDelay ( S , _rtB -> B_1036_3385_0 , _rtB
-> B_1036_3381_0 , _rtB -> B_1036_3382_0 , _rtB -> B_1036_866_0_j , & _rtB ->
OFFDelay_h , & _rtDW -> OFFDelay_h , & _rtZCE -> OFFDelay_h ) ; if ( _rtB ->
B_1036_863_0_b ) { _rtB -> B_1036_3388_0 = _rtB -> B_1036_3368_0 ; } else {
_rtB -> B_433_0_0 = ( _rtB -> ONDelay_hl . B_90_17_0 || _rtB -> OFFDelay_a0 .
B_86_19_0 ) ; if ( _rtB -> B_433_0_0 ) { _rtB -> B_432_0_0 = ( _rtB ->
ONDelay_fr . B_90_17_0 || _rtB -> OFFDelay_mj . B_86_19_0 ) ; _rtB ->
B_433_3_0 = _rtB -> B_432_0_0 ; } else { _rtB -> B_431_0_0 = ( _rtB ->
ONDelay_hs . B_90_17_0 || _rtB -> OFFDelay_h . B_86_19_0 ) ; _rtB ->
B_433_3_0 = _rtB -> B_431_0_0 ; } _rtB -> B_1036_3388_0 = _rtB -> B_433_3_0 ;
} _rtB -> B_1036_3389_0 = ( _rtB -> B_1036_3388_0 && _rtB -> B_1036_3341_0 )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_3390_0 = _rtDW -> Memory_PreviousInput_k0 ; _rtB -> B_1036_3391_0 = !
_rtB -> B_1036_3390_0 ; } _rtB -> B_1036_3392_0 = ( _rtB -> B_1036_3389_0 &&
_rtB -> B_1036_3391_0 ) ; _rtB -> B_1036_3393_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3394_0 =
_rtDW -> Memory_PreviousInput_fa ; _rtB -> B_1036_3395_0 = ! _rtB ->
B_1036_3394_0 ; } _rtB -> B_1036_3396_0 = _rtB -> B_1036_3389_0 ; _rtB ->
B_1036_3397_0 = _rtB -> B_1036_3396_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_3398_0 = _rtDW -> Memory_PreviousInput_dt
; _rtB -> B_1036_3399_0 = _rtB -> B_1036_905_0 [ 0 ] ; } DZG_v3_POSITIVEEdge
( S , _rtB -> B_1036_3399_0 , _rtB -> B_1036_3397_0 , _rtB -> B_1036_3398_0 ,
& _rtB -> POSITIVEEdge_e , & _rtDW -> POSITIVEEdge_e ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3401_0 =
_rtB -> B_1036_905_0 [ 1 ] ; } DZG_v3_NEGATIVEEdge ( S , _rtB ->
B_1036_3401_0 , _rtB -> B_1036_3397_0 , _rtB -> B_1036_3398_0 , & _rtB ->
NEGATIVEEdge_h , & _rtDW -> NEGATIVEEdge_h ) ; _rtB -> B_1036_3403_0 = ( _rtB
-> POSITIVEEdge_e . B_85_0_0 || _rtB -> NEGATIVEEdge_h . B_84_0_0 ) ; _rtB ->
B_1036_3404_0 = ( _rtB -> B_1036_3395_0 && _rtB -> B_1036_3403_0 ) ; _rtB ->
B_1036_3405_0 = _rtB -> B_1036_3404_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_3406_0 = _rtDW -> ICic_PreviousInput_a ; }
if ( _rtB -> B_1036_3405_0 ) { _rtB -> B_1036_3407_0 = _rtB -> B_1036_3393_0
; } else { _rtB -> B_1036_3407_0 = _rtB -> B_1036_3406_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3408_0 =
_rtP -> P_681 ; } _rtB -> B_1036_3409_0 = _rtB -> B_1036_3407_0 + _rtB ->
B_1036_3408_0 ; _rtB -> B_1036_3410_0 = ( _rtB -> B_1036_3409_0 > _rtB ->
B_1036_3393_0 ) ; _rtB -> B_1036_3411_0 = ( _rtB -> B_1036_3392_0 || _rtB ->
B_1036_3410_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_3412_0 = _rtDW -> Memory_PreviousInput_kg ; _rtB ->
B_1036_3413_0 = ! _rtB -> B_1036_3412_0 ; } _rtB -> B_1036_3414_0 = ( _rtB ->
B_1036_3411_0 && _rtB -> B_1036_3413_0 ) ; _rtB -> B_1036_3415_0 = ssGetT ( S
) ; _rtB -> B_1036_3416_0 = _rtB -> B_1036_3403_0 ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { DZG_v3_TriggeredSubsystem ( S , _rtB ->
B_1036_3416_0 , _rtB -> B_1036_3415_0 , & _rtB -> TriggeredSubsystem_j , &
_rtDW -> TriggeredSubsystem_j , & _rtZCE -> TriggeredSubsystem_j ) ; _rtB ->
B_1036_3418_0 = _rtB -> TriggeredSubsystem_j . B_83_0_0 + _rtB ->
B_1036_900_0 ; } _rtB -> B_1036_3419_0 = ( _rtB -> B_1036_3418_0 > _rtB ->
B_1036_3415_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_3420_0 = _rtDW -> Memory_PreviousInput_bn ; _rtB ->
B_1036_3421_0 = ! _rtB -> B_1036_3420_0 ; } _rtB -> B_1036_3422_0 = ( _rtB ->
B_1036_3261_0 && _rtB -> B_1036_3421_0 ) ; _rtB -> B_1036_3423_0 = ( _rtB ->
B_1036_3422_0 || _rtB -> B_1036_3414_0 || _rtB -> B_1036_3334_0 ) ; _rtB ->
B_1036_3424_0 = _rtB -> B_1036_3101_0 * _rtB -> B_1036_922_0_i ; _rtB ->
B_1036_3425_0 = _rtB -> B_1036_3100_0 * _rtB -> B_1036_925_0 ; _rtB ->
B_1036_3426_0 = _rtB -> B_1036_3424_0 + _rtB -> B_1036_3425_0 ; _rtB ->
B_1036_3427_0 = _rtB -> B_1036_3103_0 * _rtB -> B_1036_922_0_i ; _rtB ->
B_1036_3428_0 = _rtB -> B_1036_3102_0 * _rtB -> B_1036_924_0 ; _rtB ->
B_1036_3429_0 = _rtB -> B_1036_3427_0 + _rtB -> B_1036_3428_0 ; _rtB ->
B_1036_3430_0 = _rtB -> B_1036_3426_0 + _rtB -> B_1036_3429_0 ; _rtB ->
B_1036_3431_0 = _rtB -> B_1036_3099_0 + _rtB -> B_1036_3430_0 ; _rtB ->
B_1036_3432_0 = _rtB -> B_1036_3101_0 + _rtB -> B_1036_3103_0 ; _rtB ->
B_1036_3433_0 = _rtB -> B_1036_3432_0 + _rtB -> B_1036_3099_0 ; _rtB ->
B_1036_3434_0 = _rtB -> B_1036_3433_0 * _rtB -> B_1036_923_0 ; _rtB ->
B_1036_3435_0 = _rtB -> B_1036_3100_0 * _rtB -> B_1036_922_0_i ; _rtB ->
B_1036_3436_0 = _rtB -> B_1036_3101_0 * _rtB -> B_1036_925_0 ; _rtB ->
B_1036_3437_0 = _rtB -> B_1036_3435_0 - _rtB -> B_1036_3436_0 ; _rtB ->
B_1036_3438_0 = _rtB -> B_1036_3102_0 * _rtB -> B_1036_922_0_i ; _rtB ->
B_1036_3439_0 = _rtB -> B_1036_3103_0 * _rtB -> B_1036_924_0 ; _rtB ->
B_1036_3440_0 = _rtB -> B_1036_3438_0 - _rtB -> B_1036_3439_0 ; _rtB ->
B_1036_3441_0 = _rtB -> B_1036_3437_0 + _rtB -> B_1036_3440_0 ; _rtB ->
B_1036_3442_0 = _rtB -> B_1036_3098_0 + _rtB -> B_1036_3441_0 ; _rtB ->
B_1036_3443_0 = _rtB -> B_1036_3442_0 * _rtB -> B_1036_923_0 ; _rtB ->
B_1036_3444_0 = _rtB -> B_1036_3431_0 * _rtB -> B_1036_923_0 ; _rtB ->
B_1036_3445_0 = _rtB -> B_1036_3100_0 + _rtB -> B_1036_3102_0 ; _rtB ->
B_1036_3446_0 = _rtB -> B_1036_3445_0 + _rtB -> B_1036_3098_0 ; _rtB ->
B_1036_3447_0 = _rtB -> B_1036_3446_0 * _rtB -> B_1036_923_0 ; _rtB ->
B_1036_3448_0 = _rtB -> B_1036_1625_0 * _rtB -> B_1036_950_0 ; _rtB ->
B_1036_3449_0 = _rtB -> B_1036_1629_0 * _rtB -> B_1036_952_0_c ; _rtB ->
B_1036_3450_0 = _rtB -> B_1036_3448_0 - _rtB -> B_1036_3449_0 ; _rtB ->
B_1036_3451_0 = _rtB -> B_1036_1660_0 * _rtB -> B_1036_950_0 ; _rtB ->
B_1036_3452_0 = _rtB -> B_1036_1664_0 * _rtB -> B_1036_953_0_o ; _rtB ->
B_1036_3453_0 = _rtB -> B_1036_3451_0 - _rtB -> B_1036_3452_0 ; _rtB ->
B_1036_3454_0 = _rtB -> B_1036_3450_0 + _rtB -> B_1036_3453_0 ; _rtB ->
B_1036_3455_0 = _rtB -> B_1036_1614_0 + _rtB -> B_1036_3454_0 ; _rtB ->
B_1036_3456_0 = _rtB -> B_1036_3455_0 * _rtB -> B_1036_951_0_o ; _rtB ->
B_1036_3457_0 = _rtB -> B_1036_3456_0 * _rtB -> B_1036_3456_0 ; _rtB ->
B_1036_3458_0 = _rtB -> B_1036_1629_0 * _rtB -> B_1036_950_0 ; _rtB ->
B_1036_3459_0 = _rtB -> B_1036_1625_0 * _rtB -> B_1036_952_0_c ; _rtB ->
B_1036_3460_0 = _rtB -> B_1036_3458_0 + _rtB -> B_1036_3459_0 ; _rtB ->
B_1036_3461_0 = _rtB -> B_1036_1664_0 * _rtB -> B_1036_950_0 ; _rtB ->
B_1036_3462_0 = _rtB -> B_1036_1660_0 * _rtB -> B_1036_953_0_o ; _rtB ->
B_1036_3463_0 = _rtB -> B_1036_3461_0 + _rtB -> B_1036_3462_0 ; _rtB ->
B_1036_3464_0 = _rtB -> B_1036_3460_0 + _rtB -> B_1036_3463_0 ; _rtB ->
B_1036_3465_0 = _rtB -> B_1036_1618_0 + _rtB -> B_1036_3464_0 ; _rtB ->
B_1036_3466_0 = _rtB -> B_1036_3465_0 * _rtB -> B_1036_951_0_o ; _rtB ->
B_1036_3467_0 = _rtB -> B_1036_3466_0 * _rtB -> B_1036_3466_0 ; _rtB ->
B_1036_3468_0 = _rtB -> B_1036_3457_0 + _rtB -> B_1036_3467_0 ; _rtB ->
B_1036_3469_0 = muDoubleScalarSqrt ( _rtB -> B_1036_3468_0 ) ; _rtB ->
B_1036_3470_0 = _rtB -> B_1036_3456_0 / _rtB -> B_1036_3469_0 ; _rtB ->
B_1036_3471_0 = _rtP -> P_682 * _rtB -> B_1036_318_0 ; _rtB -> B_1036_3472_0
= _rtP -> P_683 * _rtB -> B_1036_401_0 ; _rtB -> B_1036_3473_0 = _rtB ->
B_1036_3472_0 * _rtB -> B_1036_946_0 ; _rtB -> B_1036_3474_0 = _rtP -> P_684
* _rtB -> B_1036_421_0 ; _rtB -> B_1036_3475_0 = _rtB -> B_1036_3474_0 * _rtB
-> B_1036_948_0_o ; _rtB -> B_1036_3476_0 = _rtB -> B_1036_3473_0 - _rtB ->
B_1036_3475_0 ; _rtB -> B_1036_3477_0 = _rtP -> P_685 * _rtB -> B_1036_407_0
; _rtB -> B_1036_3478_0 = _rtB -> B_1036_3477_0 * _rtB -> B_1036_946_0 ; _rtB
-> B_1036_3479_0 = _rtP -> P_686 * _rtB -> B_1036_427_0 ; _rtB ->
B_1036_3480_0 = _rtB -> B_1036_3479_0 * _rtB -> B_1036_949_0_p ; _rtB ->
B_1036_3481_0 = _rtB -> B_1036_3478_0 - _rtB -> B_1036_3480_0 ; _rtB ->
B_1036_3482_0 = _rtB -> B_1036_3476_0 + _rtB -> B_1036_3481_0 ; _rtB ->
B_1036_3483_0 = _rtB -> B_1036_3471_0 + _rtB -> B_1036_3482_0 ; _rtB ->
B_1036_3484_0 = _rtB -> B_1036_3483_0 * _rtB -> B_1036_947_0_l ; _rtB ->
B_1036_3485_0 = _rtB -> B_1036_3484_0 * _rtB -> B_1036_3484_0 ; _rtB ->
B_1036_3486_0 = _rtP -> P_687 * _rtB -> B_1036_324_0 ; _rtB -> B_1036_3487_0
= _rtB -> B_1036_3474_0 * _rtB -> B_1036_946_0 ; _rtB -> B_1036_3488_0 = _rtB
-> B_1036_3472_0 * _rtB -> B_1036_948_0_o ; _rtB -> B_1036_3489_0 = _rtB ->
B_1036_3487_0 + _rtB -> B_1036_3488_0 ; _rtB -> B_1036_3490_0 = _rtB ->
B_1036_3479_0 * _rtB -> B_1036_946_0 ; _rtB -> B_1036_3491_0 = _rtB ->
B_1036_3477_0 * _rtB -> B_1036_949_0_p ; _rtB -> B_1036_3492_0 = _rtB ->
B_1036_3490_0 + _rtB -> B_1036_3491_0 ; _rtB -> B_1036_3493_0 = _rtB ->
B_1036_3489_0 + _rtB -> B_1036_3492_0 ; _rtB -> B_1036_3494_0 = _rtB ->
B_1036_3486_0 + _rtB -> B_1036_3493_0 ; _rtB -> B_1036_3495_0 = _rtB ->
B_1036_3494_0 * _rtB -> B_1036_947_0_l ; _rtB -> B_1036_3496_0 = _rtB ->
B_1036_3495_0 * _rtB -> B_1036_3495_0 ; _rtB -> B_1036_3497_0 = _rtB ->
B_1036_3485_0 + _rtB -> B_1036_3496_0 ; _rtB -> B_1036_3498_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_3497_0 ) ; _rtB -> B_1036_3499_0 = _rtB
-> B_1036_3484_0 / _rtB -> B_1036_3498_0 ; _rtB -> B_1036_3500_0 = _rtB ->
B_1036_3470_0 - _rtB -> B_1036_3499_0 ; _rtB -> B_1036_3501_0 = _rtB ->
B_1036_3500_0 * _rtB -> B_1036_3500_0 ; _rtB -> B_1036_3502_0 = _rtB ->
B_1036_3466_0 / _rtB -> B_1036_3469_0 ; _rtB -> B_1036_3503_0 = _rtB ->
B_1036_3495_0 / _rtB -> B_1036_3498_0 ; _rtB -> B_1036_3504_0 = _rtB ->
B_1036_3502_0 - _rtB -> B_1036_3503_0 ; _rtB -> B_1036_3505_0 = _rtB ->
B_1036_3504_0 * _rtB -> B_1036_3504_0 ; _rtB -> B_1036_3506_0 = _rtB ->
B_1036_3501_0 + _rtB -> B_1036_3505_0 ; _rtB -> B_1036_3507_0 = _rtB ->
B_1036_3506_0 * _rtB -> B_1036_938_0 ; _rtB -> B_1036_3508_0 = _rtB ->
B_1036_937_0 - _rtB -> B_1036_3507_0 ; u0 = _rtB -> B_1036_3508_0 ; if ( u0 >
1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB ->
B_1036_3509_0 = muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_3510_0 = _rtB ->
B_1036_3502_0 - _rtB -> B_1036_3499_0 ; _rtB -> B_1036_3511_0 = _rtB ->
B_1036_3510_0 * _rtB -> B_1036_3510_0 ; _rtB -> B_1036_3512_0 = _rtB ->
B_1036_3470_0 + _rtB -> B_1036_3503_0 ; _rtB -> B_1036_3513_0 = _rtB ->
B_1036_3512_0 * _rtB -> B_1036_3512_0 ; _rtB -> B_1036_3514_0 = _rtB ->
B_1036_3511_0 + _rtB -> B_1036_3513_0 ; _rtB -> B_1036_3515_0 = ( _rtB ->
B_1036_3514_0 > _rtB -> B_1036_935_0 ) ; if ( _rtB -> B_1036_3515_0 ) { _rtB
-> B_481_0_0 = _rtB -> B_1036_3509_0 * _rtB -> B_1036_936_0 ; _rtB ->
B_1036_3517_0 = _rtB -> B_481_0_0 ; } else { _rtB -> B_1036_3517_0 = _rtB ->
B_1036_3509_0 ; } _rtB -> B_1036_3518_0 = muDoubleScalarCos ( _rtB ->
B_1036_3517_0 ) ; _rtB -> B_1036_3519_0 = _rtB -> B_1036_3456_0 * _rtB ->
B_1036_3456_0 ; _rtB -> B_1036_3520_0 = _rtB -> B_1036_3466_0 * _rtB ->
B_1036_3466_0 ; _rtB -> B_1036_3521_0 = _rtB -> B_1036_3519_0 + _rtB ->
B_1036_3520_0 ; _rtB -> B_1036_3522_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_3521_0 ) ; _rtB -> B_1036_3523_0 = _rtB -> B_1036_3484_0 * _rtB ->
B_1036_3484_0 ; _rtB -> B_1036_3524_0 = _rtB -> B_1036_3495_0 * _rtB ->
B_1036_3495_0 ; _rtB -> B_1036_3525_0 = _rtB -> B_1036_3523_0 + _rtB ->
B_1036_3524_0 ; _rtB -> B_1036_3526_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_3525_0 ) ; _rtB -> B_1036_3527_0 = _rtB -> B_1036_3518_0 * _rtB ->
B_1036_3522_0 * _rtB -> B_1036_3526_0 * _rtB -> B_1036_928_0 ; _rtB ->
B_1036_3528_0 = ( _rtB -> B_1036_3527_0 > _rtB -> B_1036_926_0 ) ; _rtB ->
B_1036_3529_0 = ( ( _rtB -> B_1036_2202_0 != 0.0 ) && ( _rtB -> B_1036_2200_0
!= 0.0 ) && ( _rtB -> B_1036_2198_0 != 0.0 ) ) ; _rtB -> B_1036_3530_0 = (
_rtB -> B_1036_931_0_c && _rtB -> B_1036_3528_0 && _rtB -> B_1036_3529_0 ) ;
_rtB -> B_1036_3531_0 = ( _rtB -> B_1036_3527_0 > _rtB -> B_1036_927_0 ) ;
_rtB -> B_1036_3532_0 = false ; _rtB -> B_1036_3533_0 = ( _rtB ->
B_1036_934_0_k && _rtB -> B_1036_3528_0 && _rtB -> B_1036_3529_0 ) ; _rtB ->
B_1036_3534_0 = muDoubleScalarSin ( _rtB -> B_1036_3517_0 ) ; _rtB ->
B_1036_3535_0 = ssGetT ( S ) ; _rtB -> B_1036_3536_0 = _rtB -> B_1036_3533_0
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_3537_0 = _rtB -> B_1036_943_0 ; _rtB -> B_1036_3538_0 = _rtB ->
B_1036_942_0_c ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_3538_0 , _rtB ->
B_1036_3536_0 , _rtB -> B_1036_3535_0 , _rtB -> B_1036_940_0 , & _rtB ->
ONDelay_nt , & _rtDW -> ONDelay_nt , & _rtZCE -> ONDelay_nt ) ;
DZG_v3_OFFDelay ( S , _rtB -> B_1036_3537_0 , _rtB -> B_1036_3536_0 , _rtB ->
B_1036_3535_0 , _rtB -> B_1036_940_0 , & _rtB -> OFFDelay_pc , & _rtDW ->
OFFDelay_pc , & _rtZCE -> OFFDelay_pc ) ; _rtB -> B_1036_3541_0 = ( _rtB ->
ONDelay_nt . B_90_17_0 || _rtB -> OFFDelay_pc . B_86_19_0 ) ; _rtB ->
B_1036_3542_0 = _rtB -> B_1036_3474_0 * _rtB -> B_1036_946_0 ; _rtB ->
B_1036_3543_0 = _rtB -> B_1036_3472_0 * _rtB -> B_1036_949_0_p ; _rtB ->
B_1036_3544_0 = _rtB -> B_1036_3542_0 + _rtB -> B_1036_3543_0 ; _rtB ->
B_1036_3545_0 = _rtB -> B_1036_3479_0 * _rtB -> B_1036_946_0 ; _rtB ->
B_1036_3546_0 = _rtB -> B_1036_3477_0 * _rtB -> B_1036_948_0_o ; _rtB ->
B_1036_3547_0 = _rtB -> B_1036_3545_0 + _rtB -> B_1036_3546_0 ; _rtB ->
B_1036_3548_0 = _rtB -> B_1036_3544_0 + _rtB -> B_1036_3547_0 ; _rtB ->
B_1036_3549_0 = _rtB -> B_1036_3486_0 + _rtB -> B_1036_3548_0 ; _rtB ->
B_1036_3550_0 = _rtB -> B_1036_3474_0 + _rtB -> B_1036_3479_0 ; _rtB ->
B_1036_3551_0 = _rtB -> B_1036_3550_0 + _rtB -> B_1036_3486_0 ; _rtB ->
B_1036_3552_0 = _rtB -> B_1036_3551_0 * _rtB -> B_1036_947_0_l ; _rtB ->
B_1036_3553_0 = _rtB -> B_1036_3472_0 * _rtB -> B_1036_946_0 ; _rtB ->
B_1036_3554_0 = _rtB -> B_1036_3474_0 * _rtB -> B_1036_949_0_p ; _rtB ->
B_1036_3555_0 = _rtB -> B_1036_3553_0 - _rtB -> B_1036_3554_0 ; _rtB ->
B_1036_3556_0 = _rtB -> B_1036_3477_0 * _rtB -> B_1036_946_0 ; _rtB ->
B_1036_3557_0 = _rtB -> B_1036_3479_0 * _rtB -> B_1036_948_0_o ; _rtB ->
B_1036_3558_0 = _rtB -> B_1036_3556_0 - _rtB -> B_1036_3557_0 ; _rtB ->
B_1036_3559_0 = _rtB -> B_1036_3555_0 + _rtB -> B_1036_3558_0 ; _rtB ->
B_1036_3560_0 = _rtB -> B_1036_3471_0 + _rtB -> B_1036_3559_0 ; _rtB ->
B_1036_3561_0 = _rtB -> B_1036_3560_0 * _rtB -> B_1036_947_0_l ; _rtB ->
B_1036_3562_0 = _rtB -> B_1036_3549_0 * _rtB -> B_1036_947_0_l ; _rtB ->
B_1036_3563_0 = _rtB -> B_1036_3472_0 + _rtB -> B_1036_3477_0 ; _rtB ->
B_1036_3564_0 = _rtB -> B_1036_3563_0 + _rtB -> B_1036_3471_0 ; _rtB ->
B_1036_3565_0 = _rtB -> B_1036_3564_0 * _rtB -> B_1036_947_0_l ; _rtB ->
B_1036_3566_0 = _rtB -> B_1036_1629_0 * _rtB -> B_1036_950_0 ; _rtB ->
B_1036_3567_0 = _rtB -> B_1036_1625_0 * _rtB -> B_1036_953_0_o ; _rtB ->
B_1036_3568_0 = _rtB -> B_1036_3566_0 + _rtB -> B_1036_3567_0 ; _rtB ->
B_1036_3569_0 = _rtB -> B_1036_1664_0 * _rtB -> B_1036_950_0 ; _rtB ->
B_1036_3570_0 = _rtB -> B_1036_1660_0 * _rtB -> B_1036_952_0_c ; _rtB ->
B_1036_3571_0 = _rtB -> B_1036_3569_0 + _rtB -> B_1036_3570_0 ; _rtB ->
B_1036_3572_0 = _rtB -> B_1036_3568_0 + _rtB -> B_1036_3571_0 ; _rtB ->
B_1036_3573_0 = _rtB -> B_1036_1618_0 + _rtB -> B_1036_3572_0 ; _rtB ->
B_1036_3574_0 = _rtB -> B_1036_1629_0 + _rtB -> B_1036_1664_0 ; _rtB ->
B_1036_3575_0 = _rtB -> B_1036_3574_0 + _rtB -> B_1036_1618_0 ; _rtB ->
B_1036_3576_0 = _rtB -> B_1036_3575_0 * _rtB -> B_1036_951_0_o ; _rtB ->
B_1036_3577_0 = _rtB -> B_1036_1625_0 * _rtB -> B_1036_950_0 ; _rtB ->
B_1036_3578_0 = _rtB -> B_1036_1629_0 * _rtB -> B_1036_953_0_o ; _rtB ->
B_1036_3579_0 = _rtB -> B_1036_3577_0 - _rtB -> B_1036_3578_0 ; _rtB ->
B_1036_3580_0 = _rtB -> B_1036_1660_0 * _rtB -> B_1036_950_0 ; _rtB ->
B_1036_3581_0 = _rtB -> B_1036_1664_0 * _rtB -> B_1036_952_0_c ; _rtB ->
B_1036_3582_0 = _rtB -> B_1036_3580_0 - _rtB -> B_1036_3581_0 ; _rtB ->
B_1036_3583_0 = _rtB -> B_1036_3579_0 + _rtB -> B_1036_3582_0 ; _rtB ->
B_1036_3584_0 = _rtB -> B_1036_1614_0 + _rtB -> B_1036_3583_0 ; _rtB ->
B_1036_3585_0 = _rtB -> B_1036_3584_0 * _rtB -> B_1036_951_0_o ; _rtB ->
B_1036_3586_0 = _rtB -> B_1036_3573_0 * _rtB -> B_1036_951_0_o ; _rtB ->
B_1036_3587_0 = _rtB -> B_1036_1625_0 + _rtB -> B_1036_1660_0 ; _rtB ->
B_1036_3588_0 = _rtB -> B_1036_3587_0 + _rtB -> B_1036_1614_0 ; _rtB ->
B_1036_3589_0 = _rtB -> B_1036_3588_0 * _rtB -> B_1036_951_0_o ; _rtB ->
B_1036_3590_0 = ssGetT ( S ) ; _rtB -> B_1036_3591_0 = _rtB -> B_1036_3530_0
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_3592_0 = _rtB -> B_1036_958_0_g ; _rtB -> B_1036_3593_0 = _rtB ->
B_1036_957_0_c ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_3593_0 , _rtB ->
B_1036_3591_0 , _rtB -> B_1036_3590_0 , _rtB -> B_1036_955_0_h , & _rtB ->
ONDelay_bz , & _rtDW -> ONDelay_bz , & _rtZCE -> ONDelay_bz ) ;
DZG_v3_OFFDelay ( S , _rtB -> B_1036_3592_0 , _rtB -> B_1036_3591_0 , _rtB ->
B_1036_3590_0 , _rtB -> B_1036_955_0_h , & _rtB -> OFFDelay_mm , & _rtDW ->
OFFDelay_mm , & _rtZCE -> OFFDelay_mm ) ; _rtB -> B_1036_3596_0 = ( _rtB ->
ONDelay_bz . B_90_17_0 || _rtB -> OFFDelay_mm . B_86_19_0 ) ; _rtB ->
B_1036_3597_0 = ssGetT ( S ) ; _rtB -> B_1036_3598_0 = _rtB -> B_1036_3532_0
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_3599_0 = _rtB -> B_1036_965_0_h ; _rtB -> B_1036_3600_0 = _rtB ->
B_1036_964_0_f ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_3600_0 , _rtB ->
B_1036_3598_0 , _rtB -> B_1036_3597_0 , _rtB -> B_1036_962_0_g , & _rtB ->
ONDelay_i1 , & _rtDW -> ONDelay_i1 , & _rtZCE -> ONDelay_i1 ) ;
DZG_v3_OFFDelay ( S , _rtB -> B_1036_3599_0 , _rtB -> B_1036_3598_0 , _rtB ->
B_1036_3597_0 , _rtB -> B_1036_962_0_g , & _rtB -> OFFDelay_b3 , & _rtDW ->
OFFDelay_b3 , & _rtZCE -> OFFDelay_b3 ) ; _rtB -> B_1036_3603_0 = ( _rtB ->
ONDelay_i1 . B_90_17_0 || _rtB -> OFFDelay_b3 . B_86_19_0 ) ; _rtB ->
B_1036_3604_0 = _rtB -> B_1036_3161_0 + _rtB -> B_1036_3163_0 ; _rtB ->
B_1036_3605_0 = _rtB -> B_1036_3604_0 + _rtB -> B_1036_3159_0 ; _rtB ->
B_1036_3606_0 = _rtB -> B_1036_3605_0 * _rtB -> B_1036_1022_0_f ; _rtB ->
B_1036_3607_0 = _rtB -> B_1036_3606_0 + _rtB -> B_1036_3213_0 ; _rtB ->
B_1036_3608_0 = _rtB -> B_1036_3162_0 + _rtB -> B_1036_3164_0 ; _rtB ->
B_1036_3609_0 = _rtB -> B_1036_3608_0 + _rtB -> B_1036_3160_0 ; _rtB ->
B_1036_3610_0 = _rtB -> B_1036_3609_0 * _rtB -> B_1036_1022_0_f ; _rtB ->
B_1036_3611_0 = _rtB -> B_1036_3610_0 + _rtB -> B_1036_3214_0 ; _rtB ->
B_1036_3612_0 = _rtB -> B_1036_3213_0 * _rtB -> B_1036_3213_0 ; _rtB ->
B_1036_3613_0 = _rtB -> B_1036_3214_0 * _rtB -> B_1036_3214_0 ; _rtB ->
B_1036_3614_0 = _rtB -> B_1036_3612_0 + _rtB -> B_1036_3613_0 ; _rtB ->
B_1036_3615_0 = muDoubleScalarSqrt ( _rtB -> B_1036_3614_0 ) ; _rtB ->
B_1036_3616_0 = _rtB -> B_1036_3615_0 * _rtB -> B_1036_970_0_o ; _rtB ->
B_1036_3617_0 = _rtB -> B_1036_3211_0 * _rtB -> B_1036_3211_0 ; _rtB ->
B_1036_3618_0 = _rtB -> B_1036_3212_0 * _rtB -> B_1036_3212_0 ; _rtB ->
B_1036_3619_0 = _rtB -> B_1036_3617_0 + _rtB -> B_1036_3618_0 ; _rtB ->
B_1036_3620_0 = muDoubleScalarSqrt ( _rtB -> B_1036_3619_0 ) ; if ( _rtB ->
B_1036_999_0_j > _rtP -> P_688 ) { _rtB -> B_543_0_0 = ( _rtB ->
B_1036_3620_0 < _rtB -> B_1036_1002_0_c ) ; _rtB -> B_1036_3623_0 = _rtB ->
B_543_0_0 ; } else { _rtB -> B_542_0_0 = ( _rtB -> B_1036_3620_0 > _rtB ->
B_1036_1002_0_c ) ; _rtB -> B_1036_3623_0 = _rtB -> B_542_0_0 ; } _rtB ->
B_1036_3624_0 = ( _rtB -> B_1036_3623_0 && _rtB -> B_1036_1004_0_a ) ; if (
_rtB -> B_1036_997_0_j ) { _rtB -> B_1036_3626_0 = _rtB -> B_1036_3624_0 ; }
else { _rtB -> B_541_0_0 = ( _rtB -> B_1036_3624_0 && _rtB -> B_1036_1008_0_e
) ; _rtB -> B_1036_3626_0 = _rtB -> B_541_0_0 ; } _rtB -> B_1036_3627_0 = (
_rtB -> B_1036_974_0_i && _rtB -> B_1036_3626_0 && _rtB -> B_1036_981_0_p ) ;
_rtB -> B_1036_3628_0 = _rtB -> B_1036_3607_0 * _rtB -> B_1036_3607_0 ; _rtB
-> B_1036_3629_0 = _rtB -> B_1036_3611_0 * _rtB -> B_1036_3611_0 ; _rtB ->
B_1036_3630_0 = _rtB -> B_1036_3628_0 + _rtB -> B_1036_3629_0 ; _rtB ->
B_1036_3631_0 = muDoubleScalarSqrt ( _rtB -> B_1036_3630_0 ) ; _rtB ->
B_1036_3632_0 = ( _rtB -> B_1036_3631_0 >= _rtB -> B_1036_3616_0 ) ; _rtB ->
B_1036_3633_0 = ( _rtB -> B_1036_976_0_l && _rtB -> B_1036_3632_0 && _rtB ->
B_1036_980_0_g ) ; _rtB -> B_1036_3634_0 = _rtB -> B_1036_3627_0 ; _rtB ->
B_1036_3635_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_3636_0 = _rtB -> B_1036_985_0_m ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_3636_0 , _rtB -> B_1036_3634_0 , _rtB ->
B_1036_3635_0 , _rtB -> B_1036_983_0_a , & _rtB -> ONDelay_fn , & _rtDW ->
ONDelay_fn , & _rtZCE -> ONDelay_fn ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_3638_0 = _rtB -> B_1036_986_0_m ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_3638_0 , _rtB -> B_1036_3634_0 , _rtB ->
B_1036_3635_0 , _rtB -> B_1036_983_0_a , & _rtB -> OFFDelay_hq , & _rtDW ->
OFFDelay_hq , & _rtZCE -> OFFDelay_hq ) ; _rtB -> B_1036_3640_0 = ( _rtB ->
ONDelay_fn . B_90_17_0 || _rtB -> OFFDelay_hq . B_86_19_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3641_0 =
_rtDW -> Memory_PreviousInput_dl ; _rtB -> B_1036_3642_0 = ! _rtB ->
B_1036_3641_0 ; } _rtB -> B_1036_3643_0 = ( _rtB -> B_1036_3640_0 && _rtB ->
B_1036_3642_0 ) ; _rtB -> B_1036_3644_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3645_0 =
_rtDW -> Memory_PreviousInput_fub ; _rtB -> B_1036_3646_0 = ! _rtB ->
B_1036_3645_0 ; } _rtB -> B_1036_3647_0 = _rtB -> B_1036_3640_0 ; _rtB ->
B_1036_3648_0 = _rtB -> B_1036_3647_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_3649_0 = _rtDW -> Memory_PreviousInput_m2
; _rtB -> B_1036_3650_0 = _rtB -> B_1036_1020_0_b [ 0 ] ; }
DZG_v3_POSITIVEEdge ( S , _rtB -> B_1036_3650_0 , _rtB -> B_1036_3648_0 ,
_rtB -> B_1036_3649_0 , & _rtB -> POSITIVEEdge_o , & _rtDW -> POSITIVEEdge_o
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_3652_0 = _rtB -> B_1036_1020_0_b [ 1 ] ; } DZG_v3_NEGATIVEEdge ( S ,
_rtB -> B_1036_3652_0 , _rtB -> B_1036_3648_0 , _rtB -> B_1036_3649_0 , &
_rtB -> NEGATIVEEdge_gr , & _rtDW -> NEGATIVEEdge_gr ) ; _rtB ->
B_1036_3654_0 = ( _rtB -> POSITIVEEdge_o . B_85_0_0 || _rtB ->
NEGATIVEEdge_gr . B_84_0_0 ) ; _rtB -> B_1036_3655_0 = ( _rtB ->
B_1036_3646_0 && _rtB -> B_1036_3654_0 ) ; _rtB -> B_1036_3656_0 = _rtB ->
B_1036_3655_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_3657_0 = _rtDW -> ICic_PreviousInput_p ; } if ( _rtB ->
B_1036_3656_0 ) { _rtB -> B_1036_3658_0 = _rtB -> B_1036_3644_0 ; } else {
_rtB -> B_1036_3658_0 = _rtB -> B_1036_3657_0 ; } isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3659_0 = _rtP -> P_690 ; } _rtB
-> B_1036_3660_0 = _rtB -> B_1036_3658_0 + _rtB -> B_1036_3659_0 ; _rtB ->
B_1036_3661_0 = ( _rtB -> B_1036_3660_0 > _rtB -> B_1036_3644_0 ) ; _rtB ->
B_1036_3662_0 = ( _rtB -> B_1036_3643_0 || _rtB -> B_1036_3661_0 ) ; _rtB ->
B_1036_3663_0 = _rtB -> B_1036_3633_0 ; _rtB -> B_1036_3664_0 = ssGetT ( S )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_3665_0 = _rtB -> B_1036_992_0_p ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_3665_0 , _rtB -> B_1036_3663_0 , _rtB -> B_1036_3664_0 , _rtB ->
B_1036_990_0_k , & _rtB -> ONDelay_ha , & _rtDW -> ONDelay_ha , & _rtZCE ->
ONDelay_ha ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_3667_0 = _rtB -> B_1036_993_0_j ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_3667_0 , _rtB -> B_1036_3663_0 , _rtB -> B_1036_3664_0 , _rtB ->
B_1036_990_0_k , & _rtB -> OFFDelay_ai , & _rtDW -> OFFDelay_ai , & _rtZCE ->
OFFDelay_ai ) ; _rtB -> B_1036_3669_0 = ( _rtB -> ONDelay_ha . B_90_17_0 ||
_rtB -> OFFDelay_ai . B_86_19_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if
( isHit != 0 ) { _rtB -> B_1036_3670_0 = _rtDW -> Memory_PreviousInput_j2 ;
_rtB -> B_1036_3671_0 = ! _rtB -> B_1036_3670_0 ; } _rtB -> B_1036_3672_0 = (
_rtB -> B_1036_3669_0 && _rtB -> B_1036_3671_0 ) ; _rtB -> B_1036_3673_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_3674_0 = _rtDW -> Memory_PreviousInput_eo ; _rtB -> B_1036_3675_0 =
! _rtB -> B_1036_3674_0 ; } _rtB -> B_1036_3676_0 = _rtB -> B_1036_3669_0 ;
_rtB -> B_1036_3677_0 = _rtB -> B_1036_3676_0 ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3678_0 = _rtDW ->
Memory_PreviousInput_hh ; _rtB -> B_1036_3679_0 = _rtB -> B_1036_1014_0 [ 0 ]
; } DZG_v3_POSITIVEEdge ( S , _rtB -> B_1036_3679_0 , _rtB -> B_1036_3677_0 ,
_rtB -> B_1036_3678_0 , & _rtB -> POSITIVEEdge_l , & _rtDW -> POSITIVEEdge_l
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_3681_0 = _rtB -> B_1036_1014_0 [ 1 ] ; } DZG_v3_NEGATIVEEdge ( S ,
_rtB -> B_1036_3681_0 , _rtB -> B_1036_3677_0 , _rtB -> B_1036_3678_0 , &
_rtB -> NEGATIVEEdge_e , & _rtDW -> NEGATIVEEdge_e ) ; _rtB -> B_1036_3683_0
= ( _rtB -> POSITIVEEdge_l . B_85_0_0 || _rtB -> NEGATIVEEdge_e . B_84_0_0 )
; _rtB -> B_1036_3684_0 = ( _rtB -> B_1036_3675_0 && _rtB -> B_1036_3683_0 )
; _rtB -> B_1036_3685_0 = _rtB -> B_1036_3684_0 ; isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3686_0 = _rtDW ->
ICic_PreviousInput_g2 ; } if ( _rtB -> B_1036_3685_0 ) { _rtB ->
B_1036_3687_0 = _rtB -> B_1036_3673_0 ; } else { _rtB -> B_1036_3687_0 = _rtB
-> B_1036_3686_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_1036_3688_0 = _rtP -> P_692 ; } _rtB -> B_1036_3689_0 = _rtB ->
B_1036_3687_0 + _rtB -> B_1036_3688_0 ; _rtB -> B_1036_3690_0 = ( _rtB ->
B_1036_3689_0 > _rtB -> B_1036_3673_0 ) ; _rtB -> B_1036_3691_0 = ( _rtB ->
B_1036_3672_0 || _rtB -> B_1036_3690_0 ) ; _rtB -> B_1036_3692_0 = ( _rtB ->
B_1036_3662_0 || _rtB -> B_1036_3691_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_3693_0 = _rtDW ->
Memory_PreviousInput_mdi ; _rtB -> B_1036_3694_0 = ! _rtB -> B_1036_3693_0 ;
} _rtB -> B_1036_3695_0 = ( _rtB -> B_1036_3626_0 && _rtB -> B_1036_3694_0 )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_3696_0 = _rtDW -> Memory_PreviousInput_mr ; _rtB -> B_1036_3697_0 = !
_rtB -> B_1036_3696_0 ; } _rtB -> B_1036_3698_0 = ( _rtB -> B_1036_3632_0 &&
_rtB -> B_1036_3697_0 ) ; _rtB -> B_1036_3699_0 = ( _rtB -> B_1036_3695_0 ||
_rtB -> B_1036_3698_0 ) ; _rtB -> B_1036_3700_0 = ssGetT ( S ) ; _rtB ->
B_1036_3701_0 = _rtB -> B_1036_3683_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { DZG_v3_TriggeredSubsystem ( S , _rtB -> B_1036_3701_0 ,
_rtB -> B_1036_3700_0 , & _rtB -> TriggeredSubsystem_m , & _rtDW ->
TriggeredSubsystem_m , & _rtZCE -> TriggeredSubsystem_m ) ; _rtB ->
B_1036_3703_0 = _rtB -> TriggeredSubsystem_m . B_83_0_0 + _rtB ->
B_1036_1009_0 ; } _rtB -> B_1036_3704_0 = ( _rtB -> B_1036_3703_0 > _rtB ->
B_1036_3700_0 ) ; _rtB -> B_1036_3705_0 = ssGetT ( S ) ; _rtB ->
B_1036_3706_0 = _rtB -> B_1036_3654_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { DZG_v3_TriggeredSubsystem ( S , _rtB -> B_1036_3706_0 ,
_rtB -> B_1036_3705_0 , & _rtB -> TriggeredSubsystem_l , & _rtDW ->
TriggeredSubsystem_l , & _rtZCE -> TriggeredSubsystem_l ) ; _rtB ->
B_1036_3708_0 = _rtB -> TriggeredSubsystem_l . B_83_0_0 + _rtB ->
B_1036_1015_0 ; } _rtB -> B_1036_3709_0 = ( _rtB -> B_1036_3708_0 > _rtB ->
B_1036_3705_0 ) ; _rtB -> B_1036_3710_0 = _rtB -> B_1036_3162_0 * _rtB ->
B_1036_1021_0_g ; _rtB -> B_1036_3711_0 = _rtB -> B_1036_3161_0 * _rtB ->
B_1036_1024_0_c ; _rtB -> B_1036_3712_0 = _rtB -> B_1036_3710_0 + _rtB ->
B_1036_3711_0 ; _rtB -> B_1036_3713_0 = _rtB -> B_1036_3164_0 * _rtB ->
B_1036_1021_0_g ; _rtB -> B_1036_3714_0 = _rtB -> B_1036_3163_0 * _rtB ->
B_1036_1023_0 ; _rtB -> B_1036_3715_0 = _rtB -> B_1036_3713_0 + _rtB ->
B_1036_3714_0 ; _rtB -> B_1036_3716_0 = _rtB -> B_1036_3712_0 + _rtB ->
B_1036_3715_0 ; _rtB -> B_1036_3717_0 = _rtB -> B_1036_3160_0 + _rtB ->
B_1036_3716_0 ; _rtB -> B_1036_3718_0 = _rtB -> B_1036_3162_0 * _rtB ->
B_1036_1021_0_g ; _rtB -> B_1036_3719_0 = _rtB -> B_1036_3161_0 * _rtB ->
B_1036_1023_0 ; _rtB -> B_1036_3720_0 = _rtB -> B_1036_3718_0 + _rtB ->
B_1036_3719_0 ; _rtB -> B_1036_3721_0 = _rtB -> B_1036_3164_0 * _rtB ->
B_1036_1021_0_g ; _rtB -> B_1036_3722_0 = _rtB -> B_1036_3163_0 * _rtB ->
B_1036_1024_0_c ; _rtB -> B_1036_3723_0 = _rtB -> B_1036_3721_0 + _rtB ->
B_1036_3722_0 ; _rtB -> B_1036_3724_0 = _rtB -> B_1036_3720_0 + _rtB ->
B_1036_3723_0 ; _rtB -> B_1036_3725_0 = _rtB -> B_1036_3160_0 + _rtB ->
B_1036_3724_0 ; _rtB -> B_1036_3726_0 = _rtB -> B_1036_3161_0 * _rtB ->
B_1036_1021_0_g ; _rtB -> B_1036_3727_0 = _rtB -> B_1036_3162_0 * _rtB ->
B_1036_1023_0 ; _rtB -> B_1036_3728_0 = _rtB -> B_1036_3726_0 - _rtB ->
B_1036_3727_0 ; _rtB -> B_1036_3729_0 = _rtB -> B_1036_3163_0 * _rtB ->
B_1036_1021_0_g ; _rtB -> B_1036_3730_0 = _rtB -> B_1036_3164_0 * _rtB ->
B_1036_1024_0_c ; _rtB -> B_1036_3731_0 = _rtB -> B_1036_3729_0 - _rtB ->
B_1036_3730_0 ; _rtB -> B_1036_3732_0 = _rtB -> B_1036_3728_0 + _rtB ->
B_1036_3731_0 ; _rtB -> B_1036_3733_0 = _rtB -> B_1036_3159_0 + _rtB ->
B_1036_3732_0 ; _rtB -> B_1036_3734_0 = _rtB -> B_1036_3733_0 * _rtB ->
B_1036_1022_0_f ; _rtB -> B_1036_3735_0 = _rtB -> B_1036_3161_0 * _rtB ->
B_1036_1021_0_g ; _rtB -> B_1036_3736_0 = _rtB -> B_1036_3162_0 * _rtB ->
B_1036_1024_0_c ; _rtB -> B_1036_3737_0 = _rtB -> B_1036_3735_0 - _rtB ->
B_1036_3736_0 ; _rtB -> B_1036_3738_0 = _rtB -> B_1036_3163_0 * _rtB ->
B_1036_1021_0_g ; _rtB -> B_1036_3739_0 = _rtB -> B_1036_3164_0 * _rtB ->
B_1036_1023_0 ; _rtB -> B_1036_3740_0 = _rtB -> B_1036_3738_0 - _rtB ->
B_1036_3739_0 ; _rtB -> B_1036_3741_0 = _rtB -> B_1036_3737_0 + _rtB ->
B_1036_3740_0 ; _rtB -> B_1036_3742_0 = _rtB -> B_1036_3159_0 + _rtB ->
B_1036_3741_0 ; _rtB -> B_1036_3743_0 = _rtB -> B_1036_3742_0 * _rtB ->
B_1036_1022_0_f ; _rtB -> B_1036_3744_0 = _rtB -> B_1036_3717_0 * _rtB ->
B_1036_1022_0_f ; _rtB -> B_1036_3745_0 = _rtB -> B_1036_3725_0 * _rtB ->
B_1036_1022_0_f ; _rtB -> B_1036_3746_0 = _rtB -> B_1036_318_0 * _rtB ->
B_1036_318_0 ; _rtB -> B_1036_3747_0 = _rtB -> B_1036_324_0 * _rtB ->
B_1036_324_0 ; _rtB -> B_1036_3748_0 = _rtB -> B_1036_3746_0 + _rtB ->
B_1036_3747_0 ; _rtB -> B_1036_3749_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_3748_0 ) ; _rtB -> B_1036_3750_0 = _rtB -> B_1036_401_0 * _rtB ->
B_1036_401_0 ; _rtB -> B_1036_3751_0 = _rtB -> B_1036_421_0 * _rtB ->
B_1036_421_0 ; _rtB -> B_1036_3752_0 = _rtB -> B_1036_3750_0 + _rtB ->
B_1036_3751_0 ; _rtB -> B_1036_3753_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_3752_0 ) ; _rtB -> B_1036_3754_0 = ( _rtB -> B_1036_3749_0 >= _rtB ->
B_1036_3753_0 ) ; if ( _rtB -> B_1036_3754_0 ) { _rtB -> B_1036_3755_0 = _rtB
-> B_1036_3749_0 ; } else { _rtB -> B_1036_3755_0 = _rtB -> B_1036_3753_0 ; }
_rtB -> B_1036_3756_0 = _rtB -> B_1036_407_0 * _rtB -> B_1036_407_0 ; _rtB ->
B_1036_3757_0 = _rtB -> B_1036_427_0 * _rtB -> B_1036_427_0 ; _rtB ->
B_1036_3758_0 = _rtB -> B_1036_3756_0 + _rtB -> B_1036_3757_0 ; _rtB ->
B_1036_3759_0 = muDoubleScalarSqrt ( _rtB -> B_1036_3758_0 ) ; _rtB ->
B_1036_3760_0 = ( _rtB -> B_1036_3755_0 >= _rtB -> B_1036_3759_0 ) ; if (
_rtB -> B_1036_3760_0 ) { if ( _rtB -> B_1036_3754_0 ) { _rtB -> B_550_0_0 =
_rtB -> B_1036_318_0 ; } else { _rtB -> B_550_0_0 = _rtB -> B_1036_401_0 ; }
_rtB -> B_1036_3762_0 = _rtB -> B_550_0_0 ; if ( _rtB -> B_1036_3754_0 ) {
_rtB -> B_551_0_0 = _rtB -> B_1036_324_0 ; } else { _rtB -> B_551_0_0 = _rtB
-> B_1036_421_0 ; } _rtB -> B_1036_3765_0 = _rtB -> B_551_0_0 ; } else { _rtB
-> B_1036_3762_0 = _rtB -> B_1036_407_0 ; _rtB -> B_1036_3765_0 = _rtB ->
B_1036_427_0 ; } _rtB -> B_1036_3763_0 = _rtB -> B_1036_3762_0 * _rtB ->
B_1036_3762_0 ; _rtB -> B_1036_3766_0 = _rtB -> B_1036_3765_0 * _rtB ->
B_1036_3765_0 ; _rtB -> B_1036_3767_0 = _rtB -> B_1036_3763_0 + _rtB ->
B_1036_3766_0 ; _rtB -> B_1036_3768_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_3767_0 ) ; if ( _rtB -> B_1036_1079_0 > _rtP -> P_693 ) { _rtB ->
B_586_0_0 = ( _rtB -> B_1036_3768_0 < _rtB -> B_1036_1082_0 ) ; _rtB ->
B_1036_3771_0 = _rtB -> B_586_0_0 ; } else { _rtB -> B_585_0_0 = ( _rtB ->
B_1036_3768_0 > _rtB -> B_1036_1082_0 ) ; _rtB -> B_1036_3771_0 = _rtB ->
B_585_0_0 ; } _rtB -> B_1036_3772_0 = ( _rtB -> B_1036_3771_0 && _rtB ->
B_1036_1084_0_j ) ; if ( _rtB -> B_1036_1077_0_h ) { _rtB -> B_1036_3774_0 =
_rtB -> B_1036_3772_0 ; } else { _rtB -> B_584_0_0 = ( _rtB -> B_1036_3772_0
&& _rtB -> B_1036_1088_0_i ) ; _rtB -> B_1036_3774_0 = _rtB -> B_584_0_0 ; }
_rtB -> B_1036_3775_0 = ! _rtB -> B_1036_3774_0 ; _rtB -> B_1036_3776_0 =
_rtB -> B_1036_3161_0 + _rtB -> B_1036_3163_0 ; _rtB -> B_1036_3777_0 = _rtB
-> B_1036_3776_0 + _rtB -> B_1036_3159_0 ; _rtB -> B_1036_3778_0 = _rtB ->
B_1036_3777_0 * _rtB -> B_1036_1162_0_d ; _rtB -> B_1036_3779_0 = _rtB ->
B_1036_3778_0 * _rtB -> B_1036_1027_0_n ; _rtB -> B_1036_3780_0 = _rtB ->
B_1036_3779_0 + _rtB -> B_1036_3213_0 ; _rtB -> B_1036_3781_0 = _rtB ->
B_1036_3780_0 * _rtB -> B_1036_3780_0 ; _rtB -> B_1036_3782_0 = _rtB ->
B_1036_3162_0 + _rtB -> B_1036_3164_0 ; _rtB -> B_1036_3783_0 = _rtB ->
B_1036_3782_0 + _rtB -> B_1036_3160_0 ; _rtB -> B_1036_3784_0 = _rtB ->
B_1036_3783_0 * _rtB -> B_1036_1162_0_d ; _rtB -> B_1036_3785_0 = _rtB ->
B_1036_3784_0 * _rtB -> B_1036_1027_0_n ; _rtB -> B_1036_3786_0 = _rtB ->
B_1036_3785_0 + _rtB -> B_1036_3214_0 ; _rtB -> B_1036_3787_0 = _rtB ->
B_1036_3786_0 * _rtB -> B_1036_3786_0 ; _rtB -> B_1036_3788_0 = _rtB ->
B_1036_3781_0 + _rtB -> B_1036_3787_0 ; _rtB -> B_1036_3789_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_3788_0 ) ; _rtB -> B_1036_3790_0 = _rtB
-> B_1036_3213_0 * _rtB -> B_1036_3213_0 ; _rtB -> B_1036_3791_0 = _rtB ->
B_1036_3214_0 * _rtB -> B_1036_3214_0 ; _rtB -> B_1036_3792_0 = _rtB ->
B_1036_3790_0 + _rtB -> B_1036_3791_0 ; _rtB -> B_1036_3793_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_3792_0 ) ; _rtB -> B_1036_3794_0 = _rtB
-> B_1036_3793_0 * _rtB -> B_1036_1028_0_h ; _rtB -> B_1036_3795_0 = ( _rtB
-> B_1036_3789_0 >= _rtB -> B_1036_3794_0 ) ; _rtB -> B_1036_3796_0 = ( _rtB
-> B_1036_1089_0 && _rtB -> B_1036_3795_0 ) ; _rtB -> B_1036_3797_0 = ( _rtB
-> B_1036_3775_0 && _rtB -> B_1036_3796_0 ) ; _rtB -> B_1036_3798_0 = _rtB ->
B_1036_3778_0 * _rtB -> B_1036_3213_0 ; _rtB -> B_1036_3799_0 = _rtB ->
B_1036_3784_0 * _rtB -> B_1036_3214_0 ; _rtB -> B_1036_3800_0 = _rtB ->
B_1036_3798_0 + _rtB -> B_1036_3799_0 ; _rtB -> B_1036_3801_0 = _rtB ->
B_1036_3213_0 * _rtB -> B_1036_3213_0 ; _rtB -> B_1036_3802_0 = _rtB ->
B_1036_3214_0 * _rtB -> B_1036_3214_0 ; _rtB -> B_1036_3803_0 = _rtB ->
B_1036_3801_0 + _rtB -> B_1036_3802_0 ; _rtB -> B_1036_3804_0 = ( _rtB ->
B_1036_3803_0 < _rtB -> B_1036_1142_0_b ) ; if ( _rtB -> B_1036_3804_0 ) {
_rtB -> B_1036_3805_0 = _rtB -> B_1036_1142_0_b ; } else { _rtB ->
B_1036_3805_0 = _rtB -> B_1036_3803_0 ; } _rtB -> B_1036_3806_0 = _rtB ->
B_1036_3800_0 / _rtB -> B_1036_3805_0 ; _rtB -> B_1036_3807_0 = _rtB ->
B_1036_3806_0 * _rtB -> B_1036_3806_0 ; _rtB -> B_1036_3808_0 = _rtB ->
B_1036_3784_0 * _rtB -> B_1036_3213_0 ; _rtB -> B_1036_3809_0 = _rtB ->
B_1036_3778_0 * _rtB -> B_1036_3214_0 ; _rtB -> B_1036_3810_0 = _rtB ->
B_1036_3808_0 - _rtB -> B_1036_3809_0 ; _rtB -> B_1036_3811_0 = _rtB ->
B_1036_3810_0 / _rtB -> B_1036_3805_0 ; _rtB -> B_1036_3812_0 = _rtB ->
B_1036_3811_0 * _rtB -> B_1036_3811_0 ; _rtB -> B_1036_3813_0 = _rtB ->
B_1036_3807_0 + _rtB -> B_1036_3812_0 ; _rtB -> B_1036_3814_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_3813_0 ) ; if ( _rtB -> B_1036_1093_0 >
_rtP -> P_694 ) { _rtB -> B_589_0_0 = ( _rtB -> B_1036_3814_0 < _rtB ->
B_1036_1096_0 ) ; _rtB -> B_1036_3817_0 = _rtB -> B_589_0_0 ; } else { _rtB
-> B_588_0_0 = ( _rtB -> B_1036_3814_0 > _rtB -> B_1036_1096_0 ) ; _rtB ->
B_1036_3817_0 = _rtB -> B_588_0_0 ; } _rtB -> B_1036_3818_0 = ( _rtB ->
B_1036_3817_0 && _rtB -> B_1036_1098_0_a ) ; if ( _rtB -> B_1036_1091_0_h ) {
_rtB -> B_1036_3820_0 = _rtB -> B_1036_3818_0 ; } else { _rtB -> B_587_0_0 =
( _rtB -> B_1036_3818_0 && _rtB -> B_1036_1102_0_n ) ; _rtB -> B_1036_3820_0
= _rtB -> B_587_0_0 ; } _rtB -> B_1036_3821_0 = ( _rtB -> B_1036_3820_0 &&
_rtB -> B_1036_3774_0 ) ; _rtB -> B_1036_3822_0 = _rtB -> B_1036_3211_0 *
_rtB -> B_1036_3211_0 ; _rtB -> B_1036_3823_0 = _rtB -> B_1036_3212_0 * _rtB
-> B_1036_3212_0 ; _rtB -> B_1036_3824_0 = _rtB -> B_1036_3822_0 + _rtB ->
B_1036_3823_0 ; _rtB -> B_1036_3825_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_3824_0 ) ; if ( _rtB -> B_1036_1132_0_m > _rtP -> P_695 ) { _rtB ->
B_598_0_0 = ( _rtB -> B_1036_3825_0 < _rtB -> B_1036_1135_0 ) ; _rtB ->
B_1036_3828_0 = _rtB -> B_598_0_0 ; } else { _rtB -> B_597_0_0 = ( _rtB ->
B_1036_3825_0 > _rtB -> B_1036_1135_0 ) ; _rtB -> B_1036_3828_0 = _rtB ->
B_597_0_0 ; } _rtB -> B_1036_3829_0 = ( _rtB -> B_1036_3828_0 && _rtB ->
B_1036_1137_0_b ) ; if ( _rtB -> B_1036_1130_0_b ) { _rtB -> B_1036_3831_0 =
_rtB -> B_1036_3829_0 ; } else { _rtB -> B_596_0_0 = ( _rtB -> B_1036_3829_0
&& _rtB -> B_1036_1141_0_d ) ; _rtB -> B_1036_3831_0 = _rtB -> B_596_0_0 ; }
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_3832_0 = _rtDW -> Memory_PreviousInput_nb ; _rtB -> B_1036_3833_0 = !
_rtB -> B_1036_3832_0 ; } _rtB -> B_1036_3834_0 = ( _rtB -> B_1036_3831_0 &&
_rtB -> B_1036_3833_0 ) ; _rtB -> B_1036_3835_0 = _rtB -> B_1036_1625_0 +
_rtB -> B_1036_1660_0 ; _rtB -> B_1036_3836_0 = _rtB -> B_1036_3835_0 + _rtB
-> B_1036_1614_0 ; _rtB -> B_1036_3837_0 = _rtB -> B_1036_3836_0 * _rtB ->
B_1036_1166_0 ; _rtB -> B_1036_3838_0 = _rtB -> B_1036_3837_0 * _rtB ->
B_1036_3837_0 ; _rtB -> B_1036_3839_0 = _rtB -> B_1036_1629_0 + _rtB ->
B_1036_1664_0 ; _rtB -> B_1036_3840_0 = _rtB -> B_1036_3839_0 + _rtB ->
B_1036_1618_0 ; _rtB -> B_1036_3841_0 = _rtB -> B_1036_3840_0 * _rtB ->
B_1036_1166_0 ; _rtB -> B_1036_3842_0 = _rtB -> B_1036_3841_0 * _rtB ->
B_1036_3841_0 ; _rtB -> B_1036_3843_0 = _rtB -> B_1036_3838_0 + _rtB ->
B_1036_3842_0 ; _rtB -> B_1036_3844_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_3843_0 ) ; if ( _rtB -> B_1036_1106_0_m > _rtP -> P_696 ) { _rtB ->
B_592_0_0 = ( _rtB -> B_1036_3844_0 < _rtB -> B_1036_1109_0 ) ; _rtB ->
B_1036_3847_0 = _rtB -> B_592_0_0 ; } else { _rtB -> B_591_0_0 = ( _rtB ->
B_1036_3844_0 > _rtB -> B_1036_1109_0 ) ; _rtB -> B_1036_3847_0 = _rtB ->
B_591_0_0 ; } _rtB -> B_1036_3848_0 = ( _rtB -> B_1036_3847_0 && _rtB ->
B_1036_1111_0_i ) ; if ( _rtB -> B_1036_1104_0_k ) { _rtB -> B_1036_3850_0 =
_rtB -> B_1036_3848_0 ; } else { _rtB -> B_590_0_0 = ( _rtB -> B_1036_3848_0
&& _rtB -> B_1036_1115_0_a ) ; _rtB -> B_1036_3850_0 = _rtB -> B_590_0_0 ; }
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_3851_0 = _rtDW -> Memory_PreviousInput_i1 ; _rtB -> B_1036_3852_0 = !
_rtB -> B_1036_3851_0 ; } _rtB -> B_1036_3853_0 = ( _rtB -> B_1036_3850_0 &&
_rtB -> B_1036_3852_0 ) ; _rtB -> B_1036_3854_0 = _rtB -> B_1036_3778_0 +
_rtB -> B_1036_3213_0 ; _rtB -> B_1036_3855_0 = _rtB -> B_1036_3854_0 * _rtB
-> B_1036_3854_0 ; _rtB -> B_1036_3856_0 = _rtB -> B_1036_3784_0 + _rtB ->
B_1036_3214_0 ; _rtB -> B_1036_3857_0 = _rtB -> B_1036_3856_0 * _rtB ->
B_1036_3856_0 ; _rtB -> B_1036_3858_0 = _rtB -> B_1036_3855_0 + _rtB ->
B_1036_3857_0 ; _rtB -> B_1036_3859_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_3858_0 ) ; _rtB -> B_1036_3860_0 = _rtB -> B_1036_3213_0 * _rtB ->
B_1036_3213_0 ; _rtB -> B_1036_3861_0 = _rtB -> B_1036_3214_0 * _rtB ->
B_1036_3214_0 ; _rtB -> B_1036_3862_0 = _rtB -> B_1036_3860_0 + _rtB ->
B_1036_3861_0 ; _rtB -> B_1036_3863_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_3862_0 ) ; _rtB -> B_1036_3864_0 = _rtB -> B_1036_3863_0 * _rtB ->
B_1036_1033_0 ; _rtB -> B_1036_3865_0 = ( _rtB -> B_1036_3859_0 >= _rtB ->
B_1036_3864_0 ) ; _rtB -> B_1036_3866_0 = ( _rtB -> B_1036_1075_0_c && _rtB
-> B_1036_3865_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_1036_3867_0 = _rtDW -> Memory_PreviousInput_gx ; _rtB ->
B_1036_3868_0 = ! _rtB -> B_1036_3867_0 ; } _rtB -> B_1036_3869_0 = ( _rtB ->
B_1036_3866_0 && _rtB -> B_1036_3868_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_3870_0 = _rtDW ->
Memory_PreviousInput_iy ; _rtB -> B_1036_3871_0 = ! _rtB -> B_1036_3870_0 ; }
_rtB -> B_1036_3872_0 = ( _rtB -> B_1036_3820_0 && _rtB -> B_1036_3871_0 ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_3873_0 = _rtDW -> Memory_PreviousInput_iv ; _rtB -> B_1036_3874_0 = !
_rtB -> B_1036_3873_0 ; } _rtB -> B_1036_3875_0 = ( _rtB -> B_1036_3796_0 &&
_rtB -> B_1036_3874_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_1036_3876_0 = _rtDW -> Memory_PreviousInput_ea ; _rtB ->
B_1036_3877_0 = ! _rtB -> B_1036_3876_0 ; } _rtB -> B_1036_3878_0 = ( _rtB ->
B_1036_3774_0 && _rtB -> B_1036_3877_0 ) ; _rtB -> B_1036_3879_0 = ( _rtB ->
B_1036_3834_0 || _rtB -> B_1036_3853_0 || _rtB -> B_1036_3869_0 || _rtB ->
B_1036_3872_0 || _rtB -> B_1036_3875_0 || _rtB -> B_1036_3878_0 ) ; _rtB ->
B_1036_3880_0 = ( _rtB -> B_1036_3831_0 && _rtB -> B_1036_3850_0 ) ; _rtB ->
B_1036_3881_0 = _rtB -> B_1036_3837_0 * _rtB -> B_1036_3837_0 ; _rtB ->
B_1036_3882_0 = _rtB -> B_1036_3841_0 * _rtB -> B_1036_3841_0 ; _rtB ->
B_1036_3883_0 = _rtB -> B_1036_3881_0 + _rtB -> B_1036_3882_0 ; _rtB ->
B_1036_3884_0 = muDoubleScalarSqrt ( _rtB -> B_1036_3883_0 ) ; if ( _rtB ->
B_1036_1119_0 > _rtP -> P_697 ) { _rtB -> B_595_0_0 = ( _rtB -> B_1036_3884_0
< _rtB -> B_1036_1122_0_e ) ; _rtB -> B_1036_3887_0 = _rtB -> B_595_0_0 ; }
else { _rtB -> B_594_0_0 = ( _rtB -> B_1036_3884_0 > _rtB -> B_1036_1122_0_e
) ; _rtB -> B_1036_3887_0 = _rtB -> B_594_0_0 ; } _rtB -> B_1036_3888_0 = (
_rtB -> B_1036_3887_0 && _rtB -> B_1036_1124_0_k ) ; if ( _rtB ->
B_1036_1117_0_g ) { _rtB -> B_1036_3890_0 = _rtB -> B_1036_3888_0 ; } else {
_rtB -> B_593_0_0 = ( _rtB -> B_1036_3888_0 && _rtB -> B_1036_1128_0_a ) ;
_rtB -> B_1036_3890_0 = _rtB -> B_593_0_0 ; } isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3891_0 = _rtDW ->
Memory_PreviousInput_e5y ; _rtB -> B_1036_3892_0 = ! _rtB -> B_1036_3891_0 ;
} _rtB -> B_1036_3893_0 = ( _rtB -> B_1036_3890_0 && _rtB -> B_1036_3892_0 )
; _rtB -> B_1036_3894_0 = ( _rtB -> B_1036_3880_0 || _rtB -> B_1036_3893_0 )
; _rtB -> B_1036_3895_0 = ( _rtB -> B_1036_3831_0 || _rtB -> B_1036_3893_0 )
; _rtB -> B_1036_3896_0 = ( _rtB -> B_1036_1043_0_n && _rtB -> B_1036_3895_0
) ; _rtB -> B_1036_3897_0 = ( ( _rtB -> B_1036_1032_0 != 0.0 ) && _rtB ->
B_1036_3894_0 ) ; _rtB -> B_1036_3898_0 = _rtB -> B_1036_3897_0 ; _rtB ->
B_1036_3899_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_3900_0 = _rtB -> B_1036_1050_0_n ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_3900_0 , _rtB -> B_1036_3898_0 , _rtB ->
B_1036_3899_0 , _rtB -> B_1036_1048_0_j , & _rtB -> ONDelay_hd , & _rtDW ->
ONDelay_hd , & _rtZCE -> ONDelay_hd ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_3902_0 = _rtB -> B_1036_1051_0_e ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_3902_0 , _rtB -> B_1036_3898_0 , _rtB ->
B_1036_3899_0 , _rtB -> B_1036_1048_0_j , & _rtB -> OFFDelay_dcf , & _rtDW ->
OFFDelay_dcf , & _rtZCE -> OFFDelay_dcf ) ; _rtB -> B_1036_3904_0 = ( _rtB ->
ONDelay_hd . B_90_17_0 || _rtB -> OFFDelay_dcf . B_86_19_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3905_0 =
_rtDW -> Memory_PreviousInput_gf ; _rtB -> B_1036_3906_0 = ! _rtB ->
B_1036_3905_0 ; } _rtB -> B_1036_3907_0 = ( _rtB -> B_1036_3904_0 && _rtB ->
B_1036_3906_0 ) ; _rtB -> B_1036_3908_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3909_0 =
_rtDW -> Memory_PreviousInput_ou ; _rtB -> B_1036_3910_0 = ! _rtB ->
B_1036_3909_0 ; } _rtB -> B_1036_3911_0 = _rtB -> B_1036_3904_0 ; _rtB ->
B_1036_3912_0 = _rtB -> B_1036_3911_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_3913_0 = _rtDW -> Memory_PreviousInput_bew
; _rtB -> B_1036_3914_0 = _rtB -> B_1036_1160_0_n [ 0 ] ; }
DZG_v3_POSITIVEEdge ( S , _rtB -> B_1036_3914_0 , _rtB -> B_1036_3912_0 ,
_rtB -> B_1036_3913_0 , & _rtB -> POSITIVEEdge_d , & _rtDW -> POSITIVEEdge_d
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_3916_0 = _rtB -> B_1036_1160_0_n [ 1 ] ; } DZG_v3_NEGATIVEEdge ( S ,
_rtB -> B_1036_3916_0 , _rtB -> B_1036_3912_0 , _rtB -> B_1036_3913_0 , &
_rtB -> NEGATIVEEdge_ht , & _rtDW -> NEGATIVEEdge_ht ) ; _rtB ->
B_1036_3918_0 = ( _rtB -> POSITIVEEdge_d . B_85_0_0 || _rtB ->
NEGATIVEEdge_ht . B_84_0_0 ) ; _rtB -> B_1036_3919_0 = ( _rtB ->
B_1036_3910_0 && _rtB -> B_1036_3918_0 ) ; _rtB -> B_1036_3920_0 = _rtB ->
B_1036_3919_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_3921_0 = _rtDW -> ICic_PreviousInput_gl ; } if ( _rtB ->
B_1036_3920_0 ) { _rtB -> B_1036_3922_0 = _rtB -> B_1036_3908_0 ; } else {
_rtB -> B_1036_3922_0 = _rtB -> B_1036_3921_0 ; } isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3923_0 = _rtP -> P_699 ; } _rtB
-> B_1036_3924_0 = _rtB -> B_1036_3922_0 + _rtB -> B_1036_3923_0 ; _rtB ->
B_1036_3925_0 = ( _rtB -> B_1036_3924_0 > _rtB -> B_1036_3908_0 ) ; _rtB ->
B_1036_3926_0 = ( _rtB -> B_1036_3907_0 || _rtB -> B_1036_3925_0 ) ; _rtB ->
B_1036_3927_0 = _rtB -> B_1036_3926_0 ; _rtB -> B_1036_3928_0 = ( _rtB ->
B_1036_3821_0 || _rtB -> B_1036_3797_0 ) ; if ( _rtB -> B_1036_3760_0 ) {
_rtB -> B_1036_3929_0 = _rtB -> B_1036_3755_0 ; } else { _rtB ->
B_1036_3929_0 = _rtB -> B_1036_3759_0 ; } _rtB -> B_1036_3930_0 = ssGetT ( S
) ; _rtB -> B_1036_3931_0 = _rtB -> B_1036_3896_0 ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3932_0 = _rtB ->
B_1036_1058_0_g ; _rtB -> B_1036_3933_0 = _rtB -> B_1036_1057_0 ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_3933_0 , _rtB -> B_1036_3931_0 , _rtB ->
B_1036_3930_0 , _rtB -> B_1036_1055_0 , & _rtB -> ONDelay_ll , & _rtDW ->
ONDelay_ll , & _rtZCE -> ONDelay_ll ) ; DZG_v3_OFFDelay ( S , _rtB ->
B_1036_3932_0 , _rtB -> B_1036_3931_0 , _rtB -> B_1036_3930_0 , _rtB ->
B_1036_1055_0 , & _rtB -> OFFDelay_j , & _rtDW -> OFFDelay_j , & _rtZCE ->
OFFDelay_j ) ; _rtB -> B_1036_3936_0 = ( _rtB -> ONDelay_ll . B_90_17_0 ||
_rtB -> OFFDelay_j . B_86_19_0 ) ; _rtB -> B_1036_3937_0 = ssGetT ( S ) ;
_rtB -> B_1036_3938_0 = _rtB -> B_1036_3866_0 ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3939_0 = _rtB -> B_1036_1065_0_j ;
_rtB -> B_1036_3940_0 = _rtB -> B_1036_1064_0 ; } DZG_v3_ONDelay ( S , _rtB
-> B_1036_3940_0 , _rtB -> B_1036_3938_0 , _rtB -> B_1036_3937_0 , _rtB ->
B_1036_1062_0_c , & _rtB -> ONDelay_bv , & _rtDW -> ONDelay_bv , & _rtZCE ->
ONDelay_bv ) ; DZG_v3_OFFDelay ( S , _rtB -> B_1036_3939_0 , _rtB ->
B_1036_3938_0 , _rtB -> B_1036_3937_0 , _rtB -> B_1036_1062_0_c , & _rtB ->
OFFDelay_ja , & _rtDW -> OFFDelay_ja , & _rtZCE -> OFFDelay_ja ) ; _rtB ->
B_1036_3943_0 = ( _rtB -> ONDelay_bv . B_90_17_0 || _rtB -> OFFDelay_ja .
B_86_19_0 ) ; _rtB -> B_1036_3944_0 = ssGetT ( S ) ; _rtB -> B_1036_3945_0 =
_rtB -> B_1036_3928_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_1036_3946_0 = _rtB -> B_1036_1072_0_i ; _rtB -> B_1036_3947_0 =
_rtB -> B_1036_1071_0_a ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_3947_0 , _rtB
-> B_1036_3945_0 , _rtB -> B_1036_3944_0 , _rtB -> B_1036_1069_0_d , & _rtB
-> ONDelay_ht , & _rtDW -> ONDelay_ht , & _rtZCE -> ONDelay_ht ) ;
DZG_v3_OFFDelay ( S , _rtB -> B_1036_3946_0 , _rtB -> B_1036_3945_0 , _rtB ->
B_1036_3944_0 , _rtB -> B_1036_1069_0_d , & _rtB -> OFFDelay_aq , & _rtDW ->
OFFDelay_aq , & _rtZCE -> OFFDelay_aq ) ; _rtB -> B_1036_3950_0 = ( _rtB ->
ONDelay_ht . B_90_17_0 || _rtB -> OFFDelay_aq . B_86_19_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3951_0 =
_rtDW -> Memory_PreviousInput_jf ; _rtB -> B_1036_3952_0 = ! _rtB ->
B_1036_3951_0 ; } _rtB -> B_1036_3953_0 = ( _rtB -> B_1036_3943_0 && _rtB ->
B_1036_3952_0 ) ; _rtB -> B_1036_3954_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3955_0 =
_rtDW -> Memory_PreviousInput_hw ; _rtB -> B_1036_3956_0 = ! _rtB ->
B_1036_3955_0 ; } _rtB -> B_1036_3957_0 = _rtB -> B_1036_3943_0 ; _rtB ->
B_1036_3958_0 = _rtB -> B_1036_3957_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_3959_0 = _rtDW -> Memory_PreviousInput_n3p
; _rtB -> B_1036_3960_0 = _rtB -> B_1036_1148_0_b [ 0 ] ; }
DZG_v3_POSITIVEEdge ( S , _rtB -> B_1036_3960_0 , _rtB -> B_1036_3958_0 ,
_rtB -> B_1036_3959_0 , & _rtB -> POSITIVEEdge_i , & _rtDW -> POSITIVEEdge_i
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_3962_0 = _rtB -> B_1036_1148_0_b [ 1 ] ; } DZG_v3_NEGATIVEEdge ( S ,
_rtB -> B_1036_3962_0 , _rtB -> B_1036_3958_0 , _rtB -> B_1036_3959_0 , &
_rtB -> NEGATIVEEdge_n , & _rtDW -> NEGATIVEEdge_n ) ; _rtB -> B_1036_3964_0
= ( _rtB -> POSITIVEEdge_i . B_85_0_0 || _rtB -> NEGATIVEEdge_n . B_84_0_0 )
; _rtB -> B_1036_3965_0 = ( _rtB -> B_1036_3956_0 && _rtB -> B_1036_3964_0 )
; _rtB -> B_1036_3966_0 = _rtB -> B_1036_3965_0 ; isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3967_0 = _rtDW ->
ICic_PreviousInput_h ; } if ( _rtB -> B_1036_3966_0 ) { _rtB -> B_1036_3968_0
= _rtB -> B_1036_3954_0 ; } else { _rtB -> B_1036_3968_0 = _rtB ->
B_1036_3967_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_3969_0 = _rtP -> P_701 ; } _rtB -> B_1036_3970_0 = _rtB ->
B_1036_3968_0 + _rtB -> B_1036_3969_0 ; _rtB -> B_1036_3971_0 = ( _rtB ->
B_1036_3970_0 > _rtB -> B_1036_3954_0 ) ; _rtB -> B_1036_3972_0 = ( _rtB ->
B_1036_3953_0 || _rtB -> B_1036_3971_0 ) ; _rtB -> B_1036_3973_0 = ssGetT ( S
) ; _rtB -> B_1036_3974_0 = _rtB -> B_1036_3964_0 ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { DZG_v3_TriggeredSubsystem ( S , _rtB ->
B_1036_3974_0 , _rtB -> B_1036_3973_0 , & _rtB -> TriggeredSubsystem_a , &
_rtDW -> TriggeredSubsystem_a , & _rtZCE -> TriggeredSubsystem_a ) ; _rtB ->
B_1036_3976_0 = _rtB -> TriggeredSubsystem_a . B_83_0_0 + _rtB ->
B_1036_1143_0_p ; } _rtB -> B_1036_3977_0 = ( _rtB -> B_1036_3976_0 > _rtB ->
B_1036_3973_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_3978_0 = _rtDW -> Memory_PreviousInput_ir ; _rtB ->
B_1036_3979_0 = ! _rtB -> B_1036_3978_0 ; } _rtB -> B_1036_3980_0 = ( _rtB ->
B_1036_3950_0 && _rtB -> B_1036_3979_0 ) ; _rtB -> B_1036_3981_0 = ssGetT ( S
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_3982_0 = _rtDW -> Memory_PreviousInput_cf ; _rtB -> B_1036_3983_0 = !
_rtB -> B_1036_3982_0 ; } _rtB -> B_1036_3984_0 = _rtB -> B_1036_3950_0 ;
_rtB -> B_1036_3985_0 = _rtB -> B_1036_3984_0 ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3986_0 = _rtDW ->
Memory_PreviousInput_ijg ; _rtB -> B_1036_3987_0 = _rtB -> B_1036_1154_0_h [
0 ] ; } DZG_v3_POSITIVEEdge ( S , _rtB -> B_1036_3987_0 , _rtB ->
B_1036_3985_0 , _rtB -> B_1036_3986_0 , & _rtB -> POSITIVEEdge_a , & _rtDW ->
POSITIVEEdge_a ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_3989_0 = _rtB -> B_1036_1154_0_h [ 1 ] ; } DZG_v3_NEGATIVEEdge
( S , _rtB -> B_1036_3989_0 , _rtB -> B_1036_3985_0 , _rtB -> B_1036_3986_0 ,
& _rtB -> NEGATIVEEdge_a , & _rtDW -> NEGATIVEEdge_a ) ; _rtB ->
B_1036_3991_0 = ( _rtB -> POSITIVEEdge_a . B_85_0_0 || _rtB -> NEGATIVEEdge_a
. B_84_0_0 ) ; _rtB -> B_1036_3992_0 = ( _rtB -> B_1036_3983_0 && _rtB ->
B_1036_3991_0 ) ; _rtB -> B_1036_3993_0 = _rtB -> B_1036_3992_0 ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_3994_0 =
_rtDW -> ICic_PreviousInput_e ; } if ( _rtB -> B_1036_3993_0 ) { _rtB ->
B_1036_3995_0 = _rtB -> B_1036_3981_0 ; } else { _rtB -> B_1036_3995_0 = _rtB
-> B_1036_3994_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_1036_3996_0 = _rtP -> P_703 ; } _rtB -> B_1036_3997_0 = _rtB ->
B_1036_3995_0 + _rtB -> B_1036_3996_0 ; _rtB -> B_1036_3998_0 = ( _rtB ->
B_1036_3997_0 > _rtB -> B_1036_3981_0 ) ; _rtB -> B_1036_3999_0 = ( _rtB ->
B_1036_3980_0 || _rtB -> B_1036_3998_0 ) ; _rtB -> B_1036_4000_0 = ssGetT ( S
) ; _rtB -> B_1036_4001_0 = _rtB -> B_1036_3991_0 ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { DZG_v3_TriggeredSubsystem ( S , _rtB ->
B_1036_4001_0 , _rtB -> B_1036_4000_0 , & _rtB -> TriggeredSubsystem_i , &
_rtDW -> TriggeredSubsystem_i , & _rtZCE -> TriggeredSubsystem_i ) ; _rtB ->
B_1036_4003_0 = _rtB -> TriggeredSubsystem_i . B_83_0_0 + _rtB ->
B_1036_1149_0_k ; } _rtB -> B_1036_4004_0 = ( _rtB -> B_1036_4003_0 > _rtB ->
B_1036_4000_0 ) ; _rtB -> B_1036_4005_0 = ssGetT ( S ) ; _rtB ->
B_1036_4006_0 = _rtB -> B_1036_3918_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { DZG_v3_TriggeredSubsystem ( S , _rtB -> B_1036_4006_0 ,
_rtB -> B_1036_4005_0 , & _rtB -> TriggeredSubsystem_jo , & _rtDW ->
TriggeredSubsystem_jo , & _rtZCE -> TriggeredSubsystem_jo ) ; _rtB ->
B_1036_4008_0 = _rtB -> TriggeredSubsystem_jo . B_83_0_0 + _rtB ->
B_1036_1155_0_n ; } _rtB -> B_1036_4009_0 = ( _rtB -> B_1036_4008_0 > _rtB ->
B_1036_4005_0 ) ; _rtB -> B_1036_4010_0 = _rtB -> B_1036_3162_0 * _rtB ->
B_1036_1161_0_g ; _rtB -> B_1036_4011_0 = _rtB -> B_1036_3161_0 * _rtB ->
B_1036_1164_0_m ; _rtB -> B_1036_4012_0 = _rtB -> B_1036_4010_0 + _rtB ->
B_1036_4011_0 ; _rtB -> B_1036_4013_0 = _rtB -> B_1036_3164_0 * _rtB ->
B_1036_1161_0_g ; _rtB -> B_1036_4014_0 = _rtB -> B_1036_3163_0 * _rtB ->
B_1036_1163_0_c ; _rtB -> B_1036_4015_0 = _rtB -> B_1036_4013_0 + _rtB ->
B_1036_4014_0 ; _rtB -> B_1036_4016_0 = _rtB -> B_1036_4012_0 + _rtB ->
B_1036_4015_0 ; _rtB -> B_1036_4017_0 = _rtB -> B_1036_3160_0 + _rtB ->
B_1036_4016_0 ; _rtB -> B_1036_4018_0 = _rtB -> B_1036_3162_0 * _rtB ->
B_1036_1161_0_g ; _rtB -> B_1036_4019_0 = _rtB -> B_1036_3161_0 * _rtB ->
B_1036_1163_0_c ; _rtB -> B_1036_4020_0 = _rtB -> B_1036_4018_0 + _rtB ->
B_1036_4019_0 ; _rtB -> B_1036_4021_0 = _rtB -> B_1036_3164_0 * _rtB ->
B_1036_1161_0_g ; _rtB -> B_1036_4022_0 = _rtB -> B_1036_3163_0 * _rtB ->
B_1036_1164_0_m ; _rtB -> B_1036_4023_0 = _rtB -> B_1036_4021_0 + _rtB ->
B_1036_4022_0 ; _rtB -> B_1036_4024_0 = _rtB -> B_1036_4020_0 + _rtB ->
B_1036_4023_0 ; _rtB -> B_1036_4025_0 = _rtB -> B_1036_3160_0 + _rtB ->
B_1036_4024_0 ; _rtB -> B_1036_4026_0 = _rtB -> B_1036_3161_0 * _rtB ->
B_1036_1161_0_g ; _rtB -> B_1036_4027_0 = _rtB -> B_1036_3162_0 * _rtB ->
B_1036_1163_0_c ; _rtB -> B_1036_4028_0 = _rtB -> B_1036_4026_0 - _rtB ->
B_1036_4027_0 ; _rtB -> B_1036_4029_0 = _rtB -> B_1036_3163_0 * _rtB ->
B_1036_1161_0_g ; _rtB -> B_1036_4030_0 = _rtB -> B_1036_3164_0 * _rtB ->
B_1036_1164_0_m ; _rtB -> B_1036_4031_0 = _rtB -> B_1036_4029_0 - _rtB ->
B_1036_4030_0 ; _rtB -> B_1036_4032_0 = _rtB -> B_1036_4028_0 + _rtB ->
B_1036_4031_0 ; _rtB -> B_1036_4033_0 = _rtB -> B_1036_3159_0 + _rtB ->
B_1036_4032_0 ; _rtB -> B_1036_4034_0 = _rtB -> B_1036_4033_0 * _rtB ->
B_1036_1162_0_d ; _rtB -> B_1036_4035_0 = _rtB -> B_1036_3161_0 * _rtB ->
B_1036_1161_0_g ; _rtB -> B_1036_4036_0 = _rtB -> B_1036_3162_0 * _rtB ->
B_1036_1164_0_m ; _rtB -> B_1036_4037_0 = _rtB -> B_1036_4035_0 - _rtB ->
B_1036_4036_0 ; _rtB -> B_1036_4038_0 = _rtB -> B_1036_3163_0 * _rtB ->
B_1036_1161_0_g ; _rtB -> B_1036_4039_0 = _rtB -> B_1036_3164_0 * _rtB ->
B_1036_1163_0_c ; _rtB -> B_1036_4040_0 = _rtB -> B_1036_4038_0 - _rtB ->
B_1036_4039_0 ; _rtB -> B_1036_4041_0 = _rtB -> B_1036_4037_0 + _rtB ->
B_1036_4040_0 ; _rtB -> B_1036_4042_0 = _rtB -> B_1036_3159_0 + _rtB ->
B_1036_4041_0 ; _rtB -> B_1036_4043_0 = _rtB -> B_1036_4042_0 * _rtB ->
B_1036_1162_0_d ; _rtB -> B_1036_4044_0 = _rtB -> B_1036_4017_0 * _rtB ->
B_1036_1162_0_d ; _rtB -> B_1036_4045_0 = _rtB -> B_1036_4025_0 * _rtB ->
B_1036_1162_0_d ; _rtB -> B_1036_4046_0 = _rtB -> B_1036_1629_0 * _rtB ->
B_1036_1165_0 ; _rtB -> B_1036_4047_0 = _rtB -> B_1036_1625_0 * _rtB ->
B_1036_1168_0_n ; _rtB -> B_1036_4048_0 = _rtB -> B_1036_4046_0 + _rtB ->
B_1036_4047_0 ; _rtB -> B_1036_4049_0 = _rtB -> B_1036_1664_0 * _rtB ->
B_1036_1165_0 ; _rtB -> B_1036_4050_0 = _rtB -> B_1036_1660_0 * _rtB ->
B_1036_1167_0_d ; _rtB -> B_1036_4051_0 = _rtB -> B_1036_4049_0 + _rtB ->
B_1036_4050_0 ; _rtB -> B_1036_4052_0 = _rtB -> B_1036_4048_0 + _rtB ->
B_1036_4051_0 ; _rtB -> B_1036_4053_0 = _rtB -> B_1036_1618_0 + _rtB ->
B_1036_4052_0 ; _rtB -> B_1036_4054_0 = _rtB -> B_1036_1629_0 * _rtB ->
B_1036_1165_0 ; _rtB -> B_1036_4055_0 = _rtB -> B_1036_1625_0 * _rtB ->
B_1036_1167_0_d ; _rtB -> B_1036_4056_0 = _rtB -> B_1036_4054_0 + _rtB ->
B_1036_4055_0 ; _rtB -> B_1036_4057_0 = _rtB -> B_1036_1664_0 * _rtB ->
B_1036_1165_0 ; _rtB -> B_1036_4058_0 = _rtB -> B_1036_1660_0 * _rtB ->
B_1036_1168_0_n ; _rtB -> B_1036_4059_0 = _rtB -> B_1036_4057_0 + _rtB ->
B_1036_4058_0 ; _rtB -> B_1036_4060_0 = _rtB -> B_1036_4056_0 + _rtB ->
B_1036_4059_0 ; _rtB -> B_1036_4061_0 = _rtB -> B_1036_1618_0 + _rtB ->
B_1036_4060_0 ; _rtB -> B_1036_4062_0 = _rtB -> B_1036_1625_0 * _rtB ->
B_1036_1165_0 ; _rtB -> B_1036_4063_0 = _rtB -> B_1036_1629_0 * _rtB ->
B_1036_1167_0_d ; _rtB -> B_1036_4064_0 = _rtB -> B_1036_4062_0 - _rtB ->
B_1036_4063_0 ; _rtB -> B_1036_4065_0 = _rtB -> B_1036_1660_0 * _rtB ->
B_1036_1165_0 ; _rtB -> B_1036_4066_0 = _rtB -> B_1036_1664_0 * _rtB ->
B_1036_1168_0_n ; _rtB -> B_1036_4067_0 = _rtB -> B_1036_4065_0 - _rtB ->
B_1036_4066_0 ; _rtB -> B_1036_4068_0 = _rtB -> B_1036_4064_0 + _rtB ->
B_1036_4067_0 ; _rtB -> B_1036_4069_0 = _rtB -> B_1036_1614_0 + _rtB ->
B_1036_4068_0 ; _rtB -> B_1036_4070_0 = _rtB -> B_1036_4069_0 * _rtB ->
B_1036_1166_0 ; _rtB -> B_1036_4071_0 = _rtB -> B_1036_1625_0 * _rtB ->
B_1036_1165_0 ; _rtB -> B_1036_4072_0 = _rtB -> B_1036_1629_0 * _rtB ->
B_1036_1168_0_n ; _rtB -> B_1036_4073_0 = _rtB -> B_1036_4071_0 - _rtB ->
B_1036_4072_0 ; _rtB -> B_1036_4074_0 = _rtB -> B_1036_1660_0 * _rtB ->
B_1036_1165_0 ; _rtB -> B_1036_4075_0 = _rtB -> B_1036_1664_0 * _rtB ->
B_1036_1167_0_d ; _rtB -> B_1036_4076_0 = _rtB -> B_1036_4074_0 - _rtB ->
B_1036_4075_0 ; _rtB -> B_1036_4077_0 = _rtB -> B_1036_4073_0 + _rtB ->
B_1036_4076_0 ; _rtB -> B_1036_4078_0 = _rtB -> B_1036_1614_0 + _rtB ->
B_1036_4077_0 ; _rtB -> B_1036_4079_0 = _rtB -> B_1036_4078_0 * _rtB ->
B_1036_1166_0 ; _rtB -> B_1036_4080_0 = _rtB -> B_1036_4053_0 * _rtB ->
B_1036_1166_0 ; _rtB -> B_1036_4081_0 = _rtB -> B_1036_4061_0 * _rtB ->
B_1036_1166_0 ; _rtB -> B_1036_4082_0 = _rtB -> B_1036_794_0 * _rtB ->
B_1036_794_0 ; _rtB -> B_1036_4083_0 = _rtB -> B_1036_795_0 * _rtB ->
B_1036_795_0 ; _rtB -> B_1036_4084_0 = _rtB -> B_1036_4082_0 + _rtB ->
B_1036_4083_0 ; _rtB -> B_1036_4085_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_4084_0 ) ; _rtB -> B_1036_4086_0 = _rtB -> B_1036_1136_0 * _rtB ->
B_1036_1136_0 ; _rtB -> B_1036_4087_0 = _rtB -> B_1036_1137_0 * _rtB ->
B_1036_1137_0 ; _rtB -> B_1036_4088_0 = _rtB -> B_1036_4086_0 + _rtB ->
B_1036_4087_0 ; _rtB -> B_1036_4089_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_4088_0 ) ; _rtB -> B_1036_4090_0 = _rtB -> B_1036_1372_0 * _rtB ->
B_1036_1372_0 ; _rtB -> B_1036_4091_0 = _rtB -> B_1036_1373_0 * _rtB ->
B_1036_1373_0 ; _rtB -> B_1036_4092_0 = _rtB -> B_1036_4090_0 + _rtB ->
B_1036_4091_0 ; _rtB -> B_1036_4093_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_4092_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
ssCallAccelRunBlock ( S , 1036 , 4094 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 1036 , 4095 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 1036 , 4096 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_1036_4097_0 = _rtDW -> Memory_PreviousInput_f3 ; _rtB -> B_1036_4098_0 = !
_rtB -> B_1036_4097_0 ; } _rtB -> B_1036_4099_0 = ( _rtB -> B_1036_1092_0 &&
_rtB -> B_1036_4098_0 ) ; _rtB -> B_1036_4100_0 = ( _rtB -> B_1036_4099_0 ||
_rtB -> B_1036_1591_0_o ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_1036_4101_0 = _rtDW -> Memory_PreviousInput_lzl ; _rtB ->
B_1036_4102_0 = ! _rtB -> B_1036_4101_0 ; } _rtB -> B_1036_4103_0 = ( _rtB ->
B_1036_4100_0 && _rtB -> B_1036_4102_0 ) ; _rtB -> B_1036_4104_0 =
muDoubleScalarSin ( _rtB -> B_1036_825_0 ) ; _rtB -> B_1036_4105_0 =
muDoubleScalarSin ( _rtB -> B_1036_1167_0 ) ; _rtB -> B_1036_4106_0 =
muDoubleScalarSin ( _rtB -> B_1036_1403_0 ) ; _rtB -> B_1036_4107_0 = _rtB ->
B_1036_435_0 * _rtB -> B_1036_435_0 ; _rtB -> B_1036_4108_0 = _rtB ->
B_1036_765_0 * _rtB -> B_1036_765_0 ; _rtB -> B_1036_4109_0 = _rtB ->
B_1036_4107_0 + _rtB -> B_1036_4108_0 ; _rtB -> B_1036_4110_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_4109_0 ) ; _rtB -> B_1036_4111_0 = _rtB
-> B_1036_953_0 * _rtB -> B_1036_953_0 ; _rtB -> B_1036_4112_0 = _rtB ->
B_1036_990_0 * _rtB -> B_1036_990_0 ; _rtB -> B_1036_4113_0 = _rtB ->
B_1036_4111_0 + _rtB -> B_1036_4112_0 ; _rtB -> B_1036_4114_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_4113_0 ) ; _rtB -> B_1036_4115_0 = _rtB
-> B_1036_1026_0 * _rtB -> B_1036_1026_0 ; _rtB -> B_1036_4116_0 = _rtB ->
B_1036_1063_0 * _rtB -> B_1036_1063_0 ; _rtB -> B_1036_4117_0 = _rtB ->
B_1036_4115_0 + _rtB -> B_1036_4116_0 ; _rtB -> B_1036_4118_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_4117_0 ) ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S , 1036 , 4119 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_4120_0 = _rtB -> B_1036_435_0 * _rtB
-> B_1036_435_0 ; _rtB -> B_1036_4121_0 = _rtB -> B_1036_765_0 * _rtB ->
B_1036_765_0 ; _rtB -> B_1036_4122_0 = _rtB -> B_1036_4120_0 + _rtB ->
B_1036_4121_0 ; _rtB -> B_1036_4123_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_4122_0 ) ; _rtB -> B_1036_4124_0 = _rtB -> B_1036_435_0 / _rtB ->
B_1036_4123_0 ; u0 = _rtB -> B_1036_4124_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB -> B_1036_4125_0 =
muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_4126_0 = ( _rtB -> B_1036_765_0 >
_rtB -> B_1036_1578_0_i ) ; if ( _rtB -> B_1036_4126_0 ) { _rtB ->
B_1036_4128_0 = _rtB -> B_1036_4125_0 ; } else { _rtB -> B_676_0_0 = _rtB ->
B_1036_4125_0 * _rtB -> B_1036_1577_0 ; _rtB -> B_1036_4128_0 = _rtB ->
B_676_0_0 ; } _rtB -> B_1036_4129_0 = _rtB -> B_1036_953_0 * _rtB ->
B_1036_953_0 ; _rtB -> B_1036_4130_0 = _rtB -> B_1036_990_0 * _rtB ->
B_1036_990_0 ; _rtB -> B_1036_4131_0 = _rtB -> B_1036_4129_0 + _rtB ->
B_1036_4130_0 ; _rtB -> B_1036_4132_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_4131_0 ) ; _rtB -> B_1036_4133_0 = _rtB -> B_1036_953_0 / _rtB ->
B_1036_4132_0 ; u0 = _rtB -> B_1036_4133_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB -> B_1036_4134_0 =
muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_4135_0 = ( _rtB -> B_1036_990_0 >
_rtB -> B_1036_1637_0_k ) ; if ( _rtB -> B_1036_4135_0 ) { _rtB ->
B_1036_4137_0 = _rtB -> B_1036_4134_0 ; } else { _rtB -> B_681_0_0 = _rtB ->
B_1036_4134_0 * _rtB -> B_1036_1636_0_l ; _rtB -> B_1036_4137_0 = _rtB ->
B_681_0_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
ssCallAccelRunBlock ( S , 1036 , 4138 , SS_CALL_MDL_OUTPUTS ) ; } _rtB ->
B_1036_4139_0 = _rtB -> B_1036_1026_0 * _rtB -> B_1036_1026_0 ; _rtB ->
B_1036_4140_0 = _rtB -> B_1036_1063_0 * _rtB -> B_1036_1063_0 ; _rtB ->
B_1036_4141_0 = _rtB -> B_1036_4139_0 + _rtB -> B_1036_4140_0 ; _rtB ->
B_1036_4142_0 = muDoubleScalarSqrt ( _rtB -> B_1036_4141_0 ) ; _rtB ->
B_1036_4143_0 = _rtB -> B_1036_1026_0 / _rtB -> B_1036_4142_0 ; u0 = _rtB ->
B_1036_4143_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0
= - 1.0 ; } } _rtB -> B_1036_4144_0 = muDoubleScalarAcos ( u0 ) ; _rtB ->
B_1036_4145_0 = ( _rtB -> B_1036_1063_0 > _rtB -> B_1036_1686_0_f ) ; if (
_rtB -> B_1036_4145_0 ) { _rtB -> B_1036_4147_0 = _rtB -> B_1036_4144_0 ; }
else { _rtB -> B_684_0_0 = _rtB -> B_1036_4144_0 * _rtB -> B_1036_1685_0 ;
_rtB -> B_1036_4147_0 = _rtB -> B_684_0_0 ; } _rtB -> B_1036_4148_0 = ( _rtB
-> B_1036_1838_0_b && _rtB -> B_1036_932_0 && _rtB -> B_1036_1005_0 && _rtB
-> B_1036_1078_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_1036_4149_0 = _rtDW -> Memory_PreviousInput_l4 ; _rtB ->
B_1036_4150_0 = ! _rtB -> B_1036_4149_0 ; } _rtB -> B_1036_4151_0 = ( _rtB ->
B_1036_388_0_e && _rtB -> B_1036_4150_0 ) ; _rtB -> B_1036_4152_0 = ( _rtB ->
B_1036_932_0 && _rtB -> B_1036_4151_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_4153_0 = _rtDW ->
Memory_PreviousInput_oa ; _rtB -> B_1036_4154_0 = ! _rtB -> B_1036_4153_0 ; }
_rtB -> B_1036_4155_0 = ( _rtB -> B_1036_1249_0_e && _rtB -> B_1036_4154_0 )
; _rtB -> B_1036_4156_0 = ( _rtB -> B_1036_1005_0 && _rtB -> B_1036_4155_0 )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_4157_0 = _rtDW -> Memory_PreviousInput_lz3 ; _rtB -> B_1036_4158_0 = !
_rtB -> B_1036_4157_0 ; } _rtB -> B_1036_4159_0 = ( _rtB -> B_1036_1485_0_e
&& _rtB -> B_1036_4158_0 ) ; _rtB -> B_1036_4160_0 = ( _rtB -> B_1036_1078_0
&& _rtB -> B_1036_4159_0 ) ; _rtB -> B_1036_4161_0 = ( _rtB -> B_1036_4152_0
|| _rtB -> B_1036_4156_0 || _rtB -> B_1036_4160_0 ) ; if ( _rtB ->
B_1036_4161_0 ) { _rtB -> B_1036_4162_0 = _rtB -> B_1036_1731_0_h ; } else {
_rtB -> B_1036_4162_0 = _rtB -> B_1036_1567_0 ; } _rtB -> B_1036_4163_0 = (
_rtB -> B_1036_4151_0 || _rtB -> B_1036_4155_0 || _rtB -> B_1036_4159_0 ) ;
if ( _rtB -> B_1036_4163_0 ) { _rtB -> B_1036_4164_0 = _rtB ->
B_1036_1729_0_i ; } else { _rtB -> B_1036_4164_0 = _rtB -> B_1036_1567_0 ; }
_rtB -> B_1036_4165_0 = _rtB -> B_1036_422_0 * _rtB -> B_1036_1732_0_p ; _rtB
-> B_1036_4166_0 = _rtB -> B_1036_402_0 * _rtB -> B_1036_1735_0_g ; _rtB ->
B_1036_4167_0 = _rtB -> B_1036_4165_0 + _rtB -> B_1036_4166_0 ; _rtB ->
B_1036_4168_0 = _rtB -> B_1036_428_0 * _rtB -> B_1036_1732_0_p ; _rtB ->
B_1036_4169_0 = _rtB -> B_1036_408_0 * _rtB -> B_1036_1734_0_c ; _rtB ->
B_1036_4170_0 = _rtB -> B_1036_4168_0 + _rtB -> B_1036_4169_0 ; _rtB ->
B_1036_4171_0 = _rtB -> B_1036_4167_0 + _rtB -> B_1036_4170_0 ; _rtB ->
B_1036_4172_0 = _rtB -> B_1036_415_0 + _rtB -> B_1036_4171_0 ; _rtB ->
B_1036_4173_0 = _rtB -> B_1036_422_0 * _rtB -> B_1036_1732_0_p ; _rtB ->
B_1036_4174_0 = _rtB -> B_1036_402_0 * _rtB -> B_1036_1734_0_c ; _rtB ->
B_1036_4175_0 = _rtB -> B_1036_4173_0 + _rtB -> B_1036_4174_0 ; _rtB ->
B_1036_4176_0 = _rtB -> B_1036_428_0 * _rtB -> B_1036_1732_0_p ; _rtB ->
B_1036_4177_0 = _rtB -> B_1036_408_0 * _rtB -> B_1036_1735_0_g ; _rtB ->
B_1036_4178_0 = _rtB -> B_1036_4176_0 + _rtB -> B_1036_4177_0 ; _rtB ->
B_1036_4179_0 = _rtB -> B_1036_4175_0 + _rtB -> B_1036_4178_0 ; _rtB ->
B_1036_4180_0 = _rtB -> B_1036_415_0 + _rtB -> B_1036_4179_0 ; _rtB ->
B_1036_4181_0 = _rtB -> B_1036_402_0 * _rtB -> B_1036_1732_0_p ; _rtB ->
B_1036_4182_0 = _rtB -> B_1036_422_0 * _rtB -> B_1036_1734_0_c ; _rtB ->
B_1036_4183_0 = _rtB -> B_1036_4181_0 - _rtB -> B_1036_4182_0 ; _rtB ->
B_1036_4184_0 = _rtB -> B_1036_408_0 * _rtB -> B_1036_1732_0_p ; _rtB ->
B_1036_4185_0 = _rtB -> B_1036_428_0 * _rtB -> B_1036_1735_0_g ; _rtB ->
B_1036_4186_0 = _rtB -> B_1036_4184_0 - _rtB -> B_1036_4185_0 ; _rtB ->
B_1036_4187_0 = _rtB -> B_1036_4183_0 + _rtB -> B_1036_4186_0 ; _rtB ->
B_1036_4188_0 = _rtB -> B_1036_395_0 + _rtB -> B_1036_4187_0 ; _rtB ->
B_1036_4189_0 = _rtB -> B_1036_4188_0 * _rtB -> B_1036_1733_0_g ; _rtB ->
B_1036_4190_0 = _rtB -> B_1036_402_0 * _rtB -> B_1036_1732_0_p ; _rtB ->
B_1036_4191_0 = _rtB -> B_1036_422_0 * _rtB -> B_1036_1735_0_g ; _rtB ->
B_1036_4192_0 = _rtB -> B_1036_4190_0 - _rtB -> B_1036_4191_0 ; _rtB ->
B_1036_4193_0 = _rtB -> B_1036_408_0 * _rtB -> B_1036_1732_0_p ; _rtB ->
B_1036_4194_0 = _rtB -> B_1036_428_0 * _rtB -> B_1036_1734_0_c ; _rtB ->
B_1036_4195_0 = _rtB -> B_1036_4193_0 - _rtB -> B_1036_4194_0 ; _rtB ->
B_1036_4196_0 = _rtB -> B_1036_4192_0 + _rtB -> B_1036_4195_0 ; _rtB ->
B_1036_4197_0 = _rtB -> B_1036_395_0 + _rtB -> B_1036_4196_0 ; _rtB ->
B_1036_4198_0 = _rtB -> B_1036_4197_0 * _rtB -> B_1036_1733_0_g ; _rtB ->
B_1036_4199_0 = _rtB -> B_1036_4172_0 * _rtB -> B_1036_1733_0_g ; _rtB ->
B_1036_4200_0 = _rtB -> B_1036_4180_0 * _rtB -> B_1036_1733_0_g ; _rtB ->
B_1036_4201_0 = _rtB -> B_1036_745_0 * _rtB -> B_1036_745_0 ; _rtB ->
B_1036_4202_0 = _rtB -> B_1036_771_0 * _rtB -> B_1036_771_0 ; _rtB ->
B_1036_4203_0 = _rtB -> B_1036_4201_0 + _rtB -> B_1036_4202_0 ; _rtB ->
B_1036_4204_0 = muDoubleScalarSqrt ( _rtB -> B_1036_4203_0 ) ; _rtB ->
B_1036_4205_0 = _rtB -> B_1036_974_0 * _rtB -> B_1036_974_0 ; _rtB ->
B_1036_4206_0 = _rtB -> B_1036_996_0 * _rtB -> B_1036_996_0 ; _rtB ->
B_1036_4207_0 = _rtB -> B_1036_4205_0 + _rtB -> B_1036_4206_0 ; _rtB ->
B_1036_4208_0 = muDoubleScalarSqrt ( _rtB -> B_1036_4207_0 ) ; _rtB ->
B_1036_4209_0 = _rtB -> B_1036_1047_0 * _rtB -> B_1036_1047_0 ; _rtB ->
B_1036_4210_0 = _rtB -> B_1036_1069_0 * _rtB -> B_1036_1069_0 ; _rtB ->
B_1036_4211_0 = _rtB -> B_1036_4209_0 + _rtB -> B_1036_4210_0 ; _rtB ->
B_1036_4212_0 = muDoubleScalarSqrt ( _rtB -> B_1036_4211_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
1036 , 4213 , SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_4214_0 = _rtB ->
B_1036_745_0 * _rtB -> B_1036_745_0 ; _rtB -> B_1036_4215_0 = _rtB ->
B_1036_771_0 * _rtB -> B_1036_771_0 ; _rtB -> B_1036_4216_0 = _rtB ->
B_1036_4214_0 + _rtB -> B_1036_4215_0 ; _rtB -> B_1036_4217_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_4216_0 ) ; _rtB -> B_1036_4218_0 = _rtB
-> B_1036_745_0 / _rtB -> B_1036_4217_0 ; u0 = _rtB -> B_1036_4218_0 ; if (
u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB ->
B_1036_4219_0 = muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_4220_0 = ( _rtB ->
B_1036_771_0 > _rtB -> B_1036_1747_0 ) ; if ( _rtB -> B_1036_4220_0 ) { _rtB
-> B_1036_4222_0 = _rtB -> B_1036_4219_0 ; } else { _rtB -> B_689_0_0 = _rtB
-> B_1036_4219_0 * _rtB -> B_1036_1746_0 ; _rtB -> B_1036_4222_0 = _rtB ->
B_689_0_0 ; } _rtB -> B_1036_4223_0 = _rtB -> B_1036_974_0 * _rtB ->
B_1036_974_0 ; _rtB -> B_1036_4224_0 = _rtB -> B_1036_996_0 * _rtB ->
B_1036_996_0 ; _rtB -> B_1036_4225_0 = _rtB -> B_1036_4223_0 + _rtB ->
B_1036_4224_0 ; _rtB -> B_1036_4226_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_4225_0 ) ; _rtB -> B_1036_4227_0 = _rtB -> B_1036_974_0 / _rtB ->
B_1036_4226_0 ; u0 = _rtB -> B_1036_4227_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } _rtB -> B_1036_4228_0 =
muDoubleScalarAcos ( u0 ) ; _rtB -> B_1036_4229_0 = ( _rtB -> B_1036_996_0 >
_rtB -> B_1036_1756_0_m ) ; if ( _rtB -> B_1036_4229_0 ) { _rtB ->
B_1036_4231_0 = _rtB -> B_1036_4228_0 ; } else { _rtB -> B_694_0_0 = _rtB ->
B_1036_4228_0 * _rtB -> B_1036_1755_0_b ; _rtB -> B_1036_4231_0 = _rtB ->
B_694_0_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
ssCallAccelRunBlock ( S , 1036 , 4232 , SS_CALL_MDL_OUTPUTS ) ; } _rtB ->
B_1036_4233_0 = _rtB -> B_1036_1047_0 * _rtB -> B_1036_1047_0 ; _rtB ->
B_1036_4234_0 = _rtB -> B_1036_1069_0 * _rtB -> B_1036_1069_0 ; _rtB ->
B_1036_4235_0 = _rtB -> B_1036_4233_0 + _rtB -> B_1036_4234_0 ; _rtB ->
B_1036_4236_0 = muDoubleScalarSqrt ( _rtB -> B_1036_4235_0 ) ; _rtB ->
B_1036_4237_0 = _rtB -> B_1036_1047_0 / _rtB -> B_1036_4236_0 ; u0 = _rtB ->
B_1036_4237_0 ; if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0
= - 1.0 ; } } _rtB -> B_1036_4238_0 = muDoubleScalarAcos ( u0 ) ; _rtB ->
B_1036_4239_0 = ( _rtB -> B_1036_1069_0 > _rtB -> B_1036_1765_0_e ) ; if (
_rtB -> B_1036_4239_0 ) { _rtB -> B_1036_4241_0 = _rtB -> B_1036_4238_0 ; }
else { _rtB -> B_697_0_0 = _rtB -> B_1036_4238_0 * _rtB -> B_1036_1764_0_e ;
_rtB -> B_1036_4241_0 = _rtB -> B_697_0_0 ; } _rtB -> B_1036_4242_0 = ( _rtB
-> B_1036_4148_0 && _rtB -> B_1036_935_0_j && _rtB -> B_1036_1008_0 && _rtB
-> B_1036_1081_0_o ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_1036_4243_0 = _rtDW -> Memory_PreviousInput_iys ; _rtB ->
B_1036_4244_0 = ! _rtB -> B_1036_4243_0 ; } _rtB -> B_1036_4245_0 = ( _rtB ->
B_1036_907_0 && _rtB -> B_1036_4244_0 ) ; _rtB -> B_1036_4246_0 = ( _rtB ->
B_1036_935_0_j && _rtB -> B_1036_4245_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_4247_0 = _rtDW ->
Memory_PreviousInput_fp ; _rtB -> B_1036_4248_0 = ! _rtB -> B_1036_4247_0 ; }
_rtB -> B_1036_4249_0 = ( _rtB -> B_1036_1308_0_k && _rtB -> B_1036_4248_0 )
; _rtB -> B_1036_4250_0 = ( _rtB -> B_1036_1008_0 && _rtB -> B_1036_4249_0 )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_4251_0 = _rtDW -> Memory_PreviousInput_bb ; _rtB -> B_1036_4252_0 = !
_rtB -> B_1036_4251_0 ; } _rtB -> B_1036_4253_0 = ( _rtB -> B_1036_1544_0_g
&& _rtB -> B_1036_4252_0 ) ; _rtB -> B_1036_4254_0 = ( _rtB ->
B_1036_1081_0_o && _rtB -> B_1036_4253_0 ) ; _rtB -> B_1036_4255_0 = ( _rtB
-> B_1036_4246_0 || _rtB -> B_1036_4250_0 || _rtB -> B_1036_4254_0 ) ; if (
_rtB -> B_1036_4255_0 ) { _rtB -> B_1036_4256_0 = _rtB -> B_1036_1770_0_l ; }
else { _rtB -> B_1036_4256_0 = _rtB -> B_1036_1736_0_b ; } _rtB ->
B_1036_4257_0 = ( _rtB -> B_1036_4245_0 || _rtB -> B_1036_4249_0 || _rtB ->
B_1036_4253_0 ) ; if ( _rtB -> B_1036_4257_0 ) { _rtB -> B_1036_4258_0 = _rtB
-> B_1036_1768_0_i ; } else { _rtB -> B_1036_4258_0 = _rtB -> B_1036_1736_0_b
; } _rtB -> B_1036_4259_0 = _rtB -> B_1036_733_0 * _rtB -> B_1036_1771_0_l ;
_rtB -> B_1036_4260_0 = _rtB -> B_1036_711_0 * _rtB -> B_1036_1774_0_i ; _rtB
-> B_1036_4261_0 = _rtB -> B_1036_4259_0 + _rtB -> B_1036_4260_0 ; _rtB ->
B_1036_4262_0 = _rtB -> B_1036_738_0 * _rtB -> B_1036_1771_0_l ; _rtB ->
B_1036_4263_0 = _rtB -> B_1036_716_0 * _rtB -> B_1036_1773_0_o ; _rtB ->
B_1036_4264_0 = _rtB -> B_1036_4262_0 + _rtB -> B_1036_4263_0 ; _rtB ->
B_1036_4265_0 = _rtB -> B_1036_4261_0 + _rtB -> B_1036_4264_0 ; _rtB ->
B_1036_4266_0 = _rtB -> B_1036_727_0 + _rtB -> B_1036_4265_0 ; _rtB ->
B_1036_4267_0 = _rtB -> B_1036_733_0 * _rtB -> B_1036_1771_0_l ; _rtB ->
B_1036_4268_0 = _rtB -> B_1036_711_0 * _rtB -> B_1036_1773_0_o ; _rtB ->
B_1036_4269_0 = _rtB -> B_1036_4267_0 + _rtB -> B_1036_4268_0 ; _rtB ->
B_1036_4270_0 = _rtB -> B_1036_738_0 * _rtB -> B_1036_1771_0_l ; _rtB ->
B_1036_4271_0 = _rtB -> B_1036_716_0 * _rtB -> B_1036_1774_0_i ; _rtB ->
B_1036_4272_0 = _rtB -> B_1036_4270_0 + _rtB -> B_1036_4271_0 ; _rtB ->
B_1036_4273_0 = _rtB -> B_1036_4269_0 + _rtB -> B_1036_4272_0 ; _rtB ->
B_1036_4274_0 = _rtB -> B_1036_727_0 + _rtB -> B_1036_4273_0 ; _rtB ->
B_1036_4275_0 = _rtB -> B_1036_711_0 * _rtB -> B_1036_1771_0_l ; _rtB ->
B_1036_4276_0 = _rtB -> B_1036_733_0 * _rtB -> B_1036_1773_0_o ; _rtB ->
B_1036_4277_0 = _rtB -> B_1036_4275_0 - _rtB -> B_1036_4276_0 ; _rtB ->
B_1036_4278_0 = _rtB -> B_1036_716_0 * _rtB -> B_1036_1771_0_l ; _rtB ->
B_1036_4279_0 = _rtB -> B_1036_738_0 * _rtB -> B_1036_1774_0_i ; _rtB ->
B_1036_4280_0 = _rtB -> B_1036_4278_0 - _rtB -> B_1036_4279_0 ; _rtB ->
B_1036_4281_0 = _rtB -> B_1036_4277_0 + _rtB -> B_1036_4280_0 ; _rtB ->
B_1036_4282_0 = _rtB -> B_1036_705_0 + _rtB -> B_1036_4281_0 ; _rtB ->
B_1036_4283_0 = _rtB -> B_1036_4282_0 * _rtB -> B_1036_1772_0_l ; _rtB ->
B_1036_4284_0 = _rtB -> B_1036_711_0 * _rtB -> B_1036_1771_0_l ; _rtB ->
B_1036_4285_0 = _rtB -> B_1036_733_0 * _rtB -> B_1036_1774_0_i ; _rtB ->
B_1036_4286_0 = _rtB -> B_1036_4284_0 - _rtB -> B_1036_4285_0 ; _rtB ->
B_1036_4287_0 = _rtB -> B_1036_716_0 * _rtB -> B_1036_1771_0_l ; _rtB ->
B_1036_4288_0 = _rtB -> B_1036_738_0 * _rtB -> B_1036_1773_0_o ; _rtB ->
B_1036_4289_0 = _rtB -> B_1036_4287_0 - _rtB -> B_1036_4288_0 ; _rtB ->
B_1036_4290_0 = _rtB -> B_1036_4286_0 + _rtB -> B_1036_4289_0 ; _rtB ->
B_1036_4291_0 = _rtB -> B_1036_705_0 + _rtB -> B_1036_4290_0 ; _rtB ->
B_1036_4292_0 = _rtB -> B_1036_4291_0 * _rtB -> B_1036_1772_0_l ; _rtB ->
B_1036_4293_0 = _rtB -> B_1036_4266_0 * _rtB -> B_1036_1772_0_l ; _rtB ->
B_1036_4294_0 = _rtB -> B_1036_4274_0 * _rtB -> B_1036_1772_0_l ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4295_0 =
_rtDW -> Memory_PreviousInput_fr ; _rtB -> B_1036_4296_0 = ! _rtB ->
B_1036_4295_0 ; _rtB -> B_1036_4297_0 = false ; _rtB -> B_1036_4298_0 = (
_rtB -> B_1036_1834_0_j && _rtB -> B_1036_4297_0 ) ; _rtB -> B_1036_4299_0 =
_rtDW -> Memory_PreviousInput_pq ; _rtB -> B_1036_4300_0 = ! _rtB ->
B_1036_4299_0 ; _rtB -> B_1036_4301_0 = false ; _rtB -> B_1036_4302_0 = (
_rtB -> B_1036_1830_0_n && _rtB -> B_1036_4301_0 ) ; _rtB -> B_1036_4303_0 =
_rtDW -> Memory_PreviousInput_cw ; _rtB -> B_1036_4304_0 = ! _rtB ->
B_1036_4303_0 ; _rtB -> B_1036_4305_0 = false ; _rtB -> B_1036_4306_0 = (
_rtB -> B_1036_1832_0_a && _rtB -> B_1036_4305_0 ) ; _rtB -> B_1036_4307_0 =
( _rtB -> B_1036_4298_0 || _rtB -> B_1036_4302_0 || _rtB -> B_1036_4306_0 ) ;
if ( _rtB -> B_1036_4307_0 ) { _rtB -> B_1036_4308_0 = _rtB ->
B_1036_1842_0_c ; } else { _rtB -> B_1036_4308_0 = _rtB -> B_1036_1788_0_k ;
} _rtB -> B_1036_4309_0 = ( _rtB -> B_1036_4297_0 || _rtB -> B_1036_4301_0 ||
_rtB -> B_1036_4305_0 ) ; if ( _rtB -> B_1036_4309_0 ) { _rtB ->
B_1036_4310_0 = _rtB -> B_1036_1840_0_c ; } else { _rtB -> B_1036_4310_0 =
_rtB -> B_1036_1788_0_k ; } ssCallAccelRunBlock ( S , 1036 , 4311 ,
SS_CALL_MDL_OUTPUTS ) ; { if ( ( _rtDW ->
TAQSigLogging_InsertedFor_in_at_outport_0_PWORK . AQHandles || _rtDW ->
TAQSigLogging_InsertedFor_in_at_outport_0_PWORK . SlioLTF ) && ssGetLogOutput
( S ) ) { sdiSlioSdiWriteSignal ( _rtDW ->
TAQSigLogging_InsertedFor_in_at_outport_0_PWORK . AQHandles , _rtDW ->
TAQSigLogging_InsertedFor_in_at_outport_0_PWORK . SlioLTF , 0 , ssGetTaskTime
( S , 1 ) , ( char * ) & _rtB -> B_1036_844_0 + 0 ) ; } } ssCallAccelRunBlock
( S , 1036 , 4313 , SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_4314_0 = ssGetT
( S ) ; _rtB -> B_1036_4315_0 = _rtB -> B_1036_1110_0 ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { DZG_v3_TriggeredSubsystem ( S , _rtB ->
B_1036_4315_0 , _rtB -> B_1036_4314_0 , & _rtB -> TriggeredSubsystem_l4 , &
_rtDW -> TriggeredSubsystem_l4 , & _rtZCE -> TriggeredSubsystem_l4 ) ; _rtB
-> B_1036_4317_0 = _rtB -> TriggeredSubsystem_l4 . B_83_0_0 + _rtB ->
B_1036_1950_0_b ; } _rtB -> B_1036_4318_0 = ( _rtB -> B_1036_4317_0 > _rtB ->
B_1036_4314_0 ) ; _rtB -> B_1036_4319_0 = ssGetT ( S ) ; _rtB ->
B_1036_4320_0 = _rtB -> B_1036_1346_0_e ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { DZG_v3_TriggeredSubsystem ( S , _rtB -> B_1036_4320_0 ,
_rtB -> B_1036_4319_0 , & _rtB -> TriggeredSubsystem_b , & _rtDW ->
TriggeredSubsystem_b , & _rtZCE -> TriggeredSubsystem_b ) ; _rtB ->
B_1036_4322_0 = _rtB -> TriggeredSubsystem_b . B_83_0_0 + _rtB ->
B_1036_1956_0 ; } _rtB -> B_1036_4323_0 = ( _rtB -> B_1036_4322_0 > _rtB ->
B_1036_4319_0 ) ; _rtB -> B_1036_4324_0 = ssGetT ( S ) ; _rtB ->
B_1036_4325_0 = _rtB -> B_1036_1582_0_d ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { DZG_v3_TriggeredSubsystem ( S , _rtB -> B_1036_4325_0 ,
_rtB -> B_1036_4324_0 , & _rtB -> TriggeredSubsystem_ie , & _rtDW ->
TriggeredSubsystem_ie , & _rtZCE -> TriggeredSubsystem_ie ) ; _rtB ->
B_1036_4327_0 = _rtB -> TriggeredSubsystem_ie . B_83_0_0 + _rtB ->
B_1036_1962_0_d ; } _rtB -> B_1036_4328_0 = ( _rtB -> B_1036_4327_0 > _rtB ->
B_1036_4324_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_4329_0 = _rtDW -> u4_PreviousInput_hl ; _rtB -> B_1036_4330_0
= _rtDW -> Memory_PreviousInput_dtg ; if ( _rtB -> B_1036_1969_0 > _rtP ->
P_704 ) { _rtB -> B_789_0_0 = ! _rtB -> B_1036_4330_0 ; _rtB -> B_789_1_0 =
false ; _rtB -> B_1036_4332_0 = _rtB -> B_789_1_0 ; } else { _rtB ->
B_1036_4332_0 = false ; } } _rtB -> B_1036_4333_0 = false ; _rtB ->
B_1036_4334_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_4335_0 = _rtB -> B_1036_2211_0_n ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_4335_0 , _rtB -> B_1036_4333_0 , _rtB ->
B_1036_4334_0 , _rtB -> B_1036_2209_0 , & _rtB -> ONDelay_llc , & _rtDW ->
ONDelay_llc , & _rtZCE -> ONDelay_llc ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_4337_0 = _rtB -> B_1036_2212_0_i ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_4337_0 , _rtB -> B_1036_4333_0 , _rtB ->
B_1036_4334_0 , _rtB -> B_1036_2209_0 , & _rtB -> OFFDelay_fw , & _rtDW ->
OFFDelay_fw , & _rtZCE -> OFFDelay_fw ) ; _rtB -> B_1036_4339_0 = ( _rtB ->
ONDelay_llc . B_90_17_0 || _rtB -> OFFDelay_fw . B_86_19_0 ) ; _rtB ->
B_1036_4340_0 = ( _rtB -> B_1036_4332_0 && _rtB -> B_1036_4339_0 ) ; _rtB ->
B_1036_4341_0 = ( _rtB -> B_1036_4329_0 || _rtB -> B_1036_4340_0 ) ; { real_T
* * uBuffer = ( real_T * * ) & _rtDW -> VariableTransportDelay_PWORK .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
VariableTransportDelay_PWORK . TUbufferPtrs [ 1 ] ; real_T * * xBuffer = (
real_T * * ) & _rtDW -> VariableTransportDelay_PWORK . TUbufferPtrs [ 2 ] ;
real_T simTime = ssGetT ( S ) ; real_T appliedDelay ; _rtB -> B_1036_4342_0 =
DZG_v3_acc_rt_VTDelayfindtDInterpolate ( ( ( X_DZG_v3_T * ) ssGetContStates (
S ) ) -> VariableTransportDelay_CSTATE , * tBuffer , * uBuffer , * xBuffer ,
_rtDW -> VariableTransportDelay_IWORK . CircularBufSize , _rtDW ->
VariableTransportDelay_IWORK . Head , _rtDW -> VariableTransportDelay_IWORK .
Tail , & _rtDW -> VariableTransportDelay_IWORK . Last , simTime , 0.0 , 1 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) , _rtP -> P_706 , & appliedDelay ) ; } _rtB -> B_1036_4343_0
= ! ( _rtB -> B_1036_4342_0 != 0.0 ) ; _rtB -> B_1036_4344_0 = ( _rtB ->
B_1036_4341_0 && _rtB -> B_1036_4343_0 ) ; _rtB -> B_1036_4345_0 = ( _rtB ->
B_1036_2206_0_n && _rtB -> B_1036_4344_0 ) ; _rtB -> B_1036_4346_0 = _rtB ->
B_1036_4345_0 ; _rtB -> B_1036_4347_0 = _rtB -> B_1036_4346_0 ; _rtB ->
B_1036_4348_0 = _rtB -> B_1036_318_0 * _rtB -> B_1036_318_0 ; _rtB ->
B_1036_4349_0 = _rtB -> B_1036_324_0 * _rtB -> B_1036_324_0 ; _rtB ->
B_1036_4350_0 = _rtB -> B_1036_4348_0 + _rtB -> B_1036_4349_0 ; _rtB ->
B_1036_4351_0 = muDoubleScalarSqrt ( _rtB -> B_1036_4350_0 ) ; if ( _rtB ->
B_1036_2203_0_h ) { _rtB -> B_788_0_0 = _rtB -> B_1036_4351_0 / _rtB ->
B_1036_2177_0_e ; _rtB -> B_788_1_0 = muDoubleScalarPower ( _rtB -> B_788_0_0
, _rtB -> B_1036_2202_0_a ) ; _rtB -> B_788_2_0 = _rtB -> B_1036_2173_0 -
_rtB -> B_788_1_0 ; _rtB -> B_788_3_0 = ( _rtB -> B_788_2_0 == _rtB ->
B_1036_2178_0_a ) ; if ( _rtB -> B_788_3_0 ) { _rtB -> B_788_4_0 = _rtB ->
B_1036_2179_0_e ; } else { _rtB -> B_788_4_0 = _rtB -> B_788_2_0 ; } _rtB ->
B_788_5_0 = _rtB -> B_1036_2201_0 / _rtB -> B_788_4_0 ; _rtB -> B_788_6_0 =
_rtB -> B_788_5_0 * _rtB -> B_1036_2125_0_h ; _rtB -> B_1036_4353_0 = _rtB ->
B_788_6_0 ; } else { _rtB -> B_1036_4353_0 = _rtB -> B_1036_2122_0_p ; } if (
_rtB -> B_1036_2205_0_p ) { _rtB -> B_787_0_0 = _rtB -> B_1036_4351_0 / _rtB
-> B_1036_2058_0_i ; _rtB -> B_787_1_0 = muDoubleScalarPower ( _rtB ->
B_787_0_0 , _rtB -> B_1036_2113_0 ) ; _rtB -> B_787_2_0 = _rtB ->
B_1036_2054_0_i - _rtB -> B_787_1_0 ; _rtB -> B_787_3_0 = ( _rtB -> B_787_2_0
== _rtB -> B_1036_2059_0_p ) ; if ( _rtB -> B_787_3_0 ) { _rtB -> B_787_4_0 =
_rtB -> B_1036_2060_0_a ; } else { _rtB -> B_787_4_0 = _rtB -> B_787_2_0 ; }
_rtB -> B_787_5_0 = _rtB -> B_1036_2115_0 / _rtB -> B_787_4_0 ; _rtB ->
B_787_6_0 = _rtB -> B_787_5_0 + _rtB -> B_1036_2117_0_a ; _rtB -> B_787_7_0 =
_rtB -> B_787_6_0 * _rtB -> B_1036_2128_0_a ; _rtB -> B_1036_4355_0 = _rtB ->
B_787_7_0 ; } else { _rtB -> B_1036_4355_0 = _rtB -> B_1036_2121_0 ; } _rtB
-> B_1036_4356_0 = _rtB -> B_1036_4353_0 + _rtB -> B_1036_4355_0 ; if ( _rtB
-> B_1036_2124_0_g > _rtP -> P_707 ) { _rtB -> B_786_0_0 = _rtB ->
B_1036_4351_0 / _rtB -> B_1036_2141_0 ; _rtB -> B_786_1_0 = ( _rtB ->
B_786_0_0 >= _rtB -> B_1036_2136_0 ) ; if ( _rtB -> B_786_1_0 ) { _rtB ->
B_786_3_0 = _rtB -> B_1036_2136_0 ; } else { _rtB -> B_785_0_0 = _rtB ->
B_1036_4351_0 / _rtB -> B_1036_2141_0 ; _rtB -> B_786_3_0 = _rtB -> B_785_0_0
; } _rtB -> B_786_4_0 = muDoubleScalarPower ( _rtB -> B_786_3_0 , _rtB ->
B_1036_2166_0_m ) ; _rtB -> B_786_5_0 = _rtB -> B_786_4_0 - _rtB ->
B_1036_2137_0_d ; _rtB -> B_786_6_0 = ( _rtB -> B_786_5_0 == _rtB ->
B_1036_2142_0_a ) ; if ( _rtB -> B_786_6_0 ) { _rtB -> B_786_7_0 = _rtB ->
B_1036_2143_0_k ; } else { _rtB -> B_786_7_0 = _rtB -> B_786_5_0 ; } _rtB ->
B_786_8_0 = _rtB -> B_1036_2165_0_c / _rtB -> B_786_7_0 ; _rtB -> B_786_9_0 =
_rtB -> B_786_8_0 * _rtB -> B_1036_2125_0_h ; _rtB -> B_1036_4359_0 = _rtB ->
B_786_9_0 ; } else { _rtB -> B_784_0_0 = _rtB -> B_1036_4351_0 / _rtB ->
B_1036_1985_0_e ; _rtB -> B_784_1_0 = ( _rtB -> B_784_0_0 >= _rtB ->
B_1036_1979_0_o ) ; if ( _rtB -> B_784_1_0 ) { _rtB -> B_784_2_0 = _rtB ->
B_1036_1979_0_o ; } else { _rtB -> B_784_2_0 = _rtB -> B_784_0_0 ; } _rtB ->
B_784_3_0 = muDoubleScalarPower ( _rtB -> B_784_2_0 , _rtB -> B_1036_2040_0_n
) ; _rtB -> B_784_4_0 = _rtB -> B_784_3_0 - _rtB -> B_1036_1980_0_k ; _rtB ->
B_784_5_0 = ( _rtB -> B_784_4_0 == _rtB -> B_1036_1986_0 ) ; if ( _rtB ->
B_784_5_0 ) { _rtB -> B_784_6_0 = _rtB -> B_1036_1987_0_l ; } else { _rtB ->
B_784_6_0 = _rtB -> B_784_4_0 ; } _rtB -> B_784_7_0 = _rtB -> B_1036_2042_0_c
/ _rtB -> B_784_6_0 ; _rtB -> B_784_8_0 = _rtB -> B_784_7_0 + _rtB ->
B_1036_2044_0 ; _rtB -> B_784_9_0 = _rtB -> B_784_8_0 * _rtB ->
B_1036_2128_0_a ; _rtB -> B_1036_4359_0 = _rtB -> B_784_9_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> VariableTransportDelay_PWORK_k .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
VariableTransportDelay_PWORK_k . TUbufferPtrs [ 1 ] ; real_T * * xBuffer = (
real_T * * ) & _rtDW -> VariableTransportDelay_PWORK_k . TUbufferPtrs [ 2 ] ;
real_T simTime = ssGetT ( S ) ; real_T appliedDelay ; _rtB -> B_1036_4360_0 =
DZG_v3_acc_rt_VTDelayfindtDInterpolate ( ( ( X_DZG_v3_T * ) ssGetContStates (
S ) ) -> VariableTransportDelay_CSTATE_l , * tBuffer , * uBuffer , * xBuffer
, _rtDW -> VariableTransportDelay_IWORK_g . CircularBufSize , _rtDW ->
VariableTransportDelay_IWORK_g . Head , _rtDW ->
VariableTransportDelay_IWORK_g . Tail , & _rtDW ->
VariableTransportDelay_IWORK_g . Last , simTime , 0.0 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
, _rtP -> P_709 , & appliedDelay ) ; } _rtB -> B_1036_4361_0 = false ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4362_0 =
_rtDW -> Memory_PreviousInput_lzly ; _rtB -> B_1036_4363_0 = ! _rtB ->
B_1036_4362_0 ; } _rtB -> B_1036_4364_0 = ( _rtB -> B_1036_4361_0 && _rtB ->
B_1036_4363_0 ) ; _rtB -> B_1036_4365_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4366_0 =
_rtDW -> Memory_PreviousInput_ogn ; _rtB -> B_1036_4367_0 = ! _rtB ->
B_1036_4366_0 ; } _rtB -> B_1036_4368_0 = _rtB -> B_1036_4361_0 ; _rtB ->
B_1036_4369_0 = _rtB -> B_1036_4368_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_4370_0 = _rtDW -> Memory_PreviousInput_i0
; _rtB -> B_1036_4371_0 = _rtB -> B_1036_2220_0_i [ 0 ] ; }
DZG_v3_POSITIVEEdge ( S , _rtB -> B_1036_4371_0 , _rtB -> B_1036_4369_0 ,
_rtB -> B_1036_4370_0 , & _rtB -> POSITIVEEdge_p , & _rtDW -> POSITIVEEdge_p
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_4373_0 = _rtB -> B_1036_2220_0_i [ 1 ] ; } DZG_v3_NEGATIVEEdge ( S ,
_rtB -> B_1036_4373_0 , _rtB -> B_1036_4369_0 , _rtB -> B_1036_4370_0 , &
_rtB -> NEGATIVEEdge_hv , & _rtDW -> NEGATIVEEdge_hv ) ; _rtB ->
B_1036_4375_0 = ( _rtB -> POSITIVEEdge_p . B_85_0_0 || _rtB ->
NEGATIVEEdge_hv . B_84_0_0 ) ; _rtB -> B_1036_4376_0 = ( _rtB ->
B_1036_4367_0 && _rtB -> B_1036_4375_0 ) ; _rtB -> B_1036_4377_0 = _rtB ->
B_1036_4376_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_4378_0 = _rtDW -> ICic_PreviousInput_l1 ; } if ( _rtB ->
B_1036_4377_0 ) { _rtB -> B_1036_4379_0 = _rtB -> B_1036_4365_0 ; } else {
_rtB -> B_1036_4379_0 = _rtB -> B_1036_4378_0 ; } isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4380_0 = _rtP -> P_711 ; } _rtB
-> B_1036_4381_0 = _rtB -> B_1036_4379_0 + _rtB -> B_1036_4380_0 ; _rtB ->
B_1036_4382_0 = ( _rtB -> B_1036_4381_0 > _rtB -> B_1036_4365_0 ) ; _rtB ->
B_1036_4383_0 = ( _rtB -> B_1036_4364_0 || _rtB -> B_1036_4382_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4384_0 =
_rtDW -> Memory_PreviousInput_k4 ; _rtB -> B_1036_4385_0 = ! _rtB ->
B_1036_4384_0 ; } _rtB -> B_1036_4386_0 = ( _rtB -> B_1036_4383_0 && _rtB ->
B_1036_4385_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_4387_0 = _rtDW -> u2_PreviousInput_n ; _rtB -> B_1036_4388_0 =
( _rtB -> B_1036_4387_0 && _rtB -> B_1036_2207_0_j ) ; _rtB -> B_1036_4389_0
= _rtB -> B_1036_4388_0 ; } _rtB -> B_1036_4390_0 = ssGetT ( S ) ; _rtB ->
B_1036_4391_0 = _rtB -> B_1036_4375_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { DZG_v3_TriggeredSubsystem ( S , _rtB -> B_1036_4391_0 ,
_rtB -> B_1036_4390_0 , & _rtB -> TriggeredSubsystem_f , & _rtDW ->
TriggeredSubsystem_f , & _rtZCE -> TriggeredSubsystem_f ) ; _rtB ->
B_1036_4393_0 = _rtB -> TriggeredSubsystem_f . B_83_0_0 + _rtB ->
B_1036_2215_0_n ; } _rtB -> B_1036_4394_0 = ( _rtB -> B_1036_4393_0 > _rtB ->
B_1036_4390_0 ) ; _rtB -> B_1036_4395_0 = _rtB -> B_1036_711_0 * _rtB ->
B_1036_711_0 ; _rtB -> B_1036_4396_0 = _rtB -> B_1036_733_0 * _rtB ->
B_1036_733_0 ; _rtB -> B_1036_4397_0 = _rtB -> B_1036_4395_0 + _rtB ->
B_1036_4396_0 ; _rtB -> B_1036_4398_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_4397_0 ) ; if ( _rtB -> B_1036_2227_0 > _rtP -> P_712 ) { _rtB ->
B_809_0_0 = ( _rtB -> B_1036_4398_0 < _rtB -> B_1036_2230_0_b ) ; _rtB ->
B_1036_4401_0 = _rtB -> B_809_0_0 ; } else { _rtB -> B_808_0_0 = ( _rtB ->
B_1036_4398_0 > _rtB -> B_1036_2230_0_b ) ; _rtB -> B_1036_4401_0 = _rtB ->
B_808_0_0 ; } _rtB -> B_1036_4402_0 = _rtB -> B_1036_711_0 * _rtB ->
B_1036_2388_0 ; _rtB -> B_1036_4403_0 = _rtB -> B_1036_733_0 * _rtB ->
B_1036_2391_0 ; _rtB -> B_1036_4404_0 = _rtB -> B_1036_4402_0 - _rtB ->
B_1036_4403_0 ; _rtB -> B_1036_4405_0 = _rtB -> B_1036_716_0 * _rtB ->
B_1036_2388_0 ; _rtB -> B_1036_4406_0 = _rtB -> B_1036_738_0 * _rtB ->
B_1036_2390_0 ; _rtB -> B_1036_4407_0 = _rtB -> B_1036_4405_0 - _rtB ->
B_1036_4406_0 ; _rtB -> B_1036_4408_0 = _rtB -> B_1036_4404_0 + _rtB ->
B_1036_4407_0 ; _rtB -> B_1036_4409_0 = _rtB -> B_1036_705_0 + _rtB ->
B_1036_4408_0 ; _rtB -> B_1036_4410_0 = _rtB -> B_1036_4409_0 * _rtB ->
B_1036_2389_0 ; _rtB -> B_1036_4411_0 = _rtB -> B_1036_4410_0 * _rtB ->
B_1036_4410_0 ; _rtB -> B_1036_4412_0 = _rtB -> B_1036_733_0 * _rtB ->
B_1036_2388_0 ; _rtB -> B_1036_4413_0 = _rtB -> B_1036_711_0 * _rtB ->
B_1036_2391_0 ; _rtB -> B_1036_4414_0 = _rtB -> B_1036_4412_0 + _rtB ->
B_1036_4413_0 ; _rtB -> B_1036_4415_0 = _rtB -> B_1036_738_0 * _rtB ->
B_1036_2388_0 ; _rtB -> B_1036_4416_0 = _rtB -> B_1036_716_0 * _rtB ->
B_1036_2390_0 ; _rtB -> B_1036_4417_0 = _rtB -> B_1036_4415_0 + _rtB ->
B_1036_4416_0 ; _rtB -> B_1036_4418_0 = _rtB -> B_1036_4414_0 + _rtB ->
B_1036_4417_0 ; _rtB -> B_1036_4419_0 = _rtB -> B_1036_727_0 + _rtB ->
B_1036_4418_0 ; _rtB -> B_1036_4420_0 = _rtB -> B_1036_4419_0 * _rtB ->
B_1036_2389_0 ; _rtB -> B_1036_4421_0 = _rtB -> B_1036_4420_0 * _rtB ->
B_1036_4420_0 ; _rtB -> B_1036_4422_0 = _rtB -> B_1036_4411_0 + _rtB ->
B_1036_4421_0 ; _rtB -> B_1036_4423_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_4422_0 ) ; if ( _rtB -> B_1036_2253_0_n > _rtP -> P_713 ) { _rtB ->
B_812_0_0 = ( _rtB -> B_1036_4423_0 < _rtB -> B_1036_2256_0_m ) ; _rtB ->
B_1036_4426_0 = _rtB -> B_812_0_0 ; } else { _rtB -> B_811_0_0 = ( _rtB ->
B_1036_4423_0 > _rtB -> B_1036_2256_0_m ) ; _rtB -> B_1036_4426_0 = _rtB ->
B_811_0_0 ; } _rtB -> B_1036_4427_0 = ( _rtB -> B_1036_4426_0 && _rtB ->
B_1036_2258_0_p ) ; if ( _rtB -> B_1036_2250_0_b ) { _rtB -> B_1036_4429_0 =
_rtB -> B_1036_4427_0 ; } else { _rtB -> B_810_0_0 = ( _rtB -> B_1036_4427_0
&& _rtB -> B_1036_2262_0_e ) ; _rtB -> B_1036_4429_0 = _rtB -> B_810_0_0 ; }
_rtB -> B_1036_4430_0 = ( _rtB -> B_1036_4429_0 || ( _rtB -> B_1036_2223_0_k
!= 0.0 ) || ( _rtB -> B_1036_2223_0_k != 0.0 ) || ( _rtB -> B_1036_2223_0_k
!= 0.0 ) ) ; _rtB -> B_1036_4431_0 = _rtB -> B_1036_705_0 * _rtB ->
B_1036_705_0 ; _rtB -> B_1036_4432_0 = _rtB -> B_1036_727_0 * _rtB ->
B_1036_727_0 ; _rtB -> B_1036_4433_0 = _rtB -> B_1036_4431_0 + _rtB ->
B_1036_4432_0 ; _rtB -> B_1036_4434_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_4433_0 ) ; if ( _rtB -> B_1036_2227_0 > _rtP -> P_714 ) { _rtB ->
B_807_0_0 = ( _rtB -> B_1036_4434_0 < _rtB -> B_1036_2230_0_b ) ; _rtB ->
B_1036_4437_0 = _rtB -> B_807_0_0 ; } else { _rtB -> B_806_0_0 = ( _rtB ->
B_1036_4434_0 > _rtB -> B_1036_2230_0_b ) ; _rtB -> B_1036_4437_0 = _rtB ->
B_806_0_0 ; } _rtB -> B_1036_4438_0 = _rtB -> B_1036_716_0 * _rtB ->
B_1036_716_0 ; _rtB -> B_1036_4439_0 = _rtB -> B_1036_738_0 * _rtB ->
B_1036_738_0 ; _rtB -> B_1036_4440_0 = _rtB -> B_1036_4438_0 + _rtB ->
B_1036_4439_0 ; _rtB -> B_1036_4441_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_4440_0 ) ; if ( _rtB -> B_1036_2227_0 > _rtP -> P_715 ) { _rtB ->
B_805_0_0 = ( _rtB -> B_1036_4441_0 < _rtB -> B_1036_2230_0_b ) ; _rtB ->
B_1036_4444_0 = _rtB -> B_805_0_0 ; } else { _rtB -> B_804_0_0 = ( _rtB ->
B_1036_4441_0 > _rtB -> B_1036_2230_0_b ) ; _rtB -> B_1036_4444_0 = _rtB ->
B_804_0_0 ; } _rtB -> B_1036_4445_0 = ( _rtB -> B_1036_4401_0 && _rtB ->
B_1036_2245_0_j ) ; _rtB -> B_1036_4446_0 = ( _rtB -> B_1036_4430_0 && _rtB
-> B_1036_2231_0_k ) ; _rtB -> B_1036_4447_0 = ( _rtB -> B_1036_4445_0 &&
_rtB -> B_1036_4446_0 ) ; _rtB -> B_1036_4450_0 = ( _rtB -> B_1036_4437_0 &&
_rtB -> B_1036_2244_0_b ) ; _rtB -> B_1036_4451_0 = ( _rtB -> B_1036_4450_0
&& _rtB -> B_1036_4446_0 ) ; if ( _rtB -> B_1036_2226_0_b ) { _rtB ->
B_1036_4449_0 = _rtB -> B_1036_4447_0 ; } else { _rtB -> B_803_0_0 = ( _rtB
-> B_1036_4447_0 && _rtB -> B_1036_2241_0_c ) ; _rtB -> B_1036_4449_0 = _rtB
-> B_803_0_0 ; } _rtB -> B_1036_4454_0 = ( _rtB -> B_1036_4444_0 && _rtB ->
B_1036_2246_0 ) ; _rtB -> B_1036_4455_0 = ( _rtB -> B_1036_4454_0 && _rtB ->
B_1036_4446_0 ) ; if ( _rtB -> B_1036_2226_0_b ) { _rtB -> B_1036_4453_0 =
_rtB -> B_1036_4451_0 ; _rtB -> B_1036_4457_0 = _rtB -> B_1036_4455_0 ; }
else { _rtB -> B_802_0_0 = ( _rtB -> B_1036_4451_0 && _rtB -> B_1036_2237_0 )
; _rtB -> B_1036_4453_0 = _rtB -> B_802_0_0 ; _rtB -> B_801_0_0 = ( _rtB ->
B_1036_4455_0 && _rtB -> B_1036_2243_0_a ) ; _rtB -> B_1036_4457_0 = _rtB ->
B_801_0_0 ; } _rtB -> B_1036_4458_0 = ( _rtB -> B_1036_4453_0 || _rtB ->
B_1036_4449_0 || _rtB -> B_1036_4457_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_4459_0 = _rtDW ->
Memory_PreviousInput_d12 ; _rtB -> B_1036_4460_0 = ! _rtB -> B_1036_4459_0 ;
} _rtB -> B_1036_4461_0 = ( _rtB -> B_1036_4458_0 && _rtB -> B_1036_4460_0 )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_4462_0 = _rtDW -> Memory_PreviousInput_nds ; _rtB -> B_1036_4463_0 = !
_rtB -> B_1036_4462_0 ; } _rtB -> B_1036_4464_0 = ( _rtB -> B_1036_4429_0 &&
_rtB -> B_1036_4463_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_1036_4465_0 = _rtDW -> u4_PreviousInput_du ; _rtB ->
B_1036_4466_0 = _rtDW -> Memory_PreviousInput_kt ; _rtB -> B_1036_4467_0 = !
_rtB -> B_1036_4466_0 ; } if ( _rtB -> B_1036_2268_0_p > _rtP -> P_716 ) {
_rtB -> B_878_0_0 = ( _rtB -> B_1036_4453_0 && _rtB -> B_1036_4467_0 ) ; _rtB
-> B_1036_4469_0 = _rtB -> B_878_0_0 ; } else { _rtB -> B_1036_4469_0 = _rtB
-> B_1036_4453_0 ; } _rtB -> B_1036_4470_0 = _rtB -> B_1036_907_0 ; _rtB ->
B_1036_4471_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_4472_0 = _rtB -> B_1036_2327_0_n ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_4472_0 , _rtB -> B_1036_4470_0 , _rtB ->
B_1036_4471_0 , _rtB -> B_1036_2325_0_n , & _rtB -> ONDelay_g5 , & _rtDW ->
ONDelay_g5 , & _rtZCE -> ONDelay_g5 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_4474_0 = _rtB -> B_1036_2328_0_b ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_4474_0 , _rtB -> B_1036_4470_0 , _rtB ->
B_1036_4471_0 , _rtB -> B_1036_2325_0_n , & _rtB -> OFFDelay_df , & _rtDW ->
OFFDelay_df , & _rtZCE -> OFFDelay_df ) ; _rtB -> B_1036_4476_0 = ( _rtB ->
ONDelay_g5 . B_90_17_0 || _rtB -> OFFDelay_df . B_86_19_0 ) ; _rtB ->
B_1036_4477_0 = ( _rtB -> B_1036_4469_0 && _rtB -> B_1036_4476_0 ) ; _rtB ->
B_1036_4478_0 = ( _rtB -> B_1036_4465_0 || _rtB -> B_1036_4477_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4479_0 =
_rtDW -> u2_PreviousInput_h ; } _rtB -> B_1036_4480_0 = ! _rtB ->
B_1036_4453_0 ; _rtB -> B_1036_4481_0 = ( _rtB -> B_1036_4479_0 && _rtB ->
B_1036_4480_0 ) ; _rtB -> B_1036_4482_0 = _rtB -> B_1036_4481_0 ; _rtB ->
B_1036_4483_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_4484_0 = _rtB -> B_1036_2334_0_n ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_4484_0 , _rtB -> B_1036_4482_0 , _rtB ->
B_1036_4483_0 , _rtB -> B_1036_2332_0_d , & _rtB -> ONDelay_bk , & _rtDW ->
ONDelay_bk , & _rtZCE -> ONDelay_bk ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_4486_0 = _rtB -> B_1036_2335_0_j ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_4486_0 , _rtB -> B_1036_4482_0 , _rtB ->
B_1036_4483_0 , _rtB -> B_1036_2332_0_d , & _rtB -> OFFDelay_pr , & _rtDW ->
OFFDelay_pr , & _rtZCE -> OFFDelay_pr ) ; _rtB -> B_1036_4488_0 = ( _rtB ->
ONDelay_bk . B_90_17_0 || _rtB -> OFFDelay_pr . B_86_19_0 ) ; _rtB ->
B_1036_4489_0 = ! _rtB -> B_1036_4488_0 ; _rtB -> B_1036_4490_0 = ( _rtB ->
B_1036_4478_0 && _rtB -> B_1036_4489_0 ) ; _rtB -> B_1036_4491_0 = ( _rtB ->
B_1036_2272_0_e && _rtB -> B_1036_4490_0 ) ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4492_0 = _rtDW ->
u4_PreviousInput_jh ; _rtB -> B_1036_4493_0 = _rtDW ->
Memory_PreviousInput_mk ; _rtB -> B_1036_4494_0 = ! _rtB -> B_1036_4493_0 ; }
if ( _rtB -> B_1036_2268_0_p > _rtP -> P_717 ) { _rtB -> B_895_0_0 = ( _rtB
-> B_1036_4449_0 && _rtB -> B_1036_4494_0 ) ; _rtB -> B_1036_4496_0 = _rtB ->
B_895_0_0 ; } else { _rtB -> B_1036_4496_0 = _rtB -> B_1036_4449_0 ; } _rtB
-> B_1036_4497_0 = _rtB -> B_1036_1308_0_k ; _rtB -> B_1036_4498_0 = ssGetT (
S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_4499_0 = _rtB -> B_1036_2341_0_h ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_4499_0 , _rtB -> B_1036_4497_0 , _rtB -> B_1036_4498_0 , _rtB ->
B_1036_2339_0 , & _rtB -> ONDelay_mv , & _rtDW -> ONDelay_mv , & _rtZCE ->
ONDelay_mv ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_4501_0 = _rtB -> B_1036_2342_0_k ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_4501_0 , _rtB -> B_1036_4497_0 , _rtB -> B_1036_4498_0 , _rtB ->
B_1036_2339_0 , & _rtB -> OFFDelay_ge , & _rtDW -> OFFDelay_ge , & _rtZCE ->
OFFDelay_ge ) ; _rtB -> B_1036_4503_0 = ( _rtB -> ONDelay_mv . B_90_17_0 ||
_rtB -> OFFDelay_ge . B_86_19_0 ) ; _rtB -> B_1036_4504_0 = ( _rtB ->
B_1036_4496_0 && _rtB -> B_1036_4503_0 ) ; _rtB -> B_1036_4505_0 = ( _rtB ->
B_1036_4492_0 || _rtB -> B_1036_4504_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_4506_0 = _rtDW -> u2_PreviousInput_o ;
} _rtB -> B_1036_4507_0 = ! _rtB -> B_1036_4449_0 ; _rtB -> B_1036_4508_0 = (
_rtB -> B_1036_4506_0 && _rtB -> B_1036_4507_0 ) ; _rtB -> B_1036_4509_0 =
_rtB -> B_1036_4508_0 ; _rtB -> B_1036_4510_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4511_0 =
_rtB -> B_1036_2348_0_i ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_4511_0 , _rtB
-> B_1036_4509_0 , _rtB -> B_1036_4510_0 , _rtB -> B_1036_2346_0_e , & _rtB
-> ONDelay_a , & _rtDW -> ONDelay_a , & _rtZCE -> ONDelay_a ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4513_0 =
_rtB -> B_1036_2349_0_k ; } DZG_v3_OFFDelay ( S , _rtB -> B_1036_4513_0 ,
_rtB -> B_1036_4509_0 , _rtB -> B_1036_4510_0 , _rtB -> B_1036_2346_0_e , &
_rtB -> OFFDelay_hb , & _rtDW -> OFFDelay_hb , & _rtZCE -> OFFDelay_hb ) ;
_rtB -> B_1036_4515_0 = ( _rtB -> ONDelay_a . B_90_17_0 || _rtB ->
OFFDelay_hb . B_86_19_0 ) ; _rtB -> B_1036_4516_0 = ! _rtB -> B_1036_4515_0 ;
_rtB -> B_1036_4517_0 = ( _rtB -> B_1036_4505_0 && _rtB -> B_1036_4516_0 ) ;
_rtB -> B_1036_4518_0 = ( _rtB -> B_1036_2273_0 && _rtB -> B_1036_4517_0 ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_4519_0 = _rtDW -> u4_PreviousInput_l ; _rtB -> B_1036_4520_0 = _rtDW
-> Memory_PreviousInput_cde ; _rtB -> B_1036_4521_0 = ! _rtB -> B_1036_4520_0
; } if ( _rtB -> B_1036_2268_0_p > _rtP -> P_718 ) { _rtB -> B_912_0_0 = (
_rtB -> B_1036_4457_0 && _rtB -> B_1036_4521_0 ) ; _rtB -> B_1036_4523_0 =
_rtB -> B_912_0_0 ; } else { _rtB -> B_1036_4523_0 = _rtB -> B_1036_4457_0 ;
} _rtB -> B_1036_4524_0 = _rtB -> B_1036_1544_0_g ; _rtB -> B_1036_4525_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_4526_0 = _rtB -> B_1036_2355_0_j ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_4526_0 , _rtB -> B_1036_4524_0 , _rtB -> B_1036_4525_0 , _rtB ->
B_1036_2353_0_m , & _rtB -> ONDelay_bd , & _rtDW -> ONDelay_bd , & _rtZCE ->
ONDelay_bd ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_4528_0 = _rtB -> B_1036_2356_0_k ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_4528_0 , _rtB -> B_1036_4524_0 , _rtB -> B_1036_4525_0 , _rtB ->
B_1036_2353_0_m , & _rtB -> OFFDelay_l0 , & _rtDW -> OFFDelay_l0 , & _rtZCE
-> OFFDelay_l0 ) ; _rtB -> B_1036_4530_0 = ( _rtB -> ONDelay_bd . B_90_17_0
|| _rtB -> OFFDelay_l0 . B_86_19_0 ) ; _rtB -> B_1036_4531_0 = ( _rtB ->
B_1036_4523_0 && _rtB -> B_1036_4530_0 ) ; _rtB -> B_1036_4532_0 = ( _rtB ->
B_1036_4519_0 || _rtB -> B_1036_4531_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_4533_0 = _rtDW -> u2_PreviousInput_b ;
} _rtB -> B_1036_4534_0 = ! _rtB -> B_1036_4457_0 ; _rtB -> B_1036_4535_0 = (
_rtB -> B_1036_4533_0 && _rtB -> B_1036_4534_0 ) ; _rtB -> B_1036_4536_0 =
_rtB -> B_1036_4535_0 ; _rtB -> B_1036_4537_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4538_0 =
_rtB -> B_1036_2362_0_e ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_4538_0 , _rtB
-> B_1036_4536_0 , _rtB -> B_1036_4537_0 , _rtB -> B_1036_2360_0 , & _rtB ->
ONDelay_j3 , & _rtDW -> ONDelay_j3 , & _rtZCE -> ONDelay_j3 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4540_0 =
_rtB -> B_1036_2363_0_b ; } DZG_v3_OFFDelay ( S , _rtB -> B_1036_4540_0 ,
_rtB -> B_1036_4536_0 , _rtB -> B_1036_4537_0 , _rtB -> B_1036_2360_0 , &
_rtB -> OFFDelay_kq , & _rtDW -> OFFDelay_kq , & _rtZCE -> OFFDelay_kq ) ;
_rtB -> B_1036_4542_0 = ( _rtB -> ONDelay_j3 . B_90_17_0 || _rtB ->
OFFDelay_kq . B_86_19_0 ) ; _rtB -> B_1036_4543_0 = ! _rtB -> B_1036_4542_0 ;
_rtB -> B_1036_4544_0 = ( _rtB -> B_1036_4532_0 && _rtB -> B_1036_4543_0 ) ;
_rtB -> B_1036_4545_0 = ( _rtB -> B_1036_2274_0_j && _rtB -> B_1036_4544_0 )
; _rtB -> B_1036_4546_0 = ( _rtB -> B_1036_4491_0 || _rtB -> B_1036_4518_0 ||
_rtB -> B_1036_4545_0 ) ; if ( _rtB -> B_1036_2369_0_h ) { _rtB -> B_930_0_0
= ( _rtB -> B_1036_4491_0 || _rtB -> B_1036_4518_0 || _rtB -> B_1036_4545_0 )
; _rtB -> B_1036_4549_0 = _rtB -> B_930_0_0 ; } else { _rtB -> B_929_0_0 = (
_rtB -> B_1036_4491_0 && _rtB -> B_1036_4518_0 && _rtB -> B_1036_4545_0 ) ;
_rtB -> B_929_1_0 = ( ( _rtB -> B_1036_2266_0_o != 0.0 ) && _rtB -> B_929_0_0
) ; _rtB -> B_929_2_0 = ! _rtB -> B_1036_4545_0 ; _rtB -> B_929_3_0 = ( _rtB
-> B_1036_4491_0 && _rtB -> B_1036_4518_0 && _rtB -> B_929_2_0 ) ; _rtB ->
B_929_4_0 = ! _rtB -> B_1036_4518_0 ; _rtB -> B_929_5_0 = ( _rtB ->
B_1036_4491_0 && _rtB -> B_929_4_0 && _rtB -> B_1036_4545_0 ) ; _rtB ->
B_929_6_0 = ! _rtB -> B_1036_4491_0 ; _rtB -> B_929_7_0 = ( _rtB -> B_929_6_0
&& _rtB -> B_1036_4518_0 && _rtB -> B_1036_4545_0 ) ; _rtB -> B_929_8_0 = (
_rtB -> B_929_7_0 || _rtB -> B_929_5_0 || _rtB -> B_929_3_0 ) ; _rtB ->
B_929_9_0 = ( ( _rtB -> B_1036_2265_0_l != 0.0 ) && _rtB -> B_929_8_0 ) ;
_rtB -> B_929_10_0 = ! _rtB -> B_1036_4518_0 ; _rtB -> B_929_11_0 = ! _rtB ->
B_1036_4491_0 ; _rtB -> B_929_12_0 = ( _rtB -> B_929_11_0 && _rtB ->
B_929_10_0 && _rtB -> B_1036_4545_0 ) ; _rtB -> B_929_13_0 = ! _rtB ->
B_1036_4545_0 ; _rtB -> B_929_14_0 = ! _rtB -> B_1036_4491_0 ; _rtB ->
B_929_15_0 = ( _rtB -> B_929_14_0 && _rtB -> B_1036_4518_0 && _rtB ->
B_929_13_0 ) ; _rtB -> B_929_16_0 = ! _rtB -> B_1036_4545_0 ; _rtB ->
B_929_17_0 = ! _rtB -> B_1036_4518_0 ; _rtB -> B_929_18_0 = ( _rtB ->
B_1036_4491_0 && _rtB -> B_929_17_0 && _rtB -> B_929_16_0 ) ; _rtB ->
B_929_19_0 = ( _rtB -> B_929_18_0 || _rtB -> B_929_15_0 || _rtB -> B_929_12_0
) ; _rtB -> B_929_20_0 = ( ( _rtB -> B_1036_2267_0_i != 0.0 ) && _rtB ->
B_929_19_0 ) ; _rtB -> B_929_21_0 = ( _rtB -> B_929_20_0 || _rtB -> B_929_9_0
|| _rtB -> B_929_1_0 ) ; _rtB -> B_1036_4549_0 = _rtB -> B_929_21_0 ; } _rtB
-> B_1036_4550_0 = ( _rtB -> B_1036_4549_0 && _rtB -> B_1036_4545_0 ) ; _rtB
-> B_1036_4551_0 = ( _rtB -> B_1036_4550_0 || ( _rtB -> B_1036_2263_0 != 0.0
) ) ; _rtB -> B_1036_4552_0 = _rtB -> B_1036_4551_0 ; _rtB -> B_1036_4553_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_4554_0 = _rtB -> B_1036_2299_0_n ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_4554_0 , _rtB -> B_1036_4552_0 , _rtB -> B_1036_4553_0 , _rtB ->
B_1036_2297_0_b , & _rtB -> ONDelay_eo , & _rtDW -> ONDelay_eo , & _rtZCE ->
ONDelay_eo ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_4556_0 = _rtB -> B_1036_2300_0 ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_4556_0 , _rtB -> B_1036_4552_0 , _rtB -> B_1036_4553_0 , _rtB ->
B_1036_2297_0_b , & _rtB -> OFFDelay_pb , & _rtDW -> OFFDelay_pb , & _rtZCE
-> OFFDelay_pb ) ; _rtB -> B_1036_4558_0 = _rtB -> B_1036_2270_0_c ; _rtB ->
B_1036_4559_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_4560_0 = _rtB -> B_1036_2320_0_i ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_4560_0 , _rtB -> B_1036_4558_0 , _rtB ->
B_1036_4559_0 , _rtB -> B_1036_2318_0_j , & _rtB -> ONDelay_om , & _rtDW ->
ONDelay_om , & _rtZCE -> ONDelay_om ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_4562_0 = _rtB -> B_1036_2321_0_k ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_4562_0 , _rtB -> B_1036_4558_0 , _rtB ->
B_1036_4559_0 , _rtB -> B_1036_2318_0_j , & _rtB -> OFFDelay_pj , & _rtDW ->
OFFDelay_pj , & _rtZCE -> OFFDelay_pj ) ; _rtB -> B_1036_4564_0 = ( _rtB ->
ONDelay_om . B_90_17_0 || _rtB -> OFFDelay_pj . B_86_19_0 ) ; _rtB ->
B_1036_4565_0 = _rtB -> B_1036_4551_0 ; _rtB -> B_1036_4566_0 = ssGetT ( S )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_4567_0 = _rtB -> B_1036_2285_0_b ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_4567_0 , _rtB -> B_1036_4565_0 , _rtB -> B_1036_4566_0 , _rtB ->
B_1036_2283_0_j , & _rtB -> ONDelay_ny , & _rtDW -> ONDelay_ny , & _rtZCE ->
ONDelay_ny ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_4569_0 = _rtB -> B_1036_2286_0_h ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_4569_0 , _rtB -> B_1036_4565_0 , _rtB -> B_1036_4566_0 , _rtB ->
B_1036_2283_0_j , & _rtB -> OFFDelay_ny , & _rtDW -> OFFDelay_ny , & _rtZCE
-> OFFDelay_ny ) ; if ( _rtB -> B_1036_2271_0_b ) { _rtB -> B_1036_4572_0 =
_rtB -> B_1036_4551_0 ; } else { if ( _rtB -> B_1036_4564_0 ) { _rtB ->
B_817_0_0 = ( _rtB -> ONDelay_eo . B_90_17_0 || _rtB -> OFFDelay_pb .
B_86_19_0 ) ; _rtB -> B_818_2_0 = _rtB -> B_817_0_0 ; } else { _rtB ->
B_816_0_0 = ( _rtB -> ONDelay_ny . B_90_17_0 || _rtB -> OFFDelay_ny .
B_86_19_0 ) ; _rtB -> B_818_2_0 = _rtB -> B_816_0_0 ; } _rtB -> B_1036_4572_0
= _rtB -> B_818_2_0 ; } _rtB -> B_1036_4573_0 = ( _rtB -> B_1036_4457_0 &&
_rtB -> B_1036_4572_0 && _rtB -> B_1036_4549_0 ) ; _rtB -> B_1036_4574_0 = (
_rtB -> B_1036_4549_0 && _rtB -> B_1036_4491_0 ) ; _rtB -> B_1036_4575_0 = (
_rtB -> B_1036_4549_0 && _rtB -> B_1036_4518_0 ) ; _rtB -> B_1036_4576_0 = (
_rtB -> B_1036_4574_0 || ( _rtB -> B_1036_2263_0 != 0.0 ) ) ; _rtB ->
B_1036_4577_0 = ( _rtB -> B_1036_4575_0 || ( _rtB -> B_1036_2263_0 != 0.0 ) )
; _rtB -> B_1036_4578_0 = _rtB -> B_1036_4576_0 ; _rtB -> B_1036_4579_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_4580_0 = _rtB -> B_1036_2313_0_p ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_4580_0 , _rtB -> B_1036_4578_0 , _rtB -> B_1036_4579_0 , _rtB ->
B_1036_2311_0_j , & _rtB -> ONDelay_cf , & _rtDW -> ONDelay_cf , & _rtZCE ->
ONDelay_cf ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_4582_0 = _rtB -> B_1036_2314_0_o ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_4582_0 , _rtB -> B_1036_4578_0 , _rtB -> B_1036_4579_0 , _rtB ->
B_1036_2311_0_j , & _rtB -> OFFDelay_ob , & _rtDW -> OFFDelay_ob , & _rtZCE
-> OFFDelay_ob ) ; _rtB -> B_1036_4584_0 = _rtB -> B_1036_4576_0 ; _rtB ->
B_1036_4585_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_4586_0 = _rtB -> B_1036_2292_0_c ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_4586_0 , _rtB -> B_1036_4584_0 , _rtB ->
B_1036_4585_0 , _rtB -> B_1036_2290_0 , & _rtB -> ONDelay_i3 , & _rtDW ->
ONDelay_i3 , & _rtZCE -> ONDelay_i3 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_4588_0 = _rtB -> B_1036_2293_0_h ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_4588_0 , _rtB -> B_1036_4584_0 , _rtB ->
B_1036_4585_0 , _rtB -> B_1036_2290_0 , & _rtB -> OFFDelay_c5 , & _rtDW ->
OFFDelay_c5 , & _rtZCE -> OFFDelay_c5 ) ; if ( _rtB -> B_1036_2271_0_b ) {
_rtB -> B_1036_4591_0 = _rtB -> B_1036_4576_0 ; } else { if ( _rtB ->
B_1036_4564_0 ) { _rtB -> B_814_0_0 = ( _rtB -> ONDelay_cf . B_90_17_0 ||
_rtB -> OFFDelay_ob . B_86_19_0 ) ; _rtB -> B_815_2_0 = _rtB -> B_814_0_0 ; }
else { _rtB -> B_813_0_0 = ( _rtB -> ONDelay_i3 . B_90_17_0 || _rtB ->
OFFDelay_c5 . B_86_19_0 ) ; _rtB -> B_815_2_0 = _rtB -> B_813_0_0 ; } _rtB ->
B_1036_4591_0 = _rtB -> B_815_2_0 ; } _rtB -> B_1036_4592_0 = ( _rtB ->
B_1036_4453_0 && _rtB -> B_1036_4591_0 && _rtB -> B_1036_4549_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4593_0 =
_rtDW -> Memory_PreviousInput_m5 ; _rtB -> B_1036_4594_0 = ! _rtB ->
B_1036_4593_0 ; } _rtB -> B_1036_4595_0 = ( _rtB -> B_1036_4592_0 && _rtB ->
B_1036_4594_0 ) ; _rtB -> B_1036_4596_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4597_0 =
_rtDW -> Memory_PreviousInput_ad ; _rtB -> B_1036_4598_0 = ! _rtB ->
B_1036_4597_0 ; } _rtB -> B_1036_4599_0 = _rtB -> B_1036_4592_0 ; _rtB ->
B_1036_4600_0 = _rtB -> B_1036_4599_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_4601_0 = _rtDW -> Memory_PreviousInput_bw
; _rtB -> B_1036_4602_0 = _rtB -> B_1036_2375_0_a [ 0 ] ; }
DZG_v3_POSITIVEEdge ( S , _rtB -> B_1036_4602_0 , _rtB -> B_1036_4600_0 ,
_rtB -> B_1036_4601_0 , & _rtB -> POSITIVEEdge_g , & _rtDW -> POSITIVEEdge_g
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_4604_0 = _rtB -> B_1036_2375_0_a [ 1 ] ; } DZG_v3_NEGATIVEEdge ( S ,
_rtB -> B_1036_4604_0 , _rtB -> B_1036_4600_0 , _rtB -> B_1036_4601_0 , &
_rtB -> NEGATIVEEdge_gl , & _rtDW -> NEGATIVEEdge_gl ) ; _rtB ->
B_1036_4606_0 = ( _rtB -> POSITIVEEdge_g . B_85_0_0 || _rtB ->
NEGATIVEEdge_gl . B_84_0_0 ) ; _rtB -> B_1036_4607_0 = ( _rtB ->
B_1036_4598_0 && _rtB -> B_1036_4606_0 ) ; _rtB -> B_1036_4608_0 = _rtB ->
B_1036_4607_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_4609_0 = _rtDW -> ICic_PreviousInput_ab ; } if ( _rtB ->
B_1036_4608_0 ) { _rtB -> B_1036_4610_0 = _rtB -> B_1036_4596_0 ; } else {
_rtB -> B_1036_4610_0 = _rtB -> B_1036_4609_0 ; } isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4611_0 = _rtP -> P_720 ; } _rtB
-> B_1036_4612_0 = _rtB -> B_1036_4610_0 + _rtB -> B_1036_4611_0 ; _rtB ->
B_1036_4613_0 = ( _rtB -> B_1036_4612_0 > _rtB -> B_1036_4596_0 ) ; _rtB ->
B_1036_4614_0 = ( _rtB -> B_1036_4595_0 || _rtB -> B_1036_4613_0 ) ; _rtB ->
B_1036_4615_0 = _rtB -> B_1036_4577_0 ; _rtB -> B_1036_4616_0 = ssGetT ( S )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_4617_0 = _rtB -> B_1036_2306_0_h ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_4617_0 , _rtB -> B_1036_4615_0 , _rtB -> B_1036_4616_0 , _rtB ->
B_1036_2304_0_l , & _rtB -> ONDelay_l1 , & _rtDW -> ONDelay_l1 , & _rtZCE ->
ONDelay_l1 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_4619_0 = _rtB -> B_1036_2307_0_p ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_4619_0 , _rtB -> B_1036_4615_0 , _rtB -> B_1036_4616_0 , _rtB ->
B_1036_2304_0_l , & _rtB -> OFFDelay_d2 , & _rtDW -> OFFDelay_d2 , & _rtZCE
-> OFFDelay_d2 ) ; _rtB -> B_1036_4621_0 = _rtB -> B_1036_4577_0 ; _rtB ->
B_1036_4622_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_4623_0 = _rtB -> B_1036_2278_0_d ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_4623_0 , _rtB -> B_1036_4621_0 , _rtB ->
B_1036_4622_0 , _rtB -> B_1036_2276_0_c , & _rtB -> ONDelay_bj , & _rtDW ->
ONDelay_bj , & _rtZCE -> ONDelay_bj ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_4625_0 = _rtB -> B_1036_2279_0_j ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_4625_0 , _rtB -> B_1036_4621_0 , _rtB ->
B_1036_4622_0 , _rtB -> B_1036_2276_0_c , & _rtB -> OFFDelay_nt , & _rtDW ->
OFFDelay_nt , & _rtZCE -> OFFDelay_nt ) ; if ( _rtB -> B_1036_2271_0_b ) {
_rtB -> B_1036_4628_0 = _rtB -> B_1036_4577_0 ; } else { if ( _rtB ->
B_1036_4564_0 ) { _rtB -> B_820_0_0 = ( _rtB -> ONDelay_l1 . B_90_17_0 ||
_rtB -> OFFDelay_d2 . B_86_19_0 ) ; _rtB -> B_821_2_0 = _rtB -> B_820_0_0 ; }
else { _rtB -> B_819_0_0 = ( _rtB -> ONDelay_bj . B_90_17_0 || _rtB ->
OFFDelay_nt . B_86_19_0 ) ; _rtB -> B_821_2_0 = _rtB -> B_819_0_0 ; } _rtB ->
B_1036_4628_0 = _rtB -> B_821_2_0 ; } _rtB -> B_1036_4629_0 = ( _rtB ->
B_1036_4449_0 && _rtB -> B_1036_4628_0 && _rtB -> B_1036_4549_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4630_0 =
_rtDW -> Memory_PreviousInput_ba ; _rtB -> B_1036_4631_0 = ! _rtB ->
B_1036_4630_0 ; } _rtB -> B_1036_4632_0 = ( _rtB -> B_1036_4629_0 && _rtB ->
B_1036_4631_0 ) ; _rtB -> B_1036_4633_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4634_0 =
_rtDW -> Memory_PreviousInput_ky ; _rtB -> B_1036_4635_0 = ! _rtB ->
B_1036_4634_0 ; } _rtB -> B_1036_4636_0 = _rtB -> B_1036_4629_0 ; _rtB ->
B_1036_4637_0 = _rtB -> B_1036_4636_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_4638_0 = _rtDW -> Memory_PreviousInput_jb
; _rtB -> B_1036_4639_0 = _rtB -> B_1036_2381_0 [ 0 ] ; } DZG_v3_POSITIVEEdge
( S , _rtB -> B_1036_4639_0 , _rtB -> B_1036_4637_0 , _rtB -> B_1036_4638_0 ,
& _rtB -> POSITIVEEdge_gu , & _rtDW -> POSITIVEEdge_gu ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4641_0 =
_rtB -> B_1036_2381_0 [ 1 ] ; } DZG_v3_NEGATIVEEdge ( S , _rtB ->
B_1036_4641_0 , _rtB -> B_1036_4637_0 , _rtB -> B_1036_4638_0 , & _rtB ->
NEGATIVEEdge_oc , & _rtDW -> NEGATIVEEdge_oc ) ; _rtB -> B_1036_4643_0 = (
_rtB -> POSITIVEEdge_gu . B_85_0_0 || _rtB -> NEGATIVEEdge_oc . B_84_0_0 ) ;
_rtB -> B_1036_4644_0 = ( _rtB -> B_1036_4635_0 && _rtB -> B_1036_4643_0 ) ;
_rtB -> B_1036_4645_0 = _rtB -> B_1036_4644_0 ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4646_0 = _rtDW ->
ICic_PreviousInput_nl ; } if ( _rtB -> B_1036_4645_0 ) { _rtB ->
B_1036_4647_0 = _rtB -> B_1036_4633_0 ; } else { _rtB -> B_1036_4647_0 = _rtB
-> B_1036_4646_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_1036_4648_0 = _rtP -> P_722 ; } _rtB -> B_1036_4649_0 = _rtB ->
B_1036_4647_0 + _rtB -> B_1036_4648_0 ; _rtB -> B_1036_4650_0 = ( _rtB ->
B_1036_4649_0 > _rtB -> B_1036_4633_0 ) ; _rtB -> B_1036_4651_0 = ( _rtB ->
B_1036_4632_0 || _rtB -> B_1036_4650_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_4652_0 = _rtDW ->
Memory_PreviousInput_auh ; _rtB -> B_1036_4653_0 = ! _rtB -> B_1036_4652_0 ;
} _rtB -> B_1036_4654_0 = ( _rtB -> B_1036_4573_0 && _rtB -> B_1036_4653_0 )
; _rtB -> B_1036_4655_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_4656_0 = _rtDW ->
Memory_PreviousInput_ije ; _rtB -> B_1036_4657_0 = ! _rtB -> B_1036_4656_0 ;
} _rtB -> B_1036_4658_0 = _rtB -> B_1036_4573_0 ; _rtB -> B_1036_4659_0 =
_rtB -> B_1036_4658_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_1036_4660_0 = _rtDW -> Memory_PreviousInput_gs ; _rtB ->
B_1036_4661_0 = _rtB -> B_1036_2387_0 [ 0 ] ; } DZG_v3_POSITIVEEdge ( S ,
_rtB -> B_1036_4661_0 , _rtB -> B_1036_4659_0 , _rtB -> B_1036_4660_0 , &
_rtB -> POSITIVEEdge_bj , & _rtDW -> POSITIVEEdge_bj ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4663_0 =
_rtB -> B_1036_2387_0 [ 1 ] ; } DZG_v3_NEGATIVEEdge ( S , _rtB ->
B_1036_4663_0 , _rtB -> B_1036_4659_0 , _rtB -> B_1036_4660_0 , & _rtB ->
NEGATIVEEdge_m , & _rtDW -> NEGATIVEEdge_m ) ; _rtB -> B_1036_4665_0 = ( _rtB
-> POSITIVEEdge_bj . B_85_0_0 || _rtB -> NEGATIVEEdge_m . B_84_0_0 ) ; _rtB
-> B_1036_4666_0 = ( _rtB -> B_1036_4657_0 && _rtB -> B_1036_4665_0 ) ; _rtB
-> B_1036_4667_0 = _rtB -> B_1036_4666_0 ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_4668_0 = _rtDW ->
ICic_PreviousInput_ef ; } if ( _rtB -> B_1036_4667_0 ) { _rtB ->
B_1036_4669_0 = _rtB -> B_1036_4655_0 ; } else { _rtB -> B_1036_4669_0 = _rtB
-> B_1036_4668_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_1036_4670_0 = _rtP -> P_724 ; } _rtB -> B_1036_4671_0 = _rtB ->
B_1036_4669_0 + _rtB -> B_1036_4670_0 ; _rtB -> B_1036_4672_0 = ( _rtB ->
B_1036_4671_0 > _rtB -> B_1036_4655_0 ) ; _rtB -> B_1036_4673_0 = ( _rtB ->
B_1036_4654_0 || _rtB -> B_1036_4672_0 ) ; _rtB -> B_1036_4674_0 = ( _rtB ->
B_1036_4614_0 || _rtB -> B_1036_4651_0 || _rtB -> B_1036_4673_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4675_0 =
_rtDW -> Memory_PreviousInput_e4 ; _rtB -> B_1036_4676_0 = ! _rtB ->
B_1036_4675_0 ; } _rtB -> B_1036_4677_0 = ( _rtB -> B_1036_4674_0 && _rtB ->
B_1036_4676_0 ) ; _rtB -> B_1036_4678_0 = ssGetT ( S ) ; _rtB ->
B_1036_4679_0 = _rtB -> B_1036_4606_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { DZG_v3_TriggeredSubsystem ( S , _rtB -> B_1036_4679_0 ,
_rtB -> B_1036_4678_0 , & _rtB -> TriggeredSubsystem_dl , & _rtDW ->
TriggeredSubsystem_dl , & _rtZCE -> TriggeredSubsystem_dl ) ; _rtB ->
B_1036_4681_0 = _rtB -> TriggeredSubsystem_dl . B_83_0_0 + _rtB ->
B_1036_2370_0_i ; } _rtB -> B_1036_4682_0 = ( _rtB -> B_1036_4681_0 > _rtB ->
B_1036_4678_0 ) ; _rtB -> B_1036_4683_0 = ssGetT ( S ) ; _rtB ->
B_1036_4684_0 = _rtB -> B_1036_4643_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { DZG_v3_TriggeredSubsystem ( S , _rtB -> B_1036_4684_0 ,
_rtB -> B_1036_4683_0 , & _rtB -> TriggeredSubsystem_f3 , & _rtDW ->
TriggeredSubsystem_f3 , & _rtZCE -> TriggeredSubsystem_f3 ) ; _rtB ->
B_1036_4686_0 = _rtB -> TriggeredSubsystem_f3 . B_83_0_0 + _rtB ->
B_1036_2376_0_i ; } _rtB -> B_1036_4687_0 = ( _rtB -> B_1036_4686_0 > _rtB ->
B_1036_4683_0 ) ; _rtB -> B_1036_4688_0 = ssGetT ( S ) ; _rtB ->
B_1036_4689_0 = _rtB -> B_1036_4665_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { DZG_v3_TriggeredSubsystem ( S , _rtB -> B_1036_4689_0 ,
_rtB -> B_1036_4688_0 , & _rtB -> TriggeredSubsystem_e4 , & _rtDW ->
TriggeredSubsystem_e4 , & _rtZCE -> TriggeredSubsystem_e4 ) ; _rtB ->
B_1036_4691_0 = _rtB -> TriggeredSubsystem_e4 . B_83_0_0 + _rtB ->
B_1036_2382_0 ; } _rtB -> B_1036_4692_0 = ( _rtB -> B_1036_4691_0 > _rtB ->
B_1036_4688_0 ) ; _rtB -> B_1036_4693_0 = _rtB -> B_1036_733_0 * _rtB ->
B_1036_2388_0 ; _rtB -> B_1036_4694_0 = _rtB -> B_1036_711_0 * _rtB ->
B_1036_2390_0 ; _rtB -> B_1036_4695_0 = _rtB -> B_1036_4693_0 + _rtB ->
B_1036_4694_0 ; _rtB -> B_1036_4696_0 = _rtB -> B_1036_738_0 * _rtB ->
B_1036_2388_0 ; _rtB -> B_1036_4697_0 = _rtB -> B_1036_716_0 * _rtB ->
B_1036_2391_0 ; _rtB -> B_1036_4698_0 = _rtB -> B_1036_4696_0 + _rtB ->
B_1036_4697_0 ; _rtB -> B_1036_4699_0 = _rtB -> B_1036_4695_0 + _rtB ->
B_1036_4698_0 ; _rtB -> B_1036_4700_0 = _rtB -> B_1036_727_0 + _rtB ->
B_1036_4699_0 ; _rtB -> B_1036_4701_0 = _rtB -> B_1036_733_0 + _rtB ->
B_1036_738_0 ; _rtB -> B_1036_4702_0 = _rtB -> B_1036_4701_0 + _rtB ->
B_1036_727_0 ; _rtB -> B_1036_4703_0 = _rtB -> B_1036_4702_0 * _rtB ->
B_1036_2389_0 ; _rtB -> B_1036_4704_0 = _rtB -> B_1036_711_0 * _rtB ->
B_1036_2388_0 ; _rtB -> B_1036_4705_0 = _rtB -> B_1036_733_0 * _rtB ->
B_1036_2390_0 ; _rtB -> B_1036_4706_0 = _rtB -> B_1036_4704_0 - _rtB ->
B_1036_4705_0 ; _rtB -> B_1036_4707_0 = _rtB -> B_1036_716_0 * _rtB ->
B_1036_2388_0 ; _rtB -> B_1036_4708_0 = _rtB -> B_1036_738_0 * _rtB ->
B_1036_2391_0 ; _rtB -> B_1036_4709_0 = _rtB -> B_1036_4707_0 - _rtB ->
B_1036_4708_0 ; _rtB -> B_1036_4710_0 = _rtB -> B_1036_4706_0 + _rtB ->
B_1036_4709_0 ; _rtB -> B_1036_4711_0 = _rtB -> B_1036_705_0 + _rtB ->
B_1036_4710_0 ; _rtB -> B_1036_4712_0 = _rtB -> B_1036_4711_0 * _rtB ->
B_1036_2389_0 ; _rtB -> B_1036_4713_0 = _rtB -> B_1036_4700_0 * _rtB ->
B_1036_2389_0 ; _rtB -> B_1036_4714_0 = _rtB -> B_1036_711_0 + _rtB ->
B_1036_716_0 ; _rtB -> B_1036_4715_0 = _rtB -> B_1036_4714_0 + _rtB ->
B_1036_705_0 ; _rtB -> B_1036_4716_0 = _rtB -> B_1036_4715_0 * _rtB ->
B_1036_2389_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_4717_0 = _rtDW -> Memory_PreviousInput_nh ;
ssCallAccelRunBlock ( S , 1036 , 4718 , SS_CALL_MDL_OUTPUTS ) ; } _rtB ->
B_1036_4719_0 = muDoubleScalarSin ( _rtB -> B_1036_275_0 [ 6 ] ) ; _rtB ->
B_1036_4720_0 = muDoubleScalarCos ( _rtB -> B_1036_275_0 [ 6 ] ) ; _rtB ->
B_1036_4721_0 = _rtB -> B_1036_4719_0 * _rtB -> B_1036_260_0 [ 6 ] ; _rtB ->
B_1036_4722_0 = _rtB -> B_1036_260_0 [ 6 ] * _rtB -> B_1036_4720_0 ; _rtB ->
B_1036_4723_0 = muDoubleScalarSin ( _rtB -> B_1036_275_0 [ 10 ] ) ; _rtB ->
B_1036_4724_0 = muDoubleScalarCos ( _rtB -> B_1036_275_0 [ 10 ] ) ; _rtB ->
B_1036_4725_0 = _rtB -> B_1036_4723_0 * _rtB -> B_1036_260_0 [ 10 ] ; _rtB ->
B_1036_4726_0 = _rtB -> B_1036_260_0 [ 10 ] * _rtB -> B_1036_4724_0 ; _rtB ->
B_1036_4727_0 = ( _rtX -> Integrator_CSTATE > _rtB -> B_1036_2394_0 ) ; _rtB
-> B_1036_4728_0 = _rtX -> Integrator_CSTATE - _rtB -> B_1036_2394_0 ; if ( (
_rtDW -> Initial_FirstOutputTime == ( rtMinusInf ) ) || ( _rtDW ->
Initial_FirstOutputTime == ssGetTaskTime ( S , 0 ) ) ) { _rtDW ->
Initial_FirstOutputTime = ssGetTaskTime ( S , 0 ) ; _rtB -> B_1036_4729_0 =
_rtP -> P_726 ; } else { _rtB -> B_1036_4729_0 = _rtB -> B_1036_4728_0 ; } if
( ssIsMajorTimeStep ( S ) != 0 ) { didZcEventOccur = ( _rtB -> B_1036_4727_0
&& ( _rtZCE -> Integrator_Reset_ZCE != 1 ) ) ; _rtZCE -> Integrator_Reset_ZCE
= _rtB -> B_1036_4727_0 ; if ( didZcEventOccur || ( _rtDW -> Integrator_IWORK
!= 0 ) ) { _rtX -> Integrator_CSTATE = _rtB -> B_1036_4729_0 ;
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
ssSetContTimeOutputInconsistentWithStateAtMajorStep ( S ) ; } } if ( _rtX ->
Integrator_CSTATE >= _rtP -> P_727 ) { if ( _rtX -> Integrator_CSTATE != _rtP
-> P_727 ) { _rtX -> Integrator_CSTATE = _rtP -> P_727 ;
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
ssSetContTimeOutputInconsistentWithStateAtMajorStep ( S ) ; } } else { if ( (
_rtX -> Integrator_CSTATE <= _rtP -> P_728 ) && ( _rtX -> Integrator_CSTATE
!= _rtP -> P_728 ) ) { _rtX -> Integrator_CSTATE = _rtP -> P_728 ;
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
ssSetContTimeOutputInconsistentWithStateAtMajorStep ( S ) ; } } _rtB ->
B_1036_4730_0 = _rtX -> Integrator_CSTATE ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_1036_4732_0 = _rtB -> B_1036_2393_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { if ( _rtB -> B_1036_4732_0 > 0.0 ) { if ( !
_rtDW -> AutomaticGainControl_MODE ) { if ( ssGetTaskTime ( S , 1 ) !=
ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; }
_rtDW -> AutomaticGainControl_MODE = true ; } } else { if ( _rtDW ->
AutomaticGainControl_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S
) ; if ( _rtDW -> Subsystempi2delay_b . Subsystempi2delay_MODE ) {
DZG_v3_Subsystempi2delay_Disable ( S , & _rtDW -> Subsystempi2delay_b ) ; }
if ( _rtDW -> Subsystem1_c . Subsystem1_MODE ) { DZG_v3_Subsystem1_Disable (
S , & _rtDW -> Subsystem1_c ) ; } _rtDW -> AutomaticGainControl_MODE = false
; } } } } if ( _rtDW -> AutomaticGainControl_MODE ) { { real_T * * uBuffer =
( real_T * * ) & _rtDW -> VariableTransportDelay_PWORK_a . TUbufferPtrs [ 0 ]
; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
VariableTransportDelay_PWORK_a . TUbufferPtrs [ 1 ] ; real_T * * xBuffer = (
real_T * * ) & _rtDW -> VariableTransportDelay_PWORK_a . TUbufferPtrs [ 2 ] ;
real_T simTime = ssGetT ( S ) ; real_T appliedDelay ; _rtB -> B_944_0_0 =
DZG_v3_acc_rt_VTDelayfindtDInterpolate ( ( ( X_DZG_v3_T * ) ssGetContStates (
S ) ) -> VariableTransportDelay_CSTATE_k , * tBuffer , * uBuffer , * xBuffer
, _rtDW -> VariableTransportDelay_IWORK_b . CircularBufSize , _rtDW ->
VariableTransportDelay_IWORK_b . Head , _rtDW ->
VariableTransportDelay_IWORK_b . Tail , & _rtDW ->
VariableTransportDelay_IWORK_b . Last , simTime , 0.0 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
, _rtP -> P_34 , & appliedDelay ) ; } _rtB -> B_944_1_0 = _rtX ->
integrator_CSTATE_kg ; _rtB -> B_944_2_0 = ssGetT ( S ) ; _rtB -> B_944_3_0_o
= ( _rtB -> B_944_2_0 >= _rtB -> B_944_0_0_g ) ; isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_944_4_0 = _rtDW ->
Memory_PreviousInput_lsc ; } if ( _rtB -> B_944_3_0_o ) { _rtB -> B_940_0_0 =
_rtB -> B_944_1_0 - _rtB -> B_944_0_0 ; _rtB -> B_940_1_0 = _rtB -> B_940_0_0
* _rtB -> B_1036_4717_0 ; _rtB -> B_944_6_0 = _rtB -> B_940_1_0 ; } else {
_rtB -> B_944_6_0 = _rtB -> B_944_4_0 ; } { real_T * * uBuffer = ( real_T * *
) & _rtDW -> VariableTransportDelay_PWORK_n . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> VariableTransportDelay_PWORK_n .
TUbufferPtrs [ 1 ] ; real_T * * xBuffer = ( real_T * * ) & _rtDW ->
VariableTransportDelay_PWORK_n . TUbufferPtrs [ 2 ] ; real_T simTime = ssGetT
( S ) ; real_T appliedDelay ; _rtB -> B_944_7_0 =
DZG_v3_acc_rt_VTDelayfindtDInterpolate ( ( ( X_DZG_v3_T * ) ssGetContStates (
S ) ) -> VariableTransportDelay_CSTATE_i , * tBuffer , * uBuffer , * xBuffer
, _rtDW -> VariableTransportDelay_IWORK_c . CircularBufSize , _rtDW ->
VariableTransportDelay_IWORK_c . Head , _rtDW ->
VariableTransportDelay_IWORK_c . Tail , & _rtDW ->
VariableTransportDelay_IWORK_c . Last , simTime , 0.0 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
, _rtP -> P_38 , & appliedDelay ) ; } _rtB -> B_944_8_0 = _rtX ->
integrator_CSTATE_bi0 ; _rtB -> B_944_9_0 = ssGetT ( S ) ; _rtB -> B_944_10_0
= ( _rtB -> B_944_9_0 >= _rtB -> B_944_1_0_a ) ; isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_944_11_0 = _rtDW ->
Memory_PreviousInput_kz ; } if ( _rtB -> B_944_10_0 ) { _rtB -> B_941_0_0 =
_rtB -> B_944_8_0 - _rtB -> B_944_7_0 ; _rtB -> B_941_1_0 = _rtB -> B_941_0_0
* _rtB -> B_1036_4717_0 ; _rtB -> B_944_13_0 = _rtB -> B_941_1_0 ; } else {
_rtB -> B_944_13_0 = _rtB -> B_944_11_0 ; } _rtB -> B_944_14_0 . re = _rtB ->
B_944_6_0 ; _rtB -> B_944_14_0 . im = _rtB -> B_944_13_0 ; _rtB -> B_944_15_0
= muDoubleScalarHypot ( _rtB -> B_944_14_0 . re , _rtB -> B_944_14_0 . im ) ;
_rtB -> B_944_15_1 = muDoubleScalarAtan2 ( _rtB -> B_944_14_0 . im , _rtB ->
B_944_14_0 . re ) ; u0 = _rtB -> B_944_15_0 ; maxV = _rtP -> P_42 ; u2 = _rtP
-> P_41 ; if ( u0 > u2 ) { _rtB -> B_944_16_0 = u2 ; } else if ( u0 < maxV )
{ _rtB -> B_944_16_0 = maxV ; } else { _rtB -> B_944_16_0 = u0 ; } maxV =
_rtB -> B_944_16_0 ; _rtB -> B_944_17_0 = 1.0 / maxV ; u0 = _rtB ->
B_1036_4717_0 ; maxV = _rtP -> P_44 ; u2 = _rtP -> P_43 ; if ( u0 > u2 ) {
_rtB -> B_944_18_0 = u2 ; } else if ( u0 < maxV ) { _rtB -> B_944_18_0 = maxV
; } else { _rtB -> B_944_18_0 = u0 ; } _rtB -> B_944_19_0 = 1.0 / _rtB ->
B_944_18_0 ; u0 = _rtB -> B_1036_4717_0 ; maxV = _rtP -> P_46 ; u2 = _rtP ->
P_45 ; if ( u0 > u2 ) { _rtB -> B_944_20_0 = u2 ; } else if ( u0 < maxV ) {
_rtB -> B_944_20_0 = maxV ; } else { _rtB -> B_944_20_0 = u0 ; } _rtB ->
B_944_21_0 = 1.0 / _rtB -> B_944_20_0 ; _rtB -> B_944_22_0 = _rtP -> P_47 *
_rtB -> B_944_15_1 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_944_23_0 = _rtB -> B_944_6_0_e ; _rtB -> B_944_24_0 = _rtB ->
B_944_4_0_a ; } for ( isHit = 0 ; isHit < 3 ; isHit ++ ) { _rtB -> B_944_25_0
[ isHit ] = 0.0 ; _rtB -> B_944_25_0 [ isHit ] += _rtP -> P_48 [ isHit ] *
_rtB -> B_1036_1606_0 [ 0 ] ; _rtB -> B_944_25_0 [ isHit ] += _rtP -> P_48 [
isHit + 3 ] * _rtB -> B_1036_1606_0 [ 1 ] ; _rtB -> B_944_25_0 [ isHit ] +=
_rtP -> P_48 [ isHit + 6 ] * _rtB -> B_1036_1606_0 [ 2 ] ; } _rtB ->
B_944_26_0 [ 0 ] = _rtP -> P_49 * _rtB -> B_944_25_0 [ 0 ] ; _rtB ->
B_944_26_0 [ 1 ] = _rtP -> P_49 * _rtB -> B_944_25_0 [ 1 ] ; _rtB ->
B_944_26_0 [ 2 ] = _rtP -> P_49 * _rtB -> B_944_25_0 [ 2 ] ;
DZG_v3_Subsystempi2delay ( S , _rtB -> B_944_23_0 , & _rtB -> B_944_26_0 [ 0
] , _rtB -> B_1036_4730_0 , & _rtB -> Subsystempi2delay_b , & _rtDW ->
Subsystempi2delay_b ) ; DZG_v3_Subsystem1 ( S , _rtB -> B_944_24_0 , & _rtB
-> B_944_26_0 [ 0 ] , _rtB -> B_1036_4730_0 , & _rtB -> Subsystem1_c , &
_rtDW -> Subsystem1_c ) ; if ( _rtB -> B_944_4_0_a != 0 ) { _rtB ->
B_944_29_0 [ 0 ] = _rtB -> Subsystem1_c . B_943_0_0 ; _rtB -> B_944_29_0 [ 1
] = _rtB -> Subsystem1_c . B_943_1_0 ; } else { _rtB -> B_944_29_0 [ 0 ] =
_rtB -> Subsystempi2delay_b . B_942_0_0 ; _rtB -> B_944_29_0 [ 1 ] = _rtB ->
Subsystempi2delay_b . B_942_1_0 ; } if ( ssIsMajorTimeStep ( S ) != 0 ) {
srUpdateBC ( _rtDW -> AutomaticGainControl_SubsysRanBC ) ; } } if ( _rtX ->
Integrator_CSTATE_l >= _rtP -> P_731 ) { if ( _rtX -> Integrator_CSTATE_l !=
_rtP -> P_731 ) { _rtX -> Integrator_CSTATE_l = _rtP -> P_731 ;
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } } else { if ( ( _rtX ->
Integrator_CSTATE_l <= _rtP -> P_732 ) && ( _rtX -> Integrator_CSTATE_l !=
_rtP -> P_732 ) ) { _rtX -> Integrator_CSTATE_l = _rtP -> P_732 ;
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } } _rtB -> B_1036_4734_0
= _rtX -> Integrator_CSTATE_l ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> VariableTransportDelay_PWORK_c . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> VariableTransportDelay_PWORK_c . TUbufferPtrs [ 1 ]
; real_T * * xBuffer = ( real_T * * ) & _rtDW ->
VariableTransportDelay_PWORK_c . TUbufferPtrs [ 2 ] ; real_T simTime = ssGetT
( S ) ; real_T appliedDelay ; _rtB -> B_1036_4735_0 =
DZG_v3_acc_rt_VTDelayfindtDInterpolate ( ( ( X_DZG_v3_T * ) ssGetContStates (
S ) ) -> VariableTransportDelay_CSTATE_p , * tBuffer , * uBuffer , * xBuffer
, _rtDW -> VariableTransportDelay_IWORK_n . CircularBufSize , _rtDW ->
VariableTransportDelay_IWORK_n . Head , _rtDW ->
VariableTransportDelay_IWORK_n . Tail , & _rtDW ->
VariableTransportDelay_IWORK_n . Last , simTime , 0.0 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
, _rtP -> P_734 , & appliedDelay ) ; } _rtB -> B_1036_4736_0 = _rtX ->
integrator_CSTATE_cn ; _rtB -> B_1036_4737_0 = ssGetT ( S ) ; _rtB ->
B_1036_4738_0 = ( _rtB -> B_1036_4737_0 >= _rtB -> B_1036_2395_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4739_0 =
_rtDW -> Memory_PreviousInput_hv ; } if ( _rtB -> B_1036_4738_0 ) { _rtB ->
B_945_0_0 = _rtB -> B_1036_4736_0 - _rtB -> B_1036_4735_0 ; _rtB -> B_945_1_0
= _rtB -> B_945_0_0 * _rtB -> B_1036_4717_0 ; _rtB -> B_1036_4741_0 = _rtB ->
B_945_1_0 ; } else { _rtB -> B_1036_4741_0 = _rtB -> B_1036_4739_0 ; } _rtB
-> B_1036_4742_0 = _rtB -> B_1036_4741_0 * _rtB -> B_944_17_0 ; _rtB ->
B_1036_4743_0 = _rtP -> P_737 * _rtB -> B_1036_4742_0 ; _rtB -> B_1036_4744_0
= _rtP -> P_738 * _rtB -> B_1036_4742_0 ; _rtB -> B_1036_4745_0 = _rtP ->
P_739 * _rtB -> B_1036_4742_0 ; _rtB -> B_1036_4746_0 = 0.0 ; _rtB ->
B_1036_4746_0 += _rtP -> P_741 * _rtX -> TransferFcn_CSTATE ; _rtB ->
B_1036_4746_0 += _rtP -> P_742 * _rtB -> B_1036_4745_0 ; _rtB ->
B_1036_4747_0 = ( _rtB -> B_1036_4743_0 + _rtB -> B_1036_4734_0 ) + _rtB ->
B_1036_4746_0 ; u0 = _rtB -> B_1036_4747_0 ; maxV = _rtP -> P_744 ; u2 = _rtP
-> P_743 ; if ( u0 > u2 ) { _rtB -> B_1036_4748_0 = u2 ; } else if ( u0 <
maxV ) { _rtB -> B_1036_4748_0 = maxV ; } else { _rtB -> B_1036_4748_0 = u0 ;
} _rtB -> B_1036_4749_0 = _rtP -> P_745 * _rtB -> B_1036_4748_0 ; if ( (
_rtDW -> LastMajorTimeA >= ssGetTaskTime ( S , 0 ) ) && ( _rtDW ->
LastMajorTimeB >= ssGetTaskTime ( S , 0 ) ) ) { _rtB -> B_1036_4750_0 = _rtB
-> B_1036_4749_0 ; } else { if ( ( ( _rtDW -> LastMajorTimeA < _rtDW ->
LastMajorTimeB ) && ( _rtDW -> LastMajorTimeB < ssGetTaskTime ( S , 0 ) ) )
|| ( ( _rtDW -> LastMajorTimeA >= _rtDW -> LastMajorTimeB ) && ( _rtDW ->
LastMajorTimeA >= ssGetTaskTime ( S , 0 ) ) ) ) { u2 = ssGetTaskTime ( S , 0
) - _rtDW -> LastMajorTimeB ; maxV = _rtDW -> PrevYB ; } else { u2 =
ssGetTaskTime ( S , 0 ) - _rtDW -> LastMajorTimeA ; maxV = _rtDW -> PrevYA ;
} riseValLimit = u2 * _rtP -> P_746 ; u0 = _rtB -> B_1036_4749_0 - maxV ; if
( u0 > riseValLimit ) { _rtB -> B_1036_4750_0 = maxV + riseValLimit ; } else
{ u2 *= _rtP -> P_747 ; if ( u0 < u2 ) { _rtB -> B_1036_4750_0 = maxV + u2 ;
} else { _rtB -> B_1036_4750_0 = _rtB -> B_1036_4749_0 ; } } } _rtB ->
B_1036_4751_0 = _rtX -> Integrator_x1_CSTATE ; _rtB -> B_1036_4752_0 = _rtP
-> P_749 * _rtB -> B_1036_4751_0 ; _rtB -> B_1036_4753_0 = _rtX ->
Integrator_x2_CSTATE ; _rtB -> B_1036_4754_0 = _rtP -> P_751 * _rtB ->
B_1036_4753_0 ; _rtB -> B_1036_4755_0 = _rtP -> P_752 * _rtB -> B_1036_4751_0
; _rtB -> B_1036_4756_0 = _rtP -> P_753 * _rtB -> B_1036_4753_0 ; _rtB ->
B_1036_4757_0 = _rtB -> B_1036_4752_0 + _rtB -> B_1036_4754_0 ; _rtB ->
B_1036_4758_0 = _rtB -> B_1036_4755_0 + _rtB -> B_1036_4756_0 ; _rtB ->
B_1036_4759_0 = _rtP -> P_754 * _rtB -> B_1036_4750_0 ; _rtB -> B_1036_4760_0
= _rtB -> B_1036_4757_0 + _rtB -> B_1036_4759_0 ; _rtB -> B_1036_4761_0 =
_rtP -> P_755 * _rtB -> B_1036_4750_0 ; _rtB -> B_1036_4762_0 = _rtB ->
B_1036_4758_0 + _rtB -> B_1036_4761_0 ; _rtB -> B_1036_4763_0 = _rtP -> P_756
* _rtB -> B_1036_4751_0 ; _rtB -> B_1036_4764_0 = _rtP -> P_757 * _rtB ->
B_1036_4753_0 ; _rtB -> B_1036_4765_0 = _rtB -> B_1036_4763_0 + _rtB ->
B_1036_4764_0 ; _rtB -> B_1036_4766_0 = _rtP -> P_758 * _rtB -> B_1036_4750_0
; _rtB -> B_1036_4767_0 = _rtB -> B_1036_4766_0 + _rtB -> B_1036_4765_0 ; u0
= _rtB -> B_1036_4717_0 ; maxV = _rtP -> P_760 ; u2 = _rtP -> P_759 ; if ( u0
> u2 ) { _rtB -> B_1036_4768_0 = u2 ; } else if ( u0 < maxV ) { _rtB ->
B_1036_4768_0 = maxV ; } else { _rtB -> B_1036_4768_0 = u0 ; } _rtB ->
B_1036_4769_0 = 1.0 / _rtB -> B_1036_4768_0 ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4770_0 = _rtB -> B_1036_2400_0_o ;
_rtB -> B_1036_4771_0 = _rtB -> B_1036_2398_0 ; } for ( isHit = 0 ; isHit < 3
; isHit ++ ) { _rtB -> B_1036_4772_0 [ isHit ] = 0.0 ; _rtB -> B_1036_4772_0
[ isHit ] += _rtP -> P_761 [ isHit ] * _rtB -> B_1036_1606_0 [ 0 ] ; _rtB ->
B_1036_4772_0 [ isHit ] += _rtP -> P_761 [ isHit + 3 ] * _rtB ->
B_1036_1606_0 [ 1 ] ; _rtB -> B_1036_4772_0 [ isHit ] += _rtP -> P_761 [
isHit + 6 ] * _rtB -> B_1036_1606_0 [ 2 ] ; } _rtB -> B_1036_4773_0 [ 0 ] =
_rtP -> P_762 * _rtB -> B_1036_4772_0 [ 0 ] ; _rtB -> B_1036_4773_0 [ 1 ] =
_rtP -> P_762 * _rtB -> B_1036_4772_0 [ 1 ] ; _rtB -> B_1036_4773_0 [ 2 ] =
_rtP -> P_762 * _rtB -> B_1036_4772_0 [ 2 ] ; DZG_v3_Subsystempi2delay ( S ,
_rtB -> B_1036_4770_0 , & _rtB -> B_1036_4773_0 [ 0 ] , _rtB -> B_1036_4730_0
, & _rtB -> Subsystempi2delay , & _rtDW -> Subsystempi2delay ) ;
DZG_v3_Subsystem1 ( S , _rtB -> B_1036_4771_0 , & _rtB -> B_1036_4773_0 [ 0 ]
, _rtB -> B_1036_4730_0 , & _rtB -> Subsystem1 , & _rtDW -> Subsystem1 ) ; if
( _rtB -> B_1036_2398_0 != 0 ) { _rtB -> B_1036_4776_0 [ 0 ] = _rtB ->
Subsystem1 . B_943_0_0 ; _rtB -> B_1036_4776_0 [ 1 ] = _rtB -> Subsystem1 .
B_943_1_0 ; } else { _rtB -> B_1036_4776_0 [ 0 ] = _rtB -> Subsystempi2delay
. B_942_0_0 ; _rtB -> B_1036_4776_0 [ 1 ] = _rtB -> Subsystempi2delay .
B_942_1_0 ; } _rtB -> B_1036_4777_0 = _rtB -> B_1036_1613_0 - _rtB ->
B_1036_1624_0 ; _rtB -> B_1036_4778_0 = _rtB -> B_1036_1617_0 - _rtB ->
B_1036_1628_0 ; _rtB -> B_1036_4779_0 = _rtB -> B_1036_404_0 - _rtB ->
B_1036_277_0 ; _rtB -> B_1036_4780_0 = _rtB -> B_1036_424_0 - _rtB ->
B_1036_321_0 ; _rtB -> B_1036_4781_0 = _rtB -> B_1036_277_0 - _rtB ->
B_1036_398_0 ; _rtB -> B_1036_4782_0 = _rtB -> B_1036_321_0 - _rtB ->
B_1036_418_0 ; _rtB -> B_1036_4783_0 = _rtB -> B_1036_1624_0 - _rtB ->
B_1036_1659_0 ; _rtB -> B_1036_4784_0 = _rtB -> B_1036_1628_0 - _rtB ->
B_1036_1663_0 ; _rtB -> B_1036_4785_0 = _rtB -> B_1036_398_0 - _rtB ->
B_1036_404_0 ; _rtB -> B_1036_4786_0 = _rtB -> B_1036_418_0 - _rtB ->
B_1036_424_0 ; _rtB -> B_1036_4787_0 = _rtB -> B_1036_1659_0 - _rtB ->
B_1036_1613_0 ; _rtB -> B_1036_4788_0 = _rtB -> B_1036_1663_0 - _rtB ->
B_1036_1617_0 ; _rtB -> B_1036_4789_0 = _rtB -> B_1036_4783_0 * _rtB ->
B_1036_4785_0 ; _rtB -> B_1036_4790_0 = _rtB -> B_1036_4784_0 * _rtB ->
B_1036_4786_0 ; _rtB -> B_1036_4791_0 = _rtB -> B_1036_4789_0 + _rtB ->
B_1036_4790_0 ; _rtB -> B_1036_4792_0 = _rtB -> B_1036_4785_0 * _rtB ->
B_1036_4785_0 ; _rtB -> B_1036_4793_0 = _rtB -> B_1036_4786_0 * _rtB ->
B_1036_4786_0 ; _rtB -> B_1036_4794_0 = _rtB -> B_1036_4792_0 + _rtB ->
B_1036_4793_0 ; _rtB -> B_1036_4795_0 = _rtB -> B_1036_4784_0 * _rtB ->
B_1036_4785_0 ; _rtB -> B_1036_4796_0 = _rtB -> B_1036_4783_0 * _rtB ->
B_1036_4786_0 ; _rtB -> B_1036_4797_0 = _rtB -> B_1036_4795_0 - _rtB ->
B_1036_4796_0 ; _rtB -> B_1036_4798_0 = _rtB -> B_1036_4791_0 / _rtB ->
B_1036_4794_0 ; _rtB -> B_1036_4799_0 = _rtB -> B_1036_4797_0 / _rtB ->
B_1036_4794_0 ; _rtB -> B_1036_4800_0 = _rtB -> B_1036_4777_0 * _rtB ->
B_1036_4781_0 ; _rtB -> B_1036_4801_0 = _rtB -> B_1036_4778_0 * _rtB ->
B_1036_4782_0 ; _rtB -> B_1036_4802_0 = _rtB -> B_1036_4800_0 + _rtB ->
B_1036_4801_0 ; _rtB -> B_1036_4803_0 = _rtB -> B_1036_4781_0 * _rtB ->
B_1036_4781_0 ; _rtB -> B_1036_4804_0 = _rtB -> B_1036_4782_0 * _rtB ->
B_1036_4782_0 ; _rtB -> B_1036_4805_0 = _rtB -> B_1036_4803_0 + _rtB ->
B_1036_4804_0 ; _rtB -> B_1036_4806_0 = _rtB -> B_1036_4778_0 * _rtB ->
B_1036_4781_0 ; _rtB -> B_1036_4807_0 = _rtB -> B_1036_4777_0 * _rtB ->
B_1036_4782_0 ; _rtB -> B_1036_4808_0 = _rtB -> B_1036_4806_0 - _rtB ->
B_1036_4807_0 ; _rtB -> B_1036_4809_0 = _rtB -> B_1036_4802_0 / _rtB ->
B_1036_4805_0 ; _rtB -> B_1036_4810_0 = _rtB -> B_1036_4808_0 / _rtB ->
B_1036_4805_0 ; _rtB -> B_1036_4811_0 = _rtB -> B_1036_4787_0 * _rtB ->
B_1036_4779_0 ; _rtB -> B_1036_4812_0 = _rtB -> B_1036_4788_0 * _rtB ->
B_1036_4780_0 ; _rtB -> B_1036_4813_0 = _rtB -> B_1036_4811_0 + _rtB ->
B_1036_4812_0 ; _rtB -> B_1036_4814_0 = _rtB -> B_1036_4779_0 * _rtB ->
B_1036_4779_0 ; _rtB -> B_1036_4815_0 = _rtB -> B_1036_4780_0 * _rtB ->
B_1036_4780_0 ; _rtB -> B_1036_4816_0 = _rtB -> B_1036_4814_0 + _rtB ->
B_1036_4815_0 ; _rtB -> B_1036_4817_0 = _rtB -> B_1036_4788_0 * _rtB ->
B_1036_4779_0 ; _rtB -> B_1036_4818_0 = _rtB -> B_1036_4787_0 * _rtB ->
B_1036_4780_0 ; _rtB -> B_1036_4819_0 = _rtB -> B_1036_4817_0 - _rtB ->
B_1036_4818_0 ; _rtB -> B_1036_4820_0 = _rtB -> B_1036_4813_0 / _rtB ->
B_1036_4816_0 ; _rtB -> B_1036_4821_0 = _rtB -> B_1036_4819_0 / _rtB ->
B_1036_4816_0 ; _rtB -> B_1036_4822_0 = _rtX -> integrator_CSTATE_em ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b0 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_b0 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_764 ; _rtB -> B_1036_4823_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_pg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pg . Last , _rtDW -> TransportDelay_IWORK_pg . Tail ,
_rtDW -> TransportDelay_IWORK_pg . Head , _rtP -> P_765 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_4824_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4825_0 = _rtP -> P_766 ; } _rtB ->
B_1036_4826_0 = ( _rtB -> B_1036_4824_0 >= _rtB -> B_1036_4825_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4827_0 =
_rtDW -> Memory_PreviousInput_hc ; } if ( _rtB -> B_1036_4826_0 ) { _rtB ->
B_977_0_0 = _rtB -> B_1036_4822_0 - _rtB -> B_1036_4823_0 ; _rtB -> B_977_1_0
= _rtP -> P_84 * _rtB -> B_977_0_0 ; _rtB -> B_1036_4829_0 = _rtB ->
B_977_1_0 ; } else { _rtB -> B_1036_4829_0 = _rtB -> B_1036_4827_0 ; } _rtB
-> B_1036_4830_0 = _rtX -> integrator_CSTATE_mk ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fo . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fo .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_769 ; _rtB -> B_1036_4831_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_os . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_os . Last , _rtDW -> TransportDelay_IWORK_os . Tail ,
_rtDW -> TransportDelay_IWORK_os . Head , _rtP -> P_770 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_4832_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4833_0 = _rtP -> P_771 ; } _rtB ->
B_1036_4834_0 = ( _rtB -> B_1036_4832_0 >= _rtB -> B_1036_4833_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4835_0 =
_rtDW -> Memory_PreviousInput_iq ; } if ( _rtB -> B_1036_4834_0 ) { _rtB ->
B_976_0_0 = _rtB -> B_1036_4830_0 - _rtB -> B_1036_4831_0 ; _rtB -> B_976_1_0
= _rtP -> P_83 * _rtB -> B_976_0_0 ; _rtB -> B_1036_4837_0 = _rtB ->
B_976_1_0 ; } else { _rtB -> B_1036_4837_0 = _rtB -> B_1036_4835_0 ; } _rtB
-> B_1036_4838_0 . re = _rtB -> B_1036_4829_0 ; _rtB -> B_1036_4838_0 . im =
_rtB -> B_1036_4837_0 ; _rtB -> B_1036_4839_0 = muDoubleScalarHypot ( _rtB ->
B_1036_4838_0 . re , _rtB -> B_1036_4838_0 . im ) ; _rtB -> B_1036_4839_1 =
muDoubleScalarAtan2 ( _rtB -> B_1036_4838_0 . im , _rtB -> B_1036_4838_0 . re
) ; _rtB -> B_1036_4840_0 = _rtP -> P_773 * _rtB -> B_1036_4839_1 ; _rtB ->
B_1036_4841_0 = _rtP -> P_774 * _rtB -> B_1036_4840_0 ; maxV = _rtB ->
B_1036_4841_0 ; u0 = _rtB -> B_1036_4839_0 ; u2 = muDoubleScalarCos ( maxV )
; maxV = muDoubleScalarSin ( maxV ) ; _rtB -> B_1036_4842_0 . re = u0 * u2 ;
_rtB -> B_1036_4842_0 . im = u0 * maxV ; _rtB -> B_1036_4843_0 = _rtX ->
integrator_CSTATE_ey ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_hv . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_hv . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_776 ; _rtB
-> B_1036_4844_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_jt . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_jt . Last , _rtDW -> TransportDelay_IWORK_jt .
Tail , _rtDW -> TransportDelay_IWORK_jt . Head , _rtP -> P_777 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_4845_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4846_0 =
_rtP -> P_778 ; } _rtB -> B_1036_4847_0 = ( _rtB -> B_1036_4845_0 >= _rtB ->
B_1036_4846_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_4848_0 = _rtDW -> Memory_PreviousInput_pi ; } if ( _rtB ->
B_1036_4847_0 ) { _rtB -> B_979_0_0 = _rtB -> B_1036_4843_0 - _rtB ->
B_1036_4844_0 ; _rtB -> B_979_1_0 = _rtP -> P_86 * _rtB -> B_979_0_0 ; _rtB
-> B_1036_4850_0 = _rtB -> B_979_1_0 ; } else { _rtB -> B_1036_4850_0 = _rtB
-> B_1036_4848_0 ; } _rtB -> B_1036_4851_0 = _rtX -> integrator_CSTATE_dc ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_eq .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_eq . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_781 ; _rtB -> B_1036_4852_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_it . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_it . Last , _rtDW -> TransportDelay_IWORK_it . Tail ,
_rtDW -> TransportDelay_IWORK_it . Head , _rtP -> P_782 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_4853_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4854_0 = _rtP -> P_783 ; } _rtB ->
B_1036_4855_0 = ( _rtB -> B_1036_4853_0 >= _rtB -> B_1036_4854_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4856_0 =
_rtDW -> Memory_PreviousInput_ox ; } if ( _rtB -> B_1036_4855_0 ) { _rtB ->
B_978_0_0 = _rtB -> B_1036_4851_0 - _rtB -> B_1036_4852_0 ; _rtB -> B_978_1_0
= _rtP -> P_85 * _rtB -> B_978_0_0 ; _rtB -> B_1036_4858_0 = _rtB ->
B_978_1_0 ; } else { _rtB -> B_1036_4858_0 = _rtB -> B_1036_4856_0 ; } _rtB
-> B_1036_4859_0 . re = _rtB -> B_1036_4850_0 ; _rtB -> B_1036_4859_0 . im =
_rtB -> B_1036_4858_0 ; _rtB -> B_1036_4860_0 = muDoubleScalarHypot ( _rtB ->
B_1036_4859_0 . re , _rtB -> B_1036_4859_0 . im ) ; _rtB -> B_1036_4860_1 =
muDoubleScalarAtan2 ( _rtB -> B_1036_4859_0 . im , _rtB -> B_1036_4859_0 . re
) ; _rtB -> B_1036_4861_0 = _rtP -> P_785 * _rtB -> B_1036_4860_1 ; _rtB ->
B_1036_4862_0 = _rtP -> P_786 * _rtB -> B_1036_4861_0 ; maxV = _rtB ->
B_1036_4862_0 ; u0 = _rtB -> B_1036_4860_0 ; u2 = muDoubleScalarCos ( maxV )
; maxV = muDoubleScalarSin ( maxV ) ; _rtB -> B_1036_4863_0 . re = u0 * u2 ;
_rtB -> B_1036_4863_0 . im = u0 * maxV ; _rtB -> B_1036_4864_0 = _rtX ->
integrator_CSTATE_jn ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ny . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ny . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_788 ; _rtB
-> B_1036_4865_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_bj . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_bj . Last , _rtDW -> TransportDelay_IWORK_bj .
Tail , _rtDW -> TransportDelay_IWORK_bj . Head , _rtP -> P_789 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_4866_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4867_0 =
_rtP -> P_790 ; } _rtB -> B_1036_4868_0 = ( _rtB -> B_1036_4866_0 >= _rtB ->
B_1036_4867_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_4869_0 = _rtDW -> Memory_PreviousInput_kw ; } if ( _rtB ->
B_1036_4868_0 ) { _rtB -> B_981_0_0 = _rtB -> B_1036_4864_0 - _rtB ->
B_1036_4865_0 ; _rtB -> B_981_1_0 = _rtP -> P_88 * _rtB -> B_981_0_0 ; _rtB
-> B_1036_4871_0 = _rtB -> B_981_1_0 ; } else { _rtB -> B_1036_4871_0 = _rtB
-> B_1036_4869_0 ; } _rtB -> B_1036_4872_0 = _rtX -> integrator_CSTATE_nv ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nw .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_nw . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_793 ; _rtB -> B_1036_4873_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_ayg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ayg . Last , _rtDW -> TransportDelay_IWORK_ayg . Tail ,
_rtDW -> TransportDelay_IWORK_ayg . Head , _rtP -> P_794 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } _rtB -> B_1036_4874_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4875_0 = _rtP -> P_795 ; } _rtB ->
B_1036_4876_0 = ( _rtB -> B_1036_4874_0 >= _rtB -> B_1036_4875_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4877_0 =
_rtDW -> Memory_PreviousInput_py ; } if ( _rtB -> B_1036_4876_0 ) { _rtB ->
B_980_0_0 = _rtB -> B_1036_4872_0 - _rtB -> B_1036_4873_0 ; _rtB -> B_980_1_0
= _rtP -> P_87 * _rtB -> B_980_0_0 ; _rtB -> B_1036_4879_0 = _rtB ->
B_980_1_0 ; } else { _rtB -> B_1036_4879_0 = _rtB -> B_1036_4877_0 ; } _rtB
-> B_1036_4880_0 . re = _rtB -> B_1036_4871_0 ; _rtB -> B_1036_4880_0 . im =
_rtB -> B_1036_4879_0 ; _rtB -> B_1036_4881_0 = muDoubleScalarHypot ( _rtB ->
B_1036_4880_0 . re , _rtB -> B_1036_4880_0 . im ) ; _rtB -> B_1036_4881_1 =
muDoubleScalarAtan2 ( _rtB -> B_1036_4880_0 . im , _rtB -> B_1036_4880_0 . re
) ; _rtB -> B_1036_4882_0 = _rtP -> P_797 * _rtB -> B_1036_4881_1 ; _rtB ->
B_1036_4883_0 = _rtP -> P_798 * _rtB -> B_1036_4882_0 ; maxV = _rtB ->
B_1036_4883_0 ; u0 = _rtB -> B_1036_4881_0 ; u2 = muDoubleScalarCos ( maxV )
; maxV = muDoubleScalarSin ( maxV ) ; _rtB -> B_1036_4884_0 . re = u0 * u2 ;
_rtB -> B_1036_4884_0 . im = u0 * maxV ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_4885_0 = _rtB -> B_1036_2403_0_o ; }
DZG_v3_NegSeqComputation ( S , _rtB -> B_1036_4885_0 , _rtB -> B_1036_4842_0
, _rtB -> B_1036_4863_0 , _rtB -> B_1036_4884_0 , & _rtB -> PosSeqComputation
, & _rtDW -> PosSeqComputation , & _rtP -> PosSeqComputation ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_4887_0 =
_rtB -> B_1036_2404_0_o ; } DZG_v3_NegSeqComputation ( S , _rtB ->
B_1036_4887_0 , _rtB -> B_1036_4842_0 , _rtB -> B_1036_4863_0 , _rtB ->
B_1036_4884_0 , & _rtB -> NegSeqComputation , & _rtDW -> NegSeqComputation ,
& _rtP -> NegSeqComputation ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_1036_4889_0 = _rtB -> B_1036_2405_0_n ; }
DZG_v3_ZeroSeqComputation ( S , _rtB -> B_1036_4889_0 , _rtB -> B_1036_4842_0
, _rtB -> B_1036_4863_0 , _rtB -> B_1036_4884_0 , & _rtB ->
ZeroSeqComputation , & _rtDW -> ZeroSeqComputation , & _rtP ->
ZeroSeqComputation ) ; _rtB -> B_1036_4891_0 [ 0 ] = muDoubleScalarHypot (
_rtB -> PosSeqComputation . B_982_2_0 . re , _rtB -> PosSeqComputation .
B_982_2_0 . im ) ; _rtB -> B_1036_4891_0 [ 1 ] = muDoubleScalarHypot ( _rtB
-> NegSeqComputation . B_982_2_0 . re , _rtB -> NegSeqComputation . B_982_2_0
. im ) ; _rtB -> B_1036_4891_0 [ 2 ] = muDoubleScalarHypot ( _rtB ->
ZeroSeqComputation . B_984_1_0 . re , _rtB -> ZeroSeqComputation . B_984_1_0
. im ) ; _rtB -> B_1036_4891_1 [ 0 ] = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation . B_982_2_0 . im , _rtB -> PosSeqComputation . B_982_2_0 .
re ) ; _rtB -> B_1036_4891_1 [ 1 ] = muDoubleScalarAtan2 ( _rtB ->
NegSeqComputation . B_982_2_0 . im , _rtB -> NegSeqComputation . B_982_2_0 .
re ) ; _rtB -> B_1036_4891_1 [ 2 ] = muDoubleScalarAtan2 ( _rtB ->
ZeroSeqComputation . B_984_1_0 . im , _rtB -> ZeroSeqComputation . B_984_1_0
. re ) ; _rtB -> B_1036_4892_0 = _rtP -> P_799 * _rtB -> B_1036_4891_0 [ 2 ]
; _rtB -> B_1036_4893_0 = _rtP -> P_800 * _rtB -> B_1036_4891_1 [ 2 ] ; _rtB
-> B_1036_4894_0 = _rtP -> P_801 * _rtB -> B_1036_4893_0 ; _rtB ->
B_1036_4895_0 = muDoubleScalarCos ( _rtB -> B_1036_4894_0 ) ; _rtB ->
B_1036_4896_0 = _rtB -> B_1036_4892_0 * _rtB -> B_1036_4895_0 ; _rtB ->
B_1036_4897_0 = _rtB -> B_1036_4896_0 * _rtB -> B_1036_2401_0_m ; _rtB ->
B_1036_4898_0 = muDoubleScalarSin ( _rtB -> B_1036_4894_0 ) ; _rtB ->
B_1036_4899_0 = _rtB -> B_1036_4898_0 * _rtB -> B_1036_4892_0 ; _rtB ->
B_1036_4900_0 = _rtB -> B_1036_4899_0 * _rtB -> B_1036_2402_0_a ; _rtB ->
B_1036_4901_0 = _rtB -> B_1036_4897_0 - _rtB -> B_1036_4900_0 ; _rtB ->
B_1036_4902_0 = _rtB -> B_1036_4901_0 + _rtB -> B_1036_277_0 ; _rtB ->
B_1036_4903_0 = _rtB -> B_1036_1613_0 * _rtB -> B_1036_4902_0 ; _rtB ->
B_1036_4904_0 = _rtB -> B_1036_2401_0_m * _rtB -> B_1036_4899_0 ; _rtB ->
B_1036_4905_0 = _rtB -> B_1036_2402_0_a * _rtB -> B_1036_4896_0 ; _rtB ->
B_1036_4906_0 = _rtB -> B_1036_4904_0 + _rtB -> B_1036_4905_0 ; _rtB ->
B_1036_4907_0 = _rtB -> B_1036_4906_0 + _rtB -> B_1036_321_0 ; _rtB ->
B_1036_4908_0 = _rtB -> B_1036_1617_0 * _rtB -> B_1036_4907_0 ; _rtB ->
B_1036_4909_0 = _rtB -> B_1036_4903_0 + _rtB -> B_1036_4908_0 ; _rtB ->
B_1036_4910_0 = _rtB -> B_1036_4902_0 * _rtB -> B_1036_4902_0 ; _rtB ->
B_1036_4911_0 = _rtB -> B_1036_4907_0 * _rtB -> B_1036_4907_0 ; _rtB ->
B_1036_4912_0 = _rtB -> B_1036_4910_0 + _rtB -> B_1036_4911_0 ; _rtB ->
B_1036_4913_0 = _rtB -> B_1036_4909_0 / _rtB -> B_1036_4912_0 ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
1036 , 4914 , SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_4915_0 = _rtB ->
B_1036_1617_0 * _rtB -> B_1036_4902_0 ; _rtB -> B_1036_4916_0 = _rtB ->
B_1036_1613_0 * _rtB -> B_1036_4907_0 ; _rtB -> B_1036_4917_0 = _rtB ->
B_1036_4915_0 - _rtB -> B_1036_4916_0 ; _rtB -> B_1036_4918_0 = _rtB ->
B_1036_4917_0 / _rtB -> B_1036_4912_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { ssCallAccelRunBlock ( S , 1036 , 4919 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_4920_0 = _rtB -> B_1036_4901_0 +
_rtB -> B_1036_398_0 ; _rtB -> B_1036_4921_0 = _rtB -> B_1036_1624_0 * _rtB
-> B_1036_4920_0 ; _rtB -> B_1036_4922_0 = _rtB -> B_1036_4906_0 + _rtB ->
B_1036_418_0 ; _rtB -> B_1036_4923_0 = _rtB -> B_1036_1628_0 * _rtB ->
B_1036_4922_0 ; _rtB -> B_1036_4924_0 = _rtB -> B_1036_4921_0 + _rtB ->
B_1036_4923_0 ; _rtB -> B_1036_4925_0 = _rtB -> B_1036_4920_0 * _rtB ->
B_1036_4920_0 ; _rtB -> B_1036_4926_0 = _rtB -> B_1036_4922_0 * _rtB ->
B_1036_4922_0 ; _rtB -> B_1036_4927_0 = _rtB -> B_1036_4925_0 + _rtB ->
B_1036_4926_0 ; _rtB -> B_1036_4928_0 = _rtB -> B_1036_4924_0 / _rtB ->
B_1036_4927_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
ssCallAccelRunBlock ( S , 1036 , 4929 , SS_CALL_MDL_OUTPUTS ) ; } _rtB ->
B_1036_4930_0 = _rtB -> B_1036_1628_0 * _rtB -> B_1036_4920_0 ; _rtB ->
B_1036_4931_0 = _rtB -> B_1036_1624_0 * _rtB -> B_1036_4922_0 ; _rtB ->
B_1036_4932_0 = _rtB -> B_1036_4930_0 - _rtB -> B_1036_4931_0 ; _rtB ->
B_1036_4933_0 = _rtB -> B_1036_4932_0 / _rtB -> B_1036_4927_0 ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
1036 , 4934 , SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_4935_0 = _rtB ->
B_1036_4901_0 + _rtB -> B_1036_404_0 ; _rtB -> B_1036_4936_0 = _rtB ->
B_1036_1659_0 * _rtB -> B_1036_4935_0 ; _rtB -> B_1036_4937_0 = _rtB ->
B_1036_4906_0 + _rtB -> B_1036_424_0 ; _rtB -> B_1036_4938_0 = _rtB ->
B_1036_1663_0 * _rtB -> B_1036_4937_0 ; _rtB -> B_1036_4939_0 = _rtB ->
B_1036_4936_0 + _rtB -> B_1036_4938_0 ; _rtB -> B_1036_4940_0 = _rtB ->
B_1036_4935_0 * _rtB -> B_1036_4935_0 ; _rtB -> B_1036_4941_0 = _rtB ->
B_1036_4937_0 * _rtB -> B_1036_4937_0 ; _rtB -> B_1036_4942_0 = _rtB ->
B_1036_4940_0 + _rtB -> B_1036_4941_0 ; _rtB -> B_1036_4943_0 = _rtB ->
B_1036_4939_0 / _rtB -> B_1036_4942_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { ssCallAccelRunBlock ( S , 1036 , 4944 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_4945_0 = _rtB -> B_1036_1663_0 *
_rtB -> B_1036_4935_0 ; _rtB -> B_1036_4946_0 = _rtB -> B_1036_1659_0 * _rtB
-> B_1036_4937_0 ; _rtB -> B_1036_4947_0 = _rtB -> B_1036_4945_0 - _rtB ->
B_1036_4946_0 ; _rtB -> B_1036_4948_0 = _rtB -> B_1036_4947_0 / _rtB ->
B_1036_4942_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
ssCallAccelRunBlock ( S , 1036 , 4949 , SS_CALL_MDL_OUTPUTS ) ; } _rtB ->
B_1036_4950_0 = muDoubleScalarSin ( _rtP -> P_804 * ssGetTaskTime ( S , 0 ) +
_rtP -> P_805 ) * _rtP -> P_802 + _rtP -> P_803 ; _rtB -> B_1036_4951_0 =
_rtB -> B_1036_1601_0 [ 0 ] * _rtB -> B_1036_4950_0 ; _rtB -> B_1036_4952_0 =
muDoubleScalarSin ( _rtP -> P_808 * ssGetTaskTime ( S , 0 ) + _rtP -> P_809 )
* _rtP -> P_806 + _rtP -> P_807 ; _rtB -> B_1036_4953_0 = _rtB ->
B_1036_1601_0 [ 0 ] * _rtB -> B_1036_4952_0 ; _rtB -> B_1036_4954_0 =
muDoubleScalarSin ( _rtP -> P_812 * ssGetTaskTime ( S , 0 ) + _rtP -> P_813 )
* _rtP -> P_810 + _rtP -> P_811 ; _rtB -> B_1036_4955_0 = _rtB ->
B_1036_1601_0 [ 1 ] * _rtB -> B_1036_4954_0 ; _rtB -> B_1036_4956_0 =
muDoubleScalarSin ( _rtP -> P_816 * ssGetTaskTime ( S , 0 ) + _rtP -> P_817 )
* _rtP -> P_814 + _rtP -> P_815 ; _rtB -> B_1036_4957_0 = _rtB ->
B_1036_1601_0 [ 1 ] * _rtB -> B_1036_4956_0 ; _rtB -> B_1036_4958_0 =
muDoubleScalarSin ( _rtP -> P_820 * ssGetTaskTime ( S , 0 ) + _rtP -> P_821 )
* _rtP -> P_818 + _rtP -> P_819 ; _rtB -> B_1036_4959_0 = _rtB ->
B_1036_1601_0 [ 2 ] * _rtB -> B_1036_4958_0 ; _rtB -> B_1036_4960_0 =
muDoubleScalarSin ( _rtP -> P_824 * ssGetTaskTime ( S , 0 ) + _rtP -> P_825 )
* _rtP -> P_822 + _rtP -> P_823 ; _rtB -> B_1036_4961_0 = _rtB ->
B_1036_1601_0 [ 2 ] * _rtB -> B_1036_4960_0 ; _rtB -> B_1036_4962_0 =
muDoubleScalarSin ( _rtP -> P_828 * ssGetTaskTime ( S , 0 ) + _rtP -> P_829 )
* _rtP -> P_826 + _rtP -> P_827 ; _rtB -> B_1036_4963_0 = 0.0 * _rtB ->
B_1036_4962_0 ; _rtB -> B_1036_4964_0 = muDoubleScalarSin ( _rtP -> P_832 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_833 ) * _rtP -> P_830 + _rtP -> P_831 ;
_rtB -> B_1036_4965_0 = 0.0 * _rtB -> B_1036_4964_0 ; _rtB -> B_1036_4966_0 =
muDoubleScalarSin ( _rtP -> P_836 * ssGetTaskTime ( S , 0 ) + _rtP -> P_837 )
* _rtP -> P_834 + _rtP -> P_835 ; _rtB -> B_1036_4967_0 = 0.0 * _rtB ->
B_1036_4966_0 ; _rtB -> B_1036_4968_0 = muDoubleScalarSin ( _rtP -> P_840 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_841 ) * _rtP -> P_838 + _rtP -> P_839 ;
_rtB -> B_1036_4969_0 = 0.0 * _rtB -> B_1036_4968_0 ; _rtB -> B_1036_4970_0 =
muDoubleScalarSin ( _rtP -> P_844 * ssGetTaskTime ( S , 0 ) + _rtP -> P_845 )
* _rtP -> P_842 + _rtP -> P_843 ; _rtB -> B_1036_4971_0 = _rtB ->
B_1036_1606_0 [ 0 ] * _rtB -> B_1036_4970_0 ; _rtB -> B_1036_4972_0 =
muDoubleScalarSin ( _rtP -> P_848 * ssGetTaskTime ( S , 0 ) + _rtP -> P_849 )
* _rtP -> P_846 + _rtP -> P_847 ; _rtB -> B_1036_4973_0 = _rtB ->
B_1036_1606_0 [ 0 ] * _rtB -> B_1036_4972_0 ; _rtB -> B_1036_4974_0 =
muDoubleScalarSin ( _rtP -> P_852 * ssGetTaskTime ( S , 0 ) + _rtP -> P_853 )
* _rtP -> P_850 + _rtP -> P_851 ; _rtB -> B_1036_4975_0 = _rtB ->
B_1036_1606_0 [ 0 ] * _rtB -> B_1036_4974_0 ; _rtB -> B_1036_4976_0 =
muDoubleScalarSin ( _rtP -> P_856 * ssGetTaskTime ( S , 0 ) + _rtP -> P_857 )
* _rtP -> P_854 + _rtP -> P_855 ; _rtB -> B_1036_4977_0 = _rtB ->
B_1036_1606_0 [ 0 ] * _rtB -> B_1036_4976_0 ; _rtB -> B_1036_4978_0 =
muDoubleScalarSin ( _rtP -> P_860 * ssGetTaskTime ( S , 0 ) + _rtP -> P_861 )
* _rtP -> P_858 + _rtP -> P_859 ; _rtB -> B_1036_4979_0 = _rtB ->
B_1036_1606_0 [ 0 ] * _rtB -> B_1036_4978_0 ; _rtB -> B_1036_4980_0 =
muDoubleScalarSin ( _rtP -> P_864 * ssGetTaskTime ( S , 0 ) + _rtP -> P_865 )
* _rtP -> P_862 + _rtP -> P_863 ; _rtB -> B_1036_4981_0 = _rtB ->
B_1036_1606_0 [ 0 ] * _rtB -> B_1036_4980_0 ; _rtB -> B_1036_4982_0 =
muDoubleScalarSin ( _rtP -> P_868 * ssGetTaskTime ( S , 0 ) + _rtP -> P_869 )
* _rtP -> P_866 + _rtP -> P_867 ; _rtB -> B_1036_4983_0 = _rtB ->
B_1036_1606_0 [ 1 ] * _rtB -> B_1036_4982_0 ; _rtB -> B_1036_4984_0 =
muDoubleScalarSin ( _rtP -> P_872 * ssGetTaskTime ( S , 0 ) + _rtP -> P_873 )
* _rtP -> P_870 + _rtP -> P_871 ; _rtB -> B_1036_4985_0 = _rtB ->
B_1036_1606_0 [ 1 ] * _rtB -> B_1036_4984_0 ; _rtB -> B_1036_4986_0 =
muDoubleScalarSin ( _rtP -> P_876 * ssGetTaskTime ( S , 0 ) + _rtP -> P_877 )
* _rtP -> P_874 + _rtP -> P_875 ; _rtB -> B_1036_4987_0 = _rtB ->
B_1036_1606_0 [ 1 ] * _rtB -> B_1036_4986_0 ; _rtB -> B_1036_4988_0 =
muDoubleScalarSin ( _rtP -> P_880 * ssGetTaskTime ( S , 0 ) + _rtP -> P_881 )
* _rtP -> P_878 + _rtP -> P_879 ; _rtB -> B_1036_4989_0 = _rtB ->
B_1036_1606_0 [ 1 ] * _rtB -> B_1036_4988_0 ; _rtB -> B_1036_4990_0 =
muDoubleScalarSin ( _rtP -> P_884 * ssGetTaskTime ( S , 0 ) + _rtP -> P_885 )
* _rtP -> P_882 + _rtP -> P_883 ; _rtB -> B_1036_4991_0 = _rtB ->
B_1036_1606_0 [ 1 ] * _rtB -> B_1036_4990_0 ; _rtB -> B_1036_4992_0 =
muDoubleScalarSin ( _rtP -> P_888 * ssGetTaskTime ( S , 0 ) + _rtP -> P_889 )
* _rtP -> P_886 + _rtP -> P_887 ; _rtB -> B_1036_4993_0 = _rtB ->
B_1036_1606_0 [ 1 ] * _rtB -> B_1036_4992_0 ; _rtB -> B_1036_4994_0 =
muDoubleScalarSin ( _rtP -> P_892 * ssGetTaskTime ( S , 0 ) + _rtP -> P_893 )
* _rtP -> P_890 + _rtP -> P_891 ; _rtB -> B_1036_4995_0 = _rtB ->
B_1036_1606_0 [ 2 ] * _rtB -> B_1036_4994_0 ; _rtB -> B_1036_4996_0 =
muDoubleScalarSin ( _rtP -> P_896 * ssGetTaskTime ( S , 0 ) + _rtP -> P_897 )
* _rtP -> P_894 + _rtP -> P_895 ; _rtB -> B_1036_4997_0 = _rtB ->
B_1036_1606_0 [ 2 ] * _rtB -> B_1036_4996_0 ; _rtB -> B_1036_4998_0 =
muDoubleScalarSin ( _rtP -> P_900 * ssGetTaskTime ( S , 0 ) + _rtP -> P_901 )
* _rtP -> P_898 + _rtP -> P_899 ; _rtB -> B_1036_4999_0 = _rtB ->
B_1036_1606_0 [ 2 ] * _rtB -> B_1036_4998_0 ; _rtB -> B_1036_5000_0 =
muDoubleScalarSin ( _rtP -> P_904 * ssGetTaskTime ( S , 0 ) + _rtP -> P_905 )
* _rtP -> P_902 + _rtP -> P_903 ; _rtB -> B_1036_5001_0 = _rtB ->
B_1036_1606_0 [ 2 ] * _rtB -> B_1036_5000_0 ; _rtB -> B_1036_5002_0 =
muDoubleScalarSin ( _rtP -> P_908 * ssGetTaskTime ( S , 0 ) + _rtP -> P_909 )
* _rtP -> P_906 + _rtP -> P_907 ; _rtB -> B_1036_5003_0 = _rtB ->
B_1036_1606_0 [ 2 ] * _rtB -> B_1036_5002_0 ; _rtB -> B_1036_5004_0 =
muDoubleScalarSin ( _rtP -> P_912 * ssGetTaskTime ( S , 0 ) + _rtP -> P_913 )
* _rtP -> P_910 + _rtP -> P_911 ; _rtB -> B_1036_5005_0 = _rtB ->
B_1036_1606_0 [ 2 ] * _rtB -> B_1036_5004_0 ; _rtB -> B_1036_5006_0 = _rtX ->
integrator_CSTATE_oh ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_n2 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_n2 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_915 ; _rtB
-> B_1036_5007_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_g1 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_g1 . Last , _rtDW -> TransportDelay_IWORK_g1 .
Tail , _rtDW -> TransportDelay_IWORK_g1 . Head , _rtP -> P_916 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_5008_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5009_0 =
_rtP -> P_917 ; } _rtB -> B_1036_5010_0 = ( _rtB -> B_1036_5008_0 >= _rtB ->
B_1036_5009_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_5011_0 = _rtDW -> Memory_PreviousInput_ep ; } if ( _rtB ->
B_1036_5010_0 ) { _rtB -> B_986_0_0 = _rtB -> B_1036_5006_0 - _rtB ->
B_1036_5007_0 ; _rtB -> B_986_1_0 = _rtP -> P_90 * _rtB -> B_986_0_0 ; _rtB
-> B_1036_5013_0 = _rtB -> B_986_1_0 ; } else { _rtB -> B_1036_5013_0 = _rtB
-> B_1036_5011_0 ; } _rtB -> B_1036_5014_0 = _rtX -> integrator_CSTATE_iu ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_c .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_c . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_920 ; _rtB -> B_1036_5015_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_ak . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ak . Last , _rtDW -> TransportDelay_IWORK_ak . Tail ,
_rtDW -> TransportDelay_IWORK_ak . Head , _rtP -> P_921 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_5016_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5017_0 = _rtP -> P_922 ; } _rtB ->
B_1036_5018_0 = ( _rtB -> B_1036_5016_0 >= _rtB -> B_1036_5017_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5019_0 =
_rtDW -> Memory_PreviousInput_fo ; } if ( _rtB -> B_1036_5018_0 ) { _rtB ->
B_985_0_0 = _rtB -> B_1036_5014_0 - _rtB -> B_1036_5015_0 ; _rtB -> B_985_1_0
= _rtP -> P_89 * _rtB -> B_985_0_0 ; _rtB -> B_1036_5021_0 = _rtB ->
B_985_1_0 ; } else { _rtB -> B_1036_5021_0 = _rtB -> B_1036_5019_0 ; } _rtB
-> B_1036_5022_0 . re = _rtB -> B_1036_5013_0 ; _rtB -> B_1036_5022_0 . im =
_rtB -> B_1036_5021_0 ; _rtB -> B_1036_5023_0 = muDoubleScalarHypot ( _rtB ->
B_1036_5022_0 . re , _rtB -> B_1036_5022_0 . im ) ; _rtB -> B_1036_5023_1 =
muDoubleScalarAtan2 ( _rtB -> B_1036_5022_0 . im , _rtB -> B_1036_5022_0 . re
) ; _rtB -> B_1036_5024_0 = _rtP -> P_924 * _rtB -> B_1036_5023_1 ; _rtB ->
B_1036_5025_0 = _rtP -> P_925 * _rtB -> B_1036_5024_0 ; maxV = _rtB ->
B_1036_5025_0 ; u0 = _rtB -> B_1036_5023_0 ; u2 = muDoubleScalarCos ( maxV )
; maxV = muDoubleScalarSin ( maxV ) ; _rtB -> B_1036_5026_0 . re = u0 * u2 ;
_rtB -> B_1036_5026_0 . im = u0 * maxV ; _rtB -> B_1036_5027_0 = _rtX ->
integrator_CSTATE_lt ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_l3 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_l3 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_927 ; _rtB
-> B_1036_5028_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_h . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_h . Last , _rtDW -> TransportDelay_IWORK_h .
Tail , _rtDW -> TransportDelay_IWORK_h . Head , _rtP -> P_928 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_5029_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5030_0 =
_rtP -> P_929 ; } _rtB -> B_1036_5031_0 = ( _rtB -> B_1036_5029_0 >= _rtB ->
B_1036_5030_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_5032_0 = _rtDW -> Memory_PreviousInput_lz ; } if ( _rtB ->
B_1036_5031_0 ) { _rtB -> B_988_0_0 = _rtB -> B_1036_5027_0 - _rtB ->
B_1036_5028_0 ; _rtB -> B_988_1_0 = _rtP -> P_92 * _rtB -> B_988_0_0 ; _rtB
-> B_1036_5034_0 = _rtB -> B_988_1_0 ; } else { _rtB -> B_1036_5034_0 = _rtB
-> B_1036_5032_0 ; } _rtB -> B_1036_5035_0 = _rtX -> integrator_CSTATE_a3 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hm .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_hm . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_932 ; _rtB -> B_1036_5036_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_ky . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ky . Last , _rtDW -> TransportDelay_IWORK_ky . Tail ,
_rtDW -> TransportDelay_IWORK_ky . Head , _rtP -> P_933 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1036_5037_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5038_0 = _rtP -> P_934 ; } _rtB ->
B_1036_5039_0 = ( _rtB -> B_1036_5037_0 >= _rtB -> B_1036_5038_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5040_0 =
_rtDW -> Memory_PreviousInput_fs ; } if ( _rtB -> B_1036_5039_0 ) { _rtB ->
B_987_0_0 = _rtB -> B_1036_5035_0 - _rtB -> B_1036_5036_0 ; _rtB -> B_987_1_0
= _rtP -> P_91 * _rtB -> B_987_0_0 ; _rtB -> B_1036_5042_0 = _rtB ->
B_987_1_0 ; } else { _rtB -> B_1036_5042_0 = _rtB -> B_1036_5040_0 ; } _rtB
-> B_1036_5043_0 . re = _rtB -> B_1036_5034_0 ; _rtB -> B_1036_5043_0 . im =
_rtB -> B_1036_5042_0 ; _rtB -> B_1036_5044_0 = muDoubleScalarHypot ( _rtB ->
B_1036_5043_0 . re , _rtB -> B_1036_5043_0 . im ) ; _rtB -> B_1036_5044_1 =
muDoubleScalarAtan2 ( _rtB -> B_1036_5043_0 . im , _rtB -> B_1036_5043_0 . re
) ; _rtB -> B_1036_5045_0 = _rtP -> P_936 * _rtB -> B_1036_5044_1 ; _rtB ->
B_1036_5046_0 = _rtP -> P_937 * _rtB -> B_1036_5045_0 ; maxV = _rtB ->
B_1036_5046_0 ; u0 = _rtB -> B_1036_5044_0 ; u2 = muDoubleScalarCos ( maxV )
; maxV = muDoubleScalarSin ( maxV ) ; _rtB -> B_1036_5047_0 . re = u0 * u2 ;
_rtB -> B_1036_5047_0 . im = u0 * maxV ; _rtB -> B_1036_5048_0 = _rtX ->
integrator_CSTATE_on ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_k0 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_k0 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_939 ; _rtB
-> B_1036_5049_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_cj . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_cj . Last , _rtDW -> TransportDelay_IWORK_cj .
Tail , _rtDW -> TransportDelay_IWORK_cj . Head , _rtP -> P_940 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_5050_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5051_0 =
_rtP -> P_941 ; } _rtB -> B_1036_5052_0 = ( _rtB -> B_1036_5050_0 >= _rtB ->
B_1036_5051_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_5053_0 = _rtDW -> Memory_PreviousInput_pp ; } if ( _rtB ->
B_1036_5052_0 ) { _rtB -> B_990_0_0 = _rtB -> B_1036_5048_0 - _rtB ->
B_1036_5049_0 ; _rtB -> B_990_1_0 = _rtP -> P_94 * _rtB -> B_990_0_0 ; _rtB
-> B_1036_5055_0 = _rtB -> B_990_1_0 ; } else { _rtB -> B_1036_5055_0 = _rtB
-> B_1036_5053_0 ; } _rtB -> B_1036_5056_0 = _rtX -> integrator_CSTATE_pn ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_gd .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_gd . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_944 ; _rtB -> B_1036_5057_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_oor . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_oor . Last , _rtDW -> TransportDelay_IWORK_oor . Tail ,
_rtDW -> TransportDelay_IWORK_oor . Head , _rtP -> P_945 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } _rtB -> B_1036_5058_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5059_0 = _rtP -> P_946 ; } _rtB ->
B_1036_5060_0 = ( _rtB -> B_1036_5058_0 >= _rtB -> B_1036_5059_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5061_0 =
_rtDW -> Memory_PreviousInput_j0 ; } if ( _rtB -> B_1036_5060_0 ) { _rtB ->
B_989_0_0 = _rtB -> B_1036_5056_0 - _rtB -> B_1036_5057_0 ; _rtB -> B_989_1_0
= _rtP -> P_93 * _rtB -> B_989_0_0 ; _rtB -> B_1036_5063_0 = _rtB ->
B_989_1_0 ; } else { _rtB -> B_1036_5063_0 = _rtB -> B_1036_5061_0 ; } _rtB
-> B_1036_5064_0 . re = _rtB -> B_1036_5055_0 ; _rtB -> B_1036_5064_0 . im =
_rtB -> B_1036_5063_0 ; _rtB -> B_1036_5065_0 = muDoubleScalarHypot ( _rtB ->
B_1036_5064_0 . re , _rtB -> B_1036_5064_0 . im ) ; _rtB -> B_1036_5065_1 =
muDoubleScalarAtan2 ( _rtB -> B_1036_5064_0 . im , _rtB -> B_1036_5064_0 . re
) ; _rtB -> B_1036_5066_0 = _rtP -> P_948 * _rtB -> B_1036_5065_1 ; _rtB ->
B_1036_5067_0 = _rtP -> P_949 * _rtB -> B_1036_5066_0 ; maxV = _rtB ->
B_1036_5067_0 ; u0 = _rtB -> B_1036_5065_0 ; u2 = muDoubleScalarCos ( maxV )
; maxV = muDoubleScalarSin ( maxV ) ; _rtB -> B_1036_5068_0 . re = u0 * u2 ;
_rtB -> B_1036_5068_0 . im = u0 * maxV ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_5069_0 = _rtB -> B_1036_2406_0_k ; }
DZG_v3_NegSeqComputation ( S , _rtB -> B_1036_5069_0 , _rtB -> B_1036_5026_0
, _rtB -> B_1036_5047_0 , _rtB -> B_1036_5068_0 , & _rtB ->
PosSeqComputation_o , & _rtDW -> PosSeqComputation_o , & _rtP ->
PosSeqComputation_o ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_1036_5071_0 = _rtB -> B_1036_2407_0 ; }
DZG_v3_NegSeqComputation ( S , _rtB -> B_1036_5071_0 , _rtB -> B_1036_5026_0
, _rtB -> B_1036_5047_0 , _rtB -> B_1036_5068_0 , & _rtB ->
NegSeqComputation_d , & _rtDW -> NegSeqComputation_d , & _rtP ->
NegSeqComputation_d ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_1036_5073_0 = _rtB -> B_1036_2408_0_h ; }
DZG_v3_ZeroSeqComputation ( S , _rtB -> B_1036_5073_0 , _rtB -> B_1036_5026_0
, _rtB -> B_1036_5047_0 , _rtB -> B_1036_5068_0 , & _rtB ->
ZeroSeqComputation_m , & _rtDW -> ZeroSeqComputation_m , & _rtP ->
ZeroSeqComputation_m ) ; _rtB -> B_1036_5075_0 [ 0 ] = muDoubleScalarHypot (
_rtB -> PosSeqComputation_o . B_982_2_0 . re , _rtB -> PosSeqComputation_o .
B_982_2_0 . im ) ; _rtB -> B_1036_5075_0 [ 1 ] = muDoubleScalarHypot ( _rtB
-> NegSeqComputation_d . B_982_2_0 . re , _rtB -> NegSeqComputation_d .
B_982_2_0 . im ) ; _rtB -> B_1036_5075_0 [ 2 ] = muDoubleScalarHypot ( _rtB
-> ZeroSeqComputation_m . B_984_1_0 . re , _rtB -> ZeroSeqComputation_m .
B_984_1_0 . im ) ; _rtB -> B_1036_5075_1 [ 0 ] = muDoubleScalarAtan2 ( _rtB
-> PosSeqComputation_o . B_982_2_0 . im , _rtB -> PosSeqComputation_o .
B_982_2_0 . re ) ; _rtB -> B_1036_5075_1 [ 1 ] = muDoubleScalarAtan2 ( _rtB
-> NegSeqComputation_d . B_982_2_0 . im , _rtB -> NegSeqComputation_d .
B_982_2_0 . re ) ; _rtB -> B_1036_5075_1 [ 2 ] = muDoubleScalarAtan2 ( _rtB
-> ZeroSeqComputation_m . B_984_1_0 . im , _rtB -> ZeroSeqComputation_m .
B_984_1_0 . re ) ; _rtB -> B_1036_5076_0 = _rtP -> P_950 * _rtB ->
B_1036_5075_1 [ 2 ] ; _rtB -> B_1036_5077_0 = _rtP -> P_951 * _rtB ->
B_1036_5076_0 ; _rtB -> B_1036_5078_0 = muDoubleScalarSin ( _rtB ->
B_1036_5077_0 ) ; _rtB -> B_1036_5079_0 = muDoubleScalarCos ( _rtB ->
B_1036_5077_0 ) ; _rtB -> B_1036_5080_0 = _rtP -> P_952 * _rtB ->
B_1036_5075_0 [ 2 ] ; _rtB -> B_1036_5081_0 = _rtB -> B_1036_5078_0 * _rtB ->
B_1036_5080_0 ; _rtB -> B_1036_5082_0 = _rtB -> B_1036_5080_0 * _rtB ->
B_1036_5079_0 ; _rtB -> B_1036_5083_0 = muDoubleScalarSin ( _rtP -> P_955 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_956 ) * _rtP -> P_953 + _rtP -> P_954 ;
_rtB -> B_1036_5084_0 = _rtB -> B_1036_1601_0 [ 0 ] * _rtB -> B_1036_5083_0 ;
_rtB -> B_1036_5085_0 = muDoubleScalarSin ( _rtP -> P_959 * ssGetTaskTime ( S
, 0 ) + _rtP -> P_960 ) * _rtP -> P_957 + _rtP -> P_958 ; _rtB ->
B_1036_5086_0 = _rtB -> B_1036_1601_0 [ 0 ] * _rtB -> B_1036_5085_0 ; _rtB ->
B_1036_5087_0 = muDoubleScalarSin ( _rtP -> P_963 * ssGetTaskTime ( S , 0 ) +
_rtP -> P_964 ) * _rtP -> P_961 + _rtP -> P_962 ; _rtB -> B_1036_5088_0 =
_rtB -> B_1036_1601_0 [ 1 ] * _rtB -> B_1036_5087_0 ; _rtB -> B_1036_5089_0 =
muDoubleScalarSin ( _rtP -> P_967 * ssGetTaskTime ( S , 0 ) + _rtP -> P_968 )
* _rtP -> P_965 + _rtP -> P_966 ; _rtB -> B_1036_5090_0 = _rtB ->
B_1036_1601_0 [ 1 ] * _rtB -> B_1036_5089_0 ; _rtB -> B_1036_5091_0 =
muDoubleScalarSin ( _rtP -> P_971 * ssGetTaskTime ( S , 0 ) + _rtP -> P_972 )
* _rtP -> P_969 + _rtP -> P_970 ; _rtB -> B_1036_5092_0 = _rtB ->
B_1036_1601_0 [ 2 ] * _rtB -> B_1036_5091_0 ; _rtB -> B_1036_5093_0 =
muDoubleScalarSin ( _rtP -> P_975 * ssGetTaskTime ( S , 0 ) + _rtP -> P_976 )
* _rtP -> P_973 + _rtP -> P_974 ; _rtB -> B_1036_5094_0 = _rtB ->
B_1036_1601_0 [ 2 ] * _rtB -> B_1036_5093_0 ; _rtB -> B_1036_5095_0 = _rtB ->
B_1036_5082_0 * _rtB -> B_1036_4896_0 ; _rtB -> B_1036_5096_0 = _rtB ->
B_1036_5081_0 * _rtB -> B_1036_4899_0 ; _rtB -> B_1036_5097_0 = _rtB ->
B_1036_5095_0 + _rtB -> B_1036_5096_0 ; _rtB -> B_1036_5098_0 = _rtB ->
B_1036_4896_0 * _rtB -> B_1036_4896_0 ; _rtB -> B_1036_5099_0 = _rtB ->
B_1036_4899_0 * _rtB -> B_1036_4899_0 ; _rtB -> B_1036_5100_0 = _rtB ->
B_1036_5098_0 + _rtB -> B_1036_5099_0 ; _rtB -> B_1036_5101_0 = _rtB ->
B_1036_5081_0 * _rtB -> B_1036_4896_0 ; _rtB -> B_1036_5102_0 = _rtB ->
B_1036_5082_0 * _rtB -> B_1036_4899_0 ; _rtB -> B_1036_5103_0 = _rtB ->
B_1036_5101_0 - _rtB -> B_1036_5102_0 ; _rtB -> B_1036_5104_0 = _rtB ->
B_1036_5097_0 / _rtB -> B_1036_5100_0 ; _rtB -> B_1036_5105_0 = _rtB ->
B_1036_5103_0 / _rtB -> B_1036_5100_0 ; _rtB -> B_1036_5106_0 =
muDoubleScalarSin ( _rtP -> P_979 * ssGetTaskTime ( S , 0 ) + _rtP -> P_980 )
* _rtP -> P_977 + _rtP -> P_978 ; _rtB -> B_1036_5107_0 = _rtB ->
B_1036_1606_0 [ 0 ] * _rtB -> B_1036_5106_0 ; _rtB -> B_1036_5108_0 =
muDoubleScalarSin ( _rtP -> P_983 * ssGetTaskTime ( S , 0 ) + _rtP -> P_984 )
* _rtP -> P_981 + _rtP -> P_982 ; _rtB -> B_1036_5109_0 = _rtB ->
B_1036_1606_0 [ 0 ] * _rtB -> B_1036_5108_0 ; _rtB -> B_1036_5110_0 =
muDoubleScalarSin ( _rtP -> P_987 * ssGetTaskTime ( S , 0 ) + _rtP -> P_988 )
* _rtP -> P_985 + _rtP -> P_986 ; _rtB -> B_1036_5111_0 = _rtB ->
B_1036_1606_0 [ 1 ] * _rtB -> B_1036_5110_0 ; _rtB -> B_1036_5112_0 =
muDoubleScalarSin ( _rtP -> P_991 * ssGetTaskTime ( S , 0 ) + _rtP -> P_992 )
* _rtP -> P_989 + _rtP -> P_990 ; _rtB -> B_1036_5113_0 = _rtB ->
B_1036_1606_0 [ 1 ] * _rtB -> B_1036_5112_0 ; _rtB -> B_1036_5114_0 =
muDoubleScalarSin ( _rtP -> P_995 * ssGetTaskTime ( S , 0 ) + _rtP -> P_996 )
* _rtP -> P_993 + _rtP -> P_994 ; _rtB -> B_1036_5115_0 = _rtB ->
B_1036_1606_0 [ 2 ] * _rtB -> B_1036_5114_0 ; _rtB -> B_1036_5116_0 =
muDoubleScalarSin ( _rtP -> P_999 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1000
) * _rtP -> P_997 + _rtP -> P_998 ; _rtB -> B_1036_5117_0 = _rtB ->
B_1036_1606_0 [ 2 ] * _rtB -> B_1036_5116_0 ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S , 1036 , 5118 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_5119_0 = muDoubleScalarSin ( _rtB ->
B_1036_662_0 [ 0 ] ) ; _rtB -> B_1036_5120_0 = muDoubleScalarCos ( _rtB ->
B_1036_662_0 [ 0 ] ) ; _rtB -> B_1036_5121_0 = _rtB -> B_1036_5119_0 * _rtB
-> B_1036_650_0 [ 0 ] ; _rtB -> B_1036_5122_0 = _rtB -> B_1036_650_0 [ 0 ] *
_rtB -> B_1036_5120_0 ; _rtB -> B_1036_5123_0 = muDoubleScalarSin ( _rtB ->
B_1036_662_0 [ 7 ] ) ; _rtB -> B_1036_5124_0 = muDoubleScalarCos ( _rtB ->
B_1036_662_0 [ 7 ] ) ; _rtB -> B_1036_5125_0 = _rtB -> B_1036_5123_0 * _rtB
-> B_1036_650_0 [ 7 ] ; _rtB -> B_1036_5126_0 = _rtB -> B_1036_650_0 [ 7 ] *
_rtB -> B_1036_5124_0 ; _rtB -> B_1036_5127_0 = muDoubleScalarSin ( _rtB ->
B_1036_662_0 [ 1 ] ) ; _rtB -> B_1036_5128_0 = muDoubleScalarCos ( _rtB ->
B_1036_662_0 [ 1 ] ) ; _rtB -> B_1036_5129_0 = _rtB -> B_1036_5127_0 * _rtB
-> B_1036_650_0 [ 1 ] ; _rtB -> B_1036_5130_0 = _rtB -> B_1036_650_0 [ 1 ] *
_rtB -> B_1036_5128_0 ; _rtB -> B_1036_5131_0 = muDoubleScalarSin ( _rtB ->
B_1036_662_0 [ 8 ] ) ; _rtB -> B_1036_5132_0 = muDoubleScalarCos ( _rtB ->
B_1036_662_0 [ 8 ] ) ; _rtB -> B_1036_5133_0 = _rtB -> B_1036_5131_0 * _rtB
-> B_1036_650_0 [ 8 ] ; _rtB -> B_1036_5134_0 = _rtB -> B_1036_650_0 [ 8 ] *
_rtB -> B_1036_5132_0 ; _rtB -> B_1036_5135_0 = muDoubleScalarSin ( _rtB ->
B_1036_662_0 [ 2 ] ) ; _rtB -> B_1036_5136_0 = muDoubleScalarCos ( _rtB ->
B_1036_662_0 [ 2 ] ) ; _rtB -> B_1036_5137_0 = _rtB -> B_1036_5135_0 * _rtB
-> B_1036_650_0 [ 2 ] ; _rtB -> B_1036_5138_0 = _rtB -> B_1036_650_0 [ 2 ] *
_rtB -> B_1036_5136_0 ; _rtB -> B_1036_5139_0 = muDoubleScalarSin ( _rtB ->
B_1036_662_0 [ 9 ] ) ; _rtB -> B_1036_5140_0 = muDoubleScalarCos ( _rtB ->
B_1036_662_0 [ 9 ] ) ; _rtB -> B_1036_5141_0 = _rtB -> B_1036_5139_0 * _rtB
-> B_1036_650_0 [ 9 ] ; _rtB -> B_1036_5142_0 = _rtB -> B_1036_650_0 [ 9 ] *
_rtB -> B_1036_5140_0 ; _rtB -> B_1036_5143_0 = _rtB -> B_1036_5122_0 - _rtB
-> B_1036_5130_0 ; _rtB -> B_1036_5144_0 = _rtB -> B_1036_5121_0 - _rtB ->
B_1036_5129_0 ; _rtB -> B_1036_5145_0 = _rtB -> B_1036_713_0 - _rtB ->
B_1036_664_0 ; _rtB -> B_1036_5146_0 = _rtB -> B_1036_735_0 - _rtB ->
B_1036_724_0 ; _rtB -> B_1036_5147_0 = _rtB -> B_1036_664_0 - _rtB ->
B_1036_708_0 ; _rtB -> B_1036_5148_0 = _rtB -> B_1036_724_0 - _rtB ->
B_1036_730_0 ; _rtB -> B_1036_5149_0 = _rtB -> B_1036_5130_0 - _rtB ->
B_1036_5138_0 ; _rtB -> B_1036_5150_0 = _rtB -> B_1036_5129_0 - _rtB ->
B_1036_5137_0 ; _rtB -> B_1036_5151_0 = _rtB -> B_1036_708_0 - _rtB ->
B_1036_713_0 ; _rtB -> B_1036_5152_0 = _rtB -> B_1036_730_0 - _rtB ->
B_1036_735_0 ; _rtB -> B_1036_5153_0 = _rtB -> B_1036_5138_0 - _rtB ->
B_1036_5122_0 ; _rtB -> B_1036_5154_0 = _rtB -> B_1036_5137_0 - _rtB ->
B_1036_5121_0 ; _rtB -> B_1036_5155_0 = _rtB -> B_1036_5149_0 * _rtB ->
B_1036_5151_0 ; _rtB -> B_1036_5156_0 = _rtB -> B_1036_5150_0 * _rtB ->
B_1036_5152_0 ; _rtB -> B_1036_5157_0 = _rtB -> B_1036_5155_0 + _rtB ->
B_1036_5156_0 ; _rtB -> B_1036_5158_0 = _rtB -> B_1036_5151_0 * _rtB ->
B_1036_5151_0 ; _rtB -> B_1036_5159_0 = _rtB -> B_1036_5152_0 * _rtB ->
B_1036_5152_0 ; _rtB -> B_1036_5160_0 = _rtB -> B_1036_5158_0 + _rtB ->
B_1036_5159_0 ; _rtB -> B_1036_5161_0 = _rtB -> B_1036_5150_0 * _rtB ->
B_1036_5151_0 ; _rtB -> B_1036_5162_0 = _rtB -> B_1036_5149_0 * _rtB ->
B_1036_5152_0 ; _rtB -> B_1036_5163_0 = _rtB -> B_1036_5161_0 - _rtB ->
B_1036_5162_0 ; _rtB -> B_1036_5164_0 = _rtB -> B_1036_5157_0 / _rtB ->
B_1036_5160_0 ; _rtB -> B_1036_5165_0 = _rtB -> B_1036_5163_0 / _rtB ->
B_1036_5160_0 ; _rtB -> B_1036_5166_0 = _rtB -> B_1036_5143_0 * _rtB ->
B_1036_5147_0 ; _rtB -> B_1036_5167_0 = _rtB -> B_1036_5144_0 * _rtB ->
B_1036_5148_0 ; _rtB -> B_1036_5168_0 = _rtB -> B_1036_5166_0 + _rtB ->
B_1036_5167_0 ; _rtB -> B_1036_5169_0 = _rtB -> B_1036_5147_0 * _rtB ->
B_1036_5147_0 ; _rtB -> B_1036_5170_0 = _rtB -> B_1036_5148_0 * _rtB ->
B_1036_5148_0 ; _rtB -> B_1036_5171_0 = _rtB -> B_1036_5169_0 + _rtB ->
B_1036_5170_0 ; _rtB -> B_1036_5172_0 = _rtB -> B_1036_5144_0 * _rtB ->
B_1036_5147_0 ; _rtB -> B_1036_5173_0 = _rtB -> B_1036_5143_0 * _rtB ->
B_1036_5148_0 ; _rtB -> B_1036_5174_0 = _rtB -> B_1036_5172_0 - _rtB ->
B_1036_5173_0 ; _rtB -> B_1036_5175_0 = _rtB -> B_1036_5168_0 / _rtB ->
B_1036_5171_0 ; _rtB -> B_1036_5176_0 = _rtB -> B_1036_5174_0 / _rtB ->
B_1036_5171_0 ; _rtB -> B_1036_5177_0 = _rtB -> B_1036_5153_0 * _rtB ->
B_1036_5145_0 ; _rtB -> B_1036_5178_0 = _rtB -> B_1036_5154_0 * _rtB ->
B_1036_5146_0 ; _rtB -> B_1036_5179_0 = _rtB -> B_1036_5177_0 + _rtB ->
B_1036_5178_0 ; _rtB -> B_1036_5180_0 = _rtB -> B_1036_5145_0 * _rtB ->
B_1036_5145_0 ; _rtB -> B_1036_5181_0 = _rtB -> B_1036_5146_0 * _rtB ->
B_1036_5146_0 ; _rtB -> B_1036_5182_0 = _rtB -> B_1036_5180_0 + _rtB ->
B_1036_5181_0 ; _rtB -> B_1036_5183_0 = _rtB -> B_1036_5154_0 * _rtB ->
B_1036_5145_0 ; _rtB -> B_1036_5184_0 = _rtB -> B_1036_5153_0 * _rtB ->
B_1036_5146_0 ; _rtB -> B_1036_5185_0 = _rtB -> B_1036_5183_0 - _rtB ->
B_1036_5184_0 ; _rtB -> B_1036_5186_0 = _rtB -> B_1036_5179_0 / _rtB ->
B_1036_5182_0 ; _rtB -> B_1036_5187_0 = _rtB -> B_1036_5185_0 / _rtB ->
B_1036_5182_0 ; _rtB -> B_1036_5188_0 = _rtX -> integrator_CSTATE_jy ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e1 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_e1 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1002 ; _rtB -> B_1036_5189_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_ft . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ft . Last , _rtDW -> TransportDelay_IWORK_ft . Tail ,
_rtDW -> TransportDelay_IWORK_ft . Head , _rtP -> P_1003 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } _rtB -> B_1036_5190_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5191_0 = _rtP -> P_1004 ; } _rtB
-> B_1036_5192_0 = ( _rtB -> B_1036_5190_0 >= _rtB -> B_1036_5191_0 ) ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5193_0 =
_rtDW -> Memory_PreviousInput_at ; } if ( _rtB -> B_1036_5192_0 ) { _rtB ->
B_1017_0_0 = _rtB -> B_1036_5188_0 - _rtB -> B_1036_5189_0 ; _rtB ->
B_1017_1_0 = _rtP -> P_118 * _rtB -> B_1017_0_0 ; _rtB -> B_1036_5195_0 =
_rtB -> B_1017_1_0 ; } else { _rtB -> B_1036_5195_0 = _rtB -> B_1036_5193_0 ;
} _rtB -> B_1036_5196_0 = _rtX -> integrator_CSTATE_ci ; { real_T * * uBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ap . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ap .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1007 ; _rtB -> B_1036_5197_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_p1u . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_p1u . Last , _rtDW -> TransportDelay_IWORK_p1u . Tail ,
_rtDW -> TransportDelay_IWORK_p1u . Head , _rtP -> P_1008 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } _rtB -> B_1036_5198_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5199_0 = _rtP -> P_1009 ; } _rtB
-> B_1036_5200_0 = ( _rtB -> B_1036_5198_0 >= _rtB -> B_1036_5199_0 ) ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5201_0 =
_rtDW -> Memory_PreviousInput_md ; } if ( _rtB -> B_1036_5200_0 ) { _rtB ->
B_1016_0_0 = _rtB -> B_1036_5196_0 - _rtB -> B_1036_5197_0 ; _rtB ->
B_1016_1_0 = _rtP -> P_117 * _rtB -> B_1016_0_0 ; _rtB -> B_1036_5203_0 =
_rtB -> B_1016_1_0 ; } else { _rtB -> B_1036_5203_0 = _rtB -> B_1036_5201_0 ;
} _rtB -> B_1036_5204_0 . re = _rtB -> B_1036_5195_0 ; _rtB -> B_1036_5204_0
. im = _rtB -> B_1036_5203_0 ; _rtB -> B_1036_5205_0 = muDoubleScalarHypot (
_rtB -> B_1036_5204_0 . re , _rtB -> B_1036_5204_0 . im ) ; _rtB ->
B_1036_5205_1 = muDoubleScalarAtan2 ( _rtB -> B_1036_5204_0 . im , _rtB ->
B_1036_5204_0 . re ) ; _rtB -> B_1036_5206_0 = _rtP -> P_1011 * _rtB ->
B_1036_5205_1 ; _rtB -> B_1036_5207_0 = _rtP -> P_1012 * _rtB ->
B_1036_5206_0 ; maxV = _rtB -> B_1036_5207_0 ; u0 = _rtB -> B_1036_5205_0 ;
u2 = muDoubleScalarCos ( maxV ) ; maxV = muDoubleScalarSin ( maxV ) ; _rtB ->
B_1036_5208_0 . re = u0 * u2 ; _rtB -> B_1036_5208_0 . im = u0 * maxV ; _rtB
-> B_1036_5209_0 = _rtX -> integrator_CSTATE_ni ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mj . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mj .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1014 ; _rtB -> B_1036_5210_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_jdy . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jdy . Last , _rtDW -> TransportDelay_IWORK_jdy . Tail ,
_rtDW -> TransportDelay_IWORK_jdy . Head , _rtP -> P_1015 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } _rtB -> B_1036_5211_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5212_0 = _rtP -> P_1016 ; } _rtB
-> B_1036_5213_0 = ( _rtB -> B_1036_5211_0 >= _rtB -> B_1036_5212_0 ) ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5214_0 =
_rtDW -> Memory_PreviousInput_fn ; } if ( _rtB -> B_1036_5213_0 ) { _rtB ->
B_1019_0_0 = _rtB -> B_1036_5209_0 - _rtB -> B_1036_5210_0 ; _rtB ->
B_1019_1_0 = _rtP -> P_120 * _rtB -> B_1019_0_0 ; _rtB -> B_1036_5216_0 =
_rtB -> B_1019_1_0 ; } else { _rtB -> B_1036_5216_0 = _rtB -> B_1036_5214_0 ;
} _rtB -> B_1036_5217_0 = _rtX -> integrator_CSTATE_lr ; { real_T * * uBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK_kx . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_kx .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1019 ; _rtB -> B_1036_5218_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_gj . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_gj . Last , _rtDW -> TransportDelay_IWORK_gj . Tail ,
_rtDW -> TransportDelay_IWORK_gj . Head , _rtP -> P_1020 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } _rtB -> B_1036_5219_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5220_0 = _rtP -> P_1021 ; } _rtB
-> B_1036_5221_0 = ( _rtB -> B_1036_5219_0 >= _rtB -> B_1036_5220_0 ) ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5222_0 =
_rtDW -> Memory_PreviousInput_kq ; } if ( _rtB -> B_1036_5221_0 ) { _rtB ->
B_1018_0_0 = _rtB -> B_1036_5217_0 - _rtB -> B_1036_5218_0 ; _rtB ->
B_1018_1_0 = _rtP -> P_119 * _rtB -> B_1018_0_0 ; _rtB -> B_1036_5224_0 =
_rtB -> B_1018_1_0 ; } else { _rtB -> B_1036_5224_0 = _rtB -> B_1036_5222_0 ;
} _rtB -> B_1036_5225_0 . re = _rtB -> B_1036_5216_0 ; _rtB -> B_1036_5225_0
. im = _rtB -> B_1036_5224_0 ; _rtB -> B_1036_5226_0 = muDoubleScalarHypot (
_rtB -> B_1036_5225_0 . re , _rtB -> B_1036_5225_0 . im ) ; _rtB ->
B_1036_5226_1 = muDoubleScalarAtan2 ( _rtB -> B_1036_5225_0 . im , _rtB ->
B_1036_5225_0 . re ) ; _rtB -> B_1036_5227_0 = _rtP -> P_1023 * _rtB ->
B_1036_5226_1 ; _rtB -> B_1036_5228_0 = _rtP -> P_1024 * _rtB ->
B_1036_5227_0 ; maxV = _rtB -> B_1036_5228_0 ; u0 = _rtB -> B_1036_5226_0 ;
u2 = muDoubleScalarCos ( maxV ) ; maxV = muDoubleScalarSin ( maxV ) ; _rtB ->
B_1036_5229_0 . re = u0 * u2 ; _rtB -> B_1036_5229_0 . im = u0 * maxV ; _rtB
-> B_1036_5230_0 = _rtX -> integrator_CSTATE_gm ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fc . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fc .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1026 ; _rtB -> B_1036_5231_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_cr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_cr . Last , _rtDW -> TransportDelay_IWORK_cr . Tail ,
_rtDW -> TransportDelay_IWORK_cr . Head , _rtP -> P_1027 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } _rtB -> B_1036_5232_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5233_0 = _rtP -> P_1028 ; } _rtB
-> B_1036_5234_0 = ( _rtB -> B_1036_5232_0 >= _rtB -> B_1036_5233_0 ) ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5235_0 =
_rtDW -> Memory_PreviousInput_h2 ; } if ( _rtB -> B_1036_5234_0 ) { _rtB ->
B_1021_0_0 = _rtB -> B_1036_5230_0 - _rtB -> B_1036_5231_0 ; _rtB ->
B_1021_1_0 = _rtP -> P_122 * _rtB -> B_1021_0_0 ; _rtB -> B_1036_5237_0 =
_rtB -> B_1021_1_0 ; } else { _rtB -> B_1036_5237_0 = _rtB -> B_1036_5235_0 ;
} _rtB -> B_1036_5238_0 = _rtX -> integrator_CSTATE_ptr ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o3 . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o3
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1031 ; _rtB -> B_1036_5239_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_ka . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ka . Last , _rtDW -> TransportDelay_IWORK_ka . Tail ,
_rtDW -> TransportDelay_IWORK_ka . Head , _rtP -> P_1032 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } _rtB -> B_1036_5240_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5241_0 = _rtP -> P_1033 ; } _rtB
-> B_1036_5242_0 = ( _rtB -> B_1036_5240_0 >= _rtB -> B_1036_5241_0 ) ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5243_0 =
_rtDW -> Memory_PreviousInput_mp ; } if ( _rtB -> B_1036_5242_0 ) { _rtB ->
B_1020_0_0 = _rtB -> B_1036_5238_0 - _rtB -> B_1036_5239_0 ; _rtB ->
B_1020_1_0 = _rtP -> P_121 * _rtB -> B_1020_0_0 ; _rtB -> B_1036_5245_0 =
_rtB -> B_1020_1_0 ; } else { _rtB -> B_1036_5245_0 = _rtB -> B_1036_5243_0 ;
} _rtB -> B_1036_5246_0 . re = _rtB -> B_1036_5237_0 ; _rtB -> B_1036_5246_0
. im = _rtB -> B_1036_5245_0 ; _rtB -> B_1036_5247_0 = muDoubleScalarHypot (
_rtB -> B_1036_5246_0 . re , _rtB -> B_1036_5246_0 . im ) ; _rtB ->
B_1036_5247_1 = muDoubleScalarAtan2 ( _rtB -> B_1036_5246_0 . im , _rtB ->
B_1036_5246_0 . re ) ; _rtB -> B_1036_5248_0 = _rtP -> P_1035 * _rtB ->
B_1036_5247_1 ; _rtB -> B_1036_5249_0 = _rtP -> P_1036 * _rtB ->
B_1036_5248_0 ; maxV = _rtB -> B_1036_5249_0 ; u0 = _rtB -> B_1036_5247_0 ;
u2 = muDoubleScalarCos ( maxV ) ; maxV = muDoubleScalarSin ( maxV ) ; _rtB ->
B_1036_5250_0 . re = u0 * u2 ; _rtB -> B_1036_5250_0 . im = u0 * maxV ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5251_0 =
_rtB -> B_1036_2412_0_h ; } DZG_v3_NegSeqComputation ( S , _rtB ->
B_1036_5251_0 , _rtB -> B_1036_5208_0 , _rtB -> B_1036_5229_0 , _rtB ->
B_1036_5250_0 , & _rtB -> PosSeqComputation_f , & _rtDW ->
PosSeqComputation_f , & _rtP -> PosSeqComputation_f ) ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5253_0 = _rtB ->
B_1036_2413_0_c ; } DZG_v3_NegSeqComputation ( S , _rtB -> B_1036_5253_0 ,
_rtB -> B_1036_5208_0 , _rtB -> B_1036_5229_0 , _rtB -> B_1036_5250_0 , &
_rtB -> NegSeqComputation_e , & _rtDW -> NegSeqComputation_e , & _rtP ->
NegSeqComputation_e ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_1036_5255_0 = _rtB -> B_1036_2414_0_b ; }
DZG_v3_ZeroSeqComputation ( S , _rtB -> B_1036_5255_0 , _rtB -> B_1036_5208_0
, _rtB -> B_1036_5229_0 , _rtB -> B_1036_5250_0 , & _rtB ->
ZeroSeqComputation_g , & _rtDW -> ZeroSeqComputation_g , & _rtP ->
ZeroSeqComputation_g ) ; _rtB -> B_1036_5257_0 [ 0 ] = muDoubleScalarHypot (
_rtB -> PosSeqComputation_f . B_982_2_0 . re , _rtB -> PosSeqComputation_f .
B_982_2_0 . im ) ; _rtB -> B_1036_5257_0 [ 1 ] = muDoubleScalarHypot ( _rtB
-> NegSeqComputation_e . B_982_2_0 . re , _rtB -> NegSeqComputation_e .
B_982_2_0 . im ) ; _rtB -> B_1036_5257_0 [ 2 ] = muDoubleScalarHypot ( _rtB
-> ZeroSeqComputation_g . B_984_1_0 . re , _rtB -> ZeroSeqComputation_g .
B_984_1_0 . im ) ; _rtB -> B_1036_5257_1 [ 0 ] = muDoubleScalarAtan2 ( _rtB
-> PosSeqComputation_f . B_982_2_0 . im , _rtB -> PosSeqComputation_f .
B_982_2_0 . re ) ; _rtB -> B_1036_5257_1 [ 1 ] = muDoubleScalarAtan2 ( _rtB
-> NegSeqComputation_e . B_982_2_0 . im , _rtB -> NegSeqComputation_e .
B_982_2_0 . re ) ; _rtB -> B_1036_5257_1 [ 2 ] = muDoubleScalarAtan2 ( _rtB
-> ZeroSeqComputation_g . B_984_1_0 . im , _rtB -> ZeroSeqComputation_g .
B_984_1_0 . re ) ; _rtB -> B_1036_5258_0 = _rtP -> P_1037 * _rtB ->
B_1036_5257_0 [ 2 ] ; _rtB -> B_1036_5259_0 = _rtP -> P_1038 * _rtB ->
B_1036_5257_1 [ 2 ] ; _rtB -> B_1036_5260_0 = _rtP -> P_1039 * _rtB ->
B_1036_5259_0 ; _rtB -> B_1036_5261_0 = muDoubleScalarCos ( _rtB ->
B_1036_5260_0 ) ; _rtB -> B_1036_5262_0 = _rtB -> B_1036_5258_0 * _rtB ->
B_1036_5261_0 ; _rtB -> B_1036_5263_0 = _rtB -> B_1036_5262_0 * _rtB ->
B_1036_2410_0_d ; _rtB -> B_1036_5264_0 = muDoubleScalarSin ( _rtB ->
B_1036_5260_0 ) ; _rtB -> B_1036_5265_0 = _rtB -> B_1036_5264_0 * _rtB ->
B_1036_5258_0 ; _rtB -> B_1036_5266_0 = _rtB -> B_1036_5265_0 * _rtB ->
B_1036_2411_0_a ; _rtB -> B_1036_5267_0 = _rtB -> B_1036_5263_0 - _rtB ->
B_1036_5266_0 ; _rtB -> B_1036_5268_0 = _rtB -> B_1036_5267_0 + _rtB ->
B_1036_664_0 ; _rtB -> B_1036_5269_0 = _rtB -> B_1036_5122_0 * _rtB ->
B_1036_5268_0 ; _rtB -> B_1036_5270_0 = _rtB -> B_1036_2410_0_d * _rtB ->
B_1036_5265_0 ; _rtB -> B_1036_5271_0 = _rtB -> B_1036_2411_0_a * _rtB ->
B_1036_5262_0 ; _rtB -> B_1036_5272_0 = _rtB -> B_1036_5270_0 + _rtB ->
B_1036_5271_0 ; _rtB -> B_1036_5273_0 = _rtB -> B_1036_5272_0 + _rtB ->
B_1036_724_0 ; _rtB -> B_1036_5274_0 = _rtB -> B_1036_5121_0 * _rtB ->
B_1036_5273_0 ; _rtB -> B_1036_5275_0 = _rtB -> B_1036_5269_0 + _rtB ->
B_1036_5274_0 ; _rtB -> B_1036_5276_0 = _rtB -> B_1036_5268_0 * _rtB ->
B_1036_5268_0 ; _rtB -> B_1036_5277_0 = _rtB -> B_1036_5273_0 * _rtB ->
B_1036_5273_0 ; _rtB -> B_1036_5278_0 = _rtB -> B_1036_5276_0 + _rtB ->
B_1036_5277_0 ; _rtB -> B_1036_5279_0 = _rtB -> B_1036_5275_0 / _rtB ->
B_1036_5278_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
ssCallAccelRunBlock ( S , 1036 , 5280 , SS_CALL_MDL_OUTPUTS ) ; } _rtB ->
B_1036_5281_0 = _rtB -> B_1036_5121_0 * _rtB -> B_1036_5268_0 ; _rtB ->
B_1036_5282_0 = _rtB -> B_1036_5122_0 * _rtB -> B_1036_5273_0 ; _rtB ->
B_1036_5283_0 = _rtB -> B_1036_5281_0 - _rtB -> B_1036_5282_0 ; _rtB ->
B_1036_5284_0 = _rtB -> B_1036_5283_0 / _rtB -> B_1036_5278_0 ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
1036 , 5285 , SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_5286_0 = _rtB ->
B_1036_5267_0 + _rtB -> B_1036_708_0 ; _rtB -> B_1036_5287_0 = _rtB ->
B_1036_5130_0 * _rtB -> B_1036_5286_0 ; _rtB -> B_1036_5288_0 = _rtB ->
B_1036_5272_0 + _rtB -> B_1036_730_0 ; _rtB -> B_1036_5289_0 = _rtB ->
B_1036_5129_0 * _rtB -> B_1036_5288_0 ; _rtB -> B_1036_5290_0 = _rtB ->
B_1036_5287_0 + _rtB -> B_1036_5289_0 ; _rtB -> B_1036_5291_0 = _rtB ->
B_1036_5286_0 * _rtB -> B_1036_5286_0 ; _rtB -> B_1036_5292_0 = _rtB ->
B_1036_5288_0 * _rtB -> B_1036_5288_0 ; _rtB -> B_1036_5293_0 = _rtB ->
B_1036_5291_0 + _rtB -> B_1036_5292_0 ; _rtB -> B_1036_5294_0 = _rtB ->
B_1036_5290_0 / _rtB -> B_1036_5293_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { ssCallAccelRunBlock ( S , 1036 , 5295 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_5296_0 = _rtB -> B_1036_5129_0 *
_rtB -> B_1036_5286_0 ; _rtB -> B_1036_5297_0 = _rtB -> B_1036_5130_0 * _rtB
-> B_1036_5288_0 ; _rtB -> B_1036_5298_0 = _rtB -> B_1036_5296_0 - _rtB ->
B_1036_5297_0 ; _rtB -> B_1036_5299_0 = _rtB -> B_1036_5298_0 / _rtB ->
B_1036_5293_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
ssCallAccelRunBlock ( S , 1036 , 5300 , SS_CALL_MDL_OUTPUTS ) ; } _rtB ->
B_1036_5301_0 = _rtB -> B_1036_5267_0 + _rtB -> B_1036_713_0 ; _rtB ->
B_1036_5302_0 = _rtB -> B_1036_5138_0 * _rtB -> B_1036_5301_0 ; _rtB ->
B_1036_5303_0 = _rtB -> B_1036_5272_0 + _rtB -> B_1036_735_0 ; _rtB ->
B_1036_5304_0 = _rtB -> B_1036_5137_0 * _rtB -> B_1036_5303_0 ; _rtB ->
B_1036_5305_0 = _rtB -> B_1036_5302_0 + _rtB -> B_1036_5304_0 ; _rtB ->
B_1036_5306_0 = _rtB -> B_1036_5301_0 * _rtB -> B_1036_5301_0 ; _rtB ->
B_1036_5307_0 = _rtB -> B_1036_5303_0 * _rtB -> B_1036_5303_0 ; _rtB ->
B_1036_5308_0 = _rtB -> B_1036_5306_0 + _rtB -> B_1036_5307_0 ; _rtB ->
B_1036_5309_0 = _rtB -> B_1036_5305_0 / _rtB -> B_1036_5308_0 ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
1036 , 5310 , SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_5311_0 = _rtB ->
B_1036_5137_0 * _rtB -> B_1036_5301_0 ; _rtB -> B_1036_5312_0 = _rtB ->
B_1036_5138_0 * _rtB -> B_1036_5303_0 ; _rtB -> B_1036_5313_0 = _rtB ->
B_1036_5311_0 - _rtB -> B_1036_5312_0 ; _rtB -> B_1036_5314_0 = _rtB ->
B_1036_5313_0 / _rtB -> B_1036_5308_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { ssCallAccelRunBlock ( S , 1036 , 5315 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_1036_5316_0 = muDoubleScalarSin ( _rtP ->
P_1042 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1043 ) * _rtP -> P_1040 + _rtP
-> P_1041 ; _rtB -> B_1036_5317_0 = _rtB -> B_1036_1611_0 [ 0 ] * _rtB ->
B_1036_5316_0 ; _rtB -> B_1036_5318_0 = muDoubleScalarSin ( _rtP -> P_1046 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1047 ) * _rtP -> P_1044 + _rtP -> P_1045
; _rtB -> B_1036_5319_0 = _rtB -> B_1036_1611_0 [ 0 ] * _rtB -> B_1036_5318_0
; _rtB -> B_1036_5320_0 = muDoubleScalarSin ( _rtP -> P_1050 * ssGetTaskTime
( S , 0 ) + _rtP -> P_1051 ) * _rtP -> P_1048 + _rtP -> P_1049 ; _rtB ->
B_1036_5321_0 = _rtB -> B_1036_1611_0 [ 1 ] * _rtB -> B_1036_5320_0 ; _rtB ->
B_1036_5322_0 = muDoubleScalarSin ( _rtP -> P_1054 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1055 ) * _rtP -> P_1052 + _rtP -> P_1053 ; _rtB -> B_1036_5323_0
= _rtB -> B_1036_1611_0 [ 1 ] * _rtB -> B_1036_5322_0 ; _rtB -> B_1036_5324_0
= muDoubleScalarSin ( _rtP -> P_1058 * ssGetTaskTime ( S , 0 ) + _rtP ->
P_1059 ) * _rtP -> P_1056 + _rtP -> P_1057 ; _rtB -> B_1036_5325_0 = _rtB ->
B_1036_1611_0 [ 2 ] * _rtB -> B_1036_5324_0 ; _rtB -> B_1036_5326_0 =
muDoubleScalarSin ( _rtP -> P_1062 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1063
) * _rtP -> P_1060 + _rtP -> P_1061 ; _rtB -> B_1036_5327_0 = _rtB ->
B_1036_1611_0 [ 2 ] * _rtB -> B_1036_5326_0 ; _rtB -> B_1036_5328_0 =
muDoubleScalarSin ( _rtP -> P_1066 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1067
) * _rtP -> P_1064 + _rtP -> P_1065 ; _rtB -> B_1036_5329_0 = _rtB ->
B_1036_2_0_k * _rtB -> B_1036_5328_0 ; _rtB -> B_1036_5330_0 =
muDoubleScalarSin ( _rtP -> P_1070 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1071
) * _rtP -> P_1068 + _rtP -> P_1069 ; _rtB -> B_1036_5331_0 = _rtB ->
B_1036_2_0_k * _rtB -> B_1036_5330_0 ; _rtB -> B_1036_5332_0 =
muDoubleScalarSin ( _rtP -> P_1074 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1075
) * _rtP -> P_1072 + _rtP -> P_1073 ; _rtB -> B_1036_5333_0 = _rtB ->
B_1036_2_0_k * _rtB -> B_1036_5332_0 ; _rtB -> B_1036_5334_0 =
muDoubleScalarSin ( _rtP -> P_1078 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1079
) * _rtP -> P_1076 + _rtP -> P_1077 ; _rtB -> B_1036_5335_0 = _rtB ->
B_1036_2_0_k * _rtB -> B_1036_5334_0 ; _rtB -> B_1036_5336_0 =
muDoubleScalarSin ( _rtP -> P_1082 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1083
) * _rtP -> P_1080 + _rtP -> P_1081 ; _rtB -> B_1036_5337_0 = 0.0 * _rtB ->
B_1036_5336_0 ; _rtB -> B_1036_5338_0 = muDoubleScalarSin ( _rtP -> P_1086 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1087 ) * _rtP -> P_1084 + _rtP -> P_1085
; _rtB -> B_1036_5339_0 = 0.0 * _rtB -> B_1036_5338_0 ; _rtB -> B_1036_5340_0
= muDoubleScalarSin ( _rtP -> P_1090 * ssGetTaskTime ( S , 0 ) + _rtP ->
P_1091 ) * _rtP -> P_1088 + _rtP -> P_1089 ; _rtB -> B_1036_5341_0 = 0.0 *
_rtB -> B_1036_5340_0 ; _rtB -> B_1036_5342_0 = muDoubleScalarSin ( _rtP ->
P_1094 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1095 ) * _rtP -> P_1092 + _rtP
-> P_1093 ; _rtB -> B_1036_5343_0 = 0.0 * _rtB -> B_1036_5342_0 ; _rtB ->
B_1036_5344_0 = muDoubleScalarSin ( _rtP -> P_1098 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1099 ) * _rtP -> P_1096 + _rtP -> P_1097 ; _rtB -> B_1036_5345_0
= 0.0 * _rtB -> B_1036_5344_0 ; _rtB -> B_1036_5346_0 = muDoubleScalarSin (
_rtP -> P_1102 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1103 ) * _rtP -> P_1100
+ _rtP -> P_1101 ; _rtB -> B_1036_5347_0 = 0.0 * _rtB -> B_1036_5346_0 ; _rtB
-> B_1036_5348_0 = muDoubleScalarSin ( _rtP -> P_1106 * ssGetTaskTime ( S , 0
) + _rtP -> P_1107 ) * _rtP -> P_1104 + _rtP -> P_1105 ; _rtB ->
B_1036_5349_0 = 0.0 * _rtB -> B_1036_5348_0 ; _rtB -> B_1036_5350_0 =
muDoubleScalarSin ( _rtP -> P_1110 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1111
) * _rtP -> P_1108 + _rtP -> P_1109 ; _rtB -> B_1036_5351_0 = 0.0 * _rtB ->
B_1036_5350_0 ; _rtB -> B_1036_5352_0 = muDoubleScalarSin ( _rtP -> P_1114 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1115 ) * _rtP -> P_1112 + _rtP -> P_1113
; _rtB -> B_1036_5353_0 = 0.0 * _rtB -> B_1036_5352_0 ; _rtB -> B_1036_5354_0
= muDoubleScalarSin ( _rtP -> P_1118 * ssGetTaskTime ( S , 0 ) + _rtP ->
P_1119 ) * _rtP -> P_1116 + _rtP -> P_1117 ; _rtB -> B_1036_5355_0 = 0.0 *
_rtB -> B_1036_5354_0 ; _rtB -> B_1036_5356_0 = muDoubleScalarSin ( _rtP ->
P_1122 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1123 ) * _rtP -> P_1120 + _rtP
-> P_1121 ; _rtB -> B_1036_5357_0 = 0.0 * _rtB -> B_1036_5356_0 ; _rtB ->
B_1036_5358_0 = muDoubleScalarSin ( _rtP -> P_1126 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1127 ) * _rtP -> P_1124 + _rtP -> P_1125 ; _rtB -> B_1036_5359_0
= 0.0 * _rtB -> B_1036_5358_0 ; _rtB -> B_1036_5360_0 = _rtX ->
integrator_CSTATE_am ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ff . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ff . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1129 ; _rtB
-> B_1036_5361_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_hw . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_hw . Last , _rtDW -> TransportDelay_IWORK_hw .
Tail , _rtDW -> TransportDelay_IWORK_hw . Head , _rtP -> P_1130 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_5362_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5363_0 =
_rtP -> P_1131 ; } _rtB -> B_1036_5364_0 = ( _rtB -> B_1036_5362_0 >= _rtB ->
B_1036_5363_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_5365_0 = _rtDW -> Memory_PreviousInput_fd ; } if ( _rtB ->
B_1036_5364_0 ) { _rtB -> B_1026_0_0 = _rtB -> B_1036_5360_0 - _rtB ->
B_1036_5361_0 ; _rtB -> B_1026_1_0 = _rtP -> P_124 * _rtB -> B_1026_0_0 ;
_rtB -> B_1036_5367_0 = _rtB -> B_1026_1_0 ; } else { _rtB -> B_1036_5367_0 =
_rtB -> B_1036_5365_0 ; } _rtB -> B_1036_5368_0 = _rtX ->
integrator_CSTATE_h0 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_be3 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_be3 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1134 ; _rtB
-> B_1036_5369_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_db . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_db . Last , _rtDW -> TransportDelay_IWORK_db .
Tail , _rtDW -> TransportDelay_IWORK_db . Head , _rtP -> P_1135 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_5370_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5371_0 =
_rtP -> P_1136 ; } _rtB -> B_1036_5372_0 = ( _rtB -> B_1036_5370_0 >= _rtB ->
B_1036_5371_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_5373_0 = _rtDW -> Memory_PreviousInput_la ; } if ( _rtB ->
B_1036_5372_0 ) { _rtB -> B_1025_0_0 = _rtB -> B_1036_5368_0 - _rtB ->
B_1036_5369_0 ; _rtB -> B_1025_1_0 = _rtP -> P_123 * _rtB -> B_1025_0_0 ;
_rtB -> B_1036_5375_0 = _rtB -> B_1025_1_0 ; } else { _rtB -> B_1036_5375_0 =
_rtB -> B_1036_5373_0 ; } _rtB -> B_1036_5376_0 . re = _rtB -> B_1036_5367_0
; _rtB -> B_1036_5376_0 . im = _rtB -> B_1036_5375_0 ; _rtB -> B_1036_5377_0
= muDoubleScalarHypot ( _rtB -> B_1036_5376_0 . re , _rtB -> B_1036_5376_0 .
im ) ; _rtB -> B_1036_5377_1 = muDoubleScalarAtan2 ( _rtB -> B_1036_5376_0 .
im , _rtB -> B_1036_5376_0 . re ) ; _rtB -> B_1036_5378_0 = _rtP -> P_1138 *
_rtB -> B_1036_5377_1 ; _rtB -> B_1036_5379_0 = _rtP -> P_1139 * _rtB ->
B_1036_5378_0 ; maxV = _rtB -> B_1036_5379_0 ; u0 = _rtB -> B_1036_5377_0 ;
u2 = muDoubleScalarCos ( maxV ) ; maxV = muDoubleScalarSin ( maxV ) ; _rtB ->
B_1036_5380_0 . re = u0 * u2 ; _rtB -> B_1036_5380_0 . im = u0 * maxV ; _rtB
-> B_1036_5381_0 = _rtX -> integrator_CSTATE_in ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_pn . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pn .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1141 ; _rtB -> B_1036_5382_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_k5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_k5 . Last , _rtDW -> TransportDelay_IWORK_k5 . Tail ,
_rtDW -> TransportDelay_IWORK_k5 . Head , _rtP -> P_1142 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } _rtB -> B_1036_5383_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5384_0 = _rtP -> P_1143 ; } _rtB
-> B_1036_5385_0 = ( _rtB -> B_1036_5383_0 >= _rtB -> B_1036_5384_0 ) ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5386_0 =
_rtDW -> Memory_PreviousInput_o5 ; } if ( _rtB -> B_1036_5385_0 ) { _rtB ->
B_1028_0_0 = _rtB -> B_1036_5381_0 - _rtB -> B_1036_5382_0 ; _rtB ->
B_1028_1_0 = _rtP -> P_126 * _rtB -> B_1028_0_0 ; _rtB -> B_1036_5388_0 =
_rtB -> B_1028_1_0 ; } else { _rtB -> B_1036_5388_0 = _rtB -> B_1036_5386_0 ;
} _rtB -> B_1036_5389_0 = _rtX -> integrator_CSTATE_p3 ; { real_T * * uBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cm . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cm .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1146 ; _rtB -> B_1036_5390_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_kat . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kat . Last , _rtDW -> TransportDelay_IWORK_kat . Tail ,
_rtDW -> TransportDelay_IWORK_kat . Head , _rtP -> P_1147 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } _rtB -> B_1036_5391_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5392_0 = _rtP -> P_1148 ; } _rtB
-> B_1036_5393_0 = ( _rtB -> B_1036_5391_0 >= _rtB -> B_1036_5392_0 ) ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5394_0 =
_rtDW -> Memory_PreviousInput_ef ; } if ( _rtB -> B_1036_5393_0 ) { _rtB ->
B_1027_0_0 = _rtB -> B_1036_5389_0 - _rtB -> B_1036_5390_0 ; _rtB ->
B_1027_1_0 = _rtP -> P_125 * _rtB -> B_1027_0_0 ; _rtB -> B_1036_5396_0 =
_rtB -> B_1027_1_0 ; } else { _rtB -> B_1036_5396_0 = _rtB -> B_1036_5394_0 ;
} _rtB -> B_1036_5397_0 . re = _rtB -> B_1036_5388_0 ; _rtB -> B_1036_5397_0
. im = _rtB -> B_1036_5396_0 ; _rtB -> B_1036_5398_0 = muDoubleScalarHypot (
_rtB -> B_1036_5397_0 . re , _rtB -> B_1036_5397_0 . im ) ; _rtB ->
B_1036_5398_1 = muDoubleScalarAtan2 ( _rtB -> B_1036_5397_0 . im , _rtB ->
B_1036_5397_0 . re ) ; _rtB -> B_1036_5399_0 = _rtP -> P_1150 * _rtB ->
B_1036_5398_1 ; _rtB -> B_1036_5400_0 = _rtP -> P_1151 * _rtB ->
B_1036_5399_0 ; maxV = _rtB -> B_1036_5400_0 ; u0 = _rtB -> B_1036_5398_0 ;
u2 = muDoubleScalarCos ( maxV ) ; maxV = muDoubleScalarSin ( maxV ) ; _rtB ->
B_1036_5401_0 . re = u0 * u2 ; _rtB -> B_1036_5401_0 . im = u0 * maxV ; _rtB
-> B_1036_5402_0 = _rtX -> integrator_CSTATE_bz ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_jx . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_jx .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1153 ; _rtB -> B_1036_5403_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_dt . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dt . Last , _rtDW -> TransportDelay_IWORK_dt . Tail ,
_rtDW -> TransportDelay_IWORK_dt . Head , _rtP -> P_1154 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } _rtB -> B_1036_5404_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5405_0 = _rtP -> P_1155 ; } _rtB
-> B_1036_5406_0 = ( _rtB -> B_1036_5404_0 >= _rtB -> B_1036_5405_0 ) ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5407_0 =
_rtDW -> Memory_PreviousInput_cn ; } if ( _rtB -> B_1036_5406_0 ) { _rtB ->
B_1030_0_0 = _rtB -> B_1036_5402_0 - _rtB -> B_1036_5403_0 ; _rtB ->
B_1030_1_0 = _rtP -> P_128 * _rtB -> B_1030_0_0 ; _rtB -> B_1036_5409_0 =
_rtB -> B_1030_1_0 ; } else { _rtB -> B_1036_5409_0 = _rtB -> B_1036_5407_0 ;
} _rtB -> B_1036_5410_0 = _rtX -> integrator_CSTATE_mh ; { real_T * * uBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK_c1 . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_c1 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1158 ; _rtB -> B_1036_5411_0 =
DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
_rtDW -> TransportDelay_IWORK_bl . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_bl . Last , _rtDW -> TransportDelay_IWORK_bl . Tail ,
_rtDW -> TransportDelay_IWORK_bl . Head , _rtP -> P_1159 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } _rtB -> B_1036_5412_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5413_0 = _rtP -> P_1160 ; } _rtB
-> B_1036_5414_0 = ( _rtB -> B_1036_5412_0 >= _rtB -> B_1036_5413_0 ) ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5415_0 =
_rtDW -> Memory_PreviousInput_au ; } if ( _rtB -> B_1036_5414_0 ) { _rtB ->
B_1029_0_0 = _rtB -> B_1036_5410_0 - _rtB -> B_1036_5411_0 ; _rtB ->
B_1029_1_0 = _rtP -> P_127 * _rtB -> B_1029_0_0 ; _rtB -> B_1036_5417_0 =
_rtB -> B_1029_1_0 ; } else { _rtB -> B_1036_5417_0 = _rtB -> B_1036_5415_0 ;
} _rtB -> B_1036_5418_0 . re = _rtB -> B_1036_5409_0 ; _rtB -> B_1036_5418_0
. im = _rtB -> B_1036_5417_0 ; _rtB -> B_1036_5419_0 = muDoubleScalarHypot (
_rtB -> B_1036_5418_0 . re , _rtB -> B_1036_5418_0 . im ) ; _rtB ->
B_1036_5419_1 = muDoubleScalarAtan2 ( _rtB -> B_1036_5418_0 . im , _rtB ->
B_1036_5418_0 . re ) ; _rtB -> B_1036_5420_0 = _rtP -> P_1162 * _rtB ->
B_1036_5419_1 ; _rtB -> B_1036_5421_0 = _rtP -> P_1163 * _rtB ->
B_1036_5420_0 ; maxV = _rtB -> B_1036_5421_0 ; u0 = _rtB -> B_1036_5419_0 ;
u2 = muDoubleScalarCos ( maxV ) ; maxV = muDoubleScalarSin ( maxV ) ; _rtB ->
B_1036_5422_0 . re = u0 * u2 ; _rtB -> B_1036_5422_0 . im = u0 * maxV ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5423_0 =
_rtB -> B_1036_2415_0 ; } DZG_v3_NegSeqComputation ( S , _rtB ->
B_1036_5423_0 , _rtB -> B_1036_5380_0 , _rtB -> B_1036_5401_0 , _rtB ->
B_1036_5422_0 , & _rtB -> PosSeqComputation_op , & _rtDW ->
PosSeqComputation_op , & _rtP -> PosSeqComputation_op ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5425_0 =
_rtB -> B_1036_2416_0_c ; } DZG_v3_NegSeqComputation ( S , _rtB ->
B_1036_5425_0 , _rtB -> B_1036_5380_0 , _rtB -> B_1036_5401_0 , _rtB ->
B_1036_5422_0 , & _rtB -> NegSeqComputation_ea , & _rtDW ->
NegSeqComputation_ea , & _rtP -> NegSeqComputation_ea ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5427_0 =
_rtB -> B_1036_2417_0_g ; } DZG_v3_ZeroSeqComputation ( S , _rtB ->
B_1036_5427_0 , _rtB -> B_1036_5380_0 , _rtB -> B_1036_5401_0 , _rtB ->
B_1036_5422_0 , & _rtB -> ZeroSeqComputation_d , & _rtDW ->
ZeroSeqComputation_d , & _rtP -> ZeroSeqComputation_d ) ; _rtB ->
B_1036_5429_0 [ 0 ] = muDoubleScalarHypot ( _rtB -> PosSeqComputation_op .
B_982_2_0 . re , _rtB -> PosSeqComputation_op . B_982_2_0 . im ) ; _rtB ->
B_1036_5429_0 [ 1 ] = muDoubleScalarHypot ( _rtB -> NegSeqComputation_ea .
B_982_2_0 . re , _rtB -> NegSeqComputation_ea . B_982_2_0 . im ) ; _rtB ->
B_1036_5429_0 [ 2 ] = muDoubleScalarHypot ( _rtB -> ZeroSeqComputation_d .
B_984_1_0 . re , _rtB -> ZeroSeqComputation_d . B_984_1_0 . im ) ; _rtB ->
B_1036_5429_1 [ 0 ] = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_op .
B_982_2_0 . im , _rtB -> PosSeqComputation_op . B_982_2_0 . re ) ; _rtB ->
B_1036_5429_1 [ 1 ] = muDoubleScalarAtan2 ( _rtB -> NegSeqComputation_ea .
B_982_2_0 . im , _rtB -> NegSeqComputation_ea . B_982_2_0 . re ) ; _rtB ->
B_1036_5429_1 [ 2 ] = muDoubleScalarAtan2 ( _rtB -> ZeroSeqComputation_d .
B_984_1_0 . im , _rtB -> ZeroSeqComputation_d . B_984_1_0 . re ) ; _rtB ->
B_1036_5430_0 = _rtP -> P_1164 * _rtB -> B_1036_5429_1 [ 2 ] ; _rtB ->
B_1036_5431_0 = _rtP -> P_1165 * _rtB -> B_1036_5430_0 ; _rtB ->
B_1036_5432_0 = muDoubleScalarSin ( _rtB -> B_1036_5431_0 ) ; _rtB ->
B_1036_5433_0 = muDoubleScalarCos ( _rtB -> B_1036_5431_0 ) ; _rtB ->
B_1036_5434_0 = _rtP -> P_1166 * _rtB -> B_1036_5429_0 [ 2 ] ; _rtB ->
B_1036_5435_0 = _rtB -> B_1036_5432_0 * _rtB -> B_1036_5434_0 ; _rtB ->
B_1036_5436_0 = _rtB -> B_1036_5434_0 * _rtB -> B_1036_5433_0 ; _rtB ->
B_1036_5437_0 = muDoubleScalarSin ( _rtP -> P_1169 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1170 ) * _rtP -> P_1167 + _rtP -> P_1168 ; _rtB -> B_1036_5438_0
= _rtB -> B_1036_1611_0 [ 0 ] * _rtB -> B_1036_5437_0 ; _rtB -> B_1036_5439_0
= muDoubleScalarSin ( _rtP -> P_1173 * ssGetTaskTime ( S , 0 ) + _rtP ->
P_1174 ) * _rtP -> P_1171 + _rtP -> P_1172 ; _rtB -> B_1036_5440_0 = _rtB ->
B_1036_1611_0 [ 0 ] * _rtB -> B_1036_5439_0 ; _rtB -> B_1036_5441_0 =
muDoubleScalarSin ( _rtP -> P_1177 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1178
) * _rtP -> P_1175 + _rtP -> P_1176 ; _rtB -> B_1036_5442_0 = _rtB ->
B_1036_1611_0 [ 1 ] * _rtB -> B_1036_5441_0 ; _rtB -> B_1036_5443_0 =
muDoubleScalarSin ( _rtP -> P_1181 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1182
) * _rtP -> P_1179 + _rtP -> P_1180 ; _rtB -> B_1036_5444_0 = _rtB ->
B_1036_1611_0 [ 1 ] * _rtB -> B_1036_5443_0 ; _rtB -> B_1036_5445_0 =
muDoubleScalarSin ( _rtP -> P_1185 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1186
) * _rtP -> P_1183 + _rtP -> P_1184 ; _rtB -> B_1036_5446_0 = _rtB ->
B_1036_1611_0 [ 2 ] * _rtB -> B_1036_5445_0 ; _rtB -> B_1036_5447_0 =
muDoubleScalarSin ( _rtP -> P_1189 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1190
) * _rtP -> P_1187 + _rtP -> P_1188 ; _rtB -> B_1036_5448_0 = _rtB ->
B_1036_1611_0 [ 2 ] * _rtB -> B_1036_5447_0 ; _rtB -> B_1036_5449_0 = _rtB ->
B_1036_5436_0 * _rtB -> B_1036_5262_0 ; _rtB -> B_1036_5450_0 = _rtB ->
B_1036_5435_0 * _rtB -> B_1036_5265_0 ; _rtB -> B_1036_5451_0 = _rtB ->
B_1036_5449_0 + _rtB -> B_1036_5450_0 ; _rtB -> B_1036_5452_0 = _rtB ->
B_1036_5262_0 * _rtB -> B_1036_5262_0 ; _rtB -> B_1036_5453_0 = _rtB ->
B_1036_5265_0 * _rtB -> B_1036_5265_0 ; _rtB -> B_1036_5454_0 = _rtB ->
B_1036_5452_0 + _rtB -> B_1036_5453_0 ; _rtB -> B_1036_5455_0 = _rtB ->
B_1036_5435_0 * _rtB -> B_1036_5262_0 ; _rtB -> B_1036_5456_0 = _rtB ->
B_1036_5436_0 * _rtB -> B_1036_5265_0 ; _rtB -> B_1036_5457_0 = _rtB ->
B_1036_5455_0 - _rtB -> B_1036_5456_0 ; _rtB -> B_1036_5458_0 = _rtB ->
B_1036_5451_0 / _rtB -> B_1036_5454_0 ; _rtB -> B_1036_5459_0 = _rtB ->
B_1036_5457_0 / _rtB -> B_1036_5454_0 ; _rtB -> B_1036_5460_0 =
muDoubleScalarSin ( _rtP -> P_1193 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1194
) * _rtP -> P_1191 + _rtP -> P_1192 ; _rtB -> B_1036_5461_0 = 0.0 * _rtB ->
B_1036_5460_0 ; _rtB -> B_1036_5462_0 = muDoubleScalarSin ( _rtP -> P_1197 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1198 ) * _rtP -> P_1195 + _rtP -> P_1196
; _rtB -> B_1036_5463_0 = 0.0 * _rtB -> B_1036_5462_0 ; _rtB -> B_1036_5464_0
= muDoubleScalarSin ( _rtP -> P_1201 * ssGetTaskTime ( S , 0 ) + _rtP ->
P_1202 ) * _rtP -> P_1199 + _rtP -> P_1200 ; _rtB -> B_1036_5465_0 = 0.0 *
_rtB -> B_1036_5464_0 ; _rtB -> B_1036_5466_0 = muDoubleScalarSin ( _rtP ->
P_1205 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1206 ) * _rtP -> P_1203 + _rtP
-> P_1204 ; _rtB -> B_1036_5467_0 = 0.0 * _rtB -> B_1036_5466_0 ; _rtB ->
B_1036_5468_0 = muDoubleScalarSin ( _rtP -> P_1209 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1210 ) * _rtP -> P_1207 + _rtP -> P_1208 ; _rtB -> B_1036_5469_0
= 0.0 * _rtB -> B_1036_5468_0 ; _rtB -> B_1036_5470_0 = muDoubleScalarSin (
_rtP -> P_1213 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1214 ) * _rtP -> P_1211
+ _rtP -> P_1212 ; _rtB -> B_1036_5471_0 = 0.0 * _rtB -> B_1036_5470_0 ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_5472_0 = _rtDW -> u4_PreviousInput_f ; } _rtB -> B_1036_5473_0 = false
; _rtB -> B_1036_5474_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_5475_0 = _rtB -> B_1036_2501_0 ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_5475_0 , _rtB -> B_1036_5473_0 , _rtB ->
B_1036_5474_0 , _rtB -> B_1036_2499_0_f , & _rtB -> ONDelay_be , & _rtDW ->
ONDelay_be , & _rtZCE -> ONDelay_be ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_5477_0 = _rtB -> B_1036_2502_0_j ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_5477_0 , _rtB -> B_1036_5473_0 , _rtB ->
B_1036_5474_0 , _rtB -> B_1036_2499_0_f , & _rtB -> OFFDelay_br , & _rtDW ->
OFFDelay_br , & _rtZCE -> OFFDelay_br ) ; _rtB -> B_1036_5479_0 = ( _rtB ->
ONDelay_be . B_90_17_0 || _rtB -> OFFDelay_br . B_86_19_0 ) ; _rtB ->
B_1036_5480_0 = ( _rtB -> B_1036_5472_0 || _rtB -> B_1036_5479_0 ) ; _rtB ->
B_1036_5481_0 = _rtB -> B_1036_2427_0_d ; _rtB -> B_1036_5482_0 = ssGetT ( S
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_5483_0 = _rtB -> B_1036_2508_0_o ; } DZG_v3_ONDelay ( S , _rtB ->
B_1036_5483_0 , _rtB -> B_1036_5481_0 , _rtB -> B_1036_5482_0 , _rtB ->
B_1036_2506_0 , & _rtB -> ONDelay_c3 , & _rtDW -> ONDelay_c3 , & _rtZCE ->
ONDelay_c3 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_1036_5485_0 = _rtB -> B_1036_2509_0_j ; } DZG_v3_OFFDelay ( S , _rtB ->
B_1036_5485_0 , _rtB -> B_1036_5481_0 , _rtB -> B_1036_5482_0 , _rtB ->
B_1036_2506_0 , & _rtB -> OFFDelay_nk , & _rtDW -> OFFDelay_nk , & _rtZCE ->
OFFDelay_nk ) ; _rtB -> B_1036_5487_0 = ( _rtB -> ONDelay_c3 . B_90_17_0 ||
_rtB -> OFFDelay_nk . B_86_19_0 ) ; _rtB -> B_1036_5488_0 = ! _rtB ->
B_1036_5487_0 ; _rtB -> B_1036_5489_0 = ( _rtB -> B_1036_5480_0 && _rtB ->
B_1036_5488_0 ) ; _rtB -> B_1036_5490_0 = ( _rtB -> B_1036_2456_0_e && _rtB
-> B_1036_5489_0 ) ; _rtB -> B_1036_5491_0 = ( _rtB -> B_1036_2462_0_i &&
_rtB -> B_1036_5489_0 ) ; _rtB -> B_1036_5492_0 = ( _rtB -> B_1036_2467_0_g
&& _rtB -> B_1036_5489_0 ) ; _rtB -> B_1036_5493_0 = _rtX ->
integrator_CSTATE_m0 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ak . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ak . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1216 ; _rtB
-> B_1036_5494_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_no . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_no . Last , _rtDW -> TransportDelay_IWORK_no .
Tail , _rtDW -> TransportDelay_IWORK_no . Head , _rtP -> P_1217 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_5495_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5496_0 =
_rtP -> P_1218 ; } _rtB -> B_1036_5497_0 = ( _rtB -> B_1036_5495_0 >= _rtB ->
B_1036_5496_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_5498_0 = _rtDW -> Memory_PreviousInput_ho ; } if ( _rtB ->
B_1036_5497_0 ) { _rtB -> B_1035_0_0 = _rtB -> B_1036_5493_0 - _rtB ->
B_1036_5494_0 ; _rtB -> B_1035_1_0 = _rtP -> P_130 * _rtB -> B_1035_0_0 ;
_rtB -> B_1036_5500_0 = _rtB -> B_1035_1_0 ; } else { _rtB -> B_1036_5500_0 =
_rtB -> B_1036_5498_0 ; } _rtB -> B_1036_5501_0 = _rtX ->
integrator_CSTATE_eb ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_c0 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_c0 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1221 ; _rtB
-> B_1036_5502_0 = DZG_v3_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_bm . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_bm . Last , _rtDW -> TransportDelay_IWORK_bm .
Tail , _rtDW -> TransportDelay_IWORK_bm . Head , _rtP -> P_1222 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_1036_5503_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5504_0 =
_rtP -> P_1223 ; } _rtB -> B_1036_5505_0 = ( _rtB -> B_1036_5503_0 >= _rtB ->
B_1036_5504_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_5506_0 = _rtDW -> Memory_PreviousInput_fu ; } if ( _rtB ->
B_1036_5505_0 ) { _rtB -> B_1034_0_0 = _rtB -> B_1036_5501_0 - _rtB ->
B_1036_5502_0 ; _rtB -> B_1034_1_0 = _rtP -> P_129 * _rtB -> B_1034_0_0 ;
_rtB -> B_1036_5508_0 = _rtB -> B_1034_1_0 ; } else { _rtB -> B_1036_5508_0 =
_rtB -> B_1036_5506_0 ; } _rtB -> B_1036_5509_0 . re = _rtB -> B_1036_5500_0
; _rtB -> B_1036_5509_0 . im = _rtB -> B_1036_5508_0 ; _rtB -> B_1036_5510_0
= muDoubleScalarHypot ( _rtB -> B_1036_5509_0 . re , _rtB -> B_1036_5509_0 .
im ) ; _rtB -> B_1036_5510_1 = muDoubleScalarAtan2 ( _rtB -> B_1036_5509_0 .
im , _rtB -> B_1036_5509_0 . re ) ; _rtB -> B_1036_5511_0 [ 0 ] = _rtP ->
P_1225 * _rtB -> B_1036_5510_0 ; _rtB -> B_1036_5511_0 [ 1 ] = _rtP -> P_1225
* 0.0 ; _rtB -> B_1036_5511_0 [ 2 ] = _rtP -> P_1225 * 0.0 ; _rtB ->
B_1036_5511_0 [ 3 ] = _rtP -> P_1225 * 0.0 ; _rtB -> B_1036_5511_0 [ 4 ] =
_rtP -> P_1225 * 0.0 ; _rtB -> B_1036_5511_0 [ 5 ] = _rtP -> P_1225 * 0.0 ;
_rtB -> B_1036_5511_0 [ 6 ] = _rtP -> P_1225 * 0.0 ; _rtB -> B_1036_5511_0 [
7 ] = _rtP -> P_1225 * 0.0 ; _rtB -> B_1036_5511_0 [ 8 ] = _rtP -> P_1225 *
0.0 ; _rtB -> B_1036_5511_0 [ 9 ] = _rtP -> P_1225 * 0.0 ; _rtB ->
B_1036_5511_0 [ 10 ] = _rtP -> P_1225 * 0.0 ; _rtB -> B_1036_5512_0 = _rtP ->
P_1226 * _rtB -> B_1036_5510_1 ; _rtB -> B_1036_5513_0 [ 0 ] = _rtP -> P_1227
* _rtB -> B_1036_5512_0 ; _rtB -> B_1036_5513_0 [ 1 ] = _rtP -> P_1227 * 0.0
; _rtB -> B_1036_5513_0 [ 2 ] = _rtP -> P_1227 * 0.0 ; _rtB -> B_1036_5513_0
[ 3 ] = _rtP -> P_1227 * 0.0 ; _rtB -> B_1036_5513_0 [ 4 ] = _rtP -> P_1227 *
0.0 ; _rtB -> B_1036_5513_0 [ 5 ] = _rtP -> P_1227 * 0.0 ; _rtB ->
B_1036_5513_0 [ 6 ] = _rtP -> P_1227 * 0.0 ; _rtB -> B_1036_5513_0 [ 7 ] =
_rtP -> P_1227 * 0.0 ; _rtB -> B_1036_5513_0 [ 8 ] = _rtP -> P_1227 * 0.0 ;
_rtB -> B_1036_5513_0 [ 9 ] = _rtP -> P_1227 * 0.0 ; _rtB -> B_1036_5513_0 [
10 ] = _rtP -> P_1227 * 0.0 ; _rtB -> B_1036_5514_0 = muDoubleScalarCos (
_rtB -> B_1036_5513_0 [ 0 ] ) ; _rtB -> B_1036_5515_0 = _rtB -> B_1036_5511_0
[ 0 ] * _rtB -> B_1036_5514_0 ; _rtB -> B_1036_5516_0 = _rtB -> B_1036_5515_0
* _rtB -> B_1036_5515_0 ; _rtB -> B_1036_5517_0 = muDoubleScalarSin ( _rtB ->
B_1036_5513_0 [ 0 ] ) ; _rtB -> B_1036_5518_0 = _rtB -> B_1036_5517_0 * _rtB
-> B_1036_5511_0 [ 0 ] ; _rtB -> B_1036_5519_0 = _rtB -> B_1036_5518_0 * _rtB
-> B_1036_5518_0 ; _rtB -> B_1036_5520_0 = _rtB -> B_1036_5516_0 + _rtB ->
B_1036_5519_0 ; _rtB -> B_1036_5521_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_5520_0 ) ; _rtB -> B_1036_5522_0 = ( _rtB -> B_1036_5521_0 > _rtB ->
B_1036_2422_0_c ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_1036_5523_0 = _rtDW -> u4_PreviousInput_gt ; } _rtB ->
B_1036_5524_0 = false ; _rtB -> B_1036_5525_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5526_0 =
_rtB -> B_1036_2487_0_b ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_5526_0 , _rtB
-> B_1036_5524_0 , _rtB -> B_1036_5525_0 , _rtB -> B_1036_2485_0_f , & _rtB
-> ONDelay_ix , & _rtDW -> ONDelay_ix , & _rtZCE -> ONDelay_ix ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5528_0 =
_rtB -> B_1036_2488_0_b ; } DZG_v3_OFFDelay ( S , _rtB -> B_1036_5528_0 ,
_rtB -> B_1036_5524_0 , _rtB -> B_1036_5525_0 , _rtB -> B_1036_2485_0_f , &
_rtB -> OFFDelay_mb , & _rtDW -> OFFDelay_mb , & _rtZCE -> OFFDelay_mb ) ;
_rtB -> B_1036_5530_0 = ( _rtB -> ONDelay_ix . B_90_17_0 || _rtB ->
OFFDelay_mb . B_86_19_0 ) ; _rtB -> B_1036_5531_0 = ( _rtB -> B_1036_5523_0
|| _rtB -> B_1036_5530_0 ) ; _rtB -> B_1036_5532_0 = _rtB -> B_1036_2424_0_e
; _rtB -> B_1036_5533_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_1036_5534_0 = _rtB -> B_1036_2494_0_b ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_5534_0 , _rtB -> B_1036_5532_0 , _rtB ->
B_1036_5533_0 , _rtB -> B_1036_2492_0 , & _rtB -> ONDelay_g , & _rtDW ->
ONDelay_g , & _rtZCE -> ONDelay_g ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_1036_5536_0 = _rtB -> B_1036_2495_0 ; }
DZG_v3_OFFDelay ( S , _rtB -> B_1036_5536_0 , _rtB -> B_1036_5532_0 , _rtB ->
B_1036_5533_0 , _rtB -> B_1036_2492_0 , & _rtB -> OFFDelay_a2 , & _rtDW ->
OFFDelay_a2 , & _rtZCE -> OFFDelay_a2 ) ; _rtB -> B_1036_5538_0 = ( _rtB ->
ONDelay_g . B_90_17_0 || _rtB -> OFFDelay_a2 . B_86_19_0 ) ; _rtB ->
B_1036_5539_0 = ! _rtB -> B_1036_5538_0 ; _rtB -> B_1036_5540_0 = ( _rtB ->
B_1036_5531_0 && _rtB -> B_1036_5539_0 ) ; _rtB -> B_1036_5541_0 = ( _rtB ->
B_1036_5522_0 && _rtB -> B_1036_5540_0 ) ; if ( _rtB -> B_1036_5540_0 ) {
_rtB -> B_1036_5542_0 = _rtB -> B_1036_2442_0 ; } else { _rtB ->
B_1036_5542_0 = _rtB -> B_1036_2468_0 ; } _rtB -> B_1036_5543_0 = _rtB ->
B_1036_5515_0 * _rtB -> B_1036_5542_0 ; _rtB -> B_1036_5544_0 = _rtB ->
B_1036_5518_0 * _rtB -> B_1036_5542_0 ; _rtB -> B_1036_5545_0 = 0.0 * _rtB ->
B_1036_5542_0 ; _rtB -> B_1036_5546_0 = 0.0 * _rtB -> B_1036_5542_0 ; _rtB ->
B_1036_5547_0 = 0.0 * _rtB -> B_1036_5542_0 ; _rtB -> B_1036_5548_0 = 0.0 *
_rtB -> B_1036_5542_0 ; if ( _rtB -> B_1036_5489_0 ) { _rtB -> B_1036_5549_0
= _rtB -> B_1036_2450_0 ; } else { _rtB -> B_1036_5549_0 = _rtB ->
B_1036_2469_0 ; } _rtB -> B_1036_5550_0 = 0.0 * _rtB -> B_1036_5549_0 ; _rtB
-> B_1036_5551_0 = 0.0 * _rtB -> B_1036_5549_0 ; _rtB -> B_1036_5552_0 = 0.0
* _rtB -> B_1036_5549_0 ; _rtB -> B_1036_5553_0 = 0.0 * _rtB -> B_1036_5549_0
; _rtB -> B_1036_5554_0 = 0.0 * _rtB -> B_1036_5549_0 ; _rtB -> B_1036_5555_0
= 0.0 * _rtB -> B_1036_5549_0 ; _rtB -> B_1036_5556_0 = ! _rtB ->
B_1036_5540_0 ; _rtB -> B_1036_5557_0 = ! _rtB -> B_1036_5489_0 ; _rtB ->
B_1036_5558_0 = ( _rtB -> B_1036_5556_0 && _rtB -> B_1036_5557_0 ) ; _rtB ->
B_1036_5559_0 = _rtB -> B_1036_5543_0 + _rtB -> B_1036_5550_0 ; _rtB ->
B_1036_5560_0 = _rtB -> B_1036_5544_0 + _rtB -> B_1036_5551_0 ; _rtB ->
B_1036_5561_0 = _rtB -> B_1036_5545_0 + _rtB -> B_1036_5552_0 ; _rtB ->
B_1036_5562_0 = _rtB -> B_1036_5546_0 + _rtB -> B_1036_5553_0 ; _rtB ->
B_1036_5563_0 = _rtB -> B_1036_5547_0 + _rtB -> B_1036_5554_0 ; _rtB ->
B_1036_5564_0 = _rtB -> B_1036_5548_0 + _rtB -> B_1036_5555_0 ; _rtB ->
B_1036_5565_0 = ( _rtB -> B_1036_2423_0_m && _rtB -> B_1036_5540_0 ) ; _rtB
-> B_1036_5566_0 = ( _rtB -> B_1036_2433_0_i && _rtB -> B_1036_5540_0 ) ;
_rtB -> B_1036_5567_0 = ( _rtB -> B_1036_5540_0 && _rtB -> B_1036_5489_0 ) ;
if ( _rtB -> B_1036_5558_0 ) { _rtB -> B_1036_5569_0 = _rtB -> B_1036_2470_0
; _rtB -> B_1036_5571_0 = _rtB -> B_1036_2470_0 ; _rtB -> B_1036_5573_0 =
_rtB -> B_1036_2470_0 ; } else { if ( _rtB -> B_1036_5567_0 ) { _rtB ->
B_350_0_0 = ( _rtB -> B_1036_5541_0 || _rtB -> B_1036_5490_0 ) ; _rtB ->
B_350_1_0 = ( _rtB -> B_350_0_0 && _rtB -> B_1036_5567_0 ) ; _rtB ->
B_351_2_0 = _rtB -> B_350_1_0 ; } else { if ( _rtB -> B_1036_5540_0 ) { _rtB
-> B_349_0_0 = _rtB -> B_1036_5541_0 ; } else { _rtB -> B_349_0_0 = _rtB ->
B_1036_5490_0 ; } _rtB -> B_351_2_0 = _rtB -> B_349_0_0 ; } _rtB ->
B_1036_5569_0 = _rtB -> B_351_2_0 ; if ( _rtB -> B_1036_5567_0 ) { _rtB ->
B_347_0_0 = ( _rtB -> B_1036_5565_0 || _rtB -> B_1036_5491_0 ) ; _rtB ->
B_347_1_0 = ( _rtB -> B_347_0_0 && _rtB -> B_1036_5567_0 ) ; _rtB ->
B_348_2_0 = _rtB -> B_347_1_0 ; } else { if ( _rtB -> B_1036_5540_0 ) { _rtB
-> B_346_0_0 = _rtB -> B_1036_5565_0 ; } else { _rtB -> B_346_0_0 = _rtB ->
B_1036_5491_0 ; } _rtB -> B_348_2_0 = _rtB -> B_346_0_0 ; } _rtB ->
B_1036_5571_0 = _rtB -> B_348_2_0 ; if ( _rtB -> B_1036_5567_0 ) { _rtB ->
B_344_0_0 = ( _rtB -> B_1036_5566_0 || _rtB -> B_1036_5492_0 ) ; _rtB ->
B_344_1_0 = ( _rtB -> B_344_0_0 && _rtB -> B_1036_5567_0 ) ; _rtB ->
B_345_2_0 = _rtB -> B_344_1_0 ; } else { if ( _rtB -> B_1036_5540_0 ) { _rtB
-> B_343_0_0 = _rtB -> B_1036_5566_0 ; } else { _rtB -> B_343_0_0 = _rtB ->
B_1036_5492_0 ; } _rtB -> B_345_2_0 = _rtB -> B_343_0_0 ; } _rtB ->
B_1036_5573_0 = _rtB -> B_345_2_0 ; } if ( _rtB -> B_1036_5567_0 ) { _rtB ->
B_1036_5575_0 = _rtB -> B_1036_2457_0 ; } else { if ( _rtB -> B_1036_5540_0 )
{ _rtB -> B_342_0_0 = _rtB -> B_1036_2443_0 ; } else { _rtB -> B_342_0_0 =
_rtB -> B_1036_2451_0 ; } _rtB -> B_1036_5575_0 = _rtB -> B_342_0_0 ; } _rtB
-> B_1036_5576_0 = ( ( _rtB -> B_1036_5569_0 != 0.0 ) && ( _rtB ->
B_1036_5571_0 != 0.0 ) && ( _rtB -> B_1036_5573_0 != 0.0 ) ) ; _rtB ->
B_1036_5577_0 = 0.0 * _rtB -> B_1036_5542_0 ; _rtB -> B_1036_5578_0 = _rtB ->
B_1036_5577_0 / _rtB -> B_1036_2471_0 ; _rtB -> B_1036_5579_0 = 0.0 * _rtB ->
B_1036_5542_0 ; _rtB -> B_1036_5580_0 = _rtB -> B_1036_5579_0 / _rtB ->
B_1036_2471_0 ; _rtB -> B_1036_5581_0 = 0.0 * _rtB -> B_1036_5549_0 ; _rtB ->
B_1036_5582_0 = 0.0 * _rtB -> B_1036_5549_0 ; _rtB -> B_1036_5583_0 = _rtB ->
B_1036_5577_0 + _rtB -> B_1036_5581_0 ; _rtB -> B_1036_5584_0 = _rtB ->
B_1036_5583_0 / _rtB -> B_1036_2472_0 ; _rtB -> B_1036_5585_0 = _rtB ->
B_1036_5579_0 + _rtB -> B_1036_5582_0 ; _rtB -> B_1036_5586_0 = _rtB ->
B_1036_5585_0 / _rtB -> B_1036_2472_0 ; _rtB -> B_1036_5587_0 = _rtB ->
B_1036_5581_0 / _rtB -> B_1036_2473_0 ; _rtB -> B_1036_5588_0 = _rtB ->
B_1036_5582_0 / _rtB -> B_1036_2473_0 ; _rtB -> B_1036_5589_0 = ( _rtB ->
B_1036_2478_0_n && _rtB -> B_1036_5540_0 ) ; _rtB -> B_1036_5590_0 = ( _rtB
-> B_1036_2483_0_g && _rtB -> B_1036_5489_0 ) ; if ( _rtB -> B_1036_5558_0 )
{ _rtB -> B_1036_5592_0 = _rtB -> B_1036_2470_0 ; } else { if ( _rtB ->
B_1036_5567_0 ) { _rtB -> B_340_0_0 = ( _rtB -> B_1036_5589_0 || _rtB ->
B_1036_5590_0 ) ; _rtB -> B_340_1_0 = ( _rtB -> B_340_0_0 && _rtB ->
B_1036_5567_0 ) ; _rtB -> B_341_2_0 = _rtB -> B_340_1_0 ; } else { if ( _rtB
-> B_1036_5540_0 ) { _rtB -> B_339_0_0 = _rtB -> B_1036_5589_0 ; } else {
_rtB -> B_339_0_0 = _rtB -> B_1036_5590_0 ; } _rtB -> B_341_2_0 = _rtB ->
B_339_0_0 ; } _rtB -> B_1036_5592_0 = _rtB -> B_341_2_0 ; } _rtB ->
B_1036_5593_0 = ( _rtB -> B_1036_5541_0 && _rtB -> B_1036_5565_0 && _rtB ->
B_1036_5566_0 ) ; _rtB -> B_1036_5594_0 = ( _rtB -> B_1036_5490_0 && _rtB ->
B_1036_5491_0 && _rtB -> B_1036_5492_0 ) ; _rtB -> B_1036_5595_0 = _rtB ->
B_1036_5559_0 * _rtB -> B_1036_5559_0 ; _rtB -> B_1036_5596_0 = _rtB ->
B_1036_5560_0 * _rtB -> B_1036_5560_0 ; _rtB -> B_1036_5597_0 = _rtB ->
B_1036_5595_0 + _rtB -> B_1036_5596_0 ; _rtB -> B_1036_5598_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_5597_0 ) ; if ( _rtB -> B_1036_2518_0 >
_rtP -> P_1228 ) { _rtB -> B_508_0_0 = ( _rtB -> B_1036_5598_0 < _rtB ->
B_1036_2521_0 ) ; _rtB -> B_1036_5601_0 = _rtB -> B_508_0_0 ; } else { _rtB
-> B_507_0_0 = ( _rtB -> B_1036_5598_0 > _rtB -> B_1036_2521_0 ) ; _rtB ->
B_1036_5601_0 = _rtB -> B_507_0_0 ; } _rtB -> B_1036_5602_0 = ( _rtB ->
B_1036_5601_0 && _rtB -> B_1036_2523_0_k ) ; if ( _rtB -> B_1036_2516_0_n ) {
_rtB -> B_1036_5604_0 = _rtB -> B_1036_5602_0 ; } else { _rtB -> B_506_0_0 =
( _rtB -> B_1036_5602_0 && _rtB -> B_1036_2527_0_n ) ; _rtB -> B_1036_5604_0
= _rtB -> B_506_0_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_1036_5605_0 = _rtDW -> Memory_PreviousInput_mn ; _rtB ->
B_1036_5606_0 = ! _rtB -> B_1036_5605_0 ; } _rtB -> B_1036_5607_0 = ( _rtB ->
B_1036_5604_0 && _rtB -> B_1036_5606_0 ) ; _rtB -> B_1036_5608_0 = ssGetT ( S
) ; _rtB -> B_1036_5609_0 = ( _rtB -> B_1036_5604_0 && _rtB ->
B_1036_2542_0_b ) ; _rtB -> B_1036_5610_0 = _rtB -> B_1036_5609_0 ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5611_0 =
_rtB -> B_1036_2532_0 ; _rtB -> B_1036_5612_0 = _rtB -> B_1036_2531_0_h ; }
DZG_v3_ONDelay ( S , _rtB -> B_1036_5612_0 , _rtB -> B_1036_5610_0 , _rtB ->
B_1036_5608_0 , _rtB -> B_1036_2529_0 , & _rtB -> ONDelay_pn , & _rtDW ->
ONDelay_pn , & _rtZCE -> ONDelay_pn ) ; DZG_v3_OFFDelay ( S , _rtB ->
B_1036_5611_0 , _rtB -> B_1036_5610_0 , _rtB -> B_1036_5608_0 , _rtB ->
B_1036_2529_0 , & _rtB -> OFFDelay_a4 , & _rtDW -> OFFDelay_a4 , & _rtZCE ->
OFFDelay_a4 ) ; _rtB -> B_1036_5615_0 = ( _rtB -> ONDelay_pn . B_90_17_0 ||
_rtB -> OFFDelay_a4 . B_86_19_0 ) ; _rtB -> B_1036_5616_0 = ssGetT ( S ) ;
_rtB -> B_1036_5617_0 = false ; _rtB -> B_1036_5618_0 = _rtB -> B_1036_5617_0
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_1036_5619_0 = _rtB -> B_1036_2539_0_l ; _rtB -> B_1036_5620_0 = _rtB ->
B_1036_2538_0_k ; } DZG_v3_ONDelay ( S , _rtB -> B_1036_5620_0 , _rtB ->
B_1036_5618_0 , _rtB -> B_1036_5616_0 , _rtB -> B_1036_2536_0 , & _rtB ->
ONDelay_hy , & _rtDW -> ONDelay_hy , & _rtZCE -> ONDelay_hy ) ;
DZG_v3_OFFDelay ( S , _rtB -> B_1036_5619_0 , _rtB -> B_1036_5618_0 , _rtB ->
B_1036_5616_0 , _rtB -> B_1036_2536_0 , & _rtB -> OFFDelay_i5 , & _rtDW ->
OFFDelay_i5 , & _rtZCE -> OFFDelay_i5 ) ; _rtB -> B_1036_5623_0 = ( _rtB ->
ONDelay_hy . B_90_17_0 || _rtB -> OFFDelay_i5 . B_86_19_0 ) ; _rtB ->
B_1036_5624_0 = ( _rtB -> B_1036_5615_0 || _rtB -> B_1036_5623_0 ) ; _rtB ->
B_1036_5625_0 = muDoubleScalarSin ( _rtP -> P_1231 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1232 ) * _rtP -> P_1229 + _rtP -> P_1230 ; _rtB -> B_1036_5626_0
= _rtB -> B_1036_3_0_c [ 0 ] * _rtB -> B_1036_5625_0 ; _rtB -> B_1036_5627_0
= muDoubleScalarSin ( _rtP -> P_1235 * ssGetTaskTime ( S , 0 ) + _rtP ->
P_1236 ) * _rtP -> P_1233 + _rtP -> P_1234 ; _rtB -> B_1036_5628_0 = _rtB ->
B_1036_3_0_c [ 0 ] * _rtB -> B_1036_5627_0 ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_1036_5629_0 = _rtP -> P_1237 ; _rtB ->
B_1036_5630_0 = ssGetTaskTime ( S , 1 ) ; _rtB -> B_1036_5631_0 = rt_Lookup (
_rtP -> P_1238 , 6 , _rtB -> B_1036_5630_0 , _rtP -> P_1239 ) ; if ( _rtB ->
B_1036_2657_0 >= _rtP -> P_1240 ) { _rtB -> B_1036_5632_0 = _rtB ->
B_1036_2664_0 ; } else { _rtB -> B_1036_5632_0 = _rtB -> B_1036_5631_0 ; }
_rtB -> B_1036_5633_0 = ( _rtB -> B_1036_5632_0 != 0.0 ) ; _rtB ->
B_1036_5634_0 = ssGetTaskTime ( S , 1 ) ; if ( _rtB -> B_1036_5629_0 >= _rtP
-> P_1241 ) { _rtB -> B_1_0_0 = ( _rtB -> B_1036_5633_0 && _rtB ->
B_1036_2663_0 ) ; _rtB -> B_1_1_0 = _rtB -> B_1_0_0 ; _rtB -> B_1_2_0 = _rtB
-> B_1_1_0 ; _rtB -> B_1036_5637_0 = _rtB -> B_1_2_0 ; } else { _rtB ->
B_0_0_0 = rt_Lookup ( _rtP -> P_0 , 5 , _rtB -> B_1036_5634_0 , _rtP -> P_1 )
; _rtB -> B_1036_5637_0 = _rtB -> B_0_0_0 ; } _rtB -> B_1036_5641_0 = _rtP ->
P_1242 ; _rtB -> B_1036_5642_0 = ssGetTaskTime ( S , 1 ) ; if ( _rtB ->
B_1036_5641_0 >= _rtP -> P_1243 ) { _rtB -> B_3_0_0 = ( _rtB -> B_1036_5633_0
&& _rtB -> B_1036_2662_0_e ) ; _rtB -> B_3_1_0 = _rtB -> B_3_0_0 ; _rtB ->
B_3_2_0 = _rtB -> B_3_1_0 ; _rtB -> B_1036_5645_0 = _rtB -> B_3_2_0 ; } else
{ _rtB -> B_2_0_0 = rt_Lookup ( _rtP -> P_2 , 5 , _rtB -> B_1036_5642_0 ,
_rtP -> P_3 ) ; _rtB -> B_1036_5645_0 = _rtB -> B_2_0_0 ; } _rtB ->
B_1036_5649_0 = _rtP -> P_1244 ; _rtB -> B_1036_5650_0 = ssGetTaskTime ( S ,
1 ) ; if ( _rtB -> B_1036_5649_0 >= _rtP -> P_1245 ) { _rtB -> B_5_0_0 = (
_rtB -> B_1036_5633_0 && _rtB -> B_1036_2661_0_f ) ; _rtB -> B_5_1_0 = _rtB
-> B_5_0_0 ; _rtB -> B_5_2_0 = _rtB -> B_5_1_0 ; _rtB -> B_1036_5653_0 = _rtB
-> B_5_2_0 ; } else { _rtB -> B_4_0_0 = rt_Lookup ( _rtP -> P_4 , 5 , _rtB ->
B_1036_5650_0 , _rtP -> P_5 ) ; _rtB -> B_1036_5653_0 = _rtB -> B_4_0_0 ; }
_rtB -> B_1036_5663_0 = _rtP -> P_1246 ; _rtB -> B_1036_5664_0 =
ssGetTaskTime ( S , 1 ) ; _rtB -> B_1036_5665_0 = rt_Lookup ( _rtP -> P_1247
, 6 , _rtB -> B_1036_5664_0 , _rtP -> P_1248 ) ; if ( _rtB -> B_1036_2665_0
>= _rtP -> P_1249 ) { _rtB -> B_1036_5666_0 = _rtB -> B_1036_2672_0_a ; }
else { _rtB -> B_1036_5666_0 = _rtB -> B_1036_5665_0 ; } _rtB ->
B_1036_5667_0 = ( _rtB -> B_1036_5666_0 != 0.0 ) ; _rtB -> B_1036_5668_0 =
ssGetTaskTime ( S , 1 ) ; if ( _rtB -> B_1036_5663_0 >= _rtP -> P_1250 ) {
_rtB -> B_7_0_0 = ( _rtB -> B_1036_5667_0 && _rtB -> B_1036_2671_0_e ) ; _rtB
-> B_7_1_0 = _rtB -> B_7_0_0 ; _rtB -> B_7_2_0 = _rtB -> B_7_1_0 ; _rtB ->
B_1036_5671_0 = _rtB -> B_7_2_0 ; } else { _rtB -> B_6_0_0 = rt_Lookup ( _rtP
-> P_6 , 5 , _rtB -> B_1036_5668_0 , _rtP -> P_7 ) ; _rtB -> B_1036_5671_0 =
_rtB -> B_6_0_0 ; } _rtB -> B_1036_5675_0 = _rtP -> P_1251 ; _rtB ->
B_1036_5676_0 = ssGetTaskTime ( S , 1 ) ; if ( _rtB -> B_1036_5675_0 >= _rtP
-> P_1252 ) { _rtB -> B_9_0_0 = ( _rtB -> B_1036_5667_0 && _rtB ->
B_1036_2670_0_b ) ; _rtB -> B_9_1_0 = _rtB -> B_9_0_0 ; _rtB -> B_9_2_0 =
_rtB -> B_9_1_0 ; _rtB -> B_1036_5679_0 = _rtB -> B_9_2_0 ; } else { _rtB ->
B_8_0_0 = rt_Lookup ( _rtP -> P_8 , 5 , _rtB -> B_1036_5676_0 , _rtP -> P_9 )
; _rtB -> B_1036_5679_0 = _rtB -> B_8_0_0 ; } _rtB -> B_1036_5683_0 = _rtP ->
P_1253 ; _rtB -> B_1036_5684_0 = ssGetTaskTime ( S , 1 ) ; if ( _rtB ->
B_1036_5683_0 >= _rtP -> P_1254 ) { _rtB -> B_11_0_0 = ( _rtB ->
B_1036_5667_0 && _rtB -> B_1036_2669_0_i ) ; _rtB -> B_11_1_0 = _rtB ->
B_11_0_0 ; _rtB -> B_11_2_0 = _rtB -> B_11_1_0 ; _rtB -> B_1036_5687_0 = _rtB
-> B_11_2_0 ; } else { _rtB -> B_10_0_0 = rt_Lookup ( _rtP -> P_10 , 5 , _rtB
-> B_1036_5684_0 , _rtP -> P_11 ) ; _rtB -> B_1036_5687_0 = _rtB -> B_10_0_0
; } _rtB -> B_1036_5697_0 = _rtP -> P_1255 * _rtB -> B_1036_7_0 [ 12 ] ; _rtB
-> B_1036_5698_0 = _rtP -> P_1256 * _rtB -> B_1036_7_0 [ 13 ] ; _rtB ->
B_1036_5699_0 = _rtP -> P_1257 * _rtB -> B_1036_7_0 [ 14 ] ; _rtB ->
B_1036_5700_0 [ 0 ] = _rtP -> P_1258 * _rtB -> B_1036_5697_0 ; _rtB ->
B_1036_5700_0 [ 1 ] = _rtP -> P_1258 * _rtB -> B_1036_5698_0 ; _rtB ->
B_1036_5700_0 [ 2 ] = _rtP -> P_1258 * _rtB -> B_1036_5699_0 ; _rtB ->
B_1036_5735_0 = _rtP -> P_1259 * _rtB -> B_1036_7_0 [ 36 ] ; _rtB ->
B_1036_5736_0 = _rtP -> P_1260 * _rtB -> B_1036_7_0 [ 37 ] ; _rtB ->
B_1036_5737_0 = _rtP -> P_1261 * _rtB -> B_1036_7_0 [ 38 ] ; _rtB ->
B_1036_5738_0 [ 0 ] = _rtP -> P_1262 * _rtB -> B_1036_5735_0 ; _rtB ->
B_1036_5738_0 [ 1 ] = _rtP -> P_1262 * _rtB -> B_1036_5736_0 ; _rtB ->
B_1036_5738_0 [ 2 ] = _rtP -> P_1262 * _rtB -> B_1036_5737_0 ; _rtB ->
B_1036_5739_0 = _rtP -> P_1263 * _rtB -> B_1036_7_0 [ 15 ] ; _rtB ->
B_1036_5740_0 = _rtP -> P_1264 * _rtB -> B_1036_7_0 [ 16 ] ; _rtB ->
B_1036_5741_0 = _rtP -> P_1265 * _rtB -> B_1036_7_0 [ 17 ] ; _rtB ->
B_1036_5742_0 [ 0 ] = _rtP -> P_1266 * _rtB -> B_1036_5739_0 ; _rtB ->
B_1036_5742_0 [ 1 ] = _rtP -> P_1266 * _rtB -> B_1036_5740_0 ; _rtB ->
B_1036_5742_0 [ 2 ] = _rtP -> P_1266 * _rtB -> B_1036_5741_0 ; _rtB ->
B_1036_5777_0 = _rtP -> P_1267 * _rtB -> B_1036_7_0 [ 39 ] ; _rtB ->
B_1036_5778_0 = _rtP -> P_1268 * _rtB -> B_1036_7_0 [ 40 ] ; _rtB ->
B_1036_5779_0 = _rtP -> P_1269 * _rtB -> B_1036_7_0 [ 41 ] ; _rtB ->
B_1036_5780_0 [ 0 ] = _rtP -> P_1270 * _rtB -> B_1036_5777_0 ; _rtB ->
B_1036_5780_0 [ 1 ] = _rtP -> P_1270 * _rtB -> B_1036_5778_0 ; _rtB ->
B_1036_5780_0 [ 2 ] = _rtP -> P_1270 * _rtB -> B_1036_5779_0 ; _rtB ->
B_1036_5781_0 = _rtP -> P_1271 * _rtB -> B_1036_7_0 [ 18 ] ; _rtB ->
B_1036_5782_0 = _rtP -> P_1272 * _rtB -> B_1036_7_0 [ 19 ] ; _rtB ->
B_1036_5783_0 = _rtP -> P_1273 * _rtB -> B_1036_7_0 [ 20 ] ; _rtB ->
B_1036_5784_0 [ 0 ] = _rtP -> P_1274 * _rtB -> B_1036_5781_0 ; _rtB ->
B_1036_5784_0 [ 1 ] = _rtP -> P_1274 * _rtB -> B_1036_5782_0 ; _rtB ->
B_1036_5784_0 [ 2 ] = _rtP -> P_1274 * _rtB -> B_1036_5783_0 ; _rtB ->
B_1036_5819_0 = _rtP -> P_1275 * _rtB -> B_1036_7_0 [ 30 ] ; _rtB ->
B_1036_5820_0 = _rtP -> P_1276 * _rtB -> B_1036_7_0 [ 31 ] ; _rtB ->
B_1036_5821_0 = _rtP -> P_1277 * _rtB -> B_1036_7_0 [ 32 ] ; _rtB ->
B_1036_5822_0 [ 0 ] = _rtP -> P_1278 * _rtB -> B_1036_5819_0 ; _rtB ->
B_1036_5822_0 [ 1 ] = _rtP -> P_1278 * _rtB -> B_1036_5820_0 ; _rtB ->
B_1036_5822_0 [ 2 ] = _rtP -> P_1278 * _rtB -> B_1036_5821_0 ; _rtB ->
B_1036_5823_0 = _rtP -> P_1279 * _rtB -> B_1036_7_0 [ 9 ] ; _rtB ->
B_1036_5824_0 = _rtP -> P_1280 * _rtB -> B_1036_7_0 [ 10 ] ; _rtB ->
B_1036_5825_0 = _rtP -> P_1281 * _rtB -> B_1036_7_0 [ 11 ] ; _rtB ->
B_1036_5826_0 [ 0 ] = _rtP -> P_1282 * _rtB -> B_1036_5823_0 ; _rtB ->
B_1036_5826_0 [ 1 ] = _rtP -> P_1282 * _rtB -> B_1036_5824_0 ; _rtB ->
B_1036_5826_0 [ 2 ] = _rtP -> P_1282 * _rtB -> B_1036_5825_0 ; _rtB ->
B_1036_5861_0 = _rtP -> P_1283 * _rtB -> B_1036_7_0 [ 42 ] ; _rtB ->
B_1036_5862_0 = _rtP -> P_1284 * _rtB -> B_1036_7_0 [ 43 ] ; _rtB ->
B_1036_5863_0 = _rtP -> P_1285 * _rtB -> B_1036_7_0 [ 44 ] ; _rtB ->
B_1036_5864_0 [ 0 ] = _rtP -> P_1286 * _rtB -> B_1036_5861_0 ; _rtB ->
B_1036_5864_0 [ 1 ] = _rtP -> P_1286 * _rtB -> B_1036_5862_0 ; _rtB ->
B_1036_5864_0 [ 2 ] = _rtP -> P_1286 * _rtB -> B_1036_5863_0 ; _rtB ->
B_1036_5865_0 = _rtP -> P_1287 * _rtB -> B_1036_7_0 [ 21 ] ; _rtB ->
B_1036_5866_0 = _rtP -> P_1288 * _rtB -> B_1036_7_0 [ 22 ] ; _rtB ->
B_1036_5867_0 = _rtP -> P_1289 * _rtB -> B_1036_7_0 [ 23 ] ; _rtB ->
B_1036_5868_0 [ 0 ] = _rtP -> P_1290 * _rtB -> B_1036_5865_0 ; _rtB ->
B_1036_5868_0 [ 1 ] = _rtP -> P_1290 * _rtB -> B_1036_5866_0 ; _rtB ->
B_1036_5868_0 [ 2 ] = _rtP -> P_1290 * _rtB -> B_1036_5867_0 ; _rtB ->
B_1036_5937_0 = _rtP -> P_1291 * _rtB -> B_1036_7_0 [ 27 ] ; _rtB ->
B_1036_5938_0 = _rtP -> P_1292 * _rtB -> B_1036_7_0 [ 28 ] ; _rtB ->
B_1036_5939_0 = _rtP -> P_1293 * _rtB -> B_1036_7_0 [ 29 ] ; _rtB ->
B_1036_5940_0 [ 0 ] = _rtP -> P_1294 * _rtB -> B_1036_5937_0 ; _rtB ->
B_1036_5940_0 [ 1 ] = _rtP -> P_1294 * _rtB -> B_1036_5938_0 ; _rtB ->
B_1036_5940_0 [ 2 ] = _rtP -> P_1294 * _rtB -> B_1036_5939_0 ; _rtB ->
B_1036_5941_0 = _rtP -> P_1295 * _rtB -> B_1036_7_0 [ 6 ] ; _rtB ->
B_1036_5942_0 = _rtP -> P_1296 * _rtB -> B_1036_7_0 [ 7 ] ; _rtB ->
B_1036_5943_0 = _rtP -> P_1297 * _rtB -> B_1036_7_0 [ 8 ] ; _rtB ->
B_1036_5944_0 [ 0 ] = _rtP -> P_1298 * _rtB -> B_1036_5941_0 ; _rtB ->
B_1036_5944_0 [ 1 ] = _rtP -> P_1298 * _rtB -> B_1036_5942_0 ; _rtB ->
B_1036_5944_0 [ 2 ] = _rtP -> P_1298 * _rtB -> B_1036_5943_0 ; _rtB ->
B_1036_5979_0 = _rtP -> P_1299 ; _rtB -> B_1036_5980_0 = ssGetTaskTime ( S ,
1 ) ; _rtB -> B_1036_5981_0 = rt_Lookup ( _rtP -> P_1300 , 6 , _rtB ->
B_1036_5980_0 , _rtP -> P_1301 ) ; if ( _rtB -> B_1036_2673_0 >= _rtP ->
P_1302 ) { _rtB -> B_1036_5982_0 = _rtB -> B_1036_2678_0_h ; } else { _rtB ->
B_1036_5982_0 = _rtB -> B_1036_5981_0 ; } _rtB -> B_1036_5983_0 =
ssGetTaskTime ( S , 1 ) ; if ( _rtB -> B_1036_5979_0 >= _rtP -> P_1303 ) { if
( _rtB -> B_1036_2674_0 >= _rtP -> P_14 ) { _rtB -> B_13_0_0 = _rtB ->
B_1036_5982_0 ; } else { _rtB -> B_13_0_0 = _rtB -> B_1036_2677_0 ; } _rtB ->
B_13_1_0 = _rtB -> B_13_0_0 ; _rtB -> B_1036_5986_0 = _rtB -> B_13_1_0 ; }
else { _rtB -> B_12_0_0 = rt_Lookup ( _rtP -> P_12 , 5 , _rtB ->
B_1036_5983_0 , _rtP -> P_13 ) ; _rtB -> B_1036_5986_0 = _rtB -> B_12_0_0 ; }
_rtB -> B_1036_5990_0 = _rtP -> P_1304 ; _rtB -> B_1036_5991_0 =
ssGetTaskTime ( S , 1 ) ; if ( _rtB -> B_1036_5990_0 >= _rtP -> P_1305 ) { if
( _rtB -> B_1036_2675_0 >= _rtP -> P_17 ) { _rtB -> B_15_0_0 = _rtB ->
B_1036_5982_0 ; } else { _rtB -> B_15_0_0 = _rtB -> B_1036_2677_0 ; } _rtB ->
B_15_1_0 = _rtB -> B_15_0_0 ; _rtB -> B_1036_5994_0 = _rtB -> B_15_1_0 ; }
else { _rtB -> B_14_0_0 = rt_Lookup ( _rtP -> P_15 , 5 , _rtB ->
B_1036_5991_0 , _rtP -> P_16 ) ; _rtB -> B_1036_5994_0 = _rtB -> B_14_0_0 ; }
_rtB -> B_1036_5998_0 = _rtP -> P_1306 ; _rtB -> B_1036_5999_0 =
ssGetTaskTime ( S , 1 ) ; if ( _rtB -> B_1036_5998_0 >= _rtP -> P_1307 ) { if
( _rtB -> B_1036_2676_0 >= _rtP -> P_20 ) { _rtB -> B_17_0_0 = _rtB ->
B_1036_5982_0 ; } else { _rtB -> B_17_0_0 = _rtB -> B_1036_2677_0 ; } _rtB ->
B_17_1_0 = _rtB -> B_17_0_0 ; _rtB -> B_1036_6002_0 = _rtB -> B_17_1_0 ; }
else { _rtB -> B_16_0_0 = rt_Lookup ( _rtP -> P_18 , 5 , _rtB ->
B_1036_5999_0 , _rtP -> P_19 ) ; _rtB -> B_1036_6002_0 = _rtB -> B_16_0_0 ; }
} UNUSED_PARAMETER ( tid ) ; } static void mdlOutputsTID2 ( SimStruct * S ,
int_T tid ) { real_T minV ; int32_T i ; B_DZG_v3_T * _rtB ; P_DZG_v3_T * _rtP
; DW_DZG_v3_T * _rtDW ; _rtDW = ( ( DW_DZG_v3_T * ) ssGetRootDWork ( S ) ) ;
_rtP = ( ( P_DZG_v3_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( ( B_DZG_v3_T * )
_ssGetModelBlockIO ( S ) ) ; _rtB -> B_1036_0_0_m [ 0 ] = _rtP -> P_1308 [ 0
] ; _rtB -> B_1036_0_0_m [ 1 ] = _rtP -> P_1308 [ 1 ] ; _rtB -> B_1036_0_0_m
[ 2 ] = _rtP -> P_1308 [ 2 ] ; for ( i = 0 ; i < 6 ; i ++ ) { _rtB ->
B_1036_1_0_c [ i ] = _rtP -> P_1309 [ i ] ; } _rtB -> B_1036_2_0_k = _rtP ->
P_1310 * 0.0 ; _rtB -> B_1036_3_0_c [ 0 ] = _rtP -> P_1311 * 0.0 ; _rtB ->
B_1036_3_0_c [ 1 ] = _rtP -> P_1311 * 0.0 ; _rtB -> B_1036_3_0_c [ 2 ] = _rtP
-> P_1311 * 0.0 ; _rtB -> B_1036_4_0_b = _rtP -> P_1312 ; _rtB ->
B_1036_5_0_p = _rtP -> P_1313 ; _rtB -> B_1036_6_0_c = _rtP -> P_1314 ; _rtB
-> B_1036_7_0_f = _rtB -> B_1036_5_0_p - _rtB -> B_1036_6_0_c ; _rtB ->
B_1036_8_0_g = _rtB -> B_1036_5_0_p + _rtB -> B_1036_6_0_c ; _rtB ->
B_1036_9_0_g = _rtP -> P_1315 ; _rtB -> B_1036_10_0_m = _rtP -> P_1316 ; _rtB
-> B_1036_11_0_n = _rtP -> P_1317 ; _rtB -> B_1036_12_0 = _rtP -> P_1318 ;
_rtB -> B_1036_13_0_p = _rtP -> P_1319 ; _rtB -> B_1036_14_0 = _rtP -> P_1320
; _rtB -> B_1036_15_0_l = _rtP -> P_1321 ; _rtB -> B_1036_16_0_j = _rtP ->
P_1322 ; _rtB -> B_1036_17_0_d = _rtP -> P_1323 ; _rtB -> B_1036_18_0_g =
_rtP -> P_1324 ; _rtB -> B_1036_19_0_l = _rtB -> B_1036_7_0_f * _rtB ->
B_1036_17_0_d ; _rtB -> B_1036_20_0 = _rtB -> B_1036_8_0_g * _rtB ->
B_1036_17_0_d ; _rtB -> B_1036_21_0_d = _rtP -> P_1325 ; _rtB -> B_1036_22_0
= _rtP -> P_1326 ; _rtB -> B_1036_23_0_d = _rtP -> P_1327 ; _rtB ->
B_1036_24_0_l = _rtP -> P_1328 ; _rtB -> B_1036_25_0_o = _rtB ->
B_1036_10_0_m * _rtB -> B_1036_23_0_d ; _rtB -> B_1036_26_0_b =
muDoubleScalarCos ( _rtB -> B_1036_25_0_o ) ; _rtB -> B_1036_27_0_n =
muDoubleScalarSin ( _rtB -> B_1036_25_0_o ) ; _rtB -> B_1036_28_0_b = _rtB ->
B_1036_27_0_n * _rtB -> B_1036_22_0 ; _rtB -> B_1036_29_0_l = _rtB ->
B_1036_13_0_p * _rtB -> B_1036_24_0_l ; _rtB -> B_1036_30_0 = _rtP -> P_1329
; _rtB -> B_1036_31_0_h = _rtP -> P_1330 ; _rtB -> B_1036_32_0 = _rtP ->
P_1331 ; _rtB -> B_1036_33_0_b = _rtP -> P_1332 ; _rtB -> B_1036_34_0_d =
_rtB -> B_1036_10_0_m * _rtB -> B_1036_32_0 ; _rtB -> B_1036_35_0_e =
muDoubleScalarCos ( _rtB -> B_1036_34_0_d ) ; _rtB -> B_1036_36_0_b =
muDoubleScalarSin ( _rtB -> B_1036_34_0_d ) ; _rtB -> B_1036_37_0_j = _rtB ->
B_1036_36_0_b * _rtB -> B_1036_31_0_h ; _rtB -> B_1036_38_0 = _rtB ->
B_1036_13_0_p * _rtB -> B_1036_33_0_b ; _rtB -> B_1036_39_0_f = _rtP ->
P_1333 ; _rtB -> B_1036_40_0 = _rtP -> P_1334 ; _rtB -> B_1036_41_0_a = _rtP
-> P_1335 ; _rtB -> B_1036_42_0_j = _rtP -> P_1336 ; _rtB -> B_1036_43_0_j =
_rtB -> B_1036_10_0_m * _rtB -> B_1036_41_0_a ; _rtB -> B_1036_44_0_o =
muDoubleScalarCos ( _rtB -> B_1036_43_0_j ) ; _rtB -> B_1036_45_0_n =
muDoubleScalarSin ( _rtB -> B_1036_43_0_j ) ; _rtB -> B_1036_46_0_i = _rtB ->
B_1036_45_0_n * _rtB -> B_1036_40_0 ; _rtB -> B_1036_47_0_o = _rtB ->
B_1036_13_0_p * _rtB -> B_1036_42_0_j ; _rtB -> B_1036_48_0 = _rtP -> P_1337
; _rtB -> B_1036_49_0_n = _rtP -> P_1338 ; _rtB -> B_1036_50_0 = _rtP ->
P_1339 ; _rtB -> B_1036_51_0_m = _rtP -> P_1340 ; _rtB -> B_1036_52_0_c =
_rtP -> P_1341 ; _rtB -> B_1036_53_0_m = _rtP -> P_1342 ; _rtB ->
B_1036_54_0_m = _rtP -> P_1343 ; _rtB -> B_1036_55_0_j = _rtP -> P_1344 ;
_rtB -> B_1036_56_0 = _rtP -> P_1345 ; _rtB -> B_1036_57_0_h = _rtP -> P_1346
; _rtB -> B_1036_58_0 = _rtP -> P_1347 ; _rtB -> B_1036_59_0_c = _rtP ->
P_1348 ; _rtB -> B_1036_60_0_c = _rtP -> P_1349 ; _rtB -> B_1036_61_0_p =
_rtP -> P_1350 ; _rtB -> B_1036_62_0_p = _rtP -> P_1351 ; _rtB ->
B_1036_63_0_a = _rtP -> P_1352 ; _rtB -> B_1036_64_0_e = _rtP -> P_1353 ;
_rtB -> B_1036_65_0_a = _rtP -> P_1354 ; _rtB -> B_1036_66_0 = _rtP -> P_1355
; _rtB -> B_1036_67_0_a = _rtP -> P_1356 ; _rtB -> B_1036_68_0 = _rtP ->
P_1357 ; _rtB -> B_1036_69_0_i = _rtP -> P_1358 ; _rtB -> B_1036_70_0_l =
_rtP -> P_1359 ; _rtB -> B_1036_71_0_o = _rtP -> P_1360 ; _rtB ->
B_1036_72_0_o = _rtP -> P_1361 ; _rtB -> B_1036_73_0_i = _rtP -> P_1362 ;
_rtB -> B_1036_74_0 = _rtP -> P_1363 ; _rtB -> B_1036_75_0_f = _rtP -> P_1364
; _rtB -> B_1036_76_0 = _rtP -> P_1365 ; _rtB -> B_1036_77_0_i = _rtP ->
P_1366 ; _rtB -> B_1036_78_0_f = _rtP -> P_1367 ; _rtB -> B_1036_79_0_g =
_rtP -> P_1368 ; _rtB -> B_1036_80_0_c = _rtP -> P_1369 ; _rtB ->
B_1036_81_0_o = _rtP -> P_1370 ; _rtB -> B_1036_82_0_l = _rtP -> P_1371 ;
_rtB -> B_1036_83_0_m = _rtP -> P_1372 ; _rtB -> B_1036_84_0 = _rtP -> P_1373
; _rtB -> B_1036_85_0_m = _rtP -> P_1374 ; _rtB -> B_1036_86_0 = _rtP ->
P_1375 ; _rtB -> B_1036_87_0_c = _rtP -> P_1376 ; _rtB -> B_1036_88_0_f =
_rtP -> P_1377 ; _rtB -> B_1036_89_0_p = _rtP -> P_1378 ; _rtB ->
B_1036_90_0_e = _rtP -> P_1379 ; _rtB -> B_1036_91_0_o = _rtP -> P_1380 ;
_rtB -> B_1036_92_0 = _rtP -> P_1381 ; _rtB -> B_1036_93_0_h = _rtP -> P_1382
; _rtB -> B_1036_94_0 = _rtP -> P_1383 ; _rtB -> B_1036_95_0_l = _rtP ->
P_1384 ; _rtB -> B_1036_96_0_h = _rtP -> P_1385 ; _rtB -> B_1036_97_0_m =
_rtP -> P_1386 ; _rtB -> B_1036_98_0_m = _rtP -> P_1387 ; _rtB ->
B_1036_99_0_h = _rtP -> P_1388 ; _rtB -> B_1036_100_0_c = _rtP -> P_1389 ;
_rtB -> B_1036_101_0_k = _rtP -> P_1390 ; _rtB -> B_1036_102_0 = _rtP ->
P_1391 ; _rtB -> B_1036_103_0_p = _rtP -> P_1392 ; _rtB -> B_1036_104_0 =
_rtP -> P_1393 ; _rtB -> B_1036_105_0_p = _rtP -> P_1394 ; _rtB ->
B_1036_106_0_p = _rtP -> P_1395 ; _rtB -> B_1036_107_0_a = _rtP -> P_1396 ;
_rtB -> B_1036_108_0_j = _rtP -> P_1397 ; _rtB -> B_1036_109_0_e = _rtP ->
P_1398 ; _rtB -> B_1036_110_0 = _rtP -> P_1399 ; _rtB -> B_1036_111_0_o =
_rtP -> P_1400 ; _rtB -> B_1036_112_0 = _rtP -> P_1401 ; _rtB ->
B_1036_113_0_b = _rtP -> P_1402 ; _rtB -> B_1036_114_0_a = _rtP -> P_1403 ;
_rtB -> B_1036_115_0_g = _rtP -> P_1404 ; _rtB -> B_1036_116_0_e = _rtP ->
P_1405 ; _rtB -> B_1036_117_0_f = _rtP -> P_1406 ; _rtB -> B_1036_118_0_h =
_rtP -> P_1407 ; _rtB -> B_1036_119_0_e = _rtP -> P_1408 ; _rtB ->
B_1036_120_0 = _rtP -> P_1409 ; _rtB -> B_1036_121_0_c = _rtP -> P_1410 ;
_rtB -> B_1036_122_0 = _rtP -> P_1411 ; _rtB -> B_1036_123_0_a = _rtP ->
P_1412 ; _rtB -> B_1036_124_0_d = _rtP -> P_1413 ; _rtB -> B_1036_125_0_a =
_rtP -> P_1414 ; _rtB -> B_1036_126_0_p = _rtP -> P_1415 ; _rtB ->
B_1036_127_0_m = _rtP -> P_1416 ; _rtB -> B_1036_128_0 = _rtP -> P_1417 ;
_rtB -> B_1036_129_0_o = _rtP -> P_1418 ; _rtB -> B_1036_130_0 = _rtP ->
P_1419 ; _rtB -> B_1036_131_0_n = _rtP -> P_1420 ; _rtB -> B_1036_132_0_e =
true ; _rtB -> B_1036_133_0_l = _rtP -> P_1421 ; _rtB -> B_1036_134_0_m =
false ; _rtB -> B_1036_135_0_d = ! _rtB -> B_1036_134_0_m ; _rtB ->
B_1036_136_0_p = _rtP -> P_1422 ; _rtB -> B_1036_137_0_p = _rtP -> P_1423 ;
_rtB -> B_1036_138_0 = _rtP -> P_1424 ; _rtB -> B_1036_139_0_c = ( _rtB ->
B_1036_137_0_p == _rtB -> B_1036_138_0 ) ; _rtB -> B_1036_140_0 = _rtP ->
P_1425 ; _rtB -> B_1036_141_0_i = ( _rtB -> B_1036_140_0 == _rtB ->
B_1036_138_0 ) ; _rtB -> B_1036_142_0_k = ( _rtB -> B_1036_139_0_c || _rtB ->
B_1036_141_0_i ) ; _rtB -> B_1036_143_0_f = _rtB -> B_1036_137_0_p / _rtB ->
B_1036_140_0 ; if ( _rtB -> B_1036_142_0_k ) { _rtB -> B_1036_144_0_i = _rtB
-> B_1036_136_0_p ; } else { _rtB -> B_1036_144_0_i = _rtB -> B_1036_143_0_f
; } _rtB -> B_1036_145_0_o = 0.0 * _rtB -> B_1036_144_0_i ; _rtB ->
B_1036_146_0 = _rtB -> B_1036_145_0_o / _rtB -> B_1036_133_0_l ; _rtB ->
B_1036_147_0_k = 0.0 * _rtB -> B_1036_144_0_i ; _rtB -> B_1036_148_0 = _rtB
-> B_1036_147_0_k / _rtB -> B_1036_133_0_l ; _rtB -> B_1036_149_0_i = 0.0 *
_rtB -> B_1036_144_0_i ; _rtB -> B_1036_150_0_o = _rtB -> B_1036_149_0_i /
_rtB -> B_1036_133_0_l ; _rtB -> B_1036_151_0_m = 0.0 * _rtB ->
B_1036_144_0_i ; _rtB -> B_1036_152_0_c = _rtB -> B_1036_151_0_m / _rtB ->
B_1036_133_0_l ; _rtB -> B_1036_153_0_f = 0.0 ; _rtB -> B_1036_154_0_h = 0.0
; _rtB -> B_1036_155_0_m = _rtB -> B_1036_153_0_f + _rtB -> B_1036_154_0_h ;
_rtB -> B_1036_156_0 = muDoubleScalarSqrt ( _rtB -> B_1036_155_0_m ) ; _rtB
-> B_1036_157_0_a = _rtP -> P_1426 ; _rtB -> B_1036_158_0 = ( _rtB ->
B_1036_156_0 > _rtB -> B_1036_157_0_a ) ; _rtB -> B_1036_159_0_k = 0.0 ; _rtB
-> B_1036_160_0_p = 0.0 ; _rtB -> B_1036_161_0_b = _rtB -> B_1036_159_0_k +
_rtB -> B_1036_160_0_p ; _rtB -> B_1036_162_0_c = muDoubleScalarSqrt ( _rtB
-> B_1036_161_0_b ) ; _rtB -> B_1036_163_0_c = ( _rtB -> B_1036_162_0_c >
_rtB -> B_1036_157_0_a ) ; _rtB -> B_1036_164_0_f = ( _rtB -> B_1036_158_0 &&
_rtB -> B_1036_163_0_c ) ; _rtB -> B_1036_165_0_j = ! _rtB -> B_1036_164_0_f
; _rtB -> B_1036_166_0 = 0.0 * _rtB -> B_1036_144_0_i ; _rtB ->
B_1036_167_0_n = _rtB -> B_1036_166_0 / _rtB -> B_1036_133_0_l ; _rtB ->
B_1036_168_0_i = 0.0 * _rtB -> B_1036_144_0_i ; _rtB -> B_1036_169_0_m = _rtB
-> B_1036_168_0_i / _rtB -> B_1036_133_0_l ; _rtB -> B_1036_170_0_j = 0.0 ;
_rtB -> B_1036_171_0_e = 0.0 ; _rtB -> B_1036_172_0_m = _rtB ->
B_1036_170_0_j + _rtB -> B_1036_171_0_e ; _rtB -> B_1036_173_0_m =
muDoubleScalarSqrt ( _rtB -> B_1036_172_0_m ) ; _rtB -> B_1036_174_0_i = (
_rtB -> B_1036_173_0_m > _rtB -> B_1036_157_0_a ) ; _rtB -> B_1036_175_0_c =
! _rtB -> B_1036_174_0_i ; _rtB -> B_1036_176_0 = ! _rtB -> B_1036_158_0 ;
_rtB -> B_1036_177_0_d = ! _rtB -> B_1036_163_0_c ; _rtB -> B_1036_178_0_a =
( _rtB -> B_1036_175_0_c && _rtB -> B_1036_176_0 && _rtB -> B_1036_177_0_d )
; _rtB -> B_1036_179_0_j = _rtB -> B_1036_146_0 * _rtB -> B_1036_146_0 ; _rtB
-> B_1036_180_0_f = _rtB -> B_1036_152_0_c * _rtB -> B_1036_152_0_c ; _rtB ->
B_1036_181_0_a = _rtB -> B_1036_179_0_j + _rtB -> B_1036_180_0_f ; _rtB ->
B_1036_182_0 = muDoubleScalarSqrt ( _rtB -> B_1036_181_0_a ) ; _rtB ->
B_1036_183_0_g = _rtB -> B_1036_167_0_n * _rtB -> B_1036_167_0_n ; _rtB ->
B_1036_184_0 = _rtB -> B_1036_169_0_m * _rtB -> B_1036_169_0_m ; _rtB ->
B_1036_185_0_n = _rtB -> B_1036_183_0_g + _rtB -> B_1036_184_0 ; _rtB ->
B_1036_186_0_d = muDoubleScalarSqrt ( _rtB -> B_1036_185_0_n ) ; _rtB ->
B_1036_187_0_n = _rtB -> B_1036_148_0 * _rtB -> B_1036_148_0 ; _rtB ->
B_1036_188_0_c = _rtB -> B_1036_150_0_o * _rtB -> B_1036_150_0_o ; _rtB ->
B_1036_189_0_f = _rtB -> B_1036_187_0_n + _rtB -> B_1036_188_0_c ; _rtB ->
B_1036_190_0_p = muDoubleScalarSqrt ( _rtB -> B_1036_189_0_f ) ; _rtB ->
B_1036_191_0_h = false ; _rtB -> B_1036_192_0_k = false ; _rtB ->
B_1036_193_0_p = _rtP -> P_1427 ; _rtB -> B_1036_194_0 = ( _rtB ->
B_1036_192_0_k || ( _rtB -> B_1036_193_0_p != 0.0 ) || ( _rtB ->
B_1036_193_0_p != 0.0 ) || ( _rtB -> B_1036_193_0_p != 0.0 ) ) ; _rtB ->
B_1036_195_0_n = _rtP -> P_1428 ; _rtB -> B_1036_196_0_k = ( ( _rtB ->
B_1036_195_0_n != 0.0 ) || ( _rtB -> B_1036_193_0_p != 0.0 ) || ( _rtB ->
B_1036_193_0_p != 0.0 ) || ( _rtB -> B_1036_193_0_p != 0.0 ) ) ; _rtB ->
B_1036_197_0_k = _rtP -> P_1429 ; _rtB -> B_1036_198_0_n = _rtP -> P_1430 ;
_rtB -> B_1036_199_0_o = _rtP -> P_1431 ; _rtB -> B_1036_200_0 = _rtP ->
P_1432 ; _rtB -> B_1036_201_0_n = ! _rtB -> B_1036_194_0 ; _rtB ->
B_1036_202_0 = ! ( _rtB -> B_1036_193_0_p != 0.0 ) ; _rtB -> B_1036_203_0_j =
( _rtB -> B_1036_202_0 && ( _rtB -> B_1036_199_0_o != 0.0 ) ) ; _rtB ->
B_1036_204_0_b = ( ( _rtB -> B_1036_198_0_n != 0.0 ) && ( _rtB ->
B_1036_193_0_p != 0.0 ) ) ; _rtB -> B_1036_205_0_e = ! ( _rtB ->
B_1036_193_0_p != 0.0 ) ; _rtB -> B_1036_206_0_m = ( _rtB -> B_1036_205_0_e
&& ( _rtB -> B_1036_199_0_o != 0.0 ) ) ; _rtB -> B_1036_207_0_o = ( _rtB ->
B_1036_204_0_b || _rtB -> B_1036_206_0_m ) ; _rtB -> B_1036_208_0_k = ( (
_rtB -> B_1036_198_0_n != 0.0 ) && ( _rtB -> B_1036_193_0_p != 0.0 ) ) ; _rtB
-> B_1036_209_0_a = ! ( _rtB -> B_1036_193_0_p != 0.0 ) ; _rtB ->
B_1036_210_0_n = ( _rtB -> B_1036_209_0_a && ( _rtB -> B_1036_199_0_o != 0.0
) ) ; _rtB -> B_1036_211_0_p = ( _rtB -> B_1036_208_0_k || _rtB ->
B_1036_210_0_n ) ; _rtB -> B_1036_212_0 = ( ( _rtB -> B_1036_198_0_n != 0.0 )
&& ( _rtB -> B_1036_193_0_p != 0.0 ) ) ; _rtB -> B_1036_213_0_d = ( _rtB ->
B_1036_212_0 || _rtB -> B_1036_203_0_j ) ; _rtB -> B_1036_214_0_a = ! ( _rtB
-> B_1036_193_0_p != 0.0 ) ; _rtB -> B_1036_215_0_e = ! ( _rtB ->
B_1036_193_0_p != 0.0 ) ; _rtB -> B_1036_216_0_m = ! ( _rtB -> B_1036_193_0_p
!= 0.0 ) ; _rtB -> B_1036_217_0_g = _rtP -> P_1433 ; _rtB -> B_1036_218_0_f =
( _rtB -> B_1036_192_0_k || ( _rtB -> B_1036_217_0_g != 0.0 ) || ( _rtB ->
B_1036_217_0_g != 0.0 ) || ( _rtB -> B_1036_217_0_g != 0.0 ) ) ; _rtB ->
B_1036_219_0_h = false ; _rtB -> B_1036_220_0 = ( _rtB -> B_1036_219_0_h || (
_rtB -> B_1036_217_0_g != 0.0 ) || ( _rtB -> B_1036_217_0_g != 0.0 ) || (
_rtB -> B_1036_217_0_g != 0.0 ) ) ; _rtB -> B_1036_221_0_c = _rtP -> P_1434 ;
_rtB -> B_1036_222_0_b = ( ( _rtB -> B_1036_221_0_c != 0.0 ) || ( _rtB ->
B_1036_217_0_g != 0.0 ) || ( _rtB -> B_1036_217_0_g != 0.0 ) || ( _rtB ->
B_1036_217_0_g != 0.0 ) ) ; _rtB -> B_1036_223_0_c = _rtP -> P_1435 ; _rtB ->
B_1036_224_0_m = _rtP -> P_1436 ; _rtB -> B_1036_225_0_j = _rtP -> P_1437 ;
_rtB -> B_1036_226_0_k = _rtP -> P_1438 ; _rtB -> B_1036_227_0_f = ! _rtB ->
B_1036_218_0_f ; _rtB -> B_1036_228_0_l = ( _rtB -> B_1036_222_0_b && _rtB ->
B_1036_227_0_f ) ; _rtB -> B_1036_229_0_p = false ; _rtB -> B_1036_230_0 =
true ; _rtB -> B_1036_231_0_n = ( ( _rtB -> B_1036_225_0_j != 0.0 ) && _rtB
-> B_1036_230_0 ) ; _rtB -> B_1036_232_0_i = ( _rtB -> B_1036_229_0_p || _rtB
-> B_1036_231_0_n ) ; _rtB -> B_1036_233_0_m = _rtP -> P_1439 ; _rtB ->
B_1036_234_0_b = ( _rtB -> B_1036_192_0_k || ( _rtB -> B_1036_233_0_m != 0.0
) || ( _rtB -> B_1036_233_0_m != 0.0 ) || ( _rtB -> B_1036_233_0_m != 0.0 ) )
; _rtB -> B_1036_235_0_p = _rtP -> P_1440 ; _rtB -> B_1036_236_0 = _rtP ->
P_1441 ; _rtB -> B_1036_237_0_d = _rtP -> P_1442 ; _rtB -> B_1036_238_0 =
_rtP -> P_1443 ; _rtB -> B_1036_239_0_e = false ; _rtB -> B_1036_240_0_p = (
_rtB -> B_1036_239_0_e || ( _rtB -> B_1036_233_0_m != 0.0 ) ) ; _rtB ->
B_1036_241_0_o = ( _rtB -> B_1036_191_0_h || ( _rtB -> B_1036_233_0_m != 0.0
) ) ; _rtB -> B_1036_242_0_f = ! _rtB -> B_1036_234_0_b ; _rtB ->
B_1036_243_0_d = ! _rtB -> B_1036_234_0_b ; _rtB -> B_1036_244_0_i = ! _rtB
-> B_1036_234_0_b ; _rtB -> B_1036_245_0_g = _rtP -> P_1444 ; _rtB ->
B_1036_246_0 = _rtP -> P_1445 ; _rtB -> B_1036_247_0_c = _rtP -> P_1446 ;
_rtB -> B_1036_248_0 = ( _rtB -> B_1036_247_0_c == _rtB -> B_1036_245_0_g ) ;
_rtB -> B_1036_249_0_i = ! _rtB -> B_1036_248_0 ; DZG_v3_OFFDelayTID2 ( S , &
_rtB -> OFFDelay , & _rtDW -> OFFDelay , & _rtP -> OFFDelay ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay , & _rtDW -> ONDelay , & _rtP ->
ONDelay ) ; _rtB -> B_1036_252_0_c = _rtP -> P_1447 ; _rtB -> B_1036_253_0_i
= _rtP -> P_1448 ; _rtB -> B_1036_254_0 = _rtP -> P_1449 ; _rtB ->
B_1036_255_0_h = ( _rtB -> B_1036_254_0 == _rtB -> B_1036_252_0_c ) ; _rtB ->
B_1036_256_0 = ! _rtB -> B_1036_255_0_h ; DZG_v3_OFFDelayTID2 ( S , & _rtB ->
OFFDelay_d , & _rtDW -> OFFDelay_d , & _rtP -> OFFDelay_d ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_o , & _rtDW -> ONDelay_o , & _rtP
-> ONDelay_o ) ; _rtB -> B_1036_259_0_d = _rtP -> P_1450 ; _rtB ->
B_1036_260_0_g = _rtP -> P_1451 ; _rtB -> B_1036_261_0_l = _rtP -> P_1452 ;
_rtB -> B_1036_262_0_f = ( _rtB -> B_1036_261_0_l == _rtB -> B_1036_259_0_d )
; _rtB -> B_1036_263_0_h = ! _rtB -> B_1036_262_0_f ; DZG_v3_OFFDelayTID2 ( S
, & _rtB -> OFFDelay_o , & _rtDW -> OFFDelay_o , & _rtP -> OFFDelay_o ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_e , & _rtDW -> ONDelay_e , & _rtP
-> ONDelay_e ) ; _rtB -> B_1036_266_0_f = _rtP -> P_1453 ; _rtB ->
B_1036_267_0_d = _rtP -> P_1454 ; _rtB -> B_1036_268_0_j = _rtP -> P_1455 ;
_rtB -> B_1036_269_0_l = ( _rtB -> B_1036_268_0_j == _rtB -> B_1036_266_0_f )
; _rtB -> B_1036_270_0_g = ! _rtB -> B_1036_269_0_l ; DZG_v3_OFFDelayTID2 ( S
, & _rtB -> OFFDelay_i , & _rtDW -> OFFDelay_i , & _rtP -> OFFDelay_i ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_eu , & _rtDW -> ONDelay_eu , &
_rtP -> ONDelay_eu ) ; _rtB -> B_1036_273_0_i = _rtP -> P_1456 ; _rtB ->
B_1036_274_0_h = _rtP -> P_1457 ; _rtB -> B_1036_275_0_n = _rtP -> P_1458 ;
_rtB -> B_1036_276_0_l = ( _rtB -> B_1036_275_0_n == _rtB -> B_1036_273_0_i )
; _rtB -> B_1036_277_0_m = ! _rtB -> B_1036_276_0_l ; DZG_v3_OFFDelayTID2 ( S
, & _rtB -> OFFDelay_f , & _rtDW -> OFFDelay_f , & _rtP -> OFFDelay_f ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_l , & _rtDW -> ONDelay_l , & _rtP
-> ONDelay_l ) ; _rtB -> B_1036_280_0_o = _rtP -> P_1459 ; _rtB ->
B_1036_281_0 = _rtP -> P_1460 ; _rtB -> B_1036_282_0 = _rtP -> P_1461 ; _rtB
-> B_1036_283_0_n = ( _rtB -> B_1036_282_0 == _rtB -> B_1036_280_0_o ) ; _rtB
-> B_1036_284_0 = ! _rtB -> B_1036_283_0_n ; DZG_v3_OFFDelayTID2 ( S , & _rtB
-> OFFDelay_g , & _rtDW -> OFFDelay_g , & _rtP -> OFFDelay_g ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_h , & _rtDW -> ONDelay_h , & _rtP
-> ONDelay_h ) ; _rtB -> B_1036_287_0 = _rtP -> P_1462 ; _rtB ->
B_1036_288_0_c = _rtP -> P_1463 ; _rtB -> B_1036_289_0 = _rtP -> P_1464 ;
_rtB -> B_1036_290_0 = ( _rtB -> B_1036_289_0 == _rtB -> B_1036_287_0 ) ;
_rtB -> B_1036_291_0_n = ! _rtB -> B_1036_290_0 ; DZG_v3_OFFDelayTID2 ( S , &
_rtB -> OFFDelay_c , & _rtDW -> OFFDelay_c , & _rtP -> OFFDelay_c ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_k , & _rtDW -> ONDelay_k , & _rtP
-> ONDelay_k ) ; _rtB -> B_1036_294_0 = _rtP -> P_1465 ; _rtB -> B_1036_295_0
= _rtP -> P_1466 ; _rtB -> B_1036_296_0_b = _rtP -> P_1467 ; _rtB ->
B_1036_297_0_l = ( _rtB -> B_1036_296_0_b == _rtB -> B_1036_294_0 ) ; _rtB ->
B_1036_298_0_p = ! _rtB -> B_1036_297_0_l ; DZG_v3_OFFDelayTID2 ( S , & _rtB
-> OFFDelay_oq , & _rtDW -> OFFDelay_oq , & _rtP -> OFFDelay_oq ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_p , & _rtDW -> ONDelay_p , & _rtP
-> ONDelay_p ) ; _rtB -> B_1036_301_0 = _rtP -> P_1468 ; _rtB -> B_1036_302_0
= _rtP -> P_1469 ; _rtB -> B_1036_303_0 = _rtP -> P_1470 ; _rtB ->
B_1036_304_0 = ( _rtB -> B_1036_303_0 == _rtB -> B_1036_301_0 ) ; _rtB ->
B_1036_305_0_l = ! _rtB -> B_1036_304_0 ; DZG_v3_OFFDelayTID2 ( S , & _rtB ->
OFFDelay_p , & _rtDW -> OFFDelay_p , & _rtP -> OFFDelay_p ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_b , & _rtDW -> ONDelay_b , & _rtP
-> ONDelay_b ) ; _rtB -> B_1036_308_0_e = _rtP -> P_1471 ; _rtB ->
B_1036_309_0 = _rtP -> P_1472 ; _rtB -> B_1036_310_0 = _rtP -> P_1473 ; _rtB
-> B_1036_311_0_p = ( _rtB -> B_1036_310_0 == _rtB -> B_1036_308_0_e ) ; _rtB
-> B_1036_312_0 = ! _rtB -> B_1036_311_0_p ; DZG_v3_OFFDelayTID2 ( S , & _rtB
-> OFFDelay_e , & _rtDW -> OFFDelay_e , & _rtP -> OFFDelay_e ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_f , & _rtDW -> ONDelay_f , & _rtP
-> ONDelay_f ) ; _rtB -> B_1036_315_0 = _rtP -> P_1474 ; _rtB ->
B_1036_316_0_d = _rtP -> P_1475 ; _rtB -> B_1036_317_0_i = _rtP -> P_1476 ;
_rtB -> B_1036_318_0_m = ( _rtB -> B_1036_317_0_i == _rtB -> B_1036_315_0 ) ;
_rtB -> B_1036_319_0_c = ! _rtB -> B_1036_318_0_m ; DZG_v3_OFFDelayTID2 ( S ,
& _rtB -> OFFDelay_ip , & _rtDW -> OFFDelay_ip , & _rtP -> OFFDelay_ip ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_e5 , & _rtDW -> ONDelay_e5 , &
_rtP -> ONDelay_e5 ) ; _rtB -> B_1036_322_0_g = _rtP -> P_1477 ; _rtB ->
B_1036_323_0_n = _rtP -> P_1478 ; _rtB -> B_1036_324_0_l = _rtP -> P_1479 ;
_rtB -> B_1036_325_0_a = ( _rtB -> B_1036_324_0_l == _rtB -> B_1036_322_0_g )
; _rtB -> B_1036_326_0_g = ! _rtB -> B_1036_325_0_a ; DZG_v3_OFFDelayTID2 ( S
, & _rtB -> OFFDelay_dc , & _rtDW -> OFFDelay_dc , & _rtP -> OFFDelay_dc ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_n , & _rtDW -> ONDelay_n , & _rtP
-> ONDelay_n ) ; _rtB -> B_1036_329_0 = _rtP -> P_1480 ; _rtB ->
B_1036_330_0_c = _rtP -> P_1481 ; _rtB -> B_1036_331_0_n = _rtP -> P_1482 ;
_rtB -> B_1036_332_0_c = ( _rtB -> B_1036_331_0_n == _rtB -> B_1036_329_0 ) ;
_rtB -> B_1036_333_0_c = ! _rtB -> B_1036_332_0_c ; DZG_v3_OFFDelayTID2 ( S ,
& _rtB -> OFFDelay_n , & _rtDW -> OFFDelay_n , & _rtP -> OFFDelay_n ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_j , & _rtDW -> ONDelay_j , & _rtP
-> ONDelay_j ) ; _rtB -> B_1036_336_0_m = ! ( _rtB -> B_1036_237_0_d != 0.0 )
; _rtB -> B_1036_337_0_b = ! ( _rtB -> B_1036_235_0_p != 0.0 ) ; _rtB ->
B_1036_338_0_e = ! ( _rtB -> B_1036_236_0 != 0.0 ) ; _rtB -> B_1036_339_0_b =
( _rtB -> B_1036_336_0_m && _rtB -> B_1036_337_0_b && _rtB -> B_1036_338_0_e
) ; _rtB -> B_1036_340_0_p = _rtP -> P_1483 ; _rtB -> B_1036_341_0_d = _rtP
-> P_1484 ; _rtB -> B_1036_342_0_o [ 0 ] = _rtP -> P_1485 [ 0 ] ; _rtB ->
B_1036_343_0_j [ 0 ] = _rtP -> P_1486 [ 0 ] ; _rtB -> B_1036_344_0_c [ 0 ] =
_rtP -> P_1487 [ 0 ] ; _rtB -> B_1036_342_0_o [ 1 ] = _rtP -> P_1485 [ 1 ] ;
_rtB -> B_1036_343_0_j [ 1 ] = _rtP -> P_1486 [ 1 ] ; _rtB -> B_1036_344_0_c
[ 1 ] = _rtP -> P_1487 [ 1 ] ; switch ( ( int32_T ) _rtB -> B_1036_341_0_d )
{ case 1 : _rtB -> B_1036_345_0_h [ 0 ] = _rtB -> B_1036_342_0_o [ 0 ] ; _rtB
-> B_1036_345_0_h [ 1 ] = _rtB -> B_1036_342_0_o [ 1 ] ; break ; case 2 :
_rtB -> B_1036_345_0_h [ 0 ] = _rtB -> B_1036_343_0_j [ 0 ] ; _rtB ->
B_1036_345_0_h [ 1 ] = _rtB -> B_1036_343_0_j [ 1 ] ; break ; default : _rtB
-> B_1036_345_0_h [ 0 ] = _rtB -> B_1036_344_0_c [ 0 ] ; _rtB ->
B_1036_345_0_h [ 1 ] = _rtB -> B_1036_344_0_c [ 1 ] ; break ; } _rtB ->
B_1036_346_0_d = _rtP -> P_1488 ; _rtB -> B_1036_347_0 = _rtP -> P_1489 ;
_rtB -> B_1036_348_0_c [ 0 ] = _rtP -> P_1490 [ 0 ] ; _rtB -> B_1036_349_0_p
[ 0 ] = _rtP -> P_1491 [ 0 ] ; _rtB -> B_1036_350_0 [ 0 ] = _rtP -> P_1492 [
0 ] ; _rtB -> B_1036_348_0_c [ 1 ] = _rtP -> P_1490 [ 1 ] ; _rtB ->
B_1036_349_0_p [ 1 ] = _rtP -> P_1491 [ 1 ] ; _rtB -> B_1036_350_0 [ 1 ] =
_rtP -> P_1492 [ 1 ] ; switch ( ( int32_T ) _rtB -> B_1036_347_0 ) { case 1 :
_rtB -> B_1036_351_0 [ 0 ] = _rtB -> B_1036_348_0_c [ 0 ] ; _rtB ->
B_1036_351_0 [ 1 ] = _rtB -> B_1036_348_0_c [ 1 ] ; break ; case 2 : _rtB ->
B_1036_351_0 [ 0 ] = _rtB -> B_1036_349_0_p [ 0 ] ; _rtB -> B_1036_351_0 [ 1
] = _rtB -> B_1036_349_0_p [ 1 ] ; break ; default : _rtB -> B_1036_351_0 [ 0
] = _rtB -> B_1036_350_0 [ 0 ] ; _rtB -> B_1036_351_0 [ 1 ] = _rtB ->
B_1036_350_0 [ 1 ] ; break ; } _rtB -> B_1036_352_0_p = _rtP -> P_1493 ; _rtB
-> B_1036_353_0_a = _rtP -> P_1494 ; _rtB -> B_1036_354_0_o [ 0 ] = _rtP ->
P_1495 [ 0 ] ; _rtB -> B_1036_355_0_j [ 0 ] = _rtP -> P_1496 [ 0 ] ; _rtB ->
B_1036_356_0_p [ 0 ] = _rtP -> P_1497 [ 0 ] ; _rtB -> B_1036_354_0_o [ 1 ] =
_rtP -> P_1495 [ 1 ] ; _rtB -> B_1036_355_0_j [ 1 ] = _rtP -> P_1496 [ 1 ] ;
_rtB -> B_1036_356_0_p [ 1 ] = _rtP -> P_1497 [ 1 ] ; switch ( ( int32_T )
_rtB -> B_1036_353_0_a ) { case 1 : _rtB -> B_1036_357_0_o [ 0 ] = _rtB ->
B_1036_354_0_o [ 0 ] ; _rtB -> B_1036_357_0_o [ 1 ] = _rtB -> B_1036_354_0_o
[ 1 ] ; break ; case 2 : _rtB -> B_1036_357_0_o [ 0 ] = _rtB ->
B_1036_355_0_j [ 0 ] ; _rtB -> B_1036_357_0_o [ 1 ] = _rtB -> B_1036_355_0_j
[ 1 ] ; break ; default : _rtB -> B_1036_357_0_o [ 0 ] = _rtB ->
B_1036_356_0_p [ 0 ] ; _rtB -> B_1036_357_0_o [ 1 ] = _rtB -> B_1036_356_0_p
[ 1 ] ; break ; } _rtB -> B_1036_358_0_l = _rtP -> P_1498 ; _rtB ->
B_1036_359_0 = _rtP -> P_1499 ; _rtB -> B_1036_360_0_k = _rtP -> P_1500 ;
_rtB -> B_1036_361_0_j = _rtP -> P_1501 ; _rtB -> B_1036_362_0 = _rtP ->
P_1502 ; _rtB -> B_1036_363_0_i = true ; _rtB -> B_1036_364_0_l = true ; _rtB
-> B_1036_365_0 = _rtP -> P_1503 ; _rtB -> B_1036_366_0_d = true ; _rtB ->
B_1036_367_0 = _rtP -> P_1504 ; _rtB -> B_1036_368_0 = _rtP -> P_1505 ; _rtB
-> B_1036_369_0_f = true ; _rtB -> B_1036_370_0_g = true ; _rtB ->
B_1036_371_0_l = true ; _rtB -> B_1036_372_0_o = true ; _rtB ->
B_1036_373_0_i = true ; _rtB -> B_1036_374_0_d = true ; _rtB -> B_1036_375_0
= _rtP -> P_1506 ; _rtB -> B_1036_376_0 = _rtP -> P_1507 ; _rtB ->
B_1036_377_0 = _rtP -> P_1508 ; _rtB -> B_1036_378_0_a = ( _rtB ->
B_1036_377_0 == _rtB -> B_1036_375_0 ) ; _rtB -> B_1036_379_0_o = ! _rtB ->
B_1036_378_0_a ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_fu , & _rtDW ->
OFFDelay_fu , & _rtP -> OFFDelay_fu ) ; DZG_v3_ONDelayTID2 ( S , & _rtB ->
ONDelay_i , & _rtDW -> ONDelay_i , & _rtP -> ONDelay_i ) ; _rtB ->
B_1036_382_0 = _rtP -> P_1509 ; _rtB -> B_1036_383_0 = _rtP -> P_1510 ; _rtB
-> B_1036_384_0 = _rtP -> P_1511 ; _rtB -> B_1036_385_0 = _rtP -> P_1512 ;
_rtB -> B_1036_386_0 = _rtP -> P_1513 ; _rtB -> B_1036_387_0 = _rtP -> P_1514
; _rtB -> B_1036_388_0 = _rtP -> P_1515 ; _rtB -> B_1036_389_0 = _rtP ->
P_1516 ; _rtB -> B_1036_390_0_i = ( _rtB -> B_1036_389_0 == _rtB ->
B_1036_387_0 ) ; _rtB -> B_1036_391_0_m = ! _rtB -> B_1036_390_0_i ;
DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_b , & _rtDW -> OFFDelay_b , &
_rtP -> OFFDelay_b ) ; DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_pu , &
_rtDW -> ONDelay_pu , & _rtP -> ONDelay_pu ) ; _rtB -> B_1036_394_0 = _rtP ->
P_1517 ; _rtB -> B_1036_395_0_f = _rtP -> P_1518 ; _rtB -> B_1036_396_0_c =
_rtP -> P_1519 ; _rtB -> B_1036_397_0_f = _rtP -> P_1520 ; _rtB ->
B_1036_398_0_n = _rtP -> P_1521 ; _rtB -> B_1036_399_0_i = _rtP -> P_1522 ;
_rtB -> B_1036_400_0_l = _rtP -> P_1523 ; _rtB -> B_1036_401_0_i = _rtP ->
P_1524 ; _rtB -> B_1036_402_0_h = ( _rtB -> B_1036_401_0_i == _rtB ->
B_1036_399_0_i ) ; _rtB -> B_1036_403_0_l = ! _rtB -> B_1036_402_0_h ;
DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_nd , & _rtDW -> OFFDelay_nd , &
_rtP -> OFFDelay_nd ) ; DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_ih , &
_rtDW -> ONDelay_ih , & _rtP -> ONDelay_ih ) ; _rtB -> B_1036_406_0_k = _rtP
-> P_1525 ; _rtB -> B_1036_407_0_f = _rtP -> P_1526 ; _rtB -> B_1036_408_0_a
= _rtP -> P_1527 ; _rtB -> B_1036_409_0_d = _rtP -> P_1528 ; _rtB ->
B_1036_410_0_e = _rtP -> P_1529 ; _rtB -> B_1036_411_0_a = true ; _rtB ->
B_1036_412_0 = true ; _rtB -> B_1036_413_0_e = _rtP -> P_1530 ; _rtB ->
B_1036_414_0_n = true ; _rtB -> B_1036_415_0_b = _rtP -> P_1531 ; _rtB ->
B_1036_416_0_a = _rtP -> P_1532 ; _rtB -> B_1036_417_0_j = true ; _rtB ->
B_1036_418_0_l = true ; _rtB -> B_1036_419_0_g = true ; _rtB ->
B_1036_420_0_m = true ; _rtB -> B_1036_421_0_d = true ; _rtB ->
B_1036_422_0_l = true ; _rtB -> B_1036_423_0_i = _rtP -> P_1533 ; _rtB ->
B_1036_424_0_f = _rtP -> P_1534 ; _rtB -> B_1036_425_0_j = _rtP -> P_1535 ;
_rtB -> B_1036_426_0_d = ( _rtB -> B_1036_425_0_j == _rtB -> B_1036_423_0_i )
; _rtB -> B_1036_427_0_j = ! _rtB -> B_1036_426_0_d ; DZG_v3_OFFDelayTID2 ( S
, & _rtB -> OFFDelay_a , & _rtDW -> OFFDelay_a , & _rtP -> OFFDelay_a ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_hb , & _rtDW -> ONDelay_hb , &
_rtP -> ONDelay_hb ) ; _rtB -> B_1036_430_0_o = _rtP -> P_1536 ; _rtB ->
B_1036_431_0_f = _rtP -> P_1537 ; _rtB -> B_1036_432_0 = _rtP -> P_1538 ;
_rtB -> B_1036_433_0_o = _rtP -> P_1539 ; _rtB -> B_1036_434_0_l = _rtP ->
P_1540 ; _rtB -> B_1036_435_0_l = _rtP -> P_1541 ; _rtB -> B_1036_436_0 =
_rtP -> P_1542 ; _rtB -> B_1036_437_0 = _rtP -> P_1543 ; _rtB ->
B_1036_438_0_j = ( _rtB -> B_1036_437_0 == _rtB -> B_1036_435_0_l ) ; _rtB ->
B_1036_439_0_f = ! _rtB -> B_1036_438_0_j ; DZG_v3_OFFDelayTID2 ( S , & _rtB
-> OFFDelay_m , & _rtDW -> OFFDelay_m , & _rtP -> OFFDelay_m ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_ja , & _rtDW -> ONDelay_ja , &
_rtP -> ONDelay_ja ) ; _rtB -> B_1036_442_0_g = _rtP -> P_1544 ; _rtB ->
B_1036_443_0 = _rtP -> P_1545 ; _rtB -> B_1036_444_0 = _rtP -> P_1546 ; _rtB
-> B_1036_445_0 = _rtP -> P_1547 ; _rtB -> B_1036_446_0 = _rtP -> P_1548 ;
_rtB -> B_1036_447_0 = _rtP -> P_1549 ; _rtB -> B_1036_448_0_d = _rtP ->
P_1550 ; _rtB -> B_1036_449_0 = _rtP -> P_1551 ; _rtB -> B_1036_450_0_g = (
_rtB -> B_1036_449_0 == _rtB -> B_1036_447_0 ) ; _rtB -> B_1036_451_0_n = !
_rtB -> B_1036_450_0_g ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_er , &
_rtDW -> OFFDelay_er , & _rtP -> OFFDelay_er ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_m , & _rtDW -> ONDelay_m , & _rtP -> ONDelay_m ) ; _rtB ->
B_1036_454_0_d = _rtP -> P_1552 ; _rtB -> B_1036_455_0_j = _rtP -> P_1553 ;
_rtB -> B_1036_456_0 = _rtP -> P_1554 ; _rtB -> B_1036_457_0_f = _rtP ->
P_1555 ; _rtB -> B_1036_458_0 = _rtP -> P_1556 ; _rtB -> B_1036_459_0_j =
_rtP -> P_1557 ; _rtB -> B_1036_460_0_e = ! ( _rtB -> B_1036_459_0_j != 0.0 )
; _rtB -> B_1036_461_0_h = _rtP -> P_1558 ; _rtB -> B_1036_462_0_c = _rtP ->
P_1559 ; _rtB -> B_1036_463_0_d = true ; _rtB -> B_1036_464_0 = _rtP ->
P_1560 ; _rtB -> B_1036_465_0_n = _rtP -> P_1561 ; _rtB -> B_1036_466_0 =
_rtP -> P_1562 ; _rtB -> B_1036_467_0_k = _rtP -> P_1563 ; _rtB ->
B_1036_468_0_a = _rtP -> P_1564 ; _rtB -> B_1036_469_0_f = _rtP -> P_1565 ;
_rtB -> B_1036_470_0_a = ( _rtB -> B_1036_466_0 == _rtB -> B_1036_469_0_f ) ;
_rtB -> B_1036_471_0_h = ( _rtB -> B_1036_464_0 == _rtB -> B_1036_469_0_f ) ;
_rtB -> B_1036_472_0_b = ( _rtB -> B_1036_470_0_a || _rtB -> B_1036_471_0_h )
; _rtB -> B_1036_473_0_j = _rtB -> B_1036_466_0 / _rtB -> B_1036_464_0 ; if (
_rtB -> B_1036_472_0_b ) { _rtB -> B_1036_474_0 = _rtB -> B_1036_468_0_a ; }
else { _rtB -> B_1036_474_0 = _rtB -> B_1036_473_0_j ; } _rtB ->
B_1036_475_0_k = _rtB -> B_1036_474_0 * _rtB -> B_1036_465_0_n ; _rtB ->
B_1036_476_0 = _rtP -> P_1566 ; _rtB -> B_1036_477_0_b = _rtP -> P_1567 ;
_rtB -> B_1036_478_0_i = ( _rtB -> B_1036_467_0_k == _rtB -> B_1036_477_0_b )
; _rtB -> B_1036_479_0_p = ( _rtB -> B_1036_461_0_h == _rtB -> B_1036_477_0_b
) ; _rtB -> B_1036_480_0_b = ( _rtB -> B_1036_478_0_i || _rtB ->
B_1036_479_0_p ) ; _rtB -> B_1036_481_0_h = _rtB -> B_1036_467_0_k / _rtB ->
B_1036_461_0_h ; if ( _rtB -> B_1036_480_0_b ) { _rtB -> B_1036_482_0 = _rtB
-> B_1036_476_0 ; } else { _rtB -> B_1036_482_0 = _rtB -> B_1036_481_0_h ; }
_rtB -> B_1036_483_0_e = _rtB -> B_1036_482_0 * _rtB -> B_1036_462_0_c ; _rtB
-> B_1036_484_0 = 0.0 ; _rtB -> B_1036_485_0_h = 0.0 ; _rtB -> B_1036_486_0_k
= _rtB -> B_1036_484_0 + _rtB -> B_1036_485_0_h ; _rtB -> B_1036_487_0_j =
muDoubleScalarSqrt ( _rtB -> B_1036_486_0_k ) ; _rtB -> B_1036_488_0_m = (
_rtB -> B_1036_487_0_j > _rtB -> B_1036_462_0_c ) ; minV = muDoubleScalarMin
( _rtB -> B_1036_475_0_k , _rtB -> B_1036_483_0_e ) ; _rtB -> B_1036_489_0_o
= minV ; _rtB -> B_1036_490_0_c = 0.0 ; _rtB -> B_1036_491_0_h = 0.0 ; _rtB
-> B_1036_492_0 = _rtB -> B_1036_490_0_c + _rtB -> B_1036_491_0_h ; _rtB ->
B_1036_493_0_i = muDoubleScalarSqrt ( _rtB -> B_1036_492_0 ) ; _rtB ->
B_1036_494_0 = ( _rtB -> B_1036_493_0_i > _rtB -> B_1036_462_0_c ) ; _rtB ->
B_1036_495_0_p = 0.0 ; _rtB -> B_1036_496_0_f = 0.0 ; _rtB -> B_1036_497_0_e
= _rtB -> B_1036_495_0_p + _rtB -> B_1036_496_0_f ; _rtB -> B_1036_498_0_n =
muDoubleScalarSqrt ( _rtB -> B_1036_497_0_e ) ; _rtB -> B_1036_499_0_g = (
_rtB -> B_1036_498_0_n > _rtB -> B_1036_462_0_c ) ; _rtB -> B_1036_500_0 =
_rtP -> P_1568 ; _rtB -> B_1036_501_0_h = _rtP -> P_1569 ; _rtB ->
B_1036_502_0 = _rtP -> P_1570 ; _rtB -> B_1036_503_0_h = _rtP -> P_1571 ;
_rtB -> B_1036_504_0_f = _rtP -> P_1572 ; _rtB -> B_1036_505_0_i = _rtP ->
P_1573 ; _rtB -> B_1036_506_0_f = 0.0 ; _rtB -> B_1036_507_0_c = 0.0 ; _rtB
-> B_1036_508_0_n = _rtB -> B_1036_506_0_f + _rtB -> B_1036_507_0_c ; _rtB ->
B_1036_509_0_h = muDoubleScalarSqrt ( _rtB -> B_1036_508_0_n ) ; _rtB ->
B_1036_510_0_k = ( _rtB -> B_1036_509_0_h > _rtB -> B_1036_465_0_n ) ; _rtB
-> B_1036_511_0_k = 0.0 ; _rtB -> B_1036_512_0 = 0.0 ; _rtB -> B_1036_513_0_h
= _rtB -> B_1036_511_0_k + _rtB -> B_1036_512_0 ; _rtB -> B_1036_514_0_b =
muDoubleScalarSqrt ( _rtB -> B_1036_513_0_h ) ; _rtB -> B_1036_515_0_g = (
_rtB -> B_1036_514_0_b > _rtB -> B_1036_462_0_c ) ; _rtB -> B_1036_516_0_o =
_rtP -> P_1574 ; _rtB -> B_1036_517_0_n = _rtP -> P_1575 ; _rtB ->
B_1036_518_0 = _rtP -> P_1576 ; _rtB -> B_1036_519_0_p = ( _rtB ->
B_1036_518_0 == _rtB -> B_1036_516_0_o ) ; _rtB -> B_1036_520_0 = ! _rtB ->
B_1036_519_0_p ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_mh , & _rtDW ->
OFFDelay_mh , & _rtP -> OFFDelay_mh ) ; DZG_v3_ONDelayTID2 ( S , & _rtB ->
ONDelay_kb , & _rtDW -> ONDelay_kb , & _rtP -> ONDelay_kb ) ; _rtB ->
B_1036_523_0_m = _rtP -> P_1577 ; _rtB -> B_1036_524_0_k = _rtP -> P_1578 ;
_rtB -> B_1036_525_0_j = _rtP -> P_1579 ; _rtB -> B_1036_526_0_f = ( _rtB ->
B_1036_525_0_j == _rtB -> B_1036_523_0_m ) ; _rtB -> B_1036_527_0_h = ! _rtB
-> B_1036_526_0_f ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_ic , & _rtDW
-> OFFDelay_ic , & _rtP -> OFFDelay_ic ) ; DZG_v3_ONDelayTID2 ( S , & _rtB ->
ONDelay_m4 , & _rtDW -> ONDelay_m4 , & _rtP -> ONDelay_m4 ) ; _rtB ->
B_1036_530_0 = _rtP -> P_1580 ; _rtB -> B_1036_531_0_h = _rtP -> P_1581 ;
_rtB -> B_1036_532_0_f = _rtP -> P_1582 ; _rtB -> B_1036_533_0_h = ( _rtB ->
B_1036_532_0_f == _rtB -> B_1036_530_0 ) ; _rtB -> B_1036_534_0_b = ! _rtB ->
B_1036_533_0_h ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_ig , & _rtDW ->
OFFDelay_ig , & _rtP -> OFFDelay_ig ) ; DZG_v3_ONDelayTID2 ( S , & _rtB ->
ONDelay_c , & _rtDW -> ONDelay_c , & _rtP -> ONDelay_c ) ; _rtB ->
B_1036_537_0_d = _rtP -> P_1583 ; _rtB -> B_1036_538_0 = _rtP -> P_1584 ;
_rtB -> B_1036_539_0_l = _rtP -> P_1585 ; _rtB -> B_1036_540_0_n = ( _rtB ->
B_1036_539_0_l == _rtB -> B_1036_537_0_d ) ; _rtB -> B_1036_541_0_e = ! _rtB
-> B_1036_540_0_n ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_fp , & _rtDW
-> OFFDelay_fp , & _rtP -> OFFDelay_fp ) ; DZG_v3_ONDelayTID2 ( S , & _rtB ->
ONDelay_jz , & _rtDW -> ONDelay_jz , & _rtP -> ONDelay_jz ) ; _rtB ->
B_1036_544_0_l = true ; _rtB -> B_1036_545_0_k = _rtP -> P_1586 ; _rtB ->
B_1036_546_0 = _rtP -> P_1587 ; _rtB -> B_1036_547_0_j = true ; _rtB ->
B_1036_548_0 = _rtP -> P_1588 ; _rtB -> B_1036_549_0_i = _rtP -> P_1589 ;
_rtB -> B_1036_550_0_h = _rtP -> P_1590 ; _rtB -> B_1036_551_0_m = _rtP ->
P_1591 ; _rtB -> B_1036_552_0_g = _rtP -> P_1592 ; _rtB -> B_1036_553_0_l =
_rtP -> P_1593 ; _rtB -> B_1036_554_0_m = ( _rtB -> B_1036_550_0_h == _rtB ->
B_1036_553_0_l ) ; _rtB -> B_1036_555_0_i = ( _rtB -> B_1036_548_0 == _rtB ->
B_1036_553_0_l ) ; _rtB -> B_1036_556_0 = ( _rtB -> B_1036_554_0_m || _rtB ->
B_1036_555_0_i ) ; _rtB -> B_1036_557_0_m = _rtB -> B_1036_550_0_h / _rtB ->
B_1036_548_0 ; if ( _rtB -> B_1036_556_0 ) { _rtB -> B_1036_558_0_n = _rtB ->
B_1036_552_0_g ; } else { _rtB -> B_1036_558_0_n = _rtB -> B_1036_557_0_m ; }
_rtB -> B_1036_559_0_g = _rtB -> B_1036_558_0_n * _rtB -> B_1036_549_0_i ;
_rtB -> B_1036_560_0_d = _rtP -> P_1594 ; _rtB -> B_1036_561_0_m = _rtP ->
P_1595 ; _rtB -> B_1036_562_0_b = ( _rtB -> B_1036_551_0_m == _rtB ->
B_1036_561_0_m ) ; _rtB -> B_1036_563_0_e = ( _rtB -> B_1036_545_0_k == _rtB
-> B_1036_561_0_m ) ; _rtB -> B_1036_564_0_h = ( _rtB -> B_1036_562_0_b ||
_rtB -> B_1036_563_0_e ) ; _rtB -> B_1036_565_0_f = _rtB -> B_1036_551_0_m /
_rtB -> B_1036_545_0_k ; if ( _rtB -> B_1036_564_0_h ) { _rtB -> B_1036_566_0
= _rtB -> B_1036_560_0_d ; } else { _rtB -> B_1036_566_0 = _rtB ->
B_1036_565_0_f ; } _rtB -> B_1036_567_0_g = _rtB -> B_1036_566_0 * _rtB ->
B_1036_546_0 ; _rtB -> B_1036_568_0_j = 0.0 ; _rtB -> B_1036_569_0_c = 0.0 ;
_rtB -> B_1036_570_0_e = _rtB -> B_1036_568_0_j + _rtB -> B_1036_569_0_c ;
_rtB -> B_1036_571_0_m = muDoubleScalarSqrt ( _rtB -> B_1036_570_0_e ) ; _rtB
-> B_1036_572_0_n = ( _rtB -> B_1036_571_0_m > _rtB -> B_1036_546_0 ) ; minV
= muDoubleScalarMin ( _rtB -> B_1036_559_0_g , _rtB -> B_1036_567_0_g ) ;
_rtB -> B_1036_573_0_o = minV ; _rtB -> B_1036_574_0 = 0.0 ; _rtB ->
B_1036_575_0_a = 0.0 ; _rtB -> B_1036_576_0_j = _rtB -> B_1036_574_0 + _rtB
-> B_1036_575_0_a ; _rtB -> B_1036_577_0_g = muDoubleScalarSqrt ( _rtB ->
B_1036_576_0_j ) ; _rtB -> B_1036_578_0_g = ( _rtB -> B_1036_577_0_g > _rtB
-> B_1036_546_0 ) ; _rtB -> B_1036_579_0_j = 0.0 ; _rtB -> B_1036_580_0_e =
0.0 ; _rtB -> B_1036_581_0_j = _rtB -> B_1036_579_0_j + _rtB ->
B_1036_580_0_e ; _rtB -> B_1036_582_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_581_0_j ) ; _rtB -> B_1036_583_0_i = ( _rtB -> B_1036_582_0 > _rtB ->
B_1036_546_0 ) ; _rtB -> B_1036_584_0 = _rtP -> P_1596 ; _rtB ->
B_1036_585_0_j = _rtP -> P_1597 ; _rtB -> B_1036_586_0_g = _rtP -> P_1598 ;
_rtB -> B_1036_587_0_o = _rtP -> P_1599 ; _rtB -> B_1036_588_0_h = _rtP ->
P_1600 ; _rtB -> B_1036_589_0_c = _rtP -> P_1601 ; _rtB -> B_1036_590_0 = 0.0
; _rtB -> B_1036_591_0_a = 0.0 ; _rtB -> B_1036_592_0 = _rtB -> B_1036_590_0
+ _rtB -> B_1036_591_0_a ; _rtB -> B_1036_593_0_l = muDoubleScalarSqrt ( _rtB
-> B_1036_592_0 ) ; _rtB -> B_1036_594_0_a = ( _rtB -> B_1036_593_0_l > _rtB
-> B_1036_549_0_i ) ; _rtB -> B_1036_595_0_j = 0.0 ; _rtB -> B_1036_596_0_i =
0.0 ; _rtB -> B_1036_597_0_m = _rtB -> B_1036_595_0_j + _rtB ->
B_1036_596_0_i ; _rtB -> B_1036_598_0_f = muDoubleScalarSqrt ( _rtB ->
B_1036_597_0_m ) ; _rtB -> B_1036_599_0_p = ( _rtB -> B_1036_598_0_f > _rtB
-> B_1036_546_0 ) ; _rtB -> B_1036_600_0 = _rtP -> P_1602 ; _rtB ->
B_1036_601_0_o = _rtP -> P_1603 ; _rtB -> B_1036_602_0 = _rtP -> P_1604 ;
_rtB -> B_1036_603_0_m = ( _rtB -> B_1036_602_0 == _rtB -> B_1036_600_0 ) ;
_rtB -> B_1036_604_0_k = ! _rtB -> B_1036_603_0_m ; DZG_v3_OFFDelayTID2 ( S ,
& _rtB -> OFFDelay_ek , & _rtDW -> OFFDelay_ek , & _rtP -> OFFDelay_ek ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_f3 , & _rtDW -> ONDelay_f3 , &
_rtP -> ONDelay_f3 ) ; _rtB -> B_1036_607_0_i = _rtP -> P_1605 ; _rtB ->
B_1036_608_0 = _rtP -> P_1606 ; _rtB -> B_1036_609_0_e = _rtP -> P_1607 ;
_rtB -> B_1036_610_0 = ( _rtB -> B_1036_609_0_e == _rtB -> B_1036_607_0_i ) ;
_rtB -> B_1036_611_0_h = ! _rtB -> B_1036_610_0 ; DZG_v3_OFFDelayTID2 ( S , &
_rtB -> OFFDelay_o5 , & _rtDW -> OFFDelay_o5 , & _rtP -> OFFDelay_o5 ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_jx , & _rtDW -> ONDelay_jx , &
_rtP -> ONDelay_jx ) ; _rtB -> B_1036_614_0_j = _rtP -> P_1608 ; _rtB ->
B_1036_615_0_o = _rtP -> P_1609 ; _rtB -> B_1036_616_0_f = _rtP -> P_1610 ;
_rtB -> B_1036_617_0_b = ( _rtB -> B_1036_616_0_f == _rtB -> B_1036_614_0_j )
; _rtB -> B_1036_618_0_f = ! _rtB -> B_1036_617_0_b ; DZG_v3_OFFDelayTID2 ( S
, & _rtB -> OFFDelay_d4 , & _rtDW -> OFFDelay_d4 , & _rtP -> OFFDelay_d4 ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_jn , & _rtDW -> ONDelay_jn , &
_rtP -> ONDelay_jn ) ; _rtB -> B_1036_621_0_m = _rtP -> P_1611 ; _rtB ->
B_1036_622_0_a = _rtP -> P_1612 ; _rtB -> B_1036_623_0_h = _rtP -> P_1613 ;
_rtB -> B_1036_624_0_b = ( _rtB -> B_1036_623_0_h == _rtB -> B_1036_621_0_m )
; _rtB -> B_1036_625_0_i = ! _rtB -> B_1036_624_0_b ; DZG_v3_OFFDelayTID2 ( S
, & _rtB -> OFFDelay_nq , & _rtDW -> OFFDelay_nq , & _rtP -> OFFDelay_nq ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_fa , & _rtDW -> ONDelay_fa , &
_rtP -> ONDelay_fa ) ; _rtB -> B_1036_628_0 = _rtP -> P_1614 ; _rtB ->
B_1036_629_0_o = _rtB -> B_1036_140_0 / _rtB -> B_1036_628_0 ; _rtB ->
B_1036_630_0_h = _rtP -> P_1615 ; _rtB -> B_1036_631_0_j = _rtP -> P_1616 ;
_rtB -> B_1036_632_0_g = _rtB -> B_1036_630_0_h * _rtB -> B_1036_631_0_j ;
_rtB -> B_1036_633_0_j = _rtP -> P_1617 ; _rtB -> B_1036_634_0_l = _rtP ->
P_1618 ; _rtB -> B_1036_635_0_j = ( _rtB -> B_1036_137_0_p == _rtB ->
B_1036_634_0_l ) ; _rtB -> B_1036_636_0_e = ( _rtB -> B_1036_628_0 == _rtB ->
B_1036_634_0_l ) ; _rtB -> B_1036_637_0_k = ( _rtB -> B_1036_635_0_j || _rtB
-> B_1036_636_0_e ) ; _rtB -> B_1036_638_0 = _rtB -> B_1036_137_0_p / _rtB ->
B_1036_628_0 ; if ( _rtB -> B_1036_637_0_k ) { _rtB -> B_1036_639_0_k = _rtB
-> B_1036_633_0_j ; } else { _rtB -> B_1036_639_0_k = _rtB -> B_1036_638_0 ;
} _rtB -> B_1036_640_0_d = _rtB -> B_1036_639_0_k * _rtB -> B_1036_630_0_h ;
_rtB -> B_1036_641_0_n = _rtB -> B_1036_144_0_i * _rtB -> B_1036_157_0_a ;
_rtB -> B_1036_642_0_j = 0.0 * _rtB -> B_1036_144_0_i ; _rtB ->
B_1036_643_0_a = 0.0 * _rtB -> B_1036_144_0_i ; _rtB -> B_1036_644_0 = 0.0 ;
_rtB -> B_1036_645_0_h = 0.0 ; _rtB -> B_1036_646_0 = _rtB -> B_1036_644_0 +
_rtB -> B_1036_645_0_h ; _rtB -> B_1036_647_0_i = muDoubleScalarSqrt ( _rtB
-> B_1036_646_0 ) ; _rtB -> B_1036_648_0_c = ( _rtB -> B_1036_647_0_i > _rtB
-> B_1036_157_0_a ) ; _rtB -> B_1036_649_0_d = _rtP -> P_1619 ; _rtB ->
B_1036_650_0_b = _rtP -> P_1620 ; _rtB -> B_1036_651_0_h = _rtP -> P_1621 ;
_rtB -> B_1036_652_0_p = _rtB -> B_1036_649_0_d / _rtB -> B_1036_651_0_h ;
_rtB -> B_1036_653_0_n = _rtP -> P_1622 ; _rtB -> B_1036_654_0_j = _rtP ->
P_1623 ; _rtB -> B_1036_655_0_o = _rtP -> P_1624 ; _rtB -> B_1036_656_0_b =
_rtB -> B_1036_653_0_n * _rtB -> B_1036_655_0_o ; _rtB -> B_1036_657_0_j =
_rtP -> P_1625 ; _rtB -> B_1036_658_0_e = _rtP -> P_1626 ; _rtB ->
B_1036_659_0_f = ( _rtB -> B_1036_654_0_j == _rtB -> B_1036_658_0_e ) ; _rtB
-> B_1036_660_0_g = ( _rtB -> B_1036_651_0_h == _rtB -> B_1036_658_0_e ) ;
_rtB -> B_1036_661_0_h = ( _rtB -> B_1036_659_0_f || _rtB -> B_1036_660_0_g )
; _rtB -> B_1036_662_0_i = _rtB -> B_1036_654_0_j / _rtB -> B_1036_651_0_h ;
if ( _rtB -> B_1036_661_0_h ) { _rtB -> B_1036_663_0_n = _rtB ->
B_1036_657_0_j ; } else { _rtB -> B_1036_663_0_n = _rtB -> B_1036_662_0_i ; }
_rtB -> B_1036_664_0_i = _rtB -> B_1036_663_0_n * _rtB -> B_1036_653_0_n ;
_rtB -> B_1036_665_0 = _rtP -> P_1627 ; _rtB -> B_1036_666_0 = _rtP -> P_1628
; _rtB -> B_1036_667_0_c = ( _rtB -> B_1036_654_0_j == _rtB -> B_1036_666_0 )
; _rtB -> B_1036_668_0_l = ( _rtB -> B_1036_649_0_d == _rtB -> B_1036_666_0 )
; _rtB -> B_1036_669_0 = ( _rtB -> B_1036_667_0_c || _rtB -> B_1036_668_0_l )
; _rtB -> B_1036_670_0 = _rtB -> B_1036_654_0_j / _rtB -> B_1036_649_0_d ; if
( _rtB -> B_1036_669_0 ) { _rtB -> B_1036_671_0 = _rtB -> B_1036_665_0 ; }
else { _rtB -> B_1036_671_0 = _rtB -> B_1036_670_0 ; } _rtB -> B_1036_672_0 =
_rtB -> B_1036_671_0 * _rtB -> B_1036_650_0_b ; _rtB -> B_1036_673_0 = 0.0 ;
_rtB -> B_1036_674_0 = 0.0 ; _rtB -> B_1036_675_0_p = _rtB -> B_1036_673_0 +
_rtB -> B_1036_674_0 ; _rtB -> B_1036_676_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_675_0_p ) ; _rtB -> B_1036_677_0 = ( _rtB -> B_1036_676_0 > _rtB ->
B_1036_650_0_b ) ; _rtB -> B_1036_678_0 = 0.0 * _rtB -> B_1036_671_0 ; _rtB
-> B_1036_679_0 = 0.0 * _rtB -> B_1036_671_0 ; _rtB -> B_1036_680_0 = 0.0 *
_rtB -> B_1036_671_0 ; _rtB -> B_1036_681_0 = 0.0 * _rtB -> B_1036_671_0 ;
_rtB -> B_1036_682_0 = 0.0 * _rtB -> B_1036_671_0 ; _rtB -> B_1036_683_0_o =
0.0 * _rtB -> B_1036_671_0 ; _rtB -> B_1036_684_0_m = 0.0 ; _rtB ->
B_1036_685_0 = 0.0 ; _rtB -> B_1036_686_0 = _rtB -> B_1036_684_0_m + _rtB ->
B_1036_685_0 ; _rtB -> B_1036_687_0_o = muDoubleScalarSqrt ( _rtB ->
B_1036_686_0 ) ; _rtB -> B_1036_688_0_j = ( _rtB -> B_1036_687_0_o > _rtB ->
B_1036_650_0_b ) ; _rtB -> B_1036_689_0 = 0.0 ; _rtB -> B_1036_690_0 = 0.0 ;
_rtB -> B_1036_691_0 = _rtB -> B_1036_689_0 + _rtB -> B_1036_690_0 ; _rtB ->
B_1036_692_0 = muDoubleScalarSqrt ( _rtB -> B_1036_691_0 ) ; _rtB ->
B_1036_693_0_n = ( _rtB -> B_1036_692_0 > _rtB -> B_1036_650_0_b ) ; _rtB ->
B_1036_694_0 = 0.0 * _rtB -> B_1036_671_0 ; _rtB -> B_1036_695_0_g = 0.0 *
_rtB -> B_1036_671_0 ; _rtB -> B_1036_696_0 = 0.0 ; _rtB -> B_1036_697_0 =
0.0 ; _rtB -> B_1036_698_0 = _rtB -> B_1036_696_0 + _rtB -> B_1036_697_0 ;
_rtB -> B_1036_699_0 = muDoubleScalarSqrt ( _rtB -> B_1036_698_0 ) ; _rtB ->
B_1036_700_0_f = ( _rtB -> B_1036_699_0 > _rtB -> B_1036_650_0_b ) ; _rtB ->
B_1036_701_0 = _rtP -> P_1629 ; _rtB -> B_1036_702_0 = _rtP -> P_1630 ; _rtB
-> B_1036_703_0_e = _rtP -> P_1631 ; _rtB -> B_1036_704_0_i = _rtB ->
B_1036_701_0 / _rtB -> B_1036_703_0_e ; _rtB -> B_1036_705_0_g = _rtP ->
P_1632 ; _rtB -> B_1036_706_0_g = _rtP -> P_1633 ; _rtB -> B_1036_707_0_g =
_rtP -> P_1634 ; _rtB -> B_1036_708_0_g = _rtB -> B_1036_705_0_g * _rtB ->
B_1036_707_0_g ; _rtB -> B_1036_709_0_c = _rtP -> P_1635 ; _rtB ->
B_1036_710_0_k = _rtP -> P_1636 ; _rtB -> B_1036_711_0_e = ( _rtB ->
B_1036_706_0_g == _rtB -> B_1036_710_0_k ) ; _rtB -> B_1036_712_0_p = ( _rtB
-> B_1036_703_0_e == _rtB -> B_1036_710_0_k ) ; _rtB -> B_1036_713_0_a = (
_rtB -> B_1036_711_0_e || _rtB -> B_1036_712_0_p ) ; _rtB -> B_1036_714_0_d =
_rtB -> B_1036_706_0_g / _rtB -> B_1036_703_0_e ; if ( _rtB -> B_1036_713_0_a
) { _rtB -> B_1036_715_0_k = _rtB -> B_1036_709_0_c ; } else { _rtB ->
B_1036_715_0_k = _rtB -> B_1036_714_0_d ; } _rtB -> B_1036_716_0_p = _rtB ->
B_1036_715_0_k * _rtB -> B_1036_705_0_g ; _rtB -> B_1036_717_0_p = _rtP ->
P_1637 ; _rtB -> B_1036_718_0_m = _rtP -> P_1638 ; _rtB -> B_1036_719_0_b = (
_rtB -> B_1036_706_0_g == _rtB -> B_1036_718_0_m ) ; _rtB -> B_1036_720_0 = (
_rtB -> B_1036_701_0 == _rtB -> B_1036_718_0_m ) ; _rtB -> B_1036_721_0_g = (
_rtB -> B_1036_719_0_b || _rtB -> B_1036_720_0 ) ; _rtB -> B_1036_722_0_k =
_rtB -> B_1036_706_0_g / _rtB -> B_1036_701_0 ; if ( _rtB -> B_1036_721_0_g )
{ _rtB -> B_1036_723_0_a = _rtB -> B_1036_717_0_p ; } else { _rtB ->
B_1036_723_0_a = _rtB -> B_1036_722_0_k ; } _rtB -> B_1036_724_0_f = _rtB ->
B_1036_723_0_a * _rtB -> B_1036_702_0 ; _rtB -> B_1036_725_0_c = 0.0 ; _rtB
-> B_1036_726_0_j = 0.0 ; _rtB -> B_1036_727_0_k = _rtB -> B_1036_725_0_c +
_rtB -> B_1036_726_0_j ; _rtB -> B_1036_728_0_h = muDoubleScalarSqrt ( _rtB
-> B_1036_727_0_k ) ; _rtB -> B_1036_729_0_f = ( _rtB -> B_1036_728_0_h >
_rtB -> B_1036_702_0 ) ; _rtB -> B_1036_730_0_d = 0.0 * _rtB ->
B_1036_723_0_a ; _rtB -> B_1036_731_0_j = 0.0 * _rtB -> B_1036_723_0_a ; _rtB
-> B_1036_732_0_n = 0.0 * _rtB -> B_1036_723_0_a ; _rtB -> B_1036_733_0_j =
0.0 * _rtB -> B_1036_723_0_a ; _rtB -> B_1036_734_0_l = 0.0 * _rtB ->
B_1036_723_0_a ; _rtB -> B_1036_735_0_p = 0.0 * _rtB -> B_1036_723_0_a ; _rtB
-> B_1036_736_0_p = 0.0 ; _rtB -> B_1036_737_0_l = 0.0 ; _rtB ->
B_1036_738_0_l = _rtB -> B_1036_736_0_p + _rtB -> B_1036_737_0_l ; _rtB ->
B_1036_739_0_h = muDoubleScalarSqrt ( _rtB -> B_1036_738_0_l ) ; _rtB ->
B_1036_740_0_k = ( _rtB -> B_1036_739_0_h > _rtB -> B_1036_702_0 ) ; _rtB ->
B_1036_741_0_c = 0.0 ; _rtB -> B_1036_742_0 = 0.0 ; _rtB -> B_1036_743_0_g =
_rtB -> B_1036_741_0_c + _rtB -> B_1036_742_0 ; _rtB -> B_1036_744_0_e =
muDoubleScalarSqrt ( _rtB -> B_1036_743_0_g ) ; _rtB -> B_1036_745_0_o = (
_rtB -> B_1036_744_0_e > _rtB -> B_1036_702_0 ) ; _rtB -> B_1036_746_0 = 0.0
* _rtB -> B_1036_723_0_a ; _rtB -> B_1036_747_0 = 0.0 * _rtB ->
B_1036_723_0_a ; _rtB -> B_1036_748_0 = 0.0 ; _rtB -> B_1036_749_0_n = 0.0 ;
_rtB -> B_1036_750_0_f = _rtB -> B_1036_748_0 + _rtB -> B_1036_749_0_n ; _rtB
-> B_1036_751_0_n = muDoubleScalarSqrt ( _rtB -> B_1036_750_0_f ) ; _rtB ->
B_1036_752_0_g = ( _rtB -> B_1036_751_0_n > _rtB -> B_1036_702_0 ) ; _rtB ->
B_1036_753_0 = _rtP -> P_1639 ; _rtB -> B_1036_754_0 = _rtP -> P_1640 ; _rtB
-> B_1036_755_0 = _rtP -> P_1641 ; _rtB -> B_1036_756_0 = _rtB ->
B_1036_753_0 / _rtB -> B_1036_755_0 ; _rtB -> B_1036_757_0 = _rtP -> P_1642 ;
_rtB -> B_1036_758_0_e = 0.0 ; _rtB -> B_1036_759_0 = 0.0 ; _rtB ->
B_1036_760_0_b = _rtB -> B_1036_758_0_e + _rtB -> B_1036_759_0 ; _rtB ->
B_1036_761_0_a = muDoubleScalarSqrt ( _rtB -> B_1036_760_0_b ) ; _rtB ->
B_1036_762_0_p = ( _rtB -> B_1036_761_0_a > _rtB -> B_1036_757_0 ) ; _rtB ->
B_1036_763_0_i = _rtP -> P_1643 ; _rtB -> B_1036_764_0_n = _rtP -> P_1644 ;
_rtB -> B_1036_765_0_f = _rtB -> B_1036_757_0 * _rtB -> B_1036_764_0_n ; _rtB
-> B_1036_766_0 = _rtP -> P_1645 ; _rtB -> B_1036_767_0_i = _rtP -> P_1646 ;
_rtB -> B_1036_768_0_n = ( _rtB -> B_1036_763_0_i == _rtB -> B_1036_767_0_i )
; _rtB -> B_1036_769_0_d = ( _rtB -> B_1036_755_0 == _rtB -> B_1036_767_0_i )
; _rtB -> B_1036_770_0_i = ( _rtB -> B_1036_768_0_n || _rtB -> B_1036_769_0_d
) ; _rtB -> B_1036_771_0_k = _rtB -> B_1036_763_0_i / _rtB -> B_1036_755_0 ;
if ( _rtB -> B_1036_770_0_i ) { _rtB -> B_1036_772_0 = _rtB -> B_1036_766_0 ;
} else { _rtB -> B_1036_772_0 = _rtB -> B_1036_771_0_k ; } _rtB ->
B_1036_773_0_b = _rtB -> B_1036_772_0 * _rtB -> B_1036_757_0 ; _rtB ->
B_1036_774_0_d = _rtP -> P_1647 ; _rtB -> B_1036_775_0_h = _rtP -> P_1648 ;
_rtB -> B_1036_776_0_p = ( _rtB -> B_1036_763_0_i == _rtB -> B_1036_775_0_h )
; _rtB -> B_1036_777_0_e = ( _rtB -> B_1036_753_0 == _rtB -> B_1036_775_0_h )
; _rtB -> B_1036_778_0_g = ( _rtB -> B_1036_776_0_p || _rtB -> B_1036_777_0_e
) ; _rtB -> B_1036_779_0_n = _rtB -> B_1036_763_0_i / _rtB -> B_1036_753_0 ;
if ( _rtB -> B_1036_778_0_g ) { _rtB -> B_1036_780_0_f = _rtB ->
B_1036_774_0_d ; } else { _rtB -> B_1036_780_0_f = _rtB -> B_1036_779_0_n ; }
_rtB -> B_1036_781_0_a = _rtB -> B_1036_780_0_f * _rtB -> B_1036_754_0 ; _rtB
-> B_1036_782_0 = 0.0 * _rtB -> B_1036_772_0 ; _rtB -> B_1036_783_0_m = 0.0 *
_rtB -> B_1036_772_0 ; _rtB -> B_1036_784_0_g = 0.0 ; _rtB -> B_1036_785_0_n
= 0.0 ; _rtB -> B_1036_786_0_c = _rtB -> B_1036_784_0_g + _rtB ->
B_1036_785_0_n ; _rtB -> B_1036_787_0_d = muDoubleScalarSqrt ( _rtB ->
B_1036_786_0_c ) ; _rtB -> B_1036_788_0_g = ( _rtB -> B_1036_787_0_d > _rtB
-> B_1036_754_0 ) ; _rtB -> B_1036_789_0_k = 0.0 * _rtB -> B_1036_780_0_f ;
_rtB -> B_1036_790_0_c = 0.0 * _rtB -> B_1036_780_0_f ; _rtB ->
B_1036_791_0_j = 0.0 * _rtB -> B_1036_780_0_f ; _rtB -> B_1036_792_0_m = 0.0
* _rtB -> B_1036_780_0_f ; _rtB -> B_1036_793_0 = 0.0 * _rtB ->
B_1036_780_0_f ; _rtB -> B_1036_794_0_i = 0.0 * _rtB -> B_1036_780_0_f ; _rtB
-> B_1036_795_0_b = 0.0 ; _rtB -> B_1036_796_0_o = 0.0 ; _rtB ->
B_1036_797_0_g = _rtB -> B_1036_795_0_b + _rtB -> B_1036_796_0_o ; _rtB ->
B_1036_798_0_e = muDoubleScalarSqrt ( _rtB -> B_1036_797_0_g ) ; _rtB ->
B_1036_799_0_a = ( _rtB -> B_1036_798_0_e > _rtB -> B_1036_754_0 ) ; _rtB ->
B_1036_800_0_i = 0.0 ; _rtB -> B_1036_801_0_e = 0.0 ; _rtB -> B_1036_802_0_i
= _rtB -> B_1036_800_0_i + _rtB -> B_1036_801_0_e ; _rtB -> B_1036_803_0_m =
muDoubleScalarSqrt ( _rtB -> B_1036_802_0_i ) ; _rtB -> B_1036_804_0_f = (
_rtB -> B_1036_803_0_m > _rtB -> B_1036_754_0 ) ; _rtB -> B_1036_805_0_d =
0.0 * _rtB -> B_1036_780_0_f ; _rtB -> B_1036_806_0_j = 0.0 * _rtB ->
B_1036_780_0_f ; _rtB -> B_1036_807_0_p = 0.0 ; _rtB -> B_1036_808_0_b = 0.0
; _rtB -> B_1036_809_0_p = _rtB -> B_1036_807_0_p + _rtB -> B_1036_808_0_b ;
_rtB -> B_1036_810_0_n = muDoubleScalarSqrt ( _rtB -> B_1036_809_0_p ) ; _rtB
-> B_1036_811_0_g = ( _rtB -> B_1036_810_0_n > _rtB -> B_1036_754_0 ) ; _rtB
-> B_1036_812_0_i = false ; _rtB -> B_1036_813_0_c = _rtP -> P_1649 ; _rtB ->
B_1036_814_0_c = ( _rtB -> B_1036_812_0_i || ( _rtB -> B_1036_813_0_c != 0.0
) || ( _rtB -> B_1036_813_0_c != 0.0 ) || ( _rtB -> B_1036_813_0_c != 0.0 ) )
; _rtB -> B_1036_815_0_n = _rtP -> P_1650 ; _rtB -> B_1036_816_0_p = ( _rtB
-> B_1036_813_0_c != 0.0 ) ; _rtB -> B_1036_817_0_g = ( _rtB ->
B_1036_813_0_c != 0.0 ) ; _rtB -> B_1036_818_0_j = ! _rtB -> B_1036_814_0_c ;
_rtB -> B_1036_819_0_d = _rtP -> P_1651 ; _rtB -> B_1036_820_0_i = _rtP ->
P_1652 ; _rtB -> B_1036_821_0_n = _rtP -> P_1653 ; _rtB -> B_1036_822_0_n = (
_rtB -> B_1036_821_0_n == _rtB -> B_1036_819_0_d ) ; _rtB -> B_1036_823_0_n =
! _rtB -> B_1036_822_0_n ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_k , &
_rtDW -> OFFDelay_k , & _rtP -> OFFDelay_k ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_b5 , & _rtDW -> ONDelay_b5 , & _rtP -> ONDelay_b5 ) ; _rtB ->
B_1036_826_0_b = _rtP -> P_1654 ; _rtB -> B_1036_827_0 = _rtP -> P_1655 ;
_rtB -> B_1036_828_0 = _rtP -> P_1656 ; _rtB -> B_1036_829_0_j = ( _rtB ->
B_1036_828_0 == _rtB -> B_1036_826_0_b ) ; _rtB -> B_1036_830_0 = ! _rtB ->
B_1036_829_0_j ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_cf , & _rtDW ->
OFFDelay_cf , & _rtP -> OFFDelay_cf ) ; DZG_v3_ONDelayTID2 ( S , & _rtB ->
ONDelay_k4 , & _rtDW -> ONDelay_k4 , & _rtP -> ONDelay_k4 ) ; _rtB ->
B_1036_833_0_b = _rtP -> P_1657 ; _rtB -> B_1036_834_0 = _rtP -> P_1658 ;
_rtB -> B_1036_835_0_j = _rtP -> P_1659 ; _rtB -> B_1036_836_0 = ( _rtB ->
B_1036_835_0_j == _rtB -> B_1036_833_0_b ) ; _rtB -> B_1036_837_0_j = ! _rtB
-> B_1036_836_0 ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_ga , & _rtDW
-> OFFDelay_ga , & _rtP -> OFFDelay_ga ) ; DZG_v3_ONDelayTID2 ( S , & _rtB ->
ONDelay_jn2 , & _rtDW -> ONDelay_jn2 , & _rtP -> ONDelay_jn2 ) ; _rtB ->
B_1036_840_0 = _rtP -> P_1660 ; _rtB -> B_1036_841_0 = _rtP -> P_1661 ; _rtB
-> B_1036_842_0 = _rtP -> P_1662 ; _rtB -> B_1036_843_0_h = ( _rtB ->
B_1036_842_0 == _rtB -> B_1036_840_0 ) ; _rtB -> B_1036_844_0_p = ! _rtB ->
B_1036_843_0_h ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_du , & _rtDW ->
OFFDelay_du , & _rtP -> OFFDelay_du ) ; DZG_v3_ONDelayTID2 ( S , & _rtB ->
ONDelay_l5 , & _rtDW -> ONDelay_l5 , & _rtP -> ONDelay_l5 ) ; _rtB ->
B_1036_847_0 = _rtP -> P_1663 ; _rtB -> B_1036_848_0 = _rtP -> P_1664 ; _rtB
-> B_1036_849_0_n = _rtP -> P_1665 ; _rtB -> B_1036_850_0_p = ( _rtB ->
B_1036_849_0_n == _rtB -> B_1036_847_0 ) ; _rtB -> B_1036_851_0_l = ! _rtB ->
B_1036_850_0_p ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_l , & _rtDW ->
OFFDelay_l , & _rtP -> OFFDelay_l ) ; DZG_v3_ONDelayTID2 ( S , & _rtB ->
ONDelay_kl , & _rtDW -> ONDelay_kl , & _rtP -> ONDelay_kl ) ; _rtB ->
B_1036_854_0 = _rtP -> P_1666 ; _rtB -> B_1036_855_0_f = _rtP -> P_1667 ;
_rtB -> B_1036_856_0_e [ 0 ] = _rtP -> P_1668 [ 0 ] ; _rtB -> B_1036_857_0_i
[ 0 ] = _rtP -> P_1669 [ 0 ] ; _rtB -> B_1036_858_0_a [ 0 ] = _rtP -> P_1670
[ 0 ] ; _rtB -> B_1036_856_0_e [ 1 ] = _rtP -> P_1668 [ 1 ] ; _rtB ->
B_1036_857_0_i [ 1 ] = _rtP -> P_1669 [ 1 ] ; _rtB -> B_1036_858_0_a [ 1 ] =
_rtP -> P_1670 [ 1 ] ; switch ( ( int32_T ) _rtB -> B_1036_855_0_f ) { case 1
: _rtB -> B_1036_859_0_f [ 0 ] = _rtB -> B_1036_856_0_e [ 0 ] ; _rtB ->
B_1036_859_0_f [ 1 ] = _rtB -> B_1036_856_0_e [ 1 ] ; break ; case 2 : _rtB
-> B_1036_859_0_f [ 0 ] = _rtB -> B_1036_857_0_i [ 0 ] ; _rtB ->
B_1036_859_0_f [ 1 ] = _rtB -> B_1036_857_0_i [ 1 ] ; break ; default : _rtB
-> B_1036_859_0_f [ 0 ] = _rtB -> B_1036_858_0_a [ 0 ] ; _rtB ->
B_1036_859_0_f [ 1 ] = _rtB -> B_1036_858_0_a [ 1 ] ; break ; } _rtB ->
B_1036_860_0_d = ( _rtB -> B_1036_812_0_i || ( _rtB -> B_1036_813_0_c != 0.0
) || ( _rtB -> B_1036_813_0_c != 0.0 ) || ( _rtB -> B_1036_813_0_c != 0.0 ) )
; _rtB -> B_1036_861_0_k = _rtP -> P_1671 ; _rtB -> B_1036_862_0_h = ( _rtB
-> B_1036_813_0_c != 0.0 ) ; _rtB -> B_1036_863_0_b = ( _rtB ->
B_1036_813_0_c != 0.0 ) ; _rtB -> B_1036_864_0_l = ! _rtB -> B_1036_860_0_d ;
_rtB -> B_1036_865_0_c = _rtP -> P_1672 ; _rtB -> B_1036_866_0_j = _rtP ->
P_1673 ; _rtB -> B_1036_867_0_l = _rtP -> P_1674 ; _rtB -> B_1036_868_0_g = (
_rtB -> B_1036_867_0_l == _rtB -> B_1036_865_0_c ) ; _rtB -> B_1036_869_0_g =
! _rtB -> B_1036_868_0_g ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_h , &
_rtDW -> OFFDelay_h , & _rtP -> OFFDelay_h ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_hs , & _rtDW -> ONDelay_hs , & _rtP -> ONDelay_hs ) ; _rtB ->
B_1036_872_0 = _rtP -> P_1675 ; _rtB -> B_1036_873_0_a = _rtP -> P_1676 ;
_rtB -> B_1036_874_0_i = _rtP -> P_1677 ; _rtB -> B_1036_875_0_g = ( _rtB ->
B_1036_874_0_i == _rtB -> B_1036_872_0 ) ; _rtB -> B_1036_876_0_o = ! _rtB ->
B_1036_875_0_g ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_mj , & _rtDW ->
OFFDelay_mj , & _rtP -> OFFDelay_mj ) ; DZG_v3_ONDelayTID2 ( S , & _rtB ->
ONDelay_fr , & _rtDW -> ONDelay_fr , & _rtP -> ONDelay_fr ) ; _rtB ->
B_1036_879_0_o = _rtP -> P_1678 ; _rtB -> B_1036_880_0_b = _rtP -> P_1679 ;
_rtB -> B_1036_881_0_a = _rtP -> P_1680 ; _rtB -> B_1036_882_0_k = ( _rtB ->
B_1036_881_0_a == _rtB -> B_1036_879_0_o ) ; _rtB -> B_1036_883_0_p = ! _rtB
-> B_1036_882_0_k ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_a0 , & _rtDW
-> OFFDelay_a0 , & _rtP -> OFFDelay_a0 ) ; DZG_v3_ONDelayTID2 ( S , & _rtB ->
ONDelay_hl , & _rtDW -> ONDelay_hl , & _rtP -> ONDelay_hl ) ; _rtB ->
B_1036_886_0_a = _rtP -> P_1681 ; _rtB -> B_1036_887_0 = _rtP -> P_1682 ;
_rtB -> B_1036_888_0 = _rtP -> P_1683 ; _rtB -> B_1036_889_0_p = ( _rtB ->
B_1036_888_0 == _rtB -> B_1036_886_0_a ) ; _rtB -> B_1036_890_0_e = ! _rtB ->
B_1036_889_0_p ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_f4 , & _rtDW ->
OFFDelay_f4 , & _rtP -> OFFDelay_f4 ) ; DZG_v3_ONDelayTID2 ( S , & _rtB ->
ONDelay_hn , & _rtDW -> ONDelay_hn , & _rtP -> ONDelay_hn ) ; _rtB ->
B_1036_893_0 = _rtP -> P_1684 ; _rtB -> B_1036_894_0 = _rtP -> P_1685 ; _rtB
-> B_1036_895_0 = _rtP -> P_1686 ; _rtB -> B_1036_896_0_k = ( _rtB ->
B_1036_895_0 == _rtB -> B_1036_893_0 ) ; _rtB -> B_1036_897_0_n = ! _rtB ->
B_1036_896_0_k ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_e4 , & _rtDW ->
OFFDelay_e4 , & _rtP -> OFFDelay_e4 ) ; DZG_v3_ONDelayTID2 ( S , & _rtB ->
ONDelay_nu , & _rtDW -> ONDelay_nu , & _rtP -> ONDelay_nu ) ; _rtB ->
B_1036_900_0 = _rtP -> P_1687 ; _rtB -> B_1036_901_0 = _rtP -> P_1688 ; _rtB
-> B_1036_902_0 [ 0 ] = _rtP -> P_1689 [ 0 ] ; _rtB -> B_1036_903_0 [ 0 ] =
_rtP -> P_1690 [ 0 ] ; _rtB -> B_1036_904_0 [ 0 ] = _rtP -> P_1691 [ 0 ] ;
_rtB -> B_1036_902_0 [ 1 ] = _rtP -> P_1689 [ 1 ] ; _rtB -> B_1036_903_0 [ 1
] = _rtP -> P_1690 [ 1 ] ; _rtB -> B_1036_904_0 [ 1 ] = _rtP -> P_1691 [ 1 ]
; switch ( ( int32_T ) _rtB -> B_1036_901_0 ) { case 1 : _rtB -> B_1036_905_0
[ 0 ] = _rtB -> B_1036_902_0 [ 0 ] ; _rtB -> B_1036_905_0 [ 1 ] = _rtB ->
B_1036_902_0 [ 1 ] ; break ; case 2 : _rtB -> B_1036_905_0 [ 0 ] = _rtB ->
B_1036_903_0 [ 0 ] ; _rtB -> B_1036_905_0 [ 1 ] = _rtB -> B_1036_903_0 [ 1 ]
; break ; default : _rtB -> B_1036_905_0 [ 0 ] = _rtB -> B_1036_904_0 [ 0 ] ;
_rtB -> B_1036_905_0 [ 1 ] = _rtB -> B_1036_904_0 [ 1 ] ; break ; } _rtB ->
B_1036_906_0 = _rtP -> P_1692 ; _rtB -> B_1036_907_0_e = ( _rtB ->
B_1036_812_0_i || ( _rtB -> B_1036_906_0 != 0.0 ) || ( _rtB -> B_1036_906_0
!= 0.0 ) || ( _rtB -> B_1036_906_0 != 0.0 ) ) ; _rtB -> B_1036_908_0 = _rtP
-> P_1693 ; _rtB -> B_1036_909_0_o = ( ( _rtB -> B_1036_908_0 != 0.0 ) || (
_rtB -> B_1036_906_0 != 0.0 ) || ( _rtB -> B_1036_906_0 != 0.0 ) || ( _rtB ->
B_1036_906_0 != 0.0 ) ) ; _rtB -> B_1036_910_0_d = ( ( _rtB -> B_1036_908_0
!= 0.0 ) || ( _rtB -> B_1036_906_0 != 0.0 ) || ( _rtB -> B_1036_906_0 != 0.0
) || ( _rtB -> B_1036_906_0 != 0.0 ) ) ; _rtB -> B_1036_911_0 = _rtP ->
P_1694 ; _rtB -> B_1036_912_0 = _rtP -> P_1695 ; _rtB -> B_1036_913_0 = _rtP
-> P_1696 ; _rtB -> B_1036_914_0 = _rtP -> P_1697 ; _rtB -> B_1036_915_0_g =
! _rtB -> B_1036_907_0_e ; _rtB -> B_1036_916_0_d = ( _rtB -> B_1036_910_0_d
&& _rtB -> B_1036_915_0_g ) ; _rtB -> B_1036_917_0_m = ( ( _rtB ->
B_1036_912_0 != 0.0 ) && ( _rtB -> B_1036_906_0 != 0.0 ) ) ; _rtB ->
B_1036_918_0_j = ! ( _rtB -> B_1036_906_0 != 0.0 ) ; _rtB -> B_1036_919_0_m =
( ( _rtB -> B_1036_913_0 != 0.0 ) && _rtB -> B_1036_918_0_j ) ; _rtB ->
B_1036_920_0_d = ( _rtB -> B_1036_917_0_m || _rtB -> B_1036_919_0_m ) ; _rtB
-> B_1036_921_0 = _rtP -> P_1698 ; _rtB -> B_1036_922_0_i = _rtP -> P_1699 ;
_rtB -> B_1036_923_0 = _rtP -> P_1700 ; _rtB -> B_1036_924_0 = _rtP -> P_1701
; _rtB -> B_1036_925_0 = _rtP -> P_1702 ; _rtB -> B_1036_926_0 = _rtP ->
P_1703 ; _rtB -> B_1036_927_0 = _rtP -> P_1704 ; _rtB -> B_1036_928_0 = _rtP
-> P_1705 ; _rtB -> B_1036_929_0_i = false ; _rtB -> B_1036_930_0_h = false ;
_rtB -> B_1036_931_0_c = ! _rtB -> B_1036_929_0_i ; _rtB -> B_1036_932_0_f =
! _rtB -> B_1036_930_0_h ; _rtB -> B_1036_933_0_g = false ; _rtB ->
B_1036_934_0_k = ! _rtB -> B_1036_933_0_g ; _rtB -> B_1036_935_0 = _rtP ->
P_1706 ; _rtB -> B_1036_936_0 = _rtP -> P_1707 ; _rtB -> B_1036_937_0 = _rtP
-> P_1708 ; _rtB -> B_1036_938_0 = _rtP -> P_1709 ; _rtB -> B_1036_939_0_i =
_rtP -> P_1710 ; _rtB -> B_1036_940_0 = _rtP -> P_1711 ; _rtB -> B_1036_941_0
= _rtP -> P_1712 ; _rtB -> B_1036_942_0_c = ( _rtB -> B_1036_941_0 == _rtB ->
B_1036_939_0_i ) ; _rtB -> B_1036_943_0 = ! _rtB -> B_1036_942_0_c ;
DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_pc , & _rtDW -> OFFDelay_pc , &
_rtP -> OFFDelay_pc ) ; DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_nt , &
_rtDW -> ONDelay_nt , & _rtP -> ONDelay_nt ) ; _rtB -> B_1036_946_0 = _rtP ->
P_1713 ; _rtB -> B_1036_947_0_l = _rtP -> P_1714 ; _rtB -> B_1036_948_0_o =
_rtP -> P_1715 ; _rtB -> B_1036_949_0_p = _rtP -> P_1716 ; _rtB ->
B_1036_950_0 = _rtP -> P_1717 ; _rtB -> B_1036_951_0_o = _rtP -> P_1718 ;
_rtB -> B_1036_952_0_c = _rtP -> P_1719 ; _rtB -> B_1036_953_0_o = _rtP ->
P_1720 ; _rtB -> B_1036_954_0_o = _rtP -> P_1721 ; _rtB -> B_1036_955_0_h =
_rtP -> P_1722 ; _rtB -> B_1036_956_0_i = _rtP -> P_1723 ; _rtB ->
B_1036_957_0_c = ( _rtB -> B_1036_956_0_i == _rtB -> B_1036_954_0_o ) ; _rtB
-> B_1036_958_0_g = ! _rtB -> B_1036_957_0_c ; DZG_v3_OFFDelayTID2 ( S , &
_rtB -> OFFDelay_mm , & _rtDW -> OFFDelay_mm , & _rtP -> OFFDelay_mm ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_bz , & _rtDW -> ONDelay_bz , &
_rtP -> ONDelay_bz ) ; _rtB -> B_1036_961_0 = _rtP -> P_1724 ; _rtB ->
B_1036_962_0_g = _rtP -> P_1725 ; _rtB -> B_1036_963_0_c = _rtP -> P_1726 ;
_rtB -> B_1036_964_0_f = ( _rtB -> B_1036_963_0_c == _rtB -> B_1036_961_0 ) ;
_rtB -> B_1036_965_0_h = ! _rtB -> B_1036_964_0_f ; DZG_v3_OFFDelayTID2 ( S ,
& _rtB -> OFFDelay_b3 , & _rtDW -> OFFDelay_b3 , & _rtP -> OFFDelay_b3 ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_i1 , & _rtDW -> ONDelay_i1 , &
_rtP -> ONDelay_i1 ) ; _rtB -> B_1036_968_0_o = _rtP -> P_1727 ; _rtB ->
B_1036_969_0_g = _rtP -> P_1728 ; _rtB -> B_1036_970_0_o = _rtP -> P_1729 ;
_rtB -> B_1036_971_0 = _rtP -> P_1730 ; _rtB -> B_1036_972_0_g = _rtP ->
P_1731 ; _rtB -> B_1036_973_0_e = false ; _rtB -> B_1036_974_0_i = ! _rtB ->
B_1036_973_0_e ; _rtB -> B_1036_975_0_f = false ; _rtB -> B_1036_976_0_l = !
_rtB -> B_1036_975_0_f ; _rtB -> B_1036_977_0_b = false ; _rtB ->
B_1036_978_0_f = false ; _rtB -> B_1036_979_0_i = ( _rtB -> B_1036_977_0_b ||
_rtB -> B_1036_978_0_f ) ; _rtB -> B_1036_980_0_g = ! _rtB -> B_1036_979_0_i
; _rtB -> B_1036_981_0_p = ! _rtB -> B_1036_979_0_i ; _rtB -> B_1036_982_0 =
_rtP -> P_1732 ; _rtB -> B_1036_983_0_a = _rtP -> P_1733 ; _rtB ->
B_1036_984_0_g = _rtP -> P_1734 ; _rtB -> B_1036_985_0_m = ( _rtB ->
B_1036_984_0_g == _rtB -> B_1036_982_0 ) ; _rtB -> B_1036_986_0_m = ! _rtB ->
B_1036_985_0_m ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_hq , & _rtDW ->
OFFDelay_hq , & _rtP -> OFFDelay_hq ) ; DZG_v3_ONDelayTID2 ( S , & _rtB ->
ONDelay_fn , & _rtDW -> ONDelay_fn , & _rtP -> ONDelay_fn ) ; _rtB ->
B_1036_989_0_b = _rtP -> P_1735 ; _rtB -> B_1036_990_0_k = _rtP -> P_1736 ;
_rtB -> B_1036_991_0_c = _rtP -> P_1737 ; _rtB -> B_1036_992_0_p = ( _rtB ->
B_1036_991_0_c == _rtB -> B_1036_989_0_b ) ; _rtB -> B_1036_993_0_j = ! _rtB
-> B_1036_992_0_p ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_ai , & _rtDW
-> OFFDelay_ai , & _rtP -> OFFDelay_ai ) ; DZG_v3_ONDelayTID2 ( S , & _rtB ->
ONDelay_ha , & _rtDW -> ONDelay_ha , & _rtP -> ONDelay_ha ) ; _rtB ->
B_1036_996_0_g = ( _rtB -> B_1036_973_0_e || ( _rtB -> B_1036_969_0_g != 0.0
) || ( _rtB -> B_1036_969_0_g != 0.0 ) || ( _rtB -> B_1036_969_0_g != 0.0 ) )
; _rtB -> B_1036_997_0_j = ( ( _rtB -> B_1036_968_0_o != 0.0 ) || ( _rtB ->
B_1036_969_0_g != 0.0 ) || ( _rtB -> B_1036_969_0_g != 0.0 ) || ( _rtB ->
B_1036_969_0_g != 0.0 ) ) ; _rtB -> B_1036_998_0_f = ( ( _rtB ->
B_1036_968_0_o != 0.0 ) || ( _rtB -> B_1036_969_0_g != 0.0 ) || ( _rtB ->
B_1036_969_0_g != 0.0 ) || ( _rtB -> B_1036_969_0_g != 0.0 ) ) ; _rtB ->
B_1036_999_0_j = _rtP -> P_1738 ; _rtB -> B_1036_1000_0_a = _rtP -> P_1739 ;
_rtB -> B_1036_1001_0_d = _rtP -> P_1740 ; _rtB -> B_1036_1002_0_c = _rtP ->
P_1741 ; _rtB -> B_1036_1003_0_d = ! _rtB -> B_1036_996_0_g ; _rtB ->
B_1036_1004_0_a = ( _rtB -> B_1036_998_0_f && _rtB -> B_1036_1003_0_d ) ;
_rtB -> B_1036_1005_0_c = ( ( _rtB -> B_1036_1000_0_a != 0.0 ) && ( _rtB ->
B_1036_969_0_g != 0.0 ) ) ; _rtB -> B_1036_1006_0_c = ! ( _rtB ->
B_1036_969_0_g != 0.0 ) ; _rtB -> B_1036_1007_0_i = ( ( _rtB ->
B_1036_1001_0_d != 0.0 ) && _rtB -> B_1036_1006_0_c ) ; _rtB ->
B_1036_1008_0_e = ( _rtB -> B_1036_1005_0_c || _rtB -> B_1036_1007_0_i ) ;
_rtB -> B_1036_1009_0 = _rtP -> P_1742 ; _rtB -> B_1036_1010_0 = _rtP ->
P_1743 ; _rtB -> B_1036_1011_0 [ 0 ] = _rtP -> P_1744 [ 0 ] ; _rtB ->
B_1036_1012_0_d [ 0 ] = _rtP -> P_1745 [ 0 ] ; _rtB -> B_1036_1013_0 [ 0 ] =
_rtP -> P_1746 [ 0 ] ; _rtB -> B_1036_1011_0 [ 1 ] = _rtP -> P_1744 [ 1 ] ;
_rtB -> B_1036_1012_0_d [ 1 ] = _rtP -> P_1745 [ 1 ] ; _rtB -> B_1036_1013_0
[ 1 ] = _rtP -> P_1746 [ 1 ] ; switch ( ( int32_T ) _rtB -> B_1036_1010_0 ) {
case 1 : _rtB -> B_1036_1014_0 [ 0 ] = _rtB -> B_1036_1011_0 [ 0 ] ; _rtB ->
B_1036_1014_0 [ 1 ] = _rtB -> B_1036_1011_0 [ 1 ] ; break ; case 2 : _rtB ->
B_1036_1014_0 [ 0 ] = _rtB -> B_1036_1012_0_d [ 0 ] ; _rtB -> B_1036_1014_0 [
1 ] = _rtB -> B_1036_1012_0_d [ 1 ] ; break ; default : _rtB -> B_1036_1014_0
[ 0 ] = _rtB -> B_1036_1013_0 [ 0 ] ; _rtB -> B_1036_1014_0 [ 1 ] = _rtB ->
B_1036_1013_0 [ 1 ] ; break ; } _rtB -> B_1036_1015_0 = _rtP -> P_1747 ; _rtB
-> B_1036_1016_0 = _rtP -> P_1748 ; _rtB -> B_1036_1017_0 [ 0 ] = _rtP ->
P_1749 [ 0 ] ; _rtB -> B_1036_1018_0_a [ 0 ] = _rtP -> P_1750 [ 0 ] ; _rtB ->
B_1036_1019_0 [ 0 ] = _rtP -> P_1751 [ 0 ] ; _rtB -> B_1036_1017_0 [ 1 ] =
_rtP -> P_1749 [ 1 ] ; _rtB -> B_1036_1018_0_a [ 1 ] = _rtP -> P_1750 [ 1 ] ;
_rtB -> B_1036_1019_0 [ 1 ] = _rtP -> P_1751 [ 1 ] ; switch ( ( int32_T )
_rtB -> B_1036_1016_0 ) { case 1 : _rtB -> B_1036_1020_0_b [ 0 ] = _rtB ->
B_1036_1017_0 [ 0 ] ; _rtB -> B_1036_1020_0_b [ 1 ] = _rtB -> B_1036_1017_0 [
1 ] ; break ; case 2 : _rtB -> B_1036_1020_0_b [ 0 ] = _rtB ->
B_1036_1018_0_a [ 0 ] ; _rtB -> B_1036_1020_0_b [ 1 ] = _rtB ->
B_1036_1018_0_a [ 1 ] ; break ; default : _rtB -> B_1036_1020_0_b [ 0 ] =
_rtB -> B_1036_1019_0 [ 0 ] ; _rtB -> B_1036_1020_0_b [ 1 ] = _rtB ->
B_1036_1019_0 [ 1 ] ; break ; } _rtB -> B_1036_1021_0_g = _rtP -> P_1752 ;
_rtB -> B_1036_1022_0_f = _rtP -> P_1753 ; _rtB -> B_1036_1023_0 = _rtP ->
P_1754 ; _rtB -> B_1036_1024_0_c = _rtP -> P_1755 ; _rtB -> B_1036_1025_0_p =
_rtP -> P_1756 ; _rtB -> B_1036_1026_0_a = _rtP -> P_1757 ; _rtB ->
B_1036_1027_0_n = _rtP -> P_1758 ; _rtB -> B_1036_1028_0_h = _rtP -> P_1759 ;
_rtB -> B_1036_1029_0_m = _rtP -> P_1760 ; _rtB -> B_1036_1030_0_f = _rtP ->
P_1761 ; _rtB -> B_1036_1031_0 = _rtP -> P_1762 ; _rtB -> B_1036_1032_0 =
_rtP -> P_1763 ; _rtB -> B_1036_1033_0 = _rtP -> P_1764 ; _rtB ->
B_1036_1034_0 = _rtP -> P_1765 ; _rtB -> B_1036_1035_0_b = _rtP -> P_1766 ;
_rtB -> B_1036_1036_0 = _rtP -> P_1767 ; _rtB -> B_1036_1037_0_e = _rtP ->
P_1768 ; _rtB -> B_1036_1038_0_n = _rtP -> P_1769 ; _rtB -> B_1036_1039_0_p =
_rtP -> P_1770 ; _rtB -> B_1036_1040_0 = _rtP -> P_1771 ; _rtB ->
B_1036_1041_0_i = false ; _rtB -> B_1036_1042_0_b = false ; _rtB ->
B_1036_1043_0_n = ! ( _rtB -> B_1036_1032_0 != 0.0 ) ; _rtB -> B_1036_1044_0
= false ; _rtB -> B_1036_1045_0_m = false ; _rtB -> B_1036_1046_0_i = false ;
_rtB -> B_1036_1047_0_i = _rtP -> P_1772 ; _rtB -> B_1036_1048_0_j = _rtP ->
P_1773 ; _rtB -> B_1036_1049_0_l = _rtP -> P_1774 ; _rtB -> B_1036_1050_0_n =
( _rtB -> B_1036_1049_0_l == _rtB -> B_1036_1047_0_i ) ; _rtB ->
B_1036_1051_0_e = ! _rtB -> B_1036_1050_0_n ; DZG_v3_OFFDelayTID2 ( S , &
_rtB -> OFFDelay_dcf , & _rtDW -> OFFDelay_dcf , & _rtP -> OFFDelay_dcf ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_hd , & _rtDW -> ONDelay_hd , &
_rtP -> ONDelay_hd ) ; _rtB -> B_1036_1054_0 = _rtP -> P_1775 ; _rtB ->
B_1036_1055_0 = _rtP -> P_1776 ; _rtB -> B_1036_1056_0_c = _rtP -> P_1777 ;
_rtB -> B_1036_1057_0 = ( _rtB -> B_1036_1056_0_c == _rtB -> B_1036_1054_0 )
; _rtB -> B_1036_1058_0_g = ! _rtB -> B_1036_1057_0 ; DZG_v3_OFFDelayTID2 ( S
, & _rtB -> OFFDelay_j , & _rtDW -> OFFDelay_j , & _rtP -> OFFDelay_j ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_ll , & _rtDW -> ONDelay_ll , &
_rtP -> ONDelay_ll ) ; _rtB -> B_1036_1061_0_g = _rtP -> P_1778 ; _rtB ->
B_1036_1062_0_c = _rtP -> P_1779 ; _rtB -> B_1036_1063_0_l = _rtP -> P_1780 ;
_rtB -> B_1036_1064_0 = ( _rtB -> B_1036_1063_0_l == _rtB -> B_1036_1061_0_g
) ; _rtB -> B_1036_1065_0_j = ! _rtB -> B_1036_1064_0 ; DZG_v3_OFFDelayTID2 (
S , & _rtB -> OFFDelay_ja , & _rtDW -> OFFDelay_ja , & _rtP -> OFFDelay_ja )
; DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_bv , & _rtDW -> ONDelay_bv , &
_rtP -> ONDelay_bv ) ; _rtB -> B_1036_1068_0_e = _rtP -> P_1781 ; _rtB ->
B_1036_1069_0_d = _rtP -> P_1782 ; _rtB -> B_1036_1070_0 = _rtP -> P_1783 ;
_rtB -> B_1036_1071_0_a = ( _rtB -> B_1036_1070_0 == _rtB -> B_1036_1068_0_e
) ; _rtB -> B_1036_1072_0_i = ! _rtB -> B_1036_1071_0_a ; DZG_v3_OFFDelayTID2
( S , & _rtB -> OFFDelay_aq , & _rtDW -> OFFDelay_aq , & _rtP -> OFFDelay_aq
) ; DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_ht , & _rtDW -> ONDelay_ht , &
_rtP -> ONDelay_ht ) ; _rtB -> B_1036_1075_0_c = ! _rtB -> B_1036_1041_0_i ;
_rtB -> B_1036_1076_0_l = ( _rtB -> B_1036_1038_0_n != 0.0 ) ; _rtB ->
B_1036_1077_0_h = ( ( _rtB -> B_1036_1037_0_e != 0.0 ) || ( _rtB ->
B_1036_1038_0_n != 0.0 ) || ( _rtB -> B_1036_1038_0_n != 0.0 ) || ( _rtB ->
B_1036_1038_0_n != 0.0 ) ) ; _rtB -> B_1036_1078_0_i = ( ( _rtB ->
B_1036_1037_0_e != 0.0 ) || ( _rtB -> B_1036_1038_0_n != 0.0 ) || ( _rtB ->
B_1036_1038_0_n != 0.0 ) || ( _rtB -> B_1036_1038_0_n != 0.0 ) ) ; _rtB ->
B_1036_1079_0 = _rtP -> P_1784 ; _rtB -> B_1036_1080_0 = _rtP -> P_1785 ;
_rtB -> B_1036_1081_0 = _rtP -> P_1786 ; _rtB -> B_1036_1082_0 = _rtP ->
P_1787 ; _rtB -> B_1036_1083_0_n = ! _rtB -> B_1036_1076_0_l ; _rtB ->
B_1036_1084_0_j = ( _rtB -> B_1036_1078_0_i && _rtB -> B_1036_1083_0_n ) ;
_rtB -> B_1036_1085_0_k = ( ( _rtB -> B_1036_1080_0 != 0.0 ) && ( _rtB ->
B_1036_1038_0_n != 0.0 ) ) ; _rtB -> B_1036_1086_0_m = ! ( _rtB ->
B_1036_1038_0_n != 0.0 ) ; _rtB -> B_1036_1087_0 = ( ( _rtB -> B_1036_1081_0
!= 0.0 ) && _rtB -> B_1036_1086_0_m ) ; _rtB -> B_1036_1088_0_i = ( _rtB ->
B_1036_1085_0_k || _rtB -> B_1036_1087_0 ) ; _rtB -> B_1036_1089_0 = ! _rtB
-> B_1036_1045_0_m ; _rtB -> B_1036_1090_0_p = ( _rtB -> B_1036_1045_0_m || (
_rtB -> B_1036_1040_0 != 0.0 ) || ( _rtB -> B_1036_1040_0 != 0.0 ) || ( _rtB
-> B_1036_1040_0 != 0.0 ) ) ; _rtB -> B_1036_1091_0_h = ( ( _rtB ->
B_1036_1039_0_p != 0.0 ) || ( _rtB -> B_1036_1040_0 != 0.0 ) || ( _rtB ->
B_1036_1040_0 != 0.0 ) || ( _rtB -> B_1036_1040_0 != 0.0 ) ) ; _rtB ->
B_1036_1092_0_f = ( ( _rtB -> B_1036_1039_0_p != 0.0 ) || ( _rtB ->
B_1036_1040_0 != 0.0 ) || ( _rtB -> B_1036_1040_0 != 0.0 ) || ( _rtB ->
B_1036_1040_0 != 0.0 ) ) ; _rtB -> B_1036_1093_0 = _rtP -> P_1788 ; _rtB ->
B_1036_1094_0 = _rtP -> P_1789 ; _rtB -> B_1036_1095_0 = _rtP -> P_1790 ;
_rtB -> B_1036_1096_0 = _rtP -> P_1791 ; _rtB -> B_1036_1097_0_l = ! _rtB ->
B_1036_1090_0_p ; _rtB -> B_1036_1098_0_a = ( _rtB -> B_1036_1092_0_f && _rtB
-> B_1036_1097_0_l ) ; _rtB -> B_1036_1099_0_f = ( ( _rtB -> B_1036_1094_0 !=
0.0 ) && ( _rtB -> B_1036_1040_0 != 0.0 ) ) ; _rtB -> B_1036_1100_0_c = ! (
_rtB -> B_1036_1040_0 != 0.0 ) ; _rtB -> B_1036_1101_0_a = ( ( _rtB ->
B_1036_1095_0 != 0.0 ) && _rtB -> B_1036_1100_0_c ) ; _rtB -> B_1036_1102_0_n
= ( _rtB -> B_1036_1099_0_f || _rtB -> B_1036_1101_0_a ) ; _rtB ->
B_1036_1103_0_f = ( _rtB -> B_1036_1046_0_i || _rtB -> B_1036_1044_0 || (
_rtB -> B_1036_1036_0 != 0.0 ) || ( _rtB -> B_1036_1036_0 != 0.0 ) ) ; _rtB
-> B_1036_1104_0_k = ( ( _rtB -> B_1036_1034_0 != 0.0 ) || ( _rtB ->
B_1036_1036_0 != 0.0 ) || ( _rtB -> B_1036_1036_0 != 0.0 ) || ( _rtB ->
B_1036_1036_0 != 0.0 ) ) ; _rtB -> B_1036_1105_0_h = ( ( _rtB ->
B_1036_1034_0 != 0.0 ) || ( _rtB -> B_1036_1036_0 != 0.0 ) || ( _rtB ->
B_1036_1036_0 != 0.0 ) || ( _rtB -> B_1036_1036_0 != 0.0 ) ) ; _rtB ->
B_1036_1106_0_m = _rtP -> P_1792 ; _rtB -> B_1036_1107_0 = _rtP -> P_1793 ;
_rtB -> B_1036_1108_0_f = _rtP -> P_1794 ; _rtB -> B_1036_1109_0 = _rtP ->
P_1795 ; _rtB -> B_1036_1110_0_b = ! _rtB -> B_1036_1103_0_f ; _rtB ->
B_1036_1111_0_i = ( _rtB -> B_1036_1105_0_h && _rtB -> B_1036_1110_0_b ) ;
_rtB -> B_1036_1112_0_o = ( ( _rtB -> B_1036_1107_0 != 0.0 ) && ( _rtB ->
B_1036_1036_0 != 0.0 ) ) ; _rtB -> B_1036_1113_0_g = ! ( _rtB ->
B_1036_1036_0 != 0.0 ) ; _rtB -> B_1036_1114_0_b = ( ( _rtB ->
B_1036_1108_0_f != 0.0 ) && _rtB -> B_1036_1113_0_g ) ; _rtB ->
B_1036_1115_0_a = ( _rtB -> B_1036_1112_0_o || _rtB -> B_1036_1114_0_b ) ;
_rtB -> B_1036_1116_0_h = ( _rtB -> B_1036_1044_0 || _rtB -> B_1036_1046_0_i
|| ( _rtB -> B_1036_1029_0_m != 0.0 ) || ( _rtB -> B_1036_1031_0 != 0.0 ) ) ;
_rtB -> B_1036_1117_0_g = ( ( _rtB -> B_1036_1030_0_f != 0.0 ) || ( _rtB ->
B_1036_1031_0 != 0.0 ) || ( _rtB -> B_1036_1031_0 != 0.0 ) || ( _rtB ->
B_1036_1031_0 != 0.0 ) ) ; _rtB -> B_1036_1118_0_d = ( ( _rtB ->
B_1036_1030_0_f != 0.0 ) || ( _rtB -> B_1036_1031_0 != 0.0 ) || ( _rtB ->
B_1036_1031_0 != 0.0 ) || ( _rtB -> B_1036_1031_0 != 0.0 ) ) ; _rtB ->
B_1036_1119_0 = _rtP -> P_1796 ; _rtB -> B_1036_1120_0_n = _rtP -> P_1797 ;
_rtB -> B_1036_1121_0_o = _rtP -> P_1798 ; _rtB -> B_1036_1122_0_e = _rtP ->
P_1799 ; _rtB -> B_1036_1123_0_e = ! _rtB -> B_1036_1116_0_h ; _rtB ->
B_1036_1124_0_k = ( _rtB -> B_1036_1118_0_d && _rtB -> B_1036_1123_0_e ) ;
_rtB -> B_1036_1125_0_f = ( ( _rtB -> B_1036_1120_0_n != 0.0 ) && ( _rtB ->
B_1036_1031_0 != 0.0 ) ) ; _rtB -> B_1036_1126_0_n = ! ( _rtB ->
B_1036_1031_0 != 0.0 ) ; _rtB -> B_1036_1127_0_l = ( ( _rtB ->
B_1036_1121_0_o != 0.0 ) && _rtB -> B_1036_1126_0_n ) ; _rtB ->
B_1036_1128_0_a = ( _rtB -> B_1036_1125_0_f || _rtB -> B_1036_1127_0_l ) ;
_rtB -> B_1036_1129_0_m = ( _rtB -> B_1036_1042_0_b || _rtB ->
B_1036_1046_0_i || ( _rtB -> B_1036_1026_0_a != 0.0 ) || ( _rtB ->
B_1036_1026_0_a != 0.0 ) ) ; _rtB -> B_1036_1130_0_b = ( ( _rtB ->
B_1036_1025_0_p != 0.0 ) || ( _rtB -> B_1036_1026_0_a != 0.0 ) || ( _rtB ->
B_1036_1026_0_a != 0.0 ) || ( _rtB -> B_1036_1026_0_a != 0.0 ) ) ; _rtB ->
B_1036_1131_0_e = ( ( _rtB -> B_1036_1029_0_m != 0.0 ) || ( _rtB ->
B_1036_1026_0_a != 0.0 ) || ( _rtB -> B_1036_1026_0_a != 0.0 ) || ( _rtB ->
B_1036_1026_0_a != 0.0 ) ) ; _rtB -> B_1036_1132_0_m = _rtP -> P_1800 ; _rtB
-> B_1036_1133_0_c = _rtP -> P_1801 ; _rtB -> B_1036_1134_0_g = _rtP ->
P_1802 ; _rtB -> B_1036_1135_0 = _rtP -> P_1803 ; _rtB -> B_1036_1136_0_g = !
_rtB -> B_1036_1129_0_m ; _rtB -> B_1036_1137_0_b = ( _rtB -> B_1036_1131_0_e
&& _rtB -> B_1036_1136_0_g ) ; _rtB -> B_1036_1138_0_m = ( ( _rtB ->
B_1036_1133_0_c != 0.0 ) && ( _rtB -> B_1036_1026_0_a != 0.0 ) ) ; _rtB ->
B_1036_1139_0_f = ! ( _rtB -> B_1036_1026_0_a != 0.0 ) ; _rtB ->
B_1036_1140_0_e = ( ( _rtB -> B_1036_1134_0_g != 0.0 ) && _rtB ->
B_1036_1139_0_f ) ; _rtB -> B_1036_1141_0_d = ( _rtB -> B_1036_1138_0_m ||
_rtB -> B_1036_1140_0_e ) ; _rtB -> B_1036_1142_0_b = _rtP -> P_1804 ; _rtB
-> B_1036_1143_0_p = _rtP -> P_1805 ; _rtB -> B_1036_1144_0_p = _rtP ->
P_1806 ; _rtB -> B_1036_1145_0_a [ 0 ] = _rtP -> P_1807 [ 0 ] ; _rtB ->
B_1036_1146_0_f [ 0 ] = _rtP -> P_1808 [ 0 ] ; _rtB -> B_1036_1147_0_g [ 0 ]
= _rtP -> P_1809 [ 0 ] ; _rtB -> B_1036_1145_0_a [ 1 ] = _rtP -> P_1807 [ 1 ]
; _rtB -> B_1036_1146_0_f [ 1 ] = _rtP -> P_1808 [ 1 ] ; _rtB ->
B_1036_1147_0_g [ 1 ] = _rtP -> P_1809 [ 1 ] ; switch ( ( int32_T ) _rtB ->
B_1036_1144_0_p ) { case 1 : _rtB -> B_1036_1148_0_b [ 0 ] = _rtB ->
B_1036_1145_0_a [ 0 ] ; _rtB -> B_1036_1148_0_b [ 1 ] = _rtB ->
B_1036_1145_0_a [ 1 ] ; break ; case 2 : _rtB -> B_1036_1148_0_b [ 0 ] = _rtB
-> B_1036_1146_0_f [ 0 ] ; _rtB -> B_1036_1148_0_b [ 1 ] = _rtB ->
B_1036_1146_0_f [ 1 ] ; break ; default : _rtB -> B_1036_1148_0_b [ 0 ] =
_rtB -> B_1036_1147_0_g [ 0 ] ; _rtB -> B_1036_1148_0_b [ 1 ] = _rtB ->
B_1036_1147_0_g [ 1 ] ; break ; } _rtB -> B_1036_1149_0_k = _rtP -> P_1810 ;
_rtB -> B_1036_1150_0_k = _rtP -> P_1811 ; _rtB -> B_1036_1151_0_b [ 0 ] =
_rtP -> P_1812 [ 0 ] ; _rtB -> B_1036_1152_0_i [ 0 ] = _rtP -> P_1813 [ 0 ] ;
_rtB -> B_1036_1153_0_n [ 0 ] = _rtP -> P_1814 [ 0 ] ; _rtB ->
B_1036_1151_0_b [ 1 ] = _rtP -> P_1812 [ 1 ] ; _rtB -> B_1036_1152_0_i [ 1 ]
= _rtP -> P_1813 [ 1 ] ; _rtB -> B_1036_1153_0_n [ 1 ] = _rtP -> P_1814 [ 1 ]
; switch ( ( int32_T ) _rtB -> B_1036_1150_0_k ) { case 1 : _rtB ->
B_1036_1154_0_h [ 0 ] = _rtB -> B_1036_1151_0_b [ 0 ] ; _rtB ->
B_1036_1154_0_h [ 1 ] = _rtB -> B_1036_1151_0_b [ 1 ] ; break ; case 2 : _rtB
-> B_1036_1154_0_h [ 0 ] = _rtB -> B_1036_1152_0_i [ 0 ] ; _rtB ->
B_1036_1154_0_h [ 1 ] = _rtB -> B_1036_1152_0_i [ 1 ] ; break ; default :
_rtB -> B_1036_1154_0_h [ 0 ] = _rtB -> B_1036_1153_0_n [ 0 ] ; _rtB ->
B_1036_1154_0_h [ 1 ] = _rtB -> B_1036_1153_0_n [ 1 ] ; break ; } _rtB ->
B_1036_1155_0_n = _rtP -> P_1815 ; _rtB -> B_1036_1156_0_m = _rtP -> P_1816 ;
_rtB -> B_1036_1157_0_g [ 0 ] = _rtP -> P_1817 [ 0 ] ; _rtB ->
B_1036_1158_0_j [ 0 ] = _rtP -> P_1818 [ 0 ] ; _rtB -> B_1036_1159_0_f [ 0 ]
= _rtP -> P_1819 [ 0 ] ; _rtB -> B_1036_1157_0_g [ 1 ] = _rtP -> P_1817 [ 1 ]
; _rtB -> B_1036_1158_0_j [ 1 ] = _rtP -> P_1818 [ 1 ] ; _rtB ->
B_1036_1159_0_f [ 1 ] = _rtP -> P_1819 [ 1 ] ; switch ( ( int32_T ) _rtB ->
B_1036_1156_0_m ) { case 1 : _rtB -> B_1036_1160_0_n [ 0 ] = _rtB ->
B_1036_1157_0_g [ 0 ] ; _rtB -> B_1036_1160_0_n [ 1 ] = _rtB ->
B_1036_1157_0_g [ 1 ] ; break ; case 2 : _rtB -> B_1036_1160_0_n [ 0 ] = _rtB
-> B_1036_1158_0_j [ 0 ] ; _rtB -> B_1036_1160_0_n [ 1 ] = _rtB ->
B_1036_1158_0_j [ 1 ] ; break ; default : _rtB -> B_1036_1160_0_n [ 0 ] =
_rtB -> B_1036_1159_0_f [ 0 ] ; _rtB -> B_1036_1160_0_n [ 1 ] = _rtB ->
B_1036_1159_0_f [ 1 ] ; break ; } _rtB -> B_1036_1161_0_g = _rtP -> P_1820 ;
_rtB -> B_1036_1162_0_d = _rtP -> P_1821 ; _rtB -> B_1036_1163_0_c = _rtP ->
P_1822 ; _rtB -> B_1036_1164_0_m = _rtP -> P_1823 ; _rtB -> B_1036_1165_0 =
_rtP -> P_1824 ; _rtB -> B_1036_1166_0 = _rtP -> P_1825 ; _rtB ->
B_1036_1167_0_d = _rtP -> P_1826 ; _rtB -> B_1036_1168_0_n = _rtP -> P_1827 ;
_rtB -> B_1036_1169_0 = _rtP -> P_1828 ; _rtB -> B_1036_1170_0 = _rtP ->
P_1829 ; _rtB -> B_1036_1171_0_b = _rtP -> P_1830 ; _rtB -> B_1036_1172_0 =
_rtP -> P_1831 ; _rtB -> B_1036_1173_0_p = _rtP -> P_1832 ; _rtB ->
B_1036_1174_0 = _rtP -> P_1833 ; _rtB -> B_1036_1175_0_k = _rtP -> P_1834 ;
_rtB -> B_1036_1176_0 = _rtP -> P_1835 ; _rtB -> B_1036_1177_0_c = _rtP ->
P_1836 ; _rtB -> B_1036_1178_0 = _rtP -> P_1837 ; _rtB -> B_1036_1179_0_n =
_rtP -> P_1838 ; _rtB -> B_1036_1180_0 = _rtP -> P_1839 ; _rtB ->
B_1036_1181_0 = _rtP -> P_1840 ; _rtB -> B_1036_1182_0 = _rtP -> P_1841 ;
_rtB -> B_1036_1183_0 = _rtP -> P_1842 ; _rtB -> B_1036_1184_0_m = false ;
_rtB -> B_1036_1185_0 = _rtP -> P_1843 ; _rtB -> B_1036_1186_0 = _rtP ->
P_1844 ; _rtB -> B_1036_1187_0 = _rtP -> P_1845 ; _rtB -> B_1036_1188_0 =
_rtP -> P_1846 ; _rtB -> B_1036_1189_0 = _rtP -> P_1847 ; _rtB ->
B_1036_1190_0 = _rtP -> P_1848 ; _rtB -> B_1036_1191_0_n = _rtP -> P_1849 ;
_rtB -> B_1036_1192_0_c = _rtP -> P_1850 ; _rtB -> B_1036_1193_0_d = _rtP ->
P_1851 ; _rtB -> B_1036_1194_0_n = _rtB -> B_1036_1189_0 - _rtB ->
B_1036_1171_0_b ; _rtB -> B_1036_1195_0 = _rtB -> B_1036_1170_0 - _rtB ->
B_1036_1171_0_b ; _rtB -> B_1036_1196_0 = _rtB -> B_1036_1172_0 - _rtB ->
B_1036_1192_0_c ; _rtB -> B_1036_1197_0_g = _rtB -> B_1036_1195_0 / _rtB ->
B_1036_1196_0 ; _rtB -> B_1036_1198_0_l = _rtB -> B_1036_1194_0_n / _rtB ->
B_1036_1197_0_g ; _rtB -> B_1036_1199_0_g = _rtB -> B_1036_1198_0_l + _rtB ->
B_1036_1192_0_c ; _rtB -> B_1036_1200_0_p = _rtP -> P_1852 ; _rtB ->
B_1036_1201_0_k = _rtP -> P_1853 ; _rtB -> B_1036_1202_0_f = ( _rtB ->
B_1036_1173_0_p == _rtB -> B_1036_1201_0_k ) ; _rtB -> B_1036_1203_0_h = (
_rtB -> B_1036_1174_0 == _rtB -> B_1036_1201_0_k ) ; _rtB -> B_1036_1204_0_e
= ( _rtB -> B_1036_1202_0_f && _rtB -> B_1036_1203_0_h ) ; _rtB ->
B_1036_1205_0_f = ( _rtB -> B_1036_1189_0 == _rtB -> B_1036_1200_0_p ) ; _rtB
-> B_1036_1206_0_e = ! _rtB -> B_1036_1205_0_f ; _rtB -> B_1036_1207_0_o = (
_rtB -> B_1036_1204_0_e && _rtB -> B_1036_1206_0_e ) ; if ( _rtB ->
B_1036_1207_0_o ) { _rtB -> B_1036_1208_0_l = _rtB -> B_1036_1199_0_g ; }
else { _rtB -> B_1036_1208_0_l = _rtB -> B_1036_1172_0 ; } _rtB ->
B_1036_1209_0_b = _rtP -> P_1854 ; _rtB -> B_1036_1210_0_j = ( _rtB ->
B_1036_1171_0_b == _rtB -> B_1036_1209_0_b ) ; _rtB -> B_1036_1211_0_f = (
_rtB -> B_1036_1192_0_c == _rtB -> B_1036_1209_0_b ) ; _rtB ->
B_1036_1212_0_k = ( _rtB -> B_1036_1210_0_j && _rtB -> B_1036_1211_0_f ) ;
_rtB -> B_1036_1213_0_c = _rtP -> P_1855 ; _rtB -> B_1036_1214_0_n = ( _rtB
-> B_1036_1170_0 == _rtB -> B_1036_1213_0_c ) ; _rtB -> B_1036_1215_0_f = (
_rtB -> B_1036_1172_0 == _rtB -> B_1036_1213_0_c ) ; _rtB -> B_1036_1216_0_p
= ( _rtB -> B_1036_1214_0_n && _rtB -> B_1036_1215_0_f ) ; _rtB ->
B_1036_1217_0_i = ( _rtB -> B_1036_1212_0_k || _rtB -> B_1036_1216_0_p ) ;
_rtB -> B_1036_1218_0 = _rtB -> B_1036_1171_0_b * _rtB -> B_1036_1172_0 ;
_rtB -> B_1036_1219_0_f = _rtB -> B_1036_1172_0 - _rtB -> B_1036_1192_0_c ;
_rtB -> B_1036_1220_0_o = _rtB -> B_1036_1170_0 * _rtB -> B_1036_1192_0_c ;
_rtB -> B_1036_1221_0_d = _rtB -> B_1036_1218_0 - _rtB -> B_1036_1220_0_o ;
_rtB -> B_1036_1222_0_l = _rtB -> B_1036_1221_0_d / _rtB -> B_1036_1219_0_f ;
_rtB -> B_1036_1223_0_l = _rtB -> B_1036_1189_0 - _rtB -> B_1036_1170_0 ;
_rtB -> B_1036_1224_0_d = _rtB -> B_1036_1173_0_p - _rtB -> B_1036_1170_0 ;
_rtB -> B_1036_1225_0_k = _rtB -> B_1036_1174_0 - _rtB -> B_1036_1172_0 ;
_rtB -> B_1036_1226_0 = _rtB -> B_1036_1224_0_d / _rtB -> B_1036_1225_0_k ;
_rtB -> B_1036_1227_0_f = _rtB -> B_1036_1223_0_l / _rtB -> B_1036_1226_0 ;
_rtB -> B_1036_1228_0_p = _rtB -> B_1036_1227_0_f + _rtB -> B_1036_1172_0 ;
_rtB -> B_1036_1229_0 = _rtP -> P_1856 ; _rtB -> B_1036_1230_0 = _rtP ->
P_1857 ; _rtB -> B_1036_1231_0_k = ( _rtB -> B_1036_1175_0_k == _rtB ->
B_1036_1230_0 ) ; _rtB -> B_1036_1232_0_i = ( _rtB -> B_1036_1176_0 == _rtB
-> B_1036_1230_0 ) ; _rtB -> B_1036_1233_0_c = ( _rtB -> B_1036_1231_0_k &&
_rtB -> B_1036_1232_0_i ) ; _rtB -> B_1036_1234_0_b = ( _rtB -> B_1036_1189_0
== _rtB -> B_1036_1229_0 ) ; _rtB -> B_1036_1235_0_k = ! _rtB ->
B_1036_1234_0_b ; _rtB -> B_1036_1236_0_l = ( _rtB -> B_1036_1233_0_c && _rtB
-> B_1036_1235_0_k ) ; if ( _rtB -> B_1036_1236_0_l ) { _rtB -> B_1036_1237_0
= _rtB -> B_1036_1228_0_p ; } else { _rtB -> B_1036_1237_0 = _rtB ->
B_1036_1174_0 ; } _rtB -> B_1036_1238_0 = _rtP -> P_1858 ; _rtB ->
B_1036_1239_0_b = ( _rtB -> B_1036_1170_0 == _rtB -> B_1036_1238_0 ) ; _rtB
-> B_1036_1240_0_m = ( _rtB -> B_1036_1172_0 == _rtB -> B_1036_1238_0 ) ;
_rtB -> B_1036_1241_0_b = ( _rtB -> B_1036_1239_0_b && _rtB ->
B_1036_1240_0_m ) ; _rtB -> B_1036_1242_0 = _rtP -> P_1859 ; _rtB ->
B_1036_1243_0_l = ( _rtB -> B_1036_1173_0_p == _rtB -> B_1036_1242_0 ) ; _rtB
-> B_1036_1244_0 = ( _rtB -> B_1036_1174_0 == _rtB -> B_1036_1242_0 ) ; _rtB
-> B_1036_1245_0_g = ( _rtB -> B_1036_1243_0_l && _rtB -> B_1036_1244_0 ) ;
_rtB -> B_1036_1246_0_p = ( _rtB -> B_1036_1241_0_b || _rtB ->
B_1036_1245_0_g ) ; _rtB -> B_1036_1247_0 = _rtB -> B_1036_1170_0 * _rtB ->
B_1036_1174_0 ; _rtB -> B_1036_1248_0 = _rtB -> B_1036_1174_0 - _rtB ->
B_1036_1172_0 ; _rtB -> B_1036_1249_0 = _rtB -> B_1036_1173_0_p * _rtB ->
B_1036_1172_0 ; _rtB -> B_1036_1250_0_k = _rtB -> B_1036_1247_0 - _rtB ->
B_1036_1249_0 ; _rtB -> B_1036_1251_0_k = _rtB -> B_1036_1250_0_k / _rtB ->
B_1036_1248_0 ; _rtB -> B_1036_1252_0_i = _rtB -> B_1036_1189_0 - _rtB ->
B_1036_1173_0_p ; _rtB -> B_1036_1253_0_e = _rtB -> B_1036_1175_0_k - _rtB ->
B_1036_1173_0_p ; _rtB -> B_1036_1254_0 = _rtB -> B_1036_1176_0 - _rtB ->
B_1036_1174_0 ; _rtB -> B_1036_1255_0 = _rtB -> B_1036_1253_0_e / _rtB ->
B_1036_1254_0 ; _rtB -> B_1036_1256_0_f = _rtB -> B_1036_1252_0_i / _rtB ->
B_1036_1255_0 ; _rtB -> B_1036_1257_0_k = _rtB -> B_1036_1256_0_f + _rtB ->
B_1036_1174_0 ; _rtB -> B_1036_1258_0_n = _rtP -> P_1860 ; _rtB ->
B_1036_1259_0_i = _rtP -> P_1861 ; _rtB -> B_1036_1260_0_c = ( _rtB ->
B_1036_1177_0_c == _rtB -> B_1036_1259_0_i ) ; _rtB -> B_1036_1261_0_m = (
_rtB -> B_1036_1178_0 == _rtB -> B_1036_1259_0_i ) ; _rtB -> B_1036_1262_0_i
= ( _rtB -> B_1036_1260_0_c && _rtB -> B_1036_1261_0_m ) ; _rtB ->
B_1036_1263_0_a = ( _rtB -> B_1036_1189_0 == _rtB -> B_1036_1258_0_n ) ; _rtB
-> B_1036_1264_0_d = ! _rtB -> B_1036_1263_0_a ; _rtB -> B_1036_1265_0_h = (
_rtB -> B_1036_1262_0_i && _rtB -> B_1036_1264_0_d ) ; if ( _rtB ->
B_1036_1265_0_h ) { _rtB -> B_1036_1266_0_i = _rtB -> B_1036_1257_0_k ; }
else { _rtB -> B_1036_1266_0_i = _rtB -> B_1036_1176_0 ; } _rtB ->
B_1036_1267_0_o = _rtP -> P_1862 ; _rtB -> B_1036_1268_0_e = ( _rtB ->
B_1036_1173_0_p == _rtB -> B_1036_1267_0_o ) ; _rtB -> B_1036_1269_0_b = (
_rtB -> B_1036_1174_0 == _rtB -> B_1036_1267_0_o ) ; _rtB -> B_1036_1270_0_c
= ( _rtB -> B_1036_1268_0_e && _rtB -> B_1036_1269_0_b ) ; _rtB ->
B_1036_1271_0_d = _rtP -> P_1863 ; _rtB -> B_1036_1272_0_c = ( _rtB ->
B_1036_1175_0_k == _rtB -> B_1036_1271_0_d ) ; _rtB -> B_1036_1273_0_i = (
_rtB -> B_1036_1176_0 == _rtB -> B_1036_1271_0_d ) ; _rtB -> B_1036_1274_0_p
= ( _rtB -> B_1036_1272_0_c && _rtB -> B_1036_1273_0_i ) ; _rtB ->
B_1036_1275_0_l = ( _rtB -> B_1036_1270_0_c || _rtB -> B_1036_1274_0_p ) ;
_rtB -> B_1036_1276_0 = _rtB -> B_1036_1173_0_p * _rtB -> B_1036_1176_0 ;
_rtB -> B_1036_1277_0 = _rtB -> B_1036_1176_0 - _rtB -> B_1036_1174_0 ; _rtB
-> B_1036_1278_0_i = _rtB -> B_1036_1175_0_k * _rtB -> B_1036_1174_0 ; _rtB
-> B_1036_1279_0_g = _rtB -> B_1036_1276_0 - _rtB -> B_1036_1278_0_i ; _rtB
-> B_1036_1280_0_c = _rtB -> B_1036_1279_0_g / _rtB -> B_1036_1277_0 ; _rtB
-> B_1036_1281_0_e = _rtB -> B_1036_1189_0 - _rtB -> B_1036_1175_0_k ; _rtB
-> B_1036_1282_0_a = _rtB -> B_1036_1177_0_c - _rtB -> B_1036_1175_0_k ; _rtB
-> B_1036_1283_0_h = _rtB -> B_1036_1178_0 - _rtB -> B_1036_1176_0 ; _rtB ->
B_1036_1284_0_e = _rtB -> B_1036_1282_0_a / _rtB -> B_1036_1283_0_h ; _rtB ->
B_1036_1285_0 = _rtB -> B_1036_1281_0_e / _rtB -> B_1036_1284_0_e ; _rtB ->
B_1036_1286_0_d = _rtB -> B_1036_1285_0 + _rtB -> B_1036_1176_0 ; _rtB ->
B_1036_1287_0_g = _rtP -> P_1864 ; _rtB -> B_1036_1288_0 = _rtP -> P_1865 ;
_rtB -> B_1036_1289_0_n = ( _rtB -> B_1036_1193_0_d == _rtB -> B_1036_1288_0
) ; _rtB -> B_1036_1290_0_o = ( _rtB -> B_1036_1193_0_d == _rtB ->
B_1036_1288_0 ) ; _rtB -> B_1036_1291_0_f = ( _rtB -> B_1036_1289_0_n && _rtB
-> B_1036_1290_0_o ) ; _rtB -> B_1036_1292_0_a = ( _rtB -> B_1036_1189_0 ==
_rtB -> B_1036_1287_0_g ) ; _rtB -> B_1036_1293_0_i = ! _rtB ->
B_1036_1292_0_a ; _rtB -> B_1036_1294_0_l = ( _rtB -> B_1036_1291_0_f && _rtB
-> B_1036_1293_0_i ) ; if ( _rtB -> B_1036_1294_0_l ) { _rtB -> B_1036_1295_0
= _rtB -> B_1036_1286_0_d ; } else { _rtB -> B_1036_1295_0 = _rtB ->
B_1036_1178_0 ; } _rtB -> B_1036_1296_0 = _rtP -> P_1866 ; _rtB ->
B_1036_1297_0_h = ( _rtB -> B_1036_1175_0_k == _rtB -> B_1036_1296_0 ) ; _rtB
-> B_1036_1298_0_k = ( _rtB -> B_1036_1176_0 == _rtB -> B_1036_1296_0 ) ;
_rtB -> B_1036_1299_0_n = ( _rtB -> B_1036_1297_0_h && _rtB ->
B_1036_1298_0_k ) ; _rtB -> B_1036_1300_0 = _rtP -> P_1867 ; _rtB ->
B_1036_1301_0 = ( _rtB -> B_1036_1177_0_c == _rtB -> B_1036_1300_0 ) ; _rtB
-> B_1036_1302_0_k = ( _rtB -> B_1036_1178_0 == _rtB -> B_1036_1300_0 ) ;
_rtB -> B_1036_1303_0 = ( _rtB -> B_1036_1301_0 && _rtB -> B_1036_1302_0_k )
; _rtB -> B_1036_1304_0_c = ( _rtB -> B_1036_1299_0_n || _rtB ->
B_1036_1303_0 ) ; _rtB -> B_1036_1305_0 = _rtB -> B_1036_1175_0_k * _rtB ->
B_1036_1178_0 ; _rtB -> B_1036_1306_0 = _rtB -> B_1036_1178_0 - _rtB ->
B_1036_1176_0 ; _rtB -> B_1036_1307_0 = _rtB -> B_1036_1177_0_c * _rtB ->
B_1036_1176_0 ; _rtB -> B_1036_1308_0 = _rtB -> B_1036_1305_0 - _rtB ->
B_1036_1307_0 ; _rtB -> B_1036_1309_0 = _rtB -> B_1036_1308_0 / _rtB ->
B_1036_1306_0 ; _rtB -> B_1036_1310_0 = _rtP -> P_1868 ; _rtB ->
B_1036_1311_0_e = _rtB -> B_1036_1189_0 - _rtB -> B_1036_1171_0_b ; _rtB ->
B_1036_1312_0 = _rtB -> B_1036_1170_0 - _rtB -> B_1036_1171_0_b ; _rtB ->
B_1036_1313_0 = _rtB -> B_1036_1172_0 - _rtB -> B_1036_1192_0_c ; _rtB ->
B_1036_1314_0 = _rtB -> B_1036_1312_0 / _rtB -> B_1036_1313_0 ; _rtB ->
B_1036_1315_0 = _rtB -> B_1036_1311_0_e / _rtB -> B_1036_1314_0 ; _rtB ->
B_1036_1316_0 = _rtB -> B_1036_1315_0 + _rtB -> B_1036_1192_0_c ; _rtB ->
B_1036_1317_0 = _rtP -> P_1869 ; _rtB -> B_1036_1318_0 = _rtP -> P_1870 ;
_rtB -> B_1036_1319_0_h = ( _rtB -> B_1036_1173_0_p == _rtB -> B_1036_1318_0
) ; _rtB -> B_1036_1320_0_m = ( _rtB -> B_1036_1174_0 == _rtB ->
B_1036_1318_0 ) ; _rtB -> B_1036_1321_0_f = ( _rtB -> B_1036_1319_0_h && _rtB
-> B_1036_1320_0_m ) ; _rtB -> B_1036_1322_0_g = ( _rtB -> B_1036_1189_0 ==
_rtB -> B_1036_1317_0 ) ; _rtB -> B_1036_1323_0_d = ! _rtB -> B_1036_1322_0_g
; _rtB -> B_1036_1324_0_c = ( _rtB -> B_1036_1321_0_f && _rtB ->
B_1036_1323_0_d ) ; if ( _rtB -> B_1036_1324_0_c ) { _rtB -> B_1036_1325_0 =
_rtB -> B_1036_1316_0 ; } else { _rtB -> B_1036_1325_0 = _rtB ->
B_1036_1172_0 ; } _rtB -> B_1036_1326_0 = _rtP -> P_1871 ; _rtB ->
B_1036_1327_0 = ( _rtB -> B_1036_1171_0_b == _rtB -> B_1036_1326_0 ) ; _rtB
-> B_1036_1328_0_n = ( _rtB -> B_1036_1192_0_c == _rtB -> B_1036_1326_0 ) ;
_rtB -> B_1036_1329_0_n = ( _rtB -> B_1036_1327_0 && _rtB -> B_1036_1328_0_n
) ; _rtB -> B_1036_1330_0 = _rtP -> P_1872 ; _rtB -> B_1036_1331_0_d = ( _rtB
-> B_1036_1170_0 == _rtB -> B_1036_1330_0 ) ; _rtB -> B_1036_1332_0_n = (
_rtB -> B_1036_1172_0 == _rtB -> B_1036_1330_0 ) ; _rtB -> B_1036_1333_0_l =
( _rtB -> B_1036_1331_0_d && _rtB -> B_1036_1332_0_n ) ; _rtB ->
B_1036_1334_0_p = ( _rtB -> B_1036_1329_0_n || _rtB -> B_1036_1333_0_l ) ;
_rtB -> B_1036_1335_0 = _rtB -> B_1036_1171_0_b * _rtB -> B_1036_1172_0 ;
_rtB -> B_1036_1336_0_e = _rtB -> B_1036_1172_0 - _rtB -> B_1036_1192_0_c ;
_rtB -> B_1036_1337_0 = _rtB -> B_1036_1170_0 * _rtB -> B_1036_1192_0_c ;
_rtB -> B_1036_1338_0 = _rtB -> B_1036_1335_0 - _rtB -> B_1036_1337_0 ; _rtB
-> B_1036_1339_0 = _rtB -> B_1036_1338_0 / _rtB -> B_1036_1336_0_e ; _rtB ->
B_1036_1340_0 = _rtB -> B_1036_1189_0 - _rtB -> B_1036_1170_0 ; _rtB ->
B_1036_1341_0 = _rtB -> B_1036_1173_0_p - _rtB -> B_1036_1170_0 ; _rtB ->
B_1036_1342_0_g = _rtB -> B_1036_1174_0 - _rtB -> B_1036_1172_0 ; _rtB ->
B_1036_1343_0 = _rtB -> B_1036_1341_0 / _rtB -> B_1036_1342_0_g ; _rtB ->
B_1036_1344_0_g = _rtB -> B_1036_1340_0 / _rtB -> B_1036_1343_0 ; _rtB ->
B_1036_1345_0 = _rtB -> B_1036_1344_0_g + _rtB -> B_1036_1172_0 ; _rtB ->
B_1036_1346_0 = _rtP -> P_1873 ; _rtB -> B_1036_1347_0 = _rtP -> P_1874 ;
_rtB -> B_1036_1348_0_e = ( _rtB -> B_1036_1175_0_k == _rtB -> B_1036_1347_0
) ; _rtB -> B_1036_1349_0_o = ( _rtB -> B_1036_1176_0 == _rtB ->
B_1036_1347_0 ) ; _rtB -> B_1036_1350_0_p = ( _rtB -> B_1036_1348_0_e && _rtB
-> B_1036_1349_0_o ) ; _rtB -> B_1036_1351_0_i = ( _rtB -> B_1036_1189_0 ==
_rtB -> B_1036_1346_0 ) ; _rtB -> B_1036_1352_0_d = ! _rtB -> B_1036_1351_0_i
; _rtB -> B_1036_1353_0_b = ( _rtB -> B_1036_1350_0_p && _rtB ->
B_1036_1352_0_d ) ; if ( _rtB -> B_1036_1353_0_b ) { _rtB -> B_1036_1354_0 =
_rtB -> B_1036_1345_0 ; } else { _rtB -> B_1036_1354_0 = _rtB ->
B_1036_1174_0 ; } _rtB -> B_1036_1355_0 = _rtP -> P_1875 ; _rtB ->
B_1036_1356_0_b = ( _rtB -> B_1036_1170_0 == _rtB -> B_1036_1355_0 ) ; _rtB
-> B_1036_1357_0_l = ( _rtB -> B_1036_1172_0 == _rtB -> B_1036_1355_0 ) ;
_rtB -> B_1036_1358_0_b = ( _rtB -> B_1036_1356_0_b && _rtB ->
B_1036_1357_0_l ) ; _rtB -> B_1036_1359_0_d = _rtP -> P_1876 ; _rtB ->
B_1036_1360_0_n = ( _rtB -> B_1036_1173_0_p == _rtB -> B_1036_1359_0_d ) ;
_rtB -> B_1036_1361_0_p = ( _rtB -> B_1036_1174_0 == _rtB -> B_1036_1359_0_d
) ; _rtB -> B_1036_1362_0_o = ( _rtB -> B_1036_1360_0_n && _rtB ->
B_1036_1361_0_p ) ; _rtB -> B_1036_1363_0_p = ( _rtB -> B_1036_1358_0_b ||
_rtB -> B_1036_1362_0_o ) ; _rtB -> B_1036_1364_0_n = _rtB -> B_1036_1170_0 *
_rtB -> B_1036_1174_0 ; _rtB -> B_1036_1365_0_p = _rtB -> B_1036_1174_0 -
_rtB -> B_1036_1172_0 ; _rtB -> B_1036_1366_0_b = _rtB -> B_1036_1173_0_p *
_rtB -> B_1036_1172_0 ; _rtB -> B_1036_1367_0_i = _rtB -> B_1036_1364_0_n -
_rtB -> B_1036_1366_0_b ; _rtB -> B_1036_1368_0_b = _rtB -> B_1036_1367_0_i /
_rtB -> B_1036_1365_0_p ; _rtB -> B_1036_1369_0_j = _rtB -> B_1036_1189_0 -
_rtB -> B_1036_1173_0_p ; _rtB -> B_1036_1370_0_g = _rtB -> B_1036_1175_0_k -
_rtB -> B_1036_1173_0_p ; _rtB -> B_1036_1371_0 = _rtB -> B_1036_1176_0 -
_rtB -> B_1036_1174_0 ; _rtB -> B_1036_1372_0_n = _rtB -> B_1036_1370_0_g /
_rtB -> B_1036_1371_0 ; _rtB -> B_1036_1373_0_p = _rtB -> B_1036_1369_0_j /
_rtB -> B_1036_1372_0_n ; _rtB -> B_1036_1374_0_f = _rtB -> B_1036_1373_0_p +
_rtB -> B_1036_1174_0 ; _rtB -> B_1036_1375_0_f = _rtP -> P_1877 ; _rtB ->
B_1036_1376_0_a = _rtP -> P_1878 ; _rtB -> B_1036_1377_0_h = ( _rtB ->
B_1036_1177_0_c == _rtB -> B_1036_1376_0_a ) ; _rtB -> B_1036_1378_0_o = (
_rtB -> B_1036_1178_0 == _rtB -> B_1036_1376_0_a ) ; _rtB -> B_1036_1379_0_b
= ( _rtB -> B_1036_1377_0_h && _rtB -> B_1036_1378_0_o ) ; _rtB ->
B_1036_1380_0_c = ( _rtB -> B_1036_1189_0 == _rtB -> B_1036_1375_0_f ) ; _rtB
-> B_1036_1381_0_g = ! _rtB -> B_1036_1380_0_c ; _rtB -> B_1036_1382_0_e = (
_rtB -> B_1036_1379_0_b && _rtB -> B_1036_1381_0_g ) ; if ( _rtB ->
B_1036_1382_0_e ) { _rtB -> B_1036_1383_0_g = _rtB -> B_1036_1374_0_f ; }
else { _rtB -> B_1036_1383_0_g = _rtB -> B_1036_1176_0 ; } _rtB ->
B_1036_1384_0_m = _rtP -> P_1879 ; _rtB -> B_1036_1385_0_o = ( _rtB ->
B_1036_1173_0_p == _rtB -> B_1036_1384_0_m ) ; _rtB -> B_1036_1386_0_l = (
_rtB -> B_1036_1174_0 == _rtB -> B_1036_1384_0_m ) ; _rtB -> B_1036_1387_0_o
= ( _rtB -> B_1036_1385_0_o && _rtB -> B_1036_1386_0_l ) ; _rtB ->
B_1036_1388_0_d = _rtP -> P_1880 ; _rtB -> B_1036_1389_0_e = ( _rtB ->
B_1036_1175_0_k == _rtB -> B_1036_1388_0_d ) ; _rtB -> B_1036_1390_0_o = (
_rtB -> B_1036_1176_0 == _rtB -> B_1036_1388_0_d ) ; _rtB -> B_1036_1391_0_l
= ( _rtB -> B_1036_1389_0_e && _rtB -> B_1036_1390_0_o ) ; _rtB ->
B_1036_1392_0_o = ( _rtB -> B_1036_1387_0_o || _rtB -> B_1036_1391_0_l ) ;
_rtB -> B_1036_1393_0_e = _rtB -> B_1036_1173_0_p * _rtB -> B_1036_1176_0 ;
_rtB -> B_1036_1394_0_e = _rtB -> B_1036_1176_0 - _rtB -> B_1036_1174_0 ;
_rtB -> B_1036_1395_0_d = _rtB -> B_1036_1175_0_k * _rtB -> B_1036_1174_0 ;
_rtB -> B_1036_1396_0_j = _rtB -> B_1036_1393_0_e - _rtB -> B_1036_1395_0_d ;
_rtB -> B_1036_1397_0_n = _rtB -> B_1036_1396_0_j / _rtB -> B_1036_1394_0_e ;
_rtB -> B_1036_1398_0_j = _rtB -> B_1036_1189_0 - _rtB -> B_1036_1175_0_k ;
_rtB -> B_1036_1399_0_p = _rtB -> B_1036_1177_0_c - _rtB -> B_1036_1175_0_k ;
_rtB -> B_1036_1400_0_a = _rtB -> B_1036_1178_0 - _rtB -> B_1036_1176_0 ;
_rtB -> B_1036_1401_0 = _rtB -> B_1036_1399_0_p / _rtB -> B_1036_1400_0_a ;
_rtB -> B_1036_1402_0 = _rtB -> B_1036_1398_0_j / _rtB -> B_1036_1401_0 ;
_rtB -> B_1036_1403_0_d = _rtB -> B_1036_1402_0 + _rtB -> B_1036_1176_0 ;
_rtB -> B_1036_1404_0_p = _rtP -> P_1881 ; _rtB -> B_1036_1405_0 = _rtP ->
P_1882 ; _rtB -> B_1036_1406_0 = ( _rtB -> B_1036_1310_0 == _rtB ->
B_1036_1405_0 ) ; _rtB -> B_1036_1407_0_a = ( _rtB -> B_1036_1310_0 == _rtB
-> B_1036_1405_0 ) ; _rtB -> B_1036_1408_0 = ( _rtB -> B_1036_1406_0 && _rtB
-> B_1036_1407_0_a ) ; _rtB -> B_1036_1409_0_b = ( _rtB -> B_1036_1189_0 ==
_rtB -> B_1036_1404_0_p ) ; _rtB -> B_1036_1410_0 = ! _rtB -> B_1036_1409_0_b
; _rtB -> B_1036_1411_0_o = ( _rtB -> B_1036_1408_0 && _rtB -> B_1036_1410_0
) ; if ( _rtB -> B_1036_1411_0_o ) { _rtB -> B_1036_1412_0 = _rtB ->
B_1036_1403_0_d ; } else { _rtB -> B_1036_1412_0 = _rtB -> B_1036_1178_0 ; }
_rtB -> B_1036_1413_0_d = _rtP -> P_1883 ; _rtB -> B_1036_1414_0 = ( _rtB ->
B_1036_1175_0_k == _rtB -> B_1036_1413_0_d ) ; _rtB -> B_1036_1415_0_c = (
_rtB -> B_1036_1176_0 == _rtB -> B_1036_1413_0_d ) ; _rtB -> B_1036_1416_0_f
= ( _rtB -> B_1036_1414_0 && _rtB -> B_1036_1415_0_c ) ; _rtB ->
B_1036_1417_0 = _rtP -> P_1884 ; _rtB -> B_1036_1418_0_b = ( _rtB ->
B_1036_1177_0_c == _rtB -> B_1036_1417_0 ) ; _rtB -> B_1036_1419_0_l = ( _rtB
-> B_1036_1178_0 == _rtB -> B_1036_1417_0 ) ; _rtB -> B_1036_1420_0_a = (
_rtB -> B_1036_1418_0_b && _rtB -> B_1036_1419_0_l ) ; _rtB ->
B_1036_1421_0_c = ( _rtB -> B_1036_1416_0_f || _rtB -> B_1036_1420_0_a ) ;
_rtB -> B_1036_1422_0 = _rtB -> B_1036_1175_0_k * _rtB -> B_1036_1178_0 ;
_rtB -> B_1036_1423_0 = _rtB -> B_1036_1178_0 - _rtB -> B_1036_1176_0 ; _rtB
-> B_1036_1424_0 = _rtB -> B_1036_1177_0_c * _rtB -> B_1036_1176_0 ; _rtB ->
B_1036_1425_0 = _rtB -> B_1036_1422_0 - _rtB -> B_1036_1424_0 ; _rtB ->
B_1036_1426_0 = _rtB -> B_1036_1425_0 / _rtB -> B_1036_1423_0 ; _rtB ->
B_1036_1427_0_p = _rtP -> P_1885 ; _rtB -> B_1036_1428_0_f = _rtB ->
B_1036_1189_0 - _rtB -> B_1036_1171_0_b ; _rtB -> B_1036_1429_0_g = _rtB ->
B_1036_1170_0 - _rtB -> B_1036_1171_0_b ; _rtB -> B_1036_1430_0_d = _rtB ->
B_1036_1172_0 - _rtB -> B_1036_1192_0_c ; _rtB -> B_1036_1431_0 = _rtB ->
B_1036_1429_0_g / _rtB -> B_1036_1430_0_d ; _rtB -> B_1036_1432_0 = _rtB ->
B_1036_1428_0_f / _rtB -> B_1036_1431_0 ; _rtB -> B_1036_1433_0_h = _rtB ->
B_1036_1432_0 + _rtB -> B_1036_1192_0_c ; _rtB -> B_1036_1434_0_i = _rtP ->
P_1886 ; _rtB -> B_1036_1435_0_m = _rtP -> P_1887 ; _rtB -> B_1036_1436_0_a =
( _rtB -> B_1036_1173_0_p == _rtB -> B_1036_1435_0_m ) ; _rtB ->
B_1036_1437_0_n = ( _rtB -> B_1036_1174_0 == _rtB -> B_1036_1435_0_m ) ; _rtB
-> B_1036_1438_0_o = ( _rtB -> B_1036_1436_0_a && _rtB -> B_1036_1437_0_n ) ;
_rtB -> B_1036_1439_0_a = ( _rtB -> B_1036_1189_0 == _rtB -> B_1036_1434_0_i
) ; _rtB -> B_1036_1440_0_l = ! _rtB -> B_1036_1439_0_a ; _rtB ->
B_1036_1441_0_h = ( _rtB -> B_1036_1438_0_o && _rtB -> B_1036_1440_0_l ) ; if
( _rtB -> B_1036_1441_0_h ) { _rtB -> B_1036_1442_0_c = _rtB ->
B_1036_1433_0_h ; } else { _rtB -> B_1036_1442_0_c = _rtB -> B_1036_1172_0 ;
} _rtB -> B_1036_1443_0_l = _rtP -> P_1888 ; _rtB -> B_1036_1444_0_f = ( _rtB
-> B_1036_1171_0_b == _rtB -> B_1036_1443_0_l ) ; _rtB -> B_1036_1445_0_d = (
_rtB -> B_1036_1192_0_c == _rtB -> B_1036_1443_0_l ) ; _rtB ->
B_1036_1446_0_n = ( _rtB -> B_1036_1444_0_f && _rtB -> B_1036_1445_0_d ) ;
_rtB -> B_1036_1447_0_p = _rtP -> P_1889 ; _rtB -> B_1036_1448_0_g = ( _rtB
-> B_1036_1170_0 == _rtB -> B_1036_1447_0_p ) ; _rtB -> B_1036_1449_0_p = (
_rtB -> B_1036_1172_0 == _rtB -> B_1036_1447_0_p ) ; _rtB -> B_1036_1450_0_p
= ( _rtB -> B_1036_1448_0_g && _rtB -> B_1036_1449_0_p ) ; _rtB ->
B_1036_1451_0_m = ( _rtB -> B_1036_1446_0_n || _rtB -> B_1036_1450_0_p ) ;
_rtB -> B_1036_1452_0_i = _rtB -> B_1036_1171_0_b * _rtB -> B_1036_1172_0 ;
_rtB -> B_1036_1453_0 = _rtB -> B_1036_1172_0 - _rtB -> B_1036_1192_0_c ;
_rtB -> B_1036_1454_0 = _rtB -> B_1036_1170_0 * _rtB -> B_1036_1192_0_c ;
_rtB -> B_1036_1455_0_b = _rtB -> B_1036_1452_0_i - _rtB -> B_1036_1454_0 ;
_rtB -> B_1036_1456_0_g = _rtB -> B_1036_1455_0_b / _rtB -> B_1036_1453_0 ;
_rtB -> B_1036_1457_0_o = _rtB -> B_1036_1189_0 - _rtB -> B_1036_1170_0 ;
_rtB -> B_1036_1458_0_b = _rtB -> B_1036_1173_0_p - _rtB -> B_1036_1170_0 ;
_rtB -> B_1036_1459_0_e = _rtB -> B_1036_1174_0 - _rtB -> B_1036_1172_0 ;
_rtB -> B_1036_1460_0_e = _rtB -> B_1036_1458_0_b / _rtB -> B_1036_1459_0_e ;
_rtB -> B_1036_1461_0_l = _rtB -> B_1036_1457_0_o / _rtB -> B_1036_1460_0_e ;
_rtB -> B_1036_1462_0 = _rtB -> B_1036_1461_0_l + _rtB -> B_1036_1172_0 ;
_rtB -> B_1036_1463_0_c = _rtP -> P_1890 ; _rtB -> B_1036_1464_0_e = _rtP ->
P_1891 ; _rtB -> B_1036_1465_0_p = ( _rtB -> B_1036_1175_0_k == _rtB ->
B_1036_1464_0_e ) ; _rtB -> B_1036_1466_0_d = ( _rtB -> B_1036_1176_0 == _rtB
-> B_1036_1464_0_e ) ; _rtB -> B_1036_1467_0_l = ( _rtB -> B_1036_1465_0_p &&
_rtB -> B_1036_1466_0_d ) ; _rtB -> B_1036_1468_0_b = ( _rtB -> B_1036_1189_0
== _rtB -> B_1036_1463_0_c ) ; _rtB -> B_1036_1469_0_e = ! _rtB ->
B_1036_1468_0_b ; _rtB -> B_1036_1470_0_g = ( _rtB -> B_1036_1467_0_l && _rtB
-> B_1036_1469_0_e ) ; if ( _rtB -> B_1036_1470_0_g ) { _rtB -> B_1036_1471_0
= _rtB -> B_1036_1462_0 ; } else { _rtB -> B_1036_1471_0 = _rtB ->
B_1036_1174_0 ; } _rtB -> B_1036_1472_0 = _rtP -> P_1892 ; _rtB ->
B_1036_1473_0_c = ( _rtB -> B_1036_1170_0 == _rtB -> B_1036_1472_0 ) ; _rtB
-> B_1036_1474_0_o = ( _rtB -> B_1036_1172_0 == _rtB -> B_1036_1472_0 ) ;
_rtB -> B_1036_1475_0_f = ( _rtB -> B_1036_1473_0_c && _rtB ->
B_1036_1474_0_o ) ; _rtB -> B_1036_1476_0_d = _rtP -> P_1893 ; _rtB ->
B_1036_1477_0_o = ( _rtB -> B_1036_1173_0_p == _rtB -> B_1036_1476_0_d ) ;
_rtB -> B_1036_1478_0 = ( _rtB -> B_1036_1174_0 == _rtB -> B_1036_1476_0_d )
; _rtB -> B_1036_1479_0_i = ( _rtB -> B_1036_1477_0_o && _rtB ->
B_1036_1478_0 ) ; _rtB -> B_1036_1480_0 = ( _rtB -> B_1036_1475_0_f || _rtB
-> B_1036_1479_0_i ) ; _rtB -> B_1036_1481_0 = _rtB -> B_1036_1170_0 * _rtB
-> B_1036_1174_0 ; _rtB -> B_1036_1482_0 = _rtB -> B_1036_1174_0 - _rtB ->
B_1036_1172_0 ; _rtB -> B_1036_1483_0 = _rtB -> B_1036_1173_0_p * _rtB ->
B_1036_1172_0 ; _rtB -> B_1036_1484_0 = _rtB -> B_1036_1481_0 - _rtB ->
B_1036_1483_0 ; _rtB -> B_1036_1485_0 = _rtB -> B_1036_1484_0 / _rtB ->
B_1036_1482_0 ; _rtB -> B_1036_1486_0_o = _rtB -> B_1036_1189_0 - _rtB ->
B_1036_1173_0_p ; _rtB -> B_1036_1487_0_l = _rtB -> B_1036_1175_0_k - _rtB ->
B_1036_1173_0_p ; _rtB -> B_1036_1488_0_h = _rtB -> B_1036_1176_0 - _rtB ->
B_1036_1174_0 ; _rtB -> B_1036_1489_0_l = _rtB -> B_1036_1487_0_l / _rtB ->
B_1036_1488_0_h ; _rtB -> B_1036_1490_0 = _rtB -> B_1036_1486_0_o / _rtB ->
B_1036_1489_0_l ; _rtB -> B_1036_1491_0 = _rtB -> B_1036_1490_0 + _rtB ->
B_1036_1174_0 ; _rtB -> B_1036_1492_0_c = _rtP -> P_1894 ; _rtB ->
B_1036_1493_0_c = _rtP -> P_1895 ; _rtB -> B_1036_1494_0_d = ( _rtB ->
B_1036_1177_0_c == _rtB -> B_1036_1493_0_c ) ; _rtB -> B_1036_1495_0_m = (
_rtB -> B_1036_1178_0 == _rtB -> B_1036_1493_0_c ) ; _rtB -> B_1036_1496_0_o
= ( _rtB -> B_1036_1494_0_d && _rtB -> B_1036_1495_0_m ) ; _rtB ->
B_1036_1497_0_o = ( _rtB -> B_1036_1189_0 == _rtB -> B_1036_1492_0_c ) ; _rtB
-> B_1036_1498_0_e = ! _rtB -> B_1036_1497_0_o ; _rtB -> B_1036_1499_0_m = (
_rtB -> B_1036_1496_0_o && _rtB -> B_1036_1498_0_e ) ; if ( _rtB ->
B_1036_1499_0_m ) { _rtB -> B_1036_1500_0_a = _rtB -> B_1036_1491_0 ; } else
{ _rtB -> B_1036_1500_0_a = _rtB -> B_1036_1176_0 ; } _rtB -> B_1036_1501_0_i
= _rtP -> P_1896 ; _rtB -> B_1036_1502_0_i = ( _rtB -> B_1036_1173_0_p ==
_rtB -> B_1036_1501_0_i ) ; _rtB -> B_1036_1503_0_o = ( _rtB -> B_1036_1174_0
== _rtB -> B_1036_1501_0_i ) ; _rtB -> B_1036_1504_0_h = ( _rtB ->
B_1036_1502_0_i && _rtB -> B_1036_1503_0_o ) ; _rtB -> B_1036_1505_0_l = _rtP
-> P_1897 ; _rtB -> B_1036_1506_0_l = ( _rtB -> B_1036_1175_0_k == _rtB ->
B_1036_1505_0_l ) ; _rtB -> B_1036_1507_0_j = ( _rtB -> B_1036_1176_0 == _rtB
-> B_1036_1505_0_l ) ; _rtB -> B_1036_1508_0_n = ( _rtB -> B_1036_1506_0_l &&
_rtB -> B_1036_1507_0_j ) ; _rtB -> B_1036_1509_0_j = ( _rtB ->
B_1036_1504_0_h || _rtB -> B_1036_1508_0_n ) ; _rtB -> B_1036_1510_0_i = _rtB
-> B_1036_1173_0_p * _rtB -> B_1036_1176_0 ; _rtB -> B_1036_1511_0_a = _rtB
-> B_1036_1176_0 - _rtB -> B_1036_1174_0 ; _rtB -> B_1036_1512_0 = _rtB ->
B_1036_1175_0_k * _rtB -> B_1036_1174_0 ; _rtB -> B_1036_1513_0 = _rtB ->
B_1036_1510_0_i - _rtB -> B_1036_1512_0 ; _rtB -> B_1036_1514_0_i = _rtB ->
B_1036_1513_0 / _rtB -> B_1036_1511_0_a ; _rtB -> B_1036_1515_0_c = _rtB ->
B_1036_1189_0 - _rtB -> B_1036_1175_0_k ; _rtB -> B_1036_1516_0_m = _rtB ->
B_1036_1177_0_c - _rtB -> B_1036_1175_0_k ; _rtB -> B_1036_1517_0_n = _rtB ->
B_1036_1178_0 - _rtB -> B_1036_1176_0 ; _rtB -> B_1036_1518_0_c = _rtB ->
B_1036_1516_0_m / _rtB -> B_1036_1517_0_n ; _rtB -> B_1036_1519_0_n = _rtB ->
B_1036_1515_0_c / _rtB -> B_1036_1518_0_c ; _rtB -> B_1036_1520_0_h = _rtB ->
B_1036_1519_0_n + _rtB -> B_1036_1176_0 ; _rtB -> B_1036_1521_0 = _rtP ->
P_1898 ; _rtB -> B_1036_1522_0_i = _rtP -> P_1899 ; _rtB -> B_1036_1523_0_i =
( _rtB -> B_1036_1427_0_p == _rtB -> B_1036_1522_0_i ) ; _rtB ->
B_1036_1524_0_n = ( _rtB -> B_1036_1427_0_p == _rtB -> B_1036_1522_0_i ) ;
_rtB -> B_1036_1525_0_l = ( _rtB -> B_1036_1523_0_i && _rtB ->
B_1036_1524_0_n ) ; _rtB -> B_1036_1526_0_c = ( _rtB -> B_1036_1189_0 == _rtB
-> B_1036_1521_0 ) ; _rtB -> B_1036_1527_0_f = ! _rtB -> B_1036_1526_0_c ;
_rtB -> B_1036_1528_0_a = ( _rtB -> B_1036_1525_0_l && _rtB ->
B_1036_1527_0_f ) ; if ( _rtB -> B_1036_1528_0_a ) { _rtB -> B_1036_1529_0 =
_rtB -> B_1036_1520_0_h ; } else { _rtB -> B_1036_1529_0 = _rtB ->
B_1036_1178_0 ; } _rtB -> B_1036_1530_0 = _rtP -> P_1900 ; _rtB ->
B_1036_1531_0_p = ( _rtB -> B_1036_1175_0_k == _rtB -> B_1036_1530_0 ) ; _rtB
-> B_1036_1532_0_m = ( _rtB -> B_1036_1176_0 == _rtB -> B_1036_1530_0 ) ;
_rtB -> B_1036_1533_0_p = ( _rtB -> B_1036_1531_0_p && _rtB ->
B_1036_1532_0_m ) ; _rtB -> B_1036_1534_0 = _rtP -> P_1901 ; _rtB ->
B_1036_1535_0_m = ( _rtB -> B_1036_1177_0_c == _rtB -> B_1036_1534_0 ) ; _rtB
-> B_1036_1536_0_h = ( _rtB -> B_1036_1178_0 == _rtB -> B_1036_1534_0 ) ;
_rtB -> B_1036_1537_0 = ( _rtB -> B_1036_1535_0_m && _rtB -> B_1036_1536_0_h
) ; _rtB -> B_1036_1538_0_l = ( _rtB -> B_1036_1533_0_p || _rtB ->
B_1036_1537_0 ) ; _rtB -> B_1036_1539_0 = _rtB -> B_1036_1175_0_k * _rtB ->
B_1036_1178_0 ; _rtB -> B_1036_1540_0 = _rtB -> B_1036_1178_0 - _rtB ->
B_1036_1176_0 ; _rtB -> B_1036_1541_0 = _rtB -> B_1036_1177_0_c * _rtB ->
B_1036_1176_0 ; _rtB -> B_1036_1542_0 = _rtB -> B_1036_1539_0 - _rtB ->
B_1036_1541_0 ; _rtB -> B_1036_1543_0 = _rtB -> B_1036_1542_0 / _rtB ->
B_1036_1540_0 ; _rtB -> B_1036_1544_0 = _rtP -> P_1902 ; _rtB ->
B_1036_1545_0_h = ( _rtB -> B_1036_1189_0 == _rtB -> B_1036_1544_0 ) ; _rtB
-> B_1036_1546_0_f = ! _rtB -> B_1036_1545_0_h ; _rtB -> B_1036_1547_0_e =
_rtP -> P_1903 ; _rtB -> B_1036_1548_0_j = ( _rtB -> B_1036_1189_0 == _rtB ->
B_1036_1547_0_e ) ; _rtB -> B_1036_1549_0 = ! _rtB -> B_1036_1548_0_j ; _rtB
-> B_1036_1550_0 = _rtP -> P_1904 ; _rtB -> B_1036_1551_0 = ( _rtB ->
B_1036_1189_0 == _rtB -> B_1036_1550_0 ) ; _rtB -> B_1036_1552_0_h = ! _rtB
-> B_1036_1551_0 ; _rtB -> B_1036_1553_0 = _rtP -> P_1905 ; _rtB ->
B_1036_1554_0 = _rtP -> P_1906 ; _rtB -> B_1036_1555_0 = _rtP -> P_1907 ;
_rtB -> B_1036_1556_0 = _rtP -> P_1908 ; _rtB -> B_1036_1557_0 = _rtP ->
P_1909 ; _rtB -> B_1036_1558_0 = _rtP -> P_1910 ; _rtB -> B_1036_1559_0_p =
_rtP -> P_1911 ; _rtB -> B_1036_1560_0 = _rtP -> P_1912 ; _rtB ->
B_1036_1561_0 = _rtP -> P_1913 ; _rtB -> B_1036_1562_0 = _rtP -> P_1914 ;
_rtB -> B_1036_1563_0 = _rtP -> P_1915 ; _rtB -> B_1036_1564_0 = _rtP ->
P_1916 ; _rtB -> B_1036_1565_0 = _rtP -> P_1917 ; _rtB -> B_1036_1566_0 =
_rtP -> P_1918 ; _rtB -> B_1036_1567_0 = _rtP -> P_1919 ; _rtB ->
B_1036_1568_0 = _rtP -> P_1920 ; _rtB -> B_1036_1569_0 = _rtP -> P_1921 ;
_rtB -> B_1036_1570_0 = _rtP -> P_1922 ; _rtB -> B_1036_1571_0 = _rtP ->
P_1923 ; _rtB -> B_1036_1572_0_k = _rtB -> B_1036_1569_0 - _rtB ->
B_1036_1191_0_n ; _rtB -> B_1036_1573_0 = _rtB -> B_1036_1572_0_k * _rtB ->
B_1036_1571_0 ; _rtB -> B_1036_1574_0 = _rtB -> B_1036_1570_0 / _rtB ->
B_1036_1182_0 ; _rtB -> B_1036_1575_0 = muDoubleScalarSin ( _rtB ->
B_1036_1573_0 ) ; _rtB -> B_1036_1576_0 = muDoubleScalarCos ( _rtB ->
B_1036_1573_0 ) ; _rtB -> B_1036_1577_0 = _rtP -> P_1924 ; _rtB ->
B_1036_1578_0_i = _rtP -> P_1925 ; _rtB -> B_1036_1579_0 = _rtP -> P_1926 ;
_rtB -> B_1036_1580_0_n = _rtB -> B_1036_1187_0 * _rtB -> B_1036_1187_0 ;
_rtB -> B_1036_1581_0 = _rtB -> B_1036_1187_0 * _rtB -> B_1036_1187_0 ; _rtB
-> B_1036_1582_0 = _rtB -> B_1036_1580_0_n + _rtB -> B_1036_1581_0 ; _rtB ->
B_1036_1583_0 = muDoubleScalarSqrt ( _rtB -> B_1036_1582_0 ) ; _rtB ->
B_1036_1584_0 = _rtP -> P_1927 ; _rtB -> B_1036_1585_0_o = _rtB ->
B_1036_1584_0 / _rtB -> B_1036_1186_0 ; _rtB -> B_1036_1586_0_l = 0.0 * _rtB
-> B_1036_1585_0_o ; _rtB -> B_1036_1587_0_p = 0.0 ; _rtB -> B_1036_1588_0_b
= _rtB -> B_1036_1587_0_p ; _rtB -> B_1036_1589_0 = _rtP -> P_1928 ; _rtB ->
B_1036_1590_0 = _rtB -> B_1036_1588_0_b * _rtB -> B_1036_1589_0 ; _rtB ->
B_1036_1591_0 = _rtB -> B_1036_1590_0 * _rtB -> B_1036_1585_0_o ; _rtB ->
B_1036_1592_0 = _rtB -> B_1036_1586_0_l - _rtB -> B_1036_1591_0 ; if ( _rtB
-> B_1036_1185_0 > _rtP -> P_1929 ) { _rtB -> B_1036_1593_0 = _rtB ->
B_1036_1592_0 ; } else { _rtB -> B_1036_1593_0 = _rtB -> B_1036_1586_0_l ; }
_rtB -> B_1036_1594_0 = _rtP -> P_1930 ; _rtB -> B_1036_1595_0 = _rtB ->
B_1036_1594_0 - _rtB -> B_1036_1187_0 ; _rtB -> B_1036_1596_0 = _rtP ->
P_1931 ; _rtB -> B_1036_1597_0_f = _rtB -> B_1036_1595_0 * _rtB ->
B_1036_1596_0 ; _rtB -> B_1036_1598_0_i = muDoubleScalarCos ( _rtB ->
B_1036_1597_0_f ) ; _rtB -> B_1036_1599_0_c = _rtB -> B_1036_1593_0 * _rtB ->
B_1036_1598_0_i ; _rtB -> B_1036_1600_0_g = 0.0 * _rtB -> B_1036_1585_0_o ;
_rtB -> B_1036_1601_0_i = 0.0 ; _rtB -> B_1036_1602_0_b = _rtB ->
B_1036_1601_0_i ; _rtB -> B_1036_1603_0_n = _rtB -> B_1036_1602_0_b * _rtB ->
B_1036_1589_0 ; _rtB -> B_1036_1604_0_p = _rtB -> B_1036_1603_0_n * _rtB ->
B_1036_1585_0_o ; _rtB -> B_1036_1605_0_o = _rtB -> B_1036_1600_0_g - _rtB ->
B_1036_1604_0_p ; if ( _rtB -> B_1036_1185_0 > _rtP -> P_1932 ) { _rtB ->
B_1036_1606_0_b = _rtB -> B_1036_1605_0_o ; } else { _rtB -> B_1036_1606_0_b
= _rtB -> B_1036_1600_0_g ; } _rtB -> B_1036_1607_0_o = muDoubleScalarSin (
_rtB -> B_1036_1597_0_f ) ; _rtB -> B_1036_1608_0_c = _rtB -> B_1036_1606_0_b
* _rtB -> B_1036_1607_0_o ; _rtB -> B_1036_1609_0_j = _rtB -> B_1036_1599_0_c
- _rtB -> B_1036_1608_0_c ; _rtB -> B_1036_1610_0_e = _rtP -> P_1933 ; if (
0.0 > _rtP -> P_1934 ) { _rtB -> B_1036_1611_0_p = _rtB -> B_1036_1609_0_j ;
} else { _rtB -> B_1036_1611_0_p = _rtB -> B_1036_1610_0_e ; } _rtB ->
B_1036_1612_0_n = _rtP -> P_1935 ; if ( _rtB -> B_1036_1612_0_n > _rtP ->
P_1936 ) { _rtB -> B_1036_1613_0_o = _rtB -> B_1036_1611_0_p ; } else { _rtB
-> B_1036_1613_0_o = _rtB -> B_1036_1610_0_e ; } _rtB -> B_1036_1614_0_d =
_rtB -> B_1036_1613_0_o * _rtB -> B_1036_1613_0_o ; _rtB -> B_1036_1615_0_g =
_rtB -> B_1036_1606_0_b * _rtB -> B_1036_1598_0_i ; _rtB -> B_1036_1616_0_i =
_rtB -> B_1036_1593_0 * _rtB -> B_1036_1607_0_o ; _rtB -> B_1036_1617_0_i =
_rtB -> B_1036_1615_0_g + _rtB -> B_1036_1616_0_i ; if ( 0.0 > _rtP -> P_1937
) { _rtB -> B_1036_1618_0_c = _rtB -> B_1036_1617_0_i ; } else { _rtB ->
B_1036_1618_0_c = _rtB -> B_1036_1610_0_e ; } if ( _rtB -> B_1036_1612_0_n >
_rtP -> P_1938 ) { _rtB -> B_1036_1619_0_b = _rtB -> B_1036_1618_0_c ; } else
{ _rtB -> B_1036_1619_0_b = _rtB -> B_1036_1610_0_e ; } _rtB ->
B_1036_1620_0_g = _rtB -> B_1036_1619_0_b * _rtB -> B_1036_1619_0_b ; _rtB ->
B_1036_1621_0_d = _rtB -> B_1036_1614_0_d + _rtB -> B_1036_1620_0_g ; _rtB ->
B_1036_1622_0_f = muDoubleScalarSqrt ( _rtB -> B_1036_1621_0_d ) ; _rtB ->
B_1036_1623_0_h = ( _rtB -> B_1036_1583_0 > _rtB -> B_1036_1622_0_f ) ; if (
_rtB -> B_1036_1623_0_h ) { _rtB -> B_1036_1624_0_j = _rtB -> B_1036_1187_0 ;
_rtB -> B_1036_1626_0_p = _rtB -> B_1036_1187_0 ; } else { _rtB ->
B_1036_1624_0_j = _rtB -> B_1036_1613_0_o ; _rtB -> B_1036_1626_0_p = _rtB ->
B_1036_1619_0_b ; } _rtB -> B_1036_1625_0_e = _rtB -> B_1036_1624_0_j * _rtB
-> B_1036_1624_0_j ; _rtB -> B_1036_1627_0_d = _rtB -> B_1036_1626_0_p * _rtB
-> B_1036_1626_0_p ; _rtB -> B_1036_1628_0_f = _rtB -> B_1036_1625_0_e + _rtB
-> B_1036_1627_0_d ; _rtB -> B_1036_1629_0_n = muDoubleScalarSqrt ( _rtB ->
B_1036_1628_0_f ) ; _rtB -> B_1036_1630_0_o = _rtP -> P_1939 ; _rtB ->
B_1036_1631_0_d = _rtB -> B_1036_1569_0 - _rtB -> B_1036_1191_0_n ; _rtB ->
B_1036_1632_0_l = _rtB -> B_1036_1631_0_d * _rtB -> B_1036_1630_0_o ; _rtB ->
B_1036_1633_0_f = _rtB -> B_1036_1570_0 / _rtB -> B_1036_1182_0 ; _rtB ->
B_1036_1634_0_j = muDoubleScalarSin ( _rtB -> B_1036_1632_0_l ) ; _rtB ->
B_1036_1635_0_g = muDoubleScalarCos ( _rtB -> B_1036_1632_0_l ) ; _rtB ->
B_1036_1636_0_l = _rtP -> P_1940 ; _rtB -> B_1036_1637_0_k = _rtP -> P_1941 ;
_rtB -> B_1036_1638_0_p = _rtP -> P_1942 ; _rtB -> B_1036_1639_0_b = _rtB ->
B_1036_1187_0 * _rtB -> B_1036_1187_0 ; _rtB -> B_1036_1640_0_i = _rtB ->
B_1036_1187_0 * _rtB -> B_1036_1187_0 ; _rtB -> B_1036_1641_0_p = _rtB ->
B_1036_1639_0_b + _rtB -> B_1036_1640_0_i ; _rtB -> B_1036_1642_0_l =
muDoubleScalarSqrt ( _rtB -> B_1036_1641_0_p ) ; _rtB -> B_1036_1643_0_e =
_rtB -> B_1036_1584_0 / _rtB -> B_1036_1186_0 ; _rtB -> B_1036_1644_0_e = 0.0
* _rtB -> B_1036_1643_0_e ; _rtB -> B_1036_1645_0_p = _rtB -> B_1036_1590_0 *
_rtB -> B_1036_1643_0_e ; _rtB -> B_1036_1646_0_j = _rtB -> B_1036_1644_0_e -
_rtB -> B_1036_1645_0_p ; if ( _rtB -> B_1036_1185_0 > _rtP -> P_1943 ) {
_rtB -> B_1036_1647_0_g = _rtB -> B_1036_1646_0_j ; } else { _rtB ->
B_1036_1647_0_g = _rtB -> B_1036_1644_0_e ; } _rtB -> B_1036_1648_0_l = _rtB
-> B_1036_1594_0 - _rtB -> B_1036_1187_0 ; _rtB -> B_1036_1649_0 = _rtP ->
P_1944 ; _rtB -> B_1036_1650_0 = _rtB -> B_1036_1648_0_l * _rtB ->
B_1036_1649_0 ; _rtB -> B_1036_1651_0_j = muDoubleScalarCos ( _rtB ->
B_1036_1650_0 ) ; _rtB -> B_1036_1652_0_p = _rtB -> B_1036_1647_0_g * _rtB ->
B_1036_1651_0_j ; _rtB -> B_1036_1653_0_d = 0.0 * _rtB -> B_1036_1643_0_e ;
_rtB -> B_1036_1654_0_k = _rtB -> B_1036_1603_0_n * _rtB -> B_1036_1643_0_e ;
_rtB -> B_1036_1655_0_m = _rtB -> B_1036_1653_0_d - _rtB -> B_1036_1654_0_k ;
if ( _rtB -> B_1036_1185_0 > _rtP -> P_1945 ) { _rtB -> B_1036_1656_0_f =
_rtB -> B_1036_1655_0_m ; } else { _rtB -> B_1036_1656_0_f = _rtB ->
B_1036_1653_0_d ; } _rtB -> B_1036_1657_0_m = muDoubleScalarSin ( _rtB ->
B_1036_1650_0 ) ; _rtB -> B_1036_1658_0_n = _rtB -> B_1036_1656_0_f * _rtB ->
B_1036_1657_0_m ; _rtB -> B_1036_1659_0_m = _rtB -> B_1036_1652_0_p - _rtB ->
B_1036_1658_0_n ; _rtB -> B_1036_1660_0_b = _rtP -> P_1946 ; if ( 0.0 > _rtP
-> P_1947 ) { _rtB -> B_1036_1661_0_c = _rtB -> B_1036_1659_0_m ; } else {
_rtB -> B_1036_1661_0_c = _rtB -> B_1036_1660_0_b ; } if ( _rtB ->
B_1036_1612_0_n > _rtP -> P_1948 ) { _rtB -> B_1036_1662_0_g = _rtB ->
B_1036_1661_0_c ; } else { _rtB -> B_1036_1662_0_g = _rtB -> B_1036_1660_0_b
; } _rtB -> B_1036_1663_0_c = _rtB -> B_1036_1662_0_g * _rtB ->
B_1036_1662_0_g ; _rtB -> B_1036_1664_0_i = _rtB -> B_1036_1656_0_f * _rtB ->
B_1036_1651_0_j ; _rtB -> B_1036_1665_0_k = _rtB -> B_1036_1647_0_g * _rtB ->
B_1036_1657_0_m ; _rtB -> B_1036_1666_0_g = _rtB -> B_1036_1664_0_i + _rtB ->
B_1036_1665_0_k ; if ( 0.0 > _rtP -> P_1949 ) { _rtB -> B_1036_1667_0_p =
_rtB -> B_1036_1666_0_g ; } else { _rtB -> B_1036_1667_0_p = _rtB ->
B_1036_1660_0_b ; } if ( _rtB -> B_1036_1612_0_n > _rtP -> P_1950 ) { _rtB ->
B_1036_1668_0_f = _rtB -> B_1036_1667_0_p ; } else { _rtB -> B_1036_1668_0_f
= _rtB -> B_1036_1660_0_b ; } _rtB -> B_1036_1669_0_m = _rtB ->
B_1036_1668_0_f * _rtB -> B_1036_1668_0_f ; _rtB -> B_1036_1670_0_m = _rtB ->
B_1036_1663_0_c + _rtB -> B_1036_1669_0_m ; _rtB -> B_1036_1671_0_l =
muDoubleScalarSqrt ( _rtB -> B_1036_1670_0_m ) ; _rtB -> B_1036_1672_0_a = (
_rtB -> B_1036_1642_0_l > _rtB -> B_1036_1671_0_l ) ; if ( _rtB ->
B_1036_1672_0_a ) { _rtB -> B_1036_1673_0_j = _rtB -> B_1036_1187_0 ; _rtB ->
B_1036_1675_0_j = _rtB -> B_1036_1187_0 ; } else { _rtB -> B_1036_1673_0_j =
_rtB -> B_1036_1662_0_g ; _rtB -> B_1036_1675_0_j = _rtB -> B_1036_1668_0_f ;
} _rtB -> B_1036_1674_0_m = _rtB -> B_1036_1673_0_j * _rtB -> B_1036_1673_0_j
; _rtB -> B_1036_1676_0_i = _rtB -> B_1036_1675_0_j * _rtB -> B_1036_1675_0_j
; _rtB -> B_1036_1677_0_a = _rtB -> B_1036_1674_0_m + _rtB -> B_1036_1676_0_i
; _rtB -> B_1036_1678_0_h = muDoubleScalarSqrt ( _rtB -> B_1036_1677_0_a ) ;
_rtB -> B_1036_1679_0_d = _rtP -> P_1951 ; _rtB -> B_1036_1680_0_i = _rtB ->
B_1036_1569_0 - _rtB -> B_1036_1191_0_n ; _rtB -> B_1036_1681_0_h = _rtB ->
B_1036_1680_0_i * _rtB -> B_1036_1679_0_d ; _rtB -> B_1036_1682_0_d = _rtB ->
B_1036_1570_0 / _rtB -> B_1036_1182_0 ; _rtB -> B_1036_1683_0_l =
muDoubleScalarSin ( _rtB -> B_1036_1681_0_h ) ; _rtB -> B_1036_1684_0 =
muDoubleScalarCos ( _rtB -> B_1036_1681_0_h ) ; _rtB -> B_1036_1685_0 = _rtP
-> P_1952 ; _rtB -> B_1036_1686_0_f = _rtP -> P_1953 ; _rtB ->
B_1036_1687_0_l = _rtP -> P_1954 ; _rtB -> B_1036_1688_0_l = _rtB ->
B_1036_1187_0 * _rtB -> B_1036_1187_0 ; _rtB -> B_1036_1689_0_f = _rtB ->
B_1036_1187_0 * _rtB -> B_1036_1187_0 ; _rtB -> B_1036_1690_0_g = _rtB ->
B_1036_1688_0_l + _rtB -> B_1036_1689_0_f ; _rtB -> B_1036_1691_0_g =
muDoubleScalarSqrt ( _rtB -> B_1036_1690_0_g ) ; _rtB -> B_1036_1692_0_a =
_rtB -> B_1036_1584_0 / _rtB -> B_1036_1186_0 ; _rtB -> B_1036_1693_0_d = 0.0
* _rtB -> B_1036_1692_0_a ; _rtB -> B_1036_1694_0_b = _rtB -> B_1036_1590_0 *
_rtB -> B_1036_1692_0_a ; _rtB -> B_1036_1695_0_d = _rtB -> B_1036_1693_0_d -
_rtB -> B_1036_1694_0_b ; if ( _rtB -> B_1036_1185_0 > _rtP -> P_1955 ) {
_rtB -> B_1036_1696_0_f = _rtB -> B_1036_1695_0_d ; } else { _rtB ->
B_1036_1696_0_f = _rtB -> B_1036_1693_0_d ; } _rtB -> B_1036_1697_0_a = _rtB
-> B_1036_1594_0 - _rtB -> B_1036_1187_0 ; _rtB -> B_1036_1698_0_m = _rtP ->
P_1956 ; _rtB -> B_1036_1699_0_i = _rtB -> B_1036_1697_0_a * _rtB ->
B_1036_1698_0_m ; _rtB -> B_1036_1700_0_i = muDoubleScalarCos ( _rtB ->
B_1036_1699_0_i ) ; _rtB -> B_1036_1701_0_k = _rtB -> B_1036_1696_0_f * _rtB
-> B_1036_1700_0_i ; _rtB -> B_1036_1702_0_a = 0.0 * _rtB -> B_1036_1692_0_a
; _rtB -> B_1036_1703_0_m = _rtB -> B_1036_1603_0_n * _rtB -> B_1036_1692_0_a
; _rtB -> B_1036_1704_0_c = _rtB -> B_1036_1702_0_a - _rtB -> B_1036_1703_0_m
; if ( _rtB -> B_1036_1185_0 > _rtP -> P_1957 ) { _rtB -> B_1036_1705_0_b =
_rtB -> B_1036_1704_0_c ; } else { _rtB -> B_1036_1705_0_b = _rtB ->
B_1036_1702_0_a ; } _rtB -> B_1036_1706_0_k = muDoubleScalarSin ( _rtB ->
B_1036_1699_0_i ) ; _rtB -> B_1036_1707_0_l = _rtB -> B_1036_1705_0_b * _rtB
-> B_1036_1706_0_k ; _rtB -> B_1036_1708_0_n = _rtB -> B_1036_1701_0_k - _rtB
-> B_1036_1707_0_l ; _rtB -> B_1036_1709_0_p = _rtP -> P_1958 ; if ( 0.0 >
_rtP -> P_1959 ) { _rtB -> B_1036_1710_0_f = _rtB -> B_1036_1708_0_n ; } else
{ _rtB -> B_1036_1710_0_f = _rtB -> B_1036_1709_0_p ; } if ( _rtB ->
B_1036_1612_0_n > _rtP -> P_1960 ) { _rtB -> B_1036_1711_0_k = _rtB ->
B_1036_1710_0_f ; } else { _rtB -> B_1036_1711_0_k = _rtB -> B_1036_1709_0_p
; } _rtB -> B_1036_1712_0_e = _rtB -> B_1036_1711_0_k * _rtB ->
B_1036_1711_0_k ; _rtB -> B_1036_1713_0 = _rtB -> B_1036_1705_0_b * _rtB ->
B_1036_1700_0_i ; _rtB -> B_1036_1714_0 = _rtB -> B_1036_1696_0_f * _rtB ->
B_1036_1706_0_k ; _rtB -> B_1036_1715_0_n = _rtB -> B_1036_1713_0 + _rtB ->
B_1036_1714_0 ; if ( 0.0 > _rtP -> P_1961 ) { _rtB -> B_1036_1716_0_o = _rtB
-> B_1036_1715_0_n ; } else { _rtB -> B_1036_1716_0_o = _rtB ->
B_1036_1709_0_p ; } if ( _rtB -> B_1036_1612_0_n > _rtP -> P_1962 ) { _rtB ->
B_1036_1717_0_a = _rtB -> B_1036_1716_0_o ; } else { _rtB -> B_1036_1717_0_a
= _rtB -> B_1036_1709_0_p ; } _rtB -> B_1036_1718_0_b = _rtB ->
B_1036_1717_0_a * _rtB -> B_1036_1717_0_a ; _rtB -> B_1036_1719_0_b = _rtB ->
B_1036_1712_0_e + _rtB -> B_1036_1718_0_b ; _rtB -> B_1036_1720_0_m =
muDoubleScalarSqrt ( _rtB -> B_1036_1719_0_b ) ; _rtB -> B_1036_1721_0_o = (
_rtB -> B_1036_1691_0_g > _rtB -> B_1036_1720_0_m ) ; if ( _rtB ->
B_1036_1721_0_o ) { _rtB -> B_1036_1722_0_m = _rtB -> B_1036_1187_0 ; _rtB ->
B_1036_1724_0_m = _rtB -> B_1036_1187_0 ; } else { _rtB -> B_1036_1722_0_m =
_rtB -> B_1036_1711_0_k ; _rtB -> B_1036_1724_0_m = _rtB -> B_1036_1717_0_a ;
} _rtB -> B_1036_1723_0_k = _rtB -> B_1036_1722_0_m * _rtB -> B_1036_1722_0_m
; _rtB -> B_1036_1725_0_h = _rtB -> B_1036_1724_0_m * _rtB -> B_1036_1724_0_m
; _rtB -> B_1036_1726_0_i = _rtB -> B_1036_1723_0_k + _rtB -> B_1036_1725_0_h
; _rtB -> B_1036_1727_0_c = muDoubleScalarSqrt ( _rtB -> B_1036_1726_0_i ) ;
_rtB -> B_1036_1728_0_n = _rtP -> P_1963 ; _rtB -> B_1036_1729_0_i = _rtB ->
B_1036_1567_0 + _rtB -> B_1036_1728_0_n ; _rtB -> B_1036_1730_0_o = _rtP ->
P_1964 ; _rtB -> B_1036_1731_0_h = _rtB -> B_1036_1567_0 + _rtB ->
B_1036_1730_0_o ; _rtB -> B_1036_1732_0_p = _rtP -> P_1965 ; _rtB ->
B_1036_1733_0_g = _rtP -> P_1966 ; _rtB -> B_1036_1734_0_c = _rtP -> P_1967 ;
_rtB -> B_1036_1735_0_g = _rtP -> P_1968 ; _rtB -> B_1036_1736_0_b = _rtP ->
P_1969 ; _rtB -> B_1036_1737_0_j = _rtP -> P_1970 ; _rtB -> B_1036_1738_0_h =
_rtP -> P_1971 ; _rtB -> B_1036_1739_0_d = _rtP -> P_1972 ; _rtB ->
B_1036_1740_0_o = _rtP -> P_1973 ; _rtB -> B_1036_1741_0_k = _rtB ->
B_1036_1738_0_h - _rtB -> B_1036_1183_0 ; _rtB -> B_1036_1742_0_j = _rtB ->
B_1036_1741_0_k * _rtB -> B_1036_1740_0_o ; _rtB -> B_1036_1743_0_d = _rtB ->
B_1036_1739_0_d / _rtB -> B_1036_1169_0 ; _rtB -> B_1036_1744_0_f =
muDoubleScalarSin ( _rtB -> B_1036_1742_0_j ) ; _rtB -> B_1036_1745_0_a =
muDoubleScalarCos ( _rtB -> B_1036_1742_0_j ) ; _rtB -> B_1036_1746_0 = _rtP
-> P_1974 ; _rtB -> B_1036_1747_0 = _rtP -> P_1975 ; _rtB -> B_1036_1748_0_e
= _rtP -> P_1976 ; _rtB -> B_1036_1749_0_n = _rtP -> P_1977 ; _rtB ->
B_1036_1750_0_o = _rtB -> B_1036_1738_0_h - _rtB -> B_1036_1183_0 ; _rtB ->
B_1036_1751_0_d = _rtB -> B_1036_1750_0_o * _rtB -> B_1036_1749_0_n ; _rtB ->
B_1036_1752_0_c = _rtB -> B_1036_1739_0_d / _rtB -> B_1036_1169_0 ; _rtB ->
B_1036_1753_0_m = muDoubleScalarSin ( _rtB -> B_1036_1751_0_d ) ; _rtB ->
B_1036_1754_0_o = muDoubleScalarCos ( _rtB -> B_1036_1751_0_d ) ; _rtB ->
B_1036_1755_0_b = _rtP -> P_1978 ; _rtB -> B_1036_1756_0_m = _rtP -> P_1979 ;
_rtB -> B_1036_1757_0_i = _rtP -> P_1980 ; _rtB -> B_1036_1758_0_i = _rtP ->
P_1981 ; _rtB -> B_1036_1759_0_g = _rtB -> B_1036_1738_0_h - _rtB ->
B_1036_1183_0 ; _rtB -> B_1036_1760_0_m = _rtB -> B_1036_1759_0_g * _rtB ->
B_1036_1758_0_i ; _rtB -> B_1036_1761_0_n = _rtB -> B_1036_1739_0_d / _rtB ->
B_1036_1169_0 ; _rtB -> B_1036_1762_0_c = muDoubleScalarSin ( _rtB ->
B_1036_1760_0_m ) ; _rtB -> B_1036_1763_0_j = muDoubleScalarCos ( _rtB ->
B_1036_1760_0_m ) ; _rtB -> B_1036_1764_0_e = _rtP -> P_1982 ; _rtB ->
B_1036_1765_0_e = _rtP -> P_1983 ; _rtB -> B_1036_1766_0_k = _rtP -> P_1984 ;
_rtB -> B_1036_1767_0_k = _rtP -> P_1985 ; _rtB -> B_1036_1768_0_i = _rtB ->
B_1036_1736_0_b + _rtB -> B_1036_1767_0_k ; _rtB -> B_1036_1769_0_e = _rtP ->
P_1986 ; _rtB -> B_1036_1770_0_l = _rtB -> B_1036_1736_0_b + _rtB ->
B_1036_1769_0_e ; _rtB -> B_1036_1771_0_l = _rtP -> P_1987 ; _rtB ->
B_1036_1772_0_l = _rtP -> P_1988 ; _rtB -> B_1036_1773_0_o = _rtP -> P_1989 ;
_rtB -> B_1036_1774_0_i = _rtP -> P_1990 ; _rtB -> B_1036_1775_0_e = _rtB ->
B_1036_1609_0_j * _rtB -> B_1036_1609_0_j ; _rtB -> B_1036_1776_0_l = _rtB ->
B_1036_1617_0_i * _rtB -> B_1036_1617_0_i ; _rtB -> B_1036_1777_0 = _rtB ->
B_1036_1775_0_e + _rtB -> B_1036_1776_0_l ; _rtB -> B_1036_1778_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_1777_0 ) ; _rtB -> B_1036_1779_0_n = _rtB
-> B_1036_1659_0_m * _rtB -> B_1036_1659_0_m ; _rtB -> B_1036_1780_0_e = _rtB
-> B_1036_1666_0_g * _rtB -> B_1036_1666_0_g ; _rtB -> B_1036_1781_0_c = _rtB
-> B_1036_1779_0_n + _rtB -> B_1036_1780_0_e ; _rtB -> B_1036_1782_0_a =
muDoubleScalarSqrt ( _rtB -> B_1036_1781_0_c ) ; _rtB -> B_1036_1783_0_m =
_rtB -> B_1036_1708_0_n * _rtB -> B_1036_1708_0_n ; _rtB -> B_1036_1784_0_c =
_rtB -> B_1036_1715_0_n * _rtB -> B_1036_1715_0_n ; _rtB -> B_1036_1785_0_f =
_rtB -> B_1036_1783_0_m + _rtB -> B_1036_1784_0_c ; _rtB -> B_1036_1786_0_m =
muDoubleScalarSqrt ( _rtB -> B_1036_1785_0_f ) ; ssCallAccelRunBlock ( S ,
1036 , 7727 , SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_1036_1788_0_k = _rtP ->
P_1991 ; _rtB -> B_1036_1789_0_b = _rtB -> B_1036_1609_0_j * _rtB ->
B_1036_1609_0_j ; _rtB -> B_1036_1790_0_h = _rtB -> B_1036_1617_0_i * _rtB ->
B_1036_1617_0_i ; _rtB -> B_1036_1791_0_h = _rtB -> B_1036_1789_0_b + _rtB ->
B_1036_1790_0_h ; _rtB -> B_1036_1792_0_h = muDoubleScalarSqrt ( _rtB ->
B_1036_1791_0_h ) ; _rtB -> B_1036_1793_0_b = _rtB -> B_1036_1609_0_j / _rtB
-> B_1036_1792_0_h ; _rtB -> B_1036_1794_0_b = _rtP -> P_1992 ; minV = _rtB
-> B_1036_1793_0_b ; if ( minV > 1.0 ) { minV = 1.0 ; } else { if ( minV < -
1.0 ) { minV = - 1.0 ; } } _rtB -> B_1036_1795_0_d = muDoubleScalarAcos (
minV ) ; _rtB -> B_1036_1796_0_j = _rtP -> P_1993 ; _rtB -> B_1036_1797_0_n =
( _rtB -> B_1036_1617_0_i > _rtB -> B_1036_1796_0_j ) ; _rtB ->
B_1036_1798_0_d = _rtB -> B_1036_1795_0_d * _rtB -> B_1036_1794_0_b ; if (
_rtB -> B_1036_1797_0_n ) { _rtB -> B_1036_1799_0_l = _rtB -> B_1036_1795_0_d
; } else { _rtB -> B_1036_1799_0_l = _rtB -> B_1036_1798_0_d ; } _rtB ->
B_1036_1800_0_j = _rtB -> B_1036_1187_0 + _rtB -> B_1036_1619_0_b ; _rtB ->
B_1036_1801_0_b = _rtB -> B_1036_1187_0 + _rtB -> B_1036_1613_0_o ; _rtB ->
B_1036_1802_0_f = _rtB -> B_1036_1659_0_m * _rtB -> B_1036_1659_0_m ; _rtB ->
B_1036_1803_0_p = _rtB -> B_1036_1666_0_g * _rtB -> B_1036_1666_0_g ; _rtB ->
B_1036_1804_0_l = _rtB -> B_1036_1802_0_f + _rtB -> B_1036_1803_0_p ; _rtB ->
B_1036_1805_0_l = muDoubleScalarSqrt ( _rtB -> B_1036_1804_0_l ) ; _rtB ->
B_1036_1806_0 = _rtB -> B_1036_1659_0_m / _rtB -> B_1036_1805_0_l ; _rtB ->
B_1036_1807_0 = _rtP -> P_1994 ; minV = _rtB -> B_1036_1806_0 ; if ( minV >
1.0 ) { minV = 1.0 ; } else { if ( minV < - 1.0 ) { minV = - 1.0 ; } } _rtB
-> B_1036_1808_0_i = muDoubleScalarAcos ( minV ) ; _rtB -> B_1036_1809_0_o =
_rtP -> P_1995 ; _rtB -> B_1036_1810_0_k = ( _rtB -> B_1036_1666_0_g > _rtB
-> B_1036_1809_0_o ) ; _rtB -> B_1036_1811_0_b = _rtB -> B_1036_1808_0_i *
_rtB -> B_1036_1807_0 ; if ( _rtB -> B_1036_1810_0_k ) { _rtB ->
B_1036_1812_0_i = _rtB -> B_1036_1808_0_i ; } else { _rtB -> B_1036_1812_0_i
= _rtB -> B_1036_1811_0_b ; } ssCallAccelRunBlock ( S , 1036 , 7753 ,
SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_1036_1814_0_g = _rtB -> B_1036_1187_0 +
_rtB -> B_1036_1668_0_f ; _rtB -> B_1036_1815_0 = _rtB -> B_1036_1187_0 +
_rtB -> B_1036_1662_0_g ; _rtB -> B_1036_1816_0 = _rtB -> B_1036_1708_0_n *
_rtB -> B_1036_1708_0_n ; _rtB -> B_1036_1817_0 = _rtB -> B_1036_1715_0_n *
_rtB -> B_1036_1715_0_n ; _rtB -> B_1036_1818_0 = _rtB -> B_1036_1816_0 +
_rtB -> B_1036_1817_0 ; _rtB -> B_1036_1819_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_1818_0 ) ; _rtB -> B_1036_1820_0 = _rtB -> B_1036_1708_0_n / _rtB ->
B_1036_1819_0 ; _rtB -> B_1036_1821_0 = _rtP -> P_1996 ; minV = _rtB ->
B_1036_1820_0 ; if ( minV > 1.0 ) { minV = 1.0 ; } else { if ( minV < - 1.0 )
{ minV = - 1.0 ; } } _rtB -> B_1036_1822_0 = muDoubleScalarAcos ( minV ) ;
_rtB -> B_1036_1823_0 = _rtP -> P_1997 ; _rtB -> B_1036_1824_0_n = ( _rtB ->
B_1036_1715_0_n > _rtB -> B_1036_1823_0 ) ; _rtB -> B_1036_1825_0_p = _rtB ->
B_1036_1822_0 * _rtB -> B_1036_1821_0 ; if ( _rtB -> B_1036_1824_0_n ) { _rtB
-> B_1036_1826_0_h = _rtB -> B_1036_1822_0 ; } else { _rtB -> B_1036_1826_0_h
= _rtB -> B_1036_1825_0_p ; } _rtB -> B_1036_1827_0_d = _rtB -> B_1036_1187_0
+ _rtB -> B_1036_1717_0_a ; _rtB -> B_1036_1828_0_f = _rtB -> B_1036_1187_0 +
_rtB -> B_1036_1711_0_k ; _rtB -> B_1036_1829_0_e = true ; _rtB ->
B_1036_1830_0_n = ( _rtB -> B_1036_1829_0_e && ( _rtB -> B_1036_1612_0_n !=
0.0 ) ) ; _rtB -> B_1036_1831_0_b = true ; _rtB -> B_1036_1832_0_a = ( _rtB
-> B_1036_1831_0_b && ( _rtB -> B_1036_1612_0_n != 0.0 ) ) ; _rtB ->
B_1036_1833_0_j = true ; _rtB -> B_1036_1834_0_j = ( _rtB -> B_1036_1833_0_j
&& ( _rtB -> B_1036_1612_0_n != 0.0 ) ) ; _rtB -> B_1036_1835_0_l = ( ( _rtB
-> B_1036_1187_0 != 0.0 ) || _rtB -> B_1036_1834_0_j ) ; _rtB ->
B_1036_1836_0_f = ( ( _rtB -> B_1036_1187_0 != 0.0 ) || _rtB ->
B_1036_1830_0_n ) ; _rtB -> B_1036_1837_0_i = ( ( _rtB -> B_1036_1187_0 !=
0.0 ) || _rtB -> B_1036_1832_0_a ) ; _rtB -> B_1036_1838_0_b = ( ( _rtB ->
B_1036_1187_0 != 0.0 ) && _rtB -> B_1036_1834_0_j && _rtB -> B_1036_1830_0_n
&& _rtB -> B_1036_1832_0_a ) ; _rtB -> B_1036_1839_0_f = _rtP -> P_1998 ;
_rtB -> B_1036_1840_0_c = _rtB -> B_1036_1788_0_k + _rtB -> B_1036_1839_0_f ;
_rtB -> B_1036_1841_0_d = _rtP -> P_1999 ; _rtB -> B_1036_1842_0_c = _rtB ->
B_1036_1788_0_k + _rtB -> B_1036_1841_0_d ; _rtB -> B_1036_1843_0 = _rtP ->
P_2000 ; _rtB -> B_1036_1844_0_b = 0.0 * _rtB -> B_1036_1843_0 ; _rtB ->
B_1036_1845_0_p = _rtP -> P_2001 ; _rtB -> B_1036_1846_0_h = 0.0 * _rtB ->
B_1036_1845_0_p ; _rtB -> B_1036_1847_0_e = _rtB -> B_1036_1844_0_b + _rtB ->
B_1036_1846_0_h ; _rtB -> B_1036_1848_0_l = 0.0 * _rtB -> B_1036_1843_0 ;
_rtB -> B_1036_1849_0_h = _rtP -> P_2002 ; _rtB -> B_1036_1850_0_n = 0.0 *
_rtB -> B_1036_1849_0_h ; _rtB -> B_1036_1851_0_k = _rtB -> B_1036_1848_0_l +
_rtB -> B_1036_1850_0_n ; _rtB -> B_1036_1852_0_b = _rtB -> B_1036_1847_0_e +
_rtB -> B_1036_1851_0_k ; _rtB -> B_1036_1853_0_o = _rtB -> B_1036_1852_0_b ;
_rtB -> B_1036_1854_0_n = 0.0 * _rtB -> B_1036_1843_0 ; _rtB ->
B_1036_1855_0_f = 0.0 * _rtB -> B_1036_1849_0_h ; _rtB -> B_1036_1856_0_g =
_rtB -> B_1036_1854_0_n + _rtB -> B_1036_1855_0_f ; _rtB -> B_1036_1857_0_d =
0.0 * _rtB -> B_1036_1843_0 ; _rtB -> B_1036_1858_0_d = 0.0 * _rtB ->
B_1036_1845_0_p ; _rtB -> B_1036_1859_0_e = _rtB -> B_1036_1857_0_d + _rtB ->
B_1036_1858_0_d ; _rtB -> B_1036_1860_0 = _rtB -> B_1036_1856_0_g + _rtB ->
B_1036_1859_0_e ; _rtB -> B_1036_1861_0_c = _rtB -> B_1036_1860_0 ; _rtB ->
B_1036_1862_0_a = 0.0 * _rtB -> B_1036_1843_0 ; _rtB -> B_1036_1863_0_h = 0.0
* _rtB -> B_1036_1849_0_h ; _rtB -> B_1036_1864_0_e = _rtB -> B_1036_1862_0_a
- _rtB -> B_1036_1863_0_h ; _rtB -> B_1036_1865_0_n = 0.0 * _rtB ->
B_1036_1843_0 ; _rtB -> B_1036_1866_0_h = 0.0 * _rtB -> B_1036_1845_0_p ;
_rtB -> B_1036_1867_0 = _rtB -> B_1036_1865_0_n - _rtB -> B_1036_1866_0_h ;
_rtB -> B_1036_1868_0_o = _rtB -> B_1036_1864_0_e + _rtB -> B_1036_1867_0 ;
_rtB -> B_1036_1869_0_f = _rtB -> B_1036_1868_0_o ; _rtB -> B_1036_1870_0_k =
_rtB -> B_1036_1869_0_f * _rtB -> B_1036_1589_0 ; _rtB -> B_1036_1871_0_e =
0.0 * _rtB -> B_1036_1843_0 ; _rtB -> B_1036_1872_0_i = 0.0 * _rtB ->
B_1036_1845_0_p ; _rtB -> B_1036_1873_0_o = _rtB -> B_1036_1871_0_e - _rtB ->
B_1036_1872_0_i ; _rtB -> B_1036_1874_0_j = 0.0 * _rtB -> B_1036_1843_0 ;
_rtB -> B_1036_1875_0_k = 0.0 * _rtB -> B_1036_1849_0_h ; _rtB ->
B_1036_1876_0_j = _rtB -> B_1036_1874_0_j - _rtB -> B_1036_1875_0_k ; _rtB ->
B_1036_1877_0_d = _rtB -> B_1036_1873_0_o + _rtB -> B_1036_1876_0_j ; _rtB ->
B_1036_1878_0_g = _rtB -> B_1036_1877_0_d ; _rtB -> B_1036_1879_0_c = _rtB ->
B_1036_1878_0_g * _rtB -> B_1036_1589_0 ; _rtB -> B_1036_1880_0_k = _rtB ->
B_1036_1853_0_o * _rtB -> B_1036_1589_0 ; _rtB -> B_1036_1881_0_j = _rtB ->
B_1036_1861_0_c * _rtB -> B_1036_1589_0 ; _rtB -> B_1036_1882_0_h = _rtP ->
P_2003 ; _rtB -> B_1036_1883_0_c = ( _rtB -> B_1036_1188_0 == _rtB ->
B_1036_1882_0_h ) ; _rtB -> B_1036_1884_0_e = ( _rtB -> B_1036_1188_0 == _rtB
-> B_1036_1882_0_h ) ; _rtB -> B_1036_1885_0_d = ( _rtB -> B_1036_1883_0_c &&
_rtB -> B_1036_1884_0_e ) ; _rtB -> B_1036_1886_0_n = true ; _rtB ->
B_1036_1887_0_c = _rtP -> P_2004 ; _rtB -> B_1036_1888_0_d = _rtP -> P_2005 ;
_rtB -> B_1036_1889_0_a = _rtP -> P_2006 ; _rtB -> B_1036_1890_0_l = ( _rtB
-> B_1036_1889_0_a == _rtB -> B_1036_1887_0_c ) ; _rtB -> B_1036_1891_0_o = !
_rtB -> B_1036_1890_0_l ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_c2 , &
_rtDW -> OFFDelay_c2 , & _rtP -> OFFDelay_c2 ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_os , & _rtDW -> ONDelay_os , & _rtP -> ONDelay_os ) ; _rtB ->
B_1036_1894_0_p = _rtP -> P_2007 ; _rtB -> B_1036_1895_0_f = _rtP -> P_2008 ;
_rtB -> B_1036_1896_0_h = _rtP -> P_2009 ; _rtB -> B_1036_1897_0_a = ( _rtB
-> B_1036_1896_0_h == _rtB -> B_1036_1894_0_p ) ; _rtB -> B_1036_1898_0_j = !
_rtB -> B_1036_1897_0_a ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_l4 , &
_rtDW -> OFFDelay_l4 , & _rtP -> OFFDelay_l4 ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_h2 , & _rtDW -> ONDelay_h2 , & _rtP -> ONDelay_h2 ) ; _rtB ->
B_1036_1901_0_j = _rtP -> P_2010 ; _rtB -> B_1036_1902_0_n = _rtP -> P_2011 ;
_rtB -> B_1036_1903_0_o = _rtP -> P_2012 ; _rtB -> B_1036_1904_0_b = ( _rtB
-> B_1036_1903_0_o == _rtB -> B_1036_1901_0_j ) ; _rtB -> B_1036_1905_0_l = !
_rtB -> B_1036_1904_0_b ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_gk , &
_rtDW -> OFFDelay_gk , & _rtP -> OFFDelay_gk ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_k1 , & _rtDW -> ONDelay_k1 , & _rtP -> ONDelay_k1 ) ; _rtB ->
B_1036_1908_0_o = _rtP -> P_2013 ; _rtB -> B_1036_1909_0_d = _rtP -> P_2014 ;
_rtB -> B_1036_1910_0_a = _rtP -> P_2015 ; _rtB -> B_1036_1911_0_h = ( _rtB
-> B_1036_1910_0_a == _rtB -> B_1036_1908_0_o ) ; _rtB -> B_1036_1912_0_h = !
_rtB -> B_1036_1911_0_h ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_ht , &
_rtDW -> OFFDelay_ht , & _rtP -> OFFDelay_ht ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_d , & _rtDW -> ONDelay_d , & _rtP -> ONDelay_d ) ; _rtB ->
B_1036_1915_0_k = _rtP -> P_2016 ; _rtB -> B_1036_1916_0_c = _rtP -> P_2017 ;
_rtB -> B_1036_1917_0_j = _rtP -> P_2018 ; _rtB -> B_1036_1918_0_f = ( _rtB
-> B_1036_1917_0_j == _rtB -> B_1036_1915_0_k ) ; _rtB -> B_1036_1919_0_k = !
_rtB -> B_1036_1918_0_f ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_hp , &
_rtDW -> OFFDelay_hp , & _rtP -> OFFDelay_hp ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_n0 , & _rtDW -> ONDelay_n0 , & _rtP -> ONDelay_n0 ) ; _rtB ->
B_1036_1922_0_b = _rtP -> P_2019 ; _rtB -> B_1036_1923_0 = _rtP -> P_2020 ;
_rtB -> B_1036_1924_0_n = _rtP -> P_2021 ; _rtB -> B_1036_1925_0_f = ( _rtB
-> B_1036_1924_0_n == _rtB -> B_1036_1922_0_b ) ; _rtB -> B_1036_1926_0_i = !
_rtB -> B_1036_1925_0_f ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_kf , &
_rtDW -> OFFDelay_kf , & _rtP -> OFFDelay_kf ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_fna , & _rtDW -> ONDelay_fna , & _rtP -> ONDelay_fna ) ; _rtB
-> B_1036_1929_0_d = _rtP -> P_2022 ; _rtB -> B_1036_1930_0 = _rtP -> P_2023
; _rtB -> B_1036_1931_0_h = _rtP -> P_2024 ; _rtB -> B_1036_1932_0_p = ( _rtB
-> B_1036_1931_0_h == _rtB -> B_1036_1929_0_d ) ; _rtB -> B_1036_1933_0_d = !
_rtB -> B_1036_1932_0_p ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_ec , &
_rtDW -> OFFDelay_ec , & _rtP -> OFFDelay_ec ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_mt , & _rtDW -> ONDelay_mt , & _rtP -> ONDelay_mt ) ; _rtB ->
B_1036_1936_0_b = _rtP -> P_2025 ; _rtB -> B_1036_1937_0_p = _rtP -> P_2026 ;
_rtB -> B_1036_1938_0_b = _rtP -> P_2027 ; _rtB -> B_1036_1939_0_o = ( _rtB
-> B_1036_1938_0_b == _rtB -> B_1036_1936_0_b ) ; _rtB -> B_1036_1940_0_g = !
_rtB -> B_1036_1939_0_o ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_os , &
_rtDW -> OFFDelay_os , & _rtP -> OFFDelay_os ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_ixo , & _rtDW -> ONDelay_ixo , & _rtP -> ONDelay_ixo ) ; _rtB
-> B_1036_1943_0_p = _rtP -> P_2028 ; _rtB -> B_1036_1944_0_n = _rtP ->
P_2029 ; _rtB -> B_1036_1945_0_j = _rtP -> P_2030 ; _rtB -> B_1036_1946_0_n =
( _rtB -> B_1036_1945_0_j == _rtB -> B_1036_1943_0_p ) ; _rtB ->
B_1036_1947_0_g = ! _rtB -> B_1036_1946_0_n ; DZG_v3_OFFDelayTID2 ( S , &
_rtB -> OFFDelay_dh , & _rtDW -> OFFDelay_dh , & _rtP -> OFFDelay_dh ) ;
DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_cs , & _rtDW -> ONDelay_cs , &
_rtP -> ONDelay_cs ) ; _rtB -> B_1036_1950_0_b = _rtP -> P_2031 ; _rtB ->
B_1036_1951_0_n = _rtP -> P_2032 ; _rtB -> B_1036_1952_0_b [ 0 ] = _rtP ->
P_2033 [ 0 ] ; _rtB -> B_1036_1953_0_j [ 0 ] = _rtP -> P_2034 [ 0 ] ; _rtB ->
B_1036_1954_0_k [ 0 ] = _rtP -> P_2035 [ 0 ] ; _rtB -> B_1036_1952_0_b [ 1 ]
= _rtP -> P_2033 [ 1 ] ; _rtB -> B_1036_1953_0_j [ 1 ] = _rtP -> P_2034 [ 1 ]
; _rtB -> B_1036_1954_0_k [ 1 ] = _rtP -> P_2035 [ 1 ] ; switch ( ( int32_T )
_rtB -> B_1036_1951_0_n ) { case 1 : _rtB -> B_1036_1955_0_d [ 0 ] = _rtB ->
B_1036_1952_0_b [ 0 ] ; _rtB -> B_1036_1955_0_d [ 1 ] = _rtB ->
B_1036_1952_0_b [ 1 ] ; break ; case 2 : _rtB -> B_1036_1955_0_d [ 0 ] = _rtB
-> B_1036_1953_0_j [ 0 ] ; _rtB -> B_1036_1955_0_d [ 1 ] = _rtB ->
B_1036_1953_0_j [ 1 ] ; break ; default : _rtB -> B_1036_1955_0_d [ 0 ] =
_rtB -> B_1036_1954_0_k [ 0 ] ; _rtB -> B_1036_1955_0_d [ 1 ] = _rtB ->
B_1036_1954_0_k [ 1 ] ; break ; } _rtB -> B_1036_1956_0 = _rtP -> P_2036 ;
_rtB -> B_1036_1957_0_d = _rtP -> P_2037 ; _rtB -> B_1036_1958_0_o [ 0 ] =
_rtP -> P_2038 [ 0 ] ; _rtB -> B_1036_1959_0_c [ 0 ] = _rtP -> P_2039 [ 0 ] ;
_rtB -> B_1036_1960_0_o [ 0 ] = _rtP -> P_2040 [ 0 ] ; _rtB ->
B_1036_1958_0_o [ 1 ] = _rtP -> P_2038 [ 1 ] ; _rtB -> B_1036_1959_0_c [ 1 ]
= _rtP -> P_2039 [ 1 ] ; _rtB -> B_1036_1960_0_o [ 1 ] = _rtP -> P_2040 [ 1 ]
; switch ( ( int32_T ) _rtB -> B_1036_1957_0_d ) { case 1 : _rtB ->
B_1036_1961_0_a [ 0 ] = _rtB -> B_1036_1958_0_o [ 0 ] ; _rtB ->
B_1036_1961_0_a [ 1 ] = _rtB -> B_1036_1958_0_o [ 1 ] ; break ; case 2 : _rtB
-> B_1036_1961_0_a [ 0 ] = _rtB -> B_1036_1959_0_c [ 0 ] ; _rtB ->
B_1036_1961_0_a [ 1 ] = _rtB -> B_1036_1959_0_c [ 1 ] ; break ; default :
_rtB -> B_1036_1961_0_a [ 0 ] = _rtB -> B_1036_1960_0_o [ 0 ] ; _rtB ->
B_1036_1961_0_a [ 1 ] = _rtB -> B_1036_1960_0_o [ 1 ] ; break ; } _rtB ->
B_1036_1962_0_d = _rtP -> P_2041 ; _rtB -> B_1036_1963_0_e = _rtP -> P_2042 ;
_rtB -> B_1036_1964_0_i [ 0 ] = _rtP -> P_2043 [ 0 ] ; _rtB ->
B_1036_1965_0_b [ 0 ] = _rtP -> P_2044 [ 0 ] ; _rtB -> B_1036_1966_0_a [ 0 ]
= _rtP -> P_2045 [ 0 ] ; _rtB -> B_1036_1964_0_i [ 1 ] = _rtP -> P_2043 [ 1 ]
; _rtB -> B_1036_1965_0_b [ 1 ] = _rtP -> P_2044 [ 1 ] ; _rtB ->
B_1036_1966_0_a [ 1 ] = _rtP -> P_2045 [ 1 ] ; switch ( ( int32_T ) _rtB ->
B_1036_1963_0_e ) { case 1 : _rtB -> B_1036_1967_0_k [ 0 ] = _rtB ->
B_1036_1964_0_i [ 0 ] ; _rtB -> B_1036_1967_0_k [ 1 ] = _rtB ->
B_1036_1964_0_i [ 1 ] ; break ; case 2 : _rtB -> B_1036_1967_0_k [ 0 ] = _rtB
-> B_1036_1965_0_b [ 0 ] ; _rtB -> B_1036_1967_0_k [ 1 ] = _rtB ->
B_1036_1965_0_b [ 1 ] ; break ; default : _rtB -> B_1036_1967_0_k [ 0 ] =
_rtB -> B_1036_1966_0_a [ 0 ] ; _rtB -> B_1036_1967_0_k [ 1 ] = _rtB ->
B_1036_1966_0_a [ 1 ] ; break ; } _rtB -> B_1036_1968_0_j = false ; _rtB ->
B_1036_1969_0 = _rtP -> P_2046 ; _rtB -> B_1036_1970_0_m = _rtP -> P_2047 ;
_rtB -> B_1036_1971_0_j = _rtP -> P_2048 ; _rtB -> B_1036_1972_0_j = _rtP ->
P_2049 ; _rtB -> B_1036_1973_0_k = _rtP -> P_2050 ; _rtB -> B_1036_1974_0_k =
_rtP -> P_2051 ; _rtB -> B_1036_1975_0_a = _rtP -> P_2052 ; _rtB ->
B_1036_1976_0_k = _rtP -> P_2053 ; _rtB -> B_1036_1977_0_f = _rtP -> P_2054 ;
_rtB -> B_1036_1978_0_c = _rtP -> P_2055 ; _rtB -> B_1036_1979_0_o = _rtP ->
P_2056 ; _rtB -> B_1036_1980_0_k = _rtP -> P_2057 ; _rtB -> B_1036_1981_0_p =
_rtP -> P_2058 ; _rtB -> B_1036_1982_0_f = _rtP -> P_2059 ; _rtB ->
B_1036_1983_0_d = _rtP -> P_2060 ; _rtB -> B_1036_1984_0_j = ( _rtB ->
B_1036_1983_0_d == _rtB -> B_1036_1981_0_p ) ; if ( _rtB -> B_1036_1984_0_j )
{ _rtB -> B_1036_1985_0_e = _rtB -> B_1036_1982_0_f ; } else { _rtB ->
B_1036_1985_0_e = _rtB -> B_1036_1983_0_d ; } _rtB -> B_1036_1986_0 = _rtP ->
P_2061 ; _rtB -> B_1036_1987_0_l = _rtP -> P_2062 ; _rtB -> B_1036_1988_0_e =
_rtP -> P_2063 ; _rtB -> B_1036_1989_0_g = ( _rtB -> B_1036_1988_0_e == _rtB
-> B_1036_1972_0_j ) ; _rtB -> B_1036_1990_0_j = ( _rtB -> B_1036_1988_0_e ==
_rtB -> B_1036_1973_0_k ) ; _rtB -> B_1036_1991_0_c = ( _rtB ->
B_1036_1988_0_e == _rtB -> B_1036_1974_0_k ) ; _rtB -> B_1036_1992_0_n = (
_rtB -> B_1036_1988_0_e == _rtB -> B_1036_1975_0_a ) ; _rtB ->
B_1036_1993_0_d = ( _rtB -> B_1036_1988_0_e == _rtB -> B_1036_1970_0_m ) ;
_rtB -> B_1036_1994_0_k = ( _rtB -> B_1036_1988_0_e == _rtB ->
B_1036_1976_0_k ) ; _rtB -> B_1036_1995_0_i = ( _rtB -> B_1036_1988_0_e ==
_rtB -> B_1036_1971_0_j ) ; _rtB -> B_1036_1996_0_d = ( _rtB ->
B_1036_1988_0_e == _rtB -> B_1036_1978_0_c ) ; _rtB -> B_1036_1997_0_h = _rtP
-> P_2064 ; _rtB -> B_1036_1998_0_g = _rtP -> P_2065 ; _rtB ->
B_1036_1999_0_p = _rtP -> P_2066 ; if ( _rtB -> B_1036_1989_0_g ) { _rtB ->
B_1036_2000_0_n = _rtB -> B_1036_1998_0_g ; _rtB -> B_1036_2001_0_o = _rtB ->
B_1036_1999_0_p ; _rtB -> B_1036_2002_0_l = _rtB -> B_1036_1997_0_h ; } else
{ _rtB -> B_1036_2000_0_n = _rtB -> B_1036_1977_0_f ; _rtB -> B_1036_2001_0_o
= _rtB -> B_1036_1977_0_f ; _rtB -> B_1036_2002_0_l = _rtB -> B_1036_1977_0_f
; } _rtB -> B_1036_2003_0_f = _rtP -> P_2067 ; _rtB -> B_1036_2004_0_h = _rtP
-> P_2068 ; _rtB -> B_1036_2005_0_d = _rtP -> P_2069 ; if ( _rtB ->
B_1036_1990_0_j ) { _rtB -> B_1036_2006_0_m = _rtB -> B_1036_2004_0_h ; _rtB
-> B_1036_2007_0_i = _rtB -> B_1036_2005_0_d ; _rtB -> B_1036_2008_0_j = _rtB
-> B_1036_2003_0_f ; } else { _rtB -> B_1036_2006_0_m = _rtB ->
B_1036_2000_0_n ; _rtB -> B_1036_2007_0_i = _rtB -> B_1036_2001_0_o ; _rtB ->
B_1036_2008_0_j = _rtB -> B_1036_2002_0_l ; } _rtB -> B_1036_2009_0_p = _rtP
-> P_2070 ; _rtB -> B_1036_2010_0 = _rtP -> P_2071 ; _rtB -> B_1036_2011_0 =
_rtP -> P_2072 ; if ( _rtB -> B_1036_1991_0_c ) { _rtB -> B_1036_2012_0 =
_rtB -> B_1036_2010_0 ; _rtB -> B_1036_2013_0 = _rtB -> B_1036_2011_0 ; _rtB
-> B_1036_2014_0 = _rtB -> B_1036_2009_0_p ; } else { _rtB -> B_1036_2012_0 =
_rtB -> B_1036_2006_0_m ; _rtB -> B_1036_2013_0 = _rtB -> B_1036_2007_0_i ;
_rtB -> B_1036_2014_0 = _rtB -> B_1036_2008_0_j ; } _rtB -> B_1036_2015_0 =
_rtP -> P_2073 ; _rtB -> B_1036_2016_0 = _rtP -> P_2074 ; _rtB ->
B_1036_2017_0_d = _rtP -> P_2075 ; if ( _rtB -> B_1036_1992_0_n ) { _rtB ->
B_1036_2018_0_j = _rtB -> B_1036_2016_0 ; _rtB -> B_1036_2019_0_k = _rtB ->
B_1036_2017_0_d ; _rtB -> B_1036_2020_0_m = _rtB -> B_1036_2015_0 ; } else {
_rtB -> B_1036_2018_0_j = _rtB -> B_1036_2012_0 ; _rtB -> B_1036_2019_0_k =
_rtB -> B_1036_2013_0 ; _rtB -> B_1036_2020_0_m = _rtB -> B_1036_2014_0 ; }
_rtB -> B_1036_2021_0 = _rtP -> P_2076 ; _rtB -> B_1036_2022_0 = _rtP ->
P_2077 ; _rtB -> B_1036_2023_0 = _rtP -> P_2078 ; if ( _rtB ->
B_1036_1994_0_k ) { _rtB -> B_1036_2024_0 = _rtB -> B_1036_2022_0 ; _rtB ->
B_1036_2025_0_d = _rtB -> B_1036_2023_0 ; _rtB -> B_1036_2026_0_p = _rtB ->
B_1036_2021_0 ; } else { _rtB -> B_1036_2024_0 = _rtB -> B_1036_2018_0_j ;
_rtB -> B_1036_2025_0_d = _rtB -> B_1036_2019_0_k ; _rtB -> B_1036_2026_0_p =
_rtB -> B_1036_2020_0_m ; } _rtB -> B_1036_2027_0_i = _rtP -> P_2079 ; _rtB
-> B_1036_2028_0_g = _rtP -> P_2080 ; _rtB -> B_1036_2029_0 = _rtP -> P_2081
; if ( _rtB -> B_1036_1993_0_d ) { _rtB -> B_1036_2030_0 = _rtB ->
B_1036_2028_0_g ; _rtB -> B_1036_2031_0 = _rtB -> B_1036_2029_0 ; _rtB ->
B_1036_2032_0_e = _rtB -> B_1036_2027_0_i ; } else { _rtB -> B_1036_2030_0 =
_rtB -> B_1036_2024_0 ; _rtB -> B_1036_2031_0 = _rtB -> B_1036_2025_0_d ;
_rtB -> B_1036_2032_0_e = _rtB -> B_1036_2026_0_p ; } _rtB -> B_1036_2033_0_h
= _rtP -> P_2082 ; _rtB -> B_1036_2034_0_g = _rtP -> P_2083 ; _rtB ->
B_1036_2035_0_c = _rtP -> P_2084 ; if ( _rtB -> B_1036_1995_0_i ) { _rtB ->
B_1036_2036_0_a = _rtB -> B_1036_2034_0_g ; _rtB -> B_1036_2037_0_f = _rtB ->
B_1036_2035_0_c ; _rtB -> B_1036_2038_0_i = _rtB -> B_1036_2033_0_h ; } else
{ _rtB -> B_1036_2036_0_a = _rtB -> B_1036_2030_0 ; _rtB -> B_1036_2037_0_f =
_rtB -> B_1036_2031_0 ; _rtB -> B_1036_2038_0_i = _rtB -> B_1036_2032_0_e ; }
_rtB -> B_1036_2039_0_c = _rtP -> P_2085 ; _rtB -> B_1036_2041_0_c = _rtP ->
P_2086 ; if ( _rtB -> B_1036_1996_0_d ) { _rtB -> B_1036_2040_0_n = _rtB ->
B_1036_2039_0_c ; } else { _rtB -> B_1036_2040_0_n = _rtB -> B_1036_2036_0_a
; } _rtB -> B_1036_2043_0_i = _rtP -> P_2087 ; if ( _rtB -> B_1036_1996_0_d )
{ _rtB -> B_1036_2042_0_c = _rtB -> B_1036_2041_0_c ; _rtB -> B_1036_2044_0 =
_rtB -> B_1036_2043_0_i ; } else { _rtB -> B_1036_2042_0_c = _rtB ->
B_1036_2037_0_f ; _rtB -> B_1036_2044_0 = _rtB -> B_1036_2038_0_i ; } _rtB ->
B_1036_2045_0 = _rtP -> P_2088 ; _rtB -> B_1036_2046_0 = _rtP -> P_2089 ;
_rtB -> B_1036_2047_0_h = _rtP -> P_2090 ; _rtB -> B_1036_2048_0_m = _rtP ->
P_2091 ; _rtB -> B_1036_2049_0_g = _rtP -> P_2092 ; _rtB -> B_1036_2050_0_o =
_rtP -> P_2093 ; _rtB -> B_1036_2051_0_m = _rtP -> P_2094 ; _rtB ->
B_1036_2052_0_e = _rtP -> P_2095 ; _rtB -> B_1036_2053_0_e = _rtP -> P_2096 ;
_rtB -> B_1036_2054_0_i = _rtP -> P_2097 ; _rtB -> B_1036_2055_0_f = _rtP ->
P_2098 ; _rtB -> B_1036_2056_0_h = _rtP -> P_2099 ; _rtB -> B_1036_2057_0_j =
( _rtB -> B_1036_1983_0_d == _rtB -> B_1036_2055_0_f ) ; if ( _rtB ->
B_1036_2057_0_j ) { _rtB -> B_1036_2058_0_i = _rtB -> B_1036_2056_0_h ; }
else { _rtB -> B_1036_2058_0_i = _rtB -> B_1036_1983_0_d ; } _rtB ->
B_1036_2059_0_p = _rtP -> P_2100 ; _rtB -> B_1036_2060_0_a = _rtP -> P_2101 ;
_rtB -> B_1036_2061_0_f = _rtP -> P_2102 ; _rtB -> B_1036_2062_0_g = ( _rtB
-> B_1036_2061_0_f == _rtB -> B_1036_2047_0_h ) ; _rtB -> B_1036_2063_0_g = (
_rtB -> B_1036_2061_0_f == _rtB -> B_1036_2048_0_m ) ; _rtB ->
B_1036_2064_0_j = ( _rtB -> B_1036_2061_0_f == _rtB -> B_1036_2049_0_g ) ;
_rtB -> B_1036_2065_0_h = ( _rtB -> B_1036_2061_0_f == _rtB ->
B_1036_2050_0_o ) ; _rtB -> B_1036_2066_0_o = ( _rtB -> B_1036_2061_0_f ==
_rtB -> B_1036_2045_0 ) ; _rtB -> B_1036_2067_0_o = ( _rtB -> B_1036_2061_0_f
== _rtB -> B_1036_2051_0_m ) ; _rtB -> B_1036_2068_0_k = ( _rtB ->
B_1036_2061_0_f == _rtB -> B_1036_2046_0 ) ; _rtB -> B_1036_2069_0_h = ( _rtB
-> B_1036_2061_0_f == _rtB -> B_1036_2053_0_e ) ; _rtB -> B_1036_2070_0_d =
_rtP -> P_2103 ; _rtB -> B_1036_2071_0_c = _rtP -> P_2104 ; _rtB ->
B_1036_2072_0_j = _rtP -> P_2105 ; if ( _rtB -> B_1036_2062_0_g ) { _rtB ->
B_1036_2073_0_c = _rtB -> B_1036_2071_0_c ; _rtB -> B_1036_2074_0_n = _rtB ->
B_1036_2072_0_j ; _rtB -> B_1036_2075_0_j = _rtB -> B_1036_2070_0_d ; } else
{ _rtB -> B_1036_2073_0_c = _rtB -> B_1036_2052_0_e ; _rtB -> B_1036_2074_0_n
= _rtB -> B_1036_2052_0_e ; _rtB -> B_1036_2075_0_j = _rtB -> B_1036_2052_0_e
; } _rtB -> B_1036_2076_0_k = _rtP -> P_2106 ; _rtB -> B_1036_2077_0_o = _rtP
-> P_2107 ; _rtB -> B_1036_2078_0_g = _rtP -> P_2108 ; if ( _rtB ->
B_1036_2063_0_g ) { _rtB -> B_1036_2079_0_k = _rtB -> B_1036_2077_0_o ; _rtB
-> B_1036_2080_0_j = _rtB -> B_1036_2078_0_g ; _rtB -> B_1036_2081_0 = _rtB
-> B_1036_2076_0_k ; } else { _rtB -> B_1036_2079_0_k = _rtB ->
B_1036_2073_0_c ; _rtB -> B_1036_2080_0_j = _rtB -> B_1036_2074_0_n ; _rtB ->
B_1036_2081_0 = _rtB -> B_1036_2075_0_j ; } _rtB -> B_1036_2082_0_p = _rtP ->
P_2109 ; _rtB -> B_1036_2083_0_h = _rtP -> P_2110 ; _rtB -> B_1036_2084_0_n =
_rtP -> P_2111 ; if ( _rtB -> B_1036_2064_0_j ) { _rtB -> B_1036_2085_0_b =
_rtB -> B_1036_2083_0_h ; _rtB -> B_1036_2086_0_m = _rtB -> B_1036_2084_0_n ;
_rtB -> B_1036_2087_0_k = _rtB -> B_1036_2082_0_p ; } else { _rtB ->
B_1036_2085_0_b = _rtB -> B_1036_2079_0_k ; _rtB -> B_1036_2086_0_m = _rtB ->
B_1036_2080_0_j ; _rtB -> B_1036_2087_0_k = _rtB -> B_1036_2081_0 ; } _rtB ->
B_1036_2088_0_b = _rtP -> P_2112 ; _rtB -> B_1036_2089_0 = _rtP -> P_2113 ;
_rtB -> B_1036_2090_0_l = _rtP -> P_2114 ; if ( _rtB -> B_1036_2065_0_h ) {
_rtB -> B_1036_2091_0 = _rtB -> B_1036_2089_0 ; _rtB -> B_1036_2092_0 = _rtB
-> B_1036_2090_0_l ; _rtB -> B_1036_2093_0 = _rtB -> B_1036_2088_0_b ; } else
{ _rtB -> B_1036_2091_0 = _rtB -> B_1036_2085_0_b ; _rtB -> B_1036_2092_0 =
_rtB -> B_1036_2086_0_m ; _rtB -> B_1036_2093_0 = _rtB -> B_1036_2087_0_k ; }
_rtB -> B_1036_2094_0_g = _rtP -> P_2115 ; _rtB -> B_1036_2095_0 = _rtP ->
P_2116 ; _rtB -> B_1036_2096_0 = _rtP -> P_2117 ; if ( _rtB ->
B_1036_2067_0_o ) { _rtB -> B_1036_2097_0_g = _rtB -> B_1036_2095_0 ; _rtB ->
B_1036_2098_0_n = _rtB -> B_1036_2096_0 ; _rtB -> B_1036_2099_0_c = _rtB ->
B_1036_2094_0_g ; } else { _rtB -> B_1036_2097_0_g = _rtB -> B_1036_2091_0 ;
_rtB -> B_1036_2098_0_n = _rtB -> B_1036_2092_0 ; _rtB -> B_1036_2099_0_c =
_rtB -> B_1036_2093_0 ; } _rtB -> B_1036_2100_0_e = _rtP -> P_2118 ; _rtB ->
B_1036_2101_0 = _rtP -> P_2119 ; _rtB -> B_1036_2102_0_f = _rtP -> P_2120 ;
if ( _rtB -> B_1036_2066_0_o ) { _rtB -> B_1036_2103_0_k = _rtB ->
B_1036_2101_0 ; _rtB -> B_1036_2104_0_c = _rtB -> B_1036_2102_0_f ; _rtB ->
B_1036_2105_0_d = _rtB -> B_1036_2100_0_e ; } else { _rtB -> B_1036_2103_0_k
= _rtB -> B_1036_2097_0_g ; _rtB -> B_1036_2104_0_c = _rtB -> B_1036_2098_0_n
; _rtB -> B_1036_2105_0_d = _rtB -> B_1036_2099_0_c ; } _rtB ->
B_1036_2106_0_m = _rtP -> P_2121 ; _rtB -> B_1036_2107_0_j = _rtP -> P_2122 ;
_rtB -> B_1036_2108_0_c = _rtP -> P_2123 ; if ( _rtB -> B_1036_2068_0_k ) {
_rtB -> B_1036_2109_0 = _rtB -> B_1036_2107_0_j ; _rtB -> B_1036_2110_0_d =
_rtB -> B_1036_2108_0_c ; _rtB -> B_1036_2111_0 = _rtB -> B_1036_2106_0_m ; }
else { _rtB -> B_1036_2109_0 = _rtB -> B_1036_2103_0_k ; _rtB ->
B_1036_2110_0_d = _rtB -> B_1036_2104_0_c ; _rtB -> B_1036_2111_0 = _rtB ->
B_1036_2105_0_d ; } _rtB -> B_1036_2112_0 = _rtP -> P_2124 ; _rtB ->
B_1036_2114_0_p = _rtP -> P_2125 ; if ( _rtB -> B_1036_2069_0_h ) { _rtB ->
B_1036_2113_0 = _rtB -> B_1036_2112_0 ; } else { _rtB -> B_1036_2113_0 = _rtB
-> B_1036_2109_0 ; } _rtB -> B_1036_2116_0 = _rtP -> P_2126 ; if ( _rtB ->
B_1036_2069_0_h ) { _rtB -> B_1036_2115_0 = _rtB -> B_1036_2114_0_p ; _rtB ->
B_1036_2117_0_a = _rtB -> B_1036_2116_0 ; } else { _rtB -> B_1036_2115_0 =
_rtB -> B_1036_2110_0_d ; _rtB -> B_1036_2117_0_a = _rtB -> B_1036_2111_0 ; }
_rtB -> B_1036_2118_0_m = _rtP -> P_2127 ; _rtB -> B_1036_2119_0_n = _rtP ->
P_2128 ; _rtB -> B_1036_2120_0_k = _rtP -> P_2129 ; _rtB -> B_1036_2121_0 =
_rtP -> P_2130 ; _rtB -> B_1036_2122_0_p = _rtP -> P_2131 ; _rtB ->
B_1036_2123_0_k = _rtP -> P_2132 ; _rtB -> B_1036_2124_0_g = _rtP -> P_2133 ;
_rtB -> B_1036_2125_0_h = _rtP -> P_2134 ; _rtB -> B_1036_2126_0_l = _rtP ->
P_2135 ; _rtB -> B_1036_2127_0_o = _rtP -> P_2136 ; _rtB -> B_1036_2128_0_a =
_rtP -> P_2137 ; _rtB -> B_1036_2129_0 = _rtP -> P_2138 ; _rtB ->
B_1036_2130_0_n = _rtP -> P_2139 ; _rtB -> B_1036_2131_0 = _rtP -> P_2140 ;
_rtB -> B_1036_2132_0 = _rtP -> P_2141 ; _rtB -> B_1036_2133_0 = _rtP ->
P_2142 ; _rtB -> B_1036_2134_0_o = _rtP -> P_2143 ; _rtB -> B_1036_2135_0 =
_rtP -> P_2144 ; _rtB -> B_1036_2136_0 = _rtP -> P_2145 ; _rtB ->
B_1036_2137_0_d = _rtP -> P_2146 ; _rtB -> B_1036_2138_0_h = _rtP -> P_2147 ;
_rtB -> B_1036_2139_0_h = _rtP -> P_2148 ; _rtB -> B_1036_2140_0_k = ( _rtB
-> B_1036_1983_0_d == _rtB -> B_1036_2138_0_h ) ; if ( _rtB ->
B_1036_2140_0_k ) { _rtB -> B_1036_2141_0 = _rtB -> B_1036_2139_0_h ; } else
{ _rtB -> B_1036_2141_0 = _rtB -> B_1036_1983_0_d ; } _rtB -> B_1036_2142_0_a
= _rtP -> P_2149 ; _rtB -> B_1036_2143_0_k = _rtP -> P_2150 ; _rtB ->
B_1036_2144_0_c = ( _rtB -> B_1036_2118_0_m == _rtB -> B_1036_2130_0_n ) ;
_rtB -> B_1036_2145_0_a = ( _rtB -> B_1036_2118_0_m == _rtB -> B_1036_2131_0
) ; _rtB -> B_1036_2146_0_j = ( _rtB -> B_1036_2118_0_m == _rtB ->
B_1036_2132_0 ) ; _rtB -> B_1036_2147_0_a = ( _rtB -> B_1036_2118_0_m == _rtB
-> B_1036_2133_0 ) ; _rtB -> B_1036_2148_0_l = ( _rtB -> B_1036_2118_0_m ==
_rtB -> B_1036_2134_0_o ) ; _rtB -> B_1036_2149_0 = _rtP -> P_2151 ; _rtB ->
B_1036_2150_0_j = _rtP -> P_2152 ; if ( _rtB -> B_1036_2144_0_c ) { _rtB ->
B_1036_2151_0 = _rtB -> B_1036_2149_0 ; _rtB -> B_1036_2152_0 = _rtB ->
B_1036_2150_0_j ; } else { _rtB -> B_1036_2151_0 = _rtB -> B_1036_2135_0 ;
_rtB -> B_1036_2152_0 = _rtB -> B_1036_2135_0 ; } _rtB -> B_1036_2153_0 =
_rtP -> P_2153 ; _rtB -> B_1036_2154_0_d = _rtP -> P_2154 ; if ( _rtB ->
B_1036_2145_0_a ) { _rtB -> B_1036_2155_0 = _rtB -> B_1036_2153_0 ; _rtB ->
B_1036_2156_0 = _rtB -> B_1036_2154_0_d ; } else { _rtB -> B_1036_2155_0 =
_rtB -> B_1036_2151_0 ; _rtB -> B_1036_2156_0 = _rtB -> B_1036_2152_0 ; }
_rtB -> B_1036_2157_0_p = _rtP -> P_2155 ; _rtB -> B_1036_2158_0_d = _rtP ->
P_2156 ; if ( _rtB -> B_1036_2146_0_j ) { _rtB -> B_1036_2159_0_b = _rtB ->
B_1036_2157_0_p ; _rtB -> B_1036_2160_0_o = _rtB -> B_1036_2158_0_d ; } else
{ _rtB -> B_1036_2159_0_b = _rtB -> B_1036_2155_0 ; _rtB -> B_1036_2160_0_o =
_rtB -> B_1036_2156_0 ; } _rtB -> B_1036_2161_0 = _rtP -> P_2157 ; _rtB ->
B_1036_2162_0_j = _rtP -> P_2158 ; if ( _rtB -> B_1036_2147_0_a ) { _rtB ->
B_1036_2163_0_p = _rtB -> B_1036_2161_0 ; _rtB -> B_1036_2164_0_l = _rtB ->
B_1036_2162_0_j ; } else { _rtB -> B_1036_2163_0_p = _rtB -> B_1036_2159_0_b
; _rtB -> B_1036_2164_0_l = _rtB -> B_1036_2160_0_o ; } if ( _rtB ->
B_1036_2148_0_l ) { _rtB -> B_1036_2165_0_c = _rtB -> B_1036_2119_0_n ; _rtB
-> B_1036_2166_0_m = _rtB -> B_1036_2123_0_k ; } else { _rtB ->
B_1036_2165_0_c = _rtB -> B_1036_2163_0_p ; _rtB -> B_1036_2166_0_m = _rtB ->
B_1036_2164_0_l ; } _rtB -> B_1036_2167_0_h = _rtP -> P_2159 ; _rtB ->
B_1036_2168_0_a = _rtP -> P_2160 ; _rtB -> B_1036_2169_0 = _rtP -> P_2161 ;
_rtB -> B_1036_2170_0_o = _rtP -> P_2162 ; _rtB -> B_1036_2171_0 = _rtP ->
P_2163 ; _rtB -> B_1036_2172_0 = _rtP -> P_2164 ; _rtB -> B_1036_2173_0 =
_rtP -> P_2165 ; _rtB -> B_1036_2174_0_i = _rtP -> P_2166 ; _rtB ->
B_1036_2175_0 = _rtP -> P_2167 ; _rtB -> B_1036_2176_0_b = ( _rtB ->
B_1036_1983_0_d == _rtB -> B_1036_2174_0_i ) ; if ( _rtB -> B_1036_2176_0_b )
{ _rtB -> B_1036_2177_0_e = _rtB -> B_1036_2175_0 ; } else { _rtB ->
B_1036_2177_0_e = _rtB -> B_1036_1983_0_d ; } _rtB -> B_1036_2178_0_a = _rtP
-> P_2168 ; _rtB -> B_1036_2179_0_e = _rtP -> P_2169 ; _rtB ->
B_1036_2180_0_h = ( _rtB -> B_1036_2129_0 == _rtB -> B_1036_2167_0_h ) ; _rtB
-> B_1036_2181_0_g = ( _rtB -> B_1036_2129_0 == _rtB -> B_1036_2168_0_a ) ;
_rtB -> B_1036_2182_0_l = ( _rtB -> B_1036_2129_0 == _rtB -> B_1036_2169_0 )
; _rtB -> B_1036_2183_0_h = ( _rtB -> B_1036_2129_0 == _rtB ->
B_1036_2170_0_o ) ; _rtB -> B_1036_2184_0_g = ( _rtB -> B_1036_2129_0 == _rtB
-> B_1036_2171_0 ) ; _rtB -> B_1036_2185_0_d = _rtP -> P_2170 ; _rtB ->
B_1036_2186_0_l = _rtP -> P_2171 ; if ( _rtB -> B_1036_2180_0_h ) { _rtB ->
B_1036_2187_0_i = _rtB -> B_1036_2185_0_d ; _rtB -> B_1036_2188_0_n = _rtB ->
B_1036_2186_0_l ; } else { _rtB -> B_1036_2187_0_i = _rtB -> B_1036_2172_0 ;
_rtB -> B_1036_2188_0_n = _rtB -> B_1036_2172_0 ; } _rtB -> B_1036_2189_0 =
_rtP -> P_2172 ; _rtB -> B_1036_2190_0_c = _rtP -> P_2173 ; if ( _rtB ->
B_1036_2181_0_g ) { _rtB -> B_1036_2191_0 = _rtB -> B_1036_2189_0 ; _rtB ->
B_1036_2192_0 = _rtB -> B_1036_2190_0_c ; } else { _rtB -> B_1036_2191_0 =
_rtB -> B_1036_2187_0_i ; _rtB -> B_1036_2192_0 = _rtB -> B_1036_2188_0_n ; }
_rtB -> B_1036_2193_0 = _rtP -> P_2174 ; _rtB -> B_1036_2194_0_i = _rtP ->
P_2175 ; if ( _rtB -> B_1036_2182_0_l ) { _rtB -> B_1036_2195_0 = _rtB ->
B_1036_2193_0 ; _rtB -> B_1036_2196_0 = _rtB -> B_1036_2194_0_i ; } else {
_rtB -> B_1036_2195_0 = _rtB -> B_1036_2191_0 ; _rtB -> B_1036_2196_0 = _rtB
-> B_1036_2192_0 ; } _rtB -> B_1036_2197_0 = _rtP -> P_2176 ; _rtB ->
B_1036_2198_0_d = _rtP -> P_2177 ; if ( _rtB -> B_1036_2183_0_h ) { _rtB ->
B_1036_2199_0 = _rtB -> B_1036_2197_0 ; _rtB -> B_1036_2200_0_b = _rtB ->
B_1036_2198_0_d ; } else { _rtB -> B_1036_2199_0 = _rtB -> B_1036_2195_0 ;
_rtB -> B_1036_2200_0_b = _rtB -> B_1036_2196_0 ; } if ( _rtB ->
B_1036_2184_0_g ) { _rtB -> B_1036_2201_0 = _rtB -> B_1036_2126_0_l ; _rtB ->
B_1036_2202_0_a = _rtB -> B_1036_2127_0_o ; } else { _rtB -> B_1036_2201_0 =
_rtB -> B_1036_2199_0 ; _rtB -> B_1036_2202_0_a = _rtB -> B_1036_2200_0_b ; }
_rtB -> B_1036_2203_0_h = ( ( _rtB -> B_1036_2124_0_g != 0.0 ) && ( _rtB ->
B_1036_2120_0_k != 0.0 ) ) ; _rtB -> B_1036_2204_0_k = ! ( _rtB ->
B_1036_2124_0_g != 0.0 ) ; _rtB -> B_1036_2205_0_p = ( _rtB ->
B_1036_2204_0_k && ( _rtB -> B_1036_2120_0_k != 0.0 ) ) ; _rtB ->
B_1036_2206_0_n = ! _rtB -> B_1036_1968_0_j ; _rtB -> B_1036_2207_0_j = true
; _rtB -> B_1036_2208_0_b = _rtP -> P_2178 ; _rtB -> B_1036_2209_0 = _rtP ->
P_2179 ; _rtB -> B_1036_2210_0 = _rtP -> P_2180 ; _rtB -> B_1036_2211_0_n = (
_rtB -> B_1036_2210_0 == _rtB -> B_1036_2208_0_b ) ; _rtB -> B_1036_2212_0_i
= ! _rtB -> B_1036_2211_0_n ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_fw
, & _rtDW -> OFFDelay_fw , & _rtP -> OFFDelay_fw ) ; DZG_v3_ONDelayTID2 ( S ,
& _rtB -> ONDelay_llc , & _rtDW -> ONDelay_llc , & _rtP -> ONDelay_llc ) ;
_rtB -> B_1036_2215_0_n = _rtP -> P_2181 ; _rtB -> B_1036_2216_0_b = _rtP ->
P_2182 ; _rtB -> B_1036_2217_0_p [ 0 ] = _rtP -> P_2183 [ 0 ] ; _rtB ->
B_1036_2218_0 [ 0 ] = _rtP -> P_2184 [ 0 ] ; _rtB -> B_1036_2219_0 [ 0 ] =
_rtP -> P_2185 [ 0 ] ; _rtB -> B_1036_2217_0_p [ 1 ] = _rtP -> P_2183 [ 1 ] ;
_rtB -> B_1036_2218_0 [ 1 ] = _rtP -> P_2184 [ 1 ] ; _rtB -> B_1036_2219_0 [
1 ] = _rtP -> P_2185 [ 1 ] ; switch ( ( int32_T ) _rtB -> B_1036_2216_0_b ) {
case 1 : _rtB -> B_1036_2220_0_i [ 0 ] = _rtB -> B_1036_2217_0_p [ 0 ] ; _rtB
-> B_1036_2220_0_i [ 1 ] = _rtB -> B_1036_2217_0_p [ 1 ] ; break ; case 2 :
_rtB -> B_1036_2220_0_i [ 0 ] = _rtB -> B_1036_2218_0 [ 0 ] ; _rtB ->
B_1036_2220_0_i [ 1 ] = _rtB -> B_1036_2218_0 [ 1 ] ; break ; default : _rtB
-> B_1036_2220_0_i [ 0 ] = _rtB -> B_1036_2219_0 [ 0 ] ; _rtB ->
B_1036_2220_0_i [ 1 ] = _rtB -> B_1036_2219_0 [ 1 ] ; break ; } _rtB ->
B_1036_2221_0_m = false ; _rtB -> B_1036_2222_0_i = false ; _rtB ->
B_1036_2223_0_k = _rtP -> P_2186 ; _rtB -> B_1036_2224_0_i = ( _rtB ->
B_1036_2223_0_k != 0.0 ) ; _rtB -> B_1036_2225_0_c = _rtP -> P_2187 ; _rtB ->
B_1036_2226_0_b = ( ( _rtB -> B_1036_2225_0_c != 0.0 ) || ( _rtB ->
B_1036_2223_0_k != 0.0 ) || ( _rtB -> B_1036_2223_0_k != 0.0 ) || ( _rtB ->
B_1036_2223_0_k != 0.0 ) ) ; _rtB -> B_1036_2227_0 = _rtP -> P_2188 ; _rtB ->
B_1036_2228_0 = _rtP -> P_2189 ; _rtB -> B_1036_2229_0_n = _rtP -> P_2190 ;
_rtB -> B_1036_2230_0_b = _rtP -> P_2191 ; _rtB -> B_1036_2231_0_k = ! _rtB
-> B_1036_2224_0_i ; _rtB -> B_1036_2232_0_l = ! ( _rtB -> B_1036_2223_0_k !=
0.0 ) ; _rtB -> B_1036_2233_0_f = ( _rtB -> B_1036_2232_0_l && ( _rtB ->
B_1036_2229_0_n != 0.0 ) ) ; _rtB -> B_1036_2234_0_h = ( ( _rtB ->
B_1036_2228_0 != 0.0 ) && ( _rtB -> B_1036_2223_0_k != 0.0 ) ) ; _rtB ->
B_1036_2235_0_g = ! ( _rtB -> B_1036_2223_0_k != 0.0 ) ; _rtB ->
B_1036_2236_0_h = ( _rtB -> B_1036_2235_0_g && ( _rtB -> B_1036_2229_0_n !=
0.0 ) ) ; _rtB -> B_1036_2237_0 = ( _rtB -> B_1036_2234_0_h || _rtB ->
B_1036_2236_0_h ) ; _rtB -> B_1036_2238_0_l = ( ( _rtB -> B_1036_2228_0 !=
0.0 ) && ( _rtB -> B_1036_2223_0_k != 0.0 ) ) ; _rtB -> B_1036_2239_0_p = ! (
_rtB -> B_1036_2223_0_k != 0.0 ) ; _rtB -> B_1036_2240_0_k = ( _rtB ->
B_1036_2239_0_p && ( _rtB -> B_1036_2229_0_n != 0.0 ) ) ; _rtB ->
B_1036_2241_0_c = ( _rtB -> B_1036_2238_0_l || _rtB -> B_1036_2240_0_k ) ;
_rtB -> B_1036_2242_0_j = ( ( _rtB -> B_1036_2228_0 != 0.0 ) && ( _rtB ->
B_1036_2223_0_k != 0.0 ) ) ; _rtB -> B_1036_2243_0_a = ( _rtB ->
B_1036_2242_0_j || _rtB -> B_1036_2233_0_f ) ; _rtB -> B_1036_2244_0_b = ! (
_rtB -> B_1036_2223_0_k != 0.0 ) ; _rtB -> B_1036_2245_0_j = ! ( _rtB ->
B_1036_2223_0_k != 0.0 ) ; _rtB -> B_1036_2246_0 = ! ( _rtB ->
B_1036_2223_0_k != 0.0 ) ; _rtB -> B_1036_2247_0_m = _rtP -> P_2192 ; _rtB ->
B_1036_2248_0_f = ( _rtB -> B_1036_2222_0_i || ( _rtB -> B_1036_2247_0_m !=
0.0 ) || ( _rtB -> B_1036_2247_0_m != 0.0 ) || ( _rtB -> B_1036_2247_0_m !=
0.0 ) ) ; _rtB -> B_1036_2249_0_g = false ; _rtB -> B_1036_2250_0_b = ( _rtB
-> B_1036_2249_0_g || ( _rtB -> B_1036_2247_0_m != 0.0 ) || ( _rtB ->
B_1036_2247_0_m != 0.0 ) || ( _rtB -> B_1036_2247_0_m != 0.0 ) ) ; _rtB ->
B_1036_2251_0_m = _rtP -> P_2193 ; _rtB -> B_1036_2252_0_o = ( ( _rtB ->
B_1036_2251_0_m != 0.0 ) || ( _rtB -> B_1036_2247_0_m != 0.0 ) || ( _rtB ->
B_1036_2247_0_m != 0.0 ) || ( _rtB -> B_1036_2247_0_m != 0.0 ) ) ; _rtB ->
B_1036_2253_0_n = _rtP -> P_2194 ; _rtB -> B_1036_2254_0 = _rtP -> P_2195 ;
_rtB -> B_1036_2255_0 = _rtP -> P_2196 ; _rtB -> B_1036_2256_0_m = _rtP ->
P_2197 ; _rtB -> B_1036_2257_0_a = ! _rtB -> B_1036_2248_0_f ; _rtB ->
B_1036_2258_0_p = ( _rtB -> B_1036_2252_0_o && _rtB -> B_1036_2257_0_a ) ;
_rtB -> B_1036_2259_0_l = false ; _rtB -> B_1036_2260_0_i = true ; _rtB ->
B_1036_2261_0_a = ( ( _rtB -> B_1036_2255_0 != 0.0 ) && _rtB ->
B_1036_2260_0_i ) ; _rtB -> B_1036_2262_0_e = ( _rtB -> B_1036_2259_0_l ||
_rtB -> B_1036_2261_0_a ) ; _rtB -> B_1036_2263_0 = _rtP -> P_2198 ; _rtB ->
B_1036_2264_0 = ( _rtB -> B_1036_2222_0_i || ( _rtB -> B_1036_2263_0 != 0.0 )
|| ( _rtB -> B_1036_2263_0 != 0.0 ) || ( _rtB -> B_1036_2263_0 != 0.0 ) ) ;
_rtB -> B_1036_2265_0_l = _rtP -> P_2199 ; _rtB -> B_1036_2266_0_o = _rtP ->
P_2200 ; _rtB -> B_1036_2267_0_i = _rtP -> P_2201 ; _rtB -> B_1036_2268_0_p =
_rtP -> P_2202 ; _rtB -> B_1036_2269_0_h = false ; _rtB -> B_1036_2270_0_c =
( _rtB -> B_1036_2269_0_h || ( _rtB -> B_1036_2263_0 != 0.0 ) ) ; _rtB ->
B_1036_2271_0_b = ( _rtB -> B_1036_2221_0_m || ( _rtB -> B_1036_2263_0 != 0.0
) ) ; _rtB -> B_1036_2272_0_e = ! _rtB -> B_1036_2264_0 ; _rtB ->
B_1036_2273_0 = ! _rtB -> B_1036_2264_0 ; _rtB -> B_1036_2274_0_j = ! _rtB ->
B_1036_2264_0 ; _rtB -> B_1036_2275_0_n = _rtP -> P_2203 ; _rtB ->
B_1036_2276_0_c = _rtP -> P_2204 ; _rtB -> B_1036_2277_0_e = _rtP -> P_2205 ;
_rtB -> B_1036_2278_0_d = ( _rtB -> B_1036_2277_0_e == _rtB ->
B_1036_2275_0_n ) ; _rtB -> B_1036_2279_0_j = ! _rtB -> B_1036_2278_0_d ;
DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_nt , & _rtDW -> OFFDelay_nt , &
_rtP -> OFFDelay_nt ) ; DZG_v3_ONDelayTID2 ( S , & _rtB -> ONDelay_bj , &
_rtDW -> ONDelay_bj , & _rtP -> ONDelay_bj ) ; _rtB -> B_1036_2282_0 = _rtP
-> P_2206 ; _rtB -> B_1036_2283_0_j = _rtP -> P_2207 ; _rtB ->
B_1036_2284_0_l = _rtP -> P_2208 ; _rtB -> B_1036_2285_0_b = ( _rtB ->
B_1036_2284_0_l == _rtB -> B_1036_2282_0 ) ; _rtB -> B_1036_2286_0_h = ! _rtB
-> B_1036_2285_0_b ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_ny , &
_rtDW -> OFFDelay_ny , & _rtP -> OFFDelay_ny ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_ny , & _rtDW -> ONDelay_ny , & _rtP -> ONDelay_ny ) ; _rtB ->
B_1036_2289_0_j = _rtP -> P_2209 ; _rtB -> B_1036_2290_0 = _rtP -> P_2210 ;
_rtB -> B_1036_2291_0 = _rtP -> P_2211 ; _rtB -> B_1036_2292_0_c = ( _rtB ->
B_1036_2291_0 == _rtB -> B_1036_2289_0_j ) ; _rtB -> B_1036_2293_0_h = ! _rtB
-> B_1036_2292_0_c ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_c5 , &
_rtDW -> OFFDelay_c5 , & _rtP -> OFFDelay_c5 ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_i3 , & _rtDW -> ONDelay_i3 , & _rtP -> ONDelay_i3 ) ; _rtB ->
B_1036_2296_0_c = _rtP -> P_2212 ; _rtB -> B_1036_2297_0_b = _rtP -> P_2213 ;
_rtB -> B_1036_2298_0_i = _rtP -> P_2214 ; _rtB -> B_1036_2299_0_n = ( _rtB
-> B_1036_2298_0_i == _rtB -> B_1036_2296_0_c ) ; _rtB -> B_1036_2300_0 = !
_rtB -> B_1036_2299_0_n ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_pb , &
_rtDW -> OFFDelay_pb , & _rtP -> OFFDelay_pb ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_eo , & _rtDW -> ONDelay_eo , & _rtP -> ONDelay_eo ) ; _rtB ->
B_1036_2303_0_k = _rtP -> P_2215 ; _rtB -> B_1036_2304_0_l = _rtP -> P_2216 ;
_rtB -> B_1036_2305_0_j = _rtP -> P_2217 ; _rtB -> B_1036_2306_0_h = ( _rtB
-> B_1036_2305_0_j == _rtB -> B_1036_2303_0_k ) ; _rtB -> B_1036_2307_0_p = !
_rtB -> B_1036_2306_0_h ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_d2 , &
_rtDW -> OFFDelay_d2 , & _rtP -> OFFDelay_d2 ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_l1 , & _rtDW -> ONDelay_l1 , & _rtP -> ONDelay_l1 ) ; _rtB ->
B_1036_2310_0_m = _rtP -> P_2218 ; _rtB -> B_1036_2311_0_j = _rtP -> P_2219 ;
_rtB -> B_1036_2312_0_e = _rtP -> P_2220 ; _rtB -> B_1036_2313_0_p = ( _rtB
-> B_1036_2312_0_e == _rtB -> B_1036_2310_0_m ) ; _rtB -> B_1036_2314_0_o = !
_rtB -> B_1036_2313_0_p ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_ob , &
_rtDW -> OFFDelay_ob , & _rtP -> OFFDelay_ob ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_cf , & _rtDW -> ONDelay_cf , & _rtP -> ONDelay_cf ) ; _rtB ->
B_1036_2317_0_d = _rtP -> P_2221 ; _rtB -> B_1036_2318_0_j = _rtP -> P_2222 ;
_rtB -> B_1036_2319_0_k = _rtP -> P_2223 ; _rtB -> B_1036_2320_0_i = ( _rtB
-> B_1036_2319_0_k == _rtB -> B_1036_2317_0_d ) ; _rtB -> B_1036_2321_0_k = !
_rtB -> B_1036_2320_0_i ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_pj , &
_rtDW -> OFFDelay_pj , & _rtP -> OFFDelay_pj ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_om , & _rtDW -> ONDelay_om , & _rtP -> ONDelay_om ) ; _rtB ->
B_1036_2324_0_o = _rtP -> P_2224 ; _rtB -> B_1036_2325_0_n = _rtP -> P_2225 ;
_rtB -> B_1036_2326_0_b = _rtP -> P_2226 ; _rtB -> B_1036_2327_0_n = ( _rtB
-> B_1036_2326_0_b == _rtB -> B_1036_2324_0_o ) ; _rtB -> B_1036_2328_0_b = !
_rtB -> B_1036_2327_0_n ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_df , &
_rtDW -> OFFDelay_df , & _rtP -> OFFDelay_df ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_g5 , & _rtDW -> ONDelay_g5 , & _rtP -> ONDelay_g5 ) ; _rtB ->
B_1036_2331_0_i = _rtP -> P_2227 ; _rtB -> B_1036_2332_0_d = _rtP -> P_2228 ;
_rtB -> B_1036_2333_0_d = _rtP -> P_2229 ; _rtB -> B_1036_2334_0_n = ( _rtB
-> B_1036_2333_0_d == _rtB -> B_1036_2331_0_i ) ; _rtB -> B_1036_2335_0_j = !
_rtB -> B_1036_2334_0_n ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_pr , &
_rtDW -> OFFDelay_pr , & _rtP -> OFFDelay_pr ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_bk , & _rtDW -> ONDelay_bk , & _rtP -> ONDelay_bk ) ; _rtB ->
B_1036_2338_0 = _rtP -> P_2230 ; _rtB -> B_1036_2339_0 = _rtP -> P_2231 ;
_rtB -> B_1036_2340_0 = _rtP -> P_2232 ; _rtB -> B_1036_2341_0_h = ( _rtB ->
B_1036_2340_0 == _rtB -> B_1036_2338_0 ) ; _rtB -> B_1036_2342_0_k = ! _rtB
-> B_1036_2341_0_h ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_ge , &
_rtDW -> OFFDelay_ge , & _rtP -> OFFDelay_ge ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_mv , & _rtDW -> ONDelay_mv , & _rtP -> ONDelay_mv ) ; _rtB ->
B_1036_2345_0_p = _rtP -> P_2233 ; _rtB -> B_1036_2346_0_e = _rtP -> P_2234 ;
_rtB -> B_1036_2347_0_n = _rtP -> P_2235 ; _rtB -> B_1036_2348_0_i = ( _rtB
-> B_1036_2347_0_n == _rtB -> B_1036_2345_0_p ) ; _rtB -> B_1036_2349_0_k = !
_rtB -> B_1036_2348_0_i ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_hb , &
_rtDW -> OFFDelay_hb , & _rtP -> OFFDelay_hb ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_a , & _rtDW -> ONDelay_a , & _rtP -> ONDelay_a ) ; _rtB ->
B_1036_2352_0_d = _rtP -> P_2236 ; _rtB -> B_1036_2353_0_m = _rtP -> P_2237 ;
_rtB -> B_1036_2354_0_g = _rtP -> P_2238 ; _rtB -> B_1036_2355_0_j = ( _rtB
-> B_1036_2354_0_g == _rtB -> B_1036_2352_0_d ) ; _rtB -> B_1036_2356_0_k = !
_rtB -> B_1036_2355_0_j ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_l0 , &
_rtDW -> OFFDelay_l0 , & _rtP -> OFFDelay_l0 ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_bd , & _rtDW -> ONDelay_bd , & _rtP -> ONDelay_bd ) ; _rtB ->
B_1036_2359_0 = _rtP -> P_2239 ; _rtB -> B_1036_2360_0 = _rtP -> P_2240 ;
_rtB -> B_1036_2361_0 = _rtP -> P_2241 ; _rtB -> B_1036_2362_0_e = ( _rtB ->
B_1036_2361_0 == _rtB -> B_1036_2359_0 ) ; _rtB -> B_1036_2363_0_b = ! _rtB
-> B_1036_2362_0_e ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_kq , &
_rtDW -> OFFDelay_kq , & _rtP -> OFFDelay_kq ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_j3 , & _rtDW -> ONDelay_j3 , & _rtP -> ONDelay_j3 ) ; _rtB ->
B_1036_2366_0_p = ! ( _rtB -> B_1036_2267_0_i != 0.0 ) ; _rtB ->
B_1036_2367_0_p = ! ( _rtB -> B_1036_2265_0_l != 0.0 ) ; _rtB ->
B_1036_2368_0_l = ! ( _rtB -> B_1036_2266_0_o != 0.0 ) ; _rtB ->
B_1036_2369_0_h = ( _rtB -> B_1036_2366_0_p && _rtB -> B_1036_2367_0_p &&
_rtB -> B_1036_2368_0_l ) ; _rtB -> B_1036_2370_0_i = _rtP -> P_2242 ; _rtB
-> B_1036_2371_0_k = _rtP -> P_2243 ; _rtB -> B_1036_2372_0_i [ 0 ] = _rtP ->
P_2244 [ 0 ] ; _rtB -> B_1036_2373_0_m [ 0 ] = _rtP -> P_2245 [ 0 ] ; _rtB ->
B_1036_2374_0_j [ 0 ] = _rtP -> P_2246 [ 0 ] ; _rtB -> B_1036_2372_0_i [ 1 ]
= _rtP -> P_2244 [ 1 ] ; _rtB -> B_1036_2373_0_m [ 1 ] = _rtP -> P_2245 [ 1 ]
; _rtB -> B_1036_2374_0_j [ 1 ] = _rtP -> P_2246 [ 1 ] ; switch ( ( int32_T )
_rtB -> B_1036_2371_0_k ) { case 1 : _rtB -> B_1036_2375_0_a [ 0 ] = _rtB ->
B_1036_2372_0_i [ 0 ] ; _rtB -> B_1036_2375_0_a [ 1 ] = _rtB ->
B_1036_2372_0_i [ 1 ] ; break ; case 2 : _rtB -> B_1036_2375_0_a [ 0 ] = _rtB
-> B_1036_2373_0_m [ 0 ] ; _rtB -> B_1036_2375_0_a [ 1 ] = _rtB ->
B_1036_2373_0_m [ 1 ] ; break ; default : _rtB -> B_1036_2375_0_a [ 0 ] =
_rtB -> B_1036_2374_0_j [ 0 ] ; _rtB -> B_1036_2375_0_a [ 1 ] = _rtB ->
B_1036_2374_0_j [ 1 ] ; break ; } _rtB -> B_1036_2376_0_i = _rtP -> P_2247 ;
_rtB -> B_1036_2377_0_e = _rtP -> P_2248 ; _rtB -> B_1036_2378_0_i [ 0 ] =
_rtP -> P_2249 [ 0 ] ; _rtB -> B_1036_2379_0 [ 0 ] = _rtP -> P_2250 [ 0 ] ;
_rtB -> B_1036_2380_0 [ 0 ] = _rtP -> P_2251 [ 0 ] ; _rtB -> B_1036_2378_0_i
[ 1 ] = _rtP -> P_2249 [ 1 ] ; _rtB -> B_1036_2379_0 [ 1 ] = _rtP -> P_2250 [
1 ] ; _rtB -> B_1036_2380_0 [ 1 ] = _rtP -> P_2251 [ 1 ] ; switch ( ( int32_T
) _rtB -> B_1036_2377_0_e ) { case 1 : _rtB -> B_1036_2381_0 [ 0 ] = _rtB ->
B_1036_2378_0_i [ 0 ] ; _rtB -> B_1036_2381_0 [ 1 ] = _rtB -> B_1036_2378_0_i
[ 1 ] ; break ; case 2 : _rtB -> B_1036_2381_0 [ 0 ] = _rtB -> B_1036_2379_0
[ 0 ] ; _rtB -> B_1036_2381_0 [ 1 ] = _rtB -> B_1036_2379_0 [ 1 ] ; break ;
default : _rtB -> B_1036_2381_0 [ 0 ] = _rtB -> B_1036_2380_0 [ 0 ] ; _rtB ->
B_1036_2381_0 [ 1 ] = _rtB -> B_1036_2380_0 [ 1 ] ; break ; } _rtB ->
B_1036_2382_0 = _rtP -> P_2252 ; _rtB -> B_1036_2383_0 = _rtP -> P_2253 ;
_rtB -> B_1036_2384_0 [ 0 ] = _rtP -> P_2254 [ 0 ] ; _rtB -> B_1036_2385_0 [
0 ] = _rtP -> P_2255 [ 0 ] ; _rtB -> B_1036_2386_0 [ 0 ] = _rtP -> P_2256 [ 0
] ; _rtB -> B_1036_2384_0 [ 1 ] = _rtP -> P_2254 [ 1 ] ; _rtB ->
B_1036_2385_0 [ 1 ] = _rtP -> P_2255 [ 1 ] ; _rtB -> B_1036_2386_0 [ 1 ] =
_rtP -> P_2256 [ 1 ] ; switch ( ( int32_T ) _rtB -> B_1036_2383_0 ) { case 1
: _rtB -> B_1036_2387_0 [ 0 ] = _rtB -> B_1036_2384_0 [ 0 ] ; _rtB ->
B_1036_2387_0 [ 1 ] = _rtB -> B_1036_2384_0 [ 1 ] ; break ; case 2 : _rtB ->
B_1036_2387_0 [ 0 ] = _rtB -> B_1036_2385_0 [ 0 ] ; _rtB -> B_1036_2387_0 [ 1
] = _rtB -> B_1036_2385_0 [ 1 ] ; break ; default : _rtB -> B_1036_2387_0 [ 0
] = _rtB -> B_1036_2386_0 [ 0 ] ; _rtB -> B_1036_2387_0 [ 1 ] = _rtB ->
B_1036_2386_0 [ 1 ] ; break ; } _rtB -> B_1036_2388_0 = _rtP -> P_2257 ; _rtB
-> B_1036_2389_0 = _rtP -> P_2258 ; _rtB -> B_1036_2390_0 = _rtP -> P_2259 ;
_rtB -> B_1036_2391_0 = _rtP -> P_2260 ; _rtB -> B_944_0_0_g = _rtP -> P_50 ;
_rtB -> B_944_1_0_a = _rtP -> P_51 ; _rtB -> B_944_2_0_g = _rtP -> P_52 ;
_rtB -> B_944_3_0 = _rtP -> P_53 ; _rtB -> B_944_4_0_a = ( uint8_T ) ( _rtB
-> B_944_2_0_g == _rtB -> B_944_3_0 ) ; _rtB -> B_944_5_0 = _rtP -> P_54 ;
_rtB -> B_944_6_0_e = ( uint8_T ) ( _rtB -> B_944_2_0_g == _rtB -> B_944_5_0
) ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
AutomaticGainControl_SubsysRanBC ) ; } _rtB -> B_1036_2393_0 = _rtP -> P_2261
; _rtB -> B_1036_2394_0 = _rtP -> P_2262 ; _rtB -> B_1036_2395_0 = _rtP ->
P_2263 ; _rtB -> B_1036_2396_0 = _rtP -> P_2264 ; _rtB -> B_1036_2397_0 =
_rtP -> P_2265 ; _rtB -> B_1036_2398_0 = ( uint8_T ) ( _rtB -> B_1036_2396_0
== _rtB -> B_1036_2397_0 ) ; _rtB -> B_1036_2399_0_h = _rtP -> P_2266 ; _rtB
-> B_1036_2400_0_o = ( uint8_T ) ( _rtB -> B_1036_2396_0 == _rtB ->
B_1036_2399_0_h ) ; _rtB -> B_1036_2401_0_m = _rtP -> P_2267 ; _rtB ->
B_1036_2402_0_a = _rtP -> P_2268 ; _rtB -> B_1036_2403_0_o = _rtP -> P_2269 ;
_rtB -> B_1036_2404_0_o = _rtP -> P_2270 ; _rtB -> B_1036_2405_0_n = _rtP ->
P_2271 ; _rtB -> B_1036_2406_0_k = _rtP -> P_2272 ; _rtB -> B_1036_2407_0 =
_rtP -> P_2273 ; _rtB -> B_1036_2408_0_h = _rtP -> P_2274 ;
ssCallAccelRunBlock ( S , 1036 , 8302 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_1036_2410_0_d = _rtP -> P_2275 ; _rtB -> B_1036_2411_0_a = _rtP -> P_2276 ;
_rtB -> B_1036_2412_0_h = _rtP -> P_2277 ; _rtB -> B_1036_2413_0_c = _rtP ->
P_2278 ; _rtB -> B_1036_2414_0_b = _rtP -> P_2279 ; _rtB -> B_1036_2415_0 =
_rtP -> P_2280 ; _rtB -> B_1036_2416_0_c = _rtP -> P_2281 ; _rtB ->
B_1036_2417_0_g = _rtP -> P_2282 ; _rtB -> B_1036_2418_0_p = 0.0 ; _rtB ->
B_1036_2419_0_i = 0.0 ; _rtB -> B_1036_2420_0_j = _rtB -> B_1036_2418_0_p +
_rtB -> B_1036_2419_0_i ; _rtB -> B_1036_2421_0_c = muDoubleScalarSqrt ( _rtB
-> B_1036_2420_0_j ) ; _rtB -> B_1036_2422_0_c = _rtP -> P_2283 ; _rtB ->
B_1036_2423_0_m = ( _rtB -> B_1036_2421_0_c > _rtB -> B_1036_2422_0_c ) ;
_rtB -> B_1036_2424_0_e = true ; _rtB -> B_1036_2425_0 = _rtP -> P_2284 ;
_rtB -> B_1036_2426_0 = _rtP -> P_2285 ; _rtB -> B_1036_2427_0_d = true ;
_rtB -> B_1036_2428_0 = _rtP -> P_2286 ; _rtB -> B_1036_2429_0 = 0.0 ; _rtB
-> B_1036_2430_0 = 0.0 ; _rtB -> B_1036_2431_0 = _rtB -> B_1036_2429_0 + _rtB
-> B_1036_2430_0 ; _rtB -> B_1036_2432_0 = muDoubleScalarSqrt ( _rtB ->
B_1036_2431_0 ) ; _rtB -> B_1036_2433_0_i = ( _rtB -> B_1036_2432_0 > _rtB ->
B_1036_2422_0_c ) ; _rtB -> B_1036_2434_0 = _rtP -> P_2287 ; _rtB ->
B_1036_2435_0 = _rtP -> P_2288 ; _rtB -> B_1036_2436_0 = _rtP -> P_2289 ;
_rtB -> B_1036_2437_0 = _rtP -> P_2290 ; _rtB -> B_1036_2438_0_k = ( _rtB ->
B_1036_2434_0 == _rtB -> B_1036_2437_0 ) ; _rtB -> B_1036_2439_0_k = ( _rtB
-> B_1036_2428_0 == _rtB -> B_1036_2437_0 ) ; _rtB -> B_1036_2440_0_a = (
_rtB -> B_1036_2438_0_k || _rtB -> B_1036_2439_0_k ) ; _rtB -> B_1036_2441_0
= _rtB -> B_1036_2434_0 / _rtB -> B_1036_2428_0 ; if ( _rtB ->
B_1036_2440_0_a ) { _rtB -> B_1036_2442_0 = _rtB -> B_1036_2436_0 ; } else {
_rtB -> B_1036_2442_0 = _rtB -> B_1036_2441_0 ; } _rtB -> B_1036_2443_0 =
_rtB -> B_1036_2442_0 * _rtB -> B_1036_2422_0_c ; _rtB -> B_1036_2444_0 =
_rtP -> P_2291 ; _rtB -> B_1036_2445_0 = _rtP -> P_2292 ; _rtB ->
B_1036_2446_0_a = ( _rtB -> B_1036_2435_0 == _rtB -> B_1036_2445_0 ) ; _rtB
-> B_1036_2447_0_f = ( _rtB -> B_1036_2425_0 == _rtB -> B_1036_2445_0 ) ;
_rtB -> B_1036_2448_0_j = ( _rtB -> B_1036_2446_0_a || _rtB ->
B_1036_2447_0_f ) ; _rtB -> B_1036_2449_0 = _rtB -> B_1036_2435_0 / _rtB ->
B_1036_2425_0 ; if ( _rtB -> B_1036_2448_0_j ) { _rtB -> B_1036_2450_0 = _rtB
-> B_1036_2444_0 ; } else { _rtB -> B_1036_2450_0 = _rtB -> B_1036_2449_0 ; }
_rtB -> B_1036_2451_0 = _rtB -> B_1036_2450_0 * _rtB -> B_1036_2426_0 ; _rtB
-> B_1036_2452_0 = 0.0 ; _rtB -> B_1036_2453_0 = 0.0 ; _rtB -> B_1036_2454_0
= _rtB -> B_1036_2452_0 + _rtB -> B_1036_2453_0 ; _rtB -> B_1036_2455_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_2454_0 ) ; _rtB -> B_1036_2456_0_e = (
_rtB -> B_1036_2455_0 > _rtB -> B_1036_2426_0 ) ; minV = muDoubleScalarMin (
_rtB -> B_1036_2443_0 , _rtB -> B_1036_2451_0 ) ; _rtB -> B_1036_2457_0 =
minV ; _rtB -> B_1036_2458_0 = 0.0 ; _rtB -> B_1036_2459_0 = 0.0 ; _rtB ->
B_1036_2460_0 = _rtB -> B_1036_2458_0 + _rtB -> B_1036_2459_0 ; _rtB ->
B_1036_2461_0 = muDoubleScalarSqrt ( _rtB -> B_1036_2460_0 ) ; _rtB ->
B_1036_2462_0_i = ( _rtB -> B_1036_2461_0 > _rtB -> B_1036_2426_0 ) ; _rtB ->
B_1036_2463_0 = 0.0 ; _rtB -> B_1036_2464_0 = 0.0 ; _rtB -> B_1036_2465_0 =
_rtB -> B_1036_2463_0 + _rtB -> B_1036_2464_0 ; _rtB -> B_1036_2466_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_2465_0 ) ; _rtB -> B_1036_2467_0_g = (
_rtB -> B_1036_2466_0 > _rtB -> B_1036_2426_0 ) ; _rtB -> B_1036_2468_0 =
_rtP -> P_2293 ; _rtB -> B_1036_2469_0 = _rtP -> P_2294 ; _rtB ->
B_1036_2470_0 = _rtP -> P_2295 ; _rtB -> B_1036_2471_0 = _rtP -> P_2296 ;
_rtB -> B_1036_2472_0 = _rtP -> P_2297 ; _rtB -> B_1036_2473_0 = _rtP ->
P_2298 ; _rtB -> B_1036_2474_0 = 0.0 ; _rtB -> B_1036_2475_0 = 0.0 ; _rtB ->
B_1036_2476_0 = _rtB -> B_1036_2474_0 + _rtB -> B_1036_2475_0 ; _rtB ->
B_1036_2477_0 = muDoubleScalarSqrt ( _rtB -> B_1036_2476_0 ) ; _rtB ->
B_1036_2478_0_n = ( _rtB -> B_1036_2477_0 > _rtB -> B_1036_2422_0_c ) ; _rtB
-> B_1036_2479_0 = 0.0 ; _rtB -> B_1036_2480_0 = 0.0 ; _rtB -> B_1036_2481_0
= _rtB -> B_1036_2479_0 + _rtB -> B_1036_2480_0 ; _rtB -> B_1036_2482_0 =
muDoubleScalarSqrt ( _rtB -> B_1036_2481_0 ) ; _rtB -> B_1036_2483_0_g = (
_rtB -> B_1036_2482_0 > _rtB -> B_1036_2426_0 ) ; _rtB -> B_1036_2484_0 =
_rtP -> P_2299 ; _rtB -> B_1036_2485_0_f = _rtP -> P_2300 ; _rtB ->
B_1036_2486_0_d = _rtP -> P_2301 ; _rtB -> B_1036_2487_0_b = ( _rtB ->
B_1036_2486_0_d == _rtB -> B_1036_2484_0 ) ; _rtB -> B_1036_2488_0_b = ! _rtB
-> B_1036_2487_0_b ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_mb , &
_rtDW -> OFFDelay_mb , & _rtP -> OFFDelay_mb ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_ix , & _rtDW -> ONDelay_ix , & _rtP -> ONDelay_ix ) ; _rtB ->
B_1036_2491_0 = _rtP -> P_2302 ; _rtB -> B_1036_2492_0 = _rtP -> P_2303 ;
_rtB -> B_1036_2493_0_n = _rtP -> P_2304 ; _rtB -> B_1036_2494_0_b = ( _rtB
-> B_1036_2493_0_n == _rtB -> B_1036_2491_0 ) ; _rtB -> B_1036_2495_0 = !
_rtB -> B_1036_2494_0_b ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_a2 , &
_rtDW -> OFFDelay_a2 , & _rtP -> OFFDelay_a2 ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_g , & _rtDW -> ONDelay_g , & _rtP -> ONDelay_g ) ; _rtB ->
B_1036_2498_0_b = _rtP -> P_2305 ; _rtB -> B_1036_2499_0_f = _rtP -> P_2306 ;
_rtB -> B_1036_2500_0 = _rtP -> P_2307 ; _rtB -> B_1036_2501_0 = ( _rtB ->
B_1036_2500_0 == _rtB -> B_1036_2498_0_b ) ; _rtB -> B_1036_2502_0_j = ! _rtB
-> B_1036_2501_0 ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_br , & _rtDW
-> OFFDelay_br , & _rtP -> OFFDelay_br ) ; DZG_v3_ONDelayTID2 ( S , & _rtB ->
ONDelay_be , & _rtDW -> ONDelay_be , & _rtP -> ONDelay_be ) ; _rtB ->
B_1036_2505_0 = _rtP -> P_2308 ; _rtB -> B_1036_2506_0 = _rtP -> P_2309 ;
_rtB -> B_1036_2507_0_j = _rtP -> P_2310 ; _rtB -> B_1036_2508_0_o = ( _rtB
-> B_1036_2507_0_j == _rtB -> B_1036_2505_0 ) ; _rtB -> B_1036_2509_0_j = !
_rtB -> B_1036_2508_0_o ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_nk , &
_rtDW -> OFFDelay_nk , & _rtP -> OFFDelay_nk ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_c3 , & _rtDW -> ONDelay_c3 , & _rtP -> ONDelay_c3 ) ; _rtB ->
B_1036_2512_0 = false ; _rtB -> B_1036_2513_0 = _rtP -> P_2311 ; _rtB ->
B_1036_2514_0_i = ( _rtB -> B_1036_2512_0 || ( _rtB -> B_1036_2513_0 != 0.0 )
|| ( _rtB -> B_1036_2513_0 != 0.0 ) || ( _rtB -> B_1036_2513_0 != 0.0 ) ) ;
_rtB -> B_1036_2515_0_h = _rtP -> P_2312 ; _rtB -> B_1036_2516_0_n = ( ( _rtB
-> B_1036_2515_0_h != 0.0 ) || ( _rtB -> B_1036_2513_0 != 0.0 ) || ( _rtB ->
B_1036_2513_0 != 0.0 ) || ( _rtB -> B_1036_2513_0 != 0.0 ) ) ; _rtB ->
B_1036_2517_0_o = ( ( _rtB -> B_1036_2515_0_h != 0.0 ) || ( _rtB ->
B_1036_2513_0 != 0.0 ) || ( _rtB -> B_1036_2513_0 != 0.0 ) || ( _rtB ->
B_1036_2513_0 != 0.0 ) ) ; _rtB -> B_1036_2518_0 = _rtP -> P_2313 ; _rtB ->
B_1036_2519_0 = _rtP -> P_2314 ; _rtB -> B_1036_2520_0 = _rtP -> P_2315 ;
_rtB -> B_1036_2521_0 = _rtP -> P_2316 ; _rtB -> B_1036_2522_0_d = ! _rtB ->
B_1036_2514_0_i ; _rtB -> B_1036_2523_0_k = ( _rtB -> B_1036_2517_0_o && _rtB
-> B_1036_2522_0_d ) ; _rtB -> B_1036_2524_0 = ( ( _rtB -> B_1036_2519_0 !=
0.0 ) && ( _rtB -> B_1036_2513_0 != 0.0 ) ) ; _rtB -> B_1036_2525_0_g = ! (
_rtB -> B_1036_2513_0 != 0.0 ) ; _rtB -> B_1036_2526_0_o = ( ( _rtB ->
B_1036_2520_0 != 0.0 ) && _rtB -> B_1036_2525_0_g ) ; _rtB -> B_1036_2527_0_n
= ( _rtB -> B_1036_2524_0 || _rtB -> B_1036_2526_0_o ) ; _rtB ->
B_1036_2528_0 = _rtP -> P_2317 ; _rtB -> B_1036_2529_0 = _rtP -> P_2318 ;
_rtB -> B_1036_2530_0 = _rtP -> P_2319 ; _rtB -> B_1036_2531_0_h = ( _rtB ->
B_1036_2530_0 == _rtB -> B_1036_2528_0 ) ; _rtB -> B_1036_2532_0 = ! _rtB ->
B_1036_2531_0_h ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_a4 , & _rtDW
-> OFFDelay_a4 , & _rtP -> OFFDelay_a4 ) ; DZG_v3_ONDelayTID2 ( S , & _rtB ->
ONDelay_pn , & _rtDW -> ONDelay_pn , & _rtP -> ONDelay_pn ) ; _rtB ->
B_1036_2535_0 = _rtP -> P_2320 ; _rtB -> B_1036_2536_0 = _rtP -> P_2321 ;
_rtB -> B_1036_2537_0 = _rtP -> P_2322 ; _rtB -> B_1036_2538_0_k = ( _rtB ->
B_1036_2537_0 == _rtB -> B_1036_2535_0 ) ; _rtB -> B_1036_2539_0_l = ! _rtB
-> B_1036_2538_0_k ; DZG_v3_OFFDelayTID2 ( S , & _rtB -> OFFDelay_i5 , &
_rtDW -> OFFDelay_i5 , & _rtP -> OFFDelay_i5 ) ; DZG_v3_ONDelayTID2 ( S , &
_rtB -> ONDelay_hy , & _rtDW -> ONDelay_hy , & _rtP -> ONDelay_hy ) ; _rtB ->
B_1036_2542_0_b = true ; ssCallAccelRunBlock ( S , 1036 , 8424 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8425 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8426 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8427 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8428 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8429 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8430 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8431 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8432 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8433 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8434 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8435 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8436 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8437 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8438 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8439 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8440 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8441 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8442 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8443 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8444 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8445 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8446 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8447 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8448 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8449 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8450 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8451 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8452 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8453 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8454 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8455 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8456 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8457 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8458 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8459 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8460 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8461 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8462 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8463 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8464 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8465 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8466 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8467 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8468 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8469 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8470 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8471 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8472 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8473 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8474 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8475 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8476 ,
SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_1036_2596_0 = _rtP -> P_2323 ; _rtB ->
B_1036_2597_0 = _rtP -> P_2324 ; _rtB -> B_1036_2598_0 = _rtP -> P_2325 ;
_rtB -> B_1036_2599_0 = _rtP -> P_2326 ; _rtB -> B_1036_2600_0 = _rtP ->
P_2327 ; _rtB -> B_1036_2601_0_h = _rtP -> P_2328 ; _rtB -> B_1036_2602_0 =
_rtP -> P_2329 ; _rtB -> B_1036_2603_0 = _rtP -> P_2330 ; _rtB ->
B_1036_2604_0 = _rtP -> P_2331 ; _rtB -> B_1036_2605_0 = _rtP -> P_2332 ;
_rtB -> B_1036_2606_0 = _rtP -> P_2333 ; _rtB -> B_1036_2607_0 = _rtP ->
P_2334 ; _rtB -> B_1036_2608_0 = _rtP -> P_2335 ; _rtB -> B_1036_2609_0 =
_rtP -> P_2336 ; _rtB -> B_1036_2610_0 = _rtP -> P_2337 ; _rtB ->
B_1036_2611_0 = _rtP -> P_2338 ; _rtB -> B_1036_2612_0 = _rtP -> P_2339 ;
_rtB -> B_1036_2613_0_c = _rtP -> P_2340 ; _rtB -> B_1036_2614_0 = _rtP ->
P_2341 ; _rtB -> B_1036_2615_0 = _rtP -> P_2342 ; _rtB -> B_1036_2616_0 =
_rtP -> P_2343 ; _rtB -> B_1036_2617_0 = _rtP -> P_2344 ; _rtB ->
B_1036_2618_0 = _rtP -> P_2345 ; _rtB -> B_1036_2619_0 = _rtP -> P_2346 ;
_rtB -> B_1036_2620_0 = _rtP -> P_2347 ; _rtB -> B_1036_2621_0 = _rtP ->
P_2348 ; _rtB -> B_1036_2622_0 = _rtP -> P_2349 ; _rtB -> B_1036_2623_0 =
_rtP -> P_2350 ; _rtB -> B_1036_2624_0 = _rtP -> P_2351 ; _rtB ->
B_1036_2625_0 = _rtP -> P_2352 ; _rtB -> B_1036_2626_0 = _rtP -> P_2353 ;
_rtB -> B_1036_2627_0 = _rtP -> P_2354 ; _rtB -> B_1036_2628_0 = _rtP ->
P_2355 ; _rtB -> B_1036_2629_0_g = _rtP -> P_2356 ; _rtB -> B_1036_2630_0 =
_rtP -> P_2357 ; _rtB -> B_1036_2631_0 = _rtP -> P_2358 ; _rtB ->
B_1036_2632_0 = _rtP -> P_2359 ; _rtB -> B_1036_2633_0 = _rtP -> P_2360 ;
_rtB -> B_1036_2634_0 = _rtP -> P_2361 ; _rtB -> B_1036_2635_0_e = _rtP ->
P_2362 ; _rtB -> B_1036_2636_0 = _rtP -> P_2363 ; _rtB -> B_1036_2637_0 =
_rtP -> P_2364 ; _rtB -> B_1036_2638_0 = _rtP -> P_2365 ; _rtB ->
B_1036_2639_0 = _rtP -> P_2366 ; _rtB -> B_1036_2640_0 = _rtP -> P_2367 ;
_rtB -> B_1036_2641_0 = _rtP -> P_2368 ; _rtB -> B_1036_2642_0_e = _rtP ->
P_2369 ; _rtB -> B_1036_2643_0 = _rtP -> P_2370 ; _rtB -> B_1036_2644_0 =
_rtP -> P_2371 ; _rtB -> B_1036_2645_0 = _rtP -> P_2372 ; ssCallAccelRunBlock
( S , 1036 , 8527 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 ,
8528 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8529 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8530 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8531 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8532 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8533 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8534 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8535 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8536 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 1036 , 8537 ,
SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_1036_2657_0 = _rtP -> P_2373 ; _rtB ->
B_1036_2658_0 = _rtP -> P_2374 ; _rtB -> B_1036_2659_0 = _rtP -> P_2375 ;
_rtB -> B_1036_2660_0 = _rtP -> P_2376 ; _rtB -> B_1036_2661_0_f = ( _rtB ->
B_1036_2660_0 != 0.0 ) ; _rtB -> B_1036_2662_0_e = ( _rtB -> B_1036_2659_0 !=
0.0 ) ; _rtB -> B_1036_2663_0 = ( _rtB -> B_1036_2658_0 != 0.0 ) ; _rtB ->
B_1036_2664_0 = _rtP -> P_2377 ; _rtB -> B_1036_2665_0 = _rtP -> P_2378 ;
_rtB -> B_1036_2666_0 = _rtP -> P_2379 ; _rtB -> B_1036_2667_0 = _rtP ->
P_2380 ; _rtB -> B_1036_2668_0 = _rtP -> P_2381 ; _rtB -> B_1036_2669_0_i = (
_rtB -> B_1036_2668_0 != 0.0 ) ; _rtB -> B_1036_2670_0_b = ( _rtB ->
B_1036_2667_0 != 0.0 ) ; _rtB -> B_1036_2671_0_e = ( _rtB -> B_1036_2666_0 !=
0.0 ) ; _rtB -> B_1036_2672_0_a = _rtP -> P_2382 ; _rtB -> B_1036_2673_0 =
_rtP -> P_2383 ; _rtB -> B_1036_2674_0 = _rtP -> P_2384 ; _rtB ->
B_1036_2675_0 = _rtP -> P_2385 ; _rtB -> B_1036_2676_0 = _rtP -> P_2386 ;
_rtB -> B_1036_2677_0 = _rtP -> P_2387 ; _rtB -> B_1036_2678_0_h = _rtP ->
P_2388 ; UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdate ( SimStruct * S , int_T tid ) { real_T HoldSine ;
real_T HoldCosine ; int32_T isHit ; B_DZG_v3_T * _rtB ; P_DZG_v3_T * _rtP ;
DW_DZG_v3_T * _rtDW ; _rtDW = ( ( DW_DZG_v3_T * ) ssGetRootDWork ( S ) ) ;
_rtP = ( ( P_DZG_v3_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( ( B_DZG_v3_T * )
_ssGetModelBlockIO ( S ) ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { ssCallAccelRunBlock ( S , 1036 , 0 , SS_CALL_MDL_UPDATE ) ; HoldSine
= _rtDW -> lastSin ; HoldCosine = _rtDW -> lastCos ; _rtDW -> lastSin =
HoldSine * _rtP -> P_137 + HoldCosine * _rtP -> P_136 ; _rtDW -> lastCos =
HoldCosine * _rtP -> P_137 - HoldSine * _rtP -> P_136 ; HoldSine = _rtDW ->
lastSin_a ; HoldCosine = _rtDW -> lastCos_m ; _rtDW -> lastSin_a = HoldSine *
_rtP -> P_144 + HoldCosine * _rtP -> P_143 ; _rtDW -> lastCos_m = HoldCosine
* _rtP -> P_144 - HoldSine * _rtP -> P_143 ; HoldSine = _rtDW -> lastSin_k ;
HoldCosine = _rtDW -> lastCos_k ; _rtDW -> lastSin_k = HoldSine * _rtP ->
P_151 + HoldCosine * _rtP -> P_150 ; _rtDW -> lastCos_k = HoldCosine * _rtP
-> P_151 - HoldSine * _rtP -> P_150 ; HoldSine = _rtDW -> lastSin_f ;
HoldCosine = _rtDW -> lastCos_g ; _rtDW -> lastSin_f = HoldSine * _rtP ->
P_158 + HoldCosine * _rtP -> P_157 ; _rtDW -> lastCos_g = HoldCosine * _rtP
-> P_158 - HoldSine * _rtP -> P_157 ; HoldSine = _rtDW -> lastSin_j ;
HoldCosine = _rtDW -> lastCos_m2 ; _rtDW -> lastSin_j = HoldSine * _rtP ->
P_165 + HoldCosine * _rtP -> P_164 ; _rtDW -> lastCos_m2 = HoldCosine * _rtP
-> P_165 - HoldSine * _rtP -> P_164 ; HoldSine = _rtDW -> lastSin_l ;
HoldCosine = _rtDW -> lastCos_n ; _rtDW -> lastSin_l = HoldSine * _rtP ->
P_172 + HoldCosine * _rtP -> P_171 ; _rtDW -> lastCos_n = HoldCosine * _rtP
-> P_172 - HoldSine * _rtP -> P_171 ; ssCallAccelRunBlock ( S , 1036 , 7 ,
SS_CALL_MDL_UPDATE ) ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * *
) & _rtDW -> TransportDelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK . Head = ( ( _rtDW ->
TransportDelay_IWORK . Head < ( _rtDW -> TransportDelay_IWORK .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK . Head + 1 ) : 0 )
; if ( _rtDW -> TransportDelay_IWORK . Head == _rtDW -> TransportDelay_IWORK
. Tail ) { if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK . CircularBufSize , & _rtDW -> TransportDelay_IWORK .
Tail , & _rtDW -> TransportDelay_IWORK . Head , & _rtDW ->
TransportDelay_IWORK . Last , simTime - _rtP -> P_186 , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK . Head ] = _rtB ->
B_1036_8_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput = _rtB -> B_1036_15_0 ; } { real_T * * uBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK_a . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_a .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_p . Head = ( ( _rtDW -> TransportDelay_IWORK_p . Head <
( _rtDW -> TransportDelay_IWORK_p . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_p . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_p . Head == _rtDW -> TransportDelay_IWORK_p . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_p . CircularBufSize , & _rtDW -> TransportDelay_IWORK_p
. Tail , & _rtDW -> TransportDelay_IWORK_p . Head , & _rtDW ->
TransportDelay_IWORK_p . Last , simTime - _rtP -> P_191 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_p .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_p . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_p . Head ] = _rtB ->
B_1036_16_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_i = _rtB -> B_1036_23_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_f . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_f .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_o . Head = ( ( _rtDW -> TransportDelay_IWORK_o . Head <
( _rtDW -> TransportDelay_IWORK_o . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_o . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_o . Head == _rtDW -> TransportDelay_IWORK_o . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_o . CircularBufSize , & _rtDW -> TransportDelay_IWORK_o
. Tail , & _rtDW -> TransportDelay_IWORK_o . Head , & _rtDW ->
TransportDelay_IWORK_o . Last , simTime - _rtP -> P_196 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_o .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_o . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_o . Head ] = _rtB ->
B_1036_26_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_b = _rtB -> B_1036_33_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_g . Head = ( ( _rtDW -> TransportDelay_IWORK_g . Head <
( _rtDW -> TransportDelay_IWORK_g . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_g . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_g . Head == _rtDW -> TransportDelay_IWORK_g . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_g . CircularBufSize , & _rtDW -> TransportDelay_IWORK_g
. Tail , & _rtDW -> TransportDelay_IWORK_g . Head , & _rtDW ->
TransportDelay_IWORK_g . Last , simTime - _rtP -> P_201 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_g .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_g . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_g . Head ] = _rtB ->
B_1036_34_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_k = _rtB -> B_1036_41_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fr . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fr
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_a . Head = ( ( _rtDW -> TransportDelay_IWORK_a . Head <
( _rtDW -> TransportDelay_IWORK_a . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_a . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_a . Head == _rtDW -> TransportDelay_IWORK_a . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_a . CircularBufSize , & _rtDW -> TransportDelay_IWORK_a
. Tail , & _rtDW -> TransportDelay_IWORK_a . Head , & _rtDW ->
TransportDelay_IWORK_a . Last , simTime - _rtP -> P_206 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_a .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_a . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_a . Head ] = _rtB ->
B_1036_44_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_bv = _rtB -> B_1036_51_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_e . Head = ( ( _rtDW -> TransportDelay_IWORK_e . Head <
( _rtDW -> TransportDelay_IWORK_e . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_e . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_e . Head == _rtDW -> TransportDelay_IWORK_e . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_e . CircularBufSize , & _rtDW -> TransportDelay_IWORK_e
. Tail , & _rtDW -> TransportDelay_IWORK_e . Head , & _rtDW ->
TransportDelay_IWORK_e . Last , simTime - _rtP -> P_211 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_e .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_e . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_e . Head ] = _rtB ->
B_1036_52_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_d = _rtB -> B_1036_59_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_eo . Head = ( ( _rtDW -> TransportDelay_IWORK_eo . Head
< ( _rtDW -> TransportDelay_IWORK_eo . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_eo . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_eo . Head == _rtDW -> TransportDelay_IWORK_eo . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_eo . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_eo . Tail , & _rtDW -> TransportDelay_IWORK_eo . Head ,
& _rtDW -> TransportDelay_IWORK_eo . Last , simTime - _rtP -> P_216 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_eo . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_eo . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_eo . Head ] = _rtB -> B_1036_62_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kb = _rtB -> B_1036_69_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mc . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mc .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_gp . Head = ( ( _rtDW -> TransportDelay_IWORK_gp . Head
< ( _rtDW -> TransportDelay_IWORK_gp . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_gp . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_gp . Head == _rtDW -> TransportDelay_IWORK_gp . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_gp . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_gp . Tail , & _rtDW -> TransportDelay_IWORK_gp . Head ,
& _rtDW -> TransportDelay_IWORK_gp . Last , simTime - _rtP -> P_221 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_gp . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_gp . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_gp . Head ] = _rtB -> B_1036_70_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_g = _rtB -> B_1036_77_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_l . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_l . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_gk .
Head = ( ( _rtDW -> TransportDelay_IWORK_gk . Head < ( _rtDW ->
TransportDelay_IWORK_gk . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_gk . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_gk . Head == _rtDW -> TransportDelay_IWORK_gk . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_gk . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_gk . Tail , & _rtDW -> TransportDelay_IWORK_gk . Head ,
& _rtDW -> TransportDelay_IWORK_gk . Last , simTime - _rtP -> P_226 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_gk . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_gk . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_gk . Head ] = _rtB -> B_1036_80_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_j = _rtB -> B_1036_87_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_be . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_be .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_b . Head = ( ( _rtDW -> TransportDelay_IWORK_b . Head <
( _rtDW -> TransportDelay_IWORK_b . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_b . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_b . Head == _rtDW -> TransportDelay_IWORK_b . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_b . CircularBufSize , & _rtDW -> TransportDelay_IWORK_b
. Tail , & _rtDW -> TransportDelay_IWORK_b . Head , & _rtDW ->
TransportDelay_IWORK_b . Last , simTime - _rtP -> P_231 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_b .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_b . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_b . Head ] = _rtB ->
B_1036_88_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_je = _rtB -> B_1036_95_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_k . Head = ( ( _rtDW -> TransportDelay_IWORK_k . Head <
( _rtDW -> TransportDelay_IWORK_k . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_k . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_k . Head == _rtDW -> TransportDelay_IWORK_k . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_k . CircularBufSize , & _rtDW -> TransportDelay_IWORK_k
. Tail , & _rtDW -> TransportDelay_IWORK_k . Head , & _rtDW ->
TransportDelay_IWORK_k . Last , simTime - _rtP -> P_236 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_k .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_k . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_k . Head ] = _rtB ->
B_1036_98_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_n = _rtB -> B_1036_105_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_g . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_g .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pj . Head = ( ( _rtDW -> TransportDelay_IWORK_pj . Head
< ( _rtDW -> TransportDelay_IWORK_pj . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_pj . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pj . Head == _rtDW -> TransportDelay_IWORK_pj . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pj . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pj . Tail , & _rtDW -> TransportDelay_IWORK_pj . Head ,
& _rtDW -> TransportDelay_IWORK_pj . Last , simTime - _rtP -> P_241 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pj . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pj . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pj . Head ] = _rtB -> B_1036_106_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_o = _rtB -> B_1036_113_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ba . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ba .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ga . Head = ( ( _rtDW -> TransportDelay_IWORK_ga . Head
< ( _rtDW -> TransportDelay_IWORK_ga . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ga . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ga . Head == _rtDW -> TransportDelay_IWORK_ga . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ga . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ga . Tail , & _rtDW -> TransportDelay_IWORK_ga . Head ,
& _rtDW -> TransportDelay_IWORK_ga . Last , simTime - _rtP -> P_246 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ga . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ga . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ga . Head ] = _rtB -> B_1036_116_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_a = _rtB -> B_1036_123_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_bh . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bh .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_j . Head = ( ( _rtDW -> TransportDelay_IWORK_j . Head <
( _rtDW -> TransportDelay_IWORK_j . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_j . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_j . Head == _rtDW -> TransportDelay_IWORK_j . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_j . CircularBufSize , & _rtDW -> TransportDelay_IWORK_j
. Tail , & _rtDW -> TransportDelay_IWORK_j . Head , & _rtDW ->
TransportDelay_IWORK_j . Last , simTime - _rtP -> P_251 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_j .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_j . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_j . Head ] = _rtB ->
B_1036_124_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_ay = _rtB -> B_1036_131_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bv . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bv
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_jd . Head = ( ( _rtDW -> TransportDelay_IWORK_jd . Head
< ( _rtDW -> TransportDelay_IWORK_jd . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_jd . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_jd . Head == _rtDW -> TransportDelay_IWORK_jd . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_jd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jd . Tail , & _rtDW -> TransportDelay_IWORK_jd . Head ,
& _rtDW -> TransportDelay_IWORK_jd . Last , simTime - _rtP -> P_256 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_jd . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_jd . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_jd . Head ] = _rtB -> B_1036_134_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bc = _rtB -> B_1036_141_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_bw . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bw .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_p1 . Head = ( ( _rtDW -> TransportDelay_IWORK_p1 . Head
< ( _rtDW -> TransportDelay_IWORK_p1 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_p1 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_p1 . Head == _rtDW -> TransportDelay_IWORK_p1 . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_p1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_p1 . Tail , & _rtDW -> TransportDelay_IWORK_p1 . Head ,
& _rtDW -> TransportDelay_IWORK_p1 . Last , simTime - _rtP -> P_261 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_p1 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_p1 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_p1 . Head ] = _rtB -> B_1036_142_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_o2 = _rtB -> B_1036_149_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ly . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ly .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_k4 . Head = ( ( _rtDW -> TransportDelay_IWORK_k4 . Head
< ( _rtDW -> TransportDelay_IWORK_k4 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_k4 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_k4 . Head == _rtDW -> TransportDelay_IWORK_k4 . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_k4 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_k4 . Tail , & _rtDW -> TransportDelay_IWORK_k4 . Head ,
& _rtDW -> TransportDelay_IWORK_k4 . Last , simTime - _rtP -> P_266 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_k4 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_k4 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_k4 . Head ] = _rtB -> B_1036_152_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_e = _rtB -> B_1036_159_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ow . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ow .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_og . Head = ( ( _rtDW -> TransportDelay_IWORK_og . Head
< ( _rtDW -> TransportDelay_IWORK_og . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_og . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_og . Head == _rtDW -> TransportDelay_IWORK_og . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_og . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_og . Tail , & _rtDW -> TransportDelay_IWORK_og . Head ,
& _rtDW -> TransportDelay_IWORK_og . Last , simTime - _rtP -> P_271 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_og . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_og . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_og . Head ] = _rtB -> B_1036_160_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_k3 = _rtB -> B_1036_167_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_gm . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_gm .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_i . Head = ( ( _rtDW -> TransportDelay_IWORK_i . Head <
( _rtDW -> TransportDelay_IWORK_i . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_i . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_i . Head == _rtDW -> TransportDelay_IWORK_i . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_i . CircularBufSize , & _rtDW -> TransportDelay_IWORK_i
. Tail , & _rtDW -> TransportDelay_IWORK_i . Head , & _rtDW ->
TransportDelay_IWORK_i . Last , simTime - _rtP -> P_276 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_i .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_i . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_i . Head ] = _rtB ->
B_1036_170_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_di = _rtB -> B_1036_177_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_g3 . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_g3
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pi . Head = ( ( _rtDW -> TransportDelay_IWORK_pi . Head
< ( _rtDW -> TransportDelay_IWORK_pi . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_pi . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pi . Head == _rtDW -> TransportDelay_IWORK_pi . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pi . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pi . Tail , & _rtDW -> TransportDelay_IWORK_pi . Head ,
& _rtDW -> TransportDelay_IWORK_pi . Last , simTime - _rtP -> P_281 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pi . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pi . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pi . Head ] = _rtB -> B_1036_178_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_b1 = _rtB -> B_1036_185_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_kq . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_kq .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_p4 . Head = ( ( _rtDW -> TransportDelay_IWORK_p4 . Head
< ( _rtDW -> TransportDelay_IWORK_p4 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_p4 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_p4 . Head == _rtDW -> TransportDelay_IWORK_p4 . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_p4 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_p4 . Tail , & _rtDW -> TransportDelay_IWORK_p4 . Head ,
& _rtDW -> TransportDelay_IWORK_p4 . Last , simTime - _rtP -> P_286 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_p4 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_p4 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_p4 . Head ] = _rtB -> B_1036_188_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_os = _rtB -> B_1036_195_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_g4 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_g4 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_aj . Head = ( ( _rtDW -> TransportDelay_IWORK_aj . Head
< ( _rtDW -> TransportDelay_IWORK_aj . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_aj . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_aj . Head == _rtDW -> TransportDelay_IWORK_aj . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_aj . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_aj . Tail , & _rtDW -> TransportDelay_IWORK_aj . Head ,
& _rtDW -> TransportDelay_IWORK_aj . Last , simTime - _rtP -> P_291 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_aj . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_aj . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_aj . Head ] = _rtB -> B_1036_196_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ki = _rtB -> B_1036_203_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_p . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_p . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_f .
Head = ( ( _rtDW -> TransportDelay_IWORK_f . Head < ( _rtDW ->
TransportDelay_IWORK_f . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_f . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_f . Head == _rtDW -> TransportDelay_IWORK_f . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_f . CircularBufSize , & _rtDW -> TransportDelay_IWORK_f
. Tail , & _rtDW -> TransportDelay_IWORK_f . Head , & _rtDW ->
TransportDelay_IWORK_f . Last , simTime - _rtP -> P_296 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_f .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_f . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_f . Head ] = _rtB ->
B_1036_206_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_o4 = _rtB -> B_1036_213_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_i . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_i .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_c . Head = ( ( _rtDW -> TransportDelay_IWORK_c . Head <
( _rtDW -> TransportDelay_IWORK_c . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_c . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_c . Head == _rtDW -> TransportDelay_IWORK_c . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_c . CircularBufSize , & _rtDW -> TransportDelay_IWORK_c
. Tail , & _rtDW -> TransportDelay_IWORK_c . Head , & _rtDW ->
TransportDelay_IWORK_c . Last , simTime - _rtP -> P_301 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_c .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_c . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_c . Head ] = _rtB ->
B_1036_214_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_l = _rtB -> B_1036_221_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k4 . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k4
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_m . Head = ( ( _rtDW -> TransportDelay_IWORK_m . Head <
( _rtDW -> TransportDelay_IWORK_m . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_m . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_m . Head == _rtDW -> TransportDelay_IWORK_m . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_m . CircularBufSize , & _rtDW -> TransportDelay_IWORK_m
. Tail , & _rtDW -> TransportDelay_IWORK_m . Head , & _rtDW ->
TransportDelay_IWORK_m . Last , simTime - _rtP -> P_306 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_m .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_m . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_m . Head ] = _rtB ->
B_1036_224_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_d3 = _rtB -> B_1036_231_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_l4 . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_l4
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_d . Head = ( ( _rtDW -> TransportDelay_IWORK_d . Head <
( _rtDW -> TransportDelay_IWORK_d . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_d . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_d . Head == _rtDW -> TransportDelay_IWORK_d . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_d . CircularBufSize , & _rtDW -> TransportDelay_IWORK_d
. Tail , & _rtDW -> TransportDelay_IWORK_d . Head , & _rtDW ->
TransportDelay_IWORK_d . Last , simTime - _rtP -> P_311 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_d .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_d . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_d . Head ] = _rtB ->
B_1036_232_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_bf = _rtB -> B_1036_239_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_n . Head = ( ( _rtDW -> TransportDelay_IWORK_n . Head <
( _rtDW -> TransportDelay_IWORK_n . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_n . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_n . Head == _rtDW -> TransportDelay_IWORK_n . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_n . CircularBufSize , & _rtDW -> TransportDelay_IWORK_n
. Tail , & _rtDW -> TransportDelay_IWORK_n . Head , & _rtDW ->
TransportDelay_IWORK_n . Last , simTime - _rtP -> P_316 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_n .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_n . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_n . Head ] = _rtB ->
B_1036_242_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_lx = _rtB -> B_1036_249_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ij . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ij
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_nz . Head = ( ( _rtDW -> TransportDelay_IWORK_nz . Head
< ( _rtDW -> TransportDelay_IWORK_nz . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_nz . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_nz . Head == _rtDW -> TransportDelay_IWORK_nz . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_nz . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_nz . Tail , & _rtDW -> TransportDelay_IWORK_nz . Head ,
& _rtDW -> TransportDelay_IWORK_nz . Last , simTime - _rtP -> P_321 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_nz . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_nz . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_nz . Head ] = _rtB -> B_1036_250_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_db = _rtB -> B_1036_257_0 ; _rtDW -> u4_PreviousInput =
_rtB -> B_1036_295_0_o ; } DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_kb ,
& _rtDW -> ONDelay_kb ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_mh
, & _rtDW -> OFFDelay_mh ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_m4
, & _rtDW -> ONDelay_m4 ) ; DZG_v3_OFFDelay_Update ( S , & _rtB ->
OFFDelay_ic , & _rtDW -> OFFDelay_ic ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtDW -> u4_PreviousInput_g = _rtB -> B_1036_315_0_l ;
} DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_c , & _rtDW -> ONDelay_c ) ;
DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_ig , & _rtDW -> OFFDelay_ig )
; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_jz , & _rtDW -> ONDelay_jz )
; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_fp , & _rtDW -> OFFDelay_fp
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
u_PreviousInput = _rtB -> B_1036_2838_0 ; _rtDW -> u_PreviousInput_p = _rtB
-> B_1036_2844_0 ; _rtDW -> u_PreviousInput_k = _rtB -> B_1036_2832_0 ; _rtDW
-> u_PreviousInput_e = _rtB -> B_1036_352_0 ; _rtDW -> u_PreviousInput_pi =
_rtB -> B_1036_353_0 ; _rtDW -> u4_PreviousInput_n = _rtB -> B_1036_371_0 ;
_rtDW -> u1_PreviousInput = _rtB -> B_1036_378_0 ; } DZG_v3_ONDelay_Update (
S , & _rtB -> ONDelay_i , & _rtDW -> ONDelay_i ) ; DZG_v3_OFFDelay_Update ( S
, & _rtB -> OFFDelay_fu , & _rtDW -> OFFDelay_fu ) ; isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> Memory_PreviousInput_kk = _rtB ->
B_1036_378_0 ; _rtDW -> u4_PreviousInput_m = _rtB -> B_1036_386_0_h ; _rtDW
-> Memory_PreviousInput_pu = _rtB -> B_1036_366_0 ; _rtDW ->
u4_PreviousInput_o = _rtB -> B_1036_929_0 ; } { real_T * * uBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_is . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_is . TUbufferPtrs [
1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_mn . Head
= ( ( _rtDW -> TransportDelay_IWORK_mn . Head < ( _rtDW ->
TransportDelay_IWORK_mn . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_mn . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_mn . Head == _rtDW -> TransportDelay_IWORK_mn . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_mn . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_mn . Tail , & _rtDW -> TransportDelay_IWORK_mn . Head ,
& _rtDW -> TransportDelay_IWORK_mn . Last , simTime - _rtP -> P_356 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_mn . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_mn . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_mn . Head ] = _rtB -> B_1036_452_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gt = _rtB -> B_1036_459_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_g2 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_g2 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ay . Head = ( ( _rtDW -> TransportDelay_IWORK_ay . Head
< ( _rtDW -> TransportDelay_IWORK_ay . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ay . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ay . Head == _rtDW -> TransportDelay_IWORK_ay . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ay . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ay . Tail , & _rtDW -> TransportDelay_IWORK_ay . Head ,
& _rtDW -> TransportDelay_IWORK_ay . Last , simTime - _rtP -> P_361 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ay . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ay . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ay . Head ] = _rtB -> B_1036_460_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ii = _rtB -> B_1036_467_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mv . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mv .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_nq . Head = ( ( _rtDW -> TransportDelay_IWORK_nq . Head
< ( _rtDW -> TransportDelay_IWORK_nq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_nq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_nq . Head == _rtDW -> TransportDelay_IWORK_nq . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_nq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_nq . Tail , & _rtDW -> TransportDelay_IWORK_nq . Head ,
& _rtDW -> TransportDelay_IWORK_nq . Last , simTime - _rtP -> P_366 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_nq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_nq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_nq . Head ] = _rtB -> B_1036_470_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_p = _rtB -> B_1036_477_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_k3 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k3 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ni . Head = ( ( _rtDW -> TransportDelay_IWORK_ni . Head
< ( _rtDW -> TransportDelay_IWORK_ni . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ni . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ni . Head == _rtDW -> TransportDelay_IWORK_ni . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ni . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ni . Tail , & _rtDW -> TransportDelay_IWORK_ni . Head ,
& _rtDW -> TransportDelay_IWORK_ni . Last , simTime - _rtP -> P_371 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ni . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ni . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ni . Head ] = _rtB -> B_1036_478_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dc = _rtB -> B_1036_485_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fs . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fs .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_mo . Head = ( ( _rtDW -> TransportDelay_IWORK_mo . Head
< ( _rtDW -> TransportDelay_IWORK_mo . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_mo . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_mo . Head == _rtDW -> TransportDelay_IWORK_mo . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_mo . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_mo . Tail , & _rtDW -> TransportDelay_IWORK_mo . Head ,
& _rtDW -> TransportDelay_IWORK_mo . Last , simTime - _rtP -> P_376 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_mo . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_mo . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_mo . Head ] = _rtB -> B_1036_488_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_f = _rtB -> B_1036_495_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_d . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_d . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_k2 .
Head = ( ( _rtDW -> TransportDelay_IWORK_k2 . Head < ( _rtDW ->
TransportDelay_IWORK_k2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_k2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_k2 . Head == _rtDW -> TransportDelay_IWORK_k2 . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_k2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_k2 . Tail , & _rtDW -> TransportDelay_IWORK_k2 . Head ,
& _rtDW -> TransportDelay_IWORK_k2 . Last , simTime - _rtP -> P_381 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_k2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_k2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_k2 . Head ] = _rtB -> B_1036_496_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_p5 = _rtB -> B_1036_503_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_p4 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_p4 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_or . Head = ( ( _rtDW -> TransportDelay_IWORK_or . Head
< ( _rtDW -> TransportDelay_IWORK_or . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_or . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_or . Head == _rtDW -> TransportDelay_IWORK_or . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_or . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_or . Tail , & _rtDW -> TransportDelay_IWORK_or . Head ,
& _rtDW -> TransportDelay_IWORK_or . Last , simTime - _rtP -> P_386 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_or . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_or . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_or . Head ] = _rtB -> B_1036_506_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_eu = _rtB -> B_1036_513_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ao . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ao .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_jy . Head = ( ( _rtDW -> TransportDelay_IWORK_jy . Head
< ( _rtDW -> TransportDelay_IWORK_jy . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_jy . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_jy . Head == _rtDW -> TransportDelay_IWORK_jy . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_jy . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jy . Tail , & _rtDW -> TransportDelay_IWORK_jy . Head ,
& _rtDW -> TransportDelay_IWORK_jy . Last , simTime - _rtP -> P_391 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_jy . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_jy . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_jy . Head ] = _rtB -> B_1036_514_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_c = _rtB -> B_1036_521_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oz . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oz .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_il . Head = ( ( _rtDW -> TransportDelay_IWORK_il . Head
< ( _rtDW -> TransportDelay_IWORK_il . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_il . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_il . Head == _rtDW -> TransportDelay_IWORK_il . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_il . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_il . Tail , & _rtDW -> TransportDelay_IWORK_il . Head ,
& _rtDW -> TransportDelay_IWORK_il . Last , simTime - _rtP -> P_396 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_il . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_il . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_il . Head ] = _rtB -> B_1036_524_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jx = _rtB -> B_1036_531_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_od . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_od .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_g2 . Head = ( ( _rtDW -> TransportDelay_IWORK_g2 . Head
< ( _rtDW -> TransportDelay_IWORK_g2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_g2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_g2 . Head == _rtDW -> TransportDelay_IWORK_g2 . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_g2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_g2 . Tail , & _rtDW -> TransportDelay_IWORK_g2 . Head ,
& _rtDW -> TransportDelay_IWORK_g2 . Last , simTime - _rtP -> P_401 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_g2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_g2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_g2 . Head ] = _rtB -> B_1036_532_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_m = _rtB -> B_1036_539_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_e . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_nf .
Head = ( ( _rtDW -> TransportDelay_IWORK_nf . Head < ( _rtDW ->
TransportDelay_IWORK_nf . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_nf . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_nf . Head == _rtDW -> TransportDelay_IWORK_nf . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_nf . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_nf . Tail , & _rtDW -> TransportDelay_IWORK_nf . Head ,
& _rtDW -> TransportDelay_IWORK_nf . Last , simTime - _rtP -> P_406 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_nf . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_nf . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_nf . Head ] = _rtB -> B_1036_542_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fv = _rtB -> B_1036_549_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hz . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hz .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_oo . Head = ( ( _rtDW -> TransportDelay_IWORK_oo . Head
< ( _rtDW -> TransportDelay_IWORK_oo . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_oo . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_oo . Head == _rtDW -> TransportDelay_IWORK_oo . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_oo . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_oo . Tail , & _rtDW -> TransportDelay_IWORK_oo . Head ,
& _rtDW -> TransportDelay_IWORK_oo . Last , simTime - _rtP -> P_411 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_oo . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_oo . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_oo . Head ] = _rtB -> B_1036_550_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_d1 = _rtB -> B_1036_557_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_j . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_j . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_ji .
Head = ( ( _rtDW -> TransportDelay_IWORK_ji . Head < ( _rtDW ->
TransportDelay_IWORK_ji . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ji . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ji . Head == _rtDW -> TransportDelay_IWORK_ji . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ji . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ji . Tail , & _rtDW -> TransportDelay_IWORK_ji . Head ,
& _rtDW -> TransportDelay_IWORK_ji . Last , simTime - _rtP -> P_416 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ji . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ji . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ji . Head ] = _rtB -> B_1036_560_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_l0 = _rtB -> B_1036_567_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_h4 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h4 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_nb . Head = ( ( _rtDW -> TransportDelay_IWORK_nb . Head
< ( _rtDW -> TransportDelay_IWORK_nb . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_nb . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_nb . Head == _rtDW -> TransportDelay_IWORK_nb . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_nb . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_nb . Tail , & _rtDW -> TransportDelay_IWORK_nb . Head ,
& _rtDW -> TransportDelay_IWORK_nb . Last , simTime - _rtP -> P_421 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_nb . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_nb . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_nb . Head ] = _rtB -> B_1036_568_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bfe = _rtB -> B_1036_575_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ax . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ax .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ca . Head = ( ( _rtDW -> TransportDelay_IWORK_ca . Head
< ( _rtDW -> TransportDelay_IWORK_ca . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ca . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ca . Head == _rtDW -> TransportDelay_IWORK_ca . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ca . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ca . Tail , & _rtDW -> TransportDelay_IWORK_ca . Head ,
& _rtDW -> TransportDelay_IWORK_ca . Last , simTime - _rtP -> P_426 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ca . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ca . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ca . Head ] = _rtB -> B_1036_578_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_df = _rtB -> B_1036_585_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_h5 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h5 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pd . Head = ( ( _rtDW -> TransportDelay_IWORK_pd . Head
< ( _rtDW -> TransportDelay_IWORK_pd . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_pd . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pd . Head == _rtDW -> TransportDelay_IWORK_pd . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pd . Tail , & _rtDW -> TransportDelay_IWORK_pd . Head ,
& _rtDW -> TransportDelay_IWORK_pd . Last , simTime - _rtP -> P_431 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pd . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pd . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pd . Head ] = _rtB -> B_1036_586_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_no = _rtB -> B_1036_593_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_pu . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pu .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_a5 . Head = ( ( _rtDW -> TransportDelay_IWORK_a5 . Head
< ( _rtDW -> TransportDelay_IWORK_a5 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_a5 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_a5 . Head == _rtDW -> TransportDelay_IWORK_a5 . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_a5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_a5 . Tail , & _rtDW -> TransportDelay_IWORK_a5 . Head ,
& _rtDW -> TransportDelay_IWORK_a5 . Last , simTime - _rtP -> P_436 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_a5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_a5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_a5 . Head ] = _rtB -> B_1036_596_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_et = _rtB -> B_1036_603_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_n . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_n . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_o2 .
Head = ( ( _rtDW -> TransportDelay_IWORK_o2 . Head < ( _rtDW ->
TransportDelay_IWORK_o2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_o2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_o2 . Head == _rtDW -> TransportDelay_IWORK_o2 . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_o2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_o2 . Tail , & _rtDW -> TransportDelay_IWORK_o2 . Head ,
& _rtDW -> TransportDelay_IWORK_o2 . Last , simTime - _rtP -> P_441 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_o2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_o2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_o2 . Head ] = _rtB -> B_1036_604_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pc = _rtB -> B_1036_611_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ml . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ml .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_c0 . Head = ( ( _rtDW -> TransportDelay_IWORK_c0 . Head
< ( _rtDW -> TransportDelay_IWORK_c0 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_c0 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_c0 . Head == _rtDW -> TransportDelay_IWORK_c0 . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_c0 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_c0 . Tail , & _rtDW -> TransportDelay_IWORK_c0 . Head ,
& _rtDW -> TransportDelay_IWORK_c0 . Last , simTime - _rtP -> P_446 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_c0 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_c0 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_c0 . Head ] = _rtB -> B_1036_614_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_h = _rtB -> B_1036_621_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_om . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_om .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dq . Head = ( ( _rtDW -> TransportDelay_IWORK_dq . Head
< ( _rtDW -> TransportDelay_IWORK_dq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dq . Head == _rtDW -> TransportDelay_IWORK_dq . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dq . Tail , & _rtDW -> TransportDelay_IWORK_dq . Head ,
& _rtDW -> TransportDelay_IWORK_dq . Last , simTime - _rtP -> P_451 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dq . Head ] = _rtB -> B_1036_622_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_h4 = _rtB -> B_1036_629_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_gx . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_gx .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ny . Head = ( ( _rtDW -> TransportDelay_IWORK_ny . Head
< ( _rtDW -> TransportDelay_IWORK_ny . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ny . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ny . Head == _rtDW -> TransportDelay_IWORK_ny . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ny . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ny . Tail , & _rtDW -> TransportDelay_IWORK_ny . Head ,
& _rtDW -> TransportDelay_IWORK_ny . Last , simTime - _rtP -> P_456 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ny . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ny . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ny . Head ] = _rtB -> B_1036_632_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_j3 = _rtB -> B_1036_639_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_k1 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k1 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_gd . Head = ( ( _rtDW -> TransportDelay_IWORK_gd . Head
< ( _rtDW -> TransportDelay_IWORK_gd . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_gd . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_gd . Head == _rtDW -> TransportDelay_IWORK_gd . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_gd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_gd . Tail , & _rtDW -> TransportDelay_IWORK_gd . Head ,
& _rtDW -> TransportDelay_IWORK_gd . Last , simTime - _rtP -> P_461 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_gd . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_gd . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_gd . Head ] = _rtB -> B_1036_640_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ls = _rtB -> B_1036_647_0 ; _rtDW -> u4_PreviousInput_b
= _rtB -> B_1036_682_0_f ; } DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_f3
, & _rtDW -> ONDelay_f3 ) ; DZG_v3_OFFDelay_Update ( S , & _rtB ->
OFFDelay_ek , & _rtDW -> OFFDelay_ek ) ; DZG_v3_ONDelay_Update ( S , & _rtB
-> ONDelay_jx , & _rtDW -> ONDelay_jx ) ; DZG_v3_OFFDelay_Update ( S , & _rtB
-> OFFDelay_o5 , & _rtDW -> OFFDelay_o5 ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtDW -> u4_PreviousInput_k = _rtB -> B_1036_702_0_m
; } DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_jn , & _rtDW -> ONDelay_jn
) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_d4 , & _rtDW ->
OFFDelay_d4 ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_fa , & _rtDW ->
ONDelay_fa ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_nq , & _rtDW
-> OFFDelay_nq ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_kv = _rtB -> B_1036_844_0 ; _rtDW ->
u_PreviousInput_pk = _rtB -> B_1036_2936_0 ; _rtDW -> u_PreviousInput_l =
_rtB -> B_1036_2942_0 ; _rtDW -> u_PreviousInput_l1 = _rtB -> B_1036_2930_0 ;
_rtDW -> u_PreviousInput_i = _rtB -> B_1036_877_0 ; _rtDW ->
u_PreviousInput_a = _rtB -> B_1036_878_0 ; _rtDW -> u4_PreviousInput_gb =
_rtB -> B_1036_896_0 ; _rtDW -> u1_PreviousInput_d = _rtB -> B_1036_903_0_b ;
} DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_hb , & _rtDW -> ONDelay_hb )
; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_a , & _rtDW -> OFFDelay_a )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pt = _rtB -> B_1036_903_0_b ; } DZG_v3_ONDelay_Update (
S , & _rtB -> ONDelay_d , & _rtDW -> ONDelay_d ) ; DZG_v3_OFFDelay_Update ( S
, & _rtB -> OFFDelay_ht , & _rtDW -> OFFDelay_ht ) ; isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> u2_PreviousInput = _rtB ->
B_1036_929_0 ; } DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_n0 , & _rtDW
-> ONDelay_n0 ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_hp , &
_rtDW -> OFFDelay_hp ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_os , &
_rtDW -> ONDelay_os ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_c2 ,
& _rtDW -> OFFDelay_c2 ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_h2 ,
& _rtDW -> ONDelay_h2 ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_l4
, & _rtDW -> OFFDelay_l4 ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_k1
, & _rtDW -> ONDelay_k1 ) ; DZG_v3_OFFDelay_Update ( S , & _rtB ->
OFFDelay_gk , & _rtDW -> OFFDelay_gk ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtDW -> Memory_PreviousInput_j3e = _rtB ->
B_1036_1096_0_k ; _rtDW -> Memory_PreviousInput_od = _rtB -> B_1036_1117_0 ;
_rtDW -> Memory_PreviousInput_a3 = _rtB -> B_1036_1104_0 ; _rtDW ->
ICic_PreviousInput = _rtB -> B_1036_1114_0 ; _rtDW -> u4_PreviousInput_d =
_rtB -> B_1036_1330_0_p ; _rtDW -> Memory_PreviousInput_j1 = _rtB ->
B_1036_1186_0_p ; _rtDW -> u_PreviousInput_n = _rtB -> B_1036_2859_0 ; _rtDW
-> u_PreviousInput_io = _rtB -> B_1036_2865_0 ; _rtDW -> u_PreviousInput_n5 =
_rtB -> B_1036_2853_0 ; _rtDW -> u_PreviousInput_pl = _rtB -> B_1036_1219_0 ;
_rtDW -> u_PreviousInput_d = _rtB -> B_1036_1220_0 ; _rtDW ->
u4_PreviousInput_a = _rtB -> B_1036_1238_0_o ; _rtDW -> u1_PreviousInput_f =
_rtB -> B_1036_1245_0 ; } DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_pu ,
& _rtDW -> ONDelay_pu ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_b ,
& _rtDW -> OFFDelay_b ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtDW -> Memory_PreviousInput_og = _rtB -> B_1036_1245_0 ; _rtDW ->
u_PreviousInput_c = _rtB -> B_1036_2957_0 ; _rtDW -> u_PreviousInput_i4 =
_rtB -> B_1036_2963_0 ; _rtDW -> u_PreviousInput_j = _rtB -> B_1036_2951_0 ;
_rtDW -> u_PreviousInput_h = _rtB -> B_1036_1278_0 ; _rtDW ->
u_PreviousInput_pg = _rtB -> B_1036_1279_0 ; _rtDW -> u4_PreviousInput_c =
_rtB -> B_1036_1297_0 ; _rtDW -> u1_PreviousInput_i = _rtB -> B_1036_1304_0 ;
} DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_ja , & _rtDW -> ONDelay_ja )
; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_m , & _rtDW -> OFFDelay_m )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bd = _rtB -> B_1036_1304_0 ; } DZG_v3_ONDelay_Update ( S
, & _rtB -> ONDelay_fna , & _rtDW -> ONDelay_fna ) ; DZG_v3_OFFDelay_Update (
S , & _rtB -> OFFDelay_kf , & _rtDW -> OFFDelay_kf ) ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> u2_PreviousInput_l = _rtB ->
B_1036_1330_0_p ; } DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_mt , &
_rtDW -> ONDelay_mt ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_ec ,
& _rtDW -> OFFDelay_ec ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtDW -> Memory_PreviousInput_oy = _rtB -> B_1036_1332_0 ; _rtDW ->
Memory_PreviousInput_ku = _rtB -> B_1036_1353_0 ; _rtDW ->
Memory_PreviousInput_by = _rtB -> B_1036_1340_0_g ; _rtDW ->
ICic_PreviousInput_j = _rtB -> B_1036_1350_0 ; _rtDW -> u4_PreviousInput_e =
_rtB -> B_1036_1566_0_k ; _rtDW -> Memory_PreviousInput_bl = _rtB ->
B_1036_1422_0_h ; _rtDW -> u_PreviousInput_h2 = _rtB -> B_1036_2880_0 ; _rtDW
-> u_PreviousInput_ab = _rtB -> B_1036_2886_0 ; _rtDW -> u_PreviousInput_js =
_rtB -> B_1036_2874_0 ; _rtDW -> u_PreviousInput_m = _rtB -> B_1036_1455_0 ;
_rtDW -> u_PreviousInput_b = _rtB -> B_1036_1456_0 ; _rtDW ->
u4_PreviousInput_gl = _rtB -> B_1036_1474_0 ; _rtDW -> u1_PreviousInput_c =
_rtB -> B_1036_1481_0_m ; } DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_ih
, & _rtDW -> ONDelay_ih ) ; DZG_v3_OFFDelay_Update ( S , & _rtB ->
OFFDelay_nd , & _rtDW -> OFFDelay_nd ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtDW -> Memory_PreviousInput_g5 = _rtB ->
B_1036_1481_0_m ; _rtDW -> u_PreviousInput_in = _rtB -> B_1036_2978_0 ; _rtDW
-> u_PreviousInput_kw = _rtB -> B_1036_2984_0 ; _rtDW -> u_PreviousInput_k2 =
_rtB -> B_1036_2972_0 ; _rtDW -> u_PreviousInput_g = _rtB -> B_1036_1514_0 ;
_rtDW -> u_PreviousInput_iw = _rtB -> B_1036_1515_0 ; _rtDW ->
u4_PreviousInput_i = _rtB -> B_1036_1533_0 ; _rtDW -> u1_PreviousInput_b =
_rtB -> B_1036_1540_0_n ; } DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_m ,
& _rtDW -> ONDelay_m ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_er ,
& _rtDW -> OFFDelay_er ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtDW -> Memory_PreviousInput_n3 = _rtB -> B_1036_1540_0_n ; }
DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_ixo , & _rtDW -> ONDelay_ixo )
; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_os , & _rtDW -> OFFDelay_os
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
u2_PreviousInput_g = _rtB -> B_1036_1566_0_k ; } DZG_v3_ONDelay_Update ( S ,
& _rtB -> ONDelay_cs , & _rtDW -> ONDelay_cs ) ; DZG_v3_OFFDelay_Update ( S ,
& _rtB -> OFFDelay_dh , & _rtDW -> OFFDelay_dh ) ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> Memory_PreviousInput_fh = _rtB ->
B_1036_1568_0_h ; _rtDW -> Memory_PreviousInput_foz = _rtB -> B_1036_1589_0_m
; _rtDW -> Memory_PreviousInput_ij = _rtB -> B_1036_1576_0_l ; _rtDW ->
ICic_PreviousInput_n = _rtB -> B_1036_1586_0 ; _rtDW -> u4_PreviousInput_el =
_rtB -> B_1036_2445_0_f ; _rtDW -> Memory_PreviousInput_b0 = _rtB ->
B_1036_2436_0_a ; _rtDW -> Memory_PreviousInput_gc = _rtB -> B_1036_2393_0_o
; _rtDW -> Memory_PreviousInput_op = _rtB -> B_1036_2445_0_f ; _rtDW ->
u4_PreviousInput_ay = _rtB -> B_1036_2475_0_o ; _rtDW ->
Memory_PreviousInput_dm = _rtB -> B_1036_2469_0_g ; _rtDW ->
Memory_PreviousInput_ll = _rtB -> B_1036_2475_0_o ; _rtDW ->
u4_PreviousInput_j = _rtB -> B_1036_2484_0_d ; _rtDW ->
Memory_PreviousInput_m4 = _rtB -> B_1036_2534_0 ; _rtDW ->
Memory_PreviousInput_nn = _rtB -> B_1036_2505_0_k ; _rtDW ->
u4_PreviousInput_p = _rtB -> B_1036_2566_0 ; _rtDW -> Memory_PreviousInput_bx
= _rtB -> B_1036_2529_0_g ; } DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_p
, & _rtDW -> ONDelay_p ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_oq
, & _rtDW -> OFFDelay_oq ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtDW -> u2_PreviousInput_k = _rtB -> B_1036_2566_0 ; }
DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_b , & _rtDW -> ONDelay_b ) ;
DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_p , & _rtDW -> OFFDelay_p ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
u4_PreviousInput_mm = _rtB -> B_1036_2593_0 ; _rtDW ->
Memory_PreviousInput_cp = _rtB -> B_1036_2525_0 ; } DZG_v3_ONDelay_Update ( S
, & _rtB -> ONDelay_f , & _rtDW -> ONDelay_f ) ; DZG_v3_OFFDelay_Update ( S ,
& _rtB -> OFFDelay_e , & _rtDW -> OFFDelay_e ) ; isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> u2_PreviousInput_i = _rtB ->
B_1036_2593_0 ; } DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_e5 , & _rtDW
-> ONDelay_e5 ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_ip , &
_rtDW -> OFFDelay_ip ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtDW -> u4_PreviousInput_h = _rtB -> B_1036_2620_0_c ; _rtDW ->
Memory_PreviousInput_kl = _rtB -> B_1036_2533_0 ; } DZG_v3_ONDelay_Update ( S
, & _rtB -> ONDelay_n , & _rtDW -> ONDelay_n ) ; DZG_v3_OFFDelay_Update ( S ,
& _rtB -> OFFDelay_dc , & _rtDW -> OFFDelay_dc ) ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> u2_PreviousInput_d = _rtB ->
B_1036_2620_0_c ; } DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_j , & _rtDW
-> ONDelay_j ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_n , & _rtDW
-> OFFDelay_n ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_eu , & _rtDW
-> ONDelay_eu ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_i , & _rtDW
-> OFFDelay_i ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_k , & _rtDW
-> ONDelay_k ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_c , & _rtDW
-> OFFDelay_c ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_o , & _rtDW
-> ONDelay_o ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_d , & _rtDW
-> OFFDelay_d ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_h , & _rtDW
-> ONDelay_h ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_g , & _rtDW
-> OFFDelay_g ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_e , & _rtDW
-> ONDelay_e ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_o , & _rtDW
-> OFFDelay_o ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_av = _rtB -> B_1036_2668_0_o ; _rtDW ->
Memory_PreviousInput_dw = _rtB -> B_1036_2689_0 ; _rtDW ->
Memory_PreviousInput_pa = _rtB -> B_1036_2676_0_j ; _rtDW ->
ICic_PreviousInput_g = _rtB -> B_1036_2686_0 ; } DZG_v3_ONDelay_Update ( S ,
& _rtB -> ONDelay_l , & _rtDW -> ONDelay_l ) ; DZG_v3_OFFDelay_Update ( S , &
_rtB -> OFFDelay_f , & _rtDW -> OFFDelay_f ) ; DZG_v3_ONDelay_Update ( S , &
_rtB -> ONDelay , & _rtDW -> ONDelay ) ; DZG_v3_OFFDelay_Update ( S , & _rtB
-> OFFDelay , & _rtDW -> OFFDelay ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtDW -> Memory_PreviousInput_e5 = _rtB -> B_1036_2705_0
; _rtDW -> Memory_PreviousInput_am = _rtB -> B_1036_2726_0 ; _rtDW ->
Memory_PreviousInput_ijz = _rtB -> B_1036_2713_0 ; _rtDW ->
ICic_PreviousInput_b = _rtB -> B_1036_2723_0 ; _rtDW ->
Memory_PreviousInput_gj = _rtB -> B_1036_2649_0 ; _rtDW ->
Memory_PreviousInput_j4 = _rtB -> B_1036_2748_0 ; _rtDW ->
Memory_PreviousInput_ai = _rtB -> B_1036_2735_0 ; _rtDW ->
ICic_PreviousInput_o = _rtB -> B_1036_2745_0 ; _rtDW ->
Memory_PreviousInput_aq = _rtB -> B_1036_2750_0 ; _rtDW ->
u4_PreviousInput_e3 = _rtB -> B_1036_2809_0 ; _rtDW ->
Memory_PreviousInput_pb = _rtB -> B_1036_1233_0 ; _rtDW ->
u4_PreviousInput_gl3 = _rtB -> B_1036_2819_0 ; _rtDW ->
Memory_PreviousInput_pr = _rtB -> B_1036_1469_0 ; _rtDW ->
u_PreviousInput_abb = _rtB -> B_1036_337_0 ; _rtDW -> u_PreviousInput_px =
_rtB -> B_1036_2829_0 ; _rtDW -> u_PreviousInput_ij = _rtB -> B_1036_2830_0 ;
_rtDW -> u_PreviousInput_o = _rtB -> B_1036_2831_0 ; _rtDW ->
u_PreviousInput_ka = _rtB -> B_1036_318_0 ; _rtDW -> u_PreviousInput_ey =
_rtB -> B_1036_2836_0 ; _rtDW -> u_PreviousInput_iu = _rtB -> B_1036_2837_0 ;
_rtDW -> u_PreviousInput_cp = _rtB -> B_1036_330_0 ; _rtDW ->
u_PreviousInput_mw = _rtB -> B_1036_2839_0 ; _rtDW -> u_PreviousInput_dl =
_rtB -> B_1036_2840_0 ; _rtDW -> u_PreviousInput_lp = _rtB -> B_1036_324_0 ;
_rtDW -> u_PreviousInput_bl = _rtB -> B_1036_2842_0 ; _rtDW ->
u_PreviousInput_os = _rtB -> B_1036_2843_0 ; _rtDW -> u_PreviousInput_k4 =
_rtB -> B_1036_333_0 ; _rtDW -> u_PreviousInput_lb = _rtB -> B_1036_2845_0 ;
_rtDW -> u_PreviousInput_e2 = _rtB -> B_1036_2846_0 ; _rtDW ->
u_PreviousInput_hd = _rtB -> B_1036_1204_0 ; _rtDW -> u_PreviousInput_f =
_rtB -> B_1036_2850_0 ; _rtDW -> u_PreviousInput_ob = _rtB -> B_1036_2851_0 ;
_rtDW -> u_PreviousInput_jm = _rtB -> B_1036_2852_0 ; _rtDW ->
u_PreviousInput_lz = _rtB -> B_1036_401_0 ; _rtDW -> u_PreviousInput_fc =
_rtB -> B_1036_2857_0 ; _rtDW -> u_PreviousInput_ii = _rtB -> B_1036_2858_0 ;
_rtDW -> u_PreviousInput_db = _rtB -> B_1036_1197_0 ; _rtDW ->
u_PreviousInput_oe = _rtB -> B_1036_2860_0 ; _rtDW -> u_PreviousInput_f1 =
_rtB -> B_1036_2861_0 ; _rtDW -> u_PreviousInput_dt = _rtB -> B_1036_421_0 ;
_rtDW -> u_PreviousInput_bs = _rtB -> B_1036_2863_0 ; _rtDW ->
u_PreviousInput_pv = _rtB -> B_1036_2864_0 ; _rtDW -> u_PreviousInput_c4 =
_rtB -> B_1036_1200_0 ; _rtDW -> u_PreviousInput_go = _rtB -> B_1036_2866_0 ;
_rtDW -> u_PreviousInput_oc = _rtB -> B_1036_2867_0 ; _rtDW ->
u_PreviousInput_gw = _rtB -> B_1036_1440_0 ; _rtDW -> u_PreviousInput_es =
_rtB -> B_1036_2871_0 ; _rtDW -> u_PreviousInput_jmi = _rtB -> B_1036_2872_0
; _rtDW -> u_PreviousInput_cq = _rtB -> B_1036_2873_0 ; _rtDW ->
u_PreviousInput_gx = _rtB -> B_1036_407_0 ; _rtDW -> u_PreviousInput_bt =
_rtB -> B_1036_2878_0 ; _rtDW -> u_PreviousInput_iv = _rtB -> B_1036_2879_0 ;
_rtDW -> u_PreviousInput_lj = _rtB -> B_1036_1433_0 ; _rtDW ->
u_PreviousInput_pvb = _rtB -> B_1036_2881_0 ; _rtDW -> u_PreviousInput_ba =
_rtB -> B_1036_2882_0 ; _rtDW -> u_PreviousInput_dr = _rtB -> B_1036_427_0 ;
_rtDW -> u_PreviousInput_fh = _rtB -> B_1036_2884_0 ; _rtDW ->
u_PreviousInput_hj = _rtB -> B_1036_2885_0 ; _rtDW -> u_PreviousInput_l1q =
_rtB -> B_1036_1436_0 ; _rtDW -> u_PreviousInput_nm = _rtB -> B_1036_2887_0 ;
_rtDW -> u_PreviousInput_fq = _rtB -> B_1036_2888_0 ; _rtDW ->
Memory_PreviousInput_be = _rtB -> B_1036_2826_0 ; _rtDW ->
u4_PreviousInput_jw = _rtB -> B_1036_2897_0 ; _rtDW ->
Memory_PreviousInput_bde = _rtB -> B_1036_1292_0 ; _rtDW ->
u4_PreviousInput_hz = _rtB -> B_1036_2907_0 ; _rtDW -> u4_PreviousInput_pn =
_rtB -> B_1036_2913_0 ; _rtDW -> Memory_PreviousInput_oh = _rtB ->
B_1036_1528_0 ; _rtDW -> Memory_PreviousInput_f0 = _rtB -> B_1036_891_0 ;
_rtDW -> u_PreviousInput_mh = _rtB -> B_1036_862_0 ; _rtDW ->
u_PreviousInput_na = _rtB -> B_1036_2927_0 ; _rtDW -> u_PreviousInput_mo =
_rtB -> B_1036_2928_0 ; _rtDW -> u_PreviousInput_n2 = _rtB -> B_1036_2929_0 ;
_rtDW -> u_PreviousInput_kn = _rtB -> B_1036_705_0 ; _rtDW ->
u_PreviousInput_de = _rtB -> B_1036_2934_0 ; _rtDW -> u_PreviousInput_lx =
_rtB -> B_1036_2935_0 ; _rtDW -> u_PreviousInput_n4 = _rtB -> B_1036_855_0 ;
_rtDW -> u_PreviousInput_ll = _rtB -> B_1036_2937_0 ; _rtDW ->
u_PreviousInput_hh = _rtB -> B_1036_2938_0 ; _rtDW -> u_PreviousInput_c4c =
_rtB -> B_1036_727_0 ; _rtDW -> u_PreviousInput_m4 = _rtB -> B_1036_2940_0 ;
_rtDW -> u_PreviousInput_ol = _rtB -> B_1036_2941_0 ; _rtDW ->
u_PreviousInput_el = _rtB -> B_1036_858_0 ; _rtDW -> u_PreviousInput_mv =
_rtB -> B_1036_2943_0 ; _rtDW -> u_PreviousInput_pu = _rtB -> B_1036_2944_0 ;
_rtDW -> u_PreviousInput_nap = _rtB -> B_1036_1263_0 ; _rtDW ->
u_PreviousInput_jh = _rtB -> B_1036_2948_0 ; _rtDW -> u_PreviousInput_hb =
_rtB -> B_1036_2949_0 ; _rtDW -> u_PreviousInput_av = _rtB -> B_1036_2950_0 ;
_rtDW -> u_PreviousInput_lx0 = _rtB -> B_1036_711_0 ; _rtDW ->
u_PreviousInput_iim = _rtB -> B_1036_2955_0 ; _rtDW -> u_PreviousInput_hi =
_rtB -> B_1036_2956_0 ; _rtDW -> u_PreviousInput_jg = _rtB -> B_1036_1256_0 ;
_rtDW -> u_PreviousInput_jx = _rtB -> B_1036_2958_0 ; _rtDW ->
u_PreviousInput_bi = _rtB -> B_1036_2959_0 ; _rtDW -> u_PreviousInput_ioi =
_rtB -> B_1036_733_0 ; _rtDW -> u_PreviousInput_ni = _rtB -> B_1036_2961_0 ;
_rtDW -> u_PreviousInput_mho = _rtB -> B_1036_2962_0 ; _rtDW ->
u_PreviousInput_baq = _rtB -> B_1036_1259_0 ; _rtDW -> u_PreviousInput_piy =
_rtB -> B_1036_2964_0 ; _rtDW -> u_PreviousInput_gz = _rtB -> B_1036_2965_0 ;
_rtDW -> u_PreviousInput_be = _rtB -> B_1036_1499_0 ; _rtDW ->
u_PreviousInput_llk = _rtB -> B_1036_2969_0 ; _rtDW -> u_PreviousInput_bm =
_rtB -> B_1036_2970_0 ; _rtDW -> u_PreviousInput_iwj = _rtB -> B_1036_2971_0
; _rtDW -> u_PreviousInput_i0 = _rtB -> B_1036_716_0 ; _rtDW ->
u_PreviousInput_ci = _rtB -> B_1036_2976_0 ; _rtDW -> u_PreviousInput_cj =
_rtB -> B_1036_2977_0 ; _rtDW -> u_PreviousInput_ax = _rtB -> B_1036_1492_0 ;
_rtDW -> u_PreviousInput_kc = _rtB -> B_1036_2979_0 ; _rtDW ->
u_PreviousInput_jn = _rtB -> B_1036_2980_0 ; _rtDW -> u_PreviousInput_lg =
_rtB -> B_1036_738_0 ; _rtDW -> u_PreviousInput_au = _rtB -> B_1036_2982_0 ;
_rtDW -> u_PreviousInput_drc = _rtB -> B_1036_2983_0 ; _rtDW ->
u_PreviousInput_ljv = _rtB -> B_1036_1495_0 ; _rtDW -> u_PreviousInput_oh =
_rtB -> B_1036_2985_0 ; _rtDW -> u_PreviousInput_dh = _rtB -> B_1036_2986_0 ;
_rtDW -> Memory_PreviousInput_cd = _rtB -> B_1036_2924_0 ; _rtDW ->
u4_PreviousInput_eb = _rtB -> B_1036_3286_0 ; _rtDW ->
Memory_PreviousInput_j4j = _rtB -> B_1036_3261_0 ; } DZG_v3_ONDelay_Update (
S , & _rtB -> ONDelay_l5 , & _rtDW -> ONDelay_l5 ) ; DZG_v3_OFFDelay_Update (
S , & _rtB -> OFFDelay_du , & _rtDW -> OFFDelay_du ) ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> u2_PreviousInput_e = _rtB ->
B_1036_3286_0 ; } DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_kl , & _rtDW
-> ONDelay_kl ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_l , & _rtDW
-> OFFDelay_l ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_k4 , & _rtDW
-> ONDelay_k4 ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_cf , &
_rtDW -> OFFDelay_cf ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_jn2 ,
& _rtDW -> ONDelay_jn2 ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_ga
, & _rtDW -> OFFDelay_ga ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_b5
, & _rtDW -> ONDelay_b5 ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_k
, & _rtDW -> OFFDelay_k ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtDW -> Memory_PreviousInput_kub = _rtB -> B_1036_3309_0 ; _rtDW ->
Memory_PreviousInput_bj = _rtB -> B_1036_3330_0 ; _rtDW ->
Memory_PreviousInput_nd = _rtB -> B_1036_3317_0 ; _rtDW ->
ICic_PreviousInput_l = _rtB -> B_1036_3327_0 ; _rtDW ->
Memory_PreviousInput_na = _rtB -> B_1036_3331_0 ; _rtDW ->
u4_PreviousInput_ef = _rtB -> B_1036_3366_0 ; _rtDW ->
Memory_PreviousInput_bu = _rtB -> B_1036_3341_0 ; } DZG_v3_ONDelay_Update ( S
, & _rtB -> ONDelay_hn , & _rtDW -> ONDelay_hn ) ; DZG_v3_OFFDelay_Update ( S
, & _rtB -> OFFDelay_f4 , & _rtDW -> OFFDelay_f4 ) ; isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> u2_PreviousInput_gy = _rtB ->
B_1036_3366_0 ; } DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_nu , & _rtDW
-> ONDelay_nu ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_e4 , &
_rtDW -> OFFDelay_e4 ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_fr , &
_rtDW -> ONDelay_fr ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_mj ,
& _rtDW -> OFFDelay_mj ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_hl ,
& _rtDW -> ONDelay_hl ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_a0
, & _rtDW -> OFFDelay_a0 ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_hs
, & _rtDW -> ONDelay_hs ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_h
, & _rtDW -> OFFDelay_h ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtDW -> Memory_PreviousInput_k0 = _rtB -> B_1036_3389_0 ; _rtDW ->
Memory_PreviousInput_fa = _rtB -> B_1036_3410_0 ; _rtDW ->
Memory_PreviousInput_dt = _rtB -> B_1036_3397_0 ; _rtDW ->
ICic_PreviousInput_a = _rtB -> B_1036_3407_0 ; _rtDW ->
Memory_PreviousInput_kg = _rtB -> B_1036_3411_0 ; _rtDW ->
Memory_PreviousInput_bn = _rtB -> B_1036_3261_0 ; } DZG_v3_ONDelay_Update ( S
, & _rtB -> ONDelay_nt , & _rtDW -> ONDelay_nt ) ; DZG_v3_OFFDelay_Update ( S
, & _rtB -> OFFDelay_pc , & _rtDW -> OFFDelay_pc ) ; DZG_v3_ONDelay_Update (
S , & _rtB -> ONDelay_bz , & _rtDW -> ONDelay_bz ) ; DZG_v3_OFFDelay_Update (
S , & _rtB -> OFFDelay_mm , & _rtDW -> OFFDelay_mm ) ; DZG_v3_ONDelay_Update
( S , & _rtB -> ONDelay_i1 , & _rtDW -> ONDelay_i1 ) ; DZG_v3_OFFDelay_Update
( S , & _rtB -> OFFDelay_b3 , & _rtDW -> OFFDelay_b3 ) ;
DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_fn , & _rtDW -> ONDelay_fn ) ;
DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_hq , & _rtDW -> OFFDelay_hq )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dl = _rtB -> B_1036_3640_0 ; _rtDW ->
Memory_PreviousInput_fub = _rtB -> B_1036_3661_0 ; _rtDW ->
Memory_PreviousInput_m2 = _rtB -> B_1036_3648_0 ; _rtDW ->
ICic_PreviousInput_p = _rtB -> B_1036_3658_0 ; } DZG_v3_ONDelay_Update ( S ,
& _rtB -> ONDelay_ha , & _rtDW -> ONDelay_ha ) ; DZG_v3_OFFDelay_Update ( S ,
& _rtB -> OFFDelay_ai , & _rtDW -> OFFDelay_ai ) ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> Memory_PreviousInput_j2 = _rtB ->
B_1036_3669_0 ; _rtDW -> Memory_PreviousInput_eo = _rtB -> B_1036_3690_0 ;
_rtDW -> Memory_PreviousInput_hh = _rtB -> B_1036_3677_0 ; _rtDW ->
ICic_PreviousInput_g2 = _rtB -> B_1036_3687_0 ; _rtDW ->
Memory_PreviousInput_mdi = _rtB -> B_1036_3626_0 ; _rtDW ->
Memory_PreviousInput_mr = _rtB -> B_1036_3632_0 ; _rtDW ->
Memory_PreviousInput_nb = _rtB -> B_1036_3831_0 ; _rtDW ->
Memory_PreviousInput_i1 = _rtB -> B_1036_3850_0 ; _rtDW ->
Memory_PreviousInput_gx = _rtB -> B_1036_3866_0 ; _rtDW ->
Memory_PreviousInput_iy = _rtB -> B_1036_3820_0 ; _rtDW ->
Memory_PreviousInput_iv = _rtB -> B_1036_3796_0 ; _rtDW ->
Memory_PreviousInput_ea = _rtB -> B_1036_3774_0 ; _rtDW ->
Memory_PreviousInput_e5y = _rtB -> B_1036_3890_0 ; } DZG_v3_ONDelay_Update (
S , & _rtB -> ONDelay_hd , & _rtDW -> ONDelay_hd ) ; DZG_v3_OFFDelay_Update (
S , & _rtB -> OFFDelay_dcf , & _rtDW -> OFFDelay_dcf ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gf = _rtB -> B_1036_3904_0 ; _rtDW ->
Memory_PreviousInput_ou = _rtB -> B_1036_3925_0 ; _rtDW ->
Memory_PreviousInput_bew = _rtB -> B_1036_3912_0 ; _rtDW ->
ICic_PreviousInput_gl = _rtB -> B_1036_3922_0 ; } DZG_v3_ONDelay_Update ( S ,
& _rtB -> ONDelay_ll , & _rtDW -> ONDelay_ll ) ; DZG_v3_OFFDelay_Update ( S ,
& _rtB -> OFFDelay_j , & _rtDW -> OFFDelay_j ) ; DZG_v3_ONDelay_Update ( S ,
& _rtB -> ONDelay_bv , & _rtDW -> ONDelay_bv ) ; DZG_v3_OFFDelay_Update ( S ,
& _rtB -> OFFDelay_ja , & _rtDW -> OFFDelay_ja ) ; DZG_v3_ONDelay_Update ( S
, & _rtB -> ONDelay_ht , & _rtDW -> ONDelay_ht ) ; DZG_v3_OFFDelay_Update ( S
, & _rtB -> OFFDelay_aq , & _rtDW -> OFFDelay_aq ) ; isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> Memory_PreviousInput_jf = _rtB ->
B_1036_3943_0 ; _rtDW -> Memory_PreviousInput_hw = _rtB -> B_1036_3971_0 ;
_rtDW -> Memory_PreviousInput_n3p = _rtB -> B_1036_3958_0 ; _rtDW ->
ICic_PreviousInput_h = _rtB -> B_1036_3968_0 ; _rtDW ->
Memory_PreviousInput_ir = _rtB -> B_1036_3950_0 ; _rtDW ->
Memory_PreviousInput_cf = _rtB -> B_1036_3998_0 ; _rtDW ->
Memory_PreviousInput_ijg = _rtB -> B_1036_3985_0 ; _rtDW ->
ICic_PreviousInput_e = _rtB -> B_1036_3995_0 ; _rtDW ->
Memory_PreviousInput_f3 = _rtB -> B_1036_1092_0 ; _rtDW ->
Memory_PreviousInput_lzl = _rtB -> B_1036_4100_0 ; _rtDW ->
Memory_PreviousInput_l4 = _rtB -> B_1036_388_0_e ; _rtDW ->
Memory_PreviousInput_oa = _rtB -> B_1036_1249_0_e ; _rtDW ->
Memory_PreviousInput_lz3 = _rtB -> B_1036_1485_0_e ; _rtDW ->
Memory_PreviousInput_iys = _rtB -> B_1036_907_0 ; _rtDW ->
Memory_PreviousInput_fp = _rtB -> B_1036_1308_0_k ; _rtDW ->
Memory_PreviousInput_bb = _rtB -> B_1036_1544_0_g ; _rtDW ->
Memory_PreviousInput_fr = false ; _rtDW -> Memory_PreviousInput_pq = false ;
_rtDW -> Memory_PreviousInput_cw = false ; _rtDW -> u4_PreviousInput_hl =
_rtB -> B_1036_4344_0 ; _rtDW -> Memory_PreviousInput_dtg = false ; }
DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_llc , & _rtDW -> ONDelay_llc )
; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_fw , & _rtDW -> OFFDelay_fw
) ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
VariableTransportDelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> VariableTransportDelay_PWORK . TUbufferPtrs [ 1 ] ;
real_T * * xBuffer = ( real_T * * ) & _rtDW -> VariableTransportDelay_PWORK .
TUbufferPtrs [ 2 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
VariableTransportDelay_IWORK . Head = ( ( _rtDW ->
VariableTransportDelay_IWORK . Head < ( _rtDW -> VariableTransportDelay_IWORK
. CircularBufSize - 1 ) ) ? ( _rtDW -> VariableTransportDelay_IWORK . Head +
1 ) : 0 ) ; if ( _rtDW -> VariableTransportDelay_IWORK . Head == _rtDW ->
VariableTransportDelay_IWORK . Tail ) { if ( !
DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
VariableTransportDelay_IWORK . CircularBufSize , & _rtDW ->
VariableTransportDelay_IWORK . Tail , & _rtDW -> VariableTransportDelay_IWORK
. Head , & _rtDW -> VariableTransportDelay_IWORK . Last , simTime - _rtP ->
P_705 , tBuffer , uBuffer , xBuffer , ( boolean_T ) 0 , ( boolean_T ) 1 , &
_rtDW -> VariableTransportDelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus
( S , "vtdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [
_rtDW -> VariableTransportDelay_IWORK . Head ] = simTime ; ( * uBuffer ) [
_rtDW -> VariableTransportDelay_IWORK . Head ] = _rtB -> B_1036_4389_0 ; ( *
xBuffer ) [ _rtDW -> VariableTransportDelay_IWORK . Head ] = ( ( X_DZG_v3_T *
) ssGetContStates ( S ) ) -> VariableTransportDelay_CSTATE ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> VariableTransportDelay_PWORK_k .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
VariableTransportDelay_PWORK_k . TUbufferPtrs [ 1 ] ; real_T * * xBuffer = (
real_T * * ) & _rtDW -> VariableTransportDelay_PWORK_k . TUbufferPtrs [ 2 ] ;
real_T simTime = ssGetT ( S ) ; _rtDW -> VariableTransportDelay_IWORK_g .
Head = ( ( _rtDW -> VariableTransportDelay_IWORK_g . Head < ( _rtDW ->
VariableTransportDelay_IWORK_g . CircularBufSize - 1 ) ) ? ( _rtDW ->
VariableTransportDelay_IWORK_g . Head + 1 ) : 0 ) ; if ( _rtDW ->
VariableTransportDelay_IWORK_g . Head == _rtDW ->
VariableTransportDelay_IWORK_g . Tail ) { if ( !
DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
VariableTransportDelay_IWORK_g . CircularBufSize , & _rtDW ->
VariableTransportDelay_IWORK_g . Tail , & _rtDW ->
VariableTransportDelay_IWORK_g . Head , & _rtDW ->
VariableTransportDelay_IWORK_g . Last , simTime - _rtP -> P_708 , tBuffer ,
uBuffer , xBuffer , ( boolean_T ) 0 , ( boolean_T ) 1 , & _rtDW ->
VariableTransportDelay_IWORK_g . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"vtdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
VariableTransportDelay_IWORK_g . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
VariableTransportDelay_IWORK_g . Head ] = _rtB -> B_1036_4347_0 ; ( * xBuffer
) [ _rtDW -> VariableTransportDelay_IWORK_g . Head ] = ( ( X_DZG_v3_T * )
ssGetContStates ( S ) ) -> VariableTransportDelay_CSTATE_l ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lzly = _rtB -> B_1036_4361_0 ; _rtDW ->
Memory_PreviousInput_ogn = _rtB -> B_1036_4382_0 ; _rtDW ->
Memory_PreviousInput_i0 = _rtB -> B_1036_4369_0 ; _rtDW ->
ICic_PreviousInput_l1 = _rtB -> B_1036_4379_0 ; _rtDW ->
Memory_PreviousInput_k4 = _rtB -> B_1036_4383_0 ; _rtDW -> u2_PreviousInput_n
= _rtB -> B_1036_4344_0 ; _rtDW -> Memory_PreviousInput_d12 = _rtB ->
B_1036_4458_0 ; _rtDW -> Memory_PreviousInput_nds = _rtB -> B_1036_4429_0 ;
_rtDW -> u4_PreviousInput_du = _rtB -> B_1036_4490_0 ; _rtDW ->
Memory_PreviousInput_kt = _rtB -> B_1036_4453_0 ; } DZG_v3_ONDelay_Update ( S
, & _rtB -> ONDelay_g5 , & _rtDW -> ONDelay_g5 ) ; DZG_v3_OFFDelay_Update ( S
, & _rtB -> OFFDelay_df , & _rtDW -> OFFDelay_df ) ; isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> u2_PreviousInput_h = _rtB ->
B_1036_4490_0 ; } DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_bk , & _rtDW
-> ONDelay_bk ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_pr , &
_rtDW -> OFFDelay_pr ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtDW -> u4_PreviousInput_jh = _rtB -> B_1036_4517_0 ; _rtDW ->
Memory_PreviousInput_mk = _rtB -> B_1036_4449_0 ; } DZG_v3_ONDelay_Update ( S
, & _rtB -> ONDelay_mv , & _rtDW -> ONDelay_mv ) ; DZG_v3_OFFDelay_Update ( S
, & _rtB -> OFFDelay_ge , & _rtDW -> OFFDelay_ge ) ; isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> u2_PreviousInput_o = _rtB ->
B_1036_4517_0 ; } DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_a , & _rtDW
-> ONDelay_a ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_hb , & _rtDW
-> OFFDelay_hb ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> u4_PreviousInput_l = _rtB -> B_1036_4544_0 ; _rtDW ->
Memory_PreviousInput_cde = _rtB -> B_1036_4457_0 ; } DZG_v3_ONDelay_Update (
S , & _rtB -> ONDelay_bd , & _rtDW -> ONDelay_bd ) ; DZG_v3_OFFDelay_Update (
S , & _rtB -> OFFDelay_l0 , & _rtDW -> OFFDelay_l0 ) ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> u2_PreviousInput_b = _rtB ->
B_1036_4544_0 ; } DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_j3 , & _rtDW
-> ONDelay_j3 ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_kq , &
_rtDW -> OFFDelay_kq ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_eo , &
_rtDW -> ONDelay_eo ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_pb ,
& _rtDW -> OFFDelay_pb ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_om ,
& _rtDW -> ONDelay_om ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_pj
, & _rtDW -> OFFDelay_pj ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_ny
, & _rtDW -> ONDelay_ny ) ; DZG_v3_OFFDelay_Update ( S , & _rtB ->
OFFDelay_ny , & _rtDW -> OFFDelay_ny ) ; DZG_v3_ONDelay_Update ( S , & _rtB
-> ONDelay_cf , & _rtDW -> ONDelay_cf ) ; DZG_v3_OFFDelay_Update ( S , & _rtB
-> OFFDelay_ob , & _rtDW -> OFFDelay_ob ) ; DZG_v3_ONDelay_Update ( S , &
_rtB -> ONDelay_i3 , & _rtDW -> ONDelay_i3 ) ; DZG_v3_OFFDelay_Update ( S , &
_rtB -> OFFDelay_c5 , & _rtDW -> OFFDelay_c5 ) ; isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> Memory_PreviousInput_m5 = _rtB ->
B_1036_4592_0 ; _rtDW -> Memory_PreviousInput_ad = _rtB -> B_1036_4613_0 ;
_rtDW -> Memory_PreviousInput_bw = _rtB -> B_1036_4600_0 ; _rtDW ->
ICic_PreviousInput_ab = _rtB -> B_1036_4610_0 ; } DZG_v3_ONDelay_Update ( S ,
& _rtB -> ONDelay_l1 , & _rtDW -> ONDelay_l1 ) ; DZG_v3_OFFDelay_Update ( S ,
& _rtB -> OFFDelay_d2 , & _rtDW -> OFFDelay_d2 ) ; DZG_v3_ONDelay_Update ( S
, & _rtB -> ONDelay_bj , & _rtDW -> ONDelay_bj ) ; DZG_v3_OFFDelay_Update ( S
, & _rtB -> OFFDelay_nt , & _rtDW -> OFFDelay_nt ) ; isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> Memory_PreviousInput_ba = _rtB ->
B_1036_4629_0 ; _rtDW -> Memory_PreviousInput_ky = _rtB -> B_1036_4650_0 ;
_rtDW -> Memory_PreviousInput_jb = _rtB -> B_1036_4637_0 ; _rtDW ->
ICic_PreviousInput_nl = _rtB -> B_1036_4647_0 ; _rtDW ->
Memory_PreviousInput_auh = _rtB -> B_1036_4573_0 ; _rtDW ->
Memory_PreviousInput_ije = _rtB -> B_1036_4672_0 ; _rtDW ->
Memory_PreviousInput_gs = _rtB -> B_1036_4659_0 ; _rtDW ->
ICic_PreviousInput_ef = _rtB -> B_1036_4669_0 ; _rtDW ->
Memory_PreviousInput_e4 = _rtB -> B_1036_4674_0 ; _rtDW ->
Memory_PreviousInput_nh = _rtB -> B_1036_4767_0 ; } _rtDW -> Integrator_IWORK
= 0 ; if ( _rtDW -> AutomaticGainControl_MODE ) { { real_T * * uBuffer = (
real_T * * ) & _rtDW -> VariableTransportDelay_PWORK_a . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> VariableTransportDelay_PWORK_a
. TUbufferPtrs [ 1 ] ; real_T * * xBuffer = ( real_T * * ) & _rtDW ->
VariableTransportDelay_PWORK_a . TUbufferPtrs [ 2 ] ; real_T simTime = ssGetT
( S ) ; _rtDW -> VariableTransportDelay_IWORK_b . Head = ( ( _rtDW ->
VariableTransportDelay_IWORK_b . Head < ( _rtDW ->
VariableTransportDelay_IWORK_b . CircularBufSize - 1 ) ) ? ( _rtDW ->
VariableTransportDelay_IWORK_b . Head + 1 ) : 0 ) ; if ( _rtDW ->
VariableTransportDelay_IWORK_b . Head == _rtDW ->
VariableTransportDelay_IWORK_b . Tail ) { if ( !
DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
VariableTransportDelay_IWORK_b . CircularBufSize , & _rtDW ->
VariableTransportDelay_IWORK_b . Tail , & _rtDW ->
VariableTransportDelay_IWORK_b . Head , & _rtDW ->
VariableTransportDelay_IWORK_b . Last , simTime - _rtP -> P_33 , tBuffer ,
uBuffer , xBuffer , ( boolean_T ) 0 , ( boolean_T ) 1 , & _rtDW ->
VariableTransportDelay_IWORK_b . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"vtdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
VariableTransportDelay_IWORK_b . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
VariableTransportDelay_IWORK_b . Head ] = _rtB -> B_944_1_0 ; ( * xBuffer ) [
_rtDW -> VariableTransportDelay_IWORK_b . Head ] = ( ( X_DZG_v3_T * )
ssGetContStates ( S ) ) -> VariableTransportDelay_CSTATE_k ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lsc = _rtB -> B_944_6_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> VariableTransportDelay_PWORK_n . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> VariableTransportDelay_PWORK_n
. TUbufferPtrs [ 1 ] ; real_T * * xBuffer = ( real_T * * ) & _rtDW ->
VariableTransportDelay_PWORK_n . TUbufferPtrs [ 2 ] ; real_T simTime = ssGetT
( S ) ; _rtDW -> VariableTransportDelay_IWORK_c . Head = ( ( _rtDW ->
VariableTransportDelay_IWORK_c . Head < ( _rtDW ->
VariableTransportDelay_IWORK_c . CircularBufSize - 1 ) ) ? ( _rtDW ->
VariableTransportDelay_IWORK_c . Head + 1 ) : 0 ) ; if ( _rtDW ->
VariableTransportDelay_IWORK_c . Head == _rtDW ->
VariableTransportDelay_IWORK_c . Tail ) { if ( !
DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
VariableTransportDelay_IWORK_c . CircularBufSize , & _rtDW ->
VariableTransportDelay_IWORK_c . Tail , & _rtDW ->
VariableTransportDelay_IWORK_c . Head , & _rtDW ->
VariableTransportDelay_IWORK_c . Last , simTime - _rtP -> P_37 , tBuffer ,
uBuffer , xBuffer , ( boolean_T ) 0 , ( boolean_T ) 1 , & _rtDW ->
VariableTransportDelay_IWORK_c . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"vtdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
VariableTransportDelay_IWORK_c . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
VariableTransportDelay_IWORK_c . Head ] = _rtB -> B_944_8_0 ; ( * xBuffer ) [
_rtDW -> VariableTransportDelay_IWORK_c . Head ] = ( ( X_DZG_v3_T * )
ssGetContStates ( S ) ) -> VariableTransportDelay_CSTATE_i ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kz = _rtB -> B_944_13_0 ; } } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> VariableTransportDelay_PWORK_c . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> VariableTransportDelay_PWORK_c
. TUbufferPtrs [ 1 ] ; real_T * * xBuffer = ( real_T * * ) & _rtDW ->
VariableTransportDelay_PWORK_c . TUbufferPtrs [ 2 ] ; real_T simTime = ssGetT
( S ) ; _rtDW -> VariableTransportDelay_IWORK_n . Head = ( ( _rtDW ->
VariableTransportDelay_IWORK_n . Head < ( _rtDW ->
VariableTransportDelay_IWORK_n . CircularBufSize - 1 ) ) ? ( _rtDW ->
VariableTransportDelay_IWORK_n . Head + 1 ) : 0 ) ; if ( _rtDW ->
VariableTransportDelay_IWORK_n . Head == _rtDW ->
VariableTransportDelay_IWORK_n . Tail ) { if ( !
DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
VariableTransportDelay_IWORK_n . CircularBufSize , & _rtDW ->
VariableTransportDelay_IWORK_n . Tail , & _rtDW ->
VariableTransportDelay_IWORK_n . Head , & _rtDW ->
VariableTransportDelay_IWORK_n . Last , simTime - _rtP -> P_733 , tBuffer ,
uBuffer , xBuffer , ( boolean_T ) 0 , ( boolean_T ) 1 , & _rtDW ->
VariableTransportDelay_IWORK_n . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"vtdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
VariableTransportDelay_IWORK_n . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
VariableTransportDelay_IWORK_n . Head ] = _rtB -> B_1036_4736_0 ; ( * xBuffer
) [ _rtDW -> VariableTransportDelay_IWORK_n . Head ] = ( ( X_DZG_v3_T * )
ssGetContStates ( S ) ) -> VariableTransportDelay_CSTATE_p ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_hv = _rtB -> B_1036_4741_0 ; } if ( _rtDW ->
LastMajorTimeA == ( rtInf ) ) { _rtDW -> LastMajorTimeA = ssGetTaskTime ( S ,
0 ) ; _rtDW -> PrevYA = _rtB -> B_1036_4750_0 ; } else if ( _rtDW ->
LastMajorTimeB == ( rtInf ) ) { _rtDW -> LastMajorTimeB = ssGetTaskTime ( S ,
0 ) ; _rtDW -> PrevYB = _rtB -> B_1036_4750_0 ; } else if ( _rtDW ->
LastMajorTimeA < _rtDW -> LastMajorTimeB ) { _rtDW -> LastMajorTimeA =
ssGetTaskTime ( S , 0 ) ; _rtDW -> PrevYA = _rtB -> B_1036_4750_0 ; } else {
_rtDW -> LastMajorTimeB = ssGetTaskTime ( S , 0 ) ; _rtDW -> PrevYB = _rtB ->
B_1036_4750_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_b0 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_b0 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_pg . Head = ( ( _rtDW
-> TransportDelay_IWORK_pg . Head < ( _rtDW -> TransportDelay_IWORK_pg .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_pg . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_pg . Head == _rtDW ->
TransportDelay_IWORK_pg . Tail ) { if ( !
DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW -> TransportDelay_IWORK_pg
. CircularBufSize , & _rtDW -> TransportDelay_IWORK_pg . Tail , & _rtDW ->
TransportDelay_IWORK_pg . Head , & _rtDW -> TransportDelay_IWORK_pg . Last ,
simTime - _rtP -> P_764 , tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 ,
false , & _rtDW -> TransportDelay_IWORK_pg . MaxNewBufSize ) ) {
ssSetErrorStatus ( S , "tdelay memory allocation error" ) ; return ; } } ( *
tBuffer ) [ _rtDW -> TransportDelay_IWORK_pg . Head ] = simTime ; ( * uBuffer
) [ _rtDW -> TransportDelay_IWORK_pg . Head ] = _rtB -> B_1036_4822_0 ; }
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_hc = _rtB -> B_1036_4829_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fo . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fo .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_os . Head = ( ( _rtDW -> TransportDelay_IWORK_os . Head
< ( _rtDW -> TransportDelay_IWORK_os . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_os . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_os . Head == _rtDW -> TransportDelay_IWORK_os . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_os . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_os . Tail , & _rtDW -> TransportDelay_IWORK_os . Head ,
& _rtDW -> TransportDelay_IWORK_os . Last , simTime - _rtP -> P_769 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_os . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_os . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_os . Head ] = _rtB -> B_1036_4830_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_iq = _rtB -> B_1036_4837_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hv . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hv .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_jt . Head = ( ( _rtDW -> TransportDelay_IWORK_jt . Head
< ( _rtDW -> TransportDelay_IWORK_jt . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_jt . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_jt . Head == _rtDW -> TransportDelay_IWORK_jt . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_jt . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jt . Tail , & _rtDW -> TransportDelay_IWORK_jt . Head ,
& _rtDW -> TransportDelay_IWORK_jt . Last , simTime - _rtP -> P_776 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_jt . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_jt . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_jt . Head ] = _rtB -> B_1036_4843_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pi = _rtB -> B_1036_4850_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_eq . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_eq .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_it . Head = ( ( _rtDW -> TransportDelay_IWORK_it . Head
< ( _rtDW -> TransportDelay_IWORK_it . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_it . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_it . Head == _rtDW -> TransportDelay_IWORK_it . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_it . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_it . Tail , & _rtDW -> TransportDelay_IWORK_it . Head ,
& _rtDW -> TransportDelay_IWORK_it . Last , simTime - _rtP -> P_781 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_it . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_it . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_it . Head ] = _rtB -> B_1036_4851_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ox = _rtB -> B_1036_4858_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ny . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ny .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_bj . Head = ( ( _rtDW -> TransportDelay_IWORK_bj . Head
< ( _rtDW -> TransportDelay_IWORK_bj . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_bj . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_bj . Head == _rtDW -> TransportDelay_IWORK_bj . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_bj . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_bj . Tail , & _rtDW -> TransportDelay_IWORK_bj . Head ,
& _rtDW -> TransportDelay_IWORK_bj . Last , simTime - _rtP -> P_788 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_bj . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_bj . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_bj . Head ] = _rtB -> B_1036_4864_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kw = _rtB -> B_1036_4871_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nw . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nw .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ayg . Head = ( ( _rtDW -> TransportDelay_IWORK_ayg .
Head < ( _rtDW -> TransportDelay_IWORK_ayg . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_ayg . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ayg . Head == _rtDW -> TransportDelay_IWORK_ayg . Tail )
{ if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ayg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ayg . Tail , & _rtDW -> TransportDelay_IWORK_ayg . Head
, & _rtDW -> TransportDelay_IWORK_ayg . Last , simTime - _rtP -> P_793 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ayg . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ayg . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ayg . Head ] = _rtB -> B_1036_4872_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_py = _rtB -> B_1036_4879_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_n2 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_n2 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_g1 . Head = ( ( _rtDW -> TransportDelay_IWORK_g1 . Head
< ( _rtDW -> TransportDelay_IWORK_g1 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_g1 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_g1 . Head == _rtDW -> TransportDelay_IWORK_g1 . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_g1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_g1 . Tail , & _rtDW -> TransportDelay_IWORK_g1 . Head ,
& _rtDW -> TransportDelay_IWORK_g1 . Last , simTime - _rtP -> P_915 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_g1 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_g1 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_g1 . Head ] = _rtB -> B_1036_5006_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ep = _rtB -> B_1036_5013_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_c . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_c . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_ak .
Head = ( ( _rtDW -> TransportDelay_IWORK_ak . Head < ( _rtDW ->
TransportDelay_IWORK_ak . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ak . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ak . Head == _rtDW -> TransportDelay_IWORK_ak . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ak . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ak . Tail , & _rtDW -> TransportDelay_IWORK_ak . Head ,
& _rtDW -> TransportDelay_IWORK_ak . Last , simTime - _rtP -> P_920 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ak . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ak . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ak . Head ] = _rtB -> B_1036_5014_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fo = _rtB -> B_1036_5021_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_l3 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_l3 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_h . Head = ( ( _rtDW -> TransportDelay_IWORK_h . Head <
( _rtDW -> TransportDelay_IWORK_h . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_h . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_h . Head == _rtDW -> TransportDelay_IWORK_h . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_h . CircularBufSize , & _rtDW -> TransportDelay_IWORK_h
. Tail , & _rtDW -> TransportDelay_IWORK_h . Head , & _rtDW ->
TransportDelay_IWORK_h . Last , simTime - _rtP -> P_927 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_h .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_h . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_h . Head ] = _rtB ->
B_1036_5027_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_lz = _rtB -> B_1036_5034_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hm . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hm
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ky . Head = ( ( _rtDW -> TransportDelay_IWORK_ky . Head
< ( _rtDW -> TransportDelay_IWORK_ky . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ky . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ky . Head == _rtDW -> TransportDelay_IWORK_ky . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ky . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ky . Tail , & _rtDW -> TransportDelay_IWORK_ky . Head ,
& _rtDW -> TransportDelay_IWORK_ky . Last , simTime - _rtP -> P_932 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ky . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ky . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ky . Head ] = _rtB -> B_1036_5035_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fs = _rtB -> B_1036_5042_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_k0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_cj . Head = ( ( _rtDW -> TransportDelay_IWORK_cj . Head
< ( _rtDW -> TransportDelay_IWORK_cj . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_cj . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_cj . Head == _rtDW -> TransportDelay_IWORK_cj . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_cj . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_cj . Tail , & _rtDW -> TransportDelay_IWORK_cj . Head ,
& _rtDW -> TransportDelay_IWORK_cj . Last , simTime - _rtP -> P_939 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_cj . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_cj . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_cj . Head ] = _rtB -> B_1036_5048_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pp = _rtB -> B_1036_5055_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_gd . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_gd .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_oor . Head = ( ( _rtDW -> TransportDelay_IWORK_oor .
Head < ( _rtDW -> TransportDelay_IWORK_oor . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_oor . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_oor . Head == _rtDW -> TransportDelay_IWORK_oor . Tail )
{ if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_oor . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_oor . Tail , & _rtDW -> TransportDelay_IWORK_oor . Head
, & _rtDW -> TransportDelay_IWORK_oor . Last , simTime - _rtP -> P_944 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_oor . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_oor . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_oor . Head ] = _rtB -> B_1036_5056_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_j0 = _rtB -> B_1036_5063_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_e1 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e1 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ft . Head = ( ( _rtDW -> TransportDelay_IWORK_ft . Head
< ( _rtDW -> TransportDelay_IWORK_ft . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ft . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ft . Head == _rtDW -> TransportDelay_IWORK_ft . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ft . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ft . Tail , & _rtDW -> TransportDelay_IWORK_ft . Head ,
& _rtDW -> TransportDelay_IWORK_ft . Last , simTime - _rtP -> P_1002 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ft . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ft . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ft . Head ] = _rtB -> B_1036_5188_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_at = _rtB -> B_1036_5195_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ap . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ap .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_p1u . Head = ( ( _rtDW -> TransportDelay_IWORK_p1u .
Head < ( _rtDW -> TransportDelay_IWORK_p1u . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_p1u . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_p1u . Head == _rtDW -> TransportDelay_IWORK_p1u . Tail )
{ if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_p1u . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_p1u . Tail , & _rtDW -> TransportDelay_IWORK_p1u . Head
, & _rtDW -> TransportDelay_IWORK_p1u . Last , simTime - _rtP -> P_1007 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_p1u . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_p1u . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_p1u . Head ] = _rtB -> B_1036_5196_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_md = _rtB -> B_1036_5203_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mj . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mj .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_jdy . Head = ( ( _rtDW -> TransportDelay_IWORK_jdy .
Head < ( _rtDW -> TransportDelay_IWORK_jdy . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_jdy . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_jdy . Head == _rtDW -> TransportDelay_IWORK_jdy . Tail )
{ if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_jdy . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jdy . Tail , & _rtDW -> TransportDelay_IWORK_jdy . Head
, & _rtDW -> TransportDelay_IWORK_jdy . Last , simTime - _rtP -> P_1014 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_jdy . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_jdy . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_jdy . Head ] = _rtB -> B_1036_5209_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fn = _rtB -> B_1036_5216_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_kx . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_kx .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_gj . Head = ( ( _rtDW -> TransportDelay_IWORK_gj . Head
< ( _rtDW -> TransportDelay_IWORK_gj . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_gj . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_gj . Head == _rtDW -> TransportDelay_IWORK_gj . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_gj . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_gj . Tail , & _rtDW -> TransportDelay_IWORK_gj . Head ,
& _rtDW -> TransportDelay_IWORK_gj . Last , simTime - _rtP -> P_1019 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_gj . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_gj . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_gj . Head ] = _rtB -> B_1036_5217_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kq = _rtB -> B_1036_5224_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fc . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fc .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_cr . Head = ( ( _rtDW -> TransportDelay_IWORK_cr . Head
< ( _rtDW -> TransportDelay_IWORK_cr . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_cr . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_cr . Head == _rtDW -> TransportDelay_IWORK_cr . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_cr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_cr . Tail , & _rtDW -> TransportDelay_IWORK_cr . Head ,
& _rtDW -> TransportDelay_IWORK_cr . Last , simTime - _rtP -> P_1026 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_cr . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_cr . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_cr . Head ] = _rtB -> B_1036_5230_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_h2 = _rtB -> B_1036_5237_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_o3 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o3 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ka . Head = ( ( _rtDW -> TransportDelay_IWORK_ka . Head
< ( _rtDW -> TransportDelay_IWORK_ka . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ka . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ka . Head == _rtDW -> TransportDelay_IWORK_ka . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ka . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ka . Tail , & _rtDW -> TransportDelay_IWORK_ka . Head ,
& _rtDW -> TransportDelay_IWORK_ka . Last , simTime - _rtP -> P_1031 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ka . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ka . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ka . Head ] = _rtB -> B_1036_5238_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_mp = _rtB -> B_1036_5245_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ff . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ff .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_hw . Head = ( ( _rtDW -> TransportDelay_IWORK_hw . Head
< ( _rtDW -> TransportDelay_IWORK_hw . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_hw . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hw . Head == _rtDW -> TransportDelay_IWORK_hw . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hw . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hw . Tail , & _rtDW -> TransportDelay_IWORK_hw . Head ,
& _rtDW -> TransportDelay_IWORK_hw . Last , simTime - _rtP -> P_1129 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hw . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hw . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hw . Head ] = _rtB -> B_1036_5360_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fd = _rtB -> B_1036_5367_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_be3 . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_be3 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_db . Head = ( ( _rtDW -> TransportDelay_IWORK_db . Head
< ( _rtDW -> TransportDelay_IWORK_db . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_db . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_db . Head == _rtDW -> TransportDelay_IWORK_db . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_db . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_db . Tail , & _rtDW -> TransportDelay_IWORK_db . Head ,
& _rtDW -> TransportDelay_IWORK_db . Last , simTime - _rtP -> P_1134 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_db . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_db . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_db . Head ] = _rtB -> B_1036_5368_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_la = _rtB -> B_1036_5375_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_pn . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pn .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_k5 . Head = ( ( _rtDW -> TransportDelay_IWORK_k5 . Head
< ( _rtDW -> TransportDelay_IWORK_k5 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_k5 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_k5 . Head == _rtDW -> TransportDelay_IWORK_k5 . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_k5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_k5 . Tail , & _rtDW -> TransportDelay_IWORK_k5 . Head ,
& _rtDW -> TransportDelay_IWORK_k5 . Last , simTime - _rtP -> P_1141 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_k5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_k5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_k5 . Head ] = _rtB -> B_1036_5381_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_o5 = _rtB -> B_1036_5388_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cm . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cm .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kat . Head = ( ( _rtDW -> TransportDelay_IWORK_kat .
Head < ( _rtDW -> TransportDelay_IWORK_kat . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_kat . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kat . Head == _rtDW -> TransportDelay_IWORK_kat . Tail )
{ if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kat . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kat . Tail , & _rtDW -> TransportDelay_IWORK_kat . Head
, & _rtDW -> TransportDelay_IWORK_kat . Last , simTime - _rtP -> P_1146 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kat . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kat . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kat . Head ] = _rtB -> B_1036_5389_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ef = _rtB -> B_1036_5396_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_jx . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_jx .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dt . Head = ( ( _rtDW -> TransportDelay_IWORK_dt . Head
< ( _rtDW -> TransportDelay_IWORK_dt . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dt . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dt . Head == _rtDW -> TransportDelay_IWORK_dt . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dt . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dt . Tail , & _rtDW -> TransportDelay_IWORK_dt . Head ,
& _rtDW -> TransportDelay_IWORK_dt . Last , simTime - _rtP -> P_1153 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dt . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dt . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dt . Head ] = _rtB -> B_1036_5402_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_cn = _rtB -> B_1036_5409_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_c1 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_c1 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_bl . Head = ( ( _rtDW -> TransportDelay_IWORK_bl . Head
< ( _rtDW -> TransportDelay_IWORK_bl . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_bl . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_bl . Head == _rtDW -> TransportDelay_IWORK_bl . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_bl . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_bl . Tail , & _rtDW -> TransportDelay_IWORK_bl . Head ,
& _rtDW -> TransportDelay_IWORK_bl . Last , simTime - _rtP -> P_1158 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_bl . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_bl . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_bl . Head ] = _rtB -> B_1036_5410_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_au = _rtB -> B_1036_5417_0 ; _rtDW -> u4_PreviousInput_f
= _rtB -> B_1036_5489_0 ; } DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_be
, & _rtDW -> ONDelay_be ) ; DZG_v3_OFFDelay_Update ( S , & _rtB ->
OFFDelay_br , & _rtDW -> OFFDelay_br ) ; DZG_v3_ONDelay_Update ( S , & _rtB
-> ONDelay_c3 , & _rtDW -> ONDelay_c3 ) ; DZG_v3_OFFDelay_Update ( S , & _rtB
-> OFFDelay_nk , & _rtDW -> OFFDelay_nk ) ; { real_T * * uBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_ak . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ak . TUbufferPtrs [
1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_no . Head
= ( ( _rtDW -> TransportDelay_IWORK_no . Head < ( _rtDW ->
TransportDelay_IWORK_no . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_no . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_no . Head == _rtDW -> TransportDelay_IWORK_no . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_no . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_no . Tail , & _rtDW -> TransportDelay_IWORK_no . Head ,
& _rtDW -> TransportDelay_IWORK_no . Last , simTime - _rtP -> P_1216 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_no . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_no . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_no . Head ] = _rtB -> B_1036_5493_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ho = _rtB -> B_1036_5500_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_c0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_c0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_bm . Head = ( ( _rtDW -> TransportDelay_IWORK_bm . Head
< ( _rtDW -> TransportDelay_IWORK_bm . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_bm . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_bm . Head == _rtDW -> TransportDelay_IWORK_bm . Tail ) {
if ( ! DZG_v3_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_bm . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_bm . Tail , & _rtDW -> TransportDelay_IWORK_bm . Head ,
& _rtDW -> TransportDelay_IWORK_bm . Last , simTime - _rtP -> P_1221 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_bm . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_bm . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_bm . Head ] = _rtB -> B_1036_5501_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fu = _rtB -> B_1036_5508_0 ; _rtDW ->
u4_PreviousInput_gt = _rtB -> B_1036_5540_0 ; } DZG_v3_ONDelay_Update ( S , &
_rtB -> ONDelay_ix , & _rtDW -> ONDelay_ix ) ; DZG_v3_OFFDelay_Update ( S , &
_rtB -> OFFDelay_mb , & _rtDW -> OFFDelay_mb ) ; DZG_v3_ONDelay_Update ( S ,
& _rtB -> ONDelay_g , & _rtDW -> ONDelay_g ) ; DZG_v3_OFFDelay_Update ( S , &
_rtB -> OFFDelay_a2 , & _rtDW -> OFFDelay_a2 ) ; isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> Memory_PreviousInput_mn = _rtB ->
B_1036_5604_0 ; } DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_pn , & _rtDW
-> ONDelay_pn ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_a4 , &
_rtDW -> OFFDelay_a4 ) ; DZG_v3_ONDelay_Update ( S , & _rtB -> ONDelay_hy , &
_rtDW -> ONDelay_hy ) ; DZG_v3_OFFDelay_Update ( S , & _rtB -> OFFDelay_i5 ,
& _rtDW -> OFFDelay_i5 ) ; UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdateTID2 ( SimStruct * S , int_T tid ) { UNUSED_PARAMETER (
tid ) ; }
#define MDL_DERIVATIVES
static void mdlDerivatives ( SimStruct * S ) { boolean_T lsat ; boolean_T
usat ; B_DZG_v3_T * _rtB ; P_DZG_v3_T * _rtP ; X_DZG_v3_T * _rtX ;
XDot_DZG_v3_T * _rtXdot ; DW_DZG_v3_T * _rtDW ; _rtDW = ( ( DW_DZG_v3_T * )
ssGetRootDWork ( S ) ) ; _rtXdot = ( ( XDot_DZG_v3_T * ) ssGetdX ( S ) ) ;
_rtX = ( ( X_DZG_v3_T * ) ssGetContStates ( S ) ) ; _rtP = ( ( P_DZG_v3_T * )
ssGetModelRtp ( S ) ) ; _rtB = ( ( B_DZG_v3_T * ) _ssGetModelBlockIO ( S ) )
; _rtXdot -> integrator_CSTATE = _rtB -> B_1036_4971_0 ; _rtXdot ->
integrator_CSTATE_c = _rtB -> B_1036_4973_0 ; _rtXdot -> integrator_CSTATE_o
= _rtB -> B_1036_4983_0 ; _rtXdot -> integrator_CSTATE_p = _rtB ->
B_1036_4985_0 ; _rtXdot -> integrator_CSTATE_i = _rtB -> B_1036_4995_0 ;
_rtXdot -> integrator_CSTATE_e = _rtB -> B_1036_4997_0 ; _rtXdot ->
integrator_CSTATE_f = _rtB -> B_1036_4951_0 ; _rtXdot -> integrator_CSTATE_g
= _rtB -> B_1036_4953_0 ; _rtXdot -> integrator_CSTATE_k = _rtB ->
B_1036_4955_0 ; _rtXdot -> integrator_CSTATE_e3 = _rtB -> B_1036_4957_0 ;
_rtXdot -> integrator_CSTATE_a = _rtB -> B_1036_4959_0 ; _rtXdot ->
integrator_CSTATE_gp = _rtB -> B_1036_4961_0 ; _rtXdot ->
integrator_CSTATE_ac = _rtB -> B_1036_4963_0 ; _rtXdot -> integrator_CSTATE_b
= _rtB -> B_1036_4965_0 ; _rtXdot -> integrator_CSTATE_n = _rtB ->
B_1036_4979_0 ; _rtXdot -> integrator_CSTATE_l = _rtB -> B_1036_4981_0 ;
_rtXdot -> integrator_CSTATE_m = _rtB -> B_1036_4991_0 ; _rtXdot ->
integrator_CSTATE_po = _rtB -> B_1036_4993_0 ; _rtXdot ->
integrator_CSTATE_ch = _rtB -> B_1036_5003_0 ; _rtXdot ->
integrator_CSTATE_l0 = _rtB -> B_1036_5005_0 ; _rtXdot ->
integrator_CSTATE_pf = _rtB -> B_1036_4967_0 ; _rtXdot ->
integrator_CSTATE_ly = _rtB -> B_1036_4969_0 ; _rtXdot ->
integrator_CSTATE_ah = _rtB -> B_1036_4975_0 ; _rtXdot -> integrator_CSTATE_d
= _rtB -> B_1036_4977_0 ; _rtXdot -> integrator_CSTATE_ff = _rtB ->
B_1036_4987_0 ; _rtXdot -> integrator_CSTATE_kc = _rtB -> B_1036_4989_0 ;
_rtXdot -> integrator_CSTATE_pt = _rtB -> B_1036_4999_0 ; _rtXdot ->
integrator_CSTATE_d1 = _rtB -> B_1036_5001_0 ; _rtXdot ->
integrator_CSTATE_gv = _rtB -> B_1036_5337_0 ; _rtXdot ->
integrator_CSTATE_d3 = _rtB -> B_1036_5339_0 ; _rtXdot ->
integrator_CSTATE_lg = _rtB -> B_1036_5345_0 ; _rtXdot ->
integrator_CSTATE_fo = _rtB -> B_1036_5347_0 ; _rtXdot -> integrator_CSTATE_h
= _rtB -> B_1036_5353_0 ; _rtXdot -> integrator_CSTATE_gi = _rtB ->
B_1036_5355_0 ; _rtXdot -> integrator_CSTATE_df = _rtB -> B_1036_5317_0 ;
_rtXdot -> integrator_CSTATE_cv = _rtB -> B_1036_5319_0 ; _rtXdot ->
integrator_CSTATE_lz = _rtB -> B_1036_5321_0 ; _rtXdot ->
integrator_CSTATE_kk = _rtB -> B_1036_5323_0 ; _rtXdot ->
integrator_CSTATE_gx = _rtB -> B_1036_5325_0 ; _rtXdot ->
integrator_CSTATE_fc = _rtB -> B_1036_5327_0 ; _rtXdot ->
integrator_CSTATE_bg = _rtB -> B_1036_5329_0 ; _rtXdot ->
integrator_CSTATE_g1 = _rtB -> B_1036_5331_0 ; _rtXdot ->
integrator_CSTATE_n3 = _rtB -> B_1036_5341_0 ; _rtXdot ->
integrator_CSTATE_cb = _rtB -> B_1036_5343_0 ; _rtXdot ->
integrator_CSTATE_bx = _rtB -> B_1036_5349_0 ; _rtXdot ->
integrator_CSTATE_di = _rtB -> B_1036_5351_0 ; _rtXdot ->
integrator_CSTATE_o3 = _rtB -> B_1036_5357_0 ; _rtXdot ->
integrator_CSTATE_mn = _rtB -> B_1036_5359_0 ; _rtXdot ->
integrator_CSTATE_bi = _rtB -> B_1036_5333_0 ; _rtXdot -> integrator_CSTATE_j
= _rtB -> B_1036_5335_0 ; { real_T instantDelay ; instantDelay = _rtB ->
B_1036_4356_0 ; if ( instantDelay > _rtP -> P_705 ) { instantDelay = _rtP ->
P_705 ; } if ( instantDelay < 0.0 ) { ( ( XDot_DZG_v3_T * ) ssGetdX ( S ) )
-> VariableTransportDelay_CSTATE = 0 ; } else { ( ( XDot_DZG_v3_T * ) ssGetdX
( S ) ) -> VariableTransportDelay_CSTATE = 1.0 / instantDelay ; } } { real_T
instantDelay ; instantDelay = _rtB -> B_1036_4359_0 ; if ( instantDelay >
_rtP -> P_708 ) { instantDelay = _rtP -> P_708 ; } if ( instantDelay < 0.0 )
{ ( ( XDot_DZG_v3_T * ) ssGetdX ( S ) ) -> VariableTransportDelay_CSTATE_l =
0 ; } else { ( ( XDot_DZG_v3_T * ) ssGetdX ( S ) ) ->
VariableTransportDelay_CSTATE_l = 1.0 / instantDelay ; } } lsat = ( _rtX ->
Integrator_CSTATE <= _rtP -> P_728 ) ; usat = ( _rtX -> Integrator_CSTATE >=
_rtP -> P_727 ) ; if ( ( ( ! lsat ) && ( ! usat ) ) || ( lsat && ( _rtB ->
B_1036_4748_0 > 0.0 ) ) || ( usat && ( _rtB -> B_1036_4748_0 < 0.0 ) ) ) {
_rtXdot -> Integrator_CSTATE = _rtB -> B_1036_4748_0 ; } else { _rtXdot ->
Integrator_CSTATE = 0.0 ; } if ( _rtDW -> AutomaticGainControl_MODE ) { {
real_T instantDelay ; instantDelay = _rtB -> B_944_19_0 ; if ( instantDelay >
_rtP -> P_33 ) { instantDelay = _rtP -> P_33 ; } if ( instantDelay < 0.0 ) {
( ( XDot_DZG_v3_T * ) ssGetdX ( S ) ) -> VariableTransportDelay_CSTATE_k = 0
; } else { ( ( XDot_DZG_v3_T * ) ssGetdX ( S ) ) ->
VariableTransportDelay_CSTATE_k = 1.0 / instantDelay ; } } _rtXdot ->
integrator_CSTATE_kg = _rtB -> B_944_29_0 [ 0 ] ; { real_T instantDelay ;
instantDelay = _rtB -> B_944_21_0 ; if ( instantDelay > _rtP -> P_37 ) {
instantDelay = _rtP -> P_37 ; } if ( instantDelay < 0.0 ) { ( ( XDot_DZG_v3_T
* ) ssGetdX ( S ) ) -> VariableTransportDelay_CSTATE_i = 0 ; } else { ( (
XDot_DZG_v3_T * ) ssGetdX ( S ) ) -> VariableTransportDelay_CSTATE_i = 1.0 /
instantDelay ; } } _rtXdot -> integrator_CSTATE_bi0 = _rtB -> B_944_29_0 [ 1
] ; } else { { real_T * dx ; int_T i ; dx = & ( ( ( XDot_DZG_v3_T * ) ssGetdX
( S ) ) -> VariableTransportDelay_CSTATE_k ) ; for ( i = 0 ; i < 4 ; i ++ ) {
dx [ i ] = 0.0 ; } } } lsat = ( _rtX -> Integrator_CSTATE_l <= _rtP -> P_732
) ; usat = ( _rtX -> Integrator_CSTATE_l >= _rtP -> P_731 ) ; if ( ( ( ! lsat
) && ( ! usat ) ) || ( lsat && ( _rtB -> B_1036_4744_0 > 0.0 ) ) || ( usat &&
( _rtB -> B_1036_4744_0 < 0.0 ) ) ) { _rtXdot -> Integrator_CSTATE_l = _rtB
-> B_1036_4744_0 ; } else { _rtXdot -> Integrator_CSTATE_l = 0.0 ; } { real_T
instantDelay ; instantDelay = _rtB -> B_1036_4769_0 ; if ( instantDelay >
_rtP -> P_733 ) { instantDelay = _rtP -> P_733 ; } if ( instantDelay < 0.0 )
{ ( ( XDot_DZG_v3_T * ) ssGetdX ( S ) ) -> VariableTransportDelay_CSTATE_p =
0 ; } else { ( ( XDot_DZG_v3_T * ) ssGetdX ( S ) ) ->
VariableTransportDelay_CSTATE_p = 1.0 / instantDelay ; } } _rtXdot ->
integrator_CSTATE_cn = _rtB -> B_1036_4776_0 [ 1 ] ; _rtXdot ->
TransferFcn_CSTATE = 0.0 ; _rtXdot -> TransferFcn_CSTATE += _rtP -> P_740 *
_rtX -> TransferFcn_CSTATE ; _rtXdot -> TransferFcn_CSTATE += _rtB ->
B_1036_4745_0 ; _rtXdot -> Integrator_x1_CSTATE = _rtB -> B_1036_4760_0 ;
_rtXdot -> Integrator_x2_CSTATE = _rtB -> B_1036_4762_0 ; _rtXdot ->
integrator_CSTATE_em = _rtB -> B_1036_5084_0 ; _rtXdot ->
integrator_CSTATE_mk = _rtB -> B_1036_5086_0 ; _rtXdot ->
integrator_CSTATE_ey = _rtB -> B_1036_5088_0 ; _rtXdot ->
integrator_CSTATE_dc = _rtB -> B_1036_5090_0 ; _rtXdot ->
integrator_CSTATE_jn = _rtB -> B_1036_5092_0 ; _rtXdot ->
integrator_CSTATE_nv = _rtB -> B_1036_5094_0 ; _rtXdot ->
integrator_CSTATE_oh = _rtB -> B_1036_5107_0 ; _rtXdot ->
integrator_CSTATE_iu = _rtB -> B_1036_5109_0 ; _rtXdot ->
integrator_CSTATE_lt = _rtB -> B_1036_5111_0 ; _rtXdot ->
integrator_CSTATE_a3 = _rtB -> B_1036_5113_0 ; _rtXdot ->
integrator_CSTATE_on = _rtB -> B_1036_5115_0 ; _rtXdot ->
integrator_CSTATE_pn = _rtB -> B_1036_5117_0 ; _rtXdot ->
integrator_CSTATE_jy = _rtB -> B_1036_5438_0 ; _rtXdot ->
integrator_CSTATE_ci = _rtB -> B_1036_5440_0 ; _rtXdot ->
integrator_CSTATE_ni = _rtB -> B_1036_5442_0 ; _rtXdot ->
integrator_CSTATE_lr = _rtB -> B_1036_5444_0 ; _rtXdot ->
integrator_CSTATE_gm = _rtB -> B_1036_5446_0 ; _rtXdot ->
integrator_CSTATE_ptr = _rtB -> B_1036_5448_0 ; _rtXdot ->
integrator_CSTATE_am = _rtB -> B_1036_5461_0 ; _rtXdot ->
integrator_CSTATE_h0 = _rtB -> B_1036_5463_0 ; _rtXdot ->
integrator_CSTATE_in = _rtB -> B_1036_5465_0 ; _rtXdot ->
integrator_CSTATE_p3 = _rtB -> B_1036_5467_0 ; _rtXdot ->
integrator_CSTATE_bz = _rtB -> B_1036_5469_0 ; _rtXdot ->
integrator_CSTATE_mh = _rtB -> B_1036_5471_0 ; _rtXdot ->
integrator_CSTATE_m0 = _rtB -> B_1036_5626_0 ; _rtXdot ->
integrator_CSTATE_eb = _rtB -> B_1036_5628_0 ; } static void
mdlInitializeSizes ( SimStruct * S ) { ssSetChecksumVal ( S , 0 , 202936082U
) ; ssSetChecksumVal ( S , 1 , 3792155030U ) ; ssSetChecksumVal ( S , 2 ,
3805464216U ) ; ssSetChecksumVal ( S , 3 , 2014550459U ) ; { mxArray *
slVerStructMat = NULL ; mxArray * slStrMat = mxCreateString ( "simulink" ) ;
char slVerChar [ 10 ] ; int status = mexCallMATLAB ( 1 , & slVerStructMat , 1
, & slStrMat , "ver" ) ; if ( status == 0 ) { mxArray * slVerMat = mxGetField
( slVerStructMat , 0 , "Version" ) ; if ( slVerMat == NULL ) { status = 1 ; }
else { status = mxGetString ( slVerMat , slVerChar , 10 ) ; } }
mxDestroyArray ( slStrMat ) ; mxDestroyArray ( slVerStructMat ) ; if ( (
status == 1 ) || ( strcmp ( slVerChar , "10.1" ) != 0 ) ) { return ; } }
ssSetOptions ( S , SS_OPTION_EXCEPTION_FREE_CODE ) ; if ( ssGetSizeofDWork (
S ) != sizeof ( DW_DZG_v3_T ) ) { ssSetErrorStatus ( S ,
"Unexpected error: Internal DWork sizes do "
"not match for accelerator mex file." ) ; } if ( ssGetSizeofGlobalBlockIO ( S
) != sizeof ( B_DZG_v3_T ) ) { ssSetErrorStatus ( S ,
"Unexpected error: Internal BlockIO sizes do "
"not match for accelerator mex file." ) ; } { int ssSizeofParams ;
ssGetSizeofParams ( S , & ssSizeofParams ) ; if ( ssSizeofParams != sizeof (
P_DZG_v3_T ) ) { static char msg [ 256 ] ; sprintf ( msg ,
"Unexpected error: Internal Parameters sizes do "
"not match for accelerator mex file." ) ; } } _ssSetModelRtp ( S , ( real_T *
) & DZG_v3_rtDefaultP ) ; rt_InitInfAndNaN ( sizeof ( real_T ) ) ; ( (
P_DZG_v3_T * ) ssGetModelRtp ( S ) ) -> P_41 = rtInf ; ( ( P_DZG_v3_T * )
ssGetModelRtp ( S ) ) -> P_727 = rtInf ; ( ( P_DZG_v3_T * ) ssGetModelRtp ( S
) ) -> P_728 = rtMinusInf ; ( ( P_DZG_v3_T * ) ssGetModelRtp ( S ) ) -> P_731
= rtInf ; ( ( P_DZG_v3_T * ) ssGetModelRtp ( S ) ) -> P_743 = rtInf ; }
static void mdlInitializeSampleTimes ( SimStruct * S ) { slAccRegPrmChangeFcn
( S , mdlOutputsTID2 ) ; } static void mdlTerminate ( SimStruct * S ) { }
#include "simulink.c"
