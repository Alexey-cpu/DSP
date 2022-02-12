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
void DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer_Init ( SimStruct *
S , B_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T * localB ,
P_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T * localP ) { localB
-> B_49_1_0 = localP -> P_0 ; } void
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer ( SimStruct * S , real_T
rtu_Enable , real_T rtu_In1 ,
B_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T * localB ,
DW_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T * localDW ,
P_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T * localP ) { if (
rtu_Enable > 0.0 ) { { localB -> B_49_0_0 = localP -> P_4 * rtu_In1 ; } {
localB -> B_49_1_0 = localP -> P_9 * localB -> B_49_0_0 ; } if (
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
, 2 , 0 ) ; if ( isHit != 0 ) { memcpy ( & _rtB -> B_55_0_0 [ 0 ] , & _rtDW
-> UnitDelay_DSTATE [ 0 ] , 12U * sizeof ( real_T ) ) ; _rtB -> B_55_1_0 [ 0
] = _rtB -> B_55_93_0 [ 0 ] - _rtB -> B_55_0_0 [ 0 ] ; _rtB -> B_55_1_0 [ 1 ]
= _rtB -> B_55_93_0 [ 1 ] - _rtB -> B_55_0_0 [ 1 ] ; _rtB -> B_55_1_0 [ 2 ] =
_rtB -> B_55_93_0 [ 2 ] - _rtB -> B_55_0_0 [ 2 ] ; _rtB -> B_55_1_0 [ 3 ] =
_rtB -> B_55_93_0 [ 3 ] - _rtB -> B_55_0_0 [ 3 ] ; _rtB -> B_55_1_0 [ 4 ] =
_rtB -> B_55_95_0 [ 0 ] - _rtB -> B_55_0_0 [ 4 ] ; _rtB -> B_55_1_0 [ 5 ] =
_rtB -> B_55_95_0 [ 1 ] - _rtB -> B_55_0_0 [ 5 ] ; _rtB -> B_55_1_0 [ 6 ] =
_rtB -> B_55_95_0 [ 2 ] - _rtB -> B_55_0_0 [ 6 ] ; _rtB -> B_55_1_0 [ 7 ] =
_rtB -> B_55_95_0 [ 3 ] - _rtB -> B_55_0_0 [ 7 ] ; _rtB -> B_55_1_0 [ 8 ] =
_rtB -> B_55_97_0 [ 0 ] - _rtB -> B_55_0_0 [ 8 ] ; _rtB -> B_55_1_0 [ 9 ] =
_rtB -> B_55_97_0 [ 1 ] - _rtB -> B_55_0_0 [ 9 ] ; _rtB -> B_55_1_0 [ 10 ] =
_rtB -> B_55_97_0 [ 2 ] - _rtB -> B_55_0_0 [ 10 ] ; _rtB -> B_55_1_0 [ 11 ] =
_rtB -> B_55_97_0 [ 3 ] - _rtB -> B_55_0_0 [ 11 ] ; ssCallAccelRunBlock ( S ,
55 , 2 , SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_55_3_0 = _rtDW ->
Rotorangledthetae_DSTATE ; _rtB -> B_55_4_0 = ssGetTaskTime ( S , 2 ) ; _rtB
-> B_55_5_0 = _rtP -> P_123 * _rtB -> B_55_4_0 ; _rtB -> B_55_6_0 = _rtB ->
B_55_3_0 + _rtB -> B_55_5_0 ; muDoubleScalarSinCos ( _rtB -> B_55_6_0 , &
_rtB -> B_55_7_0 , & _rtB -> B_55_7_1 ) ; for ( i = 0 ; i < 6 ; i ++ ) { _rtB
-> B_55_8_0 [ i ] = _rtDW -> fluxes_DSTATE [ i ] ; } _rtB -> B_55_9_0 = _rtB
-> B_55_11_0_g ; if ( _rtB -> B_55_9_0 > 0.0 ) { _rtB -> B_47_0_0 = _rtB ->
B_47_0_0_h ; _rtB -> B_47_1_0 [ 0 ] = _rtP -> P_66 [ 0 ] * _rtB -> B_55_8_0 [
1 ] ; _rtB -> B_47_1_0 [ 1 ] = _rtP -> P_66 [ 1 ] * _rtB -> B_55_8_0 [ 2 ] ;
_rtB -> B_47_1_0 [ 2 ] = _rtP -> P_66 [ 2 ] * _rtB -> B_55_8_0 [ 3 ] ;
tmpForInput_idx_0 = _rtB -> B_47_1_0 [ 0 ] ; tmpForInput_idx_0 += _rtB ->
B_47_1_0 [ 1 ] ; tmpForInput_idx_0 += _rtB -> B_47_1_0 [ 2 ] ; _rtB ->
B_47_2_0 = tmpForInput_idx_0 ; _rtB -> B_47_3_0 = _rtDW -> Lmd_sat_DSTATE ;
tmpForInput_idx_0 = _rtB -> B_47_3_0 ; _rtB -> B_47_4_0 = 1.0 /
tmpForInput_idx_0 ; tmpForInput_idx_0 = _rtB -> B_47_4_0_i [ 0 ] ;
tmpForInput_idx_1 = _rtB -> B_47_4_0_i [ 1 ] ; tmpForInput_idx_2 = _rtB ->
B_47_4_0 ; tmpForInput_idx_0 += tmpForInput_idx_1 ; tmpForInput_idx_0 +=
tmpForInput_idx_2 ; _rtB -> B_47_5_0 = tmpForInput_idx_0 ; tmpForInput_idx_0
= _rtB -> B_47_5_0 ; _rtB -> B_47_6_0 = 1.0 / tmpForInput_idx_0 ; _rtB ->
B_47_7_0 = _rtB -> B_47_2_0 * _rtB -> B_47_6_0 ; _rtB -> B_47_8_0 =
muDoubleScalarAbs ( _rtB -> B_47_7_0 ) ; _rtB -> B_47_9_0 = look1_pbinlxpw (
_rtB -> B_47_8_0 , _rtP -> P_69 , _rtP -> P_68 , & _rtDW -> m_bpIndex , 1U )
; if ( _rtB -> B_47_9_0 != 0.0 ) { _rtB -> B_44_0_0 = _rtB -> B_47_8_0 / _rtB
-> B_47_9_0 ; _rtB -> B_47_11_0 = _rtB -> B_44_0_0 ; } else { _rtB ->
B_47_11_0 = _rtB -> B_47_3_0_a ; } _rtB -> B_47_12_0 = _rtP -> P_70 * _rtB ->
B_47_11_0 ; if ( _rtB -> B_47_0_0 ) { _rtB -> B_46_0_0 [ 0 ] = _rtP -> P_55 [
0 ] * _rtB -> B_55_8_0 [ 0 ] ; _rtB -> B_46_0_0 [ 1 ] = _rtP -> P_55 [ 1 ] *
_rtB -> B_55_8_0 [ 4 ] ; _rtB -> B_46_0_0 [ 2 ] = _rtP -> P_55 [ 2 ] * _rtB
-> B_55_8_0 [ 5 ] ; _rtB -> B_46_2_0 = _rtDW -> Lmq_sat_DSTATE ;
tmpForInput_idx_0 = _rtB -> B_46_0_0 [ 0 ] ; tmpForInput_idx_0_0 = _rtB ->
B_46_1_0_i [ 0 ] ; tmpForInput_idx_0 += _rtB -> B_46_0_0 [ 1 ] ;
tmpForInput_idx_1 = _rtB -> B_46_1_0_i [ 1 ] ; tmpForInput_idx_0 += _rtB ->
B_46_0_0 [ 2 ] ; tmpForInput_idx_2 = _rtB -> B_46_1_0_i [ 2 ] ; _rtB ->
B_46_1_0 = tmpForInput_idx_0 ; tmpForInput_idx_0 = _rtB -> B_46_2_0 ; _rtB ->
B_46_3_0 = 1.0 / tmpForInput_idx_0 ; tmpForInput_idx_3 = _rtB -> B_46_3_0 ;
tmpForInput_idx_0 = tmpForInput_idx_0_0 ; tmpForInput_idx_0 +=
tmpForInput_idx_1 ; tmpForInput_idx_0 += tmpForInput_idx_2 ;
tmpForInput_idx_0 += tmpForInput_idx_3 ; _rtB -> B_46_4_0 = tmpForInput_idx_0
; tmpForInput_idx_0 = _rtB -> B_46_4_0 ; _rtB -> B_46_5_0 = 1.0 /
tmpForInput_idx_0 ; _rtB -> B_46_6_0 = _rtB -> B_46_1_0 * _rtB -> B_46_5_0 ;
_rtB -> B_46_7_0 = muDoubleScalarAbs ( _rtB -> B_46_6_0 ) ; _rtB -> B_46_8_0
= look1_pbinlxpw ( _rtB -> B_46_7_0 , _rtP -> P_58 , _rtP -> P_57 , & _rtDW
-> m_bpIndex_o , 1U ) ; if ( _rtB -> B_46_8_0 != 0.0 ) { _rtB -> B_45_0_0 =
_rtB -> B_46_7_0 / _rtB -> B_46_8_0 ; _rtB -> B_46_10_0 = _rtB -> B_45_0_0 ;
} else { _rtB -> B_46_10_0 = _rtB -> B_46_0_0_f ; } _rtB -> B_46_11_0 = _rtP
-> P_59 * _rtB -> B_46_10_0 ; srUpdateBC ( _rtDW -> Lmq_sat_SubsysRanBC ) ; }
if ( _rtB -> B_47_1_0_l ) { _rtB -> B_47_14_0 = _rtB -> B_46_11_0 ; } else {
_rtB -> B_47_14_0 = _rtB -> B_47_10_0 ; } memcpy ( & _rtB -> B_47_15_0 [ 0 ]
, & _rtB -> B_47_6_0_l [ 0 ] , 36U * sizeof ( real_T ) ) ; for ( isHit = 0 ;
isHit < 3 ; isHit ++ ) { tmp_3 = tmp_4 [ isHit ] ; _rtB -> B_47_15_0 [ 6 *
tmp_3 ] = _rtB -> B_47_14_0 ; _rtB -> B_47_15_0 [ 6 * tmp_3 + 4 ] = _rtB ->
B_47_14_0 ; _rtB -> B_47_15_0 [ 6 * tmp_3 + 5 ] = _rtB -> B_47_14_0 ; }
memcpy ( & _rtB -> B_47_16_0 [ 0 ] , & _rtB -> B_47_15_0 [ 0 ] , 36U * sizeof
( real_T ) ) ; for ( isHit = 0 ; isHit < 3 ; isHit ++ ) { _rtB -> B_47_16_0 [
6 * ( isHit + 1 ) + 1 ] = _rtB -> B_47_12_0 ; _rtB -> B_47_16_0 [ 6 * ( isHit
+ 1 ) + 2 ] = _rtB -> B_47_12_0 ; _rtB -> B_47_16_0 [ 6 * ( isHit + 1 ) + 3 ]
= _rtB -> B_47_12_0 ; } for ( i = 0 ; i < 36 ; i ++ ) { _rtB -> B_47_17_0 [ i
] = _rtB -> B_47_16_0 [ i ] + _rtB -> B_47_7_0_o [ i ] ; } rt_invd6x6_snf (
_rtB -> B_47_17_0 , _rtB -> B_47_18_0 ) ; memcpy ( & tmp_1 [ 0 ] , & _rtB ->
B_47_8_0_o [ 0 ] , 36U * sizeof ( real_T ) ) ; memcpy ( & tmp_2 [ 0 ] , &
_rtB -> B_47_18_0 [ 0 ] , 36U * sizeof ( real_T ) ) ; for ( isHit = 0 ; isHit
< 6 ; isHit ++ ) { for ( i = 0 ; i < 6 ; i ++ ) { _rtB -> B_47_19_0 [ i + 6 *
isHit ] = 0.0 ; for ( i_0 = 0 ; i_0 < 6 ; i_0 ++ ) { _rtB -> B_47_19_0 [ i +
6 * isHit ] += tmp_1 [ 6 * i_0 + i ] * tmp_2 [ 6 * isHit + i_0 ] ; } } } if (
_rtB -> B_47_2_0_h ) { _rtB -> B_47_20_0 = _rtB -> B_46_5_0 ; } else { _rtB
-> B_47_20_0 = _rtB -> B_47_9_0_i ; } if ( ssIsMajorTimeStep ( S ) != 0 ) {
srUpdateBC ( _rtDW -> Saturation_SubsysRanBC ) ; } } for ( i = 0 ; i < 36 ; i
++ ) { if ( _rtB -> B_55_13_0_n >= _rtP -> P_125 ) { _rtB -> B_55_11_0 [ i ]
= _rtB -> B_47_18_0 [ i ] ; } else { _rtB -> B_55_11_0 [ i ] = _rtB ->
B_55_14_0_p [ i ] ; } tmp_1 [ i ] = _rtB -> B_55_11_0 [ i ] ; } for ( i = 0 ;
i < 6 ; i ++ ) { tmp [ i ] = _rtB -> B_55_8_0 [ i ] ; } for ( i = 0 ; i < 6 ;
i ++ ) { tmp_0 [ i ] = 0.0 ; for ( isHit = 0 ; isHit < 6 ; isHit ++ ) {
tmpForInput_idx_1 = tmp_0 [ i ] ; tmpForInput_idx_1 += tmp_1 [ 6 * isHit + i
] * tmp [ isHit ] ; tmp_0 [ i ] = tmpForInput_idx_1 ; } _rtB -> B_55_12_0 [ i
] = tmp_0 [ i ] ; _rtB -> B_55_13_0 [ i ] = _rtP -> P_126 [ i ] * _rtB ->
B_55_12_0 [ i ] ; } _rtB -> B_55_14_0 = _rtB -> B_55_13_0 [ 0 ] * _rtB ->
B_55_7_1 + _rtB -> B_55_13_0 [ 1 ] * _rtB -> B_55_7_0 ; _rtB -> B_55_15_0 = (
( - _rtB -> B_55_13_0 [ 0 ] - 1.7320508075688772 * _rtB -> B_55_13_0 [ 1 ] )
* _rtB -> B_55_7_1 + ( 1.7320508075688772 * _rtB -> B_55_13_0 [ 0 ] - _rtB ->
B_55_13_0 [ 1 ] ) * _rtB -> B_55_7_0 ) * 0.5 ; _rtB -> B_55_16_0 = ( 0.0 -
_rtB -> B_55_14_0 ) - _rtB -> B_55_15_0 ; _rtB -> B_55_17_0 [ 0 ] = _rtP ->
P_127 * _rtB -> B_55_14_0 ; _rtB -> B_55_17_0 [ 1 ] = _rtP -> P_127 * _rtB ->
B_55_15_0 ; _rtB -> B_55_17_0 [ 2 ] = _rtP -> P_127 * _rtB -> B_55_16_0 ;
isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_18_0 [
0 ] = _rtB -> B_55_17_0 [ 0 ] ; _rtB -> B_55_18_0 [ 1 ] = _rtB -> B_55_17_0 [
1 ] ; } } _rtB -> B_55_19_0 = ssGetT ( S ) ; tmpForInput_idx_0 =
ssGetTaskTime ( S , 0 ) ; if ( tmpForInput_idx_0 < _rtP -> P_128 ) { _rtB ->
B_55_20_0 = _rtP -> P_129 ; } else { _rtB -> B_55_20_0 = _rtP -> P_130 ; } if
( _rtB -> B_55_20_0 > _rtP -> P_131 ) { _rtB -> B_1_0_0 = _rtP -> P_1 * _rtB
-> B_55_19_0 ; _rtB -> B_55_23_0 = _rtB -> B_1_0_0 ; } else { _rtB -> B_0_0_0
= _rtP -> P_0 * _rtB -> B_55_19_0 ; _rtB -> B_55_23_0 = _rtB -> B_0_0_0 ; }
_rtB -> B_55_24_0 = muDoubleScalarSin ( _rtP -> P_134 * ssGetTaskTime ( S , 0
) + _rtP -> P_135 ) * _rtP -> P_132 + _rtP -> P_133 ; if ( _rtB -> B_55_20_0
> _rtP -> P_136 ) { _rtB -> B_55_25_0 = _rtB -> B_55_24_0 ; } else { _rtB ->
B_55_25_0 = 0.0 ; } _rtB -> B_55_26_0 [ 0 ] = ( _rtB -> B_55_23_0 + _rtB ->
B_55_90_0_c [ 0 ] ) + _rtB -> B_55_25_0 ; _rtB -> B_55_26_0 [ 1 ] = ( _rtB ->
B_55_23_0 + _rtB -> B_55_90_0_c [ 1 ] ) + _rtB -> B_55_25_0 ; _rtB ->
B_55_26_0 [ 2 ] = ( _rtB -> B_55_23_0 + _rtB -> B_55_90_0_c [ 2 ] ) + _rtB ->
B_55_25_0 ; _rtB -> B_55_27_0 [ 0 ] = muDoubleScalarSin ( _rtB -> B_55_26_0 [
0 ] ) ; _rtB -> B_55_27_0 [ 1 ] = muDoubleScalarSin ( _rtB -> B_55_26_0 [ 1 ]
) ; _rtB -> B_55_27_0 [ 2 ] = muDoubleScalarSin ( _rtB -> B_55_26_0 [ 2 ] ) ;
_rtB -> B_55_28_0 [ 0 ] = _rtP -> P_137 * _rtB -> B_55_27_0 [ 0 ] ; _rtB ->
B_55_28_0 [ 1 ] = _rtP -> P_137 * _rtB -> B_55_27_0 [ 1 ] ; _rtB -> B_55_28_0
[ 2 ] = _rtP -> P_137 * _rtB -> B_55_27_0 [ 2 ] ; isHit = ssIsSampleHit ( S ,
2 , 0 ) ; if ( isHit != 0 ) { if ( _rtDW -> systemEnable != 0 ) { _rtDW ->
lastSin = muDoubleScalarSin ( _rtP -> P_140 * ssGetTaskTime ( S , 2 ) ) ;
_rtDW -> lastCos = muDoubleScalarCos ( _rtP -> P_140 * ssGetTaskTime ( S , 2
) ) ; _rtDW -> systemEnable = 0 ; } _rtB -> B_55_29_0 = ( ( _rtDW -> lastSin
* _rtP -> P_144 + _rtDW -> lastCos * _rtP -> P_143 ) * _rtP -> P_142 + (
_rtDW -> lastCos * _rtP -> P_144 - _rtDW -> lastSin * _rtP -> P_143 ) * _rtP
-> P_141 ) * _rtP -> P_138 + _rtP -> P_139 ; if ( _rtDW -> systemEnable_m !=
0 ) { _rtDW -> lastSin_m = muDoubleScalarSin ( _rtP -> P_147 * ssGetTaskTime
( S , 2 ) ) ; _rtDW -> lastCos_f = muDoubleScalarCos ( _rtP -> P_147 *
ssGetTaskTime ( S , 2 ) ) ; _rtDW -> systemEnable_m = 0 ; } _rtB -> B_55_30_0
= ( ( _rtDW -> lastSin_m * _rtP -> P_151 + _rtDW -> lastCos_f * _rtP -> P_150
) * _rtP -> P_149 + ( _rtDW -> lastCos_f * _rtP -> P_151 - _rtDW -> lastSin_m
* _rtP -> P_150 ) * _rtP -> P_148 ) * _rtP -> P_145 + _rtP -> P_146 ; if (
_rtDW -> systemEnable_e != 0 ) { _rtDW -> lastSin_b = muDoubleScalarSin (
_rtP -> P_154 * ssGetTaskTime ( S , 2 ) ) ; _rtDW -> lastCos_d =
muDoubleScalarCos ( _rtP -> P_154 * ssGetTaskTime ( S , 2 ) ) ; _rtDW ->
systemEnable_e = 0 ; } _rtB -> B_55_31_0 = ( ( _rtDW -> lastSin_b * _rtP ->
P_158 + _rtDW -> lastCos_d * _rtP -> P_157 ) * _rtP -> P_156 + ( _rtDW ->
lastCos_d * _rtP -> P_158 - _rtDW -> lastSin_b * _rtP -> P_157 ) * _rtP ->
P_155 ) * _rtP -> P_152 + _rtP -> P_153 ; ssCallAccelRunBlock ( S , 55 , 32 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_55_33_0 = _rtX -> integrator_CSTATE ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_170 ; _rtB -> B_55_34_0 =
DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> TransportDelay_IWORK . CircularBufSize , & _rtDW ->
TransportDelay_IWORK . Last , _rtDW -> TransportDelay_IWORK . Tail , _rtDW ->
TransportDelay_IWORK . Head , _rtP -> P_171 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_55_35_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_55_36_0 = _rtP -> P_172 ; } _rtB ->
B_55_37_0_m = ( _rtB -> B_55_35_0 >= _rtB -> B_55_36_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_38_0 = _rtDW
-> Memory_PreviousInput ; } if ( _rtB -> B_55_37_0_m ) { _rtB -> B_7_0_0 =
_rtB -> B_55_33_0 - _rtB -> B_55_34_0 ; _rtB -> B_7_1_0 = _rtP -> P_7 * _rtB
-> B_7_0_0 ; _rtB -> B_55_40_0 = _rtB -> B_7_1_0 ; } else { _rtB -> B_55_40_0
= _rtB -> B_55_38_0 ; } _rtB -> B_55_41_0 = _rtX -> integrator_CSTATE_l ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_k . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_175 ; _rtB -> B_55_42_0 =
DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> TransportDelay_IWORK_c . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_c . Last , _rtDW -> TransportDelay_IWORK_c . Tail ,
_rtDW -> TransportDelay_IWORK_c . Head , _rtP -> P_176 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_55_43_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_55_44_0 = _rtP -> P_177 ; } _rtB ->
B_55_45_0_m = ( _rtB -> B_55_43_0 >= _rtB -> B_55_44_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_46_0 = _rtDW
-> Memory_PreviousInput_p ; } if ( _rtB -> B_55_45_0_m ) { _rtB -> B_6_0_0 =
_rtB -> B_55_41_0 - _rtB -> B_55_42_0 ; _rtB -> B_6_1_0 = _rtP -> P_6 * _rtB
-> B_6_0_0 ; _rtB -> B_55_48_0 = _rtB -> B_6_1_0 ; } else { _rtB -> B_55_48_0
= _rtB -> B_55_46_0 ; } _rtB -> B_55_49_0 . re = _rtB -> B_55_40_0 ; _rtB ->
B_55_49_0 . im = _rtB -> B_55_48_0 ; _rtB -> B_55_50_0 = muDoubleScalarHypot
( _rtB -> B_55_49_0 . re , _rtB -> B_55_49_0 . im ) ; _rtB -> B_55_50_1 =
muDoubleScalarAtan2 ( _rtB -> B_55_49_0 . im , _rtB -> B_55_49_0 . re ) ;
_rtB -> B_55_51_0 = _rtP -> P_179 * _rtB -> B_55_50_1 ; _rtB -> B_55_52_0 =
_rtX -> integrator_CSTATE_i ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_f . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_f . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_181 ; _rtB
-> B_55_53_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_e . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_e . Last , _rtDW -> TransportDelay_IWORK_e .
Tail , _rtDW -> TransportDelay_IWORK_e . Head , _rtP -> P_182 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_55_54_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_55_0 = _rtP ->
P_183 ; } _rtB -> B_55_56_0 = ( _rtB -> B_55_54_0 >= _rtB -> B_55_55_0 ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_57_0 =
_rtDW -> Memory_PreviousInput_l ; } if ( _rtB -> B_55_56_0 ) { _rtB ->
B_9_0_0 = _rtB -> B_55_52_0 - _rtB -> B_55_53_0 ; _rtB -> B_9_1_0 = _rtP ->
P_9 * _rtB -> B_9_0_0 ; _rtB -> B_55_59_0 = _rtB -> B_9_1_0 ; } else { _rtB
-> B_55_59_0 = _rtB -> B_55_57_0 ; } _rtB -> B_55_60_0 = _rtX ->
integrator_CSTATE_f ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_d . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_d . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_186 ; _rtB ->
B_55_61_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_j . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_j . Last , _rtDW -> TransportDelay_IWORK_j .
Tail , _rtDW -> TransportDelay_IWORK_j . Head , _rtP -> P_187 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_55_62_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_63_0 = _rtP ->
P_188 ; } _rtB -> B_55_64_0 = ( _rtB -> B_55_62_0 >= _rtB -> B_55_63_0 ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_65_0 =
_rtDW -> Memory_PreviousInput_po ; } if ( _rtB -> B_55_64_0 ) { _rtB ->
B_8_0_0 = _rtB -> B_55_60_0 - _rtB -> B_55_61_0 ; _rtB -> B_8_1_0 = _rtP ->
P_8 * _rtB -> B_8_0_0 ; _rtB -> B_55_67_0 = _rtB -> B_8_1_0 ; } else { _rtB
-> B_55_67_0 = _rtB -> B_55_65_0 ; } _rtB -> B_55_68_0 . re = _rtB ->
B_55_59_0 ; _rtB -> B_55_68_0 . im = _rtB -> B_55_67_0 ; _rtB -> B_55_69_0 =
muDoubleScalarHypot ( _rtB -> B_55_68_0 . re , _rtB -> B_55_68_0 . im ) ;
_rtB -> B_55_69_1 = muDoubleScalarAtan2 ( _rtB -> B_55_68_0 . im , _rtB ->
B_55_68_0 . re ) ; _rtB -> B_55_70_0 = _rtP -> P_190 * _rtB -> B_55_69_1 ;
_rtB -> B_55_71_0 = _rtB -> B_55_51_0 - _rtB -> B_55_70_0 ; _rtB -> B_55_72_0
= _rtP -> P_191 * _rtB -> B_55_71_0 ; _rtB -> B_55_73_0 = muDoubleScalarCos (
_rtB -> B_55_72_0 ) ; _rtB -> B_55_74_0 = _rtP -> P_192 * _rtB -> B_55_50_0 ;
_rtB -> B_55_75_0 = _rtP -> P_193 * _rtB -> B_55_69_0 ; _rtB -> B_55_76_0 =
_rtB -> B_55_1_0_c * _rtB -> B_55_73_0 * _rtB -> B_55_74_0 * _rtB ->
B_55_75_0 ; _rtB -> B_55_77_0 = _rtP -> P_194 * _rtB -> B_55_76_0 ; _rtB ->
B_55_78_0 = muDoubleScalarSin ( _rtB -> B_55_72_0 ) ; _rtB -> B_55_79_0 =
_rtB -> B_55_1_0_c * _rtB -> B_55_78_0 * _rtB -> B_55_74_0 * _rtB ->
B_55_75_0 ; _rtB -> B_55_80_0 = _rtP -> P_195 * _rtB -> B_55_79_0 ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
55 , 81 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 82 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 83 ,
SS_CALL_MDL_OUTPUTS ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_55_84_0 = _rtP -> P_196 * _rtB -> B_55_32_0 [ 23 ] ; _rtB ->
B_55_85_0 = _rtP -> P_197 * _rtB -> B_55_32_0 [ 24 ] ; _rtB -> B_55_86_0 =
_rtP -> P_198 * _rtB -> B_55_32_0 [ 25 ] ; _rtB -> B_55_87_0 [ 0 ] = _rtP ->
P_199 * _rtB -> B_55_84_0 ; _rtB -> B_55_87_0 [ 1 ] = _rtP -> P_199 * _rtB ->
B_55_85_0 ; _rtB -> B_55_87_0 [ 2 ] = _rtP -> P_199 * _rtB -> B_55_86_0 ;
_rtB -> B_55_88_0 = _rtP -> P_200 * _rtB -> B_55_32_0 [ 47 ] ; _rtB ->
B_55_89_0 = _rtP -> P_201 * _rtB -> B_55_32_0 [ 48 ] ; _rtB -> B_55_90_0 =
_rtP -> P_202 * _rtB -> B_55_32_0 [ 49 ] ; _rtB -> B_55_91_0 [ 0 ] = _rtP ->
P_203 * _rtB -> B_55_88_0 ; _rtB -> B_55_91_0 [ 1 ] = _rtP -> P_203 * _rtB ->
B_55_89_0 ; _rtB -> B_55_91_0 [ 2 ] = _rtP -> P_203 * _rtB -> B_55_90_0 ;
ssCallAccelRunBlock ( S , 55 , 92 , SS_CALL_MDL_OUTPUTS ) ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
55 , 93 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 94 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 95 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 96 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 97 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 98 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_55_99_0 = muDoubleScalarSin ( _rtP ->
P_206 * ssGetTaskTime ( S , 0 ) + _rtP -> P_207 ) * _rtP -> P_204 + _rtP ->
P_205 ; _rtB -> B_55_100_0 = _rtB -> B_55_87_0 [ 0 ] * _rtB -> B_55_99_0 ;
_rtB -> B_55_101_0 = muDoubleScalarSin ( _rtP -> P_210 * ssGetTaskTime ( S ,
0 ) + _rtP -> P_211 ) * _rtP -> P_208 + _rtP -> P_209 ; _rtB -> B_55_102_0 =
_rtB -> B_55_87_0 [ 0 ] * _rtB -> B_55_101_0 ; _rtB -> B_55_103_0 =
muDoubleScalarSin ( _rtP -> P_214 * ssGetTaskTime ( S , 0 ) + _rtP -> P_215 )
* _rtP -> P_212 + _rtP -> P_213 ; _rtB -> B_55_104_0 = _rtB -> B_55_91_0 [ 0
] * _rtB -> B_55_103_0 ; _rtB -> B_55_105_0 = muDoubleScalarSin ( _rtP ->
P_218 * ssGetTaskTime ( S , 0 ) + _rtP -> P_219 ) * _rtP -> P_216 + _rtP ->
P_217 ; _rtB -> B_55_106_0 = _rtB -> B_55_91_0 [ 0 ] * _rtB -> B_55_105_0 ;
_rtB -> B_55_107_0 = _rtX -> integrator_CSTATE_ly ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_a . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_a . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP
-> P_221 ; _rtB -> B_55_108_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_eq
. CircularBufSize , & _rtDW -> TransportDelay_IWORK_eq . Last , _rtDW ->
TransportDelay_IWORK_eq . Tail , _rtDW -> TransportDelay_IWORK_eq . Head ,
_rtP -> P_222 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } _rtB -> B_55_109_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_55_110_0 = _rtP -> P_223 ; } _rtB -> B_55_111_0 = ( _rtB -> B_55_109_0
>= _rtB -> B_55_110_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_55_112_0 = _rtDW -> Memory_PreviousInput_n ; } if ( _rtB ->
B_55_111_0 ) { _rtB -> B_11_0_0 = _rtB -> B_55_107_0 - _rtB -> B_55_108_0 ;
_rtB -> B_11_1_0 = _rtP -> P_11 * _rtB -> B_11_0_0 ; _rtB -> B_55_114_0 =
_rtB -> B_11_1_0 ; } else { _rtB -> B_55_114_0 = _rtB -> B_55_112_0 ; } _rtB
-> B_55_115_0 = _rtX -> integrator_CSTATE_c ; { real_T * * uBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_l . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_l . TUbufferPtrs [ 1
] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP ->
P_226 ; _rtB -> B_55_116_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_e5
. CircularBufSize , & _rtDW -> TransportDelay_IWORK_e5 . Last , _rtDW ->
TransportDelay_IWORK_e5 . Tail , _rtDW -> TransportDelay_IWORK_e5 . Head ,
_rtP -> P_227 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } _rtB -> B_55_117_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_55_118_0 = _rtP -> P_228 ; } _rtB -> B_55_119_0 = ( _rtB -> B_55_117_0
>= _rtB -> B_55_118_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_55_120_0 = _rtDW -> Memory_PreviousInput_j ; } if ( _rtB ->
B_55_119_0 ) { _rtB -> B_10_0_0 = _rtB -> B_55_115_0 - _rtB -> B_55_116_0 ;
_rtB -> B_10_1_0 = _rtP -> P_10 * _rtB -> B_10_0_0 ; _rtB -> B_55_122_0 =
_rtB -> B_10_1_0 ; } else { _rtB -> B_55_122_0 = _rtB -> B_55_120_0 ; } _rtB
-> B_55_123_0 . re = _rtB -> B_55_114_0 ; _rtB -> B_55_123_0 . im = _rtB ->
B_55_122_0 ; _rtB -> B_55_124_0 = muDoubleScalarHypot ( _rtB -> B_55_123_0 .
re , _rtB -> B_55_123_0 . im ) ; _rtB -> B_55_124_1 = muDoubleScalarAtan2 (
_rtB -> B_55_123_0 . im , _rtB -> B_55_123_0 . re ) ; _rtB -> B_55_125_0 =
_rtP -> P_230 * _rtB -> B_55_124_1 ; _rtB -> B_55_126_0 = _rtX ->
integrator_CSTATE_j ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fp . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_fp . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_232 ; _rtB
-> B_55_127_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 ,
* tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_g . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_g . Last , _rtDW -> TransportDelay_IWORK_g .
Tail , _rtDW -> TransportDelay_IWORK_g . Head , _rtP -> P_233 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_55_128_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_129_0 = _rtP
-> P_234 ; } _rtB -> B_55_130_0 = ( _rtB -> B_55_128_0 >= _rtB -> B_55_129_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_55_131_0 = _rtDW -> Memory_PreviousInput_g ; } if ( _rtB -> B_55_130_0 ) {
_rtB -> B_13_0_0 = _rtB -> B_55_126_0 - _rtB -> B_55_127_0 ; _rtB -> B_13_1_0
= _rtP -> P_13 * _rtB -> B_13_0_0 ; _rtB -> B_55_133_0 = _rtB -> B_13_1_0 ; }
else { _rtB -> B_55_133_0 = _rtB -> B_55_131_0 ; } _rtB -> B_55_134_0 = _rtX
-> integrator_CSTATE_m ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_g . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_g . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_237 ; _rtB ->
B_55_135_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_m . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_m . Last , _rtDW -> TransportDelay_IWORK_m .
Tail , _rtDW -> TransportDelay_IWORK_m . Head , _rtP -> P_238 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_55_136_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_137_0 = _rtP
-> P_239 ; } _rtB -> B_55_138_0 = ( _rtB -> B_55_136_0 >= _rtB -> B_55_137_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_55_139_0 = _rtDW -> Memory_PreviousInput_e ; } if ( _rtB -> B_55_138_0 ) {
_rtB -> B_12_0_0 = _rtB -> B_55_134_0 - _rtB -> B_55_135_0 ; _rtB -> B_12_1_0
= _rtP -> P_12 * _rtB -> B_12_0_0 ; _rtB -> B_55_141_0 = _rtB -> B_12_1_0 ; }
else { _rtB -> B_55_141_0 = _rtB -> B_55_139_0 ; } _rtB -> B_55_142_0 . re =
_rtB -> B_55_133_0 ; _rtB -> B_55_142_0 . im = _rtB -> B_55_141_0 ; _rtB ->
B_55_143_0 = muDoubleScalarHypot ( _rtB -> B_55_142_0 . re , _rtB ->
B_55_142_0 . im ) ; _rtB -> B_55_143_1 = muDoubleScalarAtan2 ( _rtB ->
B_55_142_0 . im , _rtB -> B_55_142_0 . re ) ; _rtB -> B_55_144_0 = _rtP ->
P_241 * _rtB -> B_55_143_1 ; _rtB -> B_55_145_0 = _rtB -> B_55_125_0 - _rtB
-> B_55_144_0 ; _rtB -> B_55_146_0 = _rtP -> P_242 * _rtB -> B_55_145_0 ;
_rtB -> B_55_147_0 = muDoubleScalarCos ( _rtB -> B_55_146_0 ) ; _rtB ->
B_55_148_0 = _rtP -> P_243 * _rtB -> B_55_124_0 ; _rtB -> B_55_149_0 = _rtP
-> P_244 * _rtB -> B_55_143_0 ; _rtB -> B_55_150_0 = _rtB -> B_55_2_0_k *
_rtB -> B_55_147_0 * _rtB -> B_55_148_0 * _rtB -> B_55_149_0 ; _rtB ->
B_55_151_0 = _rtP -> P_245 * _rtB -> B_55_150_0 ; _rtB -> B_55_152_0 =
muDoubleScalarSin ( _rtB -> B_55_146_0 ) ; _rtB -> B_55_153_0 = _rtB ->
B_55_2_0_k * _rtB -> B_55_152_0 * _rtB -> B_55_148_0 * _rtB -> B_55_149_0 ;
_rtB -> B_55_154_0 = _rtP -> P_246 * _rtB -> B_55_153_0 ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
55 , 155 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 156 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 157 ,
SS_CALL_MDL_OUTPUTS ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_55_158_0 = _rtP -> P_247 * _rtB -> B_55_32_0 [ 20 ] ; _rtB ->
B_55_159_0 = _rtP -> P_248 * _rtB -> B_55_32_0 [ 21 ] ; _rtB -> B_55_160_0 =
_rtP -> P_249 * _rtB -> B_55_32_0 [ 22 ] ; _rtB -> B_55_161_0 [ 0 ] = _rtP ->
P_250 * _rtB -> B_55_158_0 ; _rtB -> B_55_161_0 [ 1 ] = _rtP -> P_250 * _rtB
-> B_55_159_0 ; _rtB -> B_55_161_0 [ 2 ] = _rtP -> P_250 * _rtB -> B_55_160_0
; _rtB -> B_55_162_0 = _rtP -> P_251 * _rtB -> B_55_32_0 [ 44 ] ; _rtB ->
B_55_163_0 = _rtP -> P_252 * _rtB -> B_55_32_0 [ 45 ] ; _rtB -> B_55_164_0 =
_rtP -> P_253 * _rtB -> B_55_32_0 [ 46 ] ; _rtB -> B_55_165_0 [ 0 ] = _rtP ->
P_254 * _rtB -> B_55_162_0 ; _rtB -> B_55_165_0 [ 1 ] = _rtP -> P_254 * _rtB
-> B_55_163_0 ; _rtB -> B_55_165_0 [ 2 ] = _rtP -> P_254 * _rtB -> B_55_164_0
; ssCallAccelRunBlock ( S , 55 , 166 , SS_CALL_MDL_OUTPUTS ) ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
55 , 167 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 168 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 169 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 170 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 171 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 172 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_55_173_0 = muDoubleScalarSin ( _rtP ->
P_257 * ssGetTaskTime ( S , 0 ) + _rtP -> P_258 ) * _rtP -> P_255 + _rtP ->
P_256 ; _rtB -> B_55_174_0 = _rtB -> B_55_161_0 [ 0 ] * _rtB -> B_55_173_0 ;
_rtB -> B_55_175_0 = muDoubleScalarSin ( _rtP -> P_261 * ssGetTaskTime ( S ,
0 ) + _rtP -> P_262 ) * _rtP -> P_259 + _rtP -> P_260 ; _rtB -> B_55_176_0 =
_rtB -> B_55_161_0 [ 0 ] * _rtB -> B_55_175_0 ; _rtB -> B_55_177_0 =
muDoubleScalarSin ( _rtP -> P_265 * ssGetTaskTime ( S , 0 ) + _rtP -> P_266 )
* _rtP -> P_263 + _rtP -> P_264 ; _rtB -> B_55_178_0 = _rtB -> B_55_165_0 [ 0
] * _rtB -> B_55_177_0 ; _rtB -> B_55_179_0 = muDoubleScalarSin ( _rtP ->
P_269 * ssGetTaskTime ( S , 0 ) + _rtP -> P_270 ) * _rtP -> P_267 + _rtP ->
P_268 ; _rtB -> B_55_180_0 = _rtB -> B_55_165_0 [ 0 ] * _rtB -> B_55_179_0 ;
_rtB -> B_55_181_0 = _rtX -> integrator_CSTATE_o ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_j . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_j . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP
-> P_272 ; _rtB -> B_55_182_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_o .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_o . Last , _rtDW ->
TransportDelay_IWORK_o . Tail , _rtDW -> TransportDelay_IWORK_o . Head , _rtP
-> P_273 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } _rtB -> B_55_183_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_55_184_0 = _rtP -> P_274 ; } _rtB -> B_55_185_0 = ( _rtB -> B_55_183_0
>= _rtB -> B_55_184_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_55_186_0 = _rtDW -> Memory_PreviousInput_d ; } if ( _rtB ->
B_55_185_0 ) { _rtB -> B_27_0_0 = _rtB -> B_55_181_0 - _rtB -> B_55_182_0 ;
_rtB -> B_27_1_0 = _rtP -> P_27 * _rtB -> B_27_0_0 ; _rtB -> B_55_188_0 =
_rtB -> B_27_1_0 ; } else { _rtB -> B_55_188_0 = _rtB -> B_55_186_0 ; } _rtB
-> B_55_189_0 = _rtX -> integrator_CSTATE_d ; { real_T * * uBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_lk . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_lk . TUbufferPtrs [
1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP ->
P_277 ; _rtB -> B_55_190_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_l .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_l . Last , _rtDW ->
TransportDelay_IWORK_l . Tail , _rtDW -> TransportDelay_IWORK_l . Head , _rtP
-> P_278 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } _rtB -> B_55_191_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_55_192_0 = _rtP -> P_279 ; } _rtB -> B_55_193_0 = ( _rtB -> B_55_191_0
>= _rtB -> B_55_192_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_55_194_0 = _rtDW -> Memory_PreviousInput_c ; } if ( _rtB ->
B_55_193_0 ) { _rtB -> B_26_0_0 = _rtB -> B_55_189_0 - _rtB -> B_55_190_0 ;
_rtB -> B_26_1_0 = _rtP -> P_26 * _rtB -> B_26_0_0 ; _rtB -> B_55_196_0 =
_rtB -> B_26_1_0 ; } else { _rtB -> B_55_196_0 = _rtB -> B_55_194_0 ; } _rtB
-> B_55_197_0 . re = _rtB -> B_55_188_0 ; _rtB -> B_55_197_0 . im = _rtB ->
B_55_196_0 ; _rtB -> B_55_198_0 = muDoubleScalarHypot ( _rtB -> B_55_197_0 .
re , _rtB -> B_55_197_0 . im ) ; _rtB -> B_55_198_1 = muDoubleScalarAtan2 (
_rtB -> B_55_197_0 . im , _rtB -> B_55_197_0 . re ) ; _rtB -> B_55_199_0 =
_rtP -> P_281 * _rtB -> B_55_198_1 ; _rtB -> B_55_200_0 = _rtX ->
integrator_CSTATE_jx ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_e . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_e . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_283 ; _rtB ->
B_55_201_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_cm . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_cm . Last , _rtDW -> TransportDelay_IWORK_cm .
Tail , _rtDW -> TransportDelay_IWORK_cm . Head , _rtP -> P_284 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_55_202_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_203_0 = _rtP
-> P_285 ; } _rtB -> B_55_204_0 = ( _rtB -> B_55_202_0 >= _rtB -> B_55_203_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_55_205_0 = _rtDW -> Memory_PreviousInput_h ; } if ( _rtB -> B_55_204_0 ) {
_rtB -> B_29_0_0 = _rtB -> B_55_200_0 - _rtB -> B_55_201_0 ; _rtB -> B_29_1_0
= _rtP -> P_29 * _rtB -> B_29_0_0 ; _rtB -> B_55_207_0 = _rtB -> B_29_1_0 ; }
else { _rtB -> B_55_207_0 = _rtB -> B_55_205_0 ; } _rtB -> B_55_208_0 = _rtX
-> integrator_CSTATE_k ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_k2 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_k2 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_288 ; _rtB
-> B_55_209_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 ,
* tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_p . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_p . Last , _rtDW -> TransportDelay_IWORK_p .
Tail , _rtDW -> TransportDelay_IWORK_p . Head , _rtP -> P_289 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_55_210_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_211_0 = _rtP
-> P_290 ; } _rtB -> B_55_212_0 = ( _rtB -> B_55_210_0 >= _rtB -> B_55_211_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_55_213_0 = _rtDW -> Memory_PreviousInput_m ; } if ( _rtB -> B_55_212_0 ) {
_rtB -> B_28_0_0 = _rtB -> B_55_208_0 - _rtB -> B_55_209_0 ; _rtB -> B_28_1_0
= _rtP -> P_28 * _rtB -> B_28_0_0 ; _rtB -> B_55_215_0 = _rtB -> B_28_1_0 ; }
else { _rtB -> B_55_215_0 = _rtB -> B_55_213_0 ; } _rtB -> B_55_216_0 . re =
_rtB -> B_55_207_0 ; _rtB -> B_55_216_0 . im = _rtB -> B_55_215_0 ; _rtB ->
B_55_217_0 = muDoubleScalarHypot ( _rtB -> B_55_216_0 . re , _rtB ->
B_55_216_0 . im ) ; _rtB -> B_55_217_1 = muDoubleScalarAtan2 ( _rtB ->
B_55_216_0 . im , _rtB -> B_55_216_0 . re ) ; _rtB -> B_55_218_0 = _rtP ->
P_292 * _rtB -> B_55_217_1 ; _rtB -> B_55_219_0 = _rtB -> B_55_199_0 - _rtB
-> B_55_218_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
ssCallAccelRunBlock ( S , 55 , 220 , SS_CALL_MDL_OUTPUTS ) ; } _rtB ->
B_55_221_0 = _rtP -> P_293 * _rtB -> B_55_219_0 ; _rtB -> B_55_222_0 =
muDoubleScalarCos ( _rtB -> B_55_221_0 ) ; _rtB -> B_55_223_0 = _rtP -> P_294
* _rtB -> B_55_198_0 ; _rtB -> B_55_224_0 = _rtP -> P_295 * _rtB ->
B_55_217_0 ; _rtB -> B_55_225_0 = _rtB -> B_55_3_0_c * _rtB -> B_55_222_0 *
_rtB -> B_55_223_0 * _rtB -> B_55_224_0 ; _rtB -> B_55_226_0 = _rtP -> P_296
* _rtB -> B_55_225_0 ; _rtB -> B_55_227_0 = muDoubleScalarSin ( _rtB ->
B_55_221_0 ) ; _rtB -> B_55_228_0 = _rtB -> B_55_3_0_c * _rtB -> B_55_227_0 *
_rtB -> B_55_223_0 * _rtB -> B_55_224_0 ; _rtB -> B_55_229_0 = _rtP -> P_297
* _rtB -> B_55_228_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { ssCallAccelRunBlock ( S , 55 , 230 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 231 , SS_CALL_MDL_OUTPUTS ) ; } isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_232_0 = _rtP
-> P_298 * _rtB -> B_55_32_0 [ 32 ] ; _rtB -> B_55_233_0 = _rtP -> P_299 *
_rtB -> B_55_32_0 [ 33 ] ; _rtB -> B_55_234_0 = _rtP -> P_300 * _rtB ->
B_55_32_0 [ 34 ] ; _rtB -> B_55_235_0 [ 0 ] = _rtP -> P_301 * _rtB ->
B_55_232_0 ; _rtB -> B_55_235_0 [ 1 ] = _rtP -> P_301 * _rtB -> B_55_233_0 ;
_rtB -> B_55_235_0 [ 2 ] = _rtP -> P_301 * _rtB -> B_55_234_0 ; _rtB ->
B_55_236_0 = _rtP -> P_302 * _rtB -> B_55_32_0 [ 56 ] ; _rtB -> B_55_237_0 =
_rtP -> P_303 * _rtB -> B_55_32_0 [ 57 ] ; _rtB -> B_55_238_0 = _rtP -> P_304
* _rtB -> B_55_32_0 [ 58 ] ; _rtB -> B_55_239_0 [ 0 ] = _rtP -> P_305 * _rtB
-> B_55_236_0 ; _rtB -> B_55_239_0 [ 1 ] = _rtP -> P_305 * _rtB -> B_55_237_0
; _rtB -> B_55_239_0 [ 2 ] = _rtP -> P_305 * _rtB -> B_55_238_0 ;
ssCallAccelRunBlock ( S , 55 , 240 , SS_CALL_MDL_OUTPUTS ) ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
55 , 241 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 242 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 243 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 244 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 245 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 246 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_55_247_0 = muDoubleScalarSin ( _rtP ->
P_308 * ssGetTaskTime ( S , 0 ) + _rtP -> P_309 ) * _rtP -> P_306 + _rtP ->
P_307 ; _rtB -> B_55_248_0 = _rtB -> B_55_235_0 [ 0 ] * _rtB -> B_55_247_0 ;
_rtB -> B_55_249_0 = muDoubleScalarSin ( _rtP -> P_312 * ssGetTaskTime ( S ,
0 ) + _rtP -> P_313 ) * _rtP -> P_310 + _rtP -> P_311 ; _rtB -> B_55_250_0 =
_rtB -> B_55_235_0 [ 0 ] * _rtB -> B_55_249_0 ; _rtB -> B_55_251_0 =
muDoubleScalarSin ( _rtP -> P_316 * ssGetTaskTime ( S , 0 ) + _rtP -> P_317 )
* _rtP -> P_314 + _rtP -> P_315 ; _rtB -> B_55_252_0 = _rtB -> B_55_239_0 [ 0
] * _rtB -> B_55_251_0 ; _rtB -> B_55_253_0 = muDoubleScalarSin ( _rtP ->
P_320 * ssGetTaskTime ( S , 0 ) + _rtP -> P_321 ) * _rtP -> P_318 + _rtP ->
P_319 ; _rtB -> B_55_254_0 = _rtB -> B_55_239_0 [ 0 ] * _rtB -> B_55_253_0 ;
_rtB -> B_55_255_0 = _rtX -> integrator_CSTATE_kg ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_i . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_i . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP
-> P_323 ; _rtB -> B_55_256_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_j1
. CircularBufSize , & _rtDW -> TransportDelay_IWORK_j1 . Last , _rtDW ->
TransportDelay_IWORK_j1 . Tail , _rtDW -> TransportDelay_IWORK_j1 . Head ,
_rtP -> P_324 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } _rtB -> B_55_257_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_55_258_0 = _rtP -> P_325 ; } _rtB -> B_55_259_0 = ( _rtB -> B_55_257_0
>= _rtB -> B_55_258_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_55_260_0 = _rtDW -> Memory_PreviousInput_nd ; } if ( _rtB ->
B_55_259_0 ) { _rtB -> B_15_0_0 = _rtB -> B_55_255_0 - _rtB -> B_55_256_0 ;
_rtB -> B_15_1_0 = _rtP -> P_15 * _rtB -> B_15_0_0 ; _rtB -> B_55_262_0 =
_rtB -> B_15_1_0 ; } else { _rtB -> B_55_262_0 = _rtB -> B_55_260_0 ; } _rtB
-> B_55_263_0 = _rtX -> integrator_CSTATE_fi ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_o . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP
-> P_328 ; _rtB -> B_55_264_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ln
. CircularBufSize , & _rtDW -> TransportDelay_IWORK_ln . Last , _rtDW ->
TransportDelay_IWORK_ln . Tail , _rtDW -> TransportDelay_IWORK_ln . Head ,
_rtP -> P_329 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } _rtB -> B_55_265_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_55_266_0 = _rtP -> P_330 ; } _rtB -> B_55_267_0 = ( _rtB -> B_55_265_0
>= _rtB -> B_55_266_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_55_268_0 = _rtDW -> Memory_PreviousInput_cy ; } if ( _rtB ->
B_55_267_0 ) { _rtB -> B_14_0_0 = _rtB -> B_55_263_0 - _rtB -> B_55_264_0 ;
_rtB -> B_14_1_0 = _rtP -> P_14 * _rtB -> B_14_0_0 ; _rtB -> B_55_270_0 =
_rtB -> B_14_1_0 ; } else { _rtB -> B_55_270_0 = _rtB -> B_55_268_0 ; } _rtB
-> B_55_271_0 . re = _rtB -> B_55_262_0 ; _rtB -> B_55_271_0 . im = _rtB ->
B_55_270_0 ; _rtB -> B_55_272_0 = muDoubleScalarHypot ( _rtB -> B_55_271_0 .
re , _rtB -> B_55_271_0 . im ) ; _rtB -> B_55_272_1 = muDoubleScalarAtan2 (
_rtB -> B_55_271_0 . im , _rtB -> B_55_271_0 . re ) ; _rtB -> B_55_273_0 =
_rtP -> P_332 * _rtB -> B_55_272_1 ; _rtB -> B_55_274_0 = _rtX ->
integrator_CSTATE_e ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_b . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_b . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_334 ; _rtB ->
B_55_275_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_f . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_f . Last , _rtDW -> TransportDelay_IWORK_f .
Tail , _rtDW -> TransportDelay_IWORK_f . Head , _rtP -> P_335 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_55_276_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_277_0 = _rtP
-> P_336 ; } _rtB -> B_55_278_0 = ( _rtB -> B_55_276_0 >= _rtB -> B_55_277_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_55_279_0 = _rtDW -> Memory_PreviousInput_a ; } if ( _rtB -> B_55_278_0 ) {
_rtB -> B_17_0_0 = _rtB -> B_55_274_0 - _rtB -> B_55_275_0 ; _rtB -> B_17_1_0
= _rtP -> P_17 * _rtB -> B_17_0_0 ; _rtB -> B_55_281_0 = _rtB -> B_17_1_0 ; }
else { _rtB -> B_55_281_0 = _rtB -> B_55_279_0 ; } _rtB -> B_55_282_0 = _rtX
-> integrator_CSTATE_a ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_c . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_c . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_339 ; _rtB ->
B_55_283_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_gl . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_gl . Last , _rtDW -> TransportDelay_IWORK_gl .
Tail , _rtDW -> TransportDelay_IWORK_gl . Head , _rtP -> P_340 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_55_284_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_285_0 = _rtP
-> P_341 ; } _rtB -> B_55_286_0 = ( _rtB -> B_55_284_0 >= _rtB -> B_55_285_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_55_287_0 = _rtDW -> Memory_PreviousInput_c2 ; } if ( _rtB -> B_55_286_0 ) {
_rtB -> B_16_0_0 = _rtB -> B_55_282_0 - _rtB -> B_55_283_0 ; _rtB -> B_16_1_0
= _rtP -> P_16 * _rtB -> B_16_0_0 ; _rtB -> B_55_289_0 = _rtB -> B_16_1_0 ; }
else { _rtB -> B_55_289_0 = _rtB -> B_55_287_0 ; } _rtB -> B_55_290_0 . re =
_rtB -> B_55_281_0 ; _rtB -> B_55_290_0 . im = _rtB -> B_55_289_0 ; _rtB ->
B_55_291_0 = muDoubleScalarHypot ( _rtB -> B_55_290_0 . re , _rtB ->
B_55_290_0 . im ) ; _rtB -> B_55_291_1 = muDoubleScalarAtan2 ( _rtB ->
B_55_290_0 . im , _rtB -> B_55_290_0 . re ) ; _rtB -> B_55_292_0 = _rtP ->
P_343 * _rtB -> B_55_291_1 ; _rtB -> B_55_293_0 = _rtB -> B_55_273_0 - _rtB
-> B_55_292_0 ; _rtB -> B_55_294_0 = _rtP -> P_344 * _rtB -> B_55_293_0 ;
_rtB -> B_55_295_0 = muDoubleScalarCos ( _rtB -> B_55_294_0 ) ; _rtB ->
B_55_296_0 = _rtP -> P_345 * _rtB -> B_55_272_0 ; _rtB -> B_55_297_0 = _rtP
-> P_346 * _rtB -> B_55_291_0 ; _rtB -> B_55_298_0 = _rtB -> B_55_4_0_b *
_rtB -> B_55_295_0 * _rtB -> B_55_296_0 * _rtB -> B_55_297_0 ; _rtB ->
B_55_299_0 = _rtP -> P_347 * _rtB -> B_55_298_0 ; _rtB -> B_55_300_0 =
muDoubleScalarSin ( _rtB -> B_55_294_0 ) ; _rtB -> B_55_301_0 = _rtB ->
B_55_4_0_b * _rtB -> B_55_300_0 * _rtB -> B_55_296_0 * _rtB -> B_55_297_0 ;
_rtB -> B_55_302_0 = _rtP -> P_348 * _rtB -> B_55_301_0 ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
55 , 303 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 304 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 305 ,
SS_CALL_MDL_OUTPUTS ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_55_306_0 = _rtP -> P_349 * _rtB -> B_55_32_0 [ 17 ] ; _rtB ->
B_55_307_0 = _rtP -> P_350 * _rtB -> B_55_32_0 [ 18 ] ; _rtB -> B_55_308_0 =
_rtP -> P_351 * _rtB -> B_55_32_0 [ 19 ] ; _rtB -> B_55_309_0 [ 0 ] = _rtP ->
P_352 * _rtB -> B_55_306_0 ; _rtB -> B_55_309_0 [ 1 ] = _rtP -> P_352 * _rtB
-> B_55_307_0 ; _rtB -> B_55_309_0 [ 2 ] = _rtP -> P_352 * _rtB -> B_55_308_0
; _rtB -> B_55_310_0 = _rtP -> P_353 * _rtB -> B_55_32_0 [ 41 ] ; _rtB ->
B_55_311_0 = _rtP -> P_354 * _rtB -> B_55_32_0 [ 42 ] ; _rtB -> B_55_312_0 =
_rtP -> P_355 * _rtB -> B_55_32_0 [ 43 ] ; _rtB -> B_55_313_0 [ 0 ] = _rtP ->
P_356 * _rtB -> B_55_310_0 ; _rtB -> B_55_313_0 [ 1 ] = _rtP -> P_356 * _rtB
-> B_55_311_0 ; _rtB -> B_55_313_0 [ 2 ] = _rtP -> P_356 * _rtB -> B_55_312_0
; ssCallAccelRunBlock ( S , 55 , 314 , SS_CALL_MDL_OUTPUTS ) ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
55 , 315 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 316 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 317 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 318 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 319 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 320 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_55_321_0 = muDoubleScalarSin ( _rtP ->
P_359 * ssGetTaskTime ( S , 0 ) + _rtP -> P_360 ) * _rtP -> P_357 + _rtP ->
P_358 ; _rtB -> B_55_322_0 = _rtB -> B_55_309_0 [ 0 ] * _rtB -> B_55_321_0 ;
_rtB -> B_55_323_0 = muDoubleScalarSin ( _rtP -> P_363 * ssGetTaskTime ( S ,
0 ) + _rtP -> P_364 ) * _rtP -> P_361 + _rtP -> P_362 ; _rtB -> B_55_324_0 =
_rtB -> B_55_309_0 [ 0 ] * _rtB -> B_55_323_0 ; _rtB -> B_55_325_0 =
muDoubleScalarSin ( _rtP -> P_367 * ssGetTaskTime ( S , 0 ) + _rtP -> P_368 )
* _rtP -> P_365 + _rtP -> P_366 ; _rtB -> B_55_326_0 = _rtB -> B_55_313_0 [ 0
] * _rtB -> B_55_325_0 ; _rtB -> B_55_327_0 = muDoubleScalarSin ( _rtP ->
P_371 * ssGetTaskTime ( S , 0 ) + _rtP -> P_372 ) * _rtP -> P_369 + _rtP ->
P_370 ; _rtB -> B_55_328_0 = _rtB -> B_55_313_0 [ 0 ] * _rtB -> B_55_327_0 ;
_rtB -> B_55_329_0 = _rtX -> integrator_CSTATE_ol ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fx . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fx .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_374 ; _rtB -> B_55_330_0 =
DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> TransportDelay_IWORK_k . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_k . Last , _rtDW -> TransportDelay_IWORK_k . Tail ,
_rtDW -> TransportDelay_IWORK_k . Head , _rtP -> P_375 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_55_331_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_55_332_0 = _rtP -> P_376 ; } _rtB ->
B_55_333_0 = ( _rtB -> B_55_331_0 >= _rtB -> B_55_332_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_334_0 = _rtDW
-> Memory_PreviousInput_dh ; } if ( _rtB -> B_55_333_0 ) { _rtB -> B_19_0_0 =
_rtB -> B_55_329_0 - _rtB -> B_55_330_0 ; _rtB -> B_19_1_0 = _rtP -> P_19 *
_rtB -> B_19_0_0 ; _rtB -> B_55_336_0 = _rtB -> B_19_1_0 ; } else { _rtB ->
B_55_336_0 = _rtB -> B_55_334_0 ; } _rtB -> B_55_337_0 = _rtX ->
integrator_CSTATE_h ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_eg . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_eg . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_379 ; _rtB
-> B_55_338_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 ,
* tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_od . CircularBufSize ,
& _rtDW -> TransportDelay_IWORK_od . Last , _rtDW -> TransportDelay_IWORK_od
. Tail , _rtDW -> TransportDelay_IWORK_od . Head , _rtP -> P_380 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_55_339_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_340_0 = _rtP
-> P_381 ; } _rtB -> B_55_341_0 = ( _rtB -> B_55_339_0 >= _rtB -> B_55_340_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_55_342_0 = _rtDW -> Memory_PreviousInput_i ; } if ( _rtB -> B_55_341_0 ) {
_rtB -> B_18_0_0 = _rtB -> B_55_337_0 - _rtB -> B_55_338_0 ; _rtB -> B_18_1_0
= _rtP -> P_18 * _rtB -> B_18_0_0 ; _rtB -> B_55_344_0 = _rtB -> B_18_1_0 ; }
else { _rtB -> B_55_344_0 = _rtB -> B_55_342_0 ; } _rtB -> B_55_345_0 . re =
_rtB -> B_55_336_0 ; _rtB -> B_55_345_0 . im = _rtB -> B_55_344_0 ; _rtB ->
B_55_346_0 = muDoubleScalarHypot ( _rtB -> B_55_345_0 . re , _rtB ->
B_55_345_0 . im ) ; _rtB -> B_55_346_1 = muDoubleScalarAtan2 ( _rtB ->
B_55_345_0 . im , _rtB -> B_55_345_0 . re ) ; _rtB -> B_55_347_0 = _rtP ->
P_383 * _rtB -> B_55_346_1 ; _rtB -> B_55_348_0 = _rtX -> integrator_CSTATE_b
; { real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e0 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_e0 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_385 ; _rtB -> B_55_349_0 =
DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> TransportDelay_IWORK_ed . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ed . Last , _rtDW -> TransportDelay_IWORK_ed . Tail ,
_rtDW -> TransportDelay_IWORK_ed . Head , _rtP -> P_386 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_55_350_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_55_351_0 = _rtP -> P_387 ; } _rtB ->
B_55_352_0 = ( _rtB -> B_55_350_0 >= _rtB -> B_55_351_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_353_0 = _rtDW
-> Memory_PreviousInput_di ; } if ( _rtB -> B_55_352_0 ) { _rtB -> B_21_0_0 =
_rtB -> B_55_348_0 - _rtB -> B_55_349_0 ; _rtB -> B_21_1_0 = _rtP -> P_21 *
_rtB -> B_21_0_0 ; _rtB -> B_55_355_0 = _rtB -> B_21_1_0 ; } else { _rtB ->
B_55_355_0 = _rtB -> B_55_353_0 ; } _rtB -> B_55_356_0 = _rtX ->
integrator_CSTATE_jb ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_n . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_n . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_390 ; _rtB ->
B_55_357_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_h . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_h . Last , _rtDW -> TransportDelay_IWORK_h .
Tail , _rtDW -> TransportDelay_IWORK_h . Head , _rtP -> P_391 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_55_358_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_359_0 = _rtP
-> P_392 ; } _rtB -> B_55_360_0 = ( _rtB -> B_55_358_0 >= _rtB -> B_55_359_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_55_361_0 = _rtDW -> Memory_PreviousInput_gk ; } if ( _rtB -> B_55_360_0 ) {
_rtB -> B_20_0_0 = _rtB -> B_55_356_0 - _rtB -> B_55_357_0 ; _rtB -> B_20_1_0
= _rtP -> P_20 * _rtB -> B_20_0_0 ; _rtB -> B_55_363_0 = _rtB -> B_20_1_0 ; }
else { _rtB -> B_55_363_0 = _rtB -> B_55_361_0 ; } _rtB -> B_55_364_0 . re =
_rtB -> B_55_355_0 ; _rtB -> B_55_364_0 . im = _rtB -> B_55_363_0 ; _rtB ->
B_55_365_0 = muDoubleScalarHypot ( _rtB -> B_55_364_0 . re , _rtB ->
B_55_364_0 . im ) ; _rtB -> B_55_365_1 = muDoubleScalarAtan2 ( _rtB ->
B_55_364_0 . im , _rtB -> B_55_364_0 . re ) ; _rtB -> B_55_366_0 = _rtP ->
P_394 * _rtB -> B_55_365_1 ; _rtB -> B_55_367_0 = _rtB -> B_55_347_0 - _rtB
-> B_55_366_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
ssCallAccelRunBlock ( S , 55 , 368 , SS_CALL_MDL_OUTPUTS ) ; } _rtB ->
B_55_369_0 = _rtP -> P_395 * _rtB -> B_55_367_0 ; _rtB -> B_55_370_0 =
muDoubleScalarCos ( _rtB -> B_55_369_0 ) ; _rtB -> B_55_371_0 = _rtP -> P_396
* _rtB -> B_55_346_0 ; _rtB -> B_55_372_0 = _rtP -> P_397 * _rtB ->
B_55_365_0 ; _rtB -> B_55_373_0 = _rtB -> B_55_5_0_p * _rtB -> B_55_370_0 *
_rtB -> B_55_371_0 * _rtB -> B_55_372_0 ; _rtB -> B_55_374_0 = _rtP -> P_398
* _rtB -> B_55_373_0 ; _rtB -> B_55_375_0 = muDoubleScalarSin ( _rtB ->
B_55_369_0 ) ; _rtB -> B_55_376_0 = _rtB -> B_55_5_0_p * _rtB -> B_55_375_0 *
_rtB -> B_55_371_0 * _rtB -> B_55_372_0 ; _rtB -> B_55_377_0 = _rtP -> P_399
* _rtB -> B_55_376_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { ssCallAccelRunBlock ( S , 55 , 378 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 379 , SS_CALL_MDL_OUTPUTS ) ; } isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_380_0 = _rtP
-> P_400 * _rtB -> B_55_32_0 [ 14 ] ; _rtB -> B_55_381_0 = _rtP -> P_401 *
_rtB -> B_55_32_0 [ 15 ] ; _rtB -> B_55_382_0 = _rtP -> P_402 * _rtB ->
B_55_32_0 [ 16 ] ; _rtB -> B_55_383_0 [ 0 ] = _rtP -> P_403 * _rtB ->
B_55_380_0 ; _rtB -> B_55_383_0 [ 1 ] = _rtP -> P_403 * _rtB -> B_55_381_0 ;
_rtB -> B_55_383_0 [ 2 ] = _rtP -> P_403 * _rtB -> B_55_382_0 ; _rtB ->
B_55_384_0 = _rtP -> P_404 * _rtB -> B_55_32_0 [ 38 ] ; _rtB -> B_55_385_0 =
_rtP -> P_405 * _rtB -> B_55_32_0 [ 39 ] ; _rtB -> B_55_386_0 = _rtP -> P_406
* _rtB -> B_55_32_0 [ 40 ] ; _rtB -> B_55_387_0 [ 0 ] = _rtP -> P_407 * _rtB
-> B_55_384_0 ; _rtB -> B_55_387_0 [ 1 ] = _rtP -> P_407 * _rtB -> B_55_385_0
; _rtB -> B_55_387_0 [ 2 ] = _rtP -> P_407 * _rtB -> B_55_386_0 ;
ssCallAccelRunBlock ( S , 55 , 388 , SS_CALL_MDL_OUTPUTS ) ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
55 , 389 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 390 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 391 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 392 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 393 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 394 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_55_395_0 = muDoubleScalarSin ( _rtP ->
P_410 * ssGetTaskTime ( S , 0 ) + _rtP -> P_411 ) * _rtP -> P_408 + _rtP ->
P_409 ; _rtB -> B_55_396_0 = _rtB -> B_55_383_0 [ 0 ] * _rtB -> B_55_395_0 ;
_rtB -> B_55_397_0 = muDoubleScalarSin ( _rtP -> P_414 * ssGetTaskTime ( S ,
0 ) + _rtP -> P_415 ) * _rtP -> P_412 + _rtP -> P_413 ; _rtB -> B_55_398_0 =
_rtB -> B_55_383_0 [ 0 ] * _rtB -> B_55_397_0 ; _rtB -> B_55_399_0 =
muDoubleScalarSin ( _rtP -> P_418 * ssGetTaskTime ( S , 0 ) + _rtP -> P_419 )
* _rtP -> P_416 + _rtP -> P_417 ; _rtB -> B_55_400_0 = _rtB -> B_55_387_0 [ 0
] * _rtB -> B_55_399_0 ; _rtB -> B_55_401_0 = muDoubleScalarSin ( _rtP ->
P_422 * ssGetTaskTime ( S , 0 ) + _rtP -> P_423 ) * _rtP -> P_420 + _rtP ->
P_421 ; _rtB -> B_55_402_0 = _rtB -> B_55_387_0 [ 0 ] * _rtB -> B_55_401_0 ;
_rtB -> B_55_403_0 = _rtX -> integrator_CSTATE_df ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_h . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP
-> P_425 ; _rtB -> B_55_404_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_d .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_d . Last , _rtDW ->
TransportDelay_IWORK_d . Tail , _rtDW -> TransportDelay_IWORK_d . Head , _rtP
-> P_426 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } _rtB -> B_55_405_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_55_406_0 = _rtP -> P_427 ; } _rtB -> B_55_407_0 = ( _rtB -> B_55_405_0
>= _rtB -> B_55_406_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_55_408_0 = _rtDW -> Memory_PreviousInput_jh ; } if ( _rtB ->
B_55_407_0 ) { _rtB -> B_23_0_0 = _rtB -> B_55_403_0 - _rtB -> B_55_404_0 ;
_rtB -> B_23_1_0 = _rtP -> P_23 * _rtB -> B_23_0_0 ; _rtB -> B_55_410_0 =
_rtB -> B_23_1_0 ; } else { _rtB -> B_55_410_0 = _rtB -> B_55_408_0 ; } _rtB
-> B_55_411_0 = _rtX -> integrator_CSTATE_p ; { real_T * * uBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_jr . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_jr . TUbufferPtrs [
1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP ->
P_430 ; _rtB -> B_55_412_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_kn
. CircularBufSize , & _rtDW -> TransportDelay_IWORK_kn . Last , _rtDW ->
TransportDelay_IWORK_kn . Tail , _rtDW -> TransportDelay_IWORK_kn . Head ,
_rtP -> P_431 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } _rtB -> B_55_413_0 =
ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_55_414_0 = _rtP -> P_432 ; } _rtB -> B_55_415_0 = ( _rtB -> B_55_413_0
>= _rtB -> B_55_414_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_55_416_0 = _rtDW -> Memory_PreviousInput_il ; } if ( _rtB ->
B_55_415_0 ) { _rtB -> B_22_0_0 = _rtB -> B_55_411_0 - _rtB -> B_55_412_0 ;
_rtB -> B_22_1_0 = _rtP -> P_22 * _rtB -> B_22_0_0 ; _rtB -> B_55_418_0 =
_rtB -> B_22_1_0 ; } else { _rtB -> B_55_418_0 = _rtB -> B_55_416_0 ; } _rtB
-> B_55_419_0 . re = _rtB -> B_55_410_0 ; _rtB -> B_55_419_0 . im = _rtB ->
B_55_418_0 ; _rtB -> B_55_420_0 = muDoubleScalarHypot ( _rtB -> B_55_419_0 .
re , _rtB -> B_55_419_0 . im ) ; _rtB -> B_55_420_1 = muDoubleScalarAtan2 (
_rtB -> B_55_419_0 . im , _rtB -> B_55_419_0 . re ) ; _rtB -> B_55_421_0 =
_rtP -> P_434 * _rtB -> B_55_420_1 ; _rtB -> B_55_422_0 = _rtX ->
integrator_CSTATE_he ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_j3 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_j3 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_436 ; _rtB
-> B_55_423_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 ,
* tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_gj . CircularBufSize ,
& _rtDW -> TransportDelay_IWORK_gj . Last , _rtDW -> TransportDelay_IWORK_gj
. Tail , _rtDW -> TransportDelay_IWORK_gj . Head , _rtP -> P_437 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_55_424_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_425_0 = _rtP
-> P_438 ; } _rtB -> B_55_426_0 = ( _rtB -> B_55_424_0 >= _rtB -> B_55_425_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_55_427_0 = _rtDW -> Memory_PreviousInput_gc ; } if ( _rtB -> B_55_426_0 ) {
_rtB -> B_25_0_0 = _rtB -> B_55_422_0 - _rtB -> B_55_423_0 ; _rtB -> B_25_1_0
= _rtP -> P_25 * _rtB -> B_25_0_0 ; _rtB -> B_55_429_0 = _rtB -> B_25_1_0 ; }
else { _rtB -> B_55_429_0 = _rtB -> B_55_427_0 ; } _rtB -> B_55_430_0 = _rtX
-> integrator_CSTATE_hj ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_dm . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_dm . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_441 ; _rtB
-> B_55_431_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 ,
* tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lu . CircularBufSize ,
& _rtDW -> TransportDelay_IWORK_lu . Last , _rtDW -> TransportDelay_IWORK_lu
. Tail , _rtDW -> TransportDelay_IWORK_lu . Head , _rtP -> P_442 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_55_432_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_433_0 = _rtP
-> P_443 ; } _rtB -> B_55_434_0 = ( _rtB -> B_55_432_0 >= _rtB -> B_55_433_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_55_435_0 = _rtDW -> Memory_PreviousInput_o ; } if ( _rtB -> B_55_434_0 ) {
_rtB -> B_24_0_0 = _rtB -> B_55_430_0 - _rtB -> B_55_431_0 ; _rtB -> B_24_1_0
= _rtP -> P_24 * _rtB -> B_24_0_0 ; _rtB -> B_55_437_0 = _rtB -> B_24_1_0 ; }
else { _rtB -> B_55_437_0 = _rtB -> B_55_435_0 ; } _rtB -> B_55_438_0 . re =
_rtB -> B_55_429_0 ; _rtB -> B_55_438_0 . im = _rtB -> B_55_437_0 ; _rtB ->
B_55_439_0 = muDoubleScalarHypot ( _rtB -> B_55_438_0 . re , _rtB ->
B_55_438_0 . im ) ; _rtB -> B_55_439_1 = muDoubleScalarAtan2 ( _rtB ->
B_55_438_0 . im , _rtB -> B_55_438_0 . re ) ; _rtB -> B_55_440_0 = _rtP ->
P_445 * _rtB -> B_55_439_1 ; _rtB -> B_55_441_0 = _rtB -> B_55_421_0 - _rtB
-> B_55_440_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
ssCallAccelRunBlock ( S , 55 , 442 , SS_CALL_MDL_OUTPUTS ) ; } _rtB ->
B_55_443_0 = _rtP -> P_446 * _rtB -> B_55_441_0 ; _rtB -> B_55_444_0 =
muDoubleScalarCos ( _rtB -> B_55_443_0 ) ; _rtB -> B_55_445_0 = _rtP -> P_447
* _rtB -> B_55_420_0 ; _rtB -> B_55_446_0 = _rtP -> P_448 * _rtB ->
B_55_439_0 ; _rtB -> B_55_447_0 = _rtB -> B_55_6_0_c * _rtB -> B_55_444_0 *
_rtB -> B_55_445_0 * _rtB -> B_55_446_0 ; _rtB -> B_55_448_0 = _rtP -> P_449
* _rtB -> B_55_447_0 ; _rtB -> B_55_449_0 = muDoubleScalarSin ( _rtB ->
B_55_443_0 ) ; _rtB -> B_55_450_0 = _rtB -> B_55_6_0_c * _rtB -> B_55_449_0 *
_rtB -> B_55_445_0 * _rtB -> B_55_446_0 ; _rtB -> B_55_451_0 = _rtP -> P_450
* _rtB -> B_55_450_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { ssCallAccelRunBlock ( S , 55 , 452 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 453 , SS_CALL_MDL_OUTPUTS ) ; } isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_454_0 = _rtP
-> P_451 * _rtB -> B_55_32_0 [ 11 ] ; _rtB -> B_55_455_0 = _rtP -> P_452 *
_rtB -> B_55_32_0 [ 12 ] ; _rtB -> B_55_456_0 = _rtP -> P_453 * _rtB ->
B_55_32_0 [ 13 ] ; _rtB -> B_55_457_0 [ 0 ] = _rtP -> P_454 * _rtB ->
B_55_454_0 ; _rtB -> B_55_457_0 [ 1 ] = _rtP -> P_454 * _rtB -> B_55_455_0 ;
_rtB -> B_55_457_0 [ 2 ] = _rtP -> P_454 * _rtB -> B_55_456_0 ; _rtB ->
B_55_458_0 = _rtP -> P_455 * _rtB -> B_55_32_0 [ 35 ] ; _rtB -> B_55_459_0 =
_rtP -> P_456 * _rtB -> B_55_32_0 [ 36 ] ; _rtB -> B_55_460_0 = _rtP -> P_457
* _rtB -> B_55_32_0 [ 37 ] ; _rtB -> B_55_461_0 [ 0 ] = _rtP -> P_458 * _rtB
-> B_55_458_0 ; _rtB -> B_55_461_0 [ 1 ] = _rtP -> P_458 * _rtB -> B_55_459_0
; _rtB -> B_55_461_0 [ 2 ] = _rtP -> P_458 * _rtB -> B_55_460_0 ;
ssCallAccelRunBlock ( S , 55 , 462 , SS_CALL_MDL_OUTPUTS ) ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
55 , 463 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 464 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 465 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 466 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 467 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 468 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_55_469_0 = muDoubleScalarSin ( _rtP ->
P_461 * ssGetTaskTime ( S , 0 ) + _rtP -> P_462 ) * _rtP -> P_459 + _rtP ->
P_460 ; _rtB -> B_55_470_0 = _rtB -> B_55_457_0 [ 0 ] * _rtB -> B_55_469_0 ;
_rtB -> B_55_471_0 = muDoubleScalarSin ( _rtP -> P_465 * ssGetTaskTime ( S ,
0 ) + _rtP -> P_466 ) * _rtP -> P_463 + _rtP -> P_464 ; _rtB -> B_55_472_0 =
_rtB -> B_55_457_0 [ 0 ] * _rtB -> B_55_471_0 ; _rtB -> B_55_473_0 =
muDoubleScalarSin ( _rtP -> P_469 * ssGetTaskTime ( S , 0 ) + _rtP -> P_470 )
* _rtP -> P_467 + _rtP -> P_468 ; _rtB -> B_55_474_0 = _rtB -> B_55_461_0 [ 0
] * _rtB -> B_55_473_0 ; _rtB -> B_55_475_0 = muDoubleScalarSin ( _rtP ->
P_473 * ssGetTaskTime ( S , 0 ) + _rtP -> P_474 ) * _rtP -> P_471 + _rtP ->
P_472 ; _rtB -> B_55_476_0 = _rtB -> B_55_461_0 [ 0 ] * _rtB -> B_55_475_0 ;
_rtB -> B_55_477_0 = _rtX -> integrator_CSTATE_n ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_lg . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_lg .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_476 ; _rtB -> B_55_478_0 =
DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> TransportDelay_IWORK_n . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_n . Last , _rtDW -> TransportDelay_IWORK_n . Tail ,
_rtDW -> TransportDelay_IWORK_n . Head , _rtP -> P_477 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_55_479_0 = ssGetT ( S ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_55_480_0 = _rtP -> P_478 ; } _rtB ->
B_55_481_0 = ( _rtB -> B_55_479_0 >= _rtB -> B_55_480_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_482_0 = _rtDW
-> Memory_PreviousInput_ct ; } if ( _rtB -> B_55_481_0 ) { _rtB -> B_3_0_0 =
_rtB -> B_55_477_0 - _rtB -> B_55_478_0 ; _rtB -> B_3_1_0 = _rtP -> P_3 *
_rtB -> B_3_0_0 ; _rtB -> B_55_484_0 = _rtB -> B_3_1_0 ; } else { _rtB ->
B_55_484_0 = _rtB -> B_55_482_0 ; } _rtB -> B_55_485_0 = _rtX ->
integrator_CSTATE_iy ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_dw . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_dw . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_481 ; _rtB
-> B_55_486_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 ,
* tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dz . CircularBufSize ,
& _rtDW -> TransportDelay_IWORK_dz . Last , _rtDW -> TransportDelay_IWORK_dz
. Tail , _rtDW -> TransportDelay_IWORK_dz . Head , _rtP -> P_482 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_55_487_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_488_0 = _rtP
-> P_483 ; } _rtB -> B_55_489_0 = ( _rtB -> B_55_487_0 >= _rtB -> B_55_488_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_55_490_0 = _rtDW -> Memory_PreviousInput_ek ; } if ( _rtB -> B_55_489_0 ) {
_rtB -> B_2_0_0 = _rtB -> B_55_485_0 - _rtB -> B_55_486_0 ; _rtB -> B_2_1_0 =
_rtP -> P_2 * _rtB -> B_2_0_0 ; _rtB -> B_55_492_0 = _rtB -> B_2_1_0 ; } else
{ _rtB -> B_55_492_0 = _rtB -> B_55_490_0 ; } _rtB -> B_55_493_0 . re = _rtB
-> B_55_484_0 ; _rtB -> B_55_493_0 . im = _rtB -> B_55_492_0 ; _rtB ->
B_55_494_0 = muDoubleScalarHypot ( _rtB -> B_55_493_0 . re , _rtB ->
B_55_493_0 . im ) ; _rtB -> B_55_494_1 = muDoubleScalarAtan2 ( _rtB ->
B_55_493_0 . im , _rtB -> B_55_493_0 . re ) ; _rtB -> B_55_495_0 = _rtP ->
P_485 * _rtB -> B_55_494_1 ; _rtB -> B_55_496_0 = _rtX ->
integrator_CSTATE_jz ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_m . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_m . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_487 ; _rtB ->
B_55_497_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_er . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_er . Last , _rtDW -> TransportDelay_IWORK_er .
Tail , _rtDW -> TransportDelay_IWORK_er . Head , _rtP -> P_488 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_55_498_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_499_0 = _rtP
-> P_489 ; } _rtB -> B_55_500_0 = ( _rtB -> B_55_498_0 >= _rtB -> B_55_499_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_55_501_0 = _rtDW -> Memory_PreviousInput_mp ; } if ( _rtB -> B_55_500_0 ) {
_rtB -> B_5_0_0 = _rtB -> B_55_496_0 - _rtB -> B_55_497_0 ; _rtB -> B_5_1_0 =
_rtP -> P_5 * _rtB -> B_5_0_0 ; _rtB -> B_55_503_0 = _rtB -> B_5_1_0 ; } else
{ _rtB -> B_55_503_0 = _rtB -> B_55_501_0 ; } _rtB -> B_55_504_0 = _rtX ->
integrator_CSTATE_lg ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_el . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_el . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_492 ; _rtB
-> B_55_505_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 ,
* tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_eb . CircularBufSize ,
& _rtDW -> TransportDelay_IWORK_eb . Last , _rtDW -> TransportDelay_IWORK_eb
. Tail , _rtDW -> TransportDelay_IWORK_eb . Head , _rtP -> P_493 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_55_506_0 = ssGetT ( S ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_507_0 = _rtP
-> P_494 ; } _rtB -> B_55_508_0 = ( _rtB -> B_55_506_0 >= _rtB -> B_55_507_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_55_509_0 = _rtDW -> Memory_PreviousInput_k ; } if ( _rtB -> B_55_508_0 ) {
_rtB -> B_4_0_0 = _rtB -> B_55_504_0 - _rtB -> B_55_505_0 ; _rtB -> B_4_1_0 =
_rtP -> P_4 * _rtB -> B_4_0_0 ; _rtB -> B_55_511_0 = _rtB -> B_4_1_0 ; } else
{ _rtB -> B_55_511_0 = _rtB -> B_55_509_0 ; } _rtB -> B_55_512_0 . re = _rtB
-> B_55_503_0 ; _rtB -> B_55_512_0 . im = _rtB -> B_55_511_0 ; _rtB ->
B_55_513_0 = muDoubleScalarHypot ( _rtB -> B_55_512_0 . re , _rtB ->
B_55_512_0 . im ) ; _rtB -> B_55_513_1 = muDoubleScalarAtan2 ( _rtB ->
B_55_512_0 . im , _rtB -> B_55_512_0 . re ) ; _rtB -> B_55_514_0 = _rtP ->
P_496 * _rtB -> B_55_513_1 ; _rtB -> B_55_515_0 = _rtB -> B_55_495_0 - _rtB
-> B_55_514_0 ; _rtB -> B_55_516_0 = _rtP -> P_497 * _rtB -> B_55_515_0 ;
_rtB -> B_55_517_0 = muDoubleScalarCos ( _rtB -> B_55_516_0 ) ; _rtB ->
B_55_518_0 = _rtP -> P_498 * _rtB -> B_55_494_0 ; _rtB -> B_55_519_0 = _rtP
-> P_499 * _rtB -> B_55_513_0 ; _rtB -> B_55_520_0 = _rtB -> B_55_7_0_f *
_rtB -> B_55_517_0 * _rtB -> B_55_518_0 * _rtB -> B_55_519_0 ; _rtB ->
B_55_521_0 = _rtP -> P_500 * _rtB -> B_55_520_0 ; _rtB -> B_55_522_0 =
muDoubleScalarSin ( _rtB -> B_55_516_0 ) ; _rtB -> B_55_523_0 = _rtB ->
B_55_7_0_f * _rtB -> B_55_522_0 * _rtB -> B_55_518_0 * _rtB -> B_55_519_0 ;
_rtB -> B_55_524_0 = _rtP -> P_501 * _rtB -> B_55_523_0 ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
55 , 525 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 526 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 527 ,
SS_CALL_MDL_OUTPUTS ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_55_528_0 = _rtP -> P_502 * _rtB -> B_55_32_0 [ 26 ] ; _rtB ->
B_55_529_0 = _rtP -> P_503 * _rtB -> B_55_32_0 [ 27 ] ; _rtB -> B_55_530_0 =
_rtP -> P_504 * _rtB -> B_55_32_0 [ 28 ] ; _rtB -> B_55_531_0 [ 0 ] = _rtP ->
P_505 * _rtB -> B_55_528_0 ; _rtB -> B_55_531_0 [ 1 ] = _rtP -> P_505 * _rtB
-> B_55_529_0 ; _rtB -> B_55_531_0 [ 2 ] = _rtP -> P_505 * _rtB -> B_55_530_0
; _rtB -> B_55_532_0 = _rtP -> P_506 * _rtB -> B_55_32_0 [ 50 ] ; _rtB ->
B_55_533_0 = _rtP -> P_507 * _rtB -> B_55_32_0 [ 51 ] ; _rtB -> B_55_534_0 =
_rtP -> P_508 * _rtB -> B_55_32_0 [ 52 ] ; _rtB -> B_55_535_0 [ 0 ] = _rtP ->
P_509 * _rtB -> B_55_532_0 ; _rtB -> B_55_535_0 [ 1 ] = _rtP -> P_509 * _rtB
-> B_55_533_0 ; _rtB -> B_55_535_0 [ 2 ] = _rtP -> P_509 * _rtB -> B_55_534_0
; ssCallAccelRunBlock ( S , 55 , 536 , SS_CALL_MDL_OUTPUTS ) ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
55 , 537 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 538 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 539 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 540 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 541 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 542 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_55_543_0 = muDoubleScalarSin ( _rtP ->
P_512 * ssGetTaskTime ( S , 0 ) + _rtP -> P_513 ) * _rtP -> P_510 + _rtP ->
P_511 ; _rtB -> B_55_544_0 = _rtB -> B_55_531_0 [ 0 ] * _rtB -> B_55_543_0 ;
_rtB -> B_55_545_0 = muDoubleScalarSin ( _rtP -> P_516 * ssGetTaskTime ( S ,
0 ) + _rtP -> P_517 ) * _rtP -> P_514 + _rtP -> P_515 ; _rtB -> B_55_546_0 =
_rtB -> B_55_531_0 [ 0 ] * _rtB -> B_55_545_0 ; _rtB -> B_55_547_0 =
muDoubleScalarSin ( _rtP -> P_520 * ssGetTaskTime ( S , 0 ) + _rtP -> P_521 )
* _rtP -> P_518 + _rtP -> P_519 ; _rtB -> B_55_548_0 = _rtB -> B_55_535_0 [ 0
] * _rtB -> B_55_547_0 ; _rtB -> B_55_549_0 = muDoubleScalarSin ( _rtP ->
P_524 * ssGetTaskTime ( S , 0 ) + _rtP -> P_525 ) * _rtP -> P_522 + _rtP ->
P_523 ; _rtB -> B_55_550_0 = _rtB -> B_55_535_0 [ 0 ] * _rtB -> B_55_549_0 ;
isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_551_0
= _rtDW -> dw_delay_DSTATE ; _rtB -> B_55_552_0 = _rtP -> P_527 * _rtB ->
B_55_551_0 ; _rtB -> B_55_553_0 = _rtDW -> dw_predict_DSTATE ; _rtB ->
B_55_554_0 = _rtB -> B_55_552_0 - _rtB -> B_55_553_0 ; _rtB -> B_55_555_0 =
_rtB -> B_55_24_0_l + _rtB -> B_55_554_0 ; _rtB -> B_55_556_0 = _rtP -> P_529
* _rtB -> B_55_555_0 ; _rtB -> B_55_557_0 [ 0 ] = _rtP -> P_530 [ 0 ] * _rtB
-> B_55_13_0 [ 0 ] ; _rtB -> B_55_557_0 [ 1 ] = _rtP -> P_530 [ 1 ] * _rtB ->
B_55_13_0 [ 1 ] ; _rtB -> B_55_558_0 [ 0 ] = _rtB -> B_55_8_0 [ 1 ] * _rtB ->
B_55_557_0 [ 0 ] ; _rtB -> B_55_558_0 [ 1 ] = _rtB -> B_55_8_0 [ 0 ] * _rtB
-> B_55_557_0 [ 1 ] ; tmpForInput_idx_0 = _rtB -> B_55_558_0 [ 0 ] ;
tmpForInput_idx_0 += _rtB -> B_55_558_0 [ 1 ] ; _rtB -> B_55_559_0 =
tmpForInput_idx_0 ; _rtB -> B_55_560_0 = _rtB -> B_55_555_0 * _rtB ->
B_55_559_0 ; _rtB -> B_55_561_0 = _rtP -> P_531 * _rtB -> B_55_560_0 ; _rtB
-> B_55_562_0 = _rtDW -> theta_DSTATE ; tmpForInput_idx_1 = _rtB ->
B_55_562_0 ; tmpForInput_idx_0 = muDoubleScalarRem ( tmpForInput_idx_1 ,
6.2831853071795862 ) ; _rtB -> B_55_563_0 = tmpForInput_idx_0 ; _rtB ->
B_55_564_0 = _rtP -> P_534 * _rtB -> B_55_563_0 ; _rtB -> B_55_565_0 = _rtP
-> P_535 * _rtB -> B_55_559_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_55_566_0 [ 0 ] = _rtB -> B_55_3_0 ; _rtB ->
B_55_566_0 [ 1 ] = _rtB -> B_55_556_0 ; _rtB -> B_55_566_0 [ 2 ] = _rtB ->
B_55_561_0 ; _rtB -> B_55_566_0 [ 3 ] = _rtB -> B_55_554_0 ; _rtB ->
B_55_566_0 [ 4 ] = _rtB -> B_55_564_0 ; _rtB -> B_55_566_0 [ 5 ] = _rtB ->
B_55_565_0 ; } ssCallAccelRunBlock ( S , 55 , 567 , SS_CALL_MDL_OUTPUTS ) ; }
_rtB -> B_55_568_0 = _rtX -> Integrator2_CSTATE ; _rtB -> B_55_569_0 = _rtP
-> P_537 * _rtB -> B_55_568_0 ; _rtB -> B_55_570_0 = _rtX ->
Integrator1_CSTATE ; _rtB -> B_55_571_0 = _rtP -> P_539 * _rtB -> B_55_570_0
; _rtB -> B_55_572_0 = _rtX -> Integrator3_CSTATE ; _rtB -> B_55_573_0 = _rtP
-> P_541 * _rtB -> B_55_572_0 ; _rtB -> B_55_574_0 = ( _rtB -> B_55_569_0 +
_rtB -> B_55_571_0 ) + _rtB -> B_55_573_0 ; isHit = ssIsSampleHit ( S , 2 , 0
) ; if ( isHit != 0 ) { _rtB -> B_55_575_0 = _rtDW -> UnitDelay2_DSTATE ; if
( _rtB -> B_55_25_0_o > _rtP -> P_543 ) { _rtB -> B_55_576_0 = _rtB ->
B_55_575_0 ; } else { _rtB -> B_55_576_0 = _rtB -> B_55_26_0_b ; } } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
55 , 577 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 578 ,
SS_CALL_MDL_OUTPUTS ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit !=
0 ) { if ( _rtB -> B_55_9_0_c ) { _rtB -> B_55_579_0 = _rtB -> B_55_13_0 [ 5
] ; } else { _rtB -> B_55_579_0 = _rtB -> B_55_10_0 ; } _rtB -> B_55_580_0 [
0 ] = _rtP -> P_544 [ 0 ] * _rtB -> B_55_8_0 [ 0 ] ; _rtB -> B_55_580_0 [ 1 ]
= _rtP -> P_544 [ 1 ] * _rtB -> B_55_8_0 [ 4 ] ; _rtB -> B_55_580_0 [ 2 ] =
_rtP -> P_544 [ 2 ] * _rtB -> B_55_8_0 [ 5 ] ; if ( _rtB -> B_55_17_0_d >=
_rtP -> P_545 ) { _rtB -> B_55_581_0 [ 0 ] = _rtB -> B_47_20_0 ; _rtB ->
B_55_581_0 [ 1 ] = _rtB -> B_47_6_0 ; } else { _rtB -> B_55_581_0 [ 0 ] =
_rtB -> B_55_18_0_g [ 0 ] ; _rtB -> B_55_581_0 [ 1 ] = _rtB -> B_55_18_0_g [
1 ] ; } _rtB -> B_55_582_0 [ 0 ] = _rtB -> B_55_580_0 [ 0 ] * _rtB ->
B_55_581_0 [ 0 ] ; _rtB -> B_55_582_0 [ 1 ] = _rtB -> B_55_580_0 [ 1 ] * _rtB
-> B_55_581_0 [ 0 ] ; _rtB -> B_55_582_0 [ 2 ] = _rtB -> B_55_580_0 [ 2 ] *
_rtB -> B_55_581_0 [ 0 ] ; tmpForInput_idx_0 = _rtB -> B_55_582_0 [ 0 ] ;
_rtB -> B_55_584_0 [ 0 ] = _rtP -> P_546 [ 0 ] * _rtB -> B_55_8_0 [ 1 ] ;
_rtB -> B_55_585_0 [ 0 ] = _rtB -> B_55_584_0 [ 0 ] * _rtB -> B_55_581_0 [ 1
] ; tmpForInput_idx_0 += _rtB -> B_55_582_0 [ 1 ] ; _rtB -> B_55_584_0 [ 1 ]
= _rtP -> P_546 [ 1 ] * _rtB -> B_55_8_0 [ 2 ] ; _rtB -> B_55_585_0 [ 1 ] =
_rtB -> B_55_584_0 [ 1 ] * _rtB -> B_55_581_0 [ 1 ] ; tmpForInput_idx_0 +=
_rtB -> B_55_582_0 [ 2 ] ; _rtB -> B_55_584_0 [ 2 ] = _rtP -> P_546 [ 2 ] *
_rtB -> B_55_8_0 [ 3 ] ; _rtB -> B_55_585_0 [ 2 ] = _rtB -> B_55_584_0 [ 2 ]
* _rtB -> B_55_581_0 [ 1 ] ; _rtB -> B_55_583_0 = tmpForInput_idx_0 ;
tmpForInput_idx_0 = _rtB -> B_55_585_0 [ 0 ] ; tmpForInput_idx_0 += _rtB ->
B_55_585_0 [ 1 ] ; tmpForInput_idx_0 += _rtB -> B_55_585_0 [ 2 ] ; _rtB ->
B_55_586_0 = tmpForInput_idx_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_55_587_0 [ 0 ] = _rtB -> B_55_32_0 [ 9 ] ; _rtB ->
B_55_587_0 [ 1 ] = _rtB -> B_55_32_0 [ 10 ] ; } _rtB -> B_55_588_0 [ 0 ] =
_rtP -> P_547 * _rtB -> B_55_587_0 [ 0 ] ; _rtB -> B_55_588_0 [ 1 ] = _rtP ->
P_547 * _rtB -> B_55_587_0 [ 1 ] ; _rtB -> B_55_589_0 = ( ( 2.0 * _rtB ->
B_55_588_0 [ 0 ] + _rtB -> B_55_588_0 [ 1 ] ) * _rtB -> B_55_7_1 +
1.7320508075688772 * _rtB -> B_55_588_0 [ 1 ] * _rtB -> B_55_7_0 ) *
0.33333333333333331 ; _rtB -> B_55_590_0 = ( ( 2.0 * _rtB -> B_55_588_0 [ 0 ]
+ _rtB -> B_55_588_0 [ 1 ] ) * _rtB -> B_55_7_0 + - 1.7320508075688772 * _rtB
-> B_55_588_0 [ 1 ] * _rtB -> B_55_7_1 ) * 0.33333333333333331 ; if ( _rtB ->
B_55_15_0_l >= _rtP -> P_548 ) { _rtB -> B_55_591_0 [ 0 ] = _rtB -> B_47_14_0
; _rtB -> B_55_591_0 [ 1 ] = _rtB -> B_47_12_0 ; } else { _rtB -> B_55_591_0
[ 0 ] = _rtB -> B_55_19_0_l [ 0 ] ; _rtB -> B_55_591_0 [ 1 ] = _rtB ->
B_55_19_0_l [ 1 ] ; } _rtB -> B_55_592_0 = muDoubleScalarAtan2 ( _rtB ->
B_55_590_0 , _rtB -> B_55_589_0 ) ; _rtB -> B_55_593_0 = _rtP -> P_549 * _rtB
-> B_55_592_0 ; _rtB -> B_55_594_0 = _rtB -> B_55_589_0 * _rtB -> B_55_13_0 [
0 ] ; _rtB -> B_55_595_0 = _rtB -> B_55_590_0 * _rtB -> B_55_13_0 [ 1 ] ;
_rtB -> B_55_596_0 = _rtB -> B_55_594_0 + _rtB -> B_55_595_0 ; _rtB ->
B_55_597_0 = _rtP -> P_550 * _rtB -> B_55_596_0 ; _rtB -> B_55_598_0 = _rtB
-> B_55_589_0 * _rtB -> B_55_13_0 [ 1 ] ; _rtB -> B_55_599_0 = _rtB ->
B_55_590_0 * _rtB -> B_55_13_0 [ 0 ] ; _rtB -> B_55_600_0 = _rtB ->
B_55_598_0 - _rtB -> B_55_599_0 ; _rtB -> B_55_601_0 = _rtP -> P_551 * _rtB
-> B_55_600_0 ; _rtB -> B_55_602_0 [ 0 ] = _rtP -> P_552 [ 0 ] * _rtB ->
B_55_14_0 ; _rtB -> B_55_602_0 [ 1 ] = _rtP -> P_552 [ 1 ] * _rtB ->
B_55_15_0 ; _rtB -> B_55_602_0 [ 2 ] = _rtP -> P_552 [ 2 ] * _rtB ->
B_55_16_0 ; _rtB -> B_55_602_0 [ 3 ] = _rtP -> P_552 [ 3 ] * _rtB ->
B_55_13_0 [ 0 ] ; _rtB -> B_55_602_0 [ 4 ] = _rtP -> P_552 [ 4 ] * _rtB ->
B_55_13_0 [ 1 ] ; _rtB -> B_55_602_0 [ 5 ] = _rtP -> P_552 [ 5 ] * _rtB ->
B_55_13_0 [ 2 ] ; _rtB -> B_55_602_0 [ 6 ] = _rtP -> P_552 [ 6 ] * _rtB ->
B_55_13_0 [ 4 ] ; _rtB -> B_55_602_0 [ 7 ] = _rtP -> P_552 [ 7 ] * _rtB ->
B_55_579_0 ; _rtB -> B_55_602_0 [ 8 ] = _rtP -> P_552 [ 8 ] * _rtB ->
B_55_13_0 [ 3 ] ; _rtB -> B_55_602_0 [ 9 ] = _rtP -> P_552 [ 9 ] * _rtB ->
B_55_583_0 ; _rtB -> B_55_602_0 [ 10 ] = _rtP -> P_552 [ 10 ] * _rtB ->
B_55_586_0 ; _rtB -> B_55_602_0 [ 11 ] = _rtP -> P_552 [ 11 ] * _rtB ->
B_55_589_0 ; _rtB -> B_55_602_0 [ 12 ] = _rtP -> P_552 [ 12 ] * _rtB ->
B_55_590_0 ; _rtB -> B_55_602_0 [ 13 ] = _rtP -> P_552 [ 13 ] * _rtB ->
B_55_591_0 [ 0 ] ; _rtB -> B_55_602_0 [ 14 ] = _rtP -> P_552 [ 14 ] * _rtB ->
B_55_591_0 [ 1 ] ; _rtB -> B_55_602_0 [ 15 ] = _rtP -> P_552 [ 15 ] * _rtB ->
B_55_593_0 ; _rtB -> B_55_602_0 [ 16 ] = _rtP -> P_552 [ 16 ] * _rtB ->
B_55_597_0 ; _rtB -> B_55_602_0 [ 17 ] = _rtP -> P_552 [ 17 ] * _rtB ->
B_55_601_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { memcpy
( & _rtB -> B_55_603_0 [ 0 ] , & _rtB -> B_55_602_0 [ 0 ] , 18U * sizeof (
real_T ) ) ; } _rtB -> B_55_604_0 = _rtP -> P_553 * _rtB -> B_55_603_0 [ 16 ]
; ssCallAccelRunBlock ( S , 55 , 605 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 606 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_55_607_0 = _rtP -> P_554 * _rtB -> B_55_603_0 [ 17 ] ; ssCallAccelRunBlock
( S , 55 , 608 , SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_55_609_0 = _rtB ->
B_55_603_0 [ 14 ] * _rtB -> B_55_8_0_g ; _rtB -> B_55_610_0 = _rtB ->
B_55_603_0 [ 5 ] * _rtB -> B_55_609_0 ; ssCallAccelRunBlock ( S , 55 , 611 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 55 , 612 ,
SS_CALL_MDL_OUTPUTS ) ; if ( _rtP -> P_810 == 1 ) { _rtB -> B_55_613_0 = _rtB
-> B_55_576_0 ; } else { _rtB -> B_55_613_0 = 0.0 ; } _rtB -> B_55_614_0 =
muDoubleScalarHypot ( _rtB -> B_55_589_0 , _rtB -> B_55_590_0 ) ; _rtB ->
B_55_615_0 = _rtP -> P_555 * _rtB -> B_55_613_0 ; for ( i = 0 ; i < 6 ; i ++
) { _rtB -> B_55_616_0 [ i ] = _rtDW -> voltages_DSTATE [ i ] ; } _rtB ->
B_55_617_0 = ssGetTaskTime ( S , 2 ) ; if ( _rtB -> B_55_617_0 >= _rtP ->
P_557 ) { _rtB -> B_48_0_0 [ 0 ] = _rtB -> B_55_589_0 + _rtB -> B_55_616_0 [
0 ] ; _rtB -> B_48_0_0 [ 1 ] = _rtB -> B_55_590_0 + _rtB -> B_55_616_0 [ 1 ]
; _rtB -> B_48_0_0 [ 2 ] = _rtB -> B_55_615_0 + _rtB -> B_55_616_0 [ 2 ] ;
_rtB -> B_48_0_0 [ 3 ] = _rtB -> B_55_23_0_d [ 0 ] + _rtB -> B_55_616_0 [ 3 ]
; _rtB -> B_48_0_0 [ 4 ] = _rtB -> B_55_23_0_d [ 1 ] + _rtB -> B_55_616_0 [ 4
] ; _rtB -> B_48_0_0 [ 5 ] = _rtB -> B_55_23_0_d [ 2 ] + _rtB -> B_55_616_0 [
5 ] ; for ( i = 0 ; i < 36 ; i ++ ) { if ( _rtB -> B_55_12_0_m >= _rtP ->
P_78 ) { _rtB -> B_48_1_0 [ i ] = _rtB -> B_47_19_0 [ i ] ; } else { _rtB ->
B_48_1_0 [ i ] = _rtB -> B_55_16_0_j [ i ] ; } _rtB -> B_48_2_0 [ i ] = _rtB
-> B_55_20_0_d [ i ] ; } _rtB -> B_48_2_0 [ 6 ] = _rtB -> B_55_555_0 ; _rtB
-> B_48_3_0 = _rtP -> P_79 * _rtB -> B_55_555_0 ; memcpy ( & _rtB -> B_48_4_0
[ 0 ] , & _rtB -> B_48_2_0 [ 0 ] , 36U * sizeof ( real_T ) ) ; _rtB ->
B_48_4_0 [ 1 ] = _rtB -> B_48_3_0 ; for ( i = 0 ; i < 36 ; i ++ ) { _rtB ->
B_48_5_0 [ i ] = ( 0.0 - _rtB -> B_48_4_0 [ i ] ) - _rtB -> B_48_1_0 [ i ] ;
_rtB -> B_48_6_0 [ i ] = _rtP -> P_80 * _rtB -> B_48_5_0 [ i ] ; _rtB ->
B_48_7_0 [ i ] = _rtB -> B_55_22_0 [ i ] - _rtB -> B_48_6_0 [ i ] ; }
rt_invd6x6_snf ( _rtB -> B_48_7_0 , _rtB -> B_48_8_0 ) ; for ( i = 0 ; i < 36
; i ++ ) { _rtB -> B_48_9_0 [ i ] = _rtP -> P_81 * _rtB -> B_48_8_0 [ i ] ;
tmp_1 [ i ] = _rtB -> B_48_9_0 [ i ] ; } for ( i = 0 ; i < 6 ; i ++ ) { tmp [
i ] = _rtB -> B_48_0_0 [ i ] ; } for ( isHit = 0 ; isHit < 6 ; isHit ++ ) {
tmp_0 [ isHit ] = 0.0 ; for ( i = 0 ; i < 6 ; i ++ ) { tmpForInput_idx_1 =
tmp_0 [ isHit ] ; tmpForInput_idx_1 += tmp_1 [ 6 * i + isHit ] * tmp [ i ] ;
tmp_0 [ isHit ] = tmpForInput_idx_1 ; } _rtB -> B_48_10_0 [ isHit ] = tmp_0 [
isHit ] ; } for ( i = 0 ; i < 36 ; i ++ ) { _rtB -> B_48_11_0 [ i ] = _rtB ->
B_55_22_0 [ i ] + _rtB -> B_48_6_0 [ i ] ; tmp_1 [ i ] = _rtB -> B_48_8_0 [ i
] ; tmp_2 [ i ] = _rtB -> B_48_11_0 [ i ] ; } for ( isHit = 0 ; isHit < 6 ;
isHit ++ ) { for ( i = 0 ; i < 6 ; i ++ ) { _rtB -> B_48_12_0 [ i + 6 * isHit
] = 0.0 ; for ( i_0 = 0 ; i_0 < 6 ; i_0 ++ ) { _rtB -> B_48_12_0 [ i + 6 *
isHit ] += tmp_1 [ 6 * i_0 + i ] * tmp_2 [ 6 * isHit + i_0 ] ; } } } memcpy (
& tmp_1 [ 0 ] , & _rtB -> B_48_12_0 [ 0 ] , 36U * sizeof ( real_T ) ) ; for (
i = 0 ; i < 6 ; i ++ ) { tmp [ i ] = _rtB -> B_55_8_0 [ i ] ; } for ( i = 0 ;
i < 6 ; i ++ ) { tmp_0 [ i ] = 0.0 ; for ( isHit = 0 ; isHit < 6 ; isHit ++ )
{ tmpForInput_idx_1 = tmp_0 [ i ] ; tmpForInput_idx_1 += tmp_1 [ 6 * isHit +
i ] * tmp [ isHit ] ; tmp_0 [ i ] = tmpForInput_idx_1 ; } _rtB -> B_48_13_0 [
i ] = tmp_0 [ i ] ; _rtB -> B_48_14_0 [ i ] = _rtB -> B_48_13_0 [ i ] + _rtB
-> B_48_10_0 [ i ] ; } for ( i = 0 ; i < 6 ; i ++ ) { _rtB -> B_55_619_0 [ i
] = _rtB -> B_48_14_0 [ i ] ; } } else { for ( i = 0 ; i < 6 ; i ++ ) { _rtB
-> B_55_619_0 [ i ] = _rtB -> B_55_8_0 [ i ] ; } } } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( ( isHit != 0 ) && ( _rtDW ->
RateTransition1_semaphoreTaken == 0 ) ) { _rtDW -> RateTransition1_Buffer0 =
_rtB -> B_55_574_0 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0
) { _rtDW -> RateTransition1_semaphoreTaken = 1 ; _rtB -> B_55_620_0 = _rtDW
-> RateTransition1_Buffer0 ; _rtDW -> RateTransition1_semaphoreTaken = 0 ;
_rtB -> B_55_621_0 = _rtP -> P_558 * _rtB -> B_55_620_0 ; _rtB -> B_55_622_0
= _rtB -> B_55_621_0 / _rtB -> B_55_555_0 ; _rtB -> B_55_623_0 = _rtP ->
P_559 * _rtB -> B_55_555_0 ; _rtB -> B_55_624_0 = ( _rtB -> B_55_622_0 - _rtB
-> B_55_559_0 ) - _rtB -> B_55_623_0 ; _rtB -> B_55_625_0 = _rtP -> P_560 *
_rtB -> B_55_624_0 ; if ( _rtDW -> Rotorspeeddeviationdw_SYSTEM_ENABLE != 0 )
{ _rtB -> B_55_626_0 = _rtDW -> Rotorspeeddeviationdw_DSTATE ; } else { _rtB
-> B_55_626_0 = _rtP -> P_561 * _rtB -> B_55_625_0 + _rtDW ->
Rotorspeeddeviationdw_DSTATE ; } _rtB -> B_55_627_0 = _rtP -> P_563 * _rtB ->
B_55_554_0 ; _rtB -> B_55_628_0 = _rtP -> P_564 * _rtB -> B_55_555_0 ; { _rtB
-> B_55_634_0 = ( _rtP -> P_567 [ 0 ] ) * _rtDW -> DiscreteStateSpace_DSTATE
[ 0 ] + ( _rtP -> P_567 [ 1 ] ) * _rtDW -> DiscreteStateSpace_DSTATE [ 1 ] ;
_rtB -> B_55_634_0 += ( _rtP -> P_568 ) * _rtB -> B_55_566_0 [ 3 ] ; } _rtB
-> B_55_635_0 = _rtP -> P_570 * _rtB -> B_55_634_0 ; { _rtB -> B_55_636_0 =
_rtP -> P_573 * _rtDW -> DiscreteStateSpace_DSTATE_j ; _rtB -> B_55_636_0 +=
_rtP -> P_574 * _rtB -> B_55_635_0 ; } _rtB -> B_55_637_0 = _rtP -> P_576 *
_rtB -> B_55_634_0 ; { _rtB -> B_55_638_0 = _rtP -> P_579 * _rtDW ->
DiscreteStateSpace_DSTATE_b ; _rtB -> B_55_638_0 += _rtP -> P_580 * _rtB ->
B_55_637_0 ; } _rtB -> B_55_639_0 = _rtP -> P_582 * _rtB -> B_55_634_0 ; {
_rtB -> B_55_640_0 = _rtP -> P_585 * _rtDW -> DiscreteStateSpace_DSTATE_o ;
_rtB -> B_55_640_0 += _rtP -> P_586 * _rtB -> B_55_639_0 ; } _rtB ->
B_55_641_0 = _rtP -> P_588 * _rtB -> B_55_634_0 ; { _rtB -> B_55_642_0 = _rtP
-> P_591 * _rtDW -> DiscreteStateSpace_DSTATE_f ; _rtB -> B_55_642_0 += _rtP
-> P_592 * _rtB -> B_55_641_0 ; } { _rtB -> B_55_643_0 = ( _rtP -> P_596 [ 0
] ) * _rtDW -> DiscreteStateSpace_DSTATE_bk [ 0 ] + ( _rtP -> P_596 [ 1 ] ) *
_rtDW -> DiscreteStateSpace_DSTATE_bk [ 1 ] + ( _rtP -> P_596 [ 2 ] ) * _rtDW
-> DiscreteStateSpace_DSTATE_bk [ 2 ] ; _rtB -> B_55_643_0 += _rtP -> P_597 *
_rtB -> B_55_566_0 [ 3 ] ; } _rtB -> B_55_644_0 = _rtP -> P_599 * _rtB ->
B_55_643_0 ; { _rtB -> B_55_645_0 = _rtP -> P_602 * _rtDW ->
DiscreteStateSpace_DSTATE_a ; _rtB -> B_55_645_0 += _rtP -> P_603 * _rtB ->
B_55_644_0 ; } _rtB -> B_55_646_0 = _rtP -> P_605 * _rtB -> B_55_643_0 ; {
_rtB -> B_55_647_0 = _rtP -> P_608 * _rtDW -> DiscreteStateSpace_DSTATE_l ;
_rtB -> B_55_647_0 += _rtP -> P_609 * _rtB -> B_55_646_0 ; } _rtB ->
B_55_648_0 = _rtB -> B_55_27_0_n ;
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer ( S , _rtB -> B_55_648_0
, _rtB -> B_55_636_0 , & _rtB -> Transferfunctionsfordetailedstabilizer_j , &
_rtDW -> Transferfunctionsfordetailedstabilizer_j , & _rtP ->
Transferfunctionsfordetailedstabilizer_j ) ; if ( _rtB -> B_55_27_0_n >= _rtP
-> P_611 ) { _rtB -> B_55_650_0 = _rtB ->
Transferfunctionsfordetailedstabilizer_j . B_49_1_0 ; } else { _rtB ->
B_55_650_0 = _rtB -> B_55_636_0 ; } _rtB -> B_55_651_0 = _rtB -> B_55_27_0_n
; DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer ( S , _rtB ->
B_55_651_0 , _rtB -> B_55_638_0 , & _rtB ->
Transferfunctionsfordetailedstabilizer1_g , & _rtDW ->
Transferfunctionsfordetailedstabilizer1_g , & _rtP ->
Transferfunctionsfordetailedstabilizer1_g ) ; if ( _rtB -> B_55_27_0_n >=
_rtP -> P_612 ) { _rtB -> B_55_653_0 = _rtB ->
Transferfunctionsfordetailedstabilizer1_g . B_49_1_0 ; } else { _rtB ->
B_55_653_0 = _rtB -> B_55_638_0 ; } _rtB -> B_55_654_0 = _rtB -> B_55_650_0 -
_rtB -> B_55_653_0 ; _rtB -> B_55_655_0 = _rtP -> P_613 * _rtB -> B_55_654_0
; _rtB -> B_55_656_0 = _rtB -> B_55_27_0_n ;
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer ( S , _rtB -> B_55_656_0
, _rtB -> B_55_640_0 , & _rtB -> Transferfunctionsfordetailedstabilizer_e , &
_rtDW -> Transferfunctionsfordetailedstabilizer_e , & _rtP ->
Transferfunctionsfordetailedstabilizer_e ) ; if ( _rtB -> B_55_27_0_n >= _rtP
-> P_614 ) { _rtB -> B_55_658_0 = _rtB ->
Transferfunctionsfordetailedstabilizer_e . B_49_1_0 ; } else { _rtB ->
B_55_658_0 = _rtB -> B_55_640_0 ; } _rtB -> B_55_659_0 = _rtB -> B_55_27_0_n
; DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer ( S , _rtB ->
B_55_659_0 , _rtB -> B_55_642_0 , & _rtB ->
Transferfunctionsfordetailedstabilizer1_n , & _rtDW ->
Transferfunctionsfordetailedstabilizer1_n , & _rtP ->
Transferfunctionsfordetailedstabilizer1_n ) ; if ( _rtB -> B_55_27_0_n >=
_rtP -> P_615 ) { _rtB -> B_55_661_0 = _rtB ->
Transferfunctionsfordetailedstabilizer1_n . B_49_1_0 ; } else { _rtB ->
B_55_661_0 = _rtB -> B_55_642_0 ; } _rtB -> B_55_662_0 = _rtB -> B_55_658_0 -
_rtB -> B_55_661_0 ; _rtB -> B_55_663_0 = _rtP -> P_616 * _rtB -> B_55_662_0
; _rtB -> B_55_664_0 = _rtB -> B_55_27_0_n ;
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer ( S , _rtB -> B_55_664_0
, _rtB -> B_55_645_0 , & _rtB -> Transferfunctionsfordetailedstabilizer , &
_rtDW -> Transferfunctionsfordetailedstabilizer , & _rtP ->
Transferfunctionsfordetailedstabilizer ) ; if ( _rtB -> B_55_27_0_n >= _rtP
-> P_617 ) { _rtB -> B_55_666_0 = _rtB ->
Transferfunctionsfordetailedstabilizer . B_49_1_0 ; } else { _rtB ->
B_55_666_0 = _rtB -> B_55_645_0 ; } _rtB -> B_55_667_0 = _rtB -> B_55_27_0_n
; DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer ( S , _rtB ->
B_55_667_0 , _rtB -> B_55_647_0 , & _rtB ->
Transferfunctionsfordetailedstabilizer1 , & _rtDW ->
Transferfunctionsfordetailedstabilizer1 , & _rtP ->
Transferfunctionsfordetailedstabilizer1 ) ; if ( _rtB -> B_55_27_0_n >= _rtP
-> P_618 ) { _rtB -> B_55_669_0 = _rtB ->
Transferfunctionsfordetailedstabilizer1 . B_49_1_0 ; } else { _rtB ->
B_55_669_0 = _rtB -> B_55_647_0 ; } _rtB -> B_55_670_0 = _rtB -> B_55_666_0 -
_rtB -> B_55_669_0 ; _rtB -> B_55_671_0 = _rtP -> P_619 * _rtB -> B_55_670_0
; tmpForInput_idx_0 = _rtB -> B_55_655_0 ; tmpForInput_idx_1 = _rtP -> P_621
; tmpForInput_idx_2 = _rtP -> P_620 ; if ( tmpForInput_idx_0 >
tmpForInput_idx_2 ) { _rtB -> B_55_672_0 = tmpForInput_idx_2 ; } else if (
tmpForInput_idx_0 < tmpForInput_idx_1 ) { _rtB -> B_55_672_0 =
tmpForInput_idx_1 ; } else { _rtB -> B_55_672_0 = tmpForInput_idx_0 ; }
tmpForInput_idx_0 = _rtB -> B_55_663_0 ; tmpForInput_idx_1 = _rtP -> P_623 ;
tmpForInput_idx_2 = _rtP -> P_622 ; if ( tmpForInput_idx_0 >
tmpForInput_idx_2 ) { _rtB -> B_55_673_0 = tmpForInput_idx_2 ; } else if (
tmpForInput_idx_0 < tmpForInput_idx_1 ) { _rtB -> B_55_673_0 =
tmpForInput_idx_1 ; } else { _rtB -> B_55_673_0 = tmpForInput_idx_0 ; }
tmpForInput_idx_0 = _rtB -> B_55_671_0 ; tmpForInput_idx_1 = _rtP -> P_625 ;
tmpForInput_idx_2 = _rtP -> P_624 ; if ( tmpForInput_idx_0 >
tmpForInput_idx_2 ) { _rtB -> B_55_674_0 = tmpForInput_idx_2 ; } else if (
tmpForInput_idx_0 < tmpForInput_idx_1 ) { _rtB -> B_55_674_0 =
tmpForInput_idx_1 ; } else { _rtB -> B_55_674_0 = tmpForInput_idx_0 ; } _rtB
-> B_55_675_0 = ( _rtB -> B_55_672_0 + _rtB -> B_55_673_0 ) + _rtB ->
B_55_674_0 ; _rtB -> B_55_676_0 = _rtP -> P_626 * _rtB -> B_55_675_0 ;
tmpForInput_idx_0 = _rtB -> B_55_676_0 ; tmpForInput_idx_1 = _rtP -> P_628 ;
tmpForInput_idx_2 = _rtP -> P_627 ; if ( tmpForInput_idx_0 >
tmpForInput_idx_2 ) { _rtB -> B_55_677_0 = tmpForInput_idx_2 ; } else if (
tmpForInput_idx_0 < tmpForInput_idx_1 ) { _rtB -> B_55_677_0 =
tmpForInput_idx_1 ; } else { _rtB -> B_55_677_0 = tmpForInput_idx_0 ; }
tmpForInput_idx_1 = _rtB -> B_55_603_0 [ 12 ] ; tmpForInput_idx_0 =
muDoubleScalarPower ( tmpForInput_idx_1 , 2.0 ) ; tmpForInput_idx_1 = _rtB ->
B_55_603_0 [ 11 ] ; tmpForInput_idx_1 = muDoubleScalarPower (
tmpForInput_idx_1 , 2.0 ) ; tmpForInput_idx_0 += tmpForInput_idx_1 ; if (
tmpForInput_idx_0 < 0.0 ) { tmpForInput_idx_0 = - muDoubleScalarSqrt ( -
tmpForInput_idx_0 ) ; } else { tmpForInput_idx_0 = muDoubleScalarSqrt (
tmpForInput_idx_0 ) ; } _rtB -> B_55_678_0 = tmpForInput_idx_0 ; { _rtB ->
B_55_679_0 = _rtP -> P_631 * _rtDW -> DiscreteStateSpace_DSTATE_i ; _rtB ->
B_55_679_0 += _rtP -> P_632 * _rtB -> B_55_678_0 ; } _rtB -> B_55_680_0 =
_rtDW -> UnitDelay1_DSTATE ; _rtB -> B_55_681_0 = ( ( ( _rtB -> B_55_28_0_b +
_rtB -> B_55_29_0_l ) - _rtB -> B_55_679_0 ) + _rtB -> B_55_677_0 ) - _rtB ->
B_55_680_0 ; { _rtB -> B_55_682_0 = _rtP -> P_638 * _rtB -> B_55_681_0 ; }
_rtB -> B_55_683_0 = _rtP -> P_640 * _rtB -> B_55_682_0 ; { _rtB ->
B_55_684_0 = _rtP -> P_643 * _rtDW -> DiscreteStateSpace_DSTATE_h ; _rtB ->
B_55_684_0 += _rtP -> P_644 * _rtB -> B_55_683_0 ; } _rtB -> B_55_685_0 = (
_rtB -> B_55_684_0 > _rtB -> B_55_31_0_b ) ; _rtB -> B_55_686_0 = _rtP ->
P_646 * _rtB -> B_55_679_0 ; _rtB -> B_55_687_0 = _rtB -> B_55_686_0 + _rtB
-> B_55_35_0_b ; _rtB -> B_55_688_0 = ( _rtB -> B_55_684_0 < _rtB ->
B_55_687_0 ) ; _rtB -> B_55_689_0 = ( _rtB -> B_55_685_0 && _rtB ->
B_55_688_0 ) ; _rtB -> B_55_690_0 = _rtB -> B_55_689_0 ; _rtB -> B_55_691_0 =
_rtB -> B_55_684_0 * _rtB -> B_55_690_0 ; _rtB -> B_55_692_0 = ! _rtB ->
B_55_685_0 ; _rtB -> B_55_693_0 = _rtB -> B_55_692_0 ; _rtB -> B_55_694_0 =
_rtB -> B_55_31_0_b * _rtB -> B_55_693_0 ; _rtB -> B_55_695_0 = ! _rtB ->
B_55_688_0 ; _rtB -> B_55_696_0 = _rtB -> B_55_695_0 ; _rtB -> B_55_697_0 =
_rtB -> B_55_696_0 * _rtB -> B_55_687_0 ; _rtB -> B_55_698_0 = ( _rtB ->
B_55_691_0 + _rtB -> B_55_694_0 ) + _rtB -> B_55_697_0 ; _rtB -> B_55_699_0 =
_rtP -> P_647 * _rtB -> B_55_698_0 ; { _rtB -> B_55_700_0 = _rtP -> P_650 *
_rtDW -> DiscreteStateSpace_DSTATE_be ; _rtB -> B_55_700_0 += _rtP -> P_651 *
_rtB -> B_55_699_0 ; } _rtB -> B_55_701_0 = _rtP -> P_653 * _rtB ->
B_55_700_0 ; { _rtB -> B_55_702_0 = ( _rtP -> P_656 ) * _rtDW ->
DiscreteStateSpace_DSTATE_n ; _rtB -> B_55_702_0 += _rtP -> P_657 * _rtB ->
B_55_701_0 ; } } if ( _rtX -> IntegratorLimited_CSTATE >= _rtP -> P_660 ) {
if ( _rtX -> IntegratorLimited_CSTATE != _rtP -> P_660 ) { _rtX ->
IntegratorLimited_CSTATE = _rtP -> P_660 ;
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } } else { if ( ( _rtX ->
IntegratorLimited_CSTATE <= _rtP -> P_661 ) && ( _rtX ->
IntegratorLimited_CSTATE != _rtP -> P_661 ) ) { _rtX ->
IntegratorLimited_CSTATE = _rtP -> P_661 ;
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } } _rtB -> B_55_703_0 =
_rtX -> IntegratorLimited_CSTATE ; if ( _rtB -> B_55_25_0_o > _rtP -> P_662 )
{ _rtB -> B_55_704_0 = _rtB -> B_55_703_0 ; } else { _rtB -> B_55_704_0 =
_rtB -> B_55_39_0 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_55_705_0 = _rtP -> P_663 * _rtB -> B_55_566_0 [ 3 ] ; _rtB ->
B_55_706_0 = _rtP -> P_664 * _rtB -> B_55_705_0 ; if ( _rtB -> B_55_706_0 >
_rtP -> P_666 ) { _rtB -> B_55_707_0 = _rtB -> B_55_706_0 - _rtP -> P_666 ; }
else if ( _rtB -> B_55_706_0 >= _rtP -> P_665 ) { _rtB -> B_55_707_0 = 0.0 ;
} else { _rtB -> B_55_707_0 = _rtB -> B_55_706_0 - _rtP -> P_665 ; } } _rtB
-> B_55_708_0 = _rtX -> Integrator1_CSTATE_i ; _rtB -> B_55_709_0 = _rtX ->
Integrator1_CSTATE_d ; _rtB -> B_55_710_0 = _rtX -> Integrator2_CSTATE_f ;
_rtB -> B_55_711_0 = _rtB -> B_55_710_0 - _rtB -> B_55_40_0_a ; _rtB ->
B_55_712_0 = _rtP -> P_670 * _rtB -> B_55_711_0 ; _rtB -> B_55_713_0 = _rtB
-> B_55_709_0 + _rtB -> B_55_712_0 ; _rtB -> B_55_714_0 = ( ( _rtB ->
B_55_36_0_j + _rtB -> B_55_707_0 ) + _rtB -> B_55_713_0 ) - _rtB ->
B_55_708_0 ; _rtB -> B_55_715_0 = _rtP -> P_671 * _rtB -> B_55_714_0 ; _rtB
-> B_55_716_0 = _rtB -> B_55_708_0 - _rtB -> B_55_703_0 ; _rtB -> B_55_717_0
= _rtP -> P_672 * _rtB -> B_55_716_0 ; tmpForInput_idx_0 = _rtB -> B_55_717_0
; tmpForInput_idx_1 = _rtP -> P_674 ; tmpForInput_idx_2 = _rtP -> P_673 ; if
( tmpForInput_idx_0 > tmpForInput_idx_2 ) { _rtB -> B_55_718_0 =
tmpForInput_idx_2 ; } else if ( tmpForInput_idx_0 < tmpForInput_idx_1 ) {
_rtB -> B_55_718_0 = tmpForInput_idx_1 ; } else { _rtB -> B_55_718_0 =
tmpForInput_idx_0 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_55_719_0 = _rtP -> P_675 * _rtB -> B_55_705_0 ; } _rtB ->
B_55_720_0 = ( _rtB -> B_55_719_0 + _rtB -> B_55_37_0 ) - _rtB -> B_55_710_0
; _rtB -> B_55_721_0 = _rtP -> P_676 * _rtB -> B_55_720_0 ; _rtB ->
B_55_722_0 = _rtP -> P_677 * _rtB -> B_55_720_0 ; _rtB -> B_55_723_0 = _rtX
-> Integrator1_CSTATE_c ; _rtB -> B_55_724_0 = _rtB -> B_55_723_0 + _rtB ->
B_55_722_0 ; _rtB -> B_55_725_0 = 0.0 ; _rtB -> B_55_725_0 += _rtP -> P_680 *
_rtX -> TransferFcn1_CSTATE ; _rtB -> B_55_726_0 = _rtX ->
Integrator1_CSTATE_l ; _rtB -> B_55_727_0 = _rtB -> B_55_726_0 - _rtB ->
B_55_710_0 ; _rtB -> B_55_728_0 = muDoubleScalarSqrt ( _rtB -> B_55_727_0 ) ;
_rtB -> B_55_729_0 = _rtP -> P_682 * _rtB -> B_55_728_0 ; _rtB -> B_55_730_0
= ( _rtB -> B_55_725_0 + _rtB -> B_55_38_0_f ) - _rtB -> B_55_729_0 ; _rtB ->
B_55_731_0 = _rtP -> P_683 * _rtB -> B_55_730_0 ; _rtB -> B_55_732_0 = _rtB
-> B_55_704_0 * _rtB -> B_55_710_0 ; _rtB -> B_55_733_0 = _rtB -> B_55_729_0
- _rtB -> B_55_732_0 ; _rtB -> B_55_734_0 = _rtP -> P_684 * _rtB ->
B_55_733_0 ; _rtB -> B_55_735_0 = 0.0 ; _rtB -> B_55_735_0 += _rtP -> P_686 *
_rtX -> TransferFcn_CSTATE ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_aw . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_aw . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_687 ; if (
_rtP -> P_687 == 0.0 ) _rtB -> B_55_736_0 = _rtB -> B_55_724_0 ; else _rtB ->
B_55_736_0 = DZG_v6_SimGen_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_l4 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_l4 . Last , _rtDW -> TransportDelay_IWORK_l4 .
Tail , _rtDW -> TransportDelay_IWORK_l4 . Head , _rtP -> P_688 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_55_737_0 = _rtP -> P_689 * _rtB ->
B_55_711_0 ; _rtB -> B_55_738_0 = _rtB -> B_55_568_0 - _rtB -> B_55_570_0 ;
_rtB -> B_55_739_0 = _rtP -> P_690 * _rtB -> B_55_738_0 ; _rtB -> B_55_740_0
= _rtB -> B_55_732_0 - _rtB -> B_55_568_0 ; _rtB -> B_55_741_0 = _rtP ->
P_691 * _rtB -> B_55_740_0 ; _rtB -> B_55_742_0 = _rtB -> B_55_570_0 - _rtB
-> B_55_572_0 ; _rtB -> B_55_743_0 = _rtP -> P_692 * _rtB -> B_55_742_0 ;
isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_744_0
= _rtP -> P_693 ; } tmpForInput_idx_0 = ssGetTaskTime ( S , 0 ) ; if (
tmpForInput_idx_0 < _rtP -> P_694 ) { _rtB -> B_55_745_0 = _rtP -> P_695 ; }
else { _rtB -> B_55_745_0 = _rtP -> P_696 ; } if ( _rtP -> P_811 == 1 ) {
_rtB -> B_55_746_0 = _rtB -> B_55_745_0 ; } else { _rtB -> B_55_746_0 = _rtB
-> B_55_52_0_c ; } _rtB -> B_55_747_0 = ( _rtB -> B_55_746_0 != 0.0 ) ; isHit
= ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_748_0 =
ssGetTaskTime ( S , 2 ) ; _rtB -> B_55_749_0 = rt_Lookup ( _rtP -> P_697 , 4
, _rtB -> B_55_748_0 , _rtP -> P_698 ) ; } if ( _rtB -> B_55_42_0_j >= _rtP
-> P_699 ) { _rtB -> B_55_750_0 = _rtB -> B_55_747_0 ; } else { _rtB ->
B_55_750_0 = _rtB -> B_55_749_0 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if
( isHit != 0 ) { _rtB -> B_55_751_0 = ssGetTaskTime ( S , 2 ) ; _rtB ->
B_55_752_0 = rt_Lookup ( _rtP -> P_700 , 5 , _rtB -> B_55_751_0 , _rtP ->
P_701 ) ; } if ( _rtB -> B_55_744_0 >= _rtP -> P_702 ) { if ( _rtB ->
B_55_43_0_j >= _rtP -> P_36 ) { _rtB -> B_36_0_0 = _rtB -> B_55_750_0 ; }
else { _rtB -> B_36_0_0 = _rtB -> B_55_46_0_n ; } _rtB -> B_36_1_0 = _rtB ->
B_36_0_0 ; _rtB -> B_55_754_0 = _rtB -> B_36_1_0 ; } else { _rtB ->
B_55_754_0 = _rtB -> B_55_752_0 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if
( isHit != 0 ) { _rtB -> B_55_758_0 = _rtP -> P_703 ; _rtB -> B_55_759_0 =
ssGetTaskTime ( S , 2 ) ; _rtB -> B_55_760_0 = rt_Lookup ( _rtP -> P_704 , 5
, _rtB -> B_55_759_0 , _rtP -> P_705 ) ; } if ( _rtB -> B_55_758_0 >= _rtP ->
P_706 ) { if ( _rtB -> B_55_44_0_o >= _rtP -> P_37 ) { _rtB -> B_37_0_0 =
_rtB -> B_55_750_0 ; } else { _rtB -> B_37_0_0 = _rtB -> B_55_46_0_n ; } _rtB
-> B_37_1_0 = _rtB -> B_37_0_0 ; _rtB -> B_55_762_0 = _rtB -> B_37_1_0 ; }
else { _rtB -> B_55_762_0 = _rtB -> B_55_760_0 ; } isHit = ssIsSampleHit ( S
, 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_766_0 = _rtP -> P_707 ; _rtB ->
B_55_767_0 = ssGetTaskTime ( S , 2 ) ; _rtB -> B_55_768_0 = rt_Lookup ( _rtP
-> P_708 , 5 , _rtB -> B_55_767_0 , _rtP -> P_709 ) ; } if ( _rtB ->
B_55_766_0 >= _rtP -> P_710 ) { if ( _rtB -> B_55_45_0 >= _rtP -> P_38 ) {
_rtB -> B_38_0_0 = _rtB -> B_55_750_0 ; } else { _rtB -> B_38_0_0 = _rtB ->
B_55_46_0_n ; } _rtB -> B_38_1_0 = _rtB -> B_38_0_0 ; _rtB -> B_55_770_0 =
_rtB -> B_38_1_0 ; } else { _rtB -> B_55_770_0 = _rtB -> B_55_768_0 ; } isHit
= ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_781_0 = _rtP
-> P_711 ; _rtB -> B_55_782_0 = ssGetTaskTime ( S , 2 ) ; _rtB -> B_55_783_0
= rt_Lookup ( _rtP -> P_712 , 4 , _rtB -> B_55_782_0 , _rtP -> P_713 ) ; } if
( _rtB -> B_55_47_0 >= _rtP -> P_714 ) { _rtB -> B_39_0_0 = ! _rtB ->
B_55_747_0 ; _rtB -> B_55_785_0 = _rtB -> B_39_0_0 ; } else { _rtB ->
B_55_785_0 = _rtB -> B_55_783_0 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if
( isHit != 0 ) { _rtB -> B_55_786_0 = ssGetTaskTime ( S , 2 ) ; _rtB ->
B_55_787_0 = rt_Lookup ( _rtP -> P_715 , 5 , _rtB -> B_55_786_0 , _rtP ->
P_716 ) ; } if ( _rtB -> B_55_781_0 >= _rtP -> P_717 ) { if ( _rtB ->
B_55_48_0_i >= _rtP -> P_39 ) { _rtB -> B_40_0_0 = _rtB -> B_55_785_0 ; }
else { _rtB -> B_40_0_0 = _rtB -> B_55_51_0_m ; } _rtB -> B_40_1_0 = _rtB ->
B_40_0_0 ; _rtB -> B_55_789_0 = _rtB -> B_40_1_0 ; } else { _rtB ->
B_55_789_0 = _rtB -> B_55_787_0 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if
( isHit != 0 ) { _rtB -> B_55_793_0 = _rtP -> P_718 ; _rtB -> B_55_794_0 =
ssGetTaskTime ( S , 2 ) ; _rtB -> B_55_795_0 = rt_Lookup ( _rtP -> P_719 , 5
, _rtB -> B_55_794_0 , _rtP -> P_720 ) ; } if ( _rtB -> B_55_793_0 >= _rtP ->
P_721 ) { if ( _rtB -> B_55_49_0_o >= _rtP -> P_40 ) { _rtB -> B_41_0_0 =
_rtB -> B_55_785_0 ; } else { _rtB -> B_41_0_0 = _rtB -> B_55_51_0_m ; } _rtB
-> B_41_1_0 = _rtB -> B_41_0_0 ; _rtB -> B_55_797_0 = _rtB -> B_41_1_0 ; }
else { _rtB -> B_55_797_0 = _rtB -> B_55_795_0 ; } isHit = ssIsSampleHit ( S
, 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_55_801_0 = _rtP -> P_722 ; _rtB ->
B_55_802_0 = ssGetTaskTime ( S , 2 ) ; _rtB -> B_55_803_0 = rt_Lookup ( _rtP
-> P_723 , 5 , _rtB -> B_55_802_0 , _rtP -> P_724 ) ; } if ( _rtB ->
B_55_801_0 >= _rtP -> P_725 ) { if ( _rtB -> B_55_50_0_n >= _rtP -> P_41 ) {
_rtB -> B_42_0_0 = _rtB -> B_55_785_0 ; } else { _rtB -> B_42_0_0 = _rtB ->
B_55_51_0_m ; } _rtB -> B_42_1_0 = _rtB -> B_42_0_0 ; _rtB -> B_55_805_0 =
_rtB -> B_42_1_0 ; } else { _rtB -> B_55_805_0 = _rtB -> B_55_803_0 ; } _rtB
-> B_55_816_0 = ssGetT ( S ) ; _rtB -> B_55_817_0 = ( _rtB -> B_55_816_0 >=
_rtB -> B_55_54_0_m ) ; _rtB -> B_55_818_0 = _rtB -> B_55_817_0 ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S
) != 0 ) ) { if ( _rtB -> B_55_818_0 ) { if ( ! _rtDW -> Registrator_MODE ) {
if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } _rtDW ->
Registrator_MODE = true ; } } else { if ( _rtDW -> Registrator_MODE ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; _rtDW -> Registrator_MODE
= false ; } } } if ( _rtDW -> Registrator_MODE ) { isHit = ssIsSampleHit ( S
, 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_43_0_0 = _rtP -> P_42 * _rtB ->
B_55_531_0 [ 0 ] ; _rtB -> B_43_1_0 = _rtP -> P_43 * _rtB -> B_55_531_0 [ 1 ]
; _rtB -> B_43_2_0 = _rtP -> P_44 * _rtB -> B_55_531_0 [ 2 ] ; _rtB ->
B_43_3_0 = _rtP -> P_45 * _rtB -> B_55_535_0 [ 0 ] ; _rtB -> B_43_4_0 = _rtP
-> P_46 * _rtB -> B_55_535_0 [ 1 ] ; _rtB -> B_43_5_0 = _rtP -> P_47 * _rtB
-> B_55_535_0 [ 2 ] ; } isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0
) { _rtB -> B_43_6_0 [ 0 ] = _rtB -> B_55_816_0 ; _rtB -> B_43_6_0 [ 1 ] =
_rtB -> B_43_0_0 ; _rtB -> B_43_6_0 [ 2 ] = _rtB -> B_43_1_0 ; _rtB ->
B_43_6_0 [ 3 ] = _rtB -> B_43_2_0 ; _rtB -> B_43_6_0 [ 4 ] = _rtB -> B_43_3_0
; _rtB -> B_43_6_0 [ 5 ] = _rtB -> B_43_4_0 ; _rtB -> B_43_6_0 [ 6 ] = _rtB
-> B_43_5_0 ; _rtB -> B_43_6_0 [ 7 ] = _rtB -> B_43_2_0_c ; _rtB -> B_43_6_0
[ 8 ] = _rtB -> B_43_4_0_l ; _rtB -> B_43_6_0 [ 9 ] = _rtB -> B_43_3_0_o ;
_rtB -> B_43_6_0 [ 10 ] = _rtB -> B_43_1_0_g ; _rtB -> B_43_6_0 [ 11 ] = _rtB
-> B_43_0_0_f ; ssCallAccelRunBlock ( S , 43 , 7 , SS_CALL_MDL_OUTPUTS ) ; }
if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
Registrator_SubsysRanBC ) ; } } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_55_894_0 = _rtP -> P_726 ; _rtB -> B_55_895_0 =
ssGetTaskTime ( S , 2 ) ; _rtB -> B_55_896_0 = rt_Lookup ( _rtP -> P_727 , 6
, _rtB -> B_55_895_0 , _rtP -> P_728 ) ; if ( _rtB -> B_55_98_0 >= _rtP ->
P_729 ) { _rtB -> B_55_897_0 = _rtB -> B_55_105_0_a ; } else { _rtB ->
B_55_897_0 = _rtB -> B_55_896_0 ; } _rtB -> B_55_898_0 = ( _rtB -> B_55_897_0
!= 0.0 ) ; _rtB -> B_55_899_0 = ssGetTaskTime ( S , 2 ) ; if ( _rtB ->
B_55_894_0 >= _rtP -> P_730 ) { _rtB -> B_31_0_0 = ( _rtB -> B_55_898_0 &&
_rtB -> B_55_104_0_o ) ; _rtB -> B_31_1_0 = _rtB -> B_31_0_0 ; _rtB ->
B_31_2_0 = _rtB -> B_31_1_0 ; _rtB -> B_55_902_0 = _rtB -> B_31_2_0 ; } else
{ _rtB -> B_30_0_0 = rt_Lookup ( _rtP -> P_30 , 5 , _rtB -> B_55_899_0 , _rtP
-> P_31 ) ; _rtB -> B_55_902_0 = _rtB -> B_30_0_0 ; } _rtB -> B_55_906_0 =
_rtP -> P_731 ; _rtB -> B_55_907_0 = ssGetTaskTime ( S , 2 ) ; if ( _rtB ->
B_55_906_0 >= _rtP -> P_732 ) { _rtB -> B_33_0_0 = ( _rtB -> B_55_898_0 &&
_rtB -> B_55_103_0_e ) ; _rtB -> B_33_1_0 = _rtB -> B_33_0_0 ; _rtB ->
B_33_2_0 = _rtB -> B_33_1_0 ; _rtB -> B_55_910_0 = _rtB -> B_33_2_0 ; } else
{ _rtB -> B_32_0_0 = rt_Lookup ( _rtP -> P_32 , 5 , _rtB -> B_55_907_0 , _rtP
-> P_33 ) ; _rtB -> B_55_910_0 = _rtB -> B_32_0_0 ; } _rtB -> B_55_914_0 =
_rtP -> P_733 ; _rtB -> B_55_915_0 = ssGetTaskTime ( S , 2 ) ; if ( _rtB ->
B_55_914_0 >= _rtP -> P_734 ) { _rtB -> B_35_0_0 = ( _rtB -> B_55_898_0 &&
_rtB -> B_55_102_0_p ) ; _rtB -> B_35_1_0 = _rtB -> B_35_0_0 ; _rtB ->
B_35_2_0 = _rtB -> B_35_1_0 ; _rtB -> B_55_918_0 = _rtB -> B_35_2_0 ; } else
{ _rtB -> B_34_0_0 = rt_Lookup ( _rtP -> P_34 , 5 , _rtB -> B_55_915_0 , _rtP
-> P_35 ) ; _rtB -> B_55_918_0 = _rtB -> B_34_0_0 ; } _rtB -> B_55_1132_0 =
_rtP -> P_735 * _rtB -> B_55_32_0 [ 53 ] ; _rtB -> B_55_1133_0 = _rtP ->
P_736 * _rtB -> B_55_32_0 [ 54 ] ; _rtB -> B_55_1134_0 = _rtP -> P_737 * _rtB
-> B_55_32_0 [ 55 ] ; _rtB -> B_55_1135_0 [ 0 ] = _rtP -> P_738 * _rtB ->
B_55_1132_0 ; _rtB -> B_55_1135_0 [ 1 ] = _rtP -> P_738 * _rtB -> B_55_1133_0
; _rtB -> B_55_1135_0 [ 2 ] = _rtP -> P_738 * _rtB -> B_55_1134_0 ; _rtB ->
B_55_1136_0 = _rtP -> P_739 * _rtB -> B_55_32_0 [ 29 ] ; _rtB -> B_55_1137_0
= _rtP -> P_740 * _rtB -> B_55_32_0 [ 30 ] ; _rtB -> B_55_1138_0 = _rtP ->
P_741 * _rtB -> B_55_32_0 [ 31 ] ; _rtB -> B_55_1139_0 [ 0 ] = _rtP -> P_742
* _rtB -> B_55_1136_0 ; _rtB -> B_55_1139_0 [ 1 ] = _rtP -> P_742 * _rtB ->
B_55_1137_0 ; _rtB -> B_55_1139_0 [ 2 ] = _rtP -> P_742 * _rtB -> B_55_1138_0
; } UNUSED_PARAMETER ( tid ) ; } static void mdlOutputsTID4 ( SimStruct * S ,
int_T tid ) { B_DZG_v6_SimGen_T * _rtB ; P_DZG_v6_SimGen_T * _rtP ;
DW_DZG_v6_SimGen_T * _rtDW ; _rtDW = ( ( DW_DZG_v6_SimGen_T * )
ssGetRootDWork ( S ) ) ; _rtP = ( ( P_DZG_v6_SimGen_T * ) ssGetModelRtp ( S )
) ; _rtB = ( ( B_DZG_v6_SimGen_T * ) _ssGetModelBlockIO ( S ) ) ; memcpy ( &
_rtB -> B_55_0_0_m [ 0 ] , & _rtP -> P_743 [ 0 ] , 9U * sizeof ( real_T ) ) ;
_rtB -> B_55_1_0_c = _rtP -> P_744 ; _rtB -> B_55_2_0_k = _rtP -> P_745 ;
_rtB -> B_55_3_0_c = _rtP -> P_746 ; _rtB -> B_55_4_0_b = _rtP -> P_747 ;
_rtB -> B_55_5_0_p = _rtP -> P_748 ; _rtB -> B_55_6_0_c = _rtP -> P_749 ;
_rtB -> B_55_7_0_f = _rtP -> P_750 ; _rtB -> B_55_8_0_g = _rtP -> P_751 ;
_rtB -> B_55_9_0_c = _rtP -> P_809 ; _rtB -> B_55_10_0 = _rtP -> P_752 ; _rtB
-> B_55_11_0_g = _rtP -> P_753 ; _rtB -> B_55_12_0_m = _rtP -> P_754 ; _rtB
-> B_55_13_0_n = _rtP -> P_755 ; _rtB -> B_55_15_0_l = _rtP -> P_757 ; _rtB
-> B_55_17_0_d = _rtP -> P_759 ; _rtB -> B_55_18_0_g [ 0 ] = _rtP -> P_760 [
0 ] ; _rtB -> B_55_19_0_l [ 0 ] = _rtP -> P_761 [ 0 ] ; _rtB -> B_55_18_0_g [
1 ] = _rtP -> P_760 [ 1 ] ; _rtB -> B_55_19_0_l [ 1 ] = _rtP -> P_761 [ 1 ] ;
_rtB -> B_47_0_0_h = _rtP -> P_806 ; _rtB -> B_47_1_0_l = _rtP -> P_807 ;
_rtB -> B_47_2_0_h = _rtP -> P_808 ; _rtB -> B_47_3_0_a = _rtP -> P_71 ; _rtB
-> B_47_4_0_i [ 0 ] = _rtP -> P_72 [ 0 ] ; _rtB -> B_47_4_0_i [ 1 ] = _rtP ->
P_72 [ 1 ] ; _rtB -> B_46_0_0_f = _rtP -> P_60 ; _rtB -> B_46_1_0_i [ 0 ] =
_rtP -> P_61 [ 0 ] ; _rtB -> B_46_1_0_i [ 1 ] = _rtP -> P_61 [ 1 ] ; _rtB ->
B_46_1_0_i [ 2 ] = _rtP -> P_61 [ 2 ] ; _rtB -> B_47_9_0_i = _rtP -> P_76 ;
_rtB -> B_47_10_0 = _rtP -> P_77 ; memcpy ( & _rtB -> B_55_14_0_p [ 0 ] , &
_rtP -> P_756 [ 0 ] , 36U * sizeof ( real_T ) ) ; memcpy ( & _rtB ->
B_55_16_0_j [ 0 ] , & _rtP -> P_758 [ 0 ] , 36U * sizeof ( real_T ) ) ;
memcpy ( & _rtB -> B_55_20_0_d [ 0 ] , & _rtP -> P_762 [ 0 ] , 36U * sizeof (
real_T ) ) ; memcpy ( & _rtB -> B_47_6_0_l [ 0 ] , & _rtP -> P_73 [ 0 ] , 36U
* sizeof ( real_T ) ) ; memcpy ( & _rtB -> B_47_7_0_o [ 0 ] , & _rtP -> P_74
[ 0 ] , 36U * sizeof ( real_T ) ) ; memcpy ( & _rtB -> B_47_8_0_o [ 0 ] , &
_rtP -> P_75 [ 0 ] , 36U * sizeof ( real_T ) ) ; memcpy ( & _rtB -> B_55_22_0
[ 0 ] , & _rtP -> P_763 [ 0 ] , 36U * sizeof ( real_T ) ) ; _rtB ->
B_55_23_0_d [ 0 ] = _rtP -> P_764 [ 0 ] ; _rtB -> B_55_23_0_d [ 1 ] = _rtP ->
P_764 [ 1 ] ; _rtB -> B_55_23_0_d [ 2 ] = _rtP -> P_764 [ 2 ] ; _rtB ->
B_55_24_0_l = _rtP -> P_765 ; _rtB -> B_55_25_0_o = _rtP -> P_766 ; _rtB ->
B_55_26_0_b = _rtP -> P_767 ; _rtB -> B_55_27_0_n = _rtP -> P_768 ; _rtB ->
B_55_28_0_b = _rtP -> P_769 ; _rtB -> B_55_29_0_l = _rtP -> P_770 ; _rtB ->
B_55_30_0_h = _rtP -> P_771 ; _rtB -> B_55_31_0_b = _rtP -> P_772 ; _rtB ->
B_55_32_0_d = _rtP -> P_773 ; _rtB -> B_55_33_0_f = ( _rtB -> B_55_30_0_h ==
_rtB -> B_55_32_0_d ) ; _rtB -> B_55_34_0_e = _rtB -> B_55_33_0_f ; _rtB ->
B_55_35_0_b = _rtP -> P_774 * _rtB -> B_55_34_0_e ; _rtB -> B_55_36_0_j =
_rtP -> P_775 ; _rtB -> B_55_37_0 = _rtP -> P_776 ; _rtB -> B_55_38_0_f =
_rtP -> P_777 ; _rtB -> B_55_39_0 = _rtP -> P_778 ; _rtB -> B_55_40_0_a =
_rtP -> P_779 ; ssCallAccelRunBlock ( S , 55 , 1266 , SS_CALL_MDL_OUTPUTS ) ;
_rtB -> B_55_42_0_j = _rtP -> P_780 ; _rtB -> B_55_43_0_j = _rtP -> P_781 ;
_rtB -> B_55_44_0_o = _rtP -> P_782 ; _rtB -> B_55_45_0 = _rtP -> P_783 ;
_rtB -> B_55_46_0_n = _rtP -> P_784 ; _rtB -> B_55_47_0 = _rtP -> P_785 ;
_rtB -> B_55_48_0_i = _rtP -> P_786 ; _rtB -> B_55_49_0_o = _rtP -> P_787 ;
_rtB -> B_55_50_0_n = _rtP -> P_788 ; _rtB -> B_55_51_0_m = _rtP -> P_789 ;
_rtB -> B_55_52_0_c = _rtP -> P_790 ; _rtB -> B_43_0_0_f = _rtP -> P_48 * 0.0
; _rtB -> B_43_1_0_g = _rtP -> P_49 * 0.0 ; _rtB -> B_43_2_0_c = _rtP -> P_50
* 0.0 ; _rtB -> B_43_3_0_o = _rtP -> P_51 * 0.0 ; _rtB -> B_43_4_0_l = _rtP
-> P_52 * 0.0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
Registrator_SubsysRanBC ) ; } _rtB -> B_55_54_0_m = _rtP -> P_791 ; _rtB ->
B_55_55_0_m = _rtP -> P_792 ; ssCallAccelRunBlock ( S , 55 , 1280 ,
SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_55_57_0_j = _rtP -> P_793 ;
ssCallAccelRunBlock ( S , 55 , 1282 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1283 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1284 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1285 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1286 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1287 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1288 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1289 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1290 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1291 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1292 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1293 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1294 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1295 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1296 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1297 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1298 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1299 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1300 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1301 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1302 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1303 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1304 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1305 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1306 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1307 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1308 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1309 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1310 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 55 , 1311 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_55_88_0_h = _rtP -> P_794 ; _rtB -> B_55_89_0_c = _rtP -> P_795 ; _rtB ->
B_55_90_0_c [ 0 ] = _rtP -> P_796 [ 0 ] ; _rtB -> B_55_90_0_c [ 1 ] = _rtP ->
P_796 [ 1 ] ; _rtB -> B_55_90_0_c [ 2 ] = _rtP -> P_796 [ 2 ] ; _rtB ->
B_55_91_0_p = _rtP -> P_797 ; _rtB -> B_55_92_0 = _rtP -> P_798 ; _rtB ->
B_55_93_0 [ 0 ] = _rtB -> B_55_91_0_p ; _rtB -> B_55_93_0 [ 1 ] = _rtB ->
B_55_92_0 ; _rtB -> B_55_93_0 [ 2 ] = _rtB -> B_55_92_0 ; _rtB -> B_55_93_0 [
3 ] = _rtB -> B_55_91_0_p ; _rtB -> B_55_94_0 = _rtP -> P_799 ; _rtB ->
B_55_95_0 [ 0 ] = _rtB -> B_55_91_0_p ; _rtB -> B_55_95_0 [ 1 ] = _rtB ->
B_55_94_0 ; _rtB -> B_55_95_0 [ 2 ] = _rtB -> B_55_94_0 ; _rtB -> B_55_95_0 [
3 ] = _rtB -> B_55_91_0_p ; _rtB -> B_55_96_0 = _rtP -> P_800 ; _rtB ->
B_55_97_0 [ 0 ] = _rtB -> B_55_91_0_p ; _rtB -> B_55_97_0 [ 1 ] = _rtB ->
B_55_96_0 ; _rtB -> B_55_97_0 [ 2 ] = _rtB -> B_55_96_0 ; _rtB -> B_55_97_0 [
3 ] = _rtB -> B_55_91_0_p ; _rtB -> B_55_98_0 = _rtP -> P_801 ; _rtB ->
B_55_99_0_p = _rtP -> P_802 ; _rtB -> B_55_100_0_a = _rtP -> P_803 ; _rtB ->
B_55_101_0_e = _rtP -> P_804 ; _rtB -> B_55_102_0_p = ( _rtB -> B_55_101_0_e
!= 0.0 ) ; _rtB -> B_55_103_0_e = ( _rtB -> B_55_100_0_a != 0.0 ) ; _rtB ->
B_55_104_0_o = ( _rtB -> B_55_99_0_p != 0.0 ) ; _rtB -> B_55_105_0_a = _rtP
-> P_805 ; UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdate ( SimStruct * S , int_T tid ) { real_T HoldSine ;
real_T HoldCosine ; int32_T i ; B_DZG_v6_SimGen_T * _rtB ; P_DZG_v6_SimGen_T
* _rtP ; DW_DZG_v6_SimGen_T * _rtDW ; _rtDW = ( ( DW_DZG_v6_SimGen_T * )
ssGetRootDWork ( S ) ) ; _rtP = ( ( P_DZG_v6_SimGen_T * ) ssGetModelRtp ( S )
) ; _rtB = ( ( B_DZG_v6_SimGen_T * ) _ssGetModelBlockIO ( S ) ) ; i =
ssIsSampleHit ( S , 2 , 0 ) ; if ( i != 0 ) { _rtDW -> UnitDelay_DSTATE [ 0 ]
= _rtB -> B_55_93_0 [ 0 ] ; _rtDW -> UnitDelay_DSTATE [ 1 ] = _rtB ->
B_55_93_0 [ 1 ] ; _rtDW -> UnitDelay_DSTATE [ 2 ] = _rtB -> B_55_93_0 [ 2 ] ;
_rtDW -> UnitDelay_DSTATE [ 3 ] = _rtB -> B_55_93_0 [ 3 ] ; _rtDW ->
UnitDelay_DSTATE [ 4 ] = _rtB -> B_55_95_0 [ 0 ] ; _rtDW -> UnitDelay_DSTATE
[ 5 ] = _rtB -> B_55_95_0 [ 1 ] ; _rtDW -> UnitDelay_DSTATE [ 6 ] = _rtB ->
B_55_95_0 [ 2 ] ; _rtDW -> UnitDelay_DSTATE [ 7 ] = _rtB -> B_55_95_0 [ 3 ] ;
_rtDW -> UnitDelay_DSTATE [ 8 ] = _rtB -> B_55_97_0 [ 0 ] ; _rtDW ->
UnitDelay_DSTATE [ 9 ] = _rtB -> B_55_97_0 [ 1 ] ; _rtDW -> UnitDelay_DSTATE
[ 10 ] = _rtB -> B_55_97_0 [ 2 ] ; _rtDW -> UnitDelay_DSTATE [ 11 ] = _rtB ->
B_55_97_0 [ 3 ] ; ssCallAccelRunBlock ( S , 55 , 2 , SS_CALL_MDL_UPDATE ) ;
_rtDW -> Rotorangledthetae_DSTATE += _rtP -> P_121 * _rtB -> B_55_627_0 ; for
( i = 0 ; i < 6 ; i ++ ) { _rtDW -> fluxes_DSTATE [ i ] = _rtB -> B_55_619_0
[ i ] ; } if ( _rtB -> B_55_9_0 > 0.0 ) { _rtDW -> Lmd_sat_DSTATE = _rtB ->
B_47_12_0 ; if ( _rtB -> B_47_0_0 ) { _rtDW -> Lmq_sat_DSTATE = _rtB ->
B_46_11_0 ; } } HoldSine = _rtDW -> lastSin ; HoldCosine = _rtDW -> lastCos ;
_rtDW -> lastSin = HoldSine * _rtP -> P_142 + HoldCosine * _rtP -> P_141 ;
_rtDW -> lastCos = HoldCosine * _rtP -> P_142 - HoldSine * _rtP -> P_141 ;
HoldSine = _rtDW -> lastSin_m ; HoldCosine = _rtDW -> lastCos_f ; _rtDW ->
lastSin_m = HoldSine * _rtP -> P_149 + HoldCosine * _rtP -> P_148 ; _rtDW ->
lastCos_f = HoldCosine * _rtP -> P_149 - HoldSine * _rtP -> P_148 ; HoldSine
= _rtDW -> lastSin_b ; HoldCosine = _rtDW -> lastCos_d ; _rtDW -> lastSin_b =
HoldSine * _rtP -> P_156 + HoldCosine * _rtP -> P_155 ; _rtDW -> lastCos_d =
HoldCosine * _rtP -> P_156 - HoldSine * _rtP -> P_155 ; ssCallAccelRunBlock (
S , 55 , 32 , SS_CALL_MDL_UPDATE ) ; } { real_T * * uBuffer = ( real_T * * )
& _rtDW -> TransportDelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK . Head = ( ( _rtDW ->
TransportDelay_IWORK . Head < ( _rtDW -> TransportDelay_IWORK .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK . Head + 1 ) : 0 )
; if ( _rtDW -> TransportDelay_IWORK . Head == _rtDW -> TransportDelay_IWORK
. Tail ) { if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK . CircularBufSize , & _rtDW -> TransportDelay_IWORK .
Tail , & _rtDW -> TransportDelay_IWORK . Head , & _rtDW ->
TransportDelay_IWORK . Last , simTime - _rtP -> P_170 , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK . Head ] = _rtB ->
B_55_33_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput = _rtB -> B_55_40_0 ; } { real_T * * uBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_k . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k . TUbufferPtrs [ 1
] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_c . Head =
( ( _rtDW -> TransportDelay_IWORK_c . Head < ( _rtDW ->
TransportDelay_IWORK_c . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_c . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_c . Head == _rtDW -> TransportDelay_IWORK_c . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_c . CircularBufSize , & _rtDW -> TransportDelay_IWORK_c
. Tail , & _rtDW -> TransportDelay_IWORK_c . Head , & _rtDW ->
TransportDelay_IWORK_c . Last , simTime - _rtP -> P_175 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_c .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_c . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_c . Head ] = _rtB ->
B_55_41_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_p = _rtB -> B_55_48_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_f . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_f . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_e .
Head = ( ( _rtDW -> TransportDelay_IWORK_e . Head < ( _rtDW ->
TransportDelay_IWORK_e . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_e . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_e . Head == _rtDW -> TransportDelay_IWORK_e . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_e . CircularBufSize , & _rtDW -> TransportDelay_IWORK_e
. Tail , & _rtDW -> TransportDelay_IWORK_e . Head , & _rtDW ->
TransportDelay_IWORK_e . Last , simTime - _rtP -> P_181 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_e .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_e . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_e . Head ] = _rtB ->
B_55_52_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_l = _rtB -> B_55_59_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_d . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_d . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_j .
Head = ( ( _rtDW -> TransportDelay_IWORK_j . Head < ( _rtDW ->
TransportDelay_IWORK_j . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_j . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_j . Head == _rtDW -> TransportDelay_IWORK_j . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_j . CircularBufSize , & _rtDW -> TransportDelay_IWORK_j
. Tail , & _rtDW -> TransportDelay_IWORK_j . Head , & _rtDW ->
TransportDelay_IWORK_j . Last , simTime - _rtP -> P_186 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_j .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_j . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_j . Head ] = _rtB ->
B_55_60_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_po = _rtB -> B_55_67_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_a . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_a . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_eq .
Head = ( ( _rtDW -> TransportDelay_IWORK_eq . Head < ( _rtDW ->
TransportDelay_IWORK_eq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_eq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_eq . Head == _rtDW -> TransportDelay_IWORK_eq . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_eq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_eq . Tail , & _rtDW -> TransportDelay_IWORK_eq . Head ,
& _rtDW -> TransportDelay_IWORK_eq . Last , simTime - _rtP -> P_221 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_eq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_eq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_eq . Head ] = _rtB -> B_55_107_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_n = _rtB ->
B_55_114_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_l . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_l . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_e5 . Head = ( ( _rtDW ->
TransportDelay_IWORK_e5 . Head < ( _rtDW -> TransportDelay_IWORK_e5 .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_e5 . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_e5 . Head == _rtDW ->
TransportDelay_IWORK_e5 . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_e5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_e5 . Tail , & _rtDW -> TransportDelay_IWORK_e5 . Head ,
& _rtDW -> TransportDelay_IWORK_e5 . Last , simTime - _rtP -> P_226 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_e5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_e5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_e5 . Head ] = _rtB -> B_55_115_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_j = _rtB ->
B_55_122_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fp . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_fp . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_g . Head = ( ( _rtDW
-> TransportDelay_IWORK_g . Head < ( _rtDW -> TransportDelay_IWORK_g .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_g . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_g . Head == _rtDW ->
TransportDelay_IWORK_g . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_g . CircularBufSize , & _rtDW -> TransportDelay_IWORK_g
. Tail , & _rtDW -> TransportDelay_IWORK_g . Head , & _rtDW ->
TransportDelay_IWORK_g . Last , simTime - _rtP -> P_232 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_g .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_g . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_g . Head ] = _rtB ->
B_55_126_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_g = _rtB -> B_55_133_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_g . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_g . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_m .
Head = ( ( _rtDW -> TransportDelay_IWORK_m . Head < ( _rtDW ->
TransportDelay_IWORK_m . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_m . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_m . Head == _rtDW -> TransportDelay_IWORK_m . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_m . CircularBufSize , & _rtDW -> TransportDelay_IWORK_m
. Tail , & _rtDW -> TransportDelay_IWORK_m . Head , & _rtDW ->
TransportDelay_IWORK_m . Last , simTime - _rtP -> P_237 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_m .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_m . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_m . Head ] = _rtB ->
B_55_134_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_e = _rtB -> B_55_141_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_j . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_j . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_o .
Head = ( ( _rtDW -> TransportDelay_IWORK_o . Head < ( _rtDW ->
TransportDelay_IWORK_o . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_o . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_o . Head == _rtDW -> TransportDelay_IWORK_o . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_o . CircularBufSize , & _rtDW -> TransportDelay_IWORK_o
. Tail , & _rtDW -> TransportDelay_IWORK_o . Head , & _rtDW ->
TransportDelay_IWORK_o . Last , simTime - _rtP -> P_272 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_o .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_o . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_o . Head ] = _rtB ->
B_55_181_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_d = _rtB -> B_55_188_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_lk . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_lk .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_l . Head = ( ( _rtDW -> TransportDelay_IWORK_l . Head <
( _rtDW -> TransportDelay_IWORK_l . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_l . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_l . Head == _rtDW -> TransportDelay_IWORK_l . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_l . CircularBufSize , & _rtDW -> TransportDelay_IWORK_l
. Tail , & _rtDW -> TransportDelay_IWORK_l . Head , & _rtDW ->
TransportDelay_IWORK_l . Last , simTime - _rtP -> P_277 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_l .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_l . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_l . Head ] = _rtB ->
B_55_189_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_c = _rtB -> B_55_196_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_e . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_cm .
Head = ( ( _rtDW -> TransportDelay_IWORK_cm . Head < ( _rtDW ->
TransportDelay_IWORK_cm . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_cm . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_cm . Head == _rtDW -> TransportDelay_IWORK_cm . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_cm . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_cm . Tail , & _rtDW -> TransportDelay_IWORK_cm . Head ,
& _rtDW -> TransportDelay_IWORK_cm . Last , simTime - _rtP -> P_283 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_cm . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_cm . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_cm . Head ] = _rtB -> B_55_200_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_h = _rtB ->
B_55_207_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_k2 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_k2 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_p . Head = ( ( _rtDW
-> TransportDelay_IWORK_p . Head < ( _rtDW -> TransportDelay_IWORK_p .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_p . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_p . Head == _rtDW ->
TransportDelay_IWORK_p . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_p . CircularBufSize , & _rtDW -> TransportDelay_IWORK_p
. Tail , & _rtDW -> TransportDelay_IWORK_p . Head , & _rtDW ->
TransportDelay_IWORK_p . Last , simTime - _rtP -> P_288 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_p .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_p . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_p . Head ] = _rtB ->
B_55_208_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_m = _rtB -> B_55_215_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_i . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_i . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_j1 .
Head = ( ( _rtDW -> TransportDelay_IWORK_j1 . Head < ( _rtDW ->
TransportDelay_IWORK_j1 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_j1 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_j1 . Head == _rtDW -> TransportDelay_IWORK_j1 . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_j1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_j1 . Tail , & _rtDW -> TransportDelay_IWORK_j1 . Head ,
& _rtDW -> TransportDelay_IWORK_j1 . Last , simTime - _rtP -> P_323 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_j1 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_j1 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_j1 . Head ] = _rtB -> B_55_255_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_nd = _rtB ->
B_55_262_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_o . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_o . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_ln . Head = ( ( _rtDW ->
TransportDelay_IWORK_ln . Head < ( _rtDW -> TransportDelay_IWORK_ln .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_ln . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_ln . Head == _rtDW ->
TransportDelay_IWORK_ln . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ln . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ln . Tail , & _rtDW -> TransportDelay_IWORK_ln . Head ,
& _rtDW -> TransportDelay_IWORK_ln . Last , simTime - _rtP -> P_328 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ln . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ln . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ln . Head ] = _rtB -> B_55_263_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_cy = _rtB ->
B_55_270_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_b . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_b . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_f . Head = ( ( _rtDW ->
TransportDelay_IWORK_f . Head < ( _rtDW -> TransportDelay_IWORK_f .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_f . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_f . Head == _rtDW ->
TransportDelay_IWORK_f . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_f . CircularBufSize , & _rtDW -> TransportDelay_IWORK_f
. Tail , & _rtDW -> TransportDelay_IWORK_f . Head , & _rtDW ->
TransportDelay_IWORK_f . Last , simTime - _rtP -> P_334 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_f .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_f . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_f . Head ] = _rtB ->
B_55_274_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_a = _rtB -> B_55_281_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_c . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_c . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_gl .
Head = ( ( _rtDW -> TransportDelay_IWORK_gl . Head < ( _rtDW ->
TransportDelay_IWORK_gl . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_gl . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_gl . Head == _rtDW -> TransportDelay_IWORK_gl . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_gl . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_gl . Tail , & _rtDW -> TransportDelay_IWORK_gl . Head ,
& _rtDW -> TransportDelay_IWORK_gl . Last , simTime - _rtP -> P_339 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_gl . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_gl . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_gl . Head ] = _rtB -> B_55_282_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_c2 = _rtB ->
B_55_289_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fx . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_fx . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_k . Head = ( ( _rtDW
-> TransportDelay_IWORK_k . Head < ( _rtDW -> TransportDelay_IWORK_k .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_k . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_k . Head == _rtDW ->
TransportDelay_IWORK_k . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_k . CircularBufSize , & _rtDW -> TransportDelay_IWORK_k
. Tail , & _rtDW -> TransportDelay_IWORK_k . Head , & _rtDW ->
TransportDelay_IWORK_k . Last , simTime - _rtP -> P_374 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_k .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_k . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_k . Head ] = _rtB ->
B_55_329_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_dh = _rtB -> B_55_336_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_eg . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_eg .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_od . Head = ( ( _rtDW -> TransportDelay_IWORK_od . Head
< ( _rtDW -> TransportDelay_IWORK_od . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_od . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_od . Head == _rtDW -> TransportDelay_IWORK_od . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_od . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_od . Tail , & _rtDW -> TransportDelay_IWORK_od . Head ,
& _rtDW -> TransportDelay_IWORK_od . Last , simTime - _rtP -> P_379 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_od . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_od . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_od . Head ] = _rtB -> B_55_337_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_i = _rtB ->
B_55_344_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_e0 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_e0 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_ed . Head = ( ( _rtDW
-> TransportDelay_IWORK_ed . Head < ( _rtDW -> TransportDelay_IWORK_ed .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_ed . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_ed . Head == _rtDW ->
TransportDelay_IWORK_ed . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ed . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ed . Tail , & _rtDW -> TransportDelay_IWORK_ed . Head ,
& _rtDW -> TransportDelay_IWORK_ed . Last , simTime - _rtP -> P_385 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ed . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ed . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ed . Head ] = _rtB -> B_55_348_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_di = _rtB ->
B_55_355_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_n . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_n . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_h . Head = ( ( _rtDW ->
TransportDelay_IWORK_h . Head < ( _rtDW -> TransportDelay_IWORK_h .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_h . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_h . Head == _rtDW ->
TransportDelay_IWORK_h . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_h . CircularBufSize , & _rtDW -> TransportDelay_IWORK_h
. Tail , & _rtDW -> TransportDelay_IWORK_h . Head , & _rtDW ->
TransportDelay_IWORK_h . Last , simTime - _rtP -> P_390 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_h .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_h . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_h . Head ] = _rtB ->
B_55_356_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_gk = _rtB -> B_55_363_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_h . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_d .
Head = ( ( _rtDW -> TransportDelay_IWORK_d . Head < ( _rtDW ->
TransportDelay_IWORK_d . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_d . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_d . Head == _rtDW -> TransportDelay_IWORK_d . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_d . CircularBufSize , & _rtDW -> TransportDelay_IWORK_d
. Tail , & _rtDW -> TransportDelay_IWORK_d . Head , & _rtDW ->
TransportDelay_IWORK_d . Last , simTime - _rtP -> P_425 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_d .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_d . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_d . Head ] = _rtB ->
B_55_403_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_jh = _rtB -> B_55_410_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_jr . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_jr .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kn . Head = ( ( _rtDW -> TransportDelay_IWORK_kn . Head
< ( _rtDW -> TransportDelay_IWORK_kn . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_kn . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kn . Head == _rtDW -> TransportDelay_IWORK_kn . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kn . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kn . Tail , & _rtDW -> TransportDelay_IWORK_kn . Head ,
& _rtDW -> TransportDelay_IWORK_kn . Last , simTime - _rtP -> P_430 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kn . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kn . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kn . Head ] = _rtB -> B_55_411_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_il = _rtB ->
B_55_418_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_j3 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_j3 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_gj . Head = ( ( _rtDW
-> TransportDelay_IWORK_gj . Head < ( _rtDW -> TransportDelay_IWORK_gj .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_gj . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_gj . Head == _rtDW ->
TransportDelay_IWORK_gj . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_gj . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_gj . Tail , & _rtDW -> TransportDelay_IWORK_gj . Head ,
& _rtDW -> TransportDelay_IWORK_gj . Last , simTime - _rtP -> P_436 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_gj . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_gj . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_gj . Head ] = _rtB -> B_55_422_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_gc = _rtB ->
B_55_429_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_dm . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_dm . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_lu . Head = ( ( _rtDW
-> TransportDelay_IWORK_lu . Head < ( _rtDW -> TransportDelay_IWORK_lu .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_lu . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_lu . Head == _rtDW ->
TransportDelay_IWORK_lu . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lu . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lu . Tail , & _rtDW -> TransportDelay_IWORK_lu . Head ,
& _rtDW -> TransportDelay_IWORK_lu . Last , simTime - _rtP -> P_441 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lu . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lu . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lu . Head ] = _rtB -> B_55_430_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_o = _rtB ->
B_55_437_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_lg . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_lg . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_n . Head = ( ( _rtDW
-> TransportDelay_IWORK_n . Head < ( _rtDW -> TransportDelay_IWORK_n .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_n . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_n . Head == _rtDW ->
TransportDelay_IWORK_n . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_n . CircularBufSize , & _rtDW -> TransportDelay_IWORK_n
. Tail , & _rtDW -> TransportDelay_IWORK_n . Head , & _rtDW ->
TransportDelay_IWORK_n . Last , simTime - _rtP -> P_476 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_n .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_n . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_n . Head ] = _rtB ->
B_55_477_0 ; } i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtDW ->
Memory_PreviousInput_ct = _rtB -> B_55_484_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_dw . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dw .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dz . Head = ( ( _rtDW -> TransportDelay_IWORK_dz . Head
< ( _rtDW -> TransportDelay_IWORK_dz . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dz . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dz . Head == _rtDW -> TransportDelay_IWORK_dz . Tail ) {
if ( ! DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dz . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dz . Tail , & _rtDW -> TransportDelay_IWORK_dz . Head ,
& _rtDW -> TransportDelay_IWORK_dz . Last , simTime - _rtP -> P_481 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dz . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dz . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dz . Head ] = _rtB -> B_55_485_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_ek = _rtB ->
B_55_492_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_m . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_m . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_er . Head = ( ( _rtDW ->
TransportDelay_IWORK_er . Head < ( _rtDW -> TransportDelay_IWORK_er .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_er . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_er . Head == _rtDW ->
TransportDelay_IWORK_er . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_er . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_er . Tail , & _rtDW -> TransportDelay_IWORK_er . Head ,
& _rtDW -> TransportDelay_IWORK_er . Last , simTime - _rtP -> P_487 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_er . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_er . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_er . Head ] = _rtB -> B_55_496_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_mp = _rtB ->
B_55_503_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_el . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_el . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_eb . Head = ( ( _rtDW
-> TransportDelay_IWORK_eb . Head < ( _rtDW -> TransportDelay_IWORK_eb .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_eb . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_eb . Head == _rtDW ->
TransportDelay_IWORK_eb . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_eb . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_eb . Tail , & _rtDW -> TransportDelay_IWORK_eb . Head ,
& _rtDW -> TransportDelay_IWORK_eb . Last , simTime - _rtP -> P_492 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_eb . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_eb . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_eb . Head ] = _rtB -> B_55_504_0 ; } i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> Memory_PreviousInput_k = _rtB ->
B_55_511_0 ; } i = ssIsSampleHit ( S , 2 , 0 ) ; if ( i != 0 ) { _rtDW ->
dw_delay_DSTATE = _rtB -> B_55_626_0 ; _rtDW -> dw_predict_DSTATE = _rtB ->
B_55_551_0 ; _rtDW -> theta_DSTATE += _rtP -> P_532 * _rtB -> B_55_628_0 ;
_rtDW -> UnitDelay2_DSTATE = _rtB -> B_55_700_0 ; _rtDW -> voltages_DSTATE [
0 ] = _rtB -> B_55_589_0 ; _rtDW -> voltages_DSTATE [ 1 ] = _rtB ->
B_55_590_0 ; _rtDW -> voltages_DSTATE [ 2 ] = _rtB -> B_55_615_0 ; _rtDW ->
voltages_DSTATE [ 3 ] = _rtB -> B_55_23_0_d [ 0 ] ; _rtDW -> voltages_DSTATE
[ 4 ] = _rtB -> B_55_23_0_d [ 1 ] ; _rtDW -> voltages_DSTATE [ 5 ] = _rtB ->
B_55_23_0_d [ 2 ] ; _rtDW -> Rotorspeeddeviationdw_SYSTEM_ENABLE = 0U ; _rtDW
-> Rotorspeeddeviationdw_DSTATE = _rtP -> P_561 * _rtB -> B_55_625_0 + _rtB
-> B_55_626_0 ; { real_T xnew [ 2 ] ; xnew [ 0 ] = ( _rtP -> P_565 [ 0 ] ) *
_rtDW -> DiscreteStateSpace_DSTATE [ 0 ] + ( _rtP -> P_565 [ 1 ] ) * _rtDW ->
DiscreteStateSpace_DSTATE [ 1 ] ; xnew [ 0 ] += ( _rtP -> P_566 [ 0 ] ) *
_rtB -> B_55_566_0 [ 3 ] ; xnew [ 1 ] = ( _rtP -> P_565 [ 2 ] ) * _rtDW ->
DiscreteStateSpace_DSTATE [ 0 ] + ( _rtP -> P_565 [ 3 ] ) * _rtDW ->
DiscreteStateSpace_DSTATE [ 1 ] ; xnew [ 1 ] += ( _rtP -> P_566 [ 1 ] ) *
_rtB -> B_55_566_0 [ 3 ] ; ( void ) memcpy ( & _rtDW ->
DiscreteStateSpace_DSTATE [ 0 ] , xnew , sizeof ( real_T ) * 2 ) ; } { real_T
xnew [ 1 ] ; xnew [ 0 ] = _rtP -> P_571 * _rtDW ->
DiscreteStateSpace_DSTATE_j ; xnew [ 0 ] += _rtP -> P_572 * _rtB ->
B_55_635_0 ; ( void ) memcpy ( & _rtDW -> DiscreteStateSpace_DSTATE_j , xnew
, sizeof ( real_T ) * 1 ) ; } { real_T xnew [ 1 ] ; xnew [ 0 ] = _rtP ->
P_577 * _rtDW -> DiscreteStateSpace_DSTATE_b ; xnew [ 0 ] += _rtP -> P_578 *
_rtB -> B_55_637_0 ; ( void ) memcpy ( & _rtDW -> DiscreteStateSpace_DSTATE_b
, xnew , sizeof ( real_T ) * 1 ) ; } { real_T xnew [ 1 ] ; xnew [ 0 ] = _rtP
-> P_583 * _rtDW -> DiscreteStateSpace_DSTATE_o ; xnew [ 0 ] += _rtP -> P_584
* _rtB -> B_55_639_0 ; ( void ) memcpy ( & _rtDW ->
DiscreteStateSpace_DSTATE_o , xnew , sizeof ( real_T ) * 1 ) ; } { real_T
xnew [ 1 ] ; xnew [ 0 ] = _rtP -> P_589 * _rtDW ->
DiscreteStateSpace_DSTATE_f ; xnew [ 0 ] += _rtP -> P_590 * _rtB ->
B_55_641_0 ; ( void ) memcpy ( & _rtDW -> DiscreteStateSpace_DSTATE_f , xnew
, sizeof ( real_T ) * 1 ) ; } { real_T xnew [ 3 ] ; xnew [ 0 ] = ( _rtP ->
P_594 [ 0 ] ) * _rtDW -> DiscreteStateSpace_DSTATE_bk [ 0 ] + ( _rtP -> P_594
[ 1 ] ) * _rtDW -> DiscreteStateSpace_DSTATE_bk [ 1 ] + ( _rtP -> P_594 [ 2 ]
) * _rtDW -> DiscreteStateSpace_DSTATE_bk [ 2 ] ; xnew [ 0 ] += ( _rtP ->
P_595 [ 0 ] ) * _rtB -> B_55_566_0 [ 3 ] ; xnew [ 1 ] = ( _rtP -> P_594 [ 3 ]
) * _rtDW -> DiscreteStateSpace_DSTATE_bk [ 0 ] + ( _rtP -> P_594 [ 4 ] ) *
_rtDW -> DiscreteStateSpace_DSTATE_bk [ 1 ] + ( _rtP -> P_594 [ 5 ] ) * _rtDW
-> DiscreteStateSpace_DSTATE_bk [ 2 ] ; xnew [ 1 ] += ( _rtP -> P_595 [ 1 ] )
* _rtB -> B_55_566_0 [ 3 ] ; xnew [ 2 ] = ( _rtP -> P_594 [ 6 ] ) * _rtDW ->
DiscreteStateSpace_DSTATE_bk [ 0 ] + ( _rtP -> P_594 [ 7 ] ) * _rtDW ->
DiscreteStateSpace_DSTATE_bk [ 1 ] + ( _rtP -> P_594 [ 8 ] ) * _rtDW ->
DiscreteStateSpace_DSTATE_bk [ 2 ] ; xnew [ 2 ] += ( _rtP -> P_595 [ 2 ] ) *
_rtB -> B_55_566_0 [ 3 ] ; ( void ) memcpy ( & _rtDW ->
DiscreteStateSpace_DSTATE_bk [ 0 ] , xnew , sizeof ( real_T ) * 3 ) ; } {
real_T xnew [ 1 ] ; xnew [ 0 ] = _rtP -> P_600 * _rtDW ->
DiscreteStateSpace_DSTATE_a ; xnew [ 0 ] += _rtP -> P_601 * _rtB ->
B_55_644_0 ; ( void ) memcpy ( & _rtDW -> DiscreteStateSpace_DSTATE_a , xnew
, sizeof ( real_T ) * 1 ) ; } { real_T xnew [ 1 ] ; xnew [ 0 ] = _rtP ->
P_606 * _rtDW -> DiscreteStateSpace_DSTATE_l ; xnew [ 0 ] += _rtP -> P_607 *
_rtB -> B_55_646_0 ; ( void ) memcpy ( & _rtDW -> DiscreteStateSpace_DSTATE_l
, xnew , sizeof ( real_T ) * 1 ) ; }
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer_Update ( S , _rtB ->
B_55_648_0 ) ; DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer_Update (
S , _rtB -> B_55_651_0 ) ;
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer_Update ( S , _rtB ->
B_55_656_0 ) ; DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer_Update (
S , _rtB -> B_55_659_0 ) ;
DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer_Update ( S , _rtB ->
B_55_664_0 ) ; DZG_v6_SimGen_Transferfunctionsfordetailedstabilizer_Update (
S , _rtB -> B_55_667_0 ) ; { real_T xnew [ 1 ] ; xnew [ 0 ] = _rtP -> P_629 *
_rtDW -> DiscreteStateSpace_DSTATE_i ; xnew [ 0 ] += _rtP -> P_630 * _rtB ->
B_55_678_0 ; ( void ) memcpy ( & _rtDW -> DiscreteStateSpace_DSTATE_i , xnew
, sizeof ( real_T ) * 1 ) ; } _rtDW -> UnitDelay1_DSTATE = _rtB -> B_55_702_0
; { real_T xnew [ 1 ] ; xnew [ 0 ] = _rtP -> P_641 * _rtDW ->
DiscreteStateSpace_DSTATE_h ; xnew [ 0 ] += _rtP -> P_642 * _rtB ->
B_55_683_0 ; ( void ) memcpy ( & _rtDW -> DiscreteStateSpace_DSTATE_h , xnew
, sizeof ( real_T ) * 1 ) ; } { real_T xnew [ 1 ] ; xnew [ 0 ] = ( _rtP ->
P_648 ) * _rtDW -> DiscreteStateSpace_DSTATE_be ; xnew [ 0 ] += _rtP -> P_649
* _rtB -> B_55_699_0 ; ( void ) memcpy ( & _rtDW ->
DiscreteStateSpace_DSTATE_be , xnew , sizeof ( real_T ) * 1 ) ; } { real_T
xnew [ 1 ] ; xnew [ 0 ] = _rtP -> P_654 * _rtDW ->
DiscreteStateSpace_DSTATE_n ; xnew [ 0 ] += _rtP -> P_655 * _rtB ->
B_55_701_0 ; ( void ) memcpy ( & _rtDW -> DiscreteStateSpace_DSTATE_n , xnew
, sizeof ( real_T ) * 1 ) ; } } { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_aw . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_aw . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_l4 . Head = ( ( _rtDW
-> TransportDelay_IWORK_l4 . Head < ( _rtDW -> TransportDelay_IWORK_l4 .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_l4 . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_l4 . Head == _rtDW ->
TransportDelay_IWORK_l4 . Tail ) { if ( !
DZG_v6_SimGen_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_l4 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_l4 . Tail , & _rtDW -> TransportDelay_IWORK_l4 . Head ,
& _rtDW -> TransportDelay_IWORK_l4 . Last , simTime - _rtP -> P_687 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_l4 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_l4 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_l4 . Head ] = _rtB -> B_55_724_0 ; } UNUSED_PARAMETER (
tid ) ; }
#define MDL_UPDATE
static void mdlUpdateTID4 ( SimStruct * S , int_T tid ) { UNUSED_PARAMETER (
tid ) ; }
#define MDL_DERIVATIVES
static void mdlDerivatives ( SimStruct * S ) { boolean_T lsat ; boolean_T
usat ; B_DZG_v6_SimGen_T * _rtB ; P_DZG_v6_SimGen_T * _rtP ;
X_DZG_v6_SimGen_T * _rtX ; XDot_DZG_v6_SimGen_T * _rtXdot ; _rtXdot = ( (
XDot_DZG_v6_SimGen_T * ) ssGetdX ( S ) ) ; _rtX = ( ( X_DZG_v6_SimGen_T * )
ssGetContStates ( S ) ) ; _rtP = ( ( P_DZG_v6_SimGen_T * ) ssGetModelRtp ( S
) ) ; _rtB = ( ( B_DZG_v6_SimGen_T * ) _ssGetModelBlockIO ( S ) ) ; _rtXdot
-> integrator_CSTATE = _rtB -> B_55_100_0 ; _rtXdot -> integrator_CSTATE_l =
_rtB -> B_55_102_0 ; _rtXdot -> integrator_CSTATE_i = _rtB -> B_55_104_0 ;
_rtXdot -> integrator_CSTATE_f = _rtB -> B_55_106_0 ; _rtXdot ->
integrator_CSTATE_ly = _rtB -> B_55_174_0 ; _rtXdot -> integrator_CSTATE_c =
_rtB -> B_55_176_0 ; _rtXdot -> integrator_CSTATE_j = _rtB -> B_55_178_0 ;
_rtXdot -> integrator_CSTATE_m = _rtB -> B_55_180_0 ; _rtXdot ->
integrator_CSTATE_o = _rtB -> B_55_248_0 ; _rtXdot -> integrator_CSTATE_d =
_rtB -> B_55_250_0 ; _rtXdot -> integrator_CSTATE_jx = _rtB -> B_55_252_0 ;
_rtXdot -> integrator_CSTATE_k = _rtB -> B_55_254_0 ; _rtXdot ->
integrator_CSTATE_kg = _rtB -> B_55_322_0 ; _rtXdot -> integrator_CSTATE_fi =
_rtB -> B_55_324_0 ; _rtXdot -> integrator_CSTATE_e = _rtB -> B_55_326_0 ;
_rtXdot -> integrator_CSTATE_a = _rtB -> B_55_328_0 ; _rtXdot ->
integrator_CSTATE_ol = _rtB -> B_55_396_0 ; _rtXdot -> integrator_CSTATE_h =
_rtB -> B_55_398_0 ; _rtXdot -> integrator_CSTATE_b = _rtB -> B_55_400_0 ;
_rtXdot -> integrator_CSTATE_jb = _rtB -> B_55_402_0 ; _rtXdot ->
integrator_CSTATE_df = _rtB -> B_55_470_0 ; _rtXdot -> integrator_CSTATE_p =
_rtB -> B_55_472_0 ; _rtXdot -> integrator_CSTATE_he = _rtB -> B_55_474_0 ;
_rtXdot -> integrator_CSTATE_hj = _rtB -> B_55_476_0 ; _rtXdot ->
integrator_CSTATE_n = _rtB -> B_55_544_0 ; _rtXdot -> integrator_CSTATE_iy =
_rtB -> B_55_546_0 ; _rtXdot -> integrator_CSTATE_jz = _rtB -> B_55_548_0 ;
_rtXdot -> integrator_CSTATE_lg = _rtB -> B_55_550_0 ; _rtXdot ->
Integrator2_CSTATE = _rtB -> B_55_741_0 ; _rtXdot -> Integrator1_CSTATE =
_rtB -> B_55_739_0 ; _rtXdot -> Integrator3_CSTATE = _rtB -> B_55_743_0 ;
lsat = ( _rtX -> IntegratorLimited_CSTATE <= _rtP -> P_661 ) ; usat = ( _rtX
-> IntegratorLimited_CSTATE >= _rtP -> P_660 ) ; if ( ( ( ! lsat ) && ( !
usat ) ) || ( lsat && ( _rtB -> B_55_718_0 > 0.0 ) ) || ( usat && ( _rtB ->
B_55_718_0 < 0.0 ) ) ) { _rtXdot -> IntegratorLimited_CSTATE = _rtB ->
B_55_718_0 ; } else { _rtXdot -> IntegratorLimited_CSTATE = 0.0 ; } _rtXdot
-> Integrator1_CSTATE_i = _rtB -> B_55_715_0 ; _rtXdot ->
Integrator1_CSTATE_d = _rtB -> B_55_737_0 ; _rtXdot -> Integrator2_CSTATE_f =
_rtB -> B_55_734_0 ; _rtXdot -> Integrator1_CSTATE_c = _rtB -> B_55_721_0 ;
_rtXdot -> TransferFcn1_CSTATE = 0.0 ; _rtXdot -> TransferFcn1_CSTATE += _rtP
-> P_679 * _rtX -> TransferFcn1_CSTATE ; _rtXdot -> TransferFcn1_CSTATE +=
_rtB -> B_55_735_0 ; _rtXdot -> Integrator1_CSTATE_l = _rtB -> B_55_731_0 ;
_rtXdot -> TransferFcn_CSTATE = 0.0 ; _rtXdot -> TransferFcn_CSTATE += _rtP
-> P_685 * _rtX -> TransferFcn_CSTATE ; _rtXdot -> TransferFcn_CSTATE += _rtB
-> B_55_736_0 ; } static void mdlInitializeSizes ( SimStruct * S ) {
ssSetChecksumVal ( S , 0 , 2724187759U ) ; ssSetChecksumVal ( S , 1 ,
2062338473U ) ; ssSetChecksumVal ( S , 2 , 2360657506U ) ; ssSetChecksumVal (
S , 3 , 2487284680U ) ; { mxArray * slVerStructMat = NULL ; mxArray *
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
( S , mdlOutputsTID4 ) ; } static void mdlTerminate ( SimStruct * S ) { }
#include "simulink.c"
