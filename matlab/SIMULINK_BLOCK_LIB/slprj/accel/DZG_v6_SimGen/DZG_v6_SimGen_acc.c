#include <math.h>
#include "DZG_v6_SimGen_acc.h"
#include "DZG_v6_SimGen_acc_private.h"
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
boolean_T DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( int_T * bufSzPtr ,
int_T * tailPtr , int_T * headPtr , int_T * lastPtr , real_T tMinusDelay ,
real_T * * tBufPtr , real_T * * uBufPtr , real_T * * xBufPtr , boolean_T
isfixedbuf , boolean_T istransportdelay , int_T * maxNewBufSzPtr ) { int_T
testIdx ; int_T tail = * tailPtr ; int_T bufSz = * bufSzPtr ; real_T * tBuf =
* tBufPtr ; real_T * xBuf = ( NULL ) ; int_T numBuffer = 2 ; if (
istransportdelay ) { numBuffer = 3 ; xBuf = * xBufPtr ; } testIdx = ( tail <
( bufSz - 1 ) ) ? ( tail + 1 ) : 0 ; if ( ( tMinusDelay <= tBuf [ testIdx ] )
&& ! isfixedbuf ) { int_T j ; real_T * tempT ; real_T * tempU ; real_T *
tempX = ( NULL ) ; real_T * uBuf = * uBufPtr ; int_T newBufSz = bufSz + 1024
; if ( newBufSz > * maxNewBufSzPtr ) { * maxNewBufSzPtr = newBufSz ; } tempU
= ( real_T * ) utMalloc ( numBuffer * newBufSz * sizeof ( real_T ) ) ; if (
tempU == ( NULL ) ) { return ( false ) ; } tempT = tempU + newBufSz ; if (
istransportdelay ) tempX = tempT + newBufSz ; for ( j = tail ; j < bufSz ; j
++ ) { tempT [ j - tail ] = tBuf [ j ] ; tempU [ j - tail ] = uBuf [ j ] ; if
( istransportdelay ) tempX [ j - tail ] = xBuf [ j ] ; } for ( j = 0 ; j <
tail ; j ++ ) { tempT [ j + bufSz - tail ] = tBuf [ j ] ; tempU [ j + bufSz -
tail ] = uBuf [ j ] ; if ( istransportdelay ) tempX [ j + bufSz - tail ] =
xBuf [ j ] ; } if ( * lastPtr > tail ) { * lastPtr -= tail ; } else { *
lastPtr += ( bufSz - tail ) ; } * tailPtr = 0 ; * headPtr = bufSz ; utFree (
uBuf ) ; * bufSzPtr = newBufSz ; * tBufPtr = tempT ; * uBufPtr = tempU ; if (
istransportdelay ) * xBufPtr = tempX ; } else { * tailPtr = testIdx ; }
return ( true ) ; } real_T DZG_v6_SimGen_acc_rt_TDelayInterpolate ( real_T
tMinusDelay , real_T tStart , real_T * tBuf , real_T * uBuf , int_T bufSz ,
int_T * lastIdx , int_T oldestIdx , int_T newIdx , real_T initOutput ,
boolean_T discrete , boolean_T minorStepAndTAtLastMajorOutput ) { int_T i ;
real_T yout , t1 , t2 , u1 , u2 ; if ( ( newIdx == 0 ) && ( oldestIdx == 0 )
&& ( tMinusDelay > tStart ) ) return initOutput ; if ( tMinusDelay <= tStart
) return initOutput ; if ( ( tMinusDelay <= tBuf [ oldestIdx ] ) ) { if (
discrete ) { return ( uBuf [ oldestIdx ] ) ; } else { int_T tempIdx =
oldestIdx + 1 ; if ( oldestIdx == bufSz - 1 ) tempIdx = 0 ; t1 = tBuf [
oldestIdx ] ; t2 = tBuf [ tempIdx ] ; u1 = uBuf [ oldestIdx ] ; u2 = uBuf [
tempIdx ] ; if ( t2 == t1 ) { if ( tMinusDelay >= t2 ) { yout = u2 ; } else {
yout = u1 ; } } else { real_T f1 = ( t2 - tMinusDelay ) / ( t2 - t1 ) ;
real_T f2 = 1.0 - f1 ; yout = f1 * u1 + f2 * u2 ; } return yout ; } } if (
minorStepAndTAtLastMajorOutput ) { if ( newIdx != 0 ) { if ( * lastIdx ==
newIdx ) { ( * lastIdx ) -- ; } newIdx -- ; } else { if ( * lastIdx == newIdx
) { * lastIdx = bufSz - 1 ; } newIdx = bufSz - 1 ; } } i = * lastIdx ; if (
tBuf [ i ] < tMinusDelay ) { while ( tBuf [ i ] < tMinusDelay ) { if ( i ==
newIdx ) break ; i = ( i < ( bufSz - 1 ) ) ? ( i + 1 ) : 0 ; } } else { while
( tBuf [ i ] >= tMinusDelay ) { i = ( i > 0 ) ? i - 1 : ( bufSz - 1 ) ; } i =
( i < ( bufSz - 1 ) ) ? ( i + 1 ) : 0 ; } * lastIdx = i ; if ( discrete ) {
double tempEps = ( DBL_EPSILON ) * 128.0 ; double localEps = tempEps *
muDoubleScalarAbs ( tBuf [ i ] ) ; if ( tempEps > localEps ) { localEps =
tempEps ; } localEps = localEps / 2.0 ; if ( tMinusDelay >= ( tBuf [ i ] -
localEps ) ) { yout = uBuf [ i ] ; } else { if ( i == 0 ) { yout = uBuf [
bufSz - 1 ] ; } else { yout = uBuf [ i - 1 ] ; } } } else { if ( i == 0 ) {
t1 = tBuf [ bufSz - 1 ] ; u1 = uBuf [ bufSz - 1 ] ; } else { t1 = tBuf [ i -
1 ] ; u1 = uBuf [ i - 1 ] ; } t2 = tBuf [ i ] ; u2 = uBuf [ i ] ; if ( t2 ==
t1 ) { if ( tMinusDelay >= t2 ) { yout = u2 ; } else { yout = u1 ; } } else {
real_T f1 = ( t2 - tMinusDelay ) / ( t2 - t1 ) ; real_T f2 = 1.0 - f1 ; yout
= f1 * u1 + f2 * u2 ; } } return ( yout ) ; } real_T look1_pbinlxpw ( real_T
u0 , const real_T bp0 [ ] , const real_T table [ ] , uint32_T prevIndex [ ] ,
uint32_T maxIndex ) { real_T frac ; uint32_T iRght ; uint32_T iLeft ;
uint32_T bpIdx ; uint32_T found ; if ( u0 <= bp0 [ 0U ] ) { bpIdx = 0U ; frac
= ( u0 - bp0 [ 0U ] ) / ( bp0 [ 1U ] - bp0 [ 0U ] ) ; } else if ( u0 < bp0 [
maxIndex ] ) { bpIdx = prevIndex [ 0U ] ; iLeft = 0U ; iRght = maxIndex ;
found = 0U ; while ( found == 0U ) { if ( u0 < bp0 [ bpIdx ] ) { iRght =
bpIdx - 1U ; bpIdx = ( iRght + iLeft ) >> 1U ; } else if ( u0 < bp0 [ bpIdx +
1U ] ) { found = 1U ; } else { iLeft = bpIdx + 1U ; bpIdx = ( iRght + iLeft )
>> 1U ; } } frac = ( u0 - bp0 [ bpIdx ] ) / ( bp0 [ bpIdx + 1U ] - bp0 [
bpIdx ] ) ; } else { bpIdx = maxIndex - 1U ; frac = ( u0 - bp0 [ maxIndex -
1U ] ) / ( bp0 [ maxIndex ] - bp0 [ maxIndex - 1U ] ) ; } prevIndex [ 0U ] =
bpIdx ; return ( table [ bpIdx + 1U ] - table [ bpIdx ] ) * frac + table [
bpIdx ] ; } void rt_ssGetBlockPath ( SimStruct * S , int_T sysIdx , int_T
blkIdx , char_T * * path ) { _ssGetBlockPath ( S , sysIdx , blkIdx , path ) ;
} void rt_ssSet_slErrMsg ( SimStruct * S , void * diag ) { if ( !
_ssIsErrorStatusAslErrMsg ( S ) ) { _ssSet_slErrMsg ( S , diag ) ; } else {
_ssDiscardDiagnostic ( S , diag ) ; } } void rt_ssReportDiagnosticAsWarning (
SimStruct * S , void * diag ) { _ssReportDiagnosticAsWarning ( S , diag ) ; }
void DZG_v6_SimGen_NegSeqComputation_Init ( SimStruct * S ,
B_NegSeqComputation_DZG_v6_SimGen_T * localB ,
P_NegSeqComputation_DZG_v6_SimGen_T * localP ) { localB -> B_16_2_0 . re =
localP -> P_0 ; localB -> B_16_2_0 . im = 0.0 ; } void
DZG_v6_SimGen_NegSeqComputation_Disable ( SimStruct * S ,
DW_NegSeqComputation_DZG_v6_SimGen_T * localDW ) { localDW ->
NegSeqComputation_MODE = false ; } void DZG_v6_SimGen_NegSeqComputation (
SimStruct * S , real_T rtu_Enable , creal_T rtu_In , creal_T rtu_In_b ,
creal_T rtu_In_e , B_NegSeqComputation_DZG_v6_SimGen_T * localB ,
DW_NegSeqComputation_DZG_v6_SimGen_T * localDW ,
P_NegSeqComputation_DZG_v6_SimGen_T * localP ) { real_T re ; real_T im ;
int32_T isHit ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) &&
( ssIsMajorTimeStep ( S ) != 0 ) ) { if ( rtu_Enable > 0.0 ) { if ( ! localDW
-> NegSeqComputation_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S
) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
NegSeqComputation_MODE = true ; } } else { if ( localDW ->
NegSeqComputation_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
DZG_v6_SimGen_NegSeqComputation_Disable ( S , localDW ) ; } } } if ( localDW
-> NegSeqComputation_MODE ) { re = localP -> P_2 [ 0 ] . re * rtu_In . re -
localP -> P_2 [ 0 ] . im * rtu_In . im ; im = localP -> P_2 [ 0 ] . re *
rtu_In . im + localP -> P_2 [ 0 ] . im * rtu_In . re ; localB -> B_16_0_0 [ 0
] . re = re ; localB -> B_16_0_0 [ 0 ] . im = im ; re = localP -> P_2 [ 1 ] .
re * rtu_In_b . re - localP -> P_2 [ 1 ] . im * rtu_In_b . im ; im = localP
-> P_2 [ 1 ] . re * rtu_In_b . im + localP -> P_2 [ 1 ] . im * rtu_In_b . re
; localB -> B_16_0_0 [ 1 ] . re = re ; localB -> B_16_0_0 [ 1 ] . im = im ;
re = localP -> P_2 [ 2 ] . re * rtu_In_e . re - localP -> P_2 [ 2 ] . im *
rtu_In_e . im ; im = localP -> P_2 [ 2 ] . re * rtu_In_e . im + localP -> P_2
[ 2 ] . im * rtu_In_e . re ; localB -> B_16_0_0 [ 2 ] . re = re ; localB ->
B_16_0_0 [ 2 ] . im = im ; re = localB -> B_16_0_0 [ 0 ] . re ; im = localB
-> B_16_0_0 [ 0 ] . im ; re += localB -> B_16_0_0 [ 1 ] . re ; im += localB
-> B_16_0_0 [ 1 ] . im ; re += localB -> B_16_0_0 [ 2 ] . re ; im += localB
-> B_16_0_0 [ 2 ] . im ; localB -> B_16_1_0 . re = re ; localB -> B_16_1_0 .
im = im ; localB -> B_16_2_0 . re = localP -> P_1 * localB -> B_16_1_0 . re ;
localB -> B_16_2_0 . im = localP -> P_1 * localB -> B_16_1_0 . im ; if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
NegSeqComputation_SubsysRanBC ) ; } } } void
DZG_v6_SimGen_NegSeqComputation_Term ( SimStruct * const S ) { } void
DZG_v6_SimGen_ZeroSeqComputation_Init ( SimStruct * S ,
B_ZeroSeqComputation_DZG_v6_SimGen_T * localB ,
P_ZeroSeqComputation_DZG_v6_SimGen_T * localP ) { localB -> B_18_1_0 . re =
localP -> P_0 ; localB -> B_18_1_0 . im = 0.0 ; } void
DZG_v6_SimGen_ZeroSeqComputation_Disable ( SimStruct * S ,
DW_ZeroSeqComputation_DZG_v6_SimGen_T * localDW ) { localDW ->
ZeroSeqComputation_MODE = false ; } void DZG_v6_SimGen_ZeroSeqComputation (
SimStruct * S , real_T rtu_Enable , creal_T rtu_In , creal_T rtu_In_j ,
creal_T rtu_In_b , B_ZeroSeqComputation_DZG_v6_SimGen_T * localB ,
DW_ZeroSeqComputation_DZG_v6_SimGen_T * localDW ,
P_ZeroSeqComputation_DZG_v6_SimGen_T * localP ) { real_T tmpForInput_idx_0_re
; real_T tmpForInput_idx_0_im ; real_T tmpForInput_idx_1_re ; real_T
tmpForInput_idx_1_im ; real_T tmpForInput_idx_2_re ; real_T
tmpForInput_idx_2_im ; int32_T isHit ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S ) != 0 ) ) { if ( rtu_Enable >
0.0 ) { if ( ! localDW -> ZeroSeqComputation_MODE ) { if ( ssGetTaskTime ( S
, 1 ) != ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S
) ; } localDW -> ZeroSeqComputation_MODE = true ; } } else { if ( localDW ->
ZeroSeqComputation_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S )
; DZG_v6_SimGen_ZeroSeqComputation_Disable ( S , localDW ) ; } } } if (
localDW -> ZeroSeqComputation_MODE ) { tmpForInput_idx_0_re = rtu_In . re ;
tmpForInput_idx_0_im = rtu_In . im ; tmpForInput_idx_1_re = rtu_In_j . re ;
tmpForInput_idx_1_im = rtu_In_j . im ; tmpForInput_idx_2_re = rtu_In_b . re ;
tmpForInput_idx_2_im = rtu_In_b . im ; tmpForInput_idx_0_re +=
tmpForInput_idx_1_re ; tmpForInput_idx_0_im += tmpForInput_idx_1_im ;
tmpForInput_idx_0_re += tmpForInput_idx_2_re ; tmpForInput_idx_0_im +=
tmpForInput_idx_2_im ; localB -> B_18_0_0 . re = tmpForInput_idx_0_re ;
localB -> B_18_0_0 . im = tmpForInput_idx_0_im ; localB -> B_18_1_0 . re =
localP -> P_1 * localB -> B_18_0_0 . re ; localB -> B_18_1_0 . im = localP ->
P_1 * localB -> B_18_0_0 . im ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
srUpdateBC ( localDW -> ZeroSeqComputation_SubsysRanBC ) ; } } } void
DZG_v6_SimGen_ZeroSeqComputation_Term ( SimStruct * const S ) { } void
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer_Init ( SimStruct * S ,
B_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T * localB ,
P_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T * localP ) { localB
-> B_39_1_0 = localP -> P_0 ; } void
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer ( SimStruct * S , real_T
rtu_Enable , real_T rtu_In1 ,
B_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T * localB ,
DW_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T * localDW ,
P_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T * localP ) { if (
rtu_Enable > 0.0 ) { { localB -> B_39_0_0 = localP -> P_4 * rtu_In1 ; } {
localB -> B_39_1_0 = localP -> P_9 * localB -> B_39_0_0 ; } if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
Transferfunctionsfordetailedstabilizer_SubsysRanBC ) ; } } } void
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer_Update ( SimStruct * S ,
real_T rtu_Enable ) { if ( rtu_Enable > 0.0 ) { } } void
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer_Term ( SimStruct * const
S ) { } void rt_invd6x6_snf ( const real_T u [ 36 ] , real_T y [ 36 ] ) {
real_T x [ 36 ] ; int8_T p [ 6 ] ; int8_T ipiv [ 6 ] ; int32_T ipk ; int32_T
pipk ; int32_T jm1 ; int32_T mmj ; int32_T jj ; int32_T jp1j ; int32_T
jpiv_offset ; int32_T Aoffset ; real_T x_0 [ 36 ] ; int32_T ONE ; int32_T ix
; real_T smax ; real_T s ; int32_T ijA ; real_T u_0 ; for ( ix = 0 ; ix < 36
; ix ++ ) { u_0 = u [ ix ] ; y [ ix ] = 0.0 ; x [ ix ] = u_0 ; } for ( ix = 0
; ix < 6 ; ix ++ ) { ipiv [ ix ] = ( int8_T ) ( ix + 1 ) ; } for ( ipk = 0 ;
ipk < 5 ; ipk ++ ) { pipk = ipk + 1 ; jm1 = pipk - 1 ; mmj = 6 - pipk ;
Aoffset = jm1 * 7 ; ONE = 1 ; jj = Aoffset + 1 ; jp1j = jj + 1 ; Aoffset =
mmj + 1 ; for ( ix = 0 ; ix < 36 ; ix ++ ) { u_0 = x [ ix ] ; x_0 [ ix ] =
u_0 ; } ix = jj ; u_0 = x_0 [ jj - 1 ] ; s = muDoubleScalarAbs ( u_0 ) ; smax
= s ; for ( jpiv_offset = 2 ; jpiv_offset <= Aoffset ; jpiv_offset ++ ) { ix
++ ; u_0 = x_0 [ ix - 1 ] ; s = muDoubleScalarAbs ( u_0 ) ; if ( s > smax ) {
ONE = jpiv_offset ; smax = s ; } } jpiv_offset = ONE - 1 ; ONE = jj +
jpiv_offset ; if ( x [ ONE - 1 ] != 0.0 ) { if ( jpiv_offset != 0 ) { Aoffset
= pipk + jpiv_offset ; ipiv [ pipk - 1 ] = ( int8_T ) Aoffset ; jm1 ++ ; ONE
= jm1 + jpiv_offset ; for ( jpiv_offset = 0 ; jpiv_offset < 6 ; jpiv_offset
++ ) { s = x [ jm1 - 1 ] ; x [ jm1 - 1 ] = x [ ONE - 1 ] ; x [ ONE - 1 ] = s
; jm1 += 6 ; ONE += 6 ; } } Aoffset = mmj - 1 ; jm1 = jp1j + Aoffset ; for (
ONE = jp1j ; ONE <= jm1 ; ONE ++ ) { u_0 = x [ ONE - 1 ] ; s = x [ jj - 1 ] ;
s = u_0 / s ; x [ ONE - 1 ] = s ; } } Aoffset = 5 - pipk ; jpiv_offset = jj +
6 ; pipk = jj + 7 ; ONE = pipk - 1 ; for ( pipk = 0 ; pipk <= Aoffset ; pipk
++ ) { s = x [ jpiv_offset - 1 ] ; if ( s != 0.0 ) { s = - s ; ix = jp1j ;
jm1 = ONE + 1 ; jj = mmj + ONE ; for ( ijA = jm1 ; ijA <= jj ; ijA ++ ) { x [
ijA - 1 ] += x [ ix - 1 ] * s ; ix ++ ; } } jpiv_offset += 6 ; ONE += 6 ; } }
for ( ix = 0 ; ix < 6 ; ix ++ ) { p [ ix ] = ( int8_T ) ( ix + 1 ) ; } for (
jpiv_offset = 0 ; jpiv_offset < 5 ; jpiv_offset ++ ) { s = ( real_T )
jpiv_offset + 1.0 ; ipk = ipiv [ ( int32_T ) s - 1 ] ; if ( ipk > ( int32_T )
s ) { pipk = p [ ipk - 1 ] ; p [ ipk - 1 ] = p [ ( int32_T ) s - 1 ] ; p [ (
int32_T ) s - 1 ] = ( int8_T ) pipk ; } } for ( jpiv_offset = 0 ; jpiv_offset
< 6 ; jpiv_offset ++ ) { pipk = jpiv_offset + 1 ; Aoffset = p [ pipk - 1 ] ;
y [ ( pipk + 6 * ( Aoffset - 1 ) ) - 1 ] = 1.0 ; for ( ipk = pipk ; ipk < 7 ;
ipk ++ ) { if ( y [ ( ( Aoffset - 1 ) * 6 + ipk ) - 1 ] != 0.0 ) { jm1 = ipk
+ 1 ; for ( ONE = jm1 ; ONE < 7 ; ONE ++ ) { y [ ( ONE + 6 * ( Aoffset - 1 )
) - 1 ] -= y [ ( ( Aoffset - 1 ) * 6 + ipk ) - 1 ] * x [ ( ( ipk - 1 ) * 6 +
ONE ) - 1 ] ; } } } } for ( ipk = 0 ; ipk < 6 ; ipk ++ ) { pipk = ipk ; pipk
*= 6 ; for ( jpiv_offset = 5 ; jpiv_offset >= 0 ; jpiv_offset -- ) { ix = 6 *
jpiv_offset ; if ( y [ jpiv_offset + pipk ] != 0.0 ) { y [ jpiv_offset + pipk
] /= x [ jpiv_offset + ix ] ; jm1 = jpiv_offset - 1 ; for ( ONE = 0 ; ONE <=
jm1 ; ONE ++ ) { ijA = ONE + 1 ; y [ ( ijA + pipk ) - 1 ] -= x [ ( ijA + ix )
- 1 ] * y [ jpiv_offset + pipk ] ; } } } } } static void mdlOutputs (
SimStruct * S , int_T tid ) { real_T tmp [ 6 ] ; real_T tmp_0 [ 6 ] ; real_T
tmp_1 [ 36 ] ; real_T tmp_2 [ 36 ] ; int32_T i ; int32_T i_0 ; real_T
tmpForInput_idx_2 ; real_T tmpForInput_idx_3 ; real_T tmpForInput_idx_0 ;
real_T tmpForInput_idx_1 ; real_T tmpForInput_idx_0_0 ; int32_T isHit ;
int8_T tmp_3 ; static const int8_T tmp_4 [ 3 ] = { 0 , 4 , 5 } ;
B_DZG_v6_SimGen_T * _rtB ; P_DZG_v6_SimGen_T * _rtP ; X_DZG_v6_SimGen_T *
_rtX ; DW_DZG_v6_SimGen_T * _rtDW ; _rtDW = ( ( DW_DZG_v6_SimGen_T * )
ssGetRootDWork ( S ) ) ; _rtX = ( ( X_DZG_v6_SimGen_T * ) ssGetContStates ( S
) ) ; _rtP = ( ( P_DZG_v6_SimGen_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( (
B_DZG_v6_SimGen_T * ) _ssGetModelBlockIO ( S ) ) ; isHit = ssIsSampleHit ( S
, 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_0_0 = _rtDW ->
Rotorangledthetae_DSTATE ; _rtB -> B_45_1_0 = ssGetTaskTime ( S , 2 ) ; _rtB
-> B_45_2_0 = _rtP -> P_62 * _rtB -> B_45_1_0 ; _rtB -> B_45_3_0 = _rtB ->
B_45_0_0 + _rtB -> B_45_2_0 ; muDoubleScalarSinCos ( _rtB -> B_45_3_0 , &
_rtB -> B_45_4_0 , & _rtB -> B_45_4_1 ) ; for ( i = 0 ; i < 6 ; i ++ ) { _rtB
-> B_45_5_0 [ i ] = _rtDW -> fluxes_DSTATE [ i ] ; } _rtB -> B_45_6_0 = _rtB
-> B_45_13_0_m ; if ( _rtB -> B_45_6_0 > 0.0 ) { _rtB -> B_37_0_0 = _rtB ->
B_37_0_0_o ; _rtB -> B_37_1_0 [ 0 ] = _rtP -> P_44 [ 0 ] * _rtB -> B_45_5_0 [
1 ] ; _rtB -> B_37_1_0 [ 1 ] = _rtP -> P_44 [ 1 ] * _rtB -> B_45_5_0 [ 2 ] ;
_rtB -> B_37_1_0 [ 2 ] = _rtP -> P_44 [ 2 ] * _rtB -> B_45_5_0 [ 3 ] ;
tmpForInput_idx_0 = _rtB -> B_37_1_0 [ 0 ] ; tmpForInput_idx_0 += _rtB ->
B_37_1_0 [ 1 ] ; tmpForInput_idx_0 += _rtB -> B_37_1_0 [ 2 ] ; _rtB ->
B_37_2_0 = tmpForInput_idx_0 ; _rtB -> B_37_3_0 = _rtDW -> Lmd_sat_DSTATE ;
tmpForInput_idx_0 = _rtB -> B_37_3_0 ; _rtB -> B_37_4_0 = 1.0 /
tmpForInput_idx_0 ; tmpForInput_idx_0 = _rtB -> B_37_4_0_m [ 0 ] ;
tmpForInput_idx_1 = _rtB -> B_37_4_0_m [ 1 ] ; tmpForInput_idx_2 = _rtB ->
B_37_4_0 ; tmpForInput_idx_0 += tmpForInput_idx_1 ; tmpForInput_idx_0 +=
tmpForInput_idx_2 ; _rtB -> B_37_5_0 = tmpForInput_idx_0 ; tmpForInput_idx_0
= _rtB -> B_37_5_0 ; _rtB -> B_37_6_0 = 1.0 / tmpForInput_idx_0 ; _rtB ->
B_37_7_0 = _rtB -> B_37_2_0 * _rtB -> B_37_6_0 ; _rtB -> B_37_8_0 =
muDoubleScalarAbs ( _rtB -> B_37_7_0 ) ; _rtB -> B_37_9_0 = look1_pbinlxpw (
_rtB -> B_37_8_0 , _rtP -> P_47 , _rtP -> P_46 , & _rtDW -> m_bpIndex , 9U )
; if ( _rtB -> B_37_9_0 != 0.0 ) { _rtB -> B_34_0_0 = _rtB -> B_37_8_0 / _rtB
-> B_37_9_0 ; _rtB -> B_37_11_0 = _rtB -> B_34_0_0 ; } else { _rtB ->
B_37_11_0 = _rtB -> B_37_3_0_c ; } _rtB -> B_37_12_0 = _rtP -> P_48 * _rtB ->
B_37_11_0 ; if ( _rtB -> B_37_0_0 ) { _rtB -> B_36_0_0 [ 0 ] = _rtP -> P_33 [
0 ] * _rtB -> B_45_5_0 [ 0 ] ; _rtB -> B_36_0_0 [ 1 ] = _rtP -> P_33 [ 1 ] *
_rtB -> B_45_5_0 [ 4 ] ; _rtB -> B_36_0_0 [ 2 ] = _rtP -> P_33 [ 2 ] * _rtB
-> B_45_5_0 [ 5 ] ; _rtB -> B_36_2_0 = _rtDW -> Lmq_sat_DSTATE ;
tmpForInput_idx_0 = _rtB -> B_36_0_0 [ 0 ] ; tmpForInput_idx_0_0 = _rtB ->
B_36_1_0_p [ 0 ] ; tmpForInput_idx_0 += _rtB -> B_36_0_0 [ 1 ] ;
tmpForInput_idx_1 = _rtB -> B_36_1_0_p [ 1 ] ; tmpForInput_idx_0 += _rtB ->
B_36_0_0 [ 2 ] ; tmpForInput_idx_2 = _rtB -> B_36_1_0_p [ 2 ] ; _rtB ->
B_36_1_0 = tmpForInput_idx_0 ; tmpForInput_idx_0 = _rtB -> B_36_2_0 ; _rtB ->
B_36_3_0 = 1.0 / tmpForInput_idx_0 ; tmpForInput_idx_3 = _rtB -> B_36_3_0 ;
tmpForInput_idx_0 = tmpForInput_idx_0_0 ; tmpForInput_idx_0 +=
tmpForInput_idx_1 ; tmpForInput_idx_0 += tmpForInput_idx_2 ;
tmpForInput_idx_0 += tmpForInput_idx_3 ; _rtB -> B_36_4_0 = tmpForInput_idx_0
; tmpForInput_idx_0 = _rtB -> B_36_4_0 ; _rtB -> B_36_5_0 = 1.0 /
tmpForInput_idx_0 ; _rtB -> B_36_6_0 = _rtB -> B_36_1_0 * _rtB -> B_36_5_0 ;
_rtB -> B_36_7_0 = muDoubleScalarAbs ( _rtB -> B_36_6_0 ) ; _rtB -> B_36_8_0
= look1_pbinlxpw ( _rtB -> B_36_7_0 , _rtP -> P_36 , _rtP -> P_35 , & _rtDW
-> m_bpIndex_o , 9U ) ; if ( _rtB -> B_36_8_0 != 0.0 ) { _rtB -> B_35_0_0 =
_rtB -> B_36_7_0 / _rtB -> B_36_8_0 ; _rtB -> B_36_10_0 = _rtB -> B_35_0_0 ;
} else { _rtB -> B_36_10_0 = _rtB -> B_36_0_0_c ; } _rtB -> B_36_11_0 = _rtP
-> P_37 * _rtB -> B_36_10_0 ; srUpdateBC ( _rtDW -> Lmq_sat_SubsysRanBC ) ; }
if ( _rtB -> B_37_1_0_i ) { _rtB -> B_37_14_0 = _rtB -> B_36_11_0 ; } else {
_rtB -> B_37_14_0 = _rtB -> B_37_10_0 ; } memcpy ( & _rtB -> B_37_15_0 [ 0 ]
, & _rtB -> B_37_6_0_m [ 0 ] , 36U * sizeof ( real_T ) ) ; for ( isHit = 0 ;
isHit < 3 ; isHit ++ ) { tmp_3 = tmp_4 [ isHit ] ; _rtB -> B_37_15_0 [ 6 *
tmp_3 ] = _rtB -> B_37_14_0 ; _rtB -> B_37_15_0 [ 6 * tmp_3 + 4 ] = _rtB ->
B_37_14_0 ; _rtB -> B_37_15_0 [ 6 * tmp_3 + 5 ] = _rtB -> B_37_14_0 ; }
memcpy ( & _rtB -> B_37_16_0 [ 0 ] , & _rtB -> B_37_15_0 [ 0 ] , 36U * sizeof
( real_T ) ) ; for ( isHit = 0 ; isHit < 3 ; isHit ++ ) { _rtB -> B_37_16_0 [
6 * ( isHit + 1 ) + 1 ] = _rtB -> B_37_12_0 ; _rtB -> B_37_16_0 [ 6 * ( isHit
+ 1 ) + 2 ] = _rtB -> B_37_12_0 ; _rtB -> B_37_16_0 [ 6 * ( isHit + 1 ) + 3 ]
= _rtB -> B_37_12_0 ; } for ( i = 0 ; i < 36 ; i ++ ) { _rtB -> B_37_17_0 [ i
] = _rtB -> B_37_16_0 [ i ] + _rtB -> B_37_7_0_j [ i ] ; } rt_invd6x6_snf (
_rtB -> B_37_17_0 , _rtB -> B_37_18_0 ) ; memcpy ( & tmp_1 [ 0 ] , & _rtB ->
B_37_8_0_h [ 0 ] , 36U * sizeof ( real_T ) ) ; memcpy ( & tmp_2 [ 0 ] , &
_rtB -> B_37_18_0 [ 0 ] , 36U * sizeof ( real_T ) ) ; for ( isHit = 0 ; isHit
< 6 ; isHit ++ ) { for ( i = 0 ; i < 6 ; i ++ ) { _rtB -> B_37_19_0 [ i + 6 *
isHit ] = 0.0 ; for ( i_0 = 0 ; i_0 < 6 ; i_0 ++ ) { _rtB -> B_37_19_0 [ i +
6 * isHit ] += tmp_1 [ 6 * i_0 + i ] * tmp_2 [ 6 * isHit + i_0 ] ; } } } if (
_rtB -> B_37_2_0_f ) { _rtB -> B_37_20_0 = _rtB -> B_36_5_0 ; } else { _rtB
-> B_37_20_0 = _rtB -> B_37_9_0_c ; } if ( ssIsMajorTimeStep ( S ) != 0 ) {
srUpdateBC ( _rtDW -> Saturation_SubsysRanBC ) ; } } for ( i = 0 ; i < 36 ; i
++ ) { if ( _rtB -> B_45_15_0_p >= _rtP -> P_64 ) { _rtB -> B_45_8_0 [ i ] =
_rtB -> B_37_18_0 [ i ] ; } else { _rtB -> B_45_8_0 [ i ] = _rtB ->
B_45_16_0_l [ i ] ; } tmp_1 [ i ] = _rtB -> B_45_8_0 [ i ] ; } for ( i = 0 ;
i < 6 ; i ++ ) { tmp [ i ] = _rtB -> B_45_5_0 [ i ] ; } for ( i = 0 ; i < 6 ;
i ++ ) { tmp_0 [ i ] = 0.0 ; for ( isHit = 0 ; isHit < 6 ; isHit ++ ) {
tmpForInput_idx_1 = tmp_0 [ i ] ; tmpForInput_idx_1 += tmp_1 [ 6 * isHit + i
] * tmp [ isHit ] ; tmp_0 [ i ] = tmpForInput_idx_1 ; } _rtB -> B_45_9_0 [ i
] = tmp_0 [ i ] ; _rtB -> B_45_10_0 [ i ] = _rtP -> P_65 [ i ] * _rtB ->
B_45_9_0 [ i ] ; } _rtB -> B_45_11_0 = _rtB -> B_45_10_0 [ 0 ] * _rtB ->
B_45_4_1 + _rtB -> B_45_10_0 [ 1 ] * _rtB -> B_45_4_0 ; _rtB -> B_45_12_0 = (
( - _rtB -> B_45_10_0 [ 0 ] - 1.7320508075688772 * _rtB -> B_45_10_0 [ 1 ] )
* _rtB -> B_45_4_1 + ( 1.7320508075688772 * _rtB -> B_45_10_0 [ 0 ] - _rtB ->
B_45_10_0 [ 1 ] ) * _rtB -> B_45_4_0 ) * 0.5 ; _rtB -> B_45_13_0 = ( 0.0 -
_rtB -> B_45_11_0 ) - _rtB -> B_45_12_0 ; _rtB -> B_45_14_0 [ 0 ] = _rtP ->
P_66 * _rtB -> B_45_11_0 ; _rtB -> B_45_14_0 [ 1 ] = _rtP -> P_66 * _rtB ->
B_45_12_0 ; _rtB -> B_45_14_0 [ 2 ] = _rtP -> P_66 * _rtB -> B_45_13_0 ;
isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_15_0 [
0 ] = _rtB -> B_45_14_0 [ 0 ] ; _rtB -> B_45_15_0 [ 1 ] = _rtB -> B_45_14_0 [
1 ] ; } if ( _rtDW -> systemEnable != 0 ) { _rtDW -> lastSin =
muDoubleScalarSin ( _rtP -> P_69 * ssGetTaskTime ( S , 2 ) ) ; _rtDW ->
lastCos = muDoubleScalarCos ( _rtP -> P_69 * ssGetTaskTime ( S , 2 ) ) ;
_rtDW -> systemEnable = 0 ; } _rtB -> B_45_16_0 = ( ( _rtDW -> lastSin * _rtP
-> P_73 + _rtDW -> lastCos * _rtP -> P_72 ) * _rtP -> P_71 + ( _rtDW ->
lastCos * _rtP -> P_73 - _rtDW -> lastSin * _rtP -> P_72 ) * _rtP -> P_70 ) *
_rtP -> P_67 + _rtP -> P_68 ; if ( _rtDW -> systemEnable_m != 0 ) { _rtDW ->
lastSin_m = muDoubleScalarSin ( _rtP -> P_76 * ssGetTaskTime ( S , 2 ) ) ;
_rtDW -> lastCos_f = muDoubleScalarCos ( _rtP -> P_76 * ssGetTaskTime ( S , 2
) ) ; _rtDW -> systemEnable_m = 0 ; } _rtB -> B_45_17_0 = ( ( _rtDW ->
lastSin_m * _rtP -> P_80 + _rtDW -> lastCos_f * _rtP -> P_79 ) * _rtP -> P_78
+ ( _rtDW -> lastCos_f * _rtP -> P_80 - _rtDW -> lastSin_m * _rtP -> P_79 ) *
_rtP -> P_77 ) * _rtP -> P_74 + _rtP -> P_75 ; if ( _rtDW -> systemEnable_e
!= 0 ) { _rtDW -> lastSin_b = muDoubleScalarSin ( _rtP -> P_83 *
ssGetTaskTime ( S , 2 ) ) ; _rtDW -> lastCos_d = muDoubleScalarCos ( _rtP ->
P_83 * ssGetTaskTime ( S , 2 ) ) ; _rtDW -> systemEnable_e = 0 ; } _rtB ->
B_45_18_0 = ( ( _rtDW -> lastSin_b * _rtP -> P_87 + _rtDW -> lastCos_d * _rtP
-> P_86 ) * _rtP -> P_85 + ( _rtDW -> lastCos_d * _rtP -> P_87 - _rtDW ->
lastSin_b * _rtP -> P_86 ) * _rtP -> P_84 ) * _rtP -> P_81 + _rtP -> P_82 ;
ssCallAccelRunBlock ( S , 45 , 19 , SS_CALL_MDL_OUTPUTS ) ; } _rtB ->
B_45_20_0 = _rtX -> integrator_CSTATE ; { real_T * * uBuffer = ( real_T * * )
& _rtDW -> TransportDelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_99 ; _rtB
-> B_45_21_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK . CircularBufSize , &
_rtDW -> TransportDelay_IWORK . Last , _rtDW -> TransportDelay_IWORK . Tail ,
_rtDW -> TransportDelay_IWORK . Head , _rtP -> P_100 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_45_22_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_45_23_0 = _rtP -> P_101 ; } _rtB ->
B_45_24_0_p = ( _rtB -> B_45_22_0 >= _rtB -> B_45_23_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_25_0 = _rtDW
-> Memory_PreviousInput ; } if ( _rtB -> B_45_24_0_p ) { _rtB -> B_1_0_0 =
_rtB -> B_45_20_0 - _rtB -> B_45_21_0 ; _rtB -> B_1_1_0 = _rtP -> P_1 * _rtB
-> B_1_0_0 ; _rtB -> B_45_27_0 = _rtB -> B_1_1_0 ; } else { _rtB -> B_45_27_0
= _rtB -> B_45_25_0 ; } _rtB -> B_45_28_0 = _rtX -> integrator_CSTATE_i ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_d .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_d . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_104 ; _rtB -> B_45_29_0 =
DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> TransportDelay_IWORK_d . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_d . Last , _rtDW -> TransportDelay_IWORK_d . Tail ,
_rtDW -> TransportDelay_IWORK_d . Head , _rtP -> P_105 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_45_30_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_45_31_0 = _rtP -> P_106 ; } _rtB ->
B_45_32_0_a = ( _rtB -> B_45_30_0 >= _rtB -> B_45_31_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_33_0 = _rtDW
-> Memory_PreviousInput_e ; } if ( _rtB -> B_45_32_0_a ) { _rtB -> B_0_0_0 =
_rtB -> B_45_28_0 - _rtB -> B_45_29_0 ; _rtB -> B_0_1_0 = _rtP -> P_0 * _rtB
-> B_0_0_0 ; _rtB -> B_45_35_0 = _rtB -> B_0_1_0 ; } else { _rtB -> B_45_35_0
= _rtB -> B_45_33_0 ; } _rtB -> B_45_36_0 . re = _rtB -> B_45_27_0 ; _rtB ->
B_45_36_0 . im = _rtB -> B_45_35_0 ; _rtB -> B_45_37_0 = muDoubleScalarHypot
( _rtB -> B_45_36_0 . re , _rtB -> B_45_36_0 . im ) ; _rtB -> B_45_37_1 =
muDoubleScalarAtan2 ( _rtB -> B_45_36_0 . im , _rtB -> B_45_36_0 . re ) ;
_rtB -> B_45_38_0 = _rtP -> P_108 * _rtB -> B_45_37_1 ; _rtB -> B_45_39_0 =
_rtX -> integrator_CSTATE_j ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_m . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_m . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_110 ; _rtB
-> B_45_40_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_e . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_e . Last , _rtDW -> TransportDelay_IWORK_e .
Tail , _rtDW -> TransportDelay_IWORK_e . Head , _rtP -> P_111 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_45_41_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_42_0 = _rtP ->
P_112 ; } _rtB -> B_45_43_0_e = ( _rtB -> B_45_41_0 >= _rtB -> B_45_42_0 ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_44_0 =
_rtDW -> Memory_PreviousInput_m ; } if ( _rtB -> B_45_43_0_e ) { _rtB ->
B_3_0_0 = _rtB -> B_45_39_0 - _rtB -> B_45_40_0 ; _rtB -> B_3_1_0 = _rtP ->
P_3 * _rtB -> B_3_0_0 ; _rtB -> B_45_46_0 = _rtB -> B_3_1_0 ; } else { _rtB
-> B_45_46_0 = _rtB -> B_45_44_0 ; } _rtB -> B_45_47_0 = _rtX ->
integrator_CSTATE_l ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_e . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_e . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_115 ; _rtB ->
B_45_48_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_eb . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_eb . Last , _rtDW -> TransportDelay_IWORK_eb .
Tail , _rtDW -> TransportDelay_IWORK_eb . Head , _rtP -> P_116 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_45_49_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_50_0 = _rtP ->
P_117 ; } _rtB -> B_45_51_0 = ( _rtB -> B_45_49_0 >= _rtB -> B_45_50_0 ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_52_0 =
_rtDW -> Memory_PreviousInput_k ; } if ( _rtB -> B_45_51_0 ) { _rtB ->
B_2_0_0 = _rtB -> B_45_47_0 - _rtB -> B_45_48_0 ; _rtB -> B_2_1_0 = _rtP ->
P_2 * _rtB -> B_2_0_0 ; _rtB -> B_45_54_0 = _rtB -> B_2_1_0 ; } else { _rtB
-> B_45_54_0 = _rtB -> B_45_52_0 ; } _rtB -> B_45_55_0 . re = _rtB ->
B_45_46_0 ; _rtB -> B_45_55_0 . im = _rtB -> B_45_54_0 ; _rtB -> B_45_56_0 =
muDoubleScalarHypot ( _rtB -> B_45_55_0 . re , _rtB -> B_45_55_0 . im ) ;
_rtB -> B_45_56_1 = muDoubleScalarAtan2 ( _rtB -> B_45_55_0 . im , _rtB ->
B_45_55_0 . re ) ; _rtB -> B_45_57_0 = _rtP -> P_119 * _rtB -> B_45_56_1 ;
_rtB -> B_45_58_0 = _rtB -> B_45_38_0 - _rtB -> B_45_57_0 ; _rtB -> B_45_59_0
= _rtP -> P_120 * _rtB -> B_45_58_0 ; _rtB -> B_45_60_0 = muDoubleScalarCos (
_rtB -> B_45_59_0 ) ; _rtB -> B_45_61_0 = _rtP -> P_121 * _rtB -> B_45_37_0 ;
_rtB -> B_45_62_0 = _rtP -> P_122 * _rtB -> B_45_56_0 ; _rtB -> B_45_63_0 =
_rtB -> B_45_1_0_c * _rtB -> B_45_60_0 * _rtB -> B_45_61_0 * _rtB ->
B_45_62_0 ; _rtB -> B_45_64_0 = _rtP -> P_123 * _rtB -> B_45_63_0 ; _rtB ->
B_45_65_0 = muDoubleScalarSin ( _rtB -> B_45_59_0 ) ; _rtB -> B_45_66_0 =
_rtB -> B_45_1_0_c * _rtB -> B_45_65_0 * _rtB -> B_45_61_0 * _rtB ->
B_45_62_0 ; _rtB -> B_45_67_0 = _rtP -> P_124 * _rtB -> B_45_66_0 ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
45 , 68 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 45 , 69 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 45 , 70 ,
SS_CALL_MDL_OUTPUTS ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_45_71_0 = _rtP -> P_125 * _rtB -> B_45_19_0 [ 8 ] ; _rtB ->
B_45_72_0 = _rtP -> P_126 * _rtB -> B_45_19_0 [ 9 ] ; _rtB -> B_45_73_0 =
_rtP -> P_127 * _rtB -> B_45_19_0 [ 10 ] ; _rtB -> B_45_74_0 [ 0 ] = _rtP ->
P_128 * _rtB -> B_45_71_0 ; _rtB -> B_45_74_0 [ 1 ] = _rtP -> P_128 * _rtB ->
B_45_72_0 ; _rtB -> B_45_74_0 [ 2 ] = _rtP -> P_128 * _rtB -> B_45_73_0 ;
_rtB -> B_45_75_0 = _rtP -> P_129 * _rtB -> B_45_19_0 [ 29 ] ; _rtB ->
B_45_76_0 = _rtP -> P_130 * _rtB -> B_45_19_0 [ 30 ] ; _rtB -> B_45_77_0 =
_rtP -> P_131 * _rtB -> B_45_19_0 [ 31 ] ; _rtB -> B_45_78_0 [ 0 ] = _rtP ->
P_132 * _rtB -> B_45_75_0 ; _rtB -> B_45_78_0 [ 1 ] = _rtP -> P_132 * _rtB ->
B_45_76_0 ; _rtB -> B_45_78_0 [ 2 ] = _rtP -> P_132 * _rtB -> B_45_77_0 ;
ssCallAccelRunBlock ( S , 45 , 79 , SS_CALL_MDL_OUTPUTS ) ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
45 , 80 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 45 , 81 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_45_82_0 = _rtX -> integrator_CSTATE_m ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_g .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_g . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_134 ; _rtB -> B_45_83_0 =
DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> TransportDelay_IWORK_f . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_f . Last , _rtDW -> TransportDelay_IWORK_f . Tail ,
_rtDW -> TransportDelay_IWORK_f . Head , _rtP -> P_135 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_45_84_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_45_85_0 = _rtP -> P_136 ; } _rtB ->
B_45_86_0_a = ( _rtB -> B_45_84_0 >= _rtB -> B_45_85_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_87_0 = _rtDW
-> Memory_PreviousInput_l ; } if ( _rtB -> B_45_86_0_a ) { _rtB -> B_11_0_0 =
_rtB -> B_45_82_0 - _rtB -> B_45_83_0 ; _rtB -> B_11_1_0 = _rtP -> P_11 *
_rtB -> B_11_0_0 ; _rtB -> B_45_89_0 = _rtB -> B_11_1_0 ; } else { _rtB ->
B_45_89_0 = _rtB -> B_45_87_0 ; } _rtB -> B_45_90_0 = _rtX ->
integrator_CSTATE_g ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_j . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_j . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_139 ; _rtB ->
B_45_91_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_h . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_h . Last , _rtDW -> TransportDelay_IWORK_h .
Tail , _rtDW -> TransportDelay_IWORK_h . Head , _rtP -> P_140 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_45_92_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_93_0 = _rtP ->
P_141 ; } _rtB -> B_45_94_0 = ( _rtB -> B_45_92_0 >= _rtB -> B_45_93_0 ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_95_0 =
_rtDW -> Memory_PreviousInput_c ; } if ( _rtB -> B_45_94_0 ) { _rtB ->
B_10_0_0 = _rtB -> B_45_90_0 - _rtB -> B_45_91_0 ; _rtB -> B_10_1_0 = _rtP ->
P_10 * _rtB -> B_10_0_0 ; _rtB -> B_45_97_0 = _rtB -> B_10_1_0 ; } else {
_rtB -> B_45_97_0 = _rtB -> B_45_95_0 ; } _rtB -> B_45_98_0 . re = _rtB ->
B_45_89_0 ; _rtB -> B_45_98_0 . im = _rtB -> B_45_97_0 ; _rtB -> B_45_99_0 =
muDoubleScalarHypot ( _rtB -> B_45_98_0 . re , _rtB -> B_45_98_0 . im ) ;
_rtB -> B_45_99_1 = muDoubleScalarAtan2 ( _rtB -> B_45_98_0 . im , _rtB ->
B_45_98_0 . re ) ; _rtB -> B_45_100_0 = _rtP -> P_143 * _rtB -> B_45_99_1 ;
_rtB -> B_45_101_0 = _rtP -> P_144 * _rtB -> B_45_100_0 ; tmpForInput_idx_0 =
_rtB -> B_45_101_0 ; tmpForInput_idx_1 = _rtB -> B_45_99_0 ;
tmpForInput_idx_2 = muDoubleScalarCos ( tmpForInput_idx_0 ) ;
tmpForInput_idx_0 = muDoubleScalarSin ( tmpForInput_idx_0 ) ; _rtB ->
B_45_102_0 . re = tmpForInput_idx_1 * tmpForInput_idx_2 ; _rtB -> B_45_102_0
. im = tmpForInput_idx_1 * tmpForInput_idx_0 ; _rtB -> B_45_103_0 = _rtX ->
integrator_CSTATE_a ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_k . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_k . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_146 ; _rtB ->
B_45_104_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_o . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_o . Last , _rtDW -> TransportDelay_IWORK_o .
Tail , _rtDW -> TransportDelay_IWORK_o . Head , _rtP -> P_147 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_45_105_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_106_0 = _rtP
-> P_148 ; } _rtB -> B_45_107_0 = ( _rtB -> B_45_105_0 >= _rtB -> B_45_106_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_45_108_0 = _rtDW -> Memory_PreviousInput_lq ; } if ( _rtB -> B_45_107_0 ) {
_rtB -> B_13_0_0 = _rtB -> B_45_103_0 - _rtB -> B_45_104_0 ; _rtB -> B_13_1_0
= _rtP -> P_13 * _rtB -> B_13_0_0 ; _rtB -> B_45_110_0 = _rtB -> B_13_1_0 ; }
else { _rtB -> B_45_110_0 = _rtB -> B_45_108_0 ; } _rtB -> B_45_111_0 = _rtX
-> integrator_CSTATE_d ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_kj . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_kj . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_151 ; _rtB
-> B_45_112_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 ,
* tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dv . CircularBufSize ,
& _rtDW -> TransportDelay_IWORK_dv . Last , _rtDW -> TransportDelay_IWORK_dv
. Tail , _rtDW -> TransportDelay_IWORK_dv . Head , _rtP -> P_152 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_45_113_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_114_0 = _rtP
-> P_153 ; } _rtB -> B_45_115_0 = ( _rtB -> B_45_113_0 >= _rtB -> B_45_114_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_45_116_0 = _rtDW -> Memory_PreviousInput_cg ; } if ( _rtB -> B_45_115_0 ) {
_rtB -> B_12_0_0 = _rtB -> B_45_111_0 - _rtB -> B_45_112_0 ; _rtB -> B_12_1_0
= _rtP -> P_12 * _rtB -> B_12_0_0 ; _rtB -> B_45_118_0 = _rtB -> B_12_1_0 ; }
else { _rtB -> B_45_118_0 = _rtB -> B_45_116_0 ; } _rtB -> B_45_119_0 . re =
_rtB -> B_45_110_0 ; _rtB -> B_45_119_0 . im = _rtB -> B_45_118_0 ; _rtB ->
B_45_120_0 = muDoubleScalarHypot ( _rtB -> B_45_119_0 . re , _rtB ->
B_45_119_0 . im ) ; _rtB -> B_45_120_1 = muDoubleScalarAtan2 ( _rtB ->
B_45_119_0 . im , _rtB -> B_45_119_0 . re ) ; _rtB -> B_45_121_0 = _rtP ->
P_155 * _rtB -> B_45_120_1 ; _rtB -> B_45_122_0 = _rtP -> P_156 * _rtB ->
B_45_121_0 ; tmpForInput_idx_0 = _rtB -> B_45_122_0 ; tmpForInput_idx_1 =
_rtB -> B_45_120_0 ; tmpForInput_idx_2 = muDoubleScalarCos (
tmpForInput_idx_0 ) ; tmpForInput_idx_0 = muDoubleScalarSin (
tmpForInput_idx_0 ) ; _rtB -> B_45_123_0 . re = tmpForInput_idx_1 *
tmpForInput_idx_2 ; _rtB -> B_45_123_0 . im = tmpForInput_idx_1 *
tmpForInput_idx_0 ; _rtB -> B_45_124_0 = _rtX -> integrator_CSTATE_l2 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ks .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ks . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_158 ; _rtB -> B_45_125_0 =
DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> TransportDelay_IWORK_fq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fq . Last , _rtDW -> TransportDelay_IWORK_fq . Tail ,
_rtDW -> TransportDelay_IWORK_fq . Head , _rtP -> P_159 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_45_126_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_45_127_0 = _rtP -> P_160 ; } _rtB ->
B_45_128_0 = ( _rtB -> B_45_126_0 >= _rtB -> B_45_127_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_129_0 = _rtDW
-> Memory_PreviousInput_n ; } if ( _rtB -> B_45_128_0 ) { _rtB -> B_15_0_0 =
_rtB -> B_45_124_0 - _rtB -> B_45_125_0 ; _rtB -> B_15_1_0 = _rtP -> P_15 *
_rtB -> B_15_0_0 ; _rtB -> B_45_131_0 = _rtB -> B_15_1_0 ; } else { _rtB ->
B_45_131_0 = _rtB -> B_45_129_0 ; } _rtB -> B_45_132_0 = _rtX ->
integrator_CSTATE_if ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_l . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_l . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_163 ; _rtB ->
B_45_133_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_hg . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_hg . Last , _rtDW -> TransportDelay_IWORK_hg .
Tail , _rtDW -> TransportDelay_IWORK_hg . Head , _rtP -> P_164 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_45_134_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_135_0 = _rtP
-> P_165 ; } _rtB -> B_45_136_0 = ( _rtB -> B_45_134_0 >= _rtB -> B_45_135_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_45_137_0 = _rtDW -> Memory_PreviousInput_c0 ; } if ( _rtB -> B_45_136_0 ) {
_rtB -> B_14_0_0 = _rtB -> B_45_132_0 - _rtB -> B_45_133_0 ; _rtB -> B_14_1_0
= _rtP -> P_14 * _rtB -> B_14_0_0 ; _rtB -> B_45_139_0 = _rtB -> B_14_1_0 ; }
else { _rtB -> B_45_139_0 = _rtB -> B_45_137_0 ; } _rtB -> B_45_140_0 . re =
_rtB -> B_45_131_0 ; _rtB -> B_45_140_0 . im = _rtB -> B_45_139_0 ; _rtB ->
B_45_141_0 = muDoubleScalarHypot ( _rtB -> B_45_140_0 . re , _rtB ->
B_45_140_0 . im ) ; _rtB -> B_45_141_1 = muDoubleScalarAtan2 ( _rtB ->
B_45_140_0 . im , _rtB -> B_45_140_0 . re ) ; _rtB -> B_45_142_0 = _rtP ->
P_167 * _rtB -> B_45_141_1 ; _rtB -> B_45_143_0 = _rtP -> P_168 * _rtB ->
B_45_142_0 ; tmpForInput_idx_0 = _rtB -> B_45_143_0 ; tmpForInput_idx_1 =
_rtB -> B_45_141_0 ; tmpForInput_idx_2 = muDoubleScalarCos (
tmpForInput_idx_0 ) ; tmpForInput_idx_0 = muDoubleScalarSin (
tmpForInput_idx_0 ) ; _rtB -> B_45_144_0 . re = tmpForInput_idx_1 *
tmpForInput_idx_2 ; _rtB -> B_45_144_0 . im = tmpForInput_idx_1 *
tmpForInput_idx_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_45_145_0 = _rtB -> B_45_2_0_k ; } DZG_v6_SimGen_NegSeqComputation (
S , _rtB -> B_45_145_0 , _rtB -> B_45_102_0 , _rtB -> B_45_123_0 , _rtB ->
B_45_144_0 , & _rtB -> PosSeqComputation , & _rtDW -> PosSeqComputation , &
_rtP -> PosSeqComputation ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_45_147_0 = _rtB -> B_45_3_0_c ; }
DZG_v6_SimGen_NegSeqComputation ( S , _rtB -> B_45_147_0 , _rtB -> B_45_102_0
, _rtB -> B_45_123_0 , _rtB -> B_45_144_0 , & _rtB -> NegSeqComputation , &
_rtDW -> NegSeqComputation , & _rtP -> NegSeqComputation ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_149_0 = _rtB
-> B_45_4_0_b ; } DZG_v6_SimGen_ZeroSeqComputation ( S , _rtB -> B_45_149_0 ,
_rtB -> B_45_102_0 , _rtB -> B_45_123_0 , _rtB -> B_45_144_0 , & _rtB ->
ZeroSeqComputation , & _rtDW -> ZeroSeqComputation , & _rtP ->
ZeroSeqComputation ) ; _rtB -> B_45_151_0 [ 0 ] = muDoubleScalarHypot ( _rtB
-> PosSeqComputation . B_16_2_0 . re , _rtB -> PosSeqComputation . B_16_2_0 .
im ) ; _rtB -> B_45_151_0 [ 1 ] = muDoubleScalarHypot ( _rtB ->
NegSeqComputation . B_16_2_0 . re , _rtB -> NegSeqComputation . B_16_2_0 . im
) ; _rtB -> B_45_151_0 [ 2 ] = muDoubleScalarHypot ( _rtB ->
ZeroSeqComputation . B_18_1_0 . re , _rtB -> ZeroSeqComputation . B_18_1_0 .
im ) ; _rtB -> B_45_151_1 [ 0 ] = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation . B_16_2_0 . im , _rtB -> PosSeqComputation . B_16_2_0 . re
) ; _rtB -> B_45_151_1 [ 1 ] = muDoubleScalarAtan2 ( _rtB ->
NegSeqComputation . B_16_2_0 . im , _rtB -> NegSeqComputation . B_16_2_0 . re
) ; _rtB -> B_45_151_1 [ 2 ] = muDoubleScalarAtan2 ( _rtB ->
ZeroSeqComputation . B_18_1_0 . im , _rtB -> ZeroSeqComputation . B_18_1_0 .
re ) ; _rtB -> B_45_152_0 = _rtX -> integrator_CSTATE_h ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_170 ; _rtB -> B_45_153_0 =
DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> TransportDelay_IWORK_a . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_a . Last , _rtDW -> TransportDelay_IWORK_a . Tail ,
_rtDW -> TransportDelay_IWORK_a . Head , _rtP -> P_171 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_45_154_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_45_155_0 = _rtP -> P_172 ; } _rtB ->
B_45_156_0 = ( _rtB -> B_45_154_0 >= _rtB -> B_45_155_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_157_0 = _rtDW
-> Memory_PreviousInput_i ; } if ( _rtB -> B_45_156_0 ) { _rtB -> B_20_0_0 =
_rtB -> B_45_152_0 - _rtB -> B_45_153_0 ; _rtB -> B_20_1_0 = _rtP -> P_17 *
_rtB -> B_20_0_0 ; _rtB -> B_45_159_0 = _rtB -> B_20_1_0 ; } else { _rtB ->
B_45_159_0 = _rtB -> B_45_157_0 ; } _rtB -> B_45_160_0 = _rtX ->
integrator_CSTATE_b ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_a . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_a . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_175 ; _rtB ->
B_45_161_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_n . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_n . Last , _rtDW -> TransportDelay_IWORK_n .
Tail , _rtDW -> TransportDelay_IWORK_n . Head , _rtP -> P_176 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_45_162_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_163_0 = _rtP
-> P_177 ; } _rtB -> B_45_164_0 = ( _rtB -> B_45_162_0 >= _rtB -> B_45_163_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_45_165_0 = _rtDW -> Memory_PreviousInput_ei ; } if ( _rtB -> B_45_164_0 ) {
_rtB -> B_19_0_0 = _rtB -> B_45_160_0 - _rtB -> B_45_161_0 ; _rtB -> B_19_1_0
= _rtP -> P_16 * _rtB -> B_19_0_0 ; _rtB -> B_45_167_0 = _rtB -> B_19_1_0 ; }
else { _rtB -> B_45_167_0 = _rtB -> B_45_165_0 ; } _rtB -> B_45_168_0 . re =
_rtB -> B_45_159_0 ; _rtB -> B_45_168_0 . im = _rtB -> B_45_167_0 ; _rtB ->
B_45_169_0 = muDoubleScalarHypot ( _rtB -> B_45_168_0 . re , _rtB ->
B_45_168_0 . im ) ; _rtB -> B_45_169_1 = muDoubleScalarAtan2 ( _rtB ->
B_45_168_0 . im , _rtB -> B_45_168_0 . re ) ; _rtB -> B_45_170_0 = _rtP ->
P_179 * _rtB -> B_45_169_1 ; _rtB -> B_45_171_0 = _rtP -> P_180 * _rtB ->
B_45_170_0 ; tmpForInput_idx_0 = _rtB -> B_45_171_0 ; tmpForInput_idx_1 =
_rtB -> B_45_169_0 ; tmpForInput_idx_2 = muDoubleScalarCos (
tmpForInput_idx_0 ) ; tmpForInput_idx_0 = muDoubleScalarSin (
tmpForInput_idx_0 ) ; _rtB -> B_45_172_0 . re = tmpForInput_idx_1 *
tmpForInput_idx_2 ; _rtB -> B_45_172_0 . im = tmpForInput_idx_1 *
tmpForInput_idx_0 ; _rtB -> B_45_173_0 = _rtX -> integrator_CSTATE_o ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ar .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ar . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_182 ; _rtB -> B_45_174_0 =
DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> TransportDelay_IWORK_b . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_b . Last , _rtDW -> TransportDelay_IWORK_b . Tail ,
_rtDW -> TransportDelay_IWORK_b . Head , _rtP -> P_183 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_45_175_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_45_176_0 = _rtP -> P_184 ; } _rtB ->
B_45_177_0 = ( _rtB -> B_45_175_0 >= _rtB -> B_45_176_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_178_0 = _rtDW
-> Memory_PreviousInput_a ; } if ( _rtB -> B_45_177_0 ) { _rtB -> B_22_0_0 =
_rtB -> B_45_173_0 - _rtB -> B_45_174_0 ; _rtB -> B_22_1_0 = _rtP -> P_19 *
_rtB -> B_22_0_0 ; _rtB -> B_45_180_0 = _rtB -> B_22_1_0 ; } else { _rtB ->
B_45_180_0 = _rtB -> B_45_178_0 ; } _rtB -> B_45_181_0 = _rtX ->
integrator_CSTATE_lx ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_o . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_o . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_187 ; _rtB ->
B_45_182_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_hs . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_hs . Last , _rtDW -> TransportDelay_IWORK_hs .
Tail , _rtDW -> TransportDelay_IWORK_hs . Head , _rtP -> P_188 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_45_183_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_184_0 = _rtP
-> P_189 ; } _rtB -> B_45_185_0 = ( _rtB -> B_45_183_0 >= _rtB -> B_45_184_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_45_186_0 = _rtDW -> Memory_PreviousInput_in ; } if ( _rtB -> B_45_185_0 ) {
_rtB -> B_21_0_0 = _rtB -> B_45_181_0 - _rtB -> B_45_182_0 ; _rtB -> B_21_1_0
= _rtP -> P_18 * _rtB -> B_21_0_0 ; _rtB -> B_45_188_0 = _rtB -> B_21_1_0 ; }
else { _rtB -> B_45_188_0 = _rtB -> B_45_186_0 ; } _rtB -> B_45_189_0 . re =
_rtB -> B_45_180_0 ; _rtB -> B_45_189_0 . im = _rtB -> B_45_188_0 ; _rtB ->
B_45_190_0 = muDoubleScalarHypot ( _rtB -> B_45_189_0 . re , _rtB ->
B_45_189_0 . im ) ; _rtB -> B_45_190_1 = muDoubleScalarAtan2 ( _rtB ->
B_45_189_0 . im , _rtB -> B_45_189_0 . re ) ; _rtB -> B_45_191_0 = _rtP ->
P_191 * _rtB -> B_45_190_1 ; _rtB -> B_45_192_0 = _rtP -> P_192 * _rtB ->
B_45_191_0 ; tmpForInput_idx_0 = _rtB -> B_45_192_0 ; tmpForInput_idx_1 =
_rtB -> B_45_190_0 ; tmpForInput_idx_2 = muDoubleScalarCos (
tmpForInput_idx_0 ) ; tmpForInput_idx_0 = muDoubleScalarSin (
tmpForInput_idx_0 ) ; _rtB -> B_45_193_0 . re = tmpForInput_idx_1 *
tmpForInput_idx_2 ; _rtB -> B_45_193_0 . im = tmpForInput_idx_1 *
tmpForInput_idx_0 ; _rtB -> B_45_194_0 = _rtX -> integrator_CSTATE_k ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_i .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_i . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_194 ; _rtB -> B_45_195_0 =
DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> TransportDelay_IWORK_ed . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ed . Last , _rtDW -> TransportDelay_IWORK_ed . Tail ,
_rtDW -> TransportDelay_IWORK_ed . Head , _rtP -> P_195 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_45_196_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_45_197_0 = _rtP -> P_196 ; } _rtB ->
B_45_198_0 = ( _rtB -> B_45_196_0 >= _rtB -> B_45_197_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_199_0 = _rtDW
-> Memory_PreviousInput_f ; } if ( _rtB -> B_45_198_0 ) { _rtB -> B_24_0_0 =
_rtB -> B_45_194_0 - _rtB -> B_45_195_0 ; _rtB -> B_24_1_0 = _rtP -> P_21 *
_rtB -> B_24_0_0 ; _rtB -> B_45_201_0 = _rtB -> B_24_1_0 ; } else { _rtB ->
B_45_201_0 = _rtB -> B_45_199_0 ; } _rtB -> B_45_202_0 = _rtX ->
integrator_CSTATE_mu ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_lb . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_lb . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_199 ; _rtB
-> B_45_203_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 ,
* tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ns . CircularBufSize ,
& _rtDW -> TransportDelay_IWORK_ns . Last , _rtDW -> TransportDelay_IWORK_ns
. Tail , _rtDW -> TransportDelay_IWORK_ns . Head , _rtP -> P_200 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_45_204_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_205_0 = _rtP
-> P_201 ; } _rtB -> B_45_206_0 = ( _rtB -> B_45_204_0 >= _rtB -> B_45_205_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_45_207_0 = _rtDW -> Memory_PreviousInput_iw ; } if ( _rtB -> B_45_206_0 ) {
_rtB -> B_23_0_0 = _rtB -> B_45_202_0 - _rtB -> B_45_203_0 ; _rtB -> B_23_1_0
= _rtP -> P_20 * _rtB -> B_23_0_0 ; _rtB -> B_45_209_0 = _rtB -> B_23_1_0 ; }
else { _rtB -> B_45_209_0 = _rtB -> B_45_207_0 ; } _rtB -> B_45_210_0 . re =
_rtB -> B_45_201_0 ; _rtB -> B_45_210_0 . im = _rtB -> B_45_209_0 ; _rtB ->
B_45_211_0 = muDoubleScalarHypot ( _rtB -> B_45_210_0 . re , _rtB ->
B_45_210_0 . im ) ; _rtB -> B_45_211_1 = muDoubleScalarAtan2 ( _rtB ->
B_45_210_0 . im , _rtB -> B_45_210_0 . re ) ; _rtB -> B_45_212_0 = _rtP ->
P_203 * _rtB -> B_45_211_1 ; _rtB -> B_45_213_0 = _rtP -> P_204 * _rtB ->
B_45_212_0 ; tmpForInput_idx_0 = _rtB -> B_45_213_0 ; tmpForInput_idx_1 =
_rtB -> B_45_211_0 ; tmpForInput_idx_2 = muDoubleScalarCos (
tmpForInput_idx_0 ) ; tmpForInput_idx_0 = muDoubleScalarSin (
tmpForInput_idx_0 ) ; _rtB -> B_45_214_0 . re = tmpForInput_idx_1 *
tmpForInput_idx_2 ; _rtB -> B_45_214_0 . im = tmpForInput_idx_1 *
tmpForInput_idx_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_45_215_0 = _rtB -> B_45_5_0_p ; } DZG_v6_SimGen_NegSeqComputation (
S , _rtB -> B_45_215_0 , _rtB -> B_45_172_0 , _rtB -> B_45_193_0 , _rtB ->
B_45_214_0 , & _rtB -> PosSeqComputation_d , & _rtDW -> PosSeqComputation_d ,
& _rtP -> PosSeqComputation_d ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_45_217_0 = _rtB -> B_45_6_0_c ; }
DZG_v6_SimGen_NegSeqComputation ( S , _rtB -> B_45_217_0 , _rtB -> B_45_172_0
, _rtB -> B_45_193_0 , _rtB -> B_45_214_0 , & _rtB -> NegSeqComputation_c , &
_rtDW -> NegSeqComputation_c , & _rtP -> NegSeqComputation_c ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_219_0 = _rtB
-> B_45_7_0 ; } DZG_v6_SimGen_ZeroSeqComputation ( S , _rtB -> B_45_219_0 ,
_rtB -> B_45_172_0 , _rtB -> B_45_193_0 , _rtB -> B_45_214_0 , & _rtB ->
ZeroSeqComputation_g , & _rtDW -> ZeroSeqComputation_g , & _rtP ->
ZeroSeqComputation_g ) ; _rtB -> B_45_221_0 [ 0 ] = muDoubleScalarHypot (
_rtB -> PosSeqComputation_d . B_16_2_0 . re , _rtB -> PosSeqComputation_d .
B_16_2_0 . im ) ; _rtB -> B_45_221_0 [ 1 ] = muDoubleScalarHypot ( _rtB ->
NegSeqComputation_c . B_16_2_0 . re , _rtB -> NegSeqComputation_c . B_16_2_0
. im ) ; _rtB -> B_45_221_0 [ 2 ] = muDoubleScalarHypot ( _rtB ->
ZeroSeqComputation_g . B_18_1_0 . re , _rtB -> ZeroSeqComputation_g .
B_18_1_0 . im ) ; _rtB -> B_45_221_1 [ 0 ] = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_d . B_16_2_0 . im , _rtB -> PosSeqComputation_d . B_16_2_0
. re ) ; _rtB -> B_45_221_1 [ 1 ] = muDoubleScalarAtan2 ( _rtB ->
NegSeqComputation_c . B_16_2_0 . im , _rtB -> NegSeqComputation_c . B_16_2_0
. re ) ; _rtB -> B_45_221_1 [ 2 ] = muDoubleScalarAtan2 ( _rtB ->
ZeroSeqComputation_g . B_18_1_0 . im , _rtB -> ZeroSeqComputation_g .
B_18_1_0 . re ) ; _rtB -> B_45_222_0 = _rtB -> B_45_151_0 [ 0 ] * _rtB ->
B_45_221_0 [ 0 ] ; _rtB -> B_45_223_0 = _rtP -> P_205 * _rtB -> B_45_222_0 ;
_rtB -> B_45_224_0 = _rtP -> P_206 * _rtB -> B_45_151_1 [ 0 ] ; _rtB ->
B_45_225_0 = _rtP -> P_207 * _rtB -> B_45_221_1 [ 0 ] ; _rtB -> B_45_226_0 =
_rtB -> B_45_224_0 - _rtB -> B_45_225_0 ; _rtB -> B_45_227_0 = _rtP -> P_208
* _rtB -> B_45_226_0 ; muDoubleScalarSinCos ( _rtB -> B_45_227_0 , & _rtB ->
B_45_228_0 , & _rtB -> B_45_228_1 ) ; _rtB -> B_45_229_0 = _rtB -> B_45_223_0
* _rtB -> B_45_228_1 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { ssCallAccelRunBlock ( S , 45 , 230 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 231 , SS_CALL_MDL_OUTPUTS ) ; } _rtB ->
B_45_232_0 = _rtB -> B_45_223_0 * _rtB -> B_45_228_0 ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S , 45 , 233 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 45 , 234 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 45 , 235 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 45 , 236 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_45_237_0 = muDoubleScalarSin ( _rtP ->
P_211 * ssGetTaskTime ( S , 0 ) + _rtP -> P_212 ) * _rtP -> P_209 + _rtP ->
P_210 ; _rtB -> B_45_238_0 = _rtB -> B_45_74_0 [ 0 ] * _rtB -> B_45_237_0 ;
_rtB -> B_45_239_0 = muDoubleScalarSin ( _rtP -> P_215 * ssGetTaskTime ( S ,
0 ) + _rtP -> P_216 ) * _rtP -> P_213 + _rtP -> P_214 ; _rtB -> B_45_240_0 =
_rtB -> B_45_74_0 [ 0 ] * _rtB -> B_45_239_0 ; _rtB -> B_45_241_0 =
muDoubleScalarSin ( _rtP -> P_219 * ssGetTaskTime ( S , 0 ) + _rtP -> P_220 )
* _rtP -> P_217 + _rtP -> P_218 ; _rtB -> B_45_242_0 = _rtB -> B_45_78_0 [ 0
] * _rtB -> B_45_241_0 ; _rtB -> B_45_243_0 = muDoubleScalarSin ( _rtP ->
P_223 * ssGetTaskTime ( S , 0 ) + _rtP -> P_224 ) * _rtP -> P_221 + _rtP ->
P_222 ; _rtB -> B_45_244_0 = _rtB -> B_45_78_0 [ 0 ] * _rtB -> B_45_243_0 ;
_rtB -> B_45_245_0 = muDoubleScalarSin ( _rtP -> P_227 * ssGetTaskTime ( S ,
0 ) + _rtP -> P_228 ) * _rtP -> P_225 + _rtP -> P_226 ; _rtB -> B_45_246_0 =
_rtB -> B_45_74_0 [ 0 ] * _rtB -> B_45_245_0 ; _rtB -> B_45_247_0 =
muDoubleScalarSin ( _rtP -> P_231 * ssGetTaskTime ( S , 0 ) + _rtP -> P_232 )
* _rtP -> P_229 + _rtP -> P_230 ; _rtB -> B_45_248_0 = _rtB -> B_45_74_0 [ 0
] * _rtB -> B_45_247_0 ; _rtB -> B_45_249_0 = muDoubleScalarSin ( _rtP ->
P_235 * ssGetTaskTime ( S , 0 ) + _rtP -> P_236 ) * _rtP -> P_233 + _rtP ->
P_234 ; _rtB -> B_45_250_0 = _rtB -> B_45_74_0 [ 1 ] * _rtB -> B_45_249_0 ;
_rtB -> B_45_251_0 = muDoubleScalarSin ( _rtP -> P_239 * ssGetTaskTime ( S ,
0 ) + _rtP -> P_240 ) * _rtP -> P_237 + _rtP -> P_238 ; _rtB -> B_45_252_0 =
_rtB -> B_45_74_0 [ 1 ] * _rtB -> B_45_251_0 ; _rtB -> B_45_253_0 =
muDoubleScalarSin ( _rtP -> P_243 * ssGetTaskTime ( S , 0 ) + _rtP -> P_244 )
* _rtP -> P_241 + _rtP -> P_242 ; _rtB -> B_45_254_0 = _rtB -> B_45_74_0 [ 2
] * _rtB -> B_45_253_0 ; _rtB -> B_45_255_0 = muDoubleScalarSin ( _rtP ->
P_247 * ssGetTaskTime ( S , 0 ) + _rtP -> P_248 ) * _rtP -> P_245 + _rtP ->
P_246 ; _rtB -> B_45_256_0 = _rtB -> B_45_74_0 [ 2 ] * _rtB -> B_45_255_0 ;
_rtB -> B_45_257_0 = muDoubleScalarSin ( _rtP -> P_251 * ssGetTaskTime ( S ,
0 ) + _rtP -> P_252 ) * _rtP -> P_249 + _rtP -> P_250 ; _rtB -> B_45_258_0 =
_rtB -> B_45_78_0 [ 0 ] * _rtB -> B_45_257_0 ; _rtB -> B_45_259_0 =
muDoubleScalarSin ( _rtP -> P_255 * ssGetTaskTime ( S , 0 ) + _rtP -> P_256 )
* _rtP -> P_253 + _rtP -> P_254 ; _rtB -> B_45_260_0 = _rtB -> B_45_78_0 [ 0
] * _rtB -> B_45_259_0 ; _rtB -> B_45_261_0 = muDoubleScalarSin ( _rtP ->
P_259 * ssGetTaskTime ( S , 0 ) + _rtP -> P_260 ) * _rtP -> P_257 + _rtP ->
P_258 ; _rtB -> B_45_262_0 = _rtB -> B_45_78_0 [ 1 ] * _rtB -> B_45_261_0 ;
_rtB -> B_45_263_0 = muDoubleScalarSin ( _rtP -> P_263 * ssGetTaskTime ( S ,
0 ) + _rtP -> P_264 ) * _rtP -> P_261 + _rtP -> P_262 ; _rtB -> B_45_264_0 =
_rtB -> B_45_78_0 [ 1 ] * _rtB -> B_45_263_0 ; _rtB -> B_45_265_0 =
muDoubleScalarSin ( _rtP -> P_267 * ssGetTaskTime ( S , 0 ) + _rtP -> P_268 )
* _rtP -> P_265 + _rtP -> P_266 ; _rtB -> B_45_266_0 = _rtB -> B_45_78_0 [ 2
] * _rtB -> B_45_265_0 ; _rtB -> B_45_267_0 = muDoubleScalarSin ( _rtP ->
P_271 * ssGetTaskTime ( S , 0 ) + _rtP -> P_272 ) * _rtP -> P_269 + _rtP ->
P_270 ; _rtB -> B_45_268_0 = _rtB -> B_45_78_0 [ 2 ] * _rtB -> B_45_267_0 ;
isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { if ( _rtB ->
B_45_9_0_a ) { _rtB -> B_45_269_0 = _rtB -> B_45_10_0 [ 5 ] ; } else { _rtB
-> B_45_269_0 = _rtB -> B_45_10_0_f ; } _rtB -> B_45_270_0 [ 0 ] = _rtP ->
P_273 [ 0 ] * _rtB -> B_45_5_0 [ 0 ] ; _rtB -> B_45_270_0 [ 1 ] = _rtP ->
P_273 [ 1 ] * _rtB -> B_45_5_0 [ 4 ] ; _rtB -> B_45_270_0 [ 2 ] = _rtP ->
P_273 [ 2 ] * _rtB -> B_45_5_0 [ 5 ] ; if ( _rtB -> B_45_19_0_g >= _rtP ->
P_274 ) { _rtB -> B_45_271_0 [ 0 ] = _rtB -> B_37_20_0 ; _rtB -> B_45_271_0 [
1 ] = _rtB -> B_37_6_0 ; } else { _rtB -> B_45_271_0 [ 0 ] = _rtB ->
B_45_20_0_l [ 0 ] ; _rtB -> B_45_271_0 [ 1 ] = _rtB -> B_45_20_0_l [ 1 ] ; }
_rtB -> B_45_272_0 [ 0 ] = _rtB -> B_45_270_0 [ 0 ] * _rtB -> B_45_271_0 [ 0
] ; _rtB -> B_45_272_0 [ 1 ] = _rtB -> B_45_270_0 [ 1 ] * _rtB -> B_45_271_0
[ 0 ] ; _rtB -> B_45_272_0 [ 2 ] = _rtB -> B_45_270_0 [ 2 ] * _rtB ->
B_45_271_0 [ 0 ] ; tmpForInput_idx_0 = _rtB -> B_45_272_0 [ 0 ] ; _rtB ->
B_45_274_0 [ 0 ] = _rtP -> P_275 [ 0 ] * _rtB -> B_45_5_0 [ 1 ] ; _rtB ->
B_45_275_0 [ 0 ] = _rtB -> B_45_274_0 [ 0 ] * _rtB -> B_45_271_0 [ 1 ] ;
tmpForInput_idx_0 += _rtB -> B_45_272_0 [ 1 ] ; _rtB -> B_45_274_0 [ 1 ] =
_rtP -> P_275 [ 1 ] * _rtB -> B_45_5_0 [ 2 ] ; _rtB -> B_45_275_0 [ 1 ] =
_rtB -> B_45_274_0 [ 1 ] * _rtB -> B_45_271_0 [ 1 ] ; tmpForInput_idx_0 +=
_rtB -> B_45_272_0 [ 2 ] ; _rtB -> B_45_274_0 [ 2 ] = _rtP -> P_275 [ 2 ] *
_rtB -> B_45_5_0 [ 3 ] ; _rtB -> B_45_275_0 [ 2 ] = _rtB -> B_45_274_0 [ 2 ]
* _rtB -> B_45_271_0 [ 1 ] ; _rtB -> B_45_273_0 = tmpForInput_idx_0 ;
tmpForInput_idx_0 = _rtB -> B_45_275_0 [ 0 ] ; tmpForInput_idx_0 += _rtB ->
B_45_275_0 [ 1 ] ; tmpForInput_idx_0 += _rtB -> B_45_275_0 [ 2 ] ; _rtB ->
B_45_276_0 = tmpForInput_idx_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_45_277_0 [ 0 ] = _rtB -> B_45_19_0 [ 6 ] ; _rtB ->
B_45_277_0 [ 1 ] = _rtB -> B_45_19_0 [ 7 ] ; } _rtB -> B_45_278_0 [ 0 ] =
_rtP -> P_276 * _rtB -> B_45_277_0 [ 0 ] ; _rtB -> B_45_278_0 [ 1 ] = _rtP ->
P_276 * _rtB -> B_45_277_0 [ 1 ] ; _rtB -> B_45_279_0 = ( ( 2.0 * _rtB ->
B_45_278_0 [ 0 ] + _rtB -> B_45_278_0 [ 1 ] ) * _rtB -> B_45_4_1 +
1.7320508075688772 * _rtB -> B_45_278_0 [ 1 ] * _rtB -> B_45_4_0 ) *
0.33333333333333331 ; _rtB -> B_45_280_0 = ( ( 2.0 * _rtB -> B_45_278_0 [ 0 ]
+ _rtB -> B_45_278_0 [ 1 ] ) * _rtB -> B_45_4_0 + - 1.7320508075688772 * _rtB
-> B_45_278_0 [ 1 ] * _rtB -> B_45_4_1 ) * 0.33333333333333331 ; if ( _rtB ->
B_45_17_0_j >= _rtP -> P_277 ) { _rtB -> B_45_281_0 [ 0 ] = _rtB -> B_37_14_0
; _rtB -> B_45_281_0 [ 1 ] = _rtB -> B_37_12_0 ; } else { _rtB -> B_45_281_0
[ 0 ] = _rtB -> B_45_21_0_d [ 0 ] ; _rtB -> B_45_281_0 [ 1 ] = _rtB ->
B_45_21_0_d [ 1 ] ; } _rtB -> B_45_282_0 = muDoubleScalarAtan2 ( _rtB ->
B_45_280_0 , _rtB -> B_45_279_0 ) ; _rtB -> B_45_283_0 = _rtP -> P_278 * _rtB
-> B_45_282_0 ; _rtB -> B_45_284_0 = _rtB -> B_45_279_0 * _rtB -> B_45_10_0 [
0 ] ; _rtB -> B_45_285_0 = _rtB -> B_45_280_0 * _rtB -> B_45_10_0 [ 1 ] ;
_rtB -> B_45_286_0 = _rtB -> B_45_284_0 + _rtB -> B_45_285_0 ; _rtB ->
B_45_287_0 = _rtP -> P_279 * _rtB -> B_45_286_0 ; _rtB -> B_45_288_0 = _rtB
-> B_45_279_0 * _rtB -> B_45_10_0 [ 1 ] ; _rtB -> B_45_289_0 = _rtB ->
B_45_280_0 * _rtB -> B_45_10_0 [ 0 ] ; _rtB -> B_45_290_0 = _rtB ->
B_45_288_0 - _rtB -> B_45_289_0 ; _rtB -> B_45_291_0 = _rtP -> P_280 * _rtB
-> B_45_290_0 ; _rtB -> B_45_292_0 [ 0 ] = _rtP -> P_281 [ 0 ] * _rtB ->
B_45_11_0 ; _rtB -> B_45_292_0 [ 1 ] = _rtP -> P_281 [ 1 ] * _rtB ->
B_45_12_0 ; _rtB -> B_45_292_0 [ 2 ] = _rtP -> P_281 [ 2 ] * _rtB ->
B_45_13_0 ; _rtB -> B_45_292_0 [ 3 ] = _rtP -> P_281 [ 3 ] * _rtB ->
B_45_10_0 [ 0 ] ; _rtB -> B_45_292_0 [ 4 ] = _rtP -> P_281 [ 4 ] * _rtB ->
B_45_10_0 [ 1 ] ; _rtB -> B_45_292_0 [ 5 ] = _rtP -> P_281 [ 5 ] * _rtB ->
B_45_10_0 [ 2 ] ; _rtB -> B_45_292_0 [ 6 ] = _rtP -> P_281 [ 6 ] * _rtB ->
B_45_10_0 [ 4 ] ; _rtB -> B_45_292_0 [ 7 ] = _rtP -> P_281 [ 7 ] * _rtB ->
B_45_269_0 ; _rtB -> B_45_292_0 [ 8 ] = _rtP -> P_281 [ 8 ] * _rtB ->
B_45_10_0 [ 3 ] ; _rtB -> B_45_292_0 [ 9 ] = _rtP -> P_281 [ 9 ] * _rtB ->
B_45_273_0 ; _rtB -> B_45_292_0 [ 10 ] = _rtP -> P_281 [ 10 ] * _rtB ->
B_45_276_0 ; _rtB -> B_45_292_0 [ 11 ] = _rtP -> P_281 [ 11 ] * _rtB ->
B_45_279_0 ; _rtB -> B_45_292_0 [ 12 ] = _rtP -> P_281 [ 12 ] * _rtB ->
B_45_280_0 ; _rtB -> B_45_292_0 [ 13 ] = _rtP -> P_281 [ 13 ] * _rtB ->
B_45_281_0 [ 0 ] ; _rtB -> B_45_292_0 [ 14 ] = _rtP -> P_281 [ 14 ] * _rtB ->
B_45_281_0 [ 1 ] ; _rtB -> B_45_292_0 [ 15 ] = _rtP -> P_281 [ 15 ] * _rtB ->
B_45_283_0 ; _rtB -> B_45_292_0 [ 16 ] = _rtP -> P_281 [ 16 ] * _rtB ->
B_45_287_0 ; _rtB -> B_45_292_0 [ 17 ] = _rtP -> P_281 [ 17 ] * _rtB ->
B_45_291_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { memcpy
( & _rtB -> B_45_293_0 [ 0 ] , & _rtB -> B_45_292_0 [ 0 ] , 18U * sizeof (
real_T ) ) ; } _rtB -> B_45_294_0 = _rtP -> P_282 * _rtB -> B_45_293_0 [ 16 ]
; ssCallAccelRunBlock ( S , 45 , 295 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 296 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_45_297_0 = _rtP -> P_283 * _rtB -> B_45_293_0 [ 17 ] ; ssCallAccelRunBlock
( S , 45 , 298 , SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_45_299_0 = _rtP -> P_284 *
_rtB -> B_45_293_0 [ 5 ] ; ssCallAccelRunBlock ( S , 45 , 300 ,
SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_45_301_0 = muDoubleScalarHypot ( _rtB ->
B_45_279_0 , _rtB -> B_45_280_0 ) ; _rtB -> B_45_302_0 [ 0 ] = _rtP -> P_285
[ 0 ] * _rtB -> B_45_10_0 [ 0 ] ; _rtB -> B_45_302_0 [ 1 ] = _rtP -> P_285 [
1 ] * _rtB -> B_45_10_0 [ 1 ] ; _rtB -> B_45_303_0 [ 0 ] = _rtB -> B_45_5_0 [
1 ] * _rtB -> B_45_302_0 [ 0 ] ; _rtB -> B_45_303_0 [ 1 ] = _rtB -> B_45_5_0
[ 0 ] * _rtB -> B_45_302_0 [ 1 ] ; tmpForInput_idx_0 = _rtB -> B_45_303_0 [ 0
] ; tmpForInput_idx_0 += _rtB -> B_45_303_0 [ 1 ] ; _rtB -> B_45_304_0 =
tmpForInput_idx_0 ; _rtB -> B_45_305_0 = _rtDW -> dw_delay_DSTATE ; _rtB ->
B_45_306_0 = _rtP -> P_287 * _rtB -> B_45_305_0 ; _rtB -> B_45_307_0 = _rtDW
-> dw_predict_DSTATE ; _rtB -> B_45_308_0 = _rtB -> B_45_306_0 - _rtB ->
B_45_307_0 ; _rtB -> B_45_309_0 = _rtB -> B_45_26_0 + _rtB -> B_45_308_0 ;
for ( i = 0 ; i < 6 ; i ++ ) { _rtB -> B_45_310_0 [ i ] = _rtDW ->
voltages_DSTATE [ i ] ; } _rtB -> B_45_311_0 = ssGetTaskTime ( S , 2 ) ; if (
_rtB -> B_45_311_0 >= _rtP -> P_290 ) { _rtB -> B_38_0_0 [ 0 ] = _rtB ->
B_45_279_0 + _rtB -> B_45_310_0 [ 0 ] ; _rtB -> B_38_0_0 [ 1 ] = _rtB ->
B_45_280_0 + _rtB -> B_45_310_0 [ 1 ] ; _rtB -> B_38_0_0 [ 2 ] = _rtB ->
B_45_12_0_g + _rtB -> B_45_310_0 [ 2 ] ; _rtB -> B_38_0_0 [ 3 ] = _rtB ->
B_45_25_0_l [ 0 ] + _rtB -> B_45_310_0 [ 3 ] ; _rtB -> B_38_0_0 [ 4 ] = _rtB
-> B_45_25_0_l [ 1 ] + _rtB -> B_45_310_0 [ 4 ] ; _rtB -> B_38_0_0 [ 5 ] =
_rtB -> B_45_25_0_l [ 2 ] + _rtB -> B_45_310_0 [ 5 ] ; for ( i = 0 ; i < 36 ;
i ++ ) { if ( _rtB -> B_45_14_0_n >= _rtP -> P_56 ) { _rtB -> B_38_1_0 [ i ]
= _rtB -> B_37_19_0 [ i ] ; } else { _rtB -> B_38_1_0 [ i ] = _rtB ->
B_45_18_0_d [ i ] ; } _rtB -> B_38_2_0 [ i ] = _rtB -> B_45_22_0_d [ i ] ; }
_rtB -> B_38_2_0 [ 6 ] = _rtB -> B_45_309_0 ; _rtB -> B_38_3_0 = _rtP -> P_57
* _rtB -> B_45_309_0 ; memcpy ( & _rtB -> B_38_4_0 [ 0 ] , & _rtB -> B_38_2_0
[ 0 ] , 36U * sizeof ( real_T ) ) ; _rtB -> B_38_4_0 [ 1 ] = _rtB -> B_38_3_0
; for ( i = 0 ; i < 36 ; i ++ ) { _rtB -> B_38_5_0 [ i ] = ( 0.0 - _rtB ->
B_38_4_0 [ i ] ) - _rtB -> B_38_1_0 [ i ] ; _rtB -> B_38_6_0 [ i ] = _rtP ->
P_58 * _rtB -> B_38_5_0 [ i ] ; _rtB -> B_38_7_0 [ i ] = _rtB -> B_45_24_0 [
i ] - _rtB -> B_38_6_0 [ i ] ; } rt_invd6x6_snf ( _rtB -> B_38_7_0 , _rtB ->
B_38_8_0 ) ; for ( i = 0 ; i < 36 ; i ++ ) { _rtB -> B_38_9_0 [ i ] = _rtP ->
P_59 * _rtB -> B_38_8_0 [ i ] ; tmp_1 [ i ] = _rtB -> B_38_9_0 [ i ] ; } for
( i = 0 ; i < 6 ; i ++ ) { tmp [ i ] = _rtB -> B_38_0_0 [ i ] ; } for ( isHit
= 0 ; isHit < 6 ; isHit ++ ) { tmp_0 [ isHit ] = 0.0 ; for ( i = 0 ; i < 6 ;
i ++ ) { tmpForInput_idx_1 = tmp_0 [ isHit ] ; tmpForInput_idx_1 += tmp_1 [ 6
* i + isHit ] * tmp [ i ] ; tmp_0 [ isHit ] = tmpForInput_idx_1 ; } _rtB ->
B_38_10_0 [ isHit ] = tmp_0 [ isHit ] ; } for ( i = 0 ; i < 36 ; i ++ ) {
_rtB -> B_38_11_0 [ i ] = _rtB -> B_45_24_0 [ i ] + _rtB -> B_38_6_0 [ i ] ;
tmp_1 [ i ] = _rtB -> B_38_8_0 [ i ] ; tmp_2 [ i ] = _rtB -> B_38_11_0 [ i ]
; } for ( isHit = 0 ; isHit < 6 ; isHit ++ ) { for ( i = 0 ; i < 6 ; i ++ ) {
_rtB -> B_38_12_0 [ i + 6 * isHit ] = 0.0 ; for ( i_0 = 0 ; i_0 < 6 ; i_0 ++
) { _rtB -> B_38_12_0 [ i + 6 * isHit ] += tmp_1 [ 6 * i_0 + i ] * tmp_2 [ 6
* isHit + i_0 ] ; } } } memcpy ( & tmp_1 [ 0 ] , & _rtB -> B_38_12_0 [ 0 ] ,
36U * sizeof ( real_T ) ) ; for ( i = 0 ; i < 6 ; i ++ ) { tmp [ i ] = _rtB
-> B_45_5_0 [ i ] ; } for ( i = 0 ; i < 6 ; i ++ ) { tmp_0 [ i ] = 0.0 ; for
( isHit = 0 ; isHit < 6 ; isHit ++ ) { tmpForInput_idx_1 = tmp_0 [ i ] ;
tmpForInput_idx_1 += tmp_1 [ 6 * isHit + i ] * tmp [ isHit ] ; tmp_0 [ i ] =
tmpForInput_idx_1 ; } _rtB -> B_38_13_0 [ i ] = tmp_0 [ i ] ; _rtB ->
B_38_14_0 [ i ] = _rtB -> B_38_13_0 [ i ] + _rtB -> B_38_10_0 [ i ] ; } for (
i = 0 ; i < 6 ; i ++ ) { _rtB -> B_45_313_0 [ i ] = _rtB -> B_38_14_0 [ i ] ;
} } else { for ( i = 0 ; i < 6 ; i ++ ) { _rtB -> B_45_313_0 [ i ] = _rtB ->
B_45_5_0 [ i ] ; } } } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit !=
0 ) && ( _rtDW -> RateTransition1_semaphoreTaken == 0 ) ) { _rtDW ->
RateTransition1_Buffer0 = _rtB -> B_45_27_0_o ; } isHit = ssIsSampleHit ( S ,
2 , 0 ) ; if ( isHit != 0 ) { _rtDW -> RateTransition1_semaphoreTaken = 1 ;
_rtB -> B_45_314_0 = _rtDW -> RateTransition1_Buffer0 ; _rtDW ->
RateTransition1_semaphoreTaken = 0 ; _rtB -> B_45_315_0 = _rtP -> P_291 *
_rtB -> B_45_314_0 ; _rtB -> B_45_316_0 = _rtB -> B_45_315_0 / _rtB ->
B_45_309_0 ; _rtB -> B_45_317_0 = _rtP -> P_292 * _rtB -> B_45_309_0 ; _rtB
-> B_45_318_0 = ( _rtB -> B_45_316_0 - _rtB -> B_45_304_0 ) - _rtB ->
B_45_317_0 ; _rtB -> B_45_319_0 = _rtP -> P_293 * _rtB -> B_45_318_0 ; _rtB
-> B_45_320_0 = _rtDW -> theta_DSTATE ; tmpForInput_idx_1 = _rtB ->
B_45_320_0 ; tmpForInput_idx_0 = muDoubleScalarRem ( tmpForInput_idx_1 ,
6.2831853071795862 ) ; _rtB -> B_45_321_0 = tmpForInput_idx_0 ; _rtB ->
B_45_322_0 = _rtB -> B_45_309_0 * _rtB -> B_45_304_0 ; _rtB -> B_45_323_0 =
_rtP -> P_296 * _rtB -> B_45_309_0 ; _rtB -> B_45_324_0 = _rtP -> P_297 *
_rtB -> B_45_322_0 ; _rtB -> B_45_325_0 = _rtP -> P_298 * _rtB -> B_45_321_0
; _rtB -> B_45_326_0 = _rtP -> P_299 * _rtB -> B_45_304_0 ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_327_0 [ 0 ] =
_rtB -> B_45_0_0 ; _rtB -> B_45_327_0 [ 1 ] = _rtB -> B_45_323_0 ; _rtB ->
B_45_327_0 [ 2 ] = _rtB -> B_45_324_0 ; _rtB -> B_45_327_0 [ 3 ] = _rtB ->
B_45_308_0 ; _rtB -> B_45_327_0 [ 4 ] = _rtB -> B_45_325_0 ; _rtB ->
B_45_327_0 [ 5 ] = _rtB -> B_45_326_0 ; } if ( _rtDW ->
Rotorspeeddeviationdw_SYSTEM_ENABLE != 0 ) { _rtB -> B_45_328_0 = _rtDW ->
Rotorspeeddeviationdw_DSTATE ; } else { _rtB -> B_45_328_0 = _rtP -> P_300 *
_rtB -> B_45_319_0 + _rtDW -> Rotorspeeddeviationdw_DSTATE ; } _rtB ->
B_45_329_0 = _rtP -> P_302 * _rtB -> B_45_308_0 ; _rtB -> B_45_330_0 = _rtP
-> P_303 * _rtB -> B_45_309_0 ; { _rtB -> B_45_336_0 = ( _rtP -> P_306 [ 0 ]
) * _rtDW -> DiscreteStateSpace_DSTATE [ 0 ] + ( _rtP -> P_306 [ 1 ] ) *
_rtDW -> DiscreteStateSpace_DSTATE [ 1 ] ; _rtB -> B_45_336_0 += ( _rtP ->
P_307 ) * _rtB -> B_45_327_0 [ 3 ] ; } _rtB -> B_45_337_0 = _rtP -> P_309 *
_rtB -> B_45_336_0 ; { _rtB -> B_45_338_0 = _rtP -> P_312 * _rtDW ->
DiscreteStateSpace_DSTATE_j ; _rtB -> B_45_338_0 += _rtP -> P_313 * _rtB ->
B_45_337_0 ; } _rtB -> B_45_339_0 = _rtP -> P_315 * _rtB -> B_45_336_0 ; {
_rtB -> B_45_340_0 = _rtP -> P_318 * _rtDW -> DiscreteStateSpace_DSTATE_b ;
_rtB -> B_45_340_0 += _rtP -> P_319 * _rtB -> B_45_339_0 ; } _rtB ->
B_45_341_0 = _rtP -> P_321 * _rtB -> B_45_336_0 ; { _rtB -> B_45_342_0 = _rtP
-> P_324 * _rtDW -> DiscreteStateSpace_DSTATE_o ; _rtB -> B_45_342_0 += _rtP
-> P_325 * _rtB -> B_45_341_0 ; } _rtB -> B_45_343_0 = _rtP -> P_327 * _rtB
-> B_45_336_0 ; { _rtB -> B_45_344_0 = _rtP -> P_330 * _rtDW ->
DiscreteStateSpace_DSTATE_f ; _rtB -> B_45_344_0 += _rtP -> P_331 * _rtB ->
B_45_343_0 ; } { _rtB -> B_45_345_0 = ( _rtP -> P_335 [ 0 ] ) * _rtDW ->
DiscreteStateSpace_DSTATE_bk [ 0 ] + ( _rtP -> P_335 [ 1 ] ) * _rtDW ->
DiscreteStateSpace_DSTATE_bk [ 1 ] + ( _rtP -> P_335 [ 2 ] ) * _rtDW ->
DiscreteStateSpace_DSTATE_bk [ 2 ] ; _rtB -> B_45_345_0 += _rtP -> P_336 *
_rtB -> B_45_327_0 [ 3 ] ; } _rtB -> B_45_346_0 = _rtP -> P_338 * _rtB ->
B_45_345_0 ; { _rtB -> B_45_347_0 = _rtP -> P_341 * _rtDW ->
DiscreteStateSpace_DSTATE_a ; _rtB -> B_45_347_0 += _rtP -> P_342 * _rtB ->
B_45_346_0 ; } _rtB -> B_45_348_0 = _rtP -> P_344 * _rtB -> B_45_345_0 ; {
_rtB -> B_45_349_0 = _rtP -> P_347 * _rtDW -> DiscreteStateSpace_DSTATE_l ;
_rtB -> B_45_349_0 += _rtP -> P_348 * _rtB -> B_45_348_0 ; } _rtB ->
B_45_350_0 = _rtB -> B_45_30_0_b ;
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer ( S , _rtB -> B_45_350_0
, _rtB -> B_45_338_0 , & _rtB -> Transferfunctionsfordetailedstabilizer_j , &
_rtDW -> Transferfunctionsfordetailedstabilizer_j , & _rtP ->
Transferfunctionsfordetailedstabilizer_j ) ; if ( _rtB -> B_45_30_0_b >= _rtP
-> P_350 ) { _rtB -> B_45_352_0 = _rtB ->
Transferfunctionsfordetailedstabilizer_j . B_39_1_0 ; } else { _rtB ->
B_45_352_0 = _rtB -> B_45_338_0 ; } _rtB -> B_45_353_0 = _rtB -> B_45_30_0_b
; DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer ( S , _rtB ->
B_45_353_0 , _rtB -> B_45_340_0 , & _rtB ->
Transferfunctionsfordetailedstabilizer1_g , & _rtDW ->
Transferfunctionsfordetailedstabilizer1_g , & _rtP ->
Transferfunctionsfordetailedstabilizer1_g ) ; if ( _rtB -> B_45_30_0_b >=
_rtP -> P_351 ) { _rtB -> B_45_355_0 = _rtB ->
Transferfunctionsfordetailedstabilizer1_g . B_39_1_0 ; } else { _rtB ->
B_45_355_0 = _rtB -> B_45_340_0 ; } _rtB -> B_45_356_0 = _rtB -> B_45_352_0 -
_rtB -> B_45_355_0 ; _rtB -> B_45_357_0 = _rtP -> P_352 * _rtB -> B_45_356_0
; _rtB -> B_45_358_0 = _rtB -> B_45_30_0_b ;
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer ( S , _rtB -> B_45_358_0
, _rtB -> B_45_342_0 , & _rtB -> Transferfunctionsfordetailedstabilizer_e , &
_rtDW -> Transferfunctionsfordetailedstabilizer_e , & _rtP ->
Transferfunctionsfordetailedstabilizer_e ) ; if ( _rtB -> B_45_30_0_b >= _rtP
-> P_353 ) { _rtB -> B_45_360_0 = _rtB ->
Transferfunctionsfordetailedstabilizer_e . B_39_1_0 ; } else { _rtB ->
B_45_360_0 = _rtB -> B_45_342_0 ; } _rtB -> B_45_361_0 = _rtB -> B_45_30_0_b
; DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer ( S , _rtB ->
B_45_361_0 , _rtB -> B_45_344_0 , & _rtB ->
Transferfunctionsfordetailedstabilizer1_n , & _rtDW ->
Transferfunctionsfordetailedstabilizer1_n , & _rtP ->
Transferfunctionsfordetailedstabilizer1_n ) ; if ( _rtB -> B_45_30_0_b >=
_rtP -> P_354 ) { _rtB -> B_45_363_0 = _rtB ->
Transferfunctionsfordetailedstabilizer1_n . B_39_1_0 ; } else { _rtB ->
B_45_363_0 = _rtB -> B_45_344_0 ; } _rtB -> B_45_364_0 = _rtB -> B_45_360_0 -
_rtB -> B_45_363_0 ; _rtB -> B_45_365_0 = _rtP -> P_355 * _rtB -> B_45_364_0
; _rtB -> B_45_366_0 = _rtB -> B_45_30_0_b ;
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer ( S , _rtB -> B_45_366_0
, _rtB -> B_45_347_0 , & _rtB -> Transferfunctionsfordetailedstabilizer , &
_rtDW -> Transferfunctionsfordetailedstabilizer , & _rtP ->
Transferfunctionsfordetailedstabilizer ) ; if ( _rtB -> B_45_30_0_b >= _rtP
-> P_356 ) { _rtB -> B_45_368_0 = _rtB ->
Transferfunctionsfordetailedstabilizer . B_39_1_0 ; } else { _rtB ->
B_45_368_0 = _rtB -> B_45_347_0 ; } _rtB -> B_45_369_0 = _rtB -> B_45_30_0_b
; DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer ( S , _rtB ->
B_45_369_0 , _rtB -> B_45_349_0 , & _rtB ->
Transferfunctionsfordetailedstabilizer1 , & _rtDW ->
Transferfunctionsfordetailedstabilizer1 , & _rtP ->
Transferfunctionsfordetailedstabilizer1 ) ; if ( _rtB -> B_45_30_0_b >= _rtP
-> P_357 ) { _rtB -> B_45_371_0 = _rtB ->
Transferfunctionsfordetailedstabilizer1 . B_39_1_0 ; } else { _rtB ->
B_45_371_0 = _rtB -> B_45_349_0 ; } _rtB -> B_45_372_0 = _rtB -> B_45_368_0 -
_rtB -> B_45_371_0 ; _rtB -> B_45_373_0 = _rtP -> P_358 * _rtB -> B_45_372_0
; tmpForInput_idx_1 = _rtB -> B_45_357_0 ; tmpForInput_idx_2 = _rtP -> P_360
; tmpForInput_idx_0 = _rtP -> P_359 ; if ( tmpForInput_idx_1 >
tmpForInput_idx_0 ) { _rtB -> B_45_374_0 = tmpForInput_idx_0 ; } else if (
tmpForInput_idx_1 < tmpForInput_idx_2 ) { _rtB -> B_45_374_0 =
tmpForInput_idx_2 ; } else { _rtB -> B_45_374_0 = tmpForInput_idx_1 ; }
tmpForInput_idx_1 = _rtB -> B_45_365_0 ; tmpForInput_idx_2 = _rtP -> P_362 ;
tmpForInput_idx_0 = _rtP -> P_361 ; if ( tmpForInput_idx_1 >
tmpForInput_idx_0 ) { _rtB -> B_45_375_0 = tmpForInput_idx_0 ; } else if (
tmpForInput_idx_1 < tmpForInput_idx_2 ) { _rtB -> B_45_375_0 =
tmpForInput_idx_2 ; } else { _rtB -> B_45_375_0 = tmpForInput_idx_1 ; }
tmpForInput_idx_1 = _rtB -> B_45_373_0 ; tmpForInput_idx_2 = _rtP -> P_364 ;
tmpForInput_idx_0 = _rtP -> P_363 ; if ( tmpForInput_idx_1 >
tmpForInput_idx_0 ) { _rtB -> B_45_376_0 = tmpForInput_idx_0 ; } else if (
tmpForInput_idx_1 < tmpForInput_idx_2 ) { _rtB -> B_45_376_0 =
tmpForInput_idx_2 ; } else { _rtB -> B_45_376_0 = tmpForInput_idx_1 ; } _rtB
-> B_45_377_0 = ( _rtB -> B_45_374_0 + _rtB -> B_45_375_0 ) + _rtB ->
B_45_376_0 ; _rtB -> B_45_378_0 = _rtP -> P_365 * _rtB -> B_45_377_0 ;
tmpForInput_idx_1 = _rtB -> B_45_378_0 ; tmpForInput_idx_2 = _rtP -> P_367 ;
tmpForInput_idx_0 = _rtP -> P_366 ; if ( tmpForInput_idx_1 >
tmpForInput_idx_0 ) { _rtB -> B_45_379_0 = tmpForInput_idx_0 ; } else if (
tmpForInput_idx_1 < tmpForInput_idx_2 ) { _rtB -> B_45_379_0 =
tmpForInput_idx_2 ; } else { _rtB -> B_45_379_0 = tmpForInput_idx_1 ; } _rtB
-> B_45_380_0 = _rtDW -> UnitDelay2_DSTATE ; if ( _rtB -> B_45_28_0_b > _rtP
-> P_369 ) { _rtB -> B_45_381_0 = _rtB -> B_45_380_0 ; } else { _rtB ->
B_45_381_0 = _rtB -> B_45_29_0_n ; } tmpForInput_idx_1 = _rtB -> B_45_293_0 [
12 ] ; tmpForInput_idx_0 = muDoubleScalarPower ( tmpForInput_idx_1 , 2.0 ) ;
tmpForInput_idx_1 = _rtB -> B_45_293_0 [ 11 ] ; tmpForInput_idx_1 =
muDoubleScalarPower ( tmpForInput_idx_1 , 2.0 ) ; tmpForInput_idx_0 +=
tmpForInput_idx_1 ; if ( tmpForInput_idx_0 < 0.0 ) { tmpForInput_idx_0 = -
muDoubleScalarSqrt ( - tmpForInput_idx_0 ) ; } else { tmpForInput_idx_0 =
muDoubleScalarSqrt ( tmpForInput_idx_0 ) ; } _rtB -> B_45_382_0 =
tmpForInput_idx_0 ; { _rtB -> B_45_383_0 = _rtP -> P_372 * _rtDW ->
DiscreteStateSpace_DSTATE_i ; _rtB -> B_45_383_0 += _rtP -> P_373 * _rtB ->
B_45_382_0 ; } _rtB -> B_45_384_0 = _rtDW -> UnitDelay1_DSTATE ; _rtB ->
B_45_385_0 = ( ( ( _rtB -> B_45_31_0_l + _rtB -> B_45_32_0 ) - _rtB ->
B_45_383_0 ) + _rtB -> B_45_379_0 ) - _rtB -> B_45_384_0 ; { _rtB ->
B_45_386_0 = _rtP -> P_379 * _rtB -> B_45_385_0 ; } _rtB -> B_45_387_0 = _rtP
-> P_381 * _rtB -> B_45_386_0 ; { _rtB -> B_45_388_0 = _rtP -> P_384 * _rtDW
-> DiscreteStateSpace_DSTATE_h ; _rtB -> B_45_388_0 += _rtP -> P_385 * _rtB
-> B_45_387_0 ; } _rtB -> B_45_389_0 = ( _rtB -> B_45_388_0 > _rtB ->
B_45_34_0 ) ; _rtB -> B_45_390_0 = _rtP -> P_387 * _rtB -> B_45_383_0 ; _rtB
-> B_45_391_0 = _rtB -> B_45_390_0 + _rtB -> B_45_38_0_e ; _rtB -> B_45_392_0
= ( _rtB -> B_45_388_0 < _rtB -> B_45_391_0 ) ; _rtB -> B_45_393_0 = ( _rtB
-> B_45_389_0 && _rtB -> B_45_392_0 ) ; _rtB -> B_45_394_0 = _rtB ->
B_45_393_0 ; _rtB -> B_45_395_0 = _rtB -> B_45_388_0 * _rtB -> B_45_394_0 ;
_rtB -> B_45_396_0 = ! _rtB -> B_45_389_0 ; _rtB -> B_45_397_0 = _rtB ->
B_45_396_0 ; _rtB -> B_45_398_0 = _rtB -> B_45_34_0 * _rtB -> B_45_397_0 ;
_rtB -> B_45_399_0 = ! _rtB -> B_45_392_0 ; _rtB -> B_45_400_0 = _rtB ->
B_45_399_0 ; _rtB -> B_45_401_0 = _rtB -> B_45_400_0 * _rtB -> B_45_391_0 ;
_rtB -> B_45_402_0 = ( _rtB -> B_45_395_0 + _rtB -> B_45_398_0 ) + _rtB ->
B_45_401_0 ; _rtB -> B_45_403_0 = _rtP -> P_388 * _rtB -> B_45_402_0 ; { _rtB
-> B_45_404_0 = _rtP -> P_391 * _rtDW -> DiscreteStateSpace_DSTATE_be ; _rtB
-> B_45_404_0 += _rtP -> P_392 * _rtB -> B_45_403_0 ; } _rtB -> B_45_405_0 =
_rtP -> P_394 * _rtB -> B_45_404_0 ; { _rtB -> B_45_406_0 = ( _rtP -> P_397 )
* _rtDW -> DiscreteStateSpace_DSTATE_n ; _rtB -> B_45_406_0 += _rtP -> P_398
* _rtB -> B_45_405_0 ; } } if ( _rtX -> IntegratorLimited_CSTATE >= _rtP ->
P_401 ) { if ( _rtX -> IntegratorLimited_CSTATE != _rtP -> P_401 ) { _rtX ->
IntegratorLimited_CSTATE = _rtP -> P_401 ;
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } } else { if ( ( _rtX ->
IntegratorLimited_CSTATE <= _rtP -> P_402 ) && ( _rtX ->
IntegratorLimited_CSTATE != _rtP -> P_402 ) ) { _rtX ->
IntegratorLimited_CSTATE = _rtP -> P_402 ;
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } } _rtB -> B_45_407_0 =
_rtX -> IntegratorLimited_CSTATE ; if ( _rtB -> B_45_28_0_b > _rtP -> P_403 )
{ _rtB -> B_45_408_0 = _rtB -> B_45_407_0 ; } else { _rtB -> B_45_408_0 =
_rtB -> B_45_42_0_a ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0
) { _rtB -> B_45_409_0 = _rtP -> P_404 * _rtB -> B_45_327_0 [ 3 ] ; _rtB ->
B_45_410_0 = _rtP -> P_405 * _rtB -> B_45_409_0 ; if ( _rtB -> B_45_410_0 >
_rtP -> P_407 ) { _rtB -> B_45_411_0 = _rtB -> B_45_410_0 - _rtP -> P_407 ; }
else if ( _rtB -> B_45_410_0 >= _rtP -> P_406 ) { _rtB -> B_45_411_0 = 0.0 ;
} else { _rtB -> B_45_411_0 = _rtB -> B_45_410_0 - _rtP -> P_406 ; } } _rtB
-> B_45_412_0 = _rtX -> Integrator1_CSTATE ; _rtB -> B_45_413_0 = _rtX ->
Integrator1_CSTATE_d ; _rtB -> B_45_414_0 = _rtX -> Integrator2_CSTATE ; _rtB
-> B_45_415_0 = _rtB -> B_45_414_0 - _rtB -> B_45_43_0 ; _rtB -> B_45_416_0 =
_rtP -> P_411 * _rtB -> B_45_415_0 ; _rtB -> B_45_417_0 = _rtB -> B_45_413_0
+ _rtB -> B_45_416_0 ; _rtB -> B_45_418_0 = ( ( _rtB -> B_45_39_0_b + _rtB ->
B_45_411_0 ) + _rtB -> B_45_417_0 ) - _rtB -> B_45_412_0 ; _rtB -> B_45_419_0
= _rtP -> P_412 * _rtB -> B_45_418_0 ; _rtB -> B_45_420_0 = _rtB ->
B_45_412_0 - _rtB -> B_45_407_0 ; _rtB -> B_45_421_0 = _rtP -> P_413 * _rtB
-> B_45_420_0 ; tmpForInput_idx_1 = _rtB -> B_45_421_0 ; tmpForInput_idx_2 =
_rtP -> P_415 ; tmpForInput_idx_0 = _rtP -> P_414 ; if ( tmpForInput_idx_1 >
tmpForInput_idx_0 ) { _rtB -> B_45_422_0 = tmpForInput_idx_0 ; } else if (
tmpForInput_idx_1 < tmpForInput_idx_2 ) { _rtB -> B_45_422_0 =
tmpForInput_idx_2 ; } else { _rtB -> B_45_422_0 = tmpForInput_idx_1 ; } isHit
= ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_45_423_0 = _rtP
-> P_416 * _rtB -> B_45_409_0 ; } _rtB -> B_45_424_0 = ( _rtB -> B_45_423_0 +
_rtB -> B_45_40_0_j ) - _rtB -> B_45_414_0 ; _rtB -> B_45_425_0 = _rtP ->
P_417 * _rtB -> B_45_424_0 ; _rtB -> B_45_426_0 = _rtP -> P_418 * _rtB ->
B_45_424_0 ; _rtB -> B_45_427_0 = _rtX -> Integrator1_CSTATE_c ; _rtB ->
B_45_428_0 = _rtB -> B_45_427_0 + _rtB -> B_45_426_0 ; _rtB -> B_45_429_0 =
0.0 ; _rtB -> B_45_429_0 += _rtP -> P_421 * _rtX -> TransferFcn1_CSTATE ;
_rtB -> B_45_430_0 = _rtX -> Integrator1_CSTATE_l ; _rtB -> B_45_431_0 = _rtB
-> B_45_430_0 - _rtB -> B_45_414_0 ; _rtB -> B_45_432_0 = muDoubleScalarSqrt
( _rtB -> B_45_431_0 ) ; _rtB -> B_45_433_0 = _rtP -> P_423 * _rtB ->
B_45_432_0 ; _rtB -> B_45_434_0 = ( _rtB -> B_45_429_0 + _rtB -> B_45_41_0_f
) - _rtB -> B_45_433_0 ; _rtB -> B_45_435_0 = _rtP -> P_424 * _rtB ->
B_45_434_0 ; _rtB -> B_45_436_0 = _rtB -> B_45_408_0 * _rtB -> B_45_414_0 ;
_rtB -> B_45_437_0 = _rtB -> B_45_433_0 - _rtB -> B_45_436_0 ; _rtB ->
B_45_438_0 = _rtP -> P_425 * _rtB -> B_45_437_0 ; _rtB -> B_45_439_0 = 0.0 ;
_rtB -> B_45_439_0 += _rtP -> P_427 * _rtX -> TransferFcn_CSTATE ; { real_T *
* uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_aw . TUbufferPtrs
[ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_aw . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_428 ; if ( _rtP -> P_428 == 0.0 )
_rtB -> B_45_440_0 = _rtB -> B_45_428_0 ; else _rtB -> B_45_440_0 =
DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> TransportDelay_IWORK_l . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_l . Last , _rtDW -> TransportDelay_IWORK_l . Tail ,
_rtDW -> TransportDelay_IWORK_l . Head , _rtP -> P_429 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_45_441_0 = _rtP -> P_430 * _rtB -> B_45_415_0 ; _rtB ->
B_45_442_0 = _rtX -> Integrator2_CSTATE_n ; _rtB -> B_45_443_0 = _rtX ->
Integrator1_CSTATE_h ; _rtB -> B_45_444_0 = _rtB -> B_45_442_0 - _rtB ->
B_45_443_0 ; _rtB -> B_45_445_0 = _rtP -> P_433 * _rtB -> B_45_444_0 ; _rtB
-> B_45_446_0 = _rtB -> B_45_436_0 - _rtB -> B_45_442_0 ; _rtB -> B_45_447_0
= _rtP -> P_434 * _rtB -> B_45_446_0 ; _rtB -> B_45_448_0 = _rtP -> P_435 *
_rtB -> B_45_442_0 ; _rtB -> B_45_449_0 = _rtP -> P_436 * _rtB -> B_45_443_0
; _rtB -> B_45_450_0 = _rtX -> Integrator3_CSTATE ; _rtB -> B_45_451_0 = _rtP
-> P_438 * _rtB -> B_45_450_0 ; _rtB -> B_45_452_0 = _rtB -> B_45_443_0 -
_rtB -> B_45_450_0 ; _rtB -> B_45_453_0 = _rtP -> P_439 * _rtB -> B_45_452_0
; _rtB -> B_45_454_0 = ( _rtB -> B_45_448_0 + _rtB -> B_45_449_0 ) + _rtB ->
B_45_451_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_45_455_0 = _rtP -> P_440 ; _rtB -> B_45_456_0 = ssGetTaskTime ( S , 2 )
; _rtB -> B_45_457_0 = rt_Lookup ( _rtP -> P_441 , 6 , _rtB -> B_45_456_0 ,
_rtP -> P_442 ) ; if ( _rtB -> B_45_73_0_j >= _rtP -> P_443 ) { _rtB ->
B_45_458_0 = _rtB -> B_45_80_0 ; } else { _rtB -> B_45_458_0 = _rtB ->
B_45_457_0 ; } _rtB -> B_45_459_0 = ( _rtB -> B_45_458_0 != 0.0 ) ; _rtB ->
B_45_460_0 = ssGetTaskTime ( S , 2 ) ; if ( _rtB -> B_45_455_0 >= _rtP ->
P_444 ) { _rtB -> B_5_0_0 = ( _rtB -> B_45_459_0 && _rtB -> B_45_79_0 ) ;
_rtB -> B_5_1_0 = _rtB -> B_5_0_0 ; _rtB -> B_5_2_0 = _rtB -> B_5_1_0 ; _rtB
-> B_45_463_0 = _rtB -> B_5_2_0 ; } else { _rtB -> B_4_0_0 = rt_Lookup ( _rtP
-> P_4 , 5 , _rtB -> B_45_460_0 , _rtP -> P_5 ) ; _rtB -> B_45_463_0 = _rtB
-> B_4_0_0 ; } _rtB -> B_45_467_0 = _rtP -> P_445 ; _rtB -> B_45_468_0 =
ssGetTaskTime ( S , 2 ) ; if ( _rtB -> B_45_467_0 >= _rtP -> P_446 ) { _rtB
-> B_7_0_0 = ( _rtB -> B_45_459_0 && _rtB -> B_45_78_0_o ) ; _rtB -> B_7_1_0
= _rtB -> B_7_0_0 ; _rtB -> B_7_2_0 = _rtB -> B_7_1_0 ; _rtB -> B_45_471_0 =
_rtB -> B_7_2_0 ; } else { _rtB -> B_6_0_0 = rt_Lookup ( _rtP -> P_6 , 5 ,
_rtB -> B_45_468_0 , _rtP -> P_7 ) ; _rtB -> B_45_471_0 = _rtB -> B_6_0_0 ; }
_rtB -> B_45_475_0 = _rtP -> P_447 ; _rtB -> B_45_476_0 = ssGetTaskTime ( S ,
2 ) ; if ( _rtB -> B_45_475_0 >= _rtP -> P_448 ) { _rtB -> B_9_0_0 = ( _rtB
-> B_45_459_0 && _rtB -> B_45_77_0_l ) ; _rtB -> B_9_1_0 = _rtB -> B_9_0_0 ;
_rtB -> B_9_2_0 = _rtB -> B_9_1_0 ; _rtB -> B_45_479_0 = _rtB -> B_9_2_0 ; }
else { _rtB -> B_8_0_0 = rt_Lookup ( _rtP -> P_8 , 5 , _rtB -> B_45_476_0 ,
_rtP -> P_9 ) ; _rtB -> B_45_479_0 = _rtB -> B_8_0_0 ; } _rtB -> B_45_523_0 =
_rtP -> P_449 * _rtB -> B_45_19_0 [ 32 ] ; _rtB -> B_45_524_0 = _rtP -> P_450
* _rtB -> B_45_19_0 [ 33 ] ; _rtB -> B_45_525_0 = _rtP -> P_451 * _rtB ->
B_45_19_0 [ 34 ] ; _rtB -> B_45_526_0 [ 0 ] = _rtP -> P_452 * _rtB ->
B_45_523_0 ; _rtB -> B_45_526_0 [ 1 ] = _rtP -> P_452 * _rtB -> B_45_524_0 ;
_rtB -> B_45_526_0 [ 2 ] = _rtP -> P_452 * _rtB -> B_45_525_0 ; _rtB ->
B_45_527_0 = _rtP -> P_453 * _rtB -> B_45_19_0 [ 11 ] ; _rtB -> B_45_528_0 =
_rtP -> P_454 * _rtB -> B_45_19_0 [ 12 ] ; _rtB -> B_45_529_0 = _rtP -> P_455
* _rtB -> B_45_19_0 [ 13 ] ; _rtB -> B_45_530_0 [ 0 ] = _rtP -> P_456 * _rtB
-> B_45_527_0 ; _rtB -> B_45_530_0 [ 1 ] = _rtP -> P_456 * _rtB -> B_45_528_0
; _rtB -> B_45_530_0 [ 2 ] = _rtP -> P_456 * _rtB -> B_45_529_0 ; _rtB ->
B_45_565_0 = _rtP -> P_457 * _rtB -> B_45_19_0 [ 35 ] ; _rtB -> B_45_566_0 =
_rtP -> P_458 * _rtB -> B_45_19_0 [ 36 ] ; _rtB -> B_45_567_0 = _rtP -> P_459
* _rtB -> B_45_19_0 [ 37 ] ; _rtB -> B_45_568_0 [ 0 ] = _rtP -> P_460 * _rtB
-> B_45_565_0 ; _rtB -> B_45_568_0 [ 1 ] = _rtP -> P_460 * _rtB -> B_45_566_0
; _rtB -> B_45_568_0 [ 2 ] = _rtP -> P_460 * _rtB -> B_45_567_0 ; _rtB ->
B_45_569_0 = _rtP -> P_461 * _rtB -> B_45_19_0 [ 14 ] ; _rtB -> B_45_570_0 =
_rtP -> P_462 * _rtB -> B_45_19_0 [ 15 ] ; _rtB -> B_45_571_0 = _rtP -> P_463
* _rtB -> B_45_19_0 [ 16 ] ; _rtB -> B_45_572_0 [ 0 ] = _rtP -> P_464 * _rtB
-> B_45_569_0 ; _rtB -> B_45_572_0 [ 1 ] = _rtP -> P_464 * _rtB -> B_45_570_0
; _rtB -> B_45_572_0 [ 2 ] = _rtP -> P_464 * _rtB -> B_45_571_0 ; _rtB ->
B_45_607_0 = _rtP -> P_465 * _rtB -> B_45_19_0 [ 38 ] ; _rtB -> B_45_608_0 =
_rtP -> P_466 * _rtB -> B_45_19_0 [ 39 ] ; _rtB -> B_45_609_0 = _rtP -> P_467
* _rtB -> B_45_19_0 [ 40 ] ; _rtB -> B_45_610_0 [ 0 ] = _rtP -> P_468 * _rtB
-> B_45_607_0 ; _rtB -> B_45_610_0 [ 1 ] = _rtP -> P_468 * _rtB -> B_45_608_0
; _rtB -> B_45_610_0 [ 2 ] = _rtP -> P_468 * _rtB -> B_45_609_0 ; _rtB ->
B_45_611_0 = _rtP -> P_469 * _rtB -> B_45_19_0 [ 17 ] ; _rtB -> B_45_612_0 =
_rtP -> P_470 * _rtB -> B_45_19_0 [ 18 ] ; _rtB -> B_45_613_0 = _rtP -> P_471
* _rtB -> B_45_19_0 [ 19 ] ; _rtB -> B_45_614_0 [ 0 ] = _rtP -> P_472 * _rtB
-> B_45_611_0 ; _rtB -> B_45_614_0 [ 1 ] = _rtP -> P_472 * _rtB -> B_45_612_0
; _rtB -> B_45_614_0 [ 2 ] = _rtP -> P_472 * _rtB -> B_45_613_0 ; _rtB ->
B_45_649_0 = _rtP -> P_473 * _rtB -> B_45_19_0 [ 41 ] ; _rtB -> B_45_650_0 =
_rtP -> P_474 * _rtB -> B_45_19_0 [ 42 ] ; _rtB -> B_45_651_0 = _rtP -> P_475
* _rtB -> B_45_19_0 [ 43 ] ; _rtB -> B_45_652_0 [ 0 ] = _rtP -> P_476 * _rtB
-> B_45_649_0 ; _rtB -> B_45_652_0 [ 1 ] = _rtP -> P_476 * _rtB -> B_45_650_0
; _rtB -> B_45_652_0 [ 2 ] = _rtP -> P_476 * _rtB -> B_45_651_0 ; _rtB ->
B_45_653_0 = _rtP -> P_477 * _rtB -> B_45_19_0 [ 20 ] ; _rtB -> B_45_654_0 =
_rtP -> P_478 * _rtB -> B_45_19_0 [ 21 ] ; _rtB -> B_45_655_0 = _rtP -> P_479
* _rtB -> B_45_19_0 [ 22 ] ; _rtB -> B_45_656_0 [ 0 ] = _rtP -> P_480 * _rtB
-> B_45_653_0 ; _rtB -> B_45_656_0 [ 1 ] = _rtP -> P_480 * _rtB -> B_45_654_0
; _rtB -> B_45_656_0 [ 2 ] = _rtP -> P_480 * _rtB -> B_45_655_0 ; _rtB ->
B_45_691_0 = _rtP -> P_481 * _rtB -> B_45_19_0 [ 44 ] ; _rtB -> B_45_692_0 =
_rtP -> P_482 * _rtB -> B_45_19_0 [ 45 ] ; _rtB -> B_45_693_0 = _rtP -> P_483
* _rtB -> B_45_19_0 [ 46 ] ; _rtB -> B_45_694_0 [ 0 ] = _rtP -> P_484 * _rtB
-> B_45_691_0 ; _rtB -> B_45_694_0 [ 1 ] = _rtP -> P_484 * _rtB -> B_45_692_0
; _rtB -> B_45_694_0 [ 2 ] = _rtP -> P_484 * _rtB -> B_45_693_0 ; _rtB ->
B_45_695_0 = _rtP -> P_485 * _rtB -> B_45_19_0 [ 23 ] ; _rtB -> B_45_696_0 =
_rtP -> P_486 * _rtB -> B_45_19_0 [ 24 ] ; _rtB -> B_45_697_0 = _rtP -> P_487
* _rtB -> B_45_19_0 [ 25 ] ; _rtB -> B_45_698_0 [ 0 ] = _rtP -> P_488 * _rtB
-> B_45_695_0 ; _rtB -> B_45_698_0 [ 1 ] = _rtP -> P_488 * _rtB -> B_45_696_0
; _rtB -> B_45_698_0 [ 2 ] = _rtP -> P_488 * _rtB -> B_45_697_0 ; _rtB ->
B_45_733_0 = _rtP -> P_489 * _rtB -> B_45_19_0 [ 47 ] ; _rtB -> B_45_734_0 =
_rtP -> P_490 * _rtB -> B_45_19_0 [ 48 ] ; _rtB -> B_45_735_0 = _rtP -> P_491
* _rtB -> B_45_19_0 [ 49 ] ; _rtB -> B_45_736_0 [ 0 ] = _rtP -> P_492 * _rtB
-> B_45_733_0 ; _rtB -> B_45_736_0 [ 1 ] = _rtP -> P_492 * _rtB -> B_45_734_0
; _rtB -> B_45_736_0 [ 2 ] = _rtP -> P_492 * _rtB -> B_45_735_0 ; _rtB ->
B_45_737_0 = _rtP -> P_493 * _rtB -> B_45_19_0 [ 26 ] ; _rtB -> B_45_738_0 =
_rtP -> P_494 * _rtB -> B_45_19_0 [ 27 ] ; _rtB -> B_45_739_0 = _rtP -> P_495
* _rtB -> B_45_19_0 [ 28 ] ; _rtB -> B_45_740_0 [ 0 ] = _rtP -> P_496 * _rtB
-> B_45_737_0 ; _rtB -> B_45_740_0 [ 1 ] = _rtP -> P_496 * _rtB -> B_45_738_0
; _rtB -> B_45_740_0 [ 2 ] = _rtP -> P_496 * _rtB -> B_45_739_0 ; _rtB ->
B_45_775_0 = _rtP -> P_497 ; _rtB -> B_45_776_0 = ssGetTaskTime ( S , 2 ) ;
_rtB -> B_45_777_0 = rt_Lookup ( _rtP -> P_498 , 6 , _rtB -> B_45_776_0 ,
_rtP -> P_499 ) ; if ( _rtB -> B_45_81_0 >= _rtP -> P_500 ) { _rtB ->
B_45_778_0 = _rtB -> B_45_86_0 ; } else { _rtB -> B_45_778_0 = _rtB ->
B_45_777_0 ; } _rtB -> B_45_779_0 = ssGetTaskTime ( S , 2 ) ; if ( _rtB ->
B_45_775_0 >= _rtP -> P_501 ) { if ( _rtB -> B_45_82_0_i >= _rtP -> P_24 ) {
_rtB -> B_29_0_0 = _rtB -> B_45_778_0 ; } else { _rtB -> B_29_0_0 = _rtB ->
B_45_85_0_m ; } _rtB -> B_29_1_0 = _rtB -> B_29_0_0 ; _rtB -> B_45_782_0 =
_rtB -> B_29_1_0 ; } else { _rtB -> B_28_0_0 = rt_Lookup ( _rtP -> P_22 , 5 ,
_rtB -> B_45_779_0 , _rtP -> P_23 ) ; _rtB -> B_45_782_0 = _rtB -> B_28_0_0 ;
} _rtB -> B_45_786_0 = _rtP -> P_502 ; _rtB -> B_45_787_0 = ssGetTaskTime ( S
, 2 ) ; if ( _rtB -> B_45_786_0 >= _rtP -> P_503 ) { if ( _rtB -> B_45_83_0_o
>= _rtP -> P_27 ) { _rtB -> B_31_0_0 = _rtB -> B_45_778_0 ; } else { _rtB ->
B_31_0_0 = _rtB -> B_45_85_0_m ; } _rtB -> B_31_1_0 = _rtB -> B_31_0_0 ; _rtB
-> B_45_790_0 = _rtB -> B_31_1_0 ; } else { _rtB -> B_30_0_0 = rt_Lookup (
_rtP -> P_25 , 5 , _rtB -> B_45_787_0 , _rtP -> P_26 ) ; _rtB -> B_45_790_0 =
_rtB -> B_30_0_0 ; } _rtB -> B_45_794_0 = _rtP -> P_504 ; _rtB -> B_45_795_0
= ssGetTaskTime ( S , 2 ) ; if ( _rtB -> B_45_794_0 >= _rtP -> P_505 ) { if (
_rtB -> B_45_84_0_n >= _rtP -> P_30 ) { _rtB -> B_33_0_0 = _rtB -> B_45_778_0
; } else { _rtB -> B_33_0_0 = _rtB -> B_45_85_0_m ; } _rtB -> B_33_1_0 = _rtB
-> B_33_0_0 ; _rtB -> B_45_798_0 = _rtB -> B_33_1_0 ; } else { _rtB ->
B_32_0_0 = rt_Lookup ( _rtP -> P_28 , 5 , _rtB -> B_45_795_0 , _rtP -> P_29 )
; _rtB -> B_45_798_0 = _rtB -> B_32_0_0 ; } } UNUSED_PARAMETER ( tid ) ; }
static void mdlOutputsTID3 ( SimStruct * S , int_T tid ) { int32_T i ;
B_DZG_v6_SimGen_T * _rtB ; P_DZG_v6_SimGen_T * _rtP ; _rtP = ( (
P_DZG_v6_SimGen_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( ( B_DZG_v6_SimGen_T *
) _ssGetModelBlockIO ( S ) ) ; for ( i = 0 ; i < 6 ; i ++ ) { _rtB ->
B_45_0_0_m [ i ] = _rtP -> P_506 [ i ] ; } _rtB -> B_45_1_0_c = _rtP -> P_507
; _rtB -> B_45_2_0_k = _rtP -> P_508 ; _rtB -> B_45_3_0_c = _rtP -> P_509 ;
_rtB -> B_45_4_0_b = _rtP -> P_510 ; _rtB -> B_45_5_0_p = _rtP -> P_511 ;
_rtB -> B_45_6_0_c = _rtP -> P_512 ; _rtB -> B_45_7_0 = _rtP -> P_513 ;
ssCallAccelRunBlock ( S , 45 , 832 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_45_9_0_a = _rtP -> P_559 ; _rtB -> B_45_10_0_f = _rtP -> P_514 ; _rtB ->
B_45_11_0_g = _rtP -> P_515 ; _rtB -> B_45_12_0_g = _rtP -> P_516 * _rtB ->
B_45_11_0_g ; _rtB -> B_45_13_0_m = _rtP -> P_517 ; _rtB -> B_45_14_0_n =
_rtP -> P_518 ; _rtB -> B_45_15_0_p = _rtP -> P_519 ; _rtB -> B_45_17_0_j =
_rtP -> P_521 ; _rtB -> B_45_19_0_g = _rtP -> P_523 ; _rtB -> B_45_20_0_l [ 0
] = _rtP -> P_524 [ 0 ] ; _rtB -> B_45_21_0_d [ 0 ] = _rtP -> P_525 [ 0 ] ;
_rtB -> B_45_20_0_l [ 1 ] = _rtP -> P_524 [ 1 ] ; _rtB -> B_45_21_0_d [ 1 ] =
_rtP -> P_525 [ 1 ] ; _rtB -> B_37_0_0_o = _rtP -> P_556 ; _rtB -> B_37_1_0_i
= _rtP -> P_557 ; _rtB -> B_37_2_0_f = _rtP -> P_558 ; _rtB -> B_37_3_0_c =
_rtP -> P_49 ; _rtB -> B_37_4_0_m [ 0 ] = _rtP -> P_50 [ 0 ] ; _rtB ->
B_37_4_0_m [ 1 ] = _rtP -> P_50 [ 1 ] ; _rtB -> B_36_0_0_c = _rtP -> P_38 ;
_rtB -> B_36_1_0_p [ 0 ] = _rtP -> P_39 [ 0 ] ; _rtB -> B_36_1_0_p [ 1 ] =
_rtP -> P_39 [ 1 ] ; _rtB -> B_36_1_0_p [ 2 ] = _rtP -> P_39 [ 2 ] ; _rtB ->
B_37_9_0_c = _rtP -> P_54 ; _rtB -> B_37_10_0 = _rtP -> P_55 ; memcpy ( &
_rtB -> B_45_16_0_l [ 0 ] , & _rtP -> P_520 [ 0 ] , 36U * sizeof ( real_T ) )
; memcpy ( & _rtB -> B_45_18_0_d [ 0 ] , & _rtP -> P_522 [ 0 ] , 36U * sizeof
( real_T ) ) ; memcpy ( & _rtB -> B_45_22_0_d [ 0 ] , & _rtP -> P_526 [ 0 ] ,
36U * sizeof ( real_T ) ) ; memcpy ( & _rtB -> B_37_6_0_m [ 0 ] , & _rtP ->
P_51 [ 0 ] , 36U * sizeof ( real_T ) ) ; memcpy ( & _rtB -> B_37_7_0_j [ 0 ]
, & _rtP -> P_52 [ 0 ] , 36U * sizeof ( real_T ) ) ; memcpy ( & _rtB ->
B_37_8_0_h [ 0 ] , & _rtP -> P_53 [ 0 ] , 36U * sizeof ( real_T ) ) ; memcpy
( & _rtB -> B_45_24_0 [ 0 ] , & _rtP -> P_527 [ 0 ] , 36U * sizeof ( real_T )
) ; _rtB -> B_45_25_0_l [ 0 ] = _rtP -> P_528 [ 0 ] ; _rtB -> B_45_25_0_l [ 1
] = _rtP -> P_528 [ 1 ] ; _rtB -> B_45_25_0_l [ 2 ] = _rtP -> P_528 [ 2 ] ;
_rtB -> B_45_26_0 = _rtP -> P_529 ; _rtB -> B_45_27_0_o = _rtP -> P_530 ;
_rtB -> B_45_28_0_b = _rtP -> P_531 ; _rtB -> B_45_29_0_n = _rtP -> P_532 ;
_rtB -> B_45_30_0_b = _rtP -> P_533 ; _rtB -> B_45_31_0_l = _rtP -> P_534 ;
_rtB -> B_45_32_0 = _rtP -> P_535 ; _rtB -> B_45_33_0_h = _rtP -> P_536 ;
_rtB -> B_45_34_0 = _rtP -> P_537 ; _rtB -> B_45_35_0_b = _rtP -> P_538 ;
_rtB -> B_45_36_0_i = ( _rtB -> B_45_33_0_h == _rtB -> B_45_35_0_b ) ; _rtB
-> B_45_37_0_d = _rtB -> B_45_36_0_i ; _rtB -> B_45_38_0_e = _rtP -> P_539 *
_rtB -> B_45_37_0_d ; _rtB -> B_45_39_0_b = _rtP -> P_540 ; _rtB ->
B_45_40_0_j = _rtP -> P_541 ; _rtB -> B_45_41_0_f = _rtP -> P_542 ; _rtB ->
B_45_42_0_a = _rtP -> P_543 ; _rtB -> B_45_43_0 = _rtP -> P_544 ;
ssCallAccelRunBlock ( S , 45 , 867 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 868 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 869 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 870 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 871 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 872 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 873 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 874 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 875 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 876 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 877 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 878 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 879 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 880 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 881 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 882 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 883 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 884 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 885 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 886 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 887 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 888 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 889 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 890 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 891 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 892 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 893 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 894 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 45 , 895 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_45_73_0_j = _rtP -> P_545 ; _rtB -> B_45_74_0_j = _rtP -> P_546 ; _rtB ->
B_45_75_0_o = _rtP -> P_547 ; _rtB -> B_45_76_0_n = _rtP -> P_548 ; _rtB ->
B_45_77_0_l = ( _rtB -> B_45_76_0_n != 0.0 ) ; _rtB -> B_45_78_0_o = ( _rtB
-> B_45_75_0_o != 0.0 ) ; _rtB -> B_45_79_0 = ( _rtB -> B_45_74_0_j != 0.0 )
; _rtB -> B_45_80_0 = _rtP -> P_549 ; _rtB -> B_45_81_0 = _rtP -> P_550 ;
_rtB -> B_45_82_0_i = _rtP -> P_551 ; _rtB -> B_45_83_0_o = _rtP -> P_552 ;
_rtB -> B_45_84_0_n = _rtP -> P_553 ; _rtB -> B_45_85_0_m = _rtP -> P_554 ;
_rtB -> B_45_86_0 = _rtP -> P_555 ; UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdate ( SimStruct * S , int_T tid ) { real_T HoldSine ;
real_T HoldCosine ; int32_T i ; B_DZG_v6_SimGen_T * _rtB ; P_DZG_v6_SimGen_T
* _rtP ; DW_DZG_v6_SimGen_T * _rtDW ; _rtDW = ( ( DW_DZG_v6_SimGen_T * )
ssGetRootDWork ( S ) ) ; _rtP = ( ( P_DZG_v6_SimGen_T * ) ssGetModelRtp ( S )
) ; _rtB = ( ( B_DZG_v6_SimGen_T * ) _ssGetModelBlockIO ( S ) ) ; i =
ssIsSampleHit ( S , 2 , 0 ) ; if ( i != 0 ) { _rtDW ->
Rotorangledthetae_DSTATE += _rtP -> P_60 * _rtB -> B_45_329_0 ; for ( i = 0 ;
i < 6 ; i ++ ) { _rtDW -> fluxes_DSTATE [ i ] = _rtB -> B_45_313_0 [ i ] ; }
if ( _rtB -> B_45_6_0 > 0.0 ) { _rtDW -> Lmd_sat_DSTATE = _rtB -> B_37_12_0 ;
if ( _rtB -> B_37_0_0 ) { _rtDW -> Lmq_sat_DSTATE = _rtB -> B_36_11_0 ; } }
HoldSine = _rtDW -> lastSin ; HoldCosine = _rtDW -> lastCos ; _rtDW ->
lastSin = HoldSine * _rtP -> P_71 + HoldCosine * _rtP -> P_70 ; _rtDW ->
lastCos = HoldCosine * _rtP -> P_71 - HoldSine * _rtP -> P_70 ; HoldSine =
_rtDW -> lastSin_m ; HoldCosine = _rtDW -> lastCos_f ; _rtDW -> lastSin_m =
HoldSine * _rtP -> P_78 + HoldCosine * _rtP -> P_77 ; _rtDW -> lastCos_f =
HoldCosine * _rtP -> P_78 - HoldSine * _rtP -> P_77 ; HoldSine = _rtDW ->
lastSin_b ; HoldCosine = _rtDW -> lastCos_d ; _rtDW -> lastSin_b = HoldSine *
_rtP -> P_85 + HoldCosine * _rtP -> P_84 ; _rtDW -> lastCos_d = HoldCosine *
_rtP -> P_85 - HoldSine * _rtP -> P_84 ; ssCallAccelRunBlock ( S , 45 , 19 ,
SS_CALL_MDL_UPDATE ) ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * *
) & _rtDW -> TransportDelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK . Head = ( ( _rtDW ->
TransportDelay_IWORK . Head < ( _rtDW -> TransportDelay_IWORK .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK . Head + 1 ) : 0 )
; if ( _rtDW -> TransportDelay_IWORK . Head == _rtDW -> TransportDelay_IWORK
. Tail ) { if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK . CircularBufSize , & _rtDW -> TransportDelay_IWORK .
Tail , & _rtDW -> TransportDelay_IWORK . Head , & _rtDW ->
TransportDelay_IWORK . Last , simTime - _rtP -> P_99 , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK . Head ] = _rtB ->
B_45_20_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput = _rtB -> B_45_27_0 ; } { real_T * * uBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_d . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_d . TUbufferPtrs [ 1
] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_d . Head =
( ( _rtDW -> TransportDelay_IWORK_d . Head < ( _rtDW ->
TransportDelay_IWORK_d . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_d . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_d . Head == _rtDW -> TransportDelay_IWORK_d . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_d . CircularBufSize , & _rtDW -> TransportDelay_IWORK_d
. Tail , & _rtDW -> TransportDelay_IWORK_d . Head , & _rtDW ->
TransportDelay_IWORK_d . Last , simTime - _rtP -> P_104 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_d .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_d . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_d . Head ] = _rtB ->
B_45_28_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_e = _rtB -> B_45_35_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_m . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_e .
Head = ( ( _rtDW -> TransportDelay_IWORK_e . Head < ( _rtDW ->
TransportDelay_IWORK_e . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_e . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_e . Head == _rtDW -> TransportDelay_IWORK_e . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_e . CircularBufSize , & _rtDW -> TransportDelay_IWORK_e
. Tail , & _rtDW -> TransportDelay_IWORK_e . Head , & _rtDW ->
TransportDelay_IWORK_e . Last , simTime - _rtP -> P_110 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_e .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_e . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_e . Head ] = _rtB ->
B_45_39_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_m = _rtB -> B_45_46_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_e . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_eb .
Head = ( ( _rtDW -> TransportDelay_IWORK_eb . Head < ( _rtDW ->
TransportDelay_IWORK_eb . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_eb . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_eb . Head == _rtDW -> TransportDelay_IWORK_eb . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_eb . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_eb . Tail , & _rtDW -> TransportDelay_IWORK_eb . Head ,
& _rtDW -> TransportDelay_IWORK_eb . Last , simTime - _rtP -> P_115 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_eb . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_eb . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_eb . Head ] = _rtB -> B_45_47_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_k = _rtB ->
B_45_54_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_g . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_g . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_f . Head = ( ( _rtDW ->
TransportDelay_IWORK_f . Head < ( _rtDW -> TransportDelay_IWORK_f .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_f . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_f . Head == _rtDW ->
TransportDelay_IWORK_f . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_f . CircularBufSize , & _rtDW -> TransportDelay_IWORK_f
. Tail , & _rtDW -> TransportDelay_IWORK_f . Head , & _rtDW ->
TransportDelay_IWORK_f . Last , simTime - _rtP -> P_134 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_f .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_f . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_f . Head ] = _rtB ->
B_45_82_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_l = _rtB -> B_45_89_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_j . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_j . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_h .
Head = ( ( _rtDW -> TransportDelay_IWORK_h . Head < ( _rtDW ->
TransportDelay_IWORK_h . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_h . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_h . Head == _rtDW -> TransportDelay_IWORK_h . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_h . CircularBufSize , & _rtDW -> TransportDelay_IWORK_h
. Tail , & _rtDW -> TransportDelay_IWORK_h . Head , & _rtDW ->
TransportDelay_IWORK_h . Last , simTime - _rtP -> P_139 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_h .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_h . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_h . Head ] = _rtB ->
B_45_90_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_c = _rtB -> B_45_97_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_k . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_o .
Head = ( ( _rtDW -> TransportDelay_IWORK_o . Head < ( _rtDW ->
TransportDelay_IWORK_o . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_o . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_o . Head == _rtDW -> TransportDelay_IWORK_o . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_o . CircularBufSize , & _rtDW -> TransportDelay_IWORK_o
. Tail , & _rtDW -> TransportDelay_IWORK_o . Head , & _rtDW ->
TransportDelay_IWORK_o . Last , simTime - _rtP -> P_146 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_o .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_o . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_o . Head ] = _rtB ->
B_45_103_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_lq = _rtB -> B_45_110_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_kj . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_kj .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dv . Head = ( ( _rtDW -> TransportDelay_IWORK_dv . Head
< ( _rtDW -> TransportDelay_IWORK_dv . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dv . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dv . Head == _rtDW -> TransportDelay_IWORK_dv . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dv . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dv . Tail , & _rtDW -> TransportDelay_IWORK_dv . Head ,
& _rtDW -> TransportDelay_IWORK_dv . Last , simTime - _rtP -> P_151 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dv . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dv . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dv . Head ] = _rtB -> B_45_111_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_cg = _rtB ->
B_45_118_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ks . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ks . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_fq . Head = ( ( _rtDW
-> TransportDelay_IWORK_fq . Head < ( _rtDW -> TransportDelay_IWORK_fq .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_fq . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_fq . Head == _rtDW ->
TransportDelay_IWORK_fq . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_fq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fq . Tail , & _rtDW -> TransportDelay_IWORK_fq . Head ,
& _rtDW -> TransportDelay_IWORK_fq . Last , simTime - _rtP -> P_158 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_fq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_fq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_fq . Head ] = _rtB -> B_45_124_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_n = _rtB ->
B_45_131_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_l . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_l . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_hg . Head = ( ( _rtDW ->
TransportDelay_IWORK_hg . Head < ( _rtDW -> TransportDelay_IWORK_hg .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_hg . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_hg . Head == _rtDW ->
TransportDelay_IWORK_hg . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hg . Tail , & _rtDW -> TransportDelay_IWORK_hg . Head ,
& _rtDW -> TransportDelay_IWORK_hg . Last , simTime - _rtP -> P_163 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hg . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hg . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hg . Head ] = _rtB -> B_45_132_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_c0 = _rtB ->
B_45_139_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_h . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_h . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_a . Head = ( ( _rtDW ->
TransportDelay_IWORK_a . Head < ( _rtDW -> TransportDelay_IWORK_a .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_a . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_a . Head == _rtDW ->
TransportDelay_IWORK_a . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_a . CircularBufSize , & _rtDW -> TransportDelay_IWORK_a
. Tail , & _rtDW -> TransportDelay_IWORK_a . Head , & _rtDW ->
TransportDelay_IWORK_a . Last , simTime - _rtP -> P_170 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_a .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_a . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_a . Head ] = _rtB ->
B_45_152_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_i = _rtB -> B_45_159_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_a . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_a . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_n .
Head = ( ( _rtDW -> TransportDelay_IWORK_n . Head < ( _rtDW ->
TransportDelay_IWORK_n . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_n . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_n . Head == _rtDW -> TransportDelay_IWORK_n . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_n . CircularBufSize , & _rtDW -> TransportDelay_IWORK_n
. Tail , & _rtDW -> TransportDelay_IWORK_n . Head , & _rtDW ->
TransportDelay_IWORK_n . Last , simTime - _rtP -> P_175 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_n .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_n . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_n . Head ] = _rtB ->
B_45_160_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_ei = _rtB -> B_45_167_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ar . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ar .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_b . Head = ( ( _rtDW -> TransportDelay_IWORK_b . Head <
( _rtDW -> TransportDelay_IWORK_b . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_b . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_b . Head == _rtDW -> TransportDelay_IWORK_b . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_b . CircularBufSize , & _rtDW -> TransportDelay_IWORK_b
. Tail , & _rtDW -> TransportDelay_IWORK_b . Head , & _rtDW ->
TransportDelay_IWORK_b . Last , simTime - _rtP -> P_182 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_b .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_b . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_b . Head ] = _rtB ->
B_45_173_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_a = _rtB -> B_45_180_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_o . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_hs .
Head = ( ( _rtDW -> TransportDelay_IWORK_hs . Head < ( _rtDW ->
TransportDelay_IWORK_hs . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_hs . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hs . Head == _rtDW -> TransportDelay_IWORK_hs . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hs . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hs . Tail , & _rtDW -> TransportDelay_IWORK_hs . Head ,
& _rtDW -> TransportDelay_IWORK_hs . Last , simTime - _rtP -> P_187 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hs . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hs . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hs . Head ] = _rtB -> B_45_181_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_in = _rtB ->
B_45_188_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_i . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_i . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_ed . Head = ( ( _rtDW ->
TransportDelay_IWORK_ed . Head < ( _rtDW -> TransportDelay_IWORK_ed .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_ed . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_ed . Head == _rtDW ->
TransportDelay_IWORK_ed . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ed . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ed . Tail , & _rtDW -> TransportDelay_IWORK_ed . Head ,
& _rtDW -> TransportDelay_IWORK_ed . Last , simTime - _rtP -> P_194 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ed . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ed . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ed . Head ] = _rtB -> B_45_194_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_f = _rtB ->
B_45_201_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_lb . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_lb . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_ns . Head = ( ( _rtDW
-> TransportDelay_IWORK_ns . Head < ( _rtDW -> TransportDelay_IWORK_ns .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_ns . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_ns . Head == _rtDW ->
TransportDelay_IWORK_ns . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ns . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ns . Tail , & _rtDW -> TransportDelay_IWORK_ns . Head ,
& _rtDW -> TransportDelay_IWORK_ns . Last , simTime - _rtP -> P_199 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ns . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ns . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ns . Head ] = _rtB -> B_45_202_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_iw = _rtB ->
B_45_209_0 ; } i = ssIsSampleHit ( S , 2 , 0 ) ; if ( i != 0 ) { _rtDW ->
dw_delay_DSTATE = _rtB -> B_45_328_0 ; _rtDW -> dw_predict_DSTATE = _rtB ->
B_45_305_0 ; _rtDW -> voltages_DSTATE [ 0 ] = _rtB -> B_45_279_0 ; _rtDW ->
voltages_DSTATE [ 1 ] = _rtB -> B_45_280_0 ; _rtDW -> voltages_DSTATE [ 2 ] =
_rtB -> B_45_12_0_g ; _rtDW -> voltages_DSTATE [ 3 ] = _rtB -> B_45_25_0_l [
0 ] ; _rtDW -> voltages_DSTATE [ 4 ] = _rtB -> B_45_25_0_l [ 1 ] ; _rtDW ->
voltages_DSTATE [ 5 ] = _rtB -> B_45_25_0_l [ 2 ] ; _rtDW -> theta_DSTATE +=
_rtP -> P_294 * _rtB -> B_45_330_0 ; _rtDW ->
Rotorspeeddeviationdw_SYSTEM_ENABLE = 0U ; _rtDW ->
Rotorspeeddeviationdw_DSTATE = _rtP -> P_300 * _rtB -> B_45_319_0 + _rtB ->
B_45_328_0 ; { real_T xnew [ 2 ] ; xnew [ 0 ] = ( _rtP -> P_304 [ 0 ] ) *
_rtDW -> DiscreteStateSpace_DSTATE [ 0 ] + ( _rtP -> P_304 [ 1 ] ) * _rtDW ->
DiscreteStateSpace_DSTATE [ 1 ] ; xnew [ 0 ] += ( _rtP -> P_305 [ 0 ] ) *
_rtB -> B_45_327_0 [ 3 ] ; xnew [ 1 ] = ( _rtP -> P_304 [ 2 ] ) * _rtDW ->
DiscreteStateSpace_DSTATE [ 0 ] + ( _rtP -> P_304 [ 3 ] ) * _rtDW ->
DiscreteStateSpace_DSTATE [ 1 ] ; xnew [ 1 ] += ( _rtP -> P_305 [ 1 ] ) *
_rtB -> B_45_327_0 [ 3 ] ; ( void ) memcpy ( & _rtDW ->
DiscreteStateSpace_DSTATE [ 0 ] , xnew , sizeof ( real_T ) * 2 ) ; } { real_T
xnew [ 1 ] ; xnew [ 0 ] = _rtP -> P_310 * _rtDW ->
DiscreteStateSpace_DSTATE_j ; xnew [ 0 ] += _rtP -> P_311 * _rtB ->
B_45_337_0 ; ( void ) memcpy ( & _rtDW -> DiscreteStateSpace_DSTATE_j , xnew
, sizeof ( real_T ) * 1 ) ; } { real_T xnew [ 1 ] ; xnew [ 0 ] = _rtP ->
P_316 * _rtDW -> DiscreteStateSpace_DSTATE_b ; xnew [ 0 ] += _rtP -> P_317 *
_rtB -> B_45_339_0 ; ( void ) memcpy ( & _rtDW -> DiscreteStateSpace_DSTATE_b
, xnew , sizeof ( real_T ) * 1 ) ; } { real_T xnew [ 1 ] ; xnew [ 0 ] = _rtP
-> P_322 * _rtDW -> DiscreteStateSpace_DSTATE_o ; xnew [ 0 ] += _rtP -> P_323
* _rtB -> B_45_341_0 ; ( void ) memcpy ( & _rtDW ->
DiscreteStateSpace_DSTATE_o , xnew , sizeof ( real_T ) * 1 ) ; } { real_T
xnew [ 1 ] ; xnew [ 0 ] = _rtP -> P_328 * _rtDW ->
DiscreteStateSpace_DSTATE_f ; xnew [ 0 ] += _rtP -> P_329 * _rtB ->
B_45_343_0 ; ( void ) memcpy ( & _rtDW -> DiscreteStateSpace_DSTATE_f , xnew
, sizeof ( real_T ) * 1 ) ; } { real_T xnew [ 3 ] ; xnew [ 0 ] = ( _rtP ->
P_333 [ 0 ] ) * _rtDW -> DiscreteStateSpace_DSTATE_bk [ 0 ] + ( _rtP -> P_333
[ 1 ] ) * _rtDW -> DiscreteStateSpace_DSTATE_bk [ 1 ] + ( _rtP -> P_333 [ 2 ]
) * _rtDW -> DiscreteStateSpace_DSTATE_bk [ 2 ] ; xnew [ 0 ] += ( _rtP ->
P_334 [ 0 ] ) * _rtB -> B_45_327_0 [ 3 ] ; xnew [ 1 ] = ( _rtP -> P_333 [ 3 ]
) * _rtDW -> DiscreteStateSpace_DSTATE_bk [ 0 ] + ( _rtP -> P_333 [ 4 ] ) *
_rtDW -> DiscreteStateSpace_DSTATE_bk [ 1 ] + ( _rtP -> P_333 [ 5 ] ) * _rtDW
-> DiscreteStateSpace_DSTATE_bk [ 2 ] ; xnew [ 1 ] += ( _rtP -> P_334 [ 1 ] )
* _rtB -> B_45_327_0 [ 3 ] ; xnew [ 2 ] = ( _rtP -> P_333 [ 6 ] ) * _rtDW ->
DiscreteStateSpace_DSTATE_bk [ 0 ] + ( _rtP -> P_333 [ 7 ] ) * _rtDW ->
DiscreteStateSpace_DSTATE_bk [ 1 ] + ( _rtP -> P_333 [ 8 ] ) * _rtDW ->
DiscreteStateSpace_DSTATE_bk [ 2 ] ; xnew [ 2 ] += ( _rtP -> P_334 [ 2 ] ) *
_rtB -> B_45_327_0 [ 3 ] ; ( void ) memcpy ( & _rtDW ->
DiscreteStateSpace_DSTATE_bk [ 0 ] , xnew , sizeof ( real_T ) * 3 ) ; } {
real_T xnew [ 1 ] ; xnew [ 0 ] = _rtP -> P_339 * _rtDW ->
DiscreteStateSpace_DSTATE_a ; xnew [ 0 ] += _rtP -> P_340 * _rtB ->
B_45_346_0 ; ( void ) memcpy ( & _rtDW -> DiscreteStateSpace_DSTATE_a , xnew
, sizeof ( real_T ) * 1 ) ; } { real_T xnew [ 1 ] ; xnew [ 0 ] = _rtP ->
P_345 * _rtDW -> DiscreteStateSpace_DSTATE_l ; xnew [ 0 ] += _rtP -> P_346 *
_rtB -> B_45_348_0 ; ( void ) memcpy ( & _rtDW -> DiscreteStateSpace_DSTATE_l
, xnew , sizeof ( real_T ) * 1 ) ; }
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer_Update ( S , _rtB ->
B_45_350_0 ) ; DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer_Update (
S , _rtB -> B_45_353_0 ) ;
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer_Update ( S , _rtB ->
B_45_358_0 ) ; DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer_Update (
S , _rtB -> B_45_361_0 ) ;
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer_Update ( S , _rtB ->
B_45_366_0 ) ; DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer_Update (
S , _rtB -> B_45_369_0 ) ; _rtDW -> UnitDelay2_DSTATE = _rtB -> B_45_404_0 ;
{ real_T xnew [ 1 ] ; xnew [ 0 ] = _rtP -> P_370 * _rtDW ->
DiscreteStateSpace_DSTATE_i ; xnew [ 0 ] += _rtP -> P_371 * _rtB ->
B_45_382_0 ; ( void ) memcpy ( & _rtDW -> DiscreteStateSpace_DSTATE_i , xnew
, sizeof ( real_T ) * 1 ) ; } _rtDW -> UnitDelay1_DSTATE = _rtB -> B_45_406_0
; { real_T xnew [ 1 ] ; xnew [ 0 ] = _rtP -> P_382 * _rtDW ->
DiscreteStateSpace_DSTATE_h ; xnew [ 0 ] += _rtP -> P_383 * _rtB ->
B_45_387_0 ; ( void ) memcpy ( & _rtDW -> DiscreteStateSpace_DSTATE_h , xnew
, sizeof ( real_T ) * 1 ) ; } { real_T xnew [ 1 ] ; xnew [ 0 ] = ( _rtP ->
P_389 ) * _rtDW -> DiscreteStateSpace_DSTATE_be ; xnew [ 0 ] += _rtP -> P_390
* _rtB -> B_45_403_0 ; ( void ) memcpy ( & _rtDW ->
DiscreteStateSpace_DSTATE_be , xnew , sizeof ( real_T ) * 1 ) ; } { real_T
xnew [ 1 ] ; xnew [ 0 ] = _rtP -> P_395 * _rtDW ->
DiscreteStateSpace_DSTATE_n ; xnew [ 0 ] += _rtP -> P_396 * _rtB ->
B_45_405_0 ; ( void ) memcpy ( & _rtDW -> DiscreteStateSpace_DSTATE_n , xnew
, sizeof ( real_T ) * 1 ) ; } } { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_aw . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_aw . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_l . Head = ( ( _rtDW
-> TransportDelay_IWORK_l . Head < ( _rtDW -> TransportDelay_IWORK_l .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_l . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_l . Head == _rtDW ->
TransportDelay_IWORK_l . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_l . CircularBufSize , & _rtDW -> TransportDelay_IWORK_l
. Tail , & _rtDW -> TransportDelay_IWORK_l . Head , & _rtDW ->
TransportDelay_IWORK_l . Last , simTime - _rtP -> P_428 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_l .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_l . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_l . Head ] = _rtB ->
B_45_428_0 ; } UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdateTID3 ( SimStruct * S , int_T tid ) { UNUSED_PARAMETER (
tid ) ; }
#define MDL_DERIVATIVES
static void mdlDerivatives ( SimStruct * S ) { boolean_T lsat ; boolean_T
usat ; B_DZG_v6_SimGen_T * _rtB ; P_DZG_v6_SimGen_T * _rtP ;
X_DZG_v6_SimGen_T * _rtX ; XDot_DZG_v6_SimGen_T * _rtXdot ; _rtXdot = ( (
XDot_DZG_v6_SimGen_T * ) ssGetdX ( S ) ) ; _rtX = ( ( X_DZG_v6_SimGen_T * )
ssGetContStates ( S ) ) ; _rtP = ( ( P_DZG_v6_SimGen_T * ) ssGetModelRtp ( S
) ) ; _rtB = ( ( B_DZG_v6_SimGen_T * ) _ssGetModelBlockIO ( S ) ) ; _rtXdot
-> integrator_CSTATE = _rtB -> B_45_238_0 ; _rtXdot -> integrator_CSTATE_i =
_rtB -> B_45_240_0 ; _rtXdot -> integrator_CSTATE_j = _rtB -> B_45_242_0 ;
_rtXdot -> integrator_CSTATE_l = _rtB -> B_45_244_0 ; _rtXdot ->
integrator_CSTATE_m = _rtB -> B_45_246_0 ; _rtXdot -> integrator_CSTATE_g =
_rtB -> B_45_248_0 ; _rtXdot -> integrator_CSTATE_a = _rtB -> B_45_250_0 ;
_rtXdot -> integrator_CSTATE_d = _rtB -> B_45_252_0 ; _rtXdot ->
integrator_CSTATE_l2 = _rtB -> B_45_254_0 ; _rtXdot -> integrator_CSTATE_if =
_rtB -> B_45_256_0 ; _rtXdot -> integrator_CSTATE_h = _rtB -> B_45_258_0 ;
_rtXdot -> integrator_CSTATE_b = _rtB -> B_45_260_0 ; _rtXdot ->
integrator_CSTATE_o = _rtB -> B_45_262_0 ; _rtXdot -> integrator_CSTATE_lx =
_rtB -> B_45_264_0 ; _rtXdot -> integrator_CSTATE_k = _rtB -> B_45_266_0 ;
_rtXdot -> integrator_CSTATE_mu = _rtB -> B_45_268_0 ; lsat = ( _rtX ->
IntegratorLimited_CSTATE <= _rtP -> P_402 ) ; usat = ( _rtX ->
IntegratorLimited_CSTATE >= _rtP -> P_401 ) ; if ( ( ( ! lsat ) && ( ! usat )
) || ( lsat && ( _rtB -> B_45_422_0 > 0.0 ) ) || ( usat && ( _rtB ->
B_45_422_0 < 0.0 ) ) ) { _rtXdot -> IntegratorLimited_CSTATE = _rtB ->
B_45_422_0 ; } else { _rtXdot -> IntegratorLimited_CSTATE = 0.0 ; } _rtXdot
-> Integrator1_CSTATE = _rtB -> B_45_419_0 ; _rtXdot -> Integrator1_CSTATE_d
= _rtB -> B_45_441_0 ; _rtXdot -> Integrator2_CSTATE = _rtB -> B_45_438_0 ;
_rtXdot -> Integrator1_CSTATE_c = _rtB -> B_45_425_0 ; _rtXdot ->
TransferFcn1_CSTATE = 0.0 ; _rtXdot -> TransferFcn1_CSTATE += _rtP -> P_420 *
_rtX -> TransferFcn1_CSTATE ; _rtXdot -> TransferFcn1_CSTATE += _rtB ->
B_45_439_0 ; _rtXdot -> Integrator1_CSTATE_l = _rtB -> B_45_435_0 ; _rtXdot
-> TransferFcn_CSTATE = 0.0 ; _rtXdot -> TransferFcn_CSTATE += _rtP -> P_426
* _rtX -> TransferFcn_CSTATE ; _rtXdot -> TransferFcn_CSTATE += _rtB ->
B_45_440_0 ; _rtXdot -> Integrator2_CSTATE_n = _rtB -> B_45_447_0 ; _rtXdot
-> Integrator1_CSTATE_h = _rtB -> B_45_445_0 ; _rtXdot -> Integrator3_CSTATE
= _rtB -> B_45_453_0 ; } static void mdlInitializeSizes ( SimStruct * S ) {
ssSetChecksumVal ( S , 0 , 2719559451U ) ; ssSetChecksumVal ( S , 1 ,
1592541733U ) ; ssSetChecksumVal ( S , 2 , 1059418388U ) ; ssSetChecksumVal (
S , 3 , 3073309777U ) ; { mxArray * slVerStructMat = NULL ; mxArray *
slStrMat = mxCreateString ( "simulink" ) ; char slVerChar [ 10 ] ; int status
= mexCallMATLAB ( 1 , & slVerStructMat , 1 , & slStrMat , "ver" ) ; if (
status == 0 ) { mxArray * slVerMat = mxGetField ( slVerStructMat , 0 ,
"Version" ) ; if ( slVerMat == NULL ) { status = 1 ; } else { status =
mxGetString ( slVerMat , slVerChar , 10 ) ; } } mxDestroyArray ( slStrMat ) ;
mxDestroyArray ( slVerStructMat ) ; if ( ( status == 1 ) || ( strcmp (
slVerChar , "10.1" ) != 0 ) ) { return ; } } ssSetOptions ( S ,
SS_OPTION_EXCEPTION_FREE_CODE ) ; if ( ssGetSizeofDWork ( S ) != sizeof (
DW_DZG_v6_SimGen_T ) ) { ssSetErrorStatus ( S ,
"Unexpected error: Internal DWork sizes do "
"not match for accelerator mex file." ) ; } if ( ssGetSizeofGlobalBlockIO ( S
) != sizeof ( B_DZG_v6_SimGen_T ) ) { ssSetErrorStatus ( S ,
"Unexpected error: Internal BlockIO sizes do "
"not match for accelerator mex file." ) ; } { int ssSizeofParams ;
ssGetSizeofParams ( S , & ssSizeofParams ) ; if ( ssSizeofParams != sizeof (
P_DZG_v6_SimGen_T ) ) { static char msg [ 256 ] ; sprintf ( msg ,
"Unexpected error: Internal Parameters sizes do "
"not match for accelerator mex file." ) ; } } _ssSetModelRtp ( S , ( real_T *
) & DZG_v6_SimGen_rtDefaultP ) ; rt_InitInfAndNaN ( sizeof ( real_T ) ) ; }
static void mdlInitializeSampleTimes ( SimStruct * S ) { slAccRegPrmChangeFcn
( S , mdlOutputsTID3 ) ; } static void mdlTerminate ( SimStruct * S ) { }
#include "simulink.c"
