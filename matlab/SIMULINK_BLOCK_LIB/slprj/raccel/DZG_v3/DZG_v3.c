#include "rt_logging_mmi.h"
#include "DZG_v3_capi.h"
#include <math.h>
#include "DZG_v3.h"
#include "DZG_v3_private.h"
#include "DZG_v3_dt.h"
extern void * CreateDiagnosticAsVoidPtr_wrapper ( const char * id , int nargs
, ... ) ; RTWExtModeInfo * gblRTWExtModeInfo = NULL ; extern boolean_T
gblExtModeStartPktReceived ; void raccelForceExtModeShutdown ( ) { if ( !
gblExtModeStartPktReceived ) { boolean_T stopRequested = false ;
rtExtModeWaitForStartPkt ( gblRTWExtModeInfo , 2 , & stopRequested ) ; }
rtExtModeShutdown ( 2 ) ; }
#include "slsv_diagnostic_codegen_c_api.h"
const int_T gblNumToFiles = 0 ; const int_T gblNumFrFiles = 0 ; const int_T
gblNumFrWksBlocks = 0 ;
#ifdef RSIM_WITH_SOLVER_MULTITASKING
boolean_T gbl_raccel_isMultitasking = 1 ;
#else
boolean_T gbl_raccel_isMultitasking = 0 ;
#endif
boolean_T gbl_raccel_tid01eq = 1 ; int_T gbl_raccel_NumST = 3 ; const char_T
* gbl_raccel_Version = "9.3 (R2020a) 18-Nov-2019" ; void
raccel_setup_MMIStateLog ( SimStruct * S ) {
#ifdef UseMMIDataLogging
rt_FillStateSigInfoFromMMI ( ssGetRTWLogInfo ( S ) , & ssGetErrorStatus ( S )
) ;
#else
UNUSED_PARAMETER ( S ) ;
#endif
} static DataMapInfo rt_dataMapInfo ; DataMapInfo * rt_dataMapInfoPtr = &
rt_dataMapInfo ; rtwCAPI_ModelMappingInfo * rt_modelMapInfoPtr = & (
rt_dataMapInfo . mmi ) ; const char * gblSlvrJacPatternFileName =
"slprj\\raccel\\DZG_v3\\DZG_v3_Jpattern.mat" ; const int_T
gblNumRootInportBlks = 0 ; const int_T gblNumModelInputs = 0 ; extern
rtInportTUtable * gblInportTUtables ; extern const char * gblInportFileName ;
extern void * gblAperiodicPartitionHitTimes ; const int_T
gblInportDataTypeIdx [ ] = { - 1 } ; const int_T gblInportDims [ ] = { - 1 }
; const int_T gblInportComplex [ ] = { - 1 } ; const int_T
gblInportInterpoFlag [ ] = { - 1 } ; const int_T gblInportContinuous [ ] = {
- 1 } ; int_T enableFcnCallFlag [ ] = { 1 , 1 , 1 } ; const char *
raccelLoadInputsAndAperiodicHitTimes ( const char * inportFileName , int *
matFileFormat ) { return rt_RapidReadInportsMatFile ( inportFileName ,
matFileFormat , 1 ) ; }
#include "simstruc.h"
#include "fixedpoint.h"
B rtB ; X rtX ; DW rtDW ; PrevZCX rtPrevZCX ; static SimStruct model_S ;
SimStruct * const rtS = & model_S ;
#ifndef __RTW_UTFREE__  
extern void * utMalloc ( size_t ) ;
#endif
void * rt_TDelayCreateBuf ( int_T numBuffer , int_T bufSz , int_T elemSz ) {
return ( ( void * ) utMalloc ( numBuffer * bufSz * elemSz ) ) ; }
#ifndef __RTW_UTFREE__  
extern void * utMalloc ( size_t ) ; extern void utFree ( void * ) ;
#endif
boolean_T rt_TDelayUpdateTailOrGrowBuf ( int_T * bufSzPtr , int_T * tailPtr ,
int_T * headPtr , int_T * lastPtr , real_T tMinusDelay , real_T * * tBufPtr ,
real_T * * uBufPtr , real_T * * xBufPtr , boolean_T isfixedbuf , boolean_T
istransportdelay , int_T * maxNewBufSzPtr ) { int_T testIdx ; int_T tail = *
tailPtr ; int_T bufSz = * bufSzPtr ; real_T * tBuf = * tBufPtr ; real_T *
xBuf = ( NULL ) ; int_T numBuffer = 2 ; if ( istransportdelay ) { numBuffer =
3 ; xBuf = * xBufPtr ; } testIdx = ( tail < ( bufSz - 1 ) ) ? ( tail + 1 ) :
0 ; if ( ( tMinusDelay <= tBuf [ testIdx ] ) && ! isfixedbuf ) { int_T j ;
real_T * tempT ; real_T * tempU ; real_T * tempX = ( NULL ) ; real_T * uBuf =
* uBufPtr ; int_T newBufSz = bufSz + 1024 ; if ( newBufSz > * maxNewBufSzPtr
) { * maxNewBufSzPtr = newBufSz ; } tempU = ( real_T * ) utMalloc ( numBuffer
* newBufSz * sizeof ( real_T ) ) ; if ( tempU == ( NULL ) ) { return ( false
) ; } tempT = tempU + newBufSz ; if ( istransportdelay ) tempX = tempT +
newBufSz ; for ( j = tail ; j < bufSz ; j ++ ) { tempT [ j - tail ] = tBuf [
j ] ; tempU [ j - tail ] = uBuf [ j ] ; if ( istransportdelay ) tempX [ j -
tail ] = xBuf [ j ] ; } for ( j = 0 ; j < tail ; j ++ ) { tempT [ j + bufSz -
tail ] = tBuf [ j ] ; tempU [ j + bufSz - tail ] = uBuf [ j ] ; if (
istransportdelay ) tempX [ j + bufSz - tail ] = xBuf [ j ] ; } if ( * lastPtr
> tail ) { * lastPtr -= tail ; } else { * lastPtr += ( bufSz - tail ) ; } *
tailPtr = 0 ; * headPtr = bufSz ; utFree ( uBuf ) ; * bufSzPtr = newBufSz ; *
tBufPtr = tempT ; * uBufPtr = tempU ; if ( istransportdelay ) * xBufPtr =
tempX ; } else { * tailPtr = testIdx ; } return ( true ) ; } real_T
rt_TDelayInterpolate ( real_T tMinusDelay , real_T tStart , real_T * tBuf ,
real_T * uBuf , int_T bufSz , int_T * lastIdx , int_T oldestIdx , int_T
newIdx , real_T initOutput , boolean_T discrete , boolean_T
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
real_T rt_VTDelayfindtDInterpolate ( real_T x , real_T * tBuf , real_T * uBuf
, real_T * xBuf , int_T bufSz , int_T head , int_T tail , int_T * pLast ,
real_T t , real_T tStart , boolean_T discrete , boolean_T
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
appliedDelay = t - tminustD ; return uD ; }
#ifndef __RTW_UTFREE__  
extern void utFree ( void * ) ;
#endif
void rt_TDelayFreeBuf ( void * buf ) { utFree ( buf ) ; } void ora1wlqeti (
nenimphfvv * localB , dq1pbrolhq * localP ) { localB -> jhmp0c0tsm = localP
-> Out1_Y0 ; } void fx2esogudm ( SimStruct * rtS_e , boolean_T fflxnsiu0g ,
real_T orjpri2rnl , nenimphfvv * localB , c2yel4glz3 * localDW , kfjh0rdg4y *
localZCE ) { boolean_T zcEvent ; if ( ssIsMajorTimeStep ( rtS_e ) ) { zcEvent
= ( fflxnsiu0g && ( localZCE -> nz1ezojz4f != POS_ZCSIG ) ) ; if ( zcEvent )
{ localB -> jhmp0c0tsm = orjpri2rnl ; localDW -> fdd1jx2t0b = 4 ; } localZCE
-> nz1ezojz4f = fflxnsiu0g ; } } void h4dgpi453n ( pfuoywnvt4 * localB ,
dm4sxbd4wc * localP ) { localB -> l5pxivnpxv = localP -> OUT_Y0 ; } void
m3qjgqfo54 ( fittvluxdk * localDW ) { localDW -> lzdgbohn4g = false ; } void
a5f3eefi2x ( SimStruct * rtS_i , fittvluxdk * localDW ) { localDW ->
lzdgbohn4g = false ; ssSetBlockStateForSolverChangedAtMajorStep ( rtS_i ) ; }
void cgvj5xjnjk ( SimStruct * rtS_i , real_T k4grt3yuge , boolean_T
fyzi5wcstb , boolean_T h5w3yyafqq , pfuoywnvt4 * localB , fittvluxdk *
localDW ) { real_T t ; if ( ssIsSampleHit ( rtS_i , 1 , 0 ) &&
ssIsMajorTimeStep ( rtS_i ) ) { if ( k4grt3yuge > 0.0 ) { if ( ! localDW ->
lzdgbohn4g ) { t = ssGetTStart ( rtS_i ) ; if ( ssGetTaskTime ( rtS_i , 1 )
!= t ) { ssSetBlockStateForSolverChangedAtMajorStep ( rtS_i ) ; } localDW ->
lzdgbohn4g = true ; } } else { if ( localDW -> lzdgbohn4g ) {
ssSetBlockStateForSolverChangedAtMajorStep ( rtS_i ) ; m3qjgqfo54 ( localDW )
; } } } if ( localDW -> lzdgbohn4g ) { localB -> l5pxivnpxv = ( ( int32_T )
h5w3yyafqq > ( int32_T ) fyzi5wcstb ) ; if ( ssIsMajorTimeStep ( rtS_i ) ) {
srUpdateBC ( localDW -> kq4t2lkn4n ) ; } } } void h33j1jr1zm ( p4n5izx35p *
localB , mknwhmtwd0 * localP ) { localB -> guqrn12esp = localP -> OUT_Y0 ; }
void f3mc3li2yn ( alhepkh51m * localDW ) { localDW -> d2wt1smltb = false ; }
void kwn5re5v52 ( SimStruct * rtS_g , alhepkh51m * localDW ) { localDW ->
d2wt1smltb = false ; ssSetBlockStateForSolverChangedAtMajorStep ( rtS_g ) ; }
void itcd43cpff ( SimStruct * rtS_j , real_T ivz1lgh0sc , boolean_T
nqdkubdyhn , boolean_T jqq1z5jyzp , p4n5izx35p * localB , alhepkh51m *
localDW ) { real_T t ; if ( ssIsSampleHit ( rtS_j , 1 , 0 ) &&
ssIsMajorTimeStep ( rtS_j ) ) { if ( ivz1lgh0sc > 0.0 ) { if ( ! localDW ->
d2wt1smltb ) { t = ssGetTStart ( rtS_j ) ; if ( ssGetTaskTime ( rtS_j , 1 )
!= t ) { ssSetBlockStateForSolverChangedAtMajorStep ( rtS_j ) ; } localDW ->
d2wt1smltb = true ; } } else { if ( localDW -> d2wt1smltb ) {
ssSetBlockStateForSolverChangedAtMajorStep ( rtS_j ) ; f3mc3li2yn ( localDW )
; } } } if ( localDW -> d2wt1smltb ) { localB -> guqrn12esp = ( ( int32_T )
jqq1z5jyzp < ( int32_T ) nqdkubdyhn ) ; if ( ssIsMajorTimeStep ( rtS_j ) ) {
srUpdateBC ( localDW -> hjywrjdfvg ) ; } } } void p5hgyyqss4 ( jk5bp5fzdn *
localB , mrvrl4is5b * localDW , aynkghgu2e * localP , boolean_T rtp_ic ) {
localDW -> efcdoeeasm = rtp_ic ; localDW -> k14z2hdr3e = localP ->
SampleandHold_ic ; h33j1jr1zm ( & localB -> itcd43cpffs , & localP ->
itcd43cpffs ) ; h4dgpi453n ( & localB -> cgvj5xjnjkh , & localP ->
cgvj5xjnjkh ) ; ora1wlqeti ( & localB -> fx2esogudmq , & localP ->
fx2esogudmq ) ; localB -> mjqylog0rb = localP -> OUT_Y0 ; } void n0lbj4yv0e (
mrvrl4is5b * localDW ) { if ( localDW -> itcd43cpffs . d2wt1smltb ) {
f3mc3li2yn ( & localDW -> itcd43cpffs ) ; } if ( localDW -> cgvj5xjnjkh .
lzdgbohn4g ) { m3qjgqfo54 ( & localDW -> cgvj5xjnjkh ) ; } localDW ->
mc0imgx5qy = false ; } void fovxs4153s ( SimStruct * rtS_m , mrvrl4is5b *
localDW ) { localDW -> mc0imgx5qy = false ;
ssSetBlockStateForSolverChangedAtMajorStep ( rtS_m ) ; kwn5re5v52 ( rtS_m , &
localDW -> itcd43cpffs ) ; a5f3eefi2x ( rtS_m , & localDW -> cgvj5xjnjkh ) ;
} void bhc3qgnuic ( SimStruct * rtS_k , boolean_T lupksbzqrb , boolean_T
mqwzmjfrx3 , real_T fuppb0sv3g , real_T imqbiz5qjw , jk5bp5fzdn * localB ,
mrvrl4is5b * localDW , aynkghgu2e * localP , lshrplgrom * localZCE ) { real_T
t ; if ( ssIsSampleHit ( rtS_k , 1 , 0 ) && ssIsMajorTimeStep ( rtS_k ) ) {
if ( lupksbzqrb ) { if ( ! localDW -> mc0imgx5qy ) { t = ssGetTStart ( rtS_k
) ; if ( ssGetTaskTime ( rtS_k , 1 ) != t ) {
ssSetBlockStateForSolverChangedAtMajorStep ( rtS_k ) ; } localDW ->
mc0imgx5qy = true ; } } else { if ( localDW -> mc0imgx5qy ) {
ssSetBlockStateForSolverChangedAtMajorStep ( rtS_k ) ; n0lbj4yv0e ( localDW )
; } } } if ( localDW -> mc0imgx5qy ) { localB -> dh55lfppdo = ssGetT ( rtS_k
) ; localB -> n1dhiyndse = mqwzmjfrx3 ; if ( ssIsSampleHit ( rtS_k , 1 , 0 )
) { localB -> f0zcs0qzz3 = localDW -> efcdoeeasm ; } itcd43cpff ( rtS_k ,
localB -> efq0lwygf3 [ 0 ] , localB -> n1dhiyndse , localB -> f0zcs0qzz3 , &
localB -> itcd43cpffs , & localDW -> itcd43cpffs ) ; cgvj5xjnjk ( rtS_k ,
localB -> efq0lwygf3 [ 1 ] , localB -> n1dhiyndse , localB -> f0zcs0qzz3 , &
localB -> cgvj5xjnjkh , & localDW -> cgvj5xjnjkh ) ; localB -> lu4ohon3ic = (
localB -> itcd43cpffs . guqrn12esp || localB -> cgvj5xjnjkh . l5pxivnpxv ) ;
if ( ssIsSampleHit ( rtS_k , 1 , 0 ) ) { fx2esogudm ( rtS_k , localB ->
lu4ohon3ic , localB -> dh55lfppdo , & localB -> fx2esogudmq , & localDW ->
fx2esogudmq , & localZCE -> fx2esogudmq ) ; localB -> frfczxfroz = localB ->
fx2esogudmq . jhmp0c0tsm + localP -> Constant_Value ; localB -> ftduywgdsn =
localDW -> k14z2hdr3e ; } localB -> bavhmmsjzl = ( localB -> frfczxfroz >
localB -> dh55lfppdo ) ; localB -> bmrz3wftez = ! mqwzmjfrx3 ; localB ->
pitxwa0rm5 = localB -> lu4ohon3ic ; if ( localB -> pitxwa0rm5 ) { localB ->
ldmuucy4uq = fuppb0sv3g ; } else { localB -> ldmuucy4uq = localB ->
ftduywgdsn ; } localB -> n3rlehzfnt = localB -> ldmuucy4uq + imqbiz5qjw ;
localB -> jad5dv20ed = ( fuppb0sv3g >= localB -> n3rlehzfnt ) ; localB ->
h3k4qyzuku = ( localB -> jad5dv20ed && localB -> bmrz3wftez ) ; localB ->
mjqylog0rb = ! localB -> h3k4qyzuku ; if ( ssIsMajorTimeStep ( rtS_k ) ) {
srUpdateBC ( localDW -> c3fzgpzsmi ) ; } } } void bhc3qgnuicTID2 ( SimStruct
* rtS_c , jk5bp5fzdn * localB , mrvrl4is5b * localDW , aynkghgu2e * localP )
{ switch ( ( int32_T ) localP -> EdgeDetector_model ) { case 1 : localB ->
efq0lwygf3 [ 0 ] = localP -> posedge_Value [ 0 ] ; localB -> efq0lwygf3 [ 1 ]
= localP -> posedge_Value [ 1 ] ; break ; case 2 : localB -> efq0lwygf3 [ 0 ]
= localP -> negedge_Value [ 0 ] ; localB -> efq0lwygf3 [ 1 ] = localP ->
negedge_Value [ 1 ] ; break ; default : localB -> efq0lwygf3 [ 0 ] = localP
-> eitheredge_Value [ 0 ] ; localB -> efq0lwygf3 [ 1 ] = localP ->
eitheredge_Value [ 1 ] ; break ; } if ( ssIsMajorTimeStep ( rtS_c ) ) {
srUpdateBC ( localDW -> c3fzgpzsmi ) ; } } void ny353qchui ( SimStruct *
rtS_b , jk5bp5fzdn * localB , mrvrl4is5b * localDW ) { if ( localDW ->
mc0imgx5qy && ssIsSampleHit ( rtS_b , 1 , 0 ) ) { localDW -> efcdoeeasm =
localB -> n1dhiyndse ; localDW -> k14z2hdr3e = localB -> ldmuucy4uq ; } }
void mbb3rj5v0g ( ozzv521fhb * localB , mrj3zmuhdd * localDW , cpv1hiufg3 *
localP , boolean_T rtp_ic ) { localDW -> brsj2ijrj4 = rtp_ic ; localDW ->
bx5o5grkfs = localP -> SampleandHold_ic ; h33j1jr1zm ( & localB -> cascrfypvi
, & localP -> cascrfypvi ) ; h4dgpi453n ( & localB -> cuc3rxn500 , & localP
-> cuc3rxn500 ) ; ora1wlqeti ( & localB -> hirdn25ewp , & localP ->
hirdn25ewp ) ; localB -> j1vu52vt5l = localP -> OUT_Y0 ; } void cgjlc55t5n (
mrj3zmuhdd * localDW ) { if ( localDW -> cascrfypvi . d2wt1smltb ) {
f3mc3li2yn ( & localDW -> cascrfypvi ) ; } if ( localDW -> cuc3rxn500 .
lzdgbohn4g ) { m3qjgqfo54 ( & localDW -> cuc3rxn500 ) ; } localDW ->
dajs3ag4lh = false ; } void ei2bq4gymn ( SimStruct * rtS_g , mrj3zmuhdd *
localDW ) { localDW -> dajs3ag4lh = false ;
ssSetBlockStateForSolverChangedAtMajorStep ( rtS_g ) ; kwn5re5v52 ( rtS_g , &
localDW -> cascrfypvi ) ; a5f3eefi2x ( rtS_g , & localDW -> cuc3rxn500 ) ; }
void on20slldft ( SimStruct * rtS_d , boolean_T f2dowwvjdu , boolean_T
dkh4fhrrog , real_T aupbitltac , real_T p4i2dovx34 , ozzv521fhb * localB ,
mrj3zmuhdd * localDW , cpv1hiufg3 * localP , lkx3rpkald * localZCE ) { real_T
t ; if ( ssIsSampleHit ( rtS_d , 1 , 0 ) && ssIsMajorTimeStep ( rtS_d ) ) {
if ( f2dowwvjdu ) { if ( ! localDW -> dajs3ag4lh ) { t = ssGetTStart ( rtS_d
) ; if ( ssGetTaskTime ( rtS_d , 1 ) != t ) {
ssSetBlockStateForSolverChangedAtMajorStep ( rtS_d ) ; } localDW ->
dajs3ag4lh = true ; } } else { if ( localDW -> dajs3ag4lh ) {
ssSetBlockStateForSolverChangedAtMajorStep ( rtS_d ) ; cgjlc55t5n ( localDW )
; } } } if ( localDW -> dajs3ag4lh ) { localB -> jnkb2wi5zt = ssGetT ( rtS_d
) ; localB -> asd01zyioe = dkh4fhrrog ; if ( ssIsSampleHit ( rtS_d , 1 , 0 )
) { localB -> m1hqtvieyu = localDW -> brsj2ijrj4 ; } itcd43cpff ( rtS_d ,
localB -> jysrqdcssc [ 0 ] , localB -> asd01zyioe , localB -> m1hqtvieyu , &
localB -> cascrfypvi , & localDW -> cascrfypvi ) ; cgvj5xjnjk ( rtS_d ,
localB -> jysrqdcssc [ 1 ] , localB -> asd01zyioe , localB -> m1hqtvieyu , &
localB -> cuc3rxn500 , & localDW -> cuc3rxn500 ) ; localB -> f2bwvc0yjj = (
localB -> cascrfypvi . guqrn12esp || localB -> cuc3rxn500 . l5pxivnpxv ) ; if
( ssIsSampleHit ( rtS_d , 1 , 0 ) ) { fx2esogudm ( rtS_d , localB ->
f2bwvc0yjj , localB -> jnkb2wi5zt , & localB -> hirdn25ewp , & localDW ->
hirdn25ewp , & localZCE -> hirdn25ewp ) ; localB -> me4mo4kjpl = localB ->
hirdn25ewp . jhmp0c0tsm + localP -> Constant_Value ; localB -> ncxecic4hb =
localDW -> bx5o5grkfs ; } localB -> jcjtkrvt1d = ( localB -> me4mo4kjpl >
localB -> jnkb2wi5zt ) ; localB -> ka0uoi5lkp = localB -> f2bwvc0yjj ; if (
localB -> ka0uoi5lkp ) { localB -> axvj32kpfa = aupbitltac ; } else { localB
-> axvj32kpfa = localB -> ncxecic4hb ; } localB -> mx1hle02i3 = localB ->
axvj32kpfa + p4i2dovx34 ; localB -> m4vib5kgnz = ( aupbitltac >= localB ->
mx1hle02i3 ) ; localB -> j1vu52vt5l = ( localB -> m4vib5kgnz && dkh4fhrrog )
; if ( ssIsMajorTimeStep ( rtS_d ) ) { srUpdateBC ( localDW -> pbtm3vaf5b ) ;
} } } void on20slldftTID2 ( SimStruct * rtS_d , ozzv521fhb * localB ,
mrj3zmuhdd * localDW , cpv1hiufg3 * localP ) { switch ( ( int32_T ) localP ->
EdgeDetector_model ) { case 1 : localB -> jysrqdcssc [ 0 ] = localP ->
posedge_Value [ 0 ] ; localB -> jysrqdcssc [ 1 ] = localP -> posedge_Value [
1 ] ; break ; case 2 : localB -> jysrqdcssc [ 0 ] = localP -> negedge_Value [
0 ] ; localB -> jysrqdcssc [ 1 ] = localP -> negedge_Value [ 1 ] ; break ;
default : localB -> jysrqdcssc [ 0 ] = localP -> eitheredge_Value [ 0 ] ;
localB -> jysrqdcssc [ 1 ] = localP -> eitheredge_Value [ 1 ] ; break ; } if
( ssIsMajorTimeStep ( rtS_d ) ) { srUpdateBC ( localDW -> pbtm3vaf5b ) ; } }
void lwcs3jh3k2 ( SimStruct * rtS_l , ozzv521fhb * localB , mrj3zmuhdd *
localDW ) { if ( localDW -> dajs3ag4lh && ssIsSampleHit ( rtS_l , 1 , 0 ) ) {
localDW -> brsj2ijrj4 = localB -> asd01zyioe ; localDW -> bx5o5grkfs = localB
-> axvj32kpfa ; } } void pzffuxhmmn ( k0g5xdyqr2 * localB , hfic5lwxhq *
localP ) { localB -> pg4m5drjhd = localP -> dq_Y0 [ 0 ] ; localB ->
jbsowncmif = localP -> dq_Y0 [ 1 ] ; } void cl3avcjyrj ( c4a0p3nf0r * localDW
) { localDW -> naigfwcupc = false ; } void hjgrwuhwvr ( SimStruct * rtS_hn ,
c4a0p3nf0r * localDW ) { localDW -> naigfwcupc = false ;
ssSetBlockStateForSolverChangedAtMajorStep ( rtS_hn ) ; } void kseqhf442u (
SimStruct * rtS_pj , uint8_T ij3w1ygrqh , const real_T e1wis5d0jt [ 2 ] ,
real_T bcr5qqte0s , k0g5xdyqr2 * localB , c4a0p3nf0r * localDW ) { real_T t ;
if ( ssIsSampleHit ( rtS_pj , 1 , 0 ) && ssIsMajorTimeStep ( rtS_pj ) ) { if
( ij3w1ygrqh > 0 ) { if ( ! localDW -> naigfwcupc ) { t = ssGetTStart (
rtS_pj ) ; if ( ssGetTaskTime ( rtS_pj , 1 ) != t ) {
ssSetBlockStateForSolverChangedAtMajorStep ( rtS_pj ) ; } localDW ->
naigfwcupc = true ; } } else { if ( localDW -> naigfwcupc ) {
ssSetBlockStateForSolverChangedAtMajorStep ( rtS_pj ) ; cl3avcjyrj ( localDW
) ; } } } if ( localDW -> naigfwcupc ) { localB -> pg4m5drjhd = e1wis5d0jt [
0 ] * muDoubleScalarSin ( bcr5qqte0s ) - e1wis5d0jt [ 1 ] * muDoubleScalarCos
( bcr5qqte0s ) ; localB -> jbsowncmif = e1wis5d0jt [ 0 ] * muDoubleScalarCos
( bcr5qqte0s ) + e1wis5d0jt [ 1 ] * muDoubleScalarSin ( bcr5qqte0s ) ; if (
ssIsMajorTimeStep ( rtS_pj ) ) { srUpdateBC ( localDW -> fsmzugaw0h ) ; } } }
void oitgzbgdnq ( cdef0al4wh * localB , nzb40xcmsy * localP ) { localB ->
pbikwiccuu = localP -> dq_Y0 [ 0 ] ; localB -> d0xxcannsh = localP -> dq_Y0 [
1 ] ; } void c5hqhunjdy ( jevkkrp5kd * localDW ) { localDW -> htd2qpz0x3 =
false ; } void jztd2wvlzd ( SimStruct * rtS_pk , jevkkrp5kd * localDW ) {
localDW -> htd2qpz0x3 = false ; ssSetBlockStateForSolverChangedAtMajorStep (
rtS_pk ) ; } void cgsqxgexc2 ( SimStruct * rtS_kh , uint8_T akf3t4vetg ,
const real_T kijsznk4qm [ 2 ] , real_T bq2hzajal0 , cdef0al4wh * localB ,
jevkkrp5kd * localDW ) { real_T t ; if ( ssIsSampleHit ( rtS_kh , 1 , 0 ) &&
ssIsMajorTimeStep ( rtS_kh ) ) { if ( akf3t4vetg > 0 ) { if ( ! localDW ->
htd2qpz0x3 ) { t = ssGetTStart ( rtS_kh ) ; if ( ssGetTaskTime ( rtS_kh , 1 )
!= t ) { ssSetBlockStateForSolverChangedAtMajorStep ( rtS_kh ) ; } localDW ->
htd2qpz0x3 = true ; } } else { if ( localDW -> htd2qpz0x3 ) {
ssSetBlockStateForSolverChangedAtMajorStep ( rtS_kh ) ; c5hqhunjdy ( localDW
) ; } } } if ( localDW -> htd2qpz0x3 ) { localB -> pbikwiccuu = kijsznk4qm [
0 ] * muDoubleScalarCos ( bq2hzajal0 ) + kijsznk4qm [ 1 ] * muDoubleScalarSin
( bq2hzajal0 ) ; localB -> d0xxcannsh = - kijsznk4qm [ 0 ] *
muDoubleScalarSin ( bq2hzajal0 ) + kijsznk4qm [ 1 ] * muDoubleScalarCos (
bq2hzajal0 ) ; if ( ssIsMajorTimeStep ( rtS_kh ) ) { srUpdateBC ( localDW ->
kdh1m4autw ) ; } } } void azcfs245nz ( icyonxkqj1 * localB , ocqjbneyfv *
localP ) { localB -> hjcfumkc1i . re = localP -> Out_Y0 ; localB ->
hjcfumkc1i . im = 0.0 ; } void b5dtpmhnl0 ( fqje1r52jc * localDW ) { localDW
-> hz34jcw0vn = false ; } void kqrkri2pgl ( SimStruct * rtS_fv , fqje1r52jc *
localDW ) { localDW -> hz34jcw0vn = false ;
ssSetBlockStateForSolverChangedAtMajorStep ( rtS_fv ) ; } void jvoeh2x3fa (
SimStruct * rtS_nf , real_T p3nbfeyjc2 , creal_T h0x051a543 , creal_T
f2xovsyrtp , creal_T e1cbmi2xac , icyonxkqj1 * localB , fqje1r52jc * localDW
, ocqjbneyfv * localP ) { real_T im ; real_T t ; if ( ssIsSampleHit ( rtS_nf
, 1 , 0 ) && ssIsMajorTimeStep ( rtS_nf ) ) { if ( p3nbfeyjc2 > 0.0 ) { if (
! localDW -> hz34jcw0vn ) { t = ssGetTStart ( rtS_nf ) ; if ( ssGetTaskTime (
rtS_nf , 1 ) != t ) { ssSetBlockStateForSolverChangedAtMajorStep ( rtS_nf ) ;
} localDW -> hz34jcw0vn = true ; } } else { if ( localDW -> hz34jcw0vn ) {
ssSetBlockStateForSolverChangedAtMajorStep ( rtS_nf ) ; b5dtpmhnl0 ( localDW
) ; } } } if ( localDW -> hz34jcw0vn ) { t = localP -> Gain1_Gain [ 0 ] . re
* h0x051a543 . re - localP -> Gain1_Gain [ 0 ] . im * h0x051a543 . im ; im =
localP -> Gain1_Gain [ 0 ] . re * h0x051a543 . im + localP -> Gain1_Gain [ 0
] . im * h0x051a543 . re ; localB -> cwtpoywgu4 [ 0 ] . re = t ; localB ->
cwtpoywgu4 [ 0 ] . im = im ; t = localP -> Gain1_Gain [ 1 ] . re * f2xovsyrtp
. re - localP -> Gain1_Gain [ 1 ] . im * f2xovsyrtp . im ; im = localP ->
Gain1_Gain [ 1 ] . re * f2xovsyrtp . im + localP -> Gain1_Gain [ 1 ] . im *
f2xovsyrtp . re ; localB -> cwtpoywgu4 [ 1 ] . re = t ; localB -> cwtpoywgu4
[ 1 ] . im = im ; t = localP -> Gain1_Gain [ 2 ] . re * e1cbmi2xac . re -
localP -> Gain1_Gain [ 2 ] . im * e1cbmi2xac . im ; im = localP -> Gain1_Gain
[ 2 ] . re * e1cbmi2xac . im + localP -> Gain1_Gain [ 2 ] . im * e1cbmi2xac .
re ; localB -> cwtpoywgu4 [ 2 ] . re = t ; localB -> cwtpoywgu4 [ 2 ] . im =
im ; t = localB -> cwtpoywgu4 [ 0 ] . re ; im = localB -> cwtpoywgu4 [ 0 ] .
im ; t += localB -> cwtpoywgu4 [ 1 ] . re ; im += localB -> cwtpoywgu4 [ 1 ]
. im ; t += localB -> cwtpoywgu4 [ 2 ] . re ; im += localB -> cwtpoywgu4 [ 2
] . im ; localB -> gt0zb2q05f . re = t ; localB -> gt0zb2q05f . im = im ;
localB -> hjcfumkc1i . re = localP -> Gain3_Gain * localB -> gt0zb2q05f . re
; localB -> hjcfumkc1i . im = localP -> Gain3_Gain * localB -> gt0zb2q05f .
im ; if ( ssIsMajorTimeStep ( rtS_nf ) ) { srUpdateBC ( localDW -> iwk0frtfal
) ; } } } void hnxt2e2yod ( cb5z2tiqbz * localB , cbyz3vd5ik * localP ) {
localB -> fnsdbz5hxw . re = localP -> Out_Y0 ; localB -> fnsdbz5hxw . im =
0.0 ; } void ptuajwmchn ( mtg0lhh3xi * localDW ) { localDW -> ohhljkksk2 =
false ; } void f3onajihkb ( SimStruct * rtS_b0 , mtg0lhh3xi * localDW ) {
localDW -> ohhljkksk2 = false ; ssSetBlockStateForSolverChangedAtMajorStep (
rtS_b0 ) ; } void czm0w3hmwh ( SimStruct * rtS_gw , real_T cf3sa0t5du ,
creal_T jpkpejfffg , creal_T m0jbhpsmo3 , creal_T mzwlt5f1ag , cb5z2tiqbz *
localB , mtg0lhh3xi * localDW , cbyz3vd5ik * localP ) { real_T
tmpForInput_idx_0_re ; real_T tmpForInput_idx_0_im ; real_T
tmpForInput_idx_1_re ; real_T tmpForInput_idx_1_im ; real_T
tmpForInput_idx_2_re ; real_T tmpForInput_idx_2_im ; if ( ssIsSampleHit (
rtS_gw , 1 , 0 ) && ssIsMajorTimeStep ( rtS_gw ) ) { if ( cf3sa0t5du > 0.0 )
{ if ( ! localDW -> ohhljkksk2 ) { tmpForInput_idx_0_re = ssGetTStart (
rtS_gw ) ; if ( ssGetTaskTime ( rtS_gw , 1 ) != tmpForInput_idx_0_re ) {
ssSetBlockStateForSolverChangedAtMajorStep ( rtS_gw ) ; } localDW ->
ohhljkksk2 = true ; } } else { if ( localDW -> ohhljkksk2 ) {
ssSetBlockStateForSolverChangedAtMajorStep ( rtS_gw ) ; ptuajwmchn ( localDW
) ; } } } if ( localDW -> ohhljkksk2 ) { tmpForInput_idx_0_re = jpkpejfffg .
re ; tmpForInput_idx_0_im = jpkpejfffg . im ; tmpForInput_idx_1_re =
m0jbhpsmo3 . re ; tmpForInput_idx_1_im = m0jbhpsmo3 . im ;
tmpForInput_idx_2_re = mzwlt5f1ag . re ; tmpForInput_idx_2_im = mzwlt5f1ag .
im ; tmpForInput_idx_0_re += tmpForInput_idx_1_re ; tmpForInput_idx_0_im +=
tmpForInput_idx_1_im ; tmpForInput_idx_0_re += tmpForInput_idx_2_re ;
tmpForInput_idx_0_im += tmpForInput_idx_2_im ; localB -> pz4dgh2fto . re =
tmpForInput_idx_0_re ; localB -> pz4dgh2fto . im = tmpForInput_idx_0_im ;
localB -> fnsdbz5hxw . re = localP -> Gain3_Gain * localB -> pz4dgh2fto . re
; localB -> fnsdbz5hxw . im = localP -> Gain3_Gain * localB -> pz4dgh2fto .
im ; if ( ssIsMajorTimeStep ( rtS_gw ) ) { srUpdateBC ( localDW -> mjsgo5sz1r
) ; } } } void MdlInitialize ( void ) { boolean_T tmp ; { int32_T i , j ;
real_T * Ds = ( real_T * ) rtDW . orezesugg1 . DS ; for ( i = 0 ; i < 3 ; i
++ ) { for ( j = 0 ; j < 3 ; j ++ ) Ds [ i * 3 + j ] = ( rtP .
StateSpace_DS_param [ i + j * 3 ] ) ; } { int_T * switch_status = ( int_T * )
rtDW . orezesugg1 . SWITCH_STATUS ; int_T * gState = ( int_T * ) rtDW .
orezesugg1 . G_STATE ; real_T * yswitch = ( real_T * ) rtDW . orezesugg1 .
Y_SWITCH ; int_T * switchTypes = ( int_T * ) rtDW . orezesugg1 . SWITCH_TYPES
; int_T * idxOutSw = ( int_T * ) rtDW . orezesugg1 . IDX_OUT_SW ; int_T *
switch_status_init = ( int_T * ) rtDW . orezesugg1 . SWITCH_STATUS_INIT ;
switch_status [ 0 ] = 0 ; switch_status_init [ 0 ] = 0 ; gState [ 0 ] = (
int_T ) 0.0 ; yswitch [ 0 ] = 1 / 0.001 ; switchTypes [ 0 ] = ( int_T ) 2.0 ;
idxOutSw [ 0 ] = ( ( int_T ) 0.0 ) - 1 ; switch_status [ 1 ] = 0 ;
switch_status_init [ 1 ] = 0 ; gState [ 1 ] = ( int_T ) 0.0 ; yswitch [ 1 ] =
1 / 0.001 ; switchTypes [ 1 ] = ( int_T ) 2.0 ; idxOutSw [ 1 ] = ( ( int_T )
0.0 ) - 1 ; switch_status [ 2 ] = 0 ; switch_status_init [ 2 ] = 0 ; gState [
2 ] = ( int_T ) 0.0 ; yswitch [ 2 ] = 1 / 0.001 ; switchTypes [ 2 ] = ( int_T
) 2.0 ; idxOutSw [ 2 ] = ( ( int_T ) 0.0 ) - 1 ; } } { int32_T i , j ; real_T
* As = ( real_T * ) rtDW . g45z4iebyu . AS ; real_T * Bs = ( real_T * ) rtDW
. g45z4iebyu . BS ; real_T * Cs = ( real_T * ) rtDW . g45z4iebyu . CS ;
real_T * Ds = ( real_T * ) rtDW . g45z4iebyu . DS ; real_T * X0 = ( real_T *
) & rtDW . fn3owwkp0o [ 0 ] ; for ( i = 0 ; i < 6 ; i ++ ) { X0 [ i ] = ( rtP
. StateSpace_X0_param [ i ] ) ; } for ( i = 0 ; i < 6 ; i ++ ) { for ( j = 0
; j < 6 ; j ++ ) As [ i * 6 + j ] = ( rtP . StateSpace_AS_param [ i + j * 6 ]
) ; for ( j = 0 ; j < 12 ; j ++ ) Bs [ i * 12 + j ] = ( rtP .
StateSpace_BS_param [ i + j * 6 ] ) ; } for ( i = 0 ; i < 48 ; i ++ ) { for (
j = 0 ; j < 6 ; j ++ ) Cs [ i * 6 + j ] = ( rtP . StateSpace_CS_param [ i + j
* 48 ] ) ; } for ( i = 0 ; i < 48 ; i ++ ) { for ( j = 0 ; j < 12 ; j ++ ) Ds
[ i * 12 + j ] = ( rtP . StateSpace_DS_param_nkmztjt1h3 [ i + j * 48 ] ) ; }
{ int_T * switch_status = ( int_T * ) rtDW . g45z4iebyu . SWITCH_STATUS ;
int_T * gState = ( int_T * ) rtDW . g45z4iebyu . G_STATE ; real_T * yswitch =
( real_T * ) rtDW . g45z4iebyu . Y_SWITCH ; int_T * switchTypes = ( int_T * )
rtDW . g45z4iebyu . SWITCH_TYPES ; int_T * idxOutSw = ( int_T * ) rtDW .
g45z4iebyu . IDX_OUT_SW ; int_T * switch_status_init = ( int_T * ) rtDW .
g45z4iebyu . SWITCH_STATUS_INIT ; switch_status [ 0 ] = 0 ;
switch_status_init [ 0 ] = 0 ; gState [ 0 ] = ( int_T ) 0.0 ; yswitch [ 0 ] =
1 / 1.0E-6 ; switchTypes [ 0 ] = ( int_T ) 2.0 ; idxOutSw [ 0 ] = ( ( int_T )
0.0 ) - 1 ; switch_status [ 1 ] = 0 ; switch_status_init [ 1 ] = 0 ; gState [
1 ] = ( int_T ) 0.0 ; yswitch [ 1 ] = 1 / 1.0E-6 ; switchTypes [ 1 ] = (
int_T ) 2.0 ; idxOutSw [ 1 ] = ( ( int_T ) 0.0 ) - 1 ; switch_status [ 2 ] =
0 ; switch_status_init [ 2 ] = 0 ; gState [ 2 ] = ( int_T ) 0.0 ; yswitch [ 2
] = 1 / 1.0E-6 ; switchTypes [ 2 ] = ( int_T ) 2.0 ; idxOutSw [ 2 ] = ( (
int_T ) 0.0 ) - 1 ; switch_status [ 3 ] = 0 ; switch_status_init [ 3 ] = 0 ;
gState [ 3 ] = ( int_T ) 1.0 ; yswitch [ 3 ] = 1 / 1.4641 ; switchTypes [ 3 ]
= ( int_T ) 2.0 ; idxOutSw [ 3 ] = ( ( int_T ) 0.0 ) - 1 ; switch_status [ 4
] = 0 ; switch_status_init [ 4 ] = 0 ; gState [ 4 ] = ( int_T ) 1.0 ; yswitch
[ 4 ] = 1 / 1.4641 ; switchTypes [ 4 ] = ( int_T ) 2.0 ; idxOutSw [ 4 ] = ( (
int_T ) 0.0 ) - 1 ; switch_status [ 5 ] = 0 ; switch_status_init [ 5 ] = 0 ;
gState [ 5 ] = ( int_T ) 1.0 ; yswitch [ 5 ] = 1 / 1.4641 ; switchTypes [ 5 ]
= ( int_T ) 2.0 ; idxOutSw [ 5 ] = ( ( int_T ) 0.0 ) - 1 ; } } rtX .
apevsa2hcq = rtP . integrator_IC_hqset3shy0 ; rtDW . bmewgg5lxa = rtP .
Memory_InitialCondition_li33iabqpt ; rtX . ebz5npnaoh = rtP .
integrator_IC_fzoxz2nwhf ; rtDW . hztiwvsh4t = rtP .
Memory_InitialCondition_bhb5ns5y5t ; rtX . g5pl2ihnsa = rtP .
integrator_IC_dvj0lfy5c5 ; rtDW . a4bul5sdk4 = rtP .
Memory_InitialCondition_hsu1tzo41x ; rtX . dzmtcwo21u = rtP .
integrator_IC_fdjwypjiem ; rtDW . ogsgimkanv = rtP .
Memory_InitialCondition_gaetu01npy ; rtX . luwntsjfce = rtP .
integrator_IC_npanbsrltt ; rtDW . jb0wxz0ail = rtP .
Memory_InitialCondition_m3jhbf3ctp ; rtX . lrou1be5ws = rtP .
integrator_IC_gom1nsmthy ; rtDW . ddy2duwo4c = rtP .
Memory_InitialCondition_jxpgjve5se ; rtX . kipcktetym = rtP .
integrator_IC_japfbuknxy ; rtDW . jhulgqvdvl = rtP .
Memory_InitialCondition_e2o42wdfoo ; rtX . hogtze5t2d = rtP .
integrator_IC_gv0sjgnwa0 ; rtDW . lq5puhde41 = rtP .
Memory_InitialCondition_lgxkf4x0at ; rtX . aubyardb3x = rtP .
integrator_IC_c2innluttg ; rtDW . d1hkn1pn3x = rtP .
Memory_InitialCondition_klpakiiqb3 ; rtX . hummpur2b0 = rtP .
integrator_IC_gi1sbi2aqt ; rtDW . kidkiatkb2 = rtP .
Memory_InitialCondition_o0fwgur3ar ; rtX . j0rirqtd1a = rtP .
integrator_IC_me52n3144j ; rtDW . oqqwr15jx2 = rtP .
Memory_InitialCondition_l4ghnonlqr ; rtX . ip0opk45xq = rtP .
integrator_IC_lktx5vj3tu ; rtDW . dgche5odby = rtP .
Memory_InitialCondition_pi55lpoql3 ; rtX . cyudzpjr5t = rtP .
integrator_IC_j3zxtv20vm ; rtDW . olgfgh4cjq = rtP .
Memory_InitialCondition_g4b3tp4tqm ; rtX . ky01z2j0r2 = rtP .
integrator_IC_pxp35fixv0 ; rtDW . mokcdmtlad = rtP .
Memory_InitialCondition_ofyftsvzr4 ; rtX . nyvh4po1ze = rtP .
integrator_IC_bxxnsmxgnp ; rtDW . iosgfiuyq5 = rtP .
Memory_InitialCondition_ap0kypc5sf ; rtX . fsa5rzt3eu = rtP .
integrator_IC_lwrohbrhkp ; rtDW . e5wjbjcaxs = rtP .
Memory_InitialCondition_hwvdy0ygdq ; rtX . iivfgrp0dv = rtP .
integrator_IC_b12exydto5 ; rtDW . g2sm2t4hw5 = rtP .
Memory_InitialCondition_an2i1wqcxb ; rtX . etzwqxlfek = rtP .
integrator_IC_juxcfu1p4f ; rtDW . lva0bwt5gb = rtP .
Memory_InitialCondition_dpgpyfezow ; rtX . ikersjp5rv = rtP .
integrator_IC_ezteytelu3 ; rtDW . noyx1erxkm = rtP .
Memory_InitialCondition_linz5yhaqs ; rtX . hgyqbdifk0 = rtP .
integrator_IC_f01bmxgpuo ; rtDW . jsqsb2a31o = rtP .
Memory_InitialCondition_bbcvpzeqwz ; rtX . brjhw0iflo = rtP .
integrator_IC_p4lo0oegoz ; rtDW . krkllgqfro = rtP .
Memory_InitialCondition_kd2zppo5v1 ; rtX . kerqyn5qft = rtP .
integrator_IC_d2j0bqrmyd ; rtDW . bjpjcjhpzj = rtP .
Memory_InitialCondition_k0dvfmfxeq ; rtX . iytiwtpacb = rtP .
integrator_IC_nd4k01ihjn ; rtDW . a5gjxntn1h = rtP .
Memory_InitialCondition_j2v4kb5yub ; rtX . g0phs0m0dn = rtP .
integrator_IC_m0aduqmykx ; rtDW . i3uldok0qn = rtP .
Memory_InitialCondition_l0hq1gve31 ; rtX . lka0ltzwzh = rtP .
integrator_IC_d4ppld5qkj ; rtDW . a1ejgtlf3z = rtP .
Memory_InitialCondition_n2tpdjvny0 ; rtX . pquw0pmaw2 = rtP .
integrator_IC_bi53bxdauh ; rtDW . ihf24wpydd = rtP .
Memory_InitialCondition_n4ieyg30uw ; rtX . jx303cpogv = rtP .
integrator_IC_pzedstdge2 ; rtDW . jykznszm3q = rtP .
Memory_InitialCondition_cyoet2snva ; rtX . ltxxvmnz50 = rtP .
integrator_IC_gkasj3uadi ; rtDW . kwauldso4g = rtP .
Memory_InitialCondition_ajg0wygk5a ; rtDW . bw41vj1a5t = rtP .
u4_InitialCondition ; rtDW . cfe2132dnu = rtP .
u4_InitialCondition_kjtbkr3gve ; rtDW . gvkoorm242 = rtP . u_InitialCondition
; rtDW . lxxzsohnb1 = rtP . u_InitialCondition_nvgttkifjo ; rtDW . duld1iija4
= rtP . u_InitialCondition_aeriqwypjx ; rtDW . j5clkw2ofw = rtP .
u_InitialCondition_o5wj1fybyf ; rtDW . ht5aj3noet = rtP .
u_InitialCondition_gvdg00i3si ; rtDW . b54hv4iwx0 = rtP .
u4_InitialCondition_ov5ihknxpx ; rtDW . geqzl3jlyf = rtP .
u1_InitialCondition ; rtDW . bjzszecxxc = rtP .
Memory_InitialCondition_iyiv4hqij0 ; rtDW . l2ogv2jkyi = rtP .
u4_InitialCondition_ddjho25xih ; rtDW . ike0qfti5p = rtP .
Memory_InitialCondition_olebel1pck ; rtDW . m1mkpcefd4 = rtP .
u4_InitialCondition_inisizq21h ; rtX . b4ijv0yfdl = rtP .
integrator_IC_dlcwm45d2w ; rtDW . j0fgvrdpz3 = rtP .
Memory_InitialCondition_mabxyvnacx ; rtX . inx2whmsvz = rtP .
integrator_IC_bmptnnvp0v ; rtDW . cvvtotyqir = rtP .
Memory_InitialCondition_jddsmv4ufc ; rtX . obmdbjtn3e = rtP .
integrator_IC_cgjqleezzr ; rtDW . bbxfzyjpg4 = rtP .
Memory_InitialCondition_dhvwdgdeyk ; rtX . mczl1h5kue = rtP .
integrator_IC_m2wpawb2l1 ; rtDW . fuwywwq3yv = rtP .
Memory_InitialCondition_n1nhkexzoc ; rtX . b2kr0ggmvo = rtP .
integrator_IC_pbzklyhxun ; rtDW . gd2gr2srvr = rtP .
Memory_InitialCondition_oaxjrnzazs ; rtX . pdrwmhpgu2 = rtP .
integrator_IC_jsrlbq00ve ; rtDW . mumecnxvzw = rtP .
Memory_InitialCondition_ngvyn3dqvy ; rtX . ckadbag1nz = rtP .
integrator_IC_cjrde0urbb ; rtDW . md3bj5wruc = rtP .
Memory_InitialCondition_jmdctyejlu ; rtX . kfqga2xqnc = rtP .
integrator_IC_o1lpuromvh ; rtDW . hqezuuwqi2 = rtP .
Memory_InitialCondition_kqksiddjai ; rtX . ovnjy3004d = rtP .
integrator_IC_nlxoh4kwln ; rtDW . memzqzyaek = rtP .
Memory_InitialCondition_obvpizoiqa ; rtX . koitoczsgs = rtP .
integrator_IC_nd3ryuq1l5 ; rtDW . ffusyfyneb = rtP .
Memory_InitialCondition_om420erbb0 ; rtX . fwvdemyybn = rtP .
integrator_IC_p4qtko3c05 ; rtDW . bd4vt3ybt5 = rtP .
Memory_InitialCondition_moitkhfvif ; rtX . df1cmyigny = rtP .
integrator_IC_o33pj2ljvh ; rtDW . kmjfdch5hk = rtP .
Memory_InitialCondition_dhekgi2ktc ; rtX . eiq3cpodg4 = rtP .
integrator_IC_ds2bafz53z ; rtDW . oxztpbydsz = rtP .
Memory_InitialCondition_gsusp0ukz2 ; rtX . lujnb1vhch = rtP .
integrator_IC_fpnf4oq2vz ; rtDW . ol40kfwb3x = rtP .
Memory_InitialCondition_gqwot1yfzz ; rtX . ivshzrdzao = rtP .
integrator_IC_f24w45tro4 ; rtDW . eefsnpxkpz = rtP .
Memory_InitialCondition_iczinmpgrp ; rtX . b0se24ah4v = rtP .
integrator_IC_bngxvu0psv ; rtDW . j0cvdfbqu0 = rtP .
Memory_InitialCondition_kdv4pmkqua ; rtX . dzlchxjgwf = rtP .
integrator_IC_k141nz3bdy ; rtDW . fntnt2vkft = rtP .
Memory_InitialCondition_p2hh5dufer ; rtX . kxjviilbiq = rtP .
integrator_IC_cnkcmvia2m ; rtDW . dmpkou34np = rtP .
Memory_InitialCondition_pa51cquz0u ; rtX . kzm5j4om1j = rtP .
integrator_IC_lalutls5ky ; rtDW . ntlbbe2zlh = rtP .
Memory_InitialCondition_ajktcxuvfb ; rtX . hrkjiy2wbe = rtP .
integrator_IC_ingw3hisya ; rtDW . k2ecgqxawh = rtP .
Memory_InitialCondition_hlydntael2 ; rtX . bbxcrevmf2 = rtP .
integrator_IC_n4zvujvj1z ; rtDW . fbmro5ygjf = rtP .
Memory_InitialCondition_pzzialhtyv ; rtX . mecmfgexic = rtP .
integrator_IC_ps2xpe2y0r ; rtDW . nfnlqxj2g5 = rtP .
Memory_InitialCondition_pbyrgswukg ; rtDW . fzwqkwp2m1 = rtP .
u4_InitialCondition_ab2lq0gail ; rtDW . jvvuz2zzbv = rtP .
u4_InitialCondition_h2zhjwe4vo ; rtDW . dsi2mx1crg = rtP .
Memory_InitialCondition_kwstslbpbc ; rtDW . p0tzcauox5 = rtP .
u_InitialCondition_bghgumz4jr ; rtDW . cjcrhlf0g0 = rtP .
u_InitialCondition_fupwc03h5e ; rtDW . a3t0rfsk4v = rtP .
u_InitialCondition_okaoilymni ; rtDW . fj1oj1qia0 = rtP .
u_InitialCondition_jgmlyb321d ; rtDW . mpcjp04f1l = rtP .
u_InitialCondition_ldhihdzdws ; rtDW . f5mcliqi4f = rtP .
u4_InitialCondition_o5zqqfjh5x ; rtDW . pybydd0eqj = rtP .
u1_InitialCondition_dvbk5mnf12 ; rtDW . j54aiqlmrr = rtP .
Memory_InitialCondition_gzp4lp5uyy ; rtDW . gokujearn0 = rtP .
u2_InitialCondition ; rtDW . f23jtcpi0e = rtP .
Memory_InitialCondition_gvmexlhsef ; rtDW . ih0vhsirfu = rtP .
Memory_InitialCondition_fg1ecrmge2 ; rtDW . buycl0ri4c = rtP . u_ic ; rtDW .
klktcnq5ax = rtP . SampleandHold_ic ; rtDW . dgygcznzfp = rtP .
u4_InitialCondition_lpjiwdkfxq ; rtDW . hvjbgpscln = rtP .
Memory_InitialCondition_asdwh2vb2z ; rtDW . afrib4g3ns = rtP .
u_InitialCondition_mfsq4ttebz ; rtDW . jlbauzygas = rtP .
u_InitialCondition_p0chbdm5aj ; rtDW . n2ncp1ufol = rtP .
u_InitialCondition_dizuulobjq ; rtDW . jzckq3ap3v = rtP .
u_InitialCondition_pzamt3yuj2 ; rtDW . muro5i15oa = rtP .
u_InitialCondition_kjhqmtxbyn ; rtDW . h2djmvry3k = rtP .
u4_InitialCondition_ckgep1pekp ; rtDW . oicbkbao2v = rtP .
u1_InitialCondition_et4gghxofz ; rtDW . hs2gbbrj20 = rtP .
Memory_InitialCondition_h04tc20sdq ; rtDW . fnftlbxl0k = rtP .
u_InitialCondition_jtibs5ulft ; rtDW . as0d4xo4un = rtP .
u_InitialCondition_jsv3pqyjxj ; rtDW . dymvp4turd = rtP .
u_InitialCondition_awdxqvdvh0 ; rtDW . evvyvdybbq = rtP .
u_InitialCondition_abq1kflc5n ; rtDW . ct3p3arebc = rtP .
u_InitialCondition_pgvu1x1wn4 ; rtDW . evfvx3mdmk = rtP .
u4_InitialCondition_msfjg2y1zb ; rtDW . kn1fi20kkc = rtP .
u1_InitialCondition_pirdvi51ds ; rtDW . aomedx5xd0 = rtP .
Memory_InitialCondition_d4amfaaqhb ; rtDW . f3aybkapro = rtP .
u2_InitialCondition_h44amwofxg ; rtDW . g4kyas3bbl = rtP .
Memory_InitialCondition_lwd4dfxgha ; rtDW . ememr0qwkc = rtP .
Memory_InitialCondition_kxbsnpne35 ; rtDW . jirvvhrkai = rtP .
u_ic_c1ara31rjn ; rtDW . insgv4zthl = rtP . SampleandHold_ic_cjfarvicnp ;
rtDW . fwreysljhu = rtP . u4_InitialCondition_pa2xpwfwt0 ; rtDW . gvtu2sgvui
= rtP . Memory_InitialCondition_bx1ipl3bwo ; rtDW . igkazskzyf = rtP .
u_InitialCondition_ea3u21fvgi ; rtDW . fer4rqlapl = rtP .
u_InitialCondition_gliyktjxra ; rtDW . lczlugrqza = rtP .
u_InitialCondition_hlyx2lanrs ; rtDW . fkz4l3azou = rtP .
u_InitialCondition_a4515sizsk ; rtDW . hrqcp0us0o = rtP .
u_InitialCondition_clj2s3gf2h ; rtDW . ji2nko31lt = rtP .
u4_InitialCondition_jr1nddjq2b ; rtDW . cqrjo5bott = rtP .
u1_InitialCondition_g5obaexm4n ; rtDW . ndo55xdrjz = rtP .
Memory_InitialCondition_de0qdqbtzu ; rtDW . pxt53ienu3 = rtP .
u_InitialCondition_nmhayyiq1g ; rtDW . auxttyil24 = rtP .
u_InitialCondition_nh2lx5aonc ; rtDW . nd3ykklatb = rtP .
u_InitialCondition_mpgij21v4u ; rtDW . aowojzitws = rtP .
u_InitialCondition_hfjuzweb2s ; rtDW . gubginzy5v = rtP .
u_InitialCondition_l4ropaypxz ; rtDW . emo0onv4iy = rtP .
u4_InitialCondition_goor5gf4m5 ; rtDW . oevsqktqdh = rtP .
u1_InitialCondition_mcm4mo1xzg ; rtDW . h2duquj31a = rtP .
Memory_InitialCondition_cbetyrsyyd ; rtDW . ikejccrfw4 = rtP .
u2_InitialCondition_n515uqdjpr ; rtDW . fyqrim4x4y = rtP .
Memory_InitialCondition_igrxi4ft3h ; rtDW . lw0ubiboa0 = rtP .
Memory_InitialCondition_m42u5oyy4p ; rtDW . odfrq0vcqw = rtP .
u_ic_ayrimaqqy3 ; rtDW . hlxu2b4hfp = rtP . SampleandHold_ic_nlgokv0lbv ;
rtDW . nyqadzpf4p = rtP . u4_InitialCondition_cjpd2uptes ; rtDW . crlu0ruqew
= rtP . Memory_InitialCondition_gkbhbgkqia ; rtDW . k1pkt3do0q = rtP .
Memory_InitialCondition_oslnapx4n5 ; rtDW . puvpzy5tok = rtP .
Memory_InitialCondition_bzeaazz3fl ; rtDW . ci14pxvpr2 = rtP .
u4_InitialCondition_pfmd01ceef ; rtDW . jaz3uqfgz2 = rtP .
Memory_InitialCondition_i33m1vxtxq ; rtDW . d4pejtpczl = rtP .
Memory_InitialCondition_pcecqc02ah ; rtDW . aj2otrttnf = rtP .
u4_InitialCondition_mlrl5iy2qq ; rtDW . euvp233z54 = rtP .
Memory_InitialCondition_dza31jynwt ; rtDW . bpckxrqz0a = rtP .
Memory_InitialCondition_maxlfuyaua ; rtDW . laigsna1ry = rtP .
u4_InitialCondition_ie1304ztve ; rtDW . gtdowlniha = rtP .
Memory_InitialCondition_a5lo33p0ri ; rtDW . md3pjrrcpi = rtP .
u2_InitialCondition_a2yn4wjs3z ; rtDW . maqoyqobhp = rtP .
u4_InitialCondition_nlbe20l210 ; rtDW . hwukdcvf00 = rtP .
Memory_InitialCondition_fwxsfyqkio ; rtDW . ip1qcpepxp = rtP .
u2_InitialCondition_joshor3sjf ; rtDW . pxb20wrhli = rtP .
u4_InitialCondition_mituig1scw ; rtDW . gtyu3k2ot5 = rtP .
Memory_InitialCondition_gep5dqwrna ; rtDW . cm1jdei3jr = rtP .
u2_InitialCondition_nukbipml2s ; rtDW . aj21ow2g22 = rtP .
Memory_InitialCondition_maroiajibt ; rtDW . k344xgbfw1 = rtP .
Memory_InitialCondition_n15g5twcdx ; rtDW . equ3xylxak = rtP .
u_ic_k1rukmqbfq ; rtDW . p35tk2ttns = rtP . SampleandHold_ic_bffjzd2rer ;
rtDW . b55d2d4pem = rtP . Memory_InitialCondition_pphs0cxovi ; rtDW .
h5iqn1tf0f = rtP . Memory_InitialCondition_a4dhezt0gs ; rtDW . iabw2jrkb0 =
rtP . u_ic_nx43plxpey ; rtDW . mzqdovgi1x = rtP . SampleandHold_ic_e05r3q5ufm
; rtDW . i4uxbnvgsv = rtP . Memory_InitialCondition_ac5xr1b1to ; rtDW .
bmkmerzuty = rtP . Memory_InitialCondition_psbccfu5r4 ; rtDW . idmf1p4vkt =
rtP . u_ic_cegkl0mlof ; rtDW . pwnzz0enea = rtP . SampleandHold_ic_p1wtrlxcup
; rtDW . of54iatyrl = rtP . Memory_InitialCondition_jn3swib3k5 ; rtDW .
lgg4cwcbcv = rtP . u4_InitialCondition_f2lo3z1dvi ; rtDW . ldsl4h1b2m = rtP .
Memory_InitialCondition_ooyq0nphmi ; rtDW . fmwrpqnbm1 = rtP .
u4_InitialCondition_byffosdcno ; rtDW . lltb10cjgb = rtP .
Memory_InitialCondition_gshpeyrqdp ; rtDW . f3l0hj1q5z = rtP .
u_InitialCondition_efcxjfazbw ; rtDW . gwjqd3rg01 = rtP .
u_InitialCondition_aaqz1lguch ; rtDW . jn3q0bmaxx = rtP .
u_InitialCondition_d4rp04qex4 ; rtDW . mpw5qp3zhv = rtP .
u_InitialCondition_lhbkxdncsi ; rtDW . e20bjtoooq = rtP .
u_InitialCondition_btpsezsyyu ; rtDW . iphvfgl3lu = rtP .
u_InitialCondition_o0dvdcwjgi ; rtDW . a3qbrcdsa4 = rtP .
u_InitialCondition_egm3ixynuq ; rtDW . hrvs2bhnce = rtP .
u_InitialCondition_e230fbizcn ; rtDW . pnnzfpsb2c = rtP .
u_InitialCondition_bwfssszkmv ; rtDW . jgoshqlbir = rtP .
u_InitialCondition_mucssx42kz ; rtDW . jcjg4wdcj2 = rtP .
u_InitialCondition_l1gtzf5eym ; rtDW . gylvz5hxsg = rtP .
u_InitialCondition_kerwaotc4s ; rtDW . gyn5krs1zz = rtP .
u_InitialCondition_eme5ihk1wv ; rtDW . jwnv52k1wb = rtP .
u_InitialCondition_f2hzqjdiqe ; rtDW . mbo2opl3eu = rtP .
u_InitialCondition_bvgdnqwnvx ; rtDW . c3jeodupfc = rtP .
u_InitialCondition_ezxubwrlwn ; rtDW . giowad3sec = rtP .
u_InitialCondition_jup4x4vvoh ; rtDW . md4snt4zsk = rtP .
u_InitialCondition_n4u1rzbml2 ; rtDW . i1pprdx5pp = rtP .
u_InitialCondition_dz4oeidtmf ; rtDW . f2nxapnymz = rtP .
u_InitialCondition_pdyynufpfh ; rtDW . autyikzmna = rtP .
u_InitialCondition_ctjcfmres2 ; rtDW . n3upjnxoej = rtP .
u_InitialCondition_eb2pmqwnx4 ; rtDW . i0enxeox0z = rtP .
u_InitialCondition_a41msmalno ; rtDW . ifgv45o1io = rtP .
u_InitialCondition_iuzbb4m51g ; rtDW . g1pk0bzvtv = rtP .
u_InitialCondition_lvk303xylp ; rtDW . ah5hnpnyx1 = rtP .
u_InitialCondition_pxmcxx5dzf ; rtDW . gry5zwpbhu = rtP .
u_InitialCondition_nekrosfkcw ; rtDW . ptqaeanwg0 = rtP .
u_InitialCondition_fjoftlftvw ; rtDW . iug5i33rtw = rtP .
u_InitialCondition_obqgprpunc ; rtDW . mdvftfecjd = rtP .
u_InitialCondition_cw3tffbau4 ; rtDW . fufczbmjix = rtP .
u_InitialCondition_bmqphspwy4 ; rtDW . dxi5yvdasd = rtP .
u_InitialCondition_kz2zpbcyck ; rtDW . eowtljwki4 = rtP .
u_InitialCondition_iigbeq1atx ; rtDW . l4egugzm4w = rtP .
u_InitialCondition_flg2f5yj4c ; rtDW . byq22ip4jp = rtP .
u_InitialCondition_jru2n5qp0i ; rtDW . cm5vdxv24a = rtP .
u_InitialCondition_hasri2gcqn ; rtDW . flh5edlegj = rtP .
u_InitialCondition_f20p1lxvg1 ; rtDW . pxv2xpdkad = rtP .
u_InitialCondition_aaa5pkto10 ; rtDW . f4thttu240 = rtP .
u_InitialCondition_dubbwumkvz ; rtDW . jgrmmzecam = rtP .
u_InitialCondition_kfcacm4o2q ; rtDW . cjg142fgy0 = rtP .
u_InitialCondition_nktzmn1mn5 ; rtDW . eteblkpynn = rtP .
u_InitialCondition_gmj1aflror ; rtDW . pnthkernke = rtP .
u_InitialCondition_kxcjp5zjnl ; rtDW . nihpinzhrt = rtP .
u_InitialCondition_afifz0tmra ; rtDW . p3uq2rdn1i = rtP .
u_InitialCondition_gq2kektlza ; rtDW . nh3lnfbigq = rtP .
u_InitialCondition_neoj0ilywu ; rtDW . dzo14tsqn4 = rtP .
u_InitialCondition_f15aw0vl0a ; rtDW . gcy3ctyywg = rtP .
u_InitialCondition_g5mujvn4jj ; rtDW . i5a0jb1dk3 = rtP .
Memory_InitialCondition_fswbvyf12h ; rtDW . guzsz3fvfs = rtP .
u4_InitialCondition_amuqipxaia ; rtDW . az2oeo1f4o = rtP .
Memory_InitialCondition_n4ygh4nkeq ; rtDW . cij1nabjqb = rtP .
u4_InitialCondition_girhinqtuf ; rtDW . f2rohdyfa1 = rtP .
u4_InitialCondition_g01kfddvnm ; rtDW . ie01l5vavi = rtP .
Memory_InitialCondition_nclpphcwfm ; rtDW . nwhg3zqrsl = rtP .
Memory_InitialCondition_mptzfoaiq0 ; rtDW . mv54qvcn5q = rtP .
u_InitialCondition_nmmiho2d45 ; rtDW . fjxk3uaez4 = rtP .
u_InitialCondition_i14zw0mca0 ; rtDW . i21ukrhdhv = rtP .
u_InitialCondition_m1d2mff55g ; rtDW . eu3dowbkcr = rtP .
u_InitialCondition_jonqn4k2fp ; rtDW . oxmyy55gc0 = rtP .
u_InitialCondition_ei4z2byqxq ; rtDW . abpj3txsdh = rtP .
u_InitialCondition_pe5vqjzoml ; rtDW . e0oousi5de = rtP .
u_InitialCondition_acgqkabzby ; rtDW . a0eu1zkv1e = rtP .
u_InitialCondition_bpwt04vukn ; rtDW . n1z1agzu3x = rtP .
u_InitialCondition_ei1wgko2ji ; rtDW . nngs4wq1em = rtP .
u_InitialCondition_neuctrxoqq ; rtDW . aynri5lox1 = rtP .
u_InitialCondition_e4plmvlolt ; rtDW . lmvrvbjthf = rtP .
u_InitialCondition_oisvfmjxuf ; rtDW . bxrofmksin = rtP .
u_InitialCondition_hhhcb3igj4 ; rtDW . btgc2mhzkb = rtP .
u_InitialCondition_bswlzefakb ; rtDW . o55xttyljm = rtP .
u_InitialCondition_k1cnu0fqpm ; rtDW . ev3rm05gtu = rtP .
u_InitialCondition_byow3n2epj ; rtDW . e1zljawh5b = rtP .
u_InitialCondition_l5eoy4h2fw ; rtDW . dv30vpwozb = rtP .
u_InitialCondition_i1zlt2vo4g ; rtDW . fe4qsttuig = rtP .
u_InitialCondition_dzonkx2lgg ; rtDW . gijbul5bro = rtP .
u_InitialCondition_hkrfslqvd3 ; rtDW . fx2ubbeual = rtP .
u_InitialCondition_ca04dndh5d ; rtDW . kdodmy0lc5 = rtP .
u_InitialCondition_i3omzp5n5w ; rtDW . p2lcbu1i1l = rtP .
u_InitialCondition_iek4rcpdyx ; rtDW . cstjzr5zdb = rtP .
u_InitialCondition_i1154sujwt ; rtDW . o5i22qdh2m = rtP .
u_InitialCondition_lskpr1qj4y ; rtDW . nhuixzwcbu = rtP .
u_InitialCondition_i1zngtf2jc ; rtDW . k4yt0gbyvq = rtP .
u_InitialCondition_gbzjyrn0as ; rtDW . hzgwuaj0dk = rtP .
u_InitialCondition_cuk4n2rnv2 ; rtDW . p2kf00znzb = rtP .
u_InitialCondition_akrcdewgqn ; rtDW . mitpuusdiu = rtP .
u_InitialCondition_jv10sp10rb ; rtDW . gl5n5tgtur = rtP .
u_InitialCondition_awaryn3bpp ; rtDW . gm3eatvpbt = rtP .
u_InitialCondition_fknsoaxrhx ; rtDW . faqnzz3umd = rtP .
u_InitialCondition_f2ps50ebqh ; rtDW . iy0b4tzwiz = rtP .
u_InitialCondition_iugde3lnnv ; rtDW . dpmou3nofz = rtP .
u_InitialCondition_ogw1uxlvoy ; rtDW . oad2ixcyjj = rtP .
u_InitialCondition_hana11wsvj ; rtDW . mwwno0g4te = rtP .
u_InitialCondition_aaeydu03gg ; rtDW . laawtmho0w = rtP .
u_InitialCondition_pc4jpf3i1y ; rtDW . aifuwgprs1 = rtP .
u_InitialCondition_jyfsiiibo0 ; rtDW . j2ytkxdub5 = rtP .
u_InitialCondition_pavqvni2al ; rtDW . efpieuuisx = rtP .
u_InitialCondition_jzyrvpswlz ; rtDW . gnbbybfox4 = rtP .
u_InitialCondition_nalpdh00mb ; rtDW . inaoot3v53 = rtP .
u_InitialCondition_dejhelonpg ; rtDW . kzrubw1lpo = rtP .
u_InitialCondition_cu0p444cep ; rtDW . aqriwgcdaa = rtP .
u_InitialCondition_lrmkhixfid ; rtDW . akfyoog544 = rtP .
u_InitialCondition_ig0lfj5e3j ; rtDW . dloemu5bxh = rtP .
u_InitialCondition_d5g0mfjvac ; rtDW . llzhlzgbyq = rtP .
u_InitialCondition_f1rcghhjjz ; rtDW . gk45nsnm5f = rtP .
Memory_InitialCondition_oyrdmyt40g ; rtDW . bd5awmqnm1 = rtP .
u4_InitialCondition_mfihfw3ltx ; rtDW . a0zxdwo0kv = rtP .
Memory_InitialCondition_imif1yfxnc ; rtDW . cgxfb5ilba = rtP .
u2_InitialCondition_cpwoxaiac5 ; rtDW . jyeacneoju = rtP .
Memory_InitialCondition_gczferxazb ; rtDW . i5d0s3lepy = rtP .
Memory_InitialCondition_h4qhpnrl3c ; rtDW . pw1qqgmx0o = rtP .
u_ic_cgmjyslzmg ; rtDW . ly0wgshoe2 = rtP . SampleandHold_ic_fzkkmviyao ;
rtDW . osx1vowkj4 = rtP . Memory_InitialCondition_ncuwc0oixq ; rtDW .
l2fddlwysw = rtP . u4_InitialCondition_ix5epk1b4n ; rtDW . d3dlbfiuuv = rtP .
Memory_InitialCondition_d5uznsqv0u ; rtDW . odjstwuwx3 = rtP .
u2_InitialCondition_ob04fxgz10 ; rtDW . lmsqqojpvs = rtP .
Memory_InitialCondition_l44kliziar ; rtDW . kpsmnp3ug0 = rtP .
Memory_InitialCondition_agv25ln0nr ; rtDW . ovtmlyjrxp = rtP .
u_ic_l3vl1etnj2 ; rtDW . fbjudlgv4x = rtP . SampleandHold_ic_bjfrhx0myz ;
rtDW . gmqawd5msq = rtP . Memory_InitialCondition_nsyincez0b ; rtDW .
kegqko1pyk = rtP . Memory_InitialCondition_fibpognhv2 ; rtDW . crgr0yyjst =
rtP . Memory_InitialCondition_khmn2umyui ; rtDW . ex4vqumvao = rtP .
Memory_InitialCondition_hlhywqax4b ; rtDW . nbtfsiobop = rtP .
u_ic_lljusyppbu ; rtDW . jl4tnonvue = rtP . SampleandHold_ic_fxk2nzwlse ;
rtDW . m3pubmd011 = rtP . Memory_InitialCondition_mugvvm0bre ; rtDW .
fvjg0rzhqa = rtP . Memory_InitialCondition_img4ta4xvk ; rtDW . h0ue4eifzz =
rtP . u_ic_nht4dberhz ; rtDW . dxcijq0izx = rtP . SampleandHold_ic_gnsy1kf2lz
; rtDW . elckdgh4b5 = rtP . Memory_InitialCondition_hnqix3icis ; rtDW .
e4su0h1vb4 = rtP . Memory_InitialCondition_jovfg242wa ; rtDW . j2nec0kpix =
rtP . Memory_InitialCondition_ghgpl40jsu ; rtDW . kf53zbkqlj = rtP .
Memory_InitialCondition_ezvit2dk5p ; rtDW . ne0hnild1t = rtP .
Memory_InitialCondition_n40knqg4qa ; rtDW . mnr1ns2sul = rtP .
Memory_InitialCondition_hqvytlmilo ; rtDW . ozt4gwuaev = rtP .
Memory_InitialCondition_ktppd45r0g ; rtDW . e11plvgt2e = rtP .
Memory_InitialCondition_pn2hknupfb ; rtDW . davrjeeiee = rtP .
Memory_InitialCondition_fl22pyhq3j ; rtDW . avb4nhw551 = rtP .
Memory_InitialCondition_boynokwxm4 ; rtDW . fevmalljs2 = rtP .
Memory_InitialCondition_f32wf3414z ; rtDW . b0vjxwd4dx = rtP .
u_ic_km2deo5ylt ; rtDW . fpax42xxm5 = rtP . SampleandHold_ic_lhyomdqmvz ;
rtDW . pflycifi2v = rtP . Memory_InitialCondition_g5n3jyje5i ; rtDW .
kja5xrgmwv = rtP . Memory_InitialCondition_d5on21razb ; rtDW . e3tarqgwys =
rtP . u_ic_eoeulbvpj4 ; rtDW . hltxj0u51z = rtP . SampleandHold_ic_kbqyy1icf4
; rtDW . hlihecl4y1 = rtP . Memory_InitialCondition_myud52jas0 ; rtDW .
ifc1hwj1ct = rtP . Memory_InitialCondition_jzskvp20lk ; rtDW . bdb0pmfh2i =
rtP . u_ic_eq05vemvf0 ; rtDW . lb3vrehkjt = rtP . SampleandHold_ic_o40bmzkt40
; rtDW . fra1s15lot = rtP . Memory_InitialCondition_cxvz0yt112 ; rtDW .
pvwn2twq14 = rtP . Memory_InitialCondition_it43tf2rd4 ; rtDW . fwvi3ktkgn =
rtP . Memory_InitialCondition_oseupc4d5m ; rtDW . danlnozb3w = rtP .
Memory_InitialCondition_n3qug3gsyt ; rtDW . dglgxggn0s = rtP .
Memory_InitialCondition_m4lu1tophn ; rtDW . levorifkrw = rtP .
Memory_InitialCondition_fdrndqrn2b ; rtDW . hxmfwxg20s = rtP .
Memory_InitialCondition_o1okdtfajd ; rtDW . hdod2qacjn = rtP .
Memory_InitialCondition_ofhqugx2ew ; rtDW . k11kfdk1be = rtP .
Memory_InitialCondition_m0zxn0o3wk ; rtDW . bdb5r4moyp = rtP .
Memory_InitialCondition_oynrrx40vq ; rtDW . mhkbaz10cz = rtP .
Memory_InitialCondition_ovcnvroib4 ; rtDW . f5dfmvvs02 = rtP .
u4_InitialCondition_c3uwyqh1tp ; rtDW . ecq2t4tug1 = rtP .
Memory_InitialCondition_i5y1kwp0r2 ; rtX . gukucggul0 = 0.0 ; rtX .
cooihtzhhj = 0.0 ; rtDW . ph2qy0rwkn = rtP .
Memory_InitialCondition_leja2kewte ; rtDW . ojkg1k5daw = rtP .
Memory_InitialCondition_cqepdfg2cj ; rtDW . a0uzfqyhcp = rtP .
u_ic_d1yxthyrb3 ; rtDW . invgjx5x0c = rtP . SampleandHold_ic_kq04g3xvul ;
rtDW . jwthvsveef = rtP . Memory_InitialCondition_eqqc3tzhss ; rtDW .
k3ksd4k1ib = rtP . u2_InitialCondition_oroy1ixasi ; rtDW . gbzb4i3qdq = rtP .
Memory_InitialCondition_jpak2ppouu ; rtDW . gkn55q342j = rtP .
Memory_InitialCondition_andpq5e531 ; rtDW . njpxzlmy40 = rtP .
u4_InitialCondition_f2ni1wnxuu ; rtDW . f4s0lo0uob = rtP .
Memory_InitialCondition_ghatnimsod ; rtDW . mls4swjwmm = rtP .
u2_InitialCondition_parkxpipvt ; rtDW . kguu1igm24 = rtP .
u4_InitialCondition_oho1q11rfi ; rtDW . l4mjhh4eud = rtP .
Memory_InitialCondition_ktrlgy54i4 ; rtDW . nqy0vnt1sc = rtP .
u2_InitialCondition_hnyc4i3au4 ; rtDW . hhnsho4bie = rtP .
u4_InitialCondition_eywlavvztx ; rtDW . goafipr0ei = rtP .
Memory_InitialCondition_pqghfljqoa ; rtDW . jbzxllbqx1 = rtP .
u2_InitialCondition_plxbeaixj1 ; rtDW . hdv5m3k0vy = rtP .
Memory_InitialCondition_bx1ocuocuf ; rtDW . lz1cv0do2t = rtP .
Memory_InitialCondition_b4t0dtufw4 ; rtDW . cgbiv0cyqn = rtP .
u_ic_hfi0k0vrc3 ; rtDW . kvmhfywt50 = rtP . SampleandHold_ic_kdzrpxwl1l ;
rtDW . lsieos4skt = rtP . Memory_InitialCondition_oj0lsp1uv5 ; rtDW .
edyvvi3ty4 = rtP . Memory_InitialCondition_dtf23g1gco ; rtDW . hvm2hswn02 =
rtP . u_ic_lze1uspjc2 ; rtDW . ppskp5kzs0 = rtP . SampleandHold_ic_pva5qzpxrl
; rtDW . jurv4x4dxh = rtP . Memory_InitialCondition_otoarvlqtk ; rtDW .
n3llciapkl = rtP . Memory_InitialCondition_g3rs3qwz5n ; rtDW . ld1xzt33jz =
rtP . u_ic_nizgc3vew4 ; rtDW . fsgvohzvsa = rtP . SampleandHold_ic_he3crnv4qu
; rtDW . amjwljpykx = rtP . Memory_InitialCondition_efblf1i5vs ; rtDW .
keetxs0kf4 = rtP . Memory_InitialCondition_m3qayma1e4 ; rtDW . cauqpp1pbg = 1
; if ( ssIsFirstInitCond ( rtS ) ) { rtX . hbdbwheonc = 0.0 ; tmp =
slIsRapidAcceleratorSimulating ( ) ; if ( tmp ) { tmp =
ssGetGlobalInitialStatesAvailable ( rtS ) ; rtDW . cauqpp1pbg = ! tmp ; }
else { rtDW . cauqpp1pbg = 1 ; } } rtX . oipozxyfbt = rtP . Continuous_Init ;
rtX . nrjwdf4bl5 = 0.0 ; rtX . d543hn3jm3 = rtP . integrator_IC_dtuyihcf0p ;
rtDW . niklp3ydpx = rtP . Memory_InitialCondition_adhjeubj1q ; rtX .
ktq1qocrqs = 0.0 ; rtDW . oxppwiig31 = ( rtInf ) ; rtDW . idgf0xanwj = (
rtInf ) ; rtX . ih0sh50oxm = rtP . Integrator_x1_IC ; rtX . pn05gdwhsv = rtP
. Integrator_x2_IC ; rtX . jzyjyrpy3w = rtP . integrator_IC_n03suefgdk ; rtDW
. f3ocqnmitg = rtP . Memory_InitialCondition_cd5vuzt4dh ; rtX . ehyodyswyu =
rtP . integrator_IC_mqjg3drhyd ; rtDW . b1jopw25yj = rtP .
Memory_InitialCondition_l1l23vhb5m ; rtX . hyqpl142bi = rtP .
integrator_IC_fx5wr0awkd ; rtDW . ewe0bkor5i = rtP .
Memory_InitialCondition_jnzi2exqqi ; rtX . prno02pbsy = rtP .
integrator_IC_hxqgak0wvp ; rtDW . l5dlababv1 = rtP .
Memory_InitialCondition_etym4r24tx ; rtX . aztsfsn2hi = rtP .
integrator_IC_m2cmgzdlwp ; rtDW . b3csr0smh2 = rtP .
Memory_InitialCondition_ekezs5iqhg ; rtX . f04g4otdzz = rtP .
integrator_IC_bnj2h0pwuf ; rtDW . gdgisolefd = rtP .
Memory_InitialCondition_ettmvsyssi ; rtX . ouay2vk4ob = rtP .
integrator_IC_lcqnj3osio ; rtDW . fecdn2rq0x = rtP .
Memory_InitialCondition_mzxd1rwc3x ; rtX . l4w5fqeack = rtP .
integrator_IC_phlyepjg40 ; rtDW . am13fzyssx = rtP .
Memory_InitialCondition_lhgg23okjw ; rtX . dbk3vhy4fa = rtP .
integrator_IC_bz5mu0kuza ; rtDW . amun1qc2sg = rtP .
Memory_InitialCondition_divgyg2ydj ; rtX . bwyxfbkejz = rtP .
integrator_IC_iztvhyhfxr ; rtDW . dk54ih4kpw = rtP .
Memory_InitialCondition_bus52uohe1 ; rtX . cvhc14d1wd = rtP .
integrator_IC_leo3eyyfmk ; rtDW . loaprl0bph = rtP .
Memory_InitialCondition_dd42ksx5ag ; rtX . ipmtswi0si = rtP .
integrator_IC_ijd2m00yno ; rtDW . fo5lr1tooi = rtP .
Memory_InitialCondition_d34ekj04vq ; rtX . eczymfqr20 = rtP .
integrator_IC_hhofvkhx0e ; rtDW . g0siwihohw = rtP .
Memory_InitialCondition_otgah4tf0u ; rtX . lobhhamyoj = rtP .
integrator_IC_hruaxoqzu1 ; rtDW . atub5wqmcj = rtP .
Memory_InitialCondition_nxmdlpqrtv ; rtX . kenbvb3y0s = rtP .
integrator_IC_bn3mgrc21z ; rtDW . kzk400d43v = rtP .
Memory_InitialCondition_lenrreefce ; rtX . nsp4pw5vpk = rtP .
integrator_IC_nwf1u1mws3 ; rtDW . itgyxe4zdv = rtP .
Memory_InitialCondition_amsyyixfmk ; rtX . prtdiw223v = rtP .
integrator_IC_b225nqihed ; rtDW . jnixgbeum0 = rtP .
Memory_InitialCondition_p34vo210ou ; rtX . bots1ppmcj = rtP .
integrator_IC_gafg1hmaxk ; rtDW . c2b1i5ixoq = rtP .
Memory_InitialCondition_hp0lchy2vx ; rtX . n0uge3m4yf = rtP .
integrator_IC_d2hcyogdmn ; rtDW . oumxtk4xvn = rtP .
Memory_InitialCondition_kekizpcsev ; rtX . gnzuxd2raf = rtP .
integrator_IC_bd0n00dwd1 ; rtDW . cvewj2ww1t = rtP .
Memory_InitialCondition_l3w3r3yrbw ; rtX . bqo41zgbir = rtP .
integrator_IC_pldouacjij ; rtDW . gb2aoze2zq = rtP .
Memory_InitialCondition_pyhue1b3xr ; rtX . kiwleabf04 = rtP .
integrator_IC_cz4fcrrho3 ; rtDW . fxksrxtwby = rtP .
Memory_InitialCondition_dzxsc0ek02 ; rtX . aa5y12cea2 = rtP .
integrator_IC_idj4odo4gh ; rtDW . pwd5paigtu = rtP .
Memory_InitialCondition_jyfr43uchq ; rtX . cylrltmkae = rtP .
integrator_IC_nrshljnqqv ; rtDW . gaijbd3xgk = rtP .
Memory_InitialCondition_pdc33fd3mv ; rtDW . itxmk3chva = rtP .
u4_InitialCondition_ca4h51xumd ; rtX . ar5lw0nd1g = rtP .
integrator_IC_nymoqoham5 ; rtDW . n1ijw4x0rn = rtP .
Memory_InitialCondition_fcofgg3t5v ; rtX . etqlz3rohc = rtP .
integrator_IC_nw53hw5s4a ; rtDW . ciujz12ohu = rtP .
Memory_InitialCondition_ih4h00xtd0 ; rtDW . huxwzweeyi = rtP .
u4_InitialCondition_fp2slyh2az ; rtDW . fwpmmk23o2 = rtP .
Memory_InitialCondition_j4zun5313g ; mbb3rj5v0g ( & rtB . f2cfptydf3 , & rtDW
. f2cfptydf3 , & rtP . f2cfptydf3 , rtP . T1_ic ) ; p5hgyyqss4 ( & rtB .
mempnwchre , & rtDW . mempnwchre , & rtP . mempnwchre , rtP . T1_ic ) ;
mbb3rj5v0g ( & rtB . ceydu0zdw3 , & rtDW . ceydu0zdw3 , & rtP . ceydu0zdw3 ,
rtP . T2_ic ) ; p5hgyyqss4 ( & rtB . e05ysccjt5 , & rtDW . e05ysccjt5 , & rtP
. e05ysccjt5 , rtP . T2_ic ) ; mbb3rj5v0g ( & rtB . kbnljt2vok , & rtDW .
kbnljt2vok , & rtP . kbnljt2vok , rtP . T3_ic ) ; p5hgyyqss4 ( & rtB .
ffo1fpyqgq , & rtDW . ffo1fpyqgq , & rtP . ffo1fpyqgq , rtP . T3_ic ) ;
mbb3rj5v0g ( & rtB . nrt14kntrk , & rtDW . nrt14kntrk , & rtP . nrt14kntrk ,
rtP . T4_ic ) ; p5hgyyqss4 ( & rtB . nepgqug5c0 , & rtDW . nepgqug5c0 , & rtP
. nepgqug5c0 , rtP . T4_ic ) ; mbb3rj5v0g ( & rtB . inadvhzvdh , & rtDW .
inadvhzvdh , & rtP . inadvhzvdh , rtP . OffDelay_ic ) ; p5hgyyqss4 ( & rtB .
leagxggwwe , & rtDW . leagxggwwe , & rtP . leagxggwwe , rtP . OffDelay_ic ) ;
mbb3rj5v0g ( & rtB . a1srf3dmr2 , & rtDW . a1srf3dmr2 , & rtP . a1srf3dmr2 ,
rtP . T1_ic_mm0elcjvim ) ; p5hgyyqss4 ( & rtB . di10rsm5nz , & rtDW .
di10rsm5nz , & rtP . di10rsm5nz , rtP . T1_ic_mm0elcjvim ) ; mbb3rj5v0g ( &
rtB . pry2hnvt4q , & rtDW . pry2hnvt4q , & rtP . pry2hnvt4q , rtP .
T2_ic_mh4ffatebp ) ; p5hgyyqss4 ( & rtB . hgwrktvi4c , & rtDW . hgwrktvi4c ,
& rtP . hgwrktvi4c , rtP . T2_ic_mh4ffatebp ) ; mbb3rj5v0g ( & rtB .
apyjjt13q1 , & rtDW . apyjjt13q1 , & rtP . apyjjt13q1 , rtP .
T3_ic_jx43e1q4ep ) ; p5hgyyqss4 ( & rtB . arqjd0gbjl , & rtDW . arqjd0gbjl ,
& rtP . arqjd0gbjl , rtP . T3_ic_jx43e1q4ep ) ; mbb3rj5v0g ( & rtB .
e1cruf0x3a , & rtDW . e1cruf0x3a , & rtP . e1cruf0x3a , rtP .
T4_ic_aolukxevoz ) ; p5hgyyqss4 ( & rtB . pg41d4mdss , & rtDW . pg41d4mdss ,
& rtP . pg41d4mdss , rtP . T4_ic_aolukxevoz ) ; mbb3rj5v0g ( & rtB .
dej3sbopt3 , & rtDW . dej3sbopt3 , & rtP . dej3sbopt3 , rtP .
OffDelay_ic_pi0l0q2vst ) ; p5hgyyqss4 ( & rtB . ongp1nebfx , & rtDW .
ongp1nebfx , & rtP . ongp1nebfx , rtP . OffDelay_ic_pi0l0q2vst ) ; mbb3rj5v0g
( & rtB . hvibrw2awu , & rtDW . hvibrw2awu , & rtP . hvibrw2awu , rtP .
T1_ic_k0azirn4rs ) ; p5hgyyqss4 ( & rtB . allgw5ptgk , & rtDW . allgw5ptgk ,
& rtP . allgw5ptgk , rtP . T1_ic_k0azirn4rs ) ; mbb3rj5v0g ( & rtB .
f5lqeh5sbp , & rtDW . f5lqeh5sbp , & rtP . f5lqeh5sbp , rtP .
T2_ic_njbbqwwz0x ) ; p5hgyyqss4 ( & rtB . br1i12egyj , & rtDW . br1i12egyj ,
& rtP . br1i12egyj , rtP . T2_ic_njbbqwwz0x ) ; mbb3rj5v0g ( & rtB .
jvoggbn2yd , & rtDW . jvoggbn2yd , & rtP . jvoggbn2yd , rtP .
T1_ic_g0yajf3snr ) ; p5hgyyqss4 ( & rtB . boy5y1bmz3 , & rtDW . boy5y1bmz3 ,
& rtP . boy5y1bmz3 , rtP . T1_ic_g0yajf3snr ) ; mbb3rj5v0g ( & rtB .
ithg0f1jiq , & rtDW . ithg0f1jiq , & rtP . ithg0f1jiq , rtP .
T2_ic_j0uo303tqq ) ; p5hgyyqss4 ( & rtB . ojnin3nhvw , & rtDW . ojnin3nhvw ,
& rtP . ojnin3nhvw , rtP . T2_ic_j0uo303tqq ) ; mbb3rj5v0g ( & rtB .
p2xsu5tknt , & rtDW . p2xsu5tknt , & rtP . p2xsu5tknt , rtP .
T3_ic_bllm0s4eem ) ; p5hgyyqss4 ( & rtB . bhbl51zywo , & rtDW . bhbl51zywo ,
& rtP . bhbl51zywo , rtP . T3_ic_bllm0s4eem ) ; h33j1jr1zm ( & rtB .
j1rcgjyzg3 , & rtP . j1rcgjyzg3 ) ; h4dgpi453n ( & rtB . fh4f1ldkj5 , & rtP .
fh4f1ldkj5 ) ; mbb3rj5v0g ( & rtB . diplwx4hig , & rtDW . diplwx4hig , & rtP
. diplwx4hig , rtP . OffDelay_ic_f24lpvq14j ) ; p5hgyyqss4 ( & rtB .
my1xc0on3o , & rtDW . my1xc0on3o , & rtP . my1xc0on3o , rtP .
OffDelay_ic_f24lpvq14j ) ; mbb3rj5v0g ( & rtB . gn1qmb4zgp , & rtDW .
gn1qmb4zgp , & rtP . gn1qmb4zgp , rtP . OffDelay_ic_n4amwstyrr ) ; p5hgyyqss4
( & rtB . bvpxesww1j , & rtDW . bvpxesww1j , & rtP . bvpxesww1j , rtP .
OffDelay_ic_n4amwstyrr ) ; mbb3rj5v0g ( & rtB . lf152u5jm4 , & rtDW .
lf152u5jm4 , & rtP . lf152u5jm4 , rtP . T1_ic_imevbe5kcf ) ; p5hgyyqss4 ( &
rtB . dewlul2b2r , & rtDW . dewlul2b2r , & rtP . dewlul2b2r , rtP .
T1_ic_imevbe5kcf ) ; mbb3rj5v0g ( & rtB . ivzhqxkykj , & rtDW . ivzhqxkykj ,
& rtP . ivzhqxkykj , rtP . T2_ic_jmvbespov3 ) ; p5hgyyqss4 ( & rtB .
kd31jkgljk , & rtDW . kd31jkgljk , & rtP . kd31jkgljk , rtP .
T2_ic_jmvbespov3 ) ; h33j1jr1zm ( & rtB . culgjirjhd , & rtP . culgjirjhd ) ;
h4dgpi453n ( & rtB . dymcbszxp2 , & rtP . dymcbszxp2 ) ; mbb3rj5v0g ( & rtB .
if1byo0yst , & rtDW . if1byo0yst , & rtP . if1byo0yst , rtP .
OffDelay_ic_futuznlzlq ) ; p5hgyyqss4 ( & rtB . j5kctkibbm , & rtDW .
j5kctkibbm , & rtP . j5kctkibbm , rtP . OffDelay_ic_futuznlzlq ) ; mbb3rj5v0g
( & rtB . onwgyzhgir , & rtDW . onwgyzhgir , & rtP . onwgyzhgir , rtP .
OffDelay_ic_oc5xpjdprc ) ; p5hgyyqss4 ( & rtB . ofq4vda2xo , & rtDW .
ofq4vda2xo , & rtP . ofq4vda2xo , rtP . OffDelay_ic_oc5xpjdprc ) ; mbb3rj5v0g
( & rtB . b13ln335zj , & rtDW . b13ln335zj , & rtP . b13ln335zj , rtP .
T1_ic_lvhmmwkem3 ) ; p5hgyyqss4 ( & rtB . d5dfks3vhb , & rtDW . d5dfks3vhb ,
& rtP . d5dfks3vhb , rtP . T1_ic_lvhmmwkem3 ) ; mbb3rj5v0g ( & rtB .
d2dryokhth , & rtDW . d2dryokhth , & rtP . d2dryokhth , rtP .
T2_ic_lp4m5himc1 ) ; p5hgyyqss4 ( & rtB . jcev1klzwf , & rtDW . jcev1klzwf ,
& rtP . jcev1klzwf , rtP . T2_ic_lp4m5himc1 ) ; h33j1jr1zm ( & rtB .
dxl224bky1 , & rtP . dxl224bky1 ) ; h4dgpi453n ( & rtB . mixbw55a4u , & rtP .
mixbw55a4u ) ; mbb3rj5v0g ( & rtB . aa351sera2 , & rtDW . aa351sera2 , & rtP
. aa351sera2 , rtP . T1_ic_cuxpesvxlz ) ; p5hgyyqss4 ( & rtB . lfm4cvmyjw , &
rtDW . lfm4cvmyjw , & rtP . lfm4cvmyjw , rtP . T1_ic_cuxpesvxlz ) ;
mbb3rj5v0g ( & rtB . bypgcvdndn , & rtDW . bypgcvdndn , & rtP . bypgcvdndn ,
rtP . T2_ic_lrashpj1r4 ) ; p5hgyyqss4 ( & rtB . dtwz5asia3 , & rtDW .
dtwz5asia3 , & rtP . dtwz5asia3 , rtP . T2_ic_lrashpj1r4 ) ; mbb3rj5v0g ( &
rtB . dkvqk03afq , & rtDW . dkvqk03afq , & rtP . dkvqk03afq , rtP .
T1_ic_mwxsieqpfq ) ; p5hgyyqss4 ( & rtB . drlew34sm0 , & rtDW . drlew34sm0 ,
& rtP . drlew34sm0 , rtP . T1_ic_mwxsieqpfq ) ; mbb3rj5v0g ( & rtB .
j3mwdbpvpq , & rtDW . j3mwdbpvpq , & rtP . j3mwdbpvpq , rtP .
T2_ic_ludyyq4t5w ) ; p5hgyyqss4 ( & rtB . ieaaexns5m , & rtDW . ieaaexns5m ,
& rtP . ieaaexns5m , rtP . T2_ic_ludyyq4t5w ) ; mbb3rj5v0g ( & rtB .
eg2uuciuj1 , & rtDW . eg2uuciuj1 , & rtP . eg2uuciuj1 , rtP .
T1_ic_f4xqtruvul ) ; p5hgyyqss4 ( & rtB . emkwv3osqn , & rtDW . emkwv3osqn ,
& rtP . emkwv3osqn , rtP . T1_ic_f4xqtruvul ) ; mbb3rj5v0g ( & rtB .
acyd2alexd , & rtDW . acyd2alexd , & rtP . acyd2alexd , rtP .
T2_ic_hfm534htlr ) ; p5hgyyqss4 ( & rtB . kcdiwopp0s , & rtDW . kcdiwopp0s ,
& rtP . kcdiwopp0s , rtP . T2_ic_hfm534htlr ) ; mbb3rj5v0g ( & rtB .
bd4y1f4afb , & rtDW . bd4y1f4afb , & rtP . bd4y1f4afb , rtP .
T4_ic_ns0iuhqzo5 ) ; p5hgyyqss4 ( & rtB . bbo3ui3fzz , & rtDW . bbo3ui3fzz ,
& rtP . bbo3ui3fzz , rtP . T4_ic_ns0iuhqzo5 ) ; mbb3rj5v0g ( & rtB .
jejxmo41e1 , & rtDW . jejxmo41e1 , & rtP . jejxmo41e1 , rtP . T7_ic ) ;
p5hgyyqss4 ( & rtB . kwzmy55kax , & rtDW . kwzmy55kax , & rtP . kwzmy55kax ,
rtP . T7_ic ) ; mbb3rj5v0g ( & rtB . ge1ps1iuod , & rtDW . ge1ps1iuod , & rtP
. ge1ps1iuod , rtP . T2_ic_bbwq3wqim1 ) ; p5hgyyqss4 ( & rtB . bk3jg31gdb , &
rtDW . bk3jg31gdb , & rtP . bk3jg31gdb , rtP . T2_ic_bbwq3wqim1 ) ;
mbb3rj5v0g ( & rtB . i43atkcssg , & rtDW . i43atkcssg , & rtP . i43atkcssg ,
rtP . T6_ic ) ; p5hgyyqss4 ( & rtB . kqhpdk5kzz , & rtDW . kqhpdk5kzz , & rtP
. kqhpdk5kzz , rtP . T6_ic ) ; mbb3rj5v0g ( & rtB . jvraryandh , & rtDW .
jvraryandh , & rtP . jvraryandh , rtP . T3_ic_cyho1ay5s5 ) ; p5hgyyqss4 ( &
rtB . e2epyeugyn , & rtDW . e2epyeugyn , & rtP . e2epyeugyn , rtP .
T3_ic_cyho1ay5s5 ) ; h33j1jr1zm ( & rtB . i2y4oqfnlh , & rtP . i2y4oqfnlh ) ;
h4dgpi453n ( & rtB . fushkw2zny , & rtP . fushkw2zny ) ; mbb3rj5v0g ( & rtB .
dffp1wyrjz , & rtDW . dffp1wyrjz , & rtP . dffp1wyrjz , rtP . T5_ic ) ;
p5hgyyqss4 ( & rtB . mwrkb5ri4b , & rtDW . mwrkb5ri4b , & rtP . mwrkb5ri4b ,
rtP . T5_ic ) ; mbb3rj5v0g ( & rtB . on20slldft0 , & rtDW . on20slldft0 , &
rtP . on20slldft0 , rtP . T1_ic_beilry2sh5 ) ; p5hgyyqss4 ( & rtB .
bhc3qgnuicw , & rtDW . bhc3qgnuicw , & rtP . bhc3qgnuicw , rtP .
T1_ic_beilry2sh5 ) ; h33j1jr1zm ( & rtB . fau5zmxxcd , & rtP . fau5zmxxcd ) ;
h4dgpi453n ( & rtB . cg1ijj42cn , & rtP . cg1ijj42cn ) ; h33j1jr1zm ( & rtB .
ncfy3u3izo , & rtP . ncfy3u3izo ) ; h4dgpi453n ( & rtB . np33hf1v4j , & rtP .
np33hf1v4j ) ; ora1wlqeti ( & rtB . jw4rwe2x30 , & rtP . jw4rwe2x30 ) ;
ora1wlqeti ( & rtB . mydfkxdbcd , & rtP . mydfkxdbcd ) ; ora1wlqeti ( & rtB .
dcuqvbgzpd , & rtP . dcuqvbgzpd ) ; mbb3rj5v0g ( & rtB . l0mam0lcui , & rtDW
. l0mam0lcui , & rtP . l0mam0lcui , rtP . T1_ic_puya40fane ) ; p5hgyyqss4 ( &
rtB . ck45m3cw5y , & rtDW . ck45m3cw5y , & rtP . ck45m3cw5y , rtP .
T1_ic_puya40fane ) ; mbb3rj5v0g ( & rtB . o3bwas2b4l , & rtDW . o3bwas2b4l ,
& rtP . o3bwas2b4l , rtP . T2_ic_lvssotftx1 ) ; p5hgyyqss4 ( & rtB .
h330fuij3a , & rtDW . h330fuij3a , & rtP . h330fuij3a , rtP .
T2_ic_lvssotftx1 ) ; mbb3rj5v0g ( & rtB . ifezniamwz , & rtDW . ifezniamwz ,
& rtP . ifezniamwz , rtP . T6_ic_foqt1kupag ) ; p5hgyyqss4 ( & rtB .
dsdu0sw4bh , & rtDW . dsdu0sw4bh , & rtP . dsdu0sw4bh , rtP .
T6_ic_foqt1kupag ) ; mbb3rj5v0g ( & rtB . cssuh33ty1 , & rtDW . cssuh33ty1 ,
& rtP . cssuh33ty1 , rtP . T7_ic_ha53cwsmgk ) ; p5hgyyqss4 ( & rtB .
nmmyyafynb , & rtDW . nmmyyafynb , & rtP . nmmyyafynb , rtP .
T7_ic_ha53cwsmgk ) ; mbb3rj5v0g ( & rtB . pflqpv2odc , & rtDW . pflqpv2odc ,
& rtP . pflqpv2odc , rtP . T3_ic_awp5v1vfjo ) ; p5hgyyqss4 ( & rtB .
c5rp3exjwz , & rtDW . c5rp3exjwz , & rtP . c5rp3exjwz , rtP .
T3_ic_awp5v1vfjo ) ; h33j1jr1zm ( & rtB . pqmqn4c0xq , & rtP . pqmqn4c0xq ) ;
h4dgpi453n ( & rtB . bckzp5laog , & rtP . bckzp5laog ) ; ora1wlqeti ( & rtB .
lsja5jhqu0 , & rtP . lsja5jhqu0 ) ; mbb3rj5v0g ( & rtB . cqgqlcfbrn , & rtDW
. cqgqlcfbrn , & rtP . cqgqlcfbrn , rtP . T1_ic_kyx5axl4lo ) ; p5hgyyqss4 ( &
rtB . pshtvwhcvn , & rtDW . pshtvwhcvn , & rtP . pshtvwhcvn , rtP .
T1_ic_kyx5axl4lo ) ; mbb3rj5v0g ( & rtB . cwhxhlnmc3 , & rtDW . cwhxhlnmc3 ,
& rtP . cwhxhlnmc3 , rtP . T2_ic_khmmiz0sbs ) ; p5hgyyqss4 ( & rtB .
pxbtpesdlg , & rtDW . pxbtpesdlg , & rtP . pxbtpesdlg , rtP .
T2_ic_khmmiz0sbs ) ; mbb3rj5v0g ( & rtB . j1qputxmt3 , & rtDW . j1qputxmt3 ,
& rtP . j1qputxmt3 , rtP . T6_ic_oculr4hwq4 ) ; p5hgyyqss4 ( & rtB .
h4jqcza024 , & rtDW . h4jqcza024 , & rtP . h4jqcza024 , rtP .
T6_ic_oculr4hwq4 ) ; mbb3rj5v0g ( & rtB . h510twish3 , & rtDW . h510twish3 ,
& rtP . h510twish3 , rtP . T7_ic_bbbc5x05f5 ) ; p5hgyyqss4 ( & rtB .
b1csqxlk14 , & rtDW . b1csqxlk14 , & rtP . b1csqxlk14 , rtP .
T7_ic_bbbc5x05f5 ) ; mbb3rj5v0g ( & rtB . cdulylvwwz , & rtDW . cdulylvwwz ,
& rtP . cdulylvwwz , rtP . T3_ic_jsgb14plpx ) ; p5hgyyqss4 ( & rtB .
o2355kd0aj , & rtDW . o2355kd0aj , & rtP . o2355kd0aj , rtP .
T3_ic_jsgb14plpx ) ; h33j1jr1zm ( & rtB . jfvcadr0il , & rtP . jfvcadr0il ) ;
h4dgpi453n ( & rtB . pxgzwnihte , & rtP . pxgzwnihte ) ; ora1wlqeti ( & rtB .
nceplkru4a , & rtP . nceplkru4a ) ; mbb3rj5v0g ( & rtB . muhd0m2ek3 , & rtDW
. muhd0m2ek3 , & rtP . muhd0m2ek3 , rtP . sign_st1_ic ) ; p5hgyyqss4 ( & rtB
. g2vaojwvzg , & rtDW . g2vaojwvzg , & rtP . g2vaojwvzg , rtP . sign_st1_ic )
; mbb3rj5v0g ( & rtB . iosuhjjule , & rtDW . iosuhjjule , & rtP . iosuhjjule
, rtP . st1_ic ) ; p5hgyyqss4 ( & rtB . fey4zxdyka , & rtDW . fey4zxdyka , &
rtP . fey4zxdyka , rtP . st1_ic ) ; mbb3rj5v0g ( & rtB . j0x4gfzwjd , & rtDW
. j0x4gfzwjd , & rtP . j0x4gfzwjd , rtP . st2_ic ) ; p5hgyyqss4 ( & rtB .
anq402mwuh , & rtDW . anq402mwuh , & rtP . anq402mwuh , rtP . st2_ic ) ;
mbb3rj5v0g ( & rtB . kxnpw5jl23 , & rtDW . kxnpw5jl23 , & rtP . kxnpw5jl23 ,
rtP . OffDelay_ic_cwb2aegjcf ) ; p5hgyyqss4 ( & rtB . g2dmi2bhhz , & rtDW .
g2dmi2bhhz , & rtP . g2dmi2bhhz , rtP . OffDelay_ic_cwb2aegjcf ) ; h33j1jr1zm
( & rtB . mzl2ofouzz , & rtP . mzl2ofouzz ) ; h4dgpi453n ( & rtB . er0egiqxa1
, & rtP . er0egiqxa1 ) ; mbb3rj5v0g ( & rtB . jzgpron3co , & rtDW .
jzgpron3co , & rtP . jzgpron3co , rtP . OffDelay1_ic ) ; p5hgyyqss4 ( & rtB .
dzkeyq1gmc , & rtDW . dzkeyq1gmc , & rtP . dzkeyq1gmc , rtP . OffDelay1_ic )
; h33j1jr1zm ( & rtB . b0usl1ftx3 , & rtP . b0usl1ftx3 ) ; h4dgpi453n ( & rtB
. ftiqhc2zan , & rtP . ftiqhc2zan ) ; ora1wlqeti ( & rtB . a503xmz3wd , & rtP
. a503xmz3wd ) ; ora1wlqeti ( & rtB . ktoyym5lv5 , & rtP . ktoyym5lv5 ) ;
mbb3rj5v0g ( & rtB . ox1j1fxqy2 , & rtDW . ox1j1fxqy2 , & rtP . ox1j1fxqy2 ,
rtP . OffDelay_ic_foizle5h0z ) ; p5hgyyqss4 ( & rtB . nmew4qgpmi , & rtDW .
nmew4qgpmi , & rtP . nmew4qgpmi , rtP . OffDelay_ic_foizle5h0z ) ; h33j1jr1zm
( & rtB . k050vsxh1j , & rtP . k050vsxh1j ) ; h4dgpi453n ( & rtB . ku2xodfghq
, & rtP . ku2xodfghq ) ; mbb3rj5v0g ( & rtB . outjvmmxn3 , & rtDW .
outjvmmxn3 , & rtP . outjvmmxn3 , rtP . OffDelay1_ic_i1qnla0r14 ) ;
p5hgyyqss4 ( & rtB . brhbedften , & rtDW . brhbedften , & rtP . brhbedften ,
rtP . OffDelay1_ic_i1qnla0r14 ) ; mbb3rj5v0g ( & rtB . pwnmvwir3d , & rtDW .
pwnmvwir3d , & rtP . pwnmvwir3d , rtP . OffDelay2_ic ) ; p5hgyyqss4 ( & rtB .
ozvscdoxcm , & rtDW . ozvscdoxcm , & rtP . ozvscdoxcm , rtP . OffDelay2_ic )
; mbb3rj5v0g ( & rtB . dyyk14ao4t , & rtDW . dyyk14ao4t , & rtP . dyyk14ao4t
, rtP . OffDelay3_ic ) ; p5hgyyqss4 ( & rtB . k5ecxmrj0a , & rtDW .
k5ecxmrj0a , & rtP . k5ecxmrj0a , rtP . OffDelay3_ic ) ; h33j1jr1zm ( & rtB .
a4ebhrt5bq , & rtP . a4ebhrt5bq ) ; h4dgpi453n ( & rtB . b5umakrf53 , & rtP .
b5umakrf53 ) ; ora1wlqeti ( & rtB . d2nv3h1jhg , & rtP . d2nv3h1jhg ) ;
h33j1jr1zm ( & rtB . grl2yxc1of , & rtP . grl2yxc1of ) ; h4dgpi453n ( & rtB .
cnueqx4gvt , & rtP . cnueqx4gvt ) ; ora1wlqeti ( & rtB . ep3fctwraa , & rtP .
ep3fctwraa ) ; ora1wlqeti ( & rtB . kk13i4dykq , & rtP . kk13i4dykq ) ;
ora1wlqeti ( & rtB . nx03ta252g , & rtP . nx03ta252g ) ; ora1wlqeti ( & rtB .
j33xaeouj5 , & rtP . j33xaeouj5 ) ; ora1wlqeti ( & rtB . pk2r3vb4fa , & rtP .
pk2r3vb4fa ) ; mbb3rj5v0g ( & rtB . c2rt0zs300 , & rtDW . c2rt0zs300 , & rtP
. c2rt0zs300 , rtP . T1_ic_eakz0qsdw5 ) ; p5hgyyqss4 ( & rtB . pj2a3t0x3l , &
rtDW . pj2a3t0x3l , & rtP . pj2a3t0x3l , rtP . T1_ic_eakz0qsdw5 ) ;
h33j1jr1zm ( & rtB . ctjk4b4i5h , & rtP . ctjk4b4i5h ) ; h4dgpi453n ( & rtB .
j0qka4bwqf , & rtP . j0qka4bwqf ) ; ora1wlqeti ( & rtB . gk4015knwc , & rtP .
gk4015knwc ) ; mbb3rj5v0g ( & rtB . lkrts5ohpw , & rtDW . lkrts5ohpw , & rtP
. lkrts5ohpw , rtP . T1_ic_abswmovlhu ) ; p5hgyyqss4 ( & rtB . hh2cmxftpw , &
rtDW . hh2cmxftpw , & rtP . hh2cmxftpw , rtP . T1_ic_abswmovlhu ) ;
mbb3rj5v0g ( & rtB . me023aolbi , & rtDW . me023aolbi , & rtP . me023aolbi ,
rtP . T2_ic_a0h2nqcx3e ) ; p5hgyyqss4 ( & rtB . dexvjkg0hj , & rtDW .
dexvjkg0hj , & rtP . dexvjkg0hj , rtP . T2_ic_a0h2nqcx3e ) ; mbb3rj5v0g ( &
rtB . purgdi1jfk , & rtDW . purgdi1jfk , & rtP . purgdi1jfk , rtP .
T1_ic_e3mmu0vhpp ) ; p5hgyyqss4 ( & rtB . g3bxbyypfb , & rtDW . g3bxbyypfb ,
& rtP . g3bxbyypfb , rtP . T1_ic_e3mmu0vhpp ) ; mbb3rj5v0g ( & rtB .
e4qyv4smfd , & rtDW . e4qyv4smfd , & rtP . e4qyv4smfd , rtP .
T2_ic_kwvlgslfgf ) ; p5hgyyqss4 ( & rtB . bs5m40ynfy , & rtDW . bs5m40ynfy ,
& rtP . bs5m40ynfy , rtP . T2_ic_kwvlgslfgf ) ; mbb3rj5v0g ( & rtB .
lu12zasvyl , & rtDW . lu12zasvyl , & rtP . lu12zasvyl , rtP .
T1_ic_kipr1irh4l ) ; p5hgyyqss4 ( & rtB . ngrhemover , & rtDW . ngrhemover ,
& rtP . ngrhemover , rtP . T1_ic_kipr1irh4l ) ; mbb3rj5v0g ( & rtB .
myp2dcpbvg , & rtDW . myp2dcpbvg , & rtP . myp2dcpbvg , rtP .
T2_ic_nswgxuh2yg ) ; p5hgyyqss4 ( & rtB . onemmt3xbs , & rtDW . onemmt3xbs ,
& rtP . onemmt3xbs , rtP . T2_ic_nswgxuh2yg ) ; mbb3rj5v0g ( & rtB .
nci0imhwrv , & rtDW . nci0imhwrv , & rtP . nci0imhwrv , rtP .
T4_ic_euuhhocxg5 ) ; p5hgyyqss4 ( & rtB . dlrk53odir , & rtDW . dlrk53odir ,
& rtP . dlrk53odir , rtP . T4_ic_euuhhocxg5 ) ; mbb3rj5v0g ( & rtB .
mpydob1efk , & rtDW . mpydob1efk , & rtP . mpydob1efk , rtP .
T7_ic_gln0ppurxv ) ; p5hgyyqss4 ( & rtB . dlwrrwcsx5 , & rtDW . dlwrrwcsx5 ,
& rtP . dlwrrwcsx5 , rtP . T7_ic_gln0ppurxv ) ; mbb3rj5v0g ( & rtB .
lfrovwpki3 , & rtDW . lfrovwpki3 , & rtP . lfrovwpki3 , rtP .
T2_ic_aens51omaw ) ; p5hgyyqss4 ( & rtB . h145orsdu5 , & rtDW . h145orsdu5 ,
& rtP . h145orsdu5 , rtP . T2_ic_aens51omaw ) ; mbb3rj5v0g ( & rtB .
lbq0bgvgcl , & rtDW . lbq0bgvgcl , & rtP . lbq0bgvgcl , rtP .
T6_ic_k54rqambbx ) ; p5hgyyqss4 ( & rtB . cq2uqua4yo , & rtDW . cq2uqua4yo ,
& rtP . cq2uqua4yo , rtP . T6_ic_k54rqambbx ) ; mbb3rj5v0g ( & rtB .
inwm331xkg , & rtDW . inwm331xkg , & rtP . inwm331xkg , rtP .
T3_ic_cdbkrcus50 ) ; p5hgyyqss4 ( & rtB . obeddxl4xp , & rtDW . obeddxl4xp ,
& rtP . obeddxl4xp , rtP . T3_ic_cdbkrcus50 ) ; h33j1jr1zm ( & rtB .
oo1qtpn5j5 , & rtP . oo1qtpn5j5 ) ; h4dgpi453n ( & rtB . lrmch0dvn3 , & rtP .
lrmch0dvn3 ) ; mbb3rj5v0g ( & rtB . crzn5z4zte , & rtDW . crzn5z4zte , & rtP
. crzn5z4zte , rtP . T5_ic_oeikdrjd4m ) ; p5hgyyqss4 ( & rtB . llpwcowx1f , &
rtDW . llpwcowx1f , & rtP . llpwcowx1f , rtP . T5_ic_oeikdrjd4m ) ;
mbb3rj5v0g ( & rtB . hytx1hl5s5 , & rtDW . hytx1hl5s5 , & rtP . hytx1hl5s5 ,
rtP . T1_ic_j50v0hzrsk ) ; p5hgyyqss4 ( & rtB . mgkfvqxti2 , & rtDW .
mgkfvqxti2 , & rtP . mgkfvqxti2 , rtP . T1_ic_j50v0hzrsk ) ; h33j1jr1zm ( &
rtB . kuwwc0aum4 , & rtP . kuwwc0aum4 ) ; h4dgpi453n ( & rtB . c2mshkhisd , &
rtP . c2mshkhisd ) ; h33j1jr1zm ( & rtB . hwrjddg40d , & rtP . hwrjddg40d ) ;
h4dgpi453n ( & rtB . jxw41swge5 , & rtP . jxw41swge5 ) ; ora1wlqeti ( & rtB .
ivtty1aemy , & rtP . ivtty1aemy ) ; ora1wlqeti ( & rtB . mvirue1zit , & rtP .
mvirue1zit ) ; ora1wlqeti ( & rtB . f3zha40yok , & rtP . f3zha40yok ) ; rtX .
gfghljc4gn = 0.0 ; rtX . cgbqhiyy4k = rtP . integrator_IC ; rtDW . lnio5vndj4
= rtP . Memory_InitialCondition ; rtX . nhazmrtgmb = 0.0 ; rtX . pjrskjmpvo =
rtP . integrator_IC_feg5tpwv3q ; rtDW . cxai1mydgu = rtP .
Memory_InitialCondition_bnaq44k4qv ; pzffuxhmmn ( & rtB . kseqhf442uv , & rtP
. kseqhf442uv ) ; oitgzbgdnq ( & rtB . cgsqxgexc24 , & rtP . cgsqxgexc24 ) ;
rtB . hguzzrwry5 = rtP . Gain_Y0 ; pzffuxhmmn ( & rtB . n3ncmzva14 , & rtP .
n3ncmzva14 ) ; oitgzbgdnq ( & rtB . dx1zpxayg3 , & rtP . dx1zpxayg3 ) ;
azcfs245nz ( & rtB . dg2ggauitn , & rtP . dg2ggauitn ) ; azcfs245nz ( & rtB .
jvoeh2x3far , & rtP . jvoeh2x3far ) ; hnxt2e2yod ( & rtB . czm0w3hmwho , &
rtP . czm0w3hmwho ) ; azcfs245nz ( & rtB . mb5y4fcaem , & rtP . mb5y4fcaem )
; azcfs245nz ( & rtB . no1v55ban3 , & rtP . no1v55ban3 ) ; hnxt2e2yod ( & rtB
. pgdbd0g5cb , & rtP . pgdbd0g5cb ) ; azcfs245nz ( & rtB . p4hmlcjayz , & rtP
. p4hmlcjayz ) ; azcfs245nz ( & rtB . dem3so030k , & rtP . dem3so030k ) ;
hnxt2e2yod ( & rtB . mvaehis2rc , & rtP . mvaehis2rc ) ; azcfs245nz ( & rtB .
gjmkpqrmc3 , & rtP . gjmkpqrmc3 ) ; azcfs245nz ( & rtB . khd3qxar1i , & rtP .
khd3qxar1i ) ; hnxt2e2yod ( & rtB . jzsygc4sa4 , & rtP . jzsygc4sa4 ) ;
mbb3rj5v0g ( & rtB . jtynvhttl4 , & rtDW . jtynvhttl4 , & rtP . jtynvhttl4 ,
rtP . T3_ic_g2lalrw2jr ) ; p5hgyyqss4 ( & rtB . jdzdt1ap5b , & rtDW .
jdzdt1ap5b , & rtP . jdzdt1ap5b , rtP . T3_ic_g2lalrw2jr ) ; mbb3rj5v0g ( &
rtB . fvh2zdqaau , & rtDW . fvh2zdqaau , & rtP . fvh2zdqaau , rtP .
T4_ic_iqt2he1m0m ) ; p5hgyyqss4 ( & rtB . f41hg5wei1 , & rtDW . f41hg5wei1 ,
& rtP . f41hg5wei1 , rtP . T4_ic_iqt2he1m0m ) ; mbb3rj5v0g ( & rtB .
dhpm1tyfmn , & rtDW . dhpm1tyfmn , & rtP . dhpm1tyfmn , rtP .
T1_ic_nfcryrbo4h ) ; p5hgyyqss4 ( & rtB . grby3szfkd , & rtDW . grby3szfkd ,
& rtP . grby3szfkd , rtP . T1_ic_nfcryrbo4h ) ; mbb3rj5v0g ( & rtB .
nvltfn14rj , & rtDW . nvltfn14rj , & rtP . nvltfn14rj , rtP .
T2_ic_do1ojlr0cg ) ; p5hgyyqss4 ( & rtB . dzcv3as4ol , & rtDW . dzcv3as4ol ,
& rtP . dzcv3as4ol , rtP . T2_ic_do1ojlr0cg ) ; mbb3rj5v0g ( & rtB .
l2r2uh0izk , & rtDW . l2r2uh0izk , & rtP . l2r2uh0izk , rtP . DT1_ic ) ;
p5hgyyqss4 ( & rtB . feptx2ayjl , & rtDW . feptx2ayjl , & rtP . feptx2ayjl ,
rtP . DT1_ic ) ; mbb3rj5v0g ( & rtB . labvjdxfi3 , & rtDW . labvjdxfi3 , &
rtP . labvjdxfi3 , rtP . DT2_ic ) ; p5hgyyqss4 ( & rtB . nvnct41vnp , & rtDW
. nvnct41vnp , & rtP . nvnct41vnp , rtP . DT2_ic ) ; } void MdlEnable ( void
) { rtDW . cze1jlpasq = 1 ; rtDW . kdbkzvy4xq = 1 ; rtDW . ct3sr1xnt5 = 1 ;
rtDW . fnal3jedat = 1 ; rtDW . h4o3nqfxrq = 1 ; rtDW . gghl4uhzpz = 1 ; }
void MdlStart ( void ) { real_T tmp ; { void * * slioCatalogueAddr =
rt_slioCatalogueAddr ( ) ; void * r2 = ( NULL ) ; void * *
pOSigstreamManagerAddr = ( NULL ) ; const int maxErrorBufferSize = 16384 ;
char errMsgCreatingOSigstreamManager [ 16384 ] ; bool
errorCreatingOSigstreamManager = false ; const char *
errorAddingR2SharedResource = ( NULL ) ; * slioCatalogueAddr =
rtwGetNewSlioCatalogue ( rt_GetMatSigLogSelectorFileName ( ) ) ;
errorAddingR2SharedResource = rtwAddR2SharedResource (
rtwGetPointerFromUniquePtr ( rt_slioCatalogue ( ) ) , 1 ) ; if (
errorAddingR2SharedResource != ( NULL ) ) { rtwTerminateSlioCatalogue (
slioCatalogueAddr ) ; * slioCatalogueAddr = ( NULL ) ; ssSetErrorStatus ( rtS
, errorAddingR2SharedResource ) ; return ; } r2 = rtwGetR2SharedResource (
rtwGetPointerFromUniquePtr ( rt_slioCatalogue ( ) ) ) ;
pOSigstreamManagerAddr = rt_GetOSigstreamManagerAddr ( ) ;
errorCreatingOSigstreamManager = rtwOSigstreamManagerCreateInstance (
rt_GetMatSigLogSelectorFileName ( ) , r2 , pOSigstreamManagerAddr ,
errMsgCreatingOSigstreamManager , maxErrorBufferSize ) ; if (
errorCreatingOSigstreamManager ) { * pOSigstreamManagerAddr = ( NULL ) ;
ssSetErrorStatus ( rtS , errMsgCreatingOSigstreamManager ) ; return ; } } {
bool externalInputIsInDatasetFormat = false ; void * pISigstreamManager =
rt_GetISigstreamManager ( ) ; rtwISigstreamManagerGetInputIsInDatasetFormat (
pISigstreamManager , & externalInputIsInDatasetFormat ) ; if (
externalInputIsInDatasetFormat ) { } } { void * slioCatalogue =
rt_slioCatalogue ( ) ? rtwGetPointerFromUniquePtr ( rt_slioCatalogue ( ) ) :
sdiGetSlioCatalogue ( rt_dataMapInfo . mmi . InstanceMap . fullPath ) ; if (
! slioCatalogue || ! rtwIsLoggingToFile ( slioCatalogue ) ) { { {
sdiSignalSourceInfoU srcInfo ; sdiLabelU loggedName = sdiGetLabelFromChars (
"in" ) ; sdiLabelU origSigName = sdiGetLabelFromChars ( "in" ) ; sdiLabelU
propName = sdiGetLabelFromChars ( "" ) ; sdiLabelU blockPath =
sdiGetLabelFromChars (
"DZG_v3/System/�������_���-01 ���������/dzg_v1/jmp_a/in" ) ; sdiLabelU
blockSID = sdiGetLabelFromChars ( "" ) ; sdiLabelU subPath =
sdiGetLabelFromChars ( "" ) ; sdiDims sigDims ; sdiLabelU sigName =
sdiGetLabelFromChars ( "in" ) ; sdiAsyncRepoDataTypeHandle hDT =
sdiAsyncRepoGetBuiltInDataTypeHandle ( DATA_TYPE_BOOLEAN ) ; { sdiComplexity
sigComplexity = REAL ; sdiSampleTimeContinuity stCont = SAMPLE_TIME_DISCRETE
; int_T sigDimsArray [ 1 ] = { 1 } ; sigDims . nDims = 1 ; sigDims .
dimensions = sigDimsArray ; srcInfo . numBlockPathElems = 1 ; srcInfo .
fullBlockPath = ( sdiFullBlkPathU ) & blockPath ; srcInfo . SID = (
sdiSignalIDU ) & blockSID ; srcInfo . subPath = subPath ; srcInfo . portIndex
= 0 + 1 ; srcInfo . signalName = sigName ; srcInfo . sigSourceUUID = 0 ; rtDW
. kuceggnsk2 . AQHandles = sdiAsyncRepoCreateAsyncioQueue ( hDT , & srcInfo ,
rt_dataMapInfo . mmi . InstanceMap . fullPath ,
"8be895e0-c25c-4320-ab34-c7e74f0a1890" , sigComplexity , & sigDims ,
DIMENSIONS_MODE_FIXED , stCont , "" ) ; if ( rtDW . kuceggnsk2 . AQHandles )
{ sdiSetSignalSampleTimeString ( rtDW . kuceggnsk2 . AQHandles , "5e-05" ,
5.0E-5 , ssGetTFinal ( rtS ) ) ; sdiSetRunStartTime ( rtDW . kuceggnsk2 .
AQHandles , ssGetTaskTime ( rtS , 1 ) ) ; sdiAsyncRepoSetSignalExportSettings
( rtDW . kuceggnsk2 . AQHandles , 1 , 0 ) ; sdiAsyncRepoSetSignalExportName (
rtDW . kuceggnsk2 . AQHandles , loggedName , origSigName , propName ) ; }
sdiFreeLabel ( sigName ) ; sdiFreeLabel ( loggedName ) ; sdiFreeLabel (
origSigName ) ; sdiFreeLabel ( propName ) ; sdiFreeLabel ( blockPath ) ;
sdiFreeLabel ( blockSID ) ; sdiFreeLabel ( subPath ) ; } } } } rtDW .
kuceggnsk2 . SlioLTF = ( NULL ) ; { void * treeVector = ( NULL ) ; void *
accessor = ( NULL ) ; const void * signalDescriptor = ( NULL ) ; void *
loggingInterval = ( NULL ) ; char * datasetName = "tmp_raccel_logsout" ; void
* slioCatalogue = rt_slioCatalogue ( ) ? rtwGetPointerFromUniquePtr (
rt_slioCatalogue ( ) ) : sdiGetSlioCatalogue ( rt_dataMapInfo . mmi .
InstanceMap . fullPath ) ; if ( slioCatalogue && rtwIsLoggingToFile (
slioCatalogue ) ) { treeVector = rtwGetTreeVector ( ) ; { int_T sigDimsArray
[ 1 ] = { 1 } ; rtwAddLeafNode ( 8 , "in" , "zoh" , 0 , ( unsigned int * )
sigDimsArray , 1 , "logical" , "" , "5e-05" , 5.0E-5 , ssGetTFinal ( rtS ) ,
treeVector ) ; } signalDescriptor = rtwGetSignalDescriptor ( treeVector , 1 ,
1 , 0 , 1 , "in" , "" , rt_dataMapInfo . mmi . InstanceMap . fullPath ,
"DZG_v3/System/�������_���-01 ���������/dzg_v1/jmp_a/in" , 1 , 0 ,
slioCatalogue , ( NULL ) , 0 , ( NULL ) , 0 ) ; if ( ! rt_slioCatalogue ( ) )
{ sdiSlioIsLoggingSignal ( rt_dataMapInfo . mmi . InstanceMap . fullPath ,
"DZG_v3/System/�������_���-01 ���������/dzg_v1/jmp_a/in" , 1 , "in" ) ; } if
( rtwLoggingOverride ( signalDescriptor , slioCatalogue ) ) { if (
ssGetRootSS ( rtS ) -> mdlInfo -> rtwLogInfo ) { loggingInterval =
rtliGetLoggingInterval ( ssGetRootSS ( rtS ) -> mdlInfo -> rtwLogInfo ) ; }
else { loggingInterval = sdiGetLoggingIntervals ( rt_dataMapInfo . mmi .
InstanceMap . fullPath ) ; datasetName = "" ; } accessor = rtwGetAccessor (
signalDescriptor , loggingInterval ) ; rtwAddR2Client ( accessor ,
signalDescriptor , slioCatalogue , datasetName , 1 ) ; rtDW . kuceggnsk2 .
SlioLTF = accessor ; } } } } fovxs4153s ( rtS , & rtDW . bhc3qgnuicw ) ;
ei2bq4gymn ( rtS , & rtDW . on20slldft0 ) ; fovxs4153s ( rtS , & rtDW .
bk3jg31gdb ) ; ei2bq4gymn ( rtS , & rtDW . ge1ps1iuod ) ; fovxs4153s ( rtS ,
& rtDW . e2epyeugyn ) ; ei2bq4gymn ( rtS , & rtDW . jvraryandh ) ; fovxs4153s
( rtS , & rtDW . bbo3ui3fzz ) ; ei2bq4gymn ( rtS , & rtDW . bd4y1f4afb ) ;
fovxs4153s ( rtS , & rtDW . mwrkb5ri4b ) ; ei2bq4gymn ( rtS , & rtDW .
dffp1wyrjz ) ; fovxs4153s ( rtS , & rtDW . kqhpdk5kzz ) ; ei2bq4gymn ( rtS ,
& rtDW . i43atkcssg ) ; fovxs4153s ( rtS , & rtDW . kwzmy55kax ) ; ei2bq4gymn
( rtS , & rtDW . jejxmo41e1 ) ; fovxs4153s ( rtS , & rtDW . lfm4cvmyjw ) ;
ei2bq4gymn ( rtS , & rtDW . aa351sera2 ) ; fovxs4153s ( rtS , & rtDW .
dtwz5asia3 ) ; ei2bq4gymn ( rtS , & rtDW . bypgcvdndn ) ; fovxs4153s ( rtS ,
& rtDW . drlew34sm0 ) ; ei2bq4gymn ( rtS , & rtDW . dkvqk03afq ) ; fovxs4153s
( rtS , & rtDW . ieaaexns5m ) ; ei2bq4gymn ( rtS , & rtDW . j3mwdbpvpq ) ;
fovxs4153s ( rtS , & rtDW . emkwv3osqn ) ; ei2bq4gymn ( rtS , & rtDW .
eg2uuciuj1 ) ; fovxs4153s ( rtS , & rtDW . kcdiwopp0s ) ; ei2bq4gymn ( rtS ,
& rtDW . acyd2alexd ) ; fovxs4153s ( rtS , & rtDW . leagxggwwe ) ; ei2bq4gymn
( rtS , & rtDW . inadvhzvdh ) ; fovxs4153s ( rtS , & rtDW . my1xc0on3o ) ;
ei2bq4gymn ( rtS , & rtDW . diplwx4hig ) ; fovxs4153s ( rtS , & rtDW .
j5kctkibbm ) ; ei2bq4gymn ( rtS , & rtDW . if1byo0yst ) ; fovxs4153s ( rtS ,
& rtDW . ongp1nebfx ) ; ei2bq4gymn ( rtS , & rtDW . dej3sbopt3 ) ; fovxs4153s
( rtS , & rtDW . bvpxesww1j ) ; ei2bq4gymn ( rtS , & rtDW . gn1qmb4zgp ) ;
fovxs4153s ( rtS , & rtDW . ofq4vda2xo ) ; ei2bq4gymn ( rtS , & rtDW .
onwgyzhgir ) ; fovxs4153s ( rtS , & rtDW . mempnwchre ) ; ei2bq4gymn ( rtS ,
& rtDW . f2cfptydf3 ) ; fovxs4153s ( rtS , & rtDW . e05ysccjt5 ) ; ei2bq4gymn
( rtS , & rtDW . ceydu0zdw3 ) ; fovxs4153s ( rtS , & rtDW . ffo1fpyqgq ) ;
ei2bq4gymn ( rtS , & rtDW . kbnljt2vok ) ; fovxs4153s ( rtS , & rtDW .
nepgqug5c0 ) ; ei2bq4gymn ( rtS , & rtDW . nrt14kntrk ) ; fovxs4153s ( rtS ,
& rtDW . di10rsm5nz ) ; ei2bq4gymn ( rtS , & rtDW . a1srf3dmr2 ) ; fovxs4153s
( rtS , & rtDW . hgwrktvi4c ) ; ei2bq4gymn ( rtS , & rtDW . pry2hnvt4q ) ;
fovxs4153s ( rtS , & rtDW . arqjd0gbjl ) ; ei2bq4gymn ( rtS , & rtDW .
apyjjt13q1 ) ; fovxs4153s ( rtS , & rtDW . pg41d4mdss ) ; ei2bq4gymn ( rtS ,
& rtDW . e1cruf0x3a ) ; fovxs4153s ( rtS , & rtDW . c5rp3exjwz ) ; ei2bq4gymn
( rtS , & rtDW . pflqpv2odc ) ; fovxs4153s ( rtS , & rtDW . dsdu0sw4bh ) ;
ei2bq4gymn ( rtS , & rtDW . ifezniamwz ) ; fovxs4153s ( rtS , & rtDW .
nmmyyafynb ) ; ei2bq4gymn ( rtS , & rtDW . cssuh33ty1 ) ; fovxs4153s ( rtS ,
& rtDW . ck45m3cw5y ) ; ei2bq4gymn ( rtS , & rtDW . l0mam0lcui ) ; fovxs4153s
( rtS , & rtDW . h330fuij3a ) ; ei2bq4gymn ( rtS , & rtDW . o3bwas2b4l ) ;
fovxs4153s ( rtS , & rtDW . o2355kd0aj ) ; ei2bq4gymn ( rtS , & rtDW .
cdulylvwwz ) ; fovxs4153s ( rtS , & rtDW . h4jqcza024 ) ; ei2bq4gymn ( rtS ,
& rtDW . j1qputxmt3 ) ; fovxs4153s ( rtS , & rtDW . b1csqxlk14 ) ; ei2bq4gymn
( rtS , & rtDW . h510twish3 ) ; fovxs4153s ( rtS , & rtDW . pshtvwhcvn ) ;
ei2bq4gymn ( rtS , & rtDW . cqgqlcfbrn ) ; fovxs4153s ( rtS , & rtDW .
pxbtpesdlg ) ; ei2bq4gymn ( rtS , & rtDW . cwhxhlnmc3 ) ; fovxs4153s ( rtS ,
& rtDW . g2vaojwvzg ) ; ei2bq4gymn ( rtS , & rtDW . muhd0m2ek3 ) ; fovxs4153s
( rtS , & rtDW . fey4zxdyka ) ; ei2bq4gymn ( rtS , & rtDW . iosuhjjule ) ;
fovxs4153s ( rtS , & rtDW . anq402mwuh ) ; ei2bq4gymn ( rtS , & rtDW .
j0x4gfzwjd ) ; fovxs4153s ( rtS , & rtDW . g2dmi2bhhz ) ; ei2bq4gymn ( rtS ,
& rtDW . kxnpw5jl23 ) ; fovxs4153s ( rtS , & rtDW . dzkeyq1gmc ) ; ei2bq4gymn
( rtS , & rtDW . jzgpron3co ) ; fovxs4153s ( rtS , & rtDW . nmew4qgpmi ) ;
ei2bq4gymn ( rtS , & rtDW . ox1j1fxqy2 ) ; fovxs4153s ( rtS , & rtDW .
brhbedften ) ; ei2bq4gymn ( rtS , & rtDW . outjvmmxn3 ) ; fovxs4153s ( rtS ,
& rtDW . ozvscdoxcm ) ; ei2bq4gymn ( rtS , & rtDW . pwnmvwir3d ) ; fovxs4153s
( rtS , & rtDW . k5ecxmrj0a ) ; ei2bq4gymn ( rtS , & rtDW . dyyk14ao4t ) ;
fovxs4153s ( rtS , & rtDW . boy5y1bmz3 ) ; ei2bq4gymn ( rtS , & rtDW .
jvoggbn2yd ) ; fovxs4153s ( rtS , & rtDW . ojnin3nhvw ) ; ei2bq4gymn ( rtS ,
& rtDW . ithg0f1jiq ) ; fovxs4153s ( rtS , & rtDW . bhbl51zywo ) ; ei2bq4gymn
( rtS , & rtDW . p2xsu5tknt ) ; fovxs4153s ( rtS , & rtDW . allgw5ptgk ) ;
ei2bq4gymn ( rtS , & rtDW . hvibrw2awu ) ; fovxs4153s ( rtS , & rtDW .
br1i12egyj ) ; ei2bq4gymn ( rtS , & rtDW . f5lqeh5sbp ) ; fovxs4153s ( rtS ,
& rtDW . dewlul2b2r ) ; ei2bq4gymn ( rtS , & rtDW . lf152u5jm4 ) ; fovxs4153s
( rtS , & rtDW . kd31jkgljk ) ; ei2bq4gymn ( rtS , & rtDW . ivzhqxkykj ) ;
fovxs4153s ( rtS , & rtDW . d5dfks3vhb ) ; ei2bq4gymn ( rtS , & rtDW .
b13ln335zj ) ; fovxs4153s ( rtS , & rtDW . jcev1klzwf ) ; ei2bq4gymn ( rtS ,
& rtDW . d2dryokhth ) ; fovxs4153s ( rtS , & rtDW . pj2a3t0x3l ) ; ei2bq4gymn
( rtS , & rtDW . c2rt0zs300 ) ; fovxs4153s ( rtS , & rtDW . mgkfvqxti2 ) ;
ei2bq4gymn ( rtS , & rtDW . hytx1hl5s5 ) ; fovxs4153s ( rtS , & rtDW .
h145orsdu5 ) ; ei2bq4gymn ( rtS , & rtDW . lfrovwpki3 ) ; fovxs4153s ( rtS ,
& rtDW . obeddxl4xp ) ; ei2bq4gymn ( rtS , & rtDW . inwm331xkg ) ; fovxs4153s
( rtS , & rtDW . dlrk53odir ) ; ei2bq4gymn ( rtS , & rtDW . nci0imhwrv ) ;
fovxs4153s ( rtS , & rtDW . llpwcowx1f ) ; ei2bq4gymn ( rtS , & rtDW .
crzn5z4zte ) ; fovxs4153s ( rtS , & rtDW . cq2uqua4yo ) ; ei2bq4gymn ( rtS ,
& rtDW . lbq0bgvgcl ) ; fovxs4153s ( rtS , & rtDW . dlwrrwcsx5 ) ; ei2bq4gymn
( rtS , & rtDW . mpydob1efk ) ; fovxs4153s ( rtS , & rtDW . hh2cmxftpw ) ;
ei2bq4gymn ( rtS , & rtDW . lkrts5ohpw ) ; fovxs4153s ( rtS , & rtDW .
dexvjkg0hj ) ; ei2bq4gymn ( rtS , & rtDW . me023aolbi ) ; fovxs4153s ( rtS ,
& rtDW . g3bxbyypfb ) ; ei2bq4gymn ( rtS , & rtDW . purgdi1jfk ) ; fovxs4153s
( rtS , & rtDW . bs5m40ynfy ) ; ei2bq4gymn ( rtS , & rtDW . e4qyv4smfd ) ;
fovxs4153s ( rtS , & rtDW . ngrhemover ) ; ei2bq4gymn ( rtS , & rtDW .
lu12zasvyl ) ; fovxs4153s ( rtS , & rtDW . onemmt3xbs ) ; ei2bq4gymn ( rtS ,
& rtDW . myp2dcpbvg ) ; rtDW . br5wyeghdu = false ;
ssSetBlockStateForSolverChangedAtMajorStep ( rtS ) ; { int_T j ; { real_T *
pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 3 , 8192 , sizeof ( real_T ) ) ;
if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"vtdelay memory allocation error" ) ; return ; } rtDW . nklbecjxrg . Tail = 0
; rtDW . nklbecjxrg . Head = 0 ; rtDW . nklbecjxrg . Last = 0 ; rtDW .
nklbecjxrg . CircularBufSize = 8192 ; for ( j = 0 ; j < 8192 ; j ++ ) {
pBuffer [ j ] = rtP . VariableTransportDelay_InitOutput ; pBuffer [ 8192 + j
] = ssGetT ( rtS ) ; } pBuffer [ 2 * 8192 ] = 0.0 ; rtDW . gxot3lz5fc .
TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW . gxot3lz5fc .
TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; rtDW . gxot3lz5fc .
TUbufferPtrs [ 2 ] = ( void * ) & pBuffer [ 2 * 8192 ] ; } } { int_T j ; {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 3 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"vtdelay memory allocation error" ) ; return ; } rtDW . lxbhmg25bu . Tail = 0
; rtDW . lxbhmg25bu . Head = 0 ; rtDW . lxbhmg25bu . Last = 0 ; rtDW .
lxbhmg25bu . CircularBufSize = 8192 ; for ( j = 0 ; j < 8192 ; j ++ ) {
pBuffer [ j ] = rtP . VariableTransportDelay_InitOutput_hx330ne3nw ; pBuffer
[ 8192 + j ] = ssGetT ( rtS ) ; } pBuffer [ 2 * 8192 ] = 0.0 ; rtDW .
cay5bhojbk . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
cay5bhojbk . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; rtDW .
cay5bhojbk . TUbufferPtrs [ 2 ] = ( void * ) & pBuffer [ 2 * 8192 ] ; } }
hjgrwuhwvr ( rtS , & rtDW . kseqhf442uv ) ; jztd2wvlzd ( rtS , & rtDW .
cgsqxgexc24 ) ; fovxs4153s ( rtS , & rtDW . grby3szfkd ) ; ei2bq4gymn ( rtS ,
& rtDW . dhpm1tyfmn ) ; fovxs4153s ( rtS , & rtDW . dzcv3as4ol ) ; ei2bq4gymn
( rtS , & rtDW . nvltfn14rj ) ; fovxs4153s ( rtS , & rtDW . jdzdt1ap5b ) ;
ei2bq4gymn ( rtS , & rtDW . jtynvhttl4 ) ; fovxs4153s ( rtS , & rtDW .
f41hg5wei1 ) ; ei2bq4gymn ( rtS , & rtDW . fvh2zdqaau ) ; fovxs4153s ( rtS ,
& rtDW . feptx2ayjl ) ; ei2bq4gymn ( rtS , & rtDW . l2r2uh0izk ) ; fovxs4153s
( rtS , & rtDW . nvnct41vnp ) ; ei2bq4gymn ( rtS , & rtDW . labvjdxfi3 ) ; {
rtDW . orezesugg1 . DS = ( real_T * ) calloc ( 3 * 3 , sizeof ( real_T ) ) ;
rtDW . orezesugg1 . DX_COL = ( real_T * ) calloc ( 3 , sizeof ( real_T ) ) ;
rtDW . orezesugg1 . TMP2 = ( real_T * ) calloc ( 3 , sizeof ( real_T ) ) ;
rtDW . orezesugg1 . SWITCH_STATUS = ( int_T * ) calloc ( 3 , sizeof ( int_T )
) ; rtDW . orezesugg1 . SW_CHG = ( int_T * ) calloc ( 3 , sizeof ( int_T ) )
; rtDW . orezesugg1 . G_STATE = ( int_T * ) calloc ( 3 , sizeof ( int_T ) ) ;
rtDW . orezesugg1 . Y_SWITCH = ( real_T * ) calloc ( 3 , sizeof ( real_T ) )
; rtDW . orezesugg1 . SWITCH_TYPES = ( int_T * ) calloc ( 3 , sizeof ( int_T
) ) ; rtDW . orezesugg1 . IDX_OUT_SW = ( int_T * ) calloc ( 3 , sizeof (
int_T ) ) ; rtDW . orezesugg1 . SWITCH_STATUS_INIT = ( int_T * ) calloc ( 3 ,
sizeof ( int_T ) ) ; rtDW . orezesugg1 . USWLAST = ( real_T * ) calloc ( 3 ,
sizeof ( real_T ) ) ; } tmp = 6.2831853071795862 * rtP . System1_Frequency ;
rtP . SineWaveA_Hsin = muDoubleScalarSin ( tmp * 5.0E-5 ) ; tmp =
6.2831853071795862 * rtP . System1_Frequency ; rtP . SineWaveA_HCos =
muDoubleScalarCos ( tmp * 5.0E-5 ) ; tmp = 6.2831853071795862 * rtP .
System1_Frequency ; rtP . SineWaveA_PSin = muDoubleScalarSin ( 0.0 - tmp *
5.0E-5 ) ; tmp = 6.2831853071795862 * rtP . System1_Frequency ; rtP .
SineWaveA_PCos = muDoubleScalarCos ( 0.0 - tmp * 5.0E-5 ) ; tmp =
6.2831853071795862 * rtP . System1_Frequency ; rtP . SineWaveB_Hsin =
muDoubleScalarSin ( tmp * 5.0E-5 ) ; tmp = 6.2831853071795862 * rtP .
System1_Frequency ; rtP . SineWaveB_HCos = muDoubleScalarCos ( tmp * 5.0E-5 )
; tmp = 6.2831853071795862 * rtP . System1_Frequency ; rtP . SineWaveB_PSin =
muDoubleScalarSin ( - 2.0943951023931953 - tmp * 5.0E-5 ) ; tmp =
6.2831853071795862 * rtP . System1_Frequency ; rtP . SineWaveB_PCos =
muDoubleScalarCos ( - 2.0943951023931953 - tmp * 5.0E-5 ) ; tmp =
6.2831853071795862 * rtP . System1_Frequency ; rtP . SineWaveC_Hsin =
muDoubleScalarSin ( tmp * 5.0E-5 ) ; tmp = 6.2831853071795862 * rtP .
System1_Frequency ; rtP . SineWaveC_HCos = muDoubleScalarCos ( tmp * 5.0E-5 )
; tmp = 6.2831853071795862 * rtP . System1_Frequency ; rtP . SineWaveC_PSin =
muDoubleScalarSin ( 2.0943951023931953 - tmp * 5.0E-5 ) ; tmp =
6.2831853071795862 * rtP . System1_Frequency ; rtP . SineWaveC_PCos =
muDoubleScalarCos ( 2.0943951023931953 - tmp * 5.0E-5 ) ; tmp =
6.2831853071795862 * rtP . System4_Frequency ; rtP .
SineWaveA_Hsin_gtf0muyyo1 = muDoubleScalarSin ( tmp * 5.0E-5 ) ; tmp =
6.2831853071795862 * rtP . System4_Frequency ; rtP .
SineWaveA_HCos_pemhijovuq = muDoubleScalarCos ( tmp * 5.0E-5 ) ; tmp =
6.2831853071795862 * rtP . System4_Frequency ; rtP .
SineWaveA_PSin_j0jhvxxpyu = muDoubleScalarSin ( 1.1047934165124107 - tmp *
5.0E-5 ) ; tmp = 6.2831853071795862 * rtP . System4_Frequency ; rtP .
SineWaveA_PCos_dhftpvm54k = muDoubleScalarCos ( 1.1047934165124107 - tmp *
5.0E-5 ) ; tmp = 6.2831853071795862 * rtP . System4_Frequency ; rtP .
SineWaveB_Hsin_kfkb1n1f0h = muDoubleScalarSin ( tmp * 5.0E-5 ) ; tmp =
6.2831853071795862 * rtP . System4_Frequency ; rtP .
SineWaveB_HCos_o3i2ap52lt = muDoubleScalarCos ( tmp * 5.0E-5 ) ; tmp =
6.2831853071795862 * rtP . System4_Frequency ; rtP .
SineWaveB_PSin_ickyjsyajs = muDoubleScalarSin ( - 0.98960168588078479 - tmp *
5.0E-5 ) ; tmp = 6.2831853071795862 * rtP . System4_Frequency ; rtP .
SineWaveB_PCos_n1aem3ap0q = muDoubleScalarCos ( - 0.98960168588078479 - tmp *
5.0E-5 ) ; tmp = 6.2831853071795862 * rtP . System4_Frequency ; rtP .
SineWaveC_Hsin_edp2rwbke5 = muDoubleScalarSin ( tmp * 5.0E-5 ) ; tmp =
6.2831853071795862 * rtP . System4_Frequency ; rtP .
SineWaveC_HCos_kscsjgvg02 = muDoubleScalarCos ( tmp * 5.0E-5 ) ; tmp =
6.2831853071795862 * rtP . System4_Frequency ; rtP .
SineWaveC_PSin_akytznrapm = muDoubleScalarSin ( 3.1991885189056064 - tmp *
5.0E-5 ) ; tmp = 6.2831853071795862 * rtP . System4_Frequency ; rtP .
SineWaveC_PCos_ircs0plqoh = muDoubleScalarCos ( 3.1991885189056064 - tmp *
5.0E-5 ) ; { rtDW . g45z4iebyu . AS = ( real_T * ) calloc ( 6 * 6 , sizeof (
real_T ) ) ; rtDW . g45z4iebyu . BS = ( real_T * ) calloc ( 6 * 12 , sizeof (
real_T ) ) ; rtDW . g45z4iebyu . CS = ( real_T * ) calloc ( 48 * 6 , sizeof (
real_T ) ) ; rtDW . g45z4iebyu . DS = ( real_T * ) calloc ( 48 * 12 , sizeof
( real_T ) ) ; rtDW . g45z4iebyu . DX_COL = ( real_T * ) calloc ( 48 , sizeof
( real_T ) ) ; rtDW . g45z4iebyu . TMP2 = ( real_T * ) calloc ( 12 , sizeof (
real_T ) ) ; rtDW . g45z4iebyu . BD_COL = ( real_T * ) calloc ( 6 , sizeof (
real_T ) ) ; rtDW . g45z4iebyu . TMP1 = ( real_T * ) calloc ( 6 , sizeof (
real_T ) ) ; rtDW . g45z4iebyu . XTMP = ( real_T * ) calloc ( 6 , sizeof (
real_T ) ) ; rtDW . g45z4iebyu . SWITCH_STATUS = ( int_T * ) calloc ( 6 ,
sizeof ( int_T ) ) ; rtDW . g45z4iebyu . SW_CHG = ( int_T * ) calloc ( 6 ,
sizeof ( int_T ) ) ; rtDW . g45z4iebyu . G_STATE = ( int_T * ) calloc ( 6 ,
sizeof ( int_T ) ) ; rtDW . g45z4iebyu . Y_SWITCH = ( real_T * ) calloc ( 6 ,
sizeof ( real_T ) ) ; rtDW . g45z4iebyu . SWITCH_TYPES = ( int_T * ) calloc (
6 , sizeof ( int_T ) ) ; rtDW . g45z4iebyu . IDX_OUT_SW = ( int_T * ) calloc
( 6 , sizeof ( int_T ) ) ; rtDW . g45z4iebyu . SWITCH_STATUS_INIT = ( int_T *
) calloc ( 6 , sizeof ( int_T ) ) ; rtDW . g45z4iebyu . USWLAST = ( real_T *
) calloc ( 6 , sizeof ( real_T ) ) ; } { { real_T * pBuffer = ( real_T * )
rt_TDelayCreateBuf ( 2 , 8192 , sizeof ( real_T ) ) ; if ( pBuffer == ( NULL
) ) { ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ;
} rtDW . ixae4mywgu . Tail = 0 ; rtDW . ixae4mywgu . Head = 0 ; rtDW .
ixae4mywgu . Last = 0 ; rtDW . ixae4mywgu . CircularBufSize = 8192 ; pBuffer
[ 0 ] = rtP . TransportDelay_InitOutput ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . grt2oyi0sr . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
grt2oyi0sr . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . ja00u1cqz3 . Tail = 0
; rtDW . ja00u1cqz3 . Head = 0 ; rtDW . ja00u1cqz3 . Last = 0 ; rtDW .
ja00u1cqz3 . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_eaxnllrdum ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . pl0qrajpw4 . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
pl0qrajpw4 . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . iepysspqfq . Tail = 0
; rtDW . iepysspqfq . Head = 0 ; rtDW . iepysspqfq . Last = 0 ; rtDW .
iepysspqfq . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_nqf1d1yteg ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . ax1z1sifrs . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
ax1z1sifrs . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . d5g4hqdsxq . Tail = 0
; rtDW . d5g4hqdsxq . Head = 0 ; rtDW . d5g4hqdsxq . Last = 0 ; rtDW .
d5g4hqdsxq . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_jrvdftqeey ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . d5b3ghqdjb . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
d5b3ghqdjb . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . b2xp1t1hmq . Tail = 0
; rtDW . b2xp1t1hmq . Head = 0 ; rtDW . b2xp1t1hmq . Last = 0 ; rtDW .
b2xp1t1hmq . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_oo2wtyq0tx ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . d45fc0cu2n . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
d45fc0cu2n . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . ewf1j0idml . Tail = 0
; rtDW . ewf1j0idml . Head = 0 ; rtDW . ewf1j0idml . Last = 0 ; rtDW .
ewf1j0idml . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_hafu4kbvs4 ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . c5rfy4mzmq . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
c5rfy4mzmq . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . if32tdg5qk . Tail = 0
; rtDW . if32tdg5qk . Head = 0 ; rtDW . if32tdg5qk . Last = 0 ; rtDW .
if32tdg5qk . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_jatjvory4z ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . lmfxi4smjj . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
lmfxi4smjj . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . jedhnolofy . Tail = 0
; rtDW . jedhnolofy . Head = 0 ; rtDW . jedhnolofy . Last = 0 ; rtDW .
jedhnolofy . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_cru0r3mqsd ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . d3lf52anqx . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
d3lf52anqx . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . a2f4jln130 . Tail = 0
; rtDW . a2f4jln130 . Head = 0 ; rtDW . a2f4jln130 . Last = 0 ; rtDW .
a2f4jln130 . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_cl0e2vz1q1 ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . gvogk5wosq . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
gvogk5wosq . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . h43kjr3444 . Tail = 0
; rtDW . h43kjr3444 . Head = 0 ; rtDW . h43kjr3444 . Last = 0 ; rtDW .
h43kjr3444 . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_kvkmzazkam ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . innrerrg2w . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
innrerrg2w . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . gudqpjffwb . Tail = 0
; rtDW . gudqpjffwb . Head = 0 ; rtDW . gudqpjffwb . Last = 0 ; rtDW .
gudqpjffwb . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_b32jase2fn ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . b3tfxdcqfh . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
b3tfxdcqfh . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . ipe5rnt2je . Tail = 0
; rtDW . ipe5rnt2je . Head = 0 ; rtDW . ipe5rnt2je . Last = 0 ; rtDW .
ipe5rnt2je . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_fzh1fdwm1e ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . ex4axikx2n . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
ex4axikx2n . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . deyhny3sye . Tail = 0
; rtDW . deyhny3sye . Head = 0 ; rtDW . deyhny3sye . Last = 0 ; rtDW .
deyhny3sye . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_pl4ev2e413 ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . f5mcje12pk . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
f5mcje12pk . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . abila4k5tf . Tail = 0
; rtDW . abila4k5tf . Head = 0 ; rtDW . abila4k5tf . Last = 0 ; rtDW .
abila4k5tf . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_hx3xgrs1b1 ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . oi1yc2ajy5 . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
oi1yc2ajy5 . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . iwj0qe5gyd . Tail = 0
; rtDW . iwj0qe5gyd . Head = 0 ; rtDW . iwj0qe5gyd . Last = 0 ; rtDW .
iwj0qe5gyd . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_brwezlkbny ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . c2kuyduabk . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
c2kuyduabk . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . fupmlcibf5 . Tail = 0
; rtDW . fupmlcibf5 . Head = 0 ; rtDW . fupmlcibf5 . Last = 0 ; rtDW .
fupmlcibf5 . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_fge1igsmgb ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . exexriqgpr . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
exexriqgpr . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . iqlusmvwiw . Tail = 0
; rtDW . iqlusmvwiw . Head = 0 ; rtDW . iqlusmvwiw . Last = 0 ; rtDW .
iqlusmvwiw . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_jd3fo3ayg2 ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . k0yj2b2ldz . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
k0yj2b2ldz . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . gbcuvxshkr . Tail = 0
; rtDW . gbcuvxshkr . Head = 0 ; rtDW . gbcuvxshkr . Last = 0 ; rtDW .
gbcuvxshkr . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_etrpvcln3b ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . lbtex4xafe . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
lbtex4xafe . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . ov4twhz4dl . Tail = 0
; rtDW . ov4twhz4dl . Head = 0 ; rtDW . ov4twhz4dl . Last = 0 ; rtDW .
ov4twhz4dl . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_ez4yh5jpxv ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . dtlmsjjjpu . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
dtlmsjjjpu . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . iqunqpo4a3 . Tail = 0
; rtDW . iqunqpo4a3 . Head = 0 ; rtDW . iqunqpo4a3 . Last = 0 ; rtDW .
iqunqpo4a3 . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_ikh04lhagm ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . l30jrhdejr . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
l30jrhdejr . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . gxzqgoeivi . Tail = 0
; rtDW . gxzqgoeivi . Head = 0 ; rtDW . gxzqgoeivi . Last = 0 ; rtDW .
gxzqgoeivi . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_mvzaf4esul ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . hh24ilp1p1 . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
hh24ilp1p1 . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . obmianunwz . Tail = 0
; rtDW . obmianunwz . Head = 0 ; rtDW . obmianunwz . Last = 0 ; rtDW .
obmianunwz . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_fegn1hyvdl ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . djthquopua . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
djthquopua . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . nf5je2gw4m . Tail = 0
; rtDW . nf5je2gw4m . Head = 0 ; rtDW . nf5je2gw4m . Last = 0 ; rtDW .
nf5je2gw4m . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_aipaiuowqc ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . faad0u1b4n . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
faad0u1b4n . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . asmpxtvf2z . Tail = 0
; rtDW . asmpxtvf2z . Head = 0 ; rtDW . asmpxtvf2z . Last = 0 ; rtDW .
asmpxtvf2z . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_aa5aowvfp2 ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . k4bhgqdyhg . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
k4bhgqdyhg . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . prqk4dqmol . Tail = 0
; rtDW . prqk4dqmol . Head = 0 ; rtDW . prqk4dqmol . Last = 0 ; rtDW .
prqk4dqmol . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_jvpzsxmh0k ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . kx2dm0gq0l . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
kx2dm0gq0l . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . gl5lw50op2 . Tail = 0
; rtDW . gl5lw50op2 . Head = 0 ; rtDW . gl5lw50op2 . Last = 0 ; rtDW .
gl5lw50op2 . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_mxcmg3j1ft ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . milmx1vwl5 . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
milmx1vwl5 . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . ju1jo0gbez . Tail = 0
; rtDW . ju1jo0gbez . Head = 0 ; rtDW . ju1jo0gbez . Last = 0 ; rtDW .
ju1jo0gbez . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_i53veo5gam ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . e1qtan4tfe . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
e1qtan4tfe . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . ihorz5zaun . Tail = 0
; rtDW . ihorz5zaun . Head = 0 ; rtDW . ihorz5zaun . Last = 0 ; rtDW .
ihorz5zaun . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_mlqui5rcwm ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . nsvuvsgsn0 . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
nsvuvsgsn0 . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . msx5naejpe . Tail = 0
; rtDW . msx5naejpe . Head = 0 ; rtDW . msx5naejpe . Last = 0 ; rtDW .
msx5naejpe . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_mifq2pm4zt ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . e2vylf2hfx . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
e2vylf2hfx . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . n00kmgw5wb . Tail = 0
; rtDW . n00kmgw5wb . Head = 0 ; rtDW . n00kmgw5wb . Last = 0 ; rtDW .
n00kmgw5wb . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_dkuoxqd4yo ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . bflyns0cbr . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
bflyns0cbr . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . pqxdtxr3k0 . Tail = 0
; rtDW . pqxdtxr3k0 . Head = 0 ; rtDW . pqxdtxr3k0 . Last = 0 ; rtDW .
pqxdtxr3k0 . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_jolv14qahc ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . pxt2zbirdg . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
pxt2zbirdg . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . o2cxt5uwbv . Tail = 0
; rtDW . o2cxt5uwbv . Head = 0 ; rtDW . o2cxt5uwbv . Last = 0 ; rtDW .
o2cxt5uwbv . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_bwi25wucoj ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . fllltsfpxy . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
fllltsfpxy . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . owq1uvux0e . Tail = 0
; rtDW . owq1uvux0e . Head = 0 ; rtDW . owq1uvux0e . Last = 0 ; rtDW .
owq1uvux0e . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_ctbg4oq5wd ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . afb2vzbxaa . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
afb2vzbxaa . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . ffi24542he . Tail = 0
; rtDW . ffi24542he . Head = 0 ; rtDW . ffi24542he . Last = 0 ; rtDW .
ffi24542he . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_kom35bcuhc ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . fz41nona4h . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
fz41nona4h . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . kw0xkp24lw . Tail = 0
; rtDW . kw0xkp24lw . Head = 0 ; rtDW . kw0xkp24lw . Last = 0 ; rtDW .
kw0xkp24lw . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_d1bbi3uht4 ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . hqvoz0gebt . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
hqvoz0gebt . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . e54w33lz5m . Tail = 0
; rtDW . e54w33lz5m . Head = 0 ; rtDW . e54w33lz5m . Last = 0 ; rtDW .
e54w33lz5m . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_mfur1ucszt ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . pvl2ejzk4c . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
pvl2ejzk4c . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . m51gowaflk . Tail = 0
; rtDW . m51gowaflk . Head = 0 ; rtDW . m51gowaflk . Last = 0 ; rtDW .
m51gowaflk . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_n1itaqwmev ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . pzbmlqlvpm . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
pzbmlqlvpm . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . oihdsug2zq . Tail = 0
; rtDW . oihdsug2zq . Head = 0 ; rtDW . oihdsug2zq . Last = 0 ; rtDW .
oihdsug2zq . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_pcmq0yqmia ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . h0w4jpcxu0 . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
h0w4jpcxu0 . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . paswm1kxsx . Tail = 0
; rtDW . paswm1kxsx . Head = 0 ; rtDW . paswm1kxsx . Last = 0 ; rtDW .
paswm1kxsx . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_fvnrporko1 ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . bdtaclh4hr . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
bdtaclh4hr . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . i5hhavred2 . Tail = 0
; rtDW . i5hhavred2 . Head = 0 ; rtDW . i5hhavred2 . Last = 0 ; rtDW .
i5hhavred2 . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_ewfb2xuqzx ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . hwwk0yws23 . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
hwwk0yws23 . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . ph1zmaxjn3 . Tail = 0
; rtDW . ph1zmaxjn3 . Head = 0 ; rtDW . ph1zmaxjn3 . Last = 0 ; rtDW .
ph1zmaxjn3 . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_jkdixewjvk ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . eosdk1pxwr . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
eosdk1pxwr . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . azybsjku3n . Tail = 0
; rtDW . azybsjku3n . Head = 0 ; rtDW . azybsjku3n . Last = 0 ; rtDW .
azybsjku3n . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_lyvx0zioay ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . hdva23qpbu . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
hdva23qpbu . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . jzrzzypfkx . Tail = 0
; rtDW . jzrzzypfkx . Head = 0 ; rtDW . jzrzzypfkx . Last = 0 ; rtDW .
jzrzzypfkx . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_ovqbcxuq4w ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . kieb4vqgp3 . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
kieb4vqgp3 . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . g4bc1ycige . Tail = 0
; rtDW . g4bc1ycige . Head = 0 ; rtDW . g4bc1ycige . Last = 0 ; rtDW .
g4bc1ycige . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_mgxexxlite ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . oewxpxyjnd . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
oewxpxyjnd . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . juevrs3bbp . Tail = 0
; rtDW . juevrs3bbp . Head = 0 ; rtDW . juevrs3bbp . Last = 0 ; rtDW .
juevrs3bbp . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_dgtiattwbx ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . b31e0rqxiq . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
b31e0rqxiq . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . bzmcnbrysf . Tail = 0
; rtDW . bzmcnbrysf . Head = 0 ; rtDW . bzmcnbrysf . Last = 0 ; rtDW .
bzmcnbrysf . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_hipwpmfkbv ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . nhiqsehhpt . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
nhiqsehhpt . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . p5pmnzyfsd . Tail = 0
; rtDW . p5pmnzyfsd . Head = 0 ; rtDW . p5pmnzyfsd . Last = 0 ; rtDW .
p5pmnzyfsd . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_a5xg2bhsao ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . lcvmbgrgk0 . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
lcvmbgrgk0 . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . jaosajyms1 . Tail = 0
; rtDW . jaosajyms1 . Head = 0 ; rtDW . jaosajyms1 . Last = 0 ; rtDW .
jaosajyms1 . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_mkcbaww5bm ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . eogglflt5u . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
eogglflt5u . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . gimq5qj5re . Tail = 0
; rtDW . gimq5qj5re . Head = 0 ; rtDW . gimq5qj5re . Last = 0 ; rtDW .
gimq5qj5re . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_mnn0oksnci ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . kp5tjch01i . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
kp5tjch01i . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . p24tn1qizr . Tail = 0
; rtDW . p24tn1qizr . Head = 0 ; rtDW . p24tn1qizr . Last = 0 ; rtDW .
p24tn1qizr . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_gggrskr5oz ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . gfqptvfbp1 . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
gfqptvfbp1 . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } }
kwn5re5v52 ( rtS , & rtDW . j1rcgjyzg3 ) ; a5f3eefi2x ( rtS , & rtDW .
fh4f1ldkj5 ) ; kwn5re5v52 ( rtS , & rtDW . culgjirjhd ) ; a5f3eefi2x ( rtS ,
& rtDW . dymcbszxp2 ) ; kwn5re5v52 ( rtS , & rtDW . dxl224bky1 ) ; a5f3eefi2x
( rtS , & rtDW . mixbw55a4u ) ; kwn5re5v52 ( rtS , & rtDW . i2y4oqfnlh ) ;
a5f3eefi2x ( rtS , & rtDW . fushkw2zny ) ; kwn5re5v52 ( rtS , & rtDW .
fau5zmxxcd ) ; a5f3eefi2x ( rtS , & rtDW . cg1ijj42cn ) ; kwn5re5v52 ( rtS ,
& rtDW . ncfy3u3izo ) ; a5f3eefi2x ( rtS , & rtDW . np33hf1v4j ) ; kwn5re5v52
( rtS , & rtDW . pqmqn4c0xq ) ; a5f3eefi2x ( rtS , & rtDW . bckzp5laog ) ;
kwn5re5v52 ( rtS , & rtDW . jfvcadr0il ) ; a5f3eefi2x ( rtS , & rtDW .
pxgzwnihte ) ; kwn5re5v52 ( rtS , & rtDW . mzl2ofouzz ) ; a5f3eefi2x ( rtS ,
& rtDW . er0egiqxa1 ) ; kwn5re5v52 ( rtS , & rtDW . b0usl1ftx3 ) ; a5f3eefi2x
( rtS , & rtDW . ftiqhc2zan ) ; kwn5re5v52 ( rtS , & rtDW . k050vsxh1j ) ;
a5f3eefi2x ( rtS , & rtDW . ku2xodfghq ) ; kwn5re5v52 ( rtS , & rtDW .
a4ebhrt5bq ) ; a5f3eefi2x ( rtS , & rtDW . b5umakrf53 ) ; kwn5re5v52 ( rtS ,
& rtDW . grl2yxc1of ) ; a5f3eefi2x ( rtS , & rtDW . cnueqx4gvt ) ; { int_T j
; { real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 3 , 1024 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"vtdelay memory allocation error" ) ; return ; } rtDW . ees1hdl3bo . Tail = 0
; rtDW . ees1hdl3bo . Head = 0 ; rtDW . ees1hdl3bo . Last = 0 ; rtDW .
ees1hdl3bo . CircularBufSize = 1024 ; for ( j = 0 ; j < 1024 ; j ++ ) {
pBuffer [ j ] = rtP . VariableTransportDelay_InitOutput_cgrxqg4j0g ; pBuffer
[ 1024 + j ] = ssGetT ( rtS ) ; } pBuffer [ 2 * 1024 ] = 0.0 ; rtDW .
l0gmb53kdd . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
l0gmb53kdd . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 1024 ] ; rtDW .
l0gmb53kdd . TUbufferPtrs [ 2 ] = ( void * ) & pBuffer [ 2 * 1024 ] ; } } {
int_T j ; { real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 3 , 1024 ,
sizeof ( real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"vtdelay memory allocation error" ) ; return ; } rtDW . gty02ianr0 . Tail = 0
; rtDW . gty02ianr0 . Head = 0 ; rtDW . gty02ianr0 . Last = 0 ; rtDW .
gty02ianr0 . CircularBufSize = 1024 ; for ( j = 0 ; j < 1024 ; j ++ ) {
pBuffer [ j ] = rtP . VariableTransportDelay_InitOutput_dqdd2mgeon ; pBuffer
[ 1024 + j ] = ssGetT ( rtS ) ; } pBuffer [ 2 * 1024 ] = 0.0 ; rtDW .
i3r0m4gxn0 . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
i3r0m4gxn0 . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 1024 ] ; rtDW .
i3r0m4gxn0 . TUbufferPtrs [ 2 ] = ( void * ) & pBuffer [ 2 * 1024 ] ; } }
kwn5re5v52 ( rtS , & rtDW . ctjk4b4i5h ) ; a5f3eefi2x ( rtS , & rtDW .
j0qka4bwqf ) ; kwn5re5v52 ( rtS , & rtDW . oo1qtpn5j5 ) ; a5f3eefi2x ( rtS ,
& rtDW . lrmch0dvn3 ) ; kwn5re5v52 ( rtS , & rtDW . kuwwc0aum4 ) ; a5f3eefi2x
( rtS , & rtDW . c2mshkhisd ) ; kwn5re5v52 ( rtS , & rtDW . hwrjddg40d ) ;
a5f3eefi2x ( rtS , & rtDW . jxw41swge5 ) ; rtB . iaafsyfx4i = rtP .
Initial_Value ; rtDW . fpeafclpn0 = ( rtMinusInf ) ; { int_T j ; { real_T *
pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 3 , 8192 , sizeof ( real_T ) ) ;
if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"vtdelay memory allocation error" ) ; return ; } rtDW . lhuqmlzywi . Tail = 0
; rtDW . lhuqmlzywi . Head = 0 ; rtDW . lhuqmlzywi . Last = 0 ; rtDW .
lhuqmlzywi . CircularBufSize = 8192 ; for ( j = 0 ; j < 8192 ; j ++ ) {
pBuffer [ j ] = rtP . VariableTransportDelay_InitOutput_ggymofnpzk ; pBuffer
[ 8192 + j ] = ssGetT ( rtS ) ; } pBuffer [ 2 * 8192 ] = 0.0 ; rtDW .
bizzathsjz . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
bizzathsjz . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; rtDW .
bizzathsjz . TUbufferPtrs [ 2 ] = ( void * ) & pBuffer [ 2 * 8192 ] ; } }
hjgrwuhwvr ( rtS , & rtDW . n3ncmzva14 ) ; jztd2wvlzd ( rtS , & rtDW .
dx1zpxayg3 ) ; { { real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 ,
8192 , sizeof ( real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus (
rtS , "tdelay memory allocation error" ) ; return ; } rtDW . hhnmd3q34u .
Tail = 0 ; rtDW . hhnmd3q34u . Head = 0 ; rtDW . hhnmd3q34u . Last = 0 ; rtDW
. hhnmd3q34u . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_kw3wsvjvtt ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . p3evevjx4y . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
p3evevjx4y . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . kzmgrvlihj . Tail = 0
; rtDW . kzmgrvlihj . Head = 0 ; rtDW . kzmgrvlihj . Last = 0 ; rtDW .
kzmgrvlihj . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_ben3dx2m34 ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . gei2va2yg1 . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
gei2va2yg1 . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . nftilndrdl . Tail = 0
; rtDW . nftilndrdl . Head = 0 ; rtDW . nftilndrdl . Last = 0 ; rtDW .
nftilndrdl . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_nlfm4vqjuk ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . ofjbrmvngv . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
ofjbrmvngv . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . jl4zgwwouj . Tail = 0
; rtDW . jl4zgwwouj . Head = 0 ; rtDW . jl4zgwwouj . Last = 0 ; rtDW .
jl4zgwwouj . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_byvexhkqgs ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . gfxye1rcxy . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
gfxye1rcxy . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . ntkdqjpfyg . Tail = 0
; rtDW . ntkdqjpfyg . Head = 0 ; rtDW . ntkdqjpfyg . Last = 0 ; rtDW .
ntkdqjpfyg . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_akk50h0opn ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . abjlexkzmf . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
abjlexkzmf . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . krie5kar0s . Tail = 0
; rtDW . krie5kar0s . Head = 0 ; rtDW . krie5kar0s . Last = 0 ; rtDW .
krie5kar0s . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_ngohlcso1t ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . ggw2hbirki . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
ggw2hbirki . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } }
kqrkri2pgl ( rtS , & rtDW . dg2ggauitn ) ; kqrkri2pgl ( rtS , & rtDW .
jvoeh2x3far ) ; f3onajihkb ( rtS , & rtDW . czm0w3hmwho ) ; { { real_T *
pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof ( real_T ) ) ;
if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . fcxoxl4xkw . Tail = 0
; rtDW . fcxoxl4xkw . Head = 0 ; rtDW . fcxoxl4xkw . Last = 0 ; rtDW .
fcxoxl4xkw . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_e1patchl4a ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . ocixwzdlgq . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
ocixwzdlgq . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . bzqpi0kjrz . Tail = 0
; rtDW . bzqpi0kjrz . Head = 0 ; rtDW . bzqpi0kjrz . Last = 0 ; rtDW .
bzqpi0kjrz . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_guedmf10ob ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . j24stv55k5 . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
j24stv55k5 . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . p5bf4c2tu4 . Tail = 0
; rtDW . p5bf4c2tu4 . Head = 0 ; rtDW . p5bf4c2tu4 . Last = 0 ; rtDW .
p5bf4c2tu4 . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_f3pcmrh0c0 ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . hoiximvd01 . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
hoiximvd01 . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . ekzddpzsi1 . Tail = 0
; rtDW . ekzddpzsi1 . Head = 0 ; rtDW . ekzddpzsi1 . Last = 0 ; rtDW .
ekzddpzsi1 . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_mvxvcm2f5o ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . gipv43m3ie . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
gipv43m3ie . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . hz4fywtkdv . Tail = 0
; rtDW . hz4fywtkdv . Head = 0 ; rtDW . hz4fywtkdv . Last = 0 ; rtDW .
hz4fywtkdv . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_n5zytx3f40 ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . n450lruhp3 . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
n450lruhp3 . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . clbwz4dwil . Tail = 0
; rtDW . clbwz4dwil . Head = 0 ; rtDW . clbwz4dwil . Last = 0 ; rtDW .
clbwz4dwil . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_awceswb44n ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . ptmzalvxqw . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
ptmzalvxqw . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } }
kqrkri2pgl ( rtS , & rtDW . mb5y4fcaem ) ; kqrkri2pgl ( rtS , & rtDW .
no1v55ban3 ) ; f3onajihkb ( rtS , & rtDW . pgdbd0g5cb ) ; { { real_T *
pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof ( real_T ) ) ;
if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . bj3l3j2st3 . Tail = 0
; rtDW . bj3l3j2st3 . Head = 0 ; rtDW . bj3l3j2st3 . Last = 0 ; rtDW .
bj3l3j2st3 . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_aw5k4laehm ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . akp2w04qvg . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
akp2w04qvg . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . fdisg0vnhk . Tail = 0
; rtDW . fdisg0vnhk . Head = 0 ; rtDW . fdisg0vnhk . Last = 0 ; rtDW .
fdisg0vnhk . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_a23qsuusmp ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . n50ive3hhm . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
n50ive3hhm . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . nvxugce5ur . Tail = 0
; rtDW . nvxugce5ur . Head = 0 ; rtDW . nvxugce5ur . Last = 0 ; rtDW .
nvxugce5ur . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_fgetemlf4v ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . ej00tpn0xt . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
ej00tpn0xt . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . lufsk3t3r2 . Tail = 0
; rtDW . lufsk3t3r2 . Head = 0 ; rtDW . lufsk3t3r2 . Last = 0 ; rtDW .
lufsk3t3r2 . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_prtl4epgd1 ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . ilhkq3otms . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
ilhkq3otms . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . p1vhfh3p1a . Tail = 0
; rtDW . p1vhfh3p1a . Head = 0 ; rtDW . p1vhfh3p1a . Last = 0 ; rtDW .
p1vhfh3p1a . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_n1jopoie0t ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . gvpng3pvrl . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
gvpng3pvrl . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . h3jb1xttev . Tail = 0
; rtDW . h3jb1xttev . Head = 0 ; rtDW . h3jb1xttev . Last = 0 ; rtDW .
h3jb1xttev . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_nayvtolcs2 ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . cfwoq0f2ti . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
cfwoq0f2ti . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } }
kqrkri2pgl ( rtS , & rtDW . p4hmlcjayz ) ; kqrkri2pgl ( rtS , & rtDW .
dem3so030k ) ; f3onajihkb ( rtS , & rtDW . mvaehis2rc ) ; { { real_T *
pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof ( real_T ) ) ;
if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . km2x20r4ek . Tail = 0
; rtDW . km2x20r4ek . Head = 0 ; rtDW . km2x20r4ek . Last = 0 ; rtDW .
km2x20r4ek . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_j0jcaphlgu ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . ecebamajhh . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
ecebamajhh . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . ddmxqb4sul . Tail = 0
; rtDW . ddmxqb4sul . Head = 0 ; rtDW . ddmxqb4sul . Last = 0 ; rtDW .
ddmxqb4sul . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_gsjrmrtbdf ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . ogxech4wih . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
ogxech4wih . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . d11oepymjs . Tail = 0
; rtDW . d11oepymjs . Head = 0 ; rtDW . d11oepymjs . Last = 0 ; rtDW .
d11oepymjs . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_cbxjnchowy ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . j1jrbblzw5 . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
j1jrbblzw5 . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . e311nuuqb5 . Tail = 0
; rtDW . e311nuuqb5 . Head = 0 ; rtDW . e311nuuqb5 . Last = 0 ; rtDW .
e311nuuqb5 . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_gssa5ux2lo ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . hkkzlg4m02 . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
hkkzlg4m02 . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . g0lsoknhdx . Tail = 0
; rtDW . g0lsoknhdx . Head = 0 ; rtDW . g0lsoknhdx . Last = 0 ; rtDW .
g0lsoknhdx . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_inbo2ulujt ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . hva2h0mh4k . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
hva2h0mh4k . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . j1sal1hher . Tail = 0
; rtDW . j1sal1hher . Head = 0 ; rtDW . j1sal1hher . Last = 0 ; rtDW .
j1sal1hher . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_cw4myqmahz ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . crpy1zhjkz . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
crpy1zhjkz . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } }
kqrkri2pgl ( rtS , & rtDW . gjmkpqrmc3 ) ; kqrkri2pgl ( rtS , & rtDW .
khd3qxar1i ) ; f3onajihkb ( rtS , & rtDW . jzsygc4sa4 ) ; { { real_T *
pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof ( real_T ) ) ;
if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . dfz14rvn5j . Tail = 0
; rtDW . dfz14rvn5j . Head = 0 ; rtDW . dfz14rvn5j . Last = 0 ; rtDW .
dfz14rvn5j . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_eoidu31cxu ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . k5ejkktore . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
k5ejkktore . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . aic2pxcqxi . Tail = 0
; rtDW . aic2pxcqxi . Head = 0 ; rtDW . aic2pxcqxi . Last = 0 ; rtDW .
aic2pxcqxi . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_bnlvzhlygt ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . nvyaxvbcrw . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
nvyaxvbcrw . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } }
MdlInitialize ( ) ; MdlEnable ( ) ; } void MdlOutputs ( int_T tid ) { real_T
maxV ; boolean_T didZcEventOccur ; real_T riseValLimit ; int32_T i ; real_T
u0 ; real_T u2 ; srClearBC ( rtDW . bhc3qgnuicw . fx2esogudmq . fdd1jx2t0b )
; srClearBC ( rtDW . bhc3qgnuicw . cgvj5xjnjkh . kq4t2lkn4n ) ; srClearBC (
rtDW . bhc3qgnuicw . itcd43cpffs . hjywrjdfvg ) ; srClearBC ( rtDW .
bhc3qgnuicw . c3fzgpzsmi ) ; srClearBC ( rtDW . on20slldft0 . pbtm3vaf5b ) ;
srClearBC ( rtDW . kseqhf442uv . fsmzugaw0h ) ; srClearBC ( rtDW .
cgsqxgexc24 . kdh1m4autw ) ; srClearBC ( rtDW . eeuhsl20cs ) ; srClearBC (
rtDW . jvoeh2x3far . iwk0frtfal ) ; srClearBC ( rtDW . czm0w3hmwho .
mjsgo5sz1r ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { { real_T accum ; int_T
* switch_status = ( int_T * ) rtDW . orezesugg1 . SWITCH_STATUS ; int_T *
switch_status_init = ( int_T * ) rtDW . orezesugg1 . SWITCH_STATUS_INIT ;
int_T * SwitchChange = ( int_T * ) rtDW . orezesugg1 . SW_CHG ; int_T *
gState = ( int_T * ) rtDW . orezesugg1 . G_STATE ; real_T * yswitch = (
real_T * ) rtDW . orezesugg1 . Y_SWITCH ; int_T * switchTypes = ( int_T * )
rtDW . orezesugg1 . SWITCH_TYPES ; int_T * idxOutSw = ( int_T * ) rtDW .
orezesugg1 . IDX_OUT_SW ; real_T * DxCol = ( real_T * ) rtDW . orezesugg1 .
DX_COL ; real_T * tmp2 = ( real_T * ) rtDW . orezesugg1 . TMP2 ; real_T *
uswlast = ( real_T * ) rtDW . orezesugg1 . USWLAST ; int_T newState ; int_T
swChanged = 0 ; int loopsToDo = 20 ; real_T temp ; memcpy (
switch_status_init , switch_status , 3 * sizeof ( int_T ) ) ; memcpy (
uswlast , & rtB . era5tgw0hm [ 0 ] , 3 * sizeof ( real_T ) ) ; do { if (
loopsToDo == 1 ) { swChanged = 0 ; { int_T i1 ; for ( i1 = 0 ; i1 < 3 ; i1 ++
) { swChanged = ( ( SwitchChange [ i1 ] = switch_status_init [ i1 ] -
switch_status [ i1 ] ) != 0 ) ? 1 : swChanged ; switch_status [ i1 ] =
switch_status_init [ i1 ] ; } } } else { real_T * Ds = ( real_T * ) rtDW .
orezesugg1 . DS ; accum = 0.0 ; accum += * ( Ds ++ ) * rtP .
SwitchCurrents_Value [ 0 ] ; accum += * ( Ds ++ ) * rtP .
SwitchCurrents_Value [ 1 ] ; accum += * ( Ds ++ ) * rtP .
SwitchCurrents_Value [ 2 ] ; rtB . era5tgw0hm [ 0 ] = accum ; accum = 0.0 ;
accum += * ( Ds ++ ) * rtP . SwitchCurrents_Value [ 0 ] ; accum += * ( Ds ++
) * rtP . SwitchCurrents_Value [ 1 ] ; accum += * ( Ds ++ ) * rtP .
SwitchCurrents_Value [ 2 ] ; rtB . era5tgw0hm [ 1 ] = accum ; accum = 0.0 ;
accum += * ( Ds ++ ) * rtP . SwitchCurrents_Value [ 0 ] ; accum += * ( Ds ++
) * rtP . SwitchCurrents_Value [ 1 ] ; accum += * ( Ds ++ ) * rtP .
SwitchCurrents_Value [ 2 ] ; rtB . era5tgw0hm [ 2 ] = accum ; swChanged = 0 ;
newState = ( gState [ 0 ] > 0 ) ? 1 : ( ( rtB . era5tgw0hm [ 0 ] * uswlast [
0 ] < 0.0 ) ? 0 : switch_status [ 0 ] ) ; swChanged = ( ( SwitchChange [ 0 ]
= newState - switch_status [ 0 ] ) != 0 ) ? 1 : swChanged ; switch_status [ 0
] = newState ; newState = ( gState [ 1 ] > 0 ) ? 1 : ( ( rtB . era5tgw0hm [ 1
] * uswlast [ 1 ] < 0.0 ) ? 0 : switch_status [ 1 ] ) ; swChanged = ( (
SwitchChange [ 1 ] = newState - switch_status [ 1 ] ) != 0 ) ? 1 : swChanged
; switch_status [ 1 ] = newState ; newState = ( gState [ 2 ] > 0 ) ? 1 : ( (
rtB . era5tgw0hm [ 2 ] * uswlast [ 2 ] < 0.0 ) ? 0 : switch_status [ 2 ] ) ;
swChanged = ( ( SwitchChange [ 2 ] = newState - switch_status [ 2 ] ) != 0 )
? 1 : swChanged ; switch_status [ 2 ] = newState ; } if ( swChanged ) {
real_T * Ds = ( real_T * ) rtDW . orezesugg1 . DS ; real_T a1 ; { int_T i1 ;
for ( i1 = 0 ; i1 < 3 ; i1 ++ ) { if ( SwitchChange [ i1 ] != 0 ) { a1 =
1000.0 * SwitchChange [ i1 ] ; temp = 1 / ( 1 - Ds [ i1 * 4 ] * a1 ) ; DxCol
[ 0 ] = Ds [ 0 + i1 ] * temp * a1 ; DxCol [ 1 ] = Ds [ 3 + i1 ] * temp * a1 ;
DxCol [ 2 ] = Ds [ 6 + i1 ] * temp * a1 ; DxCol [ i1 ] = temp ; memcpy ( tmp2
, & Ds [ i1 * 3 ] , 3 * sizeof ( real_T ) ) ; memset ( & Ds [ i1 * 3 ] , '\0'
, 3 * sizeof ( real_T ) ) ; a1 = DxCol [ 0 ] ; Ds [ 0 + 0 ] += a1 * tmp2 [ 0
] ; Ds [ 0 + 1 ] += a1 * tmp2 [ 1 ] ; Ds [ 0 + 2 ] += a1 * tmp2 [ 2 ] ; a1 =
DxCol [ 1 ] ; Ds [ 3 + 0 ] += a1 * tmp2 [ 0 ] ; Ds [ 3 + 1 ] += a1 * tmp2 [ 1
] ; Ds [ 3 + 2 ] += a1 * tmp2 [ 2 ] ; a1 = DxCol [ 2 ] ; Ds [ 6 + 0 ] += a1 *
tmp2 [ 0 ] ; Ds [ 6 + 1 ] += a1 * tmp2 [ 1 ] ; Ds [ 6 + 2 ] += a1 * tmp2 [ 2
] ; } } } } } while ( swChanged > 0 && -- loopsToDo > 0 ) ; if ( loopsToDo ==
0 ) { real_T * Ds = ( real_T * ) rtDW . orezesugg1 . DS ; accum = 0.0 ; accum
+= * ( Ds ++ ) * rtP . SwitchCurrents_Value [ 0 ] ; accum += * ( Ds ++ ) *
rtP . SwitchCurrents_Value [ 1 ] ; accum += * ( Ds ++ ) * rtP .
SwitchCurrents_Value [ 2 ] ; rtB . era5tgw0hm [ 0 ] = accum ; accum = 0.0 ;
accum += * ( Ds ++ ) * rtP . SwitchCurrents_Value [ 0 ] ; accum += * ( Ds ++
) * rtP . SwitchCurrents_Value [ 1 ] ; accum += * ( Ds ++ ) * rtP .
SwitchCurrents_Value [ 2 ] ; rtB . era5tgw0hm [ 1 ] = accum ; accum = 0.0 ;
accum += * ( Ds ++ ) * rtP . SwitchCurrents_Value [ 0 ] ; accum += * ( Ds ++
) * rtP . SwitchCurrents_Value [ 1 ] ; accum += * ( Ds ++ ) * rtP .
SwitchCurrents_Value [ 2 ] ; rtB . era5tgw0hm [ 2 ] = accum ; } rtB .
jv25njllkq [ 0 ] = ( real_T ) switch_status [ 0 ] ; rtB . jv25njllkq [ 1 ] =
( real_T ) switch_status [ 1 ] ; rtB . jv25njllkq [ 2 ] = ( real_T )
switch_status [ 2 ] ; } if ( rtDW . cze1jlpasq != 0 ) { maxV =
6.2831853071795862 * rtP . System1_Frequency ; rtDW . gml0jqyexc =
muDoubleScalarSin ( maxV * ssGetTaskTime ( rtS , 1 ) ) ; maxV =
6.2831853071795862 * rtP . System1_Frequency ; rtDW . in1aihe14k =
muDoubleScalarCos ( maxV * ssGetTaskTime ( rtS , 1 ) ) ; rtDW . cze1jlpasq =
0 ; } rtB . hjwgkuat0w = ( ( rtDW . gml0jqyexc * rtP . SineWaveA_PCos + rtDW
. in1aihe14k * rtP . SineWaveA_PSin ) * rtP . SineWaveA_HCos + ( rtDW .
in1aihe14k * rtP . SineWaveA_PCos - rtDW . gml0jqyexc * rtP . SineWaveA_PSin
) * rtP . SineWaveA_Hsin ) * rtP . SineWaveA_Amp + rtP . SineWaveA_Bias ; if
( rtDW . kdbkzvy4xq != 0 ) { maxV = 6.2831853071795862 * rtP .
System1_Frequency ; rtDW . mnbe3gzdef = muDoubleScalarSin ( maxV *
ssGetTaskTime ( rtS , 1 ) ) ; maxV = 6.2831853071795862 * rtP .
System1_Frequency ; rtDW . k1ekplz15z = muDoubleScalarCos ( maxV *
ssGetTaskTime ( rtS , 1 ) ) ; rtDW . kdbkzvy4xq = 0 ; } rtB . irtkmeqegx = (
( rtDW . mnbe3gzdef * rtP . SineWaveB_PCos + rtDW . k1ekplz15z * rtP .
SineWaveB_PSin ) * rtP . SineWaveB_HCos + ( rtDW . k1ekplz15z * rtP .
SineWaveB_PCos - rtDW . mnbe3gzdef * rtP . SineWaveB_PSin ) * rtP .
SineWaveB_Hsin ) * rtP . SineWaveB_Amp + rtP . SineWaveB_Bias ; if ( rtDW .
ct3sr1xnt5 != 0 ) { maxV = 6.2831853071795862 * rtP . System1_Frequency ;
rtDW . apdqz11wuc = muDoubleScalarSin ( maxV * ssGetTaskTime ( rtS , 1 ) ) ;
maxV = 6.2831853071795862 * rtP . System1_Frequency ; rtDW . ili5cmy1bf =
muDoubleScalarCos ( maxV * ssGetTaskTime ( rtS , 1 ) ) ; rtDW . ct3sr1xnt5 =
0 ; } rtB . idp1uwrp5z = ( ( rtDW . apdqz11wuc * rtP . SineWaveC_PCos + rtDW
. ili5cmy1bf * rtP . SineWaveC_PSin ) * rtP . SineWaveC_HCos + ( rtDW .
ili5cmy1bf * rtP . SineWaveC_PCos - rtDW . apdqz11wuc * rtP . SineWaveC_PSin
) * rtP . SineWaveC_Hsin ) * rtP . SineWaveC_Amp + rtP . SineWaveC_Bias ; if
( rtDW . fnal3jedat != 0 ) { maxV = 6.2831853071795862 * rtP .
System4_Frequency ; rtDW . dobgip2gsc = muDoubleScalarSin ( maxV *
ssGetTaskTime ( rtS , 1 ) ) ; maxV = 6.2831853071795862 * rtP .
System4_Frequency ; rtDW . idflrqbwhk = muDoubleScalarCos ( maxV *
ssGetTaskTime ( rtS , 1 ) ) ; rtDW . fnal3jedat = 0 ; } rtB . oev0pyxhk5 = (
( rtDW . dobgip2gsc * rtP . SineWaveA_PCos_dhftpvm54k + rtDW . idflrqbwhk *
rtP . SineWaveA_PSin_j0jhvxxpyu ) * rtP . SineWaveA_HCos_pemhijovuq + ( rtDW
. idflrqbwhk * rtP . SineWaveA_PCos_dhftpvm54k - rtDW . dobgip2gsc * rtP .
SineWaveA_PSin_j0jhvxxpyu ) * rtP . SineWaveA_Hsin_gtf0muyyo1 ) * rtP .
SineWaveA_Amp_frnp5ucxk1 + rtP . SineWaveA_Bias_bcppaj1cjv ; if ( rtDW .
h4o3nqfxrq != 0 ) { maxV = 6.2831853071795862 * rtP . System4_Frequency ;
rtDW . krfjbq5jxu = muDoubleScalarSin ( maxV * ssGetTaskTime ( rtS , 1 ) ) ;
maxV = 6.2831853071795862 * rtP . System4_Frequency ; rtDW . hrbvhjj0hr =
muDoubleScalarCos ( maxV * ssGetTaskTime ( rtS , 1 ) ) ; rtDW . h4o3nqfxrq =
0 ; } rtB . i3sk0wialc = ( ( rtDW . krfjbq5jxu * rtP .
SineWaveB_PCos_n1aem3ap0q + rtDW . hrbvhjj0hr * rtP .
SineWaveB_PSin_ickyjsyajs ) * rtP . SineWaveB_HCos_o3i2ap52lt + ( rtDW .
hrbvhjj0hr * rtP . SineWaveB_PCos_n1aem3ap0q - rtDW . krfjbq5jxu * rtP .
SineWaveB_PSin_ickyjsyajs ) * rtP . SineWaveB_Hsin_kfkb1n1f0h ) * rtP .
SineWaveB_Amp_bzpo2grloq + rtP . SineWaveB_Bias_fwkba4egz2 ; if ( rtDW .
gghl4uhzpz != 0 ) { maxV = 6.2831853071795862 * rtP . System4_Frequency ;
rtDW . fh3tch5sc2 = muDoubleScalarSin ( maxV * ssGetTaskTime ( rtS , 1 ) ) ;
maxV = 6.2831853071795862 * rtP . System4_Frequency ; rtDW . hoyxwcln0j =
muDoubleScalarCos ( maxV * ssGetTaskTime ( rtS , 1 ) ) ; rtDW . gghl4uhzpz =
0 ; } rtB . flmfghomli = ( ( rtDW . fh3tch5sc2 * rtP .
SineWaveC_PCos_ircs0plqoh + rtDW . hoyxwcln0j * rtP .
SineWaveC_PSin_akytznrapm ) * rtP . SineWaveC_HCos_kscsjgvg02 + ( rtDW .
hoyxwcln0j * rtP . SineWaveC_PCos_ircs0plqoh - rtDW . fh3tch5sc2 * rtP .
SineWaveC_PSin_akytznrapm ) * rtP . SineWaveC_Hsin_edp2rwbke5 ) * rtP .
SineWaveC_Amp_gvcbc1bqac + rtP . SineWaveC_Bias_kqyhnw4pld ; { real_T accum ;
int_T * switch_status = ( int_T * ) rtDW . g45z4iebyu . SWITCH_STATUS ; int_T
* switch_status_init = ( int_T * ) rtDW . g45z4iebyu . SWITCH_STATUS_INIT ;
int_T * SwitchChange = ( int_T * ) rtDW . g45z4iebyu . SW_CHG ; int_T *
gState = ( int_T * ) rtDW . g45z4iebyu . G_STATE ; real_T * yswitch = (
real_T * ) rtDW . g45z4iebyu . Y_SWITCH ; int_T * switchTypes = ( int_T * )
rtDW . g45z4iebyu . SWITCH_TYPES ; int_T * idxOutSw = ( int_T * ) rtDW .
g45z4iebyu . IDX_OUT_SW ; real_T * DxCol = ( real_T * ) rtDW . g45z4iebyu .
DX_COL ; real_T * tmp2 = ( real_T * ) rtDW . g45z4iebyu . TMP2 ; real_T *
BDcol = ( real_T * ) rtDW . g45z4iebyu . BD_COL ; real_T * tmp1 = ( real_T *
) rtDW . g45z4iebyu . TMP1 ; real_T * uswlast = ( real_T * ) rtDW .
g45z4iebyu . USWLAST ; int_T newState ; int_T swChanged = 0 ; int loopsToDo =
20 ; real_T temp ; memcpy ( switch_status_init , switch_status , 6 * sizeof (
int_T ) ) ; memcpy ( uswlast , & rtB . hwsskqat2q [ 0 ] , 6 * sizeof ( real_T
) ) ; do { if ( loopsToDo == 1 ) { swChanged = 0 ; { int_T i1 ; for ( i1 = 0
; i1 < 6 ; i1 ++ ) { swChanged = ( ( SwitchChange [ i1 ] = switch_status_init
[ i1 ] - switch_status [ i1 ] ) != 0 ) ? 1 : swChanged ; switch_status [ i1 ]
= switch_status_init [ i1 ] ; } } } else { real_T * Cs = ( real_T * ) rtDW .
g45z4iebyu . CS ; real_T * Ds = ( real_T * ) rtDW . g45z4iebyu . DS ; { int_T
i1 ; real_T * y0 = & rtB . hwsskqat2q [ 0 ] ; for ( i1 = 0 ; i1 < 48 ; i1 ++
) { accum = 0.0 ; { int_T i2 ; real_T * xd = & rtDW . fn3owwkp0o [ 0 ] ; for
( i2 = 0 ; i2 < 6 ; i2 ++ ) { accum += * ( Cs ++ ) * xd [ i2 ] ; } } { int_T
i2 ; const real_T * u0 = & rtP . SwitchCurrents_Value_k4xgdjuoaj [ 0 ] ; for
( i2 = 0 ; i2 < 6 ; i2 ++ ) { accum += * ( Ds ++ ) * u0 [ i2 ] ; } accum += *
( Ds ++ ) * rtB . hjwgkuat0w ; accum += * ( Ds ++ ) * rtB . irtkmeqegx ;
accum += * ( Ds ++ ) * rtB . idp1uwrp5z ; accum += * ( Ds ++ ) * rtB .
oev0pyxhk5 ; accum += * ( Ds ++ ) * rtB . i3sk0wialc ; accum += * ( Ds ++ ) *
rtB . flmfghomli ; } y0 [ i1 ] = accum ; } } swChanged = 0 ; { int_T i1 ;
real_T * y0 = & rtB . hwsskqat2q [ 0 ] ; for ( i1 = 0 ; i1 < 6 ; i1 ++ ) {
newState = ( gState [ i1 ] > 0 ) ? 1 : ( ( y0 [ i1 ] * uswlast [ i1 ] < 0.0 )
? 0 : switch_status [ i1 ] ) ; swChanged = ( ( SwitchChange [ i1 ] = newState
- switch_status [ i1 ] ) != 0 ) ? 1 : swChanged ; switch_status [ i1 ] =
newState ; } } } if ( swChanged ) { real_T * As = ( real_T * ) rtDW .
g45z4iebyu . AS ; real_T * Cs = ( real_T * ) rtDW . g45z4iebyu . CS ; real_T
* Bs = ( real_T * ) rtDW . g45z4iebyu . BS ; real_T * Ds = ( real_T * ) rtDW
. g45z4iebyu . DS ; real_T a1 ; { int_T i1 ; for ( i1 = 0 ; i1 < 6 ; i1 ++ )
{ if ( SwitchChange [ i1 ] != 0 ) { a1 = yswitch [ i1 ] * SwitchChange [ i1 ]
; temp = 1 / ( 1 - Ds [ i1 * 13 ] * a1 ) ; { int_T i2 ; for ( i2 = 0 ; i2 <
48 ; i2 ++ ) { DxCol [ i2 ] = Ds [ i2 * 12 + i1 ] * temp * a1 ; } } DxCol [
i1 ] = temp ; { int_T i2 ; for ( i2 = 0 ; i2 < 6 ; i2 ++ ) { BDcol [ i2 ] =
Bs [ i2 * 12 + i1 ] * a1 ; } } memcpy ( tmp1 , & Cs [ i1 * 6 ] , 6 * sizeof (
real_T ) ) ; memset ( & Cs [ i1 * 6 ] , '\0' , 6 * sizeof ( real_T ) ) ;
memcpy ( tmp2 , & Ds [ i1 * 12 ] , 12 * sizeof ( real_T ) ) ; memset ( & Ds [
i1 * 12 ] , '\0' , 12 * sizeof ( real_T ) ) ; { int_T i2 ; for ( i2 = 0 ; i2
< 48 ; i2 ++ ) { a1 = DxCol [ i2 ] ; { int_T i3 ; for ( i3 = 0 ; i3 < 6 ; i3
++ ) { Cs [ i2 * 6 + i3 ] += a1 * tmp1 [ i3 ] ; } } { int_T i3 ; for ( i3 = 0
; i3 < 12 ; i3 ++ ) { Ds [ i2 * 12 + i3 ] += a1 * tmp2 [ i3 ] ; } } } } {
int_T i2 ; for ( i2 = 0 ; i2 < 6 ; i2 ++ ) { a1 = BDcol [ i2 ] ; { int_T i3 ;
for ( i3 = 0 ; i3 < 6 ; i3 ++ ) { As [ i2 * 6 + i3 ] += a1 * Cs [ i1 * 6 + i3
] ; } } { int_T i3 ; for ( i3 = 0 ; i3 < 12 ; i3 ++ ) { Bs [ i2 * 12 + i3 ]
+= a1 * Ds [ i1 * 12 + i3 ] ; } } } } } } } } } while ( swChanged > 0 && --
loopsToDo > 0 ) ; if ( loopsToDo == 0 ) { real_T * Cs = ( real_T * ) rtDW .
g45z4iebyu . CS ; real_T * Ds = ( real_T * ) rtDW . g45z4iebyu . DS ; { int_T
i1 ; real_T * y0 = & rtB . hwsskqat2q [ 0 ] ; for ( i1 = 0 ; i1 < 48 ; i1 ++
) { accum = 0.0 ; { int_T i2 ; real_T * xd = & rtDW . fn3owwkp0o [ 0 ] ; for
( i2 = 0 ; i2 < 6 ; i2 ++ ) { accum += * ( Cs ++ ) * xd [ i2 ] ; } } { int_T
i2 ; const real_T * u0 = & rtP . SwitchCurrents_Value_k4xgdjuoaj [ 0 ] ; for
( i2 = 0 ; i2 < 6 ; i2 ++ ) { accum += * ( Ds ++ ) * u0 [ i2 ] ; } accum += *
( Ds ++ ) * rtB . hjwgkuat0w ; accum += * ( Ds ++ ) * rtB . irtkmeqegx ;
accum += * ( Ds ++ ) * rtB . idp1uwrp5z ; accum += * ( Ds ++ ) * rtB .
oev0pyxhk5 ; accum += * ( Ds ++ ) * rtB . i3sk0wialc ; accum += * ( Ds ++ ) *
rtB . flmfghomli ; } y0 [ i1 ] = accum ; } } } { int_T i1 ; real_T * y1 = &
rtB . n50h5vifld [ 0 ] ; for ( i1 = 0 ; i1 < 6 ; i1 ++ ) { y1 [ i1 ] = (
real_T ) switch_status [ i1 ] ; } } } } rtB . oqeo1ubqos = rtX . apevsa2hcq ;
{ real_T * * uBuffer = ( real_T * * ) & rtDW . grt2oyi0sr . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & rtDW . grt2oyi0sr . TUbufferPtrs [
1 ] ; real_T simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay ; rtB . kv13gp1fgb = rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , rtDW . ixae4mywgu . CircularBufSize , & rtDW
. ixae4mywgu . Last , rtDW . ixae4mywgu . Tail , rtDW . ixae4mywgu . Head ,
rtP . TransportDelay_InitOutput , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( rtS
) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
oio4uwxmmm = ssGetT ( rtS ) ; rtB . mbzobkv3nj = ( rtB . oio4uwxmmm >= rtP .
K1_Value ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . nyteniid5b = rtDW .
bmewgg5lxa ; } if ( rtB . mbzobkv3nj ) { rtB . adgh5nos5f = rtB . oqeo1ubqos
- rtB . kv13gp1fgb ; rtB . luv5ayj2vo = rtP . Gain_Gain_lm2gn15tep * rtB .
adgh5nos5f ; rtB . p5nfnzin0c = rtB . luv5ayj2vo ; } else { rtB . p5nfnzin0c
= rtB . nyteniid5b ; } rtB . b5pj0x41v0 = rtX . ebz5npnaoh ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . pl0qrajpw4 . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . pl0qrajpw4 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_oe42moefhj ; rtB . mpkq5nnsja = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . ja00u1cqz3 .
CircularBufSize , & rtDW . ja00u1cqz3 . Last , rtDW . ja00u1cqz3 . Tail ,
rtDW . ja00u1cqz3 . Head , rtP . TransportDelay_InitOutput_eaxnllrdum , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . krpdgaof1i = ssGetT ( rtS ) ; rtB . nk5qpu1zmt
= ( rtB . krpdgaof1i >= rtP . K1_Value_lv2oy5msqf ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . imtswccya1 = rtDW . hztiwvsh4t ; } if ( rtB .
nk5qpu1zmt ) { rtB . kyavs0ao42 = rtB . b5pj0x41v0 - rtB . mpkq5nnsja ; rtB .
kaq3pxdfrq = rtP . Gain_Gain_dhy2dr5cad * rtB . kyavs0ao42 ; rtB . cjtwmfzpsv
= rtB . kaq3pxdfrq ; } else { rtB . cjtwmfzpsv = rtB . imtswccya1 ; } rtB .
o4jlcui2oj . re = rtB . p5nfnzin0c ; rtB . o4jlcui2oj . im = rtB . cjtwmfzpsv
; rtB . mwsaw5g1sb = muDoubleScalarHypot ( rtB . o4jlcui2oj . re , rtB .
o4jlcui2oj . im ) ; rtB . m1mvcrodu3 = muDoubleScalarAtan2 ( rtB . o4jlcui2oj
. im , rtB . o4jlcui2oj . re ) ; rtB . lal3cjzlj5 = rtX . g5pl2ihnsa ; {
real_T * * uBuffer = ( real_T * * ) & rtDW . ax1z1sifrs . TUbufferPtrs [ 0 ]
; real_T * * tBuffer = ( real_T * * ) & rtDW . ax1z1sifrs . TUbufferPtrs [ 1
] ; real_T simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_l554jo5rku ; rtB . cezdwms0ii = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . iepysspqfq .
CircularBufSize , & rtDW . iepysspqfq . Last , rtDW . iepysspqfq . Tail ,
rtDW . iepysspqfq . Head , rtP . TransportDelay_InitOutput_nqf1d1yteg , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . eybhk0f1u3 = ssGetT ( rtS ) ; rtB . hoyrpteevh
= ( rtB . eybhk0f1u3 >= rtP . K1_Value_alzp1zxnlk ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . lseoh4bvxs = rtDW . a4bul5sdk4 ; } if ( rtB .
hoyrpteevh ) { rtB . jr2isij1u0 = rtB . lal3cjzlj5 - rtB . cezdwms0ii ; rtB .
hmwikqfebh = rtP . Gain_Gain_pm1hcm20z4 * rtB . jr2isij1u0 ; rtB . aq1qiraf44
= rtB . hmwikqfebh ; } else { rtB . aq1qiraf44 = rtB . lseoh4bvxs ; } rtB .
bhzqnkifhr = rtX . dzmtcwo21u ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. d5b3ghqdjb . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . d5b3ghqdjb . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_bpfmvxai5z ; rtB .
l1ylzsg02t = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . d5g4hqdsxq . CircularBufSize , & rtDW . d5g4hqdsxq . Last , rtDW .
d5g4hqdsxq . Tail , rtDW . d5g4hqdsxq . Head , rtP .
TransportDelay_InitOutput_jrvdftqeey , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
nkyw43xr3s = ssGetT ( rtS ) ; rtB . odu5awkgba = ( rtB . nkyw43xr3s >= rtP .
K1_Value_alxcwgywg5 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
auazbvfnyo = rtDW . ogsgimkanv ; } if ( rtB . odu5awkgba ) { rtB . jqrpn2ob1i
= rtB . bhzqnkifhr - rtB . l1ylzsg02t ; rtB . mbukfl0xm3 = rtP .
Gain_Gain_iyibkrypda * rtB . jqrpn2ob1i ; rtB . gosgmmpf3q = rtB . mbukfl0xm3
; } else { rtB . gosgmmpf3q = rtB . auazbvfnyo ; } rtB . gxkpczu332 . re =
rtB . aq1qiraf44 ; rtB . gxkpczu332 . im = rtB . gosgmmpf3q ; rtB .
lvnv2nzwp1 = muDoubleScalarHypot ( rtB . gxkpczu332 . re , rtB . gxkpczu332 .
im ) ; rtB . dcbd243rjt = muDoubleScalarAtan2 ( rtB . gxkpczu332 . im , rtB .
gxkpczu332 . re ) ; rtB . ffcf4214zj = rtX . luwntsjfce ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . d45fc0cu2n . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . d45fc0cu2n . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_njnzvljj3k ; rtB . igb01bl0lr = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . b2xp1t1hmq .
CircularBufSize , & rtDW . b2xp1t1hmq . Last , rtDW . b2xp1t1hmq . Tail ,
rtDW . b2xp1t1hmq . Head , rtP . TransportDelay_InitOutput_oo2wtyq0tx , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . ksdllg0qmd = ssGetT ( rtS ) ; rtB . fgi0kwqfls
= ( rtB . ksdllg0qmd >= rtP . K1_Value_n3kflqoywi ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . fbyyswiptt = rtDW . jb0wxz0ail ; } if ( rtB .
fgi0kwqfls ) { rtB . dboaxuzw4k = rtB . ffcf4214zj - rtB . igb01bl0lr ; rtB .
fzgklijbtx = rtP . Gain_Gain_klpinv4i1g * rtB . dboaxuzw4k ; rtB . cbmftdx00h
= rtB . fzgklijbtx ; } else { rtB . cbmftdx00h = rtB . fbyyswiptt ; } rtB .
kocoejgchf = rtX . lrou1be5ws ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. c5rfy4mzmq . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . c5rfy4mzmq . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_jb45sdc2uw ; rtB .
jsagzqkgk2 = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . ewf1j0idml . CircularBufSize , & rtDW . ewf1j0idml . Last , rtDW .
ewf1j0idml . Tail , rtDW . ewf1j0idml . Head , rtP .
TransportDelay_InitOutput_hafu4kbvs4 , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
bdyugoivqf = ssGetT ( rtS ) ; rtB . nvlbr2jcm5 = ( rtB . bdyugoivqf >= rtP .
K1_Value_paorddltde ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
ka31dmerxx = rtDW . ddy2duwo4c ; } if ( rtB . nvlbr2jcm5 ) { rtB . jw1ocfqcoe
= rtB . kocoejgchf - rtB . jsagzqkgk2 ; rtB . f0tdzxh3pm = rtP .
Gain_Gain_fx33utd1ql * rtB . jw1ocfqcoe ; rtB . dxwcs33v2q = rtB . f0tdzxh3pm
; } else { rtB . dxwcs33v2q = rtB . ka31dmerxx ; } rtB . kogi5awnd3 . re =
rtB . cbmftdx00h ; rtB . kogi5awnd3 . im = rtB . dxwcs33v2q ; rtB .
ir0mdju3wm = muDoubleScalarHypot ( rtB . kogi5awnd3 . re , rtB . kogi5awnd3 .
im ) ; rtB . blf5njdgxt = muDoubleScalarAtan2 ( rtB . kogi5awnd3 . im , rtB .
kogi5awnd3 . re ) ; rtB . cavjbaogp0 = rtX . kipcktetym ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . lmfxi4smjj . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . lmfxi4smjj . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_o3j3xceore ; rtB . gvclzjd2es = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . if32tdg5qk .
CircularBufSize , & rtDW . if32tdg5qk . Last , rtDW . if32tdg5qk . Tail ,
rtDW . if32tdg5qk . Head , rtP . TransportDelay_InitOutput_jatjvory4z , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . lkncuruhu1 = ssGetT ( rtS ) ; rtB . k20ijwhpoq
= ( rtB . lkncuruhu1 >= rtP . K1_Value_itoj2hmmt4 ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . h1ckdbmxog = rtDW . jhulgqvdvl ; } if ( rtB .
k20ijwhpoq ) { rtB . obv35jb1q1 = rtB . cavjbaogp0 - rtB . gvclzjd2es ; rtB .
ib5qko4x0z = rtP . Gain_Gain_pypf2w52io * rtB . obv35jb1q1 ; rtB . d41xayzc2a
= rtB . ib5qko4x0z ; } else { rtB . d41xayzc2a = rtB . h1ckdbmxog ; } rtB .
gmm1i11e3j = rtX . hogtze5t2d ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. d3lf52anqx . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . d3lf52anqx . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_fanj5ft5et ; rtB .
e3oaipje4a = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . jedhnolofy . CircularBufSize , & rtDW . jedhnolofy . Last , rtDW .
jedhnolofy . Tail , rtDW . jedhnolofy . Head , rtP .
TransportDelay_InitOutput_cru0r3mqsd , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
mq0tmzsngg = ssGetT ( rtS ) ; rtB . aybhprf3z4 = ( rtB . mq0tmzsngg >= rtP .
K1_Value_o3hgrmehvj ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
chlb0e4rs1 = rtDW . lq5puhde41 ; } if ( rtB . aybhprf3z4 ) { rtB . j4aqzc14bz
= rtB . gmm1i11e3j - rtB . e3oaipje4a ; rtB . mobqjsi0y2 = rtP . Gain_Gain *
rtB . j4aqzc14bz ; rtB . b54kkc2sai = rtB . mobqjsi0y2 ; } else { rtB .
b54kkc2sai = rtB . chlb0e4rs1 ; } rtB . bam1wg2wsd . re = rtB . d41xayzc2a ;
rtB . bam1wg2wsd . im = rtB . b54kkc2sai ; rtB . l1dkizi1sv =
muDoubleScalarHypot ( rtB . bam1wg2wsd . re , rtB . bam1wg2wsd . im ) ; rtB .
b4buuids5a = muDoubleScalarAtan2 ( rtB . bam1wg2wsd . im , rtB . bam1wg2wsd .
re ) ; rtB . kyr4zi2dy0 = rtX . aubyardb3x ; { real_T * * uBuffer = ( real_T
* * ) & rtDW . gvogk5wosq . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . gvogk5wosq . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_gr3febuaiv ; rtB . m21cdto2ks = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . a2f4jln130 .
CircularBufSize , & rtDW . a2f4jln130 . Last , rtDW . a2f4jln130 . Tail ,
rtDW . a2f4jln130 . Head , rtP . TransportDelay_InitOutput_cl0e2vz1q1 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . et1nvuymus = ssGetT ( rtS ) ; rtB . d1yofz10s5
= ( rtB . et1nvuymus >= rtP . K1_Value_o22zxmliou ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . jv13xtfqxr = rtDW . d1hkn1pn3x ; } if ( rtB .
d1yofz10s5 ) { rtB . it2vvzyksk = rtB . kyr4zi2dy0 - rtB . m21cdto2ks ; rtB .
kq52hevclf = rtP . Gain_Gain_atndypl5vf * rtB . it2vvzyksk ; rtB . m4l4wj2mnm
= rtB . kq52hevclf ; } else { rtB . m4l4wj2mnm = rtB . jv13xtfqxr ; } rtB .
hbfto5kcre = rtX . hummpur2b0 ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. innrerrg2w . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . innrerrg2w . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_m5tuijz25b ; rtB .
a2cirl4mor = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . h43kjr3444 . CircularBufSize , & rtDW . h43kjr3444 . Last , rtDW .
h43kjr3444 . Tail , rtDW . h43kjr3444 . Head , rtP .
TransportDelay_InitOutput_kvkmzazkam , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
cga1nacnne = ssGetT ( rtS ) ; rtB . lhbjnya5ri = ( rtB . cga1nacnne >= rtP .
K1_Value_gz0rvjwka3 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
lsva2y2qmr = rtDW . kidkiatkb2 ; } if ( rtB . lhbjnya5ri ) { rtB . mktcyg3aek
= rtB . hbfto5kcre - rtB . a2cirl4mor ; rtB . mf24wtkngs = rtP .
Gain_Gain_fup3my2rmn * rtB . mktcyg3aek ; rtB . kwecicm223 = rtB . mf24wtkngs
; } else { rtB . kwecicm223 = rtB . lsva2y2qmr ; } rtB . pz0iqe1t4h . re =
rtB . m4l4wj2mnm ; rtB . pz0iqe1t4h . im = rtB . kwecicm223 ; rtB .
k4obkkygio = muDoubleScalarHypot ( rtB . pz0iqe1t4h . re , rtB . pz0iqe1t4h .
im ) ; rtB . lyzpgvnlrs = muDoubleScalarAtan2 ( rtB . pz0iqe1t4h . im , rtB .
pz0iqe1t4h . re ) ; rtB . odsjx3ak1l = rtX . j0rirqtd1a ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . b3tfxdcqfh . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . b3tfxdcqfh . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_kfs5cq2cxd ; rtB . oidzayd4dg = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . gudqpjffwb .
CircularBufSize , & rtDW . gudqpjffwb . Last , rtDW . gudqpjffwb . Tail ,
rtDW . gudqpjffwb . Head , rtP . TransportDelay_InitOutput_b32jase2fn , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . kkaknnjqkc = ssGetT ( rtS ) ; rtB . f1thopguwf
= ( rtB . kkaknnjqkc >= rtP . K1_Value_jy1w4gbryp ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . cmcoxsp3eq = rtDW . oqqwr15jx2 ; } if ( rtB .
f1thopguwf ) { rtB . fko35lqny4 = rtB . odsjx3ak1l - rtB . oidzayd4dg ; rtB .
gdorhhnwb0 = rtP . Gain_Gain_dhfrkbao0i * rtB . fko35lqny4 ; rtB . l50se1h1ym
= rtB . gdorhhnwb0 ; } else { rtB . l50se1h1ym = rtB . cmcoxsp3eq ; } rtB .
bwlritkk2v = rtX . ip0opk45xq ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. ex4axikx2n . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . ex4axikx2n . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_mf3buvibkv ; rtB .
gqrsm3xa3j = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . ipe5rnt2je . CircularBufSize , & rtDW . ipe5rnt2je . Last , rtDW .
ipe5rnt2je . Tail , rtDW . ipe5rnt2je . Head , rtP .
TransportDelay_InitOutput_fzh1fdwm1e , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
fok0asoeif = ssGetT ( rtS ) ; rtB . ht23doxgow = ( rtB . fok0asoeif >= rtP .
K1_Value_np55ssasjq ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
lsbe3hb5y5 = rtDW . dgche5odby ; } if ( rtB . ht23doxgow ) { rtB . dec2gvw0uu
= rtB . bwlritkk2v - rtB . gqrsm3xa3j ; rtB . nrb2ceytaa = rtP .
Gain_Gain_krscra1c5o * rtB . dec2gvw0uu ; rtB . knvfyibp1b = rtB . nrb2ceytaa
; } else { rtB . knvfyibp1b = rtB . lsbe3hb5y5 ; } rtB . jcdmxgwx4o . re =
rtB . l50se1h1ym ; rtB . jcdmxgwx4o . im = rtB . knvfyibp1b ; rtB .
mxsx1mjxrn = muDoubleScalarHypot ( rtB . jcdmxgwx4o . re , rtB . jcdmxgwx4o .
im ) ; rtB . objbxbq42a = muDoubleScalarAtan2 ( rtB . jcdmxgwx4o . im , rtB .
jcdmxgwx4o . re ) ; rtB . fm5wjqawts = rtX . cyudzpjr5t ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . f5mcje12pk . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . f5mcje12pk . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_mpt4otaavv ; rtB . ld0mhxbkmt = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . deyhny3sye .
CircularBufSize , & rtDW . deyhny3sye . Last , rtDW . deyhny3sye . Tail ,
rtDW . deyhny3sye . Head , rtP . TransportDelay_InitOutput_pl4ev2e413 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . kbtu2idjq4 = ssGetT ( rtS ) ; rtB . ofpqdgy1zk
= ( rtB . kbtu2idjq4 >= rtP . K1_Value_obymey3y1k ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . akddxt5jm4 = rtDW . olgfgh4cjq ; } if ( rtB .
ofpqdgy1zk ) { rtB . mbhc0cscue = rtB . fm5wjqawts - rtB . ld0mhxbkmt ; rtB .
nuhkjsudxu = rtP . Gain_Gain_bx5ze0gvqv * rtB . mbhc0cscue ; rtB . kbtjvwenzh
= rtB . nuhkjsudxu ; } else { rtB . kbtjvwenzh = rtB . akddxt5jm4 ; } rtB .
h2vg1mucis = rtX . ky01z2j0r2 ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. oi1yc2ajy5 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . oi1yc2ajy5 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_l2cxshyix2 ; rtB .
b232kaqvb5 = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . abila4k5tf . CircularBufSize , & rtDW . abila4k5tf . Last , rtDW .
abila4k5tf . Tail , rtDW . abila4k5tf . Head , rtP .
TransportDelay_InitOutput_hx3xgrs1b1 , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
l1m02vberu = ssGetT ( rtS ) ; rtB . mhciobi4lh = ( rtB . l1m02vberu >= rtP .
K1_Value_mmpsusaepz ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
kllcqhvqnm = rtDW . mokcdmtlad ; } if ( rtB . mhciobi4lh ) { rtB . n4gcegtvuq
= rtB . h2vg1mucis - rtB . b232kaqvb5 ; rtB . f4kzzikjdt = rtP .
Gain_Gain_iuyo23xu44 * rtB . n4gcegtvuq ; rtB . j5zcghl4k1 = rtB . f4kzzikjdt
; } else { rtB . j5zcghl4k1 = rtB . kllcqhvqnm ; } rtB . obboa1pggi . re =
rtB . kbtjvwenzh ; rtB . obboa1pggi . im = rtB . j5zcghl4k1 ; rtB .
pvedmluia0 = muDoubleScalarHypot ( rtB . obboa1pggi . re , rtB . obboa1pggi .
im ) ; rtB . e3ydoz0bpk = muDoubleScalarAtan2 ( rtB . obboa1pggi . im , rtB .
obboa1pggi . re ) ; rtB . iir3yfosnh = rtX . nyvh4po1ze ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . c2kuyduabk . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . c2kuyduabk . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_apxflo13lg ; rtB . llxbdvdqnj = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . iwj0qe5gyd .
CircularBufSize , & rtDW . iwj0qe5gyd . Last , rtDW . iwj0qe5gyd . Tail ,
rtDW . iwj0qe5gyd . Head , rtP . TransportDelay_InitOutput_brwezlkbny , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . acael4dwgu = ssGetT ( rtS ) ; rtB . np0oeucr52
= ( rtB . acael4dwgu >= rtP . K1_Value_a3wrvws54e ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . ebpuiygqm4 = rtDW . iosgfiuyq5 ; } if ( rtB .
np0oeucr52 ) { rtB . ba3qgny3ih = rtB . iir3yfosnh - rtB . llxbdvdqnj ; rtB .
a03fioh2dm = rtP . Gain_Gain_d1iwhbcz0h * rtB . ba3qgny3ih ; rtB . kt12go5wl2
= rtB . a03fioh2dm ; } else { rtB . kt12go5wl2 = rtB . ebpuiygqm4 ; } rtB .
bel33c2xhe = rtX . fsa5rzt3eu ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. exexriqgpr . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . exexriqgpr . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_bnnallqkhz ; rtB .
p4vukpkrtr = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . fupmlcibf5 . CircularBufSize , & rtDW . fupmlcibf5 . Last , rtDW .
fupmlcibf5 . Tail , rtDW . fupmlcibf5 . Head , rtP .
TransportDelay_InitOutput_fge1igsmgb , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
m3x1ayzbqz = ssGetT ( rtS ) ; rtB . cg2x2bna1t = ( rtB . m3x1ayzbqz >= rtP .
K1_Value_k53ymkxen1 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
kjxywdmesq = rtDW . e5wjbjcaxs ; } if ( rtB . cg2x2bna1t ) { rtB . cqziwxkhgg
= rtB . bel33c2xhe - rtB . p4vukpkrtr ; rtB . bo3rjpigyn = rtP .
Gain_Gain_hdzu03r31p * rtB . cqziwxkhgg ; rtB . pzcfkojr0n = rtB . bo3rjpigyn
; } else { rtB . pzcfkojr0n = rtB . kjxywdmesq ; } rtB . ccz0dfwxmy . re =
rtB . kt12go5wl2 ; rtB . ccz0dfwxmy . im = rtB . pzcfkojr0n ; rtB .
dxg5ih1pwv = muDoubleScalarHypot ( rtB . ccz0dfwxmy . re , rtB . ccz0dfwxmy .
im ) ; rtB . cys5ttuvcv = muDoubleScalarAtan2 ( rtB . ccz0dfwxmy . im , rtB .
ccz0dfwxmy . re ) ; rtB . nwi4ojgvax = rtX . iivfgrp0dv ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . k0yj2b2ldz . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . k0yj2b2ldz . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_df0az4x0nr ; rtB . inys3ekeyq = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . iqlusmvwiw .
CircularBufSize , & rtDW . iqlusmvwiw . Last , rtDW . iqlusmvwiw . Tail ,
rtDW . iqlusmvwiw . Head , rtP . TransportDelay_InitOutput_jd3fo3ayg2 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . jzteyzckdx = ssGetT ( rtS ) ; rtB . gxdzxjcpth
= ( rtB . jzteyzckdx >= rtP . K1_Value_fyz1knu0y1 ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . gvaovcor4n = rtDW . g2sm2t4hw5 ; } if ( rtB .
gxdzxjcpth ) { rtB . mp2snbp1zj = rtB . nwi4ojgvax - rtB . inys3ekeyq ; rtB .
eboby24rhm = rtP . Gain_Gain_or2gwzcdbf * rtB . mp2snbp1zj ; rtB . ehvyl2mad3
= rtB . eboby24rhm ; } else { rtB . ehvyl2mad3 = rtB . gvaovcor4n ; } rtB .
mush31qdje = rtX . etzwqxlfek ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. lbtex4xafe . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . lbtex4xafe . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_hd1c0nxozi ; rtB .
mag53ao254 = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . gbcuvxshkr . CircularBufSize , & rtDW . gbcuvxshkr . Last , rtDW .
gbcuvxshkr . Tail , rtDW . gbcuvxshkr . Head , rtP .
TransportDelay_InitOutput_etrpvcln3b , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
im0gg02j5d = ssGetT ( rtS ) ; rtB . gl4k2ynbc5 = ( rtB . im0gg02j5d >= rtP .
K1_Value_ksg1aztoxm ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
n5ad3cbuva = rtDW . lva0bwt5gb ; } if ( rtB . gl4k2ynbc5 ) { rtB . ft0l5cid4c
= rtB . mush31qdje - rtB . mag53ao254 ; rtB . m4agf2pnjm = rtP .
Gain_Gain_hev2rqik2c * rtB . ft0l5cid4c ; rtB . lwfyukagvg = rtB . m4agf2pnjm
; } else { rtB . lwfyukagvg = rtB . n5ad3cbuva ; } rtB . ozrrigkzzv . re =
rtB . ehvyl2mad3 ; rtB . ozrrigkzzv . im = rtB . lwfyukagvg ; rtB .
od0lpdqczv = muDoubleScalarHypot ( rtB . ozrrigkzzv . re , rtB . ozrrigkzzv .
im ) ; rtB . hkcp1jr5hc = muDoubleScalarAtan2 ( rtB . ozrrigkzzv . im , rtB .
ozrrigkzzv . re ) ; rtB . lhwa0vyjtr = rtX . ikersjp5rv ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . dtlmsjjjpu . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . dtlmsjjjpu . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_pswpnmajkv ; rtB . am2xpz3ahc = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . ov4twhz4dl .
CircularBufSize , & rtDW . ov4twhz4dl . Last , rtDW . ov4twhz4dl . Tail ,
rtDW . ov4twhz4dl . Head , rtP . TransportDelay_InitOutput_ez4yh5jpxv , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . aefkabqz4r = ssGetT ( rtS ) ; rtB . l5wlsoqaie
= ( rtB . aefkabqz4r >= rtP . K1_Value_lp2doxbl3i ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . lummhzpqqc = rtDW . noyx1erxkm ; } if ( rtB .
l5wlsoqaie ) { rtB . e0wsar0tex = rtB . lhwa0vyjtr - rtB . am2xpz3ahc ; rtB .
g2gvqkkjh3 = rtP . Gain_Gain_p5vapoklif * rtB . e0wsar0tex ; rtB . bdlpjubuoi
= rtB . g2gvqkkjh3 ; } else { rtB . bdlpjubuoi = rtB . lummhzpqqc ; } rtB .
b2mrhddfvq = rtX . hgyqbdifk0 ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. l30jrhdejr . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . l30jrhdejr . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_ofwfa3phpb ; rtB .
erjh1rykrn = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . iqunqpo4a3 . CircularBufSize , & rtDW . iqunqpo4a3 . Last , rtDW .
iqunqpo4a3 . Tail , rtDW . iqunqpo4a3 . Head , rtP .
TransportDelay_InitOutput_ikh04lhagm , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
dkh3msmihq = ssGetT ( rtS ) ; rtB . hii3xnbogb = ( rtB . dkh3msmihq >= rtP .
K1_Value_e4trekxtkx ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
p31f5h1puv = rtDW . jsqsb2a31o ; } if ( rtB . hii3xnbogb ) { rtB . matyp1b0mc
= rtB . b2mrhddfvq - rtB . erjh1rykrn ; rtB . l4dzdb4hhd = rtP .
Gain_Gain_lk2y3y32w0 * rtB . matyp1b0mc ; rtB . bafm53nc12 = rtB . l4dzdb4hhd
; } else { rtB . bafm53nc12 = rtB . p31f5h1puv ; } rtB . c4jhkntylr . re =
rtB . bdlpjubuoi ; rtB . c4jhkntylr . im = rtB . bafm53nc12 ; rtB .
got1muihhn = muDoubleScalarHypot ( rtB . c4jhkntylr . re , rtB . c4jhkntylr .
im ) ; rtB . boudhhqt4b = muDoubleScalarAtan2 ( rtB . c4jhkntylr . im , rtB .
c4jhkntylr . re ) ; rtB . l5rvgavbvg = rtX . brjhw0iflo ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . hh24ilp1p1 . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . hh24ilp1p1 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_bbzq04uqkz ; rtB . jsg1wy4zc4 = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . gxzqgoeivi .
CircularBufSize , & rtDW . gxzqgoeivi . Last , rtDW . gxzqgoeivi . Tail ,
rtDW . gxzqgoeivi . Head , rtP . TransportDelay_InitOutput_mvzaf4esul , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . ktoxpdu5kz = ssGetT ( rtS ) ; rtB . efcaltfugb
= ( rtB . ktoxpdu5kz >= rtP . K1_Value_kiuz51y5sv ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . mkis3n3r5e = rtDW . krkllgqfro ; } if ( rtB .
efcaltfugb ) { rtB . a3s2mcvlkj = rtB . l5rvgavbvg - rtB . jsg1wy4zc4 ; rtB .
bh4i5s2i1q = rtP . Gain_Gain_fuvntptsyn * rtB . a3s2mcvlkj ; rtB . ppoxem35uq
= rtB . bh4i5s2i1q ; } else { rtB . ppoxem35uq = rtB . mkis3n3r5e ; } rtB .
eit2f5ruxh = rtX . kerqyn5qft ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. djthquopua . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . djthquopua . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_foyaftkemu ; rtB .
gcmf3g2rfl = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . obmianunwz . CircularBufSize , & rtDW . obmianunwz . Last , rtDW .
obmianunwz . Tail , rtDW . obmianunwz . Head , rtP .
TransportDelay_InitOutput_fegn1hyvdl , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
jxxzd1swwi = ssGetT ( rtS ) ; rtB . ifayr3iran = ( rtB . jxxzd1swwi >= rtP .
K1_Value_jwfzaa2flf ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
hp1txcvzla = rtDW . bjpjcjhpzj ; } if ( rtB . ifayr3iran ) { rtB . ayn12thn3t
= rtB . eit2f5ruxh - rtB . gcmf3g2rfl ; rtB . bglcv3ras4 = rtP .
Gain_Gain_m2kqkvp0yr * rtB . ayn12thn3t ; rtB . dhlslzrf2v = rtB . bglcv3ras4
; } else { rtB . dhlslzrf2v = rtB . hp1txcvzla ; } rtB . kdb1fjbda1 . re =
rtB . ppoxem35uq ; rtB . kdb1fjbda1 . im = rtB . dhlslzrf2v ; rtB .
kjwtfmoyp2 = muDoubleScalarHypot ( rtB . kdb1fjbda1 . re , rtB . kdb1fjbda1 .
im ) ; rtB . mrqm5tjkzd = muDoubleScalarAtan2 ( rtB . kdb1fjbda1 . im , rtB .
kdb1fjbda1 . re ) ; rtB . gmehm5w3yl = rtX . iytiwtpacb ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . faad0u1b4n . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . faad0u1b4n . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_miyezd2kg1 ; rtB . d3ccd0jbe1 = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . nf5je2gw4m .
CircularBufSize , & rtDW . nf5je2gw4m . Last , rtDW . nf5je2gw4m . Tail ,
rtDW . nf5je2gw4m . Head , rtP . TransportDelay_InitOutput_aipaiuowqc , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . c23j41usx2 = ssGetT ( rtS ) ; rtB . jaua1igiyz
= ( rtB . c23j41usx2 >= rtP . K1_Value_pumfeo2aph ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . je3ut5omgg = rtDW . a5gjxntn1h ; } if ( rtB .
jaua1igiyz ) { rtB . fyntoffjpc = rtB . gmehm5w3yl - rtB . d3ccd0jbe1 ; rtB .
mjpiu3tjj0 = rtP . Gain_Gain_gaid4mcoqv * rtB . fyntoffjpc ; rtB . lcsqf4izo5
= rtB . mjpiu3tjj0 ; } else { rtB . lcsqf4izo5 = rtB . je3ut5omgg ; } rtB .
obvcacwue5 = rtX . g0phs0m0dn ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. k4bhgqdyhg . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . k4bhgqdyhg . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_lge1txljnw ; rtB .
hvg2cpivvz = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . asmpxtvf2z . CircularBufSize , & rtDW . asmpxtvf2z . Last , rtDW .
asmpxtvf2z . Tail , rtDW . asmpxtvf2z . Head , rtP .
TransportDelay_InitOutput_aa5aowvfp2 , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
er4mudexv1 = ssGetT ( rtS ) ; rtB . cu3lay3u3l = ( rtB . er4mudexv1 >= rtP .
K1_Value_bmudnafxb3 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
flbstubmun = rtDW . i3uldok0qn ; } if ( rtB . cu3lay3u3l ) { rtB . l3ojvifpsg
= rtB . obvcacwue5 - rtB . hvg2cpivvz ; rtB . bp2zo0z3u3 = rtP .
Gain_Gain_n4rgfsq5ty * rtB . l3ojvifpsg ; rtB . a4ozttu2p4 = rtB . bp2zo0z3u3
; } else { rtB . a4ozttu2p4 = rtB . flbstubmun ; } rtB . ptlg2hvt2q . re =
rtB . lcsqf4izo5 ; rtB . ptlg2hvt2q . im = rtB . a4ozttu2p4 ; rtB .
j4g1tjgr5g = muDoubleScalarHypot ( rtB . ptlg2hvt2q . re , rtB . ptlg2hvt2q .
im ) ; rtB . j5kl5ybylh = muDoubleScalarAtan2 ( rtB . ptlg2hvt2q . im , rtB .
ptlg2hvt2q . re ) ; rtB . mba3xfzjhz = rtX . lka0ltzwzh ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . kx2dm0gq0l . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . kx2dm0gq0l . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_d5wczemc11 ; rtB . fv4xbzjcka = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . prqk4dqmol .
CircularBufSize , & rtDW . prqk4dqmol . Last , rtDW . prqk4dqmol . Tail ,
rtDW . prqk4dqmol . Head , rtP . TransportDelay_InitOutput_jvpzsxmh0k , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . n1w241jfw5 = ssGetT ( rtS ) ; rtB . c5gvl04lnz
= ( rtB . n1w241jfw5 >= rtP . K1_Value_glnwbcb5jn ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . bdm3xakteu = rtDW . a1ejgtlf3z ; } if ( rtB .
c5gvl04lnz ) { rtB . ozskbcjuxl = rtB . mba3xfzjhz - rtB . fv4xbzjcka ; rtB .
fpzmfnczh5 = rtP . Gain_Gain_nqtw1zh5ct * rtB . ozskbcjuxl ; rtB . lap3zr2ns3
= rtB . fpzmfnczh5 ; } else { rtB . lap3zr2ns3 = rtB . bdm3xakteu ; } rtB .
pujm3gf3vm = rtX . pquw0pmaw2 ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. milmx1vwl5 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . milmx1vwl5 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_j4fbooars3 ; rtB .
k01s0mpenb = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . gl5lw50op2 . CircularBufSize , & rtDW . gl5lw50op2 . Last , rtDW .
gl5lw50op2 . Tail , rtDW . gl5lw50op2 . Head , rtP .
TransportDelay_InitOutput_mxcmg3j1ft , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
gtgh2v5ygu = ssGetT ( rtS ) ; rtB . aorigjbhdv = ( rtB . gtgh2v5ygu >= rtP .
K1_Value_pt4iygyjzi ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
nula0u1lkz = rtDW . ihf24wpydd ; } if ( rtB . aorigjbhdv ) { rtB . m15ljwrwc5
= rtB . pujm3gf3vm - rtB . k01s0mpenb ; rtB . ihpwnh2hw0 = rtP .
Gain_Gain_n40ulfvb55 * rtB . m15ljwrwc5 ; rtB . ii4fe1aanj = rtB . ihpwnh2hw0
; } else { rtB . ii4fe1aanj = rtB . nula0u1lkz ; } rtB . ca20ty4e3t . re =
rtB . lap3zr2ns3 ; rtB . ca20ty4e3t . im = rtB . ii4fe1aanj ; rtB .
az42jth2g3 = muDoubleScalarHypot ( rtB . ca20ty4e3t . re , rtB . ca20ty4e3t .
im ) ; rtB . nh0msgrqq0 = muDoubleScalarAtan2 ( rtB . ca20ty4e3t . im , rtB .
ca20ty4e3t . re ) ; rtB . d2r2jobgwj = rtX . jx303cpogv ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . e1qtan4tfe . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . e1qtan4tfe . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_kiqasqcb54 ; rtB . mpqysyjexu = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . ju1jo0gbez .
CircularBufSize , & rtDW . ju1jo0gbez . Last , rtDW . ju1jo0gbez . Tail ,
rtDW . ju1jo0gbez . Head , rtP . TransportDelay_InitOutput_i53veo5gam , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . peirqbqzeh = ssGetT ( rtS ) ; rtB . aikw5m1r4t
= ( rtB . peirqbqzeh >= rtP . K1_Value_ed1zt3iikf ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . kyc3dzqcpo = rtDW . jykznszm3q ; } if ( rtB .
aikw5m1r4t ) { rtB . lbnb4o1d3s = rtB . d2r2jobgwj - rtB . mpqysyjexu ; rtB .
huuvr1nz0n = rtP . Gain_Gain_jaljbdpwfc * rtB . lbnb4o1d3s ; rtB . atxnqorjb4
= rtB . huuvr1nz0n ; } else { rtB . atxnqorjb4 = rtB . kyc3dzqcpo ; } rtB .
mdgp1q5akr = rtX . ltxxvmnz50 ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. nsvuvsgsn0 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . nsvuvsgsn0 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_hdijrdpglg ; rtB .
o0tl12vy3e = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . ihorz5zaun . CircularBufSize , & rtDW . ihorz5zaun . Last , rtDW .
ihorz5zaun . Tail , rtDW . ihorz5zaun . Head , rtP .
TransportDelay_InitOutput_mlqui5rcwm , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
a2g1qakqix = ssGetT ( rtS ) ; rtB . b4lmsnccq0 = ( rtB . a2g1qakqix >= rtP .
K1_Value_koitkbvnxf ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
dtgoshe0ee = rtDW . kwauldso4g ; } if ( rtB . b4lmsnccq0 ) { rtB . kdfn0rjv1j
= rtB . mdgp1q5akr - rtB . o0tl12vy3e ; rtB . oevc5r5aso = rtP .
Gain_Gain_ekqnsj3fno * rtB . kdfn0rjv1j ; rtB . nyw0btgear = rtB . oevc5r5aso
; } else { rtB . nyw0btgear = rtB . dtgoshe0ee ; } rtB . cucmdg5lcy . re =
rtB . atxnqorjb4 ; rtB . cucmdg5lcy . im = rtB . nyw0btgear ; rtB .
dbwb1pifz4 = muDoubleScalarHypot ( rtB . cucmdg5lcy . re , rtB . cucmdg5lcy .
im ) ; rtB . f3cnu05jf2 = muDoubleScalarAtan2 ( rtB . cucmdg5lcy . im , rtB .
cucmdg5lcy . re ) ; rtB . lpgwkwyj4h [ 0 ] = rtP . Gain2_Gain * rtB .
mwsaw5g1sb ; rtB . lpgwkwyj4h [ 1 ] = rtP . Gain2_Gain * rtB . lvnv2nzwp1 ;
rtB . lpgwkwyj4h [ 2 ] = rtP . Gain2_Gain * rtB . ir0mdju3wm ; rtB .
lpgwkwyj4h [ 3 ] = rtP . Gain2_Gain * rtB . l1dkizi1sv ; rtB . lpgwkwyj4h [ 4
] = rtP . Gain2_Gain * rtB . k4obkkygio ; rtB . lpgwkwyj4h [ 5 ] = rtP .
Gain2_Gain * rtB . mxsx1mjxrn ; rtB . lpgwkwyj4h [ 6 ] = rtP . Gain2_Gain *
rtB . pvedmluia0 ; rtB . lpgwkwyj4h [ 7 ] = rtP . Gain2_Gain * rtB .
dxg5ih1pwv ; rtB . lpgwkwyj4h [ 8 ] = rtP . Gain2_Gain * rtB . od0lpdqczv ;
rtB . lpgwkwyj4h [ 9 ] = rtP . Gain2_Gain * rtB . got1muihhn ; rtB .
lpgwkwyj4h [ 10 ] = rtP . Gain2_Gain * rtB . kjwtfmoyp2 ; rtB . lpgwkwyj4h [
11 ] = rtP . Gain2_Gain * rtB . j4g1tjgr5g ; rtB . lpgwkwyj4h [ 12 ] = rtP .
Gain2_Gain * rtB . az42jth2g3 ; rtB . lpgwkwyj4h [ 13 ] = rtP . Gain2_Gain *
rtB . dbwb1pifz4 ; rtB . mssfskdrdd = rtP . RadDeg_Gain_jmr5wgb43w * rtB .
m1mvcrodu3 ; rtB . gj05gjtc2h = rtP . RadDeg_Gain_p4xuhedstd * rtB .
dcbd243rjt ; rtB . feyhmxqf0w = rtP . RadDeg_Gain_kqdehz2qom * rtB .
blf5njdgxt ; rtB . fnhtcp53hp = rtP . RadDeg_Gain_khpzzlgwdi * rtB .
b4buuids5a ; rtB . jvijyradk1 = rtP . RadDeg_Gain_mhpjxekwxk * rtB .
lyzpgvnlrs ; rtB . alwpunln5q = rtP . RadDeg_Gain_aebhru1te1 * rtB .
objbxbq42a ; rtB . danuh1ceda = rtP . RadDeg_Gain_csmmpzcd4t * rtB .
e3ydoz0bpk ; rtB . haojchkjsb = rtP . RadDeg_Gain_csr0dyabau * rtB .
cys5ttuvcv ; rtB . gu1sbxec3j = rtP . RadDeg_Gain_iinbb4d5kn * rtB .
hkcp1jr5hc ; rtB . mqncb5nyiv = rtP . RadDeg_Gain_hix3d1gfrj * rtB .
boudhhqt4b ; rtB . ozixhbme2b = rtP . RadDeg_Gain_lflq4a4313 * rtB .
mrqm5tjkzd ; rtB . oot0siwxoq = rtP . RadDeg_Gain_g5ruh2rbzc * rtB .
j5kl5ybylh ; rtB . e0asylie5u = rtP . RadDeg_Gain_dbgzyc2dhi * rtB .
nh0msgrqq0 ; rtB . ghdoeopvxm = rtP . RadDeg_Gain_br0s4ta4bv * rtB .
f3cnu05jf2 ; rtB . janxwpkdcj [ 0 ] = rtP . Gain4_Gain * rtB . mssfskdrdd ;
rtB . janxwpkdcj [ 1 ] = rtP . Gain4_Gain * rtB . gj05gjtc2h ; rtB .
janxwpkdcj [ 2 ] = rtP . Gain4_Gain * rtB . feyhmxqf0w ; rtB . janxwpkdcj [ 3
] = rtP . Gain4_Gain * rtB . fnhtcp53hp ; rtB . janxwpkdcj [ 4 ] = rtP .
Gain4_Gain * rtB . jvijyradk1 ; rtB . janxwpkdcj [ 5 ] = rtP . Gain4_Gain *
rtB . alwpunln5q ; rtB . janxwpkdcj [ 6 ] = rtP . Gain4_Gain * rtB .
danuh1ceda ; rtB . janxwpkdcj [ 7 ] = rtP . Gain4_Gain * rtB . haojchkjsb ;
rtB . janxwpkdcj [ 8 ] = rtP . Gain4_Gain * rtB . gu1sbxec3j ; rtB .
janxwpkdcj [ 9 ] = rtP . Gain4_Gain * rtB . mqncb5nyiv ; rtB . janxwpkdcj [
10 ] = rtP . Gain4_Gain * rtB . ozixhbme2b ; rtB . janxwpkdcj [ 11 ] = rtP .
Gain4_Gain * rtB . oot0siwxoq ; rtB . janxwpkdcj [ 12 ] = rtP . Gain4_Gain *
rtB . e0asylie5u ; rtB . janxwpkdcj [ 13 ] = rtP . Gain4_Gain * rtB .
ghdoeopvxm ; rtB . hj3aradqjb = muDoubleScalarCos ( rtB . janxwpkdcj [ 3 ] )
; rtB . izatkryyxm = rtB . lpgwkwyj4h [ 3 ] * rtB . hj3aradqjb ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . cuawvmyqzy = rtDW . bw41vj1a5t ; }
rtB . pwufkp45ct = ( rtP . u8_Value_doesqwqeqc != 0.0 ) ; rtB . emclen4fq3 =
ssGetT ( rtS ) ; on20slldft ( rtS , rtB . ewiv2cqq4y , rtB . pwufkp45ct , rtB
. emclen4fq3 , rtP . T1_delay_kq1j2hiyij , & rtB . f2cfptydf3 , & rtDW .
f2cfptydf3 , & rtP . f2cfptydf3 , & rtPrevZCX . f2cfptydf3 ) ; bhc3qgnuic (
rtS , rtB . macp2azgz2 , rtB . pwufkp45ct , rtB . emclen4fq3 , rtP .
T1_delay_kq1j2hiyij , & rtB . mempnwchre , & rtDW . mempnwchre , & rtP .
mempnwchre , & rtPrevZCX . mempnwchre ) ; rtB . cmh1twtsqa = ( rtB .
f2cfptydf3 . j1vu52vt5l || rtB . mempnwchre . mjqylog0rb ) ; rtB . j0qsxcx4h3
= ( rtB . cuawvmyqzy || rtB . cmh1twtsqa ) ; rtB . e5gt2p5rb3 = rtB .
mgsawex2wt ; rtB . b0ke51nwc1 = ssGetT ( rtS ) ; on20slldft ( rtS , rtB .
pdo21geevx , rtB . e5gt2p5rb3 , rtB . b0ke51nwc1 , rtP . T2_delay_ilsnuihozc
, & rtB . ceydu0zdw3 , & rtDW . ceydu0zdw3 , & rtP . ceydu0zdw3 , & rtPrevZCX
. ceydu0zdw3 ) ; bhc3qgnuic ( rtS , rtB . omqcszm1kk , rtB . e5gt2p5rb3 , rtB
. b0ke51nwc1 , rtP . T2_delay_ilsnuihozc , & rtB . e05ysccjt5 , & rtDW .
e05ysccjt5 , & rtP . e05ysccjt5 , & rtPrevZCX . e05ysccjt5 ) ; rtB .
krlccc2q1a = ( rtB . ceydu0zdw3 . j1vu52vt5l || rtB . e05ysccjt5 . mjqylog0rb
) ; rtB . p1li4qhxzw = ! rtB . krlccc2q1a ; rtB . kl4albs201 = ( rtB .
j0qsxcx4h3 && rtB . p1li4qhxzw ) ; if ( rtB . kl4albs201 ) { rtB . aeayidxpl0
= rtB . jnykas0inl ; } else { rtB . aeayidxpl0 = rtP . u8_Value_ezdl1m5snv ;
} rtB . nndcswzl2t = rtB . izatkryyxm * rtB . aeayidxpl0 ; if ( ssIsSampleHit
( rtS , 1 , 0 ) ) { rtB . n33thm3pps = rtDW . cfe2132dnu ; } rtB . cd2w3syzif
= false ; rtB . kevvashv3b = ssGetT ( rtS ) ; on20slldft ( rtS , rtB .
htr5iktw51 , rtB . cd2w3syzif , rtB . kevvashv3b , rtP . T3_delay_br1hoejpwv
, & rtB . kbnljt2vok , & rtDW . kbnljt2vok , & rtP . kbnljt2vok , & rtPrevZCX
. kbnljt2vok ) ; bhc3qgnuic ( rtS , rtB . kmmd10plti , rtB . cd2w3syzif , rtB
. kevvashv3b , rtP . T3_delay_br1hoejpwv , & rtB . ffo1fpyqgq , & rtDW .
ffo1fpyqgq , & rtP . ffo1fpyqgq , & rtPrevZCX . ffo1fpyqgq ) ; rtB .
pbaz3v0uge = ( rtB . kbnljt2vok . j1vu52vt5l || rtB . ffo1fpyqgq . mjqylog0rb
) ; rtB . ibbomoov24 = ( rtB . n33thm3pps || rtB . pbaz3v0uge ) ; rtB .
o3todgzw1w = rtB . guluxkv0ch ; rtB . jbw3drd1en = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . a0cax2dq42 , rtB . o3todgzw1w , rtB . jbw3drd1en ,
rtP . T4_delay_cwqwobasv2 , & rtB . nrt14kntrk , & rtDW . nrt14kntrk , & rtP
. nrt14kntrk , & rtPrevZCX . nrt14kntrk ) ; bhc3qgnuic ( rtS , rtB .
e5ffsl2txk , rtB . o3todgzw1w , rtB . jbw3drd1en , rtP . T4_delay_cwqwobasv2
, & rtB . nepgqug5c0 , & rtDW . nepgqug5c0 , & rtP . nepgqug5c0 , & rtPrevZCX
. nepgqug5c0 ) ; rtB . ghcjkygoqm = ( rtB . nrt14kntrk . j1vu52vt5l || rtB .
nepgqug5c0 . mjqylog0rb ) ; rtB . dzscnsz10p = ! rtB . ghcjkygoqm ; rtB .
ff4zi1sk4i = ( rtB . ibbomoov24 && rtB . dzscnsz10p ) ; if ( rtB . ff4zi1sk4i
) { rtB . a5yycj1ihr = rtB . dwms0o5flk ; } else { rtB . a5yycj1ihr = rtP .
u2_Value ; } rtB . fwlybw14jn = 0.0 * rtB . a5yycj1ihr ; rtB . dxa1300d4c =
rtB . nndcswzl2t + rtB . fwlybw14jn ; rtB . oxnowbh4dh = rtB . dxa1300d4c *
rtB . dxa1300d4c ; rtB . m5ugzcgkev = muDoubleScalarSin ( rtB . janxwpkdcj [
3 ] ) ; rtB . opvysr4u4o = rtB . m5ugzcgkev * rtB . lpgwkwyj4h [ 3 ] ; rtB .
emp1ngjjlh = rtB . opvysr4u4o * rtB . aeayidxpl0 ; rtB . ly2fidssht = 0.0 *
rtB . a5yycj1ihr ; rtB . dgvytedolp = rtB . emp1ngjjlh + rtB . ly2fidssht ;
rtB . gdbvbu2ip4 = rtB . dgvytedolp * rtB . dgvytedolp ; rtB . ecn1rg1xa2 =
rtB . oxnowbh4dh + rtB . gdbvbu2ip4 ; rtB . c0rxma5kqx = muDoubleScalarSqrt (
rtB . ecn1rg1xa2 ) ; rtB . oun10rjprl = ( rtB . c0rxma5kqx < 0.0 ) ; rtB .
k5so3nk1yc = ! rtB . oun10rjprl ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB
. nokku3vzx4 = rtDW . gvkoorm242 ; rtB . dqsdtpxg5g = rtDW . lxxzsohnb1 ; }
rtB . jlrjksphcs = rtB . dxa1300d4c - rtB . nokku3vzx4 ; rtB . aqnpzwnnc0 =
rtB . jlrjksphcs * rtB . jlrjksphcs ; rtB . enb0xhrrt3 = rtB . dgvytedolp -
rtB . dqsdtpxg5g ; rtB . kwf1kwpnhx = rtB . enb0xhrrt3 * rtB . enb0xhrrt3 ;
rtB . fiy11qvag5 = rtB . aqnpzwnnc0 + rtB . kwf1kwpnhx ; rtB . n1n20f2zar =
muDoubleScalarSqrt ( rtB . fiy11qvag5 ) ; rtB . dfg0ujq1gr = rtB . dxa1300d4c
* rtB . dxa1300d4c ; rtB . jeyge0hqws = rtB . dgvytedolp * rtB . dgvytedolp ;
rtB . olfsuubit2 = rtB . dfg0ujq1gr + rtB . jeyge0hqws ; rtB . jihq3em231 =
muDoubleScalarSqrt ( rtB . olfsuubit2 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtB . bquzarc5sj = rtB . nokku3vzx4 * rtB . nokku3vzx4 ; rtB . ouamcqq05t
= rtB . dqsdtpxg5g * rtB . dqsdtpxg5g ; rtB . lee4xhdryw = rtB . bquzarc5sj +
rtB . ouamcqq05t ; rtB . peunc2zflo = muDoubleScalarSqrt ( rtB . lee4xhdryw )
; } maxV = muDoubleScalarMax ( rtB . jihq3em231 , rtB . peunc2zflo ) ; rtB .
mln2cilizw = maxV ; rtB . guqbgdy2b5 = ( rtB . mln2cilizw == rtP .
u_Value_ahabcft5eo ) ; if ( rtB . guqbgdy2b5 ) { rtB . il5jtk2wlm = rtP .
u_Value_jonzmiyrr3 ; } else { rtB . il5jtk2wlm = rtB . mln2cilizw ; } rtB .
hhsooc0al3 = rtB . n1n20f2zar / rtB . il5jtk2wlm ; rtB . aowz3mvgh3 = ( rtB .
hhsooc0al3 > rtP . u0_Value_esll5gpc2d ) ; rtB . pbmnphmjh3 = ( rtB .
k5so3nk1yc && rtB . aowz3mvgh3 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB
. i4bdlr2sjh = rtDW . duld1iija4 ; rtB . et0kfo00yx = rtDW . j5clkw2ofw ; rtB
. gjvicer1pj = rtDW . ht5aj3noet ; rtB . b45epke2lh = ( rtB . i4bdlr2sjh +
rtB . et0kfo00yx ) + rtB . gjvicer1pj ; rtB . frv0rvlhwg = rtB . b45epke2lh *
rtP . u_Value_keufs1hsvf ; } rtB . ghtyjg5rtr = rtB . n1n20f2zar - rtB .
frv0rvlhwg ; rtB . oboucuq2ip = muDoubleScalarAbs ( rtB . ghtyjg5rtr ) ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . p3sdb2hrhl = ( rtB . frv0rvlhwg ==
rtP . u_Value_gjwerk3tdy ) ; if ( rtB . p3sdb2hrhl ) { rtB . p0bsxysoyr = rtP
. u_Value_bcv5tmykyw ; } else { rtB . p0bsxysoyr = rtB . frv0rvlhwg ; } rtB .
lp0ghi4skr = rtDW . b54hv4iwx0 ; rtB . g5aw5wstzc = rtDW . geqzl3jlyf ; rtB .
mld4spyggy = ! rtB . g5aw5wstzc ; rtB . dcoppgit5y = ( rtB . lp0ghi4skr &&
rtB . mld4spyggy ) ; } rtB . kqzj5435hf = rtB . oboucuq2ip / rtB . p0bsxysoyr
; rtB . ia4gjw0viu = ( rtB . kqzj5435hf > rtP . u8_Value_m5vidopcu4 ) ; rtB .
j5mxxuvxvc = ( rtB . n1n20f2zar < rtP . u5_Value_cgpsnnrxd4 ) ; rtB .
p3csgy1nmc = ! rtB . j5mxxuvxvc ; rtB . hthmdqq0tj = ( rtB . ia4gjw0viu &&
rtB . p3csgy1nmc ) ; rtB . k4djtoqsoe = ( rtB . pbmnphmjh3 && rtB .
hthmdqq0tj ) ; rtB . csmwalb3wx = ( rtB . k4djtoqsoe || rtB . dcoppgit5y ) ;
rtB . op2rjr3zkv = rtB . csmwalb3wx ; rtB . afdzh5sqsk = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . oztcgxwfan , rtB . op2rjr3zkv , rtB . afdzh5sqsk ,
rtP . OffDelay_delay , & rtB . inadvhzvdh , & rtDW . inadvhzvdh , & rtP .
inadvhzvdh , & rtPrevZCX . inadvhzvdh ) ; bhc3qgnuic ( rtS , rtB . d4py5iw3o2
, rtB . op2rjr3zkv , rtB . afdzh5sqsk , rtP . OffDelay_delay , & rtB .
leagxggwwe , & rtDW . leagxggwwe , & rtP . leagxggwwe , & rtPrevZCX .
leagxggwwe ) ; rtB . iraovkag4l = ( rtB . inadvhzvdh . j1vu52vt5l || rtB .
leagxggwwe . mjqylog0rb ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
jkmjo4quwz = rtDW . bjzszecxxc ; rtB . c4tbgverxw = ! rtB . jkmjo4quwz ; rtB
. cwudxix4dz = rtDW . l2ogv2jkyi ; } rtB . bbcwfeppru = ( rtB . iraovkag4l &&
rtB . c4tbgverxw ) ; rtB . hrxabukzsy = ! rtB . pbmnphmjh3 ; rtB . m13edbg5kj
= ! rtB . hrxabukzsy ; rtB . ghfba0420o = ( rtB . cwudxix4dz && rtB .
m13edbg5kj ) ; rtB . l3rpmtffkw = ( rtB . bbcwfeppru || rtB . ghfba0420o ) ;
rtB . hkwo4x0o0p = ( rtB . l3rpmtffkw && rtB . fcqneu0lzp ) ; rtB .
k3om0acsgh = ( rtB . bbcwfeppru && rtB . bs0uc3od50 ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . d1vz5aawhc = rtDW . ike0qfti5p ; rtB . prmbkuwzr1 = !
rtB . d1vz5aawhc ; } rtB . ad1jr0lujk = ( rtB . k4djtoqsoe && rtB .
prmbkuwzr1 ) ; rtB . n1fpgepsrf = ( rtB . ad1jr0lujk && rtB . iwfj542rku ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . j2ljtehmn5 = rtDW . m1mkpcefd4 ;
} rtB . jgj2f2cg2v = rtP . u_Gain * rtB . dxa1300d4c ; rtB . l01ig2e3ag = rtB
. jgj2f2cg2v * rtB . locdsamlne ; rtB . lu31zjoean = muDoubleScalarCos ( rtB
. janxwpkdcj [ 4 ] ) ; rtB . do4x3mpyxa = rtB . lpgwkwyj4h [ 4 ] * rtB .
lu31zjoean ; rtB . euazqazofk = rtB . do4x3mpyxa * rtB . aeayidxpl0 ; rtB .
brkbkade3v = 0.0 * rtB . a5yycj1ihr ; rtB . l2zahd4oox = rtB . euazqazofk +
rtB . brkbkade3v ; rtB . acu2uimhf4 = rtP . u_Gain_gwmjnm2gyx * rtB .
l2zahd4oox ; rtB . o5j430tt2c = muDoubleScalarCos ( rtB . janxwpkdcj [ 5 ] )
; rtB . egfgjyyopt = rtB . lpgwkwyj4h [ 5 ] * rtB . o5j430tt2c ; rtB .
k1dq5xdlze = rtB . egfgjyyopt * rtB . aeayidxpl0 ; rtB . lrjh2wt5jn = 0.0 *
rtB . a5yycj1ihr ; rtB . emlwggxmjw = rtB . k1dq5xdlze + rtB . lrjh2wt5jn ;
rtB . fd2yp3dpzd = rtP . u_Gain_aspyntlsoj * rtB . emlwggxmjw ; rtB .
hurcao0xld = rtB . acu2uimhf4 + rtB . fd2yp3dpzd ; rtB . oty0zl5ucw = rtB .
hurcao0xld + rtB . jgj2f2cg2v ; rtB . lmpfshyn1r = rtB . oty0zl5ucw * rtP .
u7_Value_krd2xsctxt ; if ( rtP . u3_Value_lnebph21xb > rtP .
u_Threshold_adb123ueus ) { rtB . g5iso0n03q = rtB . lmpfshyn1r * rtB .
locdsamlne ; rtB . dyk20pdh43 = rtB . l01ig2e3ag - rtB . g5iso0n03q ; rtB .
hwfoco4241 = rtB . dyk20pdh43 ; } else { rtB . hwfoco4241 = rtB . l01ig2e3ag
; } rtB . fkmgqw44vb = rtB . hwfoco4241 * rtB . lda5igrpdy ; rtB . pbzxcpqffu
= rtP . u_Gain_c2hwbpuqnt * rtB . dgvytedolp ; rtB . erpcgz3ruc = rtB .
pbzxcpqffu * rtB . locdsamlne ; rtB . kaxkaujzjx = muDoubleScalarSin ( rtB .
janxwpkdcj [ 4 ] ) ; rtB . coehnzyatq = rtB . kaxkaujzjx * rtB . lpgwkwyj4h [
4 ] ; rtB . lykixhonrg = rtB . coehnzyatq * rtB . aeayidxpl0 ; rtB .
pd2513by2v = 0.0 * rtB . a5yycj1ihr ; rtB . g340u5g2f2 = rtB . lykixhonrg +
rtB . pd2513by2v ; rtB . hxgvln4wyf = rtP . u_Gain_brbtxypk1t * rtB .
g340u5g2f2 ; rtB . myabukwkvl = muDoubleScalarSin ( rtB . janxwpkdcj [ 5 ] )
; rtB . oaxc3hrb45 = rtB . myabukwkvl * rtB . lpgwkwyj4h [ 5 ] ; rtB .
aclmwvaimb = rtB . oaxc3hrb45 * rtB . aeayidxpl0 ; rtB . nbbv3lwhqc = 0.0 *
rtB . a5yycj1ihr ; rtB . nubgwxsn52 = rtB . aclmwvaimb + rtB . nbbv3lwhqc ;
rtB . bolkwwrtqf = rtP . u_Gain_caeifufuqm * rtB . nubgwxsn52 ; rtB .
huamvc33fh = rtB . hxgvln4wyf + rtB . bolkwwrtqf ; rtB . nfmpcwiqzm = rtB .
huamvc33fh + rtB . pbzxcpqffu ; rtB . mpx3oy4ctu = rtB . nfmpcwiqzm * rtP .
u7_Value_krd2xsctxt ; if ( rtP . u3_Value_lnebph21xb > rtP .
u_Threshold_azo1mu1cp4 ) { rtB . c0nzb305m5 = rtB . mpx3oy4ctu * rtB .
locdsamlne ; rtB . jrkkjntyzs = rtB . erpcgz3ruc - rtB . c0nzb305m5 ; rtB .
mhrqxa3yrk = rtB . jrkkjntyzs ; } else { rtB . mhrqxa3yrk = rtB . erpcgz3ruc
; } rtB . fj3wz3jsv3 = rtB . mhrqxa3yrk * rtB . peijvg4ilw ; rtB . mrg3hu21rf
= rtB . fkmgqw44vb - rtB . fj3wz3jsv3 ; rtB . mw0vpi34hn = ! rtB . kl4albs201
; rtB . fennj4g41k = ! rtB . ff4zi1sk4i ; rtB . dlq2giet1z = ( rtB .
mw0vpi34hn && rtB . fennj4g41k ) ; rtB . c2tganyo0x = rtB . izatkryyxm * rtB
. izatkryyxm ; rtB . c4livuu4un = rtB . opvysr4u4o * rtB . opvysr4u4o ; rtB .
mnqfnendr5 = rtB . c2tganyo0x + rtB . c4livuu4un ; rtB . leuulxbg5x =
muDoubleScalarSqrt ( rtB . mnqfnendr5 ) ; rtB . nvvqtayzmp = ( rtB .
leuulxbg5x > rtP . u9_Value_pwo1xiraul ) ; rtB . hd1wzosq4k = ( rtB .
nvvqtayzmp && rtB . kl4albs201 ) ; rtB . hcjwk0r3h0 = ( rtB . mtdldetrtq &&
rtB . ff4zi1sk4i ) ; rtB . cxt03zh1fr = ( rtB . kl4albs201 && rtB .
ff4zi1sk4i ) ; if ( rtB . dlq2giet1z ) { rtB . laxu4i4bvg = rtP .
u7_Value_fs2w21cd1e ; } else { if ( rtB . cxt03zh1fr ) { rtB . ktlhhddv1w = (
rtB . hd1wzosq4k || rtB . hcjwk0r3h0 ) ; rtB . bwyd4id2ux = ( rtB .
ktlhhddv1w && rtB . cxt03zh1fr ) ; rtB . o4qmf5ulun = rtB . bwyd4id2ux ; }
else { if ( rtB . kl4albs201 ) { rtB . kaxmahht5k = rtB . hd1wzosq4k ; } else
{ rtB . kaxmahht5k = rtB . hcjwk0r3h0 ; } rtB . o4qmf5ulun = rtB . kaxmahht5k
; } rtB . laxu4i4bvg = rtB . o4qmf5ulun ; } if ( rtP . u_Value_iuoi1xcwxh >
rtP . u_Threshold_lbqd5llu2z ) { if ( rtB . laxu4i4bvg > rtP . u_Threshold )
{ rtB . ftfygcuw4y = rtB . mrg3hu21rf ; } else { rtB . ftfygcuw4y = rtP .
u54_Value_fcmc5me3hr ; } rtB . lehgezo4xp = rtB . ftfygcuw4y ; } else { rtB .
lehgezo4xp = rtP . u54_Value_fcmc5me3hr ; } rtB . cbz2phbqou = rtB .
lcorj1twi3 + rtB . lehgezo4xp ; rtB . cvd4e0evi0 = rtX . b4ijv0yfdl ; {
real_T * * uBuffer = ( real_T * * ) & rtDW . e2vylf2hfx . TUbufferPtrs [ 0 ]
; real_T * * tBuffer = ( real_T * * ) & rtDW . e2vylf2hfx . TUbufferPtrs [ 1
] ; real_T simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_fcqqd2zozi ; rtB . m1m0nlwsn1 = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . msx5naejpe .
CircularBufSize , & rtDW . msx5naejpe . Last , rtDW . msx5naejpe . Tail ,
rtDW . msx5naejpe . Head , rtP . TransportDelay_InitOutput_mifq2pm4zt , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . p2oemoss5g = ssGetT ( rtS ) ; rtB . kavf3npfnn
= ( rtB . p2oemoss5g >= rtP . K1_Value_gmmcfrua3i ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . bipzy3ugza = rtDW . j0fgvrdpz3 ; } if ( rtB .
kavf3npfnn ) { rtB . d2sl4wmd4q = rtB . cvd4e0evi0 - rtB . m1m0nlwsn1 ; rtB .
lkmoat0yfp = rtP . Gain_Gain_bxnyikhpon * rtB . d2sl4wmd4q ; rtB . dy5mue12t2
= rtB . lkmoat0yfp ; } else { rtB . dy5mue12t2 = rtB . bipzy3ugza ; } rtB .
k5ieikefmd = rtX . inx2whmsvz ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. bflyns0cbr . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . bflyns0cbr . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_enbrjjjv0c ; rtB .
mosfsb0yiv = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . n00kmgw5wb . CircularBufSize , & rtDW . n00kmgw5wb . Last , rtDW .
n00kmgw5wb . Tail , rtDW . n00kmgw5wb . Head , rtP .
TransportDelay_InitOutput_dkuoxqd4yo , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
btprsislvo = ssGetT ( rtS ) ; rtB . noq1oar4ax = ( rtB . btprsislvo >= rtP .
K1_Value_jv4ydpkvz5 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
ke23tqohcz = rtDW . cvvtotyqir ; } if ( rtB . noq1oar4ax ) { rtB . jzixcw0qdv
= rtB . k5ieikefmd - rtB . mosfsb0yiv ; rtB . oaaelzzcpf = rtP .
Gain_Gain_fg0y5jvj2r * rtB . jzixcw0qdv ; rtB . gmlyxhb3tx = rtB . oaaelzzcpf
; } else { rtB . gmlyxhb3tx = rtB . ke23tqohcz ; } rtB . nrgwvgu1ee . re =
rtB . dy5mue12t2 ; rtB . nrgwvgu1ee . im = rtB . gmlyxhb3tx ; rtB .
cb4kh2u023 = muDoubleScalarHypot ( rtB . nrgwvgu1ee . re , rtB . nrgwvgu1ee .
im ) ; rtB . aa4jom32f5 = muDoubleScalarAtan2 ( rtB . nrgwvgu1ee . im , rtB .
nrgwvgu1ee . re ) ; rtB . egeez0g01i = rtX . obmdbjtn3e ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . pxt2zbirdg . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . pxt2zbirdg . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_d2e14ai5vg ; rtB . ldb1ettmb5 = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . pqxdtxr3k0 .
CircularBufSize , & rtDW . pqxdtxr3k0 . Last , rtDW . pqxdtxr3k0 . Tail ,
rtDW . pqxdtxr3k0 . Head , rtP . TransportDelay_InitOutput_jolv14qahc , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . aruupsckqo = ssGetT ( rtS ) ; rtB . hvatfnmcsd
= ( rtB . aruupsckqo >= rtP . K1_Value_i544jh0cow ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . j1vqsmoby3 = rtDW . bbxfzyjpg4 ; } if ( rtB .
hvatfnmcsd ) { rtB . gam3aich52 = rtB . egeez0g01i - rtB . ldb1ettmb5 ; rtB .
aud3ls3fqx = rtP . Gain_Gain_jhhz1dl1qt * rtB . gam3aich52 ; rtB . mhjihj0vyw
= rtB . aud3ls3fqx ; } else { rtB . mhjihj0vyw = rtB . j1vqsmoby3 ; } rtB .
itzq53orwz = rtX . mczl1h5kue ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. fllltsfpxy . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . fllltsfpxy . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_o4vgxrlxi1 ; rtB .
oeungf4e3i = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . o2cxt5uwbv . CircularBufSize , & rtDW . o2cxt5uwbv . Last , rtDW .
o2cxt5uwbv . Tail , rtDW . o2cxt5uwbv . Head , rtP .
TransportDelay_InitOutput_bwi25wucoj , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
fc1vrnzg10 = ssGetT ( rtS ) ; rtB . iiuaajeuwg = ( rtB . fc1vrnzg10 >= rtP .
K1_Value_in22gkn0mz ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
d0pmlg4dma = rtDW . fuwywwq3yv ; } if ( rtB . iiuaajeuwg ) { rtB . mfm5yegjdr
= rtB . itzq53orwz - rtB . oeungf4e3i ; rtB . hwhjpndlue = rtP .
Gain_Gain_d0pgssaqhw * rtB . mfm5yegjdr ; rtB . chlpl14sjv = rtB . hwhjpndlue
; } else { rtB . chlpl14sjv = rtB . d0pmlg4dma ; } rtB . muxevglbir . re =
rtB . mhjihj0vyw ; rtB . muxevglbir . im = rtB . chlpl14sjv ; rtB .
eq25tfdqgg = muDoubleScalarHypot ( rtB . muxevglbir . re , rtB . muxevglbir .
im ) ; rtB . bd5xro2plz = muDoubleScalarAtan2 ( rtB . muxevglbir . im , rtB .
muxevglbir . re ) ; rtB . k1gjsemfzf = rtX . b2kr0ggmvo ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . afb2vzbxaa . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . afb2vzbxaa . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_ozzodunntx ; rtB . pwrldklcpp = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . owq1uvux0e .
CircularBufSize , & rtDW . owq1uvux0e . Last , rtDW . owq1uvux0e . Tail ,
rtDW . owq1uvux0e . Head , rtP . TransportDelay_InitOutput_ctbg4oq5wd , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . ov0akqifmv = ssGetT ( rtS ) ; rtB . lfv4h1xhkw
= ( rtB . ov0akqifmv >= rtP . K1_Value_caowxqneqp ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . gijags0zop = rtDW . gd2gr2srvr ; } if ( rtB .
lfv4h1xhkw ) { rtB . kj0biedn0z = rtB . k1gjsemfzf - rtB . pwrldklcpp ; rtB .
hn41r03euc = rtP . Gain_Gain_jqthbobby1 * rtB . kj0biedn0z ; rtB . bt0tzj3emo
= rtB . hn41r03euc ; } else { rtB . bt0tzj3emo = rtB . gijags0zop ; } rtB .
lifmwxrumg = rtX . pdrwmhpgu2 ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. fz41nona4h . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . fz41nona4h . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_ar4dnw40cr ; rtB .
jpoz0ommsy = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . ffi24542he . CircularBufSize , & rtDW . ffi24542he . Last , rtDW .
ffi24542he . Tail , rtDW . ffi24542he . Head , rtP .
TransportDelay_InitOutput_kom35bcuhc , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
itkklpmkqg = ssGetT ( rtS ) ; rtB . htaj2efbsr = ( rtB . itkklpmkqg >= rtP .
K1_Value_a3qfb5jptb ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
dalbaaya1k = rtDW . mumecnxvzw ; } if ( rtB . htaj2efbsr ) { rtB . mudmlffevj
= rtB . lifmwxrumg - rtB . jpoz0ommsy ; rtB . hjspwp5pds = rtP .
Gain_Gain_lk1s5uhsjx * rtB . mudmlffevj ; rtB . ariz3p4kkt = rtB . hjspwp5pds
; } else { rtB . ariz3p4kkt = rtB . dalbaaya1k ; } rtB . lsii1ckg1l . re =
rtB . bt0tzj3emo ; rtB . lsii1ckg1l . im = rtB . ariz3p4kkt ; rtB .
irfwlro2qv = muDoubleScalarHypot ( rtB . lsii1ckg1l . re , rtB . lsii1ckg1l .
im ) ; rtB . gr34fsdvpw = muDoubleScalarAtan2 ( rtB . lsii1ckg1l . im , rtB .
lsii1ckg1l . re ) ; rtB . lz4xhg4hle = rtX . ckadbag1nz ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . hqvoz0gebt . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . hqvoz0gebt . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_aygjcekf2q ; rtB . lpdd3oq4qw = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . kw0xkp24lw .
CircularBufSize , & rtDW . kw0xkp24lw . Last , rtDW . kw0xkp24lw . Tail ,
rtDW . kw0xkp24lw . Head , rtP . TransportDelay_InitOutput_d1bbi3uht4 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . e4bwpqsc3j = ssGetT ( rtS ) ; rtB . iowdmjuj4n
= ( rtB . e4bwpqsc3j >= rtP . K1_Value_d1eq3vsuiu ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . ggxo43ebmf = rtDW . md3bj5wruc ; } if ( rtB .
iowdmjuj4n ) { rtB . gnibyybhnb = rtB . lz4xhg4hle - rtB . lpdd3oq4qw ; rtB .
b1lrby3koc = rtP . Gain_Gain_otg2dyaren * rtB . gnibyybhnb ; rtB . ex1bxygkpd
= rtB . b1lrby3koc ; } else { rtB . ex1bxygkpd = rtB . ggxo43ebmf ; } rtB .
ekue3tpdoq = rtX . kfqga2xqnc ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. pvl2ejzk4c . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . pvl2ejzk4c . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_lxikka2j32 ; rtB .
o0gzcwnvaq = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . e54w33lz5m . CircularBufSize , & rtDW . e54w33lz5m . Last , rtDW .
e54w33lz5m . Tail , rtDW . e54w33lz5m . Head , rtP .
TransportDelay_InitOutput_mfur1ucszt , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
emuue0pv15 = ssGetT ( rtS ) ; rtB . jaykymfl3b = ( rtB . emuue0pv15 >= rtP .
K1_Value_lijr4hs054 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
khp4gy0tuu = rtDW . hqezuuwqi2 ; } if ( rtB . jaykymfl3b ) { rtB . fnxvipuita
= rtB . ekue3tpdoq - rtB . o0gzcwnvaq ; rtB . plhunlxpa0 = rtP .
Gain_Gain_modts34upr * rtB . fnxvipuita ; rtB . my3feqkfg0 = rtB . plhunlxpa0
; } else { rtB . my3feqkfg0 = rtB . khp4gy0tuu ; } rtB . cmcnh0hf5d . re =
rtB . ex1bxygkpd ; rtB . cmcnh0hf5d . im = rtB . my3feqkfg0 ; rtB .
d2eszzxgps = muDoubleScalarHypot ( rtB . cmcnh0hf5d . re , rtB . cmcnh0hf5d .
im ) ; rtB . fyy25cnxac = muDoubleScalarAtan2 ( rtB . cmcnh0hf5d . im , rtB .
cmcnh0hf5d . re ) ; rtB . lrgb5yt5x3 = rtX . ovnjy3004d ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . pzbmlqlvpm . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . pzbmlqlvpm . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_ojsuzuucox ; rtB . a2pujn1dkz = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . m51gowaflk .
CircularBufSize , & rtDW . m51gowaflk . Last , rtDW . m51gowaflk . Tail ,
rtDW . m51gowaflk . Head , rtP . TransportDelay_InitOutput_n1itaqwmev , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . iks4fc4m0f = ssGetT ( rtS ) ; rtB . dtb23wawia
= ( rtB . iks4fc4m0f >= rtP . K1_Value_gje2ig0sh1 ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . apkatzz11b = rtDW . memzqzyaek ; } if ( rtB .
dtb23wawia ) { rtB . a3sfakr40r = rtB . lrgb5yt5x3 - rtB . a2pujn1dkz ; rtB .
baaq3u1i5z = rtP . Gain_Gain_i1rxfmnlqu * rtB . a3sfakr40r ; rtB . dfgjzm3gbq
= rtB . baaq3u1i5z ; } else { rtB . dfgjzm3gbq = rtB . apkatzz11b ; } rtB .
po2qirhw4b = rtX . koitoczsgs ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. h0w4jpcxu0 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . h0w4jpcxu0 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_mrs4g4hz04 ; rtB .
mt31wns2on = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . oihdsug2zq . CircularBufSize , & rtDW . oihdsug2zq . Last , rtDW .
oihdsug2zq . Tail , rtDW . oihdsug2zq . Head , rtP .
TransportDelay_InitOutput_pcmq0yqmia , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
bto1xvddi4 = ssGetT ( rtS ) ; rtB . gong14xtyk = ( rtB . bto1xvddi4 >= rtP .
K1_Value_dd5zfahqag ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
cadrtpm30e = rtDW . ffusyfyneb ; } if ( rtB . gong14xtyk ) { rtB . if1bxyklch
= rtB . po2qirhw4b - rtB . mt31wns2on ; rtB . ffv5hnzx14 = rtP .
Gain_Gain_li2ggml2c4 * rtB . if1bxyklch ; rtB . fkpvxyu5uz = rtB . ffv5hnzx14
; } else { rtB . fkpvxyu5uz = rtB . cadrtpm30e ; } rtB . jf4nfxi3nb . re =
rtB . dfgjzm3gbq ; rtB . jf4nfxi3nb . im = rtB . fkpvxyu5uz ; rtB .
avijtxq4vu = muDoubleScalarHypot ( rtB . jf4nfxi3nb . re , rtB . jf4nfxi3nb .
im ) ; rtB . o0dfdozprp = muDoubleScalarAtan2 ( rtB . jf4nfxi3nb . im , rtB .
jf4nfxi3nb . re ) ; rtB . jnhluwefk5 = rtX . fwvdemyybn ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . bdtaclh4hr . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . bdtaclh4hr . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_m3owha4xxi ; rtB . gttgsz1lmy = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . paswm1kxsx .
CircularBufSize , & rtDW . paswm1kxsx . Last , rtDW . paswm1kxsx . Tail ,
rtDW . paswm1kxsx . Head , rtP . TransportDelay_InitOutput_fvnrporko1 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . hfdmz1eq0r = ssGetT ( rtS ) ; rtB . pely1vcnhk
= ( rtB . hfdmz1eq0r >= rtP . K1_Value_boqkrskgas ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . aq5ykb2q2u = rtDW . bd4vt3ybt5 ; } if ( rtB .
pely1vcnhk ) { rtB . gp34qh3y4p = rtB . jnhluwefk5 - rtB . gttgsz1lmy ; rtB .
hnzmq2dhqh = rtP . Gain_Gain_ihvov2tvvt * rtB . gp34qh3y4p ; rtB . nz5vgslpu5
= rtB . hnzmq2dhqh ; } else { rtB . nz5vgslpu5 = rtB . aq5ykb2q2u ; } rtB .
k54rta2rmh = rtX . df1cmyigny ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. hwwk0yws23 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . hwwk0yws23 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_k5pna43yah ; rtB .
fodjqqyhmz = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . i5hhavred2 . CircularBufSize , & rtDW . i5hhavred2 . Last , rtDW .
i5hhavred2 . Tail , rtDW . i5hhavred2 . Head , rtP .
TransportDelay_InitOutput_ewfb2xuqzx , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
mvulik34lu = ssGetT ( rtS ) ; rtB . ckqrqg0o2i = ( rtB . mvulik34lu >= rtP .
K1_Value_jjimd532gx ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
h3o0svmrdo = rtDW . kmjfdch5hk ; } if ( rtB . ckqrqg0o2i ) { rtB . mub4axotvd
= rtB . k54rta2rmh - rtB . fodjqqyhmz ; rtB . hqxybnlh5z = rtP .
Gain_Gain_fvfhkhflay * rtB . mub4axotvd ; rtB . gn2b2kw2ve = rtB . hqxybnlh5z
; } else { rtB . gn2b2kw2ve = rtB . h3o0svmrdo ; } rtB . pvyb3g3aw5 . re =
rtB . nz5vgslpu5 ; rtB . pvyb3g3aw5 . im = rtB . gn2b2kw2ve ; rtB .
hyp2eynh5e = muDoubleScalarHypot ( rtB . pvyb3g3aw5 . re , rtB . pvyb3g3aw5 .
im ) ; rtB . oj5iyz0vz2 = muDoubleScalarAtan2 ( rtB . pvyb3g3aw5 . im , rtB .
pvyb3g3aw5 . re ) ; rtB . boo1zm2mc5 = rtX . eiq3cpodg4 ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . eosdk1pxwr . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . eosdk1pxwr . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_gf2tvtym3j ; rtB . cpqvbnhkvb = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . ph1zmaxjn3 .
CircularBufSize , & rtDW . ph1zmaxjn3 . Last , rtDW . ph1zmaxjn3 . Tail ,
rtDW . ph1zmaxjn3 . Head , rtP . TransportDelay_InitOutput_jkdixewjvk , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . aabdflu5fj = ssGetT ( rtS ) ; rtB . fhhgrjhphy
= ( rtB . aabdflu5fj >= rtP . K1_Value_m11m2s1cdq ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . gqs0ywq52h = rtDW . oxztpbydsz ; } if ( rtB .
fhhgrjhphy ) { rtB . jyde124kre = rtB . boo1zm2mc5 - rtB . cpqvbnhkvb ; rtB .
ilh3vv3qjh = rtP . Gain_Gain_odr4qqck0r * rtB . jyde124kre ; rtB . oq0qf2bjai
= rtB . ilh3vv3qjh ; } else { rtB . oq0qf2bjai = rtB . gqs0ywq52h ; } rtB .
kyxt0ot0tk = rtX . lujnb1vhch ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. hdva23qpbu . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . hdva23qpbu . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_ahfgo2nfe0 ; rtB .
bmvewcvp2v = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . azybsjku3n . CircularBufSize , & rtDW . azybsjku3n . Last , rtDW .
azybsjku3n . Tail , rtDW . azybsjku3n . Head , rtP .
TransportDelay_InitOutput_lyvx0zioay , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
dlylguwqkv = ssGetT ( rtS ) ; rtB . m24skyar4r = ( rtB . dlylguwqkv >= rtP .
K1_Value_ab2yvg5lpr ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
gevygxpcbb = rtDW . ol40kfwb3x ; } if ( rtB . m24skyar4r ) { rtB . jasiz3flla
= rtB . kyxt0ot0tk - rtB . bmvewcvp2v ; rtB . pevpbi1ytt = rtP .
Gain_Gain_b5ycz2cbsv * rtB . jasiz3flla ; rtB . gj01fqf3oc = rtB . pevpbi1ytt
; } else { rtB . gj01fqf3oc = rtB . gevygxpcbb ; } rtB . gk3apezi4d . re =
rtB . oq0qf2bjai ; rtB . gk3apezi4d . im = rtB . gj01fqf3oc ; rtB .
gwwozv2oaj = muDoubleScalarHypot ( rtB . gk3apezi4d . re , rtB . gk3apezi4d .
im ) ; rtB . lwktzxdm0t = muDoubleScalarAtan2 ( rtB . gk3apezi4d . im , rtB .
gk3apezi4d . re ) ; rtB . mcdqy0bj11 = rtX . ivshzrdzao ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . kieb4vqgp3 . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . kieb4vqgp3 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_kvsikkt5gn ; rtB . cqhozl3urv = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . jzrzzypfkx .
CircularBufSize , & rtDW . jzrzzypfkx . Last , rtDW . jzrzzypfkx . Tail ,
rtDW . jzrzzypfkx . Head , rtP . TransportDelay_InitOutput_ovqbcxuq4w , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . jzisows1fk = ssGetT ( rtS ) ; rtB . j2ysejdif4
= ( rtB . jzisows1fk >= rtP . K1_Value_ibkmy14g0d ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . osss4lu4aa = rtDW . eefsnpxkpz ; } if ( rtB .
j2ysejdif4 ) { rtB . opd5ukei4f = rtB . mcdqy0bj11 - rtB . cqhozl3urv ; rtB .
paxln4lgfm = rtP . Gain_Gain_irsutzdsva * rtB . opd5ukei4f ; rtB . dswmj0uw3l
= rtB . paxln4lgfm ; } else { rtB . dswmj0uw3l = rtB . osss4lu4aa ; } rtB .
c14ocue5tc = rtX . b0se24ah4v ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. oewxpxyjnd . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . oewxpxyjnd . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_db4veg0dt0 ; rtB .
dihlhb1nzh = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . g4bc1ycige . CircularBufSize , & rtDW . g4bc1ycige . Last , rtDW .
g4bc1ycige . Tail , rtDW . g4bc1ycige . Head , rtP .
TransportDelay_InitOutput_mgxexxlite , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
dujknbt43s = ssGetT ( rtS ) ; rtB . nu20zgp0tc = ( rtB . dujknbt43s >= rtP .
K1_Value_kd2rlwuguo ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
l0in3luquu = rtDW . j0cvdfbqu0 ; } if ( rtB . nu20zgp0tc ) { rtB . namcbrdhe4
= rtB . c14ocue5tc - rtB . dihlhb1nzh ; rtB . legp4gckaw = rtP .
Gain_Gain_alw4cezuel * rtB . namcbrdhe4 ; rtB . cxe3gw0o1x = rtB . legp4gckaw
; } else { rtB . cxe3gw0o1x = rtB . l0in3luquu ; } rtB . lnbyofa1zh . re =
rtB . dswmj0uw3l ; rtB . lnbyofa1zh . im = rtB . cxe3gw0o1x ; rtB .
jetr5pxxjh = muDoubleScalarHypot ( rtB . lnbyofa1zh . re , rtB . lnbyofa1zh .
im ) ; rtB . ccv4ay1gwg = muDoubleScalarAtan2 ( rtB . lnbyofa1zh . im , rtB .
lnbyofa1zh . re ) ; rtB . cf3kqjr4wo = rtX . dzlchxjgwf ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . b31e0rqxiq . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . b31e0rqxiq . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_byanqwdxdk ; rtB . myddvklv2i = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . juevrs3bbp .
CircularBufSize , & rtDW . juevrs3bbp . Last , rtDW . juevrs3bbp . Tail ,
rtDW . juevrs3bbp . Head , rtP . TransportDelay_InitOutput_dgtiattwbx , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . bmjnowtjf1 = ssGetT ( rtS ) ; rtB . aiz4gvzcn1
= ( rtB . bmjnowtjf1 >= rtP . K1_Value_dkgathpkao ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . pkcacfcllc = rtDW . fntnt2vkft ; } if ( rtB .
aiz4gvzcn1 ) { rtB . fxankzb2tl = rtB . cf3kqjr4wo - rtB . myddvklv2i ; rtB .
ce4nspl4gl = rtP . Gain_Gain_njzp2emat5 * rtB . fxankzb2tl ; rtB . if3103l5l4
= rtB . ce4nspl4gl ; } else { rtB . if3103l5l4 = rtB . pkcacfcllc ; } rtB .
geqepjgoiw = rtX . kxjviilbiq ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. nhiqsehhpt . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . nhiqsehhpt . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_hh5y3si44z ; rtB .
h5vr5wbeqt = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . bzmcnbrysf . CircularBufSize , & rtDW . bzmcnbrysf . Last , rtDW .
bzmcnbrysf . Tail , rtDW . bzmcnbrysf . Head , rtP .
TransportDelay_InitOutput_hipwpmfkbv , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
emqz425dbp = ssGetT ( rtS ) ; rtB . lvn523yf40 = ( rtB . emqz425dbp >= rtP .
K1_Value_d3phvl3vkw ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
ezqqcjjlww = rtDW . dmpkou34np ; } if ( rtB . lvn523yf40 ) { rtB . a1nj1ndphd
= rtB . geqepjgoiw - rtB . h5vr5wbeqt ; rtB . kcl3z150mc = rtP .
Gain_Gain_gjzwens0ux * rtB . a1nj1ndphd ; rtB . f1nqrhp0ew = rtB . kcl3z150mc
; } else { rtB . f1nqrhp0ew = rtB . ezqqcjjlww ; } rtB . lulr5ic11c . re =
rtB . if3103l5l4 ; rtB . lulr5ic11c . im = rtB . f1nqrhp0ew ; rtB .
gbvvxwy031 = muDoubleScalarHypot ( rtB . lulr5ic11c . re , rtB . lulr5ic11c .
im ) ; rtB . aelkix3cny = muDoubleScalarAtan2 ( rtB . lulr5ic11c . im , rtB .
lulr5ic11c . re ) ; rtB . c4on2h2stl = rtX . kzm5j4om1j ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . lcvmbgrgk0 . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . lcvmbgrgk0 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_p5bivs2c3n ; rtB . ox0eeoavh0 = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . p5pmnzyfsd .
CircularBufSize , & rtDW . p5pmnzyfsd . Last , rtDW . p5pmnzyfsd . Tail ,
rtDW . p5pmnzyfsd . Head , rtP . TransportDelay_InitOutput_a5xg2bhsao , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . d0xvvirqpa = ssGetT ( rtS ) ; rtB . bzrbj0nu0k
= ( rtB . d0xvvirqpa >= rtP . K1_Value_drkdossccv ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . ljvguudo4a = rtDW . ntlbbe2zlh ; } if ( rtB .
bzrbj0nu0k ) { rtB . hynhuxczuc = rtB . c4on2h2stl - rtB . ox0eeoavh0 ; rtB .
gbxibufozx = rtP . Gain_Gain_iqc0m0h453 * rtB . hynhuxczuc ; rtB . nyb35pqqgb
= rtB . gbxibufozx ; } else { rtB . nyb35pqqgb = rtB . ljvguudo4a ; } rtB .
oqn5awplyp = rtX . hrkjiy2wbe ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. eogglflt5u . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . eogglflt5u . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_nrasksb22c ; rtB .
efhyfbqd1z = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . jaosajyms1 . CircularBufSize , & rtDW . jaosajyms1 . Last , rtDW .
jaosajyms1 . Tail , rtDW . jaosajyms1 . Head , rtP .
TransportDelay_InitOutput_mkcbaww5bm , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
o41wgvff5b = ssGetT ( rtS ) ; rtB . mleqzln43t = ( rtB . o41wgvff5b >= rtP .
K1_Value_cwpx0r11o3 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
kcmvykiu5f = rtDW . k2ecgqxawh ; } if ( rtB . mleqzln43t ) { rtB . pmiic5mlqa
= rtB . oqn5awplyp - rtB . efhyfbqd1z ; rtB . iljhdaxm50 = rtP .
Gain_Gain_lg0dklqsqp * rtB . pmiic5mlqa ; rtB . cwlfifhebb = rtB . iljhdaxm50
; } else { rtB . cwlfifhebb = rtB . kcmvykiu5f ; } rtB . ht11uyaqdy . re =
rtB . nyb35pqqgb ; rtB . ht11uyaqdy . im = rtB . cwlfifhebb ; rtB .
clwjiv4jyv = muDoubleScalarHypot ( rtB . ht11uyaqdy . re , rtB . ht11uyaqdy .
im ) ; rtB . on1aswnk4c = muDoubleScalarAtan2 ( rtB . ht11uyaqdy . im , rtB .
ht11uyaqdy . re ) ; rtB . g1uisjdg5j = rtX . bbxcrevmf2 ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . kp5tjch01i . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . kp5tjch01i . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_ki22t4grdn ; rtB . orremjhszu = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . gimq5qj5re .
CircularBufSize , & rtDW . gimq5qj5re . Last , rtDW . gimq5qj5re . Tail ,
rtDW . gimq5qj5re . Head , rtP . TransportDelay_InitOutput_mnn0oksnci , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . etrm01nnct = ssGetT ( rtS ) ; rtB . jl2oaccvgl
= ( rtB . etrm01nnct >= rtP . K1_Value_grmeq4dt4y ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . p11flzs5ko = rtDW . fbmro5ygjf ; } if ( rtB .
jl2oaccvgl ) { rtB . cstm0nuvfu = rtB . g1uisjdg5j - rtB . orremjhszu ; rtB .
lyz03o4zbz = rtP . Gain_Gain_fwqkaolusw * rtB . cstm0nuvfu ; rtB . awidn2hori
= rtB . lyz03o4zbz ; } else { rtB . awidn2hori = rtB . p11flzs5ko ; } rtB .
dzokxeyx3l = rtX . mecmfgexic ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. gfqptvfbp1 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . gfqptvfbp1 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_d4szan31ch ; rtB .
l1yusxrk1y = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . p24tn1qizr . CircularBufSize , & rtDW . p24tn1qizr . Last , rtDW .
p24tn1qizr . Tail , rtDW . p24tn1qizr . Head , rtP .
TransportDelay_InitOutput_gggrskr5oz , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
hd4d51okmk = ssGetT ( rtS ) ; rtB . jm4wmacjw5 = ( rtB . hd4d51okmk >= rtP .
K1_Value_e2vwsijscc ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
jdnlwctge4 = rtDW . nfnlqxj2g5 ; } if ( rtB . jm4wmacjw5 ) { rtB . mx04v2gmw2
= rtB . dzokxeyx3l - rtB . l1yusxrk1y ; rtB . pcgjznxlh1 = rtP .
Gain_Gain_ecikyq0kt3 * rtB . mx04v2gmw2 ; rtB . gcl4vpc3yl = rtB . pcgjznxlh1
; } else { rtB . gcl4vpc3yl = rtB . jdnlwctge4 ; } rtB . a0qv525yag . re =
rtB . awidn2hori ; rtB . a0qv525yag . im = rtB . gcl4vpc3yl ; rtB .
jmwecn5n3e = muDoubleScalarHypot ( rtB . a0qv525yag . re , rtB . a0qv525yag .
im ) ; rtB . ndkzbcvf0b = muDoubleScalarAtan2 ( rtB . a0qv525yag . im , rtB .
a0qv525yag . re ) ; rtB . bzh5umwrxf [ 0 ] = rtP . Gain2_Gain_ltxdmpdyfi *
rtB . cb4kh2u023 ; rtB . bzh5umwrxf [ 1 ] = rtP . Gain2_Gain_ltxdmpdyfi * rtB
. eq25tfdqgg ; rtB . bzh5umwrxf [ 2 ] = rtP . Gain2_Gain_ltxdmpdyfi * rtB .
irfwlro2qv ; rtB . bzh5umwrxf [ 3 ] = rtP . Gain2_Gain_ltxdmpdyfi * rtB .
d2eszzxgps ; rtB . bzh5umwrxf [ 4 ] = rtP . Gain2_Gain_ltxdmpdyfi * rtB .
avijtxq4vu ; rtB . bzh5umwrxf [ 5 ] = rtP . Gain2_Gain_ltxdmpdyfi * rtB .
hyp2eynh5e ; rtB . bzh5umwrxf [ 6 ] = rtP . Gain2_Gain_ltxdmpdyfi * rtB .
gwwozv2oaj ; rtB . bzh5umwrxf [ 7 ] = rtP . Gain2_Gain_ltxdmpdyfi * rtB .
jetr5pxxjh ; rtB . bzh5umwrxf [ 8 ] = rtP . Gain2_Gain_ltxdmpdyfi * rtB .
gbvvxwy031 ; rtB . bzh5umwrxf [ 9 ] = rtP . Gain2_Gain_ltxdmpdyfi * rtB .
clwjiv4jyv ; rtB . bzh5umwrxf [ 10 ] = rtP . Gain2_Gain_ltxdmpdyfi * rtB .
jmwecn5n3e ; rtB . gglwj1ivl1 = rtP . RadDeg_Gain_ejtgelwilx * rtB .
aa4jom32f5 ; rtB . auqxvxlcxa = rtP . RadDeg_Gain_pvsdywwycd * rtB .
bd5xro2plz ; rtB . dzbdujnjtt = rtP . RadDeg_Gain_n0exojssrs * rtB .
gr34fsdvpw ; rtB . mnndkx52jt = rtP . RadDeg_Gain_myoqgwpqpc * rtB .
fyy25cnxac ; rtB . eadpb4ys5x = rtP . RadDeg_Gain_bn4ay3vgbh * rtB .
o0dfdozprp ; rtB . nuari0w5kl = rtP . RadDeg_Gain_o44ipdice0 * rtB .
oj5iyz0vz2 ; rtB . gzf3zf4z1b = rtP . RadDeg_Gain_cj5goexotu * rtB .
lwktzxdm0t ; rtB . lzpmfnpnz1 = rtP . RadDeg_Gain_mqwmqyjik5 * rtB .
ccv4ay1gwg ; rtB . nac2rb0yso = rtP . RadDeg_Gain_ix42vpfgiw * rtB .
aelkix3cny ; rtB . dd5o12xrpp = rtP . RadDeg_Gain_kwlo5klinc * rtB .
on1aswnk4c ; rtB . oto35tnzx1 = rtP . RadDeg_Gain_j4fhwxdugy * rtB .
ndkzbcvf0b ; rtB . l5lnbu5gps [ 0 ] = rtP . Gain4_Gain_akkpdw4jsz * rtB .
gglwj1ivl1 ; rtB . l5lnbu5gps [ 1 ] = rtP . Gain4_Gain_akkpdw4jsz * rtB .
auqxvxlcxa ; rtB . l5lnbu5gps [ 2 ] = rtP . Gain4_Gain_akkpdw4jsz * rtB .
dzbdujnjtt ; rtB . l5lnbu5gps [ 3 ] = rtP . Gain4_Gain_akkpdw4jsz * rtB .
mnndkx52jt ; rtB . l5lnbu5gps [ 4 ] = rtP . Gain4_Gain_akkpdw4jsz * rtB .
eadpb4ys5x ; rtB . l5lnbu5gps [ 5 ] = rtP . Gain4_Gain_akkpdw4jsz * rtB .
nuari0w5kl ; rtB . l5lnbu5gps [ 6 ] = rtP . Gain4_Gain_akkpdw4jsz * rtB .
gzf3zf4z1b ; rtB . l5lnbu5gps [ 7 ] = rtP . Gain4_Gain_akkpdw4jsz * rtB .
lzpmfnpnz1 ; rtB . l5lnbu5gps [ 8 ] = rtP . Gain4_Gain_akkpdw4jsz * rtB .
nac2rb0yso ; rtB . l5lnbu5gps [ 9 ] = rtP . Gain4_Gain_akkpdw4jsz * rtB .
dd5o12xrpp ; rtB . l5lnbu5gps [ 10 ] = rtP . Gain4_Gain_akkpdw4jsz * rtB .
oto35tnzx1 ; rtB . i1mpevq1t5 = muDoubleScalarCos ( rtB . l5lnbu5gps [ 3 ] )
; rtB . pjxmqrmerd = rtB . bzh5umwrxf [ 3 ] * rtB . i1mpevq1t5 ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . ja4yvukuo5 = rtDW . fzwqkwp2m1 ; }
rtB . jt5eki4pbq = false ; rtB . konjoa2a51 = ssGetT ( rtS ) ; on20slldft (
rtS , rtB . lgijufwztx , rtB . jt5eki4pbq , rtB . konjoa2a51 , rtP .
T1_delay_n3unvwjlrq , & rtB . a1srf3dmr2 , & rtDW . a1srf3dmr2 , & rtP .
a1srf3dmr2 , & rtPrevZCX . a1srf3dmr2 ) ; bhc3qgnuic ( rtS , rtB . fjhinqhwpy
, rtB . jt5eki4pbq , rtB . konjoa2a51 , rtP . T1_delay_n3unvwjlrq , & rtB .
di10rsm5nz , & rtDW . di10rsm5nz , & rtP . di10rsm5nz , & rtPrevZCX .
di10rsm5nz ) ; rtB . j2tk1ylwt1 = ( rtB . a1srf3dmr2 . j1vu52vt5l || rtB .
di10rsm5nz . mjqylog0rb ) ; rtB . newpgldwuo = ( rtB . ja4yvukuo5 || rtB .
j2tk1ylwt1 ) ; rtB . e3wl3hljhw = rtB . ouucq52d4y ; rtB . ordhxzobtn =
ssGetT ( rtS ) ; on20slldft ( rtS , rtB . if03e2es0p , rtB . e3wl3hljhw , rtB
. ordhxzobtn , rtP . T2_delay_mudeqaqx1z , & rtB . pry2hnvt4q , & rtDW .
pry2hnvt4q , & rtP . pry2hnvt4q , & rtPrevZCX . pry2hnvt4q ) ; bhc3qgnuic (
rtS , rtB . kqihty0pex , rtB . e3wl3hljhw , rtB . ordhxzobtn , rtP .
T2_delay_mudeqaqx1z , & rtB . hgwrktvi4c , & rtDW . hgwrktvi4c , & rtP .
hgwrktvi4c , & rtPrevZCX . hgwrktvi4c ) ; rtB . f4tz4ooxph = ( rtB .
pry2hnvt4q . j1vu52vt5l || rtB . hgwrktvi4c . mjqylog0rb ) ; rtB . o4lr3fikgn
= ! rtB . f4tz4ooxph ; rtB . cdxhxlj30g = ( rtB . newpgldwuo && rtB .
o4lr3fikgn ) ; if ( rtB . cdxhxlj30g ) { rtB . cigvk2tavt = rtB . ohbqabvobv
; } else { rtB . cigvk2tavt = rtP . u8_Value_nplhpcg1vd ; } rtB . ks5m0lojk1
= rtB . pjxmqrmerd * rtB . cigvk2tavt ; if ( ssIsSampleHit ( rtS , 1 , 0 ) )
{ rtB . movmnniyeg = rtDW . jvvuz2zzbv ; } rtB . ces4axmjxa = false ; rtB .
h55wicjf4m = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . dynksvy0hi , rtB .
ces4axmjxa , rtB . h55wicjf4m , rtP . T3_delay_anqjt5xtzp , & rtB .
apyjjt13q1 , & rtDW . apyjjt13q1 , & rtP . apyjjt13q1 , & rtPrevZCX .
apyjjt13q1 ) ; bhc3qgnuic ( rtS , rtB . kwqertnn23 , rtB . ces4axmjxa , rtB .
h55wicjf4m , rtP . T3_delay_anqjt5xtzp , & rtB . arqjd0gbjl , & rtDW .
arqjd0gbjl , & rtP . arqjd0gbjl , & rtPrevZCX . arqjd0gbjl ) ; rtB .
ck30twldpf = ( rtB . apyjjt13q1 . j1vu52vt5l || rtB . arqjd0gbjl . mjqylog0rb
) ; rtB . ekiqymeilu = ( rtB . movmnniyeg || rtB . ck30twldpf ) ; rtB .
oq3pe4t5kl = rtB . o0vt4k10iv ; rtB . fal1cpgu0g = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . d141kjjcm3 , rtB . oq3pe4t5kl , rtB . fal1cpgu0g ,
rtP . T4_delay_nqnoy5noql , & rtB . e1cruf0x3a , & rtDW . e1cruf0x3a , & rtP
. e1cruf0x3a , & rtPrevZCX . e1cruf0x3a ) ; bhc3qgnuic ( rtS , rtB .
gpok20ykam , rtB . oq3pe4t5kl , rtB . fal1cpgu0g , rtP . T4_delay_nqnoy5noql
, & rtB . pg41d4mdss , & rtDW . pg41d4mdss , & rtP . pg41d4mdss , & rtPrevZCX
. pg41d4mdss ) ; rtB . npgp4zane2 = ( rtB . e1cruf0x3a . j1vu52vt5l || rtB .
pg41d4mdss . mjqylog0rb ) ; rtB . f0dhedg0eb = ! rtB . npgp4zane2 ; rtB .
gz04icgcdm = ( rtB . ekiqymeilu && rtB . f0dhedg0eb ) ; if ( rtB . gz04icgcdm
) { rtB . esximhugp5 = rtB . mnxuhtdjzf ; } else { rtB . esximhugp5 = rtP .
u2_Value_fcpr1dimjd ; } rtB . gmpdcm4kyq = 0.0 * rtB . esximhugp5 ; rtB .
hlmtyy2tbw = rtB . ks5m0lojk1 + rtB . gmpdcm4kyq ; rtB . ajrrt2noil = rtB .
hlmtyy2tbw * rtB . m5lfhmm3np ; rtB . bkvfmg2ck2 = muDoubleScalarCos ( rtB .
l5lnbu5gps [ 4 ] ) ; rtB . i5dpx5t34o = rtB . bzh5umwrxf [ 4 ] * rtB .
bkvfmg2ck2 ; rtB . d1ccxekuk3 = rtB . i5dpx5t34o * rtB . cigvk2tavt ; rtB .
awofkt25il = 0.0 * rtB . esximhugp5 ; rtB . asa103d02z = rtB . d1ccxekuk3 +
rtB . awofkt25il ; rtB . e51eos4lbg = muDoubleScalarCos ( rtB . l5lnbu5gps [
5 ] ) ; rtB . lnv3d3rvor = rtB . bzh5umwrxf [ 5 ] * rtB . e51eos4lbg ; rtB .
krwtwxbtsp = rtB . lnv3d3rvor * rtB . cigvk2tavt ; rtB . cwhtz3m5ng = 0.0 *
rtB . esximhugp5 ; rtB . fizn4ck4n1 = rtB . krwtwxbtsp + rtB . cwhtz3m5ng ;
rtB . dq1inndtu5 = rtB . asa103d02z + rtB . fizn4ck4n1 ; rtB . of1eujelbb =
rtB . dq1inndtu5 + rtB . hlmtyy2tbw ; rtB . gamkho1q2f = rtB . of1eujelbb *
rtP . u7_Value_cmbk5pxons ; if ( rtP . u_Value_eqa0ytrizk > rtP .
u_Threshold_lkwmwzksni ) { rtB . fqmsopavlk = rtB . gamkho1q2f * rtB .
m5lfhmm3np ; rtB . liwbcbzido = rtB . ajrrt2noil - rtB . fqmsopavlk ; rtB .
k5g15izqi2 = rtB . liwbcbzido ; } else { rtB . k5g15izqi2 = rtB . ajrrt2noil
; } rtB . gjyses2hyp = rtB . k5g15izqi2 * rtB . bi3qqodrui ; rtB . eenmrwaxdj
= muDoubleScalarSin ( rtB . l5lnbu5gps [ 3 ] ) ; rtB . koph4w1vnf = rtB .
eenmrwaxdj * rtB . bzh5umwrxf [ 3 ] ; rtB . hqqawjx4cc = rtB . koph4w1vnf *
rtB . cigvk2tavt ; rtB . b2qf5u1qni = 0.0 * rtB . esximhugp5 ; rtB .
krvqv214qo = rtB . hqqawjx4cc + rtB . b2qf5u1qni ; rtB . ao1x1h2eb5 = rtB .
krvqv214qo * rtB . m5lfhmm3np ; rtB . pm0m2mdj1i = muDoubleScalarSin ( rtB .
l5lnbu5gps [ 4 ] ) ; rtB . jlt5ytbte0 = rtB . pm0m2mdj1i * rtB . bzh5umwrxf [
4 ] ; rtB . pvw0lfwbbj = rtB . jlt5ytbte0 * rtB . cigvk2tavt ; rtB .
nczhcgvbtn = 0.0 * rtB . esximhugp5 ; rtB . ddix2mhi1g = rtB . pvw0lfwbbj +
rtB . nczhcgvbtn ; rtB . gpmfk234qe = muDoubleScalarSin ( rtB . l5lnbu5gps [
5 ] ) ; rtB . chvsplbxtl = rtB . gpmfk234qe * rtB . bzh5umwrxf [ 5 ] ; rtB .
bir2yl1r4o = rtB . chvsplbxtl * rtB . cigvk2tavt ; rtB . j5q41gky1c = 0.0 *
rtB . esximhugp5 ; rtB . ndiffna1hx = rtB . bir2yl1r4o + rtB . j5q41gky1c ;
rtB . ohaveolja2 = rtB . ddix2mhi1g + rtB . ndiffna1hx ; rtB . jtgxxeac0n =
rtB . ohaveolja2 + rtB . krvqv214qo ; rtB . aafebvzsoj = rtB . jtgxxeac0n *
rtP . u7_Value_cmbk5pxons ; if ( rtP . u_Value_eqa0ytrizk > rtP .
u_Threshold_afohdwynce ) { rtB . ibch1ygkms = rtB . aafebvzsoj * rtB .
m5lfhmm3np ; rtB . okg2sytjx0 = rtB . ao1x1h2eb5 - rtB . ibch1ygkms ; rtB .
kzhzus12q5 = rtB . okg2sytjx0 ; } else { rtB . kzhzus12q5 = rtB . ao1x1h2eb5
; } rtB . ifte0an0df = rtB . kzhzus12q5 * rtB . dsaeva2wd0 ; rtB . k01n0yrx1p
= rtB . gjyses2hyp - rtB . ifte0an0df ; rtB . a1qfsyoi1n = ! rtB . cdxhxlj30g
; rtB . hpzudxgx3o = ! rtB . gz04icgcdm ; rtB . do24p4hxxw = ( rtB .
a1qfsyoi1n && rtB . hpzudxgx3o ) ; rtB . kiz4q1fpwc = rtB . pjxmqrmerd * rtB
. pjxmqrmerd ; rtB . derkmlly3l = rtB . koph4w1vnf * rtB . koph4w1vnf ; rtB .
nwwx2p4zui = rtB . kiz4q1fpwc + rtB . derkmlly3l ; rtB . mh1rmrli3y =
muDoubleScalarSqrt ( rtB . nwwx2p4zui ) ; rtB . nxow5k3u3l = ( rtB .
mh1rmrli3y > rtP . u9_Value_gwa1ux021y ) ; rtB . oounn0ufba = ( rtB .
nxow5k3u3l && rtB . cdxhxlj30g ) ; rtB . bi5bqgqujx = ( rtB . paz1mb5sqz &&
rtB . gz04icgcdm ) ; rtB . n041v55ew1 = ( rtB . cdxhxlj30g && rtB .
gz04icgcdm ) ; if ( rtB . do24p4hxxw ) { rtB . gfj43a5l4q = rtP .
u7_Value_knfw4nz5wz ; } else { if ( rtB . n041v55ew1 ) { rtB . ebmcx0coo0 = (
rtB . oounn0ufba || rtB . bi5bqgqujx ) ; rtB . cb4eyvidbo = ( rtB .
ebmcx0coo0 && rtB . n041v55ew1 ) ; rtB . nezjf2zi0s = rtB . cb4eyvidbo ; }
else { if ( rtB . cdxhxlj30g ) { rtB . eiyfhs1qd4 = rtB . oounn0ufba ; } else
{ rtB . eiyfhs1qd4 = rtB . bi5bqgqujx ; } rtB . nezjf2zi0s = rtB . eiyfhs1qd4
; } rtB . gfj43a5l4q = rtB . nezjf2zi0s ; } if ( rtP . u_Value_h4eaya5qm4 >
rtP . u_Threshold_e4frkw14qf ) { if ( rtB . gfj43a5l4q > rtP .
u_Threshold_altb3wmit3 ) { rtB . o34r4hkocz = rtB . k01n0yrx1p ; } else { rtB
. o34r4hkocz = rtP . u54_Value_c3kx21cngg ; } rtB . ezogaajaeh = rtB .
o34r4hkocz ; } else { rtB . ezogaajaeh = rtP . u54_Value_c3kx21cngg ; } rtB .
kob4koff0c = rtB . cbz2phbqou + rtB . ezogaajaeh ; rtB . n2se3psd1e = rtB .
kob4koff0c * rtB . kob4koff0c ; rtB . eolgpv3k0b = rtB . mhrqxa3yrk * rtB .
lda5igrpdy ; rtB . fnwbo2ydb5 = rtB . hwfoco4241 * rtB . peijvg4ilw ; rtB .
kw14svafax = rtB . eolgpv3k0b + rtB . fnwbo2ydb5 ; if ( rtP .
u_Value_iuoi1xcwxh > rtP . u_Threshold_ndpmbmf5pl ) { if ( rtB . laxu4i4bvg >
rtP . u_Threshold_nmw5hybqz4 ) { rtB . apquycwtux = rtB . kw14svafax ; } else
{ rtB . apquycwtux = rtP . u54_Value_fcmc5me3hr ; } rtB . hslrpa5jjt = rtB .
apquycwtux ; } else { rtB . hslrpa5jjt = rtP . u54_Value_fcmc5me3hr ; } rtB .
aqmhod1n40 = rtB . jsgo1z1x51 + rtB . hslrpa5jjt ; rtB . n1ppanbxvk = rtB .
kzhzus12q5 * rtB . bi3qqodrui ; rtB . ehgmapt2x4 = rtB . k5g15izqi2 * rtB .
dsaeva2wd0 ; rtB . intuomh1ly = rtB . n1ppanbxvk + rtB . ehgmapt2x4 ; if (
rtP . u_Value_h4eaya5qm4 > rtP . u_Threshold_j3daou1soe ) { if ( rtB .
gfj43a5l4q > rtP . u_Threshold_hlb2o1ecwh ) { rtB . fd02u22fhy = rtB .
intuomh1ly ; } else { rtB . fd02u22fhy = rtP . u54_Value_c3kx21cngg ; } rtB .
k3dj3wwkgn = rtB . fd02u22fhy ; } else { rtB . k3dj3wwkgn = rtP .
u54_Value_c3kx21cngg ; } rtB . bv23hijrbu = rtB . aqmhod1n40 + rtB .
k3dj3wwkgn ; rtB . mq1j0ahzdo = rtB . bv23hijrbu * rtB . bv23hijrbu ; rtB .
lcq5yirapc = rtB . n2se3psd1e + rtB . mq1j0ahzdo ; rtB . g53tgrjxee =
muDoubleScalarSqrt ( rtB . lcq5yirapc ) ; rtB . bqbmlmqn0w = rtB . lehgezo4xp
* rtB . lehgezo4xp ; rtB . engtikzyur = rtB . hslrpa5jjt * rtB . hslrpa5jjt ;
rtB . njhlym4w1g = rtB . bqbmlmqn0w + rtB . engtikzyur ; rtB . nysa0fzu3h =
muDoubleScalarSqrt ( rtB . njhlym4w1g ) ; rtB . bh5misjn54 = ( rtB .
ekvelrpz4b > rtB . nysa0fzu3h ) ; if ( rtB . bh5misjn54 ) { rtB . idttvet3o1
= rtB . mupscucpvr ; rtB . hmeyepht4j = rtB . gpaztx4x30 ; } else { rtB .
idttvet3o1 = rtB . lehgezo4xp ; rtB . hmeyepht4j = rtB . hslrpa5jjt ; } rtB .
afqppvoha4 = rtB . idttvet3o1 * rtB . idttvet3o1 ; rtB . fh03eihyjo = rtB .
hmeyepht4j * rtB . hmeyepht4j ; rtB . loo0t2k4ow = rtB . afqppvoha4 + rtB .
fh03eihyjo ; rtB . puytrgpcaq = muDoubleScalarSqrt ( rtB . loo0t2k4ow ) ; rtB
. lbptz4ayox = rtB . ezogaajaeh * rtB . ezogaajaeh ; rtB . oovpkt0nhq = rtB .
k3dj3wwkgn * rtB . k3dj3wwkgn ; rtB . padxws3ro2 = rtB . lbptz4ayox + rtB .
oovpkt0nhq ; rtB . i2prjq4wdc = muDoubleScalarSqrt ( rtB . padxws3ro2 ) ; rtB
. fd2tdz2oux = ( rtB . puytrgpcaq > rtB . i2prjq4wdc ) ; if ( rtB .
fd2tdz2oux ) { rtB . lycbvdqjbi = rtB . idttvet3o1 ; rtB . l54g0x1tt4 = rtB .
hmeyepht4j ; } else { rtB . lycbvdqjbi = rtB . ezogaajaeh ; rtB . l54g0x1tt4
= rtB . k3dj3wwkgn ; } rtB . hhu25vc3gh = rtB . lycbvdqjbi - rtB . kob4koff0c
; rtB . o1kxlkvs3a = rtB . l54g0x1tt4 - rtB . bv23hijrbu ; rtB . lcckdt3dqs =
rtB . lycbvdqjbi * rtB . lycbvdqjbi ; rtB . d21lmwndex = rtB . l54g0x1tt4 *
rtB . l54g0x1tt4 ; rtB . fqduw0hl23 = rtB . lcckdt3dqs + rtB . d21lmwndex ;
rtB . ldgborxogx = muDoubleScalarSqrt ( rtB . fqduw0hl23 ) ; rtB . hvhevnl0vl
= rtB . lycbvdqjbi / rtB . ldgborxogx ; rtB . ej0tp355gg = rtB . hhu25vc3gh *
rtB . hhu25vc3gh ; rtB . hvsadgqzxw = rtB . o1kxlkvs3a * rtB . o1kxlkvs3a ;
rtB . f2odzp22p2 = rtB . ej0tp355gg + rtB . hvsadgqzxw ; rtB . j03yx100s5 =
muDoubleScalarSqrt ( rtB . f2odzp22p2 ) ; rtB . jawznz4uca = rtB . hhu25vc3gh
/ rtB . j03yx100s5 ; rtB . pbpqyvj1my = rtB . hvhevnl0vl - rtB . jawznz4uca ;
rtB . ojxlu2xl4m = rtB . pbpqyvj1my * rtB . pbpqyvj1my ; rtB . hsxghbgkmo =
rtB . l54g0x1tt4 / rtB . ldgborxogx ; rtB . krrsn2vno0 = rtB . o1kxlkvs3a /
rtB . j03yx100s5 ; rtB . ounv3uhjyy = rtB . hsxghbgkmo - rtB . krrsn2vno0 ;
rtB . ou3xjiy31q = rtB . ounv3uhjyy * rtB . ounv3uhjyy ; rtB . ioh5ei45gi =
rtB . ojxlu2xl4m + rtB . ou3xjiy31q ; rtB . pmkpvryk0i = rtB . ioh5ei45gi *
rtP . Constant3_Value_kxckuoqejt ; rtB . et3pzwzixk = rtP .
Constant2_Value_ccx1zvy40m - rtB . pmkpvryk0i ; u0 = rtB . et3pzwzixk ; if (
u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } rtB .
ct4azjnvig = muDoubleScalarAcos ( u0 ) ; rtB . f1onkgyfjv = rtB . hsxghbgkmo
- rtB . jawznz4uca ; rtB . hhmma1jdmx = rtB . f1onkgyfjv * rtB . f1onkgyfjv ;
rtB . oz1zlbcxwe = rtB . hvhevnl0vl + rtB . krrsn2vno0 ; rtB . lwpsyorvi5 =
rtB . oz1zlbcxwe * rtB . oz1zlbcxwe ; rtB . emqp1x2kai = rtB . hhmma1jdmx +
rtB . lwpsyorvi5 ; rtB . f14mnvro5v = ( rtB . emqp1x2kai > rtP .
Constant_Value_h151lavqbz ) ; if ( rtB . f14mnvro5v ) { rtB . nbmn2zonan =
rtB . ct4azjnvig * rtP . Constant1_Value_lqsedy4on2 ; rtB . nrz1ahzyq2 = rtB
. nbmn2zonan ; } else { rtB . nrz1ahzyq2 = rtB . ct4azjnvig ; } rtB .
p2t4ioqhva = muDoubleScalarCos ( rtB . nrz1ahzyq2 ) ; rtB . juxqdgiuwk = (
rtB . p2t4ioqhva >= rtP . u_Value_iyiza3iutg ) ; if ( rtB . juxqdgiuwk ) {
rtB . bddd31mbhd = rtB . o1kxlkvs3a * rtB . o1kxlkvs3a ; rtB . fq3kmdtrtz =
rtB . hhu25vc3gh * rtB . hhu25vc3gh ; rtB . bblnrgye4e = rtB . fq3kmdtrtz +
rtB . bddd31mbhd ; rtB . aeezt4cqt5 = muDoubleScalarSqrt ( rtB . bblnrgye4e )
; rtB . addjvping5 = rtB . l54g0x1tt4 * rtB . l54g0x1tt4 ; rtB . ood0kfw3g3 =
rtB . lycbvdqjbi * rtB . lycbvdqjbi ; rtB . koohzb2ttn = rtB . ood0kfw3g3 +
rtB . addjvping5 ; rtB . c0zxb1nfkm = muDoubleScalarSqrt ( rtB . koohzb2ttn )
; rtB . hg30xqtzp0 = rtB . c0zxb1nfkm * rtB . aeezt4cqt5 * rtB . p2t4ioqhva ;
rtB . ntxl3d4yux = muDoubleScalarSqrt ( rtB . hg30xqtzp0 ) ; rtB . ee1e031vk3
= rtB . ntxl3d4yux ; } else { rtB . ee1e031vk3 = rtP . u7_Value_ds2brihoaq ;
} if ( rtB . hqd3ppb3sy ) { rtB . bh3dx0wzov = rtP . u1_Value_a0mbjao3xr ; }
else { rtB . fatlfnlocw = ( rtB . ee1e031vk3 > rtB . gqcucluh02 ) ; if ( rtB
. fatlfnlocw ) { if ( rtB . h0fgnghna4 ) { rtB . pacmklywzz = rtB .
elrmpljgts * rtB . ee1e031vk3 ; rtB . byjceona4u = rtB . pacmklywzz + rtB .
furimuxjih ; rtB . aycmvylnir = rtB . byjceona4u ; } else { rtB . aycmvylnir
= rtB . nkbpr3wboi ; } rtB . jqcksvvpao = rtB . aycmvylnir ; } else { rtB .
ehc2as0qlk = ( rtB . ee1e031vk3 <= rtB . gqcucluh02 ) ; rtB . a14cibrfn1 = (
rtB . ee1e031vk3 > rtP . u_Value_n0ldpwgypr ) ; rtB . br2w13v3ng = ( rtB .
a14cibrfn1 && rtB . ehc2as0qlk ) ; if ( rtB . br2w13v3ng ) { rtB . hscxlruxdz
= rtB . ee1e031vk3 * rtB . elrmpljgts ; rtB . cp4s1lmwo3 = rtB . hscxlruxdz +
rtB . furimuxjih ; rtB . fmwkhlwse3 = rtB . cp4s1lmwo3 ; } else { rtB .
fmwkhlwse3 = rtP . u1_Value_a0mbjao3xr ; } rtB . jqcksvvpao = rtB .
fmwkhlwse3 ; } rtB . bh3dx0wzov = rtB . jqcksvvpao ; } if ( rtB . n0of4wg12k
) { rtB . ewqsssuimk = rtB . bh3dx0wzov ; } else { rtB . lwut1wr5db = ( rtB .
ee1e031vk3 > rtB . odu4dgb4dj ) ; if ( rtB . lwut1wr5db ) { if ( rtB .
aum0a5ysxg ) { rtB . lns04po1jx = rtB . gptngaxuqn * rtB . ee1e031vk3 ; rtB .
ojbbrhua0b = rtB . lns04po1jx + rtB . oi0ry2exrv ; rtB . oyfccogzce = rtB .
ojbbrhua0b ; } else { rtB . oyfccogzce = rtB . nkbpr3wboi ; } rtB .
pgi3qz513n = rtB . oyfccogzce ; } else { rtB . crucfet24m = ( rtB .
ee1e031vk3 <= rtB . odu4dgb4dj ) ; rtB . a3q3yi2pms = ( rtB . ee1e031vk3 >
rtP . u2_Value_c1sg11dlld ) ; rtB . lpp0wz5y2q = ( rtB . a3q3yi2pms && rtB .
crucfet24m ) ; if ( rtB . lpp0wz5y2q ) { rtB . iuz3hrshzg = rtB . ee1e031vk3
* rtB . gptngaxuqn ; rtB . dpsv3nc2tl = rtB . iuz3hrshzg + rtB . oi0ry2exrv ;
rtB . mubryeeuwh = rtB . dpsv3nc2tl ; } else { rtB . mubryeeuwh = rtB .
bh3dx0wzov ; } rtB . pgi3qz513n = rtB . mubryeeuwh ; } rtB . ewqsssuimk = rtB
. pgi3qz513n ; } if ( rtB . eawyepqgpv ) { rtB . ccjlu4x1rk = rtB .
ewqsssuimk ; } else { rtB . a0zpxh0t4t = ( rtB . ee1e031vk3 > rtB .
lezo3tixio ) ; if ( rtB . a0zpxh0t4t ) { if ( rtB . djydba5yvz ) { rtB .
ergy1tseuj = rtB . imuxfxrxnx * rtB . ee1e031vk3 ; rtB . ijsllfnrpn = rtB .
ergy1tseuj + rtB . mgbcoil5s3 ; rtB . dn1tlzr2pc = rtB . ijsllfnrpn ; } else
{ rtB . dn1tlzr2pc = rtB . nkbpr3wboi ; } rtB . b2deblihnb = rtB . dn1tlzr2pc
; } else { rtB . c1rql13qlq = ( rtB . ee1e031vk3 <= rtB . lezo3tixio ) ; rtB
. f40jt2pffw = ( rtB . ee1e031vk3 > rtP . u4_Value ) ; rtB . bjdh5xstls = (
rtB . f40jt2pffw && rtB . c1rql13qlq ) ; if ( rtB . bjdh5xstls ) { rtB .
gun5q3vtew = rtB . ee1e031vk3 * rtB . imuxfxrxnx ; rtB . myoijcuwpe = rtB .
gun5q3vtew + rtB . mgbcoil5s3 ; rtB . jegxcmc3ck = rtB . myoijcuwpe ; } else
{ rtB . jegxcmc3ck = rtB . ewqsssuimk ; } rtB . b2deblihnb = rtB . jegxcmc3ck
; } rtB . ccjlu4x1rk = rtB . b2deblihnb ; } if ( rtB . bramtmeqan ) { rtB .
dzlngenind = rtB . ccjlu4x1rk ; } else { rtB . o1w5n34ilp = ( rtB .
ee1e031vk3 > rtB . lm2m3iulh1 ) ; if ( rtB . o1w5n34ilp ) { if ( rtB .
ptahzsefvq ) { rtB . aso3w1lvaz = rtB . f00oz24hvd * rtB . ee1e031vk3 ; rtB .
fbgkpjoa35 = rtB . aso3w1lvaz + rtB . bh5fvogibc ; rtB . huf04q5azy = rtB .
fbgkpjoa35 ; } else { rtB . huf04q5azy = rtB . nkbpr3wboi ; } rtB .
oprl4wbyvh = rtB . huf04q5azy ; } else { rtB . ax1ptkj5a2 = ( rtB .
ee1e031vk3 <= rtB . lm2m3iulh1 ) ; rtB . fgdjlcx11y = ( rtB . ee1e031vk3 >
rtP . u6_Value_iu0fx4dooz ) ; rtB . eyznt04nvk = ( rtB . fgdjlcx11y && rtB .
ax1ptkj5a2 ) ; if ( rtB . eyznt04nvk ) { rtB . p4xug0l0g0 = rtB . ee1e031vk3
* rtB . f00oz24hvd ; rtB . l13sxepeef = rtB . p4xug0l0g0 + rtB . bh5fvogibc ;
rtB . a2nnsiqd1r = rtB . l13sxepeef ; } else { rtB . a2nnsiqd1r = rtB .
ccjlu4x1rk ; } rtB . oprl4wbyvh = rtB . a2nnsiqd1r ; } rtB . dzlngenind = rtB
. oprl4wbyvh ; } rtB . cjhhm5q014 = ( rtB . g53tgrjxee > rtB . dzlngenind ) ;
rtB . e34dklcgle = ( rtB . dzlngenind == rtP . u5_Value_pynaujk2tf ) ; rtB .
okfbj1dnto = ! rtB . e34dklcgle ; rtB . kabk0vopg4 = ( rtB . cjhhm5q014 &&
rtB . okfbj1dnto ) ; rtB . bibkqb3mf2 = ( rtB . g53tgrjxee > rtB . nkbpr3wboi
) ; rtB . iim02ibhqy = ( rtB . bibkqb3mf2 && rtB . krvxl5nblc ) ; rtB .
gu30hru3p3 = ( rtB . kabk0vopg4 || rtB . iim02ibhqy ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . fryfe3oj5i = rtDW . dsi2mx1crg ; rtB . otlog00ob2 = !
rtB . fryfe3oj5i ; } if ( rtP . u1_Value_nswrjlywfh > rtP .
u_Threshold_m00vnwciir ) { rtB . kswqfl0ube = ( rtB . gu30hru3p3 && rtB .
otlog00ob2 ) ; rtB . l3t4dtm1qd = rtB . kswqfl0ube ; } else { rtB .
l3t4dtm1qd = rtB . gu30hru3p3 ; } rtB . n2wifhoi23 = rtB . hlmtyy2tbw * rtB .
hlmtyy2tbw ; rtB . dryzkj52kg = rtB . krvqv214qo * rtB . krvqv214qo ; rtB .
eucreflifu = rtB . n2wifhoi23 + rtB . dryzkj52kg ; rtB . gm13p0jjjr =
muDoubleScalarSqrt ( rtB . eucreflifu ) ; rtB . cyksbqxjzc = ( rtB .
gm13p0jjjr < 0.0 ) ; rtB . edgbcn3o1w = ! rtB . cyksbqxjzc ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . ohyeqpd5va = rtDW . p0tzcauox5 ; rtB
. o0qdd2m5ng = rtDW . cjcrhlf0g0 ; } rtB . fe1nfy2yso = rtB . hlmtyy2tbw -
rtB . ohyeqpd5va ; rtB . fot3qy4xda = rtB . fe1nfy2yso * rtB . fe1nfy2yso ;
rtB . bruvmda2wf = rtB . krvqv214qo - rtB . o0qdd2m5ng ; rtB . lussqgeox3 =
rtB . bruvmda2wf * rtB . bruvmda2wf ; rtB . omnacjaxxm = rtB . fot3qy4xda +
rtB . lussqgeox3 ; rtB . lo2okwom4h = muDoubleScalarSqrt ( rtB . omnacjaxxm )
; rtB . n0kqxcb1hp = rtB . hlmtyy2tbw * rtB . hlmtyy2tbw ; rtB . hsmyqmw0pz =
rtB . krvqv214qo * rtB . krvqv214qo ; rtB . krgzwps0py = rtB . n0kqxcb1hp +
rtB . hsmyqmw0pz ; rtB . f1lqhq0qme = muDoubleScalarSqrt ( rtB . krgzwps0py )
; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . ozniczd15e = rtB . ohyeqpd5va
* rtB . ohyeqpd5va ; rtB . kfnbw1mzqm = rtB . o0qdd2m5ng * rtB . o0qdd2m5ng ;
rtB . o3do4bs1b3 = rtB . ozniczd15e + rtB . kfnbw1mzqm ; rtB . a0pjszrvty =
muDoubleScalarSqrt ( rtB . o3do4bs1b3 ) ; } maxV = muDoubleScalarMax ( rtB .
f1lqhq0qme , rtB . a0pjszrvty ) ; rtB . puiac3eowe = maxV ; rtB . npqxx5odvw
= ( rtB . puiac3eowe == rtP . u_Value_lnaksbsz14 ) ; if ( rtB . npqxx5odvw )
{ rtB . ewx0q3adji = rtP . u_Value_c4tn4k01br ; } else { rtB . ewx0q3adji =
rtB . puiac3eowe ; } rtB . nwnpz32fgh = rtB . lo2okwom4h / rtB . ewx0q3adji ;
rtB . os0gelo1nv = ( rtB . nwnpz32fgh > rtP . u0_Value_oix5pcw4qm ) ; rtB .
d1twsnn020 = ( rtB . edgbcn3o1w && rtB . os0gelo1nv ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . gtfhuwgpxk = rtDW . a3t0rfsk4v ; rtB . nze1yi5rvq =
rtDW . fj1oj1qia0 ; rtB . bljdkwzou2 = rtDW . mpcjp04f1l ; rtB . o5tvxpfxme =
( rtB . gtfhuwgpxk + rtB . nze1yi5rvq ) + rtB . bljdkwzou2 ; rtB . fh3kospjve
= rtB . o5tvxpfxme * rtP . u_Value_jjen03jw2c ; } rtB . pzri3d5mlf = rtB .
lo2okwom4h - rtB . fh3kospjve ; rtB . ccjbhnijqp = muDoubleScalarAbs ( rtB .
pzri3d5mlf ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . kzr5qzoedk = (
rtB . fh3kospjve == rtP . u_Value_e3jp2advft ) ; if ( rtB . kzr5qzoedk ) {
rtB . k1b4nnnjxz = rtP . u_Value_mkjgdkgytp ; } else { rtB . k1b4nnnjxz = rtB
. fh3kospjve ; } rtB . mtprsw20vk = rtDW . f5mcliqi4f ; rtB . hytm0kjtja =
rtDW . pybydd0eqj ; rtB . lm2bogehna = ! rtB . hytm0kjtja ; rtB . poapg50hmb
= ( rtB . mtprsw20vk && rtB . lm2bogehna ) ; } rtB . it5e0soa0a = rtB .
ccjbhnijqp / rtB . k1b4nnnjxz ; rtB . gwtde5nzge = ( rtB . it5e0soa0a > rtP .
u8_Value_dwoawxwhqs ) ; rtB . gxsszkfxoo = ( rtB . lo2okwom4h < rtP .
u5_Value_dsjflch1iw ) ; rtB . ppvclonoqs = ! rtB . gxsszkfxoo ; rtB .
jw4wr4mcvr = ( rtB . gwtde5nzge && rtB . ppvclonoqs ) ; rtB . ebxp2katk3 = (
rtB . d1twsnn020 && rtB . jw4wr4mcvr ) ; rtB . ifssonf1yl = ( rtB .
ebxp2katk3 || rtB . poapg50hmb ) ; rtB . eoe0rgmk0i = rtB . ifssonf1yl ; rtB
. f1nvd2e5k1 = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . mmqmljtopv , rtB .
eoe0rgmk0i , rtB . f1nvd2e5k1 , rtP . OffDelay_delay_bnu2ros4s1 , & rtB .
dej3sbopt3 , & rtDW . dej3sbopt3 , & rtP . dej3sbopt3 , & rtPrevZCX .
dej3sbopt3 ) ; bhc3qgnuic ( rtS , rtB . mewwllg0zx , rtB . eoe0rgmk0i , rtB .
f1nvd2e5k1 , rtP . OffDelay_delay_bnu2ros4s1 , & rtB . ongp1nebfx , & rtDW .
ongp1nebfx , & rtP . ongp1nebfx , & rtPrevZCX . ongp1nebfx ) ; rtB .
dibopprtmu = ( rtB . dej3sbopt3 . j1vu52vt5l || rtB . ongp1nebfx . mjqylog0rb
) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . ixqct3wbex = rtDW .
j54aiqlmrr ; rtB . dnboccccuq = ! rtB . ixqct3wbex ; } rtB . o4uojz5k3z = (
rtB . dibopprtmu && rtB . dnboccccuq ) ; rtB . a0it33vpv3 = ( rtB .
o4uojz5k3z && rtB . p0h4azjag2 ) ; rtB . efrskbhfhg = ( rtB . k3om0acsgh ||
rtB . a0it33vpv3 ) ; rtB . ifozmk2mcx = rtB . efrskbhfhg ; rtB . b5klo0ngjy =
ssGetT ( rtS ) ; on20slldft ( rtS , rtB . exlkhevyls , rtB . ifozmk2mcx , rtB
. b5klo0ngjy , rtP . T1_delay_gjzr10f3lj , & rtB . hvibrw2awu , & rtDW .
hvibrw2awu , & rtP . hvibrw2awu , & rtPrevZCX . hvibrw2awu ) ; bhc3qgnuic (
rtS , rtB . gxlekcoxi2 , rtB . ifozmk2mcx , rtB . b5klo0ngjy , rtP .
T1_delay_gjzr10f3lj , & rtB . allgw5ptgk , & rtDW . allgw5ptgk , & rtP .
allgw5ptgk , & rtPrevZCX . allgw5ptgk ) ; rtB . fzufywui32 = ( rtB .
hvibrw2awu . j1vu52vt5l || rtB . allgw5ptgk . mjqylog0rb ) ; rtB . kz3lkb351n
= ( rtB . l3t4dtm1qd && rtB . fzufywui32 ) ; rtB . c4p5csisad = ( rtB .
j2ljtehmn5 || rtB . kz3lkb351n ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB
. jmxhqxvga3 = rtDW . gokujearn0 ; } rtB . mdezt12b14 = ! rtB . gu30hru3p3 ;
rtB . aoyufyefs3 = ( rtB . jmxhqxvga3 && rtB . mdezt12b14 ) ; rtB .
kt51zrjm3b = rtB . aoyufyefs3 ; rtB . c2xffyhecd = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . mmh5eqfznn , rtB . kt51zrjm3b , rtB . c2xffyhecd ,
rtP . T2_delay_evaan1curx , & rtB . f5lqeh5sbp , & rtDW . f5lqeh5sbp , & rtP
. f5lqeh5sbp , & rtPrevZCX . f5lqeh5sbp ) ; bhc3qgnuic ( rtS , rtB .
lvi5ymbt3u , rtB . kt51zrjm3b , rtB . c2xffyhecd , rtP . T2_delay_evaan1curx
, & rtB . br1i12egyj , & rtDW . br1i12egyj , & rtP . br1i12egyj , & rtPrevZCX
. br1i12egyj ) ; rtB . ife0s2l1mx = ( rtB . f5lqeh5sbp . j1vu52vt5l || rtB .
br1i12egyj . mjqylog0rb ) ; rtB . a0dxdvgbux = ! rtB . ife0s2l1mx ; rtB .
g5smsjevum = ( rtB . c4p5csisad && rtB . a0dxdvgbux ) ; rtB . o01mgup3zv = (
rtB . g53tgrjxee > rtP . u9_Value_i1pyvzpv3x ) ; rtB . krwyfez4yk = ! ( rtB .
laxu4i4bvg != 0.0 ) ; rtB . pjuqkltmtb = ( rtB . krwyfez4yk && ( rtP .
u_Value_iuoi1xcwxh != 0.0 ) ) ; rtB . dppygg53fj = ( rtB . e2ltxh3gsr || rtB
. pjuqkltmtb ) ; rtB . mven3rjae3 = ! ( rtB . gfj43a5l4q != 0.0 ) ; rtB .
n4ttfrgcra = ( rtB . mven3rjae3 && ( rtP . u_Value_h4eaya5qm4 != 0.0 ) ) ;
rtB . jqa0vzpw4u = ( rtB . dppygg53fj || rtB . n4ttfrgcra ) ; rtB .
n3pmgdcgsz = ( rtB . o01mgup3zv && rtB . jqa0vzpw4u ) ; rtB . judqj1pdp4 =
rtB . n3pmgdcgsz ; rtB . gicy25xj2q = ssGetT ( rtS ) ; on20slldft ( rtS , rtB
. bfaj1ezwe0 , rtB . judqj1pdp4 , rtB . gicy25xj2q , rtP .
T1_delay_cmw1yrtyva , & rtB . jvoggbn2yd , & rtDW . jvoggbn2yd , & rtP .
jvoggbn2yd , & rtPrevZCX . jvoggbn2yd ) ; bhc3qgnuic ( rtS , rtB . mspaunmj5m
, rtB . judqj1pdp4 , rtB . gicy25xj2q , rtP . T1_delay_cmw1yrtyva , & rtB .
boy5y1bmz3 , & rtDW . boy5y1bmz3 , & rtP . boy5y1bmz3 , & rtPrevZCX .
boy5y1bmz3 ) ; rtB . agvngknddt = ( rtB . jvoggbn2yd . j1vu52vt5l || rtB .
boy5y1bmz3 . mjqylog0rb ) ; rtB . iseplh0lxc = rtB . acu2uimhf4 * rtB .
fgh2gbbdh5 ; if ( rtP . u3_Value_lnebph21xb > rtP . u_Threshold_hrln0zs2sl )
{ rtB . kscwxlhord = rtB . lmpfshyn1r * rtB . fgh2gbbdh5 ; rtB . cdmhghv3n4 =
rtB . iseplh0lxc - rtB . kscwxlhord ; rtB . aejognodev = rtB . cdmhghv3n4 ; }
else { rtB . aejognodev = rtB . iseplh0lxc ; } rtB . gds2pcbidh = rtB .
aejognodev * rtB . ht2utaggby ; rtB . kdnrda1af1 = rtB . hxgvln4wyf * rtB .
fgh2gbbdh5 ; if ( rtP . u3_Value_lnebph21xb > rtP . u_Threshold_hpbhtfgbn1 )
{ rtB . ibf3qs3vyj = rtB . mpx3oy4ctu * rtB . fgh2gbbdh5 ; rtB . n2nutquo4p =
rtB . kdnrda1af1 - rtB . ibf3qs3vyj ; rtB . gsbfkupvhg = rtB . n2nutquo4p ; }
else { rtB . gsbfkupvhg = rtB . kdnrda1af1 ; } rtB . k4odkmcfrn = rtB .
gsbfkupvhg * rtB . jjyoruakud ; rtB . i2jv0fogyx = rtB . gds2pcbidh - rtB .
k4odkmcfrn ; rtB . chibjwybpm = rtB . do4x3mpyxa * rtB . do4x3mpyxa ; rtB .
jloijwrdv2 = rtB . coehnzyatq * rtB . coehnzyatq ; rtB . bknkpip1ua = rtB .
chibjwybpm + rtB . jloijwrdv2 ; rtB . i133ypo3i2 = muDoubleScalarSqrt ( rtB .
bknkpip1ua ) ; rtB . b3qnikbojz = ( rtB . i133ypo3i2 > rtP .
u9_Value_pwo1xiraul ) ; rtB . ltbf4lyeyj = ( rtB . b3qnikbojz && rtB .
kl4albs201 ) ; rtB . edayvamwde = ( rtB . ityznxzj4e && rtB . ff4zi1sk4i ) ;
if ( rtB . dlq2giet1z ) { rtB . nuqqjqizth = rtP . u7_Value_fs2w21cd1e ; }
else { if ( rtB . cxt03zh1fr ) { rtB . cjl5ryp5tb = ( rtB . ltbf4lyeyj || rtB
. edayvamwde ) ; rtB . jihi1kh0r1 = ( rtB . cjl5ryp5tb && rtB . cxt03zh1fr )
; rtB . asx1zcvg4u = rtB . jihi1kh0r1 ; } else { if ( rtB . kl4albs201 ) {
rtB . jmk5jhfami = rtB . ltbf4lyeyj ; } else { rtB . jmk5jhfami = rtB .
edayvamwde ; } rtB . asx1zcvg4u = rtB . jmk5jhfami ; } rtB . nuqqjqizth = rtB
. asx1zcvg4u ; } if ( rtB . nuqqjqizth > rtP . u_Threshold_pmz3lguluj ) { rtB
. a204zpjc23 = rtB . i2jv0fogyx ; } else { rtB . a204zpjc23 = rtP .
u54_Value_kfixmbkz0t ; } if ( rtP . u_Value_iuoi1xcwxh > rtP .
u_Threshold_n50pi3rmc4 ) { rtB . ai24ux3yja = rtB . a204zpjc23 ; } else { rtB
. ai24ux3yja = rtP . u54_Value_kfixmbkz0t ; } rtB . b4t0eyqt1u = rtB .
gfkd05btay + rtB . ai24ux3yja ; rtB . grktbi1ocz = rtB . asa103d02z * rtB .
d0y4nbtz00 ; if ( rtP . u_Value_eqa0ytrizk > rtP . u_Threshold_mlet1c4khr ) {
rtB . ox2k13egs2 = rtB . gamkho1q2f * rtB . d0y4nbtz00 ; rtB . fxufq2fzxa =
rtB . grktbi1ocz - rtB . ox2k13egs2 ; rtB . e0frnt0lfx = rtB . fxufq2fzxa ; }
else { rtB . e0frnt0lfx = rtB . grktbi1ocz ; } rtB . hxjkw5odaq = rtB .
e0frnt0lfx * rtB . onbinf3mxx ; rtB . gkuno3kpu3 = rtB . ddix2mhi1g * rtB .
d0y4nbtz00 ; if ( rtP . u_Value_eqa0ytrizk > rtP . u_Threshold_ewk1skgdu5 ) {
rtB . cd1hfn5rmb = rtB . aafebvzsoj * rtB . d0y4nbtz00 ; rtB . jw05f4cqif =
rtB . gkuno3kpu3 - rtB . cd1hfn5rmb ; rtB . jsdfe2jbnh = rtB . jw05f4cqif ; }
else { rtB . jsdfe2jbnh = rtB . gkuno3kpu3 ; } rtB . ldp1vl5x2v = rtB .
jsdfe2jbnh * rtB . mgcq2y0ojp ; rtB . czvwxyormz = rtB . hxjkw5odaq - rtB .
ldp1vl5x2v ; rtB . kiour02gvb = rtB . i5dpx5t34o * rtB . i5dpx5t34o ; rtB .
pxawplgvyy = rtB . jlt5ytbte0 * rtB . jlt5ytbte0 ; rtB . c2rfbukzkg = rtB .
kiour02gvb + rtB . pxawplgvyy ; rtB . cc2ltagao0 = muDoubleScalarSqrt ( rtB .
c2rfbukzkg ) ; rtB . bxsuebpn5p = ( rtB . cc2ltagao0 > rtP .
u9_Value_gwa1ux021y ) ; rtB . aff2utpmc0 = ( rtB . bxsuebpn5p && rtB .
cdxhxlj30g ) ; rtB . djctpxyjgr = ( rtB . lz3oaerh4a && rtB . gz04icgcdm ) ;
if ( rtB . do24p4hxxw ) { rtB . gxncten00d = rtP . u7_Value_knfw4nz5wz ; }
else { if ( rtB . n041v55ew1 ) { rtB . kzjbjmq5kn = ( rtB . aff2utpmc0 || rtB
. djctpxyjgr ) ; rtB . csteyticzb = ( rtB . kzjbjmq5kn && rtB . n041v55ew1 )
; rtB . eexefyj4eq = rtB . csteyticzb ; } else { if ( rtB . cdxhxlj30g ) {
rtB . d0cyauc2vc = rtB . aff2utpmc0 ; } else { rtB . d0cyauc2vc = rtB .
djctpxyjgr ; } rtB . eexefyj4eq = rtB . d0cyauc2vc ; } rtB . gxncten00d = rtB
. eexefyj4eq ; } if ( rtB . gxncten00d > rtP . u_Threshold_ezkayyx5pw ) { rtB
. bkao3slhko = rtB . czvwxyormz ; } else { rtB . bkao3slhko = rtP .
u54_Value_nxoc5qbvsr ; } if ( rtP . u_Value_h4eaya5qm4 > rtP .
u_Threshold_nlqz0imqdb ) { rtB . dwim2bfz1h = rtB . bkao3slhko ; } else { rtB
. dwim2bfz1h = rtP . u54_Value_nxoc5qbvsr ; } rtB . jjvctyatjv = rtB .
b4t0eyqt1u + rtB . dwim2bfz1h ; rtB . m1iufi42mx = rtB . jjvctyatjv * rtB .
jjvctyatjv ; rtB . bxukbv5h4t = rtB . gsbfkupvhg * rtB . ht2utaggby ; rtB .
cwzrfkx24r = rtB . aejognodev * rtB . jjyoruakud ; rtB . k10rrojg2s = rtB .
bxukbv5h4t + rtB . cwzrfkx24r ; if ( rtB . nuqqjqizth > rtP .
u_Threshold_ngmyjyvnkm ) { rtB . h0bu2qy5m3 = rtB . k10rrojg2s ; } else { rtB
. h0bu2qy5m3 = rtP . u54_Value_kfixmbkz0t ; } if ( rtP . u_Value_iuoi1xcwxh >
rtP . u_Threshold_caqxkoolfg ) { rtB . dlwa0axeii = rtB . h0bu2qy5m3 ; } else
{ rtB . dlwa0axeii = rtP . u54_Value_kfixmbkz0t ; } rtB . jzfvcefrym = rtB .
pmjinc03yw + rtB . dlwa0axeii ; rtB . il253gyulw = rtB . jsdfe2jbnh * rtB .
onbinf3mxx ; rtB . lxckrv3ou4 = rtB . e0frnt0lfx * rtB . mgcq2y0ojp ; rtB .
j5nwsmpmbt = rtB . il253gyulw + rtB . lxckrv3ou4 ; if ( rtB . gxncten00d >
rtP . u_Threshold_e0vdeuth3a ) { rtB . gglyj20iwt = rtB . j5nwsmpmbt ; } else
{ rtB . gglyj20iwt = rtP . u54_Value_nxoc5qbvsr ; } if ( rtP .
u_Value_h4eaya5qm4 > rtP . u_Threshold_df5acg0erp ) { rtB . brnrpohokt = rtB
. gglyj20iwt ; } else { rtB . brnrpohokt = rtP . u54_Value_nxoc5qbvsr ; } rtB
. lnkvmulze1 = rtB . jzfvcefrym + rtB . brnrpohokt ; rtB . a2fikuxvhx = rtB .
lnkvmulze1 * rtB . lnkvmulze1 ; rtB . f2ycxr2xvm = rtB . m1iufi42mx + rtB .
a2fikuxvhx ; rtB . n40105jbho = muDoubleScalarSqrt ( rtB . f2ycxr2xvm ) ; rtB
. jsj04l2tiy = ( rtB . n40105jbho > rtP . u9_Value_i1pyvzpv3x ) ; rtB .
en4lgnhnzz = ! ( rtB . nuqqjqizth != 0.0 ) ; rtB . am5ndynrmd = ( rtB .
en4lgnhnzz && ( rtP . u_Value_iuoi1xcwxh != 0.0 ) ) ; rtB . gk0mkcrp0a = (
rtB . hsoy2odhbx || rtB . am5ndynrmd ) ; rtB . h2ywvm3zss = ! ( rtB .
gxncten00d != 0.0 ) ; rtB . o40bui2tqj = ( rtB . h2ywvm3zss && ( rtP .
u_Value_h4eaya5qm4 != 0.0 ) ) ; rtB . l2j4h2pvfl = ( rtB . gk0mkcrp0a || rtB
. o40bui2tqj ) ; rtB . gfoiw1xqnj = ( rtB . jsj04l2tiy && rtB . l2j4h2pvfl )
; rtB . hwbofm52o4 = rtB . gfoiw1xqnj ; rtB . kd0ykzrspw = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . fl54yqipkw , rtB . hwbofm52o4 , rtB . kd0ykzrspw ,
rtP . T2_delay_amlsw0xf1y , & rtB . ithg0f1jiq , & rtDW . ithg0f1jiq , & rtP
. ithg0f1jiq , & rtPrevZCX . ithg0f1jiq ) ; bhc3qgnuic ( rtS , rtB .
iqgtpt55z5 , rtB . hwbofm52o4 , rtB . kd0ykzrspw , rtP . T2_delay_amlsw0xf1y
, & rtB . ojnin3nhvw , & rtDW . ojnin3nhvw , & rtP . ojnin3nhvw , & rtPrevZCX
. ojnin3nhvw ) ; rtB . hpoiaacetx = ( rtB . ithg0f1jiq . j1vu52vt5l || rtB .
ojnin3nhvw . mjqylog0rb ) ; rtB . jb2jiwlptr = rtB . fd2yp3dpzd * rtB .
d3jk1drg5n ; if ( rtP . u3_Value_lnebph21xb > rtP . u_Threshold_cprdeztwb3 )
{ rtB . f5k50uljwo = rtB . lmpfshyn1r * rtB . d3jk1drg5n ; rtB . dfghiykuuk =
rtB . jb2jiwlptr - rtB . f5k50uljwo ; rtB . enexx3lei5 = rtB . dfghiykuuk ; }
else { rtB . enexx3lei5 = rtB . jb2jiwlptr ; } rtB . heqlgxr5oo = rtB .
enexx3lei5 * rtB . msn321mj2p ; rtB . o3lebyj1tn = rtB . bolkwwrtqf * rtB .
d3jk1drg5n ; if ( rtP . u3_Value_lnebph21xb > rtP . u_Threshold_hq4x3z4n2v )
{ rtB . mnson4n1ql = rtB . mpx3oy4ctu * rtB . d3jk1drg5n ; rtB . jp1k4pgztu =
rtB . o3lebyj1tn - rtB . mnson4n1ql ; rtB . kvv0bectw4 = rtB . jp1k4pgztu ; }
else { rtB . kvv0bectw4 = rtB . o3lebyj1tn ; } rtB . kjny5tj51w = rtB .
kvv0bectw4 * rtB . fgfcqxlfi0 ; rtB . bkmtj5x54m = rtB . heqlgxr5oo - rtB .
kjny5tj51w ; rtB . g2dkffkmlb = rtB . egfgjyyopt * rtB . egfgjyyopt ; rtB .
a2sxcye14v = rtB . oaxc3hrb45 * rtB . oaxc3hrb45 ; rtB . geu04jka25 = rtB .
g2dkffkmlb + rtB . a2sxcye14v ; rtB . izt1jaqlud = muDoubleScalarSqrt ( rtB .
geu04jka25 ) ; rtB . m02eogmy3l = ( rtB . izt1jaqlud > rtP .
u9_Value_pwo1xiraul ) ; rtB . kg0ylfr5bw = ( rtB . m02eogmy3l && rtB .
kl4albs201 ) ; rtB . e0h2bmnout = ( rtB . pjte25iacd && rtB . ff4zi1sk4i ) ;
if ( rtB . dlq2giet1z ) { rtB . mfzwdgo525 = rtP . u7_Value_fs2w21cd1e ; }
else { if ( rtB . cxt03zh1fr ) { rtB . ckij3kbe5l = ( rtB . kg0ylfr5bw || rtB
. e0h2bmnout ) ; rtB . aas02nzswa = ( rtB . ckij3kbe5l && rtB . cxt03zh1fr )
; rtB . jd4okepwkh = rtB . aas02nzswa ; } else { if ( rtB . kl4albs201 ) {
rtB . nbmlqwdbzw = rtB . kg0ylfr5bw ; } else { rtB . nbmlqwdbzw = rtB .
e0h2bmnout ; } rtB . jd4okepwkh = rtB . nbmlqwdbzw ; } rtB . mfzwdgo525 = rtB
. jd4okepwkh ; } if ( rtP . u_Value_iuoi1xcwxh > rtP . u_Threshold_ncqf2kep1o
) { if ( rtB . mfzwdgo525 > rtP . u_Threshold_l4sd5utz3i ) { rtB . bywjhxnhpi
= rtB . bkmtj5x54m ; } else { rtB . bywjhxnhpi = rtP . u54_Value_fumvex2ah4 ;
} rtB . dhmmov1jxy = rtB . bywjhxnhpi ; } else { rtB . dhmmov1jxy = rtP .
u54_Value_fumvex2ah4 ; } rtB . i3wtqryax1 = rtB . obhjbbjvoz + rtB .
dhmmov1jxy ; rtB . hb5fatiuly = rtB . fizn4ck4n1 * rtB . cj5sc500zh ; if (
rtP . u_Value_eqa0ytrizk > rtP . u_Threshold_huwuu554a0 ) { rtB . dcf3nkcnyj
= rtB . gamkho1q2f * rtB . cj5sc500zh ; rtB . mmhzv0whsc = rtB . hb5fatiuly -
rtB . dcf3nkcnyj ; rtB . enmnlpggqv = rtB . mmhzv0whsc ; } else { rtB .
enmnlpggqv = rtB . hb5fatiuly ; } rtB . c2hwkzkhi3 = rtB . enmnlpggqv * rtB .
cfh3zls04q ; rtB . g1gmzftdfn = rtB . ndiffna1hx * rtB . cj5sc500zh ; if (
rtP . u_Value_eqa0ytrizk > rtP . u_Threshold_ftv35w43pv ) { rtB . mmp0tzvzc0
= rtB . aafebvzsoj * rtB . cj5sc500zh ; rtB . bop3tpfarl = rtB . g1gmzftdfn -
rtB . mmp0tzvzc0 ; rtB . hyozodxwki = rtB . bop3tpfarl ; } else { rtB .
hyozodxwki = rtB . g1gmzftdfn ; } rtB . cwyc23l1nw = rtB . hyozodxwki * rtB .
eohu5a2mev ; rtB . mwhlgznwmb = rtB . c2hwkzkhi3 - rtB . cwyc23l1nw ; rtB .
mi14clrgl3 = rtB . lnv3d3rvor * rtB . lnv3d3rvor ; rtB . e43j0wctng = rtB .
chvsplbxtl * rtB . chvsplbxtl ; rtB . htxlwutbzu = rtB . mi14clrgl3 + rtB .
e43j0wctng ; rtB . gqqeuw4ork = muDoubleScalarSqrt ( rtB . htxlwutbzu ) ; rtB
. e4qsgluzwo = ( rtB . gqqeuw4ork > rtP . u9_Value_gwa1ux021y ) ; rtB .
eadzx3422g = ( rtB . e4qsgluzwo && rtB . cdxhxlj30g ) ; rtB . dwj1l4sg14 = (
rtB . kku2fb5nwp && rtB . gz04icgcdm ) ; if ( rtB . do24p4hxxw ) { rtB .
ezfrshqts0 = rtP . u7_Value_knfw4nz5wz ; } else { if ( rtB . n041v55ew1 ) {
rtB . kuub2k0jm3 = ( rtB . eadzx3422g || rtB . dwj1l4sg14 ) ; rtB .
gutn03yld1 = ( rtB . kuub2k0jm3 && rtB . n041v55ew1 ) ; rtB . fs5ewfsyc1 =
rtB . gutn03yld1 ; } else { if ( rtB . cdxhxlj30g ) { rtB . gltzkr5huf = rtB
. eadzx3422g ; } else { rtB . gltzkr5huf = rtB . dwj1l4sg14 ; } rtB .
fs5ewfsyc1 = rtB . gltzkr5huf ; } rtB . ezfrshqts0 = rtB . fs5ewfsyc1 ; } if
( rtP . u_Value_h4eaya5qm4 > rtP . u_Threshold_be5jbaykcy ) { if ( rtB .
ezfrshqts0 > rtP . u_Threshold_i3farbv2ia ) { rtB . b2zpt4bal2 = rtB .
mwhlgznwmb ; } else { rtB . b2zpt4bal2 = rtP . u54_Value_dgmayye3ox ; } rtB .
pjcdsoe04i = rtB . b2zpt4bal2 ; } else { rtB . pjcdsoe04i = rtP .
u54_Value_dgmayye3ox ; } rtB . ilqpizv1kn = rtB . i3wtqryax1 + rtB .
pjcdsoe04i ; rtB . oyubyjfaqb = rtB . ilqpizv1kn * rtB . ilqpizv1kn ; rtB .
cj1texgs0w = rtB . kvv0bectw4 * rtB . msn321mj2p ; rtB . grl0xa2ruc = rtB .
enexx3lei5 * rtB . fgfcqxlfi0 ; rtB . jtgadx53jo = rtB . cj1texgs0w + rtB .
grl0xa2ruc ; if ( rtP . u_Value_iuoi1xcwxh > rtP . u_Threshold_czigffzq4r ) {
if ( rtB . mfzwdgo525 > rtP . u_Threshold_d5jorbf5go ) { rtB . khhfmlgte4 =
rtB . jtgadx53jo ; } else { rtB . khhfmlgte4 = rtP . u54_Value_fumvex2ah4 ; }
rtB . ik2cqrdw1z = rtB . khhfmlgte4 ; } else { rtB . ik2cqrdw1z = rtP .
u54_Value_fumvex2ah4 ; } rtB . pvrfkbzmcm = rtB . n4eadhgbwx + rtB .
ik2cqrdw1z ; rtB . juung4fkvc = rtB . hyozodxwki * rtB . cfh3zls04q ; rtB .
ittnhorvya = rtB . enmnlpggqv * rtB . eohu5a2mev ; rtB . d35jbeyxvo = rtB .
juung4fkvc + rtB . ittnhorvya ; if ( rtP . u_Value_h4eaya5qm4 > rtP .
u_Threshold_j1jvw0t1vy ) { if ( rtB . ezfrshqts0 > rtP .
u_Threshold_jgjzc4lmki ) { rtB . odr33livnu = rtB . d35jbeyxvo ; } else { rtB
. odr33livnu = rtP . u54_Value_dgmayye3ox ; } rtB . lbky4lats5 = rtB .
odr33livnu ; } else { rtB . lbky4lats5 = rtP . u54_Value_dgmayye3ox ; } rtB .
l4cyxxkqo1 = rtB . pvrfkbzmcm + rtB . lbky4lats5 ; rtB . dzyynnbyl3 = rtB .
l4cyxxkqo1 * rtB . l4cyxxkqo1 ; rtB . idzdhuwnzj = rtB . oyubyjfaqb + rtB .
dzyynnbyl3 ; rtB . lwsdutl0jd = muDoubleScalarSqrt ( rtB . idzdhuwnzj ) ; rtB
. fbfimqszlb = ( rtB . lwsdutl0jd > rtP . u9_Value_i1pyvzpv3x ) ; rtB .
bnvalgrrsb = ! ( rtB . mfzwdgo525 != 0.0 ) ; rtB . jbvudmez1s = ( rtB .
bnvalgrrsb && ( rtP . u_Value_iuoi1xcwxh != 0.0 ) ) ; rtB . mlppogk5qz = (
rtB . fg2ble3kcb || rtB . jbvudmez1s ) ; rtB . e1xhq0xc2e = ! ( rtB .
ezfrshqts0 != 0.0 ) ; rtB . nlggomiw2m = ( rtB . e1xhq0xc2e && ( rtP .
u_Value_h4eaya5qm4 != 0.0 ) ) ; rtB . ckpnu2i44a = ( rtB . mlppogk5qz || rtB
. nlggomiw2m ) ; rtB . dily1v10u0 = ( rtB . fbfimqszlb && rtB . ckpnu2i44a )
; rtB . h04qfdp5cd = rtB . dily1v10u0 ; rtB . dkmqavtabh = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . f1xwmtbavl , rtB . h04qfdp5cd , rtB . dkmqavtabh ,
rtP . T3_delay_nkrtvnztd1 , & rtB . p2xsu5tknt , & rtDW . p2xsu5tknt , & rtP
. p2xsu5tknt , & rtPrevZCX . p2xsu5tknt ) ; bhc3qgnuic ( rtS , rtB .
mhw3zqrf0v , rtB . h04qfdp5cd , rtB . dkmqavtabh , rtP . T3_delay_nkrtvnztd1
, & rtB . bhbl51zywo , & rtDW . bhbl51zywo , & rtP . bhbl51zywo , & rtPrevZCX
. bhbl51zywo ) ; rtB . euuxlpjxoe = ( rtB . p2xsu5tknt . j1vu52vt5l || rtB .
bhbl51zywo . mjqylog0rb ) ; rtB . d4samfx2k5 = ( rtB . agvngknddt || rtB .
hpoiaacetx || rtB . euuxlpjxoe ) ; rtB . myd4osbk01 = ( rtB . aizy54nz41 ||
rtB . d4samfx2k5 ) ; rtB . o1flzbb3fd = ( rtB . myd4osbk01 && rtB .
p0me2ise2s ) ; rtB . jyacd1dsia = ( rtB . ospu2oo14g || rtB . o1flzbb3fd ) ;
rtB . czhgrl1x00 = ! rtB . jyacd1dsia ; rtB . azepmxoauk = ( rtB . g5smsjevum
&& rtB . czhgrl1x00 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
lhtpmbirai = rtDW . f23jtcpi0e ; rtB . hindkyduro = ! rtB . lhtpmbirai ; rtB
. pg0wfhlxky = rtDW . ih0vhsirfu ; rtB . dnssa0ulg5 = ! rtB . pg0wfhlxky ; }
rtB . h0nckan2xr = ( rtB . azepmxoauk && rtB . hindkyduro ) ; rtB .
dg4gxeeqpw = ssGetT ( rtS ) ; rtB . currldmahk = rtB . azepmxoauk ; rtB .
oqcfliecli = rtB . currldmahk ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
mczdt0mbpd = rtDW . buycl0ri4c ; } itcd43cpff ( rtS , rtB . jmp2nve0rj [ 0 ]
, rtB . oqcfliecli , rtB . mczdt0mbpd , & rtB . j1rcgjyzg3 , & rtDW .
j1rcgjyzg3 ) ; cgvj5xjnjk ( rtS , rtB . jmp2nve0rj [ 1 ] , rtB . oqcfliecli ,
rtB . mczdt0mbpd , & rtB . fh4f1ldkj5 , & rtDW . fh4f1ldkj5 ) ; rtB .
bfqbvqy5gl = ( rtB . j1rcgjyzg3 . guqrn12esp || rtB . fh4f1ldkj5 . l5pxivnpxv
) ; rtB . iyfvalnkig = ( rtB . dnssa0ulg5 && rtB . bfqbvqy5gl ) ; rtB .
n0nuhswa3y = rtB . iyfvalnkig ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
nawznxlt4l = rtDW . klktcnq5ax ; } if ( rtB . n0nuhswa3y ) { rtB . pxqprudxor
= rtB . dg4gxeeqpw ; } else { rtB . pxqprudxor = rtB . nawznxlt4l ; } maxV =
rtP . u_PulseDuration - 2.2204460492503131E-16 ; rtB . jq3ac0dm4x = rtB .
pxqprudxor + maxV ; rtB . eyr5byduqe = ( rtB . jq3ac0dm4x > rtB . dg4gxeeqpw
) ; rtB . jdvugwqi3f = ( rtB . h0nckan2xr || rtB . eyr5byduqe ) ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . cw03hxjlsy = rtDW . dgygcznzfp ; }
rtB . dzlop1x3ep = rtB . ai24ux3yja * rtB . ai24ux3yja ; rtB . adtczdzfth =
rtB . dlwa0axeii * rtB . dlwa0axeii ; rtB . blu0302agd = rtB . dzlop1x3ep +
rtB . adtczdzfth ; rtB . ldkr3vj0qt = muDoubleScalarSqrt ( rtB . blu0302agd )
; rtB . mknkoiycce = ( rtB . ent5takyzj > rtB . ldkr3vj0qt ) ; if ( rtB .
mknkoiycce ) { rtB . o5guvo5mte = rtB . d5orl0ohoz ; rtB . nkhlyok1un = rtB .
a1cyb5lbfo ; } else { rtB . o5guvo5mte = rtB . ai24ux3yja ; rtB . nkhlyok1un
= rtB . dlwa0axeii ; } rtB . mbfvulmhhs = rtB . o5guvo5mte * rtB . o5guvo5mte
; rtB . ho2pzl1twk = rtB . nkhlyok1un * rtB . nkhlyok1un ; rtB . d3viz0u3zo =
rtB . mbfvulmhhs + rtB . ho2pzl1twk ; rtB . m0iju3sgzy = muDoubleScalarSqrt (
rtB . d3viz0u3zo ) ; rtB . ag5i4yxnab = rtB . dwim2bfz1h * rtB . dwim2bfz1h ;
rtB . alworixjhf = rtB . brnrpohokt * rtB . brnrpohokt ; rtB . ighcscmvan =
rtB . ag5i4yxnab + rtB . alworixjhf ; rtB . n5hiqfss1z = muDoubleScalarSqrt (
rtB . ighcscmvan ) ; rtB . klrz2aqxa5 = ( rtB . m0iju3sgzy > rtB . n5hiqfss1z
) ; if ( rtB . klrz2aqxa5 ) { rtB . oxdenoyw40 = rtB . o5guvo5mte ; rtB .
luxihu0pi4 = rtB . nkhlyok1un ; } else { rtB . oxdenoyw40 = rtB . dwim2bfz1h
; rtB . luxihu0pi4 = rtB . brnrpohokt ; } rtB . ji1s5zztnp = rtB . oxdenoyw40
- rtB . jjvctyatjv ; rtB . gaskagbfw2 = rtB . luxihu0pi4 - rtB . lnkvmulze1 ;
rtB . oprilhd30f = rtB . oxdenoyw40 * rtB . oxdenoyw40 ; rtB . ou0rvvc0kf =
rtB . luxihu0pi4 * rtB . luxihu0pi4 ; rtB . c30rqfl4d4 = rtB . oprilhd30f +
rtB . ou0rvvc0kf ; rtB . gauaru5drm = muDoubleScalarSqrt ( rtB . c30rqfl4d4 )
; rtB . kikezmy2ly = rtB . oxdenoyw40 / rtB . gauaru5drm ; rtB . c1ru0q4gyj =
rtB . ji1s5zztnp * rtB . ji1s5zztnp ; rtB . mf4dv5ctfp = rtB . gaskagbfw2 *
rtB . gaskagbfw2 ; rtB . asidpknc0r = rtB . c1ru0q4gyj + rtB . mf4dv5ctfp ;
rtB . fexc0lxdfe = muDoubleScalarSqrt ( rtB . asidpknc0r ) ; rtB . cb4pf5bt3y
= rtB . ji1s5zztnp / rtB . fexc0lxdfe ; rtB . d3x04y4gsn = rtB . kikezmy2ly -
rtB . cb4pf5bt3y ; rtB . no1uoclimr = rtB . d3x04y4gsn * rtB . d3x04y4gsn ;
rtB . d4jehbbuw0 = rtB . luxihu0pi4 / rtB . gauaru5drm ; rtB . a30pnutfmf =
rtB . gaskagbfw2 / rtB . fexc0lxdfe ; rtB . ld0gcu055a = rtB . d4jehbbuw0 -
rtB . a30pnutfmf ; rtB . ot043gsh1n = rtB . ld0gcu055a * rtB . ld0gcu055a ;
rtB . dqpedikwyw = rtB . no1uoclimr + rtB . ot043gsh1n ; rtB . n1y0kudvlf =
rtB . dqpedikwyw * rtP . Constant3_Value_pdjdj1f4fk ; rtB . obgsmqrpdn = rtP
. Constant2_Value_b55zd3hwty - rtB . n1y0kudvlf ; u0 = rtB . obgsmqrpdn ; if
( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } rtB .
hddokadddi = muDoubleScalarAcos ( u0 ) ; rtB . gfp223qmaj = rtB . d4jehbbuw0
- rtB . cb4pf5bt3y ; rtB . n4wenk0zxb = rtB . gfp223qmaj * rtB . gfp223qmaj ;
rtB . f0benr4xyl = rtB . kikezmy2ly + rtB . a30pnutfmf ; rtB . easagtjoz3 =
rtB . f0benr4xyl * rtB . f0benr4xyl ; rtB . k2npgrsiqy = rtB . n4wenk0zxb +
rtB . easagtjoz3 ; rtB . pfhqlv1mx1 = ( rtB . k2npgrsiqy > rtP .
Constant_Value_ju3gcwkq0d ) ; if ( rtB . pfhqlv1mx1 ) { rtB . aqvx0bsmiu =
rtB . hddokadddi * rtP . Constant1_Value_pjytajcuye ; rtB . j3wnlhduer = rtB
. aqvx0bsmiu ; } else { rtB . j3wnlhduer = rtB . hddokadddi ; } rtB .
n35uomgorg = muDoubleScalarCos ( rtB . j3wnlhduer ) ; rtB . b5gux3th4c = (
rtB . n35uomgorg >= rtP . u_Value_iyiza3iutg ) ; if ( rtB . b5gux3th4c ) {
rtB . fa0e03e0xn = rtB . gaskagbfw2 * rtB . gaskagbfw2 ; rtB . ekd5jnkmy1 =
rtB . ji1s5zztnp * rtB . ji1s5zztnp ; rtB . frkzhginjk = rtB . ekd5jnkmy1 +
rtB . fa0e03e0xn ; rtB . el5nrspqcg = muDoubleScalarSqrt ( rtB . frkzhginjk )
; rtB . lptv5nqhvx = rtB . luxihu0pi4 * rtB . luxihu0pi4 ; rtB . jgjnltbeih =
rtB . oxdenoyw40 * rtB . oxdenoyw40 ; rtB . d4lenrbepm = rtB . jgjnltbeih +
rtB . lptv5nqhvx ; rtB . mnsjud2mim = muDoubleScalarSqrt ( rtB . d4lenrbepm )
; rtB . c23pfxhsnq = rtB . mnsjud2mim * rtB . el5nrspqcg * rtB . n35uomgorg ;
rtB . jc2b1hw3cy = muDoubleScalarSqrt ( rtB . c23pfxhsnq ) ; rtB . jf51clyfme
= rtB . jc2b1hw3cy ; } else { rtB . jf51clyfme = rtP . u7_Value_ds2brihoaq ;
} if ( rtB . m3w2j4hj3t ) { rtB . lp5spnhi3l = rtP . u1_Value_i5s2kfsaaw ; }
else { rtB . hvbtx1wxm2 = ( rtB . jf51clyfme > rtB . kmzxhlxwcq ) ; if ( rtB
. hvbtx1wxm2 ) { if ( rtB . pccc0d0zpp ) { rtB . imd4rdg3dt = rtB .
kmhdsfclzv * rtB . jf51clyfme ; rtB . g05bzy3zt0 = rtB . imd4rdg3dt + rtB .
d0rxrxmfaa ; rtB . ouf0fushgu = rtB . g05bzy3zt0 ; } else { rtB . ouf0fushgu
= rtB . nkbpr3wboi ; } rtB . knh41llub2 = rtB . ouf0fushgu ; } else { rtB .
k5zvbqnejw = ( rtB . jf51clyfme <= rtB . kmzxhlxwcq ) ; rtB . c1ueabz2vd = (
rtB . jf51clyfme > rtP . u_Value_n0ldpwgypr ) ; rtB . fia3vglxx1 = ( rtB .
c1ueabz2vd && rtB . k5zvbqnejw ) ; if ( rtB . fia3vglxx1 ) { rtB . f3upytsu5c
= rtB . jf51clyfme * rtB . kmhdsfclzv ; rtB . ezuror5kt4 = rtB . f3upytsu5c +
rtB . d0rxrxmfaa ; rtB . bxn1dlphdk = rtB . ezuror5kt4 ; } else { rtB .
bxn1dlphdk = rtP . u1_Value_i5s2kfsaaw ; } rtB . knh41llub2 = rtB .
bxn1dlphdk ; } rtB . lp5spnhi3l = rtB . knh41llub2 ; } if ( rtB . k55sygb5my
) { rtB . jn1xgckye3 = rtB . lp5spnhi3l ; } else { rtB . emnkltpqxv = ( rtB .
jf51clyfme > rtB . lfwnwxtobq ) ; if ( rtB . emnkltpqxv ) { if ( rtB .
ezlpmrn4ak ) { rtB . g3azgb4fdk = rtB . jmhrjlb32q * rtB . jf51clyfme ; rtB .
calppo0bys = rtB . g3azgb4fdk + rtB . f5mbrc1chn ; rtB . l2r45bbm51 = rtB .
calppo0bys ; } else { rtB . l2r45bbm51 = rtB . nkbpr3wboi ; } rtB .
jycaa3da4z = rtB . l2r45bbm51 ; } else { rtB . kz1c2zqkyg = ( rtB .
jf51clyfme <= rtB . lfwnwxtobq ) ; rtB . mslgcfsaf0 = ( rtB . jf51clyfme >
rtP . u2_Value_c1sg11dlld ) ; rtB . cu3ncrdkpz = ( rtB . mslgcfsaf0 && rtB .
kz1c2zqkyg ) ; if ( rtB . cu3ncrdkpz ) { rtB . hxoq1fddiy = rtB . jf51clyfme
* rtB . jmhrjlb32q ; rtB . ie2uc2f1bs = rtB . hxoq1fddiy + rtB . f5mbrc1chn ;
rtB . moyaq1dch3 = rtB . ie2uc2f1bs ; } else { rtB . moyaq1dch3 = rtB .
lp5spnhi3l ; } rtB . jycaa3da4z = rtB . moyaq1dch3 ; } rtB . jn1xgckye3 = rtB
. jycaa3da4z ; } if ( rtB . l4hiifoiav ) { rtB . hsdbtenegz = rtB .
jn1xgckye3 ; } else { rtB . pq2k32o34y = ( rtB . jf51clyfme > rtB .
j3dosgzpk4 ) ; if ( rtB . pq2k32o34y ) { if ( rtB . cv2uplhmcr ) { rtB .
dcae3req0w = rtB . kqagnyzho3 * rtB . jf51clyfme ; rtB . m5ju1qrxn3 = rtB .
dcae3req0w + rtB . fduxvo2nww ; rtB . loeaeh1yym = rtB . m5ju1qrxn3 ; } else
{ rtB . loeaeh1yym = rtB . nkbpr3wboi ; } rtB . jfvsanwxv2 = rtB . loeaeh1yym
; } else { rtB . csyedil0zp = ( rtB . jf51clyfme <= rtB . j3dosgzpk4 ) ; rtB
. cdf1j1kyby = ( rtB . jf51clyfme > rtP . u4_Value ) ; rtB . fk0pnrswzn = (
rtB . cdf1j1kyby && rtB . csyedil0zp ) ; if ( rtB . fk0pnrswzn ) { rtB .
hlhic2cgvc = rtB . jf51clyfme * rtB . kqagnyzho3 ; rtB . aqtpnferc0 = rtB .
hlhic2cgvc + rtB . fduxvo2nww ; rtB . mqtyk53he5 = rtB . aqtpnferc0 ; } else
{ rtB . mqtyk53he5 = rtB . jn1xgckye3 ; } rtB . jfvsanwxv2 = rtB . mqtyk53he5
; } rtB . hsdbtenegz = rtB . jfvsanwxv2 ; } if ( rtB . ozar2a1dpt ) { rtB .
o5fimor0y5 = rtB . hsdbtenegz ; } else { rtB . cr1nqnsi3z = ( rtB .
jf51clyfme > rtB . fqzg3xi2hh ) ; if ( rtB . cr1nqnsi3z ) { if ( rtB .
jshk5v4reo ) { rtB . mfk1xdygc2 = rtB . lnwm3zifhf * rtB . jf51clyfme ; rtB .
lriboanuiv = rtB . mfk1xdygc2 + rtB . pta54dovoj ; rtB . hgcysxavsc = rtB .
lriboanuiv ; } else { rtB . hgcysxavsc = rtB . nkbpr3wboi ; } rtB .
lzxrfeuegb = rtB . hgcysxavsc ; } else { rtB . cgaqg5qpzp = ( rtB .
jf51clyfme <= rtB . fqzg3xi2hh ) ; rtB . lhy55uoerz = ( rtB . jf51clyfme >
rtP . u6_Value_iu0fx4dooz ) ; rtB . l5togjlpsh = ( rtB . lhy55uoerz && rtB .
cgaqg5qpzp ) ; if ( rtB . l5togjlpsh ) { rtB . p3bzewax2h = rtB . jf51clyfme
* rtB . lnwm3zifhf ; rtB . e2r4oqn1hb = rtB . p3bzewax2h + rtB . pta54dovoj ;
rtB . loxvqdqzn3 = rtB . e2r4oqn1hb ; } else { rtB . loxvqdqzn3 = rtB .
hsdbtenegz ; } rtB . lzxrfeuegb = rtB . loxvqdqzn3 ; } rtB . o5fimor0y5 = rtB
. lzxrfeuegb ; } rtB . eyhqcgmejo = ( rtB . n40105jbho > rtB . o5fimor0y5 ) ;
rtB . iur5ky1lj1 = ( rtB . o5fimor0y5 == rtP . u5_Value_oxanybzjyo ) ; rtB .
eelrosym1j = ! rtB . iur5ky1lj1 ; rtB . ejuarw1z2u = ( rtB . eyhqcgmejo &&
rtB . eelrosym1j ) ; rtB . aivcvoibgk = ( rtB . n40105jbho > rtB . nkbpr3wboi
) ; rtB . e3uznensvg = ( rtB . aivcvoibgk && rtB . imrosj1vau ) ; rtB .
ikcpw4shmh = ( rtB . ejuarw1z2u || rtB . e3uznensvg ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . imb2wkwea3 = rtDW . hvjbgpscln ; rtB . otquvqh41e = !
rtB . imb2wkwea3 ; } if ( rtP . u1_Value_nswrjlywfh > rtP .
u_Threshold_nmiy30ziw5 ) { rtB . babtzzksvt = ( rtB . ikcpw4shmh && rtB .
otquvqh41e ) ; rtB . kmfaiq5gdy = rtB . babtzzksvt ; } else { rtB .
kmfaiq5gdy = rtB . ikcpw4shmh ; } rtB . btalyd3npc = rtB . l2zahd4oox * rtB .
l2zahd4oox ; rtB . lq3b2emhjy = rtB . g340u5g2f2 * rtB . g340u5g2f2 ; rtB .
bul0kyeh11 = rtB . btalyd3npc + rtB . lq3b2emhjy ; rtB . klpdood0ab =
muDoubleScalarSqrt ( rtB . bul0kyeh11 ) ; rtB . orcmq5h4wx = ( rtB .
klpdood0ab < 0.0 ) ; rtB . h0o5t2gtzi = ! rtB . orcmq5h4wx ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . o1vk0isp23 = rtDW . afrib4g3ns ; rtB
. ho330fy4bu = rtDW . jlbauzygas ; } rtB . kymwpq5gcj = rtB . l2zahd4oox -
rtB . o1vk0isp23 ; rtB . lklsofgty0 = rtB . kymwpq5gcj * rtB . kymwpq5gcj ;
rtB . bvp1x2glex = rtB . g340u5g2f2 - rtB . ho330fy4bu ; rtB . fyndwii1k2 =
rtB . bvp1x2glex * rtB . bvp1x2glex ; rtB . lnsf45q5xw = rtB . lklsofgty0 +
rtB . fyndwii1k2 ; rtB . abq0huqfl5 = muDoubleScalarSqrt ( rtB . lnsf45q5xw )
; rtB . ezr0114tiz = rtB . l2zahd4oox * rtB . l2zahd4oox ; rtB . hokelrdgzd =
rtB . g340u5g2f2 * rtB . g340u5g2f2 ; rtB . mrmsivtfhv = rtB . ezr0114tiz +
rtB . hokelrdgzd ; rtB . ant5kjwsif = muDoubleScalarSqrt ( rtB . mrmsivtfhv )
; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . dvrs20i5gc = rtB . o1vk0isp23
* rtB . o1vk0isp23 ; rtB . pyipmgasa3 = rtB . ho330fy4bu * rtB . ho330fy4bu ;
rtB . mlav2mi4rv = rtB . dvrs20i5gc + rtB . pyipmgasa3 ; rtB . j4j0kw1hss =
muDoubleScalarSqrt ( rtB . mlav2mi4rv ) ; } maxV = muDoubleScalarMax ( rtB .
ant5kjwsif , rtB . j4j0kw1hss ) ; rtB . o0ost4zw4n = maxV ; rtB . jjx2wwks4g
= ( rtB . o0ost4zw4n == rtP . u_Value_jc3dfvi1ol ) ; if ( rtB . jjx2wwks4g )
{ rtB . jngcxgjh3o = rtP . u_Value_akh1lq5sh4 ; } else { rtB . jngcxgjh3o =
rtB . o0ost4zw4n ; } rtB . fy5iobyz2g = rtB . abq0huqfl5 / rtB . jngcxgjh3o ;
rtB . evx14n0ivs = ( rtB . fy5iobyz2g > rtP . u0_Value_esll5gpc2d ) ; rtB .
pt05pvlu1h = ( rtB . h0o5t2gtzi && rtB . evx14n0ivs ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . hmtg2gmje2 = rtDW . n2ncp1ufol ; rtB . kq0foc5zm2 =
rtDW . jzckq3ap3v ; rtB . hgv1prqi1t = rtDW . muro5i15oa ; rtB . putci1dlly =
( rtB . hmtg2gmje2 + rtB . kq0foc5zm2 ) + rtB . hgv1prqi1t ; rtB . cid0e3bfwx
= rtB . putci1dlly * rtP . u_Value_j3kss22qsb ; } rtB . lx1tdfle01 = rtB .
abq0huqfl5 - rtB . cid0e3bfwx ; rtB . mtcrqytlni = muDoubleScalarAbs ( rtB .
lx1tdfle01 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . n0profxf4m = (
rtB . cid0e3bfwx == rtP . u_Value_bvvpe5m1yt ) ; if ( rtB . n0profxf4m ) {
rtB . a42hgpymly = rtP . u_Value_cfj5et1a5g ; } else { rtB . a42hgpymly = rtB
. cid0e3bfwx ; } rtB . aub2naof5y = rtDW . h2djmvry3k ; rtB . b3dwnnaor4 =
rtDW . oicbkbao2v ; rtB . i3mnmaeh1c = ! rtB . b3dwnnaor4 ; rtB . cu1vnqmx1v
= ( rtB . aub2naof5y && rtB . i3mnmaeh1c ) ; } rtB . jj2cm5njtp = rtB .
mtcrqytlni / rtB . a42hgpymly ; rtB . jg11kmlfq0 = ( rtB . jj2cm5njtp > rtP .
u8_Value_m5vidopcu4 ) ; rtB . axt1pfyv5h = ( rtB . abq0huqfl5 < rtP .
u5_Value_cgpsnnrxd4 ) ; rtB . m4igutdnff = ! rtB . axt1pfyv5h ; rtB .
ldfgqvnwmq = ( rtB . jg11kmlfq0 && rtB . m4igutdnff ) ; rtB . ojwyzbseek = (
rtB . pt05pvlu1h && rtB . ldfgqvnwmq ) ; rtB . d011aelgcy = ( rtB .
ojwyzbseek || rtB . cu1vnqmx1v ) ; rtB . nxnlad334s = rtB . d011aelgcy ; rtB
. e2f5wkbxmy = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . fxhy3qc20w , rtB .
nxnlad334s , rtB . e2f5wkbxmy , rtP . OffDelay_delay_h3h3vofi3d , & rtB .
diplwx4hig , & rtDW . diplwx4hig , & rtP . diplwx4hig , & rtPrevZCX .
diplwx4hig ) ; bhc3qgnuic ( rtS , rtB . j5vr5jeqw2 , rtB . nxnlad334s , rtB .
e2f5wkbxmy , rtP . OffDelay_delay_h3h3vofi3d , & rtB . my1xc0on3o , & rtDW .
my1xc0on3o , & rtP . my1xc0on3o , & rtPrevZCX . my1xc0on3o ) ; rtB .
hd1v1uva5a = ( rtB . diplwx4hig . j1vu52vt5l || rtB . my1xc0on3o . mjqylog0rb
) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . o1pqy2pltq = rtDW .
hs2gbbrj20 ; rtB . noqcx41p4r = ! rtB . o1pqy2pltq ; rtB . gleefrzwuw = rtDW
. fnftlbxl0k ; } rtB . jvaw411iaj = ( rtB . hd1v1uva5a && rtB . noqcx41p4r )
; rtB . io0nqp4diw = ( rtB . jvaw411iaj && rtB . dslqrebvta ) ; rtB .
ekeyvbz3jk = rtB . asa103d02z * rtB . asa103d02z ; rtB . k2xrwtypvb = rtB .
ddix2mhi1g * rtB . ddix2mhi1g ; rtB . axhk5jjg2x = rtB . ekeyvbz3jk + rtB .
k2xrwtypvb ; rtB . dx1400nekz = muDoubleScalarSqrt ( rtB . axhk5jjg2x ) ; rtB
. ndtagegztt = ( rtB . dx1400nekz < 0.0 ) ; rtB . nu2vkp2zu4 = ! rtB .
ndtagegztt ; rtB . iblgo5tcs4 = rtB . asa103d02z - rtB . gleefrzwuw ; rtB .
oo2izjpduq = rtB . iblgo5tcs4 * rtB . iblgo5tcs4 ; if ( ssIsSampleHit ( rtS ,
1 , 0 ) ) { rtB . iphvtnqf5a = rtDW . as0d4xo4un ; } rtB . iifhh5scid = rtB .
ddix2mhi1g - rtB . iphvtnqf5a ; rtB . lujuehxqxj = rtB . iifhh5scid * rtB .
iifhh5scid ; rtB . krdcjwyivl = rtB . oo2izjpduq + rtB . lujuehxqxj ; rtB .
me3plfssrh = muDoubleScalarSqrt ( rtB . krdcjwyivl ) ; rtB . p4yrgsubex = rtB
. asa103d02z * rtB . asa103d02z ; rtB . ijjdizgrje = rtB . ddix2mhi1g * rtB .
ddix2mhi1g ; rtB . i1tz3oxa5p = rtB . p4yrgsubex + rtB . ijjdizgrje ; rtB .
pc1hlddxpc = muDoubleScalarSqrt ( rtB . i1tz3oxa5p ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . gm2zpa2pel = rtB . gleefrzwuw * rtB . gleefrzwuw ;
rtB . ifleksknat = rtB . iphvtnqf5a * rtB . iphvtnqf5a ; rtB . jxl443031q =
rtB . gm2zpa2pel + rtB . ifleksknat ; rtB . mlwc4saci2 = muDoubleScalarSqrt (
rtB . jxl443031q ) ; } maxV = muDoubleScalarMax ( rtB . pc1hlddxpc , rtB .
mlwc4saci2 ) ; rtB . lbk4fkoopd = maxV ; rtB . maxmsidak0 = ( rtB .
lbk4fkoopd == rtP . u_Value_gbbowrjh2c ) ; if ( rtB . maxmsidak0 ) { rtB .
pa543l01h3 = rtP . u_Value_ogge3aswhx ; } else { rtB . pa543l01h3 = rtB .
lbk4fkoopd ; } rtB . atophvvvda = rtB . me3plfssrh / rtB . pa543l01h3 ; rtB .
jst1vtq3sd = ( rtB . atophvvvda > rtP . u0_Value_oix5pcw4qm ) ; rtB .
cojjqnmqq2 = ( rtB . nu2vkp2zu4 && rtB . jst1vtq3sd ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . dstxdfarbe = rtDW . dymvp4turd ; rtB . k3ivfvptjm =
rtDW . evvyvdybbq ; rtB . nuc4r4242y = rtDW . ct3p3arebc ; rtB . fxmwjlglxk =
( rtB . dstxdfarbe + rtB . k3ivfvptjm ) + rtB . nuc4r4242y ; rtB . cgtdtnybiu
= rtB . fxmwjlglxk * rtP . u_Value_hmslgx5akt ; } rtB . nlnjdqme1k = rtB .
me3plfssrh - rtB . cgtdtnybiu ; rtB . nkm1okfyxt = muDoubleScalarAbs ( rtB .
nlnjdqme1k ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . d0xbey5115 = (
rtB . cgtdtnybiu == rtP . u_Value_h3exo1iarw ) ; if ( rtB . d0xbey5115 ) {
rtB . aptnvgwdpc = rtP . u_Value_ooubpw0jaz ; } else { rtB . aptnvgwdpc = rtB
. cgtdtnybiu ; } rtB . hnr4pekmfk = rtDW . evfvx3mdmk ; rtB . iynuv5dgcp =
rtDW . kn1fi20kkc ; rtB . ilh1d5nmyk = ! rtB . iynuv5dgcp ; rtB . cpbpeih3i3
= ( rtB . hnr4pekmfk && rtB . ilh1d5nmyk ) ; } rtB . cw4cutiqnd = rtB .
nkm1okfyxt / rtB . aptnvgwdpc ; rtB . jrn3le1bzz = ( rtB . cw4cutiqnd > rtP .
u8_Value_dwoawxwhqs ) ; rtB . nkxzypvj1w = ( rtB . me3plfssrh < rtP .
u5_Value_dsjflch1iw ) ; rtB . cojnumhzpw = ! rtB . nkxzypvj1w ; rtB .
gi1bdezv4r = ( rtB . jrn3le1bzz && rtB . cojnumhzpw ) ; rtB . op5u32jxd5 = (
rtB . cojjqnmqq2 && rtB . gi1bdezv4r ) ; rtB . lecc0iibgu = ( rtB .
op5u32jxd5 || rtB . cpbpeih3i3 ) ; rtB . gql01fb0fm = rtB . lecc0iibgu ; rtB
. fkpgeyjryf = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . at3ruiicny , rtB .
gql01fb0fm , rtB . fkpgeyjryf , rtP . OffDelay_delay_p4otseudsi , & rtB .
gn1qmb4zgp , & rtDW . gn1qmb4zgp , & rtP . gn1qmb4zgp , & rtPrevZCX .
gn1qmb4zgp ) ; bhc3qgnuic ( rtS , rtB . ff1bpcza3s , rtB . gql01fb0fm , rtB .
fkpgeyjryf , rtP . OffDelay_delay_p4otseudsi , & rtB . bvpxesww1j , & rtDW .
bvpxesww1j , & rtP . bvpxesww1j , & rtPrevZCX . bvpxesww1j ) ; rtB .
c1yx2ffjij = ( rtB . gn1qmb4zgp . j1vu52vt5l || rtB . bvpxesww1j . mjqylog0rb
) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . h520gq2402 = rtDW .
aomedx5xd0 ; rtB . ejmoserol0 = ! rtB . h520gq2402 ; } rtB . dwyytovdmt = (
rtB . c1yx2ffjij && rtB . ejmoserol0 ) ; rtB . gr4vjiot5p = ( rtB .
dwyytovdmt && rtB . h1eangrp2e ) ; rtB . nz0rzqmxhs = ( rtB . io0nqp4diw ||
rtB . gr4vjiot5p ) ; rtB . awvq0vswqc = rtB . nz0rzqmxhs ; rtB . fhrzt0i0d0 =
ssGetT ( rtS ) ; on20slldft ( rtS , rtB . eog3r53qxh , rtB . awvq0vswqc , rtB
. fhrzt0i0d0 , rtP . T1_delay_hwuwy4njmq , & rtB . lf152u5jm4 , & rtDW .
lf152u5jm4 , & rtP . lf152u5jm4 , & rtPrevZCX . lf152u5jm4 ) ; bhc3qgnuic (
rtS , rtB . dxajpxc32b , rtB . awvq0vswqc , rtB . fhrzt0i0d0 , rtP .
T1_delay_hwuwy4njmq , & rtB . dewlul2b2r , & rtDW . dewlul2b2r , & rtP .
dewlul2b2r , & rtPrevZCX . dewlul2b2r ) ; rtB . ccbbv2wmil = ( rtB .
lf152u5jm4 . j1vu52vt5l || rtB . dewlul2b2r . mjqylog0rb ) ; rtB . hkxdvz0ovi
= ( rtB . kmfaiq5gdy && rtB . ccbbv2wmil ) ; rtB . izrj3uyj0l = ( rtB .
cw03hxjlsy || rtB . hkxdvz0ovi ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB
. j1ja1v3ew3 = rtDW . f3aybkapro ; } rtB . nytw12ibr4 = ! rtB . ikcpw4shmh ;
rtB . fo3gy1y2q5 = ( rtB . j1ja1v3ew3 && rtB . nytw12ibr4 ) ; rtB .
asc2r0o54z = rtB . fo3gy1y2q5 ; rtB . fxisqtm3sf = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . jph3nsjcrd , rtB . asc2r0o54z , rtB . fxisqtm3sf ,
rtP . T2_delay_nktwzimlrx , & rtB . ivzhqxkykj , & rtDW . ivzhqxkykj , & rtP
. ivzhqxkykj , & rtPrevZCX . ivzhqxkykj ) ; bhc3qgnuic ( rtS , rtB .
dy0y5mqrgw , rtB . asc2r0o54z , rtB . fxisqtm3sf , rtP . T2_delay_nktwzimlrx
, & rtB . kd31jkgljk , & rtDW . kd31jkgljk , & rtP . kd31jkgljk , & rtPrevZCX
. kd31jkgljk ) ; rtB . gxaee0yidt = ( rtB . ivzhqxkykj . j1vu52vt5l || rtB .
kd31jkgljk . mjqylog0rb ) ; rtB . kqzirsl4x4 = ! rtB . gxaee0yidt ; rtB .
itbctkinpn = ( rtB . izrj3uyj0l && rtB . kqzirsl4x4 ) ; rtB . gw2u0we0bq = !
rtB . jyacd1dsia ; rtB . pslefhjvqh = ( rtB . itbctkinpn && rtB . gw2u0we0bq
) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . equy2xio1c = rtDW .
g4kyas3bbl ; rtB . amfwi5vzqf = ! rtB . equy2xio1c ; rtB . ehgvnabetp = rtDW
. ememr0qwkc ; rtB . kkg4lfsn0n = ! rtB . ehgvnabetp ; } rtB . iw4so2inmk = (
rtB . pslefhjvqh && rtB . amfwi5vzqf ) ; rtB . kr2h0fl4w0 = ssGetT ( rtS ) ;
rtB . oehfnegpdd = rtB . pslefhjvqh ; rtB . p2k4trcdah = rtB . oehfnegpdd ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . husbkiklvj = rtDW . jirvvhrkai ;
} itcd43cpff ( rtS , rtB . byqfsens1k [ 0 ] , rtB . p2k4trcdah , rtB .
husbkiklvj , & rtB . culgjirjhd , & rtDW . culgjirjhd ) ; cgvj5xjnjk ( rtS ,
rtB . byqfsens1k [ 1 ] , rtB . p2k4trcdah , rtB . husbkiklvj , & rtB .
dymcbszxp2 , & rtDW . dymcbszxp2 ) ; rtB . ap2ehl2p0j = ( rtB . culgjirjhd .
guqrn12esp || rtB . dymcbszxp2 . l5pxivnpxv ) ; rtB . lhkl2ypirk = ( rtB .
kkg4lfsn0n && rtB . ap2ehl2p0j ) ; rtB . oc4ib4mvlb = rtB . lhkl2ypirk ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . hv22wpizlv = rtDW . insgv4zthl ; } if
( rtB . oc4ib4mvlb ) { rtB . aysknorjt2 = rtB . kr2h0fl4w0 ; } else { rtB .
aysknorjt2 = rtB . hv22wpizlv ; } maxV = rtP . u_PulseDuration_e5xjo2pujb -
2.2204460492503131E-16 ; rtB . fa0plbl4v0 = rtB . aysknorjt2 + maxV ; rtB .
mdzt5k4iw4 = ( rtB . fa0plbl4v0 > rtB . kr2h0fl4w0 ) ; rtB . p2bttd15hc = (
rtB . iw4so2inmk || rtB . mdzt5k4iw4 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) )
{ rtB . kd4badeopn = rtDW . fwreysljhu ; } rtB . mzcgswdjwk = rtB .
dhmmov1jxy * rtB . dhmmov1jxy ; rtB . jclvzpf2la = rtB . ik2cqrdw1z * rtB .
ik2cqrdw1z ; rtB . ic0fb1xu33 = rtB . mzcgswdjwk + rtB . jclvzpf2la ; rtB .
pnj2jxytns = muDoubleScalarSqrt ( rtB . ic0fb1xu33 ) ; rtB . bgufxk5qxd = (
rtB . mnoq2ms3bo > rtB . pnj2jxytns ) ; if ( rtB . bgufxk5qxd ) { rtB .
khri43sxls = rtB . m5evsv14wz ; rtB . pw0fj3ycjv = rtB . kztsytllx1 ; } else
{ rtB . khri43sxls = rtB . dhmmov1jxy ; rtB . pw0fj3ycjv = rtB . ik2cqrdw1z ;
} rtB . hcohz3weij = rtB . khri43sxls * rtB . khri43sxls ; rtB . klzpqtw34d =
rtB . pw0fj3ycjv * rtB . pw0fj3ycjv ; rtB . jtkww50tpw = rtB . hcohz3weij +
rtB . klzpqtw34d ; rtB . d2lxgjgw2u = muDoubleScalarSqrt ( rtB . jtkww50tpw )
; rtB . j1lwh5skmj = rtB . pjcdsoe04i * rtB . pjcdsoe04i ; rtB . jgzz4hslyq =
rtB . lbky4lats5 * rtB . lbky4lats5 ; rtB . erqkk4eebx = rtB . j1lwh5skmj +
rtB . jgzz4hslyq ; rtB . kafabiioyc = muDoubleScalarSqrt ( rtB . erqkk4eebx )
; rtB . dj23f1yuzs = ( rtB . d2lxgjgw2u > rtB . kafabiioyc ) ; if ( rtB .
dj23f1yuzs ) { rtB . jf10qmt5ja = rtB . khri43sxls ; rtB . bnngbimh0v = rtB .
pw0fj3ycjv ; } else { rtB . jf10qmt5ja = rtB . pjcdsoe04i ; rtB . bnngbimh0v
= rtB . lbky4lats5 ; } rtB . kaootserol = rtB . jf10qmt5ja - rtB . ilqpizv1kn
; rtB . dungd24t4x = rtB . bnngbimh0v - rtB . l4cyxxkqo1 ; rtB . l5l4uf5obc =
rtB . jf10qmt5ja * rtB . jf10qmt5ja ; rtB . ato2f3k325 = rtB . bnngbimh0v *
rtB . bnngbimh0v ; rtB . iwxztboicc = rtB . l5l4uf5obc + rtB . ato2f3k325 ;
rtB . pk5zpulaen = muDoubleScalarSqrt ( rtB . iwxztboicc ) ; rtB . ar4wcmap5b
= rtB . jf10qmt5ja / rtB . pk5zpulaen ; rtB . dbkci5dre2 = rtB . kaootserol *
rtB . kaootserol ; rtB . fzllhxid50 = rtB . dungd24t4x * rtB . dungd24t4x ;
rtB . fhtrwissi4 = rtB . dbkci5dre2 + rtB . fzllhxid50 ; rtB . idb0rjc2zz =
muDoubleScalarSqrt ( rtB . fhtrwissi4 ) ; rtB . bdd2vcqaga = rtB . kaootserol
/ rtB . idb0rjc2zz ; rtB . oe3epn5i4g = rtB . ar4wcmap5b - rtB . bdd2vcqaga ;
rtB . hhlafq3rrl = rtB . oe3epn5i4g * rtB . oe3epn5i4g ; rtB . auqsnu05d5 =
rtB . bnngbimh0v / rtB . pk5zpulaen ; rtB . l525h3uerc = rtB . dungd24t4x /
rtB . idb0rjc2zz ; rtB . hfyupooe30 = rtB . auqsnu05d5 - rtB . l525h3uerc ;
rtB . gqkkx14iqh = rtB . hfyupooe30 * rtB . hfyupooe30 ; rtB . gdvwacw2rr =
rtB . hhlafq3rrl + rtB . gqkkx14iqh ; rtB . fkkcvogoek = rtB . gdvwacw2rr *
rtP . Constant3_Value_ldczh53mwz ; rtB . mjzy2gvtqk = rtP .
Constant2_Value_eoeaaz340i - rtB . fkkcvogoek ; u0 = rtB . mjzy2gvtqk ; if (
u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } rtB .
ncizcqbp3j = muDoubleScalarAcos ( u0 ) ; rtB . e0ewcbvnk5 = rtB . auqsnu05d5
- rtB . bdd2vcqaga ; rtB . ifrwlc0gim = rtB . e0ewcbvnk5 * rtB . e0ewcbvnk5 ;
rtB . nozfqaxqha = rtB . ar4wcmap5b + rtB . l525h3uerc ; rtB . mceyrmyryj =
rtB . nozfqaxqha * rtB . nozfqaxqha ; rtB . i1civz3cda = rtB . ifrwlc0gim +
rtB . mceyrmyryj ; rtB . ffc1rjh2uo = ( rtB . i1civz3cda > rtP .
Constant_Value_bd2lhixjc3 ) ; if ( rtB . ffc1rjh2uo ) { rtB . peuuyomqif =
rtB . ncizcqbp3j * rtP . Constant1_Value_mt1fcgt2hw ; rtB . kw4f2a2ijs = rtB
. peuuyomqif ; } else { rtB . kw4f2a2ijs = rtB . ncizcqbp3j ; } rtB .
hki5gjxgg4 = muDoubleScalarCos ( rtB . kw4f2a2ijs ) ; rtB . bnm1xzooc2 = (
rtB . hki5gjxgg4 >= rtP . u_Value_iyiza3iutg ) ; if ( rtB . bnm1xzooc2 ) {
rtB . hckl12m41a = rtB . dungd24t4x * rtB . dungd24t4x ; rtB . fkgti5r0ha =
rtB . kaootserol * rtB . kaootserol ; rtB . bwlbfw1b3v = rtB . fkgti5r0ha +
rtB . hckl12m41a ; rtB . amzyqik5j5 = muDoubleScalarSqrt ( rtB . bwlbfw1b3v )
; rtB . oisrrutyav = rtB . bnngbimh0v * rtB . bnngbimh0v ; rtB . fbgauouchm =
rtB . jf10qmt5ja * rtB . jf10qmt5ja ; rtB . j0bl0c3pjl = rtB . fbgauouchm +
rtB . oisrrutyav ; rtB . fzerpmpvat = muDoubleScalarSqrt ( rtB . j0bl0c3pjl )
; rtB . fbucl1f50x = rtB . fzerpmpvat * rtB . amzyqik5j5 * rtB . hki5gjxgg4 ;
rtB . cfgtz022pw = muDoubleScalarSqrt ( rtB . fbucl1f50x ) ; rtB . dxaxhnwclg
= rtB . cfgtz022pw ; } else { rtB . dxaxhnwclg = rtP . u7_Value_ds2brihoaq ;
} if ( rtB . ewccowmzqa ) { rtB . mkfvgdpua0 = rtP . u1_Value_ccttayn34d ; }
else { rtB . ekordy4vnt = ( rtB . dxaxhnwclg > rtB . feubulmqgt ) ; if ( rtB
. ekordy4vnt ) { if ( rtB . kf40alu3eq ) { rtB . hslkrxiy5s = rtB .
egreouhkpb * rtB . dxaxhnwclg ; rtB . ot2dalpfcx = rtB . hslkrxiy5s + rtB .
luuvrrnse0 ; rtB . olhhctzfzz = rtB . ot2dalpfcx ; } else { rtB . olhhctzfzz
= rtB . nkbpr3wboi ; } rtB . bdpezbt0r4 = rtB . olhhctzfzz ; } else { rtB .
budf4hhayc = ( rtB . dxaxhnwclg <= rtB . feubulmqgt ) ; rtB . ku1uj0y5cg = (
rtB . dxaxhnwclg > rtP . u_Value_n0ldpwgypr ) ; rtB . modmsylnni = ( rtB .
ku1uj0y5cg && rtB . budf4hhayc ) ; if ( rtB . modmsylnni ) { rtB . corlxtpx5p
= rtB . dxaxhnwclg * rtB . egreouhkpb ; rtB . k1rlx1dm13 = rtB . corlxtpx5p +
rtB . luuvrrnse0 ; rtB . pywpssknlk = rtB . k1rlx1dm13 ; } else { rtB .
pywpssknlk = rtP . u1_Value_ccttayn34d ; } rtB . bdpezbt0r4 = rtB .
pywpssknlk ; } rtB . mkfvgdpua0 = rtB . bdpezbt0r4 ; } if ( rtB . ivqryhm1oo
) { rtB . juwumap0ok = rtB . mkfvgdpua0 ; } else { rtB . bznstzjges = ( rtB .
dxaxhnwclg > rtB . ijyysyrj1k ) ; if ( rtB . bznstzjges ) { if ( rtB .
jekq4zcjsb ) { rtB . fv2krwds5r = rtB . e04rtiw1z1 * rtB . dxaxhnwclg ; rtB .
aplpm2dvoy = rtB . fv2krwds5r + rtB . oy0r2tr3a0 ; rtB . eri3bho4do = rtB .
aplpm2dvoy ; } else { rtB . eri3bho4do = rtB . nkbpr3wboi ; } rtB .
ddu5lljftn = rtB . eri3bho4do ; } else { rtB . j1pnwrykwj = ( rtB .
dxaxhnwclg <= rtB . ijyysyrj1k ) ; rtB . h15fpmkv2u = ( rtB . dxaxhnwclg >
rtP . u2_Value_c1sg11dlld ) ; rtB . ddmcob2qp4 = ( rtB . h15fpmkv2u && rtB .
j1pnwrykwj ) ; if ( rtB . ddmcob2qp4 ) { rtB . gjvooiyvb0 = rtB . dxaxhnwclg
* rtB . e04rtiw1z1 ; rtB . mtzuamb3eg = rtB . gjvooiyvb0 + rtB . oy0r2tr3a0 ;
rtB . kmi2ntnod2 = rtB . mtzuamb3eg ; } else { rtB . kmi2ntnod2 = rtB .
mkfvgdpua0 ; } rtB . ddu5lljftn = rtB . kmi2ntnod2 ; } rtB . juwumap0ok = rtB
. ddu5lljftn ; } if ( rtB . j1thez4uds ) { rtB . hrlgi54x5p = rtB .
juwumap0ok ; } else { rtB . emmlspfhay = ( rtB . dxaxhnwclg > rtB .
oez50wrxue ) ; if ( rtB . emmlspfhay ) { if ( rtB . fiuzhq0zv5 ) { rtB .
pj2dc3kbv2 = rtB . dbwem4azxl * rtB . dxaxhnwclg ; rtB . kydxak5xlc = rtB .
pj2dc3kbv2 + rtB . lb40wqt2rg ; rtB . marmj32kfs = rtB . kydxak5xlc ; } else
{ rtB . marmj32kfs = rtB . nkbpr3wboi ; } rtB . danf55jte4 = rtB . marmj32kfs
; } else { rtB . musibhm0ol = ( rtB . dxaxhnwclg <= rtB . oez50wrxue ) ; rtB
. gu43vf3dht = ( rtB . dxaxhnwclg > rtP . u4_Value ) ; rtB . boisqothxb = (
rtB . gu43vf3dht && rtB . musibhm0ol ) ; if ( rtB . boisqothxb ) { rtB .
pbv5mvl1fb = rtB . dxaxhnwclg * rtB . dbwem4azxl ; rtB . muxj44q1su = rtB .
pbv5mvl1fb + rtB . lb40wqt2rg ; rtB . dgqr3pnwad = rtB . muxj44q1su ; } else
{ rtB . dgqr3pnwad = rtB . juwumap0ok ; } rtB . danf55jte4 = rtB . dgqr3pnwad
; } rtB . hrlgi54x5p = rtB . danf55jte4 ; } if ( rtB . jo1c1ihqre ) { rtB .
h414b40ucg = rtB . hrlgi54x5p ; } else { rtB . ogmsdnc1f0 = ( rtB .
dxaxhnwclg > rtB . lmt12oy223 ) ; if ( rtB . ogmsdnc1f0 ) { if ( rtB .
oqlhlhhxho ) { rtB . daiaqxwgrl = rtB . nmhqg24xmr * rtB . dxaxhnwclg ; rtB .
jrabfuheh5 = rtB . daiaqxwgrl + rtB . c4csnfqbhu ; rtB . ane544sgu5 = rtB .
jrabfuheh5 ; } else { rtB . ane544sgu5 = rtB . nkbpr3wboi ; } rtB .
hl5d1sejim = rtB . ane544sgu5 ; } else { rtB . mmxbu4z0en = ( rtB .
dxaxhnwclg <= rtB . lmt12oy223 ) ; rtB . auvhfp1yd5 = ( rtB . dxaxhnwclg >
rtP . u6_Value_iu0fx4dooz ) ; rtB . e50orlp2jc = ( rtB . auvhfp1yd5 && rtB .
mmxbu4z0en ) ; if ( rtB . e50orlp2jc ) { rtB . lvcwp011v2 = rtB . dxaxhnwclg
* rtB . nmhqg24xmr ; rtB . msglkevxne = rtB . lvcwp011v2 + rtB . c4csnfqbhu ;
rtB . ggntmankut = rtB . msglkevxne ; } else { rtB . ggntmankut = rtB .
hrlgi54x5p ; } rtB . hl5d1sejim = rtB . ggntmankut ; } rtB . h414b40ucg = rtB
. hl5d1sejim ; } rtB . lpn0g2fwyr = ( rtB . lwsdutl0jd > rtB . h414b40ucg ) ;
rtB . chpwqwb1uk = ( rtB . h414b40ucg == rtP . u5_Value_irvthq4saz ) ; rtB .
bjiuwvami4 = ! rtB . chpwqwb1uk ; rtB . nxfzol403e = ( rtB . lpn0g2fwyr &&
rtB . bjiuwvami4 ) ; rtB . gleokz4ki0 = ( rtB . lwsdutl0jd > rtB . nkbpr3wboi
) ; rtB . frykuty12p = ( rtB . gleokz4ki0 && rtB . fffmpf1xlo ) ; rtB .
hghek1rdpc = ( rtB . nxfzol403e || rtB . frykuty12p ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . lo0prdfa01 = rtDW . gvtu2sgvui ; rtB . h2tmefzyp0 = !
rtB . lo0prdfa01 ; } if ( rtP . u1_Value_nswrjlywfh > rtP .
u_Threshold_i5oelajvb0 ) { rtB . plttas13qm = ( rtB . hghek1rdpc && rtB .
h2tmefzyp0 ) ; rtB . fx3x5cng2g = rtB . plttas13qm ; } else { rtB .
fx3x5cng2g = rtB . hghek1rdpc ; } rtB . f0vs5iy3pd = rtB . emlwggxmjw * rtB .
emlwggxmjw ; rtB . fnutp2vz5c = rtB . nubgwxsn52 * rtB . nubgwxsn52 ; rtB .
bskl3e0ijz = rtB . f0vs5iy3pd + rtB . fnutp2vz5c ; rtB . giumnj2qu3 =
muDoubleScalarSqrt ( rtB . bskl3e0ijz ) ; rtB . jt0bj2jvfu = ( rtB .
giumnj2qu3 < 0.0 ) ; rtB . dzqjvvjnw2 = ! rtB . jt0bj2jvfu ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . hwhh445rbd = rtDW . igkazskzyf ; rtB
. pl2ppwdjgc = rtDW . fer4rqlapl ; } rtB . hiigrlujei = rtB . emlwggxmjw -
rtB . hwhh445rbd ; rtB . jctxuov2cu = rtB . hiigrlujei * rtB . hiigrlujei ;
rtB . jgckv3130p = rtB . nubgwxsn52 - rtB . pl2ppwdjgc ; rtB . mdsglvgchw =
rtB . jgckv3130p * rtB . jgckv3130p ; rtB . khh2pqnlit = rtB . jctxuov2cu +
rtB . mdsglvgchw ; rtB . f4thrtkrpr = muDoubleScalarSqrt ( rtB . khh2pqnlit )
; rtB . kf40oisic4 = rtB . emlwggxmjw * rtB . emlwggxmjw ; rtB . pn0qkmrutb =
rtB . nubgwxsn52 * rtB . nubgwxsn52 ; rtB . nf3uxgmzwi = rtB . kf40oisic4 +
rtB . pn0qkmrutb ; rtB . gaxihhe0hq = muDoubleScalarSqrt ( rtB . nf3uxgmzwi )
; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . eubwb3vvzt = rtB . hwhh445rbd
* rtB . hwhh445rbd ; rtB . gpdmuo3mhe = rtB . pl2ppwdjgc * rtB . pl2ppwdjgc ;
rtB . bcmkxpmgxd = rtB . eubwb3vvzt + rtB . gpdmuo3mhe ; rtB . l5kpwgdhj4 =
muDoubleScalarSqrt ( rtB . bcmkxpmgxd ) ; } maxV = muDoubleScalarMax ( rtB .
gaxihhe0hq , rtB . l5kpwgdhj4 ) ; rtB . meklopiqp5 = maxV ; rtB . dpt2qjzgmw
= ( rtB . meklopiqp5 == rtP . u_Value_gdm1synjb2 ) ; if ( rtB . dpt2qjzgmw )
{ rtB . iu1n2t4bly = rtP . u_Value_g54idoupcl ; } else { rtB . iu1n2t4bly =
rtB . meklopiqp5 ; } rtB . pkng25duhn = rtB . f4thrtkrpr / rtB . iu1n2t4bly ;
rtB . oxmmoxkhyo = ( rtB . pkng25duhn > rtP . u0_Value_esll5gpc2d ) ; rtB .
eazpmth4el = ( rtB . dzqjvvjnw2 && rtB . oxmmoxkhyo ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . e3kpmmlmpp = rtDW . lczlugrqza ; rtB . b2l3iqm0n2 =
rtDW . fkz4l3azou ; rtB . jcyxjk42b4 = rtDW . hrqcp0us0o ; rtB . hlkbexrszr =
( rtB . e3kpmmlmpp + rtB . b2l3iqm0n2 ) + rtB . jcyxjk42b4 ; rtB . es1apvv4da
= rtB . hlkbexrszr * rtP . u_Value_kurn0u2zny ; } rtB . df4xdn44no = rtB .
f4thrtkrpr - rtB . es1apvv4da ; rtB . hdxlv0puwt = muDoubleScalarAbs ( rtB .
df4xdn44no ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . iwdunlfvrg = (
rtB . es1apvv4da == rtP . u_Value_o1xjmuya3t ) ; if ( rtB . iwdunlfvrg ) {
rtB . jif3ey1yco = rtP . u_Value_bun3al3qpf ; } else { rtB . jif3ey1yco = rtB
. es1apvv4da ; } rtB . abang0bxnh = rtDW . ji2nko31lt ; rtB . lrklhkwj10 =
rtDW . cqrjo5bott ; rtB . dttksobtjv = ! rtB . lrklhkwj10 ; rtB . aocbkdjgry
= ( rtB . abang0bxnh && rtB . dttksobtjv ) ; } rtB . auue1c0tmn = rtB .
hdxlv0puwt / rtB . jif3ey1yco ; rtB . fk1n5c251t = ( rtB . auue1c0tmn > rtP .
u8_Value_m5vidopcu4 ) ; rtB . fprypu0pbm = ( rtB . f4thrtkrpr < rtP .
u5_Value_cgpsnnrxd4 ) ; rtB . mzms2rklre = ! rtB . fprypu0pbm ; rtB .
gd3q4uvt2g = ( rtB . fk1n5c251t && rtB . mzms2rklre ) ; rtB . jh443z4du0 = (
rtB . eazpmth4el && rtB . gd3q4uvt2g ) ; rtB . pcsrwwgvf2 = ( rtB .
jh443z4du0 || rtB . aocbkdjgry ) ; rtB . jkm1lqzqb3 = rtB . pcsrwwgvf2 ; rtB
. ecwda5gv31 = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . lsrdffqt2n , rtB .
jkm1lqzqb3 , rtB . ecwda5gv31 , rtP . OffDelay_delay_moh1xjvcif , & rtB .
if1byo0yst , & rtDW . if1byo0yst , & rtP . if1byo0yst , & rtPrevZCX .
if1byo0yst ) ; bhc3qgnuic ( rtS , rtB . h3p5ewzebq , rtB . jkm1lqzqb3 , rtB .
ecwda5gv31 , rtP . OffDelay_delay_moh1xjvcif , & rtB . j5kctkibbm , & rtDW .
j5kctkibbm , & rtP . j5kctkibbm , & rtPrevZCX . j5kctkibbm ) ; rtB .
l0lfzybrpw = ( rtB . if1byo0yst . j1vu52vt5l || rtB . j5kctkibbm . mjqylog0rb
) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . ptkxjyvspb = rtDW .
ndo55xdrjz ; rtB . b0gcy5kuc5 = ! rtB . ptkxjyvspb ; rtB . awh5feillk = rtDW
. pxt53ienu3 ; } rtB . nwlthkn33y = ( rtB . l0lfzybrpw && rtB . b0gcy5kuc5 )
; rtB . ktvbljhq2p = ( rtB . nwlthkn33y && rtB . lg20acdfom ) ; rtB .
hpok3utmmu = rtB . fizn4ck4n1 * rtB . fizn4ck4n1 ; rtB . h0uprmpbml = rtB .
ndiffna1hx * rtB . ndiffna1hx ; rtB . ksyqpqbwac = rtB . hpok3utmmu + rtB .
h0uprmpbml ; rtB . k5ob5yhqwa = muDoubleScalarSqrt ( rtB . ksyqpqbwac ) ; rtB
. luqicsu3jm = ( rtB . k5ob5yhqwa < 0.0 ) ; rtB . ixcgnb42ht = ! rtB .
luqicsu3jm ; rtB . dzwn2vwstj = rtB . fizn4ck4n1 - rtB . awh5feillk ; rtB .
my34pyeodi = rtB . dzwn2vwstj * rtB . dzwn2vwstj ; if ( ssIsSampleHit ( rtS ,
1 , 0 ) ) { rtB . bvagjjcpxc = rtDW . auxttyil24 ; } rtB . d101lp2nkb = rtB .
ndiffna1hx - rtB . bvagjjcpxc ; rtB . pexhg20s51 = rtB . d101lp2nkb * rtB .
d101lp2nkb ; rtB . g2ddcbxg0h = rtB . my34pyeodi + rtB . pexhg20s51 ; rtB .
momorbvhot = muDoubleScalarSqrt ( rtB . g2ddcbxg0h ) ; rtB . l53hexc4mq = rtB
. fizn4ck4n1 * rtB . fizn4ck4n1 ; rtB . gvoq5eyax4 = rtB . ndiffna1hx * rtB .
ndiffna1hx ; rtB . aqevriauna = rtB . l53hexc4mq + rtB . gvoq5eyax4 ; rtB .
hk401gnps3 = muDoubleScalarSqrt ( rtB . aqevriauna ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . i3bnd1hegq = rtB . awh5feillk * rtB . awh5feillk ;
rtB . addmwvxouo = rtB . bvagjjcpxc * rtB . bvagjjcpxc ; rtB . iswooehfxq =
rtB . i3bnd1hegq + rtB . addmwvxouo ; rtB . cara0q1dz3 = muDoubleScalarSqrt (
rtB . iswooehfxq ) ; } maxV = muDoubleScalarMax ( rtB . hk401gnps3 , rtB .
cara0q1dz3 ) ; rtB . arrxdd5ji0 = maxV ; rtB . bxdzr40hcd = ( rtB .
arrxdd5ji0 == rtP . u_Value_ie1brdhihw ) ; if ( rtB . bxdzr40hcd ) { rtB .
ol13htdczz = rtP . u_Value_i1biqdac4x ; } else { rtB . ol13htdczz = rtB .
arrxdd5ji0 ; } rtB . py5dsqc0a2 = rtB . momorbvhot / rtB . ol13htdczz ; rtB .
eafpekl2at = ( rtB . py5dsqc0a2 > rtP . u0_Value_oix5pcw4qm ) ; rtB .
jlwtkodtto = ( rtB . ixcgnb42ht && rtB . eafpekl2at ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . h0ozrpudg0 = rtDW . nd3ykklatb ; rtB . gniuklex1s =
rtDW . aowojzitws ; rtB . m5nsjxbxzf = rtDW . gubginzy5v ; rtB . cr4fltn3z1 =
( rtB . h0ozrpudg0 + rtB . gniuklex1s ) + rtB . m5nsjxbxzf ; rtB . dteg34nhdv
= rtB . cr4fltn3z1 * rtP . u_Value_j5h4jstsez ; } rtB . elttvcspil = rtB .
momorbvhot - rtB . dteg34nhdv ; rtB . mror0tlxsj = muDoubleScalarAbs ( rtB .
elttvcspil ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . hffye5tck4 = (
rtB . dteg34nhdv == rtP . u_Value_p4rj0alcis ) ; if ( rtB . hffye5tck4 ) {
rtB . ezmi0nenb0 = rtP . u_Value_czsaqnby0v ; } else { rtB . ezmi0nenb0 = rtB
. dteg34nhdv ; } rtB . dn3b1pmnmj = rtDW . emo0onv4iy ; rtB . fa2y4xvfqi =
rtDW . oevsqktqdh ; rtB . azp0bcp0l1 = ! rtB . fa2y4xvfqi ; rtB . psnkypaqq0
= ( rtB . dn3b1pmnmj && rtB . azp0bcp0l1 ) ; } rtB . afw03ajbwk = rtB .
mror0tlxsj / rtB . ezmi0nenb0 ; rtB . f1horrl12j = ( rtB . afw03ajbwk > rtP .
u8_Value_dwoawxwhqs ) ; rtB . el1okjznj3 = ( rtB . momorbvhot < rtP .
u5_Value_dsjflch1iw ) ; rtB . e2zdsultrg = ! rtB . el1okjznj3 ; rtB .
p1q4klxldu = ( rtB . f1horrl12j && rtB . e2zdsultrg ) ; rtB . prgykcukfr = (
rtB . jlwtkodtto && rtB . p1q4klxldu ) ; rtB . dt35so0slm = ( rtB .
prgykcukfr || rtB . psnkypaqq0 ) ; rtB . grndxsktw4 = rtB . dt35so0slm ; rtB
. fyqgh3asi5 = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . exhkmjxrht , rtB .
grndxsktw4 , rtB . fyqgh3asi5 , rtP . OffDelay_delay_foo322g3pr , & rtB .
onwgyzhgir , & rtDW . onwgyzhgir , & rtP . onwgyzhgir , & rtPrevZCX .
onwgyzhgir ) ; bhc3qgnuic ( rtS , rtB . m1fbpm2qm1 , rtB . grndxsktw4 , rtB .
fyqgh3asi5 , rtP . OffDelay_delay_foo322g3pr , & rtB . ofq4vda2xo , & rtDW .
ofq4vda2xo , & rtP . ofq4vda2xo , & rtPrevZCX . ofq4vda2xo ) ; rtB .
kgisxdmvsw = ( rtB . onwgyzhgir . j1vu52vt5l || rtB . ofq4vda2xo . mjqylog0rb
) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . fiml4iolx2 = rtDW .
h2duquj31a ; rtB . lhi3m2suv4 = ! rtB . fiml4iolx2 ; } rtB . mvuhtplreq = (
rtB . kgisxdmvsw && rtB . lhi3m2suv4 ) ; rtB . lz0cksews5 = ( rtB .
mvuhtplreq && rtB . cqa5nibvpx ) ; rtB . dfqlgv1ean = ( rtB . ktvbljhq2p ||
rtB . lz0cksews5 ) ; rtB . bwflb2d35o = rtB . dfqlgv1ean ; rtB . hbw4y1ulu4 =
ssGetT ( rtS ) ; on20slldft ( rtS , rtB . gll2jods23 , rtB . bwflb2d35o , rtB
. hbw4y1ulu4 , rtP . T1_delay_m00ba3b3wr , & rtB . b13ln335zj , & rtDW .
b13ln335zj , & rtP . b13ln335zj , & rtPrevZCX . b13ln335zj ) ; bhc3qgnuic (
rtS , rtB . hpmke4ckgb , rtB . bwflb2d35o , rtB . hbw4y1ulu4 , rtP .
T1_delay_m00ba3b3wr , & rtB . d5dfks3vhb , & rtDW . d5dfks3vhb , & rtP .
d5dfks3vhb , & rtPrevZCX . d5dfks3vhb ) ; rtB . l21teq4hnz = ( rtB .
b13ln335zj . j1vu52vt5l || rtB . d5dfks3vhb . mjqylog0rb ) ; rtB . lih5txvlk2
= ( rtB . fx3x5cng2g && rtB . l21teq4hnz ) ; rtB . ciqvf2d3i1 = ( rtB .
kd4badeopn || rtB . lih5txvlk2 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB
. au0bo10lqa = rtDW . ikejccrfw4 ; } rtB . iaof24xqe5 = ! rtB . hghek1rdpc ;
rtB . d5jzp1wgdz = ( rtB . au0bo10lqa && rtB . iaof24xqe5 ) ; rtB .
gslx5c5bui = rtB . d5jzp1wgdz ; rtB . dqbdvngcfr = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . hizqf10xce , rtB . gslx5c5bui , rtB . dqbdvngcfr ,
rtP . T2_delay_p4siplcste , & rtB . d2dryokhth , & rtDW . d2dryokhth , & rtP
. d2dryokhth , & rtPrevZCX . d2dryokhth ) ; bhc3qgnuic ( rtS , rtB .
nvymz5yhdv , rtB . gslx5c5bui , rtB . dqbdvngcfr , rtP . T2_delay_p4siplcste
, & rtB . jcev1klzwf , & rtDW . jcev1klzwf , & rtP . jcev1klzwf , & rtPrevZCX
. jcev1klzwf ) ; rtB . c0wvcvb2da = ( rtB . d2dryokhth . j1vu52vt5l || rtB .
jcev1klzwf . mjqylog0rb ) ; rtB . erpgh1clnk = ! rtB . c0wvcvb2da ; rtB .
hhp0z2gwbc = ( rtB . ciqvf2d3i1 && rtB . erpgh1clnk ) ; rtB . manutv2g52 = !
rtB . jyacd1dsia ; rtB . bkm0bor5oa = ( rtB . hhp0z2gwbc && rtB . manutv2g52
) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . o2wejqmssr = rtDW .
fyqrim4x4y ; rtB . endgwkoaqb = ! rtB . o2wejqmssr ; rtB . hxc3nz1scz = rtDW
. lw0ubiboa0 ; rtB . co5wmbr2je = ! rtB . hxc3nz1scz ; } rtB . dbctjfkikf = (
rtB . bkm0bor5oa && rtB . endgwkoaqb ) ; rtB . bxhrimokcm = ssGetT ( rtS ) ;
rtB . bdgn4etvh5 = rtB . bkm0bor5oa ; rtB . kvdxfpuwws = rtB . bdgn4etvh5 ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . nkm2chucx5 = rtDW . odfrq0vcqw ;
} itcd43cpff ( rtS , rtB . bvkvtsqqsw [ 0 ] , rtB . kvdxfpuwws , rtB .
nkm2chucx5 , & rtB . dxl224bky1 , & rtDW . dxl224bky1 ) ; cgvj5xjnjk ( rtS ,
rtB . bvkvtsqqsw [ 1 ] , rtB . kvdxfpuwws , rtB . nkm2chucx5 , & rtB .
mixbw55a4u , & rtDW . mixbw55a4u ) ; rtB . cyycyjy3do = ( rtB . dxl224bky1 .
guqrn12esp || rtB . mixbw55a4u . l5pxivnpxv ) ; rtB . lgzh1zl4ix = ( rtB .
co5wmbr2je && rtB . cyycyjy3do ) ; rtB . h4pohdrywx = rtB . lgzh1zl4ix ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . nszhcmi1lr = rtDW . hlxu2b4hfp ; } if
( rtB . h4pohdrywx ) { rtB . oyvm5cb5r3 = rtB . bxhrimokcm ; } else { rtB .
oyvm5cb5r3 = rtB . nszhcmi1lr ; } maxV = rtP . u_PulseDuration_laeot2mcjt -
2.2204460492503131E-16 ; rtB . fmqci4qb3a = rtB . oyvm5cb5r3 + maxV ; rtB .
pkbqlnw2xh = ( rtB . fmqci4qb3a > rtB . bxhrimokcm ) ; rtB . pdxzwxj3u0 = (
rtB . dbctjfkikf || rtB . pkbqlnw2xh ) ; rtB . i3swf20ecz = ( rtB .
jdvugwqi3f || rtB . p2bttd15hc || rtB . pdxzwxj3u0 ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . i2grayuzs3 = rtP . donotdeletethisgain_Gain * rtB .
hwsskqat2q [ 45 ] ; rtB . fkfxdqs5lh = rtP .
donotdeletethisgain_Gain_dz5np3rg44 * rtB . hwsskqat2q [ 46 ] ; rtB .
idtm4drm4u = rtP . donotdeletethisgain_Gain_gqjhpmueld * rtB . hwsskqat2q [
47 ] ; rtB . nik21ed5s0 [ 0 ] = rtP . Kv_Gain * rtB . i2grayuzs3 ; rtB .
nik21ed5s0 [ 1 ] = rtP . Kv_Gain * rtB . fkfxdqs5lh ; rtB . nik21ed5s0 [ 2 ]
= rtP . Kv_Gain * rtB . idtm4drm4u ; rtB . bmeofyowkm [ 0 ] = rtP .
u_Gain_gy1riwf4d5 * rtB . nik21ed5s0 [ 0 ] ; rtB . bmeofyowkm [ 1 ] = rtP .
u_Gain_gy1riwf4d5 * rtB . nik21ed5s0 [ 1 ] ; rtB . bmeofyowkm [ 2 ] = rtP .
u_Gain_gy1riwf4d5 * rtB . nik21ed5s0 [ 2 ] ; rtB . oje2qswi4c = rtP .
donotdeletethisgain_Gain_gt0efscrd0 * rtB . hwsskqat2q [ 24 ] ; rtB .
ivx4hnx0t3 = rtP . donotdeletethisgain_Gain_lkikmqfl5u * rtB . hwsskqat2q [
25 ] ; rtB . artw21xbt2 = rtP . donotdeletethisgain_Gain_cbpctfwtnk * rtB .
hwsskqat2q [ 26 ] ; rtB . bw32cklzez [ 0 ] = rtP . Kv1_Gain * rtB .
oje2qswi4c ; rtB . bw32cklzez [ 1 ] = rtP . Kv1_Gain * rtB . ivx4hnx0t3 ; rtB
. bw32cklzez [ 2 ] = rtP . Kv1_Gain * rtB . artw21xbt2 ; rtB . hqf4mnhqso [ 0
] = rtP . u5_Gain * rtB . bw32cklzez [ 0 ] ; rtB . hqf4mnhqso [ 1 ] = rtP .
u5_Gain * rtB . bw32cklzez [ 1 ] ; rtB . hqf4mnhqso [ 2 ] = rtP . u5_Gain *
rtB . bw32cklzez [ 2 ] ; rtB . fik45vubun = rtP .
donotdeletethisgain_Gain_nynyhfolay * rtB . hwsskqat2q [ 33 ] ; rtB .
l0ncooc0ql = rtP . donotdeletethisgain_Gain_cbacgl4d4j * rtB . hwsskqat2q [
34 ] ; rtB . hj1on03foh = rtP . donotdeletethisgain_Gain_b3ezp4yooe * rtB .
hwsskqat2q [ 35 ] ; rtB . c1gjpodm2x [ 0 ] = rtP . Kv_Gain_anqocv2vc5 * rtB .
fik45vubun ; rtB . c1gjpodm2x [ 1 ] = rtP . Kv_Gain_anqocv2vc5 * rtB .
l0ncooc0ql ; rtB . c1gjpodm2x [ 2 ] = rtP . Kv_Gain_anqocv2vc5 * rtB .
hj1on03foh ; rtB . duyxr0xofz [ 0 ] = rtP . u_Gain_ntj0acg2pr * rtB .
c1gjpodm2x [ 0 ] ; rtB . duyxr0xofz [ 1 ] = rtP . u_Gain_ntj0acg2pr * rtB .
c1gjpodm2x [ 1 ] ; rtB . duyxr0xofz [ 2 ] = rtP . u_Gain_ntj0acg2pr * rtB .
c1gjpodm2x [ 2 ] ; } rtB . eqw1o223zh = muDoubleScalarCos ( rtB . janxwpkdcj
[ 0 ] ) ; rtB . ctkxwfjqo4 = rtB . lpgwkwyj4h [ 0 ] * rtB . eqw1o223zh ; rtB
. gwkrvtlqcl = rtB . ctkxwfjqo4 * rtB . mxh01l3oxv ; rtB . as3ppx3osb = rtB .
gwkrvtlqcl * rtB . gwkrvtlqcl ; rtB . omk0plqldo = muDoubleScalarSin ( rtB .
janxwpkdcj [ 0 ] ) ; rtB . dl425wbmtl = rtB . omk0plqldo * rtB . lpgwkwyj4h [
0 ] ; rtB . e15bejz3up = rtB . dl425wbmtl * rtB . mxh01l3oxv ; rtB .
iuugcbpoba = rtB . e15bejz3up * rtB . e15bejz3up ; rtB . mjl4eqimcj = rtB .
as3ppx3osb + rtB . iuugcbpoba ; rtB . dws5hbfuon = muDoubleScalarSqrt ( rtB .
mjl4eqimcj ) ; rtB . dhaf3rywmk = rtB . gwkrvtlqcl / rtB . dws5hbfuon ; rtB .
m5gtdau2t5 = muDoubleScalarCos ( rtB . janxwpkdcj [ 1 ] ) ; rtB . nyn1hlelu1
= rtB . lpgwkwyj4h [ 1 ] * rtB . m5gtdau2t5 ; rtB . mfs1yb5ala = rtB .
nyn1hlelu1 * rtB . mxh01l3oxv ; rtB . nymcd15stv = rtB . mfs1yb5ala * rtB .
mfs1yb5ala ; rtB . lw0k0vdv1k = muDoubleScalarSin ( rtB . janxwpkdcj [ 1 ] )
; rtB . g3cditrnls = rtB . lw0k0vdv1k * rtB . lpgwkwyj4h [ 1 ] ; rtB .
h2waz5uker = rtB . g3cditrnls * rtB . mxh01l3oxv ; rtB . jrd0anajsc = rtB .
h2waz5uker * rtB . h2waz5uker ; rtB . ixwpajgq5o = rtB . nymcd15stv + rtB .
jrd0anajsc ; rtB . knrgt0up3k = muDoubleScalarSqrt ( rtB . ixwpajgq5o ) ; rtB
. cunqmzmrnj = rtB . mfs1yb5ala / rtB . knrgt0up3k ; rtB . avpap4mulk = rtB .
dhaf3rywmk - rtB . cunqmzmrnj ; rtB . acwfkhjrmo = rtB . avpap4mulk * rtB .
avpap4mulk ; rtB . odsdlbo2ub = rtB . e15bejz3up / rtB . dws5hbfuon ; rtB .
akt2smreqe = rtB . h2waz5uker / rtB . knrgt0up3k ; rtB . crlnbcdvvi = rtB .
odsdlbo2ub - rtB . akt2smreqe ; rtB . moznelqki1 = rtB . crlnbcdvvi * rtB .
crlnbcdvvi ; rtB . arhtuz5gei = rtB . acwfkhjrmo + rtB . moznelqki1 ; rtB .
phj1wwj31u = rtB . arhtuz5gei * rtP . Constant3_Value_p15r2ob0kh ; rtB .
j3mne0m4wo = rtP . Constant2_Value_j4tqpjnn4o - rtB . phj1wwj31u ; u0 = rtB .
j3mne0m4wo ; if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = -
1.0 ; } } rtB . j011qqo5xf = muDoubleScalarAcos ( u0 ) ; rtB . mtsars0c4n =
rtB . odsdlbo2ub - rtB . cunqmzmrnj ; rtB . otrsh4tfqz = rtB . mtsars0c4n *
rtB . mtsars0c4n ; rtB . ionqrx5wpo = rtB . dhaf3rywmk + rtB . akt2smreqe ;
rtB . dfdjqbza1j = rtB . ionqrx5wpo * rtB . ionqrx5wpo ; rtB . e5ixznlru1 =
rtB . otrsh4tfqz + rtB . dfdjqbza1j ; rtB . bje3n5kzkt = ( rtB . e5ixznlru1 >
rtP . Constant_Value_lpqdg01lig ) ; if ( rtB . bje3n5kzkt ) { rtB .
bnbyyxshdi = rtB . j011qqo5xf * rtP . Constant1_Value_cwu5p3tfdv ; rtB .
m1u0vcm04l = rtB . bnbyyxshdi ; } else { rtB . m1u0vcm04l = rtB . j011qqo5xf
; } rtB . mfh4qmsnux = muDoubleScalarAbs ( rtB . m1u0vcm04l ) ; rtB .
ltwpbom1xd = rtB . mfs1yb5ala * rtB . mfs1yb5ala ; rtB . j3i5qt4d2v = rtB .
h2waz5uker * rtB . h2waz5uker ; rtB . loox1jnwbj = rtB . ltwpbom1xd + rtB .
j3i5qt4d2v ; rtB . gqyadlt1o2 = muDoubleScalarSqrt ( rtB . loox1jnwbj ) ; rtB
. g1jsz0svd2 = rtB . mfs1yb5ala / rtB . gqyadlt1o2 ; rtB . pjmx2wznz0 =
muDoubleScalarCos ( rtB . janxwpkdcj [ 2 ] ) ; rtB . c5dodwizkd = rtB .
lpgwkwyj4h [ 2 ] * rtB . pjmx2wznz0 ; rtB . ndskvs431z = rtB . c5dodwizkd *
rtB . mxh01l3oxv ; rtB . ffbfscpovn = rtB . ndskvs431z * rtB . ndskvs431z ;
rtB . hoch3yqbms = muDoubleScalarSin ( rtB . janxwpkdcj [ 2 ] ) ; rtB .
pnlxv4w3fb = rtB . hoch3yqbms * rtB . lpgwkwyj4h [ 2 ] ; rtB . jslzn4rlal =
rtB . pnlxv4w3fb * rtB . mxh01l3oxv ; rtB . hmpwt30k3v = rtB . jslzn4rlal *
rtB . jslzn4rlal ; rtB . e44yqi2vuo = rtB . ffbfscpovn + rtB . hmpwt30k3v ;
rtB . g4yafvmizj = muDoubleScalarSqrt ( rtB . e44yqi2vuo ) ; rtB . l2jm2rzed3
= rtB . ndskvs431z / rtB . g4yafvmizj ; rtB . aczbaejdn4 = rtB . g1jsz0svd2 -
rtB . l2jm2rzed3 ; rtB . mrhqkzsgmo = rtB . aczbaejdn4 * rtB . aczbaejdn4 ;
rtB . h4ijq2gsy0 = rtB . h2waz5uker / rtB . gqyadlt1o2 ; rtB . nmgvze3o4g =
rtB . jslzn4rlal / rtB . g4yafvmizj ; rtB . hu4ywbijdf = rtB . h4ijq2gsy0 -
rtB . nmgvze3o4g ; rtB . davgtakgoc = rtB . hu4ywbijdf * rtB . hu4ywbijdf ;
rtB . kufisjlqx4 = rtB . mrhqkzsgmo + rtB . davgtakgoc ; rtB . oyogyjdmit =
rtB . kufisjlqx4 * rtP . Constant3_Value_a432xdgvji ; rtB . ayk1ogl4g1 = rtP
. Constant2_Value_gkbov0tljg - rtB . oyogyjdmit ; u0 = rtB . ayk1ogl4g1 ; if
( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } rtB .
kgg1htqnle = muDoubleScalarAcos ( u0 ) ; rtB . m0x0xbvgdy = rtB . h4ijq2gsy0
- rtB . l2jm2rzed3 ; rtB . n3wzpknm2n = rtB . m0x0xbvgdy * rtB . m0x0xbvgdy ;
rtB . ljua4udgk1 = rtB . g1jsz0svd2 + rtB . nmgvze3o4g ; rtB . bokxqrj35o =
rtB . ljua4udgk1 * rtB . ljua4udgk1 ; rtB . dcsfwz0xcj = rtB . n3wzpknm2n +
rtB . bokxqrj35o ; rtB . hfil0cs4n1 = ( rtB . dcsfwz0xcj > rtP .
Constant_Value_ispmaaskql ) ; if ( rtB . hfil0cs4n1 ) { rtB . pfabg3ncpf =
rtB . kgg1htqnle * rtP . Constant1_Value_euh3mkud00 ; rtB . emsb21lsmq = rtB
. pfabg3ncpf ; } else { rtB . emsb21lsmq = rtB . kgg1htqnle ; } rtB .
jq1pdzvlif = muDoubleScalarAbs ( rtB . emsb21lsmq ) ; rtB . bke23ih1tg = rtB
. ndskvs431z * rtB . ndskvs431z ; rtB . ftscf4bcph = rtB . jslzn4rlal * rtB .
jslzn4rlal ; rtB . bybxorehml = rtB . bke23ih1tg + rtB . ftscf4bcph ; rtB .
dehs5whrek = muDoubleScalarSqrt ( rtB . bybxorehml ) ; rtB . jkxqwvtcwt = rtB
. ndskvs431z / rtB . dehs5whrek ; rtB . nqe01paofu = rtB . gwkrvtlqcl * rtB .
gwkrvtlqcl ; rtB . mnlnbey33m = rtB . e15bejz3up * rtB . e15bejz3up ; rtB .
gube3putr3 = rtB . nqe01paofu + rtB . mnlnbey33m ; rtB . c2ypjpvzo4 =
muDoubleScalarSqrt ( rtB . gube3putr3 ) ; rtB . nqlzfq2i40 = rtB . gwkrvtlqcl
/ rtB . c2ypjpvzo4 ; rtB . pfgtgxxk0z = rtB . jkxqwvtcwt - rtB . nqlzfq2i40 ;
rtB . gkdqs3ychu = rtB . pfgtgxxk0z * rtB . pfgtgxxk0z ; rtB . gizsevrud3 =
rtB . jslzn4rlal / rtB . dehs5whrek ; rtB . di4ofjxkcb = rtB . e15bejz3up /
rtB . c2ypjpvzo4 ; rtB . i5qwsnxvki = rtB . gizsevrud3 - rtB . di4ofjxkcb ;
rtB . cuuyxp4bgn = rtB . i5qwsnxvki * rtB . i5qwsnxvki ; rtB . asa1wc2kgt =
rtB . gkdqs3ychu + rtB . cuuyxp4bgn ; rtB . kdoqfqmayl = rtB . asa1wc2kgt *
rtP . Constant3_Value_hvchxcik5w ; rtB . nn2fntf3py = rtP .
Constant2_Value_ajhzpvjncg - rtB . kdoqfqmayl ; u0 = rtB . nn2fntf3py ; if (
u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } rtB .
gphirsl4w4 = muDoubleScalarAcos ( u0 ) ; rtB . brwaugeenz = rtB . gizsevrud3
- rtB . nqlzfq2i40 ; rtB . f5hvkkqvyc = rtB . brwaugeenz * rtB . brwaugeenz ;
rtB . akx3uf23oc = rtB . jkxqwvtcwt + rtB . di4ofjxkcb ; rtB . lhuko5xhxi =
rtB . akx3uf23oc * rtB . akx3uf23oc ; rtB . jklxt2sz4u = rtB . f5hvkkqvyc +
rtB . lhuko5xhxi ; rtB . jqtp2ikkoi = ( rtB . jklxt2sz4u > rtP .
Constant_Value_iuptmkkupz ) ; if ( rtB . jqtp2ikkoi ) { rtB . jcneork0s3 =
rtB . gphirsl4w4 * rtP . Constant1_Value_jjlct5micx ; rtB . a4hgbff0di = rtB
. jcneork0s3 ; } else { rtB . a4hgbff0di = rtB . gphirsl4w4 ; } rtB .
jzgwpuujef = muDoubleScalarAbs ( rtB . a4hgbff0di ) ; rtB . o1kdtbicxr = rtP
. u_Gain_mgxzntjveg * rtB . dxa1300d4c ; rtB . lknud4pjx4 = rtB . o1kdtbicxr
* rtB . o1kdtbicxr ; rtB . el5gocwdua = rtP . u_Gain_khftzbcqlz * rtB .
dgvytedolp ; rtB . eht0bzysty = rtB . el5gocwdua * rtB . el5gocwdua ; rtB .
bzz0r011ej = rtB . lknud4pjx4 + rtB . eht0bzysty ; rtB . bhw0t2emh3 =
muDoubleScalarSqrt ( rtB . bzz0r011ej ) ; rtB . px3zqey0mv = rtB . o1kdtbicxr
/ rtB . bhw0t2emh3 ; rtB . g4oj1odtzq = rtP . u_Gain_jsuhldhfm3 * rtB .
l2zahd4oox ; rtB . cy5pexiop0 = rtB . g4oj1odtzq * rtB . g4oj1odtzq ; rtB .
dlhvmvlj23 = rtP . u_Gain_chflnsajkh * rtB . g340u5g2f2 ; rtB . jelywmxyvg =
rtB . dlhvmvlj23 * rtB . dlhvmvlj23 ; rtB . bvrkhiq1xm = rtB . cy5pexiop0 +
rtB . jelywmxyvg ; rtB . ferlzqh4e2 = muDoubleScalarSqrt ( rtB . bvrkhiq1xm )
; rtB . owrp1evgj4 = rtB . g4oj1odtzq / rtB . ferlzqh4e2 ; rtB . bluir4qx2h =
rtB . px3zqey0mv - rtB . owrp1evgj4 ; rtB . fl5quft0gy = rtB . bluir4qx2h *
rtB . bluir4qx2h ; rtB . lrthc5i4af = rtB . el5gocwdua / rtB . bhw0t2emh3 ;
rtB . mgbn1kbio2 = rtB . dlhvmvlj23 / rtB . ferlzqh4e2 ; rtB . pe0m0332b1 =
rtB . lrthc5i4af - rtB . mgbn1kbio2 ; rtB . fepzomhptn = rtB . pe0m0332b1 *
rtB . pe0m0332b1 ; rtB . ai0ukvdajm = rtB . fl5quft0gy + rtB . fepzomhptn ;
rtB . aejqiecsyz = rtB . ai0ukvdajm * rtP . Constant3_Value_i3vvwqvogy ; rtB
. llnk52ku0s = rtP . Constant2_Value_cz0fzqwylc - rtB . aejqiecsyz ; u0 = rtB
. llnk52ku0s ; if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 =
- 1.0 ; } } rtB . cj5dlbtv2v = muDoubleScalarAcos ( u0 ) ; rtB . aa23t4g0sn =
rtB . lrthc5i4af - rtB . owrp1evgj4 ; rtB . ao5qetbkai = rtB . aa23t4g0sn *
rtB . aa23t4g0sn ; rtB . hiephyks1v = rtB . px3zqey0mv + rtB . mgbn1kbio2 ;
rtB . cds43aol3k = rtB . hiephyks1v * rtB . hiephyks1v ; rtB . bwasqr3ojt =
rtB . ao5qetbkai + rtB . cds43aol3k ; rtB . iuikyesnvh = ( rtB . bwasqr3ojt >
rtP . Constant_Value_ggrvreh0mv ) ; if ( rtB . iuikyesnvh ) { rtB .
buevswrvo3 = rtB . cj5dlbtv2v * rtP . Constant1_Value_hoa1ab1kyy ; rtB .
iblzty2is3 = rtB . buevswrvo3 ; } else { rtB . iblzty2is3 = rtB . cj5dlbtv2v
; } rtB . j2422bbaoh = muDoubleScalarAbs ( rtB . iblzty2is3 ) ; rtB .
ely22qicgr = rtB . g4oj1odtzq * rtB . g4oj1odtzq ; rtB . guztzkddop = rtB .
dlhvmvlj23 * rtB . dlhvmvlj23 ; rtB . mka4z1yjgb = rtB . ely22qicgr + rtB .
guztzkddop ; rtB . ma5fkkt0ud = muDoubleScalarSqrt ( rtB . mka4z1yjgb ) ; rtB
. kb3jt5aaqi = rtB . g4oj1odtzq / rtB . ma5fkkt0ud ; rtB . cehg1no30i = rtP .
u_Gain_intuu2dt5l * rtB . emlwggxmjw ; rtB . n05a4kl1hy = rtB . cehg1no30i *
rtB . cehg1no30i ; rtB . k2he2sid1b = rtP . u_Gain_en3vl5ugzv * rtB .
nubgwxsn52 ; rtB . fyr4hpz20g = rtB . k2he2sid1b * rtB . k2he2sid1b ; rtB .
jkqfveaxue = rtB . n05a4kl1hy + rtB . fyr4hpz20g ; rtB . acp11swqfy =
muDoubleScalarSqrt ( rtB . jkqfveaxue ) ; rtB . epf5voq14u = rtB . cehg1no30i
/ rtB . acp11swqfy ; rtB . i2p0e1j5iy = rtB . kb3jt5aaqi - rtB . epf5voq14u ;
rtB . eycclk2a4h = rtB . i2p0e1j5iy * rtB . i2p0e1j5iy ; rtB . h1nnhqxhzh =
rtB . dlhvmvlj23 / rtB . ma5fkkt0ud ; rtB . l4glz0kqvw = rtB . k2he2sid1b /
rtB . acp11swqfy ; rtB . avo25ih0im = rtB . h1nnhqxhzh - rtB . l4glz0kqvw ;
rtB . ptawgj5gz2 = rtB . avo25ih0im * rtB . avo25ih0im ; rtB . n5v4ejx5yw =
rtB . eycclk2a4h + rtB . ptawgj5gz2 ; rtB . mevqcom3um = rtB . n5v4ejx5yw *
rtP . Constant3_Value_jybd2gc3zg ; rtB . mavzf03024 = rtP .
Constant2_Value_itjmkfse51 - rtB . mevqcom3um ; u0 = rtB . mavzf03024 ; if (
u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } rtB .
h21jtkebpa = muDoubleScalarAcos ( u0 ) ; rtB . bq3pts0qqj = rtB . h1nnhqxhzh
- rtB . epf5voq14u ; rtB . p25g4nitcc = rtB . bq3pts0qqj * rtB . bq3pts0qqj ;
rtB . bz0sazh0uh = rtB . kb3jt5aaqi + rtB . l4glz0kqvw ; rtB . fmaxlpyxbi =
rtB . bz0sazh0uh * rtB . bz0sazh0uh ; rtB . gnyxclgpuy = rtB . p25g4nitcc +
rtB . fmaxlpyxbi ; rtB . mislvzstke = ( rtB . gnyxclgpuy > rtP .
Constant_Value_bneq44ebfy ) ; if ( rtB . mislvzstke ) { rtB . hzst5ckh2t =
rtB . h21jtkebpa * rtP . Constant1_Value_ak2jaxa5s4 ; rtB . p2ftj0jghb = rtB
. hzst5ckh2t ; } else { rtB . p2ftj0jghb = rtB . h21jtkebpa ; } rtB .
h2khnvzcrt = muDoubleScalarAbs ( rtB . p2ftj0jghb ) ; rtB . a5rigcnk2f = rtB
. cehg1no30i * rtB . cehg1no30i ; rtB . jdtuiif03k = rtB . k2he2sid1b * rtB .
k2he2sid1b ; rtB . bx2iglc5li = rtB . a5rigcnk2f + rtB . jdtuiif03k ; rtB .
gqwxcfuxjk = muDoubleScalarSqrt ( rtB . bx2iglc5li ) ; rtB . a1sdlw15cd = rtB
. cehg1no30i / rtB . gqwxcfuxjk ; rtB . crogpvt2v1 = rtB . o1kdtbicxr * rtB .
o1kdtbicxr ; rtB . bytp2t3svf = rtB . el5gocwdua * rtB . el5gocwdua ; rtB .
e5c3vn1iem = rtB . crogpvt2v1 + rtB . bytp2t3svf ; rtB . asdqsxgq1h =
muDoubleScalarSqrt ( rtB . e5c3vn1iem ) ; rtB . ehgwmerh10 = rtB . o1kdtbicxr
/ rtB . asdqsxgq1h ; rtB . jqg5y2labk = rtB . a1sdlw15cd - rtB . ehgwmerh10 ;
rtB . k2ijtg0aok = rtB . jqg5y2labk * rtB . jqg5y2labk ; rtB . ihyxpxsowb =
rtB . k2he2sid1b / rtB . gqwxcfuxjk ; rtB . ejadctrroy = rtB . el5gocwdua /
rtB . asdqsxgq1h ; rtB . p3aa35ywr3 = rtB . ihyxpxsowb - rtB . ejadctrroy ;
rtB . gfx5gbmecv = rtB . p3aa35ywr3 * rtB . p3aa35ywr3 ; rtB . gndcd2uef0 =
rtB . k2ijtg0aok + rtB . gfx5gbmecv ; rtB . ibsuazqgjh = rtB . gndcd2uef0 *
rtP . Constant3_Value_it4ighfyeo ; rtB . hmbri3a11p = rtP .
Constant2_Value_bclupbtznl - rtB . ibsuazqgjh ; u0 = rtB . hmbri3a11p ; if (
u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } rtB .
bjrsehd5jj = muDoubleScalarAcos ( u0 ) ; rtB . bo05nsf40d = rtB . ihyxpxsowb
- rtB . ehgwmerh10 ; rtB . krhuoa1g5h = rtB . bo05nsf40d * rtB . bo05nsf40d ;
rtB . bomfnrojdb = rtB . a1sdlw15cd + rtB . ejadctrroy ; rtB . mzuiajfxpj =
rtB . bomfnrojdb * rtB . bomfnrojdb ; rtB . a3nd420pra = rtB . krhuoa1g5h +
rtB . mzuiajfxpj ; rtB . jgpa2m00df = ( rtB . a3nd420pra > rtP .
Constant_Value_e23ssam310 ) ; if ( rtB . jgpa2m00df ) { rtB . nqedmozmth =
rtB . bjrsehd5jj * rtP . Constant1_Value_pdry0wyo5k ; rtB . lgrm5xv4yc = rtB
. nqedmozmth ; } else { rtB . lgrm5xv4yc = rtB . bjrsehd5jj ; } rtB .
oq0xf2edkh = muDoubleScalarAbs ( rtB . lgrm5xv4yc ) ; rtB . bnwncqbzmr = ! (
rtB . laxu4i4bvg != 0.0 ) ; rtB . mnxxkuhwiq = rtB . ctkxwfjqo4 * rtB .
ctkxwfjqo4 ; rtB . gucj51kojt = rtB . dl425wbmtl * rtB . dl425wbmtl ; rtB .
dxw1jc51zv = rtB . mnxxkuhwiq + rtB . gucj51kojt ; rtB . i3w5gkyrt1 =
muDoubleScalarSqrt ( rtB . dxw1jc51zv ) ; rtB . cm2lou5sl1 = ( rtB .
i3w5gkyrt1 > rtP . u9_Value_jq2z5q24ih ) ; rtB . gfi2t12qae = ! rtB .
cm2lou5sl1 ; rtB . msxl42idda = ( rtB . bnwncqbzmr && rtB . gfi2t12qae ) ;
rtB . mglsilzh3x = ( rtB . jq1pdzvlif < rtB . leqzt1thse ) ; rtB . gpudtvyabv
= ( rtB . h2khnvzcrt > rtB . hjyco0tz1d ) ; rtB . p2btwm34gn = ( rtB .
mglsilzh3x && rtB . gpudtvyabv ) ; rtB . gqxjfmyssp = ( rtB . jzgwpuujef <
rtB . leqzt1thse ) ; rtB . my1gjxe4uh = ( rtB . oq0xf2edkh > rtB . hjyco0tz1d
) ; rtB . jgrjnjzkin = ( rtB . gqxjfmyssp && rtB . my1gjxe4uh ) ; rtB .
ddpcokaenh = rtB . mfs1yb5ala * rtB . g4oj1odtzq ; rtB . jvyvqann12 = rtB .
h2waz5uker * rtB . dlhvmvlj23 ; rtB . gujsw24vok = rtB . ddpcokaenh + rtB .
jvyvqann12 ; rtB . bpcixclj4z = rtB . g4oj1odtzq * rtB . g4oj1odtzq ; rtB .
pl0da33t0c = rtB . dlhvmvlj23 * rtB . dlhvmvlj23 ; rtB . kllfzdrhpv = rtB .
bpcixclj4z + rtB . pl0da33t0c ; rtB . fh15z24jk5 = ( rtB . kllfzdrhpv < rtP .
Constant_Value_da043dikyd ) ; if ( rtB . fh15z24jk5 ) { rtB . exdag1zeju =
rtP . Constant_Value_da043dikyd ; } else { rtB . exdag1zeju = rtB .
kllfzdrhpv ; } rtB . iktmd1vai2 = rtB . gujsw24vok / rtB . exdag1zeju ; rtB .
judofspqgo = rtB . ndskvs431z * rtB . iktmd1vai2 ; rtB . lpcddqfec1 = rtB .
h2waz5uker * rtB . g4oj1odtzq ; rtB . kc5x1vhtiw = rtB . mfs1yb5ala * rtB .
dlhvmvlj23 ; rtB . dztmkswaqm = rtB . lpcddqfec1 - rtB . kc5x1vhtiw ; rtB .
bcxzdr2ruy = rtB . dztmkswaqm / rtB . exdag1zeju ; rtB . d2vremcodb = rtB .
jslzn4rlal * rtB . bcxzdr2ruy ; rtB . edddl0y5qr = rtB . judofspqgo + rtB .
d2vremcodb ; rtB . hqnoyltvzv = rtB . iktmd1vai2 * rtB . iktmd1vai2 ; rtB .
llyp313wtv = rtB . bcxzdr2ruy * rtB . bcxzdr2ruy ; rtB . kbsp1qc4bn = rtB .
hqnoyltvzv + rtB . llyp313wtv ; rtB . nnp1ptpczp = ( rtB . kbsp1qc4bn < rtP .
Constant_Value_ffxayquacq ) ; if ( rtB . nnp1ptpczp ) { rtB . nsca4ptlsd =
rtP . Constant_Value_ffxayquacq ; } else { rtB . nsca4ptlsd = rtB .
kbsp1qc4bn ; } rtB . b3umd25l4p = rtB . edddl0y5qr / rtB . nsca4ptlsd ; rtB .
phiqbb4dxq = rtB . cehg1no30i - rtB . b3umd25l4p ; rtB . nn4i1o4tft = rtB .
phiqbb4dxq * rtB . phiqbb4dxq ; rtB . mbv0b3e31k = rtB . jslzn4rlal * rtB .
iktmd1vai2 ; rtB . lsyvsb5t42 = rtB . ndskvs431z * rtB . bcxzdr2ruy ; rtB .
kg1c44y5zz = rtB . mbv0b3e31k - rtB . lsyvsb5t42 ; rtB . mx4uao5gb2 = rtB .
kg1c44y5zz / rtB . nsca4ptlsd ; rtB . mv1lwx2btd = rtB . k2he2sid1b - rtB .
mx4uao5gb2 ; rtB . p3grnc0aot = rtB . mv1lwx2btd * rtB . mv1lwx2btd ; rtB .
aki5yzp31s = rtB . nn4i1o4tft + rtB . p3grnc0aot ; rtB . mp5rkl4dwo =
muDoubleScalarSqrt ( rtB . aki5yzp31s ) ; rtB . j31evqnstt = rtB . b3umd25l4p
* rtB . b3umd25l4p ; rtB . njieaf40os = rtB . mx4uao5gb2 * rtB . mx4uao5gb2 ;
rtB . csfbw0wami = rtB . j31evqnstt + rtB . njieaf40os ; rtB . eryywjolgw =
muDoubleScalarSqrt ( rtB . csfbw0wami ) ; rtB . dscgbagnik = ( rtB .
mp5rkl4dwo > rtB . eryywjolgw ) ; rtB . ilhyz0wejn = rtB . gwkrvtlqcl * rtB .
iktmd1vai2 ; rtB . c4gw42s2si = rtB . e15bejz3up * rtB . bcxzdr2ruy ; rtB .
f5w4toa1lk = rtB . ilhyz0wejn + rtB . c4gw42s2si ; rtB . i5is5ouktr = rtB .
iktmd1vai2 * rtB . iktmd1vai2 ; rtB . gaodbyqhoz = rtB . bcxzdr2ruy * rtB .
bcxzdr2ruy ; rtB . irmnkht23b = rtB . i5is5ouktr + rtB . gaodbyqhoz ; rtB .
dtarqcaasw = ( rtB . irmnkht23b < rtP . Constant_Value_hl3wssouxd ) ; if (
rtB . dtarqcaasw ) { rtB . f4hpdozijv = rtP . Constant_Value_hl3wssouxd ; }
else { rtB . f4hpdozijv = rtB . irmnkht23b ; } rtB . bevw5ktagn = rtB .
f5w4toa1lk / rtB . f4hpdozijv ; rtB . di5sno0jdb = rtB . o1kdtbicxr - rtB .
bevw5ktagn ; rtB . erazd4nudj = rtB . di5sno0jdb * rtB . di5sno0jdb ; rtB .
h1v2ysz32i = rtB . e15bejz3up * rtB . iktmd1vai2 ; rtB . dg1zd4tq23 = rtB .
gwkrvtlqcl * rtB . bcxzdr2ruy ; rtB . gzssbk43um = rtB . h1v2ysz32i - rtB .
dg1zd4tq23 ; rtB . dbh2c3h11i = rtB . gzssbk43um / rtB . f4hpdozijv ; rtB .
conqwlvlbz = rtB . el5gocwdua - rtB . dbh2c3h11i ; rtB . iauwrxdfw2 = rtB .
conqwlvlbz * rtB . conqwlvlbz ; rtB . einguaxsc4 = rtB . erazd4nudj + rtB .
iauwrxdfw2 ; rtB . iwdkye3kc2 = muDoubleScalarSqrt ( rtB . einguaxsc4 ) ; rtB
. dqnxgefppn = rtB . bevw5ktagn * rtB . bevw5ktagn ; rtB . bjl52zoe01 = rtB .
dbh2c3h11i * rtB . dbh2c3h11i ; rtB . lwkjilzept = rtB . dqnxgefppn + rtB .
bjl52zoe01 ; rtB . eqohwjkcbv = muDoubleScalarSqrt ( rtB . lwkjilzept ) ; rtB
. mov1l2o4r1 = ( rtB . iwdkye3kc2 > rtB . eqohwjkcbv ) ; rtB . nqknbjjpbh = (
rtB . dscgbagnik && rtB . mov1l2o4r1 ) ; rtB . dbktfrf32o = ( rtB .
nqknbjjpbh && rtB . jgrjnjzkin ) ; rtB . a1bz4gmclb = rtB . gwkrvtlqcl * rtB
. o1kdtbicxr ; rtB . lt3k1qwu0x = rtB . e15bejz3up * rtB . el5gocwdua ; rtB .
ijudp1p04t = rtB . a1bz4gmclb + rtB . lt3k1qwu0x ; rtB . gts5plyxcy = rtB .
o1kdtbicxr * rtB . o1kdtbicxr ; rtB . od5lbeuioh = rtB . el5gocwdua * rtB .
el5gocwdua ; rtB . pst0c10dk3 = rtB . gts5plyxcy + rtB . od5lbeuioh ; rtB .
hq1vqmvisk = ( rtB . pst0c10dk3 < rtP . Constant_Value_njupksslf4 ) ; if (
rtB . hq1vqmvisk ) { rtB . httnx1gyow = rtP . Constant_Value_njupksslf4 ; }
else { rtB . httnx1gyow = rtB . pst0c10dk3 ; } rtB . gjxt5kxth0 = rtB .
ijudp1p04t / rtB . httnx1gyow ; rtB . ejjytcz010 = rtB . mfs1yb5ala * rtB .
gjxt5kxth0 ; rtB . gfzlgzvley = rtB . e15bejz3up * rtB . o1kdtbicxr ; rtB .
ni1lncprcf = rtB . gwkrvtlqcl * rtB . el5gocwdua ; rtB . m20ezfcdxk = rtB .
gfzlgzvley - rtB . ni1lncprcf ; rtB . dxm3bog3lb = rtB . m20ezfcdxk / rtB .
httnx1gyow ; rtB . nftnm0rvmj = rtB . h2waz5uker * rtB . dxm3bog3lb ; rtB .
habztkdp3g = rtB . ejjytcz010 + rtB . nftnm0rvmj ; rtB . e5telgzcw0 = rtB .
gjxt5kxth0 * rtB . gjxt5kxth0 ; rtB . n2jdfavb2c = rtB . dxm3bog3lb * rtB .
dxm3bog3lb ; rtB . i22c10af4m = rtB . e5telgzcw0 + rtB . n2jdfavb2c ; rtB .
eidvqkvi1j = ( rtB . i22c10af4m < rtP . Constant_Value_a3f34iggst ) ; if (
rtB . eidvqkvi1j ) { rtB . kkph4xqnuv = rtP . Constant_Value_a3f34iggst ; }
else { rtB . kkph4xqnuv = rtB . i22c10af4m ; } rtB . i4tcejfnfx = rtB .
habztkdp3g / rtB . kkph4xqnuv ; rtB . ogqq0h0jov = rtB . g4oj1odtzq - rtB .
i4tcejfnfx ; rtB . hmetn0254c = rtB . ogqq0h0jov * rtB . ogqq0h0jov ; rtB .
ouxnmmyfst = rtB . h2waz5uker * rtB . gjxt5kxth0 ; rtB . antldrphvs = rtB .
mfs1yb5ala * rtB . dxm3bog3lb ; rtB . f1mmrceyoe = rtB . ouxnmmyfst - rtB .
antldrphvs ; rtB . o4lsbhssx0 = rtB . f1mmrceyoe / rtB . kkph4xqnuv ; rtB .
ie1w4dxmh3 = rtB . dlhvmvlj23 - rtB . o4lsbhssx0 ; rtB . eldfhwnnpp = rtB .
ie1w4dxmh3 * rtB . ie1w4dxmh3 ; rtB . hcq25evi1e = rtB . hmetn0254c + rtB .
eldfhwnnpp ; rtB . eqg5i2bzpr = muDoubleScalarSqrt ( rtB . hcq25evi1e ) ; rtB
. a14zc4gvhz = rtB . i4tcejfnfx * rtB . i4tcejfnfx ; rtB . du4ftn0udg = rtB .
o4lsbhssx0 * rtB . o4lsbhssx0 ; rtB . dyvrbbcspz = rtB . a14zc4gvhz + rtB .
du4ftn0udg ; rtB . d4ljjfl0lb = muDoubleScalarSqrt ( rtB . dyvrbbcspz ) ; rtB
. kjyk2cqavo = ( rtB . eqg5i2bzpr > rtB . d4ljjfl0lb ) ; rtB . eekyn2vfrn =
rtB . ndskvs431z * rtB . gjxt5kxth0 ; rtB . ctxl1wi4re = rtB . jslzn4rlal *
rtB . dxm3bog3lb ; rtB . nanxunlg2t = rtB . eekyn2vfrn + rtB . ctxl1wi4re ;
rtB . bvk11srpxx = rtB . gjxt5kxth0 * rtB . gjxt5kxth0 ; rtB . a3lw55cops =
rtB . dxm3bog3lb * rtB . dxm3bog3lb ; rtB . hjbskuji4i = rtB . bvk11srpxx +
rtB . a3lw55cops ; rtB . eq3v5cmohj = ( rtB . hjbskuji4i < rtP .
Constant_Value_mua31cbw2d ) ; if ( rtB . eq3v5cmohj ) { rtB . bdr5se5u3m =
rtP . Constant_Value_mua31cbw2d ; } else { rtB . bdr5se5u3m = rtB .
hjbskuji4i ; } rtB . kxgkprn3zw = rtB . nanxunlg2t / rtB . bdr5se5u3m ; rtB .
gbqhyrnm21 = rtB . cehg1no30i - rtB . kxgkprn3zw ; rtB . bub2dansaj = rtB .
gbqhyrnm21 * rtB . gbqhyrnm21 ; rtB . hxcns2xwpd = rtB . jslzn4rlal * rtB .
gjxt5kxth0 ; rtB . mnj1ehi0xr = rtB . ndskvs431z * rtB . dxm3bog3lb ; rtB .
kqwed1gnht = rtB . hxcns2xwpd - rtB . mnj1ehi0xr ; rtB . mp2jrl2zbb = rtB .
kqwed1gnht / rtB . bdr5se5u3m ; rtB . ps3c2k0ugi = rtB . k2he2sid1b - rtB .
mp2jrl2zbb ; rtB . pd5vr2rfri = rtB . ps3c2k0ugi * rtB . ps3c2k0ugi ; rtB .
owomejuhor = rtB . bub2dansaj + rtB . pd5vr2rfri ; rtB . hx44yscnw2 =
muDoubleScalarSqrt ( rtB . owomejuhor ) ; rtB . eul2hrotnv = rtB . kxgkprn3zw
* rtB . kxgkprn3zw ; rtB . l0a0p3bjqy = rtB . mp2jrl2zbb * rtB . mp2jrl2zbb ;
rtB . ljarfggmfv = rtB . eul2hrotnv + rtB . l0a0p3bjqy ; rtB . pzhjjjhohv =
muDoubleScalarSqrt ( rtB . ljarfggmfv ) ; rtB . npevg0iicr = ( rtB .
hx44yscnw2 > rtB . pzhjjjhohv ) ; rtB . iyibsqujbx = ( rtB . kjyk2cqavo &&
rtB . npevg0iicr ) ; rtB . fnp15gx0ed = ( rtB . iyibsqujbx && rtB .
p2btwm34gn ) ; rtB . p4bhqe20ay = rtB . ndskvs431z * rtB . cehg1no30i ; rtB .
dj0afzbczw = rtB . jslzn4rlal * rtB . k2he2sid1b ; rtB . cllekgxx4p = rtB .
p4bhqe20ay + rtB . dj0afzbczw ; rtB . pqsjnzdkrm = rtB . cehg1no30i * rtB .
cehg1no30i ; rtB . pexal5xr2k = rtB . k2he2sid1b * rtB . k2he2sid1b ; rtB .
faassgnrak = rtB . pqsjnzdkrm + rtB . pexal5xr2k ; rtB . et5pfsclpj = ( rtB .
faassgnrak < rtP . Constant_Value_ms0r0iarr1 ) ; if ( rtB . et5pfsclpj ) {
rtB . ks2dmc4qk1 = rtP . Constant_Value_ms0r0iarr1 ; } else { rtB .
ks2dmc4qk1 = rtB . faassgnrak ; } rtB . ije1qeybmx = rtB . cllekgxx4p / rtB .
ks2dmc4qk1 ; rtB . p2izkz3e0c = rtB . gwkrvtlqcl * rtB . ije1qeybmx ; rtB .
eiwqhzykif = rtB . jslzn4rlal * rtB . cehg1no30i ; rtB . nwnh25ytwr = rtB .
ndskvs431z * rtB . k2he2sid1b ; rtB . ditwgmnou3 = rtB . eiwqhzykif - rtB .
nwnh25ytwr ; rtB . gfeic0d1c1 = rtB . ditwgmnou3 / rtB . ks2dmc4qk1 ; rtB .
ncpkor1sj5 = rtB . e15bejz3up * rtB . gfeic0d1c1 ; rtB . hvumiq3msk = rtB .
p2izkz3e0c + rtB . ncpkor1sj5 ; rtB . diqzlytnmi = rtB . ije1qeybmx * rtB .
ije1qeybmx ; rtB . izxgm33omv = rtB . gfeic0d1c1 * rtB . gfeic0d1c1 ; rtB .
gmfnovgbir = rtB . diqzlytnmi + rtB . izxgm33omv ; rtB . onotlqmhaf = ( rtB .
gmfnovgbir < rtP . Constant_Value_malsqfejcv ) ; if ( rtB . onotlqmhaf ) {
rtB . mfcat5xkv3 = rtP . Constant_Value_malsqfejcv ; } else { rtB .
mfcat5xkv3 = rtB . gmfnovgbir ; } rtB . g02j1zranp = rtB . hvumiq3msk / rtB .
mfcat5xkv3 ; rtB . jkeuge2msz = rtB . o1kdtbicxr - rtB . g02j1zranp ; rtB .
pszp0ax2hq = rtB . jkeuge2msz * rtB . jkeuge2msz ; rtB . j41ikbxp3n = rtB .
e15bejz3up * rtB . ije1qeybmx ; rtB . erluxf1p5s = rtB . gwkrvtlqcl * rtB .
gfeic0d1c1 ; rtB . bcolz44bu1 = rtB . j41ikbxp3n - rtB . erluxf1p5s ; rtB .
cembztftld = rtB . bcolz44bu1 / rtB . mfcat5xkv3 ; rtB . h1kr2itk43 = rtB .
el5gocwdua - rtB . cembztftld ; rtB . b5tk5iusph = rtB . h1kr2itk43 * rtB .
h1kr2itk43 ; rtB . n22cdl4yst = rtB . pszp0ax2hq + rtB . b5tk5iusph ; rtB .
otrvrksn4h = muDoubleScalarSqrt ( rtB . n22cdl4yst ) ; rtB . a2fj3kyfpj = rtB
. g02j1zranp * rtB . g02j1zranp ; rtB . gqbdthvnzh = rtB . cembztftld * rtB .
cembztftld ; rtB . jsjovixp1d = rtB . a2fj3kyfpj + rtB . gqbdthvnzh ; rtB .
n5aj11awcc = muDoubleScalarSqrt ( rtB . jsjovixp1d ) ; rtB . k2w2sat3gp = (
rtB . otrvrksn4h > rtB . n5aj11awcc ) ; rtB . o5sqfhss4b = rtB . mfs1yb5ala *
rtB . ije1qeybmx ; rtB . ituvukurlt = rtB . h2waz5uker * rtB . gfeic0d1c1 ;
rtB . a30eo1u1gr = rtB . o5sqfhss4b + rtB . ituvukurlt ; rtB . m24yudla0u =
rtB . ije1qeybmx * rtB . ije1qeybmx ; rtB . kbzz3y0zap = rtB . gfeic0d1c1 *
rtB . gfeic0d1c1 ; rtB . fulilctwl0 = rtB . m24yudla0u + rtB . kbzz3y0zap ;
rtB . ld0lwikrlh = ( rtB . fulilctwl0 < rtP . Constant_Value_mivnnr4osa ) ;
if ( rtB . ld0lwikrlh ) { rtB . kjjvkitgax = rtP . Constant_Value_mivnnr4osa
; } else { rtB . kjjvkitgax = rtB . fulilctwl0 ; } rtB . amhrpcpsyi = rtB .
a30eo1u1gr / rtB . kjjvkitgax ; rtB . ot3en3053n = rtB . g4oj1odtzq - rtB .
amhrpcpsyi ; rtB . icx2vrurnm = rtB . ot3en3053n * rtB . ot3en3053n ; rtB .
nmwjcwf5og = rtB . h2waz5uker * rtB . ije1qeybmx ; rtB . ozczviqqpf = rtB .
mfs1yb5ala * rtB . gfeic0d1c1 ; rtB . fwwddkkwfl = rtB . nmwjcwf5og - rtB .
ozczviqqpf ; rtB . cvinyqvkci = rtB . fwwddkkwfl / rtB . kjjvkitgax ; rtB .
hx2pligg5m = rtB . dlhvmvlj23 - rtB . cvinyqvkci ; rtB . oebuhovcq5 = rtB .
hx2pligg5m * rtB . hx2pligg5m ; rtB . d2shqp1rqx = rtB . icx2vrurnm + rtB .
oebuhovcq5 ; rtB . p4bjn3cctq = muDoubleScalarSqrt ( rtB . d2shqp1rqx ) ; rtB
. jah2sthcjl = rtB . amhrpcpsyi * rtB . amhrpcpsyi ; rtB . nwlvf35ee0 = rtB .
cvinyqvkci * rtB . cvinyqvkci ; rtB . hjmiqczueh = rtB . jah2sthcjl + rtB .
nwlvf35ee0 ; rtB . h3nkfmozwo = muDoubleScalarSqrt ( rtB . hjmiqczueh ) ; rtB
. dzzdjakfyl = ( rtB . p4bjn3cctq > rtB . h3nkfmozwo ) ; rtB . mkyvq0pkjs = (
rtB . k2w2sat3gp && rtB . dzzdjakfyl ) ; rtB . cxkblf3rdg = ( rtB .
mfh4qmsnux < rtB . leqzt1thse ) ; rtB . ljxmkxxaxf = ( rtB . j2422bbaoh > rtB
. hjyco0tz1d ) ; rtB . mwvhia2ltj = ( rtB . cxkblf3rdg && rtB . ljxmkxxaxf )
; rtB . adg5uub1za = ( rtB . mkyvq0pkjs && rtB . mwvhia2ltj ) ; rtB .
pxelivoug3 = ! ( rtB . nuqqjqizth != 0.0 ) ; rtB . p4lyaiyx1w = rtB .
nyn1hlelu1 * rtB . nyn1hlelu1 ; rtB . p5b5ezokct = rtB . g3cditrnls * rtB .
g3cditrnls ; rtB . fobwcyahzy = rtB . p4lyaiyx1w + rtB . p5b5ezokct ; rtB .
ifiqai1xdf = muDoubleScalarSqrt ( rtB . fobwcyahzy ) ; rtB . kdch3loprk = (
rtB . ifiqai1xdf > rtP . u9_Value_jq2z5q24ih ) ; rtB . hb1rfm0pb5 = ! rtB .
kdch3loprk ; rtB . f1ly1uhbiu = ( rtB . pxelivoug3 && rtB . hb1rfm0pb5 ) ;
rtB . dfu0yusmqe = ! ( rtB . mfzwdgo525 != 0.0 ) ; rtB . ev3qvmcv1g = rtB .
c5dodwizkd * rtB . c5dodwizkd ; rtB . dsm1zbfp1p = rtB . pnlxv4w3fb * rtB .
pnlxv4w3fb ; rtB . okl3mizi0u = rtB . ev3qvmcv1g + rtB . dsm1zbfp1p ; rtB .
iapp3jgapy = muDoubleScalarSqrt ( rtB . okl3mizi0u ) ; rtB . jqpqhta4xx = (
rtB . iapp3jgapy > rtP . u9_Value_jq2z5q24ih ) ; rtB . g251b40410 = ! rtB .
jqpqhta4xx ; rtB . ebtenxgvbf = ( rtB . dfu0yusmqe && rtB . g251b40410 ) ;
rtB . e1sj4el3c0 = rtB . gwkrvtlqcl * rtB . gwkrvtlqcl ; rtB . mplbjdnhan =
rtB . e15bejz3up * rtB . e15bejz3up ; rtB . oygdv5o52z = rtB . e1sj4el3c0 +
rtB . mplbjdnhan ; rtB . gtojktqz0g = muDoubleScalarSqrt ( rtB . oygdv5o52z )
; rtB . lfzhtnoagh = rtB . mfs1yb5ala * rtB . mfs1yb5ala ; rtB . cv4wvzy5ia =
rtB . h2waz5uker * rtB . h2waz5uker ; rtB . iqc5gaue4b = rtB . lfzhtnoagh +
rtB . cv4wvzy5ia ; rtB . fc2nj11hz4 = muDoubleScalarSqrt ( rtB . iqc5gaue4b )
; rtB . bn4mtmuriu = rtB . ndskvs431z * rtB . ndskvs431z ; rtB . kg0oxtq50n =
rtB . jslzn4rlal * rtB . jslzn4rlal ; rtB . fkcemiqlk4 = rtB . bn4mtmuriu +
rtB . kg0oxtq50n ; rtB . i0cyhg0oww = muDoubleScalarSqrt ( rtB . fkcemiqlk4 )
; rtB . gy23aazqre = ( rtB . gtojktqz0g < rtP . Constant3_Value ) ; rtB .
mqm55oeyym = ( rtB . fc2nj11hz4 < rtP . Constant3_Value ) ; rtB . ps1ghpilk2
= ( rtB . i0cyhg0oww < rtP . Constant3_Value ) ; rtB . jnxasrgzg5 = rtB .
el5gocwdua * rtP . Constant_Value_cqd2l4dryt ; rtB . j1ajlqlznl = rtB .
gwkrvtlqcl * rtB . o1kdtbicxr ; rtB . as3buzbame = rtB . e15bejz3up * rtB .
jnxasrgzg5 ; rtB . pqk03m5fxo = rtB . j1ajlqlznl - rtB . as3buzbame ; rtB .
h342uroefh = rtB . e15bejz3up * rtB . o1kdtbicxr ; rtB . eq2w1jes4t = rtB .
gwkrvtlqcl * rtB . jnxasrgzg5 ; rtB . cxu02wittz = rtB . h342uroefh + rtB .
eq2w1jes4t ; rtB . hcct1ax2du = rtB . cxu02wittz * rtB . monh4vwypq ; rtB .
d2txbucqqt = rtB . pqk03m5fxo * rtB . g2t4aege51 ; rtB . e1q4tvjvux = rtB .
hcct1ax2du + rtB . d2txbucqqt ; rtB . fq2dsnxdrm = rtB . dlhvmvlj23 * rtP .
Constant_Value_dr2yldx3l2 ; rtB . hm4lo0ttob = rtB . mfs1yb5ala * rtB .
g4oj1odtzq ; rtB . mqamwel1th = rtB . h2waz5uker * rtB . fq2dsnxdrm ; rtB .
cxp00rifyv = rtB . hm4lo0ttob - rtB . mqamwel1th ; rtB . owpfdm5oci = rtB .
h2waz5uker * rtB . g4oj1odtzq ; rtB . gklcutdzqq = rtB . mfs1yb5ala * rtB .
fq2dsnxdrm ; rtB . jf02zaxg5b = rtB . owpfdm5oci + rtB . gklcutdzqq ; rtB .
f1rbumctbp = rtB . jf02zaxg5b * rtB . bwwkqjxo4k ; rtB . ljwurlgta0 = rtB .
cxp00rifyv * rtB . hzhlqhdkme ; rtB . byohyry2q1 = rtB . f1rbumctbp + rtB .
ljwurlgta0 ; rtB . dszvlmxun0 = rtB . k2he2sid1b * rtP .
Constant_Value_gftpyuopnc ; rtB . p4i5uhfolz = rtB . ndskvs431z * rtB .
cehg1no30i ; rtB . czrrdda52s = rtB . jslzn4rlal * rtB . dszvlmxun0 ; rtB .
mo505f03vo = rtB . p4i5uhfolz - rtB . czrrdda52s ; rtB . bdkzuys4pa = rtB .
jslzn4rlal * rtB . cehg1no30i ; rtB . apeyncwth0 = rtB . ndskvs431z * rtB .
dszvlmxun0 ; rtB . hxylzwdybz = rtB . bdkzuys4pa + rtB . apeyncwth0 ; rtB .
dg0panmjiz = rtB . hxylzwdybz * rtB . exnsgi2eie ; rtB . jyg3ixff0c = rtB .
mo505f03vo * rtB . pq0gfxrlwz ; rtB . feoqfem1it = rtB . dg0panmjiz + rtB .
jyg3ixff0c ; rtB . g1q30rcfid = rtB . ndskvs431z * rtB . phiqbb4dxq ; rtB .
gsdbpn2d43 = rtB . mv1lwx2btd * rtP . Constant_Value_a2wlau000l ; rtB .
aiuukwovw1 = rtB . jslzn4rlal * rtB . gsdbpn2d43 ; rtB . d4p1hwb2lv = rtB .
g1q30rcfid - rtB . aiuukwovw1 ; rtB . jwsbrfevrm = ( rtB . d4p1hwb2lv > rtP .
Constant4_Value_htsomw3bzw ) ; rtB . kj5wcuac4x = rtB . d4p1hwb2lv * rtB .
d4p1hwb2lv ; rtB . pftd0h4d5l = rtB . jslzn4rlal * rtB . phiqbb4dxq ; rtB .
c1w5yf10k4 = rtB . ndskvs431z * rtB . gsdbpn2d43 ; rtB . gssktru04k = rtB .
pftd0h4d5l + rtB . c1w5yf10k4 ; rtB . cf01y3x3db = rtB . gssktru04k * rtP .
Constant1_Value_cxp2ftcb3n ; rtB . fqhvpmp24j = rtB . gssktru04k * rtB .
cf01y3x3db ; rtB . bnombbyjs0 = rtB . kj5wcuac4x + rtB . fqhvpmp24j ; rtB .
elbjz0niuv = ( rtB . bnombbyjs0 > rtP . Constant5_Value_m1nnses1kf ) ; if (
rtP . Constant9_Value > rtP . Switch_Threshold_fhqctw22qj ) { rtB .
hxk1trgnsz = rtB . elbjz0niuv ; } else { rtB . hxk1trgnsz = rtP .
Constant10_Value ; } rtB . cs5nqsjr1m = ( rtB . jwsbrfevrm && ( rtB .
hxk1trgnsz != 0.0 ) ) ; rtB . lfioqehgdo = ( rtB . gssktru04k > rtP .
Constant4_Value_htsomw3bzw ) ; if ( rtP . Constant9_Value > rtP .
Switch1_Threshold_hlgoppug15 ) { rtB . lo1w1iueup = ! rtB . elbjz0niuv ; rtB
. pppl5cdi30 = rtB . lo1w1iueup ; } else { rtB . pppl5cdi30 = rtP .
Constant9_Value ; } rtB . h4alum5xlb = ( rtB . lfioqehgdo && ( rtB .
pppl5cdi30 != 0.0 ) ) ; rtB . ns21rbrvo1 = ( rtB . cs5nqsjr1m || rtB .
h4alum5xlb ) ; rtB . pcvcieqmu5 = rtB . ndskvs431z * rtB . cehg1no30i ; rtB .
nd4sdrzbrp = rtB . k2he2sid1b * rtP . Constant_Value_oij4djiqbq ; rtB .
cbvqtvlyhz = rtB . jslzn4rlal * rtB . nd4sdrzbrp ; rtB . apse3h0i52 = rtB .
pcvcieqmu5 - rtB . cbvqtvlyhz ; rtB . gtuj3jjtqd = ( rtB . apse3h0i52 > rtP .
Constant4_Value_iyz5aqjekt ) ; rtB . a1q55mhfua = rtB . apse3h0i52 * rtB .
apse3h0i52 ; rtB . ay4as1evz1 = rtB . jslzn4rlal * rtB . cehg1no30i ; rtB .
b0nk45bana = rtB . ndskvs431z * rtB . nd4sdrzbrp ; rtB . bqimmxl4xg = rtB .
ay4as1evz1 + rtB . b0nk45bana ; rtB . ppwh3ekknf = rtB . bqimmxl4xg * rtP .
Constant1_Value_jdaa1at4zx ; rtB . hsvwraqqqv = rtB . bqimmxl4xg * rtB .
ppwh3ekknf ; rtB . pbtvfiqau5 = rtB . a1q55mhfua + rtB . hsvwraqqqv ; rtB .
ab0n5zj2rj = ( rtB . pbtvfiqau5 > rtP . Constant5_Value_i2sqj0wsqc ) ; if (
rtB . ps1ghpilk2 ) { rtB . iyq3vjxziw = rtB . ab0n5zj2rj ; rtB . b4qas5z2d4 =
! rtB . ab0n5zj2rj ; rtB . eomslokzpj = rtB . b4qas5z2d4 ; } else { rtB .
iyq3vjxziw = rtP . Constant4_Value ; rtB . eomslokzpj = rtP . Constant5_Value
; } rtB . mkkxhjcbbv = ( rtB . gtuj3jjtqd && ( rtB . iyq3vjxziw != 0.0 ) ) ;
rtB . iemv55r5pt = ( rtB . bqimmxl4xg > rtP . Constant4_Value_iyz5aqjekt ) ;
rtB . cs2yjwyrww = ( rtB . iemv55r5pt && ( rtB . eomslokzpj != 0.0 ) ) ; rtB
. ppmhlameik = ( rtB . mkkxhjcbbv || rtB . cs2yjwyrww ) ; rtB . crnszgnatu =
rtB . mfs1yb5ala * rtB . g4oj1odtzq ; rtB . bhfufo3eju = rtB . dlhvmvlj23 *
rtP . Constant_Value_j4dloyxxkx ; rtB . kiok5sr024 = rtB . h2waz5uker * rtB .
bhfufo3eju ; rtB . kval0t5cwe = rtB . crnszgnatu - rtB . kiok5sr024 ; rtB .
jlayydicso = ( rtB . kval0t5cwe > rtP . Constant4_Value_da1khoz5y0 ) ; rtB .
altvrvti1m = rtB . kval0t5cwe * rtB . kval0t5cwe ; rtB . cdpyag4z4z = rtB .
h2waz5uker * rtB . g4oj1odtzq ; rtB . p3qg5n5ra4 = rtB . mfs1yb5ala * rtB .
bhfufo3eju ; rtB . e4xuo0zjhe = rtB . cdpyag4z4z + rtB . p3qg5n5ra4 ; rtB .
da02wokg23 = rtB . e4xuo0zjhe * rtP . Constant1_Value_n55alcnsfs ; rtB .
c323kmcm0n = rtB . e4xuo0zjhe * rtB . da02wokg23 ; rtB . iu1ve4ho5l = rtB .
altvrvti1m + rtB . c323kmcm0n ; rtB . nquxncr0vi = ( rtB . iu1ve4ho5l > rtP .
Constant5_Value_nqnvvpwhd0 ) ; if ( rtB . mqm55oeyym ) { rtB . bifz3o4k4s =
rtB . nquxncr0vi ; rtB . gz1lzhn4p1 = ! rtB . nquxncr0vi ; rtB . gh2tnga5gd =
rtB . gz1lzhn4p1 ; } else { rtB . bifz3o4k4s = rtP . Constant4_Value ; rtB .
gh2tnga5gd = rtP . Constant5_Value ; } rtB . e4w1h0cg4g = ( rtB . jlayydicso
&& ( rtB . bifz3o4k4s != 0.0 ) ) ; rtB . lnm45nalax = ( rtB . e4xuo0zjhe >
rtP . Constant4_Value_da1khoz5y0 ) ; rtB . e0fnwup1d4 = ( rtB . lnm45nalax &&
( rtB . gh2tnga5gd != 0.0 ) ) ; rtB . hyqq5unkiw = ( rtB . e4w1h0cg4g || rtB
. e0fnwup1d4 ) ; rtB . nwzkqfw0gq = rtB . gwkrvtlqcl * rtB . jkeuge2msz ; rtB
. h2tv1kxhoj = rtB . h1kr2itk43 * rtP . Constant_Value_fxzcd1lgc5 ; rtB .
fbdx0v3oev = rtB . e15bejz3up * rtB . h2tv1kxhoj ; rtB . izqqoplqj5 = rtB .
nwzkqfw0gq - rtB . fbdx0v3oev ; rtB . e2byo0m3g1 = ( rtB . izqqoplqj5 > rtP .
Constant4_Value_cgcaiq1mux ) ; rtB . jgfoljjamy = rtB . izqqoplqj5 * rtB .
izqqoplqj5 ; rtB . mdfduvmwti = rtB . e15bejz3up * rtB . jkeuge2msz ; rtB .
kuhsbc2sof = rtB . gwkrvtlqcl * rtB . h2tv1kxhoj ; rtB . grqmgghesb = rtB .
mdfduvmwti + rtB . kuhsbc2sof ; rtB . p5e04snt2q = rtB . grqmgghesb * rtP .
Constant1_Value_juv0ipzq31 ; rtB . pksodekdnr = rtB . grqmgghesb * rtB .
p5e04snt2q ; rtB . ly0tbdxer1 = rtB . jgfoljjamy + rtB . pksodekdnr ; rtB .
gimcvwhw1c = ( rtB . ly0tbdxer1 > rtP . Constant5_Value_j2kuvmf2gt ) ; if (
rtP . Constant9_Value > rtP . Switch_Threshold_dw4r4f31ct ) { rtB .
dq3llw5zvo = rtB . gimcvwhw1c ; } else { rtB . dq3llw5zvo = rtP .
Constant10_Value ; } rtB . l3p4scpbsw = ( rtB . e2byo0m3g1 && ( rtB .
dq3llw5zvo != 0.0 ) ) ; rtB . nhlbhnk5x5 = ( rtB . grqmgghesb > rtP .
Constant4_Value_cgcaiq1mux ) ; if ( rtP . Constant9_Value > rtP .
Switch1_Threshold_f5d15kuwcl ) { rtB . mufyi3ww3a = ! rtB . gimcvwhw1c ; rtB
. fynm1drryh = rtB . mufyi3ww3a ; } else { rtB . fynm1drryh = rtP .
Constant9_Value ; } rtB . nq3jbc0ntz = ( rtB . nhlbhnk5x5 && ( rtB .
fynm1drryh != 0.0 ) ) ; rtB . fftz0qpgj2 = ( rtB . l3p4scpbsw || rtB .
nq3jbc0ntz ) ; rtB . mg0n4vqbbt = rtB . mfs1yb5ala * rtB . ogqq0h0jov ; rtB .
lf542uvzu4 = rtB . ie1w4dxmh3 * rtP . Constant_Value_b22uew4lyx ; rtB .
bo1encjc4u = rtB . h2waz5uker * rtB . lf542uvzu4 ; rtB . jcvvsle0na = rtB .
mg0n4vqbbt - rtB . bo1encjc4u ; rtB . fzsxeaxjw3 = ( rtB . jcvvsle0na > rtP .
Constant4_Value_nmeqgqsunk ) ; rtB . jjctwoxiho = rtB . jcvvsle0na * rtB .
jcvvsle0na ; rtB . jra0ztqeao = rtB . h2waz5uker * rtB . ogqq0h0jov ; rtB .
aeejimxmuz = rtB . mfs1yb5ala * rtB . lf542uvzu4 ; rtB . nbvungg3nb = rtB .
jra0ztqeao + rtB . aeejimxmuz ; rtB . l4trei5rag = rtB . nbvungg3nb * rtP .
Constant1_Value_lzvqoxc3q5 ; rtB . h3cftp45vh = rtB . nbvungg3nb * rtB .
l4trei5rag ; rtB . m4tjfp5v2m = rtB . jjctwoxiho + rtB . h3cftp45vh ; rtB .
or4q0jpbtb = ( rtB . m4tjfp5v2m > rtP . Constant5_Value_n0b30ph3lq ) ; if (
rtP . Constant9_Value > rtP . Switch_Threshold_gficgwjci4 ) { rtB .
cy1dzhumun = rtB . or4q0jpbtb ; } else { rtB . cy1dzhumun = rtP .
Constant10_Value ; } rtB . gkolroewxz = ( rtB . fzsxeaxjw3 && ( rtB .
cy1dzhumun != 0.0 ) ) ; rtB . lmj0tilpah = ( rtB . nbvungg3nb > rtP .
Constant4_Value_nmeqgqsunk ) ; if ( rtP . Constant9_Value > rtP .
Switch1_Threshold_peek5novam ) { rtB . hkgjlgqc2k = ! rtB . or4q0jpbtb ; rtB
. j2eggum03d = rtB . hkgjlgqc2k ; } else { rtB . j2eggum03d = rtP .
Constant9_Value ; } rtB . nqcwrkn21u = ( rtB . lmj0tilpah && ( rtB .
j2eggum03d != 0.0 ) ) ; rtB . lr5onp42zd = ( rtB . gkolroewxz || rtB .
nqcwrkn21u ) ; rtB . bzbe3yqppk = rtB . gwkrvtlqcl * rtB . o1kdtbicxr ; rtB .
gkxxnvncwg = rtB . el5gocwdua * rtP . Constant_Value_mv1g4sapcf ; rtB .
owon4lybh4 = rtB . e15bejz3up * rtB . gkxxnvncwg ; rtB . isb4bnsigr = rtB .
bzbe3yqppk - rtB . owon4lybh4 ; rtB . cpejquvdzs = ( rtB . isb4bnsigr > rtP .
Constant4_Value_gmxflc5yqi ) ; rtB . owd1eomvjf = rtB . isb4bnsigr * rtB .
isb4bnsigr ; rtB . bmlgfxadym = rtB . e15bejz3up * rtB . o1kdtbicxr ; rtB .
ncb2pebuf3 = rtB . gwkrvtlqcl * rtB . gkxxnvncwg ; rtB . ib0uxkawqw = rtB .
bmlgfxadym + rtB . ncb2pebuf3 ; rtB . ngmcm53lej = rtB . ib0uxkawqw * rtP .
Constant1_Value_eiz3u10hj3 ; rtB . ju5hwqbmjj = rtB . ib0uxkawqw * rtB .
ngmcm53lej ; rtB . jtr22twk4v = rtB . owd1eomvjf + rtB . ju5hwqbmjj ; rtB .
j1dwevfgsp = ( rtB . jtr22twk4v > rtP . Constant5_Value_lntfguxgcm ) ; if (
rtB . gy23aazqre ) { rtB . azhbo3lskk = rtB . j1dwevfgsp ; rtB . mjq3opmu3k =
! rtB . j1dwevfgsp ; rtB . hspcxeqrwy = rtB . mjq3opmu3k ; } else { rtB .
azhbo3lskk = rtP . Constant4_Value ; rtB . hspcxeqrwy = rtP . Constant5_Value
; } rtB . mqmrxnc4id = ( rtB . cpejquvdzs && ( rtB . azhbo3lskk != 0.0 ) ) ;
rtB . hmo44sjs5u = ( rtB . ib0uxkawqw > rtP . Constant4_Value_gmxflc5yqi ) ;
rtB . hucsn5sx0c = ( rtB . hmo44sjs5u && ( rtB . hspcxeqrwy != 0.0 ) ) ; rtB
. bp5dczs1z0 = ( rtB . mqmrxnc4id || rtB . hucsn5sx0c ) ; if ( rtB .
ebtenxgvbf ) { rtB . ox4taqtsfn = rtP . Constant11_Value ; } else { if ( rtP
. Constant_Value_ahcgpcz1zs > rtP . Switch9_Threshold ) { rtB . bpvdlbasf4 =
rtB . hxylzwdybz * rtB . pq0gfxrlwz ; rtB . emrb1wep02 = rtB . mo505f03vo *
rtB . exnsgi2eie ; rtB . g50h0wom4k = rtB . emrb1wep02 - rtB . bpvdlbasf4 ;
rtB . dx0o4bb511 = ( rtB . g50h0wom4k > rtP . Constant2_Value ) ; rtB .
htahkmipr5 = ! rtB . dx0o4bb511 ; if ( rtB . htahkmipr5 ) { rtB . lhqylyhifw
= ( rtB . g50h0wom4k < rtB . ii5d3e3cbj ) ; rtB . aks15xl4lu = ! rtB .
lhqylyhifw ; rtB . ne1zhq3kus = rtB . aks15xl4lu ; } else { rtB . ne1zhq3kus
= rtB . dx0o4bb511 ; } rtB . o12egvora2 = rtB . ne1zhq3kus ; } else { if (
rtB . adg5uub1za ) { rtB . i5fauupew3 = rtB . ppmhlameik ; } else { if ( rtB
. fnp15gx0ed ) { rtB . mnqlathc1p = rtB . lr5onp42zd ; } else { if ( rtB .
dbktfrf32o ) { rtB . obey5li0lx = rtB . ns21rbrvo1 ; } else { rtB .
obey5li0lx = rtB . ppmhlameik ; } rtB . mnqlathc1p = rtB . obey5li0lx ; } rtB
. i5fauupew3 = rtB . mnqlathc1p ; } rtB . o12egvora2 = rtB . i5fauupew3 ; }
rtB . ox4taqtsfn = rtB . o12egvora2 ; } if ( rtB . f1ly1uhbiu ) { rtB .
nmuuyrrny0 = rtP . Constant11_Value ; } else { if ( rtP .
Constant_Value_ahcgpcz1zs > rtP . Switch10_Threshold ) { rtB . b3zyppbkuo =
rtB . jf02zaxg5b * rtB . hzhlqhdkme ; rtB . jrm00uqjet = rtB . cxp00rifyv *
rtB . bwwkqjxo4k ; rtB . bvmyj4avkw = rtB . jrm00uqjet - rtB . b3zyppbkuo ;
rtB . m1o51ziozj = ( rtB . bvmyj4avkw > rtP . Constant2_Value ) ; rtB .
obwszaty1j = ! rtB . m1o51ziozj ; if ( rtB . obwszaty1j ) { rtB . m1mkfsw3ow
= ( rtB . bvmyj4avkw < rtB . l0bpjfzzyt ) ; rtB . ew2f0wg0bu = ! rtB .
m1mkfsw3ow ; rtB . luqfx4hapz = rtB . ew2f0wg0bu ; } else { rtB . luqfx4hapz
= rtB . m1o51ziozj ; } rtB . b1q5pl01bk = rtB . luqfx4hapz ; } else { if (
rtB . adg5uub1za ) { rtB . llyyrzi54o = rtB . fftz0qpgj2 ; } else { if ( rtB
. fnp15gx0ed ) { rtB . dhzdhhhklc = rtB . lr5onp42zd ; } else { rtB .
bllvgtzdee = rtB . hyqq5unkiw ; rtB . dhzdhhhklc = rtB . bllvgtzdee ; } rtB .
llyyrzi54o = rtB . dhzdhhhklc ; } rtB . b1q5pl01bk = rtB . llyyrzi54o ; } rtB
. nmuuyrrny0 = rtB . b1q5pl01bk ; } if ( rtB . msxl42idda ) { rtB .
k1gomebxbf = rtP . Constant11_Value ; } else { if ( rtP .
Constant_Value_ahcgpcz1zs > rtP . Switch11_Threshold ) { rtB . e5ygwn5qkc =
rtB . cxu02wittz * rtB . g2t4aege51 ; rtB . oyqnfuqawa = rtB . pqk03m5fxo *
rtB . monh4vwypq ; rtB . nbc4t1j4wr = rtB . oyqnfuqawa - rtB . e5ygwn5qkc ;
rtB . dvd3zfidpn = ( rtB . nbc4t1j4wr > rtP . Constant2_Value ) ; rtB .
j1gko1wfdz = ! rtB . dvd3zfidpn ; if ( rtB . j1gko1wfdz ) { rtB . frmumb24qb
= ( rtB . nbc4t1j4wr < rtB . pyj5nx5nsh ) ; rtB . dsbldlxyw1 = ! rtB .
frmumb24qb ; rtB . jk0nroy035 = rtB . dsbldlxyw1 ; } else { rtB . jk0nroy035
= rtB . dvd3zfidpn ; } rtB . pvr0dynbr4 = rtB . jk0nroy035 ; } else { if (
rtB . adg5uub1za ) { rtB . cpqub53zfq = rtB . fftz0qpgj2 ; } else { if ( rtB
. fnp15gx0ed ) { rtB . o1oyjvciem = rtB . bp5dczs1z0 ; } else { if ( rtB .
dbktfrf32o ) { rtB . jgh3mitmn5 = rtB . ns21rbrvo1 ; } else { rtB .
jgh3mitmn5 = rtB . bp5dczs1z0 ; } rtB . o1oyjvciem = rtB . jgh3mitmn5 ; } rtB
. cpqub53zfq = rtB . o1oyjvciem ; } rtB . pvr0dynbr4 = rtB . cpqub53zfq ; }
rtB . k1gomebxbf = rtB . pvr0dynbr4 ; } rtB . i4qnqlcjm1 = rtB . jkeuge2msz *
rtB . jkeuge2msz ; rtB . k1ishexmax = rtB . h1kr2itk43 * rtB . h1kr2itk43 ;
rtB . mt213a3bsy = rtB . i4qnqlcjm1 + rtB . k1ishexmax ; rtB . egm20z3njk =
muDoubleScalarSqrt ( rtB . mt213a3bsy ) ; rtB . cuodpr53af = rtB . jkeuge2msz
/ rtB . egm20z3njk ; u0 = rtB . cuodpr53af ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } rtB . flvphtprom =
muDoubleScalarAcos ( u0 ) ; rtB . b42yhnezta = ( rtB . h1kr2itk43 > rtP .
u_Value ) ; if ( rtB . b42yhnezta ) { rtB . hrsqxeaqun = rtB . flvphtprom ; }
else { rtB . cxsrboi0wx = rtB . flvphtprom * rtP . u54_Value ; rtB .
hrsqxeaqun = rtB . cxsrboi0wx ; } rtB . fovikmn0zg = rtB . ot3en3053n * rtB .
ot3en3053n ; rtB . anxplmhmbw = rtB . hx2pligg5m * rtB . hx2pligg5m ; rtB .
p5gku4tqhc = rtB . fovikmn0zg + rtB . anxplmhmbw ; rtB . ei1go5qh0s =
muDoubleScalarSqrt ( rtB . p5gku4tqhc ) ; rtB . gwcz1fxuvs = rtB . ot3en3053n
/ rtB . ei1go5qh0s ; u0 = rtB . gwcz1fxuvs ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } rtB . cmawqeeo2d =
muDoubleScalarAcos ( u0 ) ; rtB . kijvsdmqln = ( rtB . hx2pligg5m > rtP .
u_Value_mtltnaxxxg ) ; if ( rtB . kijvsdmqln ) { rtB . jy1d4xdvuw = rtB .
cmawqeeo2d ; } else { rtB . jtnit0kcz1 = rtB . cmawqeeo2d * rtP .
u54_Value_ndhe4l1aqm ; rtB . jy1d4xdvuw = rtB . jtnit0kcz1 ; } rtB .
ju12s0fqrl = rtB . g02j1zranp * rtB . g02j1zranp ; rtB . cw34ok3llk = rtB .
cembztftld * rtB . cembztftld ; rtB . ghkaksutow = rtB . ju12s0fqrl + rtB .
cw34ok3llk ; rtB . jxeyx54tmi = muDoubleScalarSqrt ( rtB . ghkaksutow ) ; rtB
. i4fylx1ict = rtB . g02j1zranp / rtB . jxeyx54tmi ; u0 = rtB . i4fylx1ict ;
if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } }
rtB . hlwbhfxuhj = muDoubleScalarAcos ( u0 ) ; rtB . cvgao2hqh5 = ( rtB .
cembztftld > rtP . u_Value_fqmack5dmf ) ; if ( rtB . cvgao2hqh5 ) { rtB .
mspagvm42p = rtB . hlwbhfxuhj ; } else { rtB . eumit4snd2 = rtB . hlwbhfxuhj
* rtP . u54_Value_f0f0csbzia ; rtB . mspagvm42p = rtB . eumit4snd2 ; } rtB .
gjwtrg1cho = rtB . amhrpcpsyi * rtB . amhrpcpsyi ; rtB . hnmfielhwy = rtB .
cvinyqvkci * rtB . cvinyqvkci ; rtB . bucixtlyct = rtB . gjwtrg1cho + rtB .
hnmfielhwy ; rtB . pxzeufmue5 = muDoubleScalarSqrt ( rtB . bucixtlyct ) ; rtB
. nlf5aol3ix = rtB . amhrpcpsyi / rtB . pxzeufmue5 ; u0 = rtB . nlf5aol3ix ;
if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } }
rtB . kr30cmykef = muDoubleScalarAcos ( u0 ) ; rtB . lp0f4nv510 = ( rtB .
cvinyqvkci > rtP . u_Value_czjihromkk ) ; if ( rtB . lp0f4nv510 ) { rtB .
g31k4kii1z = rtB . kr30cmykef ; } else { rtB . bwtk3bmrjp = rtB . kr30cmykef
* rtP . u54_Value_dj3nkxwgkk ; rtB . g31k4kii1z = rtB . bwtk3bmrjp ; } rtB .
lccdqbvgw5 = rtB . ogqq0h0jov * rtB . ogqq0h0jov ; rtB . mnoci5h5xy = rtB .
ie1w4dxmh3 * rtB . ie1w4dxmh3 ; rtB . mh5xd0vutw = rtB . lccdqbvgw5 + rtB .
mnoci5h5xy ; rtB . jkqsfsvylo = muDoubleScalarSqrt ( rtB . mh5xd0vutw ) ; rtB
. pfux12jcml = rtB . ogqq0h0jov / rtB . jkqsfsvylo ; u0 = rtB . pfux12jcml ;
if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } }
rtB . ie4xjx04ma = muDoubleScalarAcos ( u0 ) ; rtB . mjow1xlmkj = ( rtB .
ie1w4dxmh3 > rtP . u_Value_coqkroabrk ) ; if ( rtB . mjow1xlmkj ) { rtB .
hzg5ha2cmx = rtB . ie4xjx04ma ; } else { rtB . i5llctpcw4 = rtB . ie4xjx04ma
* rtP . u54_Value_bclaxbhuej ; rtB . hzg5ha2cmx = rtB . i5llctpcw4 ; } rtB .
f2nzxrurcm = rtB . gbqhyrnm21 * rtB . gbqhyrnm21 ; rtB . mh22dirxab = rtB .
ps3c2k0ugi * rtB . ps3c2k0ugi ; rtB . bbi3yxizjz = rtB . f2nzxrurcm + rtB .
mh22dirxab ; rtB . gbakgcb0jb = muDoubleScalarSqrt ( rtB . bbi3yxizjz ) ; rtB
. lttwfohmsg = rtB . gbqhyrnm21 / rtB . gbakgcb0jb ; u0 = rtB . lttwfohmsg ;
if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } }
rtB . ln2at1tnrk = muDoubleScalarAcos ( u0 ) ; rtB . px5tslso5c = ( rtB .
ps3c2k0ugi > rtP . u_Value_lfzckdh0nt ) ; if ( rtB . px5tslso5c ) { rtB .
bppzpmi41z = rtB . ln2at1tnrk ; } else { rtB . npcatywnko = rtB . ln2at1tnrk
* rtP . u54_Value_hmk41fnsp5 ; rtB . bppzpmi41z = rtB . npcatywnko ; } rtB .
pjclzrxoy0 = rtB . i4tcejfnfx * rtB . i4tcejfnfx ; rtB . f5ygjhz2gg = rtB .
o4lsbhssx0 * rtB . o4lsbhssx0 ; rtB . cz2crhz4qd = rtB . pjclzrxoy0 + rtB .
f5ygjhz2gg ; rtB . ivwvvefqmf = muDoubleScalarSqrt ( rtB . cz2crhz4qd ) ; rtB
. cmgfigboeb = rtB . i4tcejfnfx / rtB . ivwvvefqmf ; u0 = rtB . cmgfigboeb ;
if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } }
rtB . nb1ezq4iy3 = muDoubleScalarAcos ( u0 ) ; rtB . onapztng0v = ( rtB .
o4lsbhssx0 > rtP . u_Value_nvcylmgtmc ) ; if ( rtB . onapztng0v ) { rtB .
dz5eu4kkyz = rtB . nb1ezq4iy3 ; } else { rtB . ldfpyte55c = rtB . nb1ezq4iy3
* rtP . u54_Value_dgteeiynac ; rtB . dz5eu4kkyz = rtB . ldfpyte55c ; } rtB .
ak1bqhfo43 = rtB . kxgkprn3zw * rtB . kxgkprn3zw ; rtB . a0vr041gpb = rtB .
mp2jrl2zbb * rtB . mp2jrl2zbb ; rtB . a1f4nkjkvg = rtB . ak1bqhfo43 + rtB .
a0vr041gpb ; rtB . kkemzlgvbq = muDoubleScalarSqrt ( rtB . a1f4nkjkvg ) ; rtB
. a4f2darl0p = rtB . kxgkprn3zw / rtB . kkemzlgvbq ; u0 = rtB . a4f2darl0p ;
if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } }
rtB . djxt1tairr = muDoubleScalarAcos ( u0 ) ; rtB . hoxjgsi5z3 = ( rtB .
mp2jrl2zbb > rtP . u_Value_pksuqf0knp ) ; if ( rtB . hoxjgsi5z3 ) { rtB .
e254sfl5bv = rtB . djxt1tairr ; } else { rtB . m10hwrna3u = rtB . djxt1tairr
* rtP . u54_Value_e303jerhu2 ; rtB . e254sfl5bv = rtB . m10hwrna3u ; } rtB .
kvcu1043kd = rtB . phiqbb4dxq * rtB . phiqbb4dxq ; rtB . ht1miflcne = rtB .
mv1lwx2btd * rtB . mv1lwx2btd ; rtB . lido4kkfyj = rtB . kvcu1043kd + rtB .
ht1miflcne ; rtB . g52qnfcput = muDoubleScalarSqrt ( rtB . lido4kkfyj ) ; rtB
. bvxobo0djv = rtB . phiqbb4dxq / rtB . g52qnfcput ; u0 = rtB . bvxobo0djv ;
if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } }
rtB . kc0qvlbyyu = muDoubleScalarAcos ( u0 ) ; rtB . au3c2mi0q4 = ( rtB .
mv1lwx2btd > rtP . u_Value_fzjtqwjmtj ) ; if ( rtB . au3c2mi0q4 ) { rtB .
fzxsymanpp = rtB . kc0qvlbyyu ; } else { rtB . bzgnzvcqiz = rtB . kc0qvlbyyu
* rtP . u54_Value_dmciet5ssa ; rtB . fzxsymanpp = rtB . bzgnzvcqiz ; } rtB .
g0wx4rrzfd = rtB . di5sno0jdb * rtB . di5sno0jdb ; rtB . f2wmx30yzf = rtB .
conqwlvlbz * rtB . conqwlvlbz ; rtB . dk01dxxieu = rtB . g0wx4rrzfd + rtB .
f2wmx30yzf ; rtB . hfaygchxmd = muDoubleScalarSqrt ( rtB . dk01dxxieu ) ; rtB
. la3zu35gfy = rtB . di5sno0jdb / rtB . hfaygchxmd ; u0 = rtB . la3zu35gfy ;
if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } }
rtB . lwgszstpuz = muDoubleScalarAcos ( u0 ) ; rtB . e01dqx1dcw = ( rtB .
conqwlvlbz > rtP . u_Value_o42mmer3fa ) ; if ( rtB . e01dqx1dcw ) { rtB .
hxkhlgd5bn = rtB . lwgszstpuz ; } else { rtB . gxinej00nl = rtB . lwgszstpuz
* rtP . u54_Value_ddkuqhkniu ; rtB . hxkhlgd5bn = rtB . gxinej00nl ; } rtB .
etxeag0u53 = rtB . b3umd25l4p * rtB . b3umd25l4p ; rtB . hymspvfvt1 = rtB .
mx4uao5gb2 * rtB . mx4uao5gb2 ; rtB . lfxlkuwz4r = rtB . etxeag0u53 + rtB .
hymspvfvt1 ; rtB . jrynr44gqr = muDoubleScalarSqrt ( rtB . lfxlkuwz4r ) ; rtB
. hfzck5d2o0 = rtB . b3umd25l4p / rtB . jrynr44gqr ; u0 = rtB . hfzck5d2o0 ;
if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } }
rtB . jkx4ojsgrw = muDoubleScalarAcos ( u0 ) ; rtB . gixq35cf2b = ( rtB .
mx4uao5gb2 > rtP . u_Value_bwcwuzg4ki ) ; if ( rtB . gixq35cf2b ) { rtB .
buczduazlg = rtB . jkx4ojsgrw ; } else { rtB . jdutady1lg = rtB . jkx4ojsgrw
* rtP . u54_Value_lqottjw2bm ; rtB . buczduazlg = rtB . jdutady1lg ; } rtB .
flwlixheqx = rtB . bevw5ktagn * rtB . bevw5ktagn ; rtB . c2n2ks5id1 = rtB .
dbh2c3h11i * rtB . dbh2c3h11i ; rtB . lowmoceazw = rtB . flwlixheqx + rtB .
c2n2ks5id1 ; rtB . blvhuhvu4r = muDoubleScalarSqrt ( rtB . lowmoceazw ) ; rtB
. oxli2m1qs0 = rtB . bevw5ktagn / rtB . blvhuhvu4r ; u0 = rtB . oxli2m1qs0 ;
if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } }
rtB . kv41022zy2 = muDoubleScalarAcos ( u0 ) ; rtB . eoulf0uosw = ( rtB .
dbh2c3h11i > rtP . u_Value_bnwv3xgd3l ) ; if ( rtB . eoulf0uosw ) { rtB .
lrintb5xw4 = rtB . kv41022zy2 ; } else { rtB . jscidq52i2 = rtB . kv41022zy2
* rtP . u54_Value_je4tdti4ib ; rtB . lrintb5xw4 = rtB . jscidq52i2 ; } rtB .
gecc44w2th = muDoubleScalarCos ( rtB . iblzty2is3 ) ; rtB . mv3zk55je5 =
muDoubleScalarSin ( rtB . iblzty2is3 ) ; rtB . k1uq4d1ikz = muDoubleScalarCos
( rtB . p2ftj0jghb ) ; rtB . bfygnrmwne = muDoubleScalarSin ( rtB .
p2ftj0jghb ) ; rtB . k4px0bftfi = muDoubleScalarCos ( rtB . lgrm5xv4yc ) ;
rtB . m4gzqztdbe = muDoubleScalarSin ( rtB . lgrm5xv4yc ) ; rtB . mmamgsu4gd
= muDoubleScalarCos ( rtB . m1u0vcm04l ) ; rtB . ewv2p44iji =
muDoubleScalarSin ( rtB . m1u0vcm04l ) ; rtB . d14i0qplod = muDoubleScalarCos
( rtB . emsb21lsmq ) ; rtB . gdj43yxqrl = muDoubleScalarSin ( rtB .
emsb21lsmq ) ; rtB . jmz1uydhce = muDoubleScalarCos ( rtB . a4hgbff0di ) ;
rtB . co55rxva3i = muDoubleScalarSin ( rtB . a4hgbff0di ) ; rtB . nhajf2dmv2
= rtB . gwkrvtlqcl - rtB . jh3sbazso2 ; rtB . peowiqxqdr = rtB . nhajf2dmv2 *
rtB . nhajf2dmv2 ; rtB . jcycyhbdhd = rtB . e15bejz3up - rtB . hhqlxrrv2w ;
rtB . cfkeh1sml3 = rtB . jcycyhbdhd * rtB . jcycyhbdhd ; rtB . ldi1f3rm3f =
rtB . peowiqxqdr + rtB . cfkeh1sml3 ; rtB . kupcrex1z1 = muDoubleScalarSqrt (
rtB . ldi1f3rm3f ) ; rtB . lxrxlbh45t = rtB . gwkrvtlqcl * rtB . gwkrvtlqcl ;
rtB . aeoxvqngzp = rtB . e15bejz3up * rtB . e15bejz3up ; rtB . gj4riizeoh =
rtB . lxrxlbh45t + rtB . aeoxvqngzp ; rtB . am5tmrnrs1 = muDoubleScalarSqrt (
rtB . gj4riizeoh ) ; maxV = muDoubleScalarMax ( rtB . am5tmrnrs1 , rtB .
npvaqugt30 ) ; rtB . adjoh52prf = maxV ; rtB . e052pyoy0j = rtB . kupcrex1z1
/ rtB . adjoh52prf ; rtB . kadzhmgkem = ( rtB . e052pyoy0j > rtP . u5_Value )
; rtB . nv1e0te4um = ( rtB . cm2lou5sl1 && rtB . kdch3loprk ) ; rtB .
khv3hg4zvd = ! rtB . nv1e0te4um ; rtB . oszicpkovw = ( rtB . kadzhmgkem &&
rtB . khv3hg4zvd ) ; rtB . erd3hbqylm = ( rtB . am5tmrnrs1 < rtB . npvaqugt30
) ; rtB . gqvoxgrd3f = ( rtB . oszicpkovw && rtB . erd3hbqylm ) ; rtB .
ms24p32s0h = ! rtB . erd3hbqylm ; rtB . fxscjvhmff = ( rtB . oszicpkovw &&
rtB . ms24p32s0h ) ; rtB . gfauzqetux = ( rtB . gqvoxgrd3f || rtB .
fxscjvhmff ) ; rtB . mttgs4gunk = ! rtB . gfauzqetux ; rtB . iw5dxoclm1 = rtB
. mfs1yb5ala - rtB . an3nprh2tn ; rtB . euhoafqyea = rtB . iw5dxoclm1 * rtB .
iw5dxoclm1 ; rtB . ae30o011ds = rtB . h2waz5uker - rtB . cza42ci4ff ; rtB .
jn2xgr23wp = rtB . ae30o011ds * rtB . ae30o011ds ; rtB . m0ztf4b0gt = rtB .
euhoafqyea + rtB . jn2xgr23wp ; rtB . bkp2jvbvzf = muDoubleScalarSqrt ( rtB .
m0ztf4b0gt ) ; rtB . jwh1vuvxwc = rtB . mfs1yb5ala * rtB . mfs1yb5ala ; rtB .
ko4oxwqi14 = rtB . h2waz5uker * rtB . h2waz5uker ; rtB . bbfjcw1k1i = rtB .
jwh1vuvxwc + rtB . ko4oxwqi14 ; rtB . mlm1yo23cy = muDoubleScalarSqrt ( rtB .
bbfjcw1k1i ) ; maxV = muDoubleScalarMax ( rtB . mlm1yo23cy , rtB . iefdfszoxf
) ; rtB . bqluxhssh3 = maxV ; rtB . guzzipjci0 = rtB . bkp2jvbvzf / rtB .
bqluxhssh3 ; rtB . gupm311rxs = ( rtB . guzzipjci0 > rtP . u5_Value ) ; rtB .
drzirxoxjb = ( rtB . jqpqhta4xx && rtB . hlazbfj0dg ) ; rtB . cjuokmxrwd = !
rtB . drzirxoxjb ; rtB . le3yzn0vyr = ( rtB . gupm311rxs && rtB . cjuokmxrwd
) ; rtB . flwbzp25vf = ( rtB . mlm1yo23cy < rtB . iefdfszoxf ) ; rtB .
kasoxtgvi4 = ( rtB . le3yzn0vyr && rtB . flwbzp25vf ) ; rtB . fkh2nid3sm = !
rtB . flwbzp25vf ; rtB . hfbhreh3w5 = ( rtB . le3yzn0vyr && rtB . fkh2nid3sm
) ; rtB . bgl54sdqio = ( rtB . kasoxtgvi4 || rtB . hfbhreh3w5 ) ; rtB .
i3ekbwnncg = ! rtB . bgl54sdqio ; rtB . g2rgceequ3 = rtB . ndskvs431z - rtB .
od4ri1yxy5 ; rtB . kq1uvyeoar = rtB . g2rgceequ3 * rtB . g2rgceequ3 ; rtB .
liib01ws0n = rtB . jslzn4rlal - rtB . m3tfc0xulv ; rtB . mwezg4q524 = rtB .
liib01ws0n * rtB . liib01ws0n ; rtB . nbvqx0lowf = rtB . kq1uvyeoar + rtB .
mwezg4q524 ; rtB . kv2vg5mcw3 = muDoubleScalarSqrt ( rtB . nbvqx0lowf ) ; rtB
. nmugnpx3az = rtB . ndskvs431z * rtB . ndskvs431z ; rtB . f4nvr1njt0 = rtB .
jslzn4rlal * rtB . jslzn4rlal ; rtB . bdacbxiwrf = rtB . nmugnpx3az + rtB .
f4nvr1njt0 ; rtB . dik0borl0q = muDoubleScalarSqrt ( rtB . bdacbxiwrf ) ;
maxV = muDoubleScalarMax ( rtB . dik0borl0q , rtB . m2octsfxob ) ; rtB .
ijuhmtmt5j = maxV ; rtB . oa1ikoqgzo = rtB . kv2vg5mcw3 / rtB . ijuhmtmt5j ;
rtB . b2t3cil1tz = ( rtB . oa1ikoqgzo > rtP . u5_Value ) ; rtB . f4u10vg5ff =
( rtB . b2t3cil1tz && rtB . l1fjmhdjgk ) ; rtB . h2z4bgzbhu = ( rtB .
dik0borl0q < rtB . m2octsfxob ) ; rtB . d3xbj21iz0 = ( rtB . f4u10vg5ff &&
rtB . h2z4bgzbhu ) ; rtB . j5hio4pbwa = ! rtB . h2z4bgzbhu ; rtB . cthl33fyaf
= ( rtB . f4u10vg5ff && rtB . j5hio4pbwa ) ; rtB . cze4gvr5jp = ( rtB .
d3xbj21iz0 || rtB . cthl33fyaf ) ; rtB . me4wdw4baw = ! rtB . cze4gvr5jp ;
rtB . pis3gc4yag = ( rtB . mttgs4gunk && rtB . i3ekbwnncg && rtB . me4wdw4baw
) ; rtB . o5345qrmk1 = ! rtB . cm2lou5sl1 ; rtB . ookpgnwplo = ! rtB .
kdch3loprk ; rtB . gbvslu5g4c = ! rtB . jqpqhta4xx ; rtB . ablnwxvasp = ( rtB
. o5345qrmk1 && rtB . ookpgnwplo && rtB . gbvslu5g4c ) ; rtB . bgyjzohyvq = (
rtB . ablnwxvasp && rtB . poyb3errh1 ) ; rtB . nijvsalqrr = ( rtB .
pis3gc4yag && rtB . bgyjzohyvq ) ; rtB . abolnonekh = ( rtB . lq0oe5npsb &&
rtB . nijvsalqrr ) ; rtB . ifpn4nn1d3 = ! rtB . nijvsalqrr ; rtB . ovhkvv0ngd
= false ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . ijsinv2ra3 = rtDW .
nyqadzpf4p ; rtB . g0fjsdmtt2 = rtDW . crlu0ruqew ; rtB . frtk3tzlyc = ! rtB
. g0fjsdmtt2 ; } rtB . n2rrxuchjs = ( rtB . gfauzqetux && rtB . bgl54sdqio &&
rtB . cze4gvr5jp ) ; rtB . i04sdtdfnb = rtB . nhajf2dmv2 - rtB . iw5dxoclm1 ;
rtB . opxdhiauoe = rtB . i04sdtdfnb * rtB . i04sdtdfnb ; rtB . hatky13h3b =
rtB . jcycyhbdhd - rtB . ae30o011ds ; rtB . kf0ireoyur = rtB . hatky13h3b *
rtB . hatky13h3b ; rtB . nvxu01hbx4 = rtB . opxdhiauoe + rtB . kf0ireoyur ;
rtB . i3531hjj2o = muDoubleScalarSqrt ( rtB . nvxu01hbx4 ) ; maxV =
muDoubleScalarMax ( rtB . kupcrex1z1 , rtB . bkp2jvbvzf ) ; maxV =
muDoubleScalarMax ( maxV , rtB . kv2vg5mcw3 ) ; rtB . lfi1mzzmg0 = maxV ; rtB
. eiz1ac3s0g = rtB . i3531hjj2o / rtB . lfi1mzzmg0 ; rtB . jkkv2dw23v = ( rtB
. eiz1ac3s0g < rtP . u9_Value ) ; rtB . kioxobqacb = rtB . iw5dxoclm1 - rtB .
g2rgceequ3 ; rtB . bfyjwk2ekj = rtB . kioxobqacb * rtB . kioxobqacb ; rtB .
gbj5mlhnkv = rtB . ae30o011ds - rtB . liib01ws0n ; rtB . cxuogczkn1 = rtB .
gbj5mlhnkv * rtB . gbj5mlhnkv ; rtB . knvdcuyzdu = rtB . bfyjwk2ekj + rtB .
cxuogczkn1 ; rtB . pa0ypakbk5 = muDoubleScalarSqrt ( rtB . knvdcuyzdu ) ; rtB
. cxwgz32bji = rtB . pa0ypakbk5 / rtB . lfi1mzzmg0 ; rtB . lfd0vnuzst = ( rtB
. cxwgz32bji < rtP . u9_Value ) ; rtB . h2hdesd4ao = rtB . g2rgceequ3 - rtB .
nhajf2dmv2 ; rtB . egv3xdq5xe = rtB . h2hdesd4ao * rtB . h2hdesd4ao ; rtB .
jn5f30clvf = rtB . liib01ws0n - rtB . jcycyhbdhd ; rtB . imm1iomc3c = rtB .
jn5f30clvf * rtB . jn5f30clvf ; rtB . bfg0j4n2eh = rtB . egv3xdq5xe + rtB .
imm1iomc3c ; rtB . ecipn0nwog = muDoubleScalarSqrt ( rtB . bfg0j4n2eh ) ; rtB
. nptedjrce2 = rtB . ecipn0nwog / rtB . lfi1mzzmg0 ; rtB . bueyl03gip = ( rtB
. nptedjrce2 < rtP . u9_Value ) ; rtB . aordyc12dx = ( rtB . jkkv2dw23v &&
rtB . lfd0vnuzst && rtB . bueyl03gip ) ; rtB . ityxpqsryc = ( rtB .
n2rrxuchjs && rtB . aordyc12dx ) ; rtB . abljdeg3mo = ( rtB . e052pyoy0j >
rtP . u8_Value ) ; rtB . n0e3mhznne = ! rtB . ityxpqsryc ; rtB . d5tugdifak =
( rtB . abljdeg3mo && rtB . n0e3mhznne ) ; rtB . lr5wvxyka5 = ( rtB .
guzzipjci0 > rtP . u8_Value ) ; rtB . oquj203fn0 = ! rtB . ityxpqsryc ; rtB .
oprmv50ov4 = ( rtB . lr5wvxyka5 && rtB . oquj203fn0 ) ; rtB . fx2wlnpkmm = (
rtB . oa1ikoqgzo > rtP . u8_Value ) ; rtB . ecxjcl4pxm = ! rtB . ityxpqsryc ;
rtB . grwan5nfqm = ( rtB . fx2wlnpkmm && rtB . ecxjcl4pxm ) ; rtB .
mx22wffr1c = ( rtB . ityxpqsryc || rtB . d5tugdifak || rtB . oprmv50ov4 ||
rtB . grwan5nfqm ) ; rtB . ncoqcmpnrk = false ; rtB . btcrna1zns = ( rtB .
ncoqcmpnrk && rtB . frtk3tzlyc ) ; rtB . ij1blfmcix = ( rtB . ijsinv2ra3 ||
rtB . btcrna1zns ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . ifeyep53qk
= rtDW . k1pkt3do0q ; rtB . lwhvquifwt = ! rtB . ifeyep53qk ; rtB .
pkohe4o45j = rtDW . puvpzy5tok ; rtB . enxeqn2z2q = ! rtB . pkohe4o45j ; }
rtB . et10b34fee = ( rtB . nijvsalqrr && rtB . lwhvquifwt ) ; rtB .
kcopdsn0gj = ! rtB . et10b34fee ; rtB . pgc5xttltd = ( rtB . ij1blfmcix &&
rtB . kcopdsn0gj ) ; rtB . klze2p54da = ( rtB . pgc5xttltd || rtB .
a5udxoo0de || rtB . ovhkvv0ngd ) ; rtB . ojotb4rusk = ( rtB . pgc5xttltd &&
rtB . enxeqn2z2q ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . conq4okzy2
= rtDW . ci14pxvpr2 ; rtB . ol4qowp1e5 = rtDW . jaz3uqfgz2 ; rtB . dqjfuokugx
= ! rtB . ol4qowp1e5 ; } rtB . bugfy5pxuf = ! rtB . hfbhreh3w5 ; rtB .
l3wkd25ebx = ( rtB . fxscjvhmff && rtB . bugfy5pxuf && rtB . cthl33fyaf ) ;
rtB . gpqpnrrizf = ! rtB . fxscjvhmff ; rtB . dcsnr5tt20 = ( rtB . gpqpnrrizf
&& rtB . hfbhreh3w5 && rtB . cthl33fyaf ) ; rtB . hnkedtxgcd = ( rtB .
l3wkd25ebx || rtB . dcsnr5tt20 ) ; rtB . gs2ogrzhv4 = ! rtB . ityxpqsryc ;
rtB . biuky0s4pm = ( rtB . abljdeg3mo && rtB . gs2ogrzhv4 ) ; rtB .
gxr5dm22i5 = ! rtB . hnkedtxgcd ; rtB . n1asqi1otu = ( rtB . biuky0s4pm &&
rtB . gxr5dm22i5 ) ; rtB . oxs43anrmq = ! rtB . ityxpqsryc ; rtB . gc2nxy4yms
= ( rtB . lr5wvxyka5 && rtB . oxs43anrmq ) ; rtB . kw3rodcm2e = ! rtB .
hnkedtxgcd ; rtB . dlgx5bq2tw = ( rtB . gc2nxy4yms && rtB . kw3rodcm2e ) ;
rtB . byepklwnoj = ! rtB . ityxpqsryc ; rtB . jqgwfu4jfp = ( rtB . fx2wlnpkmm
&& rtB . byepklwnoj ) ; rtB . p41tqj3qzj = ! rtB . hnkedtxgcd ; rtB .
fjrgfkqpe3 = ( rtB . jqgwfu4jfp && rtB . p41tqj3qzj ) ; rtB . m2cmrvrvjr = (
rtB . hnkedtxgcd || rtB . n1asqi1otu || rtB . dlgx5bq2tw || rtB . fjrgfkqpe3
) ; rtB . afinszx4uw = false ; rtB . ni1jgkstmg = ( rtB . afinszx4uw && rtB .
dqjfuokugx ) ; rtB . lczcqlk0sk = ( rtB . conq4okzy2 || rtB . ni1jgkstmg ) ;
rtB . mnwdjexzpl = ! rtB . et10b34fee ; rtB . c4wtgq3rtz = ( rtB . lczcqlk0sk
&& rtB . mnwdjexzpl ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
l2qscg5gon = rtDW . d4pejtpczl ; rtB . biasjz21pu = ! rtB . l2qscg5gon ; rtB
. feawbjtho4 = rtDW . aj2otrttnf ; } rtB . cz0rkyijqm = ( rtB . c4wtgq3rtz &&
rtB . biasjz21pu ) ; rtB . ppo0swtu1b = ( rtB . ojotb4rusk || rtB .
cz0rkyijqm ) ; rtB . fjwnxxt03c = ( rtB . abolnonekh || rtB . ebmfchpfdp ) ;
rtB . i3uimnluu1 = ( rtB . feawbjtho4 || rtB . klze2p54da ) ; rtB .
i5fvh5nex0 = ! rtB . fjwnxxt03c ; rtB . k2rjzsbrwp = ( rtB . i3uimnluu1 &&
rtB . i5fvh5nex0 ) ; rtB . evpfdltouk = rtB . asa103d02z * rtB . asa103d02z ;
rtB . gpijrilwo2 = rtB . ddix2mhi1g * rtB . ddix2mhi1g ; rtB . kjosge2k54 =
rtB . evpfdltouk + rtB . gpijrilwo2 ; rtB . lk1gj2boqi = muDoubleScalarSqrt (
rtB . kjosge2k54 ) ; if ( rtP . Constant_Value_fai1cjhbit > rtP .
u_Threshold_kzsxowyfs4 ) { rtB . fmto21eupe = ( rtB . lk1gj2boqi < rtP .
Constant3_Value_cls1t2u23z ) ; rtB . cee3crx12u = rtB . fmto21eupe ; } else {
rtB . nge2issyze = ( rtB . lk1gj2boqi > rtP . Constant3_Value_cls1t2u23z ) ;
rtB . cee3crx12u = rtB . nge2issyze ; } if ( rtP . Constant3_Value_knqreynw20
> rtP . Switch_Threshold_eduu3sa2sa ) { rtB . iaf2nrar0y = rtB . asa103d02z +
rtB . fizn4ck4n1 ; rtB . fhdw4qqquy = rtB . iaf2nrar0y + rtB . hlmtyy2tbw ;
rtB . k2v13lkexm = rtB . fhdw4qqquy * rtP . u7_Value ; rtB . czgdfmprvm = rtB
. k2v13lkexm ; } else { rtB . czgdfmprvm = 0.0 ; } rtB . hqdfxuxs1s = rtB .
czgdfmprvm * rtB . czgdfmprvm ; if ( rtP . Constant3_Value_knqreynw20 > rtP .
Switch1_Threshold_jxwfo4pfxm ) { rtB . aownc5vkto = rtB . ddix2mhi1g + rtB .
ndiffna1hx ; rtB . nfqozqq5qz = rtB . aownc5vkto + rtB . krvqv214qo ; rtB .
k5zatdijye = rtB . nfqozqq5qz * rtP . u7_Value ; rtB . kic23wg3ty = rtB .
k5zatdijye ; } else { rtB . kic23wg3ty = 0.0 ; } rtB . j1k53a2g3j = rtB .
kic23wg3ty * rtB . kic23wg3ty ; rtB . grulp4wwkf = rtB . hqdfxuxs1s + rtB .
j1k53a2g3j ; rtB . fz1it1xfay = muDoubleScalarSqrt ( rtB . grulp4wwkf ) ; if
( rtP . Constant_Value_gehlwono3s > rtP . u_Threshold_ngv5orav1u ) { rtB .
d05ajhltbz = ( rtB . fz1it1xfay < rtP . Constant3_Value_prv4dbbmco ) ; rtB .
kpo2da1cwt = rtB . d05ajhltbz ; } else { rtB . eildjhhqyo = ( rtB .
fz1it1xfay > rtP . Constant3_Value_prv4dbbmco ) ; rtB . kpo2da1cwt = rtB .
eildjhhqyo ; } rtB . esazoeulcv = ( rtB . kpo2da1cwt && rtB . dmbkutmxus ) ;
if ( rtB . meg2ajfbgr ) { rtB . dtoktjat0n = rtB . esazoeulcv ; } else { rtB
. aq3g1mi1om = ( rtB . esazoeulcv && rtB . fmaf3i5fz1 ) ; rtB . dtoktjat0n =
rtB . aq3g1mi1om ; } rtB . fizd55ambv = ( rtB . dtoktjat0n || ( rtP .
Constant13_Value != 0.0 ) || ( rtP . Constant13_Value != 0.0 ) || ( rtP .
Constant13_Value != 0.0 ) ) ; rtB . lnuveg4cnz = rtB . hlmtyy2tbw * rtB .
hlmtyy2tbw ; rtB . jqbpa5k1c3 = rtB . krvqv214qo * rtB . krvqv214qo ; rtB .
a5qv0pkuil = rtB . lnuveg4cnz + rtB . jqbpa5k1c3 ; rtB . f2ekecuuf1 =
muDoubleScalarSqrt ( rtB . a5qv0pkuil ) ; if ( rtP .
Constant_Value_fai1cjhbit > rtP . u_Threshold_aowpp3nn3j ) { rtB . dk5ydete3f
= ( rtB . f2ekecuuf1 < rtP . Constant3_Value_cls1t2u23z ) ; rtB . b05e5cuvze
= rtB . dk5ydete3f ; } else { rtB . giw10gq0hn = ( rtB . f2ekecuuf1 > rtP .
Constant3_Value_cls1t2u23z ) ; rtB . b05e5cuvze = rtB . giw10gq0hn ; } rtB .
fh5ylqnw3n = rtB . fizn4ck4n1 * rtB . fizn4ck4n1 ; rtB . kdosynj0qp = rtB .
ndiffna1hx * rtB . ndiffna1hx ; rtB . edt0epaoji = rtB . fh5ylqnw3n + rtB .
kdosynj0qp ; rtB . dwfwpjkaaj = muDoubleScalarSqrt ( rtB . edt0epaoji ) ; if
( rtP . Constant_Value_fai1cjhbit > rtP . u_Threshold_nmwke41y2d ) { rtB .
a1fvi3czcq = ( rtB . dwfwpjkaaj < rtP . Constant3_Value_cls1t2u23z ) ; rtB .
bi3egiksbe = rtB . a1fvi3czcq ; } else { rtB . bmbkno0kbp = ( rtB .
dwfwpjkaaj > rtP . Constant3_Value_cls1t2u23z ) ; rtB . bi3egiksbe = rtB .
bmbkno0kbp ; } rtB . cjcl05h54y = ( rtB . cee3crx12u && rtB . gi0isra5ck ) ;
rtB . b4j54zgnm3 = ( rtB . fizd55ambv && rtB . mhn5qc45at ) ; rtB .
bkibb0j354 = ( rtB . cjcl05h54y && rtB . b4j54zgnm3 ) ; rtB . dl3b0e1r45 = (
rtB . b05e5cuvze && rtB . pux0xcnljs ) ; rtB . f4hzi3pzwl = ( rtB .
dl3b0e1r45 && rtB . b4j54zgnm3 ) ; if ( rtB . miiinprolm ) { rtB . j4z03mncvk
= rtB . bkibb0j354 ; } else { rtB . pqimaazma4 = ( rtB . bkibb0j354 && rtB .
b4siiq1wlu ) ; rtB . j4z03mncvk = rtB . pqimaazma4 ; } rtB . krwozzj2mi = (
rtB . bi3egiksbe && rtB . glipabwfiy ) ; rtB . jdsvvqpxzv = ( rtB .
krwozzj2mi && rtB . b4j54zgnm3 ) ; if ( rtB . miiinprolm ) { rtB . eegmo1bb3b
= rtB . f4hzi3pzwl ; rtB . phtic0hp2q = rtB . jdsvvqpxzv ; } else { rtB .
i3syacg1os = ( rtB . f4hzi3pzwl && rtB . dyjklgcebd ) ; rtB . eegmo1bb3b =
rtB . i3syacg1os ; rtB . ljxcdezarf = ( rtB . jdsvvqpxzv && rtB . gy0rflgkbt
) ; rtB . phtic0hp2q = rtB . ljxcdezarf ; } rtB . iaspj3iyrw = ( rtB .
eegmo1bb3b || rtB . j4z03mncvk || rtB . phtic0hp2q ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . mqs14bnogd = rtDW . euvp233z54 ; rtB . cpmaoe4f4k = !
rtB . mqs14bnogd ; rtB . gorbahg1io = rtDW . bpckxrqz0a ; rtB . dipfp5fllb =
! rtB . gorbahg1io ; } rtB . fuc2gkmbt4 = ( rtB . iaspj3iyrw && rtB .
cpmaoe4f4k ) ; rtB . owxyenu1l2 = ( rtB . dtoktjat0n && rtB . dipfp5fllb ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . d2sgrx5n5k = rtDW . laigsna1ry ;
rtB . di2m0c3sn3 = rtDW . gtdowlniha ; rtB . kqc23er2ns = ! rtB . di2m0c3sn3
; } if ( rtP . Constant4_Value_m2htzwup50 > rtP . u_Threshold_jrag13mqxd ) {
rtB . e0n22f2tie = ( rtB . eegmo1bb3b && rtB . kqc23er2ns ) ; rtB .
jy3pjkt2we = rtB . e0n22f2tie ; } else { rtB . jy3pjkt2we = rtB . eegmo1bb3b
; } rtB . njfganzdsa = rtB . a0it33vpv3 ; rtB . g3guprj5k5 = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . l5vanzvpcs , rtB . njfganzdsa , rtB . g3guprj5k5 ,
rtP . T1_delay_njyv1wv4oa , & rtB . aa351sera2 , & rtDW . aa351sera2 , & rtP
. aa351sera2 , & rtPrevZCX . aa351sera2 ) ; bhc3qgnuic ( rtS , rtB .
c0goa0rpsy , rtB . njfganzdsa , rtB . g3guprj5k5 , rtP . T1_delay_njyv1wv4oa
, & rtB . lfm4cvmyjw , & rtDW . lfm4cvmyjw , & rtP . lfm4cvmyjw , & rtPrevZCX
. lfm4cvmyjw ) ; rtB . oamftqzvr0 = ( rtB . aa351sera2 . j1vu52vt5l || rtB .
lfm4cvmyjw . mjqylog0rb ) ; rtB . pgoiuromtj = ( rtB . jy3pjkt2we && rtB .
oamftqzvr0 ) ; rtB . ecpze4cgmm = ( rtB . d2sgrx5n5k || rtB . pgoiuromtj ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . h0eme5ndjh = rtDW . md3pjrrcpi ;
} rtB . bbs2csu0h3 = ! rtB . eegmo1bb3b ; rtB . aktewfyazq = ( rtB .
h0eme5ndjh && rtB . bbs2csu0h3 ) ; rtB . dw5gc3ughl = rtB . aktewfyazq ; rtB
. fqx2kti0gj = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . pvwir2bnni , rtB .
dw5gc3ughl , rtB . fqx2kti0gj , rtP . T2_delay_kb1ccy4lyo , & rtB .
bypgcvdndn , & rtDW . bypgcvdndn , & rtP . bypgcvdndn , & rtPrevZCX .
bypgcvdndn ) ; bhc3qgnuic ( rtS , rtB . j50xmqsfe0 , rtB . dw5gc3ughl , rtB .
fqx2kti0gj , rtP . T2_delay_kb1ccy4lyo , & rtB . dtwz5asia3 , & rtDW .
dtwz5asia3 , & rtP . dtwz5asia3 , & rtPrevZCX . dtwz5asia3 ) ; rtB .
kkwcefya5j = ( rtB . bypgcvdndn . j1vu52vt5l || rtB . dtwz5asia3 . mjqylog0rb
) ; rtB . jwbt5wodtc = ! rtB . kkwcefya5j ; rtB . o0kmsy0anj = ( rtB .
ecpze4cgmm && rtB . jwbt5wodtc ) ; rtB . aoc02dzg1a = ( rtB . onyt51xv2b &&
rtB . o0kmsy0anj ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . g512ouy5tc
= rtDW . maqoyqobhp ; rtB . df3yzctxb0 = rtDW . hwukdcvf00 ; rtB . c03iitf3pd
= ! rtB . df3yzctxb0 ; } if ( rtP . Constant4_Value_m2htzwup50 > rtP .
u_Threshold_nr4mvevk00 ) { rtB . oghysqcq3f = ( rtB . j4z03mncvk && rtB .
c03iitf3pd ) ; rtB . btbotv1dcb = rtB . oghysqcq3f ; } else { rtB .
btbotv1dcb = rtB . j4z03mncvk ; } rtB . kh2xxrbycj = rtB . gr4vjiot5p ; rtB .
fmzxpl3msq = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . glvl0tzhpx , rtB .
kh2xxrbycj , rtB . fmzxpl3msq , rtP . T1_delay_pc50qhusgz , & rtB .
dkvqk03afq , & rtDW . dkvqk03afq , & rtP . dkvqk03afq , & rtPrevZCX .
dkvqk03afq ) ; bhc3qgnuic ( rtS , rtB . fcdrb3hdx1 , rtB . kh2xxrbycj , rtB .
fmzxpl3msq , rtP . T1_delay_pc50qhusgz , & rtB . drlew34sm0 , & rtDW .
drlew34sm0 , & rtP . drlew34sm0 , & rtPrevZCX . drlew34sm0 ) ; rtB .
lkrcvvlaff = ( rtB . dkvqk03afq . j1vu52vt5l || rtB . drlew34sm0 . mjqylog0rb
) ; rtB . p0yp0lq21g = ( rtB . btbotv1dcb && rtB . lkrcvvlaff ) ; rtB .
k1njll0qvb = ( rtB . g512ouy5tc || rtB . p0yp0lq21g ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . klvwohc3my = rtDW . ip1qcpepxp ; } rtB . bsu4wyq1uj =
! rtB . j4z03mncvk ; rtB . amzbz45hy4 = ( rtB . klvwohc3my && rtB .
bsu4wyq1uj ) ; rtB . jxoa4y03ej = rtB . amzbz45hy4 ; rtB . hesqosydxf =
ssGetT ( rtS ) ; on20slldft ( rtS , rtB . gr3sucvwgc , rtB . jxoa4y03ej , rtB
. hesqosydxf , rtP . T2_delay_n5wjucecau , & rtB . j3mwdbpvpq , & rtDW .
j3mwdbpvpq , & rtP . j3mwdbpvpq , & rtPrevZCX . j3mwdbpvpq ) ; bhc3qgnuic (
rtS , rtB . g033ihps0k , rtB . jxoa4y03ej , rtB . hesqosydxf , rtP .
T2_delay_n5wjucecau , & rtB . ieaaexns5m , & rtDW . ieaaexns5m , & rtP .
ieaaexns5m , & rtPrevZCX . ieaaexns5m ) ; rtB . obsdy2xhnl = ( rtB .
j3mwdbpvpq . j1vu52vt5l || rtB . ieaaexns5m . mjqylog0rb ) ; rtB . bcoh0g1c3e
= ! rtB . obsdy2xhnl ; rtB . hww5gj4qer = ( rtB . k1njll0qvb && rtB .
bcoh0g1c3e ) ; rtB . agnnsuelgx = ( rtB . aazsjjcsgd && rtB . hww5gj4qer ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . gqn1mbex14 = rtDW . pxb20wrhli ;
rtB . benpr0ft4q = rtDW . gtyu3k2ot5 ; rtB . irhbpjovy5 = ! rtB . benpr0ft4q
; } if ( rtP . Constant4_Value_m2htzwup50 > rtP . u_Threshold_flxtgu3xpb ) {
rtB . eudppkkci1 = ( rtB . phtic0hp2q && rtB . irhbpjovy5 ) ; rtB .
hzqux2maqc = rtB . eudppkkci1 ; } else { rtB . hzqux2maqc = rtB . phtic0hp2q
; } rtB . bfrun04cem = rtB . lz0cksews5 ; rtB . bbae2gaav2 = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . dihsxcxh0i , rtB . bfrun04cem , rtB . bbae2gaav2 ,
rtP . T1_delay_okgixtggsw , & rtB . eg2uuciuj1 , & rtDW . eg2uuciuj1 , & rtP
. eg2uuciuj1 , & rtPrevZCX . eg2uuciuj1 ) ; bhc3qgnuic ( rtS , rtB .
haztzm5pvv , rtB . bfrun04cem , rtB . bbae2gaav2 , rtP . T1_delay_okgixtggsw
, & rtB . emkwv3osqn , & rtDW . emkwv3osqn , & rtP . emkwv3osqn , & rtPrevZCX
. emkwv3osqn ) ; rtB . cakyu3kqjl = ( rtB . eg2uuciuj1 . j1vu52vt5l || rtB .
emkwv3osqn . mjqylog0rb ) ; rtB . acyorezo0c = ( rtB . hzqux2maqc && rtB .
cakyu3kqjl ) ; rtB . ec2budar1s = ( rtB . gqn1mbex14 || rtB . acyorezo0c ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . od1loznbjg = rtDW . cm1jdei3jr ;
} rtB . apruvkdkcp = ! rtB . phtic0hp2q ; rtB . e0d41r5oin = ( rtB .
od1loznbjg && rtB . apruvkdkcp ) ; rtB . d00030kexq = rtB . e0d41r5oin ; rtB
. jf2cqs1k5e = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . j4p3wkehee , rtB .
d00030kexq , rtB . jf2cqs1k5e , rtP . T2_delay_cfgdvhvyw2 , & rtB .
acyd2alexd , & rtDW . acyd2alexd , & rtP . acyd2alexd , & rtPrevZCX .
acyd2alexd ) ; bhc3qgnuic ( rtS , rtB . d0y5z24lvu , rtB . d00030kexq , rtB .
jf2cqs1k5e , rtP . T2_delay_cfgdvhvyw2 , & rtB . kcdiwopp0s , & rtDW .
kcdiwopp0s , & rtP . kcdiwopp0s , & rtPrevZCX . kcdiwopp0s ) ; rtB .
ixl1m4qddm = ( rtB . acyd2alexd . j1vu52vt5l || rtB . kcdiwopp0s . mjqylog0rb
) ; rtB . gfh3h3lcfb = ! rtB . ixl1m4qddm ; rtB . b4y0mche2n = ( rtB .
ec2budar1s && rtB . gfh3h3lcfb ) ; rtB . lv424trud4 = ( rtB . czpm4yahhz &&
rtB . b4y0mche2n ) ; rtB . ddtqgozkrj = ( rtB . aoc02dzg1a || rtB .
agnnsuelgx || rtB . lv424trud4 ) ; if ( rtB . oprd2npfri ) { rtB . ln43dh25pr
= ( rtB . aoc02dzg1a || rtB . agnnsuelgx || rtB . lv424trud4 ) ; rtB .
hc31dgtb0o = rtB . ln43dh25pr ; } else { rtB . hvlc5lbgae = ( rtB .
aoc02dzg1a && rtB . agnnsuelgx && rtB . lv424trud4 ) ; rtB . obzkrnzixs = ( (
rtP . Constant2_Value_d5r4ggdjtu != 0.0 ) && rtB . hvlc5lbgae ) ; rtB .
gg2wccbaep = ! rtB . lv424trud4 ; rtB . juvzkggbm2 = ( rtB . aoc02dzg1a &&
rtB . agnnsuelgx && rtB . gg2wccbaep ) ; rtB . dtmrcs2qix = ! rtB .
agnnsuelgx ; rtB . ojfeom3qd2 = ( rtB . aoc02dzg1a && rtB . dtmrcs2qix && rtB
. lv424trud4 ) ; rtB . nxx0akim54 = ! rtB . aoc02dzg1a ; rtB . lymtgmf1xc = (
rtB . nxx0akim54 && rtB . agnnsuelgx && rtB . lv424trud4 ) ; rtB . jvkn31b332
= ( rtB . lymtgmf1xc || rtB . ojfeom3qd2 || rtB . juvzkggbm2 ) ; rtB .
nafyuou03x = ( ( rtP . Constant1_Value_kuads453r2 != 0.0 ) && rtB .
jvkn31b332 ) ; rtB . kb2zqj22wa = ! rtB . agnnsuelgx ; rtB . cxtds2nnvd = !
rtB . aoc02dzg1a ; rtB . no3zkug422 = ( rtB . cxtds2nnvd && rtB . kb2zqj22wa
&& rtB . lv424trud4 ) ; rtB . fiv4cdvwr4 = ! rtB . lv424trud4 ; rtB .
nu0fs32pja = ! rtB . aoc02dzg1a ; rtB . pme4ap2zab = ( rtB . nu0fs32pja &&
rtB . agnnsuelgx && rtB . fiv4cdvwr4 ) ; rtB . cvsjctv34k = ! rtB .
lv424trud4 ; rtB . mujckjsxrn = ! rtB . agnnsuelgx ; rtB . mtn5ffuzul = ( rtB
. aoc02dzg1a && rtB . mujckjsxrn && rtB . cvsjctv34k ) ; rtB . bh2baui4vj = (
rtB . mtn5ffuzul || rtB . pme4ap2zab || rtB . no3zkug422 ) ; rtB . j55dnvcqhb
= ( ( rtP . Constant3_Value_mteivjs2rf != 0.0 ) && rtB . bh2baui4vj ) ; rtB .
a4ymrdkscx = ( rtB . j55dnvcqhb || rtB . nafyuou03x || rtB . obzkrnzixs ) ;
rtB . hc31dgtb0o = rtB . a4ymrdkscx ; } rtB . blauzjkkb1 = ( rtB . hc31dgtb0o
&& rtB . lv424trud4 ) ; rtB . met5yjbva4 = ( rtB . blauzjkkb1 || ( rtP .
Constant11_Value_bs3cayia3q != 0.0 ) ) ; rtB . b1okrpiipz = rtB . met5yjbva4
; rtB . kngxaunbxp = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . bw04w5v3m2 ,
rtB . b1okrpiipz , rtB . kngxaunbxp , rtP . T4_delay , & rtB . bd4y1f4afb , &
rtDW . bd4y1f4afb , & rtP . bd4y1f4afb , & rtPrevZCX . bd4y1f4afb ) ;
bhc3qgnuic ( rtS , rtB . lgznclzrkr , rtB . b1okrpiipz , rtB . kngxaunbxp ,
rtP . T4_delay , & rtB . bbo3ui3fzz , & rtDW . bbo3ui3fzz , & rtP .
bbo3ui3fzz , & rtPrevZCX . bbo3ui3fzz ) ; rtB . m5jpcakflu = rtB . hwmfysoboh
; rtB . p21gmx4dhb = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . oscsgejk1g ,
rtB . m5jpcakflu , rtB . p21gmx4dhb , rtP . T7_delay , & rtB . jejxmo41e1 , &
rtDW . jejxmo41e1 , & rtP . jejxmo41e1 , & rtPrevZCX . jejxmo41e1 ) ;
bhc3qgnuic ( rtS , rtB . fbnhuuhfxe , rtB . m5jpcakflu , rtB . p21gmx4dhb ,
rtP . T7_delay , & rtB . kwzmy55kax , & rtDW . kwzmy55kax , & rtP .
kwzmy55kax , & rtPrevZCX . kwzmy55kax ) ; rtB . kq02fslw5x = ( rtB .
jejxmo41e1 . j1vu52vt5l || rtB . kwzmy55kax . mjqylog0rb ) ; rtB . l04ypurz5n
= rtB . met5yjbva4 ; rtB . huoxjbfjwi = ssGetT ( rtS ) ; on20slldft ( rtS ,
rtB . ayzloxj5bc , rtB . l04ypurz5n , rtB . huoxjbfjwi , rtP . T2_delay , &
rtB . ge1ps1iuod , & rtDW . ge1ps1iuod , & rtP . ge1ps1iuod , & rtPrevZCX .
ge1ps1iuod ) ; bhc3qgnuic ( rtS , rtB . cyagjbdmtr , rtB . l04ypurz5n , rtB .
huoxjbfjwi , rtP . T2_delay , & rtB . bk3jg31gdb , & rtDW . bk3jg31gdb , &
rtP . bk3jg31gdb , & rtPrevZCX . bk3jg31gdb ) ; if ( rtB . macrenionz ) { rtB
. ecfnoufecj = rtB . met5yjbva4 ; } else { if ( rtB . kq02fslw5x ) { rtB .
daudihdxbn = ( rtB . bd4y1f4afb . j1vu52vt5l || rtB . bbo3ui3fzz . mjqylog0rb
) ; rtB . ca4zke1t3o = rtB . daudihdxbn ; } else { rtB . bawffyc3oq = ( rtB .
ge1ps1iuod . j1vu52vt5l || rtB . bk3jg31gdb . mjqylog0rb ) ; rtB . ca4zke1t3o
= rtB . bawffyc3oq ; } rtB . ecfnoufecj = rtB . ca4zke1t3o ; } rtB .
p42kafnbz1 = ( rtB . phtic0hp2q && rtB . ecfnoufecj && rtB . hc31dgtb0o ) ;
rtB . acqmfmbdup = ( rtB . hc31dgtb0o && rtB . aoc02dzg1a ) ; rtB .
nj4qurn2ac = ( rtB . hc31dgtb0o && rtB . agnnsuelgx ) ; rtB . mmdp1szjrh = (
rtB . acqmfmbdup || ( rtP . Constant11_Value_bs3cayia3q != 0.0 ) ) ; rtB .
j5xfvukgfq = ( rtB . nj4qurn2ac || ( rtP . Constant11_Value_bs3cayia3q != 0.0
) ) ; rtB . oqv3kps3a5 = rtB . mmdp1szjrh ; rtB . c3irdl0ted = ssGetT ( rtS )
; on20slldft ( rtS , rtB . jnjcghfjwd , rtB . oqv3kps3a5 , rtB . c3irdl0ted ,
rtP . T6_delay , & rtB . i43atkcssg , & rtDW . i43atkcssg , & rtP .
i43atkcssg , & rtPrevZCX . i43atkcssg ) ; bhc3qgnuic ( rtS , rtB . afvklxyved
, rtB . oqv3kps3a5 , rtB . c3irdl0ted , rtP . T6_delay , & rtB . kqhpdk5kzz ,
& rtDW . kqhpdk5kzz , & rtP . kqhpdk5kzz , & rtPrevZCX . kqhpdk5kzz ) ; rtB .
oqc00lmczs = rtB . mmdp1szjrh ; rtB . ihxkblh2jn = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . aq45nwbsrj , rtB . oqc00lmczs , rtB . ihxkblh2jn ,
rtP . T3_delay , & rtB . jvraryandh , & rtDW . jvraryandh , & rtP .
jvraryandh , & rtPrevZCX . jvraryandh ) ; bhc3qgnuic ( rtS , rtB . fvmmjxa1a3
, rtB . oqc00lmczs , rtB . ihxkblh2jn , rtP . T3_delay , & rtB . e2epyeugyn ,
& rtDW . e2epyeugyn , & rtP . e2epyeugyn , & rtPrevZCX . e2epyeugyn ) ; if (
rtB . macrenionz ) { rtB . f0nbktvoxy = rtB . mmdp1szjrh ; } else { if ( rtB
. kq02fslw5x ) { rtB . isti21xypr = ( rtB . i43atkcssg . j1vu52vt5l || rtB .
kqhpdk5kzz . mjqylog0rb ) ; rtB . fghkbxwe3m = rtB . isti21xypr ; } else {
rtB . ou4qozine5 = ( rtB . jvraryandh . j1vu52vt5l || rtB . e2epyeugyn .
mjqylog0rb ) ; rtB . fghkbxwe3m = rtB . ou4qozine5 ; } rtB . f0nbktvoxy = rtB
. fghkbxwe3m ; } rtB . fado3mjkqp = ( rtB . eegmo1bb3b && rtB . f0nbktvoxy &&
rtB . hc31dgtb0o ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . b1rst0tcqg
= rtDW . aj21ow2g22 ; rtB . g0k5haz1m4 = ! rtB . b1rst0tcqg ; rtB .
pfrii30piy = rtDW . k344xgbfw1 ; rtB . iowldo3kzg = ! rtB . pfrii30piy ; }
rtB . fpzhnyeqru = ( rtB . fado3mjkqp && rtB . g0k5haz1m4 ) ; rtB .
hmhxksvs4w = ssGetT ( rtS ) ; rtB . pdokz3j4hc = rtB . fado3mjkqp ; rtB .
kuymr5hgfm = rtB . pdokz3j4hc ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
dgpdl40ppd = rtDW . equ3xylxak ; } itcd43cpff ( rtS , rtB . mqo2vvvrz0 [ 0 ]
, rtB . kuymr5hgfm , rtB . dgpdl40ppd , & rtB . i2y4oqfnlh , & rtDW .
i2y4oqfnlh ) ; cgvj5xjnjk ( rtS , rtB . mqo2vvvrz0 [ 1 ] , rtB . kuymr5hgfm ,
rtB . dgpdl40ppd , & rtB . fushkw2zny , & rtDW . fushkw2zny ) ; rtB .
emwthobqym = ( rtB . i2y4oqfnlh . guqrn12esp || rtB . fushkw2zny . l5pxivnpxv
) ; rtB . cqjydmfb5a = ( rtB . iowldo3kzg && rtB . emwthobqym ) ; rtB .
nczfqk2ryr = rtB . cqjydmfb5a ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
nhcggjbosq = rtDW . p35tk2ttns ; } if ( rtB . nczfqk2ryr ) { rtB . bzfuqbt5cs
= rtB . hmhxksvs4w ; } else { rtB . bzfuqbt5cs = rtB . nhcggjbosq ; } maxV =
rtP . u_PulseDuration_e0yni1mmpc - 2.2204460492503131E-16 ; rtB . l1ceyfbu5n
= rtB . bzfuqbt5cs + maxV ; rtB . ckj0njvng3 = ( rtB . l1ceyfbu5n > rtB .
hmhxksvs4w ) ; rtB . nhq4spzciz = ( rtB . fpzhnyeqru || rtB . ckj0njvng3 ) ;
rtB . cacemkvyq0 = rtB . j5xfvukgfq ; rtB . cgtbckhyjw = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . oaklkrqhff , rtB . cacemkvyq0 , rtB . cgtbckhyjw ,
rtP . T5_delay , & rtB . dffp1wyrjz , & rtDW . dffp1wyrjz , & rtP .
dffp1wyrjz , & rtPrevZCX . dffp1wyrjz ) ; bhc3qgnuic ( rtS , rtB . nlyeq12rbm
, rtB . cacemkvyq0 , rtB . cgtbckhyjw , rtP . T5_delay , & rtB . mwrkb5ri4b ,
& rtDW . mwrkb5ri4b , & rtP . mwrkb5ri4b , & rtPrevZCX . mwrkb5ri4b ) ; rtB .
d0pfzgc22b = rtB . j5xfvukgfq ; rtB . fs5u1pezzg = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . g5uya1grwm , rtB . d0pfzgc22b , rtB . fs5u1pezzg ,
rtP . T1_delay , & rtB . on20slldft0 , & rtDW . on20slldft0 , & rtP .
on20slldft0 , & rtPrevZCX . on20slldft0 ) ; bhc3qgnuic ( rtS , rtB .
d35uvtgz4o , rtB . d0pfzgc22b , rtB . fs5u1pezzg , rtP . T1_delay , & rtB .
bhc3qgnuicw , & rtDW . bhc3qgnuicw , & rtP . bhc3qgnuicw , & rtPrevZCX .
bhc3qgnuicw ) ; if ( rtB . macrenionz ) { rtB . aiqw3obtob = rtB . j5xfvukgfq
; } else { if ( rtB . kq02fslw5x ) { rtB . gos1bxotbf = ( rtB . dffp1wyrjz .
j1vu52vt5l || rtB . mwrkb5ri4b . mjqylog0rb ) ; rtB . bti3hnjbdn = rtB .
gos1bxotbf ; } else { rtB . ilrj0flben = ( rtB . on20slldft0 . j1vu52vt5l ||
rtB . bhc3qgnuicw . mjqylog0rb ) ; rtB . bti3hnjbdn = rtB . ilrj0flben ; }
rtB . aiqw3obtob = rtB . bti3hnjbdn ; } rtB . h1l2yohxy4 = ( rtB . j4z03mncvk
&& rtB . aiqw3obtob && rtB . hc31dgtb0o ) ; if ( ssIsSampleHit ( rtS , 1 , 0
) ) { rtB . pkir05ovuz = rtDW . b55d2d4pem ; rtB . mkm44bezcu = ! rtB .
pkir05ovuz ; rtB . lds2xfdzly = rtDW . h5iqn1tf0f ; rtB . a4wu4jct3b = ! rtB
. lds2xfdzly ; } rtB . ecs0zrapk4 = ( rtB . h1l2yohxy4 && rtB . mkm44bezcu )
; rtB . gdxwi1pcd4 = ssGetT ( rtS ) ; rtB . bx2sa3zrty = rtB . h1l2yohxy4 ;
rtB . jshyudx1xv = rtB . bx2sa3zrty ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) {
rtB . hqf2e0uqmw = rtDW . iabw2jrkb0 ; } itcd43cpff ( rtS , rtB . bgwcrbleld
[ 0 ] , rtB . jshyudx1xv , rtB . hqf2e0uqmw , & rtB . fau5zmxxcd , & rtDW .
fau5zmxxcd ) ; cgvj5xjnjk ( rtS , rtB . bgwcrbleld [ 1 ] , rtB . jshyudx1xv ,
rtB . hqf2e0uqmw , & rtB . cg1ijj42cn , & rtDW . cg1ijj42cn ) ; rtB .
biwwqts1pd = ( rtB . fau5zmxxcd . guqrn12esp || rtB . cg1ijj42cn . l5pxivnpxv
) ; rtB . f2qdqjwv2d = ( rtB . a4wu4jct3b && rtB . biwwqts1pd ) ; rtB .
kuepcq1k1g = rtB . f2qdqjwv2d ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
jypqsfvuar = rtDW . mzqdovgi1x ; } if ( rtB . kuepcq1k1g ) { rtB . b1qme4xqqh
= rtB . gdxwi1pcd4 ; } else { rtB . b1qme4xqqh = rtB . jypqsfvuar ; } maxV =
rtP . u_PulseDuration_ngpqbuyuuc - 2.2204460492503131E-16 ; rtB . e04nvxubpn
= rtB . b1qme4xqqh + maxV ; rtB . dbl43jye5a = ( rtB . e04nvxubpn > rtB .
gdxwi1pcd4 ) ; rtB . pjtnnybuy1 = ( rtB . ecs0zrapk4 || rtB . dbl43jye5a ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . aren0rinie = rtDW . i4uxbnvgsv ;
rtB . kqghrnieya = ! rtB . aren0rinie ; rtB . e23tc1kjxa = rtDW . bmkmerzuty
; rtB . kicgplweqk = ! rtB . e23tc1kjxa ; } rtB . jj45mhjvp4 = ( rtB .
p42kafnbz1 && rtB . kqghrnieya ) ; rtB . b42nz1tyrd = ssGetT ( rtS ) ; rtB .
fthnc0qqo0 = rtB . p42kafnbz1 ; rtB . eb1jbzfho2 = rtB . fthnc0qqo0 ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . cstsob134w = rtDW . idmf1p4vkt ; }
itcd43cpff ( rtS , rtB . el5hg4zl3f [ 0 ] , rtB . eb1jbzfho2 , rtB .
cstsob134w , & rtB . ncfy3u3izo , & rtDW . ncfy3u3izo ) ; cgvj5xjnjk ( rtS ,
rtB . el5hg4zl3f [ 1 ] , rtB . eb1jbzfho2 , rtB . cstsob134w , & rtB .
np33hf1v4j , & rtDW . np33hf1v4j ) ; rtB . jwvdn5ivzg = ( rtB . ncfy3u3izo .
guqrn12esp || rtB . np33hf1v4j . l5pxivnpxv ) ; rtB . gx2c0xjksn = ( rtB .
kicgplweqk && rtB . jwvdn5ivzg ) ; rtB . b1q350ex5v = rtB . gx2c0xjksn ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . jbvgcpsrxw = rtDW . pwnzz0enea ; } if
( rtB . b1q350ex5v ) { rtB . pzs5dww0bf = rtB . b42nz1tyrd ; } else { rtB .
pzs5dww0bf = rtB . jbvgcpsrxw ; } maxV = rtP . u_PulseDuration_gu2ej35n44 -
2.2204460492503131E-16 ; rtB . cllfmob5p0 = rtB . pzs5dww0bf + maxV ; rtB .
dutizthppc = ( rtB . cllfmob5p0 > rtB . b42nz1tyrd ) ; rtB . ijipj0fvb5 = (
rtB . jj45mhjvp4 || rtB . dutizthppc ) ; rtB . pf1chrm4t2 = ( rtB .
nhq4spzciz || rtB . pjtnnybuy1 || rtB . ijipj0fvb5 ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . dbnuw35hwj = rtDW . of54iatyrl ; rtB . b5wispvqn4 = !
rtB . dbnuw35hwj ; } rtB . i5s4qxdp4p = ( rtB . pf1chrm4t2 && rtB .
b5wispvqn4 ) ; rtB . jcqoghp3b0 = ssGetT ( rtS ) ; if ( ssIsSampleHit ( rtS ,
1 , 0 ) ) { fx2esogudm ( rtS , rtB . emwthobqym , rtB . jcqoghp3b0 , & rtB .
jw4rwe2x30 , & rtDW . jw4rwe2x30 , & rtPrevZCX . jw4rwe2x30 ) ; rtB .
d5qzw2xpuu = rtB . jw4rwe2x30 . jhmp0c0tsm + rtP . Constant_Value_ftrqn0pkto
; } rtB . flvpj5ytrb = ( rtB . d5qzw2xpuu > rtB . jcqoghp3b0 ) ; rtB .
omd0lz5ueq = ssGetT ( rtS ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) {
fx2esogudm ( rtS , rtB . biwwqts1pd , rtB . omd0lz5ueq , & rtB . mydfkxdbcd ,
& rtDW . mydfkxdbcd , & rtPrevZCX . mydfkxdbcd ) ; rtB . g1ex44vwfo = rtB .
mydfkxdbcd . jhmp0c0tsm + rtP . Constant_Value_box2onnicr ; } rtB .
ofzziop0ij = ( rtB . g1ex44vwfo > rtB . omd0lz5ueq ) ; rtB . fw5jcd4fq0 =
ssGetT ( rtS ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { fx2esogudm ( rtS ,
rtB . jwvdn5ivzg , rtB . fw5jcd4fq0 , & rtB . dcuqvbgzpd , & rtDW .
dcuqvbgzpd , & rtPrevZCX . dcuqvbgzpd ) ; rtB . fapqwr5zti = rtB . dcuqvbgzpd
. jhmp0c0tsm + rtP . Constant_Value_k3x3ehksgs ; rtB . cjrt3qytct = rtDW .
lgg4cwcbcv ; } rtB . lo4glx1rkx = ( rtB . fapqwr5zti > rtB . fw5jcd4fq0 ) ;
rtB . ejtzvjokuw = rtB . ddix2mhi1g * rtP . u_Value_mysfrmvavd ; rtB .
jjme2mxh5v = rtB . asa103d02z * rtP . u_Value_hc0ryyyxzj ; rtB . mklyfd22fl =
rtB . ejtzvjokuw + rtB . jjme2mxh5v ; rtB . klw1mx21rq = rtB . ndiffna1hx *
rtP . u_Value_mysfrmvavd ; rtB . gstzfibqgd = rtB . fizn4ck4n1 * rtP .
u_Value_mxk1xk0imt ; rtB . dhc1zpnpow = rtB . klw1mx21rq + rtB . gstzfibqgd ;
rtB . h3epn54zze = rtB . mklyfd22fl + rtB . dhc1zpnpow ; rtB . crbdm5d21p =
rtB . krvqv214qo + rtB . h3epn54zze ; rtB . ktw31tlrfm = rtB . ddix2mhi1g *
rtP . u_Value_mysfrmvavd ; rtB . goquudpfgn = rtB . asa103d02z * rtP .
u_Value_mxk1xk0imt ; rtB . l1n3twybev = rtB . ktw31tlrfm + rtB . goquudpfgn ;
rtB . f25atekrse = rtB . ndiffna1hx * rtP . u_Value_mysfrmvavd ; rtB .
eitjehbvkq = rtB . fizn4ck4n1 * rtP . u_Value_hc0ryyyxzj ; rtB . fwsmet35n3 =
rtB . f25atekrse + rtB . eitjehbvkq ; rtB . c4qmqlohtz = rtB . l1n3twybev +
rtB . fwsmet35n3 ; rtB . pzvzjty20r = rtB . krvqv214qo + rtB . c4qmqlohtz ;
rtB . pq0w4n3t5l = rtB . asa103d02z * rtP . u_Value_mysfrmvavd ; rtB .
ohmnk52tw5 = rtB . ddix2mhi1g * rtP . u_Value_mxk1xk0imt ; rtB . cxhunotz1u =
rtB . pq0w4n3t5l - rtB . ohmnk52tw5 ; rtB . hv5cp4llhb = rtB . fizn4ck4n1 *
rtP . u_Value_mysfrmvavd ; rtB . cpvxnlw4oq = rtB . ndiffna1hx * rtP .
u_Value_hc0ryyyxzj ; rtB . gidblclcvd = rtB . hv5cp4llhb - rtB . cpvxnlw4oq ;
rtB . ecv1zh4xgd = rtB . cxhunotz1u + rtB . gidblclcvd ; rtB . ouzovgvarb =
rtB . hlmtyy2tbw + rtB . ecv1zh4xgd ; rtB . jo0wb1jwp4 = rtB . ouzovgvarb *
rtP . u7_Value ; rtB . aqelbul2by = rtB . asa103d02z * rtP .
u_Value_mysfrmvavd ; rtB . n3g4fgkoi4 = rtB . ddix2mhi1g * rtP .
u_Value_hc0ryyyxzj ; rtB . oahbiodkmk = rtB . aqelbul2by - rtB . n3g4fgkoi4 ;
rtB . hlm1pcho11 = rtB . fizn4ck4n1 * rtP . u_Value_mysfrmvavd ; rtB .
jqzbc53psd = rtB . ndiffna1hx * rtP . u_Value_mxk1xk0imt ; rtB . ns3mt5awfk =
rtB . hlm1pcho11 - rtB . jqzbc53psd ; rtB . nsqe3arsng = rtB . oahbiodkmk +
rtB . ns3mt5awfk ; rtB . fjdoylw2zz = rtB . hlmtyy2tbw + rtB . nsqe3arsng ;
rtB . dytbgkti3x = rtB . fjdoylw2zz * rtP . u7_Value ; rtB . fbhu1lah4d = rtB
. crbdm5d21p * rtP . u7_Value ; rtB . poajuzqeum = rtB . pzvzjty20r * rtP .
u7_Value ; rtB . ee5wecyhln = ! rtB . pt05pvlu1h ; rtB . phxn4vfs1g = ! rtB .
ee5wecyhln ; rtB . heoxezygzy = ( rtB . cjrt3qytct && rtB . phxn4vfs1g ) ;
rtB . gyavbiowm4 = ( rtB . jvaw411iaj || rtB . heoxezygzy ) ; rtB .
nitktgtcn5 = ( rtB . gyavbiowm4 && rtB . ca0dkhyahi ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . iuxtltqbiy = rtDW . ldsl4h1b2m ; rtB . ia2cyyy4q3 = !
rtB . iuxtltqbiy ; rtB . hwb3nzdeed = rtDW . fmwrpqnbm1 ; } rtB . ocd3s1ssuw
= ( rtB . ojwyzbseek && rtB . ia2cyyy4q3 ) ; rtB . mbfnqi4rhr = ( rtB .
ocd3s1ssuw && rtB . mqsksz2kyw ) ; rtB . fawclwqzvy = ! rtB . eazpmth4el ;
rtB . ab1ktutq45 = ! rtB . fawclwqzvy ; rtB . c4o3n5nund = ( rtB . hwb3nzdeed
&& rtB . ab1ktutq45 ) ; rtB . gxsxbctriq = ( rtB . nwlthkn33y || rtB .
c4o3n5nund ) ; rtB . e0fwmeh0sv = ( rtB . gxsxbctriq && rtB . ai0bwq10ky ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . jhanqbqwqd = rtDW . lltb10cjgb ;
rtB . ka1v2i1bp0 = ! rtB . jhanqbqwqd ; } rtB . cltkyy010m = ( rtB .
jh443z4du0 && rtB . ka1v2i1bp0 ) ; rtB . nvquxs1c3l = ( rtB . cltkyy010m &&
rtB . omgvncxd3u ) ; rtB . k1rtvbrumc = ( rtB . hkwo4x0o0p || rtB .
nitktgtcn5 || rtB . e0fwmeh0sv ) ; rtB . ksj03ljl1h = ( rtB . k3om0acsgh ||
rtB . io0nqp4diw || rtB . ktvbljhq2p ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) )
{ rtB . c4xj4bdbrv = rtDW . f3l0hj1q5z ; rtB . b2etrzojot = rtDW . gwjqd3rg01
; rtB . akpips0qmp = rtDW . jn3q0bmaxx ; rtB . mqzh4us2pg = rtDW . mpw5qp3zhv
; } rtB . moprqdes3f = ( rtB . jihq3em231 == rtB . mln2cilizw ) ; rtB .
ei3id2x3ug = ( rtB . mln2cilizw == rtB . peunc2zflo ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . awhysiepcl = rtDW . e20bjtoooq ; rtB . fmj4bqqzzz =
rtDW . iphvfgl3lu ; rtB . j2jawg3ipw = rtDW . a3qbrcdsa4 ; rtB . mexmdikovu =
rtDW . hrvs2bhnce ; rtB . hbyb5yq2wa = rtDW . pnnzfpsb2c ; rtB . hq15ol4u00 =
rtDW . jgoshqlbir ; rtB . l1fyknaxfx = rtDW . jcjg4wdcj2 ; rtB . catom4ds04 =
rtDW . gylvz5hxsg ; rtB . me5qv15ino = rtDW . gyn5krs1zz ; rtB . dz5iehv51p =
rtDW . jwnv52k1wb ; rtB . iy34vxel11 = rtDW . mbo2opl3eu ; rtB . a2zy45irct =
rtDW . c3jeodupfc ; rtB . p2s4q11xzy = rtDW . giowad3sec ; rtB . nosoqcshkk =
rtDW . md4snt4zsk ; rtB . al1vrz51ml = rtDW . i1pprdx5pp ; rtB . jt1fs0q03l =
rtDW . f2nxapnymz ; } rtB . cdqzth3mj3 = ( rtB . ant5kjwsif == rtB .
o0ost4zw4n ) ; rtB . avqw4clixl = ( rtB . o0ost4zw4n == rtB . j4j0kw1hss ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . kjuppwtwvu = rtDW . autyikzmna ;
rtB . je5jzdhzwr = rtDW . n3upjnxoej ; rtB . np1paix0hh = rtDW . i0enxeox0z ;
rtB . in2bbvlk2d = rtDW . ifgv45o1io ; rtB . bfptuyuq2b = rtDW . g1pk0bzvtv ;
rtB . joqfm2xeli = rtDW . ah5hnpnyx1 ; rtB . divwonmszi = rtDW . gry5zwpbhu ;
rtB . ctjiw4lx5e = rtDW . ptqaeanwg0 ; rtB . pedrjzwjtg = rtDW . iug5i33rtw ;
rtB . gikkmuveb2 = rtDW . mdvftfecjd ; rtB . kspoa0qx51 = rtDW . fufczbmjix ;
rtB . ld1ux1jqej = rtDW . dxi5yvdasd ; rtB . cxrzkrt5zb = rtDW . eowtljwki4 ;
rtB . fafsbij0lp = rtDW . l4egugzm4w ; rtB . bywjvbcj1o = rtDW . byq22ip4jp ;
rtB . madbzdt0vb = rtDW . cm5vdxv24a ; rtB . ggfetidn5i = rtDW . flh5edlegj ;
rtB . j4ab1fsmlj = rtDW . pxv2xpdkad ; rtB . etzzrmejrs = rtDW . f4thttu240 ;
rtB . fk5mmytqj3 = rtDW . jgrmmzecam ; rtB . catr4fqykw = rtDW . cjg142fgy0 ;
rtB . awwn10zjl2 = rtDW . eteblkpynn ; rtB . e4ltx14wep = rtDW . pnthkernke ;
rtB . busrc0io0m = rtDW . nihpinzhrt ; rtB . bnlt0zss24 = rtDW . p3uq2rdn1i ;
rtB . nkf4fkpcah = rtDW . nh3lnfbigq ; rtB . iezbiixuat = rtDW . dzo14tsqn4 ;
rtB . o3k0x1avel = rtDW . gcy3ctyywg ; rtB . d2dpvad2wy = rtDW . i5a0jb1dk3 ;
rtB . j41y2yetg3 = ! rtB . d2dpvad2wy ; } rtB . e445k0l1us = ( rtB .
gaxihhe0hq == rtB . meklopiqp5 ) ; rtB . kpept0xsmi = ( rtB . meklopiqp5 ==
rtB . l5kpwgdhj4 ) ; rtB . bkxy5bgkhx = ( rtB . ksj03ljl1h && rtB .
j41y2yetg3 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . ojjbehm21m = rtDW
. guzsz3fvfs ; rtB . ohdz2zlu1d = rtDW . az2oeo1f4o ; rtB . mcfqqro3yx = !
rtB . ohdz2zlu1d ; } rtB . mf2xedwddk = ! rtB . cojjqnmqq2 ; rtB . oajsu5f1eu
= ! rtB . mf2xedwddk ; rtB . pa43h5rd2x = ( rtB . ojjbehm21m && rtB .
oajsu5f1eu ) ; rtB . bvdns4bi03 = ( rtB . dwyytovdmt || rtB . pa43h5rd2x ) ;
rtB . dvwxwugcj3 = ( rtB . bvdns4bi03 && rtB . g340fostkk ) ; rtB .
o4tcftqtac = ( rtB . op5u32jxd5 && rtB . mcfqqro3yx ) ; rtB . ltsx3n5zdu = (
rtB . o4tcftqtac && rtB . kpqmbuvakn ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) )
{ rtB . lyoyjedfok = rtDW . cij1nabjqb ; rtB . oau4kezwhg = rtDW . f2rohdyfa1
; } rtB . boyv4s5afc = ! rtB . jlwtkodtto ; rtB . azo30uuht1 = ! rtB .
boyv4s5afc ; rtB . c1g3ads4q0 = ( rtB . lyoyjedfok && rtB . azo30uuht1 ) ;
rtB . hatod3ncks = ( rtB . mvuhtplreq || rtB . c1g3ads4q0 ) ; rtB .
ciusb0buvd = ( rtB . hatod3ncks && rtB . ppuj1bwkov ) ; rtB . apamggybyk = !
rtB . d1twsnn020 ; rtB . cfpijkucsv = ! rtB . apamggybyk ; rtB . m1jm05jua2 =
( rtB . oau4kezwhg && rtB . cfpijkucsv ) ; rtB . nx1hwu0e2s = ( rtB .
o4uojz5k3z || rtB . m1jm05jua2 ) ; rtB . ceheylkdxq = ( rtB . nx1hwu0e2s &&
rtB . lbqfu0zivg ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . neeriy4x30
= rtDW . ie01l5vavi ; rtB . m2f4zuehen = ! rtB . neeriy4x30 ; rtB .
gozqu2dfcc = rtDW . nwhg3zqrsl ; rtB . ncgqqozkfq = ! rtB . gozqu2dfcc ; }
rtB . fctoid3hwh = ( rtB . prgykcukfr && rtB . m2f4zuehen ) ; rtB .
pg4ar2tqmf = ( rtB . fctoid3hwh && rtB . ata4hlycnw ) ; rtB . lrkok4l2xf = (
rtB . ceheylkdxq || rtB . dvwxwugcj3 || rtB . ciusb0buvd ) ; rtB . jkf3i3tfdr
= ( rtB . ebxp2katk3 && rtB . ncgqqozkfq ) ; rtB . kvb4ukhksk = ( rtB .
jkf3i3tfdr && rtB . pjepo1a1my ) ; rtB . falpkdmxd5 = ( rtB . a0it33vpv3 ||
rtB . gr4vjiot5p || rtB . lz0cksews5 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) )
{ rtB . jooyjga1t0 = rtDW . mv54qvcn5q ; rtB . atq3qhvwac = rtDW . fjxk3uaez4
; rtB . ejjatvy0ji = rtDW . i21ukrhdhv ; rtB . aajgk5zxa4 = rtDW . eu3dowbkcr
; } rtB . otwdzewvva = ( rtB . f1lqhq0qme == rtB . puiac3eowe ) ; rtB .
ol01gtdpue = ( rtB . puiac3eowe == rtB . a0pjszrvty ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . jlqpuiagan = rtDW . oxmyy55gc0 ; rtB . hqebmcdiwg =
rtDW . abpj3txsdh ; rtB . pdhui3he3f = rtDW . e0oousi5de ; rtB . pd2hdhthkb =
rtDW . a0eu1zkv1e ; rtB . kgrtiplduv = rtDW . n1z1agzu3x ; rtB . dsiekvvj5d =
rtDW . nngs4wq1em ; rtB . lxbpdmf1sv = rtDW . aynri5lox1 ; rtB . pqhmamvv3w =
rtDW . lmvrvbjthf ; rtB . e5wdsnxko5 = rtDW . bxrofmksin ; rtB . igt3ds4gub =
rtDW . btgc2mhzkb ; rtB . ploid00iph = rtDW . o55xttyljm ; rtB . fclwcxjui1 =
rtDW . ev3rm05gtu ; rtB . o2aewyydjc = rtDW . e1zljawh5b ; rtB . dfugdqxz5n =
rtDW . dv30vpwozb ; rtB . ohtsbgtzyy = rtDW . fe4qsttuig ; rtB . i5wvkswgxi =
rtDW . gijbul5bro ; } rtB . nq0zak3ut2 = ( rtB . pc1hlddxpc == rtB .
lbk4fkoopd ) ; rtB . ivzavcovfx = ( rtB . lbk4fkoopd == rtB . mlwc4saci2 ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . m5czbh1ofs = rtDW . fx2ubbeual ;
rtB . omlumxq3kv = rtDW . kdodmy0lc5 ; rtB . ghyvk3fl1r = rtDW . p2lcbu1i1l ;
rtB . obatadlmps = rtDW . cstjzr5zdb ; rtB . cnsaptvv05 = rtDW . o5i22qdh2m ;
rtB . mzp2yx5eyd = rtDW . nhuixzwcbu ; rtB . dsokhkpxss = rtDW . k4yt0gbyvq ;
rtB . m34qzw1e3z = rtDW . hzgwuaj0dk ; rtB . meotz4mdti = rtDW . p2kf00znzb ;
rtB . fw2ws32qdy = rtDW . mitpuusdiu ; rtB . iyjx4tiruw = rtDW . gl5n5tgtur ;
rtB . eff0y2ogr1 = rtDW . gm3eatvpbt ; rtB . corxgi5bda = rtDW . faqnzz3umd ;
rtB . d0qqsnjj4o = rtDW . iy0b4tzwiz ; rtB . gukq3uey0u = rtDW . dpmou3nofz ;
rtB . a4rgreqjnk = rtDW . oad2ixcyjj ; rtB . cg2mdcmr4c = rtDW . mwwno0g4te ;
rtB . lk3rhzaqlz = rtDW . laawtmho0w ; rtB . ksqaf3kqa5 = rtDW . aifuwgprs1 ;
rtB . mrzx2rambn = rtDW . j2ytkxdub5 ; rtB . fp5ajke5t2 = rtDW . efpieuuisx ;
rtB . gxlf1oipxj = rtDW . gnbbybfox4 ; rtB . frpynadqtk = rtDW . inaoot3v53 ;
rtB . ef0231xcnk = rtDW . kzrubw1lpo ; rtB . b0f344lzrq = rtDW . aqriwgcdaa ;
rtB . ned0nfq0np = rtDW . akfyoog544 ; rtB . cwviljcsth = rtDW . dloemu5bxh ;
rtB . h2xdkjvcrm = rtDW . llzhlzgbyq ; rtB . lojzaxa5ck = rtDW . gk45nsnm5f ;
rtB . kgd01apk54 = ! rtB . lojzaxa5ck ; } rtB . jjbiyb2p5u = ( rtB .
hk401gnps3 == rtB . arrxdd5ji0 ) ; rtB . pknlqlt04d = ( rtB . arrxdd5ji0 ==
rtB . cara0q1dz3 ) ; rtB . ahqtqmpz4q = ( rtB . falpkdmxd5 && rtB .
kgd01apk54 ) ; if ( rtB . cxt03zh1fr ) { rtB . egfmkflaqk = rtB . mf5uzgcpk4
; } else { if ( rtB . kl4albs201 ) { rtB . bufwny1i4t = rtB . ffflpratew ; }
else { rtB . bufwny1i4t = rtB . olvyzc5uqq ; } rtB . egfmkflaqk = rtB .
bufwny1i4t ; } rtB . jtqp11xllr = ( ( rtB . laxu4i4bvg != 0.0 ) && ( rtB .
nuqqjqizth != 0.0 ) && ( rtB . mfzwdgo525 != 0.0 ) ) ; rtB . ccbu3cclyx = 0.0
* rtB . aeayidxpl0 ; rtB . c1um5xv1ty = rtB . ccbu3cclyx / rtP .
u0_Value_n4wszjhcf4 ; rtB . jzo4koey54 = 0.0 * rtB . aeayidxpl0 ; rtB .
juzy3caoba = rtB . jzo4koey54 / rtP . u0_Value_n4wszjhcf4 ; rtB . brxyijfamz
= 0.0 * rtB . a5yycj1ihr ; rtB . cnuipepl2b = 0.0 * rtB . a5yycj1ihr ; rtB .
afahojfymd = rtB . ccbu3cclyx + rtB . brxyijfamz ; rtB . bhlwdicen0 = rtB .
afahojfymd / rtP . u7_Value_i1x1s1u3em ; rtB . c3utqhx03j = rtB . jzo4koey54
+ rtB . cnuipepl2b ; rtB . kndmkovixv = rtB . c3utqhx03j / rtP .
u7_Value_i1x1s1u3em ; rtB . jnhnyt34ia = rtB . brxyijfamz / rtP .
u2_Value_ep1nn1uf4v ; rtB . n0jb2fms0h = rtB . cnuipepl2b / rtP .
u2_Value_ep1nn1uf4v ; rtB . brsilsdiuk = ( rtB . gaofeahffg && rtB .
kl4albs201 ) ; rtB . ehmwguwdm1 = ( rtB . emuxatj33n && rtB . ff4zi1sk4i ) ;
if ( rtB . dlq2giet1z ) { rtB . etsymupyma = rtP . u7_Value_fs2w21cd1e ; }
else { if ( rtB . cxt03zh1fr ) { rtB . hkgzscpiv2 = ( rtB . brsilsdiuk || rtB
. ehmwguwdm1 ) ; rtB . cvtboiuyt3 = ( rtB . hkgzscpiv2 && rtB . cxt03zh1fr )
; rtB . apq1fj4pll = rtB . cvtboiuyt3 ; } else { if ( rtB . kl4albs201 ) {
rtB . ckc15kwo03 = rtB . brsilsdiuk ; } else { rtB . ckc15kwo03 = rtB .
ehmwguwdm1 ; } rtB . apq1fj4pll = rtB . ckc15kwo03 ; } rtB . etsymupyma = rtB
. apq1fj4pll ; } rtB . e4dgxmib5v = ( rtB . hd1wzosq4k && rtB . ltbf4lyeyj &&
rtB . kg0ylfr5bw ) ; rtB . gxmf2wkhvl = ( rtB . hcjwk0r3h0 && rtB .
edayvamwde && rtB . e0h2bmnout ) ; if ( rtB . n041v55ew1 ) { rtB . hbjzb3e12t
= rtB . iw0wqwlmgy ; } else { if ( rtB . cdxhxlj30g ) { rtB . nqxkmrha4c =
rtB . d4iy43twxg ; } else { rtB . nqxkmrha4c = rtB . imik4ijlly ; } rtB .
hbjzb3e12t = rtB . nqxkmrha4c ; } rtB . gztjgrnsox = ( ( rtB . gfj43a5l4q !=
0.0 ) && ( rtB . gxncten00d != 0.0 ) && ( rtB . ezfrshqts0 != 0.0 ) ) ; rtB .
obckxa1f2t = 0.0 * rtB . cigvk2tavt ; rtB . j2ltoc0vad = rtB . obckxa1f2t /
rtP . u0_Value_ivgpianhec ; rtB . hatklhe3ky = 0.0 * rtB . cigvk2tavt ; rtB .
bybgbhi43y = rtB . hatklhe3ky / rtP . u0_Value_ivgpianhec ; rtB . g54sekjsyc
= 0.0 * rtB . esximhugp5 ; rtB . bb1sclmk1l = 0.0 * rtB . esximhugp5 ; rtB .
lfhqfmbvny = rtB . obckxa1f2t + rtB . g54sekjsyc ; rtB . cnyryrztzu = rtB .
lfhqfmbvny / rtP . u7_Value_ethodxai01 ; rtB . gwmcic45sv = rtB . hatklhe3ky
+ rtB . bb1sclmk1l ; rtB . pdaor5f2td = rtB . gwmcic45sv / rtP .
u7_Value_ethodxai01 ; rtB . cky0wb13f1 = rtB . g54sekjsyc / rtP .
u2_Value_juxkcieu5p ; rtB . nlld0wojhv = rtB . bb1sclmk1l / rtP .
u2_Value_juxkcieu5p ; rtB . jjyxpa52tj = ( rtB . ohj4mhalzc && rtB .
cdxhxlj30g ) ; rtB . lwpwi3crzp = ( rtB . oiohnnmqqn && rtB . gz04icgcdm ) ;
if ( rtB . do24p4hxxw ) { rtB . eu5mqrgpa0 = rtP . u7_Value_knfw4nz5wz ; }
else { if ( rtB . n041v55ew1 ) { rtB . b55gxrw1l4 = ( rtB . jjyxpa52tj || rtB
. lwpwi3crzp ) ; rtB . bqdkk55nzm = ( rtB . b55gxrw1l4 && rtB . n041v55ew1 )
; rtB . pah0imnj15 = rtB . bqdkk55nzm ; } else { if ( rtB . cdxhxlj30g ) {
rtB . k3w1jtomw2 = rtB . jjyxpa52tj ; } else { rtB . k3w1jtomw2 = rtB .
lwpwi3crzp ; } rtB . pah0imnj15 = rtB . k3w1jtomw2 ; } rtB . eu5mqrgpa0 = rtB
. pah0imnj15 ; } rtB . kyrcfwy3jq = ( rtB . oounn0ufba && rtB . aff2utpmc0 &&
rtB . eadzx3422g ) ; rtB . lighzb1t5v = ( rtB . bi5bqgqujx && rtB .
djctpxyjgr && rtB . dwj1l4sg14 ) ; rtB . mhjo2hrrn3 = ( rtB . cm2lou5sl1 &&
rtB . kdch3loprk && rtB . jqpqhta4xx ) ; rtB . kqlpgzikw0 = rtB . ctkxwfjqo4
- rtB . nyn1hlelu1 ; rtB . cof1misrun = rtB . kqlpgzikw0 * rtB . kqlpgzikw0 ;
rtB . ew0xvkobic = rtB . dl425wbmtl - rtB . g3cditrnls ; rtB . c3qck32np0 =
rtB . ew0xvkobic * rtB . ew0xvkobic ; rtB . n5mn5tm0om = rtB . cof1misrun +
rtB . c3qck32np0 ; rtB . hil4dhh1nx = muDoubleScalarSqrt ( rtB . n5mn5tm0om )
; rtB . pftaz33yqt = ( rtB . hil4dhh1nx > rtB . m1u52pllya ) ; rtB .
nu453zsyv0 = rtB . kqlpgzikw0 * rtB . mxh01l3oxv ; rtB . icfcwgemtg = rtB .
ew0xvkobic * rtB . mxh01l3oxv ; rtB . ixl3jfd1k5 = rtB . nyn1hlelu1 - rtB .
c5dodwizkd ; rtB . hceofypdty = rtB . ixl3jfd1k5 * rtB . mxh01l3oxv ; rtB .
ewhdvtxrxp = rtB . g3cditrnls - rtB . pnlxv4w3fb ; rtB . ailhdf0gr4 = rtB .
ewhdvtxrxp * rtB . mxh01l3oxv ; rtB . mbnfgfwr0t = rtB . c5dodwizkd - rtB .
ctkxwfjqo4 ; rtB . b5zycfvydx = rtB . mbnfgfwr0t * rtB . mxh01l3oxv ; rtB .
my3mzbnpai = rtB . pnlxv4w3fb - rtB . dl425wbmtl ; rtB . bmd4c1n4ey = rtB .
my3mzbnpai * rtB . mxh01l3oxv ; rtB . ikvlmufrmh = rtB . ixl3jfd1k5 * rtB .
ixl3jfd1k5 ; rtB . hhihvszqm2 = rtB . ewhdvtxrxp * rtB . ewhdvtxrxp ; rtB .
olt1i5uozx = rtB . ikvlmufrmh + rtB . hhihvszqm2 ; rtB . jer00sa4i4 =
muDoubleScalarSqrt ( rtB . olt1i5uozx ) ; rtB . fsrsupxuu2 = ( rtB .
jer00sa4i4 > rtB . m1u52pllya ) ; rtB . hhlg1sckd5 = rtB . mbnfgfwr0t * rtB .
mbnfgfwr0t ; rtB . iuqxcvdrye = rtB . my3mzbnpai * rtB . my3mzbnpai ; rtB .
bvx342cww0 = rtB . hhlg1sckd5 + rtB . iuqxcvdrye ; rtB . psgm15eh2n =
muDoubleScalarSqrt ( rtB . bvx342cww0 ) ; rtB . kjrgrsjlug = ( rtB .
psgm15eh2n > rtB . m1u52pllya ) ; rtB . elzvr5rop3 = muDoubleScalarCos ( rtB
. janxwpkdcj [ 12 ] ) ; rtB . m1lsh0lnip = rtB . lpgwkwyj4h [ 12 ] * rtB .
elzvr5rop3 ; rtB . c4brd1qjs2 = rtB . m1lsh0lnip * rtB . m1lsh0lnip ; rtB .
gks0eufssw = muDoubleScalarSin ( rtB . janxwpkdcj [ 12 ] ) ; rtB . bnvn0ytxls
= rtB . gks0eufssw * rtB . lpgwkwyj4h [ 12 ] ; rtB . pt1sg1exvn = rtB .
bnvn0ytxls * rtB . bnvn0ytxls ; rtB . njrdlpylfk = rtB . c4brd1qjs2 + rtB .
pt1sg1exvn ; rtB . p33qxpi1jw = muDoubleScalarSqrt ( rtB . njrdlpylfk ) ; rtB
. k1lga55zvh = ( rtB . p33qxpi1jw > rtP . u9_Value_bckbcfivxe ) ; rtB .
cqvtpm0x4a = muDoubleScalarCos ( rtB . janxwpkdcj [ 11 ] ) ; rtB . insriwr0i1
= rtB . lpgwkwyj4h [ 11 ] * rtB . cqvtpm0x4a ; rtB . dsd2pyqsrk = rtB .
insriwr0i1 * rtB . insriwr0i1 ; rtB . khrymy4non = muDoubleScalarSin ( rtB .
janxwpkdcj [ 11 ] ) ; rtB . ja1qciubyg = rtB . khrymy4non * rtB . lpgwkwyj4h
[ 11 ] ; rtB . g3cj1tdbxn = rtB . ja1qciubyg * rtB . ja1qciubyg ; rtB .
ifqmnzuuvn = rtB . dsd2pyqsrk + rtB . g3cj1tdbxn ; rtB . lebhxcaqri =
muDoubleScalarSqrt ( rtB . ifqmnzuuvn ) ; rtB . lgfnmmi2vg = ( rtB .
lebhxcaqri > rtP . u9_Value_bckbcfivxe ) ; rtB . nvhonwozpj =
muDoubleScalarCos ( rtB . janxwpkdcj [ 13 ] ) ; rtB . mjfesp5yxr = rtB .
lpgwkwyj4h [ 13 ] * rtB . nvhonwozpj ; rtB . dh0riwivwz = rtB . mjfesp5yxr *
rtB . mjfesp5yxr ; rtB . glg0vqjd0s = muDoubleScalarSin ( rtB . janxwpkdcj [
13 ] ) ; rtB . aymfjhbglb = rtB . glg0vqjd0s * rtB . lpgwkwyj4h [ 13 ] ; rtB
. juf5ekmdsa = rtB . aymfjhbglb * rtB . aymfjhbglb ; rtB . nbqoe20kek = rtB .
dh0riwivwz + rtB . juf5ekmdsa ; rtB . lbdltllmbs = muDoubleScalarSqrt ( rtB .
nbqoe20kek ) ; rtB . limfoxymck = ( rtB . lbdltllmbs > rtP .
u9_Value_bckbcfivxe ) ; rtB . icd2tocsfj = ( rtB . lgfnmmi2vg && rtB .
k1lga55zvh && rtB . limfoxymck ) ; rtB . gjlat4imxj = rtB . insriwr0i1 - rtB
. m1lsh0lnip ; rtB . ibuzqldlrt = rtB . gjlat4imxj * rtB . gjlat4imxj ; rtB .
hn3be2fl2x = rtB . ja1qciubyg - rtB . bnvn0ytxls ; rtB . npqs3ycezr = rtB .
hn3be2fl2x * rtB . hn3be2fl2x ; rtB . mjdcyp0d12 = rtB . ibuzqldlrt + rtB .
npqs3ycezr ; rtB . g5i04gp1tj = muDoubleScalarSqrt ( rtB . mjdcyp0d12 ) ; rtB
. c5sfrcedzj = ( rtB . g5i04gp1tj > rtB . oswwhwdyvs ) ; rtB . dapuz4k3co =
rtB . gjlat4imxj * rtB . ojva1ku4ei ; rtB . bwjo2zfwzn = rtB . hn3be2fl2x *
rtB . ojva1ku4ei ; rtB . a5uoyeyotf = rtB . insriwr0i1 * rtB . ojva1ku4ei ;
rtB . mlosq5bue4 = rtB . ja1qciubyg * rtB . ojva1ku4ei ; rtB . iczkkrqhjn =
rtB . m1lsh0lnip * rtB . ojva1ku4ei ; rtB . p4ckg1zyh0 = rtB . bnvn0ytxls *
rtB . ojva1ku4ei ; rtB . nvegjh33zg = rtB . mjfesp5yxr * rtB . ojva1ku4ei ;
rtB . fv03qo0isx = rtB . aymfjhbglb * rtB . ojva1ku4ei ; rtB . h5vyeqg1dc =
rtB . m1lsh0lnip - rtB . mjfesp5yxr ; rtB . odw2ubspbe = rtB . h5vyeqg1dc *
rtB . ojva1ku4ei ; rtB . ko22pl2rdm = rtB . bnvn0ytxls - rtB . aymfjhbglb ;
rtB . beag5wk3nf = rtB . ko22pl2rdm * rtB . ojva1ku4ei ; rtB . frib3hengq =
rtB . mjfesp5yxr - rtB . insriwr0i1 ; rtB . e2ee13yttt = rtB . frib3hengq *
rtB . ojva1ku4ei ; rtB . p1xpps3vuz = rtB . aymfjhbglb - rtB . ja1qciubyg ;
rtB . ermewmadgf = rtB . p1xpps3vuz * rtB . ojva1ku4ei ; rtB . ogbeo0j0p3 =
rtB . h5vyeqg1dc * rtB . h5vyeqg1dc ; rtB . ia1f4hedef = rtB . ko22pl2rdm *
rtB . ko22pl2rdm ; rtB . bhdvhzp1fp = rtB . ogbeo0j0p3 + rtB . ia1f4hedef ;
rtB . gct1uibod4 = muDoubleScalarSqrt ( rtB . bhdvhzp1fp ) ; rtB . gf5ic5mfm1
= ( rtB . gct1uibod4 > rtB . oswwhwdyvs ) ; rtB . mfwhlt1qpl = rtB .
frib3hengq * rtB . frib3hengq ; rtB . ppwzady4nq = rtB . p1xpps3vuz * rtB .
p1xpps3vuz ; rtB . kqwgmx2inc = rtB . mfwhlt1qpl + rtB . ppwzady4nq ; rtB .
c0hhfjfw4k = muDoubleScalarSqrt ( rtB . kqwgmx2inc ) ; rtB . aypnp2zmwu = (
rtB . c0hhfjfw4k > rtB . oswwhwdyvs ) ; rtB . dkdtrq0tlb = muDoubleScalarCos
( rtB . janxwpkdcj [ 8 ] ) ; rtB . d3ileurysz = rtB . lpgwkwyj4h [ 8 ] * rtB
. dkdtrq0tlb ; rtB . lwo0dgman1 = rtB . d3ileurysz * rtB . d3ileurysz ; rtB .
lqwkhxd1bh = muDoubleScalarSin ( rtB . janxwpkdcj [ 8 ] ) ; rtB . pc3irt5oiu
= rtB . lqwkhxd1bh * rtB . lpgwkwyj4h [ 8 ] ; rtB . lw0gtsos3j = rtB .
pc3irt5oiu * rtB . pc3irt5oiu ; rtB . nuxzlmvkd1 = rtB . lwo0dgman1 + rtB .
lw0gtsos3j ; rtB . gz0vsqauol = muDoubleScalarSqrt ( rtB . nuxzlmvkd1 ) ; rtB
. ib0pvmnojb = ( rtB . gz0vsqauol > rtP . u9_Value_kcdi3r01mc ) ; rtB .
bveumtrzo1 = muDoubleScalarCos ( rtB . janxwpkdcj [ 7 ] ) ; rtB . ka000an4jy
= rtB . lpgwkwyj4h [ 7 ] * rtB . bveumtrzo1 ; rtB . llylkq0z2v = rtB .
ka000an4jy * rtB . ka000an4jy ; rtB . kvrtbmykda = muDoubleScalarSin ( rtB .
janxwpkdcj [ 7 ] ) ; rtB . mibm0q5lsc = rtB . kvrtbmykda * rtB . lpgwkwyj4h [
7 ] ; rtB . oga1hszwm0 = rtB . mibm0q5lsc * rtB . mibm0q5lsc ; rtB .
fdciadhpou = rtB . llylkq0z2v + rtB . oga1hszwm0 ; rtB . c124ibpfmk =
muDoubleScalarSqrt ( rtB . fdciadhpou ) ; rtB . asubrdaolo = ( rtB .
c124ibpfmk > rtP . u9_Value_kcdi3r01mc ) ; rtB . oxjnnoxbbj =
muDoubleScalarCos ( rtB . janxwpkdcj [ 9 ] ) ; rtB . nh5cjuqkq0 = rtB .
lpgwkwyj4h [ 9 ] * rtB . oxjnnoxbbj ; rtB . dcljwpr0of = rtB . nh5cjuqkq0 *
rtB . nh5cjuqkq0 ; rtB . g5vavgi3ao = muDoubleScalarSin ( rtB . janxwpkdcj [
9 ] ) ; rtB . d3t5p0pjfz = rtB . g5vavgi3ao * rtB . lpgwkwyj4h [ 9 ] ; rtB .
mclnv2chnz = rtB . d3t5p0pjfz * rtB . d3t5p0pjfz ; rtB . ixiuuvsrkd = rtB .
dcljwpr0of + rtB . mclnv2chnz ; rtB . deqr01wfh4 = muDoubleScalarSqrt ( rtB .
ixiuuvsrkd ) ; rtB . jmqgc2plxz = ( rtB . deqr01wfh4 > rtP .
u9_Value_kcdi3r01mc ) ; rtB . dyadhotlos = ( rtB . asubrdaolo && rtB .
ib0pvmnojb && rtB . jmqgc2plxz ) ; rtB . g4trokyzfj = rtB . ka000an4jy - rtB
. d3ileurysz ; rtB . nf2comzlzv = rtB . g4trokyzfj * rtB . g4trokyzfj ; rtB .
jwv4xg1mrp = rtB . mibm0q5lsc - rtB . pc3irt5oiu ; rtB . dnysiliydz = rtB .
jwv4xg1mrp * rtB . jwv4xg1mrp ; rtB . mtceeb1qzz = rtB . nf2comzlzv + rtB .
dnysiliydz ; rtB . kgds3bqy2z = muDoubleScalarSqrt ( rtB . mtceeb1qzz ) ; rtB
. avkeew1pr5 = ( rtB . kgds3bqy2z > rtB . parjunhjil ) ; rtB . ektiyxerjw =
rtB . g4trokyzfj * rtB . cr5is1bebm ; rtB . dyd1rfeik5 = rtB . jwv4xg1mrp *
rtB . cr5is1bebm ; rtB . ec3muigggy = rtB . ka000an4jy * rtB . cr5is1bebm ;
rtB . cvkdusf3n5 = rtB . mibm0q5lsc * rtB . cr5is1bebm ; rtB . bp3r01ovhm =
rtB . d3ileurysz * rtB . cr5is1bebm ; rtB . pdkc5n1zyw = rtB . pc3irt5oiu *
rtB . cr5is1bebm ; rtB . ljktjlr15i = rtB . nh5cjuqkq0 * rtB . cr5is1bebm ;
rtB . j3k2v5sks2 = rtB . d3t5p0pjfz * rtB . cr5is1bebm ; rtB . fym11g5moo =
rtB . d3ileurysz - rtB . nh5cjuqkq0 ; rtB . hwdn4io21s = rtB . fym11g5moo *
rtB . cr5is1bebm ; rtB . maocl10srw = rtB . pc3irt5oiu - rtB . d3t5p0pjfz ;
rtB . mspl2sgmhg = rtB . maocl10srw * rtB . cr5is1bebm ; rtB . kp0uqu1tgc =
rtB . nh5cjuqkq0 - rtB . ka000an4jy ; rtB . fcq0pp0yr2 = rtB . kp0uqu1tgc *
rtB . cr5is1bebm ; rtB . pyhjdjnsfn = rtB . d3t5p0pjfz - rtB . mibm0q5lsc ;
rtB . fxrzrap4g4 = rtB . pyhjdjnsfn * rtB . cr5is1bebm ; rtB . fddim301xd =
rtB . fym11g5moo * rtB . fym11g5moo ; rtB . lzd0vtyirs = rtB . maocl10srw *
rtB . maocl10srw ; rtB . muykvna3vm = rtB . fddim301xd + rtB . lzd0vtyirs ;
rtB . dgudkz0ujb = muDoubleScalarSqrt ( rtB . muykvna3vm ) ; rtB . dltse5411b
= ( rtB . dgudkz0ujb > rtB . parjunhjil ) ; rtB . be2quzfs3f = rtB .
kp0uqu1tgc * rtB . kp0uqu1tgc ; rtB . jkoeemyhwh = rtB . pyhjdjnsfn * rtB .
pyhjdjnsfn ; rtB . eeb44f0srf = rtB . be2quzfs3f + rtB . jkoeemyhwh ; rtB .
j5d0rsytt1 = muDoubleScalarSqrt ( rtB . eeb44f0srf ) ; rtB . ee0em1air2 = (
rtB . j5d0rsytt1 > rtB . parjunhjil ) ; rtB . jq1fcj3pnv = muDoubleScalarCos
( rtB . l5lnbu5gps [ 10 ] ) ; rtB . j3ncxl34ru = rtB . bzh5umwrxf [ 10 ] *
rtB . jq1fcj3pnv ; rtB . mrpc0xrhwn = rtB . j3ncxl34ru * rtB . j3ncxl34ru ;
rtB . ewreavwldx = muDoubleScalarSin ( rtB . l5lnbu5gps [ 10 ] ) ; rtB .
ap54mpgdpc = rtB . ewreavwldx * rtB . bzh5umwrxf [ 10 ] ; rtB . lhs3lt3dkr =
rtB . ap54mpgdpc * rtB . ap54mpgdpc ; rtB . flgyji0f3k = rtB . mrpc0xrhwn +
rtB . lhs3lt3dkr ; rtB . cpufgo1rcj = muDoubleScalarSqrt ( rtB . flgyji0f3k )
; rtB . dl4o1bc0vn = ( rtB . cpufgo1rcj > rtP . u9_Value_l23zhc0rg2 ) ; rtB .
h0ywrrssdv = muDoubleScalarCos ( rtB . l5lnbu5gps [ 6 ] ) ; rtB . jwndcypyf4
= rtB . bzh5umwrxf [ 6 ] * rtB . h0ywrrssdv ; rtB . db2ibtr11v = rtB .
jwndcypyf4 * rtB . jwndcypyf4 ; rtB . nioayrbb30 = muDoubleScalarSin ( rtB .
l5lnbu5gps [ 6 ] ) ; rtB . kboi0bewqt = rtB . nioayrbb30 * rtB . bzh5umwrxf [
6 ] ; rtB . i5f1jlosno = rtB . kboi0bewqt * rtB . kboi0bewqt ; rtB .
buern3uwf3 = rtB . db2ibtr11v + rtB . i5f1jlosno ; rtB . ncycwlnruw =
muDoubleScalarSqrt ( rtB . buern3uwf3 ) ; rtB . a2ujqnmzbs = ( rtB .
ncycwlnruw > rtP . u9_Value_l23zhc0rg2 ) ; rtB . llaaxlqigi = ( rtB .
a2ujqnmzbs && rtB . dl4o1bc0vn && rtB . joggdl4v5p ) ; rtB . p0r1fpm5ij = rtB
. jwndcypyf4 - rtB . j3ncxl34ru ; rtB . ob12oq2flo = rtB . p0r1fpm5ij * rtB .
p0r1fpm5ij ; rtB . i1b2gnf0w1 = rtB . kboi0bewqt - rtB . ap54mpgdpc ; rtB .
ht0hk0jrmq = rtB . i1b2gnf0w1 * rtB . i1b2gnf0w1 ; rtB . ikzmiljg4o = rtB .
ob12oq2flo + rtB . ht0hk0jrmq ; rtB . aec4zkcbvb = muDoubleScalarSqrt ( rtB .
ikzmiljg4o ) ; rtB . fvmmkwkv4m = ( rtB . aec4zkcbvb > rtB . gyjy11e32a ) ;
rtB . lfa21530ao = rtB . p0r1fpm5ij * rtB . howw1mvyyl ; rtB . hbvvkqaw5o =
rtB . i1b2gnf0w1 * rtB . howw1mvyyl ; rtB . pz2k3jltfa = rtB . jwndcypyf4 *
rtB . howw1mvyyl ; rtB . gowzdlfjhz = rtB . kboi0bewqt * rtB . howw1mvyyl ;
rtB . b2qz4pss45 = rtB . j3ncxl34ru * rtB . howw1mvyyl ; rtB . mkmmuynnwa =
rtB . ap54mpgdpc * rtB . howw1mvyyl ; rtB . bwchvkc1h3 = rtB . j3ncxl34ru ;
rtB . ma3j2o00te = rtB . bwchvkc1h3 * rtB . howw1mvyyl ; rtB . dmte5wxgdx =
rtB . ap54mpgdpc ; rtB . jxiy0pyt42 = rtB . dmte5wxgdx * rtB . howw1mvyyl ;
rtB . o0juhfa4fu = 0.0 - rtB . jwndcypyf4 ; rtB . e3apfupfra = rtB .
o0juhfa4fu * rtB . howw1mvyyl ; rtB . dov4nynfnd = 0.0 - rtB . kboi0bewqt ;
rtB . ngbncrob00 = rtB . dov4nynfnd * rtB . howw1mvyyl ; rtB . jreiugp3ij =
rtB . bwchvkc1h3 * rtB . bwchvkc1h3 ; rtB . nn3p3uglbp = rtB . dmte5wxgdx *
rtB . dmte5wxgdx ; rtB . jakywoof0u = rtB . jreiugp3ij + rtB . nn3p3uglbp ;
rtB . dkpxvgnqa5 = muDoubleScalarSqrt ( rtB . jakywoof0u ) ; rtB . fcbtmqvoq2
= ( rtB . dkpxvgnqa5 > rtB . gyjy11e32a ) ; rtB . hpciqvo4dj = rtB .
o0juhfa4fu * rtB . o0juhfa4fu ; rtB . g3wz1sktlu = rtB . dov4nynfnd * rtB .
dov4nynfnd ; rtB . e3fiqjbz3e = rtB . hpciqvo4dj + rtB . g3wz1sktlu ; rtB .
cflzm4ynwb = muDoubleScalarSqrt ( rtB . e3fiqjbz3e ) ; rtB . aqkt4i4utz = (
rtB . cflzm4ynwb > rtB . gyjy11e32a ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) )
{ rtB . em1c3syfzm = rtDW . bd5awmqnm1 ; } rtB . f2wkmmwt0i = rtB .
iczkkrqhjn * rtP . u_Value_gqdhm5pmxp ; rtB . jdzpmugq4u = rtB . p4ckg1zyh0 *
rtP . u_Value_i1olrqvpfb ; rtB . n2uloplz4l = rtB . f2wkmmwt0i - rtB .
jdzpmugq4u ; rtB . niaadzdnur = rtB . nvegjh33zg * rtP . u_Value_gqdhm5pmxp ;
rtB . gks02qi0q5 = rtB . fv03qo0isx * rtP . u_Value_hadlzqtpwh ; rtB .
ffriiqazaj = rtB . niaadzdnur - rtB . gks02qi0q5 ; rtB . iz4t4sfo4o = rtB .
n2uloplz4l + rtB . ffriiqazaj ; rtB . o43ijnghxs = rtB . a5uoyeyotf + rtB .
iz4t4sfo4o ; rtB . igwiijrpw0 = rtB . o43ijnghxs * rtP . u7_Value_fpkrrznsoo
; rtB . hjrej4vy2f = rtB . igwiijrpw0 * rtB . igwiijrpw0 ; rtB . b4ybxs4nwt =
rtB . p4ckg1zyh0 * rtP . u_Value_gqdhm5pmxp ; rtB . in4j4ynjy5 = rtB .
iczkkrqhjn * rtP . u_Value_i1olrqvpfb ; rtB . nysoveocam = rtB . b4ybxs4nwt +
rtB . in4j4ynjy5 ; rtB . gr3pwcz1t2 = rtB . fv03qo0isx * rtP .
u_Value_gqdhm5pmxp ; rtB . enyts3xbn1 = rtB . nvegjh33zg * rtP .
u_Value_hadlzqtpwh ; rtB . fsvltbb1k5 = rtB . gr3pwcz1t2 + rtB . enyts3xbn1 ;
rtB . mlzoyxymia = rtB . nysoveocam + rtB . fsvltbb1k5 ; rtB . bm01yihenb =
rtB . mlosq5bue4 + rtB . mlzoyxymia ; rtB . m2ryyof1m0 = rtB . bm01yihenb *
rtP . u7_Value_fpkrrznsoo ; rtB . ik11sfkocw = rtB . m2ryyof1m0 * rtB .
m2ryyof1m0 ; rtB . d504vm50zn = rtB . hjrej4vy2f + rtB . ik11sfkocw ; rtB .
jrzqkq315r = muDoubleScalarSqrt ( rtB . d504vm50zn ) ; if ( rtP .
Constant_Value_guugmxnagn > rtP . u_Threshold_bwuxop0ksu ) { rtB . mibcvflqrv
= ( rtB . jrzqkq315r < rtP . Constant3_Value_indm0avrul ) ; rtB . fzm0twu5ml
= rtB . mibcvflqrv ; } else { rtB . adhu0nacfx = ( rtB . jrzqkq315r > rtP .
Constant3_Value_indm0avrul ) ; rtB . fzm0twu5ml = rtB . adhu0nacfx ; } rtB .
kge5opuami = ( rtB . fzm0twu5ml && rtB . jj5lcza05y ) ; if ( rtB . nhcbwj51h3
) { rtB . bsh0vnffms = rtB . kge5opuami ; } else { rtB . fbm3xrvkmo = ( rtB .
kge5opuami && rtB . pfegpiaswj ) ; rtB . bsh0vnffms = rtB . fbm3xrvkmo ; } if
( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . ieccwge4ie = rtDW . a0zxdwo0kv ;
rtB . izlsn3ai5o = ! rtB . ieccwge4ie ; } if ( rtP .
Constant4_Value_jxjqabmaaf > rtP . u_Threshold_mcwwhq5a23 ) { rtB .
bfvvmmgliu = ( rtB . bsh0vnffms && rtB . izlsn3ai5o ) ; rtB . pwhtqllfp2 =
rtB . bfvvmmgliu ; } else { rtB . pwhtqllfp2 = rtB . bsh0vnffms ; } rtB .
o4zwzemekh = ( rtP . Constant3_Value_m2zq2bgyk2 != 0.0 ) ; rtB . pob40ryqad =
ssGetT ( rtS ) ; on20slldft ( rtS , rtB . on1kf2nxht , rtB . o4zwzemekh , rtB
. pob40ryqad , rtP . T1_delay_cjf1vpyxvz , & rtB . l0mam0lcui , & rtDW .
l0mam0lcui , & rtP . l0mam0lcui , & rtPrevZCX . l0mam0lcui ) ; bhc3qgnuic (
rtS , rtB . a2fiv1shol , rtB . o4zwzemekh , rtB . pob40ryqad , rtP .
T1_delay_cjf1vpyxvz , & rtB . ck45m3cw5y , & rtDW . ck45m3cw5y , & rtP .
ck45m3cw5y , & rtPrevZCX . ck45m3cw5y ) ; rtB . nqjkjjasbi = ( rtB .
l0mam0lcui . j1vu52vt5l || rtB . ck45m3cw5y . mjqylog0rb ) ; rtB . ji3cpiksc2
= ( rtB . pwhtqllfp2 && rtB . nqjkjjasbi ) ; rtB . dg5qm5ofac = ( rtB .
em1c3syfzm || rtB . ji3cpiksc2 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB
. dtjhqkfsis = rtDW . cgxfb5ilba ; } rtB . azpjpom2k5 = ! rtB . bsh0vnffms ;
rtB . mtiovunzo4 = ( rtB . dtjhqkfsis && rtB . azpjpom2k5 ) ; rtB .
gy0l4qfs3l = rtB . mtiovunzo4 ; rtB . jq5nesvhnt = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . nowzjtzlbh , rtB . gy0l4qfs3l , rtB . jq5nesvhnt ,
rtP . T2_delay_hendselpqy , & rtB . o3bwas2b4l , & rtDW . o3bwas2b4l , & rtP
. o3bwas2b4l , & rtPrevZCX . o3bwas2b4l ) ; bhc3qgnuic ( rtS , rtB .
nomfep0mpn , rtB . gy0l4qfs3l , rtB . jq5nesvhnt , rtP . T2_delay_hendselpqy
, & rtB . h330fuij3a , & rtDW . h330fuij3a , & rtP . h330fuij3a , & rtPrevZCX
. h330fuij3a ) ; rtB . ds04toaenl = ( rtB . o3bwas2b4l . j1vu52vt5l || rtB .
h330fuij3a . mjqylog0rb ) ; rtB . kyd34o4af2 = ! rtB . ds04toaenl ; rtB .
mj31fir04g = ( rtB . dg5qm5ofac && rtB . kyd34o4af2 ) ; rtB . hbvdyagwzu = (
rtB . p5aimos5mc && rtB . mj31fir04g ) ; rtB . id2tnv4tw0 = ( rtB .
hbvdyagwzu || ( rtP . Constant2_Value_pi1g1x2eqs != 0.0 ) ) ; rtB .
bfbejl1ukh = rtB . id2tnv4tw0 ; rtB . hcga43dcui = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . obkia2x0bm , rtB . bfbejl1ukh , rtB . hcga43dcui ,
rtP . T6_delay_bb2jdc2qe5 , & rtB . ifezniamwz , & rtDW . ifezniamwz , & rtP
. ifezniamwz , & rtPrevZCX . ifezniamwz ) ; bhc3qgnuic ( rtS , rtB .
iso3rom1ur , rtB . bfbejl1ukh , rtB . hcga43dcui , rtP . T6_delay_bb2jdc2qe5
, & rtB . dsdu0sw4bh , & rtDW . dsdu0sw4bh , & rtP . dsdu0sw4bh , & rtPrevZCX
. dsdu0sw4bh ) ; rtB . kikp5dna5x = rtB . hah1gympfy ; rtB . jmjhc0023o =
ssGetT ( rtS ) ; on20slldft ( rtS , rtB . murhwaidrn , rtB . kikp5dna5x , rtB
. jmjhc0023o , rtP . T7_delay_nnl2lj4rtu , & rtB . cssuh33ty1 , & rtDW .
cssuh33ty1 , & rtP . cssuh33ty1 , & rtPrevZCX . cssuh33ty1 ) ; bhc3qgnuic (
rtS , rtB . ggvkk2zlq3 , rtB . kikp5dna5x , rtB . jmjhc0023o , rtP .
T7_delay_nnl2lj4rtu , & rtB . nmmyyafynb , & rtDW . nmmyyafynb , & rtP .
nmmyyafynb , & rtPrevZCX . nmmyyafynb ) ; rtB . iuoowzvmji = rtB . id2tnv4tw0
; rtB . afjjxz5iij = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . carutfzjrm ,
rtB . iuoowzvmji , rtB . afjjxz5iij , rtP . T3_delay_phxsiacjlh , & rtB .
pflqpv2odc , & rtDW . pflqpv2odc , & rtP . pflqpv2odc , & rtPrevZCX .
pflqpv2odc ) ; bhc3qgnuic ( rtS , rtB . bo50yrwyfc , rtB . iuoowzvmji , rtB .
afjjxz5iij , rtP . T3_delay_phxsiacjlh , & rtB . c5rp3exjwz , & rtDW .
c5rp3exjwz , & rtP . c5rp3exjwz , & rtPrevZCX . c5rp3exjwz ) ; if ( rtB .
axakxrl3jd ) { rtB . m22violp5t = rtB . id2tnv4tw0 ; } else { rtB .
pght2iwjm3 = ( rtB . cssuh33ty1 . j1vu52vt5l || rtB . nmmyyafynb . mjqylog0rb
) ; if ( rtB . pght2iwjm3 ) { rtB . gtmyoyxqbb = ( rtB . ifezniamwz .
j1vu52vt5l || rtB . dsdu0sw4bh . mjqylog0rb ) ; rtB . et4dcu1tj2 = rtB .
gtmyoyxqbb ; } else { rtB . jupcdyb20o = ( rtB . pflqpv2odc . j1vu52vt5l ||
rtB . c5rp3exjwz . mjqylog0rb ) ; rtB . et4dcu1tj2 = rtB . jupcdyb20o ; } rtB
. m22violp5t = rtB . et4dcu1tj2 ; } rtB . b04myoxm0j = ( rtB . m22violp5t &&
rtB . bsh0vnffms ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . ppsqqbs5mr
= rtDW . jyeacneoju ; rtB . dy3u3o0hwr = ! rtB . ppsqqbs5mr ; rtB .
ibkzubzovg = rtDW . i5d0s3lepy ; rtB . pi5t5j4vym = ! rtB . ibkzubzovg ; }
rtB . fdj3z1cec0 = ( rtB . b04myoxm0j && rtB . dy3u3o0hwr ) ; rtB .
io0lpwkorw = ssGetT ( rtS ) ; rtB . n5nukddte5 = rtB . b04myoxm0j ; rtB .
hql3wzyeac = rtB . n5nukddte5 ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
h54xaz0ws3 = rtDW . pw1qqgmx0o ; } itcd43cpff ( rtS , rtB . isx0fgzuta [ 0 ]
, rtB . hql3wzyeac , rtB . h54xaz0ws3 , & rtB . pqmqn4c0xq , & rtDW .
pqmqn4c0xq ) ; cgvj5xjnjk ( rtS , rtB . isx0fgzuta [ 1 ] , rtB . hql3wzyeac ,
rtB . h54xaz0ws3 , & rtB . bckzp5laog , & rtDW . bckzp5laog ) ; rtB .
otnq03lyc2 = ( rtB . pqmqn4c0xq . guqrn12esp || rtB . bckzp5laog . l5pxivnpxv
) ; rtB . dfwgaaxhbk = ( rtB . pi5t5j4vym && rtB . otnq03lyc2 ) ; rtB .
kfb3x1i11z = rtB . dfwgaaxhbk ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
e4yxofa1he = rtDW . ly0wgshoe2 ; } if ( rtB . kfb3x1i11z ) { rtB . f05npjqjmg
= rtB . io0lpwkorw ; } else { rtB . f05npjqjmg = rtB . e4yxofa1he ; } maxV =
rtP . u_PulseDuration_m42giojde1 - 2.2204460492503131E-16 ; rtB . oekt4naune
= rtB . f05npjqjmg + maxV ; rtB . me3w3v2md1 = ( rtB . oekt4naune > rtB .
io0lpwkorw ) ; rtB . bkmtw4gidw = ( rtB . fdj3z1cec0 || rtB . me3w3v2md1 ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . ahg1fup2oj = rtDW . osx1vowkj4 ;
rtB . ak0qjrimkr = ! rtB . ahg1fup2oj ; } rtB . ddm5fcmwtf = ( rtB .
bkmtw4gidw && rtB . ak0qjrimkr ) ; rtB . djrb4durhc = ssGetT ( rtS ) ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { fx2esogudm ( rtS , rtB . otnq03lyc2 , rtB .
djrb4durhc , & rtB . lsja5jhqu0 , & rtDW . lsja5jhqu0 , & rtPrevZCX .
lsja5jhqu0 ) ; rtB . pmim2e0s13 = rtB . lsja5jhqu0 . jhmp0c0tsm + rtP .
Constant_Value_pu2zgtgo5z ; rtB . mkzsfeaq5b = rtDW . l2fddlwysw ; } rtB .
oh5rbdhju0 = ( rtB . pmim2e0s13 > rtB . djrb4durhc ) ; rtB . op3i1voznh =
false ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . hyc4xdeany = rtDW .
d3dlbfiuuv ; rtB . lr4zey0hnb = ! rtB . hyc4xdeany ; } if ( rtP .
Constant4_Value_ibs5yvxxug > rtP . u_Threshold_cpkc2fz2oe ) { rtB .
bm4olqhm33 = ( rtB . op3i1voznh && rtB . lr4zey0hnb ) ; rtB . ovhnrnk4sg =
rtB . bm4olqhm33 ; } else { rtB . ovhnrnk4sg = rtB . op3i1voznh ; } rtB .
k2a2xkzaju = ( rtP . Constant3_Value_m2zq2bgyk2 != 0.0 ) ; rtB . ef2lnkuz3w =
ssGetT ( rtS ) ; on20slldft ( rtS , rtB . pewkf3ytio , rtB . k2a2xkzaju , rtB
. ef2lnkuz3w , rtP . T1_delay_kziwzu42nd , & rtB . cqgqlcfbrn , & rtDW .
cqgqlcfbrn , & rtP . cqgqlcfbrn , & rtPrevZCX . cqgqlcfbrn ) ; bhc3qgnuic (
rtS , rtB . gdiqx3tr4c , rtB . k2a2xkzaju , rtB . ef2lnkuz3w , rtP .
T1_delay_kziwzu42nd , & rtB . pshtvwhcvn , & rtDW . pshtvwhcvn , & rtP .
pshtvwhcvn , & rtPrevZCX . pshtvwhcvn ) ; rtB . nfrm3kmxn5 = ( rtB .
cqgqlcfbrn . j1vu52vt5l || rtB . pshtvwhcvn . mjqylog0rb ) ; rtB . mbwcgm1hlk
= ( rtB . ovhnrnk4sg && rtB . nfrm3kmxn5 ) ; rtB . frbgpj2520 = ( rtB .
mkzsfeaq5b || rtB . mbwcgm1hlk ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB
. pbkerb5xcw = rtDW . odjstwuwx3 ; } rtB . o3jldsxxpd = ! rtB . op3i1voznh ;
rtB . aji3w0nxhj = ( rtB . pbkerb5xcw && rtB . o3jldsxxpd ) ; rtB .
bynczlirpq = rtB . aji3w0nxhj ; rtB . jw5vtirlj5 = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . bd32r0nedu , rtB . bynczlirpq , rtB . jw5vtirlj5 ,
rtP . T2_delay_bhkjejhmq3 , & rtB . cwhxhlnmc3 , & rtDW . cwhxhlnmc3 , & rtP
. cwhxhlnmc3 , & rtPrevZCX . cwhxhlnmc3 ) ; bhc3qgnuic ( rtS , rtB .
pvyvaukrtz , rtB . bynczlirpq , rtB . jw5vtirlj5 , rtP . T2_delay_bhkjejhmq3
, & rtB . pxbtpesdlg , & rtDW . pxbtpesdlg , & rtP . pxbtpesdlg , & rtPrevZCX
. pxbtpesdlg ) ; rtB . bxqn432u14 = ( rtB . cwhxhlnmc3 . j1vu52vt5l || rtB .
pxbtpesdlg . mjqylog0rb ) ; rtB . hxrrshxftt = ! rtB . bxqn432u14 ; rtB .
kcfeapnbix = ( rtB . frbgpj2520 && rtB . hxrrshxftt ) ; rtB . lsilifo4w0 = (
rtB . ayhrq3rnrz && rtB . kcfeapnbix ) ; rtB . bchmq1xa31 = ( rtB .
lsilifo4w0 || ( rtP . Constant2_Value_pi1g1x2eqs != 0.0 ) ) ; rtB .
efzpiqt1un = rtB . bchmq1xa31 ; rtB . btkrilrx24 = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . joiglbbniw , rtB . efzpiqt1un , rtB . btkrilrx24 ,
rtP . T6_delay_goxkt5kpjq , & rtB . j1qputxmt3 , & rtDW . j1qputxmt3 , & rtP
. j1qputxmt3 , & rtPrevZCX . j1qputxmt3 ) ; bhc3qgnuic ( rtS , rtB .
blm3mjgqvq , rtB . efzpiqt1un , rtB . btkrilrx24 , rtP . T6_delay_goxkt5kpjq
, & rtB . h4jqcza024 , & rtDW . h4jqcza024 , & rtP . h4jqcza024 , & rtPrevZCX
. h4jqcza024 ) ; rtB . exho31oqry = rtB . gnmfhowrgd ; rtB . kjaf2chqq3 =
ssGetT ( rtS ) ; on20slldft ( rtS , rtB . fmujxxi2he , rtB . exho31oqry , rtB
. kjaf2chqq3 , rtP . T7_delay_cqscehdygy , & rtB . h510twish3 , & rtDW .
h510twish3 , & rtP . h510twish3 , & rtPrevZCX . h510twish3 ) ; bhc3qgnuic (
rtS , rtB . pmonq2okjg , rtB . exho31oqry , rtB . kjaf2chqq3 , rtP .
T7_delay_cqscehdygy , & rtB . b1csqxlk14 , & rtDW . b1csqxlk14 , & rtP .
b1csqxlk14 , & rtPrevZCX . b1csqxlk14 ) ; rtB . paflhlyasn = rtB . bchmq1xa31
; rtB . l455jhctcs = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . are5psgu4r ,
rtB . paflhlyasn , rtB . l455jhctcs , rtP . T3_delay_knwy5do2sp , & rtB .
cdulylvwwz , & rtDW . cdulylvwwz , & rtP . cdulylvwwz , & rtPrevZCX .
cdulylvwwz ) ; bhc3qgnuic ( rtS , rtB . ab04pgnims , rtB . paflhlyasn , rtB .
l455jhctcs , rtP . T3_delay_knwy5do2sp , & rtB . o2355kd0aj , & rtDW .
o2355kd0aj , & rtP . o2355kd0aj , & rtPrevZCX . o2355kd0aj ) ; if ( rtB .
pyma0sbd20 ) { rtB . czrj35jywj = rtB . bchmq1xa31 ; } else { rtB .
m2h1kw51af = ( rtB . h510twish3 . j1vu52vt5l || rtB . b1csqxlk14 . mjqylog0rb
) ; if ( rtB . m2h1kw51af ) { rtB . fzbemfu2s2 = ( rtB . j1qputxmt3 .
j1vu52vt5l || rtB . h4jqcza024 . mjqylog0rb ) ; rtB . oltfx4ifeo = rtB .
fzbemfu2s2 ; } else { rtB . g4fj1flkrc = ( rtB . cdulylvwwz . j1vu52vt5l ||
rtB . o2355kd0aj . mjqylog0rb ) ; rtB . oltfx4ifeo = rtB . g4fj1flkrc ; } rtB
. czrj35jywj = rtB . oltfx4ifeo ; } rtB . forzhlyp0b = ( rtB . czrj35jywj &&
rtB . op3i1voznh ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . mvlsb5wp23
= rtDW . lmsqqojpvs ; rtB . f5tjvp1fbu = ! rtB . mvlsb5wp23 ; rtB .
bjvr3d1jul = rtDW . kpsmnp3ug0 ; rtB . cgtbjaigqi = ! rtB . bjvr3d1jul ; }
rtB . olj0wpgasg = ( rtB . forzhlyp0b && rtB . f5tjvp1fbu ) ; rtB .
mfjmjb0gb0 = ssGetT ( rtS ) ; rtB . gb0k0mzupu = rtB . forzhlyp0b ; rtB .
ditynecwsc = rtB . gb0k0mzupu ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
e4m5zvxjvc = rtDW . ovtmlyjrxp ; } itcd43cpff ( rtS , rtB . khbmveuyjg [ 0 ]
, rtB . ditynecwsc , rtB . e4m5zvxjvc , & rtB . jfvcadr0il , & rtDW .
jfvcadr0il ) ; cgvj5xjnjk ( rtS , rtB . khbmveuyjg [ 1 ] , rtB . ditynecwsc ,
rtB . e4m5zvxjvc , & rtB . pxgzwnihte , & rtDW . pxgzwnihte ) ; rtB .
po0yyyyulu = ( rtB . jfvcadr0il . guqrn12esp || rtB . pxgzwnihte . l5pxivnpxv
) ; rtB . fdett0vckw = ( rtB . cgtbjaigqi && rtB . po0yyyyulu ) ; rtB .
e1o1243wgy = rtB . fdett0vckw ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
kyi3les3pk = rtDW . fbjudlgv4x ; } if ( rtB . e1o1243wgy ) { rtB . pg4ng5z5mp
= rtB . mfjmjb0gb0 ; } else { rtB . pg4ng5z5mp = rtB . kyi3les3pk ; } maxV =
rtP . u_PulseDuration_okwkmbo3k5 - 2.2204460492503131E-16 ; rtB . elpckzyqin
= rtB . pg4ng5z5mp + maxV ; rtB . kqutakybog = ( rtB . elpckzyqin > rtB .
mfjmjb0gb0 ) ; rtB . g1e2vuafcc = ( rtB . olj0wpgasg || rtB . kqutakybog ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . fnyqip1mam = rtDW . gmqawd5msq ;
rtB . p5owqwz13z = ! rtB . fnyqip1mam ; } rtB . aj2ajyhenz = ( rtB .
g1e2vuafcc && rtB . p5owqwz13z ) ; rtB . osh0z4suhl = ssGetT ( rtS ) ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { fx2esogudm ( rtS , rtB . po0yyyyulu , rtB .
osh0z4suhl , & rtB . nceplkru4a , & rtDW . nceplkru4a , & rtPrevZCX .
nceplkru4a ) ; rtB . eyguhdwa2v = rtB . nceplkru4a . jhmp0c0tsm + rtP .
Constant_Value_gfqprsag2h ; rtB . olspkzbkdz = rtDW . kegqko1pyk ; rtB .
blreo2w4nl = ! rtB . olspkzbkdz ; } rtB . fmsazo2uz1 = ( rtB . eyguhdwa2v >
rtB . osh0z4suhl ) ; rtB . nxzfsikmod = ( rtB . bsh0vnffms && rtB .
blreo2w4nl ) ; rtB . czk1muaphx = ( rtB . nxzfsikmod || rtB . aj2ajyhenz ||
rtB . ddm5fcmwtf ) ; rtB . jmvly2itoj = rtB . p4ckg1zyh0 * rtP .
u_Value_gqdhm5pmxp ; rtB . pxowu5p011 = rtB . iczkkrqhjn * rtP .
u_Value_hadlzqtpwh ; rtB . hho2wcqcwr = rtB . jmvly2itoj + rtB . pxowu5p011 ;
rtB . e3mjw3co0u = rtB . fv03qo0isx * rtP . u_Value_gqdhm5pmxp ; rtB .
h24gaz0dxd = rtB . nvegjh33zg * rtP . u_Value_i1olrqvpfb ; rtB . hdjryqmqel =
rtB . e3mjw3co0u + rtB . h24gaz0dxd ; rtB . hdfc1p0uuz = rtB . hho2wcqcwr +
rtB . hdjryqmqel ; rtB . ky2vfqhrcd = rtB . mlosq5bue4 + rtB . hdfc1p0uuz ;
rtB . juvg01s3dm = rtB . p4ckg1zyh0 + rtB . fv03qo0isx ; rtB . mipsuvpo3d =
rtB . juvg01s3dm + rtB . mlosq5bue4 ; rtB . cs0tyalbro = rtB . mipsuvpo3d *
rtP . u7_Value_fpkrrznsoo ; rtB . jiyamrxicq = rtB . iczkkrqhjn * rtP .
u_Value_gqdhm5pmxp ; rtB . perocakkdy = rtB . p4ckg1zyh0 * rtP .
u_Value_hadlzqtpwh ; rtB . pxfvpcemwn = rtB . jiyamrxicq - rtB . perocakkdy ;
rtB . dl11la2n1u = rtB . nvegjh33zg * rtP . u_Value_gqdhm5pmxp ; rtB .
mhaho2i42d = rtB . fv03qo0isx * rtP . u_Value_i1olrqvpfb ; rtB . bflzbkglli =
rtB . dl11la2n1u - rtB . mhaho2i42d ; rtB . jmbgsh5v4s = rtB . pxfvpcemwn +
rtB . bflzbkglli ; rtB . f1edp2xwyj = rtB . a5uoyeyotf + rtB . jmbgsh5v4s ;
rtB . ox5i4bzftl = rtB . f1edp2xwyj * rtP . u7_Value_fpkrrznsoo ; rtB .
beshsqn32i = rtB . ky2vfqhrcd * rtP . u7_Value_fpkrrznsoo ; rtB . ak3jdkwcyx
= rtB . iczkkrqhjn + rtB . nvegjh33zg ; rtB . cmo2qxvztb = rtB . ak3jdkwcyx +
rtB . a5uoyeyotf ; rtB . gl0gsszs4c = rtB . cmo2qxvztb * rtP .
u7_Value_fpkrrznsoo ; rtB . ahtzfl3gmj = rtB . mfs1yb5ala * rtP .
u_Value_op25zgfuj4 ; rtB . kfgzi5vgkr = rtB . h2waz5uker * rtP .
u_Value_kucc5zdzan ; rtB . fwqseyio3e = rtB . ahtzfl3gmj - rtB . kfgzi5vgkr ;
rtB . gble34ncpv = rtB . ndskvs431z * rtP . u_Value_op25zgfuj4 ; rtB .
accp4pyj41 = rtB . jslzn4rlal * rtP . u_Value_gx4m0xgf53 ; rtB . obsz5abnkn =
rtB . gble34ncpv - rtB . accp4pyj41 ; rtB . ifysbzbsxn = rtB . fwqseyio3e +
rtB . obsz5abnkn ; rtB . hzudiwbho4 = rtB . gwkrvtlqcl + rtB . ifysbzbsxn ;
rtB . ne5ysl4jmb = rtB . hzudiwbho4 * rtP . u7_Value_brorv5ga4i ; rtB .
gibvo2dva2 = rtB . ne5ysl4jmb * rtB . ne5ysl4jmb ; rtB . h5wzd5vjlo = rtB .
h2waz5uker * rtP . u_Value_op25zgfuj4 ; rtB . lq2jk0rbxw = rtB . mfs1yb5ala *
rtP . u_Value_kucc5zdzan ; rtB . l5oveg3hzr = rtB . h5wzd5vjlo + rtB .
lq2jk0rbxw ; rtB . l5xaltthpn = rtB . jslzn4rlal * rtP . u_Value_op25zgfuj4 ;
rtB . cs5hphxcy0 = rtB . ndskvs431z * rtP . u_Value_gx4m0xgf53 ; rtB .
ptxpzw1cez = rtB . l5xaltthpn + rtB . cs5hphxcy0 ; rtB . olf4auwldq = rtB .
l5oveg3hzr + rtB . ptxpzw1cez ; rtB . ikp21o4erw = rtB . e15bejz3up + rtB .
olf4auwldq ; rtB . dhs3h4uvxj = rtB . ikp21o4erw * rtP . u7_Value_brorv5ga4i
; rtB . f42c3zeful = rtB . dhs3h4uvxj * rtB . dhs3h4uvxj ; rtB . dlv23yz00f =
rtB . gibvo2dva2 + rtB . f42c3zeful ; rtB . moa3ytyshy = muDoubleScalarSqrt (
rtB . dlv23yz00f ) ; rtB . jelhz1oeik = rtB . ne5ysl4jmb / rtB . moa3ytyshy ;
rtB . kbuozdvv30 = rtP . u_Gain_pt000lkrel * rtB . dxa1300d4c ; rtB .
k53fbzxjm3 = rtP . u_Gain_lizals2j40 * rtB . l2zahd4oox ; rtB . ivlpes343m =
rtB . k53fbzxjm3 * rtP . u_Value_mzgfto51lj ; rtB . jdqkwaavvl = rtP .
u_Gain_ecayywe5s0 * rtB . g340u5g2f2 ; rtB . jazyhecakg = rtB . jdqkwaavvl *
rtP . u_Value_fvhnioxeng ; rtB . pnfygjffoy = rtB . ivlpes343m - rtB .
jazyhecakg ; rtB . k24ayfck3b = rtP . u_Gain_namoew0qri * rtB . emlwggxmjw ;
rtB . kqndkqkoqz = rtB . k24ayfck3b * rtP . u_Value_mzgfto51lj ; rtB .
ii0s4piaiy = rtP . u_Gain_bz3e0bohyu * rtB . nubgwxsn52 ; rtB . k1xunijl5v =
rtB . ii0s4piaiy * rtP . u_Value_ammu2xiiwp ; rtB . lf5ivkgpwc = rtB .
kqndkqkoqz - rtB . k1xunijl5v ; rtB . nyswckd2u0 = rtB . pnfygjffoy + rtB .
lf5ivkgpwc ; rtB . glywm2ls0r = rtB . kbuozdvv30 + rtB . nyswckd2u0 ; rtB .
muah0pylug = rtB . glywm2ls0r * rtP . u7_Value_i1vsphebsk ; rtB . dyoyqorlid
= rtB . muah0pylug * rtB . muah0pylug ; rtB . mhlw51h5ew = rtP .
u_Gain_iroygfyp3a * rtB . dgvytedolp ; rtB . csnamnjby2 = rtB . jdqkwaavvl *
rtP . u_Value_mzgfto51lj ; rtB . byu4fi4jcj = rtB . k53fbzxjm3 * rtP .
u_Value_fvhnioxeng ; rtB . hiiramkrfh = rtB . csnamnjby2 + rtB . byu4fi4jcj ;
rtB . aibrl4fhor = rtB . ii0s4piaiy * rtP . u_Value_mzgfto51lj ; rtB .
frz2ofak3o = rtB . k24ayfck3b * rtP . u_Value_ammu2xiiwp ; rtB . hnj5hq2id2 =
rtB . aibrl4fhor + rtB . frz2ofak3o ; rtB . izni0bknfw = rtB . hiiramkrfh +
rtB . hnj5hq2id2 ; rtB . ddveprjmuk = rtB . mhlw51h5ew + rtB . izni0bknfw ;
rtB . epcge21xxs = rtB . ddveprjmuk * rtP . u7_Value_i1vsphebsk ; rtB .
ibj5134jfu = rtB . epcge21xxs * rtB . epcge21xxs ; rtB . pstaecohpk = rtB .
dyoyqorlid + rtB . ibj5134jfu ; rtB . g15vxlpw3d = muDoubleScalarSqrt ( rtB .
pstaecohpk ) ; rtB . hodb4nxe45 = rtB . muah0pylug / rtB . g15vxlpw3d ; rtB .
falrx5k3kc = rtB . jelhz1oeik - rtB . hodb4nxe45 ; rtB . lycrhqpsvm = rtB .
falrx5k3kc * rtB . falrx5k3kc ; rtB . iqwpn0lpdc = rtB . dhs3h4uvxj / rtB .
moa3ytyshy ; rtB . g1h4wl0kk3 = rtB . epcge21xxs / rtB . g15vxlpw3d ; rtB .
licha52r13 = rtB . iqwpn0lpdc - rtB . g1h4wl0kk3 ; rtB . eairb43hp3 = rtB .
licha52r13 * rtB . licha52r13 ; rtB . ocylviibpl = rtB . lycrhqpsvm + rtB .
eairb43hp3 ; rtB . g4rzaenfyb = rtB . ocylviibpl * rtP .
Constant3_Value_gmov0jdjdg ; rtB . pzw3gprjbf = rtP .
Constant2_Value_bumrwxmros - rtB . g4rzaenfyb ; u0 = rtB . pzw3gprjbf ; if (
u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } rtB .
b54ewrewef = muDoubleScalarAcos ( u0 ) ; rtB . lgduiawafn = rtB . iqwpn0lpdc
- rtB . hodb4nxe45 ; rtB . j4hyie12di = rtB . lgduiawafn * rtB . lgduiawafn ;
rtB . dcbvtxr0fo = rtB . jelhz1oeik + rtB . g1h4wl0kk3 ; rtB . nlbpbgrgit =
rtB . dcbvtxr0fo * rtB . dcbvtxr0fo ; rtB . bhskbrbvzy = rtB . j4hyie12di +
rtB . nlbpbgrgit ; rtB . mk14uo4hjd = ( rtB . bhskbrbvzy > rtP .
Constant_Value_hfk43zrif4 ) ; if ( rtB . mk14uo4hjd ) { rtB . amtoyerqnp =
rtB . b54ewrewef * rtP . Constant1_Value_pihl1rqlvc ; rtB . j442htsk53 = rtB
. amtoyerqnp ; } else { rtB . j442htsk53 = rtB . b54ewrewef ; } rtB .
gkxgknx5iu = muDoubleScalarCos ( rtB . j442htsk53 ) ; rtB . nqebrrenfk = rtB
. ne5ysl4jmb * rtB . ne5ysl4jmb ; rtB . fifdraoiug = rtB . dhs3h4uvxj * rtB .
dhs3h4uvxj ; rtB . cfhq2wibof = rtB . nqebrrenfk + rtB . fifdraoiug ; rtB .
f5y4wda4mn = muDoubleScalarSqrt ( rtB . cfhq2wibof ) ; rtB . grauavebex = rtB
. muah0pylug * rtB . muah0pylug ; rtB . at4nolk10v = rtB . epcge21xxs * rtB .
epcge21xxs ; rtB . hcyebk10zy = rtB . grauavebex + rtB . at4nolk10v ; rtB .
eudj0lsmnn = muDoubleScalarSqrt ( rtB . hcyebk10zy ) ; rtB . lb3mlbr52i = rtB
. gkxgknx5iu * rtB . f5y4wda4mn * rtB . eudj0lsmnn * rtP .
Constant2_Value_dnuhg4tew2 ; rtB . erhfq0v3b2 = ( rtB . lb3mlbr52i > rtP .
Constant_Value_agwv4jkyzv ) ; rtB . hb1vxr1g0q = ( ( rtB . k1gomebxbf != 0.0
) && ( rtB . nmuuyrrny0 != 0.0 ) && ( rtB . ox4taqtsfn != 0.0 ) ) ; rtB .
o1szy3x53x = ( rtB . bnp3fcdgvq && rtB . erhfq0v3b2 && rtB . hb1vxr1g0q ) ;
rtB . p00rg2fdjj = ( rtB . lb3mlbr52i > rtP . Constant1_Value_doeke0bbcg ) ;
rtB . oqapkw1k24 = false ; rtB . bearvd1ta3 = ( rtB . m53ngpfbkk && rtB .
erhfq0v3b2 && rtB . hb1vxr1g0q ) ; rtB . azdwrmeomb = muDoubleScalarSin ( rtB
. j442htsk53 ) ; rtB . nt30zaer5a = ssGetT ( rtS ) ; rtB . oloumktz1o = rtB .
bearvd1ta3 ; on20slldft ( rtS , rtB . jtbh3i1tza , rtB . oloumktz1o , rtB .
nt30zaer5a , rtP . sign_st1_delay , & rtB . muhd0m2ek3 , & rtDW . muhd0m2ek3
, & rtP . muhd0m2ek3 , & rtPrevZCX . muhd0m2ek3 ) ; bhc3qgnuic ( rtS , rtB .
cbsejwmzvq , rtB . oloumktz1o , rtB . nt30zaer5a , rtP . sign_st1_delay , &
rtB . g2vaojwvzg , & rtDW . g2vaojwvzg , & rtP . g2vaojwvzg , & rtPrevZCX .
g2vaojwvzg ) ; rtB . iniraune2g = ( rtB . muhd0m2ek3 . j1vu52vt5l || rtB .
g2vaojwvzg . mjqylog0rb ) ; rtB . i54xg0sbrw = rtB . jdqkwaavvl * rtP .
u_Value_mzgfto51lj ; rtB . bhqwis21kc = rtB . k53fbzxjm3 * rtP .
u_Value_ammu2xiiwp ; rtB . nqtbrjxlmf = rtB . i54xg0sbrw + rtB . bhqwis21kc ;
rtB . c4oih0lhi2 = rtB . ii0s4piaiy * rtP . u_Value_mzgfto51lj ; rtB .
otho105fgd = rtB . k24ayfck3b * rtP . u_Value_fvhnioxeng ; rtB . bmkohb40m1 =
rtB . c4oih0lhi2 + rtB . otho105fgd ; rtB . i1eay5waaj = rtB . nqtbrjxlmf +
rtB . bmkohb40m1 ; rtB . bbppgitun1 = rtB . mhlw51h5ew + rtB . i1eay5waaj ;
rtB . mckzszz4iu = rtB . jdqkwaavvl + rtB . ii0s4piaiy ; rtB . nxv1mjacnw =
rtB . mckzszz4iu + rtB . mhlw51h5ew ; rtB . pbgthzdehn = rtB . nxv1mjacnw *
rtP . u7_Value_i1vsphebsk ; rtB . hphqooc5ht = rtB . k53fbzxjm3 * rtP .
u_Value_mzgfto51lj ; rtB . kxso01vrui = rtB . jdqkwaavvl * rtP .
u_Value_ammu2xiiwp ; rtB . dkxaic1mxs = rtB . hphqooc5ht - rtB . kxso01vrui ;
rtB . ojahphltro = rtB . k24ayfck3b * rtP . u_Value_mzgfto51lj ; rtB .
jxsoh0thtw = rtB . ii0s4piaiy * rtP . u_Value_fvhnioxeng ; rtB . cgqq05grkj =
rtB . ojahphltro - rtB . jxsoh0thtw ; rtB . kxgegl4f2h = rtB . dkxaic1mxs +
rtB . cgqq05grkj ; rtB . eifs1ibg4s = rtB . kbuozdvv30 + rtB . kxgegl4f2h ;
rtB . lqki5ygltr = rtB . eifs1ibg4s * rtP . u7_Value_i1vsphebsk ; rtB .
akf3fn4hau = rtB . bbppgitun1 * rtP . u7_Value_i1vsphebsk ; rtB . bsrx0b1svv
= rtB . k53fbzxjm3 + rtB . k24ayfck3b ; rtB . hjnr323p34 = rtB . bsrx0b1svv +
rtB . kbuozdvv30 ; rtB . mwnvc1yfow = rtB . hjnr323p34 * rtP .
u7_Value_i1vsphebsk ; rtB . lrycjfrg4a = rtB . h2waz5uker * rtP .
u_Value_op25zgfuj4 ; rtB . mt3nmyfcrx = rtB . mfs1yb5ala * rtP .
u_Value_gx4m0xgf53 ; rtB . bozg5g20a4 = rtB . lrycjfrg4a + rtB . mt3nmyfcrx ;
rtB . oqqp3ke1mk = rtB . jslzn4rlal * rtP . u_Value_op25zgfuj4 ; rtB .
kji50gagik = rtB . ndskvs431z * rtP . u_Value_kucc5zdzan ; rtB . ccnqomjgft =
rtB . oqqp3ke1mk + rtB . kji50gagik ; rtB . oqveluefj3 = rtB . bozg5g20a4 +
rtB . ccnqomjgft ; rtB . gvgp2buxrr = rtB . e15bejz3up + rtB . oqveluefj3 ;
rtB . lkrnz3fr3l = rtB . h2waz5uker + rtB . jslzn4rlal ; rtB . izewfic3nk =
rtB . lkrnz3fr3l + rtB . e15bejz3up ; rtB . drxpqdjkvg = rtB . izewfic3nk *
rtP . u7_Value_brorv5ga4i ; rtB . bsaijdkagi = rtB . mfs1yb5ala * rtP .
u_Value_op25zgfuj4 ; rtB . beqphjsv35 = rtB . h2waz5uker * rtP .
u_Value_gx4m0xgf53 ; rtB . gkgmtnb4ap = rtB . bsaijdkagi - rtB . beqphjsv35 ;
rtB . lzmgs4ql3d = rtB . ndskvs431z * rtP . u_Value_op25zgfuj4 ; rtB .
cvtejgf4l1 = rtB . jslzn4rlal * rtP . u_Value_kucc5zdzan ; rtB . nwkyp5k3gb =
rtB . lzmgs4ql3d - rtB . cvtejgf4l1 ; rtB . n25ohsq3jd = rtB . gkgmtnb4ap +
rtB . nwkyp5k3gb ; rtB . fdwy4hbufv = rtB . gwkrvtlqcl + rtB . n25ohsq3jd ;
rtB . ff0g2u1bcp = rtB . fdwy4hbufv * rtP . u7_Value_brorv5ga4i ; rtB .
mnen2l0gjd = rtB . gvgp2buxrr * rtP . u7_Value_brorv5ga4i ; rtB . blbw1epues
= rtB . mfs1yb5ala + rtB . ndskvs431z ; rtB . jjo11zaj13 = rtB . blbw1epues +
rtB . gwkrvtlqcl ; rtB . bzh3gk4pv4 = rtB . jjo11zaj13 * rtP .
u7_Value_brorv5ga4i ; rtB . lq0ahpiohv = ssGetT ( rtS ) ; rtB . cc4oei0ct0 =
rtB . o1szy3x53x ; on20slldft ( rtS , rtB . dtgyneq0ks , rtB . cc4oei0ct0 ,
rtB . lq0ahpiohv , rtP . st1_delay , & rtB . iosuhjjule , & rtDW . iosuhjjule
, & rtP . iosuhjjule , & rtPrevZCX . iosuhjjule ) ; bhc3qgnuic ( rtS , rtB .
eh43tfta42 , rtB . cc4oei0ct0 , rtB . lq0ahpiohv , rtP . st1_delay , & rtB .
fey4zxdyka , & rtDW . fey4zxdyka , & rtP . fey4zxdyka , & rtPrevZCX .
fey4zxdyka ) ; rtB . ck2bpvsjfu = ( rtB . iosuhjjule . j1vu52vt5l || rtB .
fey4zxdyka . mjqylog0rb ) ; rtB . p2y5jr3xtw = ssGetT ( rtS ) ; rtB .
c5jfsgehk2 = rtB . oqapkw1k24 ; on20slldft ( rtS , rtB . d5cwdi3ojp , rtB .
c5jfsgehk2 , rtB . p2y5jr3xtw , rtP . st2_delay , & rtB . j0x4gfzwjd , & rtDW
. j0x4gfzwjd , & rtP . j0x4gfzwjd , & rtPrevZCX . j0x4gfzwjd ) ; bhc3qgnuic (
rtS , rtB . oommzbtaql , rtB . c5jfsgehk2 , rtB . p2y5jr3xtw , rtP .
st2_delay , & rtB . anq402mwuh , & rtDW . anq402mwuh , & rtP . anq402mwuh , &
rtPrevZCX . anq402mwuh ) ; rtB . beq2fm3mrr = ( rtB . j0x4gfzwjd . j1vu52vt5l
|| rtB . anq402mwuh . mjqylog0rb ) ; rtB . o5xpw4kqql = rtB . bp3r01ovhm +
rtB . ljktjlr15i ; rtB . fuwfdqr1rp = rtB . o5xpw4kqql + rtB . ec3muigggy ;
rtB . izbkkdmjn4 = rtB . fuwfdqr1rp * rtP . u7_Value_jjrzoquulk ; rtB .
jrb55hm3st = rtB . izbkkdmjn4 + rtB . b2qz4pss45 ; rtB . ndgsbzdnms = rtB .
pdkc5n1zyw + rtB . j3k2v5sks2 ; rtB . iv4wdquiae = rtB . ndgsbzdnms + rtB .
cvkdusf3n5 ; rtB . hwjxh1e5xq = rtB . iv4wdquiae * rtP . u7_Value_jjrzoquulk
; rtB . matcph14k1 = rtB . hwjxh1e5xq + rtB . mkmmuynnwa ; rtB . kl3e2fab3w =
rtB . b2qz4pss45 * rtB . b2qz4pss45 ; rtB . hhyfndweao = rtB . mkmmuynnwa *
rtB . mkmmuynnwa ; rtB . f21vt0jomx = rtB . kl3e2fab3w + rtB . hhyfndweao ;
rtB . kgijplpzji = muDoubleScalarSqrt ( rtB . f21vt0jomx ) ; rtB . daqas1kg1f
= rtB . kgijplpzji * rtP . Constant2_Value_e3mm4s35xg ; rtB . n5wssxdpfk =
rtB . pz2k3jltfa * rtB . pz2k3jltfa ; rtB . mvhl5s2dhh = rtB . gowzdlfjhz *
rtB . gowzdlfjhz ; rtB . osatrajua2 = rtB . n5wssxdpfk + rtB . mvhl5s2dhh ;
rtB . eg3lu4cwkk = muDoubleScalarSqrt ( rtB . osatrajua2 ) ; if ( rtP .
Constant_Value_ldlnslfehs > rtP . u_Threshold_dbw5wtlcwo ) { rtB . fnolagh3kf
= ( rtB . eg3lu4cwkk < rtP . Constant3_Value_a0grfyn0oo ) ; rtB . elrgkyyghg
= rtB . fnolagh3kf ; } else { rtB . fur14oicok = ( rtB . eg3lu4cwkk > rtP .
Constant3_Value_a0grfyn0oo ) ; rtB . elrgkyyghg = rtB . fur14oicok ; } rtB .
gae0ymb0eg = ( rtB . elrgkyyghg && rtB . h4zldp0im1 ) ; if ( rtB . kgv5tiygu3
) { rtB . cutoxtvypc = rtB . gae0ymb0eg ; } else { rtB . nxaf4bzyks = ( rtB .
gae0ymb0eg && rtB . fo2kcki2n0 ) ; rtB . cutoxtvypc = rtB . nxaf4bzyks ; }
rtB . jbhplb242a = ( rtB . mmgwbebi3h && rtB . cutoxtvypc && rtB . m2mz0eqwqu
) ; rtB . myawpilwin = rtB . jrb55hm3st * rtB . jrb55hm3st ; rtB . fafov4mogk
= rtB . matcph14k1 * rtB . matcph14k1 ; rtB . pge20wfvvq = rtB . myawpilwin +
rtB . fafov4mogk ; rtB . bp3jtj3uma = muDoubleScalarSqrt ( rtB . pge20wfvvq )
; rtB . puigrufrlb = ( rtB . bp3jtj3uma >= rtB . daqas1kg1f ) ; rtB .
g2wtzc0x4j = ( rtB . bglcjboygs && rtB . puigrufrlb && rtB . cxzexegdct ) ;
rtB . f5ghgxmkal = rtB . jbhplb242a ; rtB . oygnhsbir0 = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . a1rirvbzfx , rtB . f5ghgxmkal , rtB . oygnhsbir0 ,
rtP . OffDelay_delay_mo2wdvejns , & rtB . kxnpw5jl23 , & rtDW . kxnpw5jl23 ,
& rtP . kxnpw5jl23 , & rtPrevZCX . kxnpw5jl23 ) ; bhc3qgnuic ( rtS , rtB .
khzgtvhuz5 , rtB . f5ghgxmkal , rtB . oygnhsbir0 , rtP .
OffDelay_delay_mo2wdvejns , & rtB . g2dmi2bhhz , & rtDW . g2dmi2bhhz , & rtP
. g2dmi2bhhz , & rtPrevZCX . g2dmi2bhhz ) ; rtB . h4lmxj4upb = ( rtB .
kxnpw5jl23 . j1vu52vt5l || rtB . g2dmi2bhhz . mjqylog0rb ) ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . dbyqgwzsqg = rtDW . crgr0yyjst ; rtB
. nojrwbtfvz = ! rtB . dbyqgwzsqg ; rtB . hopmworwn5 = rtDW . ex4vqumvao ;
rtB . bqs5iyaior = ! rtB . hopmworwn5 ; } rtB . hy2tufpld3 = ( rtB .
h4lmxj4upb && rtB . nojrwbtfvz ) ; rtB . iobthxmux3 = ssGetT ( rtS ) ; rtB .
c2ffneko0n = rtB . h4lmxj4upb ; rtB . hse2egktr5 = rtB . c2ffneko0n ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . nmfsykat4n = rtDW . nbtfsiobop ; }
itcd43cpff ( rtS , rtB . lqelpyok5n [ 0 ] , rtB . hse2egktr5 , rtB .
nmfsykat4n , & rtB . mzl2ofouzz , & rtDW . mzl2ofouzz ) ; cgvj5xjnjk ( rtS ,
rtB . lqelpyok5n [ 1 ] , rtB . hse2egktr5 , rtB . nmfsykat4n , & rtB .
er0egiqxa1 , & rtDW . er0egiqxa1 ) ; rtB . iokqyupkrn = ( rtB . mzl2ofouzz .
guqrn12esp || rtB . er0egiqxa1 . l5pxivnpxv ) ; rtB . pa35y4jtvj = ( rtB .
bqs5iyaior && rtB . iokqyupkrn ) ; rtB . na5r1ed4kp = rtB . pa35y4jtvj ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . fqwoegkb24 = rtDW . jl4tnonvue ; } if
( rtB . na5r1ed4kp ) { rtB . m4ygpwffk0 = rtB . iobthxmux3 ; } else { rtB .
m4ygpwffk0 = rtB . fqwoegkb24 ; } maxV = rtP . u_PulseDuration_ditiav1mcr -
2.2204460492503131E-16 ; rtB . clehm3b4km = rtB . m4ygpwffk0 + maxV ; rtB .
kbpdscow2k = ( rtB . clehm3b4km > rtB . iobthxmux3 ) ; rtB . e5qucy2gg0 = (
rtB . hy2tufpld3 || rtB . kbpdscow2k ) ; rtB . nqzon13svc = rtB . g2wtzc0x4j
; rtB . bo4r525xga = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . atq3dnr2zu ,
rtB . nqzon13svc , rtB . bo4r525xga , rtP . OffDelay1_delay , & rtB .
jzgpron3co , & rtDW . jzgpron3co , & rtP . jzgpron3co , & rtPrevZCX .
jzgpron3co ) ; bhc3qgnuic ( rtS , rtB . bkf2quznmi , rtB . nqzon13svc , rtB .
bo4r525xga , rtP . OffDelay1_delay , & rtB . dzkeyq1gmc , & rtDW . dzkeyq1gmc
, & rtP . dzkeyq1gmc , & rtPrevZCX . dzkeyq1gmc ) ; rtB . ieroamoqnq = ( rtB
. jzgpron3co . j1vu52vt5l || rtB . dzkeyq1gmc . mjqylog0rb ) ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . m45catjukt = rtDW . m3pubmd011 ; rtB
. nhhyqvxjdt = ! rtB . m45catjukt ; rtB . bsmzilfw0d = rtDW . fvjg0rzhqa ;
rtB . kku3hdvgci = ! rtB . bsmzilfw0d ; } rtB . a0t15ht1hn = ( rtB .
ieroamoqnq && rtB . nhhyqvxjdt ) ; rtB . eaimka5nm1 = ssGetT ( rtS ) ; rtB .
nq2fyrgpl4 = rtB . ieroamoqnq ; rtB . mcfvfus3t5 = rtB . nq2fyrgpl4 ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . iqdysshgr1 = rtDW . h0ue4eifzz ; }
itcd43cpff ( rtS , rtB . k45bdikb3y [ 0 ] , rtB . mcfvfus3t5 , rtB .
iqdysshgr1 , & rtB . b0usl1ftx3 , & rtDW . b0usl1ftx3 ) ; cgvj5xjnjk ( rtS ,
rtB . k45bdikb3y [ 1 ] , rtB . mcfvfus3t5 , rtB . iqdysshgr1 , & rtB .
ftiqhc2zan , & rtDW . ftiqhc2zan ) ; rtB . ofgx10ocun = ( rtB . b0usl1ftx3 .
guqrn12esp || rtB . ftiqhc2zan . l5pxivnpxv ) ; rtB . j21nbfj5wr = ( rtB .
kku3hdvgci && rtB . ofgx10ocun ) ; rtB . clxhsb4mus = rtB . j21nbfj5wr ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . lmfi13rbcz = rtDW . dxcijq0izx ; } if
( rtB . clxhsb4mus ) { rtB . pj4d0qpawy = rtB . eaimka5nm1 ; } else { rtB .
pj4d0qpawy = rtB . lmfi13rbcz ; } maxV = rtP . u_PulseDuration_lr3lowrybm -
2.2204460492503131E-16 ; rtB . hqje2di4p1 = rtB . pj4d0qpawy + maxV ; rtB .
insjcpo2lh = ( rtB . hqje2di4p1 > rtB . eaimka5nm1 ) ; rtB . lzqw1lezio = (
rtB . a0t15ht1hn || rtB . insjcpo2lh ) ; rtB . p4h1p4fk0o = ( rtB .
e5qucy2gg0 || rtB . lzqw1lezio ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB
. nv5hyy1qir = rtDW . elckdgh4b5 ; rtB . eqwxfskabn = ! rtB . nv5hyy1qir ;
rtB . ov4g4hrcc1 = rtDW . e4su0h1vb4 ; rtB . hvpfi4i4wa = ! rtB . ov4g4hrcc1
; } rtB . k3erjvsn34 = ( rtB . cutoxtvypc && rtB . eqwxfskabn ) ; rtB .
hjwwmvwsku = ( rtB . puigrufrlb && rtB . hvpfi4i4wa ) ; rtB . l2ahjc3tkc = (
rtB . k3erjvsn34 || rtB . hjwwmvwsku ) ; rtB . afynrhehos = ssGetT ( rtS ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { fx2esogudm ( rtS , rtB . ofgx10ocun ,
rtB . afynrhehos , & rtB . a503xmz3wd , & rtDW . a503xmz3wd , & rtPrevZCX .
a503xmz3wd ) ; rtB . hmd5co2qt0 = rtB . a503xmz3wd . jhmp0c0tsm + rtP .
Constant_Value_azlgusb0fq ; } rtB . pcdl5twd5m = ( rtB . hmd5co2qt0 > rtB .
afynrhehos ) ; rtB . cqsjzjzwof = ssGetT ( rtS ) ; if ( ssIsSampleHit ( rtS ,
1 , 0 ) ) { fx2esogudm ( rtS , rtB . iokqyupkrn , rtB . cqsjzjzwof , & rtB .
ktoyym5lv5 , & rtDW . ktoyym5lv5 , & rtPrevZCX . ktoyym5lv5 ) ; rtB .
azh4ejrdkc = rtB . ktoyym5lv5 . jhmp0c0tsm + rtP . Constant_Value_eq5gbe5b4b
; } rtB . mziw3fqyh2 = ( rtB . azh4ejrdkc > rtB . cqsjzjzwof ) ; rtB .
aatmor3e5t = rtB . pdkc5n1zyw * rtP . u_Value_ns1c10qxcp ; rtB . get20s0cta =
rtB . bp3r01ovhm * rtP . u_Value_cxqlmyfnq0 ; rtB . agvl3p0khp = rtB .
aatmor3e5t + rtB . get20s0cta ; rtB . cgs0aequzd = rtB . j3k2v5sks2 * rtP .
u_Value_ns1c10qxcp ; rtB . br3bwoelbp = rtB . ljktjlr15i * rtP .
u_Value_f22l3naijn ; rtB . fptt4lp5lo = rtB . cgs0aequzd + rtB . br3bwoelbp ;
rtB . gn40tqnw3y = rtB . agvl3p0khp + rtB . fptt4lp5lo ; rtB . p34iijnojp =
rtB . cvkdusf3n5 + rtB . gn40tqnw3y ; rtB . mymw2ldmms = rtB . pdkc5n1zyw *
rtP . u_Value_ns1c10qxcp ; rtB . hvnvwhvimn = rtB . bp3r01ovhm * rtP .
u_Value_f22l3naijn ; rtB . ob0mw0cicx = rtB . mymw2ldmms + rtB . hvnvwhvimn ;
rtB . a0gbpvbhl5 = rtB . j3k2v5sks2 * rtP . u_Value_ns1c10qxcp ; rtB .
pyox2nk51u = rtB . ljktjlr15i * rtP . u_Value_cxqlmyfnq0 ; rtB . ol24dusf3f =
rtB . a0gbpvbhl5 + rtB . pyox2nk51u ; rtB . kxfvg5qhh0 = rtB . ob0mw0cicx +
rtB . ol24dusf3f ; rtB . dkww3krlv4 = rtB . cvkdusf3n5 + rtB . kxfvg5qhh0 ;
rtB . fccoehq1k2 = rtB . bp3r01ovhm * rtP . u_Value_ns1c10qxcp ; rtB .
a3exmjqdge = rtB . pdkc5n1zyw * rtP . u_Value_f22l3naijn ; rtB . f3jt0us3bv =
rtB . fccoehq1k2 - rtB . a3exmjqdge ; rtB . ohy1ah3311 = rtB . ljktjlr15i *
rtP . u_Value_ns1c10qxcp ; rtB . g41kmeoj3m = rtB . j3k2v5sks2 * rtP .
u_Value_cxqlmyfnq0 ; rtB . fyafh2qjeg = rtB . ohy1ah3311 - rtB . g41kmeoj3m ;
rtB . ixisuvlzw3 = rtB . f3jt0us3bv + rtB . fyafh2qjeg ; rtB . lxrehiiutr =
rtB . ec3muigggy + rtB . ixisuvlzw3 ; rtB . gx3l23gg31 = rtB . lxrehiiutr *
rtP . u7_Value_jjrzoquulk ; rtB . lg5wmsjtde = rtB . bp3r01ovhm * rtP .
u_Value_ns1c10qxcp ; rtB . nsfckqmj4t = rtB . pdkc5n1zyw * rtP .
u_Value_cxqlmyfnq0 ; rtB . g0y2un5ugt = rtB . lg5wmsjtde - rtB . nsfckqmj4t ;
rtB . hcxd0dehj3 = rtB . ljktjlr15i * rtP . u_Value_ns1c10qxcp ; rtB .
gj4qgtkicq = rtB . j3k2v5sks2 * rtP . u_Value_f22l3naijn ; rtB . affzv3iqdr =
rtB . hcxd0dehj3 - rtB . gj4qgtkicq ; rtB . jctkcfswpa = rtB . g0y2un5ugt +
rtB . affzv3iqdr ; rtB . m2mkej4nuy = rtB . ec3muigggy + rtB . jctkcfswpa ;
rtB . ocrjhwocjt = rtB . m2mkej4nuy * rtP . u7_Value_jjrzoquulk ; rtB .
azpvmkawmc = rtB . p34iijnojp * rtP . u7_Value_jjrzoquulk ; rtB . go4i0qa22w
= rtB . dkww3krlv4 * rtP . u7_Value_jjrzoquulk ; rtB . dw4jpooagh = rtB .
dxa1300d4c * rtB . dxa1300d4c ; rtB . nhxowjb2ab = rtB . dgvytedolp * rtB .
dgvytedolp ; rtB . kikybhtrzx = rtB . dw4jpooagh + rtB . nhxowjb2ab ; rtB .
ny41qpggyq = muDoubleScalarSqrt ( rtB . kikybhtrzx ) ; rtB . htocyv5udv = rtB
. l2zahd4oox * rtB . l2zahd4oox ; rtB . euactdsjed = rtB . g340u5g2f2 * rtB .
g340u5g2f2 ; rtB . pktgo0u50h = rtB . htocyv5udv + rtB . euactdsjed ; rtB .
blwm0gqf1x = muDoubleScalarSqrt ( rtB . pktgo0u50h ) ; rtB . gqhnnbeej4 = (
rtB . ny41qpggyq >= rtB . blwm0gqf1x ) ; if ( rtB . gqhnnbeej4 ) { rtB .
dold2ba2xe = rtB . ny41qpggyq ; } else { rtB . dold2ba2xe = rtB . blwm0gqf1x
; } rtB . i5xrcpdbuc = rtB . emlwggxmjw * rtB . emlwggxmjw ; rtB . pimcwu5n5a
= rtB . nubgwxsn52 * rtB . nubgwxsn52 ; rtB . lt43515pgm = rtB . i5xrcpdbuc +
rtB . pimcwu5n5a ; rtB . ht31pc1po3 = muDoubleScalarSqrt ( rtB . lt43515pgm )
; rtB . otjwraamgd = ( rtB . dold2ba2xe >= rtB . ht31pc1po3 ) ; if ( rtB .
otjwraamgd ) { if ( rtB . gqhnnbeej4 ) { rtB . lcetalc5nb = rtB . dxa1300d4c
; } else { rtB . lcetalc5nb = rtB . l2zahd4oox ; } rtB . ff2obrxiaf = rtB .
lcetalc5nb ; if ( rtB . gqhnnbeej4 ) { rtB . omam4tkvio = rtB . dgvytedolp ;
} else { rtB . omam4tkvio = rtB . g340u5g2f2 ; } rtB . pekqtbf4ni = rtB .
omam4tkvio ; } else { rtB . ff2obrxiaf = rtB . emlwggxmjw ; rtB . pekqtbf4ni
= rtB . nubgwxsn52 ; } rtB . jv5nlkt2nz = rtB . ff2obrxiaf * rtB . ff2obrxiaf
; rtB . fw3obwamkd = rtB . pekqtbf4ni * rtB . pekqtbf4ni ; rtB . g2mtlhhhmu =
rtB . jv5nlkt2nz + rtB . fw3obwamkd ; rtB . lkfej4vcys = muDoubleScalarSqrt (
rtB . g2mtlhhhmu ) ; if ( rtP . Constant_Value_cm0rgliejy > rtP .
u_Threshold_k4fhghtoj0 ) { rtB . hgxcqhf4yf = ( rtB . lkfej4vcys < rtP .
Constant3_Value_jxtozyvtt0 ) ; rtB . d4floob2bf = rtB . hgxcqhf4yf ; } else {
rtB . e2ho2q2o1s = ( rtB . lkfej4vcys > rtP . Constant3_Value_jxtozyvtt0 ) ;
rtB . d4floob2bf = rtB . e2ho2q2o1s ; } rtB . prwozb0iaj = ( rtB . d4floob2bf
&& rtB . ihhcgp0ixt ) ; if ( rtB . fzjbi0wgls ) { rtB . cglx0ysff3 = rtB .
prwozb0iaj ; } else { rtB . ho54ha1ohi = ( rtB . prwozb0iaj && rtB .
pkhgnulody ) ; rtB . cglx0ysff3 = rtB . ho54ha1ohi ; } rtB . b1mbdtazep = !
rtB . cglx0ysff3 ; rtB . obcxijpzza = rtB . bp3r01ovhm + rtB . ljktjlr15i ;
rtB . lwdyercz35 = rtB . obcxijpzza + rtB . ec3muigggy ; rtB . de3zk10z1a =
rtB . lwdyercz35 * rtP . u7_Value_mt3gzg0prb ; rtB . iv5ucevxdv = rtB .
de3zk10z1a * rtP . Constant10_Value_gcxkgath0o ; rtB . a5zlfxlvqa = rtB .
iv5ucevxdv + rtB . b2qz4pss45 ; rtB . j4pp354rws = rtB . a5zlfxlvqa * rtB .
a5zlfxlvqa ; rtB . bkbepf4twc = rtB . pdkc5n1zyw + rtB . j3k2v5sks2 ; rtB .
c25aqqg2mm = rtB . bkbepf4twc + rtB . cvkdusf3n5 ; rtB . nvrpyl0gaq = rtB .
c25aqqg2mm * rtP . u7_Value_mt3gzg0prb ; rtB . gqrtofbwdf = rtB . nvrpyl0gaq
* rtP . Constant10_Value_gcxkgath0o ; rtB . mzcmsjt4th = rtB . gqrtofbwdf +
rtB . mkmmuynnwa ; rtB . mjd2qrdenv = rtB . mzcmsjt4th * rtB . mzcmsjt4th ;
rtB . icmfd2sumy = rtB . j4pp354rws + rtB . mjd2qrdenv ; rtB . boub5kfyil =
muDoubleScalarSqrt ( rtB . icmfd2sumy ) ; rtB . m3dc1bobvl = rtB . b2qz4pss45
* rtB . b2qz4pss45 ; rtB . kv3hnnttds = rtB . mkmmuynnwa * rtB . mkmmuynnwa ;
rtB . da4bhoykvf = rtB . m3dc1bobvl + rtB . kv3hnnttds ; rtB . egryltuber =
muDoubleScalarSqrt ( rtB . da4bhoykvf ) ; rtB . azmcbinp4f = rtB . egryltuber
* rtP . Constant11_Value_cllfi2bavq ; rtB . bzsskszq4f = ( rtB . boub5kfyil
>= rtB . azmcbinp4f ) ; rtB . a1fppg4cgw = ( rtB . jfr0m0qmvk && rtB .
bzsskszq4f ) ; rtB . najlwsa45m = ( rtB . b1mbdtazep && rtB . a1fppg4cgw ) ;
rtB . nqmh1w3fmq = rtB . de3zk10z1a * rtB . b2qz4pss45 ; rtB . k2x5elbqvf =
rtB . nvrpyl0gaq * rtB . mkmmuynnwa ; rtB . dn4r03clpp = rtB . nqmh1w3fmq +
rtB . k2x5elbqvf ; rtB . fcfwjhlz3w = rtB . b2qz4pss45 * rtB . b2qz4pss45 ;
rtB . i1ikjft4fw = rtB . mkmmuynnwa * rtB . mkmmuynnwa ; rtB . p5ycflme2b =
rtB . fcfwjhlz3w + rtB . i1ikjft4fw ; rtB . hh55xik2i4 = ( rtB . p5ycflme2b <
rtP . Constant_Value_frb3a0i1af ) ; if ( rtB . hh55xik2i4 ) { rtB .
iad2a0u4y3 = rtP . Constant_Value_frb3a0i1af ; } else { rtB . iad2a0u4y3 =
rtB . p5ycflme2b ; } rtB . kyqlcfltgw = rtB . dn4r03clpp / rtB . iad2a0u4y3 ;
rtB . oi5ythw1mz = rtB . kyqlcfltgw * rtB . kyqlcfltgw ; rtB . cq52eamv4a =
rtB . nvrpyl0gaq * rtB . b2qz4pss45 ; rtB . bumul4r2g2 = rtB . de3zk10z1a *
rtB . mkmmuynnwa ; rtB . nqeho1rzvf = rtB . cq52eamv4a - rtB . bumul4r2g2 ;
rtB . oh4umz3di2 = rtB . nqeho1rzvf / rtB . iad2a0u4y3 ; rtB . gcul5rgij1 =
rtB . oh4umz3di2 * rtB . oh4umz3di2 ; rtB . knu5gwamcf = rtB . oi5ythw1mz +
rtB . gcul5rgij1 ; rtB . gtwpx0yren = muDoubleScalarSqrt ( rtB . knu5gwamcf )
; if ( rtP . Constant_Value_ou10xfsezd > rtP . u_Threshold_ak4xlha3l3 ) { rtB
. kses1svz4l = ( rtB . gtwpx0yren < rtP . Constant3_Value_milx5d22sn ) ; rtB
. c0du32l0pu = rtB . kses1svz4l ; } else { rtB . l15bc0jp4d = ( rtB .
gtwpx0yren > rtP . Constant3_Value_milx5d22sn ) ; rtB . c0du32l0pu = rtB .
l15bc0jp4d ; } rtB . kh5rlmddnr = ( rtB . c0du32l0pu && rtB . nti2tirkyr ) ;
if ( rtB . auvirykx13 ) { rtB . lvdf3hf2vq = rtB . kh5rlmddnr ; } else { rtB
. g3lox0gzhb = ( rtB . kh5rlmddnr && rtB . pksmpicrpn ) ; rtB . lvdf3hf2vq =
rtB . g3lox0gzhb ; } rtB . nyg00htfsg = ( rtB . lvdf3hf2vq && rtB .
cglx0ysff3 ) ; rtB . h4b0gegsbt = rtB . pz2k3jltfa * rtB . pz2k3jltfa ; rtB .
blaynmy5oh = rtB . gowzdlfjhz * rtB . gowzdlfjhz ; rtB . gki3ux21r0 = rtB .
h4b0gegsbt + rtB . blaynmy5oh ; rtB . ebouuzj2mh = muDoubleScalarSqrt ( rtB .
gki3ux21r0 ) ; if ( rtP . Constant_Value_diutacna4y > rtP .
u_Threshold_kgvnpxyo4k ) { rtB . lvbz3fwbkb = ( rtB . ebouuzj2mh < rtP .
Constant3_Value_g5uuigvmkt ) ; rtB . befaw4lmur = rtB . lvbz3fwbkb ; } else {
rtB . dxick5aaku = ( rtB . ebouuzj2mh > rtP . Constant3_Value_g5uuigvmkt ) ;
rtB . befaw4lmur = rtB . dxick5aaku ; } rtB . icdfnykxpf = ( rtB . befaw4lmur
&& rtB . hb4n1ud5by ) ; if ( rtB . hpln3qz1no ) { rtB . k5x5ns5y3r = rtB .
icdfnykxpf ; } else { rtB . fbvghbgewp = ( rtB . icdfnykxpf && rtB .
pxnpqvfpjn ) ; rtB . k5x5ns5y3r = rtB . fbvghbgewp ; } if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . jq0f1od2ij = rtDW . j2nec0kpix ; rtB . nzdtdh24o4 = !
rtB . jq0f1od2ij ; } rtB . ncugtneuyh = ( rtB . k5x5ns5y3r && rtB .
nzdtdh24o4 ) ; rtB . ctxnnz5430 = rtB . mfs1yb5ala + rtB . ndskvs431z ; rtB .
cuqs4zzlyw = rtB . ctxnnz5430 + rtB . gwkrvtlqcl ; rtB . hkw5ix33jf = rtB .
cuqs4zzlyw * rtP . u7_Value_cjd00wuyqc ; rtB . blyxjaenym = rtB . hkw5ix33jf
* rtB . hkw5ix33jf ; rtB . afzebyjbot = rtB . h2waz5uker + rtB . jslzn4rlal ;
rtB . felumh2b52 = rtB . afzebyjbot + rtB . e15bejz3up ; rtB . a5kdhehzwg =
rtB . felumh2b52 * rtP . u7_Value_cjd00wuyqc ; rtB . gzny2tvdzp = rtB .
a5kdhehzwg * rtB . a5kdhehzwg ; rtB . exmfinrffn = rtB . blyxjaenym + rtB .
gzny2tvdzp ; rtB . owuj25toet = muDoubleScalarSqrt ( rtB . exmfinrffn ) ; if
( rtP . Constant_Value_bmiydgrnpd > rtP . u_Threshold_kfmyglcdrm ) { rtB .
jofeamzoe4 = ( rtB . owuj25toet < rtP . Constant3_Value_m24220xbpn ) ; rtB .
pgcqkkdykt = rtB . jofeamzoe4 ; } else { rtB . bz3nl4zkfk = ( rtB .
owuj25toet > rtP . Constant3_Value_m24220xbpn ) ; rtB . pgcqkkdykt = rtB .
bz3nl4zkfk ; } rtB . dfn5331tvz = ( rtB . pgcqkkdykt && rtB . kgyemynsqc ) ;
if ( rtB . h0xdccgte4 ) { rtB . ekqskdscbf = rtB . dfn5331tvz ; } else { rtB
. ao1feff3te = ( rtB . dfn5331tvz && rtB . o0tlpivqvc ) ; rtB . ekqskdscbf =
rtB . ao1feff3te ; } if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . oakznquokv
= rtDW . kf53zbkqlj ; rtB . e5nbbplj1y = ! rtB . oakznquokv ; rtB .
jobgz2oz3k = rtDW . ne0hnild1t ; rtB . ezugy1z1yh = ! rtB . jobgz2oz3k ; }
rtB . hagjdqurgf = ( rtB . ekqskdscbf && rtB . e5nbbplj1y ) ; rtB .
k3yal1msok = rtB . de3zk10z1a + rtB . b2qz4pss45 ; rtB . a15lebxm24 = rtB .
k3yal1msok * rtB . k3yal1msok ; rtB . m5xq0hcjla = rtB . nvrpyl0gaq + rtB .
mkmmuynnwa ; rtB . h54jvhf3iq = rtB . m5xq0hcjla * rtB . m5xq0hcjla ; rtB .
kmcucmp0n4 = rtB . a15lebxm24 + rtB . h54jvhf3iq ; rtB . biui3dntr2 =
muDoubleScalarSqrt ( rtB . kmcucmp0n4 ) ; rtB . cfwlhsjp0y = rtB . b2qz4pss45
* rtB . b2qz4pss45 ; rtB . blgfveapbl = rtB . mkmmuynnwa * rtB . mkmmuynnwa ;
rtB . awtrmia3kl = rtB . cfwlhsjp0y + rtB . blgfveapbl ; rtB . bbkhnogn5d =
muDoubleScalarSqrt ( rtB . awtrmia3kl ) ; rtB . jde2atli52 = rtB . bbkhnogn5d
* rtP . Constant2_Value_eeaac3labq ; rtB . cu5bwrrxw3 = ( rtB . biui3dntr2 >=
rtB . jde2atli52 ) ; rtB . or3w5oxavu = ( rtB . ftx2jfkpss && rtB .
cu5bwrrxw3 ) ; rtB . d25p5x0mlt = ( rtB . or3w5oxavu && rtB . ezugy1z1yh ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . megspuf1jr = rtDW . mnr1ns2sul ;
rtB . kbavhm5fl2 = ! rtB . megspuf1jr ; rtB . aksomtla0i = rtDW . ozt4gwuaev
; rtB . m1rofssk41 = ! rtB . aksomtla0i ; } rtB . k1xi42y0ws = ( rtB .
lvdf3hf2vq && rtB . kbavhm5fl2 ) ; rtB . jmeoiwjnyf = ( rtB . a1fppg4cgw &&
rtB . m1rofssk41 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . cdmadmqdzi
= rtDW . e11plvgt2e ; rtB . kzbs0cpz1g = ! rtB . cdmadmqdzi ; } rtB .
kmfrmhw3i3 = ( rtB . cglx0ysff3 && rtB . kzbs0cpz1g ) ; rtB . bkydqfinz1 = (
rtB . ncugtneuyh || rtB . hagjdqurgf || rtB . d25p5x0mlt || rtB . k1xi42y0ws
|| rtB . jmeoiwjnyf || rtB . kmfrmhw3i3 ) ; rtB . hzowa0cifu = ( rtB .
k5x5ns5y3r && rtB . ekqskdscbf ) ; rtB . j5yvlqxpeq = rtB . hkw5ix33jf * rtB
. hkw5ix33jf ; rtB . hykmwh3knb = rtB . a5kdhehzwg * rtB . a5kdhehzwg ; rtB .
min2tqekdf = rtB . j5yvlqxpeq + rtB . hykmwh3knb ; rtB . areasxwmsc =
muDoubleScalarSqrt ( rtB . min2tqekdf ) ; if ( rtP .
Constant_Value_fyywy3oo35 > rtP . u_Threshold_fnzby3fd4u ) { rtB . cyk3wlbobw
= ( rtB . areasxwmsc < rtP . Constant3_Value_noabvcfap1 ) ; rtB . nfvu34enc1
= rtB . cyk3wlbobw ; } else { rtB . akx4ubhhup = ( rtB . areasxwmsc > rtP .
Constant3_Value_noabvcfap1 ) ; rtB . nfvu34enc1 = rtB . akx4ubhhup ; } rtB .
cee2eftsq1 = ( rtB . nfvu34enc1 && rtB . klwphbkdvv ) ; if ( rtB . gz1zr03e2e
) { rtB . i2zmrqbqdw = rtB . cee2eftsq1 ; } else { rtB . pyjhqmudy5 = ( rtB .
cee2eftsq1 && rtB . dlrursw4i1 ) ; rtB . i2zmrqbqdw = rtB . pyjhqmudy5 ; } if
( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . bxkul4jp1y = rtDW . davrjeeiee ;
rtB . j1q2wguzrj = ! rtB . bxkul4jp1y ; } rtB . chbbd1uop3 = ( rtB .
i2zmrqbqdw && rtB . j1q2wguzrj ) ; rtB . g3wj01hwpk = ( rtB . hzowa0cifu ||
rtB . chbbd1uop3 ) ; rtB . m0qqo0stop = ( rtB . k5x5ns5y3r || rtB .
chbbd1uop3 ) ; rtB . mzoj532lqs = ( rtB . gcvsqiszrf && rtB . m0qqo0stop ) ;
rtB . in22cxdyx1 = ( ( rtP . Constant15_Value != 0.0 ) && rtB . g3wj01hwpk )
; rtB . klo2q5p1l2 = rtB . in22cxdyx1 ; rtB . fgcykaqupq = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . ahvbaw04sr , rtB . klo2q5p1l2 , rtB . fgcykaqupq ,
rtP . OffDelay_delay_abmzre2ceu , & rtB . ox1j1fxqy2 , & rtDW . ox1j1fxqy2 ,
& rtP . ox1j1fxqy2 , & rtPrevZCX . ox1j1fxqy2 ) ; bhc3qgnuic ( rtS , rtB .
djdc13epb1 , rtB . klo2q5p1l2 , rtB . fgcykaqupq , rtP .
OffDelay_delay_abmzre2ceu , & rtB . nmew4qgpmi , & rtDW . nmew4qgpmi , & rtP
. nmew4qgpmi , & rtPrevZCX . nmew4qgpmi ) ; rtB . c4ovh2qbj4 = ( rtB .
ox1j1fxqy2 . j1vu52vt5l || rtB . nmew4qgpmi . mjqylog0rb ) ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . itb20enfew = rtDW . avb4nhw551 ; rtB
. jzejrog11u = ! rtB . itb20enfew ; rtB . bgosmcjglc = rtDW . fevmalljs2 ;
rtB . c5dz1jz3cj = ! rtB . bgosmcjglc ; } rtB . ely4tvyguv = ( rtB .
c4ovh2qbj4 && rtB . jzejrog11u ) ; rtB . m43buvs5rc = ssGetT ( rtS ) ; rtB .
lrjpgiqn1a = rtB . c4ovh2qbj4 ; rtB . hwtfjycbkc = rtB . lrjpgiqn1a ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . k5xoneoecv = rtDW . b0vjxwd4dx ; }
itcd43cpff ( rtS , rtB . azidapiaxg [ 0 ] , rtB . hwtfjycbkc , rtB .
k5xoneoecv , & rtB . k050vsxh1j , & rtDW . k050vsxh1j ) ; cgvj5xjnjk ( rtS ,
rtB . azidapiaxg [ 1 ] , rtB . hwtfjycbkc , rtB . k5xoneoecv , & rtB .
ku2xodfghq , & rtDW . ku2xodfghq ) ; rtB . bphodq443b = ( rtB . k050vsxh1j .
guqrn12esp || rtB . ku2xodfghq . l5pxivnpxv ) ; rtB . oiisbbesnh = ( rtB .
c5dz1jz3cj && rtB . bphodq443b ) ; rtB . mqp3u4tglm = rtB . oiisbbesnh ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . mlqrvjmvco = rtDW . fpax42xxm5 ; } if
( rtB . mqp3u4tglm ) { rtB . dfvymmiswe = rtB . m43buvs5rc ; } else { rtB .
dfvymmiswe = rtB . mlqrvjmvco ; } maxV = rtP . u_PulseDuration_druqoueaic -
2.2204460492503131E-16 ; rtB . a2gk430xml = rtB . dfvymmiswe + maxV ; rtB .
ka5yf13wyi = ( rtB . a2gk430xml > rtB . m43buvs5rc ) ; rtB . fahb3xtkie = (
rtB . ely4tvyguv || rtB . ka5yf13wyi ) ; rtB . cq10bsfz33 = rtB . fahb3xtkie
; rtB . dptvdkzbwu = ( rtB . nyg00htfsg || rtB . najlwsa45m ) ; if ( rtB .
otjwraamgd ) { rtB . chmapplgry = rtB . dold2ba2xe ; } else { rtB .
chmapplgry = rtB . ht31pc1po3 ; } rtB . jvni3pqgkp = ssGetT ( rtS ) ; rtB .
pzkitnrzg2 = rtB . mzoj532lqs ; on20slldft ( rtS , rtB . oqikakcicv , rtB .
pzkitnrzg2 , rtB . jvni3pqgkp , rtP . OffDelay1_delay_p2x1cdf1uu , & rtB .
outjvmmxn3 , & rtDW . outjvmmxn3 , & rtP . outjvmmxn3 , & rtPrevZCX .
outjvmmxn3 ) ; bhc3qgnuic ( rtS , rtB . dwtjxxfz20 , rtB . pzkitnrzg2 , rtB .
jvni3pqgkp , rtP . OffDelay1_delay_p2x1cdf1uu , & rtB . brhbedften , & rtDW .
brhbedften , & rtP . brhbedften , & rtPrevZCX . brhbedften ) ; rtB .
nurafxph2n = ( rtB . outjvmmxn3 . j1vu52vt5l || rtB . brhbedften . mjqylog0rb
) ; rtB . hprhlgktl1 = ssGetT ( rtS ) ; rtB . iyghnnjll1 = rtB . or3w5oxavu ;
on20slldft ( rtS , rtB . lsgy0r50gh , rtB . iyghnnjll1 , rtB . hprhlgktl1 ,
rtP . OffDelay2_delay , & rtB . pwnmvwir3d , & rtDW . pwnmvwir3d , & rtP .
pwnmvwir3d , & rtPrevZCX . pwnmvwir3d ) ; bhc3qgnuic ( rtS , rtB . pkadpgiezj
, rtB . iyghnnjll1 , rtB . hprhlgktl1 , rtP . OffDelay2_delay , & rtB .
ozvscdoxcm , & rtDW . ozvscdoxcm , & rtP . ozvscdoxcm , & rtPrevZCX .
ozvscdoxcm ) ; rtB . psquva50gj = ( rtB . pwnmvwir3d . j1vu52vt5l || rtB .
ozvscdoxcm . mjqylog0rb ) ; rtB . fc4quo1vm0 = ssGetT ( rtS ) ; rtB .
psz0b2hcmp = rtB . dptvdkzbwu ; on20slldft ( rtS , rtB . pkofunspzd , rtB .
psz0b2hcmp , rtB . fc4quo1vm0 , rtP . OffDelay3_delay , & rtB . dyyk14ao4t ,
& rtDW . dyyk14ao4t , & rtP . dyyk14ao4t , & rtPrevZCX . dyyk14ao4t ) ;
bhc3qgnuic ( rtS , rtB . j1msf0jlbx , rtB . psz0b2hcmp , rtB . fc4quo1vm0 ,
rtP . OffDelay3_delay , & rtB . k5ecxmrj0a , & rtDW . k5ecxmrj0a , & rtP .
k5ecxmrj0a , & rtPrevZCX . k5ecxmrj0a ) ; rtB . nxylvaytkx = ( rtB .
dyyk14ao4t . j1vu52vt5l || rtB . k5ecxmrj0a . mjqylog0rb ) ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . ic2dednyvx = rtDW . pflycifi2v ; rtB
. m1jgveev45 = ! rtB . ic2dednyvx ; rtB . hyi0ymtru1 = rtDW . kja5xrgmwv ;
rtB . g05v0eyqmu = ! rtB . hyi0ymtru1 ; } rtB . l2druw4pf0 = ( rtB .
psquva50gj && rtB . m1jgveev45 ) ; rtB . ibxpv3nz3n = ssGetT ( rtS ) ; rtB .
gkyply0gjm = rtB . psquva50gj ; rtB . eh1veelmvt = rtB . gkyply0gjm ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . k4qpyrckqi = rtDW . e3tarqgwys ; }
itcd43cpff ( rtS , rtB . b2khhhoouf [ 0 ] , rtB . eh1veelmvt , rtB .
k4qpyrckqi , & rtB . a4ebhrt5bq , & rtDW . a4ebhrt5bq ) ; cgvj5xjnjk ( rtS ,
rtB . b2khhhoouf [ 1 ] , rtB . eh1veelmvt , rtB . k4qpyrckqi , & rtB .
b5umakrf53 , & rtDW . b5umakrf53 ) ; rtB . adpmtm14wa = ( rtB . a4ebhrt5bq .
guqrn12esp || rtB . b5umakrf53 . l5pxivnpxv ) ; rtB . g4idulpmal = ( rtB .
g05v0eyqmu && rtB . adpmtm14wa ) ; rtB . ilszbjin4x = rtB . g4idulpmal ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . o5bu4kx03v = rtDW . hltxj0u51z ; } if
( rtB . ilszbjin4x ) { rtB . oqcqixssx1 = rtB . ibxpv3nz3n ; } else { rtB .
oqcqixssx1 = rtB . o5bu4kx03v ; } maxV = rtP . u_PulseDuration_c2wdzix0b3 -
2.2204460492503131E-16 ; rtB . nmojhwyodl = rtB . oqcqixssx1 + maxV ; rtB .
keb0gbnbec = ( rtB . nmojhwyodl > rtB . ibxpv3nz3n ) ; rtB . gyt50u4rs3 = (
rtB . l2druw4pf0 || rtB . keb0gbnbec ) ; rtB . i1znzqbqns = ssGetT ( rtS ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { fx2esogudm ( rtS , rtB . adpmtm14wa ,
rtB . i1znzqbqns , & rtB . d2nv3h1jhg , & rtDW . d2nv3h1jhg , & rtPrevZCX .
d2nv3h1jhg ) ; rtB . d4um4nabgo = rtB . d2nv3h1jhg . jhmp0c0tsm + rtP .
Constant_Value_lzj1amp5qx ; rtB . g34g3p4ifx = rtDW . hlihecl4y1 ; rtB .
aqfdvrpavg = ! rtB . g34g3p4ifx ; } rtB . pstblafmlt = ( rtB . d4um4nabgo >
rtB . i1znzqbqns ) ; rtB . p3a15jjvyy = ( rtB . nxylvaytkx && rtB .
aqfdvrpavg ) ; rtB . cp4q5wq0ef = ssGetT ( rtS ) ; if ( ssIsSampleHit ( rtS ,
1 , 0 ) ) { rtB . ntnw1g0awh = rtDW . ifc1hwj1ct ; rtB . f5cbkxjf00 = ! rtB .
ntnw1g0awh ; rtB . fks3h4xaku = rtDW . bdb0pmfh2i ; } rtB . ps41ygh1iz = rtB
. nxylvaytkx ; rtB . lpotyyyzgy = rtB . ps41ygh1iz ; itcd43cpff ( rtS , rtB .
ktunfxhhd3 [ 0 ] , rtB . lpotyyyzgy , rtB . fks3h4xaku , & rtB . grl2yxc1of ,
& rtDW . grl2yxc1of ) ; cgvj5xjnjk ( rtS , rtB . ktunfxhhd3 [ 1 ] , rtB .
lpotyyyzgy , rtB . fks3h4xaku , & rtB . cnueqx4gvt , & rtDW . cnueqx4gvt ) ;
rtB . bjqz0hobda = ( rtB . grl2yxc1of . guqrn12esp || rtB . cnueqx4gvt .
l5pxivnpxv ) ; rtB . e2beniqqk3 = ( rtB . f5cbkxjf00 && rtB . bjqz0hobda ) ;
rtB . ltxsopququ = rtB . e2beniqqk3 ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) {
rtB . ba231lvshx = rtDW . lb3vrehkjt ; } if ( rtB . ltxsopququ ) { rtB .
du5okalvyy = rtB . cp4q5wq0ef ; } else { rtB . du5okalvyy = rtB . ba231lvshx
; } maxV = rtP . u_PulseDuration_gukbubxcb5 - 2.2204460492503131E-16 ; rtB .
mx15hrizdr = rtB . du5okalvyy + maxV ; rtB . ct2axyzs4t = ( rtB . mx15hrizdr
> rtB . cp4q5wq0ef ) ; rtB . a1dgna14te = ( rtB . p3a15jjvyy || rtB .
ct2axyzs4t ) ; rtB . b4gvu2rou5 = ssGetT ( rtS ) ; if ( ssIsSampleHit ( rtS ,
1 , 0 ) ) { fx2esogudm ( rtS , rtB . bjqz0hobda , rtB . b4gvu2rou5 , & rtB .
ep3fctwraa , & rtDW . ep3fctwraa , & rtPrevZCX . ep3fctwraa ) ; rtB .
jiktuq0ivq = rtB . ep3fctwraa . jhmp0c0tsm + rtP . Constant_Value_jnovy0uozo
; } rtB . itputxd2k4 = ( rtB . jiktuq0ivq > rtB . b4gvu2rou5 ) ; rtB .
aar10g2vbe = ssGetT ( rtS ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) {
fx2esogudm ( rtS , rtB . bphodq443b , rtB . aar10g2vbe , & rtB . kk13i4dykq ,
& rtDW . kk13i4dykq , & rtPrevZCX . kk13i4dykq ) ; rtB . m3oac4wcft = rtB .
kk13i4dykq . jhmp0c0tsm + rtP . Constant_Value_nas0unw2v0 ; } rtB .
awto0k2m4c = ( rtB . m3oac4wcft > rtB . aar10g2vbe ) ; rtB . lhscspknid = rtB
. pdkc5n1zyw * rtP . u_Value_ms4iwihyl4 ; rtB . flgewwyftd = rtB . bp3r01ovhm
* rtP . u_Value_mtiqblc5v4 ; rtB . m1f3kw2xru = rtB . lhscspknid + rtB .
flgewwyftd ; rtB . gstqs2oayo = rtB . j3k2v5sks2 * rtP . u_Value_ms4iwihyl4 ;
rtB . fikjxxdhci = rtB . ljktjlr15i * rtP . u_Value_mwyupednng ; rtB .
anol2xf13f = rtB . gstqs2oayo + rtB . fikjxxdhci ; rtB . ddogwdso01 = rtB .
m1f3kw2xru + rtB . anol2xf13f ; rtB . ps455030iv = rtB . cvkdusf3n5 + rtB .
ddogwdso01 ; rtB . lsmckj1wew = rtB . pdkc5n1zyw * rtP . u_Value_ms4iwihyl4 ;
rtB . aybyh3nyvf = rtB . bp3r01ovhm * rtP . u_Value_mwyupednng ; rtB .
cj3zf4nlrs = rtB . lsmckj1wew + rtB . aybyh3nyvf ; rtB . dwdgngih0o = rtB .
j3k2v5sks2 * rtP . u_Value_ms4iwihyl4 ; rtB . fv2knsft3a = rtB . ljktjlr15i *
rtP . u_Value_mtiqblc5v4 ; rtB . biwjqunt3e = rtB . dwdgngih0o + rtB .
fv2knsft3a ; rtB . jvxwlnjimq = rtB . cj3zf4nlrs + rtB . biwjqunt3e ; rtB .
d20e5njs2s = rtB . cvkdusf3n5 + rtB . jvxwlnjimq ; rtB . assr0vkrps = rtB .
bp3r01ovhm * rtP . u_Value_ms4iwihyl4 ; rtB . ov405ryte2 = rtB . pdkc5n1zyw *
rtP . u_Value_mwyupednng ; rtB . j2oikvhh5d = rtB . assr0vkrps - rtB .
ov405ryte2 ; rtB . derce4gvpx = rtB . ljktjlr15i * rtP . u_Value_ms4iwihyl4 ;
rtB . gbtblkczch = rtB . j3k2v5sks2 * rtP . u_Value_mtiqblc5v4 ; rtB .
mra41vaiut = rtB . derce4gvpx - rtB . gbtblkczch ; rtB . fkgfj2euge = rtB .
j2oikvhh5d + rtB . mra41vaiut ; rtB . pi3mpl5ygq = rtB . ec3muigggy + rtB .
fkgfj2euge ; rtB . cwb4nsceac = rtB . pi3mpl5ygq * rtP . u7_Value_mt3gzg0prb
; rtB . ljy21xx4uw = rtB . bp3r01ovhm * rtP . u_Value_ms4iwihyl4 ; rtB .
a5oljgzmf5 = rtB . pdkc5n1zyw * rtP . u_Value_mtiqblc5v4 ; rtB . cdia5dqboe =
rtB . ljy21xx4uw - rtB . a5oljgzmf5 ; rtB . k2s2dwdxic = rtB . ljktjlr15i *
rtP . u_Value_ms4iwihyl4 ; rtB . omuj2j3olg = rtB . j3k2v5sks2 * rtP .
u_Value_mwyupednng ; rtB . jlwyuf24rn = rtB . k2s2dwdxic - rtB . omuj2j3olg ;
rtB . fmxcbw1vmm = rtB . cdia5dqboe + rtB . jlwyuf24rn ; rtB . ixlpwmoyab =
rtB . ec3muigggy + rtB . fmxcbw1vmm ; rtB . lltq2gwifd = rtB . ixlpwmoyab *
rtP . u7_Value_mt3gzg0prb ; rtB . nrrqljxgpx = rtB . ps455030iv * rtP .
u7_Value_mt3gzg0prb ; rtB . eff32rhdyw = rtB . d20e5njs2s * rtP .
u7_Value_mt3gzg0prb ; rtB . hcufcd0x3i = rtB . h2waz5uker * rtP .
u_Value_a2mgi0y2ga ; rtB . aqzgimpcy2 = rtB . mfs1yb5ala * rtP .
u_Value_ep5ouv5anm ; rtB . propjyy5wf = rtB . hcufcd0x3i + rtB . aqzgimpcy2 ;
rtB . pwxpnw4nbj = rtB . jslzn4rlal * rtP . u_Value_a2mgi0y2ga ; rtB .
gd2ttgvo0o = rtB . ndskvs431z * rtP . u_Value_free2e3yqx ; rtB . gotdud5yym =
rtB . pwxpnw4nbj + rtB . gd2ttgvo0o ; rtB . alpm3xvqtv = rtB . propjyy5wf +
rtB . gotdud5yym ; rtB . gfzbdudura = rtB . e15bejz3up + rtB . alpm3xvqtv ;
rtB . fpv2teiqd5 = rtB . h2waz5uker * rtP . u_Value_a2mgi0y2ga ; rtB .
klvtmh5ziw = rtB . mfs1yb5ala * rtP . u_Value_free2e3yqx ; rtB . p4pp4c500l =
rtB . fpv2teiqd5 + rtB . klvtmh5ziw ; rtB . pjpyldp1po = rtB . jslzn4rlal *
rtP . u_Value_a2mgi0y2ga ; rtB . aw3npqflo3 = rtB . ndskvs431z * rtP .
u_Value_ep5ouv5anm ; rtB . klj0bhb41o = rtB . pjpyldp1po + rtB . aw3npqflo3 ;
rtB . oavanxgb5q = rtB . p4pp4c500l + rtB . klj0bhb41o ; rtB . ni43lphmkt =
rtB . e15bejz3up + rtB . oavanxgb5q ; rtB . ossxjvz5ho = rtB . mfs1yb5ala *
rtP . u_Value_a2mgi0y2ga ; rtB . l12kmxqazx = rtB . h2waz5uker * rtP .
u_Value_free2e3yqx ; rtB . fldhpwx13f = rtB . ossxjvz5ho - rtB . l12kmxqazx ;
rtB . pmyjjx53d3 = rtB . ndskvs431z * rtP . u_Value_a2mgi0y2ga ; rtB .
pxghoryw0q = rtB . jslzn4rlal * rtP . u_Value_ep5ouv5anm ; rtB . m3fxbvtny0 =
rtB . pmyjjx53d3 - rtB . pxghoryw0q ; rtB . gkjwrtzvnh = rtB . fldhpwx13f +
rtB . m3fxbvtny0 ; rtB . lj2gunqkkj = rtB . gwkrvtlqcl + rtB . gkjwrtzvnh ;
rtB . ldcggrhoa4 = rtB . lj2gunqkkj * rtP . u7_Value_cjd00wuyqc ; rtB .
d0btcrbhkl = rtB . mfs1yb5ala * rtP . u_Value_a2mgi0y2ga ; rtB . p2oeiq0rqt =
rtB . h2waz5uker * rtP . u_Value_ep5ouv5anm ; rtB . bdwcjmlino = rtB .
d0btcrbhkl - rtB . p2oeiq0rqt ; rtB . i4ph1ph41j = rtB . ndskvs431z * rtP .
u_Value_a2mgi0y2ga ; rtB . fi5x4tq1ck = rtB . jslzn4rlal * rtP .
u_Value_free2e3yqx ; rtB . mjhxgaryu5 = rtB . i4ph1ph41j - rtB . fi5x4tq1ck ;
rtB . hl0peh2hzr = rtB . bdwcjmlino + rtB . mjhxgaryu5 ; rtB . p0epwq0l2x =
rtB . gwkrvtlqcl + rtB . hl0peh2hzr ; rtB . kkbichx0pz = rtB . p0epwq0l2x *
rtP . u7_Value_cjd00wuyqc ; rtB . meidp2o1j0 = rtB . gfzbdudura * rtP .
u7_Value_cjd00wuyqc ; rtB . j22ndmtdez = rtB . ni43lphmkt * rtP .
u7_Value_cjd00wuyqc ; rtB . ow1e5izle2 = rtB . lycbvdqjbi * rtB . lycbvdqjbi
; rtB . pelv55ilfw = rtB . l54g0x1tt4 * rtB . l54g0x1tt4 ; rtB . gi1xxrqhvt =
rtB . ow1e5izle2 + rtB . pelv55ilfw ; rtB . nfmqomx1kk = muDoubleScalarSqrt (
rtB . gi1xxrqhvt ) ; rtB . bu42emcfsd = rtB . oxdenoyw40 * rtB . oxdenoyw40 ;
rtB . nor1zyiqk4 = rtB . luxihu0pi4 * rtB . luxihu0pi4 ; rtB . nimucvk1vq =
rtB . bu42emcfsd + rtB . nor1zyiqk4 ; rtB . jzx4ssbydb = muDoubleScalarSqrt (
rtB . nimucvk1vq ) ; rtB . j0zbgsjfg0 = rtB . jf10qmt5ja * rtB . jf10qmt5ja ;
rtB . hhfcsvu3yd = rtB . bnngbimh0v * rtB . bnngbimh0v ; rtB . falh3jqd0l =
rtB . j0zbgsjfg0 + rtB . hhfcsvu3yd ; rtB . bwdkkqhtlx = muDoubleScalarSqrt (
rtB . falh3jqd0l ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . h4wuwcdscg
= rtDW . fra1s15lot ; rtB . iayabds0v4 = ! rtB . h4wuwcdscg ; rtB .
ezblewchvf = rtDW . pvwn2twq14 ; rtB . oycnncmtrm = ! rtB . ezblewchvf ; }
rtB . jhcojupc41 = ( rtB . myd4osbk01 && rtB . iayabds0v4 ) ; rtB .
fe2phksyjg = ( rtB . jhcojupc41 || rtB . i3swf20ecz ) ; rtB . mjclndlywt = (
rtB . fe2phksyjg && rtB . oycnncmtrm ) ; rtB . lrtj53tya4 = muDoubleScalarSin
( rtB . nrz1ahzyq2 ) ; rtB . bytm0pdk12 = muDoubleScalarSin ( rtB .
j3wnlhduer ) ; rtB . dqtwsfhpnz = muDoubleScalarSin ( rtB . kw4f2a2ijs ) ;
rtB . nljhr2sl1i = rtB . mrg3hu21rf * rtB . mrg3hu21rf ; rtB . nn2mppysdz =
rtB . kw14svafax * rtB . kw14svafax ; rtB . a22cboksmb = rtB . nljhr2sl1i +
rtB . nn2mppysdz ; rtB . hirlygmxxz = muDoubleScalarSqrt ( rtB . a22cboksmb )
; rtB . kbpabgddyc = rtB . i2jv0fogyx * rtB . i2jv0fogyx ; rtB . pdcwlwrlgk =
rtB . k10rrojg2s * rtB . k10rrojg2s ; rtB . fj1z2h1kro = rtB . kbpabgddyc +
rtB . pdcwlwrlgk ; rtB . bj3w4kshuv = muDoubleScalarSqrt ( rtB . fj1z2h1kro )
; rtB . p53murcgvx = rtB . bkmtj5x54m * rtB . bkmtj5x54m ; rtB . ppm0vy3knz =
rtB . jtgadx53jo * rtB . jtgadx53jo ; rtB . elznujysto = rtB . p53murcgvx +
rtB . ppm0vy3knz ; rtB . kg0qt4bdaw = muDoubleScalarSqrt ( rtB . elznujysto )
; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { } rtB . hxll4jrmes = rtB .
mrg3hu21rf * rtB . mrg3hu21rf ; rtB . hi32jb4gmf = rtB . kw14svafax * rtB .
kw14svafax ; rtB . ild0z40vuc = rtB . hxll4jrmes + rtB . hi32jb4gmf ; rtB .
eprfynwqta = muDoubleScalarSqrt ( rtB . ild0z40vuc ) ; rtB . dzq5rlb4oi = rtB
. mrg3hu21rf / rtB . eprfynwqta ; u0 = rtB . dzq5rlb4oi ; if ( u0 > 1.0 ) {
u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } rtB . d2gbjrqyxg =
muDoubleScalarAcos ( u0 ) ; rtB . mulgqwqezo = ( rtB . kw14svafax > rtP .
u_Value_pabbxnrts1 ) ; if ( rtB . mulgqwqezo ) { rtB . jp5uals44z = rtB .
d2gbjrqyxg ; } else { rtB . eotsoe4o5p = rtB . d2gbjrqyxg * rtP .
u54_Value_mbufiji5vm ; rtB . jp5uals44z = rtB . eotsoe4o5p ; } rtB .
lu5b3sjezi = rtB . i2jv0fogyx * rtB . i2jv0fogyx ; rtB . aizevu1s5k = rtB .
k10rrojg2s * rtB . k10rrojg2s ; rtB . nbws4eemgo = rtB . lu5b3sjezi + rtB .
aizevu1s5k ; rtB . l1r3gc4ye2 = muDoubleScalarSqrt ( rtB . nbws4eemgo ) ; rtB
. p3eflm2r3l = rtB . i2jv0fogyx / rtB . l1r3gc4ye2 ; u0 = rtB . p3eflm2r3l ;
if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } }
rtB . eaykvbwp4e = muDoubleScalarAcos ( u0 ) ; rtB . kwquago2qw = ( rtB .
k10rrojg2s > rtP . u_Value_hfcnazmhpm ) ; if ( rtB . kwquago2qw ) { rtB .
oo3yxnhr4s = rtB . eaykvbwp4e ; } else { rtB . prffjqunp5 = rtB . eaykvbwp4e
* rtP . u54_Value_akivy5tr10 ; rtB . oo3yxnhr4s = rtB . prffjqunp5 ; } if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { } rtB . clchwvc55r = rtB . bkmtj5x54m * rtB
. bkmtj5x54m ; rtB . kaujbndvxb = rtB . jtgadx53jo * rtB . jtgadx53jo ; rtB .
pp1bgzydkr = rtB . clchwvc55r + rtB . kaujbndvxb ; rtB . l4xpclaavm =
muDoubleScalarSqrt ( rtB . pp1bgzydkr ) ; rtB . nnk4stpsh1 = rtB . bkmtj5x54m
/ rtB . l4xpclaavm ; u0 = rtB . nnk4stpsh1 ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } rtB . f3un2qo4ba =
muDoubleScalarAcos ( u0 ) ; rtB . gbohxta3sq = ( rtB . jtgadx53jo > rtP .
u_Value_fbxe2zvjaz ) ; if ( rtB . gbohxta3sq ) { rtB . onzjrjcde4 = rtB .
f3un2qo4ba ; } else { rtB . ba31ad2css = rtB . f3un2qo4ba * rtP .
u54_Value_bsxv4a5mzb ; rtB . onzjrjcde4 = rtB . ba31ad2css ; } rtB .
e3q5nzopld = ( rtB . lm2e5d3i5a && rtB . pjuqkltmtb && rtB . am5ndynrmd &&
rtB . jbvudmez1s ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . joxcxuf2fr
= rtDW . fwvi3ktkgn ; rtB . bhocb04qj3 = ! rtB . joxcxuf2fr ; rtB .
hsnebhqf0j = rtDW . danlnozb3w ; rtB . nbwkcdvyj5 = ! rtB . hsnebhqf0j ; }
rtB . l0ww1zf4ch = ( rtB . k3om0acsgh && rtB . bhocb04qj3 ) ; rtB .
mw0rsnzkxz = ( rtB . pjuqkltmtb && rtB . l0ww1zf4ch ) ; rtB . c40cqj41hh = (
rtB . io0nqp4diw && rtB . nbwkcdvyj5 ) ; rtB . fpms1ltdv4 = ( rtB .
am5ndynrmd && rtB . c40cqj41hh ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB
. jgui5mt155 = rtDW . dglgxggn0s ; rtB . biszriate4 = ! rtB . jgui5mt155 ; }
rtB . gfosyp2idh = ( rtB . ktvbljhq2p && rtB . biszriate4 ) ; rtB .
aftolut4oy = ( rtB . jbvudmez1s && rtB . gfosyp2idh ) ; rtB . bsep23qvzq = (
rtB . mw0rsnzkxz || rtB . fpms1ltdv4 || rtB . aftolut4oy ) ; if ( rtB .
bsep23qvzq ) { rtB . m5mezpdp4q = rtB . hejffordvr ; } else { rtB .
m5mezpdp4q = rtP . u_Value_potytjhwin ; } rtB . c4nafd2ryr = ( rtB .
l0ww1zf4ch || rtB . c40cqj41hh || rtB . gfosyp2idh ) ; if ( rtB . c4nafd2ryr
) { rtB . p1waf1qzb5 = rtB . lrtmvxgodg ; } else { rtB . p1waf1qzb5 = rtP .
u_Value_potytjhwin ; } rtB . cwrzwiwae3 = rtB . hxgvln4wyf * rtP .
u_Value_gdlj1tkcqv ; rtB . ovidyw5bq3 = rtB . acu2uimhf4 * rtP .
u_Value_j1xcor5ntr ; rtB . pe3ggrw44h = rtB . cwrzwiwae3 + rtB . ovidyw5bq3 ;
rtB . pn4h152w44 = rtB . bolkwwrtqf * rtP . u_Value_gdlj1tkcqv ; rtB .
lnehfd55xc = rtB . fd2yp3dpzd * rtP . u_Value_gsznkmv0cs ; rtB . csfieykogx =
rtB . pn4h152w44 + rtB . lnehfd55xc ; rtB . bwdnbmizln = rtB . pe3ggrw44h +
rtB . csfieykogx ; rtB . j3un1zytzk = rtB . pbzxcpqffu + rtB . bwdnbmizln ;
rtB . krqnb2xnfb = rtB . hxgvln4wyf * rtP . u_Value_gdlj1tkcqv ; rtB .
gdgaz4hpby = rtB . acu2uimhf4 * rtP . u_Value_gsznkmv0cs ; rtB . ecd2ttmjpp =
rtB . krqnb2xnfb + rtB . gdgaz4hpby ; rtB . dv3hohx2tq = rtB . bolkwwrtqf *
rtP . u_Value_gdlj1tkcqv ; rtB . diapbf4orc = rtB . fd2yp3dpzd * rtP .
u_Value_j1xcor5ntr ; rtB . or5fomb0fm = rtB . dv3hohx2tq + rtB . diapbf4orc ;
rtB . ozhr0bmtmd = rtB . ecd2ttmjpp + rtB . or5fomb0fm ; rtB . pb4uy2x1d1 =
rtB . pbzxcpqffu + rtB . ozhr0bmtmd ; rtB . fcqpp1wqro = rtB . acu2uimhf4 *
rtP . u_Value_gdlj1tkcqv ; rtB . dujflp2eij = rtB . hxgvln4wyf * rtP .
u_Value_gsznkmv0cs ; rtB . psxtzex54p = rtB . fcqpp1wqro - rtB . dujflp2eij ;
rtB . j1ldk2y3l4 = rtB . fd2yp3dpzd * rtP . u_Value_gdlj1tkcqv ; rtB .
beeyeb24pu = rtB . bolkwwrtqf * rtP . u_Value_j1xcor5ntr ; rtB . fur2lbexzp =
rtB . j1ldk2y3l4 - rtB . beeyeb24pu ; rtB . jm4xyaomds = rtB . psxtzex54p +
rtB . fur2lbexzp ; rtB . kccbs1ybeu = rtB . jgj2f2cg2v + rtB . jm4xyaomds ;
rtB . lsapqdb0fd = rtB . kccbs1ybeu * rtP . u7_Value_krd2xsctxt ; rtB .
o32g4ya4qk = rtB . acu2uimhf4 * rtP . u_Value_gdlj1tkcqv ; rtB . gi5l2y3nxc =
rtB . hxgvln4wyf * rtP . u_Value_j1xcor5ntr ; rtB . nwost2htek = rtB .
o32g4ya4qk - rtB . gi5l2y3nxc ; rtB . axnhmi5aek = rtB . fd2yp3dpzd * rtP .
u_Value_gdlj1tkcqv ; rtB . bsb4h1qx32 = rtB . bolkwwrtqf * rtP .
u_Value_gsznkmv0cs ; rtB . hcceoxywck = rtB . axnhmi5aek - rtB . bsb4h1qx32 ;
rtB . cnvzxkzewc = rtB . nwost2htek + rtB . hcceoxywck ; rtB . oyct0jxuri =
rtB . jgj2f2cg2v + rtB . cnvzxkzewc ; rtB . kjg35zaidz = rtB . oyct0jxuri *
rtP . u7_Value_krd2xsctxt ; rtB . hq3n5h0pzp = rtB . j3un1zytzk * rtP .
u7_Value_krd2xsctxt ; rtB . dynhuy0ml2 = rtB . pb4uy2x1d1 * rtP .
u7_Value_krd2xsctxt ; rtB . p5xsjunjvv = rtB . k01n0yrx1p * rtB . k01n0yrx1p
; rtB . iltiazywvd = rtB . intuomh1ly * rtB . intuomh1ly ; rtB . efhr3sdemg =
rtB . p5xsjunjvv + rtB . iltiazywvd ; rtB . mdrvkmbhfd = muDoubleScalarSqrt (
rtB . efhr3sdemg ) ; rtB . i4m5corbf0 = rtB . czvwxyormz * rtB . czvwxyormz ;
rtB . oz5dneo0k5 = rtB . j5nwsmpmbt * rtB . j5nwsmpmbt ; rtB . ek3lb5r53t =
rtB . i4m5corbf0 + rtB . oz5dneo0k5 ; rtB . a5y21mmhoa = muDoubleScalarSqrt (
rtB . ek3lb5r53t ) ; rtB . cd5tx5yehr = rtB . mwhlgznwmb * rtB . mwhlgznwmb ;
rtB . kkkktykipv = rtB . d35jbeyxvo * rtB . d35jbeyxvo ; rtB . mqru2gz2zx =
rtB . cd5tx5yehr + rtB . kkkktykipv ; rtB . o01nupvayx = muDoubleScalarSqrt (
rtB . mqru2gz2zx ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { } rtB .
asx5ce41jq = rtB . k01n0yrx1p * rtB . k01n0yrx1p ; rtB . dkhvkzkwgb = rtB .
intuomh1ly * rtB . intuomh1ly ; rtB . ibzbniagry = rtB . asx5ce41jq + rtB .
dkhvkzkwgb ; rtB . cbjtpakggp = muDoubleScalarSqrt ( rtB . ibzbniagry ) ; rtB
. olknzam1o0 = rtB . k01n0yrx1p / rtB . cbjtpakggp ; u0 = rtB . olknzam1o0 ;
if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } }
rtB . jhygde3iv3 = muDoubleScalarAcos ( u0 ) ; rtB . iazyipllev = ( rtB .
intuomh1ly > rtP . u_Value_j433hv3m2d ) ; if ( rtB . iazyipllev ) { rtB .
psndy1flit = rtB . jhygde3iv3 ; } else { rtB . kgxo01lx52 = rtB . jhygde3iv3
* rtP . u54_Value_hqjfnn1l5l ; rtB . psndy1flit = rtB . kgxo01lx52 ; } rtB .
plysydge0k = rtB . czvwxyormz * rtB . czvwxyormz ; rtB . oasqk4ysn0 = rtB .
j5nwsmpmbt * rtB . j5nwsmpmbt ; rtB . j2gbbbjv2n = rtB . plysydge0k + rtB .
oasqk4ysn0 ; rtB . b2mro1epzb = muDoubleScalarSqrt ( rtB . j2gbbbjv2n ) ; rtB
. d10hr2yzgx = rtB . czvwxyormz / rtB . b2mro1epzb ; u0 = rtB . d10hr2yzgx ;
if ( u0 > 1.0 ) { u0 = 1.0 ; } else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } }
rtB . g5fnwehtcc = muDoubleScalarAcos ( u0 ) ; rtB . jny1y4nhtr = ( rtB .
j5nwsmpmbt > rtP . u_Value_fe45bafuas ) ; if ( rtB . jny1y4nhtr ) { rtB .
grqmsnwrof = rtB . g5fnwehtcc ; } else { rtB . n5aj5fc02u = rtB . g5fnwehtcc
* rtP . u54_Value_gtikqp3cox ; rtB . grqmsnwrof = rtB . n5aj5fc02u ; } if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { } rtB . cbhr0wplhc = rtB . mwhlgznwmb * rtB
. mwhlgznwmb ; rtB . bac5bsjmdp = rtB . d35jbeyxvo * rtB . d35jbeyxvo ; rtB .
h3fzxjd0cb = rtB . cbhr0wplhc + rtB . bac5bsjmdp ; rtB . dgyv5mj0vw =
muDoubleScalarSqrt ( rtB . h3fzxjd0cb ) ; rtB . oq4ctfse4z = rtB . mwhlgznwmb
/ rtB . dgyv5mj0vw ; u0 = rtB . oq4ctfse4z ; if ( u0 > 1.0 ) { u0 = 1.0 ; }
else { if ( u0 < - 1.0 ) { u0 = - 1.0 ; } } rtB . lk0pdi5m4q =
muDoubleScalarAcos ( u0 ) ; rtB . p1ocqoljgc = ( rtB . d35jbeyxvo > rtP .
u_Value_lmp45tiqvw ) ; if ( rtB . p1ocqoljgc ) { rtB . iamr4nmyrc = rtB .
lk0pdi5m4q ; } else { rtB . g5y4nixxdr = rtB . lk0pdi5m4q * rtP .
u54_Value_fghd4hvwe0 ; rtB . iamr4nmyrc = rtB . g5y4nixxdr ; } rtB .
hq3i55serw = ( rtB . e3q5nzopld && rtB . n4ttfrgcra && rtB . o40bui2tqj &&
rtB . nlggomiw2m ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . oh5uir2wrf
= rtDW . levorifkrw ; rtB . mebe2noqcm = ! rtB . oh5uir2wrf ; rtB .
e4abwrjlc0 = rtDW . hxmfwxg20s ; rtB . aye4q4bhn2 = ! rtB . e4abwrjlc0 ; }
rtB . erfmcziqlr = ( rtB . a0it33vpv3 && rtB . mebe2noqcm ) ; rtB .
ghiiv0ya2c = ( rtB . n4ttfrgcra && rtB . erfmcziqlr ) ; rtB . hrvopls5mp = (
rtB . gr4vjiot5p && rtB . aye4q4bhn2 ) ; rtB . c03gsqjxid = ( rtB .
o40bui2tqj && rtB . hrvopls5mp ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB
. opdavil0sw = rtDW . hdod2qacjn ; rtB . jud30e1m3c = ! rtB . opdavil0sw ; }
rtB . bfa5lgoqsk = ( rtB . lz0cksews5 && rtB . jud30e1m3c ) ; rtB .
nbiv0jxnfr = ( rtB . nlggomiw2m && rtB . bfa5lgoqsk ) ; rtB . dbynclsh2a = (
rtB . ghiiv0ya2c || rtB . c03gsqjxid || rtB . nbiv0jxnfr ) ; if ( rtB .
dbynclsh2a ) { rtB . fsoajofl04 = rtB . busiz2uss1 ; } else { rtB .
fsoajofl04 = rtP . u_Value_cgx53hb23m ; } rtB . k30gvk3ggp = ( rtB .
erfmcziqlr || rtB . hrvopls5mp || rtB . bfa5lgoqsk ) ; if ( rtB . k30gvk3ggp
) { rtB . j1lwwu5slg = rtB . emmckbzbao ; } else { rtB . j1lwwu5slg = rtP .
u_Value_cgx53hb23m ; } rtB . efbnjzd5e1 = rtB . ddix2mhi1g * rtP .
u_Value_lg2f0bb3da ; rtB . br5bhi3u43 = rtB . asa103d02z * rtP .
u_Value_by3i5cyty5 ; rtB . lqoxn3km5k = rtB . efbnjzd5e1 + rtB . br5bhi3u43 ;
rtB . afodtjyys0 = rtB . ndiffna1hx * rtP . u_Value_lg2f0bb3da ; rtB .
ort2j40cem = rtB . fizn4ck4n1 * rtP . u_Value_cvjm1sv1pd ; rtB . f5y1zqeodb =
rtB . afodtjyys0 + rtB . ort2j40cem ; rtB . clyhpw0amj = rtB . lqoxn3km5k +
rtB . f5y1zqeodb ; rtB . pohkleg242 = rtB . krvqv214qo + rtB . clyhpw0amj ;
rtB . fkh2lxz3mf = rtB . ddix2mhi1g * rtP . u_Value_lg2f0bb3da ; rtB .
egrd54z2w5 = rtB . asa103d02z * rtP . u_Value_cvjm1sv1pd ; rtB . jguof4ln5u =
rtB . fkh2lxz3mf + rtB . egrd54z2w5 ; rtB . jvutu3az2p = rtB . ndiffna1hx *
rtP . u_Value_lg2f0bb3da ; rtB . eqs1nj1mzf = rtB . fizn4ck4n1 * rtP .
u_Value_by3i5cyty5 ; rtB . j4cdm2sb3f = rtB . jvutu3az2p + rtB . eqs1nj1mzf ;
rtB . kh5jnrjphs = rtB . jguof4ln5u + rtB . j4cdm2sb3f ; rtB . bcuwmhwdkl =
rtB . krvqv214qo + rtB . kh5jnrjphs ; rtB . niki3imf1k = rtB . asa103d02z *
rtP . u_Value_lg2f0bb3da ; rtB . hidhqi0o4k = rtB . ddix2mhi1g * rtP .
u_Value_cvjm1sv1pd ; rtB . jya2sknwz0 = rtB . niki3imf1k - rtB . hidhqi0o4k ;
rtB . oguvc221nt = rtB . fizn4ck4n1 * rtP . u_Value_lg2f0bb3da ; rtB .
glksyogaev = rtB . ndiffna1hx * rtP . u_Value_by3i5cyty5 ; rtB . cbozpswss5 =
rtB . oguvc221nt - rtB . glksyogaev ; rtB . dsatg3jnbr = rtB . jya2sknwz0 +
rtB . cbozpswss5 ; rtB . g5dvl22sae = rtB . hlmtyy2tbw + rtB . dsatg3jnbr ;
rtB . cfd5bolgtd = rtB . g5dvl22sae * rtP . u7_Value_cmbk5pxons ; rtB .
llcbjv5nfw = rtB . asa103d02z * rtP . u_Value_lg2f0bb3da ; rtB . l4erw3ktde =
rtB . ddix2mhi1g * rtP . u_Value_by3i5cyty5 ; rtB . kiuthn5jyq = rtB .
llcbjv5nfw - rtB . l4erw3ktde ; rtB . i4phahkph4 = rtB . fizn4ck4n1 * rtP .
u_Value_lg2f0bb3da ; rtB . fe2gbnwlxh = rtB . ndiffna1hx * rtP .
u_Value_cvjm1sv1pd ; rtB . odaole1p4c = rtB . i4phahkph4 - rtB . fe2gbnwlxh ;
rtB . i4u005wj4c = rtB . kiuthn5jyq + rtB . odaole1p4c ; rtB . gyr1aot2zw =
rtB . hlmtyy2tbw + rtB . i4u005wj4c ; rtB . ginn2kiwz5 = rtB . gyr1aot2zw *
rtP . u7_Value_cmbk5pxons ; rtB . a4pyltb2ne = rtB . pohkleg242 * rtP .
u7_Value_cmbk5pxons ; rtB . p0zsdu3dab = rtB . bcuwmhwdkl * rtP .
u7_Value_cmbk5pxons ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . nz5jwbolrv
= rtDW . k11kfdk1be ; rtB . hea5utq0vn = ! rtB . nz5jwbolrv ; rtB .
ats4jp00vb = false ; rtB . cuha1a5gfx = ( rtB . hevngmr4q4 && rtB .
ats4jp00vb ) ; rtB . eqdq33evas = rtDW . bdb5r4moyp ; rtB . jx2rzsfuon = !
rtB . eqdq33evas ; rtB . k1hnhfwb14 = false ; rtB . g3qupsng1l = ( rtB .
kspoxb2j1y && rtB . k1hnhfwb14 ) ; rtB . n2iw4dt01a = rtDW . mhkbaz10cz ; rtB
. gh2ejch2zj = ! rtB . n2iw4dt01a ; rtB . cqswfgsdrb = false ; rtB .
lvdfjgfv0h = ( rtB . gjgc5vzmvw && rtB . cqswfgsdrb ) ; rtB . flmih5nfx0 = (
rtB . cuha1a5gfx || rtB . g3qupsng1l || rtB . lvdfjgfv0h ) ; if ( rtB .
flmih5nfx0 ) { rtB . afz4221tq3 = rtB . cmbsfr5c55 ; } else { rtB .
afz4221tq3 = rtP . u_Value_h5ek13jpgh ; } rtB . hmamvdy133 = ( rtB .
ats4jp00vb || rtB . k1hnhfwb14 || rtB . cqswfgsdrb ) ; if ( rtB . hmamvdy133
) { rtB . c1wbiiv3yd = rtB . k53iwvpvph ; } else { rtB . c1wbiiv3yd = rtP .
u_Value_h5ek13jpgh ; } { if ( ( rtDW . kuceggnsk2 . AQHandles || rtDW .
kuceggnsk2 . SlioLTF ) && ssGetLogOutput ( rtS ) ) { sdiSlioSdiWriteSignal (
rtDW . kuceggnsk2 . AQHandles , rtDW . kuceggnsk2 . SlioLTF , 0 ,
ssGetTaskTime ( rtS , 1 ) , ( char * ) & rtB . gu30hru3p3 + 0 ) ; } } } rtB .
mumln5bbcb = ssGetT ( rtS ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) {
fx2esogudm ( rtS , rtB . bfqbvqy5gl , rtB . mumln5bbcb , & rtB . nx03ta252g ,
& rtDW . nx03ta252g , & rtPrevZCX . nx03ta252g ) ; rtB . aufq34vsvo = rtB .
nx03ta252g . jhmp0c0tsm + rtP . Constant_Value_dr0a20tbni ; } rtB .
n5nxbcgxr1 = ( rtB . aufq34vsvo > rtB . mumln5bbcb ) ; rtB . hes02hxccs =
ssGetT ( rtS ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { fx2esogudm ( rtS ,
rtB . ap2ehl2p0j , rtB . hes02hxccs , & rtB . j33xaeouj5 , & rtDW .
j33xaeouj5 , & rtPrevZCX . j33xaeouj5 ) ; rtB . at0hxwglrf = rtB . j33xaeouj5
. jhmp0c0tsm + rtP . Constant_Value_ggtqbaslub ; } rtB . nmz0p3mxy5 = ( rtB .
at0hxwglrf > rtB . hes02hxccs ) ; rtB . ae5f55qiel = ssGetT ( rtS ) ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { fx2esogudm ( rtS , rtB . cyycyjy3do , rtB .
ae5f55qiel , & rtB . pk2r3vb4fa , & rtDW . pk2r3vb4fa , & rtPrevZCX .
pk2r3vb4fa ) ; rtB . pdstlnb14u = rtB . pk2r3vb4fa . jhmp0c0tsm + rtP .
Constant_Value_goom2sqacd ; rtB . ov0nifkpdx = rtDW . f5dfmvvs02 ; rtB .
aypcm4e4nx = rtDW . ecq2t4tug1 ; if ( rtP . Constant4_Value_hg2r4jwjvy > rtP
. u_Threshold_ih5cat4por ) { rtB . bkwtgv5jnq = ! rtB . aypcm4e4nx ; rtB .
jcmd1myrd0 = false ; rtB . c5jnnqrif5 = rtB . jcmd1myrd0 ; } else { rtB .
c5jnnqrif5 = false ; } } rtB . lol52o0gn3 = ( rtB . pdstlnb14u > rtB .
ae5f55qiel ) ; rtB . it14eqzvgy = false ; rtB . m5xg5a05jf = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . cenlyv43wy , rtB . it14eqzvgy , rtB . m5xg5a05jf ,
rtP . T1_delay_ng1njomtoa , & rtB . c2rt0zs300 , & rtDW . c2rt0zs300 , & rtP
. c2rt0zs300 , & rtPrevZCX . c2rt0zs300 ) ; bhc3qgnuic ( rtS , rtB .
p4fh21c2hg , rtB . it14eqzvgy , rtB . m5xg5a05jf , rtP . T1_delay_ng1njomtoa
, & rtB . pj2a3t0x3l , & rtDW . pj2a3t0x3l , & rtP . pj2a3t0x3l , & rtPrevZCX
. pj2a3t0x3l ) ; rtB . hxsmba1mii = ( rtB . c2rt0zs300 . j1vu52vt5l || rtB .
pj2a3t0x3l . mjqylog0rb ) ; rtB . ittyjcketk = ( rtB . c5jnnqrif5 && rtB .
hxsmba1mii ) ; rtB . aifwytkezh = ( rtB . ov0nifkpdx || rtB . ittyjcketk ) ;
{ real_T * * uBuffer = ( real_T * * ) & rtDW . l0gmb53kdd . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & rtDW . l0gmb53kdd . TUbufferPtrs [
1 ] ; real_T * * xBuffer = ( real_T * * ) & rtDW . l0gmb53kdd . TUbufferPtrs
[ 2 ] ; real_T simTime = ssGetT ( rtS ) ; real_T appliedDelay ; rtB .
dzfs2soc0r = rt_VTDelayfindtDInterpolate ( rtX . gukucggul0 , * tBuffer , *
uBuffer , * xBuffer , rtDW . ees1hdl3bo . CircularBufSize , rtDW . ees1hdl3bo
. Head , rtDW . ees1hdl3bo . Tail , & rtDW . ees1hdl3bo . Last , simTime ,
0.0 , 1 , ( boolean_T ) ( ssIsMinorTimeStep ( rtS ) && (
ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) , rtP .
VariableTransportDelay_InitOutput_cgrxqg4j0g , & appliedDelay ) ; } rtB .
fe5fx4wkv3 = ! ( rtB . dzfs2soc0r != 0.0 ) ; rtB . ov31k4ksh0 = ( rtB .
aifwytkezh && rtB . fe5fx4wkv3 ) ; rtB . icivudkp5g = ( rtB . azicm3ai4d &&
rtB . ov31k4ksh0 ) ; rtB . gtw2wfaccf = rtB . icivudkp5g ; rtB . obde4yjr25 =
rtB . gtw2wfaccf ; rtB . e52gwcb4vu = rtB . dxa1300d4c * rtB . dxa1300d4c ;
rtB . k4x3tk1m4y = rtB . dgvytedolp * rtB . dgvytedolp ; rtB . gbywlbvbxp =
rtB . e52gwcb4vu + rtB . k4x3tk1m4y ; rtB . jjadnwoock = muDoubleScalarSqrt (
rtB . gbywlbvbxp ) ; if ( rtB . nunmcefaox ) { rtB . kuanwhymkd = rtB .
jjadnwoock / rtB . hzvmm3pybv ; rtB . oy1ujrnuj1 = muDoubleScalarPower ( rtB
. kuanwhymkd , rtB . ek3e1fl33p ) ; rtB . ano3tmdfzc = rtP .
Constant3_Value_e3s5vo43wq - rtB . oy1ujrnuj1 ; rtB . nzewc0brxt = ( rtB .
ano3tmdfzc == rtP . Constant_Value_kudnxzu23u ) ; if ( rtB . nzewc0brxt ) {
rtB . jkyoltd1c5 = rtP . Constant1_Value_nbd4qyavgi ; } else { rtB .
jkyoltd1c5 = rtB . ano3tmdfzc ; } rtB . brnomnveod = rtB . ed10njbbmu / rtB .
jkyoltd1c5 ; rtB . k03udy0pmm = rtB . brnomnveod * rtP .
Constant3_Value_lv3u2r25ay ; rtB . dpoumefwk4 = rtB . k03udy0pmm ; } else {
rtB . dpoumefwk4 = rtP . Constant19_Value ; } if ( rtB . f2y4g1anxz ) { rtB .
iiouexeshr = rtB . jjadnwoock / rtB . i1f2menktp ; rtB . k0uonwf41p =
muDoubleScalarPower ( rtB . iiouexeshr , rtB . gmwfqnawfk ) ; rtB .
csl1n5kuyx = rtP . Constant3_Value_crc01bu1au - rtB . k0uonwf41p ; rtB .
k2nykrblur = ( rtB . csl1n5kuyx == rtP . Constant_Value_iy2152jmb3 ) ; if (
rtB . k2nykrblur ) { rtB . op2qmlinqi = rtP . Constant1_Value_dnz5snrdsl ; }
else { rtB . op2qmlinqi = rtB . csl1n5kuyx ; } rtB . f02ai0bb4t = rtB .
dzpb0wpbrp / rtB . op2qmlinqi ; rtB . evfxxd3hiy = rtB . f02ai0bb4t + rtB .
g4nuvepmng ; rtB . oyqbvyo1lm = rtB . evfxxd3hiy * rtP .
Constant7_Value_aizakccmpk ; rtB . eihylt2ocw = rtB . oyqbvyo1lm ; } else {
rtB . eihylt2ocw = rtP . Constant18_Value_pvywbsxg3c ; } rtB . lc23kplapx =
rtB . dpoumefwk4 + rtB . eihylt2ocw ; if ( rtP . Constant20_Value_eexfd5nk20
> rtP . Switch3_Threshold ) { rtB . k4zmlg4lqo = rtB . jjadnwoock / rtB .
b5gb4adov3 ; rtB . krp4otjf53 = ( rtB . k4zmlg4lqo >= rtP .
Constant18_Value_o4uwyepiv5 ) ; if ( rtB . krp4otjf53 ) { rtB . kiwxfpvkct =
rtP . Constant18_Value_o4uwyepiv5 ; } else { rtB . hpxx52kcop = rtB .
jjadnwoock / rtB . b5gb4adov3 ; rtB . kiwxfpvkct = rtB . hpxx52kcop ; } rtB .
pif34u2ff3 = muDoubleScalarPower ( rtB . kiwxfpvkct , rtB . k1laehtn1k ) ;
rtB . gfmcgnloru = rtB . pif34u2ff3 - rtP . Constant3_Value_jp3ysfutcb ; rtB
. gwgttyby5z = ( rtB . gfmcgnloru == rtP . Constant_Value_ctkd4hcfdn ) ; if (
rtB . gwgttyby5z ) { rtB . p2q0zvf2zp = rtP . Constant1_Value_mabwe2ywdl ; }
else { rtB . p2q0zvf2zp = rtB . gfmcgnloru ; } rtB . lh1nzik3tz = rtB .
psmo35sklv / rtB . p2q0zvf2zp ; rtB . ovkvpvc4lq = rtB . lh1nzik3tz * rtP .
Constant3_Value_lv3u2r25ay ; rtB . erpywr2s5s = rtB . ovkvpvc4lq ; } else {
rtB . gej1a2dipk = rtB . jjadnwoock / rtB . fprxwhlbgx ; rtB . nbpikpmjkw = (
rtB . gej1a2dipk >= rtP . Constant18_Value ) ; if ( rtB . nbpikpmjkw ) { rtB
. otxndy0vfc = rtP . Constant18_Value ; } else { rtB . otxndy0vfc = rtB .
gej1a2dipk ; } rtB . ld1xpix1wp = muDoubleScalarPower ( rtB . otxndy0vfc ,
rtB . ph2255r30k ) ; rtB . drbwcy2bkl = rtB . ld1xpix1wp - rtP .
Constant3_Value_n2hxuqkuhk ; rtB . lgb0uyzgu3 = ( rtB . drbwcy2bkl == rtP .
Constant_Value_ek05pics4d ) ; if ( rtB . lgb0uyzgu3 ) { rtB . mfy5iohnoo =
rtP . Constant1_Value_ml02k5lucj ; } else { rtB . mfy5iohnoo = rtB .
drbwcy2bkl ; } rtB . guokg2whcs = rtB . bzxe4atotg / rtB . mfy5iohnoo ; rtB .
nvhltvrkoe = rtB . guokg2whcs + rtB . d4ba3f1s5c ; rtB . pmr3sb1qkm = rtB .
nvhltvrkoe * rtP . Constant7_Value_aizakccmpk ; rtB . erpywr2s5s = rtB .
pmr3sb1qkm ; } { real_T * * uBuffer = ( real_T * * ) & rtDW . i3r0m4gxn0 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & rtDW . i3r0m4gxn0
. TUbufferPtrs [ 1 ] ; real_T * * xBuffer = ( real_T * * ) & rtDW .
i3r0m4gxn0 . TUbufferPtrs [ 2 ] ; real_T simTime = ssGetT ( rtS ) ; real_T
appliedDelay ; rtB . kaj1uwadle = rt_VTDelayfindtDInterpolate ( rtX .
cooihtzhhj , * tBuffer , * uBuffer , * xBuffer , rtDW . gty02ianr0 .
CircularBufSize , rtDW . gty02ianr0 . Head , rtDW . gty02ianr0 . Tail , &
rtDW . gty02ianr0 . Last , simTime , 0.0 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS
) ) ) , rtP . VariableTransportDelay_InitOutput_dqdd2mgeon , & appliedDelay )
; } rtB . dgkrnt3j3u = false ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
kutnkguaaq = rtDW . ph2qy0rwkn ; rtB . lw0wyy0us0 = ! rtB . kutnkguaaq ; rtB
. bqq34kehej = rtDW . ojkg1k5daw ; rtB . al5nty1gfn = ! rtB . bqq34kehej ; }
rtB . e3wvgrwqct = ( rtB . dgkrnt3j3u && rtB . lw0wyy0us0 ) ; rtB .
pzjojlj2y3 = ssGetT ( rtS ) ; rtB . pp551ftemg = rtB . dgkrnt3j3u ; rtB .
gjp3g4kh1s = rtB . pp551ftemg ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
mapzoy3ans = rtDW . a0uzfqyhcp ; } itcd43cpff ( rtS , rtB . kcvke53w1x [ 0 ]
, rtB . gjp3g4kh1s , rtB . mapzoy3ans , & rtB . ctjk4b4i5h , & rtDW .
ctjk4b4i5h ) ; cgvj5xjnjk ( rtS , rtB . kcvke53w1x [ 1 ] , rtB . gjp3g4kh1s ,
rtB . mapzoy3ans , & rtB . j0qka4bwqf , & rtDW . j0qka4bwqf ) ; rtB .
hkuiqt3qt1 = ( rtB . ctjk4b4i5h . guqrn12esp || rtB . j0qka4bwqf . l5pxivnpxv
) ; rtB . gnjlrqal4d = ( rtB . al5nty1gfn && rtB . hkuiqt3qt1 ) ; rtB .
d23vxlyygc = rtB . gnjlrqal4d ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
jori1dlivw = rtDW . invgjx5x0c ; } if ( rtB . d23vxlyygc ) { rtB . hmmpk1cj00
= rtB . pzjojlj2y3 ; } else { rtB . hmmpk1cj00 = rtB . jori1dlivw ; } maxV =
rtP . u_PulseDuration_aoizfa5k3a - 2.2204460492503131E-16 ; rtB . inlw3fpu5z
= rtB . hmmpk1cj00 + maxV ; rtB . kzvwlld5zx = ( rtB . inlw3fpu5z > rtB .
pzjojlj2y3 ) ; rtB . me0q4vq4ka = ( rtB . e3wvgrwqct || rtB . kzvwlld5zx ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . eiotbyytex = rtDW . jwthvsveef ;
rtB . n2hqgsfxi4 = ! rtB . eiotbyytex ; rtB . j3h4j5wel3 = rtDW . k3ksd4k1ib
; rtB . gonnafk1bt = ( rtB . j3h4j5wel3 && rtB . fvdmdl211j ) ; rtB .
lu21q4ctfu = rtB . gonnafk1bt ; } rtB . d1vv3wywxp = ( rtB . me0q4vq4ka &&
rtB . n2hqgsfxi4 ) ; rtB . garpnlodba = ssGetT ( rtS ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { fx2esogudm ( rtS , rtB . hkuiqt3qt1 , rtB . garpnlodba , &
rtB . gk4015knwc , & rtDW . gk4015knwc , & rtPrevZCX . gk4015knwc ) ; rtB .
csngg5srlz = rtB . gk4015knwc . jhmp0c0tsm + rtP . Constant_Value_dpr3t2xkdk
; } rtB . k1kam03l1g = ( rtB . csngg5srlz > rtB . garpnlodba ) ; rtB .
nel1anluxo = rtB . asa103d02z * rtB . asa103d02z ; rtB . bkcamitnxz = rtB .
ddix2mhi1g * rtB . ddix2mhi1g ; rtB . mmejdn5l2e = rtB . nel1anluxo + rtB .
bkcamitnxz ; rtB . mavum5qnpk = muDoubleScalarSqrt ( rtB . mmejdn5l2e ) ; if
( rtP . Constant_Value_fqi4lxzum2 > rtP . u_Threshold_kbjwqrn1av ) { rtB .
ohbqr0wjk2 = ( rtB . mavum5qnpk < rtP . Constant3_Value_er3pwszvno ) ; rtB .
o41ivegqyu = rtB . ohbqr0wjk2 ; } else { rtB . i1fmwzly02 = ( rtB .
mavum5qnpk > rtP . Constant3_Value_er3pwszvno ) ; rtB . o41ivegqyu = rtB .
i1fmwzly02 ; } rtB . pkhoyj22xd = rtB . asa103d02z * rtP . u_Value_n1cq0aprfw
; rtB . invex2t04q = rtB . ddix2mhi1g * rtP . u_Value_nmgw4a1j2t ; rtB .
k3wqziu412 = rtB . pkhoyj22xd - rtB . invex2t04q ; rtB . irirbx2h4f = rtB .
fizn4ck4n1 * rtP . u_Value_n1cq0aprfw ; rtB . kdlp3upbyx = rtB . ndiffna1hx *
rtP . u_Value_el1pa43mzw ; rtB . ejnhyfppq0 = rtB . irirbx2h4f - rtB .
kdlp3upbyx ; rtB . d0tnhklw3n = rtB . k3wqziu412 + rtB . ejnhyfppq0 ; rtB .
ib5xpyzwc3 = rtB . hlmtyy2tbw + rtB . d0tnhklw3n ; rtB . l4j5jfiqq5 = rtB .
ib5xpyzwc3 * rtP . u7_Value_eku41c2gbs ; rtB . nifdwqmwvq = rtB . l4j5jfiqq5
* rtB . l4j5jfiqq5 ; rtB . defel2jcb4 = rtB . ddix2mhi1g * rtP .
u_Value_n1cq0aprfw ; rtB . kvoccclcj5 = rtB . asa103d02z * rtP .
u_Value_nmgw4a1j2t ; rtB . hyoqxpzhd4 = rtB . defel2jcb4 + rtB . kvoccclcj5 ;
rtB . ij4fjcahho = rtB . ndiffna1hx * rtP . u_Value_n1cq0aprfw ; rtB .
pglg02yg1o = rtB . fizn4ck4n1 * rtP . u_Value_el1pa43mzw ; rtB . ltgsz5q0ft =
rtB . ij4fjcahho + rtB . pglg02yg1o ; rtB . i0dvn3jywk = rtB . hyoqxpzhd4 +
rtB . ltgsz5q0ft ; rtB . bhor5f5o5u = rtB . krvqv214qo + rtB . i0dvn3jywk ;
rtB . ncq3bjflpl = rtB . bhor5f5o5u * rtP . u7_Value_eku41c2gbs ; rtB .
nqocbjid3k = rtB . ncq3bjflpl * rtB . ncq3bjflpl ; rtB . epwmbwduk3 = rtB .
nifdwqmwvq + rtB . nqocbjid3k ; rtB . g2eh02n3vn = muDoubleScalarSqrt ( rtB .
epwmbwduk3 ) ; if ( rtP . Constant_Value_aocvditrvj > rtP .
u_Threshold_hrlwzqsdky ) { rtB . hht1zx0rzn = ( rtB . g2eh02n3vn < rtP .
Constant3_Value_jfq3amfxmc ) ; rtB . kbzyuep4ch = rtB . hht1zx0rzn ; } else {
rtB . cm3wveofjy = ( rtB . g2eh02n3vn > rtP . Constant3_Value_jfq3amfxmc ) ;
rtB . kbzyuep4ch = rtB . cm3wveofjy ; } rtB . fua5sf2ktk = ( rtB . kbzyuep4ch
&& rtB . p4ezegbo2p ) ; if ( rtB . aad3hbdttm ) { rtB . cjh0zlbcbc = rtB .
fua5sf2ktk ; } else { rtB . g3hkxccpal = ( rtB . fua5sf2ktk && rtB .
lrqd0f4cid ) ; rtB . cjh0zlbcbc = rtB . g3hkxccpal ; } rtB . defuyxrwhh = (
rtB . cjh0zlbcbc || ( rtP . Constant13_Value_l3g21zjgbe != 0.0 ) || ( rtP .
Constant13_Value_l3g21zjgbe != 0.0 ) || ( rtP . Constant13_Value_l3g21zjgbe
!= 0.0 ) ) ; rtB . o1nucjxv2u = rtB . hlmtyy2tbw * rtB . hlmtyy2tbw ; rtB .
juomj1wmhu = rtB . krvqv214qo * rtB . krvqv214qo ; rtB . phsj1fp0jx = rtB .
o1nucjxv2u + rtB . juomj1wmhu ; rtB . jzuw1am2bm = muDoubleScalarSqrt ( rtB .
phsj1fp0jx ) ; if ( rtP . Constant_Value_fqi4lxzum2 > rtP .
u_Threshold_dtl1hsh3wd ) { rtB . dcupvr3mhr = ( rtB . jzuw1am2bm < rtP .
Constant3_Value_er3pwszvno ) ; rtB . i0t4scc0ka = rtB . dcupvr3mhr ; } else {
rtB . lpkldzng3w = ( rtB . jzuw1am2bm > rtP . Constant3_Value_er3pwszvno ) ;
rtB . i0t4scc0ka = rtB . lpkldzng3w ; } rtB . odmzrzjbtz = rtB . fizn4ck4n1 *
rtB . fizn4ck4n1 ; rtB . jzbdrshs1w = rtB . ndiffna1hx * rtB . ndiffna1hx ;
rtB . npxidd11pz = rtB . odmzrzjbtz + rtB . jzbdrshs1w ; rtB . gii2gbyhob =
muDoubleScalarSqrt ( rtB . npxidd11pz ) ; if ( rtP .
Constant_Value_fqi4lxzum2 > rtP . u_Threshold_h333a2sf1a ) { rtB . ceclnmvcnn
= ( rtB . gii2gbyhob < rtP . Constant3_Value_er3pwszvno ) ; rtB . i1zamxkksg
= rtB . ceclnmvcnn ; } else { rtB . jrsybsemxq = ( rtB . gii2gbyhob > rtP .
Constant3_Value_er3pwszvno ) ; rtB . i1zamxkksg = rtB . jrsybsemxq ; } rtB .
grvkrr42w1 = ( rtB . o41ivegqyu && rtB . daekldgh4z ) ; rtB . kejgziktys = (
rtB . defuyxrwhh && rtB . d5arzfdg32 ) ; rtB . j2v2u4sx4r = ( rtB .
grvkrr42w1 && rtB . kejgziktys ) ; rtB . am3g4xmxuu = ( rtB . i0t4scc0ka &&
rtB . bdiuolr2mz ) ; rtB . fwvvtrewmh = ( rtB . am3g4xmxuu && rtB .
kejgziktys ) ; if ( rtB . by52txfadq ) { rtB . n2n40ci54z = rtB . j2v2u4sx4r
; } else { rtB . ht3p4tq35f = ( rtB . j2v2u4sx4r && rtB . nxrqwkox3h ) ; rtB
. n2n40ci54z = rtB . ht3p4tq35f ; } rtB . execwg4z1g = ( rtB . i1zamxkksg &&
rtB . ae23whgkhf ) ; rtB . pvgzclugh5 = ( rtB . execwg4z1g && rtB .
kejgziktys ) ; if ( rtB . by52txfadq ) { rtB . jzy5pkzfwf = rtB . fwvvtrewmh
; rtB . nnjlecshqw = rtB . pvgzclugh5 ; } else { rtB . k1ggnbutwo = ( rtB .
fwvvtrewmh && rtB . cub32xc11t ) ; rtB . jzy5pkzfwf = rtB . k1ggnbutwo ; rtB
. om0q0xtuog = ( rtB . pvgzclugh5 && rtB . ippcj2z3rr ) ; rtB . nnjlecshqw =
rtB . om0q0xtuog ; } rtB . ifvx3udmfy = ( rtB . jzy5pkzfwf || rtB .
n2n40ci54z || rtB . nnjlecshqw ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB
. p3rofqijh1 = rtDW . gbzb4i3qdq ; rtB . e3bhdplay2 = ! rtB . p3rofqijh1 ;
rtB . mu1wceftno = rtDW . gkn55q342j ; rtB . pt0wgeq4nz = ! rtB . mu1wceftno
; } rtB . gc1zjtemxt = ( rtB . ifvx3udmfy && rtB . e3bhdplay2 ) ; rtB .
esuys12pc3 = ( rtB . cjh0zlbcbc && rtB . pt0wgeq4nz ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . agxskcblgy = rtDW . njpxzlmy40 ; rtB . edthhprxe4 =
rtDW . f4s0lo0uob ; rtB . iolqx2jiau = ! rtB . edthhprxe4 ; } if ( rtP .
Constant4_Value_nigdcderyn > rtP . u_Threshold_asvcv3d2gh ) { rtB .
kyr42isgus = ( rtB . jzy5pkzfwf && rtB . iolqx2jiau ) ; rtB . h0dwhcvoer =
rtB . kyr42isgus ; } else { rtB . h0dwhcvoer = rtB . jzy5pkzfwf ; } rtB .
ep03e43mx4 = rtB . a0it33vpv3 ; rtB . out41twgho = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . cvfp24mc3f , rtB . ep03e43mx4 , rtB . out41twgho ,
rtP . T1_delay_ezglkjcl5g , & rtB . lkrts5ohpw , & rtDW . lkrts5ohpw , & rtP
. lkrts5ohpw , & rtPrevZCX . lkrts5ohpw ) ; bhc3qgnuic ( rtS , rtB .
jlggomoiae , rtB . ep03e43mx4 , rtB . out41twgho , rtP . T1_delay_ezglkjcl5g
, & rtB . hh2cmxftpw , & rtDW . hh2cmxftpw , & rtP . hh2cmxftpw , & rtPrevZCX
. hh2cmxftpw ) ; rtB . mrzmikbiqm = ( rtB . lkrts5ohpw . j1vu52vt5l || rtB .
hh2cmxftpw . mjqylog0rb ) ; rtB . jdhkyuxarh = ( rtB . h0dwhcvoer && rtB .
mrzmikbiqm ) ; rtB . ffvlkkq3jj = ( rtB . agxskcblgy || rtB . jdhkyuxarh ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . aagbazfj4d = rtDW . mls4swjwmm ;
} rtB . njs2iyfhl4 = ! rtB . jzy5pkzfwf ; rtB . oiiobqckcv = ( rtB .
aagbazfj4d && rtB . njs2iyfhl4 ) ; rtB . emo3yl1ywb = rtB . oiiobqckcv ; rtB
. jn1xllujne = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . i3t3iwhfmr , rtB .
emo3yl1ywb , rtB . jn1xllujne , rtP . T2_delay_ekqgaoysqs , & rtB .
me023aolbi , & rtDW . me023aolbi , & rtP . me023aolbi , & rtPrevZCX .
me023aolbi ) ; bhc3qgnuic ( rtS , rtB . ohceznfo4g , rtB . emo3yl1ywb , rtB .
jn1xllujne , rtP . T2_delay_ekqgaoysqs , & rtB . dexvjkg0hj , & rtDW .
dexvjkg0hj , & rtP . dexvjkg0hj , & rtPrevZCX . dexvjkg0hj ) ; rtB .
kb0yzsnitg = ( rtB . me023aolbi . j1vu52vt5l || rtB . dexvjkg0hj . mjqylog0rb
) ; rtB . dsiqaf1g12 = ! rtB . kb0yzsnitg ; rtB . nvrbl35obj = ( rtB .
ffvlkkq3jj && rtB . dsiqaf1g12 ) ; rtB . e44pd50qd3 = ( rtB . plghrktmu3 &&
rtB . nvrbl35obj ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . hlyc01x35e
= rtDW . kguu1igm24 ; rtB . kxsruitwaf = rtDW . l4mjhh4eud ; rtB . pyfdvhdxmc
= ! rtB . kxsruitwaf ; } if ( rtP . Constant4_Value_nigdcderyn > rtP .
u_Threshold_i3ukgonlps ) { rtB . iebbzotwt1 = ( rtB . n2n40ci54z && rtB .
pyfdvhdxmc ) ; rtB . kitwvmmmoc = rtB . iebbzotwt1 ; } else { rtB .
kitwvmmmoc = rtB . n2n40ci54z ; } rtB . h4crgpnf4d = rtB . gr4vjiot5p ; rtB .
fcsumbkzdc = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . cjrpmegtn4 , rtB .
h4crgpnf4d , rtB . fcsumbkzdc , rtP . T1_delay_hlwj1epbw1 , & rtB .
purgdi1jfk , & rtDW . purgdi1jfk , & rtP . purgdi1jfk , & rtPrevZCX .
purgdi1jfk ) ; bhc3qgnuic ( rtS , rtB . lkuadplgq1 , rtB . h4crgpnf4d , rtB .
fcsumbkzdc , rtP . T1_delay_hlwj1epbw1 , & rtB . g3bxbyypfb , & rtDW .
g3bxbyypfb , & rtP . g3bxbyypfb , & rtPrevZCX . g3bxbyypfb ) ; rtB .
ered4l2jo5 = ( rtB . purgdi1jfk . j1vu52vt5l || rtB . g3bxbyypfb . mjqylog0rb
) ; rtB . o4gbr3eu3d = ( rtB . kitwvmmmoc && rtB . ered4l2jo5 ) ; rtB .
j5sat54yps = ( rtB . hlyc01x35e || rtB . o4gbr3eu3d ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . i33ogyo4fz = rtDW . nqy0vnt1sc ; } rtB . k4qjuhb1fl =
! rtB . n2n40ci54z ; rtB . nn1vnyl511 = ( rtB . i33ogyo4fz && rtB .
k4qjuhb1fl ) ; rtB . dxu2xmkkzz = rtB . nn1vnyl511 ; rtB . csgv5bzefl =
ssGetT ( rtS ) ; on20slldft ( rtS , rtB . c5ci2jl0on , rtB . dxu2xmkkzz , rtB
. csgv5bzefl , rtP . T2_delay_fcyodig2e0 , & rtB . e4qyv4smfd , & rtDW .
e4qyv4smfd , & rtP . e4qyv4smfd , & rtPrevZCX . e4qyv4smfd ) ; bhc3qgnuic (
rtS , rtB . l3ycctbi23 , rtB . dxu2xmkkzz , rtB . csgv5bzefl , rtP .
T2_delay_fcyodig2e0 , & rtB . bs5m40ynfy , & rtDW . bs5m40ynfy , & rtP .
bs5m40ynfy , & rtPrevZCX . bs5m40ynfy ) ; rtB . al3vbfgmcr = ( rtB .
e4qyv4smfd . j1vu52vt5l || rtB . bs5m40ynfy . mjqylog0rb ) ; rtB . bf0bdnj20h
= ! rtB . al3vbfgmcr ; rtB . pb55w0r34z = ( rtB . j5sat54yps && rtB .
bf0bdnj20h ) ; rtB . f21udqugjw = ( rtB . j4bpge551b && rtB . pb55w0r34z ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . lhpzy0maxl = rtDW . hhnsho4bie ;
rtB . dg5nj2uh0s = rtDW . goafipr0ei ; rtB . iucqa4ja45 = ! rtB . dg5nj2uh0s
; } if ( rtP . Constant4_Value_nigdcderyn > rtP . u_Threshold_af2qwkwka1 ) {
rtB . i0p15ne02e = ( rtB . nnjlecshqw && rtB . iucqa4ja45 ) ; rtB .
bbnnilnic0 = rtB . i0p15ne02e ; } else { rtB . bbnnilnic0 = rtB . nnjlecshqw
; } rtB . pg1g4fqfyy = rtB . lz0cksews5 ; rtB . lw3k3tkcp3 = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . j0ubvz5m4h , rtB . pg1g4fqfyy , rtB . lw3k3tkcp3 ,
rtP . T1_delay_n04adozecf , & rtB . lu12zasvyl , & rtDW . lu12zasvyl , & rtP
. lu12zasvyl , & rtPrevZCX . lu12zasvyl ) ; bhc3qgnuic ( rtS , rtB .
gz1ov3colo , rtB . pg1g4fqfyy , rtB . lw3k3tkcp3 , rtP . T1_delay_n04adozecf
, & rtB . ngrhemover , & rtDW . ngrhemover , & rtP . ngrhemover , & rtPrevZCX
. ngrhemover ) ; rtB . heas0fa3xo = ( rtB . lu12zasvyl . j1vu52vt5l || rtB .
ngrhemover . mjqylog0rb ) ; rtB . fozexy52e0 = ( rtB . bbnnilnic0 && rtB .
heas0fa3xo ) ; rtB . il20wv3eht = ( rtB . lhpzy0maxl || rtB . fozexy52e0 ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . avywyxskzq = rtDW . jbzxllbqx1 ;
} rtB . kneu1c0yzf = ! rtB . nnjlecshqw ; rtB . aq4ez3aqdt = ( rtB .
avywyxskzq && rtB . kneu1c0yzf ) ; rtB . gqldyewno1 = rtB . aq4ez3aqdt ; rtB
. mc4k3s2eiv = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . aiibyohiyh , rtB .
gqldyewno1 , rtB . mc4k3s2eiv , rtP . T2_delay_ec5i0zygdl , & rtB .
myp2dcpbvg , & rtDW . myp2dcpbvg , & rtP . myp2dcpbvg , & rtPrevZCX .
myp2dcpbvg ) ; bhc3qgnuic ( rtS , rtB . ejunrbhwth , rtB . gqldyewno1 , rtB .
mc4k3s2eiv , rtP . T2_delay_ec5i0zygdl , & rtB . onemmt3xbs , & rtDW .
onemmt3xbs , & rtP . onemmt3xbs , & rtPrevZCX . onemmt3xbs ) ; rtB .
jiihe3yeev = ( rtB . myp2dcpbvg . j1vu52vt5l || rtB . onemmt3xbs . mjqylog0rb
) ; rtB . lsjdizl3ad = ! rtB . jiihe3yeev ; rtB . djcrc2xxmk = ( rtB .
il20wv3eht && rtB . lsjdizl3ad ) ; rtB . bsqhg0vgl1 = ( rtB . behnpma0jr &&
rtB . djcrc2xxmk ) ; rtB . lzkbyuc0np = ( rtB . e44pd50qd3 || rtB .
f21udqugjw || rtB . bsqhg0vgl1 ) ; if ( rtB . ivr4rwq42s ) { rtB . ds4ks50los
= ( rtB . e44pd50qd3 || rtB . f21udqugjw || rtB . bsqhg0vgl1 ) ; rtB .
c2mu2awuzk = rtB . ds4ks50los ; } else { rtB . m0jq4idjvb = ( rtB .
e44pd50qd3 && rtB . f21udqugjw && rtB . bsqhg0vgl1 ) ; rtB . gmqcvx3ywb = ( (
rtP . Constant2_Value_idz5g0e2ns != 0.0 ) && rtB . m0jq4idjvb ) ; rtB .
jkcjvgmalg = ! rtB . bsqhg0vgl1 ; rtB . fdpiweg2ss = ( rtB . e44pd50qd3 &&
rtB . f21udqugjw && rtB . jkcjvgmalg ) ; rtB . oe3t3zmicq = ! rtB .
f21udqugjw ; rtB . ejj4ah1y4w = ( rtB . e44pd50qd3 && rtB . oe3t3zmicq && rtB
. bsqhg0vgl1 ) ; rtB . obi0jactbv = ! rtB . e44pd50qd3 ; rtB . cpxet3vyej = (
rtB . obi0jactbv && rtB . f21udqugjw && rtB . bsqhg0vgl1 ) ; rtB . ebu5vb3tqd
= ( rtB . cpxet3vyej || rtB . ejj4ah1y4w || rtB . fdpiweg2ss ) ; rtB .
hxuj4rjm5q = ( ( rtP . Constant1_Value_h2l3yfta2m != 0.0 ) && rtB .
ebu5vb3tqd ) ; rtB . ouk4hsxjoc = ! rtB . f21udqugjw ; rtB . asby30mdsb = !
rtB . e44pd50qd3 ; rtB . gi0i50zkxy = ( rtB . asby30mdsb && rtB . ouk4hsxjoc
&& rtB . bsqhg0vgl1 ) ; rtB . mvzjfkmelg = ! rtB . bsqhg0vgl1 ; rtB .
gzmyb5otl1 = ! rtB . e44pd50qd3 ; rtB . fhb401ohhh = ( rtB . gzmyb5otl1 &&
rtB . f21udqugjw && rtB . mvzjfkmelg ) ; rtB . ozidcmlmz1 = ! rtB .
bsqhg0vgl1 ; rtB . f4vyo505pi = ! rtB . f21udqugjw ; rtB . d1wmosylh3 = ( rtB
. e44pd50qd3 && rtB . f4vyo505pi && rtB . ozidcmlmz1 ) ; rtB . kkacfnczfm = (
rtB . d1wmosylh3 || rtB . fhb401ohhh || rtB . gi0i50zkxy ) ; rtB . bd141doxdb
= ( ( rtP . Constant3_Value_donksybvgr != 0.0 ) && rtB . kkacfnczfm ) ; rtB .
i0ctpvp54k = ( rtB . bd141doxdb || rtB . hxuj4rjm5q || rtB . gmqcvx3ywb ) ;
rtB . c2mu2awuzk = rtB . i0ctpvp54k ; } rtB . bxmhdsboyr = ( rtB . c2mu2awuzk
&& rtB . bsqhg0vgl1 ) ; rtB . pyl5tuglyt = ( rtB . bxmhdsboyr || ( rtP .
Constant11_Value_fj52spnxjl != 0.0 ) ) ; rtB . lsdvjz23kc = rtB . pyl5tuglyt
; rtB . p40r5lgtbr = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . iitw2p4j23 ,
rtB . lsdvjz23kc , rtB . p40r5lgtbr , rtP . T4_delay_joozymdxow , & rtB .
nci0imhwrv , & rtDW . nci0imhwrv , & rtP . nci0imhwrv , & rtPrevZCX .
nci0imhwrv ) ; bhc3qgnuic ( rtS , rtB . jur04llf4l , rtB . lsdvjz23kc , rtB .
p40r5lgtbr , rtP . T4_delay_joozymdxow , & rtB . dlrk53odir , & rtDW .
dlrk53odir , & rtP . dlrk53odir , & rtPrevZCX . dlrk53odir ) ; rtB .
co3yzgrp2q = rtB . khtc1wrv1m ; rtB . lmry0grszx = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . oplr5q4s21 , rtB . co3yzgrp2q , rtB . lmry0grszx ,
rtP . T7_delay_bydvi40r5i , & rtB . mpydob1efk , & rtDW . mpydob1efk , & rtP
. mpydob1efk , & rtPrevZCX . mpydob1efk ) ; bhc3qgnuic ( rtS , rtB .
osnitzd20w , rtB . co3yzgrp2q , rtB . lmry0grszx , rtP . T7_delay_bydvi40r5i
, & rtB . dlwrrwcsx5 , & rtDW . dlwrrwcsx5 , & rtP . dlwrrwcsx5 , & rtPrevZCX
. dlwrrwcsx5 ) ; rtB . d4d35rrzqa = ( rtB . mpydob1efk . j1vu52vt5l || rtB .
dlwrrwcsx5 . mjqylog0rb ) ; rtB . jwkufjlqk0 = rtB . pyl5tuglyt ; rtB .
crcb0tkuif = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . niah0e0t52 , rtB .
jwkufjlqk0 , rtB . crcb0tkuif , rtP . T2_delay_hhm4kohiwo , & rtB .
lfrovwpki3 , & rtDW . lfrovwpki3 , & rtP . lfrovwpki3 , & rtPrevZCX .
lfrovwpki3 ) ; bhc3qgnuic ( rtS , rtB . ahjyepadcb , rtB . jwkufjlqk0 , rtB .
crcb0tkuif , rtP . T2_delay_hhm4kohiwo , & rtB . h145orsdu5 , & rtDW .
h145orsdu5 , & rtP . h145orsdu5 , & rtPrevZCX . h145orsdu5 ) ; if ( rtB .
dugoe1l42l ) { rtB . dymstohium = rtB . pyl5tuglyt ; } else { if ( rtB .
d4d35rrzqa ) { rtB . egxfey3mnw = ( rtB . nci0imhwrv . j1vu52vt5l || rtB .
dlrk53odir . mjqylog0rb ) ; rtB . mf4wer3dlq = rtB . egxfey3mnw ; } else {
rtB . ic5jzxleip = ( rtB . lfrovwpki3 . j1vu52vt5l || rtB . h145orsdu5 .
mjqylog0rb ) ; rtB . mf4wer3dlq = rtB . ic5jzxleip ; } rtB . dymstohium = rtB
. mf4wer3dlq ; } rtB . eulqidvnch = ( rtB . nnjlecshqw && rtB . dymstohium &&
rtB . c2mu2awuzk ) ; rtB . itdurdpn2d = ( rtB . c2mu2awuzk && rtB .
e44pd50qd3 ) ; rtB . ldjxbs5btx = ( rtB . c2mu2awuzk && rtB . f21udqugjw ) ;
rtB . jonhjwizjb = ( rtB . itdurdpn2d || ( rtP . Constant11_Value_fj52spnxjl
!= 0.0 ) ) ; rtB . hov2mnbmrz = ( rtB . ldjxbs5btx || ( rtP .
Constant11_Value_fj52spnxjl != 0.0 ) ) ; rtB . jp4leo0uan = rtB . jonhjwizjb
; rtB . dkksbe1yls = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . p40dytx3zu ,
rtB . jp4leo0uan , rtB . dkksbe1yls , rtP . T6_delay_o0mmlr4paz , & rtB .
lbq0bgvgcl , & rtDW . lbq0bgvgcl , & rtP . lbq0bgvgcl , & rtPrevZCX .
lbq0bgvgcl ) ; bhc3qgnuic ( rtS , rtB . k1emjxsvef , rtB . jp4leo0uan , rtB .
dkksbe1yls , rtP . T6_delay_o0mmlr4paz , & rtB . cq2uqua4yo , & rtDW .
cq2uqua4yo , & rtP . cq2uqua4yo , & rtPrevZCX . cq2uqua4yo ) ; rtB .
cvuuqen5ik = rtB . jonhjwizjb ; rtB . haem5is2sh = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . hubguf2n1v , rtB . cvuuqen5ik , rtB . haem5is2sh ,
rtP . T3_delay_n4jiaagfoa , & rtB . inwm331xkg , & rtDW . inwm331xkg , & rtP
. inwm331xkg , & rtPrevZCX . inwm331xkg ) ; bhc3qgnuic ( rtS , rtB .
nixfhlnmmc , rtB . cvuuqen5ik , rtB . haem5is2sh , rtP . T3_delay_n4jiaagfoa
, & rtB . obeddxl4xp , & rtDW . obeddxl4xp , & rtP . obeddxl4xp , & rtPrevZCX
. obeddxl4xp ) ; if ( rtB . dugoe1l42l ) { rtB . nvimg3kb5u = rtB .
jonhjwizjb ; } else { if ( rtB . d4d35rrzqa ) { rtB . nn3oh2ymrm = ( rtB .
lbq0bgvgcl . j1vu52vt5l || rtB . cq2uqua4yo . mjqylog0rb ) ; rtB . cyfyh3do4b
= rtB . nn3oh2ymrm ; } else { rtB . exu3tco4wq = ( rtB . inwm331xkg .
j1vu52vt5l || rtB . obeddxl4xp . mjqylog0rb ) ; rtB . cyfyh3do4b = rtB .
exu3tco4wq ; } rtB . nvimg3kb5u = rtB . cyfyh3do4b ; } rtB . ixkx5kqkpr = (
rtB . jzy5pkzfwf && rtB . nvimg3kb5u && rtB . c2mu2awuzk ) ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . bypn2gveor = rtDW . hdv5m3k0vy ; rtB
. jddnqvgsp2 = ! rtB . bypn2gveor ; rtB . d4fdjujtbn = rtDW . lz1cv0do2t ;
rtB . lp4lyyes1m = ! rtB . d4fdjujtbn ; } rtB . iac5cqeqlz = ( rtB .
ixkx5kqkpr && rtB . jddnqvgsp2 ) ; rtB . ffc3gguln3 = ssGetT ( rtS ) ; rtB .
dtm0witzm5 = rtB . ixkx5kqkpr ; rtB . n110gjtipb = rtB . dtm0witzm5 ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . ecat25s11f = rtDW . cgbiv0cyqn ; }
itcd43cpff ( rtS , rtB . mgblsyen2k [ 0 ] , rtB . n110gjtipb , rtB .
ecat25s11f , & rtB . oo1qtpn5j5 , & rtDW . oo1qtpn5j5 ) ; cgvj5xjnjk ( rtS ,
rtB . mgblsyen2k [ 1 ] , rtB . n110gjtipb , rtB . ecat25s11f , & rtB .
lrmch0dvn3 , & rtDW . lrmch0dvn3 ) ; rtB . dloffoxawu = ( rtB . oo1qtpn5j5 .
guqrn12esp || rtB . lrmch0dvn3 . l5pxivnpxv ) ; rtB . ix4onscbx2 = ( rtB .
lp4lyyes1m && rtB . dloffoxawu ) ; rtB . drlxbmhn3i = rtB . ix4onscbx2 ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . ijil4xgjne = rtDW . kvmhfywt50 ; } if
( rtB . drlxbmhn3i ) { rtB . okjxoxwhum = rtB . ffc3gguln3 ; } else { rtB .
okjxoxwhum = rtB . ijil4xgjne ; } maxV = rtP . u_PulseDuration_gh5ecgijhx -
2.2204460492503131E-16 ; rtB . kadxu1sowu = rtB . okjxoxwhum + maxV ; rtB .
lmwmcpsvhm = ( rtB . kadxu1sowu > rtB . ffc3gguln3 ) ; rtB . dmvdzgif22 = (
rtB . iac5cqeqlz || rtB . lmwmcpsvhm ) ; rtB . ewttaxisyn = rtB . hov2mnbmrz
; rtB . ep23rzg25d = ssGetT ( rtS ) ; on20slldft ( rtS , rtB . c4svzwiub2 ,
rtB . ewttaxisyn , rtB . ep23rzg25d , rtP . T5_delay_nlou2kyiik , & rtB .
crzn5z4zte , & rtDW . crzn5z4zte , & rtP . crzn5z4zte , & rtPrevZCX .
crzn5z4zte ) ; bhc3qgnuic ( rtS , rtB . ho1jpoo2z0 , rtB . ewttaxisyn , rtB .
ep23rzg25d , rtP . T5_delay_nlou2kyiik , & rtB . llpwcowx1f , & rtDW .
llpwcowx1f , & rtP . llpwcowx1f , & rtPrevZCX . llpwcowx1f ) ; rtB .
k1qabotiay = rtB . hov2mnbmrz ; rtB . p3pfju02b3 = ssGetT ( rtS ) ;
on20slldft ( rtS , rtB . jfvb44flq5 , rtB . k1qabotiay , rtB . p3pfju02b3 ,
rtP . T1_delay_bm02130jwf , & rtB . hytx1hl5s5 , & rtDW . hytx1hl5s5 , & rtP
. hytx1hl5s5 , & rtPrevZCX . hytx1hl5s5 ) ; bhc3qgnuic ( rtS , rtB .
o1rgmf5rqs , rtB . k1qabotiay , rtB . p3pfju02b3 , rtP . T1_delay_bm02130jwf
, & rtB . mgkfvqxti2 , & rtDW . mgkfvqxti2 , & rtP . mgkfvqxti2 , & rtPrevZCX
. mgkfvqxti2 ) ; if ( rtB . dugoe1l42l ) { rtB . cpqhydmgqp = rtB .
hov2mnbmrz ; } else { if ( rtB . d4d35rrzqa ) { rtB . bwwpaq5j3c = ( rtB .
crzn5z4zte . j1vu52vt5l || rtB . llpwcowx1f . mjqylog0rb ) ; rtB . nck2bnt2tz
= rtB . bwwpaq5j3c ; } else { rtB . bcxsjpophv = ( rtB . hytx1hl5s5 .
j1vu52vt5l || rtB . mgkfvqxti2 . mjqylog0rb ) ; rtB . nck2bnt2tz = rtB .
bcxsjpophv ; } rtB . cpqhydmgqp = rtB . nck2bnt2tz ; } rtB . pwh01jekkk = (
rtB . n2n40ci54z && rtB . cpqhydmgqp && rtB . c2mu2awuzk ) ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . m14qyfazpd = rtDW . lsieos4skt ; rtB
. af1xlozbad = ! rtB . m14qyfazpd ; rtB . dvywkamczq = rtDW . edyvvi3ty4 ;
rtB . pmw3y3ceun = ! rtB . dvywkamczq ; } rtB . odr2chj5ox = ( rtB .
pwh01jekkk && rtB . af1xlozbad ) ; rtB . n0z0bcj1pp = ssGetT ( rtS ) ; rtB .
k31qxbpmlq = rtB . pwh01jekkk ; rtB . bfzarqag03 = rtB . k31qxbpmlq ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . ad5obf3chl = rtDW . hvm2hswn02 ; }
itcd43cpff ( rtS , rtB . emgyzaybxu [ 0 ] , rtB . bfzarqag03 , rtB .
ad5obf3chl , & rtB . kuwwc0aum4 , & rtDW . kuwwc0aum4 ) ; cgvj5xjnjk ( rtS ,
rtB . emgyzaybxu [ 1 ] , rtB . bfzarqag03 , rtB . ad5obf3chl , & rtB .
c2mshkhisd , & rtDW . c2mshkhisd ) ; rtB . lhqi3wbsnc = ( rtB . kuwwc0aum4 .
guqrn12esp || rtB . c2mshkhisd . l5pxivnpxv ) ; rtB . d3zx1a0zf0 = ( rtB .
pmw3y3ceun && rtB . lhqi3wbsnc ) ; rtB . g3myqdi0g1 = rtB . d3zx1a0zf0 ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . lgdo5wixno = rtDW . ppskp5kzs0 ; } if
( rtB . g3myqdi0g1 ) { rtB . pn5gpdyogd = rtB . n0z0bcj1pp ; } else { rtB .
pn5gpdyogd = rtB . lgdo5wixno ; } maxV = rtP . u_PulseDuration_ar130r3lup -
2.2204460492503131E-16 ; rtB . koh0dkpqvz = rtB . pn5gpdyogd + maxV ; rtB .
ligdly522h = ( rtB . koh0dkpqvz > rtB . n0z0bcj1pp ) ; rtB . lxhr2aaydm = (
rtB . odr2chj5ox || rtB . ligdly522h ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) )
{ rtB . ajtzofjn3n = rtDW . jurv4x4dxh ; rtB . gchd1e4xmi = ! rtB .
ajtzofjn3n ; rtB . a1anx5d5rd = rtDW . n3llciapkl ; rtB . hqfmhvxzzo = ! rtB
. a1anx5d5rd ; } rtB . hg4lyhggfx = ( rtB . eulqidvnch && rtB . gchd1e4xmi )
; rtB . ojwvxuse43 = ssGetT ( rtS ) ; rtB . gtuzjvnthg = rtB . eulqidvnch ;
rtB . k5dchzskoq = rtB . gtuzjvnthg ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) {
rtB . lruif5ggiz = rtDW . ld1xzt33jz ; } itcd43cpff ( rtS , rtB . khzixdgyr4
[ 0 ] , rtB . k5dchzskoq , rtB . lruif5ggiz , & rtB . hwrjddg40d , & rtDW .
hwrjddg40d ) ; cgvj5xjnjk ( rtS , rtB . khzixdgyr4 [ 1 ] , rtB . k5dchzskoq ,
rtB . lruif5ggiz , & rtB . jxw41swge5 , & rtDW . jxw41swge5 ) ; rtB .
gvsvwxpjbi = ( rtB . hwrjddg40d . guqrn12esp || rtB . jxw41swge5 . l5pxivnpxv
) ; rtB . gqonlbn1sg = ( rtB . hqfmhvxzzo && rtB . gvsvwxpjbi ) ; rtB .
piytd1vz2l = rtB . gqonlbn1sg ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
duntttrxji = rtDW . fsgvohzvsa ; } if ( rtB . piytd1vz2l ) { rtB . ntrbre3nrs
= rtB . ojwvxuse43 ; } else { rtB . ntrbre3nrs = rtB . duntttrxji ; } maxV =
rtP . u_PulseDuration_antsnvmygw - 2.2204460492503131E-16 ; rtB . ke4a2453vp
= rtB . ntrbre3nrs + maxV ; rtB . jl3mlxyvfa = ( rtB . ke4a2453vp > rtB .
ojwvxuse43 ) ; rtB . lxnxndvv2r = ( rtB . hg4lyhggfx || rtB . jl3mlxyvfa ) ;
rtB . cdjlsurquj = ( rtB . dmvdzgif22 || rtB . lxhr2aaydm || rtB . lxnxndvv2r
) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . gmj2ch3nqc = rtDW .
amjwljpykx ; rtB . ihlhv3dwad = ! rtB . gmj2ch3nqc ; } rtB . jdne4hpxzo = (
rtB . cdjlsurquj && rtB . ihlhv3dwad ) ; rtB . habuls35ce = ssGetT ( rtS ) ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { fx2esogudm ( rtS , rtB . dloffoxawu ,
rtB . habuls35ce , & rtB . ivtty1aemy , & rtDW . ivtty1aemy , & rtPrevZCX .
ivtty1aemy ) ; rtB . oh1denynze = rtB . ivtty1aemy . jhmp0c0tsm + rtP .
Constant_Value_gk5zjpa0wu ; } rtB . eq5clkyrat = ( rtB . oh1denynze > rtB .
habuls35ce ) ; rtB . hd4ysyvttg = ssGetT ( rtS ) ; if ( ssIsSampleHit ( rtS ,
1 , 0 ) ) { fx2esogudm ( rtS , rtB . lhqi3wbsnc , rtB . hd4ysyvttg , & rtB .
mvirue1zit , & rtDW . mvirue1zit , & rtPrevZCX . mvirue1zit ) ; rtB .
exepvpv5pt = rtB . mvirue1zit . jhmp0c0tsm + rtP . Constant_Value_mq1mynr50a
; } rtB . bxh4oqfrqa = ( rtB . exepvpv5pt > rtB . hd4ysyvttg ) ; rtB .
b3gcuynvsr = ssGetT ( rtS ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) {
fx2esogudm ( rtS , rtB . gvsvwxpjbi , rtB . b3gcuynvsr , & rtB . f3zha40yok ,
& rtDW . f3zha40yok , & rtPrevZCX . f3zha40yok ) ; rtB . kei1grw0zv = rtB .
f3zha40yok . jhmp0c0tsm + rtP . Constant_Value_khg11o10d4 ; } rtB .
ovcjl3ywbj = ( rtB . kei1grw0zv > rtB . b3gcuynvsr ) ; rtB . n4wxzf5mfu = rtB
. ddix2mhi1g * rtP . u_Value_n1cq0aprfw ; rtB . gnmfn3ghp0 = rtB . asa103d02z
* rtP . u_Value_el1pa43mzw ; rtB . p2ms0svh2r = rtB . n4wxzf5mfu + rtB .
gnmfn3ghp0 ; rtB . hreupja2ko = rtB . ndiffna1hx * rtP . u_Value_n1cq0aprfw ;
rtB . brifk2yygs = rtB . fizn4ck4n1 * rtP . u_Value_nmgw4a1j2t ; rtB .
mxy3hvbnwj = rtB . hreupja2ko + rtB . brifk2yygs ; rtB . ocrokcdjj5 = rtB .
p2ms0svh2r + rtB . mxy3hvbnwj ; rtB . ohlxxojvzp = rtB . krvqv214qo + rtB .
ocrokcdjj5 ; rtB . lklhlcmgtm = rtB . ddix2mhi1g + rtB . ndiffna1hx ; rtB .
lspdteffcd = rtB . lklhlcmgtm + rtB . krvqv214qo ; rtB . orckgxqawd = rtB .
lspdteffcd * rtP . u7_Value_eku41c2gbs ; rtB . mntkodot1n = rtB . asa103d02z
* rtP . u_Value_n1cq0aprfw ; rtB . al1tz0xrmr = rtB . ddix2mhi1g * rtP .
u_Value_el1pa43mzw ; rtB . jvvynr2ra5 = rtB . mntkodot1n - rtB . al1tz0xrmr ;
rtB . gpox32aoar = rtB . fizn4ck4n1 * rtP . u_Value_n1cq0aprfw ; rtB .
oa1z0mazaj = rtB . ndiffna1hx * rtP . u_Value_nmgw4a1j2t ; rtB . auups2234o =
rtB . gpox32aoar - rtB . oa1z0mazaj ; rtB . n0zlhqu15o = rtB . jvvynr2ra5 +
rtB . auups2234o ; rtB . bjghrwuyss = rtB . hlmtyy2tbw + rtB . n0zlhqu15o ;
rtB . oa1jb4acbi = rtB . bjghrwuyss * rtP . u7_Value_eku41c2gbs ; rtB .
dbazerhpey = rtB . ohlxxojvzp * rtP . u7_Value_eku41c2gbs ; rtB . c2m0f3ldgn
= rtB . asa103d02z + rtB . fizn4ck4n1 ; rtB . fbhxdkmr5f = rtB . c2m0f3ldgn +
rtB . hlmtyy2tbw ; rtB . jtowj3dcqc = rtB . fbhxdkmr5f * rtP .
u7_Value_eku41c2gbs ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . hjgindzzpd
= rtDW . keetxs0kf4 ; } rtB . lod5myckrr = muDoubleScalarSin ( rtB .
janxwpkdcj [ 6 ] ) ; rtB . h5eeblphjh = muDoubleScalarCos ( rtB . janxwpkdcj
[ 6 ] ) ; rtB . gwc1r5nfkf = rtB . lod5myckrr * rtB . lpgwkwyj4h [ 6 ] ; rtB
. jo4ob1ynks = rtB . lpgwkwyj4h [ 6 ] * rtB . h5eeblphjh ; rtB . nxvb0yitzn =
muDoubleScalarSin ( rtB . janxwpkdcj [ 10 ] ) ; rtB . irz4fkeeym =
muDoubleScalarCos ( rtB . janxwpkdcj [ 10 ] ) ; rtB . j5keoeghnq = rtB .
nxvb0yitzn * rtB . lpgwkwyj4h [ 10 ] ; rtB . d1ets4e1cr = rtB . lpgwkwyj4h [
10 ] * rtB . irz4fkeeym ; rtB . kpeqbfndy4 = ( rtX . hbdbwheonc > rtP .
Constant2_Value_nfwkaka1tb ) ; rtB . egbt5wn3et = rtX . hbdbwheonc - rtP .
Constant2_Value_nfwkaka1tb ; if ( ( rtDW . fpeafclpn0 == ( rtMinusInf ) ) ||
( rtDW . fpeafclpn0 == ssGetTaskTime ( rtS , 0 ) ) ) { rtDW . fpeafclpn0 =
ssGetTaskTime ( rtS , 0 ) ; rtB . iaafsyfx4i = rtP . Initial_Value ; } else {
rtB . iaafsyfx4i = rtB . egbt5wn3et ; } if ( ssIsMajorTimeStep ( rtS ) ) {
didZcEventOccur = ( rtB . kpeqbfndy4 && ( rtPrevZCX . oqfhcxlpho != 1 ) ) ;
rtPrevZCX . oqfhcxlpho = rtB . kpeqbfndy4 ; if ( didZcEventOccur || ( rtDW .
cauqpp1pbg != 0 ) ) { rtX . hbdbwheonc = rtB . iaafsyfx4i ;
ssSetBlockStateForSolverChangedAtMajorStep ( rtS ) ;
ssSetContTimeOutputInconsistentWithStateAtMajorStep ( rtS ) ; } } if ( rtX .
hbdbwheonc >= rtP . Integrator_UpperSat ) { if ( rtX . hbdbwheonc != rtP .
Integrator_UpperSat ) { rtX . hbdbwheonc = rtP . Integrator_UpperSat ;
ssSetBlockStateForSolverChangedAtMajorStep ( rtS ) ;
ssSetContTimeOutputInconsistentWithStateAtMajorStep ( rtS ) ; } } else { if (
( rtX . hbdbwheonc <= rtP . Integrator_LowerSat ) && ( rtX . hbdbwheonc !=
rtP . Integrator_LowerSat ) ) { rtX . hbdbwheonc = rtP . Integrator_LowerSat
; ssSetBlockStateForSolverChangedAtMajorStep ( rtS ) ;
ssSetContTimeOutputInconsistentWithStateAtMajorStep ( rtS ) ; } } rtB .
nncind1ifh = rtX . hbdbwheonc ; if ( ssIsSampleHit ( rtS , 1 , 0 ) &&
ssIsMajorTimeStep ( rtS ) ) { if ( rtP . Constant1_Value_fnd3rhhz1s > 0.0 ) {
if ( ! rtDW . br5wyeghdu ) { u0 = ssGetTStart ( rtS ) ; if ( ssGetTaskTime (
rtS , 1 ) != u0 ) { ssSetBlockStateForSolverChangedAtMajorStep ( rtS ) ; }
rtDW . br5wyeghdu = true ; } } else { if ( rtDW . br5wyeghdu ) {
ssSetBlockStateForSolverChangedAtMajorStep ( rtS ) ; if ( rtDW . kseqhf442uv
. naigfwcupc ) { cl3avcjyrj ( & rtDW . kseqhf442uv ) ; } if ( rtDW .
cgsqxgexc24 . htd2qpz0x3 ) { c5hqhunjdy ( & rtDW . cgsqxgexc24 ) ; } rtDW .
br5wyeghdu = false ; } } } if ( rtDW . br5wyeghdu ) { { real_T * * uBuffer =
( real_T * * ) & rtDW . gxot3lz5fc . TUbufferPtrs [ 0 ] ; real_T * * tBuffer
= ( real_T * * ) & rtDW . gxot3lz5fc . TUbufferPtrs [ 1 ] ; real_T * *
xBuffer = ( real_T * * ) & rtDW . gxot3lz5fc . TUbufferPtrs [ 2 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T appliedDelay ; rtB . dd112zwqm2 =
rt_VTDelayfindtDInterpolate ( rtX . gfghljc4gn , * tBuffer , * uBuffer , *
xBuffer , rtDW . nklbecjxrg . CircularBufSize , rtDW . nklbecjxrg . Head ,
rtDW . nklbecjxrg . Tail , & rtDW . nklbecjxrg . Last , simTime , 0.0 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) , rtP . VariableTransportDelay_InitOutput , & appliedDelay
) ; } rtB . i4ietgmv3i = rtX . cgbqhiyy4k ; rtB . bjpxkki04u = ssGetT ( rtS )
; rtB . nkg3x3lyuy = ( rtB . bjpxkki04u >= rtP . Constant_Value ) ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . hafmzsxmva = rtDW . lnio5vndj4 ; } if
( rtB . nkg3x3lyuy ) { rtB . hsdnhqzxh4 = rtB . i4ietgmv3i - rtB . dd112zwqm2
; rtB . i2actwuxms = rtB . hsdnhqzxh4 * rtB . hjgindzzpd ; rtB . mdhu3gdhh2 =
rtB . i2actwuxms ; } else { rtB . mdhu3gdhh2 = rtB . hafmzsxmva ; } { real_T
* * uBuffer = ( real_T * * ) & rtDW . cay5bhojbk . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & rtDW . cay5bhojbk . TUbufferPtrs [ 1 ]
; real_T * * xBuffer = ( real_T * * ) & rtDW . cay5bhojbk . TUbufferPtrs [ 2
] ; real_T simTime = ssGetT ( rtS ) ; real_T appliedDelay ; rtB . e2yp0mvpr2
= rt_VTDelayfindtDInterpolate ( rtX . nhazmrtgmb , * tBuffer , * uBuffer , *
xBuffer , rtDW . lxbhmg25bu . CircularBufSize , rtDW . lxbhmg25bu . Head ,
rtDW . lxbhmg25bu . Tail , & rtDW . lxbhmg25bu . Last , simTime , 0.0 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) , rtP . VariableTransportDelay_InitOutput_hx330ne3nw , &
appliedDelay ) ; } rtB . lyqkavx3aw = rtX . pjrskjmpvo ; rtB . phmd0eflgv =
ssGetT ( rtS ) ; rtB . ethsaet3tl = ( rtB . phmd0eflgv >= rtP .
Constant_Value_fmk5ezykoy ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
bt51e1cisj = rtDW . cxai1mydgu ; } if ( rtB . ethsaet3tl ) { rtB . p5grv1pbp0
= rtB . lyqkavx3aw - rtB . e2yp0mvpr2 ; rtB . gphwxsxelo = rtB . p5grv1pbp0 *
rtB . hjgindzzpd ; rtB . ergyhagb2w = rtB . gphwxsxelo ; } else { rtB .
ergyhagb2w = rtB . bt51e1cisj ; } rtB . dzdmf2wqsp . re = rtB . mdhu3gdhh2 ;
rtB . dzdmf2wqsp . im = rtB . ergyhagb2w ; rtB . bl02keycds =
muDoubleScalarHypot ( rtB . dzdmf2wqsp . re , rtB . dzdmf2wqsp . im ) ; rtB .
ivf3ymyffp = muDoubleScalarAtan2 ( rtB . dzdmf2wqsp . im , rtB . dzdmf2wqsp .
re ) ; u0 = rtB . bl02keycds ; maxV = rtP . Saturation_LowerSat ; u2 = rtP .
Saturation_UpperSat ; if ( u0 > u2 ) { rtB . hoq1r30cg5 = u2 ; } else if ( u0
< maxV ) { rtB . hoq1r30cg5 = maxV ; } else { rtB . hoq1r30cg5 = u0 ; } maxV
= rtB . hoq1r30cg5 ; rtB . hguzzrwry5 = 1.0 / maxV ; u0 = rtB . hjgindzzpd ;
maxV = rtP . Toavoiddivisionbyzero_LowerSat ; u2 = rtP .
Toavoiddivisionbyzero_UpperSat ; if ( u0 > u2 ) { rtB . kq0ft4vekw = u2 ; }
else if ( u0 < maxV ) { rtB . kq0ft4vekw = maxV ; } else { rtB . kq0ft4vekw =
u0 ; } rtB . gl2tdboxeo = 1.0 / rtB . kq0ft4vekw ; u0 = rtB . hjgindzzpd ;
maxV = rtP . Toavoiddivisionbyzero_LowerSat_h4nn04btge ; u2 = rtP .
Toavoiddivisionbyzero_UpperSat_cy1y3pchfk ; if ( u0 > u2 ) { rtB . nmwseh5ngl
= u2 ; } else if ( u0 < maxV ) { rtB . nmwseh5ngl = maxV ; } else { rtB .
nmwseh5ngl = u0 ; } rtB . aayb2fnbde = 1.0 / rtB . nmwseh5ngl ; rtB .
mlf2mpucqx = rtP . RadDeg_Gain * rtB . ivf3ymyffp ; for ( i = 0 ; i < 3 ; i
++ ) { rtB . pktagbnkvw [ i ] = 0.0 ; rtB . pktagbnkvw [ i ] += rtP .
Gain3_Gain [ i ] * rtB . hqf4mnhqso [ 0 ] ; rtB . pktagbnkvw [ i ] += rtP .
Gain3_Gain [ i + 3 ] * rtB . hqf4mnhqso [ 1 ] ; rtB . pktagbnkvw [ i ] += rtP
. Gain3_Gain [ i + 6 ] * rtB . hqf4mnhqso [ 2 ] ; } rtB . pow4amgqo5 [ 0 ] =
rtP . Gain1_Gain * rtB . pktagbnkvw [ 0 ] ; rtB . pow4amgqo5 [ 1 ] = rtP .
Gain1_Gain * rtB . pktagbnkvw [ 1 ] ; rtB . pow4amgqo5 [ 2 ] = rtP .
Gain1_Gain * rtB . pktagbnkvw [ 2 ] ; kseqhf442u ( rtS , rtB . dq5pywjhv3 , &
rtB . pow4amgqo5 [ 0 ] , rtB . nncind1ifh , & rtB . kseqhf442uv , & rtDW .
kseqhf442uv ) ; cgsqxgexc2 ( rtS , rtB . n2vcnfd4ps , & rtB . pow4amgqo5 [ 0
] , rtB . nncind1ifh , & rtB . cgsqxgexc24 , & rtDW . cgsqxgexc24 ) ; if (
rtB . n2vcnfd4ps != 0 ) { rtB . guosjh35iw [ 0 ] = rtB . cgsqxgexc24 .
pbikwiccuu ; rtB . guosjh35iw [ 1 ] = rtB . cgsqxgexc24 . d0xxcannsh ; } else
{ rtB . guosjh35iw [ 0 ] = rtB . kseqhf442uv . pg4m5drjhd ; rtB . guosjh35iw
[ 1 ] = rtB . kseqhf442uv . jbsowncmif ; } if ( ssIsMajorTimeStep ( rtS ) ) {
srUpdateBC ( rtDW . eeuhsl20cs ) ; } } if ( rtX . oipozxyfbt >= rtP .
Integrator_UpperSat_hkcvjiz0hz ) { if ( rtX . oipozxyfbt != rtP .
Integrator_UpperSat_hkcvjiz0hz ) { rtX . oipozxyfbt = rtP .
Integrator_UpperSat_hkcvjiz0hz ; ssSetBlockStateForSolverChangedAtMajorStep (
rtS ) ; } } else { if ( ( rtX . oipozxyfbt <= rtP .
Integrator_LowerSat_i4hvpryy1p ) && ( rtX . oipozxyfbt != rtP .
Integrator_LowerSat_i4hvpryy1p ) ) { rtX . oipozxyfbt = rtP .
Integrator_LowerSat_i4hvpryy1p ; ssSetBlockStateForSolverChangedAtMajorStep (
rtS ) ; } } rtB . nk0xczz2qt = rtX . oipozxyfbt ; { real_T * * uBuffer = (
real_T * * ) & rtDW . bizzathsjz . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & rtDW . bizzathsjz . TUbufferPtrs [ 1 ] ; real_T * * xBuffer
= ( real_T * * ) & rtDW . bizzathsjz . TUbufferPtrs [ 2 ] ; real_T simTime =
ssGetT ( rtS ) ; real_T appliedDelay ; rtB . che4dwwuod =
rt_VTDelayfindtDInterpolate ( rtX . nrjwdf4bl5 , * tBuffer , * uBuffer , *
xBuffer , rtDW . lhuqmlzywi . CircularBufSize , rtDW . lhuqmlzywi . Head ,
rtDW . lhuqmlzywi . Tail , & rtDW . lhuqmlzywi . Last , simTime , 0.0 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) , rtP . VariableTransportDelay_InitOutput_ggymofnpzk , &
appliedDelay ) ; } rtB . pbkcujkqmn = rtX . d543hn3jm3 ; rtB . ngvspq1n0c =
ssGetT ( rtS ) ; rtB . eesppz0ub1 = ( rtB . ngvspq1n0c >= rtP .
Constant_Value_g5ytttslrx ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
aj4pxngesa = rtDW . niklp3ydpx ; } if ( rtB . eesppz0ub1 ) { rtB . jvxhenkiyr
= rtB . pbkcujkqmn - rtB . che4dwwuod ; rtB . lnx4xuds4p = rtB . jvxhenkiyr *
rtB . hjgindzzpd ; rtB . pqu4hljq1u = rtB . lnx4xuds4p ; } else { rtB .
pqu4hljq1u = rtB . aj4pxngesa ; } rtB . pyr403nrwd = rtB . pqu4hljq1u * rtB .
hguzzrwry5 ; rtB . omzj4fst4w = rtP . Continuous_Kp * rtB . pyr403nrwd ; rtB
. l11wobikak = rtP . Continuous_Ki * rtB . pyr403nrwd ; rtB . d2ggya3nxd =
rtP . Continuous_Kd * rtB . pyr403nrwd ; rtB . dqtc2zjznc = 0.0 ; rtB .
dqtc2zjznc += rtP . TransferFcn_C * rtX . ktq1qocrqs ; rtB . dqtc2zjznc +=
rtP . TransferFcn_D * rtB . d2ggya3nxd ; rtB . p5vr0lqqox = ( rtB .
omzj4fst4w + rtB . nk0xczz2qt ) + rtB . dqtc2zjznc ; u0 = rtB . p5vr0lqqox ;
maxV = rtP . Saturation2_LowerSat ; u2 = rtP . Saturation2_UpperSat ; if ( u0
> u2 ) { rtB . aq0oxjnh5o = u2 ; } else if ( u0 < maxV ) { rtB . aq0oxjnh5o =
maxV ; } else { rtB . aq0oxjnh5o = u0 ; } rtB . jvmzxxtvin = rtP .
Gain10_Gain * rtB . aq0oxjnh5o ; if ( ( rtDW . oxppwiig31 >= ssGetTaskTime (
rtS , 0 ) ) && ( rtDW . idgf0xanwj >= ssGetTaskTime ( rtS , 0 ) ) ) { rtB .
ifd3zv0tlm = rtB . jvmzxxtvin ; } else { if ( ( ( rtDW . oxppwiig31 < rtDW .
idgf0xanwj ) && ( rtDW . idgf0xanwj < ssGetTaskTime ( rtS , 0 ) ) ) || ( (
rtDW . oxppwiig31 >= rtDW . idgf0xanwj ) && ( rtDW . oxppwiig31 >=
ssGetTaskTime ( rtS , 0 ) ) ) ) { u2 = ssGetTaskTime ( rtS , 0 ) - rtDW .
idgf0xanwj ; maxV = rtDW . kriwkpf2uu ; } else { u2 = ssGetTaskTime ( rtS , 0
) - rtDW . oxppwiig31 ; maxV = rtDW . medpytok2l ; } riseValLimit = u2 * rtP
. RateLimiter_RisingLim ; u0 = rtB . jvmzxxtvin - maxV ; if ( u0 >
riseValLimit ) { rtB . ifd3zv0tlm = maxV + riseValLimit ; } else { u2 *= rtP
. RateLimiter_FallingLim ; if ( u0 < u2 ) { rtB . ifd3zv0tlm = maxV + u2 ; }
else { rtB . ifd3zv0tlm = rtB . jvmzxxtvin ; } } } rtB . mtcz0lnn0n = rtX .
ih0sh50oxm ; rtB . kbbjhroyxq = rtP . A11_Gain * rtB . mtcz0lnn0n ; rtB .
cazifiivxq = rtX . pn05gdwhsv ; rtB . gzqcgknmhx = rtP . A12_Gain * rtB .
cazifiivxq ; rtB . cjra1jzusk = rtP . A21_Gain * rtB . mtcz0lnn0n ; rtB .
c3akryd55j = rtP . A22_Gain * rtB . cazifiivxq ; rtB . okqckozhbq = rtB .
kbbjhroyxq + rtB . gzqcgknmhx ; rtB . kga22oycoi = rtB . cjra1jzusk + rtB .
c3akryd55j ; rtB . nl25lbukzt = rtP . B11_Gain * rtB . ifd3zv0tlm ; rtB .
frpbg1j3eo = rtB . okqckozhbq + rtB . nl25lbukzt ; rtB . a1qsclad2n = rtP .
B21_Gain * rtB . ifd3zv0tlm ; rtB . ajbsi41ckn = rtB . kga22oycoi + rtB .
a1qsclad2n ; rtB . oqazm3qnjo = rtP . C11_Gain * rtB . mtcz0lnn0n ; rtB .
jzywiqlcz1 = rtP . C12_Gain * rtB . cazifiivxq ; rtB . encorhdguj = rtB .
oqazm3qnjo + rtB . jzywiqlcz1 ; rtB . deii30qqhr = rtP . Du_Gain * rtB .
ifd3zv0tlm ; rtB . hcvzaszisz = rtB . deii30qqhr + rtB . encorhdguj ; u0 =
rtB . hjgindzzpd ; maxV = rtP . Toavoiddivisionbyzero_LowerSat_ig0rnyxqnm ;
u2 = rtP . Toavoiddivisionbyzero_UpperSat_nilybetqa2 ; if ( u0 > u2 ) { rtB .
gjggxafgsz = u2 ; } else if ( u0 < maxV ) { rtB . gjggxafgsz = maxV ; } else
{ rtB . gjggxafgsz = u0 ; } rtB . dyp34geds0 = 1.0 / rtB . gjggxafgsz ; for (
i = 0 ; i < 3 ; i ++ ) { rtB . ah3sipge5p [ i ] = 0.0 ; rtB . ah3sipge5p [ i
] += rtP . Gain3_Gain_g50yfs0ujj [ i ] * rtB . hqf4mnhqso [ 0 ] ; rtB .
ah3sipge5p [ i ] += rtP . Gain3_Gain_g50yfs0ujj [ i + 3 ] * rtB . hqf4mnhqso
[ 1 ] ; rtB . ah3sipge5p [ i ] += rtP . Gain3_Gain_g50yfs0ujj [ i + 6 ] * rtB
. hqf4mnhqso [ 2 ] ; } rtB . b2b0msxifu [ 0 ] = rtP . Gain1_Gain_bdwdgwdftn *
rtB . ah3sipge5p [ 0 ] ; rtB . b2b0msxifu [ 1 ] = rtP . Gain1_Gain_bdwdgwdftn
* rtB . ah3sipge5p [ 1 ] ; rtB . b2b0msxifu [ 2 ] = rtP .
Gain1_Gain_bdwdgwdftn * rtB . ah3sipge5p [ 2 ] ; kseqhf442u ( rtS , rtB .
mzpjbmxfwp , & rtB . b2b0msxifu [ 0 ] , rtB . nncind1ifh , & rtB . n3ncmzva14
, & rtDW . n3ncmzva14 ) ; cgsqxgexc2 ( rtS , rtB . mbtqxty3np , & rtB .
b2b0msxifu [ 0 ] , rtB . nncind1ifh , & rtB . dx1zpxayg3 , & rtDW .
dx1zpxayg3 ) ; if ( rtB . mbtqxty3np != 0 ) { rtB . jbrvxtovno [ 0 ] = rtB .
dx1zpxayg3 . pbikwiccuu ; rtB . jbrvxtovno [ 1 ] = rtB . dx1zpxayg3 .
d0xxcannsh ; } else { rtB . jbrvxtovno [ 0 ] = rtB . n3ncmzva14 . pg4m5drjhd
; rtB . jbrvxtovno [ 1 ] = rtB . n3ncmzva14 . jbsowncmif ; } rtB . aad3aylvaq
= rtB . ctkxwfjqo4 - rtB . nyn1hlelu1 ; rtB . jbgrxhaht3 = rtB . dl425wbmtl -
rtB . g3cditrnls ; rtB . mdva2sqrrs = rtB . egfgjyyopt - rtB . izatkryyxm ;
rtB . m1r1k0zbbv = rtB . oaxc3hrb45 - rtB . opvysr4u4o ; rtB . ho540azixt =
rtB . izatkryyxm - rtB . do4x3mpyxa ; rtB . isgwkn2qqf = rtB . opvysr4u4o -
rtB . coehnzyatq ; rtB . aogexskr2j = rtB . nyn1hlelu1 - rtB . c5dodwizkd ;
rtB . btaqf2t04r = rtB . g3cditrnls - rtB . pnlxv4w3fb ; rtB . gsgbnukbwp =
rtB . do4x3mpyxa - rtB . egfgjyyopt ; rtB . aegzucdjst = rtB . coehnzyatq -
rtB . oaxc3hrb45 ; rtB . kggl5nc31g = rtB . c5dodwizkd - rtB . ctkxwfjqo4 ;
rtB . gmin1igijm = rtB . pnlxv4w3fb - rtB . dl425wbmtl ; rtB . hpzdfhi3xg =
rtB . aogexskr2j * rtB . gsgbnukbwp ; rtB . ag20b0nda4 = rtB . btaqf2t04r *
rtB . aegzucdjst ; rtB . czbsafrvcd = rtB . hpzdfhi3xg + rtB . ag20b0nda4 ;
rtB . fhgg255jnc = rtB . gsgbnukbwp * rtB . gsgbnukbwp ; rtB . ggo3ciht4v =
rtB . aegzucdjst * rtB . aegzucdjst ; rtB . pvdrehmase = rtB . fhgg255jnc +
rtB . ggo3ciht4v ; rtB . ocaakssu25 = rtB . btaqf2t04r * rtB . gsgbnukbwp ;
rtB . jeq4nvdarz = rtB . aogexskr2j * rtB . aegzucdjst ; rtB . ovxftaqng0 =
rtB . ocaakssu25 - rtB . jeq4nvdarz ; rtB . eksjtbzisx = rtB . czbsafrvcd /
rtB . pvdrehmase ; rtB . lhdjsf04o4 = rtB . ovxftaqng0 / rtB . pvdrehmase ;
rtB . iyii4lobb4 = rtB . aad3aylvaq * rtB . ho540azixt ; rtB . mphawcr3dt =
rtB . jbgrxhaht3 * rtB . isgwkn2qqf ; rtB . lwllou5nih = rtB . iyii4lobb4 +
rtB . mphawcr3dt ; rtB . d3csgx14me = rtB . ho540azixt * rtB . ho540azixt ;
rtB . m210skhpce = rtB . isgwkn2qqf * rtB . isgwkn2qqf ; rtB . b2gsvmmzy5 =
rtB . d3csgx14me + rtB . m210skhpce ; rtB . f21ghdzteb = rtB . jbgrxhaht3 *
rtB . ho540azixt ; rtB . dbdvmk0lqm = rtB . aad3aylvaq * rtB . isgwkn2qqf ;
rtB . o5fdegujcz = rtB . f21ghdzteb - rtB . dbdvmk0lqm ; rtB . kzyjynoljr =
rtB . lwllou5nih / rtB . b2gsvmmzy5 ; rtB . cjck0dkf3t = rtB . o5fdegujcz /
rtB . b2gsvmmzy5 ; rtB . p3yklbbxgx = rtB . kggl5nc31g * rtB . mdva2sqrrs ;
rtB . ibsdc25b3e = rtB . gmin1igijm * rtB . m1r1k0zbbv ; rtB . dj02l2etis =
rtB . p3yklbbxgx + rtB . ibsdc25b3e ; rtB . pm5gnnfecx = rtB . mdva2sqrrs *
rtB . mdva2sqrrs ; rtB . kk2zyebqld = rtB . m1r1k0zbbv * rtB . m1r1k0zbbv ;
rtB . k4lrqanid3 = rtB . pm5gnnfecx + rtB . kk2zyebqld ; rtB . l0dksmhc5f =
rtB . gmin1igijm * rtB . mdva2sqrrs ; rtB . mh4sjhhzyz = rtB . kggl5nc31g *
rtB . m1r1k0zbbv ; rtB . mp1qltziuv = rtB . l0dksmhc5f - rtB . mh4sjhhzyz ;
rtB . ewamnoynhf = rtB . dj02l2etis / rtB . k4lrqanid3 ; rtB . gbu2xp4ckn =
rtB . mp1qltziuv / rtB . k4lrqanid3 ; rtB . iaa1jkuobs = rtX . jzyjyrpy3w ; {
real_T * * uBuffer = ( real_T * * ) & rtDW . p3evevjx4y . TUbufferPtrs [ 0 ]
; real_T * * tBuffer = ( real_T * * ) & rtDW . p3evevjx4y . TUbufferPtrs [ 1
] ; real_T simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_dy4stxs3xg ; rtB . hsooj23eby = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . hhnmd3q34u .
CircularBufSize , & rtDW . hhnmd3q34u . Last , rtDW . hhnmd3q34u . Tail ,
rtDW . hhnmd3q34u . Head , rtP . TransportDelay_InitOutput_kw3wsvjvtt , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . gddx0kvolx = ssGetT ( rtS ) ; rtB . mjv20fyguk
= ( rtB . gddx0kvolx >= rtP . K1_Value_icgt2ckuyw ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . izrhcqz2hs = rtDW . f3ocqnmitg ; } if ( rtB .
mjv20fyguk ) { rtB . nbtforitey = rtB . iaa1jkuobs - rtB . hsooj23eby ; rtB .
hvibvgdhss = rtP . Gain_Gain_etuimejytz * rtB . nbtforitey ; rtB . kk2ddruedb
= rtB . hvibvgdhss ; } else { rtB . kk2ddruedb = rtB . izrhcqz2hs ; } rtB .
fodvyk4sp5 = rtX . ehyodyswyu ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. gei2va2yg1 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . gei2va2yg1 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_bvh43vdfug ; rtB .
kgj3crhp20 = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . kzmgrvlihj . CircularBufSize , & rtDW . kzmgrvlihj . Last , rtDW .
kzmgrvlihj . Tail , rtDW . kzmgrvlihj . Head , rtP .
TransportDelay_InitOutput_ben3dx2m34 , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
osajrbwpib = ssGetT ( rtS ) ; rtB . dllqq41asf = ( rtB . osajrbwpib >= rtP .
K1_Value_b5f1iwcuvl ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
chynv15tox = rtDW . b1jopw25yj ; } if ( rtB . dllqq41asf ) { rtB . dwt4ygl2dm
= rtB . fodvyk4sp5 - rtB . kgj3crhp20 ; rtB . pyupjia3ir = rtP .
Gain_Gain_flbc3peo4e * rtB . dwt4ygl2dm ; rtB . clpub5lrjn = rtB . pyupjia3ir
; } else { rtB . clpub5lrjn = rtB . chynv15tox ; } rtB . ozteq355fg . re =
rtB . kk2ddruedb ; rtB . ozteq355fg . im = rtB . clpub5lrjn ; rtB .
ddquqjpoyz = muDoubleScalarHypot ( rtB . ozteq355fg . re , rtB . ozteq355fg .
im ) ; rtB . gilwdv5hxt = muDoubleScalarAtan2 ( rtB . ozteq355fg . im , rtB .
ozteq355fg . re ) ; rtB . abxak3drwh = rtP . RadDeg_Gain_lmailopoyl * rtB .
gilwdv5hxt ; rtB . mk2j5mq0z1 = rtP . degrad_Gain * rtB . abxak3drwh ; maxV =
rtB . mk2j5mq0z1 ; u0 = rtB . ddquqjpoyz ; u2 = muDoubleScalarCos ( maxV ) ;
maxV = muDoubleScalarSin ( maxV ) ; rtB . fjfop01sxf . re = u0 * u2 ; rtB .
fjfop01sxf . im = u0 * maxV ; rtB . lqwcgolu0u = rtX . hyqpl142bi ; { real_T
* * uBuffer = ( real_T * * ) & rtDW . ofjbrmvngv . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & rtDW . ofjbrmvngv . TUbufferPtrs [ 1 ]
; real_T simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_i0kl3kk3fa ; rtB . da123lpa1c = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . nftilndrdl .
CircularBufSize , & rtDW . nftilndrdl . Last , rtDW . nftilndrdl . Tail ,
rtDW . nftilndrdl . Head , rtP . TransportDelay_InitOutput_nlfm4vqjuk , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . fsrqs1x1x5 = ssGetT ( rtS ) ; rtB . fqb0xwwc35
= ( rtB . fsrqs1x1x5 >= rtP . K1_Value_opwlow5zkg ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . a32biypngl = rtDW . ewe0bkor5i ; } if ( rtB .
fqb0xwwc35 ) { rtB . borlfbs0dv = rtB . lqwcgolu0u - rtB . da123lpa1c ; rtB .
h3vqcgn2px = rtP . Gain_Gain_npz0erokpf * rtB . borlfbs0dv ; rtB . mpaaevtb0h
= rtB . h3vqcgn2px ; } else { rtB . mpaaevtb0h = rtB . a32biypngl ; } rtB .
byucczfrd5 = rtX . prno02pbsy ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. gfxye1rcxy . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . gfxye1rcxy . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_i1bwpxgdon ; rtB .
i2slpiutt5 = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . jl4zgwwouj . CircularBufSize , & rtDW . jl4zgwwouj . Last , rtDW .
jl4zgwwouj . Tail , rtDW . jl4zgwwouj . Head , rtP .
TransportDelay_InitOutput_byvexhkqgs , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
gug0vq1ph1 = ssGetT ( rtS ) ; rtB . d4xnfwj5rk = ( rtB . gug0vq1ph1 >= rtP .
K1_Value_blpuhq4bqk ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
js2tgm2mvb = rtDW . l5dlababv1 ; } if ( rtB . d4xnfwj5rk ) { rtB . i0w3qtz5ww
= rtB . byucczfrd5 - rtB . i2slpiutt5 ; rtB . ld4w4tkofr = rtP .
Gain_Gain_j0jjpr0thv * rtB . i0w3qtz5ww ; rtB . m3qxkgrymt = rtB . ld4w4tkofr
; } else { rtB . m3qxkgrymt = rtB . js2tgm2mvb ; } rtB . borse0k0c1 . re =
rtB . mpaaevtb0h ; rtB . borse0k0c1 . im = rtB . m3qxkgrymt ; rtB .
bhihq3ly1v = muDoubleScalarHypot ( rtB . borse0k0c1 . re , rtB . borse0k0c1 .
im ) ; rtB . e0wozwcf3t = muDoubleScalarAtan2 ( rtB . borse0k0c1 . im , rtB .
borse0k0c1 . re ) ; rtB . cccanviagj = rtP . RadDeg_Gain_jyyu23b1lt * rtB .
e0wozwcf3t ; rtB . lpqtmabs0w = rtP . degrad1_Gain * rtB . cccanviagj ; maxV
= rtB . lpqtmabs0w ; u0 = rtB . bhihq3ly1v ; u2 = muDoubleScalarCos ( maxV )
; maxV = muDoubleScalarSin ( maxV ) ; rtB . nzo2cbsxc1 . re = u0 * u2 ; rtB .
nzo2cbsxc1 . im = u0 * maxV ; rtB . gznbijk505 = rtX . aztsfsn2hi ; { real_T
* * uBuffer = ( real_T * * ) & rtDW . abjlexkzmf . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & rtDW . abjlexkzmf . TUbufferPtrs [ 1 ]
; real_T simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_eocvd5lrwh ; rtB . fgsl4jezwt = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . ntkdqjpfyg .
CircularBufSize , & rtDW . ntkdqjpfyg . Last , rtDW . ntkdqjpfyg . Tail ,
rtDW . ntkdqjpfyg . Head , rtP . TransportDelay_InitOutput_akk50h0opn , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . d3bqkgormz = ssGetT ( rtS ) ; rtB . os0vs1nlbx
= ( rtB . d3bqkgormz >= rtP . K1_Value_k44hcys2dx ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . jqcjj0zofh = rtDW . b3csr0smh2 ; } if ( rtB .
os0vs1nlbx ) { rtB . e0z0xfmdsg = rtB . gznbijk505 - rtB . fgsl4jezwt ; rtB .
ga0i4z0sfw = rtP . Gain_Gain_a3gcbd4jez * rtB . e0z0xfmdsg ; rtB . b1nghd5jfn
= rtB . ga0i4z0sfw ; } else { rtB . b1nghd5jfn = rtB . jqcjj0zofh ; } rtB .
imfsawafbw = rtX . f04g4otdzz ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. ggw2hbirki . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . ggw2hbirki . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_l0yi3guf4c ; rtB .
fsfr2eq4is = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . krie5kar0s . CircularBufSize , & rtDW . krie5kar0s . Last , rtDW .
krie5kar0s . Tail , rtDW . krie5kar0s . Head , rtP .
TransportDelay_InitOutput_ngohlcso1t , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
k0godhp5eq = ssGetT ( rtS ) ; rtB . j4liuar15z = ( rtB . k0godhp5eq >= rtP .
K1_Value_bzwr4f2dg4 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
jgpydkatty = rtDW . gdgisolefd ; } if ( rtB . j4liuar15z ) { rtB . lbz02lzasu
= rtB . imfsawafbw - rtB . fsfr2eq4is ; rtB . alu3en21vg = rtP .
Gain_Gain_pgtxpkcyze * rtB . lbz02lzasu ; rtB . hegy2zggyj = rtB . alu3en21vg
; } else { rtB . hegy2zggyj = rtB . jgpydkatty ; } rtB . mtnovueigj . re =
rtB . b1nghd5jfn ; rtB . mtnovueigj . im = rtB . hegy2zggyj ; rtB .
nf42rgcjj0 = muDoubleScalarHypot ( rtB . mtnovueigj . re , rtB . mtnovueigj .
im ) ; rtB . j0q3ks0kdz = muDoubleScalarAtan2 ( rtB . mtnovueigj . im , rtB .
mtnovueigj . re ) ; rtB . mrl0wnwkwl = rtP . RadDeg_Gain_ldmucatwbe * rtB .
j0q3ks0kdz ; rtB . jv41eyyq4v = rtP . degrad2_Gain * rtB . mrl0wnwkwl ; maxV
= rtB . jv41eyyq4v ; u0 = rtB . nf42rgcjj0 ; u2 = muDoubleScalarCos ( maxV )
; maxV = muDoubleScalarSin ( maxV ) ; rtB . hh2x5ue2yt . re = u0 * u2 ; rtB .
hh2x5ue2yt . im = u0 * maxV ; jvoeh2x3fa ( rtS , rtP .
Constant_Value_b3oixf3fv3 , rtB . fjfop01sxf , rtB . nzo2cbsxc1 , rtB .
hh2x5ue2yt , & rtB . dg2ggauitn , & rtDW . dg2ggauitn , & rtP . dg2ggauitn )
; jvoeh2x3fa ( rtS , rtP . Constant1_Value_kgwpiyl0dx , rtB . fjfop01sxf ,
rtB . nzo2cbsxc1 , rtB . hh2x5ue2yt , & rtB . jvoeh2x3far , & rtDW .
jvoeh2x3far , & rtP . jvoeh2x3far ) ; czm0w3hmwh ( rtS , rtP .
Constant2_Value_ptygdyn2z2 , rtB . fjfop01sxf , rtB . nzo2cbsxc1 , rtB .
hh2x5ue2yt , & rtB . czm0w3hmwho , & rtDW . czm0w3hmwho , & rtP . czm0w3hmwho
) ; rtB . dtqin2jgeo [ 0 ] = muDoubleScalarHypot ( rtB . dg2ggauitn .
hjcfumkc1i . re , rtB . dg2ggauitn . hjcfumkc1i . im ) ; rtB . dtqin2jgeo [ 1
] = muDoubleScalarHypot ( rtB . jvoeh2x3far . hjcfumkc1i . re , rtB .
jvoeh2x3far . hjcfumkc1i . im ) ; rtB . dtqin2jgeo [ 2 ] =
muDoubleScalarHypot ( rtB . czm0w3hmwho . fnsdbz5hxw . re , rtB . czm0w3hmwho
. fnsdbz5hxw . im ) ; rtB . cqfwms51ms [ 0 ] = muDoubleScalarAtan2 ( rtB .
dg2ggauitn . hjcfumkc1i . im , rtB . dg2ggauitn . hjcfumkc1i . re ) ; rtB .
cqfwms51ms [ 1 ] = muDoubleScalarAtan2 ( rtB . jvoeh2x3far . hjcfumkc1i . im
, rtB . jvoeh2x3far . hjcfumkc1i . re ) ; rtB . cqfwms51ms [ 2 ] =
muDoubleScalarAtan2 ( rtB . czm0w3hmwho . fnsdbz5hxw . im , rtB . czm0w3hmwho
. fnsdbz5hxw . re ) ; rtB . hxo01rllki = rtP . Gain3_Gain_pubvz1sets * rtB .
dtqin2jgeo [ 2 ] ; rtB . eq32hkutpu = rtP . raddeg1_Gain * rtB . cqfwms51ms [
2 ] ; rtB . dbeegne0gu = rtP . Gain2_Gain_ktxavolvev * rtB . eq32hkutpu ; rtB
. f0l1g4vcjw = muDoubleScalarCos ( rtB . dbeegne0gu ) ; rtB . n01bgh2bvn =
rtB . hxo01rllki * rtB . f0l1g4vcjw ; rtB . dooquzjqh0 = rtB . n01bgh2bvn *
rtP . Constant_Value_pftixvuxf3 ; rtB . gmozqnjl4g = muDoubleScalarSin ( rtB
. dbeegne0gu ) ; rtB . onz1gvrvhg = rtB . gmozqnjl4g * rtB . hxo01rllki ; rtB
. aoutc2cnuh = rtB . onz1gvrvhg * rtP . Constant1_Value_ajnerby2tq ; rtB .
cfzptmwazk = rtB . dooquzjqh0 - rtB . aoutc2cnuh ; rtB . ejeztrap5o = rtB .
cfzptmwazk + rtB . izatkryyxm ; rtB . jlrqxouuyv = rtB . ctkxwfjqo4 * rtB .
ejeztrap5o ; rtB . lefzubgbyl = rtP . Constant_Value_pftixvuxf3 * rtB .
onz1gvrvhg ; rtB . jnjc2g42mo = rtP . Constant1_Value_ajnerby2tq * rtB .
n01bgh2bvn ; rtB . gwtv3svq4f = rtB . lefzubgbyl + rtB . jnjc2g42mo ; rtB .
hoqfdr02sr = rtB . gwtv3svq4f + rtB . opvysr4u4o ; rtB . a4z3t5z3ky = rtB .
dl425wbmtl * rtB . hoqfdr02sr ; rtB . fxepc2xcpp = rtB . jlrqxouuyv + rtB .
a4z3t5z3ky ; rtB . d5qucfuocs = rtB . ejeztrap5o * rtB . ejeztrap5o ; rtB .
ixy4y4fia1 = rtB . hoqfdr02sr * rtB . hoqfdr02sr ; rtB . n5hwfgbaf2 = rtB .
d5qucfuocs + rtB . ixy4y4fia1 ; rtB . adfpvas4y1 = rtB . fxepc2xcpp / rtB .
n5hwfgbaf2 ; rtB . epghzivjmg = rtB . dl425wbmtl * rtB . ejeztrap5o ; rtB .
falxxywdu5 = rtB . ctkxwfjqo4 * rtB . hoqfdr02sr ; rtB . bzk10r0yza = rtB .
epghzivjmg - rtB . falxxywdu5 ; rtB . d3rx15nkcn = rtB . bzk10r0yza / rtB .
n5hwfgbaf2 ; rtB . mhgchbkh5s = rtB . cfzptmwazk + rtB . do4x3mpyxa ; rtB .
h0z3vive0a = rtB . nyn1hlelu1 * rtB . mhgchbkh5s ; rtB . opxeiyrrrv = rtB .
gwtv3svq4f + rtB . coehnzyatq ; rtB . mwnzw34142 = rtB . g3cditrnls * rtB .
opxeiyrrrv ; rtB . bzfieltzgo = rtB . h0z3vive0a + rtB . mwnzw34142 ; rtB .
alkhnbken2 = rtB . mhgchbkh5s * rtB . mhgchbkh5s ; rtB . allmugjfeq = rtB .
opxeiyrrrv * rtB . opxeiyrrrv ; rtB . p1pbe1trte = rtB . alkhnbken2 + rtB .
allmugjfeq ; rtB . g03pfnivhz = rtB . bzfieltzgo / rtB . p1pbe1trte ; rtB .
lqf4aq05vl = rtB . g3cditrnls * rtB . mhgchbkh5s ; rtB . b4xgm103fj = rtB .
nyn1hlelu1 * rtB . opxeiyrrrv ; rtB . hkxe0dcrb3 = rtB . lqf4aq05vl - rtB .
b4xgm103fj ; rtB . h5rckzkxqi = rtB . hkxe0dcrb3 / rtB . p1pbe1trte ; rtB .
jpyodorphz = rtB . cfzptmwazk + rtB . egfgjyyopt ; rtB . e1aeiuhvcu = rtB .
c5dodwizkd * rtB . jpyodorphz ; rtB . lfkbw1irgn = rtB . gwtv3svq4f + rtB .
oaxc3hrb45 ; rtB . ixklt4kvhj = rtB . pnlxv4w3fb * rtB . lfkbw1irgn ; rtB .
hzjpucb2gj = rtB . e1aeiuhvcu + rtB . ixklt4kvhj ; rtB . pzcqppraex = rtB .
jpyodorphz * rtB . jpyodorphz ; rtB . ir5ocnrusv = rtB . lfkbw1irgn * rtB .
lfkbw1irgn ; rtB . pqfbst2tsy = rtB . pzcqppraex + rtB . ir5ocnrusv ; rtB .
fxxgpo2esf = rtB . hzjpucb2gj / rtB . pqfbst2tsy ; rtB . o2ydv2ywzy = rtB .
pnlxv4w3fb * rtB . jpyodorphz ; rtB . kfuzbcvbzl = rtB . c5dodwizkd * rtB .
lfkbw1irgn ; rtB . ilafbrraqo = rtB . o2ydv2ywzy - rtB . kfuzbcvbzl ; rtB .
bgzphi1knr = rtB . ilafbrraqo / rtB . pqfbst2tsy ; rtB . mi0kjscaxs =
muDoubleScalarSin ( rtP . sinwt_Freq * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase ) * rtP . sinwt_Amp + rtP . sinwt_Bias ; rtB . nvoqqf2upc = rtB .
bmeofyowkm [ 0 ] * rtB . mi0kjscaxs ; rtB . mkgcucntmi = muDoubleScalarSin (
rtP . coswt_Freq * ssGetTaskTime ( rtS , 0 ) + rtP . coswt_Phase ) * rtP .
coswt_Amp + rtP . coswt_Bias ; rtB . lhomssayy1 = rtB . bmeofyowkm [ 0 ] *
rtB . mkgcucntmi ; rtB . cx2tw3ew51 = muDoubleScalarSin ( rtP .
sinwt_Freq_hw50vv4fuh * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_hghinjrt5e ) * rtP . sinwt_Amp_m353m31jj3 + rtP .
sinwt_Bias_j5zfckuupo ; rtB . cnrayaptfm = rtB . bmeofyowkm [ 1 ] * rtB .
cx2tw3ew51 ; rtB . gxsmkeiu0v = muDoubleScalarSin ( rtP .
coswt_Freq_ffqpvnxk5s * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_eaybejuvbf ) * rtP . coswt_Amp_gmtrem0mhg + rtP .
coswt_Bias_ncz5abircu ; rtB . kxzvknusul = rtB . bmeofyowkm [ 1 ] * rtB .
gxsmkeiu0v ; rtB . fqpooms1ri = muDoubleScalarSin ( rtP .
sinwt_Freq_irbg4n0lhx * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_d2pv1k4bgo ) * rtP . sinwt_Amp_kyu2z0oezu + rtP .
sinwt_Bias_nerautubgx ; rtB . bt5cg0l4bg = rtB . bmeofyowkm [ 2 ] * rtB .
fqpooms1ri ; rtB . ppkjzr42ko = muDoubleScalarSin ( rtP .
coswt_Freq_jfkotiwh03 * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_pfr55e0jjx ) * rtP . coswt_Amp_lqknf0cpy5 + rtP .
coswt_Bias_gm0d4ltm2a ; rtB . jyjlz3qjng = rtB . bmeofyowkm [ 2 ] * rtB .
ppkjzr42ko ; rtB . btufxxakak = muDoubleScalarSin ( rtP .
sinwt_Freq_mawt4kygws * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_gmnejpcsay ) * rtP . sinwt_Amp_n5ppui3oo0 + rtP .
sinwt_Bias_crxhy3i2ms ; rtB . ejz1w0mtlj = 0.0 * rtB . btufxxakak ; rtB .
bfsb0zjexf = muDoubleScalarSin ( rtP . coswt_Freq_mexg5soj0n * ssGetTaskTime
( rtS , 0 ) + rtP . coswt_Phase_aio1khmk2p ) * rtP . coswt_Amp_bjcnjzj40m +
rtP . coswt_Bias_e4ad3uhai1 ; rtB . hxcjlgtiv4 = 0.0 * rtB . bfsb0zjexf ; rtB
. kg3rio0iz3 = muDoubleScalarSin ( rtP . sinwt_Freq_azodpxcnk1 *
ssGetTaskTime ( rtS , 0 ) + rtP . sinwt_Phase_eqgyqi2lmk ) * rtP .
sinwt_Amp_gv1bb3vjuu + rtP . sinwt_Bias_pihyneinpb ; rtB . dkctjqa3dq = 0.0 *
rtB . kg3rio0iz3 ; rtB . mzt4mh2wjm = muDoubleScalarSin ( rtP .
coswt_Freq_njhfvfylh0 * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_lvxduh3as1 ) * rtP . coswt_Amp_f43p5kykpt + rtP .
coswt_Bias_j2axim5mgf ; rtB . fir35h1or5 = 0.0 * rtB . mzt4mh2wjm ; rtB .
puvpssqe2s = muDoubleScalarSin ( rtP . sinwt_Freq_kyrhf4mqil * ssGetTaskTime
( rtS , 0 ) + rtP . sinwt_Phase_j51l4miy0r ) * rtP . sinwt_Amp_iapouiprdd +
rtP . sinwt_Bias_a3vqgq0xcv ; rtB . m4sokjzatx = rtB . hqf4mnhqso [ 0 ] * rtB
. puvpssqe2s ; rtB . ik4s4bl0lu = muDoubleScalarSin ( rtP .
coswt_Freq_m5skmxmxcy * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_mls20e1bck ) * rtP . coswt_Amp_kudaubsnau + rtP .
coswt_Bias_gnn4qmzzoq ; rtB . nupattijm1 = rtB . hqf4mnhqso [ 0 ] * rtB .
ik4s4bl0lu ; rtB . ggaxsfaosu = muDoubleScalarSin ( rtP .
sinwt_Freq_l5mjwace0n * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_ileflkqcyv ) * rtP . sinwt_Amp_hsmq2iulno + rtP .
sinwt_Bias_jqczfm5bbl ; rtB . ga110lqx5z = rtB . hqf4mnhqso [ 0 ] * rtB .
ggaxsfaosu ; rtB . ff3exya00x = muDoubleScalarSin ( rtP .
coswt_Freq_e3lsls4s2t * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_ifu0arp13r ) * rtP . coswt_Amp_anp5vrtcrq + rtP .
coswt_Bias_ns5grojo3n ; rtB . ie1qostli2 = rtB . hqf4mnhqso [ 0 ] * rtB .
ff3exya00x ; rtB . pvfae3ezwt = muDoubleScalarSin ( rtP .
sinwt_Freq_frasmq3yuj * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_jnmnupgcxo ) * rtP . sinwt_Amp_i4ppcbnwdl + rtP .
sinwt_Bias_hgjq13wkoq ; rtB . mb3ddccayl = rtB . hqf4mnhqso [ 0 ] * rtB .
pvfae3ezwt ; rtB . n3ym03f110 = muDoubleScalarSin ( rtP .
coswt_Freq_hxf5wwsema * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_pxs0u4vca5 ) * rtP . coswt_Amp_jtfpay3nk4 + rtP .
coswt_Bias_ole3l3offb ; rtB . iqa3f5efos = rtB . hqf4mnhqso [ 0 ] * rtB .
n3ym03f110 ; rtB . ko5y4amvrd = muDoubleScalarSin ( rtP .
sinwt_Freq_eb1eweasil * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_jqbmunangj ) * rtP . sinwt_Amp_firl1o4wjc + rtP .
sinwt_Bias_dc5n5s5q0w ; rtB . kgiixo3yn5 = rtB . hqf4mnhqso [ 1 ] * rtB .
ko5y4amvrd ; rtB . lxfesnjhku = muDoubleScalarSin ( rtP .
coswt_Freq_afbihnbfvc * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_m2evvsz4kv ) * rtP . coswt_Amp_j1cfjbwir5 + rtP .
coswt_Bias_besstr11dt ; rtB . lcu3nftu1f = rtB . hqf4mnhqso [ 1 ] * rtB .
lxfesnjhku ; rtB . kx2den0v5l = muDoubleScalarSin ( rtP .
sinwt_Freq_guj5u15aze * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_j2d2ndbeth ) * rtP . sinwt_Amp_b4k31c0wn4 + rtP .
sinwt_Bias_oczdmbfngi ; rtB . bow54nzha3 = rtB . hqf4mnhqso [ 1 ] * rtB .
kx2den0v5l ; rtB . j0natwynsc = muDoubleScalarSin ( rtP .
coswt_Freq_kkbrg0gv2o * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_aou5t52lct ) * rtP . coswt_Amp_p0spbzihvi + rtP .
coswt_Bias_bmzyvzwxzx ; rtB . bcobobx5fo = rtB . hqf4mnhqso [ 1 ] * rtB .
j0natwynsc ; rtB . njb0hitwa1 = muDoubleScalarSin ( rtP .
sinwt_Freq_jbhgdeqyfx * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_dbm3uwmlqx ) * rtP . sinwt_Amp_peodfliqk1 + rtP .
sinwt_Bias_iixhbmptvp ; rtB . irurbxken0 = rtB . hqf4mnhqso [ 1 ] * rtB .
njb0hitwa1 ; rtB . op3piplkum = muDoubleScalarSin ( rtP .
coswt_Freq_apchsiiw1z * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_fo352wqjb1 ) * rtP . coswt_Amp_jltdbs0too + rtP .
coswt_Bias_lh4p2s10k1 ; rtB . clqrqgqr4k = rtB . hqf4mnhqso [ 1 ] * rtB .
op3piplkum ; rtB . o1yzh4z3jh = muDoubleScalarSin ( rtP .
sinwt_Freq_f5ndwuiumn * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_hxd5tnlq3l ) * rtP . sinwt_Amp_hb1i0yjkjg + rtP .
sinwt_Bias_newgfm01wa ; rtB . hnvusovzin = rtB . hqf4mnhqso [ 2 ] * rtB .
o1yzh4z3jh ; rtB . bezxswalh1 = muDoubleScalarSin ( rtP .
coswt_Freq_cdqncjgvsq * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_otijp4wyta ) * rtP . coswt_Amp_jinoxoc353 + rtP .
coswt_Bias_j4ntxlyepj ; rtB . kgap4iwtv4 = rtB . hqf4mnhqso [ 2 ] * rtB .
bezxswalh1 ; rtB . fxjosnmcfv = muDoubleScalarSin ( rtP .
sinwt_Freq_mxns0nelix * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_dotml00nbn ) * rtP . sinwt_Amp_mbxlb5fr3u + rtP .
sinwt_Bias_bh3dqyzdif ; rtB . aesfinetlj = rtB . hqf4mnhqso [ 2 ] * rtB .
fxjosnmcfv ; rtB . bs3ragztwv = muDoubleScalarSin ( rtP .
coswt_Freq_jfsql15rwo * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_c5irlyaeq0 ) * rtP . coswt_Amp_nupbzjsoc2 + rtP .
coswt_Bias_edgtphozfo ; rtB . kibaejq5te = rtB . hqf4mnhqso [ 2 ] * rtB .
bs3ragztwv ; rtB . kpy5unu4yh = muDoubleScalarSin ( rtP .
sinwt_Freq_iqpbnie2xj * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_lp1wjbft3x ) * rtP . sinwt_Amp_ikldopw2p4 + rtP .
sinwt_Bias_kmng1zdjya ; rtB . ohyn2fvrkm = rtB . hqf4mnhqso [ 2 ] * rtB .
kpy5unu4yh ; rtB . n1rxs0shfw = muDoubleScalarSin ( rtP .
coswt_Freq_jvqfkzeebg * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_adfipykaog ) * rtP . coswt_Amp_ivirijymqe + rtP .
coswt_Bias_ankwoilzmy ; rtB . gv1th4qcwu = rtB . hqf4mnhqso [ 2 ] * rtB .
n1rxs0shfw ; rtB . b1c3s0yd1z = rtX . ouay2vk4ob ; { real_T * * uBuffer = (
real_T * * ) & rtDW . ocixwzdlgq . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & rtDW . ocixwzdlgq . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_klfp210l4z ; rtB . nvenjfp0nr = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . fcxoxl4xkw .
CircularBufSize , & rtDW . fcxoxl4xkw . Last , rtDW . fcxoxl4xkw . Tail ,
rtDW . fcxoxl4xkw . Head , rtP . TransportDelay_InitOutput_e1patchl4a , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . jztdyeh4vd = ssGetT ( rtS ) ; rtB . ntztm1t4pe
= ( rtB . jztdyeh4vd >= rtP . K1_Value_lai2tl2gk3 ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . jxg2xoqtak = rtDW . fecdn2rq0x ; } if ( rtB .
ntztm1t4pe ) { rtB . cvw52fjwl2 = rtB . b1c3s0yd1z - rtB . nvenjfp0nr ; rtB .
odbmkac5ul = rtP . Gain_Gain_iyn4yp4vug * rtB . cvw52fjwl2 ; rtB . khrnej05te
= rtB . odbmkac5ul ; } else { rtB . khrnej05te = rtB . jxg2xoqtak ; } rtB .
nvxcpentgh = rtX . l4w5fqeack ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. j24stv55k5 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . j24stv55k5 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_kds3wvorrd ; rtB .
ouqtqneo0o = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . bzqpi0kjrz . CircularBufSize , & rtDW . bzqpi0kjrz . Last , rtDW .
bzqpi0kjrz . Tail , rtDW . bzqpi0kjrz . Head , rtP .
TransportDelay_InitOutput_guedmf10ob , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
ickfddkf0g = ssGetT ( rtS ) ; rtB . ox0wsz3ry3 = ( rtB . ickfddkf0g >= rtP .
K1_Value_f1hriybll1 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
fwzsm5a1ht = rtDW . am13fzyssx ; } if ( rtB . ox0wsz3ry3 ) { rtB . jdv43z4nyd
= rtB . nvxcpentgh - rtB . ouqtqneo0o ; rtB . l4ybh2exy4 = rtP .
Gain_Gain_mvgodosicm * rtB . jdv43z4nyd ; rtB . lhj0rg1wlh = rtB . l4ybh2exy4
; } else { rtB . lhj0rg1wlh = rtB . fwzsm5a1ht ; } rtB . p4fr4qns54 . re =
rtB . khrnej05te ; rtB . p4fr4qns54 . im = rtB . lhj0rg1wlh ; rtB .
f0aycpbdxx = muDoubleScalarHypot ( rtB . p4fr4qns54 . re , rtB . p4fr4qns54 .
im ) ; rtB . jy5lwoqboy = muDoubleScalarAtan2 ( rtB . p4fr4qns54 . im , rtB .
p4fr4qns54 . re ) ; rtB . l34by0f0gq = rtP . RadDeg_Gain_j5dgyw4eu3 * rtB .
jy5lwoqboy ; rtB . cqypayq4kb = rtP . degrad_Gain_p1mv0vjzql * rtB .
l34by0f0gq ; maxV = rtB . cqypayq4kb ; u0 = rtB . f0aycpbdxx ; u2 =
muDoubleScalarCos ( maxV ) ; maxV = muDoubleScalarSin ( maxV ) ; rtB .
n4jolavsi0 . re = u0 * u2 ; rtB . n4jolavsi0 . im = u0 * maxV ; rtB .
i24kygikwy = rtX . dbk3vhy4fa ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. hoiximvd01 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . hoiximvd01 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_hpj5c34w2y ; rtB .
ivw2zwnegi = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . p5bf4c2tu4 . CircularBufSize , & rtDW . p5bf4c2tu4 . Last , rtDW .
p5bf4c2tu4 . Tail , rtDW . p5bf4c2tu4 . Head , rtP .
TransportDelay_InitOutput_f3pcmrh0c0 , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
bwosbwc51l = ssGetT ( rtS ) ; rtB . dshsp0zqx4 = ( rtB . bwosbwc51l >= rtP .
K1_Value_lab0npzsgi ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
nzdmerrpjh = rtDW . amun1qc2sg ; } if ( rtB . dshsp0zqx4 ) { rtB . jltkq3zppf
= rtB . i24kygikwy - rtB . ivw2zwnegi ; rtB . dz4ooqd4ow = rtP .
Gain_Gain_io1bs5y4og * rtB . jltkq3zppf ; rtB . c4z4t2mhqf = rtB . dz4ooqd4ow
; } else { rtB . c4z4t2mhqf = rtB . nzdmerrpjh ; } rtB . h4utvvvw0w = rtX .
bwyxfbkejz ; { real_T * * uBuffer = ( real_T * * ) & rtDW . gipv43m3ie .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & rtDW . gipv43m3ie
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ; real_T tMinusDelay =
simTime - rtP . TransportDelay_Delay_kekk22xqvx ; rtB . kvs5qzqv3i =
rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW .
ekzddpzsi1 . CircularBufSize , & rtDW . ekzddpzsi1 . Last , rtDW . ekzddpzsi1
. Tail , rtDW . ekzddpzsi1 . Head , rtP .
TransportDelay_InitOutput_mvxvcm2f5o , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
gxmrwxqfgx = ssGetT ( rtS ) ; rtB . iuyckdpoop = ( rtB . gxmrwxqfgx >= rtP .
K1_Value_ovyhkyk4o3 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
j3pedaaiga = rtDW . dk54ih4kpw ; } if ( rtB . iuyckdpoop ) { rtB . c1qto1mdbj
= rtB . h4utvvvw0w - rtB . kvs5qzqv3i ; rtB . o2zbeayaag = rtP .
Gain_Gain_f25a3hro0o * rtB . c1qto1mdbj ; rtB . ekpfykko11 = rtB . o2zbeayaag
; } else { rtB . ekpfykko11 = rtB . j3pedaaiga ; } rtB . j0crhu5vhx . re =
rtB . c4z4t2mhqf ; rtB . j0crhu5vhx . im = rtB . ekpfykko11 ; rtB .
mcqyqasynq = muDoubleScalarHypot ( rtB . j0crhu5vhx . re , rtB . j0crhu5vhx .
im ) ; rtB . me5lfbg4xr = muDoubleScalarAtan2 ( rtB . j0crhu5vhx . im , rtB .
j0crhu5vhx . re ) ; rtB . nlotkknhtr = rtP . RadDeg_Gain_cxqp4cnau5 * rtB .
me5lfbg4xr ; rtB . nccz3hylur = rtP . degrad1_Gain_aagv12bl3e * rtB .
nlotkknhtr ; maxV = rtB . nccz3hylur ; u0 = rtB . mcqyqasynq ; u2 =
muDoubleScalarCos ( maxV ) ; maxV = muDoubleScalarSin ( maxV ) ; rtB .
nry5430qvz . re = u0 * u2 ; rtB . nry5430qvz . im = u0 * maxV ; rtB .
kwhlwb5uqn = rtX . cvhc14d1wd ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. n450lruhp3 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . n450lruhp3 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_jartwkytza ; rtB .
bzvqg5vv1f = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . hz4fywtkdv . CircularBufSize , & rtDW . hz4fywtkdv . Last , rtDW .
hz4fywtkdv . Tail , rtDW . hz4fywtkdv . Head , rtP .
TransportDelay_InitOutput_n5zytx3f40 , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
bsnrvhn1x0 = ssGetT ( rtS ) ; rtB . d0vg5nnvjl = ( rtB . bsnrvhn1x0 >= rtP .
K1_Value_hntvvucdz4 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
gahf44lzby = rtDW . loaprl0bph ; } if ( rtB . d0vg5nnvjl ) { rtB . gggxjij2pc
= rtB . kwhlwb5uqn - rtB . bzvqg5vv1f ; rtB . a0nbablzu3 = rtP .
Gain_Gain_ns0o5w42x2 * rtB . gggxjij2pc ; rtB . oiykawjblz = rtB . a0nbablzu3
; } else { rtB . oiykawjblz = rtB . gahf44lzby ; } rtB . pdx0x2ahct = rtX .
ipmtswi0si ; { real_T * * uBuffer = ( real_T * * ) & rtDW . ptmzalvxqw .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & rtDW . ptmzalvxqw
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ; real_T tMinusDelay =
simTime - rtP . TransportDelay_Delay_nc4nuquf3r ; rtB . lbpluzw41s =
rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW .
clbwz4dwil . CircularBufSize , & rtDW . clbwz4dwil . Last , rtDW . clbwz4dwil
. Tail , rtDW . clbwz4dwil . Head , rtP .
TransportDelay_InitOutput_awceswb44n , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
cgyvefqgtz = ssGetT ( rtS ) ; rtB . ful5xza33x = ( rtB . cgyvefqgtz >= rtP .
K1_Value_hbo0t3qqvd ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
kbbs5w33ph = rtDW . fo5lr1tooi ; } if ( rtB . ful5xza33x ) { rtB . f4ciisrxj4
= rtB . pdx0x2ahct - rtB . lbpluzw41s ; rtB . exp1u3sa4m = rtP .
Gain_Gain_htyry5mtho * rtB . f4ciisrxj4 ; rtB . hatthunapb = rtB . exp1u3sa4m
; } else { rtB . hatthunapb = rtB . kbbs5w33ph ; } rtB . hxm2idhbhg . re =
rtB . oiykawjblz ; rtB . hxm2idhbhg . im = rtB . hatthunapb ; rtB .
jwtzuz4vid = muDoubleScalarHypot ( rtB . hxm2idhbhg . re , rtB . hxm2idhbhg .
im ) ; rtB . f24b5x0xbi = muDoubleScalarAtan2 ( rtB . hxm2idhbhg . im , rtB .
hxm2idhbhg . re ) ; rtB . k3a1n11llq = rtP . RadDeg_Gain_i1rtkuhb4y * rtB .
f24b5x0xbi ; rtB . b0fkogzawg = rtP . degrad2_Gain_nzul4uh3dh * rtB .
k3a1n11llq ; maxV = rtB . b0fkogzawg ; u0 = rtB . jwtzuz4vid ; u2 =
muDoubleScalarCos ( maxV ) ; maxV = muDoubleScalarSin ( maxV ) ; rtB .
i2tb4wqggw . re = u0 * u2 ; rtB . i2tb4wqggw . im = u0 * maxV ; jvoeh2x3fa (
rtS , rtP . Constant_Value_ikb0c1wpdt , rtB . n4jolavsi0 , rtB . nry5430qvz ,
rtB . i2tb4wqggw , & rtB . mb5y4fcaem , & rtDW . mb5y4fcaem , & rtP .
mb5y4fcaem ) ; jvoeh2x3fa ( rtS , rtP . Constant1_Value_mlncsarxbq , rtB .
n4jolavsi0 , rtB . nry5430qvz , rtB . i2tb4wqggw , & rtB . no1v55ban3 , &
rtDW . no1v55ban3 , & rtP . no1v55ban3 ) ; czm0w3hmwh ( rtS , rtP .
Constant2_Value_omskwcx03k , rtB . n4jolavsi0 , rtB . nry5430qvz , rtB .
i2tb4wqggw , & rtB . pgdbd0g5cb , & rtDW . pgdbd0g5cb , & rtP . pgdbd0g5cb )
; rtB . jqkabf2cle [ 0 ] = muDoubleScalarHypot ( rtB . mb5y4fcaem .
hjcfumkc1i . re , rtB . mb5y4fcaem . hjcfumkc1i . im ) ; rtB . jqkabf2cle [ 1
] = muDoubleScalarHypot ( rtB . no1v55ban3 . hjcfumkc1i . re , rtB .
no1v55ban3 . hjcfumkc1i . im ) ; rtB . jqkabf2cle [ 2 ] = muDoubleScalarHypot
( rtB . pgdbd0g5cb . fnsdbz5hxw . re , rtB . pgdbd0g5cb . fnsdbz5hxw . im ) ;
rtB . neqcoyw4hd [ 0 ] = muDoubleScalarAtan2 ( rtB . mb5y4fcaem . hjcfumkc1i
. im , rtB . mb5y4fcaem . hjcfumkc1i . re ) ; rtB . neqcoyw4hd [ 1 ] =
muDoubleScalarAtan2 ( rtB . no1v55ban3 . hjcfumkc1i . im , rtB . no1v55ban3 .
hjcfumkc1i . re ) ; rtB . neqcoyw4hd [ 2 ] = muDoubleScalarAtan2 ( rtB .
pgdbd0g5cb . fnsdbz5hxw . im , rtB . pgdbd0g5cb . fnsdbz5hxw . re ) ; rtB .
kqh0pwgzfr = rtP . raddeg1_Gain_mbfz0ha3ew * rtB . neqcoyw4hd [ 2 ] ; rtB .
j3jv3heq3m = rtP . Gain1_Gain_nobxxgd500 * rtB . kqh0pwgzfr ; rtB .
m5rzn25hom = muDoubleScalarSin ( rtB . j3jv3heq3m ) ; rtB . fbgdlr24qp =
muDoubleScalarCos ( rtB . j3jv3heq3m ) ; rtB . m0awmyvk2a = rtP .
Gain4_Gain_nxkfoxzs4t * rtB . jqkabf2cle [ 2 ] ; rtB . kis4k5y20r = rtB .
m5rzn25hom * rtB . m0awmyvk2a ; rtB . ddx00mewbj = rtB . m0awmyvk2a * rtB .
fbgdlr24qp ; rtB . jskbmf1jkj = muDoubleScalarSin ( rtP .
sinwt_Freq_okpqmfdf2w * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_lgzo0uqv1s ) * rtP . sinwt_Amp_pgstqnj01h + rtP .
sinwt_Bias_icflbu2d20 ; rtB . f3z3rdoc5x = rtB . bmeofyowkm [ 0 ] * rtB .
jskbmf1jkj ; rtB . j1y2j5ahfz = muDoubleScalarSin ( rtP .
coswt_Freq_cdip1s1kqs * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_prmtboi3nk ) * rtP . coswt_Amp_mmf1qdw11g + rtP .
coswt_Bias_j4wqqyhqud ; rtB . m0niy4vc14 = rtB . bmeofyowkm [ 0 ] * rtB .
j1y2j5ahfz ; rtB . l1ubjdjmch = muDoubleScalarSin ( rtP .
sinwt_Freq_em2vu51mtw * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_mls3rw5amn ) * rtP . sinwt_Amp_gwoun2e2at + rtP .
sinwt_Bias_itau0lsxbz ; rtB . n3lyabdxz3 = rtB . bmeofyowkm [ 1 ] * rtB .
l1ubjdjmch ; rtB . mk1ha43vae = muDoubleScalarSin ( rtP .
coswt_Freq_gnrysmy5r3 * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_kz4keltlvl ) * rtP . coswt_Amp_jogr1lq5zf + rtP .
coswt_Bias_ok0zulummz ; rtB . p2kd0bqbmj = rtB . bmeofyowkm [ 1 ] * rtB .
mk1ha43vae ; rtB . lmy2ggrpns = muDoubleScalarSin ( rtP .
sinwt_Freq_dcypgh50oc * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_am2wyvp0gc ) * rtP . sinwt_Amp_gpp1rybwwh + rtP .
sinwt_Bias_fwwgwi4evc ; rtB . fobm5wf0a2 = rtB . bmeofyowkm [ 2 ] * rtB .
lmy2ggrpns ; rtB . ehf4xjxkse = muDoubleScalarSin ( rtP .
coswt_Freq_aprj4q5ooy * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_hxtqsbfdbh ) * rtP . coswt_Amp_ejtffcesf0 + rtP .
coswt_Bias_j5ptusnn51 ; rtB . fpnuiuit1d = rtB . bmeofyowkm [ 2 ] * rtB .
ehf4xjxkse ; rtB . espnteaje4 = rtB . ddx00mewbj * rtB . n01bgh2bvn ; rtB .
gm2yugnkm2 = rtB . kis4k5y20r * rtB . onz1gvrvhg ; rtB . id5eetvrsa = rtB .
espnteaje4 + rtB . gm2yugnkm2 ; rtB . ow3cnyl5zm = rtB . n01bgh2bvn * rtB .
n01bgh2bvn ; rtB . jab2kmu2hp = rtB . onz1gvrvhg * rtB . onz1gvrvhg ; rtB .
leckopukfn = rtB . ow3cnyl5zm + rtB . jab2kmu2hp ; rtB . bzllth2vgo = rtB .
kis4k5y20r * rtB . n01bgh2bvn ; rtB . fxh4xlods3 = rtB . ddx00mewbj * rtB .
onz1gvrvhg ; rtB . nu3ac0hf5r = rtB . bzllth2vgo - rtB . fxh4xlods3 ; rtB .
hz2ue4ftay = rtB . id5eetvrsa / rtB . leckopukfn ; rtB . lcuvrwquya = rtB .
nu3ac0hf5r / rtB . leckopukfn ; rtB . gnk5kwrjzz = muDoubleScalarSin ( rtP .
sinwt_Freq_plsithp0mp * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_f5twymsxzm ) * rtP . sinwt_Amp_juhepcobvg + rtP .
sinwt_Bias_hs1yzs4vri ; rtB . l1mnh3vw14 = rtB . hqf4mnhqso [ 0 ] * rtB .
gnk5kwrjzz ; rtB . crme0i1u55 = muDoubleScalarSin ( rtP .
coswt_Freq_lsp5scr5xu * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_f3ojpmc3zn ) * rtP . coswt_Amp_jmzyh35nwp + rtP .
coswt_Bias_mvegyj1dzt ; rtB . ge0vbj12o1 = rtB . hqf4mnhqso [ 0 ] * rtB .
crme0i1u55 ; rtB . k4qimnucjg = muDoubleScalarSin ( rtP .
sinwt_Freq_icfq4aazpt * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_icpuekp1g2 ) * rtP . sinwt_Amp_jzsiytndrh + rtP .
sinwt_Bias_njiq433bkn ; rtB . pfmt42o2gw = rtB . hqf4mnhqso [ 1 ] * rtB .
k4qimnucjg ; rtB . dysjdqxhwd = muDoubleScalarSin ( rtP .
coswt_Freq_dp3vez03yh * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_gdqxrjc02b ) * rtP . coswt_Amp_kubpjqpudy + rtP .
coswt_Bias_azevn4xle2 ; rtB . dpeoqqz1p4 = rtB . hqf4mnhqso [ 1 ] * rtB .
dysjdqxhwd ; rtB . mpiz4j4yj2 = muDoubleScalarSin ( rtP .
sinwt_Freq_n3vipc1bws * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_glq1kqv5p5 ) * rtP . sinwt_Amp_lsxyruwtc2 + rtP .
sinwt_Bias_blc3waapzg ; rtB . l4rs54j5xg = rtB . hqf4mnhqso [ 2 ] * rtB .
mpiz4j4yj2 ; rtB . dh3ho2cvxn = muDoubleScalarSin ( rtP .
coswt_Freq_p1ejsntzzp * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_leuxbgyef0 ) * rtP . coswt_Amp_prp4e3fgst + rtP .
coswt_Bias_nfccroify1 ; rtB . b34qhbnabh = rtB . hqf4mnhqso [ 2 ] * rtB .
dh3ho2cvxn ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { } rtB . gepoa204a5 =
muDoubleScalarSin ( rtB . l5lnbu5gps [ 0 ] ) ; rtB . o4mfnq0eo2 =
muDoubleScalarCos ( rtB . l5lnbu5gps [ 0 ] ) ; rtB . baroi4q3qd = rtB .
gepoa204a5 * rtB . bzh5umwrxf [ 0 ] ; rtB . fipekfgd1i = rtB . bzh5umwrxf [ 0
] * rtB . o4mfnq0eo2 ; rtB . lwzxxxqcun = muDoubleScalarSin ( rtB .
l5lnbu5gps [ 7 ] ) ; rtB . fa1kgu3ovm = muDoubleScalarCos ( rtB . l5lnbu5gps
[ 7 ] ) ; rtB . nrnpnwwq31 = rtB . lwzxxxqcun * rtB . bzh5umwrxf [ 7 ] ; rtB
. cu4nrloglz = rtB . bzh5umwrxf [ 7 ] * rtB . fa1kgu3ovm ; rtB . b1ldl31fed =
muDoubleScalarSin ( rtB . l5lnbu5gps [ 1 ] ) ; rtB . fifw3ie142 =
muDoubleScalarCos ( rtB . l5lnbu5gps [ 1 ] ) ; rtB . dktcpt50pn = rtB .
b1ldl31fed * rtB . bzh5umwrxf [ 1 ] ; rtB . hgxwusxbgy = rtB . bzh5umwrxf [ 1
] * rtB . fifw3ie142 ; rtB . d2b1qrvjvc = muDoubleScalarSin ( rtB .
l5lnbu5gps [ 8 ] ) ; rtB . ijyfwt34ks = muDoubleScalarCos ( rtB . l5lnbu5gps
[ 8 ] ) ; rtB . mswejlvkx1 = rtB . d2b1qrvjvc * rtB . bzh5umwrxf [ 8 ] ; rtB
. lpcqlloa3r = rtB . bzh5umwrxf [ 8 ] * rtB . ijyfwt34ks ; rtB . mfyunauug4 =
muDoubleScalarSin ( rtB . l5lnbu5gps [ 2 ] ) ; rtB . ldzodrjrm0 =
muDoubleScalarCos ( rtB . l5lnbu5gps [ 2 ] ) ; rtB . cuggjv2ise = rtB .
mfyunauug4 * rtB . bzh5umwrxf [ 2 ] ; rtB . lmcprjcgyb = rtB . bzh5umwrxf [ 2
] * rtB . ldzodrjrm0 ; rtB . ahtvk15dyl = muDoubleScalarSin ( rtB .
l5lnbu5gps [ 9 ] ) ; rtB . pqcgx2d3df = muDoubleScalarCos ( rtB . l5lnbu5gps
[ 9 ] ) ; rtB . f1dgsmei2s = rtB . ahtvk15dyl * rtB . bzh5umwrxf [ 9 ] ; rtB
. ck1iml4hal = rtB . bzh5umwrxf [ 9 ] * rtB . pqcgx2d3df ; rtB . gumm3ad2qq =
rtB . fipekfgd1i - rtB . hgxwusxbgy ; rtB . ismrcvqbc1 = rtB . baroi4q3qd -
rtB . dktcpt50pn ; rtB . d10t5ubi0n = rtB . lnv3d3rvor - rtB . pjxmqrmerd ;
rtB . kmxo351ofr = rtB . chvsplbxtl - rtB . koph4w1vnf ; rtB . hmmwi4fpci =
rtB . pjxmqrmerd - rtB . i5dpx5t34o ; rtB . ijsenoio2w = rtB . koph4w1vnf -
rtB . jlt5ytbte0 ; rtB . kjh1mq1x2n = rtB . hgxwusxbgy - rtB . lmcprjcgyb ;
rtB . meondie1kt = rtB . dktcpt50pn - rtB . cuggjv2ise ; rtB . bv0ls4swxe =
rtB . i5dpx5t34o - rtB . lnv3d3rvor ; rtB . fpcv52xyib = rtB . jlt5ytbte0 -
rtB . chvsplbxtl ; rtB . idvzinyjb0 = rtB . lmcprjcgyb - rtB . fipekfgd1i ;
rtB . ckxlaknjhr = rtB . cuggjv2ise - rtB . baroi4q3qd ; rtB . jqyecaxq4q =
rtB . kjh1mq1x2n * rtB . bv0ls4swxe ; rtB . ew5vmsbygg = rtB . meondie1kt *
rtB . fpcv52xyib ; rtB . prlh0kv4do = rtB . jqyecaxq4q + rtB . ew5vmsbygg ;
rtB . lvtdrb1qxv = rtB . bv0ls4swxe * rtB . bv0ls4swxe ; rtB . ipzmeuwvyr =
rtB . fpcv52xyib * rtB . fpcv52xyib ; rtB . dcweexcft0 = rtB . lvtdrb1qxv +
rtB . ipzmeuwvyr ; rtB . g40txzlmpc = rtB . meondie1kt * rtB . bv0ls4swxe ;
rtB . pfn1l02lx5 = rtB . kjh1mq1x2n * rtB . fpcv52xyib ; rtB . but4mryczt =
rtB . g40txzlmpc - rtB . pfn1l02lx5 ; rtB . in4jqa5afj = rtB . prlh0kv4do /
rtB . dcweexcft0 ; rtB . cnvmdift1w = rtB . but4mryczt / rtB . dcweexcft0 ;
rtB . avieuwwm3o = rtB . gumm3ad2qq * rtB . hmmwi4fpci ; rtB . nviulahbnp =
rtB . ismrcvqbc1 * rtB . ijsenoio2w ; rtB . bksikaiipm = rtB . avieuwwm3o +
rtB . nviulahbnp ; rtB . fk451yvbi3 = rtB . hmmwi4fpci * rtB . hmmwi4fpci ;
rtB . fss3lb1zjr = rtB . ijsenoio2w * rtB . ijsenoio2w ; rtB . kapwqusieq =
rtB . fk451yvbi3 + rtB . fss3lb1zjr ; rtB . lt1sypjclg = rtB . ismrcvqbc1 *
rtB . hmmwi4fpci ; rtB . gs2f4muprt = rtB . gumm3ad2qq * rtB . ijsenoio2w ;
rtB . huzylabuhb = rtB . lt1sypjclg - rtB . gs2f4muprt ; rtB . kqbuibgwpb =
rtB . bksikaiipm / rtB . kapwqusieq ; rtB . goghnkjc0s = rtB . huzylabuhb /
rtB . kapwqusieq ; rtB . bfoztqrbr2 = rtB . idvzinyjb0 * rtB . d10t5ubi0n ;
rtB . hithztqk1n = rtB . ckxlaknjhr * rtB . kmxo351ofr ; rtB . op3zi3tq51 =
rtB . bfoztqrbr2 + rtB . hithztqk1n ; rtB . f3uvwtnnm2 = rtB . d10t5ubi0n *
rtB . d10t5ubi0n ; rtB . c4x44rk00c = rtB . kmxo351ofr * rtB . kmxo351ofr ;
rtB . k054mckgxw = rtB . f3uvwtnnm2 + rtB . c4x44rk00c ; rtB . nh2zuzz4dc =
rtB . ckxlaknjhr * rtB . d10t5ubi0n ; rtB . im0ialcnv5 = rtB . idvzinyjb0 *
rtB . kmxo351ofr ; rtB . c3peuq04ry = rtB . nh2zuzz4dc - rtB . im0ialcnv5 ;
rtB . bfchbarrr2 = rtB . op3zi3tq51 / rtB . k054mckgxw ; rtB . jjf0dsq00o =
rtB . c3peuq04ry / rtB . k054mckgxw ; rtB . nxn02am0kd = rtX . eczymfqr20 ; {
real_T * * uBuffer = ( real_T * * ) & rtDW . akp2w04qvg . TUbufferPtrs [ 0 ]
; real_T * * tBuffer = ( real_T * * ) & rtDW . akp2w04qvg . TUbufferPtrs [ 1
] ; real_T simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_dunuuw4awi ; rtB . djbd3zqibn = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . bj3l3j2st3 .
CircularBufSize , & rtDW . bj3l3j2st3 . Last , rtDW . bj3l3j2st3 . Tail ,
rtDW . bj3l3j2st3 . Head , rtP . TransportDelay_InitOutput_aw5k4laehm , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . a3aiwbtu0e = ssGetT ( rtS ) ; rtB . m5rxijy4ry
= ( rtB . a3aiwbtu0e >= rtP . K1_Value_ebw4pantvk ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . colke5y0at = rtDW . g0siwihohw ; } if ( rtB .
m5rxijy4ry ) { rtB . e55eohvmqi = rtB . nxn02am0kd - rtB . djbd3zqibn ; rtB .
c45oftbprf = rtP . Gain_Gain_fc4uwpsai2 * rtB . e55eohvmqi ; rtB . mv5v4tmemn
= rtB . c45oftbprf ; } else { rtB . mv5v4tmemn = rtB . colke5y0at ; } rtB .
nyigkdebdw = rtX . lobhhamyoj ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. n50ive3hhm . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . n50ive3hhm . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_eaoxoc1w3r ; rtB .
n3hpizpwxo = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . fdisg0vnhk . CircularBufSize , & rtDW . fdisg0vnhk . Last , rtDW .
fdisg0vnhk . Tail , rtDW . fdisg0vnhk . Head , rtP .
TransportDelay_InitOutput_a23qsuusmp , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
hrh0skrvpa = ssGetT ( rtS ) ; rtB . pkjw0kemwk = ( rtB . hrh0skrvpa >= rtP .
K1_Value_iiwn3tnsty ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
adi4zoov20 = rtDW . atub5wqmcj ; } if ( rtB . pkjw0kemwk ) { rtB . pgvpu2wpk2
= rtB . nyigkdebdw - rtB . n3hpizpwxo ; rtB . frhzaugzmo = rtP .
Gain_Gain_bizydeqqc5 * rtB . pgvpu2wpk2 ; rtB . nercvrdumc = rtB . frhzaugzmo
; } else { rtB . nercvrdumc = rtB . adi4zoov20 ; } rtB . ip5ym03lnn . re =
rtB . mv5v4tmemn ; rtB . ip5ym03lnn . im = rtB . nercvrdumc ; rtB .
fpctj2ax3l = muDoubleScalarHypot ( rtB . ip5ym03lnn . re , rtB . ip5ym03lnn .
im ) ; rtB . mvcedwxrvg = muDoubleScalarAtan2 ( rtB . ip5ym03lnn . im , rtB .
ip5ym03lnn . re ) ; rtB . kipifflj2d = rtP . RadDeg_Gain_m1ivkfpcdf * rtB .
mvcedwxrvg ; rtB . prmblnuw22 = rtP . degrad_Gain_i3f4rqhwgs * rtB .
kipifflj2d ; maxV = rtB . prmblnuw22 ; u0 = rtB . fpctj2ax3l ; u2 =
muDoubleScalarCos ( maxV ) ; maxV = muDoubleScalarSin ( maxV ) ; rtB .
ogkcemo3dw . re = u0 * u2 ; rtB . ogkcemo3dw . im = u0 * maxV ; rtB .
bmvqo4bjiq = rtX . kenbvb3y0s ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. ej00tpn0xt . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . ej00tpn0xt . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_ewi0elycaa ; rtB .
bo42iuxrb5 = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . nvxugce5ur . CircularBufSize , & rtDW . nvxugce5ur . Last , rtDW .
nvxugce5ur . Tail , rtDW . nvxugce5ur . Head , rtP .
TransportDelay_InitOutput_fgetemlf4v , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
fqdt4grcek = ssGetT ( rtS ) ; rtB . b4yt0gq5iy = ( rtB . fqdt4grcek >= rtP .
K1_Value_k44ztwvgxh ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
eyjyv0tts4 = rtDW . kzk400d43v ; } if ( rtB . b4yt0gq5iy ) { rtB . jbwykihb1e
= rtB . bmvqo4bjiq - rtB . bo42iuxrb5 ; rtB . jvzvrqwxc4 = rtP .
Gain_Gain_cqvs4sii1f * rtB . jbwykihb1e ; rtB . fhgutxj4v5 = rtB . jvzvrqwxc4
; } else { rtB . fhgutxj4v5 = rtB . eyjyv0tts4 ; } rtB . plbwcbylcg = rtX .
nsp4pw5vpk ; { real_T * * uBuffer = ( real_T * * ) & rtDW . ilhkq3otms .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & rtDW . ilhkq3otms
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ; real_T tMinusDelay =
simTime - rtP . TransportDelay_Delay_kexin5mfwd ; rtB . n3r0frhtxf =
rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW .
lufsk3t3r2 . CircularBufSize , & rtDW . lufsk3t3r2 . Last , rtDW . lufsk3t3r2
. Tail , rtDW . lufsk3t3r2 . Head , rtP .
TransportDelay_InitOutput_prtl4epgd1 , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
cmplr0vzdb = ssGetT ( rtS ) ; rtB . ldygbefh53 = ( rtB . cmplr0vzdb >= rtP .
K1_Value_ozpdlbwsw4 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
c1mfptet5m = rtDW . itgyxe4zdv ; } if ( rtB . ldygbefh53 ) { rtB . de2hv0u2iv
= rtB . plbwcbylcg - rtB . n3r0frhtxf ; rtB . iquplows5n = rtP .
Gain_Gain_ltnldkumka * rtB . de2hv0u2iv ; rtB . pkwpszftqs = rtB . iquplows5n
; } else { rtB . pkwpszftqs = rtB . c1mfptet5m ; } rtB . jvril55nsb . re =
rtB . fhgutxj4v5 ; rtB . jvril55nsb . im = rtB . pkwpszftqs ; rtB .
ijrujoz1x4 = muDoubleScalarHypot ( rtB . jvril55nsb . re , rtB . jvril55nsb .
im ) ; rtB . bg20ahlsh4 = muDoubleScalarAtan2 ( rtB . jvril55nsb . im , rtB .
jvril55nsb . re ) ; rtB . hvgaooxh3r = rtP . RadDeg_Gain_jus0fptbn0 * rtB .
bg20ahlsh4 ; rtB . bmwnqrf1wc = rtP . degrad1_Gain_byxhhwuqt0 * rtB .
hvgaooxh3r ; maxV = rtB . bmwnqrf1wc ; u0 = rtB . ijrujoz1x4 ; u2 =
muDoubleScalarCos ( maxV ) ; maxV = muDoubleScalarSin ( maxV ) ; rtB .
mq5dj2hfit . re = u0 * u2 ; rtB . mq5dj2hfit . im = u0 * maxV ; rtB .
mlxeaid3dj = rtX . prtdiw223v ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. gvpng3pvrl . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . gvpng3pvrl . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_gps5uogsr2 ; rtB .
ejzxy01nsc = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . p1vhfh3p1a . CircularBufSize , & rtDW . p1vhfh3p1a . Last , rtDW .
p1vhfh3p1a . Tail , rtDW . p1vhfh3p1a . Head , rtP .
TransportDelay_InitOutput_n1jopoie0t , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
k4gtf4oe3t = ssGetT ( rtS ) ; rtB . cktlwncgtu = ( rtB . k4gtf4oe3t >= rtP .
K1_Value_hbe5d3j1oy ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
ftpzihtg5w = rtDW . jnixgbeum0 ; } if ( rtB . cktlwncgtu ) { rtB . ftwkwzjgif
= rtB . mlxeaid3dj - rtB . ejzxy01nsc ; rtB . ciulaejklx = rtP .
Gain_Gain_cmldqnpoqn * rtB . ftwkwzjgif ; rtB . dgjeksbyyv = rtB . ciulaejklx
; } else { rtB . dgjeksbyyv = rtB . ftpzihtg5w ; } rtB . g13nplvhwh = rtX .
bots1ppmcj ; { real_T * * uBuffer = ( real_T * * ) & rtDW . cfwoq0f2ti .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & rtDW . cfwoq0f2ti
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ; real_T tMinusDelay =
simTime - rtP . TransportDelay_Delay_oc5ujwjout ; rtB . m2h1bo2b0c =
rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW .
h3jb1xttev . CircularBufSize , & rtDW . h3jb1xttev . Last , rtDW . h3jb1xttev
. Tail , rtDW . h3jb1xttev . Head , rtP .
TransportDelay_InitOutput_nayvtolcs2 , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
bgvvdiyup2 = ssGetT ( rtS ) ; rtB . kevz1pk25n = ( rtB . bgvvdiyup2 >= rtP .
K1_Value_f2gnltgzok ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
d2l12bbk0f = rtDW . c2b1i5ixoq ; } if ( rtB . kevz1pk25n ) { rtB . ivzkasnk1l
= rtB . g13nplvhwh - rtB . m2h1bo2b0c ; rtB . lqln4qxdkc = rtP .
Gain_Gain_jeqo2fa2pp * rtB . ivzkasnk1l ; rtB . hmdmab20r1 = rtB . lqln4qxdkc
; } else { rtB . hmdmab20r1 = rtB . d2l12bbk0f ; } rtB . cu4cyphi0d . re =
rtB . dgjeksbyyv ; rtB . cu4cyphi0d . im = rtB . hmdmab20r1 ; rtB .
cumgj511g1 = muDoubleScalarHypot ( rtB . cu4cyphi0d . re , rtB . cu4cyphi0d .
im ) ; rtB . mygdrvo53g = muDoubleScalarAtan2 ( rtB . cu4cyphi0d . im , rtB .
cu4cyphi0d . re ) ; rtB . cyjrxlbbnf = rtP . RadDeg_Gain_cow0p5spzh * rtB .
mygdrvo53g ; rtB . lysyjmvij4 = rtP . degrad2_Gain_d2jlqbu2gm * rtB .
cyjrxlbbnf ; maxV = rtB . lysyjmvij4 ; u0 = rtB . cumgj511g1 ; u2 =
muDoubleScalarCos ( maxV ) ; maxV = muDoubleScalarSin ( maxV ) ; rtB .
excsuws5ab . re = u0 * u2 ; rtB . excsuws5ab . im = u0 * maxV ; jvoeh2x3fa (
rtS , rtP . Constant_Value_dstlqvqnz4 , rtB . ogkcemo3dw , rtB . mq5dj2hfit ,
rtB . excsuws5ab , & rtB . p4hmlcjayz , & rtDW . p4hmlcjayz , & rtP .
p4hmlcjayz ) ; jvoeh2x3fa ( rtS , rtP . Constant1_Value_kx5mu21vxm , rtB .
ogkcemo3dw , rtB . mq5dj2hfit , rtB . excsuws5ab , & rtB . dem3so030k , &
rtDW . dem3so030k , & rtP . dem3so030k ) ; czm0w3hmwh ( rtS , rtP .
Constant2_Value_bf2litqi4f , rtB . ogkcemo3dw , rtB . mq5dj2hfit , rtB .
excsuws5ab , & rtB . mvaehis2rc , & rtDW . mvaehis2rc , & rtP . mvaehis2rc )
; rtB . ni31zmm3ns [ 0 ] = muDoubleScalarHypot ( rtB . p4hmlcjayz .
hjcfumkc1i . re , rtB . p4hmlcjayz . hjcfumkc1i . im ) ; rtB . ni31zmm3ns [ 1
] = muDoubleScalarHypot ( rtB . dem3so030k . hjcfumkc1i . re , rtB .
dem3so030k . hjcfumkc1i . im ) ; rtB . ni31zmm3ns [ 2 ] = muDoubleScalarHypot
( rtB . mvaehis2rc . fnsdbz5hxw . re , rtB . mvaehis2rc . fnsdbz5hxw . im ) ;
rtB . p5tenuhx12 [ 0 ] = muDoubleScalarAtan2 ( rtB . p4hmlcjayz . hjcfumkc1i
. im , rtB . p4hmlcjayz . hjcfumkc1i . re ) ; rtB . p5tenuhx12 [ 1 ] =
muDoubleScalarAtan2 ( rtB . dem3so030k . hjcfumkc1i . im , rtB . dem3so030k .
hjcfumkc1i . re ) ; rtB . p5tenuhx12 [ 2 ] = muDoubleScalarAtan2 ( rtB .
mvaehis2rc . fnsdbz5hxw . im , rtB . mvaehis2rc . fnsdbz5hxw . re ) ; rtB .
j5ghyi33x1 = rtP . Gain3_Gain_gqwkvw13nc * rtB . ni31zmm3ns [ 2 ] ; rtB .
oa2tsdkuod = rtP . raddeg1_Gain_pc4jldpgsd * rtB . p5tenuhx12 [ 2 ] ; rtB .
l3leaj3jgl = rtP . Gain2_Gain_kb5k4jghpk * rtB . oa2tsdkuod ; rtB .
ewzregcihg = muDoubleScalarCos ( rtB . l3leaj3jgl ) ; rtB . meaomogfsi = rtB
. j5ghyi33x1 * rtB . ewzregcihg ; rtB . eci4chb5l5 = rtB . meaomogfsi * rtP .
Constant_Value_ibzdxhwhml ; rtB . ojak2ro5zt = muDoubleScalarSin ( rtB .
l3leaj3jgl ) ; rtB . a3te0jryv2 = rtB . ojak2ro5zt * rtB . j5ghyi33x1 ; rtB .
jcebwcskug = rtB . a3te0jryv2 * rtP . Constant1_Value_aucg3qgymz ; rtB .
jzu5exwh33 = rtB . eci4chb5l5 - rtB . jcebwcskug ; rtB . h5u2jzd4b3 = rtB .
jzu5exwh33 + rtB . pjxmqrmerd ; rtB . bmczk22ayh = rtB . fipekfgd1i * rtB .
h5u2jzd4b3 ; rtB . hpjsoxp04e = rtP . Constant_Value_ibzdxhwhml * rtB .
a3te0jryv2 ; rtB . aagx24vyu4 = rtP . Constant1_Value_aucg3qgymz * rtB .
meaomogfsi ; rtB . cvesxjplpv = rtB . hpjsoxp04e + rtB . aagx24vyu4 ; rtB .
lvycmrjj1h = rtB . cvesxjplpv + rtB . koph4w1vnf ; rtB . g4mjkuxlb5 = rtB .
baroi4q3qd * rtB . lvycmrjj1h ; rtB . bwayrobyvc = rtB . bmczk22ayh + rtB .
g4mjkuxlb5 ; rtB . daa053gbds = rtB . h5u2jzd4b3 * rtB . h5u2jzd4b3 ; rtB .
pgz0qx31fz = rtB . lvycmrjj1h * rtB . lvycmrjj1h ; rtB . a0m1jbzdga = rtB .
daa053gbds + rtB . pgz0qx31fz ; rtB . pelomrw3zd = rtB . bwayrobyvc / rtB .
a0m1jbzdga ; rtB . gmchkgfefe = rtB . baroi4q3qd * rtB . h5u2jzd4b3 ; rtB .
korxywuocs = rtB . fipekfgd1i * rtB . lvycmrjj1h ; rtB . imxmpbk4iv = rtB .
gmchkgfefe - rtB . korxywuocs ; rtB . bdu5v1b4hw = rtB . imxmpbk4iv / rtB .
a0m1jbzdga ; rtB . jrmkmmw3iv = rtB . jzu5exwh33 + rtB . i5dpx5t34o ; rtB .
ghkcy5i2vh = rtB . hgxwusxbgy * rtB . jrmkmmw3iv ; rtB . cfmfjxta45 = rtB .
cvesxjplpv + rtB . jlt5ytbte0 ; rtB . mdbh5jjea0 = rtB . dktcpt50pn * rtB .
cfmfjxta45 ; rtB . aduq24zg4v = rtB . ghkcy5i2vh + rtB . mdbh5jjea0 ; rtB .
ctjcg4d0dh = rtB . jrmkmmw3iv * rtB . jrmkmmw3iv ; rtB . kjxmnjwiut = rtB .
cfmfjxta45 * rtB . cfmfjxta45 ; rtB . brb5mmddx0 = rtB . ctjcg4d0dh + rtB .
kjxmnjwiut ; rtB . f5x1wgqi5x = rtB . aduq24zg4v / rtB . brb5mmddx0 ; rtB .
oviuuwujte = rtB . dktcpt50pn * rtB . jrmkmmw3iv ; rtB . k5g2qp5fxb = rtB .
hgxwusxbgy * rtB . cfmfjxta45 ; rtB . d001eqwcrr = rtB . oviuuwujte - rtB .
k5g2qp5fxb ; rtB . bcz05rjpyd = rtB . d001eqwcrr / rtB . brb5mmddx0 ; rtB .
f0ezr3ayn2 = rtB . jzu5exwh33 + rtB . lnv3d3rvor ; rtB . pj1dfknc1m = rtB .
lmcprjcgyb * rtB . f0ezr3ayn2 ; rtB . aeptmjqndo = rtB . cvesxjplpv + rtB .
chvsplbxtl ; rtB . dl5vptdskv = rtB . cuggjv2ise * rtB . aeptmjqndo ; rtB .
mvpublwmms = rtB . pj1dfknc1m + rtB . dl5vptdskv ; rtB . abxyszcyfw = rtB .
f0ezr3ayn2 * rtB . f0ezr3ayn2 ; rtB . moputfy3yg = rtB . aeptmjqndo * rtB .
aeptmjqndo ; rtB . l5i2oxwrvz = rtB . abxyszcyfw + rtB . moputfy3yg ; rtB .
ikx33rrotl = rtB . mvpublwmms / rtB . l5i2oxwrvz ; rtB . p3whgf2p25 = rtB .
cuggjv2ise * rtB . f0ezr3ayn2 ; rtB . b05jvvdri2 = rtB . lmcprjcgyb * rtB .
aeptmjqndo ; rtB . hayde1wwht = rtB . p3whgf2p25 - rtB . b05jvvdri2 ; rtB .
bom11wg1z5 = rtB . hayde1wwht / rtB . l5i2oxwrvz ; rtB . d0ejra0usp =
muDoubleScalarSin ( rtP . sinwt_Freq_f0rbwbacph * ssGetTaskTime ( rtS , 0 ) +
rtP . sinwt_Phase_io3kczfndz ) * rtP . sinwt_Amp_ge1lfbcp5s + rtP .
sinwt_Bias_exzm2rgrwb ; rtB . kznsejiloq = rtB . duyxr0xofz [ 0 ] * rtB .
d0ejra0usp ; rtB . djnnzumkhr = muDoubleScalarSin ( rtP .
coswt_Freq_grrhw2zxoh * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_dvixgswhlj ) * rtP . coswt_Amp_fxztqw15q5 + rtP .
coswt_Bias_gvm32teu11 ; rtB . e40bwzvglm = rtB . duyxr0xofz [ 0 ] * rtB .
djnnzumkhr ; rtB . mrm3wdd4zp = muDoubleScalarSin ( rtP .
sinwt_Freq_dac5s1zyay * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_potakoeye3 ) * rtP . sinwt_Amp_bdhc5zih2a + rtP .
sinwt_Bias_czrdpt4lan ; rtB . hc2o4lmxgu = rtB . duyxr0xofz [ 1 ] * rtB .
mrm3wdd4zp ; rtB . fo0zj2dddx = muDoubleScalarSin ( rtP .
coswt_Freq_pjlwn44ks2 * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_h4cvgknuvk ) * rtP . coswt_Amp_j5rqb25oa2 + rtP .
coswt_Bias_hdnewhjlvc ; rtB . lxhpidiqog = rtB . duyxr0xofz [ 1 ] * rtB .
fo0zj2dddx ; rtB . hqw4vva3qa = muDoubleScalarSin ( rtP .
sinwt_Freq_ol0q4lhgol * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_n0hiqx4gnb ) * rtP . sinwt_Amp_njfhwjpcvl + rtP .
sinwt_Bias_giefzghqhe ; rtB . gy3dadahiv = rtB . duyxr0xofz [ 2 ] * rtB .
hqw4vva3qa ; rtB . ncboacc51m = muDoubleScalarSin ( rtP .
coswt_Freq_hqgvopl0nr * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_ic4hfnossn ) * rtP . coswt_Amp_cfkursvjia + rtP .
coswt_Bias_mkyyy3wllb ; rtB . p3s31yxgop = rtB . duyxr0xofz [ 2 ] * rtB .
ncboacc51m ; rtB . eblhjm5hc2 = muDoubleScalarSin ( rtP .
sinwt_Freq_j0jndzhehn * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_jpmhxafwi0 ) * rtP . sinwt_Amp_ar25xslr3n + rtP .
sinwt_Bias_moixbcaa1z ; rtB . awjpq3b3zw = rtB . oxfyqex5ti * rtB .
eblhjm5hc2 ; rtB . kkqmttw0c0 = muDoubleScalarSin ( rtP .
coswt_Freq_bbfatkvcxs * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_fkjgv2jja4 ) * rtP . coswt_Amp_laelpd2aua + rtP .
coswt_Bias_od4llw2btg ; rtB . pkycymvyje = rtB . oxfyqex5ti * rtB .
kkqmttw0c0 ; rtB . nxcfdkf1d1 = muDoubleScalarSin ( rtP .
sinwt_Freq_msku4mgfbh * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_mo4k2zufrc ) * rtP . sinwt_Amp_o13lyncasa + rtP .
sinwt_Bias_p3dpdj5gq3 ; rtB . f4bmazlri0 = rtB . oxfyqex5ti * rtB .
nxcfdkf1d1 ; rtB . o4iq30rwyk = muDoubleScalarSin ( rtP .
coswt_Freq_bvniry5j0f * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_g44uuw0pjj ) * rtP . coswt_Amp_ot0y23h1b1 + rtP .
coswt_Bias_dqey5aryc1 ; rtB . b2davm33jr = rtB . oxfyqex5ti * rtB .
o4iq30rwyk ; rtB . ppe55rry1u = muDoubleScalarSin ( rtP .
sinwt_Freq_mwdevuoutt * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_bdqbwmks1g ) * rtP . sinwt_Amp_lpanhjeiqc + rtP .
sinwt_Bias_ml2yalrkyj ; rtB . iiconzrnl4 = 0.0 * rtB . ppe55rry1u ; rtB .
nfb1mlt3zx = muDoubleScalarSin ( rtP . coswt_Freq_cxrlo2yhfg * ssGetTaskTime
( rtS , 0 ) + rtP . coswt_Phase_lxukmjorth ) * rtP . coswt_Amp_fzj2hujsv1 +
rtP . coswt_Bias_oqzqvjxf0f ; rtB . hh35giszvi = 0.0 * rtB . nfb1mlt3zx ; rtB
. h2tnalkd1x = muDoubleScalarSin ( rtP . sinwt_Freq_lahhfi35mf *
ssGetTaskTime ( rtS , 0 ) + rtP . sinwt_Phase_pk23pfdaau ) * rtP .
sinwt_Amp_cqeu5f4eqd + rtP . sinwt_Bias_adnmqizfui ; rtB . ith2vi2lkm = 0.0 *
rtB . h2tnalkd1x ; rtB . homvjcwnrd = muDoubleScalarSin ( rtP .
coswt_Freq_k2rm3ufcdk * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_j4yq4exglq ) * rtP . coswt_Amp_obpcmtwyhd + rtP .
coswt_Bias_dh1qcql0r2 ; rtB . bialu2hh5u = 0.0 * rtB . homvjcwnrd ; rtB .
aata5nx1pa = muDoubleScalarSin ( rtP . sinwt_Freq_nnzpk1nham * ssGetTaskTime
( rtS , 0 ) + rtP . sinwt_Phase_daazdgfb5v ) * rtP . sinwt_Amp_loesp40qcp +
rtP . sinwt_Bias_kij0kifqfs ; rtB . mufvugkq55 = 0.0 * rtB . aata5nx1pa ; rtB
. kpdld4fk5y = muDoubleScalarSin ( rtP . coswt_Freq_nw0g5worxa *
ssGetTaskTime ( rtS , 0 ) + rtP . coswt_Phase_m3p3fdt0xc ) * rtP .
coswt_Amp_fdwsqw0uvh + rtP . coswt_Bias_okdvlto52z ; rtB . grm0i134jq = 0.0 *
rtB . kpdld4fk5y ; rtB . jivikhp45s = muDoubleScalarSin ( rtP .
sinwt_Freq_hwfnsn2nnz * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_hwdsw3fn1s ) * rtP . sinwt_Amp_dmfg5ds4ju + rtP .
sinwt_Bias_f0dovjnmjw ; rtB . fykapa13hu = 0.0 * rtB . jivikhp45s ; rtB .
ozawsrt1hm = muDoubleScalarSin ( rtP . coswt_Freq_ckozjdexxj * ssGetTaskTime
( rtS , 0 ) + rtP . coswt_Phase_h3hazfvfdl ) * rtP . coswt_Amp_ihpvk25wtx +
rtP . coswt_Bias_lwt4rk44yf ; rtB . kpzv0mtsrl = 0.0 * rtB . ozawsrt1hm ; rtB
. lmbhb3013h = muDoubleScalarSin ( rtP . sinwt_Freq_dwecndq4y4 *
ssGetTaskTime ( rtS , 0 ) + rtP . sinwt_Phase_p0zdze03pr ) * rtP .
sinwt_Amp_medc5xwua2 + rtP . sinwt_Bias_jw2dohr3ue ; rtB . ptc5im5mxx = 0.0 *
rtB . lmbhb3013h ; rtB . bfpyk4s4dg = muDoubleScalarSin ( rtP .
coswt_Freq_das4eyceru * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_lqntzdwr00 ) * rtP . coswt_Amp_oq4bxk0xmq + rtP .
coswt_Bias_kicv3c4l5u ; rtB . hjme1gtmfr = 0.0 * rtB . bfpyk4s4dg ; rtB .
guol2z454b = muDoubleScalarSin ( rtP . sinwt_Freq_dbiddisefd * ssGetTaskTime
( rtS , 0 ) + rtP . sinwt_Phase_cs3tfqwssj ) * rtP . sinwt_Amp_kjo1pq5oej +
rtP . sinwt_Bias_dm0qhlfrpe ; rtB . b0tbnfmob2 = 0.0 * rtB . guol2z454b ; rtB
. c3agggfby4 = muDoubleScalarSin ( rtP . coswt_Freq_anzjoydr0j *
ssGetTaskTime ( rtS , 0 ) + rtP . coswt_Phase_gx23ygf35u ) * rtP .
coswt_Amp_hqg21ojuqg + rtP . coswt_Bias_g5sna5itui ; rtB . fohd0pwzvp = 0.0 *
rtB . c3agggfby4 ; rtB . ftjitujlmu = rtX . n0uge3m4yf ; { real_T * * uBuffer
= ( real_T * * ) & rtDW . ecebamajhh . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & rtDW . ecebamajhh . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_dywj1im3o2 ; rtB . pqcrncmb2v = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . km2x20r4ek .
CircularBufSize , & rtDW . km2x20r4ek . Last , rtDW . km2x20r4ek . Tail ,
rtDW . km2x20r4ek . Head , rtP . TransportDelay_InitOutput_j0jcaphlgu , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . nggrwz42yb = ssGetT ( rtS ) ; rtB . giprbd0acg
= ( rtB . nggrwz42yb >= rtP . K1_Value_oopkw5qtep ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . caejf2tnls = rtDW . oumxtk4xvn ; } if ( rtB .
giprbd0acg ) { rtB . noua5slrcd = rtB . ftjitujlmu - rtB . pqcrncmb2v ; rtB .
gunavgzfbp = rtP . Gain_Gain_mubzyiyngt * rtB . noua5slrcd ; rtB . ngkpxovgoe
= rtB . gunavgzfbp ; } else { rtB . ngkpxovgoe = rtB . caejf2tnls ; } rtB .
gz52grzsnh = rtX . gnzuxd2raf ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. ogxech4wih . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . ogxech4wih . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_j5wvotkdow ; rtB .
mpfow4v2cw = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . ddmxqb4sul . CircularBufSize , & rtDW . ddmxqb4sul . Last , rtDW .
ddmxqb4sul . Tail , rtDW . ddmxqb4sul . Head , rtP .
TransportDelay_InitOutput_gsjrmrtbdf , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
ax420bqo2s = ssGetT ( rtS ) ; rtB . ch25chmnmh = ( rtB . ax420bqo2s >= rtP .
K1_Value_ea003eeslf ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
l53heo5qx0 = rtDW . cvewj2ww1t ; } if ( rtB . ch25chmnmh ) { rtB . aoqnzftpqh
= rtB . gz52grzsnh - rtB . mpfow4v2cw ; rtB . oxxphyzsc1 = rtP .
Gain_Gain_kwzfdsjo25 * rtB . aoqnzftpqh ; rtB . kmr42jbgqs = rtB . oxxphyzsc1
; } else { rtB . kmr42jbgqs = rtB . l53heo5qx0 ; } rtB . cezmjtbhqr . re =
rtB . ngkpxovgoe ; rtB . cezmjtbhqr . im = rtB . kmr42jbgqs ; rtB .
kg5d523rya = muDoubleScalarHypot ( rtB . cezmjtbhqr . re , rtB . cezmjtbhqr .
im ) ; rtB . noa5gi4o55 = muDoubleScalarAtan2 ( rtB . cezmjtbhqr . im , rtB .
cezmjtbhqr . re ) ; rtB . in52vk3wa3 = rtP . RadDeg_Gain_gcoodpw0cg * rtB .
noa5gi4o55 ; rtB . ffuqyqeatn = rtP . degrad_Gain_fxlchd12gf * rtB .
in52vk3wa3 ; maxV = rtB . ffuqyqeatn ; u0 = rtB . kg5d523rya ; u2 =
muDoubleScalarCos ( maxV ) ; maxV = muDoubleScalarSin ( maxV ) ; rtB .
gh0xnrxdhc . re = u0 * u2 ; rtB . gh0xnrxdhc . im = u0 * maxV ; rtB .
dmefqabr3w = rtX . bqo41zgbir ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. j1jrbblzw5 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . j1jrbblzw5 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_lqgrxsjeg1 ; rtB .
liorrixuwz = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . d11oepymjs . CircularBufSize , & rtDW . d11oepymjs . Last , rtDW .
d11oepymjs . Tail , rtDW . d11oepymjs . Head , rtP .
TransportDelay_InitOutput_cbxjnchowy , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
hlpcmzqn4c = ssGetT ( rtS ) ; rtB . hyacswucko = ( rtB . hlpcmzqn4c >= rtP .
K1_Value_jatccw30rn ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
de0c1vmyya = rtDW . gb2aoze2zq ; } if ( rtB . hyacswucko ) { rtB . f21at2g5cr
= rtB . dmefqabr3w - rtB . liorrixuwz ; rtB . ejh1zwxvx5 = rtP .
Gain_Gain_acs5nclqai * rtB . f21at2g5cr ; rtB . pomuja1z5w = rtB . ejh1zwxvx5
; } else { rtB . pomuja1z5w = rtB . de0c1vmyya ; } rtB . er1wxf0naj = rtX .
kiwleabf04 ; { real_T * * uBuffer = ( real_T * * ) & rtDW . hkkzlg4m02 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & rtDW . hkkzlg4m02
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ; real_T tMinusDelay =
simTime - rtP . TransportDelay_Delay_hnythpuhce ; rtB . fp10whx1uu =
rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW .
e311nuuqb5 . CircularBufSize , & rtDW . e311nuuqb5 . Last , rtDW . e311nuuqb5
. Tail , rtDW . e311nuuqb5 . Head , rtP .
TransportDelay_InitOutput_gssa5ux2lo , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
gjosk30pc4 = ssGetT ( rtS ) ; rtB . dt2du4wcz3 = ( rtB . gjosk30pc4 >= rtP .
K1_Value_jabgbrde43 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
lx0jneawil = rtDW . fxksrxtwby ; } if ( rtB . dt2du4wcz3 ) { rtB . mh1xqorv24
= rtB . er1wxf0naj - rtB . fp10whx1uu ; rtB . lm3bkkirpl = rtP .
Gain_Gain_oymuya3l0a * rtB . mh1xqorv24 ; rtB . deg3xxymgh = rtB . lm3bkkirpl
; } else { rtB . deg3xxymgh = rtB . lx0jneawil ; } rtB . mue43qt4w5 . re =
rtB . pomuja1z5w ; rtB . mue43qt4w5 . im = rtB . deg3xxymgh ; rtB .
cpwbkvi0o5 = muDoubleScalarHypot ( rtB . mue43qt4w5 . re , rtB . mue43qt4w5 .
im ) ; rtB . mjcqog1sc4 = muDoubleScalarAtan2 ( rtB . mue43qt4w5 . im , rtB .
mue43qt4w5 . re ) ; rtB . feohnoysbe = rtP . RadDeg_Gain_glvhb2u05e * rtB .
mjcqog1sc4 ; rtB . etnycofpqj = rtP . degrad1_Gain_aol4q44hvd * rtB .
feohnoysbe ; maxV = rtB . etnycofpqj ; u0 = rtB . cpwbkvi0o5 ; u2 =
muDoubleScalarCos ( maxV ) ; maxV = muDoubleScalarSin ( maxV ) ; rtB .
j2ph2sfcub . re = u0 * u2 ; rtB . j2ph2sfcub . im = u0 * maxV ; rtB .
lzyxcnmkb0 = rtX . aa5y12cea2 ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. hva2h0mh4k . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . hva2h0mh4k . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_hzbv3ih5sh ; rtB .
hi5s2vyv5m = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . g0lsoknhdx . CircularBufSize , & rtDW . g0lsoknhdx . Last , rtDW .
g0lsoknhdx . Tail , rtDW . g0lsoknhdx . Head , rtP .
TransportDelay_InitOutput_inbo2ulujt , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
bu3nlos2ih = ssGetT ( rtS ) ; rtB . bju43fk3nr = ( rtB . bu3nlos2ih >= rtP .
K1_Value_ofzunistqm ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
nfb5nafszt = rtDW . pwd5paigtu ; } if ( rtB . bju43fk3nr ) { rtB . gudmbgs1u5
= rtB . lzyxcnmkb0 - rtB . hi5s2vyv5m ; rtB . gp02y1o1k0 = rtP .
Gain_Gain_hxnrtpkrvt * rtB . gudmbgs1u5 ; rtB . jwv42cyom4 = rtB . gp02y1o1k0
; } else { rtB . jwv42cyom4 = rtB . nfb5nafszt ; } rtB . ekyn2dmd41 = rtX .
cylrltmkae ; { real_T * * uBuffer = ( real_T * * ) & rtDW . crpy1zhjkz .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & rtDW . crpy1zhjkz
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ; real_T tMinusDelay =
simTime - rtP . TransportDelay_Delay_l1fxkrpmnr ; rtB . m50o3r1kqh =
rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW .
j1sal1hher . CircularBufSize , & rtDW . j1sal1hher . Last , rtDW . j1sal1hher
. Tail , rtDW . j1sal1hher . Head , rtP .
TransportDelay_InitOutput_cw4myqmahz , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
lnjqor22wv = ssGetT ( rtS ) ; rtB . bxfijzccux = ( rtB . lnjqor22wv >= rtP .
K1_Value_i0yex2sdgr ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
l0fktxsudd = rtDW . gaijbd3xgk ; } if ( rtB . bxfijzccux ) { rtB . n42hbl0ydr
= rtB . ekyn2dmd41 - rtB . m50o3r1kqh ; rtB . k443m2mley = rtP .
Gain_Gain_da5dxytfd3 * rtB . n42hbl0ydr ; rtB . l0isenqrj3 = rtB . k443m2mley
; } else { rtB . l0isenqrj3 = rtB . l0fktxsudd ; } rtB . mnumucta4y . re =
rtB . jwv42cyom4 ; rtB . mnumucta4y . im = rtB . l0isenqrj3 ; rtB .
aj5cxv41jo = muDoubleScalarHypot ( rtB . mnumucta4y . re , rtB . mnumucta4y .
im ) ; rtB . fn1daf2qh4 = muDoubleScalarAtan2 ( rtB . mnumucta4y . im , rtB .
mnumucta4y . re ) ; rtB . e4xg5zfzm1 = rtP . RadDeg_Gain_fnebdkxwml * rtB .
fn1daf2qh4 ; rtB . fvkgxstys2 = rtP . degrad2_Gain_bb55t3p5nq * rtB .
e4xg5zfzm1 ; maxV = rtB . fvkgxstys2 ; u0 = rtB . aj5cxv41jo ; u2 =
muDoubleScalarCos ( maxV ) ; maxV = muDoubleScalarSin ( maxV ) ; rtB .
li44kuv5dr . re = u0 * u2 ; rtB . li44kuv5dr . im = u0 * maxV ; jvoeh2x3fa (
rtS , rtP . Constant_Value_kh5gqwnzi1 , rtB . gh0xnrxdhc , rtB . j2ph2sfcub ,
rtB . li44kuv5dr , & rtB . gjmkpqrmc3 , & rtDW . gjmkpqrmc3 , & rtP .
gjmkpqrmc3 ) ; jvoeh2x3fa ( rtS , rtP . Constant1_Value_e1u3ttw0nl , rtB .
gh0xnrxdhc , rtB . j2ph2sfcub , rtB . li44kuv5dr , & rtB . khd3qxar1i , &
rtDW . khd3qxar1i , & rtP . khd3qxar1i ) ; czm0w3hmwh ( rtS , rtP .
Constant2_Value_lukrbqmoqp , rtB . gh0xnrxdhc , rtB . j2ph2sfcub , rtB .
li44kuv5dr , & rtB . jzsygc4sa4 , & rtDW . jzsygc4sa4 , & rtP . jzsygc4sa4 )
; rtB . htios0055t [ 0 ] = muDoubleScalarHypot ( rtB . gjmkpqrmc3 .
hjcfumkc1i . re , rtB . gjmkpqrmc3 . hjcfumkc1i . im ) ; rtB . htios0055t [ 1
] = muDoubleScalarHypot ( rtB . khd3qxar1i . hjcfumkc1i . re , rtB .
khd3qxar1i . hjcfumkc1i . im ) ; rtB . htios0055t [ 2 ] = muDoubleScalarHypot
( rtB . jzsygc4sa4 . fnsdbz5hxw . re , rtB . jzsygc4sa4 . fnsdbz5hxw . im ) ;
rtB . hgc5iiga4m [ 0 ] = muDoubleScalarAtan2 ( rtB . gjmkpqrmc3 . hjcfumkc1i
. im , rtB . gjmkpqrmc3 . hjcfumkc1i . re ) ; rtB . hgc5iiga4m [ 1 ] =
muDoubleScalarAtan2 ( rtB . khd3qxar1i . hjcfumkc1i . im , rtB . khd3qxar1i .
hjcfumkc1i . re ) ; rtB . hgc5iiga4m [ 2 ] = muDoubleScalarAtan2 ( rtB .
jzsygc4sa4 . fnsdbz5hxw . im , rtB . jzsygc4sa4 . fnsdbz5hxw . re ) ; rtB .
jg5ubzkurd = rtP . raddeg1_Gain_ivovx0j5nx * rtB . hgc5iiga4m [ 2 ] ; rtB .
ism3fsuitn = rtP . Gain1_Gain_d5g045kift * rtB . jg5ubzkurd ; rtB .
klthtkdakw = muDoubleScalarSin ( rtB . ism3fsuitn ) ; rtB . nntynydd1n =
muDoubleScalarCos ( rtB . ism3fsuitn ) ; rtB . al1cuve5b3 = rtP .
Gain4_Gain_bcyx4l4ouf * rtB . htios0055t [ 2 ] ; rtB . ofu324lwps = rtB .
klthtkdakw * rtB . al1cuve5b3 ; rtB . gcs4rzvfj0 = rtB . al1cuve5b3 * rtB .
nntynydd1n ; rtB . p5zvwmm3nj = muDoubleScalarSin ( rtP .
sinwt_Freq_bffbs3dujr * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_cyob2zrcym ) * rtP . sinwt_Amp_kix5ol1yha + rtP .
sinwt_Bias_mbb4e1q4r1 ; rtB . gtajd15451 = rtB . duyxr0xofz [ 0 ] * rtB .
p5zvwmm3nj ; rtB . m3nrqkjgnu = muDoubleScalarSin ( rtP .
coswt_Freq_f0mfemzie4 * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_aenmzc4bjq ) * rtP . coswt_Amp_av1cfalog1 + rtP .
coswt_Bias_p0nzqxhsjz ; rtB . m5wvrgyai1 = rtB . duyxr0xofz [ 0 ] * rtB .
m3nrqkjgnu ; rtB . knf2q3kcfc = muDoubleScalarSin ( rtP .
sinwt_Freq_jqmycgm0ba * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_d4zc3g3csg ) * rtP . sinwt_Amp_jkz1lrx4cn + rtP .
sinwt_Bias_iqks01y1as ; rtB . hghbeybdaf = rtB . duyxr0xofz [ 1 ] * rtB .
knf2q3kcfc ; rtB . ijajvxmsgi = muDoubleScalarSin ( rtP .
coswt_Freq_fmumc3tso0 * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_boqubpo3un ) * rtP . coswt_Amp_jz1fvo2uvd + rtP .
coswt_Bias_aqvwvx2orq ; rtB . gql2g0whz1 = rtB . duyxr0xofz [ 1 ] * rtB .
ijajvxmsgi ; rtB . psnohihaaz = muDoubleScalarSin ( rtP .
sinwt_Freq_kl1nwu422p * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_cbqec4drqm ) * rtP . sinwt_Amp_pucwxgos4v + rtP .
sinwt_Bias_ctvl1bu1sa ; rtB . ba2brgvdad = rtB . duyxr0xofz [ 2 ] * rtB .
psnohihaaz ; rtB . klepxhytzh = muDoubleScalarSin ( rtP .
coswt_Freq_ba1yn3qfqi * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_avbkykdf0x ) * rtP . coswt_Amp_lwidljxufa + rtP .
coswt_Bias_kfmcr0fvzu ; rtB . b5vrpo3tf1 = rtB . duyxr0xofz [ 2 ] * rtB .
klepxhytzh ; rtB . haickunxan = rtB . gcs4rzvfj0 * rtB . meaomogfsi ; rtB .
l5dmywmayq = rtB . ofu324lwps * rtB . a3te0jryv2 ; rtB . pjsc2sskcs = rtB .
haickunxan + rtB . l5dmywmayq ; rtB . ahjz5zh3iw = rtB . meaomogfsi * rtB .
meaomogfsi ; rtB . na1nlshigc = rtB . a3te0jryv2 * rtB . a3te0jryv2 ; rtB .
pcuuzi33iv = rtB . ahjz5zh3iw + rtB . na1nlshigc ; rtB . fzvlma4cif = rtB .
ofu324lwps * rtB . meaomogfsi ; rtB . k5bjbv4jro = rtB . gcs4rzvfj0 * rtB .
a3te0jryv2 ; rtB . kdyxgdxu2x = rtB . fzvlma4cif - rtB . k5bjbv4jro ; rtB .
lnibbzh3bv = rtB . pjsc2sskcs / rtB . pcuuzi33iv ; rtB . iuj1veuade = rtB .
kdyxgdxu2x / rtB . pcuuzi33iv ; rtB . ersui1inkl = muDoubleScalarSin ( rtP .
sinwt_Freq_pig3mxeohl * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_an24mywvj3 ) * rtP . sinwt_Amp_kqdiawrmz2 + rtP .
sinwt_Bias_hx2dqe0e4s ; rtB . p3wccudzxi = 0.0 * rtB . ersui1inkl ; rtB .
hythy5jrn0 = muDoubleScalarSin ( rtP . coswt_Freq_ar2wr0eft4 * ssGetTaskTime
( rtS , 0 ) + rtP . coswt_Phase_cir3iepapf ) * rtP . coswt_Amp_a3htruy0qf +
rtP . coswt_Bias_mkusztmp2i ; rtB . onvggmtysa = 0.0 * rtB . hythy5jrn0 ; rtB
. n0mlkruix5 = muDoubleScalarSin ( rtP . sinwt_Freq_fgpf1jvqbs *
ssGetTaskTime ( rtS , 0 ) + rtP . sinwt_Phase_iq4w4utrik ) * rtP .
sinwt_Amp_eakfbhtm5l + rtP . sinwt_Bias_prfznlswxz ; rtB . a0uzpwsi3s = 0.0 *
rtB . n0mlkruix5 ; rtB . bkhop11r4z = muDoubleScalarSin ( rtP .
coswt_Freq_lc0o2ttn4f * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_hsau4dtqoi ) * rtP . coswt_Amp_druvyl0kle + rtP .
coswt_Bias_nhs43pxahw ; rtB . ojyy3nnssd = 0.0 * rtB . bkhop11r4z ; rtB .
hjlfom02br = muDoubleScalarSin ( rtP . sinwt_Freq_b4vrdwhx3c * ssGetTaskTime
( rtS , 0 ) + rtP . sinwt_Phase_ng4pz55zhc ) * rtP . sinwt_Amp_jzwvynozil +
rtP . sinwt_Bias_m2m425ygni ; rtB . lpgyvrpws0 = 0.0 * rtB . hjlfom02br ; rtB
. m4ax1utvkc = muDoubleScalarSin ( rtP . coswt_Freq_lnejw2v1ss *
ssGetTaskTime ( rtS , 0 ) + rtP . coswt_Phase_cn3ec334p2 ) * rtP .
coswt_Amp_gu3afvu2kf + rtP . coswt_Bias_iqjmrqecm4 ; rtB . ffbwz4di24 = 0.0 *
rtB . m4ax1utvkc ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . gd5uqicci0 =
rtDW . itxmk3chva ; } rtB . gz4uxocku5 = false ; rtB . ncfrmbqixl = ssGetT (
rtS ) ; on20slldft ( rtS , rtB . gs1tauzuoz , rtB . gz4uxocku5 , rtB .
ncfrmbqixl , rtP . T3_delay_orqh51qtvr , & rtB . jtynvhttl4 , & rtDW .
jtynvhttl4 , & rtP . jtynvhttl4 , & rtPrevZCX . jtynvhttl4 ) ; bhc3qgnuic (
rtS , rtB . nh31osveik , rtB . gz4uxocku5 , rtB . ncfrmbqixl , rtP .
T3_delay_orqh51qtvr , & rtB . jdzdt1ap5b , & rtDW . jdzdt1ap5b , & rtP .
jdzdt1ap5b , & rtPrevZCX . jdzdt1ap5b ) ; rtB . p2i0v5b15c = ( rtB .
jtynvhttl4 . j1vu52vt5l || rtB . jdzdt1ap5b . mjqylog0rb ) ; rtB . jiouik3pwt
= ( rtB . gd5uqicci0 || rtB . p2i0v5b15c ) ; rtB . eivqctg2cj = rtB .
hiouz3k3hf ; rtB . jo5ovoumqt = ssGetT ( rtS ) ; on20slldft ( rtS , rtB .
jzxofq1tg5 , rtB . eivqctg2cj , rtB . jo5ovoumqt , rtP . T4_delay_htrvgvsavj
, & rtB . fvh2zdqaau , & rtDW . fvh2zdqaau , & rtP . fvh2zdqaau , & rtPrevZCX
. fvh2zdqaau ) ; bhc3qgnuic ( rtS , rtB . m0txojuzpn , rtB . eivqctg2cj , rtB
. jo5ovoumqt , rtP . T4_delay_htrvgvsavj , & rtB . f41hg5wei1 , & rtDW .
f41hg5wei1 , & rtP . f41hg5wei1 , & rtPrevZCX . f41hg5wei1 ) ; rtB .
c5pox3ve0g = ( rtB . fvh2zdqaau . j1vu52vt5l || rtB . f41hg5wei1 . mjqylog0rb
) ; rtB . naxkbtx04s = ! rtB . c5pox3ve0g ; rtB . ckw23s2vuu = ( rtB .
jiouik3pwt && rtB . naxkbtx04s ) ; rtB . driongi2gk = ( rtB . ae0hls5jne &&
rtB . ckw23s2vuu ) ; rtB . pio4hbpadl = ( rtB . lso32itsny && rtB .
ckw23s2vuu ) ; rtB . c45mn4j204 = ( rtB . huukrkmwe1 && rtB . ckw23s2vuu ) ;
rtB . fwvph3d1an = rtX . ar5lw0nd1g ; { real_T * * uBuffer = ( real_T * * ) &
rtDW . k5ejkktore . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * )
& rtDW . k5ejkktore . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_nqpof1vzvj ; rtB .
pxokw52jjz = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . dfz14rvn5j . CircularBufSize , & rtDW . dfz14rvn5j . Last , rtDW .
dfz14rvn5j . Tail , rtDW . dfz14rvn5j . Head , rtP .
TransportDelay_InitOutput_eoidu31cxu , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
gyvxu0thle = ssGetT ( rtS ) ; rtB . n3g1rwgywx = ( rtB . gyvxu0thle >= rtP .
K1_Value_c3megqnwu5 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
hhce0ycmpl = rtDW . n1ijw4x0rn ; } if ( rtB . n3g1rwgywx ) { rtB . obpmn2q51j
= rtB . fwvph3d1an - rtB . pxokw52jjz ; rtB . puve00zc2i = rtP .
Gain_Gain_awhaqm5dim * rtB . obpmn2q51j ; rtB . mitrdmujtk = rtB . puve00zc2i
; } else { rtB . mitrdmujtk = rtB . hhce0ycmpl ; } rtB . m1qmwbvls5 = rtX .
etqlz3rohc ; { real_T * * uBuffer = ( real_T * * ) & rtDW . nvyaxvbcrw .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & rtDW . nvyaxvbcrw
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ; real_T tMinusDelay =
simTime - rtP . TransportDelay_Delay_krge4c0xm3 ; rtB . bncidyk5rz =
rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW .
aic2pxcqxi . CircularBufSize , & rtDW . aic2pxcqxi . Last , rtDW . aic2pxcqxi
. Tail , rtDW . aic2pxcqxi . Head , rtP .
TransportDelay_InitOutput_bnlvzhlygt , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
gwqni52a4g = ssGetT ( rtS ) ; rtB . expglqonng = ( rtB . gwqni52a4g >= rtP .
K1_Value_o0qhwwa5bs ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
ia1r4xls41 = rtDW . ciujz12ohu ; } if ( rtB . expglqonng ) { rtB . gwx4re4tkv
= rtB . m1qmwbvls5 - rtB . bncidyk5rz ; rtB . gzw4bq40yb = rtP .
Gain_Gain_lgbeljn3if * rtB . gwx4re4tkv ; rtB . gfru2n0eh5 = rtB . gzw4bq40yb
; } else { rtB . gfru2n0eh5 = rtB . ia1r4xls41 ; } rtB . kx4vzkpdgq . re =
rtB . mitrdmujtk ; rtB . kx4vzkpdgq . im = rtB . gfru2n0eh5 ; rtB .
anat1cfsan = muDoubleScalarHypot ( rtB . kx4vzkpdgq . re , rtB . kx4vzkpdgq .
im ) ; rtB . mlfo254b01 = muDoubleScalarAtan2 ( rtB . kx4vzkpdgq . im , rtB .
kx4vzkpdgq . re ) ; rtB . oqy5z3a4ld [ 0 ] = rtP . Gain2_Gain_llwd2t54xd *
rtB . anat1cfsan ; rtB . oqy5z3a4ld [ 1 ] = rtP . Gain2_Gain_llwd2t54xd * 0.0
; rtB . oqy5z3a4ld [ 2 ] = rtP . Gain2_Gain_llwd2t54xd * 0.0 ; rtB .
oqy5z3a4ld [ 3 ] = rtP . Gain2_Gain_llwd2t54xd * 0.0 ; rtB . oqy5z3a4ld [ 4 ]
= rtP . Gain2_Gain_llwd2t54xd * 0.0 ; rtB . oqy5z3a4ld [ 5 ] = rtP .
Gain2_Gain_llwd2t54xd * 0.0 ; rtB . oqy5z3a4ld [ 6 ] = rtP .
Gain2_Gain_llwd2t54xd * 0.0 ; rtB . oqy5z3a4ld [ 7 ] = rtP .
Gain2_Gain_llwd2t54xd * 0.0 ; rtB . oqy5z3a4ld [ 8 ] = rtP .
Gain2_Gain_llwd2t54xd * 0.0 ; rtB . oqy5z3a4ld [ 9 ] = rtP .
Gain2_Gain_llwd2t54xd * 0.0 ; rtB . oqy5z3a4ld [ 10 ] = rtP .
Gain2_Gain_llwd2t54xd * 0.0 ; rtB . h4sr3evpnn = rtP . RadDeg_Gain_js4sl0mhyl
* rtB . mlfo254b01 ; rtB . g34gfrsv4f [ 0 ] = rtP . Gain4_Gain_ktvhkcxsc3 *
rtB . h4sr3evpnn ; rtB . g34gfrsv4f [ 1 ] = rtP . Gain4_Gain_ktvhkcxsc3 * 0.0
; rtB . g34gfrsv4f [ 2 ] = rtP . Gain4_Gain_ktvhkcxsc3 * 0.0 ; rtB .
g34gfrsv4f [ 3 ] = rtP . Gain4_Gain_ktvhkcxsc3 * 0.0 ; rtB . g34gfrsv4f [ 4 ]
= rtP . Gain4_Gain_ktvhkcxsc3 * 0.0 ; rtB . g34gfrsv4f [ 5 ] = rtP .
Gain4_Gain_ktvhkcxsc3 * 0.0 ; rtB . g34gfrsv4f [ 6 ] = rtP .
Gain4_Gain_ktvhkcxsc3 * 0.0 ; rtB . g34gfrsv4f [ 7 ] = rtP .
Gain4_Gain_ktvhkcxsc3 * 0.0 ; rtB . g34gfrsv4f [ 8 ] = rtP .
Gain4_Gain_ktvhkcxsc3 * 0.0 ; rtB . g34gfrsv4f [ 9 ] = rtP .
Gain4_Gain_ktvhkcxsc3 * 0.0 ; rtB . g34gfrsv4f [ 10 ] = rtP .
Gain4_Gain_ktvhkcxsc3 * 0.0 ; rtB . etdgtwcj3w = muDoubleScalarCos ( rtB .
g34gfrsv4f [ 0 ] ) ; rtB . m3awtjmx3j = rtB . oqy5z3a4ld [ 0 ] * rtB .
etdgtwcj3w ; rtB . l2guhgov2q = rtB . m3awtjmx3j * rtB . m3awtjmx3j ; rtB .
iu0swzzeqc = muDoubleScalarSin ( rtB . g34gfrsv4f [ 0 ] ) ; rtB . aj4u5ct1vk
= rtB . iu0swzzeqc * rtB . oqy5z3a4ld [ 0 ] ; rtB . gq2cdlbyip = rtB .
aj4u5ct1vk * rtB . aj4u5ct1vk ; rtB . oljuvu5tog = rtB . l2guhgov2q + rtB .
gq2cdlbyip ; rtB . okdt2zsqto = muDoubleScalarSqrt ( rtB . oljuvu5tog ) ; rtB
. p52rpnz5lm = ( rtB . okdt2zsqto > rtP . u9_Value_drtq23l3nx ) ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . jm024v2ke1 = rtDW . huxwzweeyi ; }
rtB . pummbgqsvi = false ; rtB . dgd5f4xhjb = ssGetT ( rtS ) ; on20slldft (
rtS , rtB . i0ajfkj3ns , rtB . pummbgqsvi , rtB . dgd5f4xhjb , rtP .
T1_delay_lvvr1mpyz0 , & rtB . dhpm1tyfmn , & rtDW . dhpm1tyfmn , & rtP .
dhpm1tyfmn , & rtPrevZCX . dhpm1tyfmn ) ; bhc3qgnuic ( rtS , rtB . eibnca50sv
, rtB . pummbgqsvi , rtB . dgd5f4xhjb , rtP . T1_delay_lvvr1mpyz0 , & rtB .
grby3szfkd , & rtDW . grby3szfkd , & rtP . grby3szfkd , & rtPrevZCX .
grby3szfkd ) ; rtB . jbvzceqccg = ( rtB . dhpm1tyfmn . j1vu52vt5l || rtB .
grby3szfkd . mjqylog0rb ) ; rtB . mwmnvy0z2i = ( rtB . jm024v2ke1 || rtB .
jbvzceqccg ) ; rtB . hfzh3lqmuf = rtB . bvlgh3rhsj ; rtB . cv3o5vq2i2 =
ssGetT ( rtS ) ; on20slldft ( rtS , rtB . fxn1ue5bba , rtB . hfzh3lqmuf , rtB
. cv3o5vq2i2 , rtP . T2_delay_jay4pfmrqk , & rtB . nvltfn14rj , & rtDW .
nvltfn14rj , & rtP . nvltfn14rj , & rtPrevZCX . nvltfn14rj ) ; bhc3qgnuic (
rtS , rtB . dwaxzbytpi , rtB . hfzh3lqmuf , rtB . cv3o5vq2i2 , rtP .
T2_delay_jay4pfmrqk , & rtB . dzcv3as4ol , & rtDW . dzcv3as4ol , & rtP .
dzcv3as4ol , & rtPrevZCX . dzcv3as4ol ) ; rtB . jyurjup5iq = ( rtB .
nvltfn14rj . j1vu52vt5l || rtB . dzcv3as4ol . mjqylog0rb ) ; rtB . px130y0tj2
= ! rtB . jyurjup5iq ; rtB . nponljxli4 = ( rtB . mwmnvy0z2i && rtB .
px130y0tj2 ) ; rtB . f4chbkcuvv = ( rtB . p52rpnz5lm && rtB . nponljxli4 ) ;
if ( rtB . nponljxli4 ) { rtB . o3zin1t0m3 = rtB . fwbuvpwccv ; } else { rtB
. o3zin1t0m3 = rtP . u8_Value_iwy3i5jn2w ; } rtB . hfoabgbiq1 = rtB .
m3awtjmx3j * rtB . o3zin1t0m3 ; rtB . fqkhgsclyj = rtB . aj4u5ct1vk * rtB .
o3zin1t0m3 ; rtB . k1zwdq4dzw = 0.0 * rtB . o3zin1t0m3 ; rtB . i0ijwcecit =
0.0 * rtB . o3zin1t0m3 ; rtB . l1ta5gisrn = 0.0 * rtB . o3zin1t0m3 ; rtB .
pezwguzs1x = 0.0 * rtB . o3zin1t0m3 ; if ( rtB . ckw23s2vuu ) { rtB .
nvkld4bna4 = rtB . pzaolrfodg ; } else { rtB . nvkld4bna4 = rtP .
u2_Value_eorae2zjq2 ; } rtB . nclsxiokqk = 0.0 * rtB . nvkld4bna4 ; rtB .
idqudzzqfz = 0.0 * rtB . nvkld4bna4 ; rtB . nnjzrqjkxt = 0.0 * rtB .
nvkld4bna4 ; rtB . o2agm5t2ko = 0.0 * rtB . nvkld4bna4 ; rtB . ay5btobub2 =
0.0 * rtB . nvkld4bna4 ; rtB . lht5w4kdxn = 0.0 * rtB . nvkld4bna4 ; rtB .
g5vibvrfsw = ! rtB . nponljxli4 ; rtB . foo430jucv = ! rtB . ckw23s2vuu ; rtB
. oueknrwpjx = ( rtB . g5vibvrfsw && rtB . foo430jucv ) ; rtB . ndaxowfaeo =
rtB . hfoabgbiq1 + rtB . nclsxiokqk ; rtB . k2ptfagtj1 = rtB . fqkhgsclyj +
rtB . idqudzzqfz ; rtB . frsyfvc2nn = rtB . k1zwdq4dzw + rtB . nnjzrqjkxt ;
rtB . flpzz4xz1a = rtB . i0ijwcecit + rtB . o2agm5t2ko ; rtB . jgsnuktpsz =
rtB . l1ta5gisrn + rtB . ay5btobub2 ; rtB . m00z4oneqc = rtB . pezwguzs1x +
rtB . lht5w4kdxn ; rtB . o4xgindkq4 = ( rtB . c2nts3pzpu && rtB . nponljxli4
) ; rtB . ktjha1cgoi = ( rtB . ey2r001gvf && rtB . nponljxli4 ) ; rtB .
lh00t34sm0 = ( rtB . nponljxli4 && rtB . ckw23s2vuu ) ; if ( rtB . oueknrwpjx
) { rtB . a4kas4rylq = rtP . u7_Value_bsqbrgzmjl ; rtB . jeujuheiok = rtP .
u7_Value_bsqbrgzmjl ; rtB . jsak3auyyb = rtP . u7_Value_bsqbrgzmjl ; } else {
if ( rtB . lh00t34sm0 ) { rtB . cibnuj3r1d = ( rtB . f4chbkcuvv || rtB .
driongi2gk ) ; rtB . gaqccckflg = ( rtB . cibnuj3r1d && rtB . lh00t34sm0 ) ;
rtB . kzbqkap0ys = rtB . gaqccckflg ; } else { if ( rtB . nponljxli4 ) { rtB
. bxqpidnnpo = rtB . f4chbkcuvv ; } else { rtB . bxqpidnnpo = rtB .
driongi2gk ; } rtB . kzbqkap0ys = rtB . bxqpidnnpo ; } rtB . a4kas4rylq = rtB
. kzbqkap0ys ; if ( rtB . lh00t34sm0 ) { rtB . nhedo3gbx5 = ( rtB .
o4xgindkq4 || rtB . pio4hbpadl ) ; rtB . iq4dg13kqw = ( rtB . nhedo3gbx5 &&
rtB . lh00t34sm0 ) ; rtB . icyiw2ydz5 = rtB . iq4dg13kqw ; } else { if ( rtB
. nponljxli4 ) { rtB . eeloodelku = rtB . o4xgindkq4 ; } else { rtB .
eeloodelku = rtB . pio4hbpadl ; } rtB . icyiw2ydz5 = rtB . eeloodelku ; } rtB
. jeujuheiok = rtB . icyiw2ydz5 ; if ( rtB . lh00t34sm0 ) { rtB . aay4zxv0dg
= ( rtB . ktjha1cgoi || rtB . c45mn4j204 ) ; rtB . h2o2mylh0g = ( rtB .
aay4zxv0dg && rtB . lh00t34sm0 ) ; rtB . haoraik2d1 = rtB . h2o2mylh0g ; }
else { if ( rtB . nponljxli4 ) { rtB . l1ua5bxsol = rtB . ktjha1cgoi ; } else
{ rtB . l1ua5bxsol = rtB . c45mn4j204 ; } rtB . haoraik2d1 = rtB . l1ua5bxsol
; } rtB . jsak3auyyb = rtB . haoraik2d1 ; } if ( rtB . lh00t34sm0 ) { rtB .
d3waajrccu = rtB . nvmto2zef3 ; } else { if ( rtB . nponljxli4 ) { rtB .
cntfplxzu2 = rtB . nmdunsunog ; } else { rtB . cntfplxzu2 = rtB . hj12c0u10s
; } rtB . d3waajrccu = rtB . cntfplxzu2 ; } rtB . guyue4ym0u = ( ( rtB .
a4kas4rylq != 0.0 ) && ( rtB . jeujuheiok != 0.0 ) && ( rtB . jsak3auyyb !=
0.0 ) ) ; rtB . fo3ate04uw = 0.0 * rtB . o3zin1t0m3 ; rtB . iqad4ij3ix = rtB
. fo3ate04uw / rtP . u0_Value_a4nxwnpij3 ; rtB . elo4nj5u4f = 0.0 * rtB .
o3zin1t0m3 ; rtB . ld2rktxf3x = rtB . elo4nj5u4f / rtP . u0_Value_a4nxwnpij3
; rtB . ld50h0xnwj = 0.0 * rtB . nvkld4bna4 ; rtB . evliept3oo = 0.0 * rtB .
nvkld4bna4 ; rtB . mt5vmlrirg = rtB . fo3ate04uw + rtB . ld50h0xnwj ; rtB .
i3bxo0he01 = rtB . mt5vmlrirg / rtP . u7_Value_f5nzf0vct1 ; rtB . nfpuszfqmt
= rtB . elo4nj5u4f + rtB . evliept3oo ; rtB . isls2hahuw = rtB . nfpuszfqmt /
rtP . u7_Value_f5nzf0vct1 ; rtB . hftt22dxk0 = rtB . ld50h0xnwj / rtP .
u2_Value_cdrscqftiw ; rtB . behbaojvzo = rtB . evliept3oo / rtP .
u2_Value_cdrscqftiw ; rtB . mzzlzqrnpb = ( rtB . hvudnvvnmj && rtB .
nponljxli4 ) ; rtB . hr5pqrvxsl = ( rtB . fbsjnue34g && rtB . ckw23s2vuu ) ;
if ( rtB . oueknrwpjx ) { rtB . ikxa4ghpd1 = rtP . u7_Value_bsqbrgzmjl ; }
else { if ( rtB . lh00t34sm0 ) { rtB . ddrfb2g53a = ( rtB . mzzlzqrnpb || rtB
. hr5pqrvxsl ) ; rtB . aaklblxpvs = ( rtB . ddrfb2g53a && rtB . lh00t34sm0 )
; rtB . hgukaccn2s = rtB . aaklblxpvs ; } else { if ( rtB . nponljxli4 ) {
rtB . lnngmrcbhz = rtB . mzzlzqrnpb ; } else { rtB . lnngmrcbhz = rtB .
hr5pqrvxsl ; } rtB . hgukaccn2s = rtB . lnngmrcbhz ; } rtB . ikxa4ghpd1 = rtB
. hgukaccn2s ; } rtB . kyjue3cgfl = ( rtB . f4chbkcuvv && rtB . o4xgindkq4 &&
rtB . ktjha1cgoi ) ; rtB . mcxcnwu4uv = ( rtB . driongi2gk && rtB .
pio4hbpadl && rtB . c45mn4j204 ) ; rtB . fj5rejmbra = rtB . ndaxowfaeo * rtB
. ndaxowfaeo ; rtB . bra1mfkmph = rtB . k2ptfagtj1 * rtB . k2ptfagtj1 ; rtB .
i20haivvca = rtB . fj5rejmbra + rtB . bra1mfkmph ; rtB . ovzx3oqa04 =
muDoubleScalarSqrt ( rtB . i20haivvca ) ; if ( rtP .
Constant_Value_li4jgwm4zn > rtP . u_Threshold_b5azafx2zl ) { rtB . lh5s1lvb0i
= ( rtB . ovzx3oqa04 < rtP . Constant3_Value_d4qswynlcj ) ; rtB . naflf54ifk
= rtB . lh5s1lvb0i ; } else { rtB . kcqiejd2oh = ( rtB . ovzx3oqa04 > rtP .
Constant3_Value_d4qswynlcj ) ; rtB . naflf54ifk = rtB . kcqiejd2oh ; } rtB .
ok5qqqln1m = ( rtB . naflf54ifk && rtB . bt0130pijc ) ; if ( rtB . fksjfhc3xx
) { rtB . fymmjzo24c = rtB . ok5qqqln1m ; } else { rtB . egvxfcnixt = ( rtB .
ok5qqqln1m && rtB . i04s1r5t2j ) ; rtB . fymmjzo24c = rtB . egvxfcnixt ; } if
( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . p1yd4ksydw = rtDW . fwpmmk23o2 ;
rtB . evj2qonj1n = ! rtB . p1yd4ksydw ; } rtB . aesnqvhx52 = ( rtB .
fymmjzo24c && rtB . evj2qonj1n ) ; rtB . pmmr45ob4e = ssGetT ( rtS ) ; rtB .
dvh33eeffn = ( rtB . fymmjzo24c && rtB . mk3unqym3s ) ; rtB . gcwfhyhhnw =
rtB . dvh33eeffn ; on20slldft ( rtS , rtB . ks4my0k3sr , rtB . gcwfhyhhnw ,
rtB . pmmr45ob4e , rtP . DT1_delay , & rtB . l2r2uh0izk , & rtDW . l2r2uh0izk
, & rtP . l2r2uh0izk , & rtPrevZCX . l2r2uh0izk ) ; bhc3qgnuic ( rtS , rtB .
dbamzcq2yi , rtB . gcwfhyhhnw , rtB . pmmr45ob4e , rtP . DT1_delay , & rtB .
feptx2ayjl , & rtDW . feptx2ayjl , & rtP . feptx2ayjl , & rtPrevZCX .
feptx2ayjl ) ; rtB . pkcgndbb1n = ( rtB . l2r2uh0izk . j1vu52vt5l || rtB .
feptx2ayjl . mjqylog0rb ) ; rtB . jzynfhllyb = ssGetT ( rtS ) ; rtB .
bgynuvbh0z = false ; rtB . n0ocottp4e = rtB . bgynuvbh0z ; on20slldft ( rtS ,
rtB . okz3124rda , rtB . n0ocottp4e , rtB . jzynfhllyb , rtP . DT2_delay , &
rtB . labvjdxfi3 , & rtDW . labvjdxfi3 , & rtP . labvjdxfi3 , & rtPrevZCX .
labvjdxfi3 ) ; bhc3qgnuic ( rtS , rtB . dqmtufmhv1 , rtB . n0ocottp4e , rtB .
jzynfhllyb , rtP . DT2_delay , & rtB . nvnct41vnp , & rtDW . nvnct41vnp , &
rtP . nvnct41vnp , & rtPrevZCX . nvnct41vnp ) ; rtB . cjhkfc0kbt = ( rtB .
labvjdxfi3 . j1vu52vt5l || rtB . nvnct41vnp . mjqylog0rb ) ; rtB . kiqdizjisp
= ( rtB . pkcgndbb1n || rtB . cjhkfc0kbt ) ; rtB . krwwdr5b5g =
muDoubleScalarSin ( rtP . sinwt_Freq_nxiuov2jpu * ssGetTaskTime ( rtS , 0 ) +
rtP . sinwt_Phase_pewgdi2sgc ) * rtP . sinwt_Amp_or1w5rembs + rtP .
sinwt_Bias_p0jrktsdpe ; rtB . ihpo1wu1st = rtB . m3ur2q15em [ 0 ] * rtB .
krwwdr5b5g ; rtB . kaaavz1uxk = muDoubleScalarSin ( rtP .
coswt_Freq_aoisvgkt21 * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_pm3wwl21k0 ) * rtP . coswt_Amp_evdgkphrye + rtP .
coswt_Bias_ac4rx1igwk ; rtB . pdkipll4oh = rtB . m3ur2q15em [ 0 ] * rtB .
kaaavz1uxk ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . a5ymymfmoc =
ssGetTaskTime ( rtS , 1 ) ; rtB . c4yjoz04xm = rt_Lookup ( rtP .
LookUpTable_XData_eiy23gy3gy , 6 , rtB . a5ymymfmoc , rtP .
LookUpTable_YData_g2vrwcmh4s ) ; if ( rtP . C4_Value_crx1hodrbk >= rtP .
Switch3_Threshold_c50cjk4nsx ) { rtB . hziydbsdw3 = rtP . com_Value ; } else
{ rtB . hziydbsdw3 = rtB . c4yjoz04xm ; } rtB . jpmepnle3u = ( rtB .
hziydbsdw3 != 0.0 ) ; rtB . ca4aftfngg = ssGetTaskTime ( rtS , 1 ) ; if ( rtP
. C4_Value >= rtP . Switch3_Threshold_pefthirxz5 ) { rtB . gqaaipnp42 = ( rtB
. jpmepnle3u && rtB . cahx5tm0wg ) ; rtB . ckq4vfn0d5 = rtB . gqaaipnp42 ;
rtB . drqwghyiul = rtB . ckq4vfn0d5 ; rtB . c0mh0lrtzp = rtB . drqwghyiul ; }
else { rtB . p0oxb5qck5 = rt_Lookup ( rtP . LookUpTable_XData , 5 , rtB .
ca4aftfngg , rtP . LookUpTable_YData ) ; rtB . c0mh0lrtzp = rtB . p0oxb5qck5
; } rtB . ocbrdch0j3 = ssGetTaskTime ( rtS , 1 ) ; if ( rtP .
C4_Value_dj4jymukkx >= rtP . Switch3_Threshold_n2ziqwbv3f ) { rtB .
im4cxs2qc2 = ( rtB . jpmepnle3u && rtB . gqiym5ozq1 ) ; rtB . ollknphhvk =
rtB . im4cxs2qc2 ; rtB . jxjv0q3j1e = rtB . ollknphhvk ; rtB . fbm1wk5xyr =
rtB . jxjv0q3j1e ; } else { rtB . cusewujfjq = rt_Lookup ( rtP .
LookUpTable_XData_a0dgjxxdxd , 5 , rtB . ocbrdch0j3 , rtP .
LookUpTable_YData_g3a4c2ijqm ) ; rtB . fbm1wk5xyr = rtB . cusewujfjq ; } rtB
. jirn1l1m0v = ssGetTaskTime ( rtS , 1 ) ; if ( rtP . C4_Value_evs3s3l0qd >=
rtP . Switch3_Threshold_nzszlp1rz0 ) { rtB . e1brirxt0j = ( rtB . jpmepnle3u
&& rtB . gncgbkiuyu ) ; rtB . hoqjp3o3ea = rtB . e1brirxt0j ; rtB .
gkxbzd4tzo = rtB . hoqjp3o3ea ; rtB . l3lfgi5op5 = rtB . gkxbzd4tzo ; } else
{ rtB . llhcc3e5pl = rt_Lookup ( rtP . LookUpTable_XData_ck4ovjve3u , 5 , rtB
. jirn1l1m0v , rtP . LookUpTable_YData_oht2ajekg2 ) ; rtB . l3lfgi5op5 = rtB
. llhcc3e5pl ; } rtB . c3nicjswpi = ssGetTaskTime ( rtS , 1 ) ; rtB .
hxmui1pdxg = rt_Lookup ( rtP . LookUpTable_XData_bnrj03i2yz , 6 , rtB .
c3nicjswpi , rtP . LookUpTable_YData_ihgamzfpom ) ; if ( rtP .
C4_Value_dt01ijjkxx >= rtP . Switch3_Threshold_h54fuhg32s ) { rtB .
okcswcmxk1 = rtP . com_Value_h4005bftuq ; } else { rtB . okcswcmxk1 = rtB .
hxmui1pdxg ; } rtB . ecs4zjsuan = ( rtB . okcswcmxk1 != 0.0 ) ; rtB .
mmkk2pkfau = ssGetTaskTime ( rtS , 1 ) ; if ( rtP . C4_Value_fjqffeohhm >=
rtP . Switch3_Threshold_nzab0okbys ) { rtB . bqdrzh35jm = ( rtB . ecs4zjsuan
&& rtB . mlcvflupnu ) ; rtB . mnmmr24kpi = rtB . bqdrzh35jm ; rtB .
d5zmhjq3du = rtB . mnmmr24kpi ; rtB . ipfzduy1yk = rtB . d5zmhjq3du ; } else
{ rtB . neqjs15xn3 = rt_Lookup ( rtP . LookUpTable_XData_m30u40nt5x , 5 , rtB
. mmkk2pkfau , rtP . LookUpTable_YData_huyxtwcyos ) ; rtB . ipfzduy1yk = rtB
. neqjs15xn3 ; } rtB . pxqzajkvqe = ssGetTaskTime ( rtS , 1 ) ; if ( rtP .
C4_Value_h3ehbrtu12 >= rtP . Switch3_Threshold_hy0dhy3bup ) { rtB .
i0yhnoiarp = ( rtB . ecs4zjsuan && rtB . jz0eiv3n4w ) ; rtB . knea1u0h5m =
rtB . i0yhnoiarp ; rtB . g4dn0pm1lw = rtB . knea1u0h5m ; rtB . juaggbpzrf =
rtB . g4dn0pm1lw ; } else { rtB . esxurjn4ni = rt_Lookup ( rtP .
LookUpTable_XData_hyynx54i2k , 5 , rtB . pxqzajkvqe , rtP .
LookUpTable_YData_mnn2add1ai ) ; rtB . juaggbpzrf = rtB . esxurjn4ni ; } rtB
. hhx5tfrzab = ssGetTaskTime ( rtS , 1 ) ; if ( rtP . C4_Value_i20ma1tdj2 >=
rtP . Switch3_Threshold_e04cahsutl ) { rtB . cc1uqqh0bm = ( rtB . ecs4zjsuan
&& rtB . ixn4i5wf1o ) ; rtB . fprxlehlfz = rtB . cc1uqqh0bm ; rtB .
ghiegai4zs = rtB . fprxlehlfz ; rtB . mjjqenybvd = rtB . ghiegai4zs ; } else
{ rtB . jv3njlwbc0 = rt_Lookup ( rtP . LookUpTable_XData_dyn5fyt25r , 5 , rtB
. hhx5tfrzab , rtP . LookUpTable_YData_f3ltrtnohw ) ; rtB . mjjqenybvd = rtB
. jv3njlwbc0 ; } rtB . gutroqswik = rtP . donotdeletethisgain_Gain_bppwyuavz3
* rtB . hwsskqat2q [ 12 ] ; rtB . o5j3enlqop = rtP .
donotdeletethisgain_Gain_pxh2ienp0j * rtB . hwsskqat2q [ 13 ] ; rtB .
frwbp0l5ho = rtP . donotdeletethisgain_Gain_baijbpvqmu * rtB . hwsskqat2q [
14 ] ; rtB . pkxweqgnvi [ 0 ] = rtP . Kv1_Gain_bnwyxrhgaq * rtB . gutroqswik
; rtB . pkxweqgnvi [ 1 ] = rtP . Kv1_Gain_bnwyxrhgaq * rtB . o5j3enlqop ; rtB
. pkxweqgnvi [ 2 ] = rtP . Kv1_Gain_bnwyxrhgaq * rtB . frwbp0l5ho ; rtB .
ee4vrspw33 = rtP . donotdeletethisgain_Gain_b11zjsc3cf * rtB . hwsskqat2q [
36 ] ; rtB . opmiif52wh = rtP . donotdeletethisgain_Gain_ko15fryvio * rtB .
hwsskqat2q [ 37 ] ; rtB . iimoz45uet = rtP .
donotdeletethisgain_Gain_doamnd0rn3 * rtB . hwsskqat2q [ 38 ] ; rtB .
i03p2gj3lj [ 0 ] = rtP . Kv_Gain_oagceli1w0 * rtB . ee4vrspw33 ; rtB .
i03p2gj3lj [ 1 ] = rtP . Kv_Gain_oagceli1w0 * rtB . opmiif52wh ; rtB .
i03p2gj3lj [ 2 ] = rtP . Kv_Gain_oagceli1w0 * rtB . iimoz45uet ; rtB .
pjtnbnrrjd = rtP . donotdeletethisgain_Gain_el5t1tcmib * rtB . hwsskqat2q [
15 ] ; rtB . dz4zvmkicu = rtP . donotdeletethisgain_Gain_ipt2xbsiua * rtB .
hwsskqat2q [ 16 ] ; rtB . dhrfhgeczt = rtP .
donotdeletethisgain_Gain_m0aumtn540 * rtB . hwsskqat2q [ 17 ] ; rtB .
aagme3a21n [ 0 ] = rtP . Kv1_Gain_a11rdes31z * rtB . pjtnbnrrjd ; rtB .
aagme3a21n [ 1 ] = rtP . Kv1_Gain_a11rdes31z * rtB . dz4zvmkicu ; rtB .
aagme3a21n [ 2 ] = rtP . Kv1_Gain_a11rdes31z * rtB . dhrfhgeczt ; rtB .
kpp11ownkv = rtP . donotdeletethisgain_Gain_mihbuej5uh * rtB . hwsskqat2q [
39 ] ; rtB . iol2nkxnqw = rtP . donotdeletethisgain_Gain_ftryll3qhy * rtB .
hwsskqat2q [ 40 ] ; rtB . nfno1ghp0c = rtP .
donotdeletethisgain_Gain_c3lcfk1043 * rtB . hwsskqat2q [ 41 ] ; rtB .
inv0rzec2h [ 0 ] = rtP . Kv_Gain_pazndqhdq4 * rtB . kpp11ownkv ; rtB .
inv0rzec2h [ 1 ] = rtP . Kv_Gain_pazndqhdq4 * rtB . iol2nkxnqw ; rtB .
inv0rzec2h [ 2 ] = rtP . Kv_Gain_pazndqhdq4 * rtB . nfno1ghp0c ; rtB .
g1s5yjcdkh = rtP . donotdeletethisgain_Gain_fivm3xvrsj * rtB . hwsskqat2q [
18 ] ; rtB . h50plr4wps = rtP . donotdeletethisgain_Gain_brgh00idj2 * rtB .
hwsskqat2q [ 19 ] ; rtB . j3oqqdiikz = rtP .
donotdeletethisgain_Gain_h1fnmcafla * rtB . hwsskqat2q [ 20 ] ; rtB .
aakvecnouz [ 0 ] = rtP . Kv1_Gain_obbtynvmpl * rtB . g1s5yjcdkh ; rtB .
aakvecnouz [ 1 ] = rtP . Kv1_Gain_obbtynvmpl * rtB . h50plr4wps ; rtB .
aakvecnouz [ 2 ] = rtP . Kv1_Gain_obbtynvmpl * rtB . j3oqqdiikz ; rtB .
fdhflawuuz = rtP . donotdeletethisgain_Gain_n4uqma12ag * rtB . hwsskqat2q [
30 ] ; rtB . kw30hb52la = rtP . donotdeletethisgain_Gain_dfrrwvtmt2 * rtB .
hwsskqat2q [ 31 ] ; rtB . m2f4k4lfyt = rtP .
donotdeletethisgain_Gain_owd5yngu54 * rtB . hwsskqat2q [ 32 ] ; rtB .
addkl1evhb [ 0 ] = rtP . Kv_Gain_bka3xcqmiz * rtB . fdhflawuuz ; rtB .
addkl1evhb [ 1 ] = rtP . Kv_Gain_bka3xcqmiz * rtB . kw30hb52la ; rtB .
addkl1evhb [ 2 ] = rtP . Kv_Gain_bka3xcqmiz * rtB . m2f4k4lfyt ; rtB .
jraih14e02 = rtP . donotdeletethisgain_Gain_p2fmce1qyv * rtB . hwsskqat2q [ 9
] ; rtB . bxsct0gdft = rtP . donotdeletethisgain_Gain_avjhyeieoh * rtB .
hwsskqat2q [ 10 ] ; rtB . kimnxp4no2 = rtP .
donotdeletethisgain_Gain_ejfvlvedhy * rtB . hwsskqat2q [ 11 ] ; rtB .
hyu0j3yaa0 [ 0 ] = rtP . Kv1_Gain_a2iei2nvcs * rtB . jraih14e02 ; rtB .
hyu0j3yaa0 [ 1 ] = rtP . Kv1_Gain_a2iei2nvcs * rtB . bxsct0gdft ; rtB .
hyu0j3yaa0 [ 2 ] = rtP . Kv1_Gain_a2iei2nvcs * rtB . kimnxp4no2 ; rtB .
ehdy5fprtp = rtP . donotdeletethisgain_Gain_lfj3y0efqb * rtB . hwsskqat2q [
42 ] ; rtB . ogk0z5okix = rtP . donotdeletethisgain_Gain_mlkx33f1ek * rtB .
hwsskqat2q [ 43 ] ; rtB . f21jt1khws = rtP .
donotdeletethisgain_Gain_i3amwau1ac * rtB . hwsskqat2q [ 44 ] ; rtB .
ntifjkuoc1 [ 0 ] = rtP . Kv_Gain_afmlcwxcxx * rtB . ehdy5fprtp ; rtB .
ntifjkuoc1 [ 1 ] = rtP . Kv_Gain_afmlcwxcxx * rtB . ogk0z5okix ; rtB .
ntifjkuoc1 [ 2 ] = rtP . Kv_Gain_afmlcwxcxx * rtB . f21jt1khws ; rtB .
e54qt2efwf = rtP . donotdeletethisgain_Gain_oirag3juk1 * rtB . hwsskqat2q [
21 ] ; rtB . p2t01mi3lq = rtP . donotdeletethisgain_Gain_mqdl22ho2j * rtB .
hwsskqat2q [ 22 ] ; rtB . luepljy2rn = rtP .
donotdeletethisgain_Gain_lkevjxskuc * rtB . hwsskqat2q [ 23 ] ; rtB .
n12bxunbwk [ 0 ] = rtP . Kv1_Gain_fynfzvmte1 * rtB . e54qt2efwf ; rtB .
n12bxunbwk [ 1 ] = rtP . Kv1_Gain_fynfzvmte1 * rtB . p2t01mi3lq ; rtB .
n12bxunbwk [ 2 ] = rtP . Kv1_Gain_fynfzvmte1 * rtB . luepljy2rn ; rtB .
gatpnqo45n = rtP . donotdeletethisgain_Gain_ishclfmdim * rtB . hwsskqat2q [
27 ] ; rtB . eapoxq4dy2 = rtP . donotdeletethisgain_Gain_bryyhtwaj5 * rtB .
hwsskqat2q [ 28 ] ; rtB . fmmhmv51lq = rtP .
donotdeletethisgain_Gain_puqsku4btm * rtB . hwsskqat2q [ 29 ] ; rtB .
ibxwszc41l [ 0 ] = rtP . Kv_Gain_nvecevjm22 * rtB . gatpnqo45n ; rtB .
ibxwszc41l [ 1 ] = rtP . Kv_Gain_nvecevjm22 * rtB . eapoxq4dy2 ; rtB .
ibxwszc41l [ 2 ] = rtP . Kv_Gain_nvecevjm22 * rtB . fmmhmv51lq ; rtB .
hjekcnhak5 = rtP . donotdeletethisgain_Gain_ncdaf4gslr * rtB . hwsskqat2q [ 6
] ; rtB . gmbiduo4ez = rtP . donotdeletethisgain_Gain_b5bekhu3yw * rtB .
hwsskqat2q [ 7 ] ; rtB . flouh4ouwp = rtP .
donotdeletethisgain_Gain_dlevjdqkxe * rtB . hwsskqat2q [ 8 ] ; rtB .
lgfp5siese [ 0 ] = rtP . Kv1_Gain_etje0jwowd * rtB . hjekcnhak5 ; rtB .
lgfp5siese [ 1 ] = rtP . Kv1_Gain_etje0jwowd * rtB . gmbiduo4ez ; rtB .
lgfp5siese [ 2 ] = rtP . Kv1_Gain_etje0jwowd * rtB . flouh4ouwp ; rtB .
ee3of1nk1h = ssGetTaskTime ( rtS , 1 ) ; rtB . opbbsowrec = rt_Lookup ( rtP .
LookUpTable_XData_jqj2achouu , 6 , rtB . ee3of1nk1h , rtP .
LookUpTable_YData_ltia20sdgh ) ; if ( rtP . C4_Value_ow2hft4fh0 >= rtP .
Switch3_Threshold_icq4wlzdb5 ) { rtB . i2lrfidcno = rtP .
com_Value_nijy41ftvj ; } else { rtB . i2lrfidcno = rtB . opbbsowrec ; } rtB .
magv5vbngs = ssGetTaskTime ( rtS , 1 ) ; if ( rtP . C4_Value_h1brvwyqzj >=
rtP . Switch3_Threshold_l1z4ennhbv ) { if ( rtP . Q2_SwitchA >= rtP .
Switch_Threshold ) { rtB . k4mzt0wgrv = rtB . i2lrfidcno ; } else { rtB .
k4mzt0wgrv = rtP . Constant5_Value_nrt04fs2oi ; } rtB . h44q1uizkp = rtB .
k4mzt0wgrv ; rtB . ilmgds3nyj = rtB . h44q1uizkp ; } else { rtB . lv4ap4dhcm
= rt_Lookup ( rtP . LookUpTable_XData_lmnj2bp4dj , 5 , rtB . magv5vbngs , rtP
. LookUpTable_YData_ihlyfpzseq ) ; rtB . ilmgds3nyj = rtB . lv4ap4dhcm ; }
rtB . m0rfha4rf0 = ssGetTaskTime ( rtS , 1 ) ; if ( rtP . C4_Value_coojxeh1yn
>= rtP . Switch3_Threshold_pxvvzcq34y ) { if ( rtP . Q2_SwitchB >= rtP .
Switch1_Threshold ) { rtB . fbvbbnybhp = rtB . i2lrfidcno ; } else { rtB .
fbvbbnybhp = rtP . Constant5_Value_nrt04fs2oi ; } rtB . hdhzy03yc2 = rtB .
fbvbbnybhp ; rtB . bncuow3blf = rtB . hdhzy03yc2 ; } else { rtB . eybstaavyy
= rt_Lookup ( rtP . LookUpTable_XData_ewxwkoq2bu , 5 , rtB . m0rfha4rf0 , rtP
. LookUpTable_YData_pr32bfyttn ) ; rtB . bncuow3blf = rtB . eybstaavyy ; }
rtB . ixevofzgef = ssGetTaskTime ( rtS , 1 ) ; if ( rtP . C4_Value_litcp0wkqb
>= rtP . Switch3_Threshold_nopzqpc1im ) { if ( rtP . Q2_SwitchC >= rtP .
Switch2_Threshold ) { rtB . or0dttgfz3 = rtB . i2lrfidcno ; } else { rtB .
or0dttgfz3 = rtP . Constant5_Value_nrt04fs2oi ; } rtB . crxyeiszff = rtB .
or0dttgfz3 ; rtB . brjdxprewh = rtB . crxyeiszff ; } else { rtB . fvjksfcabf
= rt_Lookup ( rtP . LookUpTable_XData_lcf4u2xkc5 , 5 , rtB . ixevofzgef , rtP
. LookUpTable_YData_dhnbxbi4km ) ; rtB . brjdxprewh = rtB . fvjksfcabf ; } }
UNUSED_PARAMETER ( tid ) ; } void MdlOutputsTID2 ( int_T tid ) { real_T minV
; rtB . oxfyqex5ti = rtP . u0_Gain * 0.0 ; rtB . m3ur2q15em [ 0 ] = rtP .
u_Gain_n1yodizxlo * 0.0 ; rtB . m3ur2q15em [ 1 ] = rtP . u_Gain_n1yodizxlo *
0.0 ; rtB . m3ur2q15em [ 2 ] = rtP . u_Gain_n1yodizxlo * 0.0 ; rtB .
dxdvgwcdud = rtP . Constant12_Value ; rtB . mmksh1zcgn = rtP .
Constant6_Value - rtP . Constant8_Value ; rtB . l44t1whgkz = rtP .
Constant6_Value + rtP . Constant8_Value ; rtB . leqzt1thse = rtB . mmksh1zcgn
* rtP . Constant7_Value ; rtB . hjyco0tz1d = rtB . l44t1whgkz * rtP .
Constant7_Value ; rtB . bfbj0ob44k = rtP . Constant1_Value * rtP .
Constant4_Value_erw10tjqvc ; rtB . monh4vwypq = muDoubleScalarCos ( rtB .
bfbj0ob44k ) ; rtB . dle0ixlsdj = muDoubleScalarSin ( rtB . bfbj0ob44k ) ;
rtB . g2t4aege51 = rtB . dle0ixlsdj * rtP . Constant1_Value_ars5zi0cns ; rtB
. pyj5nx5nsh = rtP . Constant2_Value * rtP . Constant5_Value_fxy0c4lode ; rtB
. nmm0a5pfph = rtP . Constant1_Value * rtP . Constant4_Value_la2pqcbuj3 ; rtB
. bwwkqjxo4k = muDoubleScalarCos ( rtB . nmm0a5pfph ) ; rtB . ouj0dtqbog =
muDoubleScalarSin ( rtB . nmm0a5pfph ) ; rtB . hzhlqhdkme = rtB . ouj0dtqbog
* rtP . Constant1_Value_bzr4l5r5kb ; rtB . l0bpjfzzyt = rtP . Constant2_Value
* rtP . Constant5_Value_e4quuoivpw ; rtB . awq34lg42p = rtP . Constant1_Value
* rtP . Constant4_Value_av3emyql4f ; rtB . exnsgi2eie = muDoubleScalarCos (
rtB . awq34lg42p ) ; rtB . fy4ddyair3 = muDoubleScalarSin ( rtB . awq34lg42p
) ; rtB . pq0gfxrlwz = rtB . fy4ddyair3 * rtP . Constant1_Value_flj5imooog ;
rtB . ii5d3e3cbj = rtP . Constant2_Value * rtP . Constant5_Value_am3wv5nhug ;
rtB . lq0oe5npsb = true ; rtB . a5udxoo0de = false ; rtB . ebmfchpfdp = ! rtB
. a5udxoo0de ; rtB . eveilmf1ep = ( rtP . u0_Value_baausp0kjz == rtP .
u0_Value_fkcombbzh2 ) ; rtB . pyil1bkh2d = ( rtP . u5_Value_gphznhvgln == rtP
. u0_Value_fkcombbzh2 ) ; rtB . mhyy2ycg3l = ( rtB . eveilmf1ep || rtB .
pyil1bkh2d ) ; rtB . nw2iqmudaa = rtP . u0_Value_baausp0kjz / rtP .
u5_Value_gphznhvgln ; if ( rtB . mhyy2ycg3l ) { rtB . mcrpzdehxj = rtP .
u9_Value_dmhpyt3xvy ; } else { rtB . mcrpzdehxj = rtB . nw2iqmudaa ; } rtB .
ld2hdivymo = 0.0 * rtB . mcrpzdehxj ; rtB . jh3sbazso2 = rtB . ld2hdivymo /
rtP . u0_Value ; rtB . fvfiazyb2i = 0.0 * rtB . mcrpzdehxj ; rtB . od4ri1yxy5
= rtB . fvfiazyb2i / rtP . u0_Value ; rtB . ok23gz4w5z = 0.0 * rtB .
mcrpzdehxj ; rtB . m3tfc0xulv = rtB . ok23gz4w5z / rtP . u0_Value ; rtB .
h3kalxuzuj = 0.0 * rtB . mcrpzdehxj ; rtB . hhqlxrrv2w = rtB . h3kalxuzuj /
rtP . u0_Value ; rtB . ep3xeiifz4 = 0.0 ; rtB . lv5ghpc2jl = 0.0 ; rtB .
cpwzpneyrv = rtB . ep3xeiifz4 + rtB . lv5ghpc2jl ; rtB . gbk3a43bhp =
muDoubleScalarSqrt ( rtB . cpwzpneyrv ) ; rtB . ckyhekdsmw = ( rtB .
gbk3a43bhp > rtP . u6_Value ) ; rtB . j3ficanggh = 0.0 ; rtB . ekv1pni3cz =
0.0 ; rtB . cqcssv5rt3 = rtB . j3ficanggh + rtB . ekv1pni3cz ; rtB .
l3jchiign4 = muDoubleScalarSqrt ( rtB . cqcssv5rt3 ) ; rtB . gvygxvdyzl = (
rtB . l3jchiign4 > rtP . u6_Value ) ; rtB . psarde2hgs = ( rtB . ckyhekdsmw
&& rtB . gvygxvdyzl ) ; rtB . l1fjmhdjgk = ! rtB . psarde2hgs ; rtB .
pbsmjkaqoy = 0.0 * rtB . mcrpzdehxj ; rtB . an3nprh2tn = rtB . pbsmjkaqoy /
rtP . u0_Value ; rtB . io3zdqyvq4 = 0.0 * rtB . mcrpzdehxj ; rtB . cza42ci4ff
= rtB . io3zdqyvq4 / rtP . u0_Value ; rtB . fhjgyiwsrr = 0.0 ; rtB .
pgonam4zbi = 0.0 ; rtB . cpqc2pivaq = rtB . fhjgyiwsrr + rtB . pgonam4zbi ;
rtB . ddiqraw2tu = muDoubleScalarSqrt ( rtB . cpqc2pivaq ) ; rtB . hlazbfj0dg
= ( rtB . ddiqraw2tu > rtP . u6_Value ) ; rtB . edohrnysff = ! rtB .
hlazbfj0dg ; rtB . jla14acxcm = ! rtB . ckyhekdsmw ; rtB . opgogctltj = ! rtB
. gvygxvdyzl ; rtB . poyb3errh1 = ( rtB . edohrnysff && rtB . jla14acxcm &&
rtB . opgogctltj ) ; rtB . e4jkiqbccx = rtB . jh3sbazso2 * rtB . jh3sbazso2 ;
rtB . n4e3qi4ayd = rtB . hhqlxrrv2w * rtB . hhqlxrrv2w ; rtB . be3tdtnsjh =
rtB . e4jkiqbccx + rtB . n4e3qi4ayd ; rtB . npvaqugt30 = muDoubleScalarSqrt (
rtB . be3tdtnsjh ) ; rtB . pnwyhqwh1r = rtB . an3nprh2tn * rtB . an3nprh2tn ;
rtB . oxyas5agzm = rtB . cza42ci4ff * rtB . cza42ci4ff ; rtB . o4qpts1ngd =
rtB . pnwyhqwh1r + rtB . oxyas5agzm ; rtB . iefdfszoxf = muDoubleScalarSqrt (
rtB . o4qpts1ngd ) ; rtB . gwjkmuegg4 = rtB . od4ri1yxy5 * rtB . od4ri1yxy5 ;
rtB . jj3j0qcxjs = rtB . m3tfc0xulv * rtB . m3tfc0xulv ; rtB . awfox5mfzz =
rtB . gwjkmuegg4 + rtB . jj3j0qcxjs ; rtB . m2octsfxob = muDoubleScalarSqrt (
rtB . awfox5mfzz ) ; rtB . jac4p5k1xk = false ; rtB . al1juqksh0 = false ;
rtB . jknv4aevaf = ( rtB . al1juqksh0 || ( rtP . Constant13_Value != 0.0 ) ||
( rtP . Constant13_Value != 0.0 ) || ( rtP . Constant13_Value != 0.0 ) ) ;
rtB . miiinprolm = ( ( rtP . Constant12_Value_ctlh2xge1l != 0.0 ) || ( rtP .
Constant13_Value != 0.0 ) || ( rtP . Constant13_Value != 0.0 ) || ( rtP .
Constant13_Value != 0.0 ) ) ; rtB . mhn5qc45at = ! rtB . jknv4aevaf ; rtB .
lba2fk2nat = ! ( rtP . Constant13_Value != 0.0 ) ; rtB . mkhhlwrjox = ( rtB .
lba2fk2nat && ( rtP . Constant2_Value_msdtwhq3od != 0.0 ) ) ; rtB .
h1zlegqmxl = ( ( rtP . Constant1_Value_e31gzmwcyq != 0.0 ) && ( rtP .
Constant13_Value != 0.0 ) ) ; rtB . a20zwypiqw = ! ( rtP . Constant13_Value
!= 0.0 ) ; rtB . oreceynpdj = ( rtB . a20zwypiqw && ( rtP .
Constant2_Value_msdtwhq3od != 0.0 ) ) ; rtB . dyjklgcebd = ( rtB . h1zlegqmxl
|| rtB . oreceynpdj ) ; rtB . i5noetpmre = ( ( rtP .
Constant1_Value_e31gzmwcyq != 0.0 ) && ( rtP . Constant13_Value != 0.0 ) ) ;
rtB . o5zayjplnz = ! ( rtP . Constant13_Value != 0.0 ) ; rtB . n5bmbwlzvk = (
rtB . o5zayjplnz && ( rtP . Constant2_Value_msdtwhq3od != 0.0 ) ) ; rtB .
b4siiq1wlu = ( rtB . i5noetpmre || rtB . n5bmbwlzvk ) ; rtB . p32sdldake = (
( rtP . Constant1_Value_e31gzmwcyq != 0.0 ) && ( rtP . Constant13_Value !=
0.0 ) ) ; rtB . gy0rflgkbt = ( rtB . p32sdldake || rtB . mkhhlwrjox ) ; rtB .
pux0xcnljs = ! ( rtP . Constant13_Value != 0.0 ) ; rtB . gi0isra5ck = ! ( rtP
. Constant13_Value != 0.0 ) ; rtB . glipabwfiy = ! ( rtP . Constant13_Value
!= 0.0 ) ; rtB . pknlnjxfhy = ( rtB . al1juqksh0 || ( rtP .
Constant10_Value_jnr3xp20iu != 0.0 ) || ( rtP . Constant10_Value_jnr3xp20iu
!= 0.0 ) || ( rtP . Constant10_Value_jnr3xp20iu != 0.0 ) ) ; rtB . efmvrcrltk
= false ; rtB . meg2ajfbgr = ( rtB . efmvrcrltk || ( rtP .
Constant10_Value_jnr3xp20iu != 0.0 ) || ( rtP . Constant10_Value_jnr3xp20iu
!= 0.0 ) || ( rtP . Constant10_Value_jnr3xp20iu != 0.0 ) ) ; rtB . dlk5lnvt4s
= ( ( rtP . Constant_Value_lm1xva0tux != 0.0 ) || ( rtP .
Constant10_Value_jnr3xp20iu != 0.0 ) || ( rtP . Constant10_Value_jnr3xp20iu
!= 0.0 ) || ( rtP . Constant10_Value_jnr3xp20iu != 0.0 ) ) ; rtB . fb4vq42nl1
= ! rtB . pknlnjxfhy ; rtB . dmbkutmxus = ( rtB . dlk5lnvt4s && rtB .
fb4vq42nl1 ) ; rtB . khy1pynkz0 = false ; rtB . itkvmthppg = true ; rtB .
fotvnu0ocx = ( ( rtP . Constant2_Value_apx2o0ldkj != 0.0 ) && rtB .
itkvmthppg ) ; rtB . fmaf3i5fz1 = ( rtB . khy1pynkz0 || rtB . fotvnu0ocx ) ;
rtB . erufjgnylk = ( rtB . al1juqksh0 || ( rtP . Constant11_Value_bs3cayia3q
!= 0.0 ) || ( rtP . Constant11_Value_bs3cayia3q != 0.0 ) || ( rtP .
Constant11_Value_bs3cayia3q != 0.0 ) ) ; rtB . jgkx23fola = false ; rtB .
hwmfysoboh = ( rtB . jgkx23fola || ( rtP . Constant11_Value_bs3cayia3q != 0.0
) ) ; rtB . macrenionz = ( rtB . jac4p5k1xk || ( rtP .
Constant11_Value_bs3cayia3q != 0.0 ) ) ; rtB . onyt51xv2b = ! rtB .
erufjgnylk ; rtB . aazsjjcsgd = ! rtB . erufjgnylk ; rtB . czpm4yahhz = ! rtB
. erufjgnylk ; rtB . g5uya1grwm = ( rtP . T1_DelayType == rtP .
Constant_Value_gcgbaidbzv ) ; rtB . d35uvtgz4o = ! rtB . g5uya1grwm ;
bhc3qgnuicTID2 ( rtS , & rtB . bhc3qgnuicw , & rtDW . bhc3qgnuicw , & rtP .
bhc3qgnuicw ) ; on20slldftTID2 ( rtS , & rtB . on20slldft0 , & rtDW .
on20slldft0 , & rtP . on20slldft0 ) ; rtB . ayzloxj5bc = ( rtP . T2_DelayType
== rtP . Constant_Value_enbglm5ivq ) ; rtB . cyagjbdmtr = ! rtB . ayzloxj5bc
; bhc3qgnuicTID2 ( rtS , & rtB . bk3jg31gdb , & rtDW . bk3jg31gdb , & rtP .
bk3jg31gdb ) ; on20slldftTID2 ( rtS , & rtB . ge1ps1iuod , & rtDW .
ge1ps1iuod , & rtP . ge1ps1iuod ) ; rtB . aq45nwbsrj = ( rtP . T3_DelayType
== rtP . Constant_Value_ltbqvioup1 ) ; rtB . fvmmjxa1a3 = ! rtB . aq45nwbsrj
; bhc3qgnuicTID2 ( rtS , & rtB . e2epyeugyn , & rtDW . e2epyeugyn , & rtP .
e2epyeugyn ) ; on20slldftTID2 ( rtS , & rtB . jvraryandh , & rtDW .
jvraryandh , & rtP . jvraryandh ) ; rtB . bw04w5v3m2 = ( rtP . T4_DelayType
== rtP . Constant_Value_mbak1bi13z ) ; rtB . lgznclzrkr = ! rtB . bw04w5v3m2
; bhc3qgnuicTID2 ( rtS , & rtB . bbo3ui3fzz , & rtDW . bbo3ui3fzz , & rtP .
bbo3ui3fzz ) ; on20slldftTID2 ( rtS , & rtB . bd4y1f4afb , & rtDW .
bd4y1f4afb , & rtP . bd4y1f4afb ) ; rtB . oaklkrqhff = ( rtP . T5_DelayType
== rtP . Constant_Value_g21jvfmrl2 ) ; rtB . nlyeq12rbm = ! rtB . oaklkrqhff
; bhc3qgnuicTID2 ( rtS , & rtB . mwrkb5ri4b , & rtDW . mwrkb5ri4b , & rtP .
mwrkb5ri4b ) ; on20slldftTID2 ( rtS , & rtB . dffp1wyrjz , & rtDW .
dffp1wyrjz , & rtP . dffp1wyrjz ) ; rtB . jnjcghfjwd = ( rtP . T6_DelayType
== rtP . Constant_Value_kymukkky42 ) ; rtB . afvklxyved = ! rtB . jnjcghfjwd
; bhc3qgnuicTID2 ( rtS , & rtB . kqhpdk5kzz , & rtDW . kqhpdk5kzz , & rtP .
kqhpdk5kzz ) ; on20slldftTID2 ( rtS , & rtB . i43atkcssg , & rtDW .
i43atkcssg , & rtP . i43atkcssg ) ; rtB . oscsgejk1g = ( rtP . T7_DelayType
== rtP . Constant_Value_mfhbupabq1 ) ; rtB . fbnhuuhfxe = ! rtB . oscsgejk1g
; bhc3qgnuicTID2 ( rtS , & rtB . kwzmy55kax , & rtDW . kwzmy55kax , & rtP .
kwzmy55kax ) ; on20slldftTID2 ( rtS , & rtB . jejxmo41e1 , & rtDW .
jejxmo41e1 , & rtP . jejxmo41e1 ) ; rtB . l5vanzvpcs = ( rtP .
T1_DelayType_lb0l2fdrep == rtP . Constant_Value_invzfdaptp ) ; rtB .
c0goa0rpsy = ! rtB . l5vanzvpcs ; bhc3qgnuicTID2 ( rtS , & rtB . lfm4cvmyjw ,
& rtDW . lfm4cvmyjw , & rtP . lfm4cvmyjw ) ; on20slldftTID2 ( rtS , & rtB .
aa351sera2 , & rtDW . aa351sera2 , & rtP . aa351sera2 ) ; rtB . pvwir2bnni =
( rtP . T2_DelayType_az50zpjrsd == rtP . Constant_Value_gckqmuavk1 ) ; rtB .
j50xmqsfe0 = ! rtB . pvwir2bnni ; bhc3qgnuicTID2 ( rtS , & rtB . dtwz5asia3 ,
& rtDW . dtwz5asia3 , & rtP . dtwz5asia3 ) ; on20slldftTID2 ( rtS , & rtB .
bypgcvdndn , & rtDW . bypgcvdndn , & rtP . bypgcvdndn ) ; rtB . glvl0tzhpx =
( rtP . T1_DelayType_laau3lziwy == rtP . Constant_Value_njuhzoliav ) ; rtB .
fcdrb3hdx1 = ! rtB . glvl0tzhpx ; bhc3qgnuicTID2 ( rtS , & rtB . drlew34sm0 ,
& rtDW . drlew34sm0 , & rtP . drlew34sm0 ) ; on20slldftTID2 ( rtS , & rtB .
dkvqk03afq , & rtDW . dkvqk03afq , & rtP . dkvqk03afq ) ; rtB . gr3sucvwgc =
( rtP . T2_DelayType_dw0sa3filw == rtP . Constant_Value_omsbgyvvsl ) ; rtB .
g033ihps0k = ! rtB . gr3sucvwgc ; bhc3qgnuicTID2 ( rtS , & rtB . ieaaexns5m ,
& rtDW . ieaaexns5m , & rtP . ieaaexns5m ) ; on20slldftTID2 ( rtS , & rtB .
j3mwdbpvpq , & rtDW . j3mwdbpvpq , & rtP . j3mwdbpvpq ) ; rtB . dihsxcxh0i =
( rtP . T1_DelayType_ljzz1ts23j == rtP . Constant_Value_i2alsr1nnu ) ; rtB .
haztzm5pvv = ! rtB . dihsxcxh0i ; bhc3qgnuicTID2 ( rtS , & rtB . emkwv3osqn ,
& rtDW . emkwv3osqn , & rtP . emkwv3osqn ) ; on20slldftTID2 ( rtS , & rtB .
eg2uuciuj1 , & rtDW . eg2uuciuj1 , & rtP . eg2uuciuj1 ) ; rtB . j4p3wkehee =
( rtP . T2_DelayType_beyaste2jf == rtP . Constant_Value_p5esroxmq5 ) ; rtB .
d0y5z24lvu = ! rtB . j4p3wkehee ; bhc3qgnuicTID2 ( rtS , & rtB . kcdiwopp0s ,
& rtDW . kcdiwopp0s , & rtP . kcdiwopp0s ) ; on20slldftTID2 ( rtS , & rtB .
acyd2alexd , & rtDW . acyd2alexd , & rtP . acyd2alexd ) ; rtB . a3ejnq3pxj =
! ( rtP . Constant3_Value_mteivjs2rf != 0.0 ) ; rtB . mveey3ixpg = ! ( rtP .
Constant1_Value_kuads453r2 != 0.0 ) ; rtB . obili1orw1 = ! ( rtP .
Constant2_Value_d5r4ggdjtu != 0.0 ) ; rtB . oprd2npfri = ( rtB . a3ejnq3pxj
&& rtB . mveey3ixpg && rtB . obili1orw1 ) ; switch ( ( int32_T ) rtP .
Constant1_Value_pbra4xwpmj ) { case 1 : rtB . mqo2vvvrz0 [ 0 ] = rtP .
posedge_Value [ 0 ] ; rtB . mqo2vvvrz0 [ 1 ] = rtP . posedge_Value [ 1 ] ;
break ; case 2 : rtB . mqo2vvvrz0 [ 0 ] = rtP . negedge_Value [ 0 ] ; rtB .
mqo2vvvrz0 [ 1 ] = rtP . negedge_Value [ 1 ] ; break ; default : rtB .
mqo2vvvrz0 [ 0 ] = rtP . eitheredge_Value [ 0 ] ; rtB . mqo2vvvrz0 [ 1 ] =
rtP . eitheredge_Value [ 1 ] ; break ; } switch ( ( int32_T ) rtP .
Constant1_Value_n01hav2gtd ) { case 1 : rtB . bgwcrbleld [ 0 ] = rtP .
posedge_Value_attmbyitau [ 0 ] ; rtB . bgwcrbleld [ 1 ] = rtP .
posedge_Value_attmbyitau [ 1 ] ; break ; case 2 : rtB . bgwcrbleld [ 0 ] =
rtP . negedge_Value_g11r1imvgw [ 0 ] ; rtB . bgwcrbleld [ 1 ] = rtP .
negedge_Value_g11r1imvgw [ 1 ] ; break ; default : rtB . bgwcrbleld [ 0 ] =
rtP . eitheredge_Value_emg0q4ytid [ 0 ] ; rtB . bgwcrbleld [ 1 ] = rtP .
eitheredge_Value_emg0q4ytid [ 1 ] ; break ; } switch ( ( int32_T ) rtP .
Constant1_Value_moacrkskqm ) { case 1 : rtB . el5hg4zl3f [ 0 ] = rtP .
posedge_Value_hxs4ce54gi [ 0 ] ; rtB . el5hg4zl3f [ 1 ] = rtP .
posedge_Value_hxs4ce54gi [ 1 ] ; break ; case 2 : rtB . el5hg4zl3f [ 0 ] =
rtP . negedge_Value_docym23cph [ 0 ] ; rtB . el5hg4zl3f [ 1 ] = rtP .
negedge_Value_docym23cph [ 1 ] ; break ; default : rtB . el5hg4zl3f [ 0 ] =
rtP . eitheredge_Value_oy1s1z4tly [ 0 ] ; rtB . el5hg4zl3f [ 1 ] = rtP .
eitheredge_Value_oy1s1z4tly [ 1 ] ; break ; } rtB . dslqrebvta = true ; rtB .
mqsksz2kyw = true ; rtB . ai0bwq10ky = true ; rtB . lg20acdfom = true ; rtB .
omgvncxd3u = true ; rtB . fcqneu0lzp = true ; rtB . bs0uc3od50 = true ; rtB .
iwfj542rku = true ; rtB . ca0dkhyahi = true ; rtB . oztcgxwfan = ( rtP .
OffDelay_DelayType == rtP . Constant_Value_pi1ezht3og ) ; rtB . d4py5iw3o2 =
! rtB . oztcgxwfan ; bhc3qgnuicTID2 ( rtS , & rtB . leagxggwwe , & rtDW .
leagxggwwe , & rtP . leagxggwwe ) ; on20slldftTID2 ( rtS , & rtB . inadvhzvdh
, & rtDW . inadvhzvdh , & rtP . inadvhzvdh ) ; rtB . fxhy3qc20w = ( rtP .
OffDelay_DelayType_jxdgdi4xvm == rtP . Constant_Value_l2r0djehna ) ; rtB .
j5vr5jeqw2 = ! rtB . fxhy3qc20w ; bhc3qgnuicTID2 ( rtS , & rtB . my1xc0on3o ,
& rtDW . my1xc0on3o , & rtP . my1xc0on3o ) ; on20slldftTID2 ( rtS , & rtB .
diplwx4hig , & rtDW . diplwx4hig , & rtP . diplwx4hig ) ; rtB . lsrdffqt2n =
( rtP . OffDelay_DelayType_iv1b0efza4 == rtP . Constant_Value_a4spzsssnt ) ;
rtB . h3p5ewzebq = ! rtB . lsrdffqt2n ; bhc3qgnuicTID2 ( rtS , & rtB .
j5kctkibbm , & rtDW . j5kctkibbm , & rtP . j5kctkibbm ) ; on20slldftTID2 (
rtS , & rtB . if1byo0yst , & rtDW . if1byo0yst , & rtP . if1byo0yst ) ; rtB .
h1eangrp2e = true ; rtB . kpqmbuvakn = true ; rtB . ppuj1bwkov = true ; rtB .
cqa5nibvpx = true ; rtB . ata4hlycnw = true ; rtB . lbqfu0zivg = true ; rtB .
p0h4azjag2 = true ; rtB . pjepo1a1my = true ; rtB . g340fostkk = true ; rtB .
mmqmljtopv = ( rtP . OffDelay_DelayType_iwilq4ipez == rtP .
Constant_Value_nf4jl2f34x ) ; rtB . mewwllg0zx = ! rtB . mmqmljtopv ;
bhc3qgnuicTID2 ( rtS , & rtB . ongp1nebfx , & rtDW . ongp1nebfx , & rtP .
ongp1nebfx ) ; on20slldftTID2 ( rtS , & rtB . dej3sbopt3 , & rtDW .
dej3sbopt3 , & rtP . dej3sbopt3 ) ; rtB . at3ruiicny = ( rtP .
OffDelay_DelayType_abmzup1oro == rtP . Constant_Value_eptymlppaf ) ; rtB .
ff1bpcza3s = ! rtB . at3ruiicny ; bhc3qgnuicTID2 ( rtS , & rtB . bvpxesww1j ,
& rtDW . bvpxesww1j , & rtP . bvpxesww1j ) ; on20slldftTID2 ( rtS , & rtB .
gn1qmb4zgp , & rtDW . gn1qmb4zgp , & rtP . gn1qmb4zgp ) ; rtB . exhkmjxrht =
( rtP . OffDelay_DelayType_l5sasm2vph == rtP . Constant_Value_ndi2zrwu2i ) ;
rtB . m1fbpm2qm1 = ! rtB . exhkmjxrht ; bhc3qgnuicTID2 ( rtS , & rtB .
ofq4vda2xo , & rtDW . ofq4vda2xo , & rtP . ofq4vda2xo ) ; on20slldftTID2 (
rtS , & rtB . onwgyzhgir , & rtDW . onwgyzhgir , & rtP . onwgyzhgir ) ; rtB .
mgsawex2wt = ! ( rtP . u8_Value_doesqwqeqc != 0.0 ) ; rtB . guluxkv0ch = true
; rtB . ggscruems0 = ( rtP . u0_Value_peek1cnic5 == rtP . u0_Value_kgjfg25lfy
) ; rtB . iytaxoypzi = ( rtP . u8_Value_gkx53ol1kp == rtP .
u0_Value_kgjfg25lfy ) ; rtB . eeqsb5x23m = ( rtB . ggscruems0 || rtB .
iytaxoypzi ) ; rtB . ldhdd1srhl = rtP . u0_Value_peek1cnic5 / rtP .
u8_Value_gkx53ol1kp ; if ( rtB . eeqsb5x23m ) { rtB . jnykas0inl = rtP .
u9_Value_naxhqaii22 ; } else { rtB . jnykas0inl = rtB . ldhdd1srhl ; } rtB .
ffflpratew = rtB . jnykas0inl * rtP . u9_Value_pwo1xiraul ; rtB . gdqn4l0olx
= ( rtP . u1_Value == rtP . u0_Value_lkiaujwove ) ; rtB . c15vadxmz1 = ( rtP
. u5_Value_fgekqn053f == rtP . u0_Value_lkiaujwove ) ; rtB . hzso5vew2y = (
rtB . gdqn4l0olx || rtB . c15vadxmz1 ) ; rtB . j0g0rgdojq = rtP . u1_Value /
rtP . u5_Value_fgekqn053f ; if ( rtB . hzso5vew2y ) { rtB . dwms0o5flk = rtP
. u9_Value_b0uuc4jkmm ; } else { rtB . dwms0o5flk = rtB . j0g0rgdojq ; } rtB
. olvyzc5uqq = rtB . dwms0o5flk * rtP . u6_Value_m3jd1ty455 ; rtB .
ajxdzyfgat = 0.0 ; rtB . mwnkdp4pqt = 0.0 ; rtB . iu14gf3fts = rtB .
ajxdzyfgat + rtB . mwnkdp4pqt ; rtB . fwpxveqpj5 = muDoubleScalarSqrt ( rtB .
iu14gf3fts ) ; rtB . mtdldetrtq = ( rtB . fwpxveqpj5 > rtP .
u6_Value_m3jd1ty455 ) ; minV = muDoubleScalarMin ( rtB . ffflpratew , rtB .
olvyzc5uqq ) ; rtB . mf5uzgcpk4 = minV ; rtB . oe024kjwye = 0.0 ; rtB .
gugbc3oqlr = 0.0 ; rtB . bg12gzmtwe = rtB . oe024kjwye + rtB . gugbc3oqlr ;
rtB . hdxkmzsziu = muDoubleScalarSqrt ( rtB . bg12gzmtwe ) ; rtB . ityznxzj4e
= ( rtB . hdxkmzsziu > rtP . u6_Value_m3jd1ty455 ) ; rtB . lzlorlgpjm = 0.0 ;
rtB . ozpacztdlj = 0.0 ; rtB . bkpgz000r2 = rtB . lzlorlgpjm + rtB .
ozpacztdlj ; rtB . nysc32ju20 = muDoubleScalarSqrt ( rtB . bkpgz000r2 ) ; rtB
. pjte25iacd = ( rtB . nysc32ju20 > rtP . u6_Value_m3jd1ty455 ) ; rtB .
cjvcxffefr = 0.0 ; rtB . e4ilsnkbwn = 0.0 ; rtB . bhun4s3h03 = rtB .
cjvcxffefr + rtB . e4ilsnkbwn ; rtB . berqo3g15j = muDoubleScalarSqrt ( rtB .
bhun4s3h03 ) ; rtB . gaofeahffg = ( rtB . berqo3g15j > rtP .
u9_Value_pwo1xiraul ) ; rtB . nyj1wt2cfl = 0.0 ; rtB . o5ixbkp2ze = 0.0 ; rtB
. l3ufkxp3dw = rtB . nyj1wt2cfl + rtB . o5ixbkp2ze ; rtB . de12z0l52j =
muDoubleScalarSqrt ( rtB . l3ufkxp3dw ) ; rtB . emuxatj33n = ( rtB .
de12z0l52j > rtP . u6_Value_m3jd1ty455 ) ; rtB . ewiv2cqq4y = ( rtP .
T1_DelayType_agac4h4ysi == rtP . Constant_Value_d10s5l3p5t ) ; rtB .
macp2azgz2 = ! rtB . ewiv2cqq4y ; bhc3qgnuicTID2 ( rtS , & rtB . mempnwchre ,
& rtDW . mempnwchre , & rtP . mempnwchre ) ; on20slldftTID2 ( rtS , & rtB .
f2cfptydf3 , & rtDW . f2cfptydf3 , & rtP . f2cfptydf3 ) ; rtB . pdo21geevx =
( rtP . T2_DelayType_dr1zk41djw == rtP . Constant_Value_cv0qln25pn ) ; rtB .
omqcszm1kk = ! rtB . pdo21geevx ; bhc3qgnuicTID2 ( rtS , & rtB . e05ysccjt5 ,
& rtDW . e05ysccjt5 , & rtP . e05ysccjt5 ) ; on20slldftTID2 ( rtS , & rtB .
ceydu0zdw3 , & rtDW . ceydu0zdw3 , & rtP . ceydu0zdw3 ) ; rtB . htr5iktw51 =
( rtP . T3_DelayType_lfaaktahik == rtP . Constant_Value_nvgl3kd2cc ) ; rtB .
kmmd10plti = ! rtB . htr5iktw51 ; bhc3qgnuicTID2 ( rtS , & rtB . ffo1fpyqgq ,
& rtDW . ffo1fpyqgq , & rtP . ffo1fpyqgq ) ; on20slldftTID2 ( rtS , & rtB .
kbnljt2vok , & rtDW . kbnljt2vok , & rtP . kbnljt2vok ) ; rtB . a0cax2dq42 =
( rtP . T4_DelayType_edgnp12kjr == rtP . Constant_Value_anx4ewuioa ) ; rtB .
e5ffsl2txk = ! rtB . a0cax2dq42 ; bhc3qgnuicTID2 ( rtS , & rtB . nepgqug5c0 ,
& rtDW . nepgqug5c0 , & rtP . nepgqug5c0 ) ; on20slldftTID2 ( rtS , & rtB .
nrt14kntrk , & rtDW . nrt14kntrk , & rtP . nrt14kntrk ) ; rtB . ouucq52d4y =
true ; rtB . o0vt4k10iv = true ; rtB . cjiqpxca3e = ( rtP .
u0_Value_leihd05k55 == rtP . u0_Value_flsfpgt11x ) ; rtB . m3qmmw1bvg = ( rtP
. u8_Value_eotsvdc1bm == rtP . u0_Value_flsfpgt11x ) ; rtB . ddqbnv0ba5 = (
rtB . cjiqpxca3e || rtB . m3qmmw1bvg ) ; rtB . kaarmdimki = rtP .
u0_Value_leihd05k55 / rtP . u8_Value_eotsvdc1bm ; if ( rtB . ddqbnv0ba5 ) {
rtB . ohbqabvobv = rtP . u9_Value_e2bagg01jf ; } else { rtB . ohbqabvobv =
rtB . kaarmdimki ; } rtB . d4iy43twxg = rtB . ohbqabvobv * rtP .
u9_Value_gwa1ux021y ; rtB . et1wu5vzcp = ( rtP . u1_Value_kjkcbvrxie == rtP .
u0_Value_oxnchbprdn ) ; rtB . myzwvqsiae = ( rtP . u5_Value_jt45sjvp0m == rtP
. u0_Value_oxnchbprdn ) ; rtB . blwvtkcgny = ( rtB . et1wu5vzcp || rtB .
myzwvqsiae ) ; rtB . iqyyanfxwq = rtP . u1_Value_kjkcbvrxie / rtP .
u5_Value_jt45sjvp0m ; if ( rtB . blwvtkcgny ) { rtB . mnxuhtdjzf = rtP .
u9_Value_izkv0sbcdh ; } else { rtB . mnxuhtdjzf = rtB . iqyyanfxwq ; } rtB .
imik4ijlly = rtB . mnxuhtdjzf * rtP . u6_Value_krkblraugp ; rtB . jkkfn4bfb1
= 0.0 ; rtB . cdas5er0kw = 0.0 ; rtB . ctbfwnigfn = rtB . jkkfn4bfb1 + rtB .
cdas5er0kw ; rtB . bryhf4zwyk = muDoubleScalarSqrt ( rtB . ctbfwnigfn ) ; rtB
. paz1mb5sqz = ( rtB . bryhf4zwyk > rtP . u6_Value_krkblraugp ) ; minV =
muDoubleScalarMin ( rtB . d4iy43twxg , rtB . imik4ijlly ) ; rtB . iw0wqwlmgy
= minV ; rtB . fl45dzadal = 0.0 ; rtB . orxq22v1fc = 0.0 ; rtB . ewqb3wr2sy =
rtB . fl45dzadal + rtB . orxq22v1fc ; rtB . emnm4v3in0 = muDoubleScalarSqrt (
rtB . ewqb3wr2sy ) ; rtB . lz3oaerh4a = ( rtB . emnm4v3in0 > rtP .
u6_Value_krkblraugp ) ; rtB . p31unnzkdo = 0.0 ; rtB . hbcmmpnpag = 0.0 ; rtB
. hei1owqsfx = rtB . p31unnzkdo + rtB . hbcmmpnpag ; rtB . mzxehzomtt =
muDoubleScalarSqrt ( rtB . hei1owqsfx ) ; rtB . kku2fb5nwp = ( rtB .
mzxehzomtt > rtP . u6_Value_krkblraugp ) ; rtB . kfj3cvsl5b = 0.0 ; rtB .
gsn3qjvv5q = 0.0 ; rtB . a3zytutj0z = rtB . kfj3cvsl5b + rtB . gsn3qjvv5q ;
rtB . inbvt15cf1 = muDoubleScalarSqrt ( rtB . a3zytutj0z ) ; rtB . ohj4mhalzc
= ( rtB . inbvt15cf1 > rtP . u9_Value_gwa1ux021y ) ; rtB . l24k3mxttt = 0.0 ;
rtB . jvywrem2z3 = 0.0 ; rtB . cmy0ahirhc = rtB . l24k3mxttt + rtB .
jvywrem2z3 ; rtB . mewkkfs0wo = muDoubleScalarSqrt ( rtB . cmy0ahirhc ) ; rtB
. oiohnnmqqn = ( rtB . mewkkfs0wo > rtP . u6_Value_krkblraugp ) ; rtB .
lgijufwztx = ( rtP . T1_DelayType_g0nto4lc3r == rtP .
Constant_Value_dr1eh44ly0 ) ; rtB . fjhinqhwpy = ! rtB . lgijufwztx ;
bhc3qgnuicTID2 ( rtS , & rtB . di10rsm5nz , & rtDW . di10rsm5nz , & rtP .
di10rsm5nz ) ; on20slldftTID2 ( rtS , & rtB . a1srf3dmr2 , & rtDW .
a1srf3dmr2 , & rtP . a1srf3dmr2 ) ; rtB . if03e2es0p = ( rtP .
T2_DelayType_fyoi11la5y == rtP . Constant_Value_evmybcivn5 ) ; rtB .
kqihty0pex = ! rtB . if03e2es0p ; bhc3qgnuicTID2 ( rtS , & rtB . hgwrktvi4c ,
& rtDW . hgwrktvi4c , & rtP . hgwrktvi4c ) ; on20slldftTID2 ( rtS , & rtB .
pry2hnvt4q , & rtDW . pry2hnvt4q , & rtP . pry2hnvt4q ) ; rtB . dynksvy0hi =
( rtP . T3_DelayType_djg1rcxex4 == rtP . Constant_Value_oktqxg1cc0 ) ; rtB .
kwqertnn23 = ! rtB . dynksvy0hi ; bhc3qgnuicTID2 ( rtS , & rtB . arqjd0gbjl ,
& rtDW . arqjd0gbjl , & rtP . arqjd0gbjl ) ; on20slldftTID2 ( rtS , & rtB .
apyjjt13q1 , & rtDW . apyjjt13q1 , & rtP . apyjjt13q1 ) ; rtB . d141kjjcm3 =
( rtP . T4_DelayType_h34epqdm1j == rtP . Constant_Value_mxiz3d5rvo ) ; rtB .
gpok20ykam = ! rtB . d141kjjcm3 ; bhc3qgnuicTID2 ( rtS , & rtB . pg41d4mdss ,
& rtDW . pg41d4mdss , & rtP . pg41d4mdss ) ; on20slldftTID2 ( rtS , & rtB .
e1cruf0x3a , & rtDW . e1cruf0x3a , & rtP . e1cruf0x3a ) ; rtB . mlgsc4sfpf =
rtP . u5_Value_gphznhvgln / rtP . u8_Value_p4bs1po4g1 ; rtB . m1u52pllya =
rtP . u9_Value_jq2z5q24ih * rtP . u7_Value_nqefso2r1b ; rtB . m5oxi4rh22 = (
rtP . u0_Value_baausp0kjz == rtP . u0_Value_jycxgvrg4t ) ; rtB . h3sod2nflx =
( rtP . u8_Value_p4bs1po4g1 == rtP . u0_Value_jycxgvrg4t ) ; rtB . jwfnhznnix
= ( rtB . m5oxi4rh22 || rtB . h3sod2nflx ) ; rtB . gfptmfvxhz = rtP .
u0_Value_baausp0kjz / rtP . u8_Value_p4bs1po4g1 ; if ( rtB . jwfnhznnix ) {
rtB . mxh01l3oxv = rtP . u9_Value_bihewf2wqr ; } else { rtB . mxh01l3oxv =
rtB . gfptmfvxhz ; } rtB . i3ushz3twh = rtB . mxh01l3oxv * rtP .
u9_Value_jq2z5q24ih ; rtB . oloh4ofkxp = rtB . mcrpzdehxj * rtP . u6_Value ;
rtB . b4pinc0l0d = 0.0 * rtB . mcrpzdehxj ; rtB . boslg4feuv = 0.0 * rtB .
mcrpzdehxj ; rtB . lo0k3fq05t = 0.0 ; rtB . ocx32tabnr = 0.0 ; rtB .
p2jzs2b2qi = rtB . lo0k3fq05t + rtB . ocx32tabnr ; rtB . jfejf1v254 =
muDoubleScalarSqrt ( rtB . p2jzs2b2qi ) ; rtB . fkhohl4ybd = ( rtB .
jfejf1v254 > rtP . u6_Value ) ; rtB . fa5fbmicwx = rtP . u5_Value_pdsjw23ovm
/ rtP . u8_Value_c4bjshfvjr ; rtB . oswwhwdyvs = rtP . u9_Value_bckbcfivxe *
rtP . u7_Value_cykorwolkt ; rtB . llshld3rsc = ( rtP . u0_Value_ffqs1o14tw ==
rtP . u0_Value_of5ofvzk3q ) ; rtB . jkz3far02e = ( rtP . u8_Value_c4bjshfvjr
== rtP . u0_Value_of5ofvzk3q ) ; rtB . feeep2y5up = ( rtB . llshld3rsc || rtB
. jkz3far02e ) ; rtB . hxzxbdluqt = rtP . u0_Value_ffqs1o14tw / rtP .
u8_Value_c4bjshfvjr ; if ( rtB . feeep2y5up ) { rtB . ojva1ku4ei = rtP .
u9_Value_o4wcuzocvc ; } else { rtB . ojva1ku4ei = rtB . hxzxbdluqt ; } rtB .
apvzvyql3u = rtB . ojva1ku4ei * rtP . u9_Value_bckbcfivxe ; rtB . ifhuhhk3fp
= ( rtP . u0_Value_ffqs1o14tw == rtP . u0_Value_kfvumf3suo ) ; rtB .
acsmikz2yk = ( rtP . u5_Value_pdsjw23ovm == rtP . u0_Value_kfvumf3suo ) ; rtB
. b5negg15kl = ( rtB . ifhuhhk3fp || rtB . acsmikz2yk ) ; rtB . mpnjsfuvnf =
rtP . u0_Value_ffqs1o14tw / rtP . u5_Value_pdsjw23ovm ; if ( rtB . b5negg15kl
) { rtB . f22atmyz5x = rtP . u9_Value_b5yequc53t ; } else { rtB . f22atmyz5x
= rtB . mpnjsfuvnf ; } rtB . afsxuc4qtt = rtB . f22atmyz5x * rtP .
u6_Value_jnqr3iyk5e ; rtB . p2dvnsydzu = 0.0 ; rtB . nlvsslrr2k = 0.0 ; rtB .
ipsqcew5sg = rtB . p2dvnsydzu + rtB . nlvsslrr2k ; rtB . ppewbknluc =
muDoubleScalarSqrt ( rtB . ipsqcew5sg ) ; rtB . gnckr4dk34 = ( rtB .
ppewbknluc > rtP . u6_Value_jnqr3iyk5e ) ; rtB . emc54lhdfh = 0.0 * rtB .
f22atmyz5x ; rtB . fuph04mxky = 0.0 * rtB . f22atmyz5x ; rtB . p3phnbdg40 =
0.0 * rtB . f22atmyz5x ; rtB . le1cltguvs = 0.0 * rtB . f22atmyz5x ; rtB .
ba0u34p45y = 0.0 * rtB . f22atmyz5x ; rtB . iq33vpxio2 = 0.0 * rtB .
f22atmyz5x ; rtB . dlwgbrkrvk = 0.0 ; rtB . cqczjtdqik = 0.0 ; rtB .
doaokc5lse = rtB . dlwgbrkrvk + rtB . cqczjtdqik ; rtB . atpukendev =
muDoubleScalarSqrt ( rtB . doaokc5lse ) ; rtB . g1ze435f4f = ( rtB .
atpukendev > rtP . u6_Value_jnqr3iyk5e ) ; rtB . pf3bts1tp0 = 0.0 ; rtB .
czao3mpton = 0.0 ; rtB . e2xwicyzrd = rtB . pf3bts1tp0 + rtB . czao3mpton ;
rtB . nvdqyrrpyz = muDoubleScalarSqrt ( rtB . e2xwicyzrd ) ; rtB . e5a1flxmeo
= ( rtB . nvdqyrrpyz > rtP . u6_Value_jnqr3iyk5e ) ; rtB . k2ambx1rae = 0.0 *
rtB . f22atmyz5x ; rtB . n2pcmj3ear = 0.0 * rtB . f22atmyz5x ; rtB .
fd5ejlwr1q = 0.0 ; rtB . bt0dtmz2za = 0.0 ; rtB . lptzx2pi1k = rtB .
fd5ejlwr1q + rtB . bt0dtmz2za ; rtB . omkm3eexxk = muDoubleScalarSqrt ( rtB .
lptzx2pi1k ) ; rtB . da40ib24oa = ( rtB . omkm3eexxk > rtP .
u6_Value_jnqr3iyk5e ) ; rtB . bgdy0du4zt = rtP . u5_Value_jvamqed34h / rtP .
u8_Value_prnkibwkit ; rtB . parjunhjil = rtP . u9_Value_kcdi3r01mc * rtP .
u7_Value_i4bf30tqdf ; rtB . ojxyk5cwkm = ( rtP . u0_Value_hol5pcfyxu == rtP .
u0_Value_hlnud5qocx ) ; rtB . ebjobqsrvp = ( rtP . u8_Value_prnkibwkit == rtP
. u0_Value_hlnud5qocx ) ; rtB . pwwkjbkvj4 = ( rtB . ojxyk5cwkm || rtB .
ebjobqsrvp ) ; rtB . j5olerl302 = rtP . u0_Value_hol5pcfyxu / rtP .
u8_Value_prnkibwkit ; if ( rtB . pwwkjbkvj4 ) { rtB . cr5is1bebm = rtP .
u9_Value_oi3i0l3ykh ; } else { rtB . cr5is1bebm = rtB . j5olerl302 ; } rtB .
bl0doyrf4k = rtB . cr5is1bebm * rtP . u9_Value_kcdi3r01mc ; rtB . ayjcu30let
= ( rtP . u0_Value_hol5pcfyxu == rtP . u0_Value_posikndzpw ) ; rtB .
dglhk5dpn3 = ( rtP . u5_Value_jvamqed34h == rtP . u0_Value_posikndzpw ) ; rtB
. nho5e0ryah = ( rtB . ayjcu30let || rtB . dglhk5dpn3 ) ; rtB . kewjwuk4ik =
rtP . u0_Value_hol5pcfyxu / rtP . u5_Value_jvamqed34h ; if ( rtB . nho5e0ryah
) { rtB . dt1nklg4pg = rtP . u9_Value_k1c4w5umsj ; } else { rtB . dt1nklg4pg
= rtB . kewjwuk4ik ; } rtB . grle1zpinc = rtB . dt1nklg4pg * rtP .
u6_Value_kipz3r3qi1 ; rtB . iltculh5gi = 0.0 ; rtB . eanpfvpcst = 0.0 ; rtB .
cdympakjtz = rtB . iltculh5gi + rtB . eanpfvpcst ; rtB . ed0z34vsvn =
muDoubleScalarSqrt ( rtB . cdympakjtz ) ; rtB . mi0jmxdcg2 = ( rtB .
ed0z34vsvn > rtP . u6_Value_kipz3r3qi1 ) ; rtB . f2wjg3smpe = 0.0 * rtB .
dt1nklg4pg ; rtB . iw3x3exuh1 = 0.0 * rtB . dt1nklg4pg ; rtB . hekkwclym0 =
0.0 * rtB . dt1nklg4pg ; rtB . njgf35s2dc = 0.0 * rtB . dt1nklg4pg ; rtB .
l2xaubr5ks = 0.0 * rtB . dt1nklg4pg ; rtB . jis1w2jmkv = 0.0 * rtB .
dt1nklg4pg ; rtB . j2y3yxg0lh = 0.0 ; rtB . cuwq2rercz = 0.0 ; rtB .
on2lbd50sp = rtB . j2y3yxg0lh + rtB . cuwq2rercz ; rtB . frvtg4fgwz =
muDoubleScalarSqrt ( rtB . on2lbd50sp ) ; rtB . mdwdufyuqo = ( rtB .
frvtg4fgwz > rtP . u6_Value_kipz3r3qi1 ) ; rtB . l0jln23g5a = 0.0 ; rtB .
na52niqd33 = 0.0 ; rtB . inb5ftbbxe = rtB . l0jln23g5a + rtB . na52niqd33 ;
rtB . jbvvneufoh = muDoubleScalarSqrt ( rtB . inb5ftbbxe ) ; rtB . k1nfpeunz2
= ( rtB . jbvvneufoh > rtP . u6_Value_kipz3r3qi1 ) ; rtB . apb2hjhato = 0.0 *
rtB . dt1nklg4pg ; rtB . a3d0thu041 = 0.0 * rtB . dt1nklg4pg ; rtB .
guezavgr5i = 0.0 ; rtB . aelvnwgdgi = 0.0 ; rtB . nrxahfur14 = rtB .
guezavgr5i + rtB . aelvnwgdgi ; rtB . b0ywmgdzjb = muDoubleScalarSqrt ( rtB .
nrxahfur14 ) ; rtB . gg0kncsrc4 = ( rtB . b0ywmgdzjb > rtP .
u6_Value_kipz3r3qi1 ) ; rtB . cgjfqgdnel = rtP . u5_Value_g233sgh2cn / rtP .
u8_Value_lkzp5copu0 ; rtB . hxmelqln4k = 0.0 ; rtB . kjmtxo30g0 = 0.0 ; rtB .
f332bmtp0n = rtB . hxmelqln4k + rtB . kjmtxo30g0 ; rtB . jvfhqsnirk =
muDoubleScalarSqrt ( rtB . f332bmtp0n ) ; rtB . joggdl4v5p = ( rtB .
jvfhqsnirk > rtP . u9_Value_l23zhc0rg2 ) ; rtB . gyjy11e32a = rtP .
u9_Value_l23zhc0rg2 * rtP . u7_Value_l4xt23vbn0 ; rtB . mu4iyffuln = ( rtP .
u0_Value_mvcxheaq5a == rtP . u0_Value_l4biha544o ) ; rtB . iivvvgh20n = ( rtP
. u8_Value_lkzp5copu0 == rtP . u0_Value_l4biha544o ) ; rtB . kcgbz4llp2 = (
rtB . mu4iyffuln || rtB . iivvvgh20n ) ; rtB . psach0z3oe = rtP .
u0_Value_mvcxheaq5a / rtP . u8_Value_lkzp5copu0 ; if ( rtB . kcgbz4llp2 ) {
rtB . howw1mvyyl = rtP . u9_Value_hd0yopwwl3 ; } else { rtB . howw1mvyyl =
rtB . psach0z3oe ; } rtB . pz3p03b331 = rtB . howw1mvyyl * rtP .
u9_Value_l23zhc0rg2 ; rtB . n4t35bkgtg = ( rtP . u0_Value_mvcxheaq5a == rtP .
u0_Value_bti2ecgmh5 ) ; rtB . pwidt04owc = ( rtP . u5_Value_g233sgh2cn == rtP
. u0_Value_bti2ecgmh5 ) ; rtB . gruufbz450 = ( rtB . n4t35bkgtg || rtB .
pwidt04owc ) ; rtB . f2udvk0gjn = rtP . u0_Value_mvcxheaq5a / rtP .
u5_Value_g233sgh2cn ; if ( rtB . gruufbz450 ) { rtB . i2zc1hnixf = rtP .
u9_Value_fsexfk0amq ; } else { rtB . i2zc1hnixf = rtB . f2udvk0gjn ; } rtB .
jemz30xbq5 = rtB . i2zc1hnixf * rtP . u6_Value_j0yxtfyqds ; rtB . alqgjpllxy
= 0.0 * rtB . howw1mvyyl ; rtB . asiqysuvhp = 0.0 * rtB . howw1mvyyl ; rtB .
izsclt2wvl = 0.0 ; rtB . lwdrc3sjm1 = 0.0 ; rtB . g5s1olahtn = rtB .
izsclt2wvl + rtB . lwdrc3sjm1 ; rtB . gdp5tnk3yd = muDoubleScalarSqrt ( rtB .
g5s1olahtn ) ; rtB . hsqgwgu5t0 = ( rtB . gdp5tnk3yd > rtP .
u6_Value_j0yxtfyqds ) ; rtB . l5ymxnzgmx = 0.0 * rtB . i2zc1hnixf ; rtB .
a22zbv2apn = 0.0 * rtB . i2zc1hnixf ; rtB . nileqt05mu = 0.0 * rtB .
i2zc1hnixf ; rtB . gautr5ktof = 0.0 * rtB . i2zc1hnixf ; rtB . k3psmbe5co =
0.0 * rtB . i2zc1hnixf ; rtB . pptltrhgzg = 0.0 * rtB . i2zc1hnixf ; rtB .
awf3e2rkez = 0.0 ; rtB . fmhjr5c02a = 0.0 ; rtB . hj45hsvd1b = rtB .
awf3e2rkez + rtB . fmhjr5c02a ; rtB . fwlhkckwxh = muDoubleScalarSqrt ( rtB .
hj45hsvd1b ) ; rtB . ehsdvuaox1 = ( rtB . fwlhkckwxh > rtP .
u6_Value_j0yxtfyqds ) ; rtB . hhjpc2oupl = 0.0 ; rtB . gzh4qo4prn = 0.0 ; rtB
. ccpzozzzlr = rtB . hhjpc2oupl + rtB . gzh4qo4prn ; rtB . jsdglhbr5b =
muDoubleScalarSqrt ( rtB . ccpzozzzlr ) ; rtB . hlmwrfvui3 = ( rtB .
jsdglhbr5b > rtP . u6_Value_j0yxtfyqds ) ; rtB . eszffs2bv1 = 0.0 * rtB .
i2zc1hnixf ; rtB . mvu3rm15jj = 0.0 * rtB . i2zc1hnixf ; rtB . gevqw1hywn =
0.0 ; rtB . ox4aagim10 = 0.0 ; rtB . fu2azihkbp = rtB . gevqw1hywn + rtB .
ox4aagim10 ; rtB . muxekqj2sl = muDoubleScalarSqrt ( rtB . fu2azihkbp ) ; rtB
. gg1xonyehg = ( rtB . muxekqj2sl > rtP . u6_Value_j0yxtfyqds ) ; rtB .
ndpikbb43o = false ; rtB . ip4f5pzjmo = ( rtB . ndpikbb43o || ( rtP .
Constant2_Value_pi1g1x2eqs != 0.0 ) || ( rtP . Constant2_Value_pi1g1x2eqs !=
0.0 ) || ( rtP . Constant2_Value_pi1g1x2eqs != 0.0 ) ) ; rtB . hah1gympfy = (
rtP . Constant2_Value_pi1g1x2eqs != 0.0 ) ; rtB . axakxrl3jd = ( rtP .
Constant2_Value_pi1g1x2eqs != 0.0 ) ; rtB . p5aimos5mc = ! rtB . ip4f5pzjmo ;
rtB . carutfzjrm = ( rtP . T3_DelayType_lessikkflo == rtP .
Constant_Value_lttxv4qeau ) ; rtB . bo50yrwyfc = ! rtB . carutfzjrm ;
bhc3qgnuicTID2 ( rtS , & rtB . c5rp3exjwz , & rtDW . c5rp3exjwz , & rtP .
c5rp3exjwz ) ; on20slldftTID2 ( rtS , & rtB . pflqpv2odc , & rtDW .
pflqpv2odc , & rtP . pflqpv2odc ) ; rtB . obkia2x0bm = ( rtP .
T6_DelayType_asgdgj2dqh == rtP . Constant_Value_h5kkz3oibz ) ; rtB .
iso3rom1ur = ! rtB . obkia2x0bm ; bhc3qgnuicTID2 ( rtS , & rtB . dsdu0sw4bh ,
& rtDW . dsdu0sw4bh , & rtP . dsdu0sw4bh ) ; on20slldftTID2 ( rtS , & rtB .
ifezniamwz , & rtDW . ifezniamwz , & rtP . ifezniamwz ) ; rtB . murhwaidrn =
( rtP . T7_DelayType_dcj0lqhqw1 == rtP . Constant_Value_pwdd2yiqo5 ) ; rtB .
ggvkk2zlq3 = ! rtB . murhwaidrn ; bhc3qgnuicTID2 ( rtS , & rtB . nmmyyafynb ,
& rtDW . nmmyyafynb , & rtP . nmmyyafynb ) ; on20slldftTID2 ( rtS , & rtB .
cssuh33ty1 , & rtDW . cssuh33ty1 , & rtP . cssuh33ty1 ) ; rtB . on1kf2nxht =
( rtP . T1_DelayType_brdscvfrur == rtP . Constant_Value_arxj2yudyc ) ; rtB .
a2fiv1shol = ! rtB . on1kf2nxht ; bhc3qgnuicTID2 ( rtS , & rtB . ck45m3cw5y ,
& rtDW . ck45m3cw5y , & rtP . ck45m3cw5y ) ; on20slldftTID2 ( rtS , & rtB .
l0mam0lcui , & rtDW . l0mam0lcui , & rtP . l0mam0lcui ) ; rtB . nowzjtzlbh =
( rtP . T2_DelayType_kvjez1b2du == rtP . Constant_Value_p1bf545hrd ) ; rtB .
nomfep0mpn = ! rtB . nowzjtzlbh ; bhc3qgnuicTID2 ( rtS , & rtB . h330fuij3a ,
& rtDW . h330fuij3a , & rtP . h330fuij3a ) ; on20slldftTID2 ( rtS , & rtB .
o3bwas2b4l , & rtDW . o3bwas2b4l , & rtP . o3bwas2b4l ) ; switch ( ( int32_T
) rtP . Constant1_Value_a4inox31os ) { case 1 : rtB . isx0fgzuta [ 0 ] = rtP
. posedge_Value_e5lpl1mrg0 [ 0 ] ; rtB . isx0fgzuta [ 1 ] = rtP .
posedge_Value_e5lpl1mrg0 [ 1 ] ; break ; case 2 : rtB . isx0fgzuta [ 0 ] =
rtP . negedge_Value_ebrr0iweyy [ 0 ] ; rtB . isx0fgzuta [ 1 ] = rtP .
negedge_Value_ebrr0iweyy [ 1 ] ; break ; default : rtB . isx0fgzuta [ 0 ] =
rtP . eitheredge_Value_iei4n0mttv [ 0 ] ; rtB . isx0fgzuta [ 1 ] = rtP .
eitheredge_Value_iei4n0mttv [ 1 ] ; break ; } rtB . eczhlpl4tk = ( rtB .
ndpikbb43o || ( rtP . Constant2_Value_pi1g1x2eqs != 0.0 ) || ( rtP .
Constant2_Value_pi1g1x2eqs != 0.0 ) || ( rtP . Constant2_Value_pi1g1x2eqs !=
0.0 ) ) ; rtB . gnmfhowrgd = ( rtP . Constant2_Value_pi1g1x2eqs != 0.0 ) ;
rtB . pyma0sbd20 = ( rtP . Constant2_Value_pi1g1x2eqs != 0.0 ) ; rtB .
ayhrq3rnrz = ! rtB . eczhlpl4tk ; rtB . are5psgu4r = ( rtP .
T3_DelayType_htdaurtj3u == rtP . Constant_Value_h41mvacfi0 ) ; rtB .
ab04pgnims = ! rtB . are5psgu4r ; bhc3qgnuicTID2 ( rtS , & rtB . o2355kd0aj ,
& rtDW . o2355kd0aj , & rtP . o2355kd0aj ) ; on20slldftTID2 ( rtS , & rtB .
cdulylvwwz , & rtDW . cdulylvwwz , & rtP . cdulylvwwz ) ; rtB . joiglbbniw =
( rtP . T6_DelayType_o4zkruokow == rtP . Constant_Value_d1slwuyjem ) ; rtB .
blm3mjgqvq = ! rtB . joiglbbniw ; bhc3qgnuicTID2 ( rtS , & rtB . h4jqcza024 ,
& rtDW . h4jqcza024 , & rtP . h4jqcza024 ) ; on20slldftTID2 ( rtS , & rtB .
j1qputxmt3 , & rtDW . j1qputxmt3 , & rtP . j1qputxmt3 ) ; rtB . fmujxxi2he =
( rtP . T7_DelayType_dge1ldgopa == rtP . Constant_Value_iqrr5gmff5 ) ; rtB .
pmonq2okjg = ! rtB . fmujxxi2he ; bhc3qgnuicTID2 ( rtS , & rtB . b1csqxlk14 ,
& rtDW . b1csqxlk14 , & rtP . b1csqxlk14 ) ; on20slldftTID2 ( rtS , & rtB .
h510twish3 , & rtDW . h510twish3 , & rtP . h510twish3 ) ; rtB . pewkf3ytio =
( rtP . T1_DelayType_dlmc2duqup == rtP . Constant_Value_hjrgnvwwhf ) ; rtB .
gdiqx3tr4c = ! rtB . pewkf3ytio ; bhc3qgnuicTID2 ( rtS , & rtB . pshtvwhcvn ,
& rtDW . pshtvwhcvn , & rtP . pshtvwhcvn ) ; on20slldftTID2 ( rtS , & rtB .
cqgqlcfbrn , & rtDW . cqgqlcfbrn , & rtP . cqgqlcfbrn ) ; rtB . bd32r0nedu =
( rtP . T2_DelayType_glka5ybr5z == rtP . Constant_Value_jpf1qlakkl ) ; rtB .
pvyvaukrtz = ! rtB . bd32r0nedu ; bhc3qgnuicTID2 ( rtS , & rtB . pxbtpesdlg ,
& rtDW . pxbtpesdlg , & rtP . pxbtpesdlg ) ; on20slldftTID2 ( rtS , & rtB .
cwhxhlnmc3 , & rtDW . cwhxhlnmc3 , & rtP . cwhxhlnmc3 ) ; switch ( ( int32_T
) rtP . Constant1_Value_e4jqx2jc4m ) { case 1 : rtB . khbmveuyjg [ 0 ] = rtP
. posedge_Value_aip5jo5lj2 [ 0 ] ; rtB . khbmveuyjg [ 1 ] = rtP .
posedge_Value_aip5jo5lj2 [ 1 ] ; break ; case 2 : rtB . khbmveuyjg [ 0 ] =
rtP . negedge_Value_fypt5zy52t [ 0 ] ; rtB . khbmveuyjg [ 1 ] = rtP .
negedge_Value_fypt5zy52t [ 1 ] ; break ; default : rtB . khbmveuyjg [ 0 ] =
rtP . eitheredge_Value_pfbyqaponf [ 0 ] ; rtB . khbmveuyjg [ 1 ] = rtP .
eitheredge_Value_pfbyqaponf [ 1 ] ; break ; } rtB . mezghxaqux = ( rtB .
ndpikbb43o || ( rtP . Constant1_Value_hevig4bniw != 0.0 ) || ( rtP .
Constant1_Value_hevig4bniw != 0.0 ) || ( rtP . Constant1_Value_hevig4bniw !=
0.0 ) ) ; rtB . nhcbwj51h3 = ( ( rtP . Constant_Value_axrunjqqaz != 0.0 ) ||
( rtP . Constant1_Value_hevig4bniw != 0.0 ) || ( rtP .
Constant1_Value_hevig4bniw != 0.0 ) || ( rtP . Constant1_Value_hevig4bniw !=
0.0 ) ) ; rtB . fyiz2i22kv = ( ( rtP . Constant_Value_axrunjqqaz != 0.0 ) ||
( rtP . Constant1_Value_hevig4bniw != 0.0 ) || ( rtP .
Constant1_Value_hevig4bniw != 0.0 ) || ( rtP . Constant1_Value_hevig4bniw !=
0.0 ) ) ; rtB . frxpasnlsa = ! rtB . mezghxaqux ; rtB . jj5lcza05y = ( rtB .
fyiz2i22kv && rtB . frxpasnlsa ) ; rtB . anermfj0j4 = ( ( rtP .
Constant1_Value_jvefpugw4i != 0.0 ) && ( rtP . Constant1_Value_hevig4bniw !=
0.0 ) ) ; rtB . df4lzxx1xa = ! ( rtP . Constant1_Value_hevig4bniw != 0.0 ) ;
rtB . irju5re3ge = ( ( rtP . Constant2_Value_ewkz4woyib != 0.0 ) && rtB .
df4lzxx1xa ) ; rtB . pfegpiaswj = ( rtB . anermfj0j4 || rtB . irju5re3ge ) ;
rtB . pgdq2rmolw = false ; rtB . hboff40tkb = false ; rtB . bnp3fcdgvq = !
rtB . pgdq2rmolw ; rtB . kagsenvyij = ! rtB . hboff40tkb ; rtB . pafyfwzpzd =
false ; rtB . m53ngpfbkk = ! rtB . pafyfwzpzd ; rtB . jtbh3i1tza = ( rtP .
sign_st1_DelayType == rtP . Constant_Value_dhmn2bjsma ) ; rtB . cbsejwmzvq =
! rtB . jtbh3i1tza ; bhc3qgnuicTID2 ( rtS , & rtB . g2vaojwvzg , & rtDW .
g2vaojwvzg , & rtP . g2vaojwvzg ) ; on20slldftTID2 ( rtS , & rtB . muhd0m2ek3
, & rtDW . muhd0m2ek3 , & rtP . muhd0m2ek3 ) ; rtB . dtgyneq0ks = ( rtP .
st1_DelayType == rtP . Constant_Value_juwf13ywoo ) ; rtB . eh43tfta42 = ! rtB
. dtgyneq0ks ; bhc3qgnuicTID2 ( rtS , & rtB . fey4zxdyka , & rtDW .
fey4zxdyka , & rtP . fey4zxdyka ) ; on20slldftTID2 ( rtS , & rtB . iosuhjjule
, & rtDW . iosuhjjule , & rtP . iosuhjjule ) ; rtB . d5cwdi3ojp = ( rtP .
st2_DelayType == rtP . Constant_Value_lxtjpovoo1 ) ; rtB . oommzbtaql = ! rtB
. d5cwdi3ojp ; bhc3qgnuicTID2 ( rtS , & rtB . anq402mwuh , & rtDW .
anq402mwuh , & rtP . anq402mwuh ) ; on20slldftTID2 ( rtS , & rtB . j0x4gfzwjd
, & rtDW . j0x4gfzwjd , & rtP . j0x4gfzwjd ) ; rtB . fiarg4hp4s = false ; rtB
. mmgwbebi3h = ! rtB . fiarg4hp4s ; rtB . b00gwqyuxt = false ; rtB .
bglcjboygs = ! rtB . b00gwqyuxt ; rtB . bqhyaubf5s = false ; rtB . k2np20myhh
= false ; rtB . l5dahm5nsd = ( rtB . bqhyaubf5s || rtB . k2np20myhh ) ; rtB .
cxzexegdct = ! rtB . l5dahm5nsd ; rtB . m2mz0eqwqu = ! rtB . l5dahm5nsd ; rtB
. a1rirvbzfx = ( rtP . OffDelay_DelayType_epeeq5tm5t == rtP .
Constant_Value_gsspdvg2gi ) ; rtB . khzgtvhuz5 = ! rtB . a1rirvbzfx ;
bhc3qgnuicTID2 ( rtS , & rtB . g2dmi2bhhz , & rtDW . g2dmi2bhhz , & rtP .
g2dmi2bhhz ) ; on20slldftTID2 ( rtS , & rtB . kxnpw5jl23 , & rtDW .
kxnpw5jl23 , & rtP . kxnpw5jl23 ) ; rtB . atq3dnr2zu = ( rtP .
OffDelay1_DelayType == rtP . Constant_Value_hffad3s4m0 ) ; rtB . bkf2quznmi =
! rtB . atq3dnr2zu ; bhc3qgnuicTID2 ( rtS , & rtB . dzkeyq1gmc , & rtDW .
dzkeyq1gmc , & rtP . dzkeyq1gmc ) ; on20slldftTID2 ( rtS , & rtB . jzgpron3co
, & rtDW . jzgpron3co , & rtP . jzgpron3co ) ; rtB . ncx4wwz511 = ( rtB .
fiarg4hp4s || ( rtP . Constant1_Value_bt5acu3yd4 != 0.0 ) || ( rtP .
Constant1_Value_bt5acu3yd4 != 0.0 ) || ( rtP . Constant1_Value_bt5acu3yd4 !=
0.0 ) ) ; rtB . kgv5tiygu3 = ( ( rtP . Constant_Value_ffgrxefq15 != 0.0 ) ||
( rtP . Constant1_Value_bt5acu3yd4 != 0.0 ) || ( rtP .
Constant1_Value_bt5acu3yd4 != 0.0 ) || ( rtP . Constant1_Value_bt5acu3yd4 !=
0.0 ) ) ; rtB . kbouik4yfk = ( ( rtP . Constant_Value_ffgrxefq15 != 0.0 ) ||
( rtP . Constant1_Value_bt5acu3yd4 != 0.0 ) || ( rtP .
Constant1_Value_bt5acu3yd4 != 0.0 ) || ( rtP . Constant1_Value_bt5acu3yd4 !=
0.0 ) ) ; rtB . l3uguzluqb = ! rtB . ncx4wwz511 ; rtB . h4zldp0im1 = ( rtB .
kbouik4yfk && rtB . l3uguzluqb ) ; rtB . glci2okinx = ( ( rtP .
Constant1_Value_mucxr2g2vq != 0.0 ) && ( rtP . Constant1_Value_bt5acu3yd4 !=
0.0 ) ) ; rtB . liaiz3ig34 = ! ( rtP . Constant1_Value_bt5acu3yd4 != 0.0 ) ;
rtB . pnecvglc53 = ( ( rtP . Constant2_Value_goaqfqfkfe != 0.0 ) && rtB .
liaiz3ig34 ) ; rtB . fo2kcki2n0 = ( rtB . glci2okinx || rtB . pnecvglc53 ) ;
switch ( ( int32_T ) rtP . Constant1_Value_divgma2rq2 ) { case 1 : rtB .
k45bdikb3y [ 0 ] = rtP . posedge_Value_nfzru14wwr [ 0 ] ; rtB . k45bdikb3y [
1 ] = rtP . posedge_Value_nfzru14wwr [ 1 ] ; break ; case 2 : rtB .
k45bdikb3y [ 0 ] = rtP . negedge_Value_fcoqtctxuh [ 0 ] ; rtB . k45bdikb3y [
1 ] = rtP . negedge_Value_fcoqtctxuh [ 1 ] ; break ; default : rtB .
k45bdikb3y [ 0 ] = rtP . eitheredge_Value_ejkngcqpvh [ 0 ] ; rtB . k45bdikb3y
[ 1 ] = rtP . eitheredge_Value_ejkngcqpvh [ 1 ] ; break ; } switch ( (
int32_T ) rtP . Constant1_Value_iag5a2fpcs ) { case 1 : rtB . lqelpyok5n [ 0
] = rtP . posedge_Value_nseabf5shj [ 0 ] ; rtB . lqelpyok5n [ 1 ] = rtP .
posedge_Value_nseabf5shj [ 1 ] ; break ; case 2 : rtB . lqelpyok5n [ 0 ] =
rtP . negedge_Value_ptymflnnfm [ 0 ] ; rtB . lqelpyok5n [ 1 ] = rtP .
negedge_Value_ptymflnnfm [ 1 ] ; break ; default : rtB . lqelpyok5n [ 0 ] =
rtP . eitheredge_Value_mavae2npn1 [ 0 ] ; rtB . lqelpyok5n [ 1 ] = rtP .
eitheredge_Value_mavae2npn1 [ 1 ] ; break ; } rtB . jz5thmtomj = false ; rtB
. ptknihjghp = false ; rtB . gcvsqiszrf = ! ( rtP . Constant15_Value != 0.0 )
; rtB . oaxbbbavc3 = false ; rtB . p3aogcwg04 = false ; rtB . kqsbirsogi =
false ; rtB . ahvbaw04sr = ( rtP . OffDelay_DelayType_lvsmitupdp == rtP .
Constant_Value_iykol5g5m3 ) ; rtB . djdc13epb1 = ! rtB . ahvbaw04sr ;
bhc3qgnuicTID2 ( rtS , & rtB . nmew4qgpmi , & rtDW . nmew4qgpmi , & rtP .
nmew4qgpmi ) ; on20slldftTID2 ( rtS , & rtB . ox1j1fxqy2 , & rtDW .
ox1j1fxqy2 , & rtP . ox1j1fxqy2 ) ; rtB . oqikakcicv = ( rtP .
OffDelay1_DelayType_muxhtgonnc == rtP . Constant_Value_eyih5fpmvy ) ; rtB .
dwtjxxfz20 = ! rtB . oqikakcicv ; bhc3qgnuicTID2 ( rtS , & rtB . brhbedften ,
& rtDW . brhbedften , & rtP . brhbedften ) ; on20slldftTID2 ( rtS , & rtB .
outjvmmxn3 , & rtDW . outjvmmxn3 , & rtP . outjvmmxn3 ) ; rtB . lsgy0r50gh =
( rtP . OffDelay2_DelayType == rtP . Constant_Value_cwmxs0jg4y ) ; rtB .
pkadpgiezj = ! rtB . lsgy0r50gh ; bhc3qgnuicTID2 ( rtS , & rtB . ozvscdoxcm ,
& rtDW . ozvscdoxcm , & rtP . ozvscdoxcm ) ; on20slldftTID2 ( rtS , & rtB .
pwnmvwir3d , & rtDW . pwnmvwir3d , & rtP . pwnmvwir3d ) ; rtB . pkofunspzd =
( rtP . OffDelay3_DelayType == rtP . Constant_Value_grmye4nhyb ) ; rtB .
j1msf0jlbx = ! rtB . pkofunspzd ; bhc3qgnuicTID2 ( rtS , & rtB . k5ecxmrj0a ,
& rtDW . k5ecxmrj0a , & rtP . k5ecxmrj0a ) ; on20slldftTID2 ( rtS , & rtB .
dyyk14ao4t , & rtDW . dyyk14ao4t , & rtP . dyyk14ao4t ) ; rtB . ftx2jfkpss =
! rtB . jz5thmtomj ; rtB . llh1qrx1e2 = ( rtP . Constant7_Value_b3lmoiohdm !=
0.0 ) ; rtB . fzjbi0wgls = ( ( rtP . Constant6_Value_ffhryhuicj != 0.0 ) || (
rtP . Constant7_Value_b3lmoiohdm != 0.0 ) || ( rtP .
Constant7_Value_b3lmoiohdm != 0.0 ) || ( rtP . Constant7_Value_b3lmoiohdm !=
0.0 ) ) ; rtB . gw5saq2kuq = ( ( rtP . Constant6_Value_ffhryhuicj != 0.0 ) ||
( rtP . Constant7_Value_b3lmoiohdm != 0.0 ) || ( rtP .
Constant7_Value_b3lmoiohdm != 0.0 ) || ( rtP . Constant7_Value_b3lmoiohdm !=
0.0 ) ) ; rtB . ltszmiqlyr = ! rtB . llh1qrx1e2 ; rtB . ihhcgp0ixt = ( rtB .
gw5saq2kuq && rtB . ltszmiqlyr ) ; rtB . pz41pirhkz = ( ( rtP .
Constant1_Value_btihtdli0b != 0.0 ) && ( rtP . Constant7_Value_b3lmoiohdm !=
0.0 ) ) ; rtB . lu31q5qpla = ! ( rtP . Constant7_Value_b3lmoiohdm != 0.0 ) ;
rtB . m3vssxd4e2 = ( ( rtP . Constant2_Value_puqaj21w5e != 0.0 ) && rtB .
lu31q5qpla ) ; rtB . pkhgnulody = ( rtB . pz41pirhkz || rtB . m3vssxd4e2 ) ;
rtB . jfr0m0qmvk = ! rtB . p3aogcwg04 ; rtB . k11quyylu5 = ( rtB . p3aogcwg04
|| ( rtP . Constant9_Value_cknrkticvl != 0.0 ) || ( rtP .
Constant9_Value_cknrkticvl != 0.0 ) || ( rtP . Constant9_Value_cknrkticvl !=
0.0 ) ) ; rtB . auvirykx13 = ( ( rtP . Constant8_Value_c25jgznemb != 0.0 ) ||
( rtP . Constant9_Value_cknrkticvl != 0.0 ) || ( rtP .
Constant9_Value_cknrkticvl != 0.0 ) || ( rtP . Constant9_Value_cknrkticvl !=
0.0 ) ) ; rtB . pcbcw5jcil = ( ( rtP . Constant8_Value_c25jgznemb != 0.0 ) ||
( rtP . Constant9_Value_cknrkticvl != 0.0 ) || ( rtP .
Constant9_Value_cknrkticvl != 0.0 ) || ( rtP . Constant9_Value_cknrkticvl !=
0.0 ) ) ; rtB . jje1uonhfj = ! rtB . k11quyylu5 ; rtB . nti2tirkyr = ( rtB .
pcbcw5jcil && rtB . jje1uonhfj ) ; rtB . p4plaa3mcd = ( ( rtP .
Constant1_Value_gxl2z5kfkk != 0.0 ) && ( rtP . Constant9_Value_cknrkticvl !=
0.0 ) ) ; rtB . dfvylnohw4 = ! ( rtP . Constant9_Value_cknrkticvl != 0.0 ) ;
rtB . cv00q5mjbi = ( ( rtP . Constant2_Value_ewpmzpnclm != 0.0 ) && rtB .
dfvylnohw4 ) ; rtB . pksmpicrpn = ( rtB . p4plaa3mcd || rtB . cv00q5mjbi ) ;
rtB . myngmwf504 = ( rtB . kqsbirsogi || rtB . oaxbbbavc3 || ( rtP .
Constant5_Value_bl2dehj2t4 != 0.0 ) || ( rtP . Constant5_Value_bl2dehj2t4 !=
0.0 ) ) ; rtB . h0xdccgte4 = ( ( rtP . Constant3_Value_hj10kxmwbb != 0.0 ) ||
( rtP . Constant5_Value_bl2dehj2t4 != 0.0 ) || ( rtP .
Constant5_Value_bl2dehj2t4 != 0.0 ) || ( rtP . Constant5_Value_bl2dehj2t4 !=
0.0 ) ) ; rtB . oykywur52b = ( ( rtP . Constant3_Value_hj10kxmwbb != 0.0 ) ||
( rtP . Constant5_Value_bl2dehj2t4 != 0.0 ) || ( rtP .
Constant5_Value_bl2dehj2t4 != 0.0 ) || ( rtP . Constant5_Value_bl2dehj2t4 !=
0.0 ) ) ; rtB . h0jvrhga4s = ! rtB . myngmwf504 ; rtB . kgyemynsqc = ( rtB .
oykywur52b && rtB . h0jvrhga4s ) ; rtB . bpaclm2jwx = ( ( rtP .
Constant1_Value_j43k101bpb != 0.0 ) && ( rtP . Constant5_Value_bl2dehj2t4 !=
0.0 ) ) ; rtB . dyaprnhehc = ! ( rtP . Constant5_Value_bl2dehj2t4 != 0.0 ) ;
rtB . ahh2bwfvyb = ( ( rtP . Constant2_Value_mmru3p4ln3 != 0.0 ) && rtB .
dyaprnhehc ) ; rtB . o0tlpivqvc = ( rtB . bpaclm2jwx || rtB . ahh2bwfvyb ) ;
rtB . a0rmug0ruf = ( rtB . oaxbbbavc3 || rtB . kqsbirsogi || ( rtP .
Constant12_Value_cxukqqxhyg != 0.0 ) || ( rtP . Constant14_Value != 0.0 ) ) ;
rtB . gz1zr03e2e = ( ( rtP . Constant13_Value_i15dakpeie != 0.0 ) || ( rtP .
Constant14_Value != 0.0 ) || ( rtP . Constant14_Value != 0.0 ) || ( rtP .
Constant14_Value != 0.0 ) ) ; rtB . jx5cab5drj = ( ( rtP .
Constant13_Value_i15dakpeie != 0.0 ) || ( rtP . Constant14_Value != 0.0 ) ||
( rtP . Constant14_Value != 0.0 ) || ( rtP . Constant14_Value != 0.0 ) ) ;
rtB . fagunwnusa = ! rtB . a0rmug0ruf ; rtB . klwphbkdvv = ( rtB . jx5cab5drj
&& rtB . fagunwnusa ) ; rtB . mrm11bxrkj = ( ( rtP .
Constant1_Value_icng0d5wtx != 0.0 ) && ( rtP . Constant14_Value != 0.0 ) ) ;
rtB . gcwyxp5aoy = ! ( rtP . Constant14_Value != 0.0 ) ; rtB . mluvdcz3be = (
( rtP . Constant2_Value_bdd0py01di != 0.0 ) && rtB . gcwyxp5aoy ) ; rtB .
dlrursw4i1 = ( rtB . mrm11bxrkj || rtB . mluvdcz3be ) ; rtB . evfa2ub5ny = (
rtB . ptknihjghp || rtB . kqsbirsogi || ( rtP . Constant1_Value_jghm0qsqhp !=
0.0 ) || ( rtP . Constant1_Value_jghm0qsqhp != 0.0 ) ) ; rtB . hpln3qz1no = (
( rtP . Constant_Value_hpvty51hcv != 0.0 ) || ( rtP .
Constant1_Value_jghm0qsqhp != 0.0 ) || ( rtP . Constant1_Value_jghm0qsqhp !=
0.0 ) || ( rtP . Constant1_Value_jghm0qsqhp != 0.0 ) ) ; rtB . crpp2c2a0g = (
( rtP . Constant12_Value_cxukqqxhyg != 0.0 ) || ( rtP .
Constant1_Value_jghm0qsqhp != 0.0 ) || ( rtP . Constant1_Value_jghm0qsqhp !=
0.0 ) || ( rtP . Constant1_Value_jghm0qsqhp != 0.0 ) ) ; rtB . mngmspp2ud = !
rtB . evfa2ub5ny ; rtB . hb4n1ud5by = ( rtB . crpp2c2a0g && rtB . mngmspp2ud
) ; rtB . eg4h1jqats = ( ( rtP . Constant1_Value_huabt4r2i2 != 0.0 ) && ( rtP
. Constant1_Value_jghm0qsqhp != 0.0 ) ) ; rtB . ft4ith0ars = ! ( rtP .
Constant1_Value_jghm0qsqhp != 0.0 ) ; rtB . c41wfgvqqp = ( ( rtP .
Constant2_Value_jsweunrkdi != 0.0 ) && rtB . ft4ith0ars ) ; rtB . pxnpqvfpjn
= ( rtB . eg4h1jqats || rtB . c41wfgvqqp ) ; switch ( ( int32_T ) rtP .
Constant1_Value_o0luuazw3l ) { case 1 : rtB . b2khhhoouf [ 0 ] = rtP .
posedge_Value_kxqsau1wxk [ 0 ] ; rtB . b2khhhoouf [ 1 ] = rtP .
posedge_Value_kxqsau1wxk [ 1 ] ; break ; case 2 : rtB . b2khhhoouf [ 0 ] =
rtP . negedge_Value_flavvxd1v2 [ 0 ] ; rtB . b2khhhoouf [ 1 ] = rtP .
negedge_Value_flavvxd1v2 [ 1 ] ; break ; default : rtB . b2khhhoouf [ 0 ] =
rtP . eitheredge_Value_pb132xvz0v [ 0 ] ; rtB . b2khhhoouf [ 1 ] = rtP .
eitheredge_Value_pb132xvz0v [ 1 ] ; break ; } switch ( ( int32_T ) rtP .
Constant1_Value_mtukwbylpv ) { case 1 : rtB . ktunfxhhd3 [ 0 ] = rtP .
posedge_Value_lkfnuzulxz [ 0 ] ; rtB . ktunfxhhd3 [ 1 ] = rtP .
posedge_Value_lkfnuzulxz [ 1 ] ; break ; case 2 : rtB . ktunfxhhd3 [ 0 ] =
rtP . negedge_Value_hc2x2r5so3 [ 0 ] ; rtB . ktunfxhhd3 [ 1 ] = rtP .
negedge_Value_hc2x2r5so3 [ 1 ] ; break ; default : rtB . ktunfxhhd3 [ 0 ] =
rtP . eitheredge_Value_py3lws1z1e [ 0 ] ; rtB . ktunfxhhd3 [ 1 ] = rtP .
eitheredge_Value_py3lws1z1e [ 1 ] ; break ; } switch ( ( int32_T ) rtP .
Constant1_Value_dddjc1mhet ) { case 1 : rtB . azidapiaxg [ 0 ] = rtP .
posedge_Value_lgenegrdxt [ 0 ] ; rtB . azidapiaxg [ 1 ] = rtP .
posedge_Value_lgenegrdxt [ 1 ] ; break ; case 2 : rtB . azidapiaxg [ 0 ] =
rtP . negedge_Value_afuepzedyz [ 0 ] ; rtB . azidapiaxg [ 1 ] = rtP .
negedge_Value_afuepzedyz [ 1 ] ; break ; default : rtB . azidapiaxg [ 0 ] =
rtP . eitheredge_Value_h1swdm23mc [ 0 ] ; rtB . azidapiaxg [ 1 ] = rtP .
eitheredge_Value_h1swdm23mc [ 1 ] ; break ; } rtB . ospu2oo14g = false ; rtB
. nkbpr3wboi = rtP . u2_Value_pzszgzvx54 ; rtB . f45gsaor4q = rtB .
nkbpr3wboi - rtP . u1_Value_a2tdwqzjg5 ; rtB . bvwkawmjbg = rtP .
u0_Value_ppnw2qt1wp - rtP . u1_Value_a2tdwqzjg5 ; rtB . mlfwifctmw = rtP .
u2_Value_c1sg11dlld - rtP . u_Value_n0ldpwgypr ; rtB . elrmpljgts = rtB .
bvwkawmjbg / rtB . mlfwifctmw ; rtB . cp1mdefdsm = rtB . f45gsaor4q / rtB .
elrmpljgts ; rtB . lagcgbwrmy = rtB . cp1mdefdsm + rtP . u_Value_n0ldpwgypr ;
rtB . gpzx223lsx = ( rtP . u3_Value == rtP . u6_Value_ljkdtrefax ) ; rtB .
avpmkxze2x = ( rtP . u4_Value == rtP . u6_Value_ljkdtrefax ) ; rtB .
ozwd2otrba = ( rtB . gpzx223lsx && rtB . avpmkxze2x ) ; rtB . h0fgnghna4 = (
rtB . nkbpr3wboi == rtP . u57_Value ) ; rtB . dgt1ykgbqc = ! rtB . h0fgnghna4
; rtB . dnzkiuoi4j = ( rtB . ozwd2otrba && rtB . dgt1ykgbqc ) ; if ( rtB .
dnzkiuoi4j ) { rtB . gqcucluh02 = rtB . lagcgbwrmy ; } else { rtB .
gqcucluh02 = rtP . u2_Value_c1sg11dlld ; } rtB . i1phmxngn3 = ( rtP .
u1_Value_a2tdwqzjg5 == rtP . u6_Value_iqy3dkpmmv ) ; rtB . mrqijie1x4 = ( rtP
. u_Value_n0ldpwgypr == rtP . u6_Value_iqy3dkpmmv ) ; rtB . letm2gy5yo = (
rtB . i1phmxngn3 && rtB . mrqijie1x4 ) ; rtB . ayzzalxumn = ( rtP .
u0_Value_ppnw2qt1wp == rtP . u6_Value_athempxgpu ) ; rtB . fdkflyhqwn = ( rtP
. u2_Value_c1sg11dlld == rtP . u6_Value_athempxgpu ) ; rtB . giqwwqw5rs = (
rtB . ayzzalxumn && rtB . fdkflyhqwn ) ; rtB . hqd3ppb3sy = ( rtB .
letm2gy5yo || rtB . giqwwqw5rs ) ; rtB . ngapcg5juh = rtP .
u1_Value_a2tdwqzjg5 * rtP . u2_Value_c1sg11dlld ; rtB . icqadxh5fi = rtP .
u2_Value_c1sg11dlld - rtP . u_Value_n0ldpwgypr ; rtB . mrzwkofgyu = rtP .
u0_Value_ppnw2qt1wp * rtP . u_Value_n0ldpwgypr ; rtB . emj0juw121 = rtB .
ngapcg5juh - rtB . mrzwkofgyu ; rtB . furimuxjih = rtB . emj0juw121 / rtB .
icqadxh5fi ; rtB . i2dcnemcf3 = rtB . nkbpr3wboi - rtP . u0_Value_ppnw2qt1wp
; rtB . ccryyzevi3 = rtP . u3_Value - rtP . u0_Value_ppnw2qt1wp ; rtB .
agalcfl0od = rtP . u4_Value - rtP . u2_Value_c1sg11dlld ; rtB . gptngaxuqn =
rtB . ccryyzevi3 / rtB . agalcfl0od ; rtB . mwqgfa1v0l = rtB . i2dcnemcf3 /
rtB . gptngaxuqn ; rtB . gs5i500mao = rtB . mwqgfa1v0l + rtP .
u2_Value_c1sg11dlld ; rtB . poo5w0hmib = ( rtP . u5_Value_kukyrmodwc == rtP .
u6_Value_c2vmhd0iwc ) ; rtB . psqz1xns2i = ( rtP . u6_Value_iu0fx4dooz == rtP
. u6_Value_c2vmhd0iwc ) ; rtB . lgtzkxgru1 = ( rtB . poo5w0hmib && rtB .
psqz1xns2i ) ; rtB . aum0a5ysxg = ( rtB . nkbpr3wboi == rtP .
u57_Value_km44fzoitg ) ; rtB . mt0wpxf3az = ! rtB . aum0a5ysxg ; rtB .
gxq2lictdc = ( rtB . lgtzkxgru1 && rtB . mt0wpxf3az ) ; if ( rtB . gxq2lictdc
) { rtB . odu4dgb4dj = rtB . gs5i500mao ; } else { rtB . odu4dgb4dj = rtP .
u4_Value ; } rtB . kpo03irhrr = ( rtP . u0_Value_ppnw2qt1wp == rtP .
u6_Value_j4l42tpcb0 ) ; rtB . gx5rzrmi1t = ( rtP . u2_Value_c1sg11dlld == rtP
. u6_Value_j4l42tpcb0 ) ; rtB . dviwdyragm = ( rtB . kpo03irhrr && rtB .
gx5rzrmi1t ) ; rtB . ktf15kptah = ( rtP . u3_Value == rtP .
u6_Value_ocd2w0gxh4 ) ; rtB . jr2tkmzzh4 = ( rtP . u4_Value == rtP .
u6_Value_ocd2w0gxh4 ) ; rtB . dlxo0sg32v = ( rtB . ktf15kptah && rtB .
jr2tkmzzh4 ) ; rtB . n0of4wg12k = ( rtB . dviwdyragm || rtB . dlxo0sg32v ) ;
rtB . mvt0nmo5bq = rtP . u0_Value_ppnw2qt1wp * rtP . u4_Value ; rtB .
g52zh5s2bf = rtP . u4_Value - rtP . u2_Value_c1sg11dlld ; rtB . dcdqvil4w3 =
rtP . u3_Value * rtP . u2_Value_c1sg11dlld ; rtB . arthnbt1gz = rtB .
mvt0nmo5bq - rtB . dcdqvil4w3 ; rtB . oi0ry2exrv = rtB . arthnbt1gz / rtB .
g52zh5s2bf ; rtB . glk5lhdltz = rtB . nkbpr3wboi - rtP . u3_Value ; rtB .
a4uztxhlmx = rtP . u5_Value_kukyrmodwc - rtP . u3_Value ; rtB . d1rfcl4zuo =
rtP . u6_Value_iu0fx4dooz - rtP . u4_Value ; rtB . imuxfxrxnx = rtB .
a4uztxhlmx / rtB . d1rfcl4zuo ; rtB . nhb1rxvh0n = rtB . glk5lhdltz / rtB .
imuxfxrxnx ; rtB . nuolmtbuq4 = rtB . nhb1rxvh0n + rtP . u4_Value ; rtB .
jneh1napic = ( rtP . u7_Value_bjxfucumnh == rtP . u6_Value_huzaa4z0sz ) ; rtB
. b31irbyemy = ( rtP . u8_Value_ngflpra4om == rtP . u6_Value_huzaa4z0sz ) ;
rtB . fc3o0pjcss = ( rtB . jneh1napic && rtB . b31irbyemy ) ; rtB .
djydba5yvz = ( rtB . nkbpr3wboi == rtP . u57_Value_fxecwup2ql ) ; rtB .
ghfraazax3 = ! rtB . djydba5yvz ; rtB . dcbvd2aiqt = ( rtB . fc3o0pjcss &&
rtB . ghfraazax3 ) ; if ( rtB . dcbvd2aiqt ) { rtB . lezo3tixio = rtB .
nuolmtbuq4 ; } else { rtB . lezo3tixio = rtP . u6_Value_iu0fx4dooz ; } rtB .
ecxobus5nx = ( rtP . u3_Value == rtP . u6_Value_k3ultzrzj2 ) ; rtB .
dktfhul4wx = ( rtP . u4_Value == rtP . u6_Value_k3ultzrzj2 ) ; rtB .
pyh40xh0to = ( rtB . ecxobus5nx && rtB . dktfhul4wx ) ; rtB . n40xj2qgub = (
rtP . u5_Value_kukyrmodwc == rtP . u6_Value_oda2k23xop ) ; rtB . bmrydq5x0f =
( rtP . u6_Value_iu0fx4dooz == rtP . u6_Value_oda2k23xop ) ; rtB . drxvg5ex41
= ( rtB . n40xj2qgub && rtB . bmrydq5x0f ) ; rtB . eawyepqgpv = ( rtB .
pyh40xh0to || rtB . drxvg5ex41 ) ; rtB . cl2jtwgbid = rtP . u3_Value * rtP .
u6_Value_iu0fx4dooz ; rtB . gitzrz11ge = rtP . u6_Value_iu0fx4dooz - rtP .
u4_Value ; rtB . emvxvnkxc5 = rtP . u5_Value_kukyrmodwc * rtP . u4_Value ;
rtB . cfa32pxwam = rtB . cl2jtwgbid - rtB . emvxvnkxc5 ; rtB . mgbcoil5s3 =
rtB . cfa32pxwam / rtB . gitzrz11ge ; rtB . bcxwslr1fh = rtB . nkbpr3wboi -
rtP . u5_Value_kukyrmodwc ; rtB . fl2fczfnyt = rtP . u7_Value_bjxfucumnh -
rtP . u5_Value_kukyrmodwc ; rtB . mwbgeq3x2r = rtP . u8_Value_ngflpra4om -
rtP . u6_Value_iu0fx4dooz ; rtB . f00oz24hvd = rtB . fl2fczfnyt / rtB .
mwbgeq3x2r ; rtB . fkyp0hbu4p = rtB . bcxwslr1fh / rtB . f00oz24hvd ; rtB .
nmywxdf3vh = rtB . fkyp0hbu4p + rtP . u6_Value_iu0fx4dooz ; rtB . pxwzkrzepk
= ( rtP . u1_Value_a0mbjao3xr == rtP . u6_Value_ptxdshdxgm ) ; rtB .
bicyyw4nv1 = ( rtP . u1_Value_a0mbjao3xr == rtP . u6_Value_ptxdshdxgm ) ; rtB
. o024qmpn0r = ( rtB . pxwzkrzepk && rtB . bicyyw4nv1 ) ; rtB . ptahzsefvq =
( rtB . nkbpr3wboi == rtP . u57_Value_p20hq2folb ) ; rtB . acy2whhkuk = ! rtB
. ptahzsefvq ; rtB . ggx3u35ot2 = ( rtB . o024qmpn0r && rtB . acy2whhkuk ) ;
if ( rtB . ggx3u35ot2 ) { rtB . lm2m3iulh1 = rtB . nmywxdf3vh ; } else { rtB
. lm2m3iulh1 = rtP . u8_Value_ngflpra4om ; } rtB . dumtnuqwe1 = ( rtP .
u5_Value_kukyrmodwc == rtP . u6_Value_jcubt4chhj ) ; rtB . jltlwqal5l = ( rtP
. u6_Value_iu0fx4dooz == rtP . u6_Value_jcubt4chhj ) ; rtB . pyvu0sjlbl = (
rtB . dumtnuqwe1 && rtB . jltlwqal5l ) ; rtB . l0zzfcydve = ( rtP .
u7_Value_bjxfucumnh == rtP . u6_Value_iq2w5kjki5 ) ; rtB . hkjbkr4skb = ( rtP
. u8_Value_ngflpra4om == rtP . u6_Value_iq2w5kjki5 ) ; rtB . nlqdeg53vc = (
rtB . l0zzfcydve && rtB . hkjbkr4skb ) ; rtB . bramtmeqan = ( rtB .
pyvu0sjlbl || rtB . nlqdeg53vc ) ; rtB . i3z03sdxoi = rtP .
u5_Value_kukyrmodwc * rtP . u8_Value_ngflpra4om ; rtB . bfvyin0mdr = rtP .
u8_Value_ngflpra4om - rtP . u6_Value_iu0fx4dooz ; rtB . jul24njktl = rtP .
u7_Value_bjxfucumnh * rtP . u6_Value_iu0fx4dooz ; rtB . eu5i3wolkr = rtB .
i3z03sdxoi - rtB . jul24njktl ; rtB . bh5fvogibc = rtB . eu5i3wolkr / rtB .
bfvyin0mdr ; rtB . ntkasrotok = rtB . nkbpr3wboi - rtP . u1_Value_a2tdwqzjg5
; rtB . ipivtz3cbd = rtP . u0_Value_ppnw2qt1wp - rtP . u1_Value_a2tdwqzjg5 ;
rtB . ngr3zch0to = rtP . u2_Value_c1sg11dlld - rtP . u_Value_n0ldpwgypr ; rtB
. kmhdsfclzv = rtB . ipivtz3cbd / rtB . ngr3zch0to ; rtB . jmjyae4pkc = rtB .
ntkasrotok / rtB . kmhdsfclzv ; rtB . kntbs2nq3t = rtB . jmjyae4pkc + rtP .
u_Value_n0ldpwgypr ; rtB . hzx53nkfmk = ( rtP . u3_Value == rtP .
u6_Value_dshtiz21lj ) ; rtB . afre0wptff = ( rtP . u4_Value == rtP .
u6_Value_dshtiz21lj ) ; rtB . duj5g2ab13 = ( rtB . hzx53nkfmk && rtB .
afre0wptff ) ; rtB . pccc0d0zpp = ( rtB . nkbpr3wboi == rtP .
u57_Value_d14vk4pksu ) ; rtB . f1ra0nypjs = ! rtB . pccc0d0zpp ; rtB .
crlwi1qowg = ( rtB . duj5g2ab13 && rtB . f1ra0nypjs ) ; if ( rtB . crlwi1qowg
) { rtB . kmzxhlxwcq = rtB . kntbs2nq3t ; } else { rtB . kmzxhlxwcq = rtP .
u2_Value_c1sg11dlld ; } rtB . kz2hkdmes0 = ( rtP . u1_Value_a2tdwqzjg5 == rtP
. u6_Value_fimgcleg4p ) ; rtB . jfpe4tbn5l = ( rtP . u_Value_n0ldpwgypr ==
rtP . u6_Value_fimgcleg4p ) ; rtB . ku1mimxrbx = ( rtB . kz2hkdmes0 && rtB .
jfpe4tbn5l ) ; rtB . gcsi1ec05m = ( rtP . u0_Value_ppnw2qt1wp == rtP .
u6_Value_djjptgtxmc ) ; rtB . eil2iwv4he = ( rtP . u2_Value_c1sg11dlld == rtP
. u6_Value_djjptgtxmc ) ; rtB . bolzobsijd = ( rtB . gcsi1ec05m && rtB .
eil2iwv4he ) ; rtB . m3w2j4hj3t = ( rtB . ku1mimxrbx || rtB . bolzobsijd ) ;
rtB . pwlnufmdpo = rtP . u1_Value_a2tdwqzjg5 * rtP . u2_Value_c1sg11dlld ;
rtB . fyb05lavs2 = rtP . u2_Value_c1sg11dlld - rtP . u_Value_n0ldpwgypr ; rtB
. cfl23q2zii = rtP . u0_Value_ppnw2qt1wp * rtP . u_Value_n0ldpwgypr ; rtB .
elewnealnx = rtB . pwlnufmdpo - rtB . cfl23q2zii ; rtB . d0rxrxmfaa = rtB .
elewnealnx / rtB . fyb05lavs2 ; rtB . pzof1b2pot = rtB . nkbpr3wboi - rtP .
u0_Value_ppnw2qt1wp ; rtB . b13vx2xzgs = rtP . u3_Value - rtP .
u0_Value_ppnw2qt1wp ; rtB . ovsplst331 = rtP . u4_Value - rtP .
u2_Value_c1sg11dlld ; rtB . jmhrjlb32q = rtB . b13vx2xzgs / rtB . ovsplst331
; rtB . akejgphvyi = rtB . pzof1b2pot / rtB . jmhrjlb32q ; rtB . j3fqj42fqx =
rtB . akejgphvyi + rtP . u2_Value_c1sg11dlld ; rtB . al3ysudyoq = ( rtP .
u5_Value_kukyrmodwc == rtP . u6_Value_aapeae3jk3 ) ; rtB . ciepyfciwf = ( rtP
. u6_Value_iu0fx4dooz == rtP . u6_Value_aapeae3jk3 ) ; rtB . e51nhh4yjq = (
rtB . al3ysudyoq && rtB . ciepyfciwf ) ; rtB . ezlpmrn4ak = ( rtB .
nkbpr3wboi == rtP . u57_Value_nxeasvzw4d ) ; rtB . fxobjxadcr = ! rtB .
ezlpmrn4ak ; rtB . imrn23mop3 = ( rtB . e51nhh4yjq && rtB . fxobjxadcr ) ; if
( rtB . imrn23mop3 ) { rtB . lfwnwxtobq = rtB . j3fqj42fqx ; } else { rtB .
lfwnwxtobq = rtP . u4_Value ; } rtB . ilhza24nl3 = ( rtP .
u0_Value_ppnw2qt1wp == rtP . u6_Value_a15r4j3av5 ) ; rtB . jzomfdp5lk = ( rtP
. u2_Value_c1sg11dlld == rtP . u6_Value_a15r4j3av5 ) ; rtB . htf3ldu4ve = (
rtB . ilhza24nl3 && rtB . jzomfdp5lk ) ; rtB . lcmnl2hnvt = ( rtP . u3_Value
== rtP . u6_Value_blg2odqzic ) ; rtB . fziz4iwrzk = ( rtP . u4_Value == rtP .
u6_Value_blg2odqzic ) ; rtB . gywfjxdnxd = ( rtB . lcmnl2hnvt && rtB .
fziz4iwrzk ) ; rtB . k55sygb5my = ( rtB . htf3ldu4ve || rtB . gywfjxdnxd ) ;
rtB . fqni2mpeyo = rtP . u0_Value_ppnw2qt1wp * rtP . u4_Value ; rtB .
jvxlx1vepe = rtP . u4_Value - rtP . u2_Value_c1sg11dlld ; rtB . amyarph0qt =
rtP . u3_Value * rtP . u2_Value_c1sg11dlld ; rtB . csmuevivvv = rtB .
fqni2mpeyo - rtB . amyarph0qt ; rtB . f5mbrc1chn = rtB . csmuevivvv / rtB .
jvxlx1vepe ; rtB . p2d5ksgvie = rtB . nkbpr3wboi - rtP . u3_Value ; rtB .
lgqvvffikg = rtP . u5_Value_kukyrmodwc - rtP . u3_Value ; rtB . ayi2c1oks4 =
rtP . u6_Value_iu0fx4dooz - rtP . u4_Value ; rtB . kqagnyzho3 = rtB .
lgqvvffikg / rtB . ayi2c1oks4 ; rtB . hxvxwnwk1b = rtB . p2d5ksgvie / rtB .
kqagnyzho3 ; rtB . jdxa1cflvq = rtB . hxvxwnwk1b + rtP . u4_Value ; rtB .
ktvgr5arxx = ( rtP . u7_Value_bjxfucumnh == rtP . u6_Value_gltmg3eq4u ) ; rtB
. pkbesbd3sw = ( rtP . u8_Value_ngflpra4om == rtP . u6_Value_gltmg3eq4u ) ;
rtB . ebyxlkhnk1 = ( rtB . ktvgr5arxx && rtB . pkbesbd3sw ) ; rtB .
cv2uplhmcr = ( rtB . nkbpr3wboi == rtP . u57_Value_aphnhpycau ) ; rtB .
cs4cm3qfos = ! rtB . cv2uplhmcr ; rtB . a14np20xqv = ( rtB . ebyxlkhnk1 &&
rtB . cs4cm3qfos ) ; if ( rtB . a14np20xqv ) { rtB . j3dosgzpk4 = rtB .
jdxa1cflvq ; } else { rtB . j3dosgzpk4 = rtP . u6_Value_iu0fx4dooz ; } rtB .
oxilxyuozf = ( rtP . u3_Value == rtP . u6_Value_lkf4vbnuog ) ; rtB .
fssoxlnabh = ( rtP . u4_Value == rtP . u6_Value_lkf4vbnuog ) ; rtB .
nksqkvs5wo = ( rtB . oxilxyuozf && rtB . fssoxlnabh ) ; rtB . ivaoby24ws = (
rtP . u5_Value_kukyrmodwc == rtP . u6_Value_m5nkazczng ) ; rtB . p312gt4sjo =
( rtP . u6_Value_iu0fx4dooz == rtP . u6_Value_m5nkazczng ) ; rtB . ogfgz14cps
= ( rtB . ivaoby24ws && rtB . p312gt4sjo ) ; rtB . l4hiifoiav = ( rtB .
nksqkvs5wo || rtB . ogfgz14cps ) ; rtB . d0vsgq2jar = rtP . u3_Value * rtP .
u6_Value_iu0fx4dooz ; rtB . i4syfgwfde = rtP . u6_Value_iu0fx4dooz - rtP .
u4_Value ; rtB . psekoeijvz = rtP . u5_Value_kukyrmodwc * rtP . u4_Value ;
rtB . djczkf3xsn = rtB . d0vsgq2jar - rtB . psekoeijvz ; rtB . fduxvo2nww =
rtB . djczkf3xsn / rtB . i4syfgwfde ; rtB . jcctkewruc = rtB . nkbpr3wboi -
rtP . u5_Value_kukyrmodwc ; rtB . jootua24aa = rtP . u7_Value_bjxfucumnh -
rtP . u5_Value_kukyrmodwc ; rtB . nfdqcfabbr = rtP . u8_Value_ngflpra4om -
rtP . u6_Value_iu0fx4dooz ; rtB . lnwm3zifhf = rtB . jootua24aa / rtB .
nfdqcfabbr ; rtB . i34tqsa5fs = rtB . jcctkewruc / rtB . lnwm3zifhf ; rtB .
hutfa4jkyu = rtB . i34tqsa5fs + rtP . u6_Value_iu0fx4dooz ; rtB . etzfajbxyb
= ( rtP . u1_Value_i5s2kfsaaw == rtP . u6_Value_ebfoncpamn ) ; rtB .
cl4hlbdbc5 = ( rtP . u1_Value_i5s2kfsaaw == rtP . u6_Value_ebfoncpamn ) ; rtB
. nhorw51pkm = ( rtB . etzfajbxyb && rtB . cl4hlbdbc5 ) ; rtB . jshk5v4reo =
( rtB . nkbpr3wboi == rtP . u57_Value_l1hzmcrgxi ) ; rtB . fgjghwkjtu = ! rtB
. jshk5v4reo ; rtB . kjgdkofbrr = ( rtB . nhorw51pkm && rtB . fgjghwkjtu ) ;
if ( rtB . kjgdkofbrr ) { rtB . fqzg3xi2hh = rtB . hutfa4jkyu ; } else { rtB
. fqzg3xi2hh = rtP . u8_Value_ngflpra4om ; } rtB . k54eje1ntq = ( rtP .
u5_Value_kukyrmodwc == rtP . u6_Value_ahcyhgi2de ) ; rtB . gz3lqghe3z = ( rtP
. u6_Value_iu0fx4dooz == rtP . u6_Value_ahcyhgi2de ) ; rtB . e1ejrpqx1x = (
rtB . k54eje1ntq && rtB . gz3lqghe3z ) ; rtB . df2pa3wtgn = ( rtP .
u7_Value_bjxfucumnh == rtP . u6_Value_hqgjhksz0y ) ; rtB . b5tbrubobn = ( rtP
. u8_Value_ngflpra4om == rtP . u6_Value_hqgjhksz0y ) ; rtB . lcqjhfmgha = (
rtB . df2pa3wtgn && rtB . b5tbrubobn ) ; rtB . ozar2a1dpt = ( rtB .
e1ejrpqx1x || rtB . lcqjhfmgha ) ; rtB . edcpkeescq = rtP .
u5_Value_kukyrmodwc * rtP . u8_Value_ngflpra4om ; rtB . as0yz04aup = rtP .
u8_Value_ngflpra4om - rtP . u6_Value_iu0fx4dooz ; rtB . b1dkiy5gna = rtP .
u7_Value_bjxfucumnh * rtP . u6_Value_iu0fx4dooz ; rtB . hqi0a0pkyk = rtB .
edcpkeescq - rtB . b1dkiy5gna ; rtB . pta54dovoj = rtB . hqi0a0pkyk / rtB .
as0yz04aup ; rtB . p0bq21m4k4 = rtB . nkbpr3wboi - rtP . u1_Value_a2tdwqzjg5
; rtB . mdnsdwreli = rtP . u0_Value_ppnw2qt1wp - rtP . u1_Value_a2tdwqzjg5 ;
rtB . bahvve01a5 = rtP . u2_Value_c1sg11dlld - rtP . u_Value_n0ldpwgypr ; rtB
. egreouhkpb = rtB . mdnsdwreli / rtB . bahvve01a5 ; rtB . f2ieh4rwzl = rtB .
p0bq21m4k4 / rtB . egreouhkpb ; rtB . e4n03d4e1v = rtB . f2ieh4rwzl + rtP .
u_Value_n0ldpwgypr ; rtB . dqyquslcpa = ( rtP . u3_Value == rtP .
u6_Value_jafsckivvz ) ; rtB . ngodoiqsdy = ( rtP . u4_Value == rtP .
u6_Value_jafsckivvz ) ; rtB . dpatpsej5p = ( rtB . dqyquslcpa && rtB .
ngodoiqsdy ) ; rtB . kf40alu3eq = ( rtB . nkbpr3wboi == rtP .
u57_Value_cdscxpquku ) ; rtB . dhtzdia3fz = ! rtB . kf40alu3eq ; rtB .
ml002be1ci = ( rtB . dpatpsej5p && rtB . dhtzdia3fz ) ; if ( rtB . ml002be1ci
) { rtB . feubulmqgt = rtB . e4n03d4e1v ; } else { rtB . feubulmqgt = rtP .
u2_Value_c1sg11dlld ; } rtB . hmwugl5f5j = ( rtP . u1_Value_a2tdwqzjg5 == rtP
. u6_Value_htud5ubgj2 ) ; rtB . ovpyeb3jed = ( rtP . u_Value_n0ldpwgypr ==
rtP . u6_Value_htud5ubgj2 ) ; rtB . i1fcolqomj = ( rtB . hmwugl5f5j && rtB .
ovpyeb3jed ) ; rtB . jot1tmvmhj = ( rtP . u0_Value_ppnw2qt1wp == rtP .
u6_Value_b2x2w3ttfg ) ; rtB . ge112wbaoz = ( rtP . u2_Value_c1sg11dlld == rtP
. u6_Value_b2x2w3ttfg ) ; rtB . bfqj20gizu = ( rtB . jot1tmvmhj && rtB .
ge112wbaoz ) ; rtB . ewccowmzqa = ( rtB . i1fcolqomj || rtB . bfqj20gizu ) ;
rtB . m4he5kfv5h = rtP . u1_Value_a2tdwqzjg5 * rtP . u2_Value_c1sg11dlld ;
rtB . nikqy01c4y = rtP . u2_Value_c1sg11dlld - rtP . u_Value_n0ldpwgypr ; rtB
. nf0qejeshy = rtP . u0_Value_ppnw2qt1wp * rtP . u_Value_n0ldpwgypr ; rtB .
cgdj2eafn5 = rtB . m4he5kfv5h - rtB . nf0qejeshy ; rtB . luuvrrnse0 = rtB .
cgdj2eafn5 / rtB . nikqy01c4y ; rtB . abnozvtgb4 = rtB . nkbpr3wboi - rtP .
u0_Value_ppnw2qt1wp ; rtB . e04evjufmi = rtP . u3_Value - rtP .
u0_Value_ppnw2qt1wp ; rtB . p4gnzk1qmr = rtP . u4_Value - rtP .
u2_Value_c1sg11dlld ; rtB . e04rtiw1z1 = rtB . e04evjufmi / rtB . p4gnzk1qmr
; rtB . mf1smrauho = rtB . abnozvtgb4 / rtB . e04rtiw1z1 ; rtB . dfmfpn0iir =
rtB . mf1smrauho + rtP . u2_Value_c1sg11dlld ; rtB . i3c3xgimvu = ( rtP .
u5_Value_kukyrmodwc == rtP . u6_Value_hpstzsdih1 ) ; rtB . dqegied5bz = ( rtP
. u6_Value_iu0fx4dooz == rtP . u6_Value_hpstzsdih1 ) ; rtB . lfdauhaauo = (
rtB . i3c3xgimvu && rtB . dqegied5bz ) ; rtB . jekq4zcjsb = ( rtB .
nkbpr3wboi == rtP . u57_Value_fthshy021g ) ; rtB . hr1lmu01qr = ! rtB .
jekq4zcjsb ; rtB . m0r0kmcguz = ( rtB . lfdauhaauo && rtB . hr1lmu01qr ) ; if
( rtB . m0r0kmcguz ) { rtB . ijyysyrj1k = rtB . dfmfpn0iir ; } else { rtB .
ijyysyrj1k = rtP . u4_Value ; } rtB . lzdn024lsa = ( rtP .
u0_Value_ppnw2qt1wp == rtP . u6_Value_n144v2estr ) ; rtB . lusuubg1wt = ( rtP
. u2_Value_c1sg11dlld == rtP . u6_Value_n144v2estr ) ; rtB . ez3h3srcfn = (
rtB . lzdn024lsa && rtB . lusuubg1wt ) ; rtB . dlxluqhcbx = ( rtP . u3_Value
== rtP . u6_Value_aj2txnb1jv ) ; rtB . i04r1h1ajx = ( rtP . u4_Value == rtP .
u6_Value_aj2txnb1jv ) ; rtB . f4nfl3brao = ( rtB . dlxluqhcbx && rtB .
i04r1h1ajx ) ; rtB . ivqryhm1oo = ( rtB . ez3h3srcfn || rtB . f4nfl3brao ) ;
rtB . ikbrazgj50 = rtP . u0_Value_ppnw2qt1wp * rtP . u4_Value ; rtB .
gbikcaj3mc = rtP . u4_Value - rtP . u2_Value_c1sg11dlld ; rtB . hnj0z1id1u =
rtP . u3_Value * rtP . u2_Value_c1sg11dlld ; rtB . d0caxmnbja = rtB .
ikbrazgj50 - rtB . hnj0z1id1u ; rtB . oy0r2tr3a0 = rtB . d0caxmnbja / rtB .
gbikcaj3mc ; rtB . erivjakhuy = rtB . nkbpr3wboi - rtP . u3_Value ; rtB .
dm3kwists5 = rtP . u5_Value_kukyrmodwc - rtP . u3_Value ; rtB . g5v55vekny =
rtP . u6_Value_iu0fx4dooz - rtP . u4_Value ; rtB . dbwem4azxl = rtB .
dm3kwists5 / rtB . g5v55vekny ; rtB . ad2esqixgp = rtB . erivjakhuy / rtB .
dbwem4azxl ; rtB . igvtctmndh = rtB . ad2esqixgp + rtP . u4_Value ; rtB .
ofiaxzmmep = ( rtP . u7_Value_bjxfucumnh == rtP . u6_Value_afyst1cu2w ) ; rtB
. cx0ahmet0t = ( rtP . u8_Value_ngflpra4om == rtP . u6_Value_afyst1cu2w ) ;
rtB . bocz0hkzax = ( rtB . ofiaxzmmep && rtB . cx0ahmet0t ) ; rtB .
fiuzhq0zv5 = ( rtB . nkbpr3wboi == rtP . u57_Value_iwc1pexgsw ) ; rtB .
fmpmyvmzhy = ! rtB . fiuzhq0zv5 ; rtB . copifzajzw = ( rtB . bocz0hkzax &&
rtB . fmpmyvmzhy ) ; if ( rtB . copifzajzw ) { rtB . oez50wrxue = rtB .
igvtctmndh ; } else { rtB . oez50wrxue = rtP . u6_Value_iu0fx4dooz ; } rtB .
jn350c3uii = ( rtP . u3_Value == rtP . u6_Value_g0fg50gygn ) ; rtB .
nj4blwgmok = ( rtP . u4_Value == rtP . u6_Value_g0fg50gygn ) ; rtB .
bjj4oerf5n = ( rtB . jn350c3uii && rtB . nj4blwgmok ) ; rtB . jsniv0ok5u = (
rtP . u5_Value_kukyrmodwc == rtP . u6_Value_gss2ektz0a ) ; rtB . he1ksey0px =
( rtP . u6_Value_iu0fx4dooz == rtP . u6_Value_gss2ektz0a ) ; rtB . nhpxdmtgvw
= ( rtB . jsniv0ok5u && rtB . he1ksey0px ) ; rtB . j1thez4uds = ( rtB .
bjj4oerf5n || rtB . nhpxdmtgvw ) ; rtB . ofwmrx5pe4 = rtP . u3_Value * rtP .
u6_Value_iu0fx4dooz ; rtB . awmcbqaf3q = rtP . u6_Value_iu0fx4dooz - rtP .
u4_Value ; rtB . bpztiyx1br = rtP . u5_Value_kukyrmodwc * rtP . u4_Value ;
rtB . cwk1whlubg = rtB . ofwmrx5pe4 - rtB . bpztiyx1br ; rtB . lb40wqt2rg =
rtB . cwk1whlubg / rtB . awmcbqaf3q ; rtB . p2rqjeaj13 = rtB . nkbpr3wboi -
rtP . u5_Value_kukyrmodwc ; rtB . gk13j4r1oq = rtP . u7_Value_bjxfucumnh -
rtP . u5_Value_kukyrmodwc ; rtB . iix12x2jgc = rtP . u8_Value_ngflpra4om -
rtP . u6_Value_iu0fx4dooz ; rtB . nmhqg24xmr = rtB . gk13j4r1oq / rtB .
iix12x2jgc ; rtB . lczva1hd5f = rtB . p2rqjeaj13 / rtB . nmhqg24xmr ; rtB .
jj2fv0gyii = rtB . lczva1hd5f + rtP . u6_Value_iu0fx4dooz ; rtB . bir1xpvkgx
= ( rtP . u1_Value_ccttayn34d == rtP . u6_Value_csgm0403ph ) ; rtB .
ldk3urebku = ( rtP . u1_Value_ccttayn34d == rtP . u6_Value_csgm0403ph ) ; rtB
. n4dwi2rzv3 = ( rtB . bir1xpvkgx && rtB . ldk3urebku ) ; rtB . oqlhlhhxho =
( rtB . nkbpr3wboi == rtP . u57_Value_oya3absyg5 ) ; rtB . g0szp1uapk = ! rtB
. oqlhlhhxho ; rtB . fyxsao05ut = ( rtB . n4dwi2rzv3 && rtB . g0szp1uapk ) ;
if ( rtB . fyxsao05ut ) { rtB . lmt12oy223 = rtB . jj2fv0gyii ; } else { rtB
. lmt12oy223 = rtP . u8_Value_ngflpra4om ; } rtB . ktyhyh52qg = ( rtP .
u5_Value_kukyrmodwc == rtP . u6_Value_jp5hcn1ubn ) ; rtB . kojy2nz0mx = ( rtP
. u6_Value_iu0fx4dooz == rtP . u6_Value_jp5hcn1ubn ) ; rtB . f4nob3rh2b = (
rtB . ktyhyh52qg && rtB . kojy2nz0mx ) ; rtB . elennt4thp = ( rtP .
u7_Value_bjxfucumnh == rtP . u6_Value_keja0pv2qb ) ; rtB . hchg3xiya2 = ( rtP
. u8_Value_ngflpra4om == rtP . u6_Value_keja0pv2qb ) ; rtB . kk3uzc2flj = (
rtB . elennt4thp && rtB . hchg3xiya2 ) ; rtB . jo1c1ihqre = ( rtB .
f4nob3rh2b || rtB . kk3uzc2flj ) ; rtB . mqi2zq3zhe = rtP .
u5_Value_kukyrmodwc * rtP . u8_Value_ngflpra4om ; rtB . eteuoihyif = rtP .
u8_Value_ngflpra4om - rtP . u6_Value_iu0fx4dooz ; rtB . ovo5efalzw = rtP .
u7_Value_bjxfucumnh * rtP . u6_Value_iu0fx4dooz ; rtB . fvg05mjjmv = rtB .
mqi2zq3zhe - rtB . ovo5efalzw ; rtB . c4csnfqbhu = rtB . fvg05mjjmv / rtB .
eteuoihyif ; rtB . d4k5f2b2td = ( rtB . nkbpr3wboi == rtP .
u5_Value_pynaujk2tf ) ; rtB . krvxl5nblc = ! rtB . d4k5f2b2td ; rtB .
erpofmwfnx = ( rtB . nkbpr3wboi == rtP . u5_Value_oxanybzjyo ) ; rtB .
imrosj1vau = ! rtB . erpofmwfnx ; rtB . etz3vu5eap = ( rtB . nkbpr3wboi ==
rtP . u5_Value_irvthq4saz ) ; rtB . fffmpf1xlo = ! rtB . etz3vu5eap ; rtB .
o5xnwww5re = rtP . u_Value_cwfu0ujcvc - rtP . u4_Value_lznh0iwahh ; rtB .
frimtz5j0q = rtB . o5xnwww5re * rtP . u_Value_jnetrbwzqg ; rtB . locdsamlne =
rtP . u_Value_p2gorrkvy3 / rtP . u5_Value_h4l5jynuvu ; rtB . peijvg4ilw =
muDoubleScalarSin ( rtB . frimtz5j0q ) ; rtB . lda5igrpdy = muDoubleScalarCos
( rtB . frimtz5j0q ) ; rtB . p2tc1tgs4u = rtP . u_Value_hkyziycqsm * rtP .
u_Value_hkyziycqsm ; rtB . euteqs4wwy = rtP . u_Value_hkyziycqsm * rtP .
u_Value_hkyziycqsm ; rtB . i0m54oyz3n = rtB . p2tc1tgs4u + rtB . euteqs4wwy ;
rtB . bvlfetlfwh = muDoubleScalarSqrt ( rtB . i0m54oyz3n ) ; rtB . i5yyfplv0k
= rtP . u_Value_dou3bzc3qp / rtP . u_Value_azxo40w5eu ; rtB . b3mlfykrgt =
0.0 * rtB . i5yyfplv0k ; rtB . maogtscng4 = 0.0 ; rtB . jtcgy13zoh = rtB .
maogtscng4 ; rtB . ixo52tjohv = rtB . jtcgy13zoh * rtP . u7_Value_nlwoeh544c
; rtB . obwmavctv4 = rtB . ixo52tjohv * rtB . i5yyfplv0k ; rtB . a3o3sxj0i1 =
rtB . b3mlfykrgt - rtB . obwmavctv4 ; if ( rtP . u_Value_cdkgfcvzug > rtP .
u_Threshold_hfmoqwusc1 ) { rtB . o24n1kqihb = rtB . a3o3sxj0i1 ; } else { rtB
. o24n1kqihb = rtB . b3mlfykrgt ; } rtB . d4lsopldng = rtP .
u_Value_jqhbbg1tsi - rtP . u_Value_hkyziycqsm ; rtB . gagcz335qf = rtB .
d4lsopldng * rtP . u_Value_mc1e3bz1oi ; rtB . iewbvtb1kb = muDoubleScalarCos
( rtB . gagcz335qf ) ; rtB . peozgftdag = rtB . o24n1kqihb * rtB . iewbvtb1kb
; rtB . jvqnjdwsuj = 0.0 * rtB . i5yyfplv0k ; rtB . gbhxfpo0do = 0.0 ; rtB .
hzl02bsfnu = rtB . gbhxfpo0do ; rtB . ju4ejsjik4 = rtB . hzl02bsfnu * rtP .
u7_Value_nlwoeh544c ; rtB . p5cmgviz2v = rtB . ju4ejsjik4 * rtB . i5yyfplv0k
; rtB . jrwq3sur2x = rtB . jvqnjdwsuj - rtB . p5cmgviz2v ; if ( rtP .
u_Value_cdkgfcvzug > rtP . u_Threshold_i5jaqs1z4i ) { rtB . iypmeaocch = rtB
. jrwq3sur2x ; } else { rtB . iypmeaocch = rtB . jvqnjdwsuj ; } rtB .
ebp0jauxuq = muDoubleScalarSin ( rtB . gagcz335qf ) ; rtB . dbodkm2qnw = rtB
. iypmeaocch * rtB . ebp0jauxuq ; rtB . jaahpo4mr4 = rtB . peozgftdag - rtB .
dbodkm2qnw ; if ( 0.0 > rtP . u_Threshold_exp4dyuynb ) { rtB . bzlyen5ic4 =
rtB . jaahpo4mr4 ; } else { rtB . bzlyen5ic4 = rtP . u54_Value_n3c5a1ytjb ; }
if ( rtP . u_Value_luxc10x2ch > rtP . u_Threshold_bbsvtkvqf2 ) { rtB .
pihsgrzb25 = rtB . bzlyen5ic4 ; } else { rtB . pihsgrzb25 = rtP .
u54_Value_n3c5a1ytjb ; } rtB . jchqewupig = rtB . pihsgrzb25 * rtB .
pihsgrzb25 ; rtB . dozu4q0tcy = rtB . iypmeaocch * rtB . iewbvtb1kb ; rtB .
lppajnhuxv = rtB . o24n1kqihb * rtB . ebp0jauxuq ; rtB . fivxhoygb2 = rtB .
dozu4q0tcy + rtB . lppajnhuxv ; if ( 0.0 > rtP . u_Threshold_bcbjus2nzf ) {
rtB . omlsyy3oxc = rtB . fivxhoygb2 ; } else { rtB . omlsyy3oxc = rtP .
u54_Value_n3c5a1ytjb ; } if ( rtP . u_Value_luxc10x2ch > rtP .
u_Threshold_mdd2ixjr4z ) { rtB . kpdsr4w32s = rtB . omlsyy3oxc ; } else { rtB
. kpdsr4w32s = rtP . u54_Value_n3c5a1ytjb ; } rtB . ow1qfpxjc3 = rtB .
kpdsr4w32s * rtB . kpdsr4w32s ; rtB . mpnlbw42f1 = rtB . jchqewupig + rtB .
ow1qfpxjc3 ; rtB . hdpwgfnlvq = muDoubleScalarSqrt ( rtB . mpnlbw42f1 ) ; rtB
. dio3nkdo4x = ( rtB . bvlfetlfwh > rtB . hdpwgfnlvq ) ; if ( rtB .
dio3nkdo4x ) { rtB . mupscucpvr = rtP . u_Value_hkyziycqsm ; rtB . gpaztx4x30
= rtP . u_Value_hkyziycqsm ; } else { rtB . mupscucpvr = rtB . pihsgrzb25 ;
rtB . gpaztx4x30 = rtB . kpdsr4w32s ; } rtB . ktmaz2ouvn = rtB . mupscucpvr *
rtB . mupscucpvr ; rtB . gtsm3o3xgn = rtB . gpaztx4x30 * rtB . gpaztx4x30 ;
rtB . l00pbhlgb2 = rtB . ktmaz2ouvn + rtB . gtsm3o3xgn ; rtB . ekvelrpz4b =
muDoubleScalarSqrt ( rtB . l00pbhlgb2 ) ; rtB . k0oqs3bwiw = rtP .
u_Value_cwfu0ujcvc - rtP . u4_Value_lznh0iwahh ; rtB . boqwzrmryd = rtB .
k0oqs3bwiw * rtP . u_Value_l5cy5wbi1m ; rtB . fgh2gbbdh5 = rtP .
u_Value_p2gorrkvy3 / rtP . u5_Value_h4l5jynuvu ; rtB . jjyoruakud =
muDoubleScalarSin ( rtB . boqwzrmryd ) ; rtB . ht2utaggby = muDoubleScalarCos
( rtB . boqwzrmryd ) ; rtB . dfo5lllwy4 = rtP . u_Value_hkyziycqsm * rtP .
u_Value_hkyziycqsm ; rtB . dumpipiie3 = rtP . u_Value_hkyziycqsm * rtP .
u_Value_hkyziycqsm ; rtB . dhjran0shq = rtB . dfo5lllwy4 + rtB . dumpipiie3 ;
rtB . hpap00rt5n = muDoubleScalarSqrt ( rtB . dhjran0shq ) ; rtB . liwgerymsk
= rtP . u_Value_dou3bzc3qp / rtP . u_Value_azxo40w5eu ; rtB . gkpx0ktrw1 =
0.0 * rtB . liwgerymsk ; rtB . ogzx4xf1tx = rtB . ixo52tjohv * rtB .
liwgerymsk ; rtB . aenhffw5zs = rtB . gkpx0ktrw1 - rtB . ogzx4xf1tx ; if (
rtP . u_Value_cdkgfcvzug > rtP . u_Threshold_lvktwezhli ) { rtB . ozdibq2qgt
= rtB . aenhffw5zs ; } else { rtB . ozdibq2qgt = rtB . gkpx0ktrw1 ; } rtB .
cjboj41u20 = rtP . u_Value_jqhbbg1tsi - rtP . u_Value_hkyziycqsm ; rtB .
gxwkczvimb = rtB . cjboj41u20 * rtP . u_Value_j1usv3tbl0 ; rtB . mff2ww5lz0 =
muDoubleScalarCos ( rtB . gxwkczvimb ) ; rtB . p41oickmab = rtB . ozdibq2qgt
* rtB . mff2ww5lz0 ; rtB . prtibyzzbb = 0.0 * rtB . liwgerymsk ; rtB .
jieenim0wf = rtB . ju4ejsjik4 * rtB . liwgerymsk ; rtB . lt5fcfdvio = rtB .
prtibyzzbb - rtB . jieenim0wf ; if ( rtP . u_Value_cdkgfcvzug > rtP .
u_Threshold_galrvlpcq3 ) { rtB . eel40lxbi0 = rtB . lt5fcfdvio ; } else { rtB
. eel40lxbi0 = rtB . prtibyzzbb ; } rtB . c3mqzlcqx5 = muDoubleScalarSin (
rtB . gxwkczvimb ) ; rtB . hij2sai1oc = rtB . eel40lxbi0 * rtB . c3mqzlcqx5 ;
rtB . gbj2i2jpi4 = rtB . p41oickmab - rtB . hij2sai1oc ; if ( 0.0 > rtP .
u_Threshold_ff1t425rcc ) { rtB . pn4yw3z4ov = rtB . gbj2i2jpi4 ; } else { rtB
. pn4yw3z4ov = rtP . u54_Value_ol1vqnpul5 ; } if ( rtP . u_Value_luxc10x2ch >
rtP . u_Threshold_gbuv3e3dra ) { rtB . or1bi1t23w = rtB . pn4yw3z4ov ; } else
{ rtB . or1bi1t23w = rtP . u54_Value_ol1vqnpul5 ; } rtB . oxkuavludj = rtB .
or1bi1t23w * rtB . or1bi1t23w ; rtB . jnjzg4wn4b = rtB . eel40lxbi0 * rtB .
mff2ww5lz0 ; rtB . py3gfvyv02 = rtB . ozdibq2qgt * rtB . c3mqzlcqx5 ; rtB .
kkyh4hqoaa = rtB . jnjzg4wn4b + rtB . py3gfvyv02 ; if ( 0.0 > rtP .
u_Threshold_bui1ylb04k ) { rtB . esxecvrqmd = rtB . kkyh4hqoaa ; } else { rtB
. esxecvrqmd = rtP . u54_Value_ol1vqnpul5 ; } if ( rtP . u_Value_luxc10x2ch >
rtP . u_Threshold_bou1ii3jpu ) { rtB . plrtqrlkdv = rtB . esxecvrqmd ; } else
{ rtB . plrtqrlkdv = rtP . u54_Value_ol1vqnpul5 ; } rtB . abja014eaz = rtB .
plrtqrlkdv * rtB . plrtqrlkdv ; rtB . mg5l1cmprn = rtB . oxkuavludj + rtB .
abja014eaz ; rtB . dntnpby4td = muDoubleScalarSqrt ( rtB . mg5l1cmprn ) ; rtB
. l1wr5ryxc5 = ( rtB . hpap00rt5n > rtB . dntnpby4td ) ; if ( rtB .
l1wr5ryxc5 ) { rtB . d5orl0ohoz = rtP . u_Value_hkyziycqsm ; rtB . a1cyb5lbfo
= rtP . u_Value_hkyziycqsm ; } else { rtB . d5orl0ohoz = rtB . or1bi1t23w ;
rtB . a1cyb5lbfo = rtB . plrtqrlkdv ; } rtB . glfuzyxwxx = rtB . d5orl0ohoz *
rtB . d5orl0ohoz ; rtB . gafwcvcmf0 = rtB . a1cyb5lbfo * rtB . a1cyb5lbfo ;
rtB . jxc2ywzjiv = rtB . glfuzyxwxx + rtB . gafwcvcmf0 ; rtB . ent5takyzj =
muDoubleScalarSqrt ( rtB . jxc2ywzjiv ) ; rtB . bubt2hm0vk = rtP .
u_Value_cwfu0ujcvc - rtP . u4_Value_lznh0iwahh ; rtB . fe0qaq3yyv = rtB .
bubt2hm0vk * rtP . u_Value_obehd4fdpu ; rtB . d3jk1drg5n = rtP .
u_Value_p2gorrkvy3 / rtP . u5_Value_h4l5jynuvu ; rtB . fgfcqxlfi0 =
muDoubleScalarSin ( rtB . fe0qaq3yyv ) ; rtB . msn321mj2p = muDoubleScalarCos
( rtB . fe0qaq3yyv ) ; rtB . c4wzbdzucc = rtP . u_Value_hkyziycqsm * rtP .
u_Value_hkyziycqsm ; rtB . nsx0rg5c0v = rtP . u_Value_hkyziycqsm * rtP .
u_Value_hkyziycqsm ; rtB . gyctnw1bxi = rtB . c4wzbdzucc + rtB . nsx0rg5c0v ;
rtB . iljlo2a1vs = muDoubleScalarSqrt ( rtB . gyctnw1bxi ) ; rtB . pd2scshitg
= rtP . u_Value_dou3bzc3qp / rtP . u_Value_azxo40w5eu ; rtB . n00kfoiafr =
0.0 * rtB . pd2scshitg ; rtB . bnbdirzrq2 = rtB . ixo52tjohv * rtB .
pd2scshitg ; rtB . cvevszyzvb = rtB . n00kfoiafr - rtB . bnbdirzrq2 ; if (
rtP . u_Value_cdkgfcvzug > rtP . u_Threshold_ef10bk1aoc ) { rtB . keh2kkvvcy
= rtB . cvevszyzvb ; } else { rtB . keh2kkvvcy = rtB . n00kfoiafr ; } rtB .
h5zu4xxrrn = rtP . u_Value_jqhbbg1tsi - rtP . u_Value_hkyziycqsm ; rtB .
iqyq2tshl3 = rtB . h5zu4xxrrn * rtP . u_Value_jd0g0mqpgk ; rtB . m0rgxr12oq =
muDoubleScalarCos ( rtB . iqyq2tshl3 ) ; rtB . iatvzftwyq = rtB . keh2kkvvcy
* rtB . m0rgxr12oq ; rtB . nb110xr3l4 = 0.0 * rtB . pd2scshitg ; rtB .
e3f3dq14yc = rtB . ju4ejsjik4 * rtB . pd2scshitg ; rtB . aljqnyp2xo = rtB .
nb110xr3l4 - rtB . e3f3dq14yc ; if ( rtP . u_Value_cdkgfcvzug > rtP .
u_Threshold_dktr2n4own ) { rtB . jvld3ibj13 = rtB . aljqnyp2xo ; } else { rtB
. jvld3ibj13 = rtB . nb110xr3l4 ; } rtB . guxxhx0lxq = muDoubleScalarSin (
rtB . iqyq2tshl3 ) ; rtB . frn01iogjt = rtB . jvld3ibj13 * rtB . guxxhx0lxq ;
rtB . chxnhzvx4o = rtB . iatvzftwyq - rtB . frn01iogjt ; if ( 0.0 > rtP .
u_Threshold_khksqnqn1v ) { rtB . hqcz5ikngo = rtB . chxnhzvx4o ; } else { rtB
. hqcz5ikngo = rtP . u54_Value_fugcllcxhe ; } if ( rtP . u_Value_luxc10x2ch >
rtP . u_Threshold_abk42s1flo ) { rtB . kl24dmgzdr = rtB . hqcz5ikngo ; } else
{ rtB . kl24dmgzdr = rtP . u54_Value_fugcllcxhe ; } rtB . k30oivffgl = rtB .
kl24dmgzdr * rtB . kl24dmgzdr ; rtB . norhej5agh = rtB . jvld3ibj13 * rtB .
m0rgxr12oq ; rtB . cgcwckregz = rtB . keh2kkvvcy * rtB . guxxhx0lxq ; rtB .
n0bgrn3hxl = rtB . norhej5agh + rtB . cgcwckregz ; if ( 0.0 > rtP .
u_Threshold_fk3eiemx43 ) { rtB . pu1fqkk0ti = rtB . n0bgrn3hxl ; } else { rtB
. pu1fqkk0ti = rtP . u54_Value_fugcllcxhe ; } if ( rtP . u_Value_luxc10x2ch >
rtP . u_Threshold_pzq044hgda ) { rtB . bkfivkkl2o = rtB . pu1fqkk0ti ; } else
{ rtB . bkfivkkl2o = rtP . u54_Value_fugcllcxhe ; } rtB . iexbqymoqi = rtB .
bkfivkkl2o * rtB . bkfivkkl2o ; rtB . kfc2nx5yzq = rtB . k30oivffgl + rtB .
iexbqymoqi ; rtB . dl52ek0ous = muDoubleScalarSqrt ( rtB . kfc2nx5yzq ) ; rtB
. pdxytib21z = ( rtB . iljlo2a1vs > rtB . dl52ek0ous ) ; if ( rtB .
pdxytib21z ) { rtB . m5evsv14wz = rtP . u_Value_hkyziycqsm ; rtB . kztsytllx1
= rtP . u_Value_hkyziycqsm ; } else { rtB . m5evsv14wz = rtB . kl24dmgzdr ;
rtB . kztsytllx1 = rtB . bkfivkkl2o ; } rtB . bfy31tn4c4 = rtB . m5evsv14wz *
rtB . m5evsv14wz ; rtB . c5fruwu3kf = rtB . kztsytllx1 * rtB . kztsytllx1 ;
rtB . n425tjm135 = rtB . bfy31tn4c4 + rtB . c5fruwu3kf ; rtB . mnoq2ms3bo =
muDoubleScalarSqrt ( rtB . n425tjm135 ) ; rtB . lrtmvxgodg = rtP .
u_Value_potytjhwin + rtP . u9_Value_pcy34qth12 ; rtB . hejffordvr = rtP .
u_Value_potytjhwin + rtP . u1_Value_kmdyop2c4e ; rtB . o0wttlbm2m = rtP .
u_Value_acoq23d02a - rtP . u_Value_pvwnoasp13 ; rtB . jnni1cu2vp = rtB .
o0wttlbm2m * rtP . u_Value_mtmjnuxcr1 ; rtB . m5lfhmm3np = rtP .
u_Value_mlmyqsbvus / rtP . u_Value_hi0lmnwmi0 ; rtB . dsaeva2wd0 =
muDoubleScalarSin ( rtB . jnni1cu2vp ) ; rtB . bi3qqodrui = muDoubleScalarCos
( rtB . jnni1cu2vp ) ; rtB . fhzgcxzomg = rtP . u_Value_acoq23d02a - rtP .
u_Value_pvwnoasp13 ; rtB . furlmt52wk = rtB . fhzgcxzomg * rtP .
u_Value_dqrpul5go5 ; rtB . d0y4nbtz00 = rtP . u_Value_mlmyqsbvus / rtP .
u_Value_hi0lmnwmi0 ; rtB . mgcq2y0ojp = muDoubleScalarSin ( rtB . furlmt52wk
) ; rtB . onbinf3mxx = muDoubleScalarCos ( rtB . furlmt52wk ) ; rtB .
b14m1ek5py = rtP . u_Value_acoq23d02a - rtP . u_Value_pvwnoasp13 ; rtB .
jab42xdhiy = rtB . b14m1ek5py * rtP . u_Value_mgy0c1azgv ; rtB . cj5sc500zh =
rtP . u_Value_mlmyqsbvus / rtP . u_Value_hi0lmnwmi0 ; rtB . eohu5a2mev =
muDoubleScalarSin ( rtB . jab42xdhiy ) ; rtB . cfh3zls04q = muDoubleScalarCos
( rtB . jab42xdhiy ) ; rtB . emmckbzbao = rtP . u_Value_cgx53hb23m + rtP .
u9_Value_kudj12dq2r ; rtB . busiz2uss1 = rtP . u_Value_cgx53hb23m + rtP .
u1_Value_ctjghf0amj ; rtB . jbzyzm1nxi = rtB . jaahpo4mr4 * rtB . jaahpo4mr4
; rtB . cxmzaqitem = rtB . fivxhoygb2 * rtB . fivxhoygb2 ; rtB . lrhoaxhldk =
rtB . jbzyzm1nxi + rtB . cxmzaqitem ; rtB . dyn5tjwaqz = muDoubleScalarSqrt (
rtB . lrhoaxhldk ) ; rtB . ku543cmrzp = rtB . gbj2i2jpi4 * rtB . gbj2i2jpi4 ;
rtB . is40tdhq4f = rtB . kkyh4hqoaa * rtB . kkyh4hqoaa ; rtB . cq42eswkqz =
rtB . ku543cmrzp + rtB . is40tdhq4f ; rtB . hhxwkk00gb = muDoubleScalarSqrt (
rtB . cq42eswkqz ) ; rtB . hwfdd0cjwx = rtB . chxnhzvx4o * rtB . chxnhzvx4o ;
rtB . bceyok5mqb = rtB . n0bgrn3hxl * rtB . n0bgrn3hxl ; rtB . p5iupzor51 =
rtB . hwfdd0cjwx + rtB . bceyok5mqb ; rtB . ltgo52xwvz = muDoubleScalarSqrt (
rtB . p5iupzor51 ) ; rtB . hzjl3fcwi5 = rtB . jaahpo4mr4 * rtB . jaahpo4mr4 ;
rtB . hfqwxvmot4 = rtB . fivxhoygb2 * rtB . fivxhoygb2 ; rtB . p5fjvfjsaq =
rtB . hzjl3fcwi5 + rtB . hfqwxvmot4 ; rtB . mxdivmdlgg = muDoubleScalarSqrt (
rtB . p5fjvfjsaq ) ; rtB . hjoq3fd24c = rtB . jaahpo4mr4 / rtB . mxdivmdlgg ;
minV = rtB . hjoq3fd24c ; if ( minV > 1.0 ) { minV = 1.0 ; } else { if ( minV
< - 1.0 ) { minV = - 1.0 ; } } rtB . nvcsxqn2zd = muDoubleScalarAcos ( minV )
; rtB . eyk5wm4rwl = ( rtB . fivxhoygb2 > rtP . u_Value_m0xa3ft5bf ) ; rtB .
ma1xu5nnvc = rtB . nvcsxqn2zd * rtP . u54_Value_nvtlda2oeq ; if ( rtB .
eyk5wm4rwl ) { rtB . axi0fgip5c = rtB . nvcsxqn2zd ; } else { rtB .
axi0fgip5c = rtB . ma1xu5nnvc ; } rtB . jsgo1z1x51 = rtP . u_Value_hkyziycqsm
+ rtB . kpdsr4w32s ; rtB . lcorj1twi3 = rtP . u_Value_hkyziycqsm + rtB .
pihsgrzb25 ; rtB . b1aohu3qxw = rtB . gbj2i2jpi4 * rtB . gbj2i2jpi4 ; rtB .
k0pa5kc0wt = rtB . kkyh4hqoaa * rtB . kkyh4hqoaa ; rtB . nrgahzl1xw = rtB .
b1aohu3qxw + rtB . k0pa5kc0wt ; rtB . guu2guuws2 = muDoubleScalarSqrt ( rtB .
nrgahzl1xw ) ; rtB . ljf4fcvmhw = rtB . gbj2i2jpi4 / rtB . guu2guuws2 ; minV
= rtB . ljf4fcvmhw ; if ( minV > 1.0 ) { minV = 1.0 ; } else { if ( minV < -
1.0 ) { minV = - 1.0 ; } } rtB . lubwjxexpu = muDoubleScalarAcos ( minV ) ;
rtB . ez0jgwczcr = ( rtB . kkyh4hqoaa > rtP . u_Value_bxxwcuzf4h ) ; rtB .
llkgh1r5q4 = rtB . lubwjxexpu * rtP . u54_Value_lzc0i3l03r ; if ( rtB .
ez0jgwczcr ) { rtB . bklrpa44xp = rtB . lubwjxexpu ; } else { rtB .
bklrpa44xp = rtB . llkgh1r5q4 ; } rtB . pmjinc03yw = rtP . u_Value_hkyziycqsm
+ rtB . plrtqrlkdv ; rtB . gfkd05btay = rtP . u_Value_hkyziycqsm + rtB .
or1bi1t23w ; rtB . oxgwksudte = rtB . chxnhzvx4o * rtB . chxnhzvx4o ; rtB .
dbuyxhfljf = rtB . n0bgrn3hxl * rtB . n0bgrn3hxl ; rtB . gg5bwwro5l = rtB .
oxgwksudte + rtB . dbuyxhfljf ; rtB . npzogkbiug = muDoubleScalarSqrt ( rtB .
gg5bwwro5l ) ; rtB . joxdfst4ut = rtB . chxnhzvx4o / rtB . npzogkbiug ; minV
= rtB . joxdfst4ut ; if ( minV > 1.0 ) { minV = 1.0 ; } else { if ( minV < -
1.0 ) { minV = - 1.0 ; } } rtB . hjlepy3y3n = muDoubleScalarAcos ( minV ) ;
rtB . j5yma4uhzk = ( rtB . n0bgrn3hxl > rtP . u_Value_l0u1qhqrxt ) ; rtB .
i3fpfo0tpq = rtB . hjlepy3y3n * rtP . u54_Value_nfv2hwtcgt ; if ( rtB .
j5yma4uhzk ) { rtB . cb1fzzb0lz = rtB . hjlepy3y3n ; } else { rtB .
cb1fzzb0lz = rtB . i3fpfo0tpq ; } rtB . n4eadhgbwx = rtP . u_Value_hkyziycqsm
+ rtB . bkfivkkl2o ; rtB . obhjbbjvoz = rtP . u_Value_hkyziycqsm + rtB .
kl24dmgzdr ; rtB . nadbvgowuz = true ; rtB . kspoxb2j1y = ( rtB . nadbvgowuz
&& ( rtP . u_Value_luxc10x2ch != 0.0 ) ) ; rtB . oe3z3ofj5v = true ; rtB .
gjgc5vzmvw = ( rtB . oe3z3ofj5v && ( rtP . u_Value_luxc10x2ch != 0.0 ) ) ;
rtB . k432ypcxlh = true ; rtB . hevngmr4q4 = ( rtB . k432ypcxlh && ( rtP .
u_Value_luxc10x2ch != 0.0 ) ) ; rtB . e2ltxh3gsr = ( ( rtP .
u_Value_hkyziycqsm != 0.0 ) || rtB . hevngmr4q4 ) ; rtB . hsoy2odhbx = ( (
rtP . u_Value_hkyziycqsm != 0.0 ) || rtB . kspoxb2j1y ) ; rtB . fg2ble3kcb =
( ( rtP . u_Value_hkyziycqsm != 0.0 ) || rtB . gjgc5vzmvw ) ; rtB .
lm2e5d3i5a = ( ( rtP . u_Value_hkyziycqsm != 0.0 ) && rtB . hevngmr4q4 && rtB
. kspoxb2j1y && rtB . gjgc5vzmvw ) ; rtB . k53iwvpvph = rtP .
u_Value_h5ek13jpgh + rtP . u9_Value_pmeesf41jp ; rtB . cmbsfr5c55 = rtP .
u_Value_h5ek13jpgh + rtP . u1_Value_gwityscxwj ; rtB . grk4jm4os0 = 0.0 * rtP
. u_Value_mpihktmmlg ; rtB . onnh5dkb0p = 0.0 * rtP . u_Value_nbijboof3r ;
rtB . i4ij4qt3ch = rtB . grk4jm4os0 + rtB . onnh5dkb0p ; rtB . dxylcvlt0l =
0.0 * rtP . u_Value_mpihktmmlg ; rtB . k4hv5mfno0 = 0.0 * rtP .
u_Value_ol0blkdmze ; rtB . ijawl5zy0j = rtB . dxylcvlt0l + rtB . k4hv5mfno0 ;
rtB . ixjwhyhufz = rtB . i4ij4qt3ch + rtB . ijawl5zy0j ; rtB . itrypfmfxg =
rtB . ixjwhyhufz ; rtB . m3ky5tdvnl = 0.0 * rtP . u_Value_mpihktmmlg ; rtB .
eheo51tkpy = 0.0 * rtP . u_Value_ol0blkdmze ; rtB . jxanqyfpjn = rtB .
m3ky5tdvnl + rtB . eheo51tkpy ; rtB . frg5sdnl02 = 0.0 * rtP .
u_Value_mpihktmmlg ; rtB . orxhtf2wec = 0.0 * rtP . u_Value_nbijboof3r ; rtB
. hbdjfp5k1i = rtB . frg5sdnl02 + rtB . orxhtf2wec ; rtB . lkfgoj2qc5 = rtB .
jxanqyfpjn + rtB . hbdjfp5k1i ; rtB . m3ifjmzrfv = rtB . lkfgoj2qc5 ; rtB .
midehfrpir = 0.0 * rtP . u_Value_mpihktmmlg ; rtB . cj2n2s5xmn = 0.0 * rtP .
u_Value_ol0blkdmze ; rtB . ckaqrnttqk = rtB . midehfrpir - rtB . cj2n2s5xmn ;
rtB . dppvcoeoa5 = 0.0 * rtP . u_Value_mpihktmmlg ; rtB . m1wsxlobv5 = 0.0 *
rtP . u_Value_nbijboof3r ; rtB . ba3rd41usk = rtB . dppvcoeoa5 - rtB .
m1wsxlobv5 ; rtB . ph1tn0rpjj = rtB . ckaqrnttqk + rtB . ba3rd41usk ; rtB .
m0yuztvzvj = rtB . ph1tn0rpjj ; rtB . oitamojhxu = rtB . m0yuztvzvj * rtP .
u7_Value_nlwoeh544c ; rtB . akso5lzyx5 = 0.0 * rtP . u_Value_mpihktmmlg ; rtB
. okbdozdlx5 = 0.0 * rtP . u_Value_nbijboof3r ; rtB . hh5ah5uxdo = rtB .
akso5lzyx5 - rtB . okbdozdlx5 ; rtB . hvsxbcvka1 = 0.0 * rtP .
u_Value_mpihktmmlg ; rtB . pxsoy5jzbf = 0.0 * rtP . u_Value_ol0blkdmze ; rtB
. izax25saco = rtB . hvsxbcvka1 - rtB . pxsoy5jzbf ; rtB . fca0lybxe5 = rtB .
hh5ah5uxdo + rtB . izax25saco ; rtB . gro4k4wogm = rtB . fca0lybxe5 ; rtB .
lsfuppb5lb = rtB . gro4k4wogm * rtP . u7_Value_nlwoeh544c ; rtB . me5dpwp20s
= rtB . itrypfmfxg * rtP . u7_Value_nlwoeh544c ; rtB . dtm0312to1 = rtB .
m3ifjmzrfv * rtP . u7_Value_nlwoeh544c ; rtB . ojm40kdwon = ( rtP .
u8_Value_lhwj3cth4y == rtP . u_Value_kkn14jtfyq ) ; rtB . ej40onpoj3 = ( rtP
. u8_Value_lhwj3cth4y == rtP . u_Value_kkn14jtfyq ) ; rtB . aizy54nz41 = (
rtB . ojm40kdwon && rtB . ej40onpoj3 ) ; rtB . p0me2ise2s = true ; rtB .
bfaj1ezwe0 = ( rtP . T1_DelayType_iqx01xujrs == rtP .
Constant_Value_frpzwq52sg ) ; rtB . mspaunmj5m = ! rtB . bfaj1ezwe0 ;
bhc3qgnuicTID2 ( rtS , & rtB . boy5y1bmz3 , & rtDW . boy5y1bmz3 , & rtP .
boy5y1bmz3 ) ; on20slldftTID2 ( rtS , & rtB . jvoggbn2yd , & rtDW .
jvoggbn2yd , & rtP . jvoggbn2yd ) ; rtB . fl54yqipkw = ( rtP .
T2_DelayType_ls1abdv4uc == rtP . Constant_Value_a4aafxxsuh ) ; rtB .
iqgtpt55z5 = ! rtB . fl54yqipkw ; bhc3qgnuicTID2 ( rtS , & rtB . ojnin3nhvw ,
& rtDW . ojnin3nhvw , & rtP . ojnin3nhvw ) ; on20slldftTID2 ( rtS , & rtB .
ithg0f1jiq , & rtDW . ithg0f1jiq , & rtP . ithg0f1jiq ) ; rtB . f1xwmtbavl =
( rtP . T3_DelayType_fraenq3g1b == rtP . Constant_Value_pkf0yi5y4n ) ; rtB .
mhw3zqrf0v = ! rtB . f1xwmtbavl ; bhc3qgnuicTID2 ( rtS , & rtB . bhbl51zywo ,
& rtDW . bhbl51zywo , & rtP . bhbl51zywo ) ; on20slldftTID2 ( rtS , & rtB .
p2xsu5tknt , & rtDW . p2xsu5tknt , & rtP . p2xsu5tknt ) ; rtB . exlkhevyls =
( rtP . T1_DelayType_am24y4rwkq == rtP . Constant_Value_gzyv5ktan2 ) ; rtB .
gxlekcoxi2 = ! rtB . exlkhevyls ; bhc3qgnuicTID2 ( rtS , & rtB . allgw5ptgk ,
& rtDW . allgw5ptgk , & rtP . allgw5ptgk ) ; on20slldftTID2 ( rtS , & rtB .
hvibrw2awu , & rtDW . hvibrw2awu , & rtP . hvibrw2awu ) ; rtB . mmh5eqfznn =
( rtP . T2_DelayType_e5ub1dmeej == rtP . Constant_Value_erta51b3q5 ) ; rtB .
lvi5ymbt3u = ! rtB . mmh5eqfznn ; bhc3qgnuicTID2 ( rtS , & rtB . br1i12egyj ,
& rtDW . br1i12egyj , & rtP . br1i12egyj ) ; on20slldftTID2 ( rtS , & rtB .
f5lqeh5sbp , & rtDW . f5lqeh5sbp , & rtP . f5lqeh5sbp ) ; rtB . eog3r53qxh =
( rtP . T1_DelayType_a35gqth4mk == rtP . Constant_Value_bvex4qszct ) ; rtB .
dxajpxc32b = ! rtB . eog3r53qxh ; bhc3qgnuicTID2 ( rtS , & rtB . dewlul2b2r ,
& rtDW . dewlul2b2r , & rtP . dewlul2b2r ) ; on20slldftTID2 ( rtS , & rtB .
lf152u5jm4 , & rtDW . lf152u5jm4 , & rtP . lf152u5jm4 ) ; rtB . jph3nsjcrd =
( rtP . T2_DelayType_d4pe42n5xu == rtP . Constant_Value_biwbmkb5qa ) ; rtB .
dy0y5mqrgw = ! rtB . jph3nsjcrd ; bhc3qgnuicTID2 ( rtS , & rtB . kd31jkgljk ,
& rtDW . kd31jkgljk , & rtP . kd31jkgljk ) ; on20slldftTID2 ( rtS , & rtB .
ivzhqxkykj , & rtDW . ivzhqxkykj , & rtP . ivzhqxkykj ) ; rtB . gll2jods23 =
( rtP . T1_DelayType_dh3dkhhony == rtP . Constant_Value_gbssbywxq3 ) ; rtB .
hpmke4ckgb = ! rtB . gll2jods23 ; bhc3qgnuicTID2 ( rtS , & rtB . d5dfks3vhb ,
& rtDW . d5dfks3vhb , & rtP . d5dfks3vhb ) ; on20slldftTID2 ( rtS , & rtB .
b13ln335zj , & rtDW . b13ln335zj , & rtP . b13ln335zj ) ; rtB . hizqf10xce =
( rtP . T2_DelayType_pgeftvpoaf == rtP . Constant_Value_hsyezicavr ) ; rtB .
nvymz5yhdv = ! rtB . hizqf10xce ; bhc3qgnuicTID2 ( rtS , & rtB . jcev1klzwf ,
& rtDW . jcev1klzwf , & rtP . jcev1klzwf ) ; on20slldftTID2 ( rtS , & rtB .
d2dryokhth , & rtDW . d2dryokhth , & rtP . d2dryokhth ) ; switch ( ( int32_T
) rtP . Constant1_Value_h0ppemjcxo ) { case 1 : rtB . jmp2nve0rj [ 0 ] = rtP
. posedge_Value_med25hs022 [ 0 ] ; rtB . jmp2nve0rj [ 1 ] = rtP .
posedge_Value_med25hs022 [ 1 ] ; break ; case 2 : rtB . jmp2nve0rj [ 0 ] =
rtP . negedge_Value_mwprg3ki1m [ 0 ] ; rtB . jmp2nve0rj [ 1 ] = rtP .
negedge_Value_mwprg3ki1m [ 1 ] ; break ; default : rtB . jmp2nve0rj [ 0 ] =
rtP . eitheredge_Value_mc14ux4bg4 [ 0 ] ; rtB . jmp2nve0rj [ 1 ] = rtP .
eitheredge_Value_mc14ux4bg4 [ 1 ] ; break ; } switch ( ( int32_T ) rtP .
Constant1_Value_aecpos4ruo ) { case 1 : rtB . byqfsens1k [ 0 ] = rtP .
posedge_Value_ftcex4iwfm [ 0 ] ; rtB . byqfsens1k [ 1 ] = rtP .
posedge_Value_ftcex4iwfm [ 1 ] ; break ; case 2 : rtB . byqfsens1k [ 0 ] =
rtP . negedge_Value_mtm0udb34l [ 0 ] ; rtB . byqfsens1k [ 1 ] = rtP .
negedge_Value_mtm0udb34l [ 1 ] ; break ; default : rtB . byqfsens1k [ 0 ] =
rtP . eitheredge_Value_mta0c4eewt [ 0 ] ; rtB . byqfsens1k [ 1 ] = rtP .
eitheredge_Value_mta0c4eewt [ 1 ] ; break ; } switch ( ( int32_T ) rtP .
Constant1_Value_oj0kau3fee ) { case 1 : rtB . bvkvtsqqsw [ 0 ] = rtP .
posedge_Value_hpjfvcjmkt [ 0 ] ; rtB . bvkvtsqqsw [ 1 ] = rtP .
posedge_Value_hpjfvcjmkt [ 1 ] ; break ; case 2 : rtB . bvkvtsqqsw [ 0 ] =
rtP . negedge_Value_nj4oggh4gc [ 0 ] ; rtB . bvkvtsqqsw [ 1 ] = rtP .
negedge_Value_nj4oggh4gc [ 1 ] ; break ; default : rtB . bvkvtsqqsw [ 0 ] =
rtP . eitheredge_Value_cjvcbook42 [ 0 ] ; rtB . bvkvtsqqsw [ 1 ] = rtP .
eitheredge_Value_cjvcbook42 [ 1 ] ; break ; } rtB . itgjzfbrcz = false ; rtB
. hhzqdhvmmu = ( rtP . Constant6_Value_m1liwvrmf2 == rtP .
Constant_Value_pg4yqspvv5 ) ; if ( rtB . hhzqdhvmmu ) { rtB . fprxwhlbgx =
rtP . Constant1_Value_jlwa0ekrrg ; } else { rtB . fprxwhlbgx = rtP .
Constant6_Value_m1liwvrmf2 ; } rtB . nrw2l054aj = ( rtP .
Constant13_Value_py1xiy0ibz == rtP . Constant20_Value ) ; rtB . hujlybgagd =
( rtP . Constant13_Value_py1xiy0ibz == rtP . Constant21_Value ) ; rtB .
pat0uefyqq = ( rtP . Constant13_Value_py1xiy0ibz == rtP . Constant22_Value )
; rtB . lnvgk2obiv = ( rtP . Constant13_Value_py1xiy0ibz == rtP .
Constant23_Value ) ; rtB . eqsiwryrnq = ( rtP . Constant13_Value_py1xiy0ibz
== rtP . Constant1_Value_mgwpsjiaag ) ; rtB . jge4qya4hc = ( rtP .
Constant13_Value_py1xiy0ibz == rtP . Constant25_Value ) ; rtB . oubybjusp0 =
( rtP . Constant13_Value_py1xiy0ibz == rtP . Constant2_Value_fnwyky3yb0 ) ;
rtB . ky3ixrzmwc = ( rtP . Constant13_Value_py1xiy0ibz == rtP .
Constant4_Value_bk145dmlsi ) ; if ( rtB . nrw2l054aj ) { rtB . oenftlbsmq =
rtP . Constant3_Value_oe1dqsg2p2 ; rtB . hn25uqebqv = rtP .
Constant6_Value_p1gtlxsnjf ; rtB . ppbidan1g0 = rtP .
Constant1_Value_mekerzczcs ; } else { rtB . oenftlbsmq = rtP .
Constant3_Value_lqmhq442ss ; rtB . hn25uqebqv = rtP .
Constant3_Value_lqmhq442ss ; rtB . ppbidan1g0 = rtP .
Constant3_Value_lqmhq442ss ; } if ( rtB . hujlybgagd ) { rtB . hllq0ooprf =
rtP . Constant3_Value_jsls0wnla5 ; rtB . gn0yzrbokc = rtP .
Constant6_Value_meeiy5lusc ; rtB . khhsd0lreh = rtP .
Constant1_Value_gv0o0w4hpu ; } else { rtB . hllq0ooprf = rtB . oenftlbsmq ;
rtB . gn0yzrbokc = rtB . hn25uqebqv ; rtB . khhsd0lreh = rtB . ppbidan1g0 ; }
if ( rtB . pat0uefyqq ) { rtB . ml1ccdlb11 = rtP . Constant3_Value_mysbtvkw0u
; rtB . c55bamfo1f = rtP . Constant6_Value_eqog4q1c1m ; rtB . bboqgjcqdx =
rtP . Constant1_Value_ezxavzy3zw ; } else { rtB . ml1ccdlb11 = rtB .
hllq0ooprf ; rtB . c55bamfo1f = rtB . gn0yzrbokc ; rtB . bboqgjcqdx = rtB .
khhsd0lreh ; } if ( rtB . lnvgk2obiv ) { rtB . gzlnmhyfsu = rtP .
Constant3_Value_ckobmyjz2u ; rtB . pcvluq2vpu = rtP .
Constant6_Value_co31kqeu5g ; rtB . pqlczu5olo = rtP .
Constant1_Value_kz2enlemtu ; } else { rtB . gzlnmhyfsu = rtB . ml1ccdlb11 ;
rtB . pcvluq2vpu = rtB . c55bamfo1f ; rtB . pqlczu5olo = rtB . bboqgjcqdx ; }
if ( rtB . jge4qya4hc ) { rtB . ou14503v5a = rtP . Constant3_Value_dj5up50cow
; rtB . oxtwrnbrzg = rtP . Constant6_Value_kyuenc3reg ; rtB . idj1lqisw0 =
rtP . Constant1_Value_cn4q4jpmyr ; } else { rtB . ou14503v5a = rtB .
gzlnmhyfsu ; rtB . oxtwrnbrzg = rtB . pcvluq2vpu ; rtB . idj1lqisw0 = rtB .
pqlczu5olo ; } if ( rtB . eqsiwryrnq ) { rtB . bbpu3w04na = rtP .
Constant3_Value_lsea5cthsz ; rtB . g3fyrh0mat = rtP .
Constant6_Value_aio33ucogw ; rtB . ixrfdb23gl = rtP .
Constant1_Value_hugvxhmu44 ; } else { rtB . bbpu3w04na = rtB . ou14503v5a ;
rtB . g3fyrh0mat = rtB . oxtwrnbrzg ; rtB . ixrfdb23gl = rtB . idj1lqisw0 ; }
if ( rtB . oubybjusp0 ) { rtB . dtgzlrvmv4 = rtP . Constant3_Value_jnjnlbjypn
; rtB . bmnijg4sgz = rtP . Constant6_Value_kgbkoeyr0l ; rtB . fwnkpyhmw5 =
rtP . Constant1_Value_d0hcufbmk2 ; } else { rtB . dtgzlrvmv4 = rtB .
bbpu3w04na ; rtB . bmnijg4sgz = rtB . g3fyrh0mat ; rtB . fwnkpyhmw5 = rtB .
ixrfdb23gl ; } if ( rtB . ky3ixrzmwc ) { rtB . ph2255r30k = rtP .
Constant10_Value_o2gulseulh ; rtB . bzxe4atotg = rtP .
Constant11_Value_m3t0yjcowt ; rtB . d4ba3f1s5c = rtP .
Constant9_Value_odznroykdg ; } else { rtB . ph2255r30k = rtB . dtgzlrvmv4 ;
rtB . bzxe4atotg = rtB . bmnijg4sgz ; rtB . d4ba3f1s5c = rtB . fwnkpyhmw5 ; }
rtB . i3csokzcao = ( rtP . Constant6_Value_m1liwvrmf2 == rtP .
Constant_Value_cccyahfswq ) ; if ( rtB . i3csokzcao ) { rtB . i1f2menktp =
rtP . Constant1_Value_g35sgkxtm3 ; } else { rtB . i1f2menktp = rtP .
Constant6_Value_m1liwvrmf2 ; } rtB . htws4ppr03 = ( rtP .
Constant15_Value_byqzjgo132 == rtP . Constant20_Value_bbkxz0zlju ) ; rtB .
oubblwxofs = ( rtP . Constant15_Value_byqzjgo132 == rtP .
Constant21_Value_bvf5gb21bq ) ; rtB . l4enpulqfv = ( rtP .
Constant15_Value_byqzjgo132 == rtP . Constant22_Value_mmjc3dl0tj ) ; rtB .
b3mfxwapyt = ( rtP . Constant15_Value_byqzjgo132 == rtP .
Constant23_Value_cmcbteomkj ) ; rtB . anozeymp2v = ( rtP .
Constant15_Value_byqzjgo132 == rtP . Constant1_Value_n3j3t2oefh ) ; rtB .
dwcvwnfefp = ( rtP . Constant15_Value_byqzjgo132 == rtP .
Constant25_Value_b1pgiufzlz ) ; rtB . p3dearmhf4 = ( rtP .
Constant15_Value_byqzjgo132 == rtP . Constant2_Value_htbfjngqxl ) ; rtB .
ptcl2vvegn = ( rtP . Constant15_Value_byqzjgo132 == rtP .
Constant4_Value_ed1n5tikll ) ; if ( rtB . htws4ppr03 ) { rtB . azg5aiyvnm =
rtP . Constant3_Value_kbytjrooaq ; rtB . clpbbvspty = rtP .
Constant6_Value_d2hexexang ; rtB . g5k1oaahhp = rtP .
Constant1_Value_odpzfodmya ; } else { rtB . azg5aiyvnm = rtP .
Constant3_Value_etssewlgi4 ; rtB . clpbbvspty = rtP .
Constant3_Value_etssewlgi4 ; rtB . g5k1oaahhp = rtP .
Constant3_Value_etssewlgi4 ; } if ( rtB . oubblwxofs ) { rtB . bpvtu5vosu =
rtP . Constant3_Value_lwseknfrnm ; rtB . pz4stfkaay = rtP .
Constant6_Value_c00tluxwcf ; rtB . frpn23onyd = rtP .
Constant1_Value_fgh1iw1qke ; } else { rtB . bpvtu5vosu = rtB . azg5aiyvnm ;
rtB . pz4stfkaay = rtB . clpbbvspty ; rtB . frpn23onyd = rtB . g5k1oaahhp ; }
if ( rtB . l4enpulqfv ) { rtB . iwolvgiv0u = rtP . Constant3_Value_jmcb04an3w
; rtB . im5b4wvfe5 = rtP . Constant6_Value_eqoajyycao ; rtB . bxmh5rzf2n =
rtP . Constant1_Value_inmzk4v0xz ; } else { rtB . iwolvgiv0u = rtB .
bpvtu5vosu ; rtB . im5b4wvfe5 = rtB . pz4stfkaay ; rtB . bxmh5rzf2n = rtB .
frpn23onyd ; } if ( rtB . b3mfxwapyt ) { rtB . dutprffs50 = rtP .
Constant3_Value_ligmqxkgvc ; rtB . deirpo1mnq = rtP .
Constant6_Value_d2i1mmoi2w ; rtB . ksicwztbk5 = rtP .
Constant1_Value_p34zlj44a2 ; } else { rtB . dutprffs50 = rtB . iwolvgiv0u ;
rtB . deirpo1mnq = rtB . im5b4wvfe5 ; rtB . ksicwztbk5 = rtB . bxmh5rzf2n ; }
if ( rtB . dwcvwnfefp ) { rtB . kqxm2lcvr2 = rtP . Constant3_Value_axo5dbgmag
; rtB . fx5ipuyjuy = rtP . Constant6_Value_omu2ey22wk ; rtB . hs0qbkhwc4 =
rtP . Constant1_Value_ohbuoxlo5a ; } else { rtB . kqxm2lcvr2 = rtB .
dutprffs50 ; rtB . fx5ipuyjuy = rtB . deirpo1mnq ; rtB . hs0qbkhwc4 = rtB .
ksicwztbk5 ; } if ( rtB . anozeymp2v ) { rtB . iyb0skp41p = rtP .
Constant3_Value_ene5nsipz2 ; rtB . bpobmbqvvo = rtP .
Constant6_Value_clryme1lq3 ; rtB . gk4u5ffxwf = rtP .
Constant1_Value_mjrv4lmm33 ; } else { rtB . iyb0skp41p = rtB . kqxm2lcvr2 ;
rtB . bpobmbqvvo = rtB . fx5ipuyjuy ; rtB . gk4u5ffxwf = rtB . hs0qbkhwc4 ; }
if ( rtB . p3dearmhf4 ) { rtB . mxahf30mgo = rtP . Constant3_Value_kkyaiuqqsh
; rtB . dtvzw04ksg = rtP . Constant6_Value_my0xjur1pp ; rtB . ohpp0ylirx =
rtP . Constant1_Value_cfyidygvft ; } else { rtB . mxahf30mgo = rtB .
iyb0skp41p ; rtB . dtvzw04ksg = rtB . bpobmbqvvo ; rtB . ohpp0ylirx = rtB .
gk4u5ffxwf ; } if ( rtB . ptcl2vvegn ) { rtB . gmwfqnawfk = rtP .
Constant12_Value_j4k2dfs3i4 ; rtB . dzpb0wpbrp = rtP .
Constant14_Value_ndgbv0p1x1 ; rtB . g4nuvepmng = rtP . Constant16_Value ; }
else { rtB . gmwfqnawfk = rtB . mxahf30mgo ; rtB . dzpb0wpbrp = rtB .
dtvzw04ksg ; rtB . g4nuvepmng = rtB . ohpp0ylirx ; } rtB . bbuakhr0ch = ( rtP
. Constant6_Value_m1liwvrmf2 == rtP . Constant_Value_m3u4owe4ec ) ; if ( rtB
. bbuakhr0ch ) { rtB . b5gb4adov3 = rtP . Constant1_Value_h5l5ppl5bo ; } else
{ rtB . b5gb4adov3 = rtP . Constant6_Value_m1liwvrmf2 ; } rtB . inntdijtqo =
( rtP . Constant_Value_gfn3i1wusu == rtP . Constant20_Value_fpl0ixnsuf ) ;
rtB . fu222a1eab = ( rtP . Constant_Value_gfn3i1wusu == rtP .
Constant21_Value_hezjr301ui ) ; rtB . banyhbp1zy = ( rtP .
Constant_Value_gfn3i1wusu == rtP . Constant22_Value_b4bmpftzyh ) ; rtB .
eweursw25g = ( rtP . Constant_Value_gfn3i1wusu == rtP .
Constant23_Value_knhelh01ht ) ; rtB . obkxqrlqqp = ( rtP .
Constant_Value_gfn3i1wusu == rtP . Constant25_Value_ilek2kpp5b ) ; if ( rtB .
inntdijtqo ) { rtB . lr3pvnhefp = rtP . Constant3_Value_czq3gmfuw5 ; rtB .
exftv3oxvk = rtP . Constant6_Value_kejnmbkab5 ; } else { rtB . lr3pvnhefp =
rtP . Constant3_Value_aomi5nm2c0 ; rtB . exftv3oxvk = rtP .
Constant3_Value_aomi5nm2c0 ; } if ( rtB . fu222a1eab ) { rtB . fsuc0nqgep =
rtP . Constant3_Value_f4bw5bbmcu ; rtB . j1btdcdfnb = rtP .
Constant6_Value_kqy3n3mhbq ; } else { rtB . fsuc0nqgep = rtB . lr3pvnhefp ;
rtB . j1btdcdfnb = rtB . exftv3oxvk ; } if ( rtB . banyhbp1zy ) { rtB .
dlvz1aaufw = rtP . Constant3_Value_nvxrvwmga0 ; rtB . hxi5hy35ng = rtP .
Constant6_Value_isnrluywml ; } else { rtB . dlvz1aaufw = rtB . fsuc0nqgep ;
rtB . hxi5hy35ng = rtB . j1btdcdfnb ; } if ( rtB . eweursw25g ) { rtB .
i2wrewdjqy = rtP . Constant3_Value_phbahx3gaj ; rtB . dwsyqat3yu = rtP .
Constant6_Value_c3kbhdqhuu ; } else { rtB . i2wrewdjqy = rtB . dlvz1aaufw ;
rtB . dwsyqat3yu = rtB . hxi5hy35ng ; } if ( rtB . obkxqrlqqp ) { rtB .
psmo35sklv = rtP . Constant1_Value_a30hdcrmd5 ; rtB . k1laehtn1k = rtP .
Constant2_Value_pxeaedxlgc ; } else { rtB . psmo35sklv = rtB . i2wrewdjqy ;
rtB . k1laehtn1k = rtB . dwsyqat3yu ; } rtB . oj1dezaigx = ( rtP .
Constant6_Value_m1liwvrmf2 == rtP . Constant_Value_fnzq4tvavq ) ; if ( rtB .
oj1dezaigx ) { rtB . hzvmm3pybv = rtP . Constant1_Value_mbmqmm21gz ; } else {
rtB . hzvmm3pybv = rtP . Constant6_Value_m1liwvrmf2 ; } rtB . ote32uibrd = (
rtP . Constant8_Value_mqddbo44um == rtP . Constant20_Value_e5qhmjccb1 ) ; rtB
. melsqx5byc = ( rtP . Constant8_Value_mqddbo44um == rtP .
Constant21_Value_pn4hlvawzu ) ; rtB . cggftyepdi = ( rtP .
Constant8_Value_mqddbo44um == rtP . Constant22_Value_fxywregy00 ) ; rtB .
h4chgni5v2 = ( rtP . Constant8_Value_mqddbo44um == rtP .
Constant23_Value_irptuovixy ) ; rtB . fajbccxnm3 = ( rtP .
Constant8_Value_mqddbo44um == rtP . Constant25_Value_djg5p0gqps ) ; if ( rtB
. ote32uibrd ) { rtB . fvpcblg1z2 = rtP . Constant3_Value_ajjenxsttc ; rtB .
ppymzrghcq = rtP . Constant6_Value_nwgbxrbkrm ; } else { rtB . fvpcblg1z2 =
rtP . Constant3_Value_fhjdpz54zl ; rtB . ppymzrghcq = rtP .
Constant3_Value_fhjdpz54zl ; } if ( rtB . melsqx5byc ) { rtB . pvpkeq5ibz =
rtP . Constant3_Value_lk4pd0zsvj ; rtB . mvwn2ecb1i = rtP .
Constant6_Value_dedqvxixw2 ; } else { rtB . pvpkeq5ibz = rtB . fvpcblg1z2 ;
rtB . mvwn2ecb1i = rtB . ppymzrghcq ; } if ( rtB . cggftyepdi ) { rtB .
d02qq4rpxe = rtP . Constant3_Value_k1qtqed30g ; rtB . bswa30ihri = rtP .
Constant6_Value_n1mny5y322 ; } else { rtB . d02qq4rpxe = rtB . pvpkeq5ibz ;
rtB . bswa30ihri = rtB . mvwn2ecb1i ; } if ( rtB . h4chgni5v2 ) { rtB .
bmcj04qnzy = rtP . Constant3_Value_a5t4owgvir ; rtB . ls4wirhary = rtP .
Constant6_Value_ar0i55dyry ; } else { rtB . bmcj04qnzy = rtB . d02qq4rpxe ;
rtB . ls4wirhary = rtB . bswa30ihri ; } if ( rtB . fajbccxnm3 ) { rtB .
ed10njbbmu = rtP . Constant4_Value_ocilplvr0k ; rtB . ek3e1fl33p = rtP .
Constant5_Value_axto1geprm ; } else { rtB . ed10njbbmu = rtB . bmcj04qnzy ;
rtB . ek3e1fl33p = rtB . ls4wirhary ; } rtB . nunmcefaox = ( ( rtP .
Constant20_Value_eexfd5nk20 != 0.0 ) && ( rtP . Constant17_Value != 0.0 ) ) ;
rtB . ay1m1gtqnu = ! ( rtP . Constant20_Value_eexfd5nk20 != 0.0 ) ; rtB .
f2y4g1anxz = ( rtB . ay1m1gtqnu && ( rtP . Constant17_Value != 0.0 ) ) ; rtB
. azicm3ai4d = ! rtB . itgjzfbrcz ; rtB . fvdmdl211j = true ; rtB .
cenlyv43wy = ( rtP . T1_DelayType_opsr1cq3jh == rtP .
Constant_Value_fi0v3gd1gn ) ; rtB . p4fh21c2hg = ! rtB . cenlyv43wy ;
bhc3qgnuicTID2 ( rtS , & rtB . pj2a3t0x3l , & rtDW . pj2a3t0x3l , & rtP .
pj2a3t0x3l ) ; on20slldftTID2 ( rtS , & rtB . c2rt0zs300 , & rtDW .
c2rt0zs300 , & rtP . c2rt0zs300 ) ; switch ( ( int32_T ) rtP .
Constant1_Value_j0vbvounng ) { case 1 : rtB . kcvke53w1x [ 0 ] = rtP .
posedge_Value_fy12ejm4ei [ 0 ] ; rtB . kcvke53w1x [ 1 ] = rtP .
posedge_Value_fy12ejm4ei [ 1 ] ; break ; case 2 : rtB . kcvke53w1x [ 0 ] =
rtP . negedge_Value_ftc2q11qgz [ 0 ] ; rtB . kcvke53w1x [ 1 ] = rtP .
negedge_Value_ftc2q11qgz [ 1 ] ; break ; default : rtB . kcvke53w1x [ 0 ] =
rtP . eitheredge_Value_l4iouw3wch [ 0 ] ; rtB . kcvke53w1x [ 1 ] = rtP .
eitheredge_Value_l4iouw3wch [ 1 ] ; break ; } rtB . mvlqqpu2ck = false ; rtB
. ok3oulsqma = false ; rtB . jkk01k0psn = ( rtP . Constant13_Value_l3g21zjgbe
!= 0.0 ) ; rtB . by52txfadq = ( ( rtP . Constant12_Value_hvdgy4oexz != 0.0 )
|| ( rtP . Constant13_Value_l3g21zjgbe != 0.0 ) || ( rtP .
Constant13_Value_l3g21zjgbe != 0.0 ) || ( rtP . Constant13_Value_l3g21zjgbe
!= 0.0 ) ) ; rtB . d5arzfdg32 = ! rtB . jkk01k0psn ; rtB . opo2e32x2v = ! (
rtP . Constant13_Value_l3g21zjgbe != 0.0 ) ; rtB . egvau1eycn = ( rtB .
opo2e32x2v && ( rtP . Constant2_Value_gln34bn3vp != 0.0 ) ) ; rtB .
e13jrtpc4h = ( ( rtP . Constant1_Value_mxigjefghs != 0.0 ) && ( rtP .
Constant13_Value_l3g21zjgbe != 0.0 ) ) ; rtB . mstar1f0rf = ! ( rtP .
Constant13_Value_l3g21zjgbe != 0.0 ) ; rtB . b4ghzh2ez3 = ( rtB . mstar1f0rf
&& ( rtP . Constant2_Value_gln34bn3vp != 0.0 ) ) ; rtB . cub32xc11t = ( rtB .
e13jrtpc4h || rtB . b4ghzh2ez3 ) ; rtB . lcylmqba3f = ( ( rtP .
Constant1_Value_mxigjefghs != 0.0 ) && ( rtP . Constant13_Value_l3g21zjgbe !=
0.0 ) ) ; rtB . lashxc5sqn = ! ( rtP . Constant13_Value_l3g21zjgbe != 0.0 ) ;
rtB . ppf1xshxid = ( rtB . lashxc5sqn && ( rtP . Constant2_Value_gln34bn3vp
!= 0.0 ) ) ; rtB . nxrqwkox3h = ( rtB . lcylmqba3f || rtB . ppf1xshxid ) ;
rtB . o53rwctbpq = ( ( rtP . Constant1_Value_mxigjefghs != 0.0 ) && ( rtP .
Constant13_Value_l3g21zjgbe != 0.0 ) ) ; rtB . ippcj2z3rr = ( rtB .
o53rwctbpq || rtB . egvau1eycn ) ; rtB . bdiuolr2mz = ! ( rtP .
Constant13_Value_l3g21zjgbe != 0.0 ) ; rtB . daekldgh4z = ! ( rtP .
Constant13_Value_l3g21zjgbe != 0.0 ) ; rtB . ae23whgkhf = ! ( rtP .
Constant13_Value_l3g21zjgbe != 0.0 ) ; rtB . bx3k00kd1o = ( rtB . ok3oulsqma
|| ( rtP . Constant10_Value_jgoflc55y2 != 0.0 ) || ( rtP .
Constant10_Value_jgoflc55y2 != 0.0 ) || ( rtP . Constant10_Value_jgoflc55y2
!= 0.0 ) ) ; rtB . c3pfugfh3m = false ; rtB . aad3hbdttm = ( rtB . c3pfugfh3m
|| ( rtP . Constant10_Value_jgoflc55y2 != 0.0 ) || ( rtP .
Constant10_Value_jgoflc55y2 != 0.0 ) || ( rtP . Constant10_Value_jgoflc55y2
!= 0.0 ) ) ; rtB . ftk3ysnstp = ( ( rtP . Constant_Value_b3tsqirf1k != 0.0 )
|| ( rtP . Constant10_Value_jgoflc55y2 != 0.0 ) || ( rtP .
Constant10_Value_jgoflc55y2 != 0.0 ) || ( rtP . Constant10_Value_jgoflc55y2
!= 0.0 ) ) ; rtB . gygy4d2so4 = ! rtB . bx3k00kd1o ; rtB . p4ezegbo2p = ( rtB
. ftk3ysnstp && rtB . gygy4d2so4 ) ; rtB . htjaz3kvvv = false ; rtB .
kvhrqpq1a4 = true ; rtB . i5ijdg1mbl = ( ( rtP . Constant2_Value_mhmfl1sewx
!= 0.0 ) && rtB . kvhrqpq1a4 ) ; rtB . lrqd0f4cid = ( rtB . htjaz3kvvv || rtB
. i5ijdg1mbl ) ; rtB . dn1m3sgau0 = ( rtB . ok3oulsqma || ( rtP .
Constant11_Value_fj52spnxjl != 0.0 ) || ( rtP . Constant11_Value_fj52spnxjl
!= 0.0 ) || ( rtP . Constant11_Value_fj52spnxjl != 0.0 ) ) ; rtB . kbpnccov5b
= false ; rtB . khtc1wrv1m = ( rtB . kbpnccov5b || ( rtP .
Constant11_Value_fj52spnxjl != 0.0 ) ) ; rtB . dugoe1l42l = ( rtB .
mvlqqpu2ck || ( rtP . Constant11_Value_fj52spnxjl != 0.0 ) ) ; rtB .
plghrktmu3 = ! rtB . dn1m3sgau0 ; rtB . j4bpge551b = ! rtB . dn1m3sgau0 ; rtB
. behnpma0jr = ! rtB . dn1m3sgau0 ; rtB . jfvb44flq5 = ( rtP .
T1_DelayType_jlimzd2hq2 == rtP . Constant_Value_eh13jp1s55 ) ; rtB .
o1rgmf5rqs = ! rtB . jfvb44flq5 ; bhc3qgnuicTID2 ( rtS , & rtB . mgkfvqxti2 ,
& rtDW . mgkfvqxti2 , & rtP . mgkfvqxti2 ) ; on20slldftTID2 ( rtS , & rtB .
hytx1hl5s5 , & rtDW . hytx1hl5s5 , & rtP . hytx1hl5s5 ) ; rtB . niah0e0t52 =
( rtP . T2_DelayType_gmvh2143ey == rtP . Constant_Value_h3emodjh0p ) ; rtB .
ahjyepadcb = ! rtB . niah0e0t52 ; bhc3qgnuicTID2 ( rtS , & rtB . h145orsdu5 ,
& rtDW . h145orsdu5 , & rtP . h145orsdu5 ) ; on20slldftTID2 ( rtS , & rtB .
lfrovwpki3 , & rtDW . lfrovwpki3 , & rtP . lfrovwpki3 ) ; rtB . hubguf2n1v =
( rtP . T3_DelayType_lvrdfj1u3s == rtP . Constant_Value_cvrtghfxqz ) ; rtB .
nixfhlnmmc = ! rtB . hubguf2n1v ; bhc3qgnuicTID2 ( rtS , & rtB . obeddxl4xp ,
& rtDW . obeddxl4xp , & rtP . obeddxl4xp ) ; on20slldftTID2 ( rtS , & rtB .
inwm331xkg , & rtDW . inwm331xkg , & rtP . inwm331xkg ) ; rtB . iitw2p4j23 =
( rtP . T4_DelayType_aw55ilmq3a == rtP . Constant_Value_ippwoya11y ) ; rtB .
jur04llf4l = ! rtB . iitw2p4j23 ; bhc3qgnuicTID2 ( rtS , & rtB . dlrk53odir ,
& rtDW . dlrk53odir , & rtP . dlrk53odir ) ; on20slldftTID2 ( rtS , & rtB .
nci0imhwrv , & rtDW . nci0imhwrv , & rtP . nci0imhwrv ) ; rtB . c4svzwiub2 =
( rtP . T5_DelayType_jgp0criqk3 == rtP . Constant_Value_pyzjogjd4v ) ; rtB .
ho1jpoo2z0 = ! rtB . c4svzwiub2 ; bhc3qgnuicTID2 ( rtS , & rtB . llpwcowx1f ,
& rtDW . llpwcowx1f , & rtP . llpwcowx1f ) ; on20slldftTID2 ( rtS , & rtB .
crzn5z4zte , & rtDW . crzn5z4zte , & rtP . crzn5z4zte ) ; rtB . p40dytx3zu =
( rtP . T6_DelayType_pc2h3g0omr == rtP . Constant_Value_cqw2syrtxs ) ; rtB .
k1emjxsvef = ! rtB . p40dytx3zu ; bhc3qgnuicTID2 ( rtS , & rtB . cq2uqua4yo ,
& rtDW . cq2uqua4yo , & rtP . cq2uqua4yo ) ; on20slldftTID2 ( rtS , & rtB .
lbq0bgvgcl , & rtDW . lbq0bgvgcl , & rtP . lbq0bgvgcl ) ; rtB . oplr5q4s21 =
( rtP . T7_DelayType_fgjyfjholn == rtP . Constant_Value_egbsvw2k0f ) ; rtB .
osnitzd20w = ! rtB . oplr5q4s21 ; bhc3qgnuicTID2 ( rtS , & rtB . dlwrrwcsx5 ,
& rtDW . dlwrrwcsx5 , & rtP . dlwrrwcsx5 ) ; on20slldftTID2 ( rtS , & rtB .
mpydob1efk , & rtDW . mpydob1efk , & rtP . mpydob1efk ) ; rtB . cvfp24mc3f =
( rtP . T1_DelayType_dawotyh2pv == rtP . Constant_Value_hbkjho2x1y ) ; rtB .
jlggomoiae = ! rtB . cvfp24mc3f ; bhc3qgnuicTID2 ( rtS , & rtB . hh2cmxftpw ,
& rtDW . hh2cmxftpw , & rtP . hh2cmxftpw ) ; on20slldftTID2 ( rtS , & rtB .
lkrts5ohpw , & rtDW . lkrts5ohpw , & rtP . lkrts5ohpw ) ; rtB . i3t3iwhfmr =
( rtP . T2_DelayType_hpkn01qkop == rtP . Constant_Value_bpkepix0rv ) ; rtB .
ohceznfo4g = ! rtB . i3t3iwhfmr ; bhc3qgnuicTID2 ( rtS , & rtB . dexvjkg0hj ,
& rtDW . dexvjkg0hj , & rtP . dexvjkg0hj ) ; on20slldftTID2 ( rtS , & rtB .
me023aolbi , & rtDW . me023aolbi , & rtP . me023aolbi ) ; rtB . cjrpmegtn4 =
( rtP . T1_DelayType_ao04at5ead == rtP . Constant_Value_h0nznupprp ) ; rtB .
lkuadplgq1 = ! rtB . cjrpmegtn4 ; bhc3qgnuicTID2 ( rtS , & rtB . g3bxbyypfb ,
& rtDW . g3bxbyypfb , & rtP . g3bxbyypfb ) ; on20slldftTID2 ( rtS , & rtB .
purgdi1jfk , & rtDW . purgdi1jfk , & rtP . purgdi1jfk ) ; rtB . c5ci2jl0on =
( rtP . T2_DelayType_brnfha0vzp == rtP . Constant_Value_nwh5ejib0x ) ; rtB .
l3ycctbi23 = ! rtB . c5ci2jl0on ; bhc3qgnuicTID2 ( rtS , & rtB . bs5m40ynfy ,
& rtDW . bs5m40ynfy , & rtP . bs5m40ynfy ) ; on20slldftTID2 ( rtS , & rtB .
e4qyv4smfd , & rtDW . e4qyv4smfd , & rtP . e4qyv4smfd ) ; rtB . j0ubvz5m4h =
( rtP . T1_DelayType_pbyaxk4jky == rtP . Constant_Value_bzikjtxngm ) ; rtB .
gz1ov3colo = ! rtB . j0ubvz5m4h ; bhc3qgnuicTID2 ( rtS , & rtB . ngrhemover ,
& rtDW . ngrhemover , & rtP . ngrhemover ) ; on20slldftTID2 ( rtS , & rtB .
lu12zasvyl , & rtDW . lu12zasvyl , & rtP . lu12zasvyl ) ; rtB . aiibyohiyh =
( rtP . T2_DelayType_kxiwo2dsb4 == rtP . Constant_Value_keqnucbbrh ) ; rtB .
ejunrbhwth = ! rtB . aiibyohiyh ; bhc3qgnuicTID2 ( rtS , & rtB . onemmt3xbs ,
& rtDW . onemmt3xbs , & rtP . onemmt3xbs ) ; on20slldftTID2 ( rtS , & rtB .
myp2dcpbvg , & rtDW . myp2dcpbvg , & rtP . myp2dcpbvg ) ; rtB . j3xru3qgvr =
! ( rtP . Constant3_Value_donksybvgr != 0.0 ) ; rtB . i41w1spys5 = ! ( rtP .
Constant1_Value_h2l3yfta2m != 0.0 ) ; rtB . czygesorfg = ! ( rtP .
Constant2_Value_idz5g0e2ns != 0.0 ) ; rtB . ivr4rwq42s = ( rtB . j3xru3qgvr
&& rtB . i41w1spys5 && rtB . czygesorfg ) ; switch ( ( int32_T ) rtP .
Constant1_Value_eyvw1th1wq ) { case 1 : rtB . mgblsyen2k [ 0 ] = rtP .
posedge_Value_ozfinsodwb [ 0 ] ; rtB . mgblsyen2k [ 1 ] = rtP .
posedge_Value_ozfinsodwb [ 1 ] ; break ; case 2 : rtB . mgblsyen2k [ 0 ] =
rtP . negedge_Value_imealx4nba [ 0 ] ; rtB . mgblsyen2k [ 1 ] = rtP .
negedge_Value_imealx4nba [ 1 ] ; break ; default : rtB . mgblsyen2k [ 0 ] =
rtP . eitheredge_Value_lcmutznrfn [ 0 ] ; rtB . mgblsyen2k [ 1 ] = rtP .
eitheredge_Value_lcmutznrfn [ 1 ] ; break ; } switch ( ( int32_T ) rtP .
Constant1_Value_prb0eylcqw ) { case 1 : rtB . emgyzaybxu [ 0 ] = rtP .
posedge_Value_ik0zbwauus [ 0 ] ; rtB . emgyzaybxu [ 1 ] = rtP .
posedge_Value_ik0zbwauus [ 1 ] ; break ; case 2 : rtB . emgyzaybxu [ 0 ] =
rtP . negedge_Value_nem2g02k30 [ 0 ] ; rtB . emgyzaybxu [ 1 ] = rtP .
negedge_Value_nem2g02k30 [ 1 ] ; break ; default : rtB . emgyzaybxu [ 0 ] =
rtP . eitheredge_Value_aujwij4vsa [ 0 ] ; rtB . emgyzaybxu [ 1 ] = rtP .
eitheredge_Value_aujwij4vsa [ 1 ] ; break ; } switch ( ( int32_T ) rtP .
Constant1_Value_ctzvd13mbv ) { case 1 : rtB . khzixdgyr4 [ 0 ] = rtP .
posedge_Value_aeqttdscgk [ 0 ] ; rtB . khzixdgyr4 [ 1 ] = rtP .
posedge_Value_aeqttdscgk [ 1 ] ; break ; case 2 : rtB . khzixdgyr4 [ 0 ] =
rtP . negedge_Value_aktuezniz1 [ 0 ] ; rtB . khzixdgyr4 [ 1 ] = rtP .
negedge_Value_aktuezniz1 [ 1 ] ; break ; default : rtB . khzixdgyr4 [ 0 ] =
rtP . eitheredge_Value_m1sblo45fx [ 0 ] ; rtB . khzixdgyr4 [ 1 ] = rtP .
eitheredge_Value_m1sblo45fx [ 1 ] ; break ; } rtB . n2vcnfd4ps = ( uint8_T )
( rtP . AlphaBetaZerotodq0_Alignment == rtP . CompareToConstant_const ) ; rtB
. dq5pywjhv3 = ( uint8_T ) ( rtP . AlphaBetaZerotodq0_Alignment == rtP .
CompareToConstant1_const ) ; if ( ssIsMajorTimeStep ( rtS ) ) { srUpdateBC (
rtDW . eeuhsl20cs ) ; } rtB . mbtqxty3np = ( uint8_T ) ( rtP .
AlphaBetaZerotodq0_Alignment_ozwpf24wbm == rtP .
CompareToConstant_const_opnccmsnc5 ) ; rtB . mzpjbmxfwp = ( uint8_T ) ( rtP .
AlphaBetaZerotodq0_Alignment_ozwpf24wbm == rtP .
CompareToConstant1_const_cicjv3b0gf ) ; rtB . om0empctma = 0.0 ; rtB .
bx1z0pb2ry = 0.0 ; rtB . fnsw1bhvmj = rtB . om0empctma + rtB . bx1z0pb2ry ;
rtB . nf4ldv14hc = muDoubleScalarSqrt ( rtB . fnsw1bhvmj ) ; rtB . c2nts3pzpu
= ( rtB . nf4ldv14hc > rtP . u9_Value_drtq23l3nx ) ; rtB . bvlgh3rhsj = true
; rtB . hiouz3k3hf = true ; rtB . iaqjeb0zon = 0.0 ; rtB . k2q242jq4q = 0.0 ;
rtB . ebgme1xcjv = rtB . iaqjeb0zon + rtB . k2q242jq4q ; rtB . cpdezbqdit =
muDoubleScalarSqrt ( rtB . ebgme1xcjv ) ; rtB . ey2r001gvf = ( rtB .
cpdezbqdit > rtP . u9_Value_drtq23l3nx ) ; rtB . g40c4fravk = ( rtP .
u0_Value_dfudxpbfjt == rtP . u0_Value_kbeulphqgl ) ; rtB . cvxpofecuq = ( rtP
. u8_Value_jcqgcaahbl == rtP . u0_Value_kbeulphqgl ) ; rtB . oay5qpaedo = (
rtB . g40c4fravk || rtB . cvxpofecuq ) ; rtB . hebvbjqekx = rtP .
u0_Value_dfudxpbfjt / rtP . u8_Value_jcqgcaahbl ; if ( rtB . oay5qpaedo ) {
rtB . fwbuvpwccv = rtP . u9_Value_ckusvgpnkm ; } else { rtB . fwbuvpwccv =
rtB . hebvbjqekx ; } rtB . nmdunsunog = rtB . fwbuvpwccv * rtP .
u9_Value_drtq23l3nx ; rtB . g1mzgdtpxe = ( rtP . u1_Value_hqheaodncp == rtP .
u0_Value_lc53db0dmf ) ; rtB . p4yeawrw5i = ( rtP . u5_Value_g4sp2qc2yk == rtP
. u0_Value_lc53db0dmf ) ; rtB . jrfxlccjrr = ( rtB . g1mzgdtpxe || rtB .
p4yeawrw5i ) ; rtB . avrvkn3oly = rtP . u1_Value_hqheaodncp / rtP .
u5_Value_g4sp2qc2yk ; if ( rtB . jrfxlccjrr ) { rtB . pzaolrfodg = rtP .
u9_Value_ba02hlytwi ; } else { rtB . pzaolrfodg = rtB . avrvkn3oly ; } rtB .
hj12c0u10s = rtB . pzaolrfodg * rtP . u6_Value_oe2fu30vrp ; rtB . ovna3tg02i
= 0.0 ; rtB . hjwma4ev0k = 0.0 ; rtB . hsip5qri0m = rtB . ovna3tg02i + rtB .
hjwma4ev0k ; rtB . g1hmxec4r2 = muDoubleScalarSqrt ( rtB . hsip5qri0m ) ; rtB
. ae0hls5jne = ( rtB . g1hmxec4r2 > rtP . u6_Value_oe2fu30vrp ) ; minV =
muDoubleScalarMin ( rtB . nmdunsunog , rtB . hj12c0u10s ) ; rtB . nvmto2zef3
= minV ; rtB . ixwknequqy = 0.0 ; rtB . enxc5eoj0p = 0.0 ; rtB . ftwyyx2s5x =
rtB . ixwknequqy + rtB . enxc5eoj0p ; rtB . hogedpxqbs = muDoubleScalarSqrt (
rtB . ftwyyx2s5x ) ; rtB . lso32itsny = ( rtB . hogedpxqbs > rtP .
u6_Value_oe2fu30vrp ) ; rtB . m0choj5oai = 0.0 ; rtB . ex0t4jagrg = 0.0 ; rtB
. du4cvbyiw4 = rtB . m0choj5oai + rtB . ex0t4jagrg ; rtB . dmw0o4tlfo =
muDoubleScalarSqrt ( rtB . du4cvbyiw4 ) ; rtB . huukrkmwe1 = ( rtB .
dmw0o4tlfo > rtP . u6_Value_oe2fu30vrp ) ; rtB . pvhdlczmm2 = 0.0 ; rtB .
mwgsoxx5h0 = 0.0 ; rtB . flmewqqfwb = rtB . pvhdlczmm2 + rtB . mwgsoxx5h0 ;
rtB . c3bk23pfi0 = muDoubleScalarSqrt ( rtB . flmewqqfwb ) ; rtB . hvudnvvnmj
= ( rtB . c3bk23pfi0 > rtP . u9_Value_drtq23l3nx ) ; rtB . ep4lo31tgg = 0.0 ;
rtB . d0ylkrdzps = 0.0 ; rtB . ckixtloxqm = rtB . ep4lo31tgg + rtB .
d0ylkrdzps ; rtB . ge0buwh1dv = muDoubleScalarSqrt ( rtB . ckixtloxqm ) ; rtB
. fbsjnue34g = ( rtB . ge0buwh1dv > rtP . u6_Value_oe2fu30vrp ) ; rtB .
i0ajfkj3ns = ( rtP . T1_DelayType_pc3tzgy4tw == rtP .
Constant_Value_j5gs40f1or ) ; rtB . eibnca50sv = ! rtB . i0ajfkj3ns ;
bhc3qgnuicTID2 ( rtS , & rtB . grby3szfkd , & rtDW . grby3szfkd , & rtP .
grby3szfkd ) ; on20slldftTID2 ( rtS , & rtB . dhpm1tyfmn , & rtDW .
dhpm1tyfmn , & rtP . dhpm1tyfmn ) ; rtB . fxn1ue5bba = ( rtP .
T2_DelayType_cxckwbg3gz == rtP . Constant_Value_gujjkcerti ) ; rtB .
dwaxzbytpi = ! rtB . fxn1ue5bba ; bhc3qgnuicTID2 ( rtS , & rtB . dzcv3as4ol ,
& rtDW . dzcv3as4ol , & rtP . dzcv3as4ol ) ; on20slldftTID2 ( rtS , & rtB .
nvltfn14rj , & rtDW . nvltfn14rj , & rtP . nvltfn14rj ) ; rtB . gs1tauzuoz =
( rtP . T3_DelayType_cqn1bua1n0 == rtP . Constant_Value_pgem0amozt ) ; rtB .
nh31osveik = ! rtB . gs1tauzuoz ; bhc3qgnuicTID2 ( rtS , & rtB . jdzdt1ap5b ,
& rtDW . jdzdt1ap5b , & rtP . jdzdt1ap5b ) ; on20slldftTID2 ( rtS , & rtB .
jtynvhttl4 , & rtDW . jtynvhttl4 , & rtP . jtynvhttl4 ) ; rtB . jzxofq1tg5 =
( rtP . T4_DelayType_nqxwaccf41 == rtP . Constant_Value_bqrwmd1jbo ) ; rtB .
m0txojuzpn = ! rtB . jzxofq1tg5 ; bhc3qgnuicTID2 ( rtS , & rtB . f41hg5wei1 ,
& rtDW . f41hg5wei1 , & rtP . f41hg5wei1 ) ; on20slldftTID2 ( rtS , & rtB .
fvh2zdqaau , & rtDW . fvh2zdqaau , & rtP . fvh2zdqaau ) ; rtB . mht0g0r5iz =
false ; rtB . hmor11nynp = ( rtB . mht0g0r5iz || ( rtP .
Constant1_Value_hwnkd044dm != 0.0 ) || ( rtP . Constant1_Value_hwnkd044dm !=
0.0 ) || ( rtP . Constant1_Value_hwnkd044dm != 0.0 ) ) ; rtB . fksjfhc3xx = (
( rtP . Constant_Value_b5r3o3yt5v != 0.0 ) || ( rtP .
Constant1_Value_hwnkd044dm != 0.0 ) || ( rtP . Constant1_Value_hwnkd044dm !=
0.0 ) || ( rtP . Constant1_Value_hwnkd044dm != 0.0 ) ) ; rtB . fs1fzxxgqc = (
( rtP . Constant_Value_b5r3o3yt5v != 0.0 ) || ( rtP .
Constant1_Value_hwnkd044dm != 0.0 ) || ( rtP . Constant1_Value_hwnkd044dm !=
0.0 ) || ( rtP . Constant1_Value_hwnkd044dm != 0.0 ) ) ; rtB . gqqqc2ezgy = !
rtB . hmor11nynp ; rtB . bt0130pijc = ( rtB . fs1fzxxgqc && rtB . gqqqc2ezgy
) ; rtB . dznrzuaexm = ( ( rtP . Constant1_Value_pqcvrrmqnk != 0.0 ) && ( rtP
. Constant1_Value_hwnkd044dm != 0.0 ) ) ; rtB . kwvnwelrc1 = ! ( rtP .
Constant1_Value_hwnkd044dm != 0.0 ) ; rtB . nbkvfvqb5l = ( ( rtP .
Constant2_Value_osrql13tcr != 0.0 ) && rtB . kwvnwelrc1 ) ; rtB . i04s1r5t2j
= ( rtB . dznrzuaexm || rtB . nbkvfvqb5l ) ; rtB . ks4my0k3sr = ( rtP .
DT1_DelayType == rtP . Constant_Value_olz0hpfdbt ) ; rtB . dbamzcq2yi = ! rtB
. ks4my0k3sr ; bhc3qgnuicTID2 ( rtS , & rtB . feptx2ayjl , & rtDW .
feptx2ayjl , & rtP . feptx2ayjl ) ; on20slldftTID2 ( rtS , & rtB . l2r2uh0izk
, & rtDW . l2r2uh0izk , & rtP . l2r2uh0izk ) ; rtB . okz3124rda = ( rtP .
DT2_DelayType == rtP . Constant_Value_jawtdf1rgj ) ; rtB . dqmtufmhv1 = ! rtB
. okz3124rda ; bhc3qgnuicTID2 ( rtS , & rtB . nvnct41vnp , & rtDW .
nvnct41vnp , & rtP . nvnct41vnp ) ; on20slldftTID2 ( rtS , & rtB . labvjdxfi3
, & rtDW . labvjdxfi3 , & rtP . labvjdxfi3 ) ; rtB . mk3unqym3s = true ; rtB
. bxxovnfyd0 = rtP . u9_Value_hokzeyiypf ; rtB . dk1nprd0de = rtP .
u0_Value_mafmzm4vri ; rtB . k1bysmqrzk = rtP . u1_Value_jwsffdue1l ; rtB .
hszk45dxra = rtP . u2_Value_j51aevp15z ; rtB . eyqxityiri = rtP .
u3_Value_hd3jtr0ii1 ; rtB . mwh5flfehb = rtP . u4_Value_ol2lsck3un ; rtB .
l0pudo43wd = rtP . u5_Value_fdit1mfklm ; rtB . npdthc2tw5 = rtP .
u6_Value_galnhml3na ; rtB . haqrkptrg2 = rtP . u7_Value_mdxz2w5izo ; rtB .
gcsfblxnw1 = rtP . u8_Value_ofv3h5yaje ; rtB . mv0hzsp0of = rtP .
u9_Value_bnnoxbiefm ; rtB . nuyrh0xnql = rtP . u0_Value_epcns2elzj ; rtB .
p1fpyutmvs = rtP . u1_Value_kg0i4grnu4 ; rtB . a24iirrzj3 = rtP .
u2_Value_b4cgva4s3l ; rtB . lqxuboilj4 = rtP . u3_Value_lqhtesjvvg ; rtB .
brmg4rqiyt = rtP . u4_Value_fqzuk3cw5i ; rtB . nlo4flhvgy = rtP .
u5_Value_aqttzukdr1 ; rtB . jmfayzjun2 = rtP . u6_Value_ghgy2poayj ; rtB .
mdp2ykvsly = rtP . u7_Value_iiaa4p3pe1 ; rtB . hva3cowmcg = rtP .
u9_Value_knt2jnv0u1 ; rtB . nkhpqzywkw = rtP . u0_Value_jizhnskjcb ; rtB .
g2bzbdu4ux = rtP . u1_Value_o3zcze02eq ; rtB . pi2aw4lqij = rtP .
u2_Value_dbpgbwakft ; rtB . kr5ncwps5g = rtP . u3_Value_kzwm5v5ndm ; rtB .
deed4mozfx = rtP . u4_Value_d2lkjq1enr ; rtB . entaae0pfl = rtP .
u5_Value_ipcflxpykt ; rtB . kcvao53zxk = rtP . u6_Value_p5q3sxvg1k ; rtB .
f1ki32tr0i = rtP . u7_Value_nhn10kic0y ; rtB . kj44xk1hrp = rtP .
u8_Value_jawdy4folg ; rtB . asgrnyfkrq = rtP . u9_Value_fulegdydib ; rtB .
c13hk3cxlu = rtP . u0_Value_bb0xh0aiin ; rtB . nho5tnb4dc = rtP .
u1_Value_egfymg22ka ; rtB . icur1uukp2 = rtP . u2_Value_kbhdsz1yra ; rtB .
mjhma3t2w5 = rtP . u3_Value_cw43rz3k35 ; rtB . ip4tox1s0x = rtP .
u4_Value_g1sernv2w3 ; rtB . evw2kx1dcj = rtP . u5_Value_nlqmuyqd3c ; rtB .
hu2oft3h4d = rtP . u6_Value_ntf0jx5o4a ; rtB . hslwzoway0 = rtP .
u7_Value_h4l2ea5jkz ; rtB . nuqk0l2i24 = rtP . u8_Value_hbggnsujaw ; rtB .
lfoj330o0w = rtP . u9_Value_iunplaq4at ; rtB . fn5bdzeueb = rtP .
u0_Value_ghzthwwtn2 ; rtB . eluqzbmblx = rtP . u1_Value_pjdjb34lup ; rtB .
hpksxf2u12 = rtP . u2_Value_lcjb3bbgjh ; rtB . cupys00l0h = rtP .
u3_Value_ovtbfd0p13 ; rtB . bfhfhvzjmh = rtP . u4_Value_iguji55pav ; rtB .
lxgrqlgj4c = rtP . u5_Value_gfuar3ecme ; rtB . emrkwl4xdm = rtP .
u6_Value_h5s1uhute3 ; rtB . kumetkpa3r = rtP . u7_Value_jnmsa1lrxd ; rtB .
g5lafvqfmm = rtP . u8_Value_py1b5ib52z ; rtB . by2zsnbwmy = rtP .
u9_Value_b2deyasekn ; rtB . gncgbkiuyu = ( rtP . KZTest_FaultC != 0.0 ) ; rtB
. gqiym5ozq1 = ( rtP . KZTest_FaultB != 0.0 ) ; rtB . cahx5tm0wg = ( rtP .
KZTest_FaultA != 0.0 ) ; rtB . ixn4i5wf1o = ( rtP . KZTest1_FaultC != 0.0 ) ;
rtB . jz0eiv3n4w = ( rtP . KZTest1_FaultB != 0.0 ) ; rtB . mlcvflupnu = ( rtP
. KZTest1_FaultA != 0.0 ) ; UNUSED_PARAMETER ( tid ) ; } void MdlUpdate (
int_T tid ) { real_T HoldSine ; real_T HoldCosine ; if ( ssIsSampleHit ( rtS
, 1 , 0 ) ) { { int_T * gState = ( int_T * ) rtDW . orezesugg1 . G_STATE ; *
( gState ++ ) = ( int_T ) rtB . ipfzduy1yk ; * ( gState ++ ) = ( int_T ) rtB
. juaggbpzrf ; * ( gState ++ ) = ( int_T ) rtB . mjjqenybvd ; } HoldSine =
rtDW . gml0jqyexc ; HoldCosine = rtDW . in1aihe14k ; rtDW . gml0jqyexc =
HoldSine * rtP . SineWaveA_HCos + HoldCosine * rtP . SineWaveA_Hsin ; rtDW .
in1aihe14k = HoldCosine * rtP . SineWaveA_HCos - HoldSine * rtP .
SineWaveA_Hsin ; HoldSine = rtDW . mnbe3gzdef ; HoldCosine = rtDW .
k1ekplz15z ; rtDW . mnbe3gzdef = HoldSine * rtP . SineWaveB_HCos + HoldCosine
* rtP . SineWaveB_Hsin ; rtDW . k1ekplz15z = HoldCosine * rtP .
SineWaveB_HCos - HoldSine * rtP . SineWaveB_Hsin ; HoldSine = rtDW .
apdqz11wuc ; HoldCosine = rtDW . ili5cmy1bf ; rtDW . apdqz11wuc = HoldSine *
rtP . SineWaveC_HCos + HoldCosine * rtP . SineWaveC_Hsin ; rtDW . ili5cmy1bf
= HoldCosine * rtP . SineWaveC_HCos - HoldSine * rtP . SineWaveC_Hsin ;
HoldSine = rtDW . dobgip2gsc ; HoldCosine = rtDW . idflrqbwhk ; rtDW .
dobgip2gsc = HoldSine * rtP . SineWaveA_HCos_pemhijovuq + HoldCosine * rtP .
SineWaveA_Hsin_gtf0muyyo1 ; rtDW . idflrqbwhk = HoldCosine * rtP .
SineWaveA_HCos_pemhijovuq - HoldSine * rtP . SineWaveA_Hsin_gtf0muyyo1 ;
HoldSine = rtDW . krfjbq5jxu ; HoldCosine = rtDW . hrbvhjj0hr ; rtDW .
krfjbq5jxu = HoldSine * rtP . SineWaveB_HCos_o3i2ap52lt + HoldCosine * rtP .
SineWaveB_Hsin_kfkb1n1f0h ; rtDW . hrbvhjj0hr = HoldCosine * rtP .
SineWaveB_HCos_o3i2ap52lt - HoldSine * rtP . SineWaveB_Hsin_kfkb1n1f0h ;
HoldSine = rtDW . fh3tch5sc2 ; HoldCosine = rtDW . hoyxwcln0j ; rtDW .
fh3tch5sc2 = HoldSine * rtP . SineWaveC_HCos_kscsjgvg02 + HoldCosine * rtP .
SineWaveC_Hsin_edp2rwbke5 ; rtDW . hoyxwcln0j = HoldCosine * rtP .
SineWaveC_HCos_kscsjgvg02 - HoldSine * rtP . SineWaveC_Hsin_edp2rwbke5 ; {
const real_T * As = ( real_T * ) rtDW . g45z4iebyu . AS ; const real_T * Bs =
( real_T * ) rtDW . g45z4iebyu . BS ; real_T * xtmp = ( real_T * ) rtDW .
g45z4iebyu . XTMP ; real_T accum ; { int_T i1 ; real_T * xd = & rtDW .
fn3owwkp0o [ 0 ] ; for ( i1 = 0 ; i1 < 6 ; i1 ++ ) { accum = 0.0 ; { int_T i2
; real_T * xd = & rtDW . fn3owwkp0o [ 0 ] ; for ( i2 = 0 ; i2 < 6 ; i2 ++ ) {
accum += * ( As ++ ) * xd [ i2 ] ; } } { int_T i2 ; const real_T * u0 = rtP .
SwitchCurrents_Value_k4xgdjuoaj ; for ( i2 = 0 ; i2 < 6 ; i2 ++ ) { accum +=
* ( Bs ++ ) * u0 [ i2 ] ; } accum += * ( Bs ++ ) * rtB . hjwgkuat0w ; accum
+= * ( Bs ++ ) * rtB . irtkmeqegx ; accum += * ( Bs ++ ) * rtB . idp1uwrp5z ;
accum += * ( Bs ++ ) * rtB . oev0pyxhk5 ; accum += * ( Bs ++ ) * rtB .
i3sk0wialc ; accum += * ( Bs ++ ) * rtB . flmfghomli ; } xtmp [ i1 ] = accum
; } } { int_T i1 ; real_T * xd = & rtDW . fn3owwkp0o [ 0 ] ; for ( i1 = 0 ;
i1 < 6 ; i1 ++ ) { xd [ i1 ] = xtmp [ i1 ] ; } } { int_T * gState = ( int_T *
) rtDW . g45z4iebyu . G_STATE ; * ( gState ++ ) = ( int_T ) rtB . c0mh0lrtzp
; * ( gState ++ ) = ( int_T ) rtB . fbm1wk5xyr ; * ( gState ++ ) = ( int_T )
rtB . l3lfgi5op5 ; * ( gState ++ ) = ( int_T ) rtB . ilmgds3nyj ; * ( gState
++ ) = ( int_T ) rtB . bncuow3blf ; * ( gState ++ ) = ( int_T ) rtB .
brjdxprewh ; } } } { real_T * * uBuffer = ( real_T * * ) & rtDW . grt2oyi0sr
. TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & rtDW .
grt2oyi0sr . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ; rtDW .
ixae4mywgu . Head = ( ( rtDW . ixae4mywgu . Head < ( rtDW . ixae4mywgu .
CircularBufSize - 1 ) ) ? ( rtDW . ixae4mywgu . Head + 1 ) : 0 ) ; if ( rtDW
. ixae4mywgu . Head == rtDW . ixae4mywgu . Tail ) { if ( !
rt_TDelayUpdateTailOrGrowBuf ( & rtDW . ixae4mywgu . CircularBufSize , & rtDW
. ixae4mywgu . Tail , & rtDW . ixae4mywgu . Head , & rtDW . ixae4mywgu . Last
, simTime - rtP . TransportDelay_Delay , tBuffer , uBuffer , ( NULL ) , (
boolean_T ) 0 , false , & rtDW . ixae4mywgu . MaxNewBufSize ) ) {
ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } } (
* tBuffer ) [ rtDW . ixae4mywgu . Head ] = simTime ; ( * uBuffer ) [ rtDW .
ixae4mywgu . Head ] = rtB . oqeo1ubqos ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . bmewgg5lxa = rtB . p5nfnzin0c ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . pl0qrajpw4 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . pl0qrajpw4 . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . ja00u1cqz3 . Head = ( ( rtDW . ja00u1cqz3 . Head < (
rtDW . ja00u1cqz3 . CircularBufSize - 1 ) ) ? ( rtDW . ja00u1cqz3 . Head + 1
) : 0 ) ; if ( rtDW . ja00u1cqz3 . Head == rtDW . ja00u1cqz3 . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . ja00u1cqz3 . CircularBufSize , &
rtDW . ja00u1cqz3 . Tail , & rtDW . ja00u1cqz3 . Head , & rtDW . ja00u1cqz3 .
Last , simTime - rtP . TransportDelay_Delay_oe42moefhj , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . ja00u1cqz3 . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . ja00u1cqz3 . Head ] = simTime ; ( * uBuffer ) [ rtDW .
ja00u1cqz3 . Head ] = rtB . b5pj0x41v0 ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . hztiwvsh4t = rtB . cjtwmfzpsv ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . ax1z1sifrs . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . ax1z1sifrs . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . iepysspqfq . Head = ( ( rtDW . iepysspqfq . Head < (
rtDW . iepysspqfq . CircularBufSize - 1 ) ) ? ( rtDW . iepysspqfq . Head + 1
) : 0 ) ; if ( rtDW . iepysspqfq . Head == rtDW . iepysspqfq . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . iepysspqfq . CircularBufSize , &
rtDW . iepysspqfq . Tail , & rtDW . iepysspqfq . Head , & rtDW . iepysspqfq .
Last , simTime - rtP . TransportDelay_Delay_l554jo5rku , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . iepysspqfq . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . iepysspqfq . Head ] = simTime ; ( * uBuffer ) [ rtDW .
iepysspqfq . Head ] = rtB . lal3cjzlj5 ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . a4bul5sdk4 = rtB . aq1qiraf44 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . d5b3ghqdjb . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . d5b3ghqdjb . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . d5g4hqdsxq . Head = ( ( rtDW . d5g4hqdsxq . Head < (
rtDW . d5g4hqdsxq . CircularBufSize - 1 ) ) ? ( rtDW . d5g4hqdsxq . Head + 1
) : 0 ) ; if ( rtDW . d5g4hqdsxq . Head == rtDW . d5g4hqdsxq . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . d5g4hqdsxq . CircularBufSize , &
rtDW . d5g4hqdsxq . Tail , & rtDW . d5g4hqdsxq . Head , & rtDW . d5g4hqdsxq .
Last , simTime - rtP . TransportDelay_Delay_bpfmvxai5z , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . d5g4hqdsxq . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . d5g4hqdsxq . Head ] = simTime ; ( * uBuffer ) [ rtDW .
d5g4hqdsxq . Head ] = rtB . bhzqnkifhr ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . ogsgimkanv = rtB . gosgmmpf3q ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . d45fc0cu2n . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . d45fc0cu2n . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . b2xp1t1hmq . Head = ( ( rtDW . b2xp1t1hmq . Head < (
rtDW . b2xp1t1hmq . CircularBufSize - 1 ) ) ? ( rtDW . b2xp1t1hmq . Head + 1
) : 0 ) ; if ( rtDW . b2xp1t1hmq . Head == rtDW . b2xp1t1hmq . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . b2xp1t1hmq . CircularBufSize , &
rtDW . b2xp1t1hmq . Tail , & rtDW . b2xp1t1hmq . Head , & rtDW . b2xp1t1hmq .
Last , simTime - rtP . TransportDelay_Delay_njnzvljj3k , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . b2xp1t1hmq . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . b2xp1t1hmq . Head ] = simTime ; ( * uBuffer ) [ rtDW .
b2xp1t1hmq . Head ] = rtB . ffcf4214zj ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . jb0wxz0ail = rtB . cbmftdx00h ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . c5rfy4mzmq . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . c5rfy4mzmq . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . ewf1j0idml . Head = ( ( rtDW . ewf1j0idml . Head < (
rtDW . ewf1j0idml . CircularBufSize - 1 ) ) ? ( rtDW . ewf1j0idml . Head + 1
) : 0 ) ; if ( rtDW . ewf1j0idml . Head == rtDW . ewf1j0idml . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . ewf1j0idml . CircularBufSize , &
rtDW . ewf1j0idml . Tail , & rtDW . ewf1j0idml . Head , & rtDW . ewf1j0idml .
Last , simTime - rtP . TransportDelay_Delay_jb45sdc2uw , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . ewf1j0idml . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . ewf1j0idml . Head ] = simTime ; ( * uBuffer ) [ rtDW .
ewf1j0idml . Head ] = rtB . kocoejgchf ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . ddy2duwo4c = rtB . dxwcs33v2q ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . lmfxi4smjj . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . lmfxi4smjj . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . if32tdg5qk . Head = ( ( rtDW . if32tdg5qk . Head < (
rtDW . if32tdg5qk . CircularBufSize - 1 ) ) ? ( rtDW . if32tdg5qk . Head + 1
) : 0 ) ; if ( rtDW . if32tdg5qk . Head == rtDW . if32tdg5qk . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . if32tdg5qk . CircularBufSize , &
rtDW . if32tdg5qk . Tail , & rtDW . if32tdg5qk . Head , & rtDW . if32tdg5qk .
Last , simTime - rtP . TransportDelay_Delay_o3j3xceore , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . if32tdg5qk . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . if32tdg5qk . Head ] = simTime ; ( * uBuffer ) [ rtDW .
if32tdg5qk . Head ] = rtB . cavjbaogp0 ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . jhulgqvdvl = rtB . d41xayzc2a ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . d3lf52anqx . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . d3lf52anqx . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . jedhnolofy . Head = ( ( rtDW . jedhnolofy . Head < (
rtDW . jedhnolofy . CircularBufSize - 1 ) ) ? ( rtDW . jedhnolofy . Head + 1
) : 0 ) ; if ( rtDW . jedhnolofy . Head == rtDW . jedhnolofy . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . jedhnolofy . CircularBufSize , &
rtDW . jedhnolofy . Tail , & rtDW . jedhnolofy . Head , & rtDW . jedhnolofy .
Last , simTime - rtP . TransportDelay_Delay_fanj5ft5et , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . jedhnolofy . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . jedhnolofy . Head ] = simTime ; ( * uBuffer ) [ rtDW .
jedhnolofy . Head ] = rtB . gmm1i11e3j ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . lq5puhde41 = rtB . b54kkc2sai ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . gvogk5wosq . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . gvogk5wosq . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . a2f4jln130 . Head = ( ( rtDW . a2f4jln130 . Head < (
rtDW . a2f4jln130 . CircularBufSize - 1 ) ) ? ( rtDW . a2f4jln130 . Head + 1
) : 0 ) ; if ( rtDW . a2f4jln130 . Head == rtDW . a2f4jln130 . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . a2f4jln130 . CircularBufSize , &
rtDW . a2f4jln130 . Tail , & rtDW . a2f4jln130 . Head , & rtDW . a2f4jln130 .
Last , simTime - rtP . TransportDelay_Delay_gr3febuaiv , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . a2f4jln130 . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . a2f4jln130 . Head ] = simTime ; ( * uBuffer ) [ rtDW .
a2f4jln130 . Head ] = rtB . kyr4zi2dy0 ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . d1hkn1pn3x = rtB . m4l4wj2mnm ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . innrerrg2w . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . innrerrg2w . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . h43kjr3444 . Head = ( ( rtDW . h43kjr3444 . Head < (
rtDW . h43kjr3444 . CircularBufSize - 1 ) ) ? ( rtDW . h43kjr3444 . Head + 1
) : 0 ) ; if ( rtDW . h43kjr3444 . Head == rtDW . h43kjr3444 . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . h43kjr3444 . CircularBufSize , &
rtDW . h43kjr3444 . Tail , & rtDW . h43kjr3444 . Head , & rtDW . h43kjr3444 .
Last , simTime - rtP . TransportDelay_Delay_m5tuijz25b , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . h43kjr3444 . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . h43kjr3444 . Head ] = simTime ; ( * uBuffer ) [ rtDW .
h43kjr3444 . Head ] = rtB . hbfto5kcre ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . kidkiatkb2 = rtB . kwecicm223 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . b3tfxdcqfh . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . b3tfxdcqfh . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . gudqpjffwb . Head = ( ( rtDW . gudqpjffwb . Head < (
rtDW . gudqpjffwb . CircularBufSize - 1 ) ) ? ( rtDW . gudqpjffwb . Head + 1
) : 0 ) ; if ( rtDW . gudqpjffwb . Head == rtDW . gudqpjffwb . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . gudqpjffwb . CircularBufSize , &
rtDW . gudqpjffwb . Tail , & rtDW . gudqpjffwb . Head , & rtDW . gudqpjffwb .
Last , simTime - rtP . TransportDelay_Delay_kfs5cq2cxd , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . gudqpjffwb . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . gudqpjffwb . Head ] = simTime ; ( * uBuffer ) [ rtDW .
gudqpjffwb . Head ] = rtB . odsjx3ak1l ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . oqqwr15jx2 = rtB . l50se1h1ym ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . ex4axikx2n . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . ex4axikx2n . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . ipe5rnt2je . Head = ( ( rtDW . ipe5rnt2je . Head < (
rtDW . ipe5rnt2je . CircularBufSize - 1 ) ) ? ( rtDW . ipe5rnt2je . Head + 1
) : 0 ) ; if ( rtDW . ipe5rnt2je . Head == rtDW . ipe5rnt2je . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . ipe5rnt2je . CircularBufSize , &
rtDW . ipe5rnt2je . Tail , & rtDW . ipe5rnt2je . Head , & rtDW . ipe5rnt2je .
Last , simTime - rtP . TransportDelay_Delay_mf3buvibkv , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . ipe5rnt2je . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . ipe5rnt2je . Head ] = simTime ; ( * uBuffer ) [ rtDW .
ipe5rnt2je . Head ] = rtB . bwlritkk2v ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . dgche5odby = rtB . knvfyibp1b ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . f5mcje12pk . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . f5mcje12pk . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . deyhny3sye . Head = ( ( rtDW . deyhny3sye . Head < (
rtDW . deyhny3sye . CircularBufSize - 1 ) ) ? ( rtDW . deyhny3sye . Head + 1
) : 0 ) ; if ( rtDW . deyhny3sye . Head == rtDW . deyhny3sye . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . deyhny3sye . CircularBufSize , &
rtDW . deyhny3sye . Tail , & rtDW . deyhny3sye . Head , & rtDW . deyhny3sye .
Last , simTime - rtP . TransportDelay_Delay_mpt4otaavv , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . deyhny3sye . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . deyhny3sye . Head ] = simTime ; ( * uBuffer ) [ rtDW .
deyhny3sye . Head ] = rtB . fm5wjqawts ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . olgfgh4cjq = rtB . kbtjvwenzh ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . oi1yc2ajy5 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . oi1yc2ajy5 . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . abila4k5tf . Head = ( ( rtDW . abila4k5tf . Head < (
rtDW . abila4k5tf . CircularBufSize - 1 ) ) ? ( rtDW . abila4k5tf . Head + 1
) : 0 ) ; if ( rtDW . abila4k5tf . Head == rtDW . abila4k5tf . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . abila4k5tf . CircularBufSize , &
rtDW . abila4k5tf . Tail , & rtDW . abila4k5tf . Head , & rtDW . abila4k5tf .
Last , simTime - rtP . TransportDelay_Delay_l2cxshyix2 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . abila4k5tf . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . abila4k5tf . Head ] = simTime ; ( * uBuffer ) [ rtDW .
abila4k5tf . Head ] = rtB . h2vg1mucis ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . mokcdmtlad = rtB . j5zcghl4k1 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . c2kuyduabk . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . c2kuyduabk . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . iwj0qe5gyd . Head = ( ( rtDW . iwj0qe5gyd . Head < (
rtDW . iwj0qe5gyd . CircularBufSize - 1 ) ) ? ( rtDW . iwj0qe5gyd . Head + 1
) : 0 ) ; if ( rtDW . iwj0qe5gyd . Head == rtDW . iwj0qe5gyd . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . iwj0qe5gyd . CircularBufSize , &
rtDW . iwj0qe5gyd . Tail , & rtDW . iwj0qe5gyd . Head , & rtDW . iwj0qe5gyd .
Last , simTime - rtP . TransportDelay_Delay_apxflo13lg , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . iwj0qe5gyd . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . iwj0qe5gyd . Head ] = simTime ; ( * uBuffer ) [ rtDW .
iwj0qe5gyd . Head ] = rtB . iir3yfosnh ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . iosgfiuyq5 = rtB . kt12go5wl2 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . exexriqgpr . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . exexriqgpr . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . fupmlcibf5 . Head = ( ( rtDW . fupmlcibf5 . Head < (
rtDW . fupmlcibf5 . CircularBufSize - 1 ) ) ? ( rtDW . fupmlcibf5 . Head + 1
) : 0 ) ; if ( rtDW . fupmlcibf5 . Head == rtDW . fupmlcibf5 . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . fupmlcibf5 . CircularBufSize , &
rtDW . fupmlcibf5 . Tail , & rtDW . fupmlcibf5 . Head , & rtDW . fupmlcibf5 .
Last , simTime - rtP . TransportDelay_Delay_bnnallqkhz , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . fupmlcibf5 . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . fupmlcibf5 . Head ] = simTime ; ( * uBuffer ) [ rtDW .
fupmlcibf5 . Head ] = rtB . bel33c2xhe ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . e5wjbjcaxs = rtB . pzcfkojr0n ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . k0yj2b2ldz . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . k0yj2b2ldz . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . iqlusmvwiw . Head = ( ( rtDW . iqlusmvwiw . Head < (
rtDW . iqlusmvwiw . CircularBufSize - 1 ) ) ? ( rtDW . iqlusmvwiw . Head + 1
) : 0 ) ; if ( rtDW . iqlusmvwiw . Head == rtDW . iqlusmvwiw . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . iqlusmvwiw . CircularBufSize , &
rtDW . iqlusmvwiw . Tail , & rtDW . iqlusmvwiw . Head , & rtDW . iqlusmvwiw .
Last , simTime - rtP . TransportDelay_Delay_df0az4x0nr , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . iqlusmvwiw . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . iqlusmvwiw . Head ] = simTime ; ( * uBuffer ) [ rtDW .
iqlusmvwiw . Head ] = rtB . nwi4ojgvax ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . g2sm2t4hw5 = rtB . ehvyl2mad3 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . lbtex4xafe . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . lbtex4xafe . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . gbcuvxshkr . Head = ( ( rtDW . gbcuvxshkr . Head < (
rtDW . gbcuvxshkr . CircularBufSize - 1 ) ) ? ( rtDW . gbcuvxshkr . Head + 1
) : 0 ) ; if ( rtDW . gbcuvxshkr . Head == rtDW . gbcuvxshkr . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . gbcuvxshkr . CircularBufSize , &
rtDW . gbcuvxshkr . Tail , & rtDW . gbcuvxshkr . Head , & rtDW . gbcuvxshkr .
Last , simTime - rtP . TransportDelay_Delay_hd1c0nxozi , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . gbcuvxshkr . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . gbcuvxshkr . Head ] = simTime ; ( * uBuffer ) [ rtDW .
gbcuvxshkr . Head ] = rtB . mush31qdje ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . lva0bwt5gb = rtB . lwfyukagvg ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . dtlmsjjjpu . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . dtlmsjjjpu . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . ov4twhz4dl . Head = ( ( rtDW . ov4twhz4dl . Head < (
rtDW . ov4twhz4dl . CircularBufSize - 1 ) ) ? ( rtDW . ov4twhz4dl . Head + 1
) : 0 ) ; if ( rtDW . ov4twhz4dl . Head == rtDW . ov4twhz4dl . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . ov4twhz4dl . CircularBufSize , &
rtDW . ov4twhz4dl . Tail , & rtDW . ov4twhz4dl . Head , & rtDW . ov4twhz4dl .
Last , simTime - rtP . TransportDelay_Delay_pswpnmajkv , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . ov4twhz4dl . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . ov4twhz4dl . Head ] = simTime ; ( * uBuffer ) [ rtDW .
ov4twhz4dl . Head ] = rtB . lhwa0vyjtr ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . noyx1erxkm = rtB . bdlpjubuoi ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . l30jrhdejr . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . l30jrhdejr . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . iqunqpo4a3 . Head = ( ( rtDW . iqunqpo4a3 . Head < (
rtDW . iqunqpo4a3 . CircularBufSize - 1 ) ) ? ( rtDW . iqunqpo4a3 . Head + 1
) : 0 ) ; if ( rtDW . iqunqpo4a3 . Head == rtDW . iqunqpo4a3 . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . iqunqpo4a3 . CircularBufSize , &
rtDW . iqunqpo4a3 . Tail , & rtDW . iqunqpo4a3 . Head , & rtDW . iqunqpo4a3 .
Last , simTime - rtP . TransportDelay_Delay_ofwfa3phpb , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . iqunqpo4a3 . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . iqunqpo4a3 . Head ] = simTime ; ( * uBuffer ) [ rtDW .
iqunqpo4a3 . Head ] = rtB . b2mrhddfvq ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . jsqsb2a31o = rtB . bafm53nc12 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . hh24ilp1p1 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . hh24ilp1p1 . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . gxzqgoeivi . Head = ( ( rtDW . gxzqgoeivi . Head < (
rtDW . gxzqgoeivi . CircularBufSize - 1 ) ) ? ( rtDW . gxzqgoeivi . Head + 1
) : 0 ) ; if ( rtDW . gxzqgoeivi . Head == rtDW . gxzqgoeivi . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . gxzqgoeivi . CircularBufSize , &
rtDW . gxzqgoeivi . Tail , & rtDW . gxzqgoeivi . Head , & rtDW . gxzqgoeivi .
Last , simTime - rtP . TransportDelay_Delay_bbzq04uqkz , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . gxzqgoeivi . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . gxzqgoeivi . Head ] = simTime ; ( * uBuffer ) [ rtDW .
gxzqgoeivi . Head ] = rtB . l5rvgavbvg ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . krkllgqfro = rtB . ppoxem35uq ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . djthquopua . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . djthquopua . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . obmianunwz . Head = ( ( rtDW . obmianunwz . Head < (
rtDW . obmianunwz . CircularBufSize - 1 ) ) ? ( rtDW . obmianunwz . Head + 1
) : 0 ) ; if ( rtDW . obmianunwz . Head == rtDW . obmianunwz . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . obmianunwz . CircularBufSize , &
rtDW . obmianunwz . Tail , & rtDW . obmianunwz . Head , & rtDW . obmianunwz .
Last , simTime - rtP . TransportDelay_Delay_foyaftkemu , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . obmianunwz . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . obmianunwz . Head ] = simTime ; ( * uBuffer ) [ rtDW .
obmianunwz . Head ] = rtB . eit2f5ruxh ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . bjpjcjhpzj = rtB . dhlslzrf2v ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . faad0u1b4n . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . faad0u1b4n . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . nf5je2gw4m . Head = ( ( rtDW . nf5je2gw4m . Head < (
rtDW . nf5je2gw4m . CircularBufSize - 1 ) ) ? ( rtDW . nf5je2gw4m . Head + 1
) : 0 ) ; if ( rtDW . nf5je2gw4m . Head == rtDW . nf5je2gw4m . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . nf5je2gw4m . CircularBufSize , &
rtDW . nf5je2gw4m . Tail , & rtDW . nf5je2gw4m . Head , & rtDW . nf5je2gw4m .
Last , simTime - rtP . TransportDelay_Delay_miyezd2kg1 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . nf5je2gw4m . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . nf5je2gw4m . Head ] = simTime ; ( * uBuffer ) [ rtDW .
nf5je2gw4m . Head ] = rtB . gmehm5w3yl ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . a5gjxntn1h = rtB . lcsqf4izo5 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . k4bhgqdyhg . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . k4bhgqdyhg . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . asmpxtvf2z . Head = ( ( rtDW . asmpxtvf2z . Head < (
rtDW . asmpxtvf2z . CircularBufSize - 1 ) ) ? ( rtDW . asmpxtvf2z . Head + 1
) : 0 ) ; if ( rtDW . asmpxtvf2z . Head == rtDW . asmpxtvf2z . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . asmpxtvf2z . CircularBufSize , &
rtDW . asmpxtvf2z . Tail , & rtDW . asmpxtvf2z . Head , & rtDW . asmpxtvf2z .
Last , simTime - rtP . TransportDelay_Delay_lge1txljnw , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . asmpxtvf2z . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . asmpxtvf2z . Head ] = simTime ; ( * uBuffer ) [ rtDW .
asmpxtvf2z . Head ] = rtB . obvcacwue5 ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . i3uldok0qn = rtB . a4ozttu2p4 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . kx2dm0gq0l . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . kx2dm0gq0l . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . prqk4dqmol . Head = ( ( rtDW . prqk4dqmol . Head < (
rtDW . prqk4dqmol . CircularBufSize - 1 ) ) ? ( rtDW . prqk4dqmol . Head + 1
) : 0 ) ; if ( rtDW . prqk4dqmol . Head == rtDW . prqk4dqmol . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . prqk4dqmol . CircularBufSize , &
rtDW . prqk4dqmol . Tail , & rtDW . prqk4dqmol . Head , & rtDW . prqk4dqmol .
Last , simTime - rtP . TransportDelay_Delay_d5wczemc11 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . prqk4dqmol . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . prqk4dqmol . Head ] = simTime ; ( * uBuffer ) [ rtDW .
prqk4dqmol . Head ] = rtB . mba3xfzjhz ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . a1ejgtlf3z = rtB . lap3zr2ns3 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . milmx1vwl5 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . milmx1vwl5 . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . gl5lw50op2 . Head = ( ( rtDW . gl5lw50op2 . Head < (
rtDW . gl5lw50op2 . CircularBufSize - 1 ) ) ? ( rtDW . gl5lw50op2 . Head + 1
) : 0 ) ; if ( rtDW . gl5lw50op2 . Head == rtDW . gl5lw50op2 . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . gl5lw50op2 . CircularBufSize , &
rtDW . gl5lw50op2 . Tail , & rtDW . gl5lw50op2 . Head , & rtDW . gl5lw50op2 .
Last , simTime - rtP . TransportDelay_Delay_j4fbooars3 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . gl5lw50op2 . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . gl5lw50op2 . Head ] = simTime ; ( * uBuffer ) [ rtDW .
gl5lw50op2 . Head ] = rtB . pujm3gf3vm ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . ihf24wpydd = rtB . ii4fe1aanj ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . e1qtan4tfe . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . e1qtan4tfe . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . ju1jo0gbez . Head = ( ( rtDW . ju1jo0gbez . Head < (
rtDW . ju1jo0gbez . CircularBufSize - 1 ) ) ? ( rtDW . ju1jo0gbez . Head + 1
) : 0 ) ; if ( rtDW . ju1jo0gbez . Head == rtDW . ju1jo0gbez . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . ju1jo0gbez . CircularBufSize , &
rtDW . ju1jo0gbez . Tail , & rtDW . ju1jo0gbez . Head , & rtDW . ju1jo0gbez .
Last , simTime - rtP . TransportDelay_Delay_kiqasqcb54 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . ju1jo0gbez . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . ju1jo0gbez . Head ] = simTime ; ( * uBuffer ) [ rtDW .
ju1jo0gbez . Head ] = rtB . d2r2jobgwj ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . jykznszm3q = rtB . atxnqorjb4 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . nsvuvsgsn0 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . nsvuvsgsn0 . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . ihorz5zaun . Head = ( ( rtDW . ihorz5zaun . Head < (
rtDW . ihorz5zaun . CircularBufSize - 1 ) ) ? ( rtDW . ihorz5zaun . Head + 1
) : 0 ) ; if ( rtDW . ihorz5zaun . Head == rtDW . ihorz5zaun . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . ihorz5zaun . CircularBufSize , &
rtDW . ihorz5zaun . Tail , & rtDW . ihorz5zaun . Head , & rtDW . ihorz5zaun .
Last , simTime - rtP . TransportDelay_Delay_hdijrdpglg , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . ihorz5zaun . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . ihorz5zaun . Head ] = simTime ; ( * uBuffer ) [ rtDW .
ihorz5zaun . Head ] = rtB . mdgp1q5akr ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . kwauldso4g = rtB . nyw0btgear ; rtDW . bw41vj1a5t = rtB .
kl4albs201 ; } lwcs3jh3k2 ( rtS , & rtB . f2cfptydf3 , & rtDW . f2cfptydf3 )
; ny353qchui ( rtS , & rtB . mempnwchre , & rtDW . mempnwchre ) ; lwcs3jh3k2
( rtS , & rtB . ceydu0zdw3 , & rtDW . ceydu0zdw3 ) ; ny353qchui ( rtS , & rtB
. e05ysccjt5 , & rtDW . e05ysccjt5 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) {
rtDW . cfe2132dnu = rtB . ff4zi1sk4i ; } lwcs3jh3k2 ( rtS , & rtB .
kbnljt2vok , & rtDW . kbnljt2vok ) ; ny353qchui ( rtS , & rtB . ffo1fpyqgq ,
& rtDW . ffo1fpyqgq ) ; lwcs3jh3k2 ( rtS , & rtB . nrt14kntrk , & rtDW .
nrt14kntrk ) ; ny353qchui ( rtS , & rtB . nepgqug5c0 , & rtDW . nepgqug5c0 )
; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW . gvkoorm242 = rtB . j2jawg3ipw
; rtDW . lxxzsohnb1 = rtB . me5qv15ino ; rtDW . duld1iija4 = rtB . mqzh4us2pg
; rtDW . j5clkw2ofw = rtB . i4bdlr2sjh ; rtDW . ht5aj3noet = rtB . et0kfo00yx
; rtDW . b54hv4iwx0 = rtB . csmwalb3wx ; rtDW . geqzl3jlyf = rtB . iraovkag4l
; } lwcs3jh3k2 ( rtS , & rtB . inadvhzvdh , & rtDW . inadvhzvdh ) ;
ny353qchui ( rtS , & rtB . leagxggwwe , & rtDW . leagxggwwe ) ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW . bjzszecxxc = rtB . iraovkag4l ; rtDW
. l2ogv2jkyi = rtB . l3rpmtffkw ; rtDW . ike0qfti5p = rtB . k4djtoqsoe ; rtDW
. m1mkpcefd4 = rtB . g5smsjevum ; } { real_T * * uBuffer = ( real_T * * ) &
rtDW . e2vylf2hfx . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * )
& rtDW . e2vylf2hfx . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
rtDW . msx5naejpe . Head = ( ( rtDW . msx5naejpe . Head < ( rtDW . msx5naejpe
. CircularBufSize - 1 ) ) ? ( rtDW . msx5naejpe . Head + 1 ) : 0 ) ; if (
rtDW . msx5naejpe . Head == rtDW . msx5naejpe . Tail ) { if ( !
rt_TDelayUpdateTailOrGrowBuf ( & rtDW . msx5naejpe . CircularBufSize , & rtDW
. msx5naejpe . Tail , & rtDW . msx5naejpe . Head , & rtDW . msx5naejpe . Last
, simTime - rtP . TransportDelay_Delay_fcqqd2zozi , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & rtDW . msx5naejpe . MaxNewBufSize ) ) {
ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } } (
* tBuffer ) [ rtDW . msx5naejpe . Head ] = simTime ; ( * uBuffer ) [ rtDW .
msx5naejpe . Head ] = rtB . cvd4e0evi0 ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . j0fgvrdpz3 = rtB . dy5mue12t2 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . bflyns0cbr . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . bflyns0cbr . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . n00kmgw5wb . Head = ( ( rtDW . n00kmgw5wb . Head < (
rtDW . n00kmgw5wb . CircularBufSize - 1 ) ) ? ( rtDW . n00kmgw5wb . Head + 1
) : 0 ) ; if ( rtDW . n00kmgw5wb . Head == rtDW . n00kmgw5wb . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . n00kmgw5wb . CircularBufSize , &
rtDW . n00kmgw5wb . Tail , & rtDW . n00kmgw5wb . Head , & rtDW . n00kmgw5wb .
Last , simTime - rtP . TransportDelay_Delay_enbrjjjv0c , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . n00kmgw5wb . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . n00kmgw5wb . Head ] = simTime ; ( * uBuffer ) [ rtDW .
n00kmgw5wb . Head ] = rtB . k5ieikefmd ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . cvvtotyqir = rtB . gmlyxhb3tx ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . pxt2zbirdg . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . pxt2zbirdg . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . pqxdtxr3k0 . Head = ( ( rtDW . pqxdtxr3k0 . Head < (
rtDW . pqxdtxr3k0 . CircularBufSize - 1 ) ) ? ( rtDW . pqxdtxr3k0 . Head + 1
) : 0 ) ; if ( rtDW . pqxdtxr3k0 . Head == rtDW . pqxdtxr3k0 . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . pqxdtxr3k0 . CircularBufSize , &
rtDW . pqxdtxr3k0 . Tail , & rtDW . pqxdtxr3k0 . Head , & rtDW . pqxdtxr3k0 .
Last , simTime - rtP . TransportDelay_Delay_d2e14ai5vg , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . pqxdtxr3k0 . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . pqxdtxr3k0 . Head ] = simTime ; ( * uBuffer ) [ rtDW .
pqxdtxr3k0 . Head ] = rtB . egeez0g01i ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . bbxfzyjpg4 = rtB . mhjihj0vyw ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . fllltsfpxy . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . fllltsfpxy . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . o2cxt5uwbv . Head = ( ( rtDW . o2cxt5uwbv . Head < (
rtDW . o2cxt5uwbv . CircularBufSize - 1 ) ) ? ( rtDW . o2cxt5uwbv . Head + 1
) : 0 ) ; if ( rtDW . o2cxt5uwbv . Head == rtDW . o2cxt5uwbv . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . o2cxt5uwbv . CircularBufSize , &
rtDW . o2cxt5uwbv . Tail , & rtDW . o2cxt5uwbv . Head , & rtDW . o2cxt5uwbv .
Last , simTime - rtP . TransportDelay_Delay_o4vgxrlxi1 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . o2cxt5uwbv . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . o2cxt5uwbv . Head ] = simTime ; ( * uBuffer ) [ rtDW .
o2cxt5uwbv . Head ] = rtB . itzq53orwz ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . fuwywwq3yv = rtB . chlpl14sjv ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . afb2vzbxaa . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . afb2vzbxaa . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . owq1uvux0e . Head = ( ( rtDW . owq1uvux0e . Head < (
rtDW . owq1uvux0e . CircularBufSize - 1 ) ) ? ( rtDW . owq1uvux0e . Head + 1
) : 0 ) ; if ( rtDW . owq1uvux0e . Head == rtDW . owq1uvux0e . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . owq1uvux0e . CircularBufSize , &
rtDW . owq1uvux0e . Tail , & rtDW . owq1uvux0e . Head , & rtDW . owq1uvux0e .
Last , simTime - rtP . TransportDelay_Delay_ozzodunntx , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . owq1uvux0e . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . owq1uvux0e . Head ] = simTime ; ( * uBuffer ) [ rtDW .
owq1uvux0e . Head ] = rtB . k1gjsemfzf ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . gd2gr2srvr = rtB . bt0tzj3emo ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . fz41nona4h . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . fz41nona4h . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . ffi24542he . Head = ( ( rtDW . ffi24542he . Head < (
rtDW . ffi24542he . CircularBufSize - 1 ) ) ? ( rtDW . ffi24542he . Head + 1
) : 0 ) ; if ( rtDW . ffi24542he . Head == rtDW . ffi24542he . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . ffi24542he . CircularBufSize , &
rtDW . ffi24542he . Tail , & rtDW . ffi24542he . Head , & rtDW . ffi24542he .
Last , simTime - rtP . TransportDelay_Delay_ar4dnw40cr , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . ffi24542he . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . ffi24542he . Head ] = simTime ; ( * uBuffer ) [ rtDW .
ffi24542he . Head ] = rtB . lifmwxrumg ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . mumecnxvzw = rtB . ariz3p4kkt ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . hqvoz0gebt . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . hqvoz0gebt . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . kw0xkp24lw . Head = ( ( rtDW . kw0xkp24lw . Head < (
rtDW . kw0xkp24lw . CircularBufSize - 1 ) ) ? ( rtDW . kw0xkp24lw . Head + 1
) : 0 ) ; if ( rtDW . kw0xkp24lw . Head == rtDW . kw0xkp24lw . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . kw0xkp24lw . CircularBufSize , &
rtDW . kw0xkp24lw . Tail , & rtDW . kw0xkp24lw . Head , & rtDW . kw0xkp24lw .
Last , simTime - rtP . TransportDelay_Delay_aygjcekf2q , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . kw0xkp24lw . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . kw0xkp24lw . Head ] = simTime ; ( * uBuffer ) [ rtDW .
kw0xkp24lw . Head ] = rtB . lz4xhg4hle ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . md3bj5wruc = rtB . ex1bxygkpd ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . pvl2ejzk4c . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . pvl2ejzk4c . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . e54w33lz5m . Head = ( ( rtDW . e54w33lz5m . Head < (
rtDW . e54w33lz5m . CircularBufSize - 1 ) ) ? ( rtDW . e54w33lz5m . Head + 1
) : 0 ) ; if ( rtDW . e54w33lz5m . Head == rtDW . e54w33lz5m . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . e54w33lz5m . CircularBufSize , &
rtDW . e54w33lz5m . Tail , & rtDW . e54w33lz5m . Head , & rtDW . e54w33lz5m .
Last , simTime - rtP . TransportDelay_Delay_lxikka2j32 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . e54w33lz5m . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . e54w33lz5m . Head ] = simTime ; ( * uBuffer ) [ rtDW .
e54w33lz5m . Head ] = rtB . ekue3tpdoq ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . hqezuuwqi2 = rtB . my3feqkfg0 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . pzbmlqlvpm . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . pzbmlqlvpm . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . m51gowaflk . Head = ( ( rtDW . m51gowaflk . Head < (
rtDW . m51gowaflk . CircularBufSize - 1 ) ) ? ( rtDW . m51gowaflk . Head + 1
) : 0 ) ; if ( rtDW . m51gowaflk . Head == rtDW . m51gowaflk . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . m51gowaflk . CircularBufSize , &
rtDW . m51gowaflk . Tail , & rtDW . m51gowaflk . Head , & rtDW . m51gowaflk .
Last , simTime - rtP . TransportDelay_Delay_ojsuzuucox , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . m51gowaflk . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . m51gowaflk . Head ] = simTime ; ( * uBuffer ) [ rtDW .
m51gowaflk . Head ] = rtB . lrgb5yt5x3 ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . memzqzyaek = rtB . dfgjzm3gbq ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . h0w4jpcxu0 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . h0w4jpcxu0 . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . oihdsug2zq . Head = ( ( rtDW . oihdsug2zq . Head < (
rtDW . oihdsug2zq . CircularBufSize - 1 ) ) ? ( rtDW . oihdsug2zq . Head + 1
) : 0 ) ; if ( rtDW . oihdsug2zq . Head == rtDW . oihdsug2zq . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . oihdsug2zq . CircularBufSize , &
rtDW . oihdsug2zq . Tail , & rtDW . oihdsug2zq . Head , & rtDW . oihdsug2zq .
Last , simTime - rtP . TransportDelay_Delay_mrs4g4hz04 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . oihdsug2zq . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . oihdsug2zq . Head ] = simTime ; ( * uBuffer ) [ rtDW .
oihdsug2zq . Head ] = rtB . po2qirhw4b ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . ffusyfyneb = rtB . fkpvxyu5uz ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . bdtaclh4hr . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . bdtaclh4hr . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . paswm1kxsx . Head = ( ( rtDW . paswm1kxsx . Head < (
rtDW . paswm1kxsx . CircularBufSize - 1 ) ) ? ( rtDW . paswm1kxsx . Head + 1
) : 0 ) ; if ( rtDW . paswm1kxsx . Head == rtDW . paswm1kxsx . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . paswm1kxsx . CircularBufSize , &
rtDW . paswm1kxsx . Tail , & rtDW . paswm1kxsx . Head , & rtDW . paswm1kxsx .
Last , simTime - rtP . TransportDelay_Delay_m3owha4xxi , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . paswm1kxsx . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . paswm1kxsx . Head ] = simTime ; ( * uBuffer ) [ rtDW .
paswm1kxsx . Head ] = rtB . jnhluwefk5 ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . bd4vt3ybt5 = rtB . nz5vgslpu5 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . hwwk0yws23 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . hwwk0yws23 . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . i5hhavred2 . Head = ( ( rtDW . i5hhavred2 . Head < (
rtDW . i5hhavred2 . CircularBufSize - 1 ) ) ? ( rtDW . i5hhavred2 . Head + 1
) : 0 ) ; if ( rtDW . i5hhavred2 . Head == rtDW . i5hhavred2 . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . i5hhavred2 . CircularBufSize , &
rtDW . i5hhavred2 . Tail , & rtDW . i5hhavred2 . Head , & rtDW . i5hhavred2 .
Last , simTime - rtP . TransportDelay_Delay_k5pna43yah , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . i5hhavred2 . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . i5hhavred2 . Head ] = simTime ; ( * uBuffer ) [ rtDW .
i5hhavred2 . Head ] = rtB . k54rta2rmh ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . kmjfdch5hk = rtB . gn2b2kw2ve ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . eosdk1pxwr . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . eosdk1pxwr . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . ph1zmaxjn3 . Head = ( ( rtDW . ph1zmaxjn3 . Head < (
rtDW . ph1zmaxjn3 . CircularBufSize - 1 ) ) ? ( rtDW . ph1zmaxjn3 . Head + 1
) : 0 ) ; if ( rtDW . ph1zmaxjn3 . Head == rtDW . ph1zmaxjn3 . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . ph1zmaxjn3 . CircularBufSize , &
rtDW . ph1zmaxjn3 . Tail , & rtDW . ph1zmaxjn3 . Head , & rtDW . ph1zmaxjn3 .
Last , simTime - rtP . TransportDelay_Delay_gf2tvtym3j , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . ph1zmaxjn3 . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . ph1zmaxjn3 . Head ] = simTime ; ( * uBuffer ) [ rtDW .
ph1zmaxjn3 . Head ] = rtB . boo1zm2mc5 ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . oxztpbydsz = rtB . oq0qf2bjai ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . hdva23qpbu . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . hdva23qpbu . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . azybsjku3n . Head = ( ( rtDW . azybsjku3n . Head < (
rtDW . azybsjku3n . CircularBufSize - 1 ) ) ? ( rtDW . azybsjku3n . Head + 1
) : 0 ) ; if ( rtDW . azybsjku3n . Head == rtDW . azybsjku3n . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . azybsjku3n . CircularBufSize , &
rtDW . azybsjku3n . Tail , & rtDW . azybsjku3n . Head , & rtDW . azybsjku3n .
Last , simTime - rtP . TransportDelay_Delay_ahfgo2nfe0 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . azybsjku3n . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . azybsjku3n . Head ] = simTime ; ( * uBuffer ) [ rtDW .
azybsjku3n . Head ] = rtB . kyxt0ot0tk ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . ol40kfwb3x = rtB . gj01fqf3oc ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . kieb4vqgp3 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . kieb4vqgp3 . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . jzrzzypfkx . Head = ( ( rtDW . jzrzzypfkx . Head < (
rtDW . jzrzzypfkx . CircularBufSize - 1 ) ) ? ( rtDW . jzrzzypfkx . Head + 1
) : 0 ) ; if ( rtDW . jzrzzypfkx . Head == rtDW . jzrzzypfkx . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . jzrzzypfkx . CircularBufSize , &
rtDW . jzrzzypfkx . Tail , & rtDW . jzrzzypfkx . Head , & rtDW . jzrzzypfkx .
Last , simTime - rtP . TransportDelay_Delay_kvsikkt5gn , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . jzrzzypfkx . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . jzrzzypfkx . Head ] = simTime ; ( * uBuffer ) [ rtDW .
jzrzzypfkx . Head ] = rtB . mcdqy0bj11 ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . eefsnpxkpz = rtB . dswmj0uw3l ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . oewxpxyjnd . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . oewxpxyjnd . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . g4bc1ycige . Head = ( ( rtDW . g4bc1ycige . Head < (
rtDW . g4bc1ycige . CircularBufSize - 1 ) ) ? ( rtDW . g4bc1ycige . Head + 1
) : 0 ) ; if ( rtDW . g4bc1ycige . Head == rtDW . g4bc1ycige . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . g4bc1ycige . CircularBufSize , &
rtDW . g4bc1ycige . Tail , & rtDW . g4bc1ycige . Head , & rtDW . g4bc1ycige .
Last , simTime - rtP . TransportDelay_Delay_db4veg0dt0 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . g4bc1ycige . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . g4bc1ycige . Head ] = simTime ; ( * uBuffer ) [ rtDW .
g4bc1ycige . Head ] = rtB . c14ocue5tc ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . j0cvdfbqu0 = rtB . cxe3gw0o1x ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . b31e0rqxiq . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . b31e0rqxiq . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . juevrs3bbp . Head = ( ( rtDW . juevrs3bbp . Head < (
rtDW . juevrs3bbp . CircularBufSize - 1 ) ) ? ( rtDW . juevrs3bbp . Head + 1
) : 0 ) ; if ( rtDW . juevrs3bbp . Head == rtDW . juevrs3bbp . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . juevrs3bbp . CircularBufSize , &
rtDW . juevrs3bbp . Tail , & rtDW . juevrs3bbp . Head , & rtDW . juevrs3bbp .
Last , simTime - rtP . TransportDelay_Delay_byanqwdxdk , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . juevrs3bbp . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . juevrs3bbp . Head ] = simTime ; ( * uBuffer ) [ rtDW .
juevrs3bbp . Head ] = rtB . cf3kqjr4wo ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . fntnt2vkft = rtB . if3103l5l4 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . nhiqsehhpt . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . nhiqsehhpt . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . bzmcnbrysf . Head = ( ( rtDW . bzmcnbrysf . Head < (
rtDW . bzmcnbrysf . CircularBufSize - 1 ) ) ? ( rtDW . bzmcnbrysf . Head + 1
) : 0 ) ; if ( rtDW . bzmcnbrysf . Head == rtDW . bzmcnbrysf . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . bzmcnbrysf . CircularBufSize , &
rtDW . bzmcnbrysf . Tail , & rtDW . bzmcnbrysf . Head , & rtDW . bzmcnbrysf .
Last , simTime - rtP . TransportDelay_Delay_hh5y3si44z , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . bzmcnbrysf . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . bzmcnbrysf . Head ] = simTime ; ( * uBuffer ) [ rtDW .
bzmcnbrysf . Head ] = rtB . geqepjgoiw ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . dmpkou34np = rtB . f1nqrhp0ew ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . lcvmbgrgk0 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . lcvmbgrgk0 . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . p5pmnzyfsd . Head = ( ( rtDW . p5pmnzyfsd . Head < (
rtDW . p5pmnzyfsd . CircularBufSize - 1 ) ) ? ( rtDW . p5pmnzyfsd . Head + 1
) : 0 ) ; if ( rtDW . p5pmnzyfsd . Head == rtDW . p5pmnzyfsd . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . p5pmnzyfsd . CircularBufSize , &
rtDW . p5pmnzyfsd . Tail , & rtDW . p5pmnzyfsd . Head , & rtDW . p5pmnzyfsd .
Last , simTime - rtP . TransportDelay_Delay_p5bivs2c3n , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . p5pmnzyfsd . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . p5pmnzyfsd . Head ] = simTime ; ( * uBuffer ) [ rtDW .
p5pmnzyfsd . Head ] = rtB . c4on2h2stl ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . ntlbbe2zlh = rtB . nyb35pqqgb ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . eogglflt5u . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . eogglflt5u . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . jaosajyms1 . Head = ( ( rtDW . jaosajyms1 . Head < (
rtDW . jaosajyms1 . CircularBufSize - 1 ) ) ? ( rtDW . jaosajyms1 . Head + 1
) : 0 ) ; if ( rtDW . jaosajyms1 . Head == rtDW . jaosajyms1 . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . jaosajyms1 . CircularBufSize , &
rtDW . jaosajyms1 . Tail , & rtDW . jaosajyms1 . Head , & rtDW . jaosajyms1 .
Last , simTime - rtP . TransportDelay_Delay_nrasksb22c , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . jaosajyms1 . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . jaosajyms1 . Head ] = simTime ; ( * uBuffer ) [ rtDW .
jaosajyms1 . Head ] = rtB . oqn5awplyp ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . k2ecgqxawh = rtB . cwlfifhebb ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . kp5tjch01i . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . kp5tjch01i . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . gimq5qj5re . Head = ( ( rtDW . gimq5qj5re . Head < (
rtDW . gimq5qj5re . CircularBufSize - 1 ) ) ? ( rtDW . gimq5qj5re . Head + 1
) : 0 ) ; if ( rtDW . gimq5qj5re . Head == rtDW . gimq5qj5re . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . gimq5qj5re . CircularBufSize , &
rtDW . gimq5qj5re . Tail , & rtDW . gimq5qj5re . Head , & rtDW . gimq5qj5re .
Last , simTime - rtP . TransportDelay_Delay_ki22t4grdn , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . gimq5qj5re . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . gimq5qj5re . Head ] = simTime ; ( * uBuffer ) [ rtDW .
gimq5qj5re . Head ] = rtB . g1uisjdg5j ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . fbmro5ygjf = rtB . awidn2hori ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . gfqptvfbp1 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . gfqptvfbp1 . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . p24tn1qizr . Head = ( ( rtDW . p24tn1qizr . Head < (
rtDW . p24tn1qizr . CircularBufSize - 1 ) ) ? ( rtDW . p24tn1qizr . Head + 1
) : 0 ) ; if ( rtDW . p24tn1qizr . Head == rtDW . p24tn1qizr . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . p24tn1qizr . CircularBufSize , &
rtDW . p24tn1qizr . Tail , & rtDW . p24tn1qizr . Head , & rtDW . p24tn1qizr .
Last , simTime - rtP . TransportDelay_Delay_d4szan31ch , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . p24tn1qizr . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . p24tn1qizr . Head ] = simTime ; ( * uBuffer ) [ rtDW .
p24tn1qizr . Head ] = rtB . dzokxeyx3l ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . nfnlqxj2g5 = rtB . gcl4vpc3yl ; rtDW . fzwqkwp2m1 = rtB .
cdxhxlj30g ; } lwcs3jh3k2 ( rtS , & rtB . a1srf3dmr2 , & rtDW . a1srf3dmr2 )
; ny353qchui ( rtS , & rtB . di10rsm5nz , & rtDW . di10rsm5nz ) ; lwcs3jh3k2
( rtS , & rtB . pry2hnvt4q , & rtDW . pry2hnvt4q ) ; ny353qchui ( rtS , & rtB
. hgwrktvi4c , & rtDW . hgwrktvi4c ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) {
rtDW . jvvuz2zzbv = rtB . gz04icgcdm ; } lwcs3jh3k2 ( rtS , & rtB .
apyjjt13q1 , & rtDW . apyjjt13q1 ) ; ny353qchui ( rtS , & rtB . arqjd0gbjl ,
& rtDW . arqjd0gbjl ) ; lwcs3jh3k2 ( rtS , & rtB . e1cruf0x3a , & rtDW .
e1cruf0x3a ) ; ny353qchui ( rtS , & rtB . pg41d4mdss , & rtDW . pg41d4mdss )
; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW . dsi2mx1crg = rtB . gu30hru3p3
; rtDW . p0tzcauox5 = rtB . pdhui3he3f ; rtDW . cjcrhlf0g0 = rtB . e5wdsnxko5
; rtDW . a3t0rfsk4v = rtB . aajgk5zxa4 ; rtDW . fj1oj1qia0 = rtB . gtfhuwgpxk
; rtDW . mpcjp04f1l = rtB . nze1yi5rvq ; rtDW . f5mcliqi4f = rtB . ifssonf1yl
; rtDW . pybydd0eqj = rtB . dibopprtmu ; } lwcs3jh3k2 ( rtS , & rtB .
dej3sbopt3 , & rtDW . dej3sbopt3 ) ; ny353qchui ( rtS , & rtB . ongp1nebfx ,
& rtDW . ongp1nebfx ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW .
j54aiqlmrr = rtB . dibopprtmu ; } lwcs3jh3k2 ( rtS , & rtB . hvibrw2awu , &
rtDW . hvibrw2awu ) ; ny353qchui ( rtS , & rtB . allgw5ptgk , & rtDW .
allgw5ptgk ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW . gokujearn0 = rtB
. g5smsjevum ; } lwcs3jh3k2 ( rtS , & rtB . f5lqeh5sbp , & rtDW . f5lqeh5sbp
) ; ny353qchui ( rtS , & rtB . br1i12egyj , & rtDW . br1i12egyj ) ;
lwcs3jh3k2 ( rtS , & rtB . jvoggbn2yd , & rtDW . jvoggbn2yd ) ; ny353qchui (
rtS , & rtB . boy5y1bmz3 , & rtDW . boy5y1bmz3 ) ; lwcs3jh3k2 ( rtS , & rtB .
ithg0f1jiq , & rtDW . ithg0f1jiq ) ; ny353qchui ( rtS , & rtB . ojnin3nhvw ,
& rtDW . ojnin3nhvw ) ; lwcs3jh3k2 ( rtS , & rtB . p2xsu5tknt , & rtDW .
p2xsu5tknt ) ; ny353qchui ( rtS , & rtB . bhbl51zywo , & rtDW . bhbl51zywo )
; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW . f23jtcpi0e = rtB . azepmxoauk
; rtDW . ih0vhsirfu = rtB . eyr5byduqe ; rtDW . buycl0ri4c = rtB . oqcfliecli
; rtDW . klktcnq5ax = rtB . pxqprudxor ; rtDW . dgygcznzfp = rtB . itbctkinpn
; rtDW . hvjbgpscln = rtB . ikcpw4shmh ; rtDW . afrib4g3ns = rtB . np1paix0hh
; rtDW . jlbauzygas = rtB . pedrjzwjtg ; rtDW . n2ncp1ufol = rtB . jt1fs0q03l
; rtDW . jzckq3ap3v = rtB . hmtg2gmje2 ; rtDW . muro5i15oa = rtB . kq0foc5zm2
; rtDW . h2djmvry3k = rtB . d011aelgcy ; rtDW . oicbkbao2v = rtB . hd1v1uva5a
; } lwcs3jh3k2 ( rtS , & rtB . diplwx4hig , & rtDW . diplwx4hig ) ;
ny353qchui ( rtS , & rtB . my1xc0on3o , & rtDW . my1xc0on3o ) ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW . hs2gbbrj20 = rtB . hd1v1uva5a ; rtDW
. fnftlbxl0k = rtB . ghyvk3fl1r ; rtDW . as0d4xo4un = rtB . meotz4mdti ; rtDW
. dymvp4turd = rtB . i5wvkswgxi ; rtDW . evvyvdybbq = rtB . dstxdfarbe ; rtDW
. ct3p3arebc = rtB . k3ivfvptjm ; rtDW . evfvx3mdmk = rtB . lecc0iibgu ; rtDW
. kn1fi20kkc = rtB . c1yx2ffjij ; } lwcs3jh3k2 ( rtS , & rtB . gn1qmb4zgp , &
rtDW . gn1qmb4zgp ) ; ny353qchui ( rtS , & rtB . bvpxesww1j , & rtDW .
bvpxesww1j ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW . aomedx5xd0 = rtB
. c1yx2ffjij ; } lwcs3jh3k2 ( rtS , & rtB . lf152u5jm4 , & rtDW . lf152u5jm4
) ; ny353qchui ( rtS , & rtB . dewlul2b2r , & rtDW . dewlul2b2r ) ; if (
ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW . f3aybkapro = rtB . itbctkinpn ; }
lwcs3jh3k2 ( rtS , & rtB . ivzhqxkykj , & rtDW . ivzhqxkykj ) ; ny353qchui (
rtS , & rtB . kd31jkgljk , & rtDW . kd31jkgljk ) ; if ( ssIsSampleHit ( rtS ,
1 , 0 ) ) { rtDW . g4kyas3bbl = rtB . pslefhjvqh ; rtDW . ememr0qwkc = rtB .
mdzt5k4iw4 ; rtDW . jirvvhrkai = rtB . p2k4trcdah ; rtDW . insgv4zthl = rtB .
aysknorjt2 ; rtDW . fwreysljhu = rtB . hhp0z2gwbc ; rtDW . gvtu2sgvui = rtB .
hghek1rdpc ; rtDW . igkazskzyf = rtB . etzzrmejrs ; rtDW . fer4rqlapl = rtB .
bnlt0zss24 ; rtDW . lczlugrqza = rtB . madbzdt0vb ; rtDW . fkz4l3azou = rtB .
e3kpmmlmpp ; rtDW . hrqcp0us0o = rtB . b2l3iqm0n2 ; rtDW . ji2nko31lt = rtB .
pcsrwwgvf2 ; rtDW . cqrjo5bott = rtB . l0lfzybrpw ; } lwcs3jh3k2 ( rtS , &
rtB . if1byo0yst , & rtDW . if1byo0yst ) ; ny353qchui ( rtS , & rtB .
j5kctkibbm , & rtDW . j5kctkibbm ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) {
rtDW . ndo55xdrjz = rtB . l0lfzybrpw ; rtDW . pxt53ienu3 = rtB . ksqaf3kqa5 ;
rtDW . auxttyil24 = rtB . b0f344lzrq ; rtDW . nd3ykklatb = rtB . a4rgreqjnk ;
rtDW . aowojzitws = rtB . h0ozrpudg0 ; rtDW . gubginzy5v = rtB . gniuklex1s ;
rtDW . emo0onv4iy = rtB . dt35so0slm ; rtDW . oevsqktqdh = rtB . kgisxdmvsw ;
} lwcs3jh3k2 ( rtS , & rtB . onwgyzhgir , & rtDW . onwgyzhgir ) ; ny353qchui
( rtS , & rtB . ofq4vda2xo , & rtDW . ofq4vda2xo ) ; if ( ssIsSampleHit ( rtS
, 1 , 0 ) ) { rtDW . h2duquj31a = rtB . kgisxdmvsw ; } lwcs3jh3k2 ( rtS , &
rtB . b13ln335zj , & rtDW . b13ln335zj ) ; ny353qchui ( rtS , & rtB .
d5dfks3vhb , & rtDW . d5dfks3vhb ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) {
rtDW . ikejccrfw4 = rtB . hhp0z2gwbc ; } lwcs3jh3k2 ( rtS , & rtB .
d2dryokhth , & rtDW . d2dryokhth ) ; ny353qchui ( rtS , & rtB . jcev1klzwf ,
& rtDW . jcev1klzwf ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW .
fyqrim4x4y = rtB . bkm0bor5oa ; rtDW . lw0ubiboa0 = rtB . pkbqlnw2xh ; rtDW .
odfrq0vcqw = rtB . kvdxfpuwws ; rtDW . hlxu2b4hfp = rtB . oyvm5cb5r3 ; rtDW .
nyqadzpf4p = rtB . pgc5xttltd ; rtDW . crlu0ruqew = rtB . ncoqcmpnrk ; rtDW .
k1pkt3do0q = rtB . nijvsalqrr ; rtDW . puvpzy5tok = rtB . pgc5xttltd ; rtDW .
ci14pxvpr2 = rtB . c4wtgq3rtz ; rtDW . jaz3uqfgz2 = rtB . afinszx4uw ; rtDW .
d4pejtpczl = rtB . c4wtgq3rtz ; rtDW . aj2otrttnf = rtB . k2rjzsbrwp ; rtDW .
euvp233z54 = rtB . iaspj3iyrw ; rtDW . bpckxrqz0a = rtB . dtoktjat0n ; rtDW .
laigsna1ry = rtB . o0kmsy0anj ; rtDW . gtdowlniha = rtB . eegmo1bb3b ; }
lwcs3jh3k2 ( rtS , & rtB . aa351sera2 , & rtDW . aa351sera2 ) ; ny353qchui (
rtS , & rtB . lfm4cvmyjw , & rtDW . lfm4cvmyjw ) ; if ( ssIsSampleHit ( rtS ,
1 , 0 ) ) { rtDW . md3pjrrcpi = rtB . o0kmsy0anj ; } lwcs3jh3k2 ( rtS , & rtB
. bypgcvdndn , & rtDW . bypgcvdndn ) ; ny353qchui ( rtS , & rtB . dtwz5asia3
, & rtDW . dtwz5asia3 ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW .
maqoyqobhp = rtB . hww5gj4qer ; rtDW . hwukdcvf00 = rtB . j4z03mncvk ; }
lwcs3jh3k2 ( rtS , & rtB . dkvqk03afq , & rtDW . dkvqk03afq ) ; ny353qchui (
rtS , & rtB . drlew34sm0 , & rtDW . drlew34sm0 ) ; if ( ssIsSampleHit ( rtS ,
1 , 0 ) ) { rtDW . ip1qcpepxp = rtB . hww5gj4qer ; } lwcs3jh3k2 ( rtS , & rtB
. j3mwdbpvpq , & rtDW . j3mwdbpvpq ) ; ny353qchui ( rtS , & rtB . ieaaexns5m
, & rtDW . ieaaexns5m ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW .
pxb20wrhli = rtB . b4y0mche2n ; rtDW . gtyu3k2ot5 = rtB . phtic0hp2q ; }
lwcs3jh3k2 ( rtS , & rtB . eg2uuciuj1 , & rtDW . eg2uuciuj1 ) ; ny353qchui (
rtS , & rtB . emkwv3osqn , & rtDW . emkwv3osqn ) ; if ( ssIsSampleHit ( rtS ,
1 , 0 ) ) { rtDW . cm1jdei3jr = rtB . b4y0mche2n ; } lwcs3jh3k2 ( rtS , & rtB
. acyd2alexd , & rtDW . acyd2alexd ) ; ny353qchui ( rtS , & rtB . kcdiwopp0s
, & rtDW . kcdiwopp0s ) ; lwcs3jh3k2 ( rtS , & rtB . bd4y1f4afb , & rtDW .
bd4y1f4afb ) ; ny353qchui ( rtS , & rtB . bbo3ui3fzz , & rtDW . bbo3ui3fzz )
; lwcs3jh3k2 ( rtS , & rtB . jejxmo41e1 , & rtDW . jejxmo41e1 ) ; ny353qchui
( rtS , & rtB . kwzmy55kax , & rtDW . kwzmy55kax ) ; lwcs3jh3k2 ( rtS , & rtB
. ge1ps1iuod , & rtDW . ge1ps1iuod ) ; ny353qchui ( rtS , & rtB . bk3jg31gdb
, & rtDW . bk3jg31gdb ) ; lwcs3jh3k2 ( rtS , & rtB . i43atkcssg , & rtDW .
i43atkcssg ) ; ny353qchui ( rtS , & rtB . kqhpdk5kzz , & rtDW . kqhpdk5kzz )
; lwcs3jh3k2 ( rtS , & rtB . jvraryandh , & rtDW . jvraryandh ) ; ny353qchui
( rtS , & rtB . e2epyeugyn , & rtDW . e2epyeugyn ) ; if ( ssIsSampleHit ( rtS
, 1 , 0 ) ) { rtDW . aj21ow2g22 = rtB . fado3mjkqp ; rtDW . k344xgbfw1 = rtB
. ckj0njvng3 ; rtDW . equ3xylxak = rtB . kuymr5hgfm ; rtDW . p35tk2ttns = rtB
. bzfuqbt5cs ; } lwcs3jh3k2 ( rtS , & rtB . dffp1wyrjz , & rtDW . dffp1wyrjz
) ; ny353qchui ( rtS , & rtB . mwrkb5ri4b , & rtDW . mwrkb5ri4b ) ;
lwcs3jh3k2 ( rtS , & rtB . on20slldft0 , & rtDW . on20slldft0 ) ; ny353qchui
( rtS , & rtB . bhc3qgnuicw , & rtDW . bhc3qgnuicw ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtDW . b55d2d4pem = rtB . h1l2yohxy4 ; rtDW . h5iqn1tf0f =
rtB . dbl43jye5a ; rtDW . iabw2jrkb0 = rtB . jshyudx1xv ; rtDW . mzqdovgi1x =
rtB . b1qme4xqqh ; rtDW . i4uxbnvgsv = rtB . p42kafnbz1 ; rtDW . bmkmerzuty =
rtB . dutizthppc ; rtDW . idmf1p4vkt = rtB . eb1jbzfho2 ; rtDW . pwnzz0enea =
rtB . pzs5dww0bf ; rtDW . of54iatyrl = rtB . pf1chrm4t2 ; rtDW . lgg4cwcbcv =
rtB . gyavbiowm4 ; rtDW . ldsl4h1b2m = rtB . ojwyzbseek ; rtDW . fmwrpqnbm1 =
rtB . gxsxbctriq ; rtDW . lltb10cjgb = rtB . jh443z4du0 ; rtDW . f3l0hj1q5z =
rtB . n1n20f2zar ; rtDW . gwjqd3rg01 = rtB . c4xj4bdbrv ; rtDW . jn3q0bmaxx =
rtB . b2etrzojot ; rtDW . mpw5qp3zhv = rtB . akpips0qmp ; rtDW . e20bjtoooq =
rtB . dxa1300d4c ; rtDW . iphvfgl3lu = rtB . awhysiepcl ; rtDW . a3qbrcdsa4 =
rtB . fmj4bqqzzz ; rtDW . hrvs2bhnce = rtB . nokku3vzx4 ; rtDW . pnnzfpsb2c =
rtB . mexmdikovu ; rtDW . jgoshqlbir = rtB . hbyb5yq2wa ; rtDW . jcjg4wdcj2 =
rtB . dgvytedolp ; rtDW . gylvz5hxsg = rtB . l1fyknaxfx ; rtDW . gyn5krs1zz =
rtB . catom4ds04 ; rtDW . jwnv52k1wb = rtB . dqsdtpxg5g ; rtDW . mbo2opl3eu =
rtB . dz5iehv51p ; rtDW . c3jeodupfc = rtB . iy34vxel11 ; rtDW . giowad3sec =
rtB . abq0huqfl5 ; rtDW . md4snt4zsk = rtB . p2s4q11xzy ; rtDW . i1pprdx5pp =
rtB . nosoqcshkk ; rtDW . f2nxapnymz = rtB . al1vrz51ml ; rtDW . autyikzmna =
rtB . l2zahd4oox ; rtDW . n3upjnxoej = rtB . kjuppwtwvu ; rtDW . i0enxeox0z =
rtB . je5jzdhzwr ; rtDW . ifgv45o1io = rtB . o1vk0isp23 ; rtDW . g1pk0bzvtv =
rtB . in2bbvlk2d ; rtDW . ah5hnpnyx1 = rtB . bfptuyuq2b ; rtDW . gry5zwpbhu =
rtB . g340u5g2f2 ; rtDW . ptqaeanwg0 = rtB . divwonmszi ; rtDW . iug5i33rtw =
rtB . ctjiw4lx5e ; rtDW . mdvftfecjd = rtB . ho330fy4bu ; rtDW . fufczbmjix =
rtB . gikkmuveb2 ; rtDW . dxi5yvdasd = rtB . kspoa0qx51 ; rtDW . eowtljwki4 =
rtB . f4thrtkrpr ; rtDW . l4egugzm4w = rtB . cxrzkrt5zb ; rtDW . byq22ip4jp =
rtB . fafsbij0lp ; rtDW . cm5vdxv24a = rtB . bywjvbcj1o ; rtDW . flh5edlegj =
rtB . emlwggxmjw ; rtDW . pxv2xpdkad = rtB . ggfetidn5i ; rtDW . f4thttu240 =
rtB . j4ab1fsmlj ; rtDW . jgrmmzecam = rtB . hwhh445rbd ; rtDW . cjg142fgy0 =
rtB . fk5mmytqj3 ; rtDW . eteblkpynn = rtB . catr4fqykw ; rtDW . pnthkernke =
rtB . nubgwxsn52 ; rtDW . nihpinzhrt = rtB . e4ltx14wep ; rtDW . p3uq2rdn1i =
rtB . busrc0io0m ; rtDW . nh3lnfbigq = rtB . pl2ppwdjgc ; rtDW . dzo14tsqn4 =
rtB . nkf4fkpcah ; rtDW . gcy3ctyywg = rtB . iezbiixuat ; rtDW . i5a0jb1dk3 =
rtB . ksj03ljl1h ; rtDW . guzsz3fvfs = rtB . bvdns4bi03 ; rtDW . az2oeo1f4o =
rtB . op5u32jxd5 ; rtDW . cij1nabjqb = rtB . hatod3ncks ; rtDW . f2rohdyfa1 =
rtB . nx1hwu0e2s ; rtDW . ie01l5vavi = rtB . prgykcukfr ; rtDW . nwhg3zqrsl =
rtB . ebxp2katk3 ; rtDW . mv54qvcn5q = rtB . lo2okwom4h ; rtDW . fjxk3uaez4 =
rtB . jooyjga1t0 ; rtDW . i21ukrhdhv = rtB . atq3qhvwac ; rtDW . eu3dowbkcr =
rtB . ejjatvy0ji ; rtDW . oxmyy55gc0 = rtB . hlmtyy2tbw ; rtDW . abpj3txsdh =
rtB . jlqpuiagan ; rtDW . e0oousi5de = rtB . hqebmcdiwg ; rtDW . a0eu1zkv1e =
rtB . ohyeqpd5va ; rtDW . n1z1agzu3x = rtB . pd2hdhthkb ; rtDW . nngs4wq1em =
rtB . kgrtiplduv ; rtDW . aynri5lox1 = rtB . krvqv214qo ; rtDW . lmvrvbjthf =
rtB . lxbpdmf1sv ; rtDW . bxrofmksin = rtB . pqhmamvv3w ; rtDW . btgc2mhzkb =
rtB . o0qdd2m5ng ; rtDW . o55xttyljm = rtB . igt3ds4gub ; rtDW . ev3rm05gtu =
rtB . ploid00iph ; rtDW . e1zljawh5b = rtB . me3plfssrh ; rtDW . dv30vpwozb =
rtB . o2aewyydjc ; rtDW . fe4qsttuig = rtB . dfugdqxz5n ; rtDW . gijbul5bro =
rtB . ohtsbgtzyy ; rtDW . fx2ubbeual = rtB . asa103d02z ; rtDW . kdodmy0lc5 =
rtB . m5czbh1ofs ; rtDW . p2lcbu1i1l = rtB . omlumxq3kv ; rtDW . cstjzr5zdb =
rtB . gleefrzwuw ; rtDW . o5i22qdh2m = rtB . obatadlmps ; rtDW . nhuixzwcbu =
rtB . cnsaptvv05 ; rtDW . k4yt0gbyvq = rtB . ddix2mhi1g ; rtDW . hzgwuaj0dk =
rtB . dsokhkpxss ; rtDW . p2kf00znzb = rtB . m34qzw1e3z ; rtDW . mitpuusdiu =
rtB . iphvtnqf5a ; rtDW . gl5n5tgtur = rtB . fw2ws32qdy ; rtDW . gm3eatvpbt =
rtB . iyjx4tiruw ; rtDW . faqnzz3umd = rtB . momorbvhot ; rtDW . iy0b4tzwiz =
rtB . corxgi5bda ; rtDW . dpmou3nofz = rtB . d0qqsnjj4o ; rtDW . oad2ixcyjj =
rtB . gukq3uey0u ; rtDW . mwwno0g4te = rtB . fizn4ck4n1 ; rtDW . laawtmho0w =
rtB . cg2mdcmr4c ; rtDW . aifuwgprs1 = rtB . lk3rhzaqlz ; rtDW . j2ytkxdub5 =
rtB . awh5feillk ; rtDW . efpieuuisx = rtB . mrzx2rambn ; rtDW . gnbbybfox4 =
rtB . fp5ajke5t2 ; rtDW . inaoot3v53 = rtB . ndiffna1hx ; rtDW . kzrubw1lpo =
rtB . frpynadqtk ; rtDW . aqriwgcdaa = rtB . ef0231xcnk ; rtDW . akfyoog544 =
rtB . bvagjjcpxc ; rtDW . dloemu5bxh = rtB . ned0nfq0np ; rtDW . llzhlzgbyq =
rtB . cwviljcsth ; rtDW . gk45nsnm5f = rtB . falpkdmxd5 ; rtDW . bd5awmqnm1 =
rtB . mj31fir04g ; rtDW . a0zxdwo0kv = rtB . bsh0vnffms ; } lwcs3jh3k2 ( rtS
, & rtB . l0mam0lcui , & rtDW . l0mam0lcui ) ; ny353qchui ( rtS , & rtB .
ck45m3cw5y , & rtDW . ck45m3cw5y ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) {
rtDW . cgxfb5ilba = rtB . mj31fir04g ; } lwcs3jh3k2 ( rtS , & rtB .
o3bwas2b4l , & rtDW . o3bwas2b4l ) ; ny353qchui ( rtS , & rtB . h330fuij3a ,
& rtDW . h330fuij3a ) ; lwcs3jh3k2 ( rtS , & rtB . ifezniamwz , & rtDW .
ifezniamwz ) ; ny353qchui ( rtS , & rtB . dsdu0sw4bh , & rtDW . dsdu0sw4bh )
; lwcs3jh3k2 ( rtS , & rtB . cssuh33ty1 , & rtDW . cssuh33ty1 ) ; ny353qchui
( rtS , & rtB . nmmyyafynb , & rtDW . nmmyyafynb ) ; lwcs3jh3k2 ( rtS , & rtB
. pflqpv2odc , & rtDW . pflqpv2odc ) ; ny353qchui ( rtS , & rtB . c5rp3exjwz
, & rtDW . c5rp3exjwz ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW .
jyeacneoju = rtB . b04myoxm0j ; rtDW . i5d0s3lepy = rtB . me3w3v2md1 ; rtDW .
pw1qqgmx0o = rtB . hql3wzyeac ; rtDW . ly0wgshoe2 = rtB . f05npjqjmg ; rtDW .
osx1vowkj4 = rtB . bkmtw4gidw ; rtDW . l2fddlwysw = rtB . kcfeapnbix ; rtDW .
d3dlbfiuuv = rtB . op3i1voznh ; } lwcs3jh3k2 ( rtS , & rtB . cqgqlcfbrn , &
rtDW . cqgqlcfbrn ) ; ny353qchui ( rtS , & rtB . pshtvwhcvn , & rtDW .
pshtvwhcvn ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW . odjstwuwx3 = rtB
. kcfeapnbix ; } lwcs3jh3k2 ( rtS , & rtB . cwhxhlnmc3 , & rtDW . cwhxhlnmc3
) ; ny353qchui ( rtS , & rtB . pxbtpesdlg , & rtDW . pxbtpesdlg ) ;
lwcs3jh3k2 ( rtS , & rtB . j1qputxmt3 , & rtDW . j1qputxmt3 ) ; ny353qchui (
rtS , & rtB . h4jqcza024 , & rtDW . h4jqcza024 ) ; lwcs3jh3k2 ( rtS , & rtB .
h510twish3 , & rtDW . h510twish3 ) ; ny353qchui ( rtS , & rtB . b1csqxlk14 ,
& rtDW . b1csqxlk14 ) ; lwcs3jh3k2 ( rtS , & rtB . cdulylvwwz , & rtDW .
cdulylvwwz ) ; ny353qchui ( rtS , & rtB . o2355kd0aj , & rtDW . o2355kd0aj )
; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW . lmsqqojpvs = rtB . forzhlyp0b
; rtDW . kpsmnp3ug0 = rtB . kqutakybog ; rtDW . ovtmlyjrxp = rtB . ditynecwsc
; rtDW . fbjudlgv4x = rtB . pg4ng5z5mp ; rtDW . gmqawd5msq = rtB . g1e2vuafcc
; rtDW . kegqko1pyk = rtB . bsh0vnffms ; } lwcs3jh3k2 ( rtS , & rtB .
muhd0m2ek3 , & rtDW . muhd0m2ek3 ) ; ny353qchui ( rtS , & rtB . g2vaojwvzg ,
& rtDW . g2vaojwvzg ) ; lwcs3jh3k2 ( rtS , & rtB . iosuhjjule , & rtDW .
iosuhjjule ) ; ny353qchui ( rtS , & rtB . fey4zxdyka , & rtDW . fey4zxdyka )
; lwcs3jh3k2 ( rtS , & rtB . j0x4gfzwjd , & rtDW . j0x4gfzwjd ) ; ny353qchui
( rtS , & rtB . anq402mwuh , & rtDW . anq402mwuh ) ; lwcs3jh3k2 ( rtS , & rtB
. kxnpw5jl23 , & rtDW . kxnpw5jl23 ) ; ny353qchui ( rtS , & rtB . g2dmi2bhhz
, & rtDW . g2dmi2bhhz ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW .
crgr0yyjst = rtB . h4lmxj4upb ; rtDW . ex4vqumvao = rtB . kbpdscow2k ; rtDW .
nbtfsiobop = rtB . hse2egktr5 ; rtDW . jl4tnonvue = rtB . m4ygpwffk0 ; }
lwcs3jh3k2 ( rtS , & rtB . jzgpron3co , & rtDW . jzgpron3co ) ; ny353qchui (
rtS , & rtB . dzkeyq1gmc , & rtDW . dzkeyq1gmc ) ; if ( ssIsSampleHit ( rtS ,
1 , 0 ) ) { rtDW . m3pubmd011 = rtB . ieroamoqnq ; rtDW . fvjg0rzhqa = rtB .
insjcpo2lh ; rtDW . h0ue4eifzz = rtB . mcfvfus3t5 ; rtDW . dxcijq0izx = rtB .
pj4d0qpawy ; rtDW . elckdgh4b5 = rtB . cutoxtvypc ; rtDW . e4su0h1vb4 = rtB .
puigrufrlb ; rtDW . j2nec0kpix = rtB . k5x5ns5y3r ; rtDW . kf53zbkqlj = rtB .
ekqskdscbf ; rtDW . ne0hnild1t = rtB . or3w5oxavu ; rtDW . mnr1ns2sul = rtB .
lvdf3hf2vq ; rtDW . ozt4gwuaev = rtB . a1fppg4cgw ; rtDW . e11plvgt2e = rtB .
cglx0ysff3 ; rtDW . davrjeeiee = rtB . i2zmrqbqdw ; } lwcs3jh3k2 ( rtS , &
rtB . ox1j1fxqy2 , & rtDW . ox1j1fxqy2 ) ; ny353qchui ( rtS , & rtB .
nmew4qgpmi , & rtDW . nmew4qgpmi ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) {
rtDW . avb4nhw551 = rtB . c4ovh2qbj4 ; rtDW . fevmalljs2 = rtB . ka5yf13wyi ;
rtDW . b0vjxwd4dx = rtB . hwtfjycbkc ; rtDW . fpax42xxm5 = rtB . dfvymmiswe ;
} lwcs3jh3k2 ( rtS , & rtB . outjvmmxn3 , & rtDW . outjvmmxn3 ) ; ny353qchui
( rtS , & rtB . brhbedften , & rtDW . brhbedften ) ; lwcs3jh3k2 ( rtS , & rtB
. pwnmvwir3d , & rtDW . pwnmvwir3d ) ; ny353qchui ( rtS , & rtB . ozvscdoxcm
, & rtDW . ozvscdoxcm ) ; lwcs3jh3k2 ( rtS , & rtB . dyyk14ao4t , & rtDW .
dyyk14ao4t ) ; ny353qchui ( rtS , & rtB . k5ecxmrj0a , & rtDW . k5ecxmrj0a )
; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW . pflycifi2v = rtB . psquva50gj
; rtDW . kja5xrgmwv = rtB . keb0gbnbec ; rtDW . e3tarqgwys = rtB . eh1veelmvt
; rtDW . hltxj0u51z = rtB . oqcqixssx1 ; rtDW . hlihecl4y1 = rtB . nxylvaytkx
; rtDW . ifc1hwj1ct = rtB . ct2axyzs4t ; rtDW . bdb0pmfh2i = rtB . lpotyyyzgy
; rtDW . lb3vrehkjt = rtB . du5okalvyy ; rtDW . fra1s15lot = rtB . myd4osbk01
; rtDW . pvwn2twq14 = rtB . fe2phksyjg ; rtDW . fwvi3ktkgn = rtB . k3om0acsgh
; rtDW . danlnozb3w = rtB . io0nqp4diw ; rtDW . dglgxggn0s = rtB . ktvbljhq2p
; rtDW . levorifkrw = rtB . a0it33vpv3 ; rtDW . hxmfwxg20s = rtB . gr4vjiot5p
; rtDW . hdod2qacjn = rtB . lz0cksews5 ; rtDW . k11kfdk1be = false ; rtDW .
bdb5r4moyp = false ; rtDW . mhkbaz10cz = false ; rtDW . f5dfmvvs02 = rtB .
ov31k4ksh0 ; rtDW . ecq2t4tug1 = false ; } lwcs3jh3k2 ( rtS , & rtB .
c2rt0zs300 , & rtDW . c2rt0zs300 ) ; ny353qchui ( rtS , & rtB . pj2a3t0x3l ,
& rtDW . pj2a3t0x3l ) ; { real_T * * uBuffer = ( real_T * * ) & rtDW .
l0gmb53kdd . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & rtDW
. l0gmb53kdd . TUbufferPtrs [ 1 ] ; real_T * * xBuffer = ( real_T * * ) &
rtDW . l0gmb53kdd . TUbufferPtrs [ 2 ] ; real_T simTime = ssGetT ( rtS ) ;
rtDW . ees1hdl3bo . Head = ( ( rtDW . ees1hdl3bo . Head < ( rtDW . ees1hdl3bo
. CircularBufSize - 1 ) ) ? ( rtDW . ees1hdl3bo . Head + 1 ) : 0 ) ; if (
rtDW . ees1hdl3bo . Head == rtDW . ees1hdl3bo . Tail ) { if ( !
rt_TDelayUpdateTailOrGrowBuf ( & rtDW . ees1hdl3bo . CircularBufSize , & rtDW
. ees1hdl3bo . Tail , & rtDW . ees1hdl3bo . Head , & rtDW . ees1hdl3bo . Last
, simTime - rtP . VariableTransportDelay_MaxDelay_baatiozaev , tBuffer ,
uBuffer , xBuffer , ( boolean_T ) 0 , ( boolean_T ) 1 , & rtDW . ees1hdl3bo .
MaxNewBufSize ) ) { ssSetErrorStatus ( rtS ,
"vtdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ rtDW .
ees1hdl3bo . Head ] = simTime ; ( * uBuffer ) [ rtDW . ees1hdl3bo . Head ] =
rtB . lu21q4ctfu ; ( * xBuffer ) [ rtDW . ees1hdl3bo . Head ] = rtX .
gukucggul0 ; } { real_T * * uBuffer = ( real_T * * ) & rtDW . i3r0m4gxn0 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & rtDW . i3r0m4gxn0
. TUbufferPtrs [ 1 ] ; real_T * * xBuffer = ( real_T * * ) & rtDW .
i3r0m4gxn0 . TUbufferPtrs [ 2 ] ; real_T simTime = ssGetT ( rtS ) ; rtDW .
gty02ianr0 . Head = ( ( rtDW . gty02ianr0 . Head < ( rtDW . gty02ianr0 .
CircularBufSize - 1 ) ) ? ( rtDW . gty02ianr0 . Head + 1 ) : 0 ) ; if ( rtDW
. gty02ianr0 . Head == rtDW . gty02ianr0 . Tail ) { if ( !
rt_TDelayUpdateTailOrGrowBuf ( & rtDW . gty02ianr0 . CircularBufSize , & rtDW
. gty02ianr0 . Tail , & rtDW . gty02ianr0 . Head , & rtDW . gty02ianr0 . Last
, simTime - rtP . VariableTransportDelay_MaxDelay_ml14mdwepr , tBuffer ,
uBuffer , xBuffer , ( boolean_T ) 0 , ( boolean_T ) 1 , & rtDW . gty02ianr0 .
MaxNewBufSize ) ) { ssSetErrorStatus ( rtS ,
"vtdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ rtDW .
gty02ianr0 . Head ] = simTime ; ( * uBuffer ) [ rtDW . gty02ianr0 . Head ] =
rtB . obde4yjr25 ; ( * xBuffer ) [ rtDW . gty02ianr0 . Head ] = rtX .
cooihtzhhj ; } if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW . ph2qy0rwkn = rtB
. dgkrnt3j3u ; rtDW . ojkg1k5daw = rtB . kzvwlld5zx ; rtDW . a0uzfqyhcp = rtB
. gjp3g4kh1s ; rtDW . invgjx5x0c = rtB . hmmpk1cj00 ; rtDW . jwthvsveef = rtB
. me0q4vq4ka ; rtDW . k3ksd4k1ib = rtB . ov31k4ksh0 ; rtDW . gbzb4i3qdq = rtB
. ifvx3udmfy ; rtDW . gkn55q342j = rtB . cjh0zlbcbc ; rtDW . njpxzlmy40 = rtB
. nvrbl35obj ; rtDW . f4s0lo0uob = rtB . jzy5pkzfwf ; } lwcs3jh3k2 ( rtS , &
rtB . lkrts5ohpw , & rtDW . lkrts5ohpw ) ; ny353qchui ( rtS , & rtB .
hh2cmxftpw , & rtDW . hh2cmxftpw ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) {
rtDW . mls4swjwmm = rtB . nvrbl35obj ; } lwcs3jh3k2 ( rtS , & rtB .
me023aolbi , & rtDW . me023aolbi ) ; ny353qchui ( rtS , & rtB . dexvjkg0hj ,
& rtDW . dexvjkg0hj ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW .
kguu1igm24 = rtB . pb55w0r34z ; rtDW . l4mjhh4eud = rtB . n2n40ci54z ; }
lwcs3jh3k2 ( rtS , & rtB . purgdi1jfk , & rtDW . purgdi1jfk ) ; ny353qchui (
rtS , & rtB . g3bxbyypfb , & rtDW . g3bxbyypfb ) ; if ( ssIsSampleHit ( rtS ,
1 , 0 ) ) { rtDW . nqy0vnt1sc = rtB . pb55w0r34z ; } lwcs3jh3k2 ( rtS , & rtB
. e4qyv4smfd , & rtDW . e4qyv4smfd ) ; ny353qchui ( rtS , & rtB . bs5m40ynfy
, & rtDW . bs5m40ynfy ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW .
hhnsho4bie = rtB . djcrc2xxmk ; rtDW . goafipr0ei = rtB . nnjlecshqw ; }
lwcs3jh3k2 ( rtS , & rtB . lu12zasvyl , & rtDW . lu12zasvyl ) ; ny353qchui (
rtS , & rtB . ngrhemover , & rtDW . ngrhemover ) ; if ( ssIsSampleHit ( rtS ,
1 , 0 ) ) { rtDW . jbzxllbqx1 = rtB . djcrc2xxmk ; } lwcs3jh3k2 ( rtS , & rtB
. myp2dcpbvg , & rtDW . myp2dcpbvg ) ; ny353qchui ( rtS , & rtB . onemmt3xbs
, & rtDW . onemmt3xbs ) ; lwcs3jh3k2 ( rtS , & rtB . nci0imhwrv , & rtDW .
nci0imhwrv ) ; ny353qchui ( rtS , & rtB . dlrk53odir , & rtDW . dlrk53odir )
; lwcs3jh3k2 ( rtS , & rtB . mpydob1efk , & rtDW . mpydob1efk ) ; ny353qchui
( rtS , & rtB . dlwrrwcsx5 , & rtDW . dlwrrwcsx5 ) ; lwcs3jh3k2 ( rtS , & rtB
. lfrovwpki3 , & rtDW . lfrovwpki3 ) ; ny353qchui ( rtS , & rtB . h145orsdu5
, & rtDW . h145orsdu5 ) ; lwcs3jh3k2 ( rtS , & rtB . lbq0bgvgcl , & rtDW .
lbq0bgvgcl ) ; ny353qchui ( rtS , & rtB . cq2uqua4yo , & rtDW . cq2uqua4yo )
; lwcs3jh3k2 ( rtS , & rtB . inwm331xkg , & rtDW . inwm331xkg ) ; ny353qchui
( rtS , & rtB . obeddxl4xp , & rtDW . obeddxl4xp ) ; if ( ssIsSampleHit ( rtS
, 1 , 0 ) ) { rtDW . hdv5m3k0vy = rtB . ixkx5kqkpr ; rtDW . lz1cv0do2t = rtB
. lmwmcpsvhm ; rtDW . cgbiv0cyqn = rtB . n110gjtipb ; rtDW . kvmhfywt50 = rtB
. okjxoxwhum ; } lwcs3jh3k2 ( rtS , & rtB . crzn5z4zte , & rtDW . crzn5z4zte
) ; ny353qchui ( rtS , & rtB . llpwcowx1f , & rtDW . llpwcowx1f ) ;
lwcs3jh3k2 ( rtS , & rtB . hytx1hl5s5 , & rtDW . hytx1hl5s5 ) ; ny353qchui (
rtS , & rtB . mgkfvqxti2 , & rtDW . mgkfvqxti2 ) ; if ( ssIsSampleHit ( rtS ,
1 , 0 ) ) { rtDW . lsieos4skt = rtB . pwh01jekkk ; rtDW . edyvvi3ty4 = rtB .
ligdly522h ; rtDW . hvm2hswn02 = rtB . bfzarqag03 ; rtDW . ppskp5kzs0 = rtB .
pn5gpdyogd ; rtDW . jurv4x4dxh = rtB . eulqidvnch ; rtDW . n3llciapkl = rtB .
jl3mlxyvfa ; rtDW . ld1xzt33jz = rtB . k5dchzskoq ; rtDW . fsgvohzvsa = rtB .
ntrbre3nrs ; rtDW . amjwljpykx = rtB . cdjlsurquj ; rtDW . keetxs0kf4 = rtB .
hcvzaszisz ; } rtDW . cauqpp1pbg = 0 ; if ( rtDW . br5wyeghdu ) { { real_T *
* uBuffer = ( real_T * * ) & rtDW . gxot3lz5fc . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & rtDW . gxot3lz5fc . TUbufferPtrs [ 1 ] ;
real_T * * xBuffer = ( real_T * * ) & rtDW . gxot3lz5fc . TUbufferPtrs [ 2 ]
; real_T simTime = ssGetT ( rtS ) ; rtDW . nklbecjxrg . Head = ( ( rtDW .
nklbecjxrg . Head < ( rtDW . nklbecjxrg . CircularBufSize - 1 ) ) ? ( rtDW .
nklbecjxrg . Head + 1 ) : 0 ) ; if ( rtDW . nklbecjxrg . Head == rtDW .
nklbecjxrg . Tail ) { if ( ! rt_TDelayUpdateTailOrGrowBuf ( & rtDW .
nklbecjxrg . CircularBufSize , & rtDW . nklbecjxrg . Tail , & rtDW .
nklbecjxrg . Head , & rtDW . nklbecjxrg . Last , simTime - rtP .
VariableTransportDelay_MaxDelay , tBuffer , uBuffer , xBuffer , ( boolean_T )
0 , ( boolean_T ) 1 , & rtDW . nklbecjxrg . MaxNewBufSize ) ) {
ssSetErrorStatus ( rtS , "vtdelay memory allocation error" ) ; return ; } } (
* tBuffer ) [ rtDW . nklbecjxrg . Head ] = simTime ; ( * uBuffer ) [ rtDW .
nklbecjxrg . Head ] = rtB . i4ietgmv3i ; ( * xBuffer ) [ rtDW . nklbecjxrg .
Head ] = rtX . gfghljc4gn ; } if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW .
lnio5vndj4 = rtB . mdhu3gdhh2 ; } { real_T * * uBuffer = ( real_T * * ) &
rtDW . cay5bhojbk . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * )
& rtDW . cay5bhojbk . TUbufferPtrs [ 1 ] ; real_T * * xBuffer = ( real_T * *
) & rtDW . cay5bhojbk . TUbufferPtrs [ 2 ] ; real_T simTime = ssGetT ( rtS )
; rtDW . lxbhmg25bu . Head = ( ( rtDW . lxbhmg25bu . Head < ( rtDW .
lxbhmg25bu . CircularBufSize - 1 ) ) ? ( rtDW . lxbhmg25bu . Head + 1 ) : 0 )
; if ( rtDW . lxbhmg25bu . Head == rtDW . lxbhmg25bu . Tail ) { if ( !
rt_TDelayUpdateTailOrGrowBuf ( & rtDW . lxbhmg25bu . CircularBufSize , & rtDW
. lxbhmg25bu . Tail , & rtDW . lxbhmg25bu . Head , & rtDW . lxbhmg25bu . Last
, simTime - rtP . VariableTransportDelay_MaxDelay_du1vxketzx , tBuffer ,
uBuffer , xBuffer , ( boolean_T ) 0 , ( boolean_T ) 1 , & rtDW . lxbhmg25bu .
MaxNewBufSize ) ) { ssSetErrorStatus ( rtS ,
"vtdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ rtDW .
lxbhmg25bu . Head ] = simTime ; ( * uBuffer ) [ rtDW . lxbhmg25bu . Head ] =
rtB . lyqkavx3aw ; ( * xBuffer ) [ rtDW . lxbhmg25bu . Head ] = rtX .
nhazmrtgmb ; } if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW . cxai1mydgu = rtB
. ergyhagb2w ; } } { real_T * * uBuffer = ( real_T * * ) & rtDW . bizzathsjz
. TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & rtDW .
bizzathsjz . TUbufferPtrs [ 1 ] ; real_T * * xBuffer = ( real_T * * ) & rtDW
. bizzathsjz . TUbufferPtrs [ 2 ] ; real_T simTime = ssGetT ( rtS ) ; rtDW .
lhuqmlzywi . Head = ( ( rtDW . lhuqmlzywi . Head < ( rtDW . lhuqmlzywi .
CircularBufSize - 1 ) ) ? ( rtDW . lhuqmlzywi . Head + 1 ) : 0 ) ; if ( rtDW
. lhuqmlzywi . Head == rtDW . lhuqmlzywi . Tail ) { if ( !
rt_TDelayUpdateTailOrGrowBuf ( & rtDW . lhuqmlzywi . CircularBufSize , & rtDW
. lhuqmlzywi . Tail , & rtDW . lhuqmlzywi . Head , & rtDW . lhuqmlzywi . Last
, simTime - rtP . VariableTransportDelay_MaxDelay_pwsg0j33ef , tBuffer ,
uBuffer , xBuffer , ( boolean_T ) 0 , ( boolean_T ) 1 , & rtDW . lhuqmlzywi .
MaxNewBufSize ) ) { ssSetErrorStatus ( rtS ,
"vtdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ rtDW .
lhuqmlzywi . Head ] = simTime ; ( * uBuffer ) [ rtDW . lhuqmlzywi . Head ] =
rtB . pbkcujkqmn ; ( * xBuffer ) [ rtDW . lhuqmlzywi . Head ] = rtX .
nrjwdf4bl5 ; } if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtDW . niklp3ydpx = rtB
. pqu4hljq1u ; } if ( rtDW . oxppwiig31 == ( rtInf ) ) { rtDW . oxppwiig31 =
ssGetTaskTime ( rtS , 0 ) ; rtDW . medpytok2l = rtB . ifd3zv0tlm ; } else if
( rtDW . idgf0xanwj == ( rtInf ) ) { rtDW . idgf0xanwj = ssGetTaskTime ( rtS
, 0 ) ; rtDW . kriwkpf2uu = rtB . ifd3zv0tlm ; } else if ( rtDW . oxppwiig31
< rtDW . idgf0xanwj ) { rtDW . oxppwiig31 = ssGetTaskTime ( rtS , 0 ) ; rtDW
. medpytok2l = rtB . ifd3zv0tlm ; } else { rtDW . idgf0xanwj = ssGetTaskTime
( rtS , 0 ) ; rtDW . kriwkpf2uu = rtB . ifd3zv0tlm ; } { real_T * * uBuffer =
( real_T * * ) & rtDW . p3evevjx4y . TUbufferPtrs [ 0 ] ; real_T * * tBuffer
= ( real_T * * ) & rtDW . p3evevjx4y . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . hhnmd3q34u . Head = ( ( rtDW . hhnmd3q34u . Head < (
rtDW . hhnmd3q34u . CircularBufSize - 1 ) ) ? ( rtDW . hhnmd3q34u . Head + 1
) : 0 ) ; if ( rtDW . hhnmd3q34u . Head == rtDW . hhnmd3q34u . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . hhnmd3q34u . CircularBufSize , &
rtDW . hhnmd3q34u . Tail , & rtDW . hhnmd3q34u . Head , & rtDW . hhnmd3q34u .
Last , simTime - rtP . TransportDelay_Delay_dy4stxs3xg , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . hhnmd3q34u . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . hhnmd3q34u . Head ] = simTime ; ( * uBuffer ) [ rtDW .
hhnmd3q34u . Head ] = rtB . iaa1jkuobs ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . f3ocqnmitg = rtB . kk2ddruedb ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . gei2va2yg1 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . gei2va2yg1 . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . kzmgrvlihj . Head = ( ( rtDW . kzmgrvlihj . Head < (
rtDW . kzmgrvlihj . CircularBufSize - 1 ) ) ? ( rtDW . kzmgrvlihj . Head + 1
) : 0 ) ; if ( rtDW . kzmgrvlihj . Head == rtDW . kzmgrvlihj . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . kzmgrvlihj . CircularBufSize , &
rtDW . kzmgrvlihj . Tail , & rtDW . kzmgrvlihj . Head , & rtDW . kzmgrvlihj .
Last , simTime - rtP . TransportDelay_Delay_bvh43vdfug , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . kzmgrvlihj . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . kzmgrvlihj . Head ] = simTime ; ( * uBuffer ) [ rtDW .
kzmgrvlihj . Head ] = rtB . fodvyk4sp5 ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . b1jopw25yj = rtB . clpub5lrjn ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . ofjbrmvngv . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . ofjbrmvngv . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . nftilndrdl . Head = ( ( rtDW . nftilndrdl . Head < (
rtDW . nftilndrdl . CircularBufSize - 1 ) ) ? ( rtDW . nftilndrdl . Head + 1
) : 0 ) ; if ( rtDW . nftilndrdl . Head == rtDW . nftilndrdl . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . nftilndrdl . CircularBufSize , &
rtDW . nftilndrdl . Tail , & rtDW . nftilndrdl . Head , & rtDW . nftilndrdl .
Last , simTime - rtP . TransportDelay_Delay_i0kl3kk3fa , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . nftilndrdl . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . nftilndrdl . Head ] = simTime ; ( * uBuffer ) [ rtDW .
nftilndrdl . Head ] = rtB . lqwcgolu0u ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . ewe0bkor5i = rtB . mpaaevtb0h ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . gfxye1rcxy . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . gfxye1rcxy . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . jl4zgwwouj . Head = ( ( rtDW . jl4zgwwouj . Head < (
rtDW . jl4zgwwouj . CircularBufSize - 1 ) ) ? ( rtDW . jl4zgwwouj . Head + 1
) : 0 ) ; if ( rtDW . jl4zgwwouj . Head == rtDW . jl4zgwwouj . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . jl4zgwwouj . CircularBufSize , &
rtDW . jl4zgwwouj . Tail , & rtDW . jl4zgwwouj . Head , & rtDW . jl4zgwwouj .
Last , simTime - rtP . TransportDelay_Delay_i1bwpxgdon , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . jl4zgwwouj . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . jl4zgwwouj . Head ] = simTime ; ( * uBuffer ) [ rtDW .
jl4zgwwouj . Head ] = rtB . byucczfrd5 ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . l5dlababv1 = rtB . m3qxkgrymt ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . abjlexkzmf . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . abjlexkzmf . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . ntkdqjpfyg . Head = ( ( rtDW . ntkdqjpfyg . Head < (
rtDW . ntkdqjpfyg . CircularBufSize - 1 ) ) ? ( rtDW . ntkdqjpfyg . Head + 1
) : 0 ) ; if ( rtDW . ntkdqjpfyg . Head == rtDW . ntkdqjpfyg . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . ntkdqjpfyg . CircularBufSize , &
rtDW . ntkdqjpfyg . Tail , & rtDW . ntkdqjpfyg . Head , & rtDW . ntkdqjpfyg .
Last , simTime - rtP . TransportDelay_Delay_eocvd5lrwh , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . ntkdqjpfyg . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . ntkdqjpfyg . Head ] = simTime ; ( * uBuffer ) [ rtDW .
ntkdqjpfyg . Head ] = rtB . gznbijk505 ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . b3csr0smh2 = rtB . b1nghd5jfn ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . ggw2hbirki . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . ggw2hbirki . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . krie5kar0s . Head = ( ( rtDW . krie5kar0s . Head < (
rtDW . krie5kar0s . CircularBufSize - 1 ) ) ? ( rtDW . krie5kar0s . Head + 1
) : 0 ) ; if ( rtDW . krie5kar0s . Head == rtDW . krie5kar0s . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . krie5kar0s . CircularBufSize , &
rtDW . krie5kar0s . Tail , & rtDW . krie5kar0s . Head , & rtDW . krie5kar0s .
Last , simTime - rtP . TransportDelay_Delay_l0yi3guf4c , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . krie5kar0s . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . krie5kar0s . Head ] = simTime ; ( * uBuffer ) [ rtDW .
krie5kar0s . Head ] = rtB . imfsawafbw ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . gdgisolefd = rtB . hegy2zggyj ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . ocixwzdlgq . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . ocixwzdlgq . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . fcxoxl4xkw . Head = ( ( rtDW . fcxoxl4xkw . Head < (
rtDW . fcxoxl4xkw . CircularBufSize - 1 ) ) ? ( rtDW . fcxoxl4xkw . Head + 1
) : 0 ) ; if ( rtDW . fcxoxl4xkw . Head == rtDW . fcxoxl4xkw . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . fcxoxl4xkw . CircularBufSize , &
rtDW . fcxoxl4xkw . Tail , & rtDW . fcxoxl4xkw . Head , & rtDW . fcxoxl4xkw .
Last , simTime - rtP . TransportDelay_Delay_klfp210l4z , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . fcxoxl4xkw . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . fcxoxl4xkw . Head ] = simTime ; ( * uBuffer ) [ rtDW .
fcxoxl4xkw . Head ] = rtB . b1c3s0yd1z ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . fecdn2rq0x = rtB . khrnej05te ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . j24stv55k5 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . j24stv55k5 . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . bzqpi0kjrz . Head = ( ( rtDW . bzqpi0kjrz . Head < (
rtDW . bzqpi0kjrz . CircularBufSize - 1 ) ) ? ( rtDW . bzqpi0kjrz . Head + 1
) : 0 ) ; if ( rtDW . bzqpi0kjrz . Head == rtDW . bzqpi0kjrz . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . bzqpi0kjrz . CircularBufSize , &
rtDW . bzqpi0kjrz . Tail , & rtDW . bzqpi0kjrz . Head , & rtDW . bzqpi0kjrz .
Last , simTime - rtP . TransportDelay_Delay_kds3wvorrd , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . bzqpi0kjrz . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . bzqpi0kjrz . Head ] = simTime ; ( * uBuffer ) [ rtDW .
bzqpi0kjrz . Head ] = rtB . nvxcpentgh ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . am13fzyssx = rtB . lhj0rg1wlh ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . hoiximvd01 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . hoiximvd01 . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . p5bf4c2tu4 . Head = ( ( rtDW . p5bf4c2tu4 . Head < (
rtDW . p5bf4c2tu4 . CircularBufSize - 1 ) ) ? ( rtDW . p5bf4c2tu4 . Head + 1
) : 0 ) ; if ( rtDW . p5bf4c2tu4 . Head == rtDW . p5bf4c2tu4 . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . p5bf4c2tu4 . CircularBufSize , &
rtDW . p5bf4c2tu4 . Tail , & rtDW . p5bf4c2tu4 . Head , & rtDW . p5bf4c2tu4 .
Last , simTime - rtP . TransportDelay_Delay_hpj5c34w2y , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . p5bf4c2tu4 . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . p5bf4c2tu4 . Head ] = simTime ; ( * uBuffer ) [ rtDW .
p5bf4c2tu4 . Head ] = rtB . i24kygikwy ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . amun1qc2sg = rtB . c4z4t2mhqf ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . gipv43m3ie . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . gipv43m3ie . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . ekzddpzsi1 . Head = ( ( rtDW . ekzddpzsi1 . Head < (
rtDW . ekzddpzsi1 . CircularBufSize - 1 ) ) ? ( rtDW . ekzddpzsi1 . Head + 1
) : 0 ) ; if ( rtDW . ekzddpzsi1 . Head == rtDW . ekzddpzsi1 . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . ekzddpzsi1 . CircularBufSize , &
rtDW . ekzddpzsi1 . Tail , & rtDW . ekzddpzsi1 . Head , & rtDW . ekzddpzsi1 .
Last , simTime - rtP . TransportDelay_Delay_kekk22xqvx , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . ekzddpzsi1 . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . ekzddpzsi1 . Head ] = simTime ; ( * uBuffer ) [ rtDW .
ekzddpzsi1 . Head ] = rtB . h4utvvvw0w ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . dk54ih4kpw = rtB . ekpfykko11 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . n450lruhp3 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . n450lruhp3 . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . hz4fywtkdv . Head = ( ( rtDW . hz4fywtkdv . Head < (
rtDW . hz4fywtkdv . CircularBufSize - 1 ) ) ? ( rtDW . hz4fywtkdv . Head + 1
) : 0 ) ; if ( rtDW . hz4fywtkdv . Head == rtDW . hz4fywtkdv . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . hz4fywtkdv . CircularBufSize , &
rtDW . hz4fywtkdv . Tail , & rtDW . hz4fywtkdv . Head , & rtDW . hz4fywtkdv .
Last , simTime - rtP . TransportDelay_Delay_jartwkytza , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . hz4fywtkdv . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . hz4fywtkdv . Head ] = simTime ; ( * uBuffer ) [ rtDW .
hz4fywtkdv . Head ] = rtB . kwhlwb5uqn ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . loaprl0bph = rtB . oiykawjblz ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . ptmzalvxqw . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . ptmzalvxqw . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . clbwz4dwil . Head = ( ( rtDW . clbwz4dwil . Head < (
rtDW . clbwz4dwil . CircularBufSize - 1 ) ) ? ( rtDW . clbwz4dwil . Head + 1
) : 0 ) ; if ( rtDW . clbwz4dwil . Head == rtDW . clbwz4dwil . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . clbwz4dwil . CircularBufSize , &
rtDW . clbwz4dwil . Tail , & rtDW . clbwz4dwil . Head , & rtDW . clbwz4dwil .
Last , simTime - rtP . TransportDelay_Delay_nc4nuquf3r , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . clbwz4dwil . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . clbwz4dwil . Head ] = simTime ; ( * uBuffer ) [ rtDW .
clbwz4dwil . Head ] = rtB . pdx0x2ahct ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . fo5lr1tooi = rtB . hatthunapb ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . akp2w04qvg . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . akp2w04qvg . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . bj3l3j2st3 . Head = ( ( rtDW . bj3l3j2st3 . Head < (
rtDW . bj3l3j2st3 . CircularBufSize - 1 ) ) ? ( rtDW . bj3l3j2st3 . Head + 1
) : 0 ) ; if ( rtDW . bj3l3j2st3 . Head == rtDW . bj3l3j2st3 . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . bj3l3j2st3 . CircularBufSize , &
rtDW . bj3l3j2st3 . Tail , & rtDW . bj3l3j2st3 . Head , & rtDW . bj3l3j2st3 .
Last , simTime - rtP . TransportDelay_Delay_dunuuw4awi , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . bj3l3j2st3 . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . bj3l3j2st3 . Head ] = simTime ; ( * uBuffer ) [ rtDW .
bj3l3j2st3 . Head ] = rtB . nxn02am0kd ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . g0siwihohw = rtB . mv5v4tmemn ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . n50ive3hhm . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . n50ive3hhm . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . fdisg0vnhk . Head = ( ( rtDW . fdisg0vnhk . Head < (
rtDW . fdisg0vnhk . CircularBufSize - 1 ) ) ? ( rtDW . fdisg0vnhk . Head + 1
) : 0 ) ; if ( rtDW . fdisg0vnhk . Head == rtDW . fdisg0vnhk . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . fdisg0vnhk . CircularBufSize , &
rtDW . fdisg0vnhk . Tail , & rtDW . fdisg0vnhk . Head , & rtDW . fdisg0vnhk .
Last , simTime - rtP . TransportDelay_Delay_eaoxoc1w3r , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . fdisg0vnhk . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . fdisg0vnhk . Head ] = simTime ; ( * uBuffer ) [ rtDW .
fdisg0vnhk . Head ] = rtB . nyigkdebdw ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . atub5wqmcj = rtB . nercvrdumc ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . ej00tpn0xt . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . ej00tpn0xt . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . nvxugce5ur . Head = ( ( rtDW . nvxugce5ur . Head < (
rtDW . nvxugce5ur . CircularBufSize - 1 ) ) ? ( rtDW . nvxugce5ur . Head + 1
) : 0 ) ; if ( rtDW . nvxugce5ur . Head == rtDW . nvxugce5ur . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . nvxugce5ur . CircularBufSize , &
rtDW . nvxugce5ur . Tail , & rtDW . nvxugce5ur . Head , & rtDW . nvxugce5ur .
Last , simTime - rtP . TransportDelay_Delay_ewi0elycaa , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . nvxugce5ur . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . nvxugce5ur . Head ] = simTime ; ( * uBuffer ) [ rtDW .
nvxugce5ur . Head ] = rtB . bmvqo4bjiq ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . kzk400d43v = rtB . fhgutxj4v5 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . ilhkq3otms . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . ilhkq3otms . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . lufsk3t3r2 . Head = ( ( rtDW . lufsk3t3r2 . Head < (
rtDW . lufsk3t3r2 . CircularBufSize - 1 ) ) ? ( rtDW . lufsk3t3r2 . Head + 1
) : 0 ) ; if ( rtDW . lufsk3t3r2 . Head == rtDW . lufsk3t3r2 . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . lufsk3t3r2 . CircularBufSize , &
rtDW . lufsk3t3r2 . Tail , & rtDW . lufsk3t3r2 . Head , & rtDW . lufsk3t3r2 .
Last , simTime - rtP . TransportDelay_Delay_kexin5mfwd , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . lufsk3t3r2 . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . lufsk3t3r2 . Head ] = simTime ; ( * uBuffer ) [ rtDW .
lufsk3t3r2 . Head ] = rtB . plbwcbylcg ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . itgyxe4zdv = rtB . pkwpszftqs ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . gvpng3pvrl . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . gvpng3pvrl . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . p1vhfh3p1a . Head = ( ( rtDW . p1vhfh3p1a . Head < (
rtDW . p1vhfh3p1a . CircularBufSize - 1 ) ) ? ( rtDW . p1vhfh3p1a . Head + 1
) : 0 ) ; if ( rtDW . p1vhfh3p1a . Head == rtDW . p1vhfh3p1a . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . p1vhfh3p1a . CircularBufSize , &
rtDW . p1vhfh3p1a . Tail , & rtDW . p1vhfh3p1a . Head , & rtDW . p1vhfh3p1a .
Last , simTime - rtP . TransportDelay_Delay_gps5uogsr2 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . p1vhfh3p1a . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . p1vhfh3p1a . Head ] = simTime ; ( * uBuffer ) [ rtDW .
p1vhfh3p1a . Head ] = rtB . mlxeaid3dj ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . jnixgbeum0 = rtB . dgjeksbyyv ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . cfwoq0f2ti . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . cfwoq0f2ti . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . h3jb1xttev . Head = ( ( rtDW . h3jb1xttev . Head < (
rtDW . h3jb1xttev . CircularBufSize - 1 ) ) ? ( rtDW . h3jb1xttev . Head + 1
) : 0 ) ; if ( rtDW . h3jb1xttev . Head == rtDW . h3jb1xttev . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . h3jb1xttev . CircularBufSize , &
rtDW . h3jb1xttev . Tail , & rtDW . h3jb1xttev . Head , & rtDW . h3jb1xttev .
Last , simTime - rtP . TransportDelay_Delay_oc5ujwjout , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . h3jb1xttev . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . h3jb1xttev . Head ] = simTime ; ( * uBuffer ) [ rtDW .
h3jb1xttev . Head ] = rtB . g13nplvhwh ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . c2b1i5ixoq = rtB . hmdmab20r1 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . ecebamajhh . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . ecebamajhh . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . km2x20r4ek . Head = ( ( rtDW . km2x20r4ek . Head < (
rtDW . km2x20r4ek . CircularBufSize - 1 ) ) ? ( rtDW . km2x20r4ek . Head + 1
) : 0 ) ; if ( rtDW . km2x20r4ek . Head == rtDW . km2x20r4ek . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . km2x20r4ek . CircularBufSize , &
rtDW . km2x20r4ek . Tail , & rtDW . km2x20r4ek . Head , & rtDW . km2x20r4ek .
Last , simTime - rtP . TransportDelay_Delay_dywj1im3o2 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . km2x20r4ek . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . km2x20r4ek . Head ] = simTime ; ( * uBuffer ) [ rtDW .
km2x20r4ek . Head ] = rtB . ftjitujlmu ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . oumxtk4xvn = rtB . ngkpxovgoe ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . ogxech4wih . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . ogxech4wih . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . ddmxqb4sul . Head = ( ( rtDW . ddmxqb4sul . Head < (
rtDW . ddmxqb4sul . CircularBufSize - 1 ) ) ? ( rtDW . ddmxqb4sul . Head + 1
) : 0 ) ; if ( rtDW . ddmxqb4sul . Head == rtDW . ddmxqb4sul . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . ddmxqb4sul . CircularBufSize , &
rtDW . ddmxqb4sul . Tail , & rtDW . ddmxqb4sul . Head , & rtDW . ddmxqb4sul .
Last , simTime - rtP . TransportDelay_Delay_j5wvotkdow , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . ddmxqb4sul . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . ddmxqb4sul . Head ] = simTime ; ( * uBuffer ) [ rtDW .
ddmxqb4sul . Head ] = rtB . gz52grzsnh ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . cvewj2ww1t = rtB . kmr42jbgqs ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . j1jrbblzw5 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . j1jrbblzw5 . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . d11oepymjs . Head = ( ( rtDW . d11oepymjs . Head < (
rtDW . d11oepymjs . CircularBufSize - 1 ) ) ? ( rtDW . d11oepymjs . Head + 1
) : 0 ) ; if ( rtDW . d11oepymjs . Head == rtDW . d11oepymjs . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . d11oepymjs . CircularBufSize , &
rtDW . d11oepymjs . Tail , & rtDW . d11oepymjs . Head , & rtDW . d11oepymjs .
Last , simTime - rtP . TransportDelay_Delay_lqgrxsjeg1 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . d11oepymjs . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . d11oepymjs . Head ] = simTime ; ( * uBuffer ) [ rtDW .
d11oepymjs . Head ] = rtB . dmefqabr3w ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . gb2aoze2zq = rtB . pomuja1z5w ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . hkkzlg4m02 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . hkkzlg4m02 . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . e311nuuqb5 . Head = ( ( rtDW . e311nuuqb5 . Head < (
rtDW . e311nuuqb5 . CircularBufSize - 1 ) ) ? ( rtDW . e311nuuqb5 . Head + 1
) : 0 ) ; if ( rtDW . e311nuuqb5 . Head == rtDW . e311nuuqb5 . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . e311nuuqb5 . CircularBufSize , &
rtDW . e311nuuqb5 . Tail , & rtDW . e311nuuqb5 . Head , & rtDW . e311nuuqb5 .
Last , simTime - rtP . TransportDelay_Delay_hnythpuhce , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . e311nuuqb5 . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . e311nuuqb5 . Head ] = simTime ; ( * uBuffer ) [ rtDW .
e311nuuqb5 . Head ] = rtB . er1wxf0naj ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . fxksrxtwby = rtB . deg3xxymgh ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . hva2h0mh4k . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . hva2h0mh4k . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . g0lsoknhdx . Head = ( ( rtDW . g0lsoknhdx . Head < (
rtDW . g0lsoknhdx . CircularBufSize - 1 ) ) ? ( rtDW . g0lsoknhdx . Head + 1
) : 0 ) ; if ( rtDW . g0lsoknhdx . Head == rtDW . g0lsoknhdx . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . g0lsoknhdx . CircularBufSize , &
rtDW . g0lsoknhdx . Tail , & rtDW . g0lsoknhdx . Head , & rtDW . g0lsoknhdx .
Last , simTime - rtP . TransportDelay_Delay_hzbv3ih5sh , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . g0lsoknhdx . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . g0lsoknhdx . Head ] = simTime ; ( * uBuffer ) [ rtDW .
g0lsoknhdx . Head ] = rtB . lzyxcnmkb0 ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . pwd5paigtu = rtB . jwv42cyom4 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . crpy1zhjkz . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . crpy1zhjkz . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . j1sal1hher . Head = ( ( rtDW . j1sal1hher . Head < (
rtDW . j1sal1hher . CircularBufSize - 1 ) ) ? ( rtDW . j1sal1hher . Head + 1
) : 0 ) ; if ( rtDW . j1sal1hher . Head == rtDW . j1sal1hher . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . j1sal1hher . CircularBufSize , &
rtDW . j1sal1hher . Tail , & rtDW . j1sal1hher . Head , & rtDW . j1sal1hher .
Last , simTime - rtP . TransportDelay_Delay_l1fxkrpmnr , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . j1sal1hher . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . j1sal1hher . Head ] = simTime ; ( * uBuffer ) [ rtDW .
j1sal1hher . Head ] = rtB . ekyn2dmd41 ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . gaijbd3xgk = rtB . l0isenqrj3 ; rtDW . itxmk3chva = rtB .
ckw23s2vuu ; } lwcs3jh3k2 ( rtS , & rtB . jtynvhttl4 , & rtDW . jtynvhttl4 )
; ny353qchui ( rtS , & rtB . jdzdt1ap5b , & rtDW . jdzdt1ap5b ) ; lwcs3jh3k2
( rtS , & rtB . fvh2zdqaau , & rtDW . fvh2zdqaau ) ; ny353qchui ( rtS , & rtB
. f41hg5wei1 , & rtDW . f41hg5wei1 ) ; { real_T * * uBuffer = ( real_T * * )
& rtDW . k5ejkktore . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * *
) & rtDW . k5ejkktore . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS )
; rtDW . dfz14rvn5j . Head = ( ( rtDW . dfz14rvn5j . Head < ( rtDW .
dfz14rvn5j . CircularBufSize - 1 ) ) ? ( rtDW . dfz14rvn5j . Head + 1 ) : 0 )
; if ( rtDW . dfz14rvn5j . Head == rtDW . dfz14rvn5j . Tail ) { if ( !
rt_TDelayUpdateTailOrGrowBuf ( & rtDW . dfz14rvn5j . CircularBufSize , & rtDW
. dfz14rvn5j . Tail , & rtDW . dfz14rvn5j . Head , & rtDW . dfz14rvn5j . Last
, simTime - rtP . TransportDelay_Delay_nqpof1vzvj , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & rtDW . dfz14rvn5j . MaxNewBufSize ) ) {
ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } } (
* tBuffer ) [ rtDW . dfz14rvn5j . Head ] = simTime ; ( * uBuffer ) [ rtDW .
dfz14rvn5j . Head ] = rtB . fwvph3d1an ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . n1ijw4x0rn = rtB . mitrdmujtk ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . nvyaxvbcrw . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . nvyaxvbcrw . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . aic2pxcqxi . Head = ( ( rtDW . aic2pxcqxi . Head < (
rtDW . aic2pxcqxi . CircularBufSize - 1 ) ) ? ( rtDW . aic2pxcqxi . Head + 1
) : 0 ) ; if ( rtDW . aic2pxcqxi . Head == rtDW . aic2pxcqxi . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . aic2pxcqxi . CircularBufSize , &
rtDW . aic2pxcqxi . Tail , & rtDW . aic2pxcqxi . Head , & rtDW . aic2pxcqxi .
Last , simTime - rtP . TransportDelay_Delay_krge4c0xm3 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . aic2pxcqxi . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . aic2pxcqxi . Head ] = simTime ; ( * uBuffer ) [ rtDW .
aic2pxcqxi . Head ] = rtB . m1qmwbvls5 ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . ciujz12ohu = rtB . gfru2n0eh5 ; rtDW . huxwzweeyi = rtB .
nponljxli4 ; } lwcs3jh3k2 ( rtS , & rtB . dhpm1tyfmn , & rtDW . dhpm1tyfmn )
; ny353qchui ( rtS , & rtB . grby3szfkd , & rtDW . grby3szfkd ) ; lwcs3jh3k2
( rtS , & rtB . nvltfn14rj , & rtDW . nvltfn14rj ) ; ny353qchui ( rtS , & rtB
. dzcv3as4ol , & rtDW . dzcv3as4ol ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) {
rtDW . fwpmmk23o2 = rtB . fymmjzo24c ; } lwcs3jh3k2 ( rtS , & rtB .
l2r2uh0izk , & rtDW . l2r2uh0izk ) ; ny353qchui ( rtS , & rtB . feptx2ayjl ,
& rtDW . feptx2ayjl ) ; lwcs3jh3k2 ( rtS , & rtB . labvjdxfi3 , & rtDW .
labvjdxfi3 ) ; ny353qchui ( rtS , & rtB . nvnct41vnp , & rtDW . nvnct41vnp )
; UNUSED_PARAMETER ( tid ) ; } void MdlUpdateTID2 ( int_T tid ) {
UNUSED_PARAMETER ( tid ) ; } void MdlDerivatives ( void ) { boolean_T lsat ;
boolean_T usat ; XDot * _rtXdot ; _rtXdot = ( ( XDot * ) ssGetdX ( rtS ) ) ;
_rtXdot -> apevsa2hcq = rtB . m4sokjzatx ; _rtXdot -> ebz5npnaoh = rtB .
nupattijm1 ; _rtXdot -> g5pl2ihnsa = rtB . kgiixo3yn5 ; _rtXdot -> dzmtcwo21u
= rtB . lcu3nftu1f ; _rtXdot -> luwntsjfce = rtB . hnvusovzin ; _rtXdot ->
lrou1be5ws = rtB . kgap4iwtv4 ; _rtXdot -> kipcktetym = rtB . nvoqqf2upc ;
_rtXdot -> hogtze5t2d = rtB . lhomssayy1 ; _rtXdot -> aubyardb3x = rtB .
cnrayaptfm ; _rtXdot -> hummpur2b0 = rtB . kxzvknusul ; _rtXdot -> j0rirqtd1a
= rtB . bt5cg0l4bg ; _rtXdot -> ip0opk45xq = rtB . jyjlz3qjng ; _rtXdot ->
cyudzpjr5t = rtB . ejz1w0mtlj ; _rtXdot -> ky01z2j0r2 = rtB . hxcjlgtiv4 ;
_rtXdot -> nyvh4po1ze = rtB . mb3ddccayl ; _rtXdot -> fsa5rzt3eu = rtB .
iqa3f5efos ; _rtXdot -> iivfgrp0dv = rtB . irurbxken0 ; _rtXdot -> etzwqxlfek
= rtB . clqrqgqr4k ; _rtXdot -> ikersjp5rv = rtB . ohyn2fvrkm ; _rtXdot ->
hgyqbdifk0 = rtB . gv1th4qcwu ; _rtXdot -> brjhw0iflo = rtB . dkctjqa3dq ;
_rtXdot -> kerqyn5qft = rtB . fir35h1or5 ; _rtXdot -> iytiwtpacb = rtB .
ga110lqx5z ; _rtXdot -> g0phs0m0dn = rtB . ie1qostli2 ; _rtXdot -> lka0ltzwzh
= rtB . bow54nzha3 ; _rtXdot -> pquw0pmaw2 = rtB . bcobobx5fo ; _rtXdot ->
jx303cpogv = rtB . aesfinetlj ; _rtXdot -> ltxxvmnz50 = rtB . kibaejq5te ;
_rtXdot -> b4ijv0yfdl = rtB . iiconzrnl4 ; _rtXdot -> inx2whmsvz = rtB .
hh35giszvi ; _rtXdot -> obmdbjtn3e = rtB . mufvugkq55 ; _rtXdot -> mczl1h5kue
= rtB . grm0i134jq ; _rtXdot -> b2kr0ggmvo = rtB . ptc5im5mxx ; _rtXdot ->
pdrwmhpgu2 = rtB . hjme1gtmfr ; _rtXdot -> ckadbag1nz = rtB . kznsejiloq ;
_rtXdot -> kfqga2xqnc = rtB . e40bwzvglm ; _rtXdot -> ovnjy3004d = rtB .
hc2o4lmxgu ; _rtXdot -> koitoczsgs = rtB . lxhpidiqog ; _rtXdot -> fwvdemyybn
= rtB . gy3dadahiv ; _rtXdot -> df1cmyigny = rtB . p3s31yxgop ; _rtXdot ->
eiq3cpodg4 = rtB . awjpq3b3zw ; _rtXdot -> lujnb1vhch = rtB . pkycymvyje ;
_rtXdot -> ivshzrdzao = rtB . ith2vi2lkm ; _rtXdot -> b0se24ah4v = rtB .
bialu2hh5u ; _rtXdot -> dzlchxjgwf = rtB . fykapa13hu ; _rtXdot -> kxjviilbiq
= rtB . kpzv0mtsrl ; _rtXdot -> kzm5j4om1j = rtB . b0tbnfmob2 ; _rtXdot ->
hrkjiy2wbe = rtB . fohd0pwzvp ; _rtXdot -> bbxcrevmf2 = rtB . f4bmazlri0 ;
_rtXdot -> mecmfgexic = rtB . b2davm33jr ; { real_T instantDelay ;
instantDelay = rtB . lc23kplapx ; if ( instantDelay > rtP .
VariableTransportDelay_MaxDelay_baatiozaev ) { instantDelay = rtP .
VariableTransportDelay_MaxDelay_baatiozaev ; } if ( instantDelay < 0.0 ) { (
( XDot * ) ssGetdX ( rtS ) ) -> gukucggul0 = 0 ; } else { ( ( XDot * )
ssGetdX ( rtS ) ) -> gukucggul0 = 1.0 / instantDelay ; } } { real_T
instantDelay ; instantDelay = rtB . erpywr2s5s ; if ( instantDelay > rtP .
VariableTransportDelay_MaxDelay_ml14mdwepr ) { instantDelay = rtP .
VariableTransportDelay_MaxDelay_ml14mdwepr ; } if ( instantDelay < 0.0 ) { (
( XDot * ) ssGetdX ( rtS ) ) -> cooihtzhhj = 0 ; } else { ( ( XDot * )
ssGetdX ( rtS ) ) -> cooihtzhhj = 1.0 / instantDelay ; } } lsat = ( rtX .
hbdbwheonc <= rtP . Integrator_LowerSat ) ; usat = ( rtX . hbdbwheonc >= rtP
. Integrator_UpperSat ) ; if ( ( ( ! lsat ) && ( ! usat ) ) || ( lsat && (
rtB . aq0oxjnh5o > 0.0 ) ) || ( usat && ( rtB . aq0oxjnh5o < 0.0 ) ) ) {
_rtXdot -> hbdbwheonc = rtB . aq0oxjnh5o ; } else { _rtXdot -> hbdbwheonc =
0.0 ; } if ( rtDW . br5wyeghdu ) { { real_T instantDelay ; instantDelay = rtB
. gl2tdboxeo ; if ( instantDelay > rtP . VariableTransportDelay_MaxDelay ) {
instantDelay = rtP . VariableTransportDelay_MaxDelay ; } if ( instantDelay <
0.0 ) { ( ( XDot * ) ssGetdX ( rtS ) ) -> gfghljc4gn = 0 ; } else { ( ( XDot
* ) ssGetdX ( rtS ) ) -> gfghljc4gn = 1.0 / instantDelay ; } } _rtXdot ->
cgbqhiyy4k = rtB . guosjh35iw [ 0 ] ; { real_T instantDelay ; instantDelay =
rtB . aayb2fnbde ; if ( instantDelay > rtP .
VariableTransportDelay_MaxDelay_du1vxketzx ) { instantDelay = rtP .
VariableTransportDelay_MaxDelay_du1vxketzx ; } if ( instantDelay < 0.0 ) { (
( XDot * ) ssGetdX ( rtS ) ) -> nhazmrtgmb = 0 ; } else { ( ( XDot * )
ssGetdX ( rtS ) ) -> nhazmrtgmb = 1.0 / instantDelay ; } } _rtXdot ->
pjrskjmpvo = rtB . guosjh35iw [ 1 ] ; } else { { real_T * dx ; int_T i ; dx =
& ( ( ( XDot * ) ssGetdX ( rtS ) ) -> gfghljc4gn ) ; for ( i = 0 ; i < 4 ; i
++ ) { dx [ i ] = 0.0 ; } } } lsat = ( rtX . oipozxyfbt <= rtP .
Integrator_LowerSat_i4hvpryy1p ) ; usat = ( rtX . oipozxyfbt >= rtP .
Integrator_UpperSat_hkcvjiz0hz ) ; if ( ( ( ! lsat ) && ( ! usat ) ) || (
lsat && ( rtB . l11wobikak > 0.0 ) ) || ( usat && ( rtB . l11wobikak < 0.0 )
) ) { _rtXdot -> oipozxyfbt = rtB . l11wobikak ; } else { _rtXdot ->
oipozxyfbt = 0.0 ; } { real_T instantDelay ; instantDelay = rtB . dyp34geds0
; if ( instantDelay > rtP . VariableTransportDelay_MaxDelay_pwsg0j33ef ) {
instantDelay = rtP . VariableTransportDelay_MaxDelay_pwsg0j33ef ; } if (
instantDelay < 0.0 ) { ( ( XDot * ) ssGetdX ( rtS ) ) -> nrjwdf4bl5 = 0 ; }
else { ( ( XDot * ) ssGetdX ( rtS ) ) -> nrjwdf4bl5 = 1.0 / instantDelay ; }
} _rtXdot -> d543hn3jm3 = rtB . jbrvxtovno [ 1 ] ; _rtXdot -> ktq1qocrqs =
0.0 ; _rtXdot -> ktq1qocrqs += rtP . TransferFcn_A * rtX . ktq1qocrqs ;
_rtXdot -> ktq1qocrqs += rtB . d2ggya3nxd ; _rtXdot -> ih0sh50oxm = rtB .
frpbg1j3eo ; _rtXdot -> pn05gdwhsv = rtB . ajbsi41ckn ; _rtXdot -> jzyjyrpy3w
= rtB . f3z3rdoc5x ; _rtXdot -> ehyodyswyu = rtB . m0niy4vc14 ; _rtXdot ->
hyqpl142bi = rtB . n3lyabdxz3 ; _rtXdot -> prno02pbsy = rtB . p2kd0bqbmj ;
_rtXdot -> aztsfsn2hi = rtB . fobm5wf0a2 ; _rtXdot -> f04g4otdzz = rtB .
fpnuiuit1d ; _rtXdot -> ouay2vk4ob = rtB . l1mnh3vw14 ; _rtXdot -> l4w5fqeack
= rtB . ge0vbj12o1 ; _rtXdot -> dbk3vhy4fa = rtB . pfmt42o2gw ; _rtXdot ->
bwyxfbkejz = rtB . dpeoqqz1p4 ; _rtXdot -> cvhc14d1wd = rtB . l4rs54j5xg ;
_rtXdot -> ipmtswi0si = rtB . b34qhbnabh ; _rtXdot -> eczymfqr20 = rtB .
gtajd15451 ; _rtXdot -> lobhhamyoj = rtB . m5wvrgyai1 ; _rtXdot -> kenbvb3y0s
= rtB . hghbeybdaf ; _rtXdot -> nsp4pw5vpk = rtB . gql2g0whz1 ; _rtXdot ->
prtdiw223v = rtB . ba2brgvdad ; _rtXdot -> bots1ppmcj = rtB . b5vrpo3tf1 ;
_rtXdot -> n0uge3m4yf = rtB . p3wccudzxi ; _rtXdot -> gnzuxd2raf = rtB .
onvggmtysa ; _rtXdot -> bqo41zgbir = rtB . a0uzpwsi3s ; _rtXdot -> kiwleabf04
= rtB . ojyy3nnssd ; _rtXdot -> aa5y12cea2 = rtB . lpgyvrpws0 ; _rtXdot ->
cylrltmkae = rtB . ffbwz4di24 ; _rtXdot -> ar5lw0nd1g = rtB . ihpo1wu1st ;
_rtXdot -> etqlz3rohc = rtB . pdkipll4oh ; } void MdlProjection ( void ) { }
void MdlTerminate ( void ) { rt_TDelayFreeBuf ( rtDW . gxot3lz5fc .
TUbufferPtrs [ 0 ] ) ; rt_TDelayFreeBuf ( rtDW . cay5bhojbk . TUbufferPtrs [
0 ] ) ; { free ( rtDW . orezesugg1 . DS ) ; free ( rtDW . orezesugg1 . DX_COL
) ; free ( rtDW . orezesugg1 . TMP2 ) ; free ( rtDW . orezesugg1 . G_STATE )
; free ( rtDW . orezesugg1 . SWITCH_STATUS ) ; free ( rtDW . orezesugg1 .
SW_CHG ) ; free ( rtDW . orezesugg1 . SWITCH_STATUS_INIT ) ; } { free ( rtDW
. g45z4iebyu . AS ) ; free ( rtDW . g45z4iebyu . BS ) ; free ( rtDW .
g45z4iebyu . CS ) ; free ( rtDW . g45z4iebyu . DS ) ; free ( rtDW .
g45z4iebyu . DX_COL ) ; free ( rtDW . g45z4iebyu . TMP2 ) ; free ( rtDW .
g45z4iebyu . BD_COL ) ; free ( rtDW . g45z4iebyu . TMP1 ) ; free ( rtDW .
g45z4iebyu . XTMP ) ; free ( rtDW . g45z4iebyu . G_STATE ) ; free ( rtDW .
g45z4iebyu . SWITCH_STATUS ) ; free ( rtDW . g45z4iebyu . SW_CHG ) ; free (
rtDW . g45z4iebyu . SWITCH_STATUS_INIT ) ; } rt_TDelayFreeBuf ( rtDW .
grt2oyi0sr . TUbufferPtrs [ 0 ] ) ; rt_TDelayFreeBuf ( rtDW . pl0qrajpw4 .
TUbufferPtrs [ 0 ] ) ; rt_TDelayFreeBuf ( rtDW . ax1z1sifrs . TUbufferPtrs [
0 ] ) ; rt_TDelayFreeBuf ( rtDW . d5b3ghqdjb . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . d45fc0cu2n . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . c5rfy4mzmq . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . lmfxi4smjj . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . d3lf52anqx . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . gvogk5wosq . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . innrerrg2w . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . b3tfxdcqfh . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . ex4axikx2n . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . f5mcje12pk . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . oi1yc2ajy5 . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . c2kuyduabk . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . exexriqgpr . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . k0yj2b2ldz . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . lbtex4xafe . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . dtlmsjjjpu . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . l30jrhdejr . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . hh24ilp1p1 . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . djthquopua . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . faad0u1b4n . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . k4bhgqdyhg . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . kx2dm0gq0l . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . milmx1vwl5 . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . e1qtan4tfe . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . nsvuvsgsn0 . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . e2vylf2hfx . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . bflyns0cbr . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . pxt2zbirdg . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . fllltsfpxy . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . afb2vzbxaa . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . fz41nona4h . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . hqvoz0gebt . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . pvl2ejzk4c . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . pzbmlqlvpm . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . h0w4jpcxu0 . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . bdtaclh4hr . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . hwwk0yws23 . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . eosdk1pxwr . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . hdva23qpbu . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . kieb4vqgp3 . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . oewxpxyjnd . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . b31e0rqxiq . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . nhiqsehhpt . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . lcvmbgrgk0 . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . eogglflt5u . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . kp5tjch01i . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . gfqptvfbp1 . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . l0gmb53kdd . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . i3r0m4gxn0 . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . bizzathsjz . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . p3evevjx4y . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . gei2va2yg1 . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . ofjbrmvngv . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . gfxye1rcxy . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . abjlexkzmf . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . ggw2hbirki . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . ocixwzdlgq . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . j24stv55k5 . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . hoiximvd01 . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . gipv43m3ie . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . n450lruhp3 . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . ptmzalvxqw . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . akp2w04qvg . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . n50ive3hhm . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . ej00tpn0xt . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . ilhkq3otms . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . gvpng3pvrl . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . cfwoq0f2ti . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . ecebamajhh . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . ogxech4wih . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . j1jrbblzw5 . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . hkkzlg4m02 . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . hva2h0mh4k . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . crpy1zhjkz . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . k5ejkktore . TUbufferPtrs [ 0 ] ) ;
rt_TDelayFreeBuf ( rtDW . nvyaxvbcrw . TUbufferPtrs [ 0 ] ) ; { if ( rtDW .
kuceggnsk2 . AQHandles ) { sdiTerminateStreaming ( & rtDW . kuceggnsk2 .
AQHandles ) ; } if ( rtDW . kuceggnsk2 . SlioLTF ) {
rtwDestructAccessorPointer ( rtDW . kuceggnsk2 . SlioLTF ) ; } } if (
rt_slioCatalogue ( ) != ( NULL ) ) { void * * slioCatalogueAddr =
rt_slioCatalogueAddr ( ) ; rtwSaveDatasetsToMatFile (
rtwGetPointerFromUniquePtr ( rt_slioCatalogue ( ) ) ,
rt_GetMatSigstreamLoggingFileName ( ) ) ; rtwTerminateSlioCatalogue (
slioCatalogueAddr ) ; * slioCatalogueAddr = NULL ; } } void
MdlInitializeSizes ( void ) { ssSetNumContStates ( rtS , 89 ) ;
ssSetNumPeriodicContStates ( rtS , 0 ) ; ssSetNumY ( rtS , 0 ) ; ssSetNumU (
rtS , 0 ) ; ssSetDirectFeedThrough ( rtS , 0 ) ; ssSetNumSampleTimes ( rtS ,
2 ) ; ssSetNumBlocks ( rtS , 9221 ) ; ssSetNumBlockIO ( rtS , 9730 ) ;
ssSetNumBlockParams ( rtS , 5832 ) ; } void MdlInitializeSampleTimes ( void )
{ ssSetSampleTime ( rtS , 0 , 0.0 ) ; ssSetSampleTime ( rtS , 1 , 5.0E-5 ) ;
ssSetOffsetTime ( rtS , 0 , 0.0 ) ; ssSetOffsetTime ( rtS , 1 , 0.0 ) ; }
void raccel_set_checksum ( ) { ssSetChecksumVal ( rtS , 0 , 3592701390U ) ;
ssSetChecksumVal ( rtS , 1 , 3673746601U ) ; ssSetChecksumVal ( rtS , 2 ,
3157903671U ) ; ssSetChecksumVal ( rtS , 3 , 808023130U ) ; }
#if defined(_MSC_VER)
#pragma optimize( "", off )
#endif
SimStruct * raccel_register_model ( void ) { static struct _ssMdlInfo mdlInfo
; ( void ) memset ( ( char * ) rtS , 0 , sizeof ( SimStruct ) ) ; ( void )
memset ( ( char * ) & mdlInfo , 0 , sizeof ( struct _ssMdlInfo ) ) ;
ssSetMdlInfoPtr ( rtS , & mdlInfo ) ; { static time_T mdlPeriod [
NSAMPLE_TIMES ] ; static time_T mdlOffset [ NSAMPLE_TIMES ] ; static time_T
mdlTaskTimes [ NSAMPLE_TIMES ] ; static int_T mdlTsMap [ NSAMPLE_TIMES ] ;
static int_T mdlSampleHits [ NSAMPLE_TIMES ] ; static boolean_T
mdlTNextWasAdjustedPtr [ NSAMPLE_TIMES ] ; static int_T mdlPerTaskSampleHits
[ NSAMPLE_TIMES * NSAMPLE_TIMES ] ; static time_T mdlTimeOfNextSampleHit [
NSAMPLE_TIMES ] ; { int_T i ; for ( i = 0 ; i < NSAMPLE_TIMES ; i ++ ) {
mdlPeriod [ i ] = 0.0 ; mdlOffset [ i ] = 0.0 ; mdlTaskTimes [ i ] = 0.0 ;
mdlTsMap [ i ] = i ; mdlSampleHits [ i ] = 1 ; } } ssSetSampleTimePtr ( rtS ,
& mdlPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rtS , & mdlOffset [ 0 ] ) ;
ssSetSampleTimeTaskIDPtr ( rtS , & mdlTsMap [ 0 ] ) ; ssSetTPtr ( rtS , &
mdlTaskTimes [ 0 ] ) ; ssSetSampleHitPtr ( rtS , & mdlSampleHits [ 0 ] ) ;
ssSetTNextWasAdjustedPtr ( rtS , & mdlTNextWasAdjustedPtr [ 0 ] ) ;
ssSetPerTaskSampleHitsPtr ( rtS , & mdlPerTaskSampleHits [ 0 ] ) ;
ssSetTimeOfNextSampleHitPtr ( rtS , & mdlTimeOfNextSampleHit [ 0 ] ) ; }
ssSetSolverMode ( rtS , SOLVER_MODE_SINGLETASKING ) ; { ssSetBlockIO ( rtS ,
( ( void * ) & rtB ) ) ; ( void ) memset ( ( ( void * ) & rtB ) , 0 , sizeof
( B ) ) ; } { real_T * x = ( real_T * ) & rtX ; ssSetContStates ( rtS , x ) ;
( void ) memset ( ( void * ) x , 0 , sizeof ( X ) ) ; } { void * dwork = (
void * ) & rtDW ; ssSetRootDWork ( rtS , dwork ) ; ( void ) memset ( dwork ,
0 , sizeof ( DW ) ) ; } { static DataTypeTransInfo dtInfo ; ( void ) memset (
( char_T * ) & dtInfo , 0 , sizeof ( dtInfo ) ) ; ssSetModelMappingInfo ( rtS
, & dtInfo ) ; dtInfo . numDataTypes = 23 ; dtInfo . dataTypeSizes = &
rtDataTypeSizes [ 0 ] ; dtInfo . dataTypeNames = & rtDataTypeNames [ 0 ] ;
dtInfo . BTransTable = & rtBTransTable ; dtInfo . PTransTable = &
rtPTransTable ; dtInfo . dataTypeInfoTable = rtDataTypeInfoTable ; }
DZG_v3_InitializeDataMapInfo ( ) ; ssSetIsRapidAcceleratorActive ( rtS , true
) ; ssSetRootSS ( rtS , rtS ) ; ssSetVersion ( rtS , SIMSTRUCT_VERSION_LEVEL2
) ; ssSetModelName ( rtS , "DZG_v3" ) ; ssSetPath ( rtS , "DZG_v3" ) ;
ssSetTStart ( rtS , 0.0 ) ; ssSetTFinal ( rtS , 3.0 ) ; ssSetStepSize ( rtS ,
5.0E-5 ) ; ssSetFixedStepSize ( rtS , 5.0E-5 ) ;
ssSetSolverExtrapolationOrder ( rtS , 4 ) ; ssSetSolverNumberNewtonIterations
( rtS , 1 ) ; { static RTWLogInfo rt_DataLoggingInfo ; rt_DataLoggingInfo .
loggingInterval = NULL ; ssSetRTWLogInfo ( rtS , & rt_DataLoggingInfo ) ; } {
{ static int_T rt_LoggedStateWidths [ ] = { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
, 1 , 1 , 6 } ; static int_T rt_LoggedStateNumDimensions [ ] = { 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 } ; static int_T rt_LoggedStateDimensions [
] = { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 6 } ; static boolean_T
rt_LoggedStateIsVarDims [ ] = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
} ; static BuiltInDTypeId rt_LoggedStateDataTypeIds [ ] = { SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE } ; static int_T
rt_LoggedStateComplexSignals [ ] = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 } ; static RTWPreprocessingFcnPtr rt_LoggingStatePreprocessingFcnPtrs [
] = { ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , (
NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , (
NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , (
NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , (
NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , (
NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , (
NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , (
NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , (
NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , (
NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , (
NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , (
NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , (
NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) } ;
static const char_T * rt_LoggedStateLabels [ ] = { "CSTATE" , "CSTATE" ,
"CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" ,
"CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" ,
"CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" ,
"CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" ,
"CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" ,
"CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" ,
"CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" ,
"CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" ,
"CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" ,
"CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" ,
"CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" ,
"CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" ,
"CSTATE" , "CSTATE" , "CSTATE" , "DSTATE" } ; static const char_T *
rt_LoggedStateBlockNames [ ] = {
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Ua/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Ua/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Ub/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Ub/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Uc/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Uc/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Ia/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Ia/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Ib/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Ib/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Ic/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Ic/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� UN/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� UN/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Ua3/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Ua3/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Ub3/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Ub3/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Uc3/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Uc3/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� UN3/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� UN3/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Ua2/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Ua2/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Ub2/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Ub2/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Uc2/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/������ � ���� (���)/����� Uc2/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� Ua/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� Ua/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� Ub/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� Ub/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� Uc/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� Uc/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� Ia/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� Ia/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� Ib/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� Ib/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� Ic/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� Ic/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� UN/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� UN/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� Ua3/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� Ua3/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� Ub3/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� Ub3/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� Uc3/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� Uc3/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� UN3/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/������ � ���� (���)/����� UN3/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/st_inverse/in_1/Variable\nTransport Delay"
,
"DZG_v3/System/�������_���-01\n���������/st_inverse/Variable\nTransport Delay"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/PLL\n(3ph)/Model/Integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/PLL\n(3ph)/Model/Continuous/Integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/PLL\n(3ph)/Model/Variable Frequency\nMean value/Model/Variable\nTransport Delay"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/PLL\n(3ph)/Model/Variable Frequency\nMean value/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/PLL\n(3ph)/Model/Continuous/Transfer Fcn"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/PLL\n(3ph)/Model/Second-Order\nFilter/Model/Integrator_x1"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/PLL\n(3ph)/Model/Second-Order\nFilter/Model/Integrator_x2"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/���. ������������/IabcCalc/Fourier_A/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/���. ������������/IabcCalc/Fourier_A/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/���. ������������/IabcCalc/Fourier_B/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/���. ������������/IabcCalc/Fourier_B/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/���. ������������/IabcCalc/Fourier_C/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/���. ������������/IabcCalc/Fourier_C/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/���. ������������/UabcCalc/Fourier_A/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/���. ������������/UabcCalc/Fourier_A/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/���. ������������/UabcCalc/Fourier_B/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/���. ������������/UabcCalc/Fourier_B/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/���. ������������/UabcCalc/Fourier_C/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/���. ������������/UabcCalc/Fourier_C/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/���. ������������/IabcCalc/Fourier_A/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/���. ������������/IabcCalc/Fourier_A/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/���. ������������/IabcCalc/Fourier_B/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/���. ������������/IabcCalc/Fourier_B/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/���. ������������/IabcCalc/Fourier_C/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/���. ������������/IabcCalc/Fourier_C/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/���. ������������/UabcCalc/Fourier_A/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/���. ������������/UabcCalc/Fourier_A/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/���. ������������/UabcCalc/Fourier_B/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/���. ������������/UabcCalc/Fourier_B/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/���. ������������/UabcCalc/Fourier_C/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/����������� \n������_G2/���. ������������/UabcCalc/Fourier_C/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/��������� ����� ���������� \n������������ �������/������ � ���� (���)/����� IN1N2/Mean value1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/��������� ����� ���������� \n������������ �������/������ � ���� (���)/����� IN1N2/Mean/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/PLL\n(3ph)/Model/Automatic Gain\nControl/Positive-Sequence\n(PLL-Driven)/Mean\n(Variable Frequency)1/Model/Variable\nTransport Delay"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/PLL\n(3ph)/Model/Automatic Gain\nControl/Positive-Sequence\n(PLL-Driven)/Mean\n(Variable Frequency)1/Model/integrator"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/PLL\n(3ph)/Model/Automatic Gain\nControl/Positive-Sequence\n(PLL-Driven)/Mean\n(Variable Frequency)2/Model/Variable\nTransport Delay"
,
 "DZG_v3/System/�������_���-01\n���������/�������� ������ \n����������_G1/PLL\n(3ph)/Model/Automatic Gain\nControl/Positive-Sequence\n(PLL-Driven)/Mean\n(Variable Frequency)2/Model/integrator"
, "DZG_v3/powergui/EquivalentModel2/State-Space" } ; static const char_T *
rt_LoggedStateNames [ ] = { "" , "" , "" , "" , "" , "" , "" , "" , "" , "" ,
"" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , ""
, "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" ,
"" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , ""
, "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" ,
"" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , ""
, "" , "DSTATE" } ; static boolean_T rt_LoggedStateCrossMdlRef [ ] = { 0 , 0
, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ; static RTWLogDataTypeConvert
rt_RTWLogDataTypeConvert [ ] = { { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 ,
1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } ,
{ 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } } ; static RTWLogSignalInfo rt_LoggedStateSignalInfo =
{ 90 , rt_LoggedStateWidths , rt_LoggedStateNumDimensions ,
rt_LoggedStateDimensions , rt_LoggedStateIsVarDims , ( NULL ) , ( NULL ) ,
rt_LoggedStateDataTypeIds , rt_LoggedStateComplexSignals , ( NULL ) ,
rt_LoggingStatePreprocessingFcnPtrs , { rt_LoggedStateLabels } , ( NULL ) , (
NULL ) , ( NULL ) , { rt_LoggedStateBlockNames } , { rt_LoggedStateNames } ,
rt_LoggedStateCrossMdlRef , rt_RTWLogDataTypeConvert } ; static void *
rt_LoggedStateSignalPtrs [ 90 ] ; rtliSetLogXSignalPtrs ( ssGetRTWLogInfo (
rtS ) , ( LogSignalPtrsType ) rt_LoggedStateSignalPtrs ) ;
rtliSetLogXSignalInfo ( ssGetRTWLogInfo ( rtS ) , & rt_LoggedStateSignalInfo
) ; rt_LoggedStateSignalPtrs [ 0 ] = ( void * ) & rtX . apevsa2hcq ;
rt_LoggedStateSignalPtrs [ 1 ] = ( void * ) & rtX . ebz5npnaoh ;
rt_LoggedStateSignalPtrs [ 2 ] = ( void * ) & rtX . g5pl2ihnsa ;
rt_LoggedStateSignalPtrs [ 3 ] = ( void * ) & rtX . dzmtcwo21u ;
rt_LoggedStateSignalPtrs [ 4 ] = ( void * ) & rtX . luwntsjfce ;
rt_LoggedStateSignalPtrs [ 5 ] = ( void * ) & rtX . lrou1be5ws ;
rt_LoggedStateSignalPtrs [ 6 ] = ( void * ) & rtX . kipcktetym ;
rt_LoggedStateSignalPtrs [ 7 ] = ( void * ) & rtX . hogtze5t2d ;
rt_LoggedStateSignalPtrs [ 8 ] = ( void * ) & rtX . aubyardb3x ;
rt_LoggedStateSignalPtrs [ 9 ] = ( void * ) & rtX . hummpur2b0 ;
rt_LoggedStateSignalPtrs [ 10 ] = ( void * ) & rtX . j0rirqtd1a ;
rt_LoggedStateSignalPtrs [ 11 ] = ( void * ) & rtX . ip0opk45xq ;
rt_LoggedStateSignalPtrs [ 12 ] = ( void * ) & rtX . cyudzpjr5t ;
rt_LoggedStateSignalPtrs [ 13 ] = ( void * ) & rtX . ky01z2j0r2 ;
rt_LoggedStateSignalPtrs [ 14 ] = ( void * ) & rtX . nyvh4po1ze ;
rt_LoggedStateSignalPtrs [ 15 ] = ( void * ) & rtX . fsa5rzt3eu ;
rt_LoggedStateSignalPtrs [ 16 ] = ( void * ) & rtX . iivfgrp0dv ;
rt_LoggedStateSignalPtrs [ 17 ] = ( void * ) & rtX . etzwqxlfek ;
rt_LoggedStateSignalPtrs [ 18 ] = ( void * ) & rtX . ikersjp5rv ;
rt_LoggedStateSignalPtrs [ 19 ] = ( void * ) & rtX . hgyqbdifk0 ;
rt_LoggedStateSignalPtrs [ 20 ] = ( void * ) & rtX . brjhw0iflo ;
rt_LoggedStateSignalPtrs [ 21 ] = ( void * ) & rtX . kerqyn5qft ;
rt_LoggedStateSignalPtrs [ 22 ] = ( void * ) & rtX . iytiwtpacb ;
rt_LoggedStateSignalPtrs [ 23 ] = ( void * ) & rtX . g0phs0m0dn ;
rt_LoggedStateSignalPtrs [ 24 ] = ( void * ) & rtX . lka0ltzwzh ;
rt_LoggedStateSignalPtrs [ 25 ] = ( void * ) & rtX . pquw0pmaw2 ;
rt_LoggedStateSignalPtrs [ 26 ] = ( void * ) & rtX . jx303cpogv ;
rt_LoggedStateSignalPtrs [ 27 ] = ( void * ) & rtX . ltxxvmnz50 ;
rt_LoggedStateSignalPtrs [ 28 ] = ( void * ) & rtX . b4ijv0yfdl ;
rt_LoggedStateSignalPtrs [ 29 ] = ( void * ) & rtX . inx2whmsvz ;
rt_LoggedStateSignalPtrs [ 30 ] = ( void * ) & rtX . obmdbjtn3e ;
rt_LoggedStateSignalPtrs [ 31 ] = ( void * ) & rtX . mczl1h5kue ;
rt_LoggedStateSignalPtrs [ 32 ] = ( void * ) & rtX . b2kr0ggmvo ;
rt_LoggedStateSignalPtrs [ 33 ] = ( void * ) & rtX . pdrwmhpgu2 ;
rt_LoggedStateSignalPtrs [ 34 ] = ( void * ) & rtX . ckadbag1nz ;
rt_LoggedStateSignalPtrs [ 35 ] = ( void * ) & rtX . kfqga2xqnc ;
rt_LoggedStateSignalPtrs [ 36 ] = ( void * ) & rtX . ovnjy3004d ;
rt_LoggedStateSignalPtrs [ 37 ] = ( void * ) & rtX . koitoczsgs ;
rt_LoggedStateSignalPtrs [ 38 ] = ( void * ) & rtX . fwvdemyybn ;
rt_LoggedStateSignalPtrs [ 39 ] = ( void * ) & rtX . df1cmyigny ;
rt_LoggedStateSignalPtrs [ 40 ] = ( void * ) & rtX . eiq3cpodg4 ;
rt_LoggedStateSignalPtrs [ 41 ] = ( void * ) & rtX . lujnb1vhch ;
rt_LoggedStateSignalPtrs [ 42 ] = ( void * ) & rtX . ivshzrdzao ;
rt_LoggedStateSignalPtrs [ 43 ] = ( void * ) & rtX . b0se24ah4v ;
rt_LoggedStateSignalPtrs [ 44 ] = ( void * ) & rtX . dzlchxjgwf ;
rt_LoggedStateSignalPtrs [ 45 ] = ( void * ) & rtX . kxjviilbiq ;
rt_LoggedStateSignalPtrs [ 46 ] = ( void * ) & rtX . kzm5j4om1j ;
rt_LoggedStateSignalPtrs [ 47 ] = ( void * ) & rtX . hrkjiy2wbe ;
rt_LoggedStateSignalPtrs [ 48 ] = ( void * ) & rtX . bbxcrevmf2 ;
rt_LoggedStateSignalPtrs [ 49 ] = ( void * ) & rtX . mecmfgexic ;
rt_LoggedStateSignalPtrs [ 50 ] = ( void * ) & rtX . gukucggul0 ;
rt_LoggedStateSignalPtrs [ 51 ] = ( void * ) & rtX . cooihtzhhj ;
rt_LoggedStateSignalPtrs [ 52 ] = ( void * ) & rtX . hbdbwheonc ;
rt_LoggedStateSignalPtrs [ 53 ] = ( void * ) & rtX . oipozxyfbt ;
rt_LoggedStateSignalPtrs [ 54 ] = ( void * ) & rtX . nrjwdf4bl5 ;
rt_LoggedStateSignalPtrs [ 55 ] = ( void * ) & rtX . d543hn3jm3 ;
rt_LoggedStateSignalPtrs [ 56 ] = ( void * ) & rtX . ktq1qocrqs ;
rt_LoggedStateSignalPtrs [ 57 ] = ( void * ) & rtX . ih0sh50oxm ;
rt_LoggedStateSignalPtrs [ 58 ] = ( void * ) & rtX . pn05gdwhsv ;
rt_LoggedStateSignalPtrs [ 59 ] = ( void * ) & rtX . jzyjyrpy3w ;
rt_LoggedStateSignalPtrs [ 60 ] = ( void * ) & rtX . ehyodyswyu ;
rt_LoggedStateSignalPtrs [ 61 ] = ( void * ) & rtX . hyqpl142bi ;
rt_LoggedStateSignalPtrs [ 62 ] = ( void * ) & rtX . prno02pbsy ;
rt_LoggedStateSignalPtrs [ 63 ] = ( void * ) & rtX . aztsfsn2hi ;
rt_LoggedStateSignalPtrs [ 64 ] = ( void * ) & rtX . f04g4otdzz ;
rt_LoggedStateSignalPtrs [ 65 ] = ( void * ) & rtX . ouay2vk4ob ;
rt_LoggedStateSignalPtrs [ 66 ] = ( void * ) & rtX . l4w5fqeack ;
rt_LoggedStateSignalPtrs [ 67 ] = ( void * ) & rtX . dbk3vhy4fa ;
rt_LoggedStateSignalPtrs [ 68 ] = ( void * ) & rtX . bwyxfbkejz ;
rt_LoggedStateSignalPtrs [ 69 ] = ( void * ) & rtX . cvhc14d1wd ;
rt_LoggedStateSignalPtrs [ 70 ] = ( void * ) & rtX . ipmtswi0si ;
rt_LoggedStateSignalPtrs [ 71 ] = ( void * ) & rtX . eczymfqr20 ;
rt_LoggedStateSignalPtrs [ 72 ] = ( void * ) & rtX . lobhhamyoj ;
rt_LoggedStateSignalPtrs [ 73 ] = ( void * ) & rtX . kenbvb3y0s ;
rt_LoggedStateSignalPtrs [ 74 ] = ( void * ) & rtX . nsp4pw5vpk ;
rt_LoggedStateSignalPtrs [ 75 ] = ( void * ) & rtX . prtdiw223v ;
rt_LoggedStateSignalPtrs [ 76 ] = ( void * ) & rtX . bots1ppmcj ;
rt_LoggedStateSignalPtrs [ 77 ] = ( void * ) & rtX . n0uge3m4yf ;
rt_LoggedStateSignalPtrs [ 78 ] = ( void * ) & rtX . gnzuxd2raf ;
rt_LoggedStateSignalPtrs [ 79 ] = ( void * ) & rtX . bqo41zgbir ;
rt_LoggedStateSignalPtrs [ 80 ] = ( void * ) & rtX . kiwleabf04 ;
rt_LoggedStateSignalPtrs [ 81 ] = ( void * ) & rtX . aa5y12cea2 ;
rt_LoggedStateSignalPtrs [ 82 ] = ( void * ) & rtX . cylrltmkae ;
rt_LoggedStateSignalPtrs [ 83 ] = ( void * ) & rtX . ar5lw0nd1g ;
rt_LoggedStateSignalPtrs [ 84 ] = ( void * ) & rtX . etqlz3rohc ;
rt_LoggedStateSignalPtrs [ 85 ] = ( void * ) & rtX . gfghljc4gn ;
rt_LoggedStateSignalPtrs [ 86 ] = ( void * ) & rtX . cgbqhiyy4k ;
rt_LoggedStateSignalPtrs [ 87 ] = ( void * ) & rtX . nhazmrtgmb ;
rt_LoggedStateSignalPtrs [ 88 ] = ( void * ) & rtX . pjrskjmpvo ;
rt_LoggedStateSignalPtrs [ 89 ] = ( void * ) rtDW . fn3owwkp0o ; }
rtliSetLogT ( ssGetRTWLogInfo ( rtS ) , "tout" ) ; rtliSetLogX (
ssGetRTWLogInfo ( rtS ) , "" ) ; rtliSetLogXFinal ( ssGetRTWLogInfo ( rtS ) ,
"" ) ; rtliSetLogVarNameModifier ( ssGetRTWLogInfo ( rtS ) , "none" ) ;
rtliSetLogFormat ( ssGetRTWLogInfo ( rtS ) , 4 ) ; rtliSetLogMaxRows (
ssGetRTWLogInfo ( rtS ) , 0 ) ; rtliSetLogDecimation ( ssGetRTWLogInfo ( rtS
) , 1 ) ; rtliSetLogY ( ssGetRTWLogInfo ( rtS ) , "" ) ;
rtliSetLogYSignalInfo ( ssGetRTWLogInfo ( rtS ) , ( NULL ) ) ;
rtliSetLogYSignalPtrs ( ssGetRTWLogInfo ( rtS ) , ( NULL ) ) ; } { static
struct _ssStatesInfo2 statesInfo2 ; ssSetStatesInfo2 ( rtS , & statesInfo2 )
; } { static ssPeriodicStatesInfo periodicStatesInfo ;
ssSetPeriodicStatesInfo ( rtS , & periodicStatesInfo ) ; } { static
ssJacobianPerturbationBounds jacobianPerturbationBounds ;
ssSetJacobianPerturbationBounds ( rtS , & jacobianPerturbationBounds ) ; } {
static ssSolverInfo slvrInfo ; static boolean_T contStatesDisabled [ 89 ] ;
ssSetSolverInfo ( rtS , & slvrInfo ) ; ssSetSolverName ( rtS , "ode14x" ) ;
ssSetVariableStepSolver ( rtS , 0 ) ; ssSetSolverConsistencyChecking ( rtS ,
0 ) ; ssSetSolverAdaptiveZcDetection ( rtS , 0 ) ;
ssSetSolverRobustResetMethod ( rtS , 0 ) ; ssSetSolverStateProjection ( rtS ,
0 ) ; ssSetSolverMassMatrixType ( rtS , ( ssMatrixType ) 0 ) ;
ssSetSolverMassMatrixNzMax ( rtS , 0 ) ; ssSetModelOutputs ( rtS , MdlOutputs
) ; ssSetModelLogData ( rtS , rt_UpdateTXYLogVars ) ;
ssSetModelLogDataIfInInterval ( rtS , rt_UpdateTXXFYLogVars ) ;
ssSetModelUpdate ( rtS , MdlUpdate ) ; ssSetModelDerivatives ( rtS ,
MdlDerivatives ) ; ssSetTNextTid ( rtS , INT_MIN ) ; ssSetTNext ( rtS ,
rtMinusInf ) ; ssSetSolverNeedsReset ( rtS ) ; ssSetNumNonsampledZCs ( rtS ,
0 ) ; ssSetContStateDisabled ( rtS , contStatesDisabled ) ; } { ZCSigState *
zc = ( ZCSigState * ) & rtPrevZCX ; ssSetPrevZCSigState ( rtS , zc ) ; } {
rtPrevZCX . oqfhcxlpho = UNINITIALIZED_ZCSIG ; rtPrevZCX . f3zha40yok .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . mvirue1zit . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . ivtty1aemy . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . myp2dcpbvg .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . onemmt3xbs . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . lu12zasvyl . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . ngrhemover . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . e4qyv4smfd . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
bs5m40ynfy . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . purgdi1jfk .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . g3bxbyypfb . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . me023aolbi . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . dexvjkg0hj . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . lkrts5ohpw . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
hh2cmxftpw . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . mpydob1efk .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . dlwrrwcsx5 . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . lbq0bgvgcl . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . cq2uqua4yo . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . crzn5z4zte . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
llpwcowx1f . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . nci0imhwrv .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . dlrk53odir . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . inwm331xkg . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . obeddxl4xp . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . lfrovwpki3 . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
h145orsdu5 . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . hytx1hl5s5 .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . mgkfvqxti2 . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . gk4015knwc . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . c2rt0zs300 . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
pj2a3t0x3l . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . pk2r3vb4fa .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . j33xaeouj5 . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . nx03ta252g . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . d2dryokhth .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . jcev1klzwf . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . b13ln335zj . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . d5dfks3vhb . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . ivzhqxkykj . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
kd31jkgljk . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . lf152u5jm4 .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . dewlul2b2r . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . f5lqeh5sbp . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . br1i12egyj . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . hvibrw2awu . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
allgw5ptgk . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . p2xsu5tknt .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . bhbl51zywo . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . ithg0f1jiq . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . ojnin3nhvw . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . jvoggbn2yd . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
boy5y1bmz3 . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . kk13i4dykq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . ep3fctwraa . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . d2nv3h1jhg . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . dyyk14ao4t .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . k5ecxmrj0a . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . pwnmvwir3d . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . ozvscdoxcm . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . outjvmmxn3 . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
brhbedften . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . ox1j1fxqy2 .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . nmew4qgpmi . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . ktoyym5lv5 . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . a503xmz3wd . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . jzgpron3co .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . dzkeyq1gmc . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . kxnpw5jl23 . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . g2dmi2bhhz . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . labvjdxfi3 . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
nvnct41vnp . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . l2r2uh0izk .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . feptx2ayjl . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . j0x4gfzwjd . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . anq402mwuh . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . iosuhjjule . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
fey4zxdyka . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . muhd0m2ek3 .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . g2vaojwvzg . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . nceplkru4a . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . cwhxhlnmc3 . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
pxbtpesdlg . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . cqgqlcfbrn .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . pshtvwhcvn . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . h510twish3 . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . b1csqxlk14 . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . j1qputxmt3 . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
h4jqcza024 . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . cdulylvwwz .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . o2355kd0aj . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . lsja5jhqu0 . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . o3bwas2b4l . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
h330fuij3a . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . l0mam0lcui .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . ck45m3cw5y . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . cssuh33ty1 . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . nmmyyafynb . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . ifezniamwz . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
dsdu0sw4bh . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . pflqpv2odc .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . c5rp3exjwz . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . fvh2zdqaau . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . f41hg5wei1 . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . jtynvhttl4 . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
jdzdt1ap5b . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . nvltfn14rj .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . dzcv3as4ol . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . dhpm1tyfmn . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . grby3szfkd . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . e1cruf0x3a . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
pg41d4mdss . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . apyjjt13q1 .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . arqjd0gbjl . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . pry2hnvt4q . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . hgwrktvi4c . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . a1srf3dmr2 . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
di10rsm5nz . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . nrt14kntrk .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . nepgqug5c0 . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . kbnljt2vok . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . ffo1fpyqgq . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . ceydu0zdw3 . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
e05ysccjt5 . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . f2cfptydf3 .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . mempnwchre . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . onwgyzhgir . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . ofq4vda2xo . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . gn1qmb4zgp . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
bvpxesww1j . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . dej3sbopt3 .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . ongp1nebfx . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . if1byo0yst . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . j5kctkibbm . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . diplwx4hig . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
my1xc0on3o . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . inadvhzvdh .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . leagxggwwe . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . dcuqvbgzpd . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . mydfkxdbcd . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . jw4rwe2x30 .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . acyd2alexd . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . kcdiwopp0s . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . eg2uuciuj1 . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
emkwv3osqn . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . j3mwdbpvpq .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . ieaaexns5m . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . dkvqk03afq . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . drlew34sm0 . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . bypgcvdndn . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
dtwz5asia3 . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . aa351sera2 .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . lfm4cvmyjw . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . jejxmo41e1 . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . kwzmy55kax . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . i43atkcssg . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
kqhpdk5kzz . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . dffp1wyrjz .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . mwrkb5ri4b . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . bd4y1f4afb . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . bbo3ui3fzz . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
rtPrevZCX . jvraryandh . hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX .
e2epyeugyn . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . ge1ps1iuod .
hirdn25ewp . nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . bk3jg31gdb . fx2esogudmq .
nz1ezojz4f = POS_ZCSIG ; rtPrevZCX . on20slldft0 . hirdn25ewp . nz1ezojz4f =
POS_ZCSIG ; rtPrevZCX . bhc3qgnuicw . fx2esogudmq . nz1ezojz4f = POS_ZCSIG ;
} ssSetChecksumVal ( rtS , 0 , 3592701390U ) ; ssSetChecksumVal ( rtS , 1 ,
3673746601U ) ; ssSetChecksumVal ( rtS , 2 , 3157903671U ) ; ssSetChecksumVal
( rtS , 3 , 808023130U ) ; { static const sysRanDType rtAlwaysEnabled =
SUBSYS_RAN_BC_ENABLE ; static RTWExtModeInfo rt_ExtModeInfo ; static const
sysRanDType * systemRan [ 1037 ] ; gblRTWExtModeInfo = & rt_ExtModeInfo ;
ssSetRTWExtModeInfo ( rtS , & rt_ExtModeInfo ) ;
rteiSetSubSystemActiveVectorAddresses ( & rt_ExtModeInfo , systemRan ) ;
systemRan [ 0 ] = & rtAlwaysEnabled ; systemRan [ 1 ] = & rtAlwaysEnabled ;
systemRan [ 2 ] = & rtAlwaysEnabled ; systemRan [ 3 ] = & rtAlwaysEnabled ;
systemRan [ 4 ] = & rtAlwaysEnabled ; systemRan [ 5 ] = & rtAlwaysEnabled ;
systemRan [ 6 ] = & rtAlwaysEnabled ; systemRan [ 7 ] = & rtAlwaysEnabled ;
systemRan [ 8 ] = & rtAlwaysEnabled ; systemRan [ 9 ] = & rtAlwaysEnabled ;
systemRan [ 10 ] = & rtAlwaysEnabled ; systemRan [ 11 ] = & rtAlwaysEnabled ;
systemRan [ 12 ] = & rtAlwaysEnabled ; systemRan [ 13 ] = & rtAlwaysEnabled ;
systemRan [ 14 ] = & rtAlwaysEnabled ; systemRan [ 15 ] = & rtAlwaysEnabled ;
systemRan [ 16 ] = & rtAlwaysEnabled ; systemRan [ 17 ] = & rtAlwaysEnabled ;
systemRan [ 18 ] = & rtAlwaysEnabled ; systemRan [ 19 ] = & rtAlwaysEnabled ;
systemRan [ 20 ] = & rtAlwaysEnabled ; systemRan [ 21 ] = & rtAlwaysEnabled ;
systemRan [ 22 ] = & rtAlwaysEnabled ; systemRan [ 23 ] = & rtAlwaysEnabled ;
systemRan [ 24 ] = & rtAlwaysEnabled ; systemRan [ 25 ] = & rtAlwaysEnabled ;
systemRan [ 26 ] = & rtAlwaysEnabled ; systemRan [ 27 ] = & rtAlwaysEnabled ;
systemRan [ 28 ] = & rtAlwaysEnabled ; systemRan [ 29 ] = & rtAlwaysEnabled ;
systemRan [ 30 ] = & rtAlwaysEnabled ; systemRan [ 31 ] = & rtAlwaysEnabled ;
systemRan [ 32 ] = & rtAlwaysEnabled ; systemRan [ 33 ] = & rtAlwaysEnabled ;
systemRan [ 34 ] = & rtAlwaysEnabled ; systemRan [ 35 ] = & rtAlwaysEnabled ;
systemRan [ 36 ] = & rtAlwaysEnabled ; systemRan [ 37 ] = & rtAlwaysEnabled ;
systemRan [ 38 ] = & rtAlwaysEnabled ; systemRan [ 39 ] = & rtAlwaysEnabled ;
systemRan [ 40 ] = & rtAlwaysEnabled ; systemRan [ 41 ] = & rtAlwaysEnabled ;
systemRan [ 42 ] = & rtAlwaysEnabled ; systemRan [ 43 ] = & rtAlwaysEnabled ;
systemRan [ 44 ] = & rtAlwaysEnabled ; systemRan [ 45 ] = & rtAlwaysEnabled ;
systemRan [ 46 ] = & rtAlwaysEnabled ; systemRan [ 47 ] = & rtAlwaysEnabled ;
systemRan [ 48 ] = & rtAlwaysEnabled ; systemRan [ 49 ] = & rtAlwaysEnabled ;
systemRan [ 50 ] = & rtAlwaysEnabled ; systemRan [ 51 ] = & rtAlwaysEnabled ;
systemRan [ 52 ] = & rtAlwaysEnabled ; systemRan [ 53 ] = & rtAlwaysEnabled ;
systemRan [ 54 ] = & rtAlwaysEnabled ; systemRan [ 55 ] = & rtAlwaysEnabled ;
systemRan [ 56 ] = & rtAlwaysEnabled ; systemRan [ 57 ] = & rtAlwaysEnabled ;
systemRan [ 58 ] = & rtAlwaysEnabled ; systemRan [ 59 ] = & rtAlwaysEnabled ;
systemRan [ 60 ] = & rtAlwaysEnabled ; systemRan [ 61 ] = & rtAlwaysEnabled ;
systemRan [ 62 ] = & rtAlwaysEnabled ; systemRan [ 63 ] = & rtAlwaysEnabled ;
systemRan [ 64 ] = & rtAlwaysEnabled ; systemRan [ 65 ] = & rtAlwaysEnabled ;
systemRan [ 66 ] = & rtAlwaysEnabled ; systemRan [ 67 ] = & rtAlwaysEnabled ;
systemRan [ 68 ] = & rtAlwaysEnabled ; systemRan [ 69 ] = & rtAlwaysEnabled ;
systemRan [ 70 ] = & rtAlwaysEnabled ; systemRan [ 71 ] = & rtAlwaysEnabled ;
systemRan [ 72 ] = & rtAlwaysEnabled ; systemRan [ 73 ] = & rtAlwaysEnabled ;
systemRan [ 74 ] = & rtAlwaysEnabled ; systemRan [ 75 ] = & rtAlwaysEnabled ;
systemRan [ 76 ] = & rtAlwaysEnabled ; systemRan [ 77 ] = & rtAlwaysEnabled ;
systemRan [ 78 ] = & rtAlwaysEnabled ; systemRan [ 79 ] = & rtAlwaysEnabled ;
systemRan [ 80 ] = & rtAlwaysEnabled ; systemRan [ 81 ] = & rtAlwaysEnabled ;
systemRan [ 82 ] = & rtAlwaysEnabled ; systemRan [ 83 ] = & rtAlwaysEnabled ;
systemRan [ 84 ] = ( sysRanDType * ) & rtDW . bhc3qgnuicw . fx2esogudmq .
fdd1jx2t0b ; systemRan [ 85 ] = ( sysRanDType * ) & rtDW . bhc3qgnuicw .
cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 86 ] = ( sysRanDType * ) & rtDW .
bhc3qgnuicw . itcd43cpffs . hjywrjdfvg ; systemRan [ 87 ] = ( sysRanDType * )
& rtDW . bhc3qgnuicw . c3fzgpzsmi ; systemRan [ 88 ] = ( sysRanDType * ) &
rtDW . on20slldft0 . hirdn25ewp . fdd1jx2t0b ; systemRan [ 89 ] = (
sysRanDType * ) & rtDW . on20slldft0 . cuc3rxn500 . kq4t2lkn4n ; systemRan [
90 ] = ( sysRanDType * ) & rtDW . on20slldft0 . cascrfypvi . hjywrjdfvg ;
systemRan [ 91 ] = ( sysRanDType * ) & rtDW . on20slldft0 . pbtm3vaf5b ;
systemRan [ 92 ] = ( sysRanDType * ) & rtDW . bk3jg31gdb . fx2esogudmq .
fdd1jx2t0b ; systemRan [ 93 ] = ( sysRanDType * ) & rtDW . bk3jg31gdb .
cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 94 ] = ( sysRanDType * ) & rtDW .
bk3jg31gdb . itcd43cpffs . hjywrjdfvg ; systemRan [ 95 ] = ( sysRanDType * )
& rtDW . bk3jg31gdb . c3fzgpzsmi ; systemRan [ 96 ] = ( sysRanDType * ) &
rtDW . ge1ps1iuod . hirdn25ewp . fdd1jx2t0b ; systemRan [ 97 ] = (
sysRanDType * ) & rtDW . ge1ps1iuod . cuc3rxn500 . kq4t2lkn4n ; systemRan [
98 ] = ( sysRanDType * ) & rtDW . ge1ps1iuod . cascrfypvi . hjywrjdfvg ;
systemRan [ 99 ] = ( sysRanDType * ) & rtDW . ge1ps1iuod . pbtm3vaf5b ;
systemRan [ 100 ] = ( sysRanDType * ) & rtDW . e2epyeugyn . fx2esogudmq .
fdd1jx2t0b ; systemRan [ 101 ] = ( sysRanDType * ) & rtDW . e2epyeugyn .
cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 102 ] = ( sysRanDType * ) & rtDW .
e2epyeugyn . itcd43cpffs . hjywrjdfvg ; systemRan [ 103 ] = ( sysRanDType * )
& rtDW . e2epyeugyn . c3fzgpzsmi ; systemRan [ 104 ] = ( sysRanDType * ) &
rtDW . jvraryandh . hirdn25ewp . fdd1jx2t0b ; systemRan [ 105 ] = (
sysRanDType * ) & rtDW . jvraryandh . cuc3rxn500 . kq4t2lkn4n ; systemRan [
106 ] = ( sysRanDType * ) & rtDW . jvraryandh . cascrfypvi . hjywrjdfvg ;
systemRan [ 107 ] = ( sysRanDType * ) & rtDW . jvraryandh . pbtm3vaf5b ;
systemRan [ 108 ] = ( sysRanDType * ) & rtDW . bbo3ui3fzz . fx2esogudmq .
fdd1jx2t0b ; systemRan [ 109 ] = ( sysRanDType * ) & rtDW . bbo3ui3fzz .
cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 110 ] = ( sysRanDType * ) & rtDW .
bbo3ui3fzz . itcd43cpffs . hjywrjdfvg ; systemRan [ 111 ] = ( sysRanDType * )
& rtDW . bbo3ui3fzz . c3fzgpzsmi ; systemRan [ 112 ] = ( sysRanDType * ) &
rtDW . bd4y1f4afb . hirdn25ewp . fdd1jx2t0b ; systemRan [ 113 ] = (
sysRanDType * ) & rtDW . bd4y1f4afb . cuc3rxn500 . kq4t2lkn4n ; systemRan [
114 ] = ( sysRanDType * ) & rtDW . bd4y1f4afb . cascrfypvi . hjywrjdfvg ;
systemRan [ 115 ] = ( sysRanDType * ) & rtDW . bd4y1f4afb . pbtm3vaf5b ;
systemRan [ 116 ] = ( sysRanDType * ) & rtDW . mwrkb5ri4b . fx2esogudmq .
fdd1jx2t0b ; systemRan [ 117 ] = ( sysRanDType * ) & rtDW . mwrkb5ri4b .
cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 118 ] = ( sysRanDType * ) & rtDW .
mwrkb5ri4b . itcd43cpffs . hjywrjdfvg ; systemRan [ 119 ] = ( sysRanDType * )
& rtDW . mwrkb5ri4b . c3fzgpzsmi ; systemRan [ 120 ] = ( sysRanDType * ) &
rtDW . dffp1wyrjz . hirdn25ewp . fdd1jx2t0b ; systemRan [ 121 ] = (
sysRanDType * ) & rtDW . dffp1wyrjz . cuc3rxn500 . kq4t2lkn4n ; systemRan [
122 ] = ( sysRanDType * ) & rtDW . dffp1wyrjz . cascrfypvi . hjywrjdfvg ;
systemRan [ 123 ] = ( sysRanDType * ) & rtDW . dffp1wyrjz . pbtm3vaf5b ;
systemRan [ 124 ] = ( sysRanDType * ) & rtDW . kqhpdk5kzz . fx2esogudmq .
fdd1jx2t0b ; systemRan [ 125 ] = ( sysRanDType * ) & rtDW . kqhpdk5kzz .
cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 126 ] = ( sysRanDType * ) & rtDW .
kqhpdk5kzz . itcd43cpffs . hjywrjdfvg ; systemRan [ 127 ] = ( sysRanDType * )
& rtDW . kqhpdk5kzz . c3fzgpzsmi ; systemRan [ 128 ] = ( sysRanDType * ) &
rtDW . i43atkcssg . hirdn25ewp . fdd1jx2t0b ; systemRan [ 129 ] = (
sysRanDType * ) & rtDW . i43atkcssg . cuc3rxn500 . kq4t2lkn4n ; systemRan [
130 ] = ( sysRanDType * ) & rtDW . i43atkcssg . cascrfypvi . hjywrjdfvg ;
systemRan [ 131 ] = ( sysRanDType * ) & rtDW . i43atkcssg . pbtm3vaf5b ;
systemRan [ 132 ] = ( sysRanDType * ) & rtDW . kwzmy55kax . fx2esogudmq .
fdd1jx2t0b ; systemRan [ 133 ] = ( sysRanDType * ) & rtDW . kwzmy55kax .
cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 134 ] = ( sysRanDType * ) & rtDW .
kwzmy55kax . itcd43cpffs . hjywrjdfvg ; systemRan [ 135 ] = ( sysRanDType * )
& rtDW . kwzmy55kax . c3fzgpzsmi ; systemRan [ 136 ] = ( sysRanDType * ) &
rtDW . jejxmo41e1 . hirdn25ewp . fdd1jx2t0b ; systemRan [ 137 ] = (
sysRanDType * ) & rtDW . jejxmo41e1 . cuc3rxn500 . kq4t2lkn4n ; systemRan [
138 ] = ( sysRanDType * ) & rtDW . jejxmo41e1 . cascrfypvi . hjywrjdfvg ;
systemRan [ 139 ] = ( sysRanDType * ) & rtDW . jejxmo41e1 . pbtm3vaf5b ;
systemRan [ 140 ] = & rtAlwaysEnabled ; systemRan [ 141 ] = ( sysRanDType * )
& rtDW . lfm4cvmyjw . fx2esogudmq . fdd1jx2t0b ; systemRan [ 142 ] = (
sysRanDType * ) & rtDW . lfm4cvmyjw . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [
143 ] = ( sysRanDType * ) & rtDW . lfm4cvmyjw . itcd43cpffs . hjywrjdfvg ;
systemRan [ 144 ] = ( sysRanDType * ) & rtDW . lfm4cvmyjw . c3fzgpzsmi ;
systemRan [ 145 ] = ( sysRanDType * ) & rtDW . aa351sera2 . hirdn25ewp .
fdd1jx2t0b ; systemRan [ 146 ] = ( sysRanDType * ) & rtDW . aa351sera2 .
cuc3rxn500 . kq4t2lkn4n ; systemRan [ 147 ] = ( sysRanDType * ) & rtDW .
aa351sera2 . cascrfypvi . hjywrjdfvg ; systemRan [ 148 ] = ( sysRanDType * )
& rtDW . aa351sera2 . pbtm3vaf5b ; systemRan [ 149 ] = ( sysRanDType * ) &
rtDW . dtwz5asia3 . fx2esogudmq . fdd1jx2t0b ; systemRan [ 150 ] = (
sysRanDType * ) & rtDW . dtwz5asia3 . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [
151 ] = ( sysRanDType * ) & rtDW . dtwz5asia3 . itcd43cpffs . hjywrjdfvg ;
systemRan [ 152 ] = ( sysRanDType * ) & rtDW . dtwz5asia3 . c3fzgpzsmi ;
systemRan [ 153 ] = ( sysRanDType * ) & rtDW . bypgcvdndn . hirdn25ewp .
fdd1jx2t0b ; systemRan [ 154 ] = ( sysRanDType * ) & rtDW . bypgcvdndn .
cuc3rxn500 . kq4t2lkn4n ; systemRan [ 155 ] = ( sysRanDType * ) & rtDW .
bypgcvdndn . cascrfypvi . hjywrjdfvg ; systemRan [ 156 ] = ( sysRanDType * )
& rtDW . bypgcvdndn . pbtm3vaf5b ; systemRan [ 157 ] = & rtAlwaysEnabled ;
systemRan [ 158 ] = ( sysRanDType * ) & rtDW . drlew34sm0 . fx2esogudmq .
fdd1jx2t0b ; systemRan [ 159 ] = ( sysRanDType * ) & rtDW . drlew34sm0 .
cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 160 ] = ( sysRanDType * ) & rtDW .
drlew34sm0 . itcd43cpffs . hjywrjdfvg ; systemRan [ 161 ] = ( sysRanDType * )
& rtDW . drlew34sm0 . c3fzgpzsmi ; systemRan [ 162 ] = ( sysRanDType * ) &
rtDW . dkvqk03afq . hirdn25ewp . fdd1jx2t0b ; systemRan [ 163 ] = (
sysRanDType * ) & rtDW . dkvqk03afq . cuc3rxn500 . kq4t2lkn4n ; systemRan [
164 ] = ( sysRanDType * ) & rtDW . dkvqk03afq . cascrfypvi . hjywrjdfvg ;
systemRan [ 165 ] = ( sysRanDType * ) & rtDW . dkvqk03afq . pbtm3vaf5b ;
systemRan [ 166 ] = ( sysRanDType * ) & rtDW . ieaaexns5m . fx2esogudmq .
fdd1jx2t0b ; systemRan [ 167 ] = ( sysRanDType * ) & rtDW . ieaaexns5m .
cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 168 ] = ( sysRanDType * ) & rtDW .
ieaaexns5m . itcd43cpffs . hjywrjdfvg ; systemRan [ 169 ] = ( sysRanDType * )
& rtDW . ieaaexns5m . c3fzgpzsmi ; systemRan [ 170 ] = ( sysRanDType * ) &
rtDW . j3mwdbpvpq . hirdn25ewp . fdd1jx2t0b ; systemRan [ 171 ] = (
sysRanDType * ) & rtDW . j3mwdbpvpq . cuc3rxn500 . kq4t2lkn4n ; systemRan [
172 ] = ( sysRanDType * ) & rtDW . j3mwdbpvpq . cascrfypvi . hjywrjdfvg ;
systemRan [ 173 ] = ( sysRanDType * ) & rtDW . j3mwdbpvpq . pbtm3vaf5b ;
systemRan [ 174 ] = & rtAlwaysEnabled ; systemRan [ 175 ] = ( sysRanDType * )
& rtDW . emkwv3osqn . fx2esogudmq . fdd1jx2t0b ; systemRan [ 176 ] = (
sysRanDType * ) & rtDW . emkwv3osqn . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [
177 ] = ( sysRanDType * ) & rtDW . emkwv3osqn . itcd43cpffs . hjywrjdfvg ;
systemRan [ 178 ] = ( sysRanDType * ) & rtDW . emkwv3osqn . c3fzgpzsmi ;
systemRan [ 179 ] = ( sysRanDType * ) & rtDW . eg2uuciuj1 . hirdn25ewp .
fdd1jx2t0b ; systemRan [ 180 ] = ( sysRanDType * ) & rtDW . eg2uuciuj1 .
cuc3rxn500 . kq4t2lkn4n ; systemRan [ 181 ] = ( sysRanDType * ) & rtDW .
eg2uuciuj1 . cascrfypvi . hjywrjdfvg ; systemRan [ 182 ] = ( sysRanDType * )
& rtDW . eg2uuciuj1 . pbtm3vaf5b ; systemRan [ 183 ] = ( sysRanDType * ) &
rtDW . kcdiwopp0s . fx2esogudmq . fdd1jx2t0b ; systemRan [ 184 ] = (
sysRanDType * ) & rtDW . kcdiwopp0s . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [
185 ] = ( sysRanDType * ) & rtDW . kcdiwopp0s . itcd43cpffs . hjywrjdfvg ;
systemRan [ 186 ] = ( sysRanDType * ) & rtDW . kcdiwopp0s . c3fzgpzsmi ;
systemRan [ 187 ] = ( sysRanDType * ) & rtDW . acyd2alexd . hirdn25ewp .
fdd1jx2t0b ; systemRan [ 188 ] = ( sysRanDType * ) & rtDW . acyd2alexd .
cuc3rxn500 . kq4t2lkn4n ; systemRan [ 189 ] = ( sysRanDType * ) & rtDW .
acyd2alexd . cascrfypvi . hjywrjdfvg ; systemRan [ 190 ] = ( sysRanDType * )
& rtDW . acyd2alexd . pbtm3vaf5b ; systemRan [ 191 ] = & rtAlwaysEnabled ;
systemRan [ 192 ] = & rtAlwaysEnabled ; systemRan [ 193 ] = ( sysRanDType * )
& rtDW . jw4rwe2x30 . fdd1jx2t0b ; systemRan [ 194 ] = ( sysRanDType * ) &
rtDW . fushkw2zny . kq4t2lkn4n ; systemRan [ 195 ] = ( sysRanDType * ) & rtDW
. i2y4oqfnlh . hjywrjdfvg ; systemRan [ 196 ] = ( sysRanDType * ) & rtDW .
mydfkxdbcd . fdd1jx2t0b ; systemRan [ 197 ] = ( sysRanDType * ) & rtDW .
cg1ijj42cn . kq4t2lkn4n ; systemRan [ 198 ] = ( sysRanDType * ) & rtDW .
fau5zmxxcd . hjywrjdfvg ; systemRan [ 199 ] = ( sysRanDType * ) & rtDW .
dcuqvbgzpd . fdd1jx2t0b ; systemRan [ 200 ] = ( sysRanDType * ) & rtDW .
np33hf1v4j . kq4t2lkn4n ; systemRan [ 201 ] = ( sysRanDType * ) & rtDW .
ncfy3u3izo . hjywrjdfvg ; systemRan [ 202 ] = ( sysRanDType * ) & rtDW .
leagxggwwe . fx2esogudmq . fdd1jx2t0b ; systemRan [ 203 ] = ( sysRanDType * )
& rtDW . leagxggwwe . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 204 ] = (
sysRanDType * ) & rtDW . leagxggwwe . itcd43cpffs . hjywrjdfvg ; systemRan [
205 ] = ( sysRanDType * ) & rtDW . leagxggwwe . c3fzgpzsmi ; systemRan [ 206
] = ( sysRanDType * ) & rtDW . inadvhzvdh . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 207 ] = ( sysRanDType * ) & rtDW . inadvhzvdh . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 208 ] = ( sysRanDType * ) & rtDW . inadvhzvdh .
cascrfypvi . hjywrjdfvg ; systemRan [ 209 ] = ( sysRanDType * ) & rtDW .
inadvhzvdh . pbtm3vaf5b ; systemRan [ 210 ] = ( sysRanDType * ) & rtDW .
my1xc0on3o . fx2esogudmq . fdd1jx2t0b ; systemRan [ 211 ] = ( sysRanDType * )
& rtDW . my1xc0on3o . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 212 ] = (
sysRanDType * ) & rtDW . my1xc0on3o . itcd43cpffs . hjywrjdfvg ; systemRan [
213 ] = ( sysRanDType * ) & rtDW . my1xc0on3o . c3fzgpzsmi ; systemRan [ 214
] = ( sysRanDType * ) & rtDW . diplwx4hig . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 215 ] = ( sysRanDType * ) & rtDW . diplwx4hig . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 216 ] = ( sysRanDType * ) & rtDW . diplwx4hig .
cascrfypvi . hjywrjdfvg ; systemRan [ 217 ] = ( sysRanDType * ) & rtDW .
diplwx4hig . pbtm3vaf5b ; systemRan [ 218 ] = ( sysRanDType * ) & rtDW .
j5kctkibbm . fx2esogudmq . fdd1jx2t0b ; systemRan [ 219 ] = ( sysRanDType * )
& rtDW . j5kctkibbm . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 220 ] = (
sysRanDType * ) & rtDW . j5kctkibbm . itcd43cpffs . hjywrjdfvg ; systemRan [
221 ] = ( sysRanDType * ) & rtDW . j5kctkibbm . c3fzgpzsmi ; systemRan [ 222
] = ( sysRanDType * ) & rtDW . if1byo0yst . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 223 ] = ( sysRanDType * ) & rtDW . if1byo0yst . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 224 ] = ( sysRanDType * ) & rtDW . if1byo0yst .
cascrfypvi . hjywrjdfvg ; systemRan [ 225 ] = ( sysRanDType * ) & rtDW .
if1byo0yst . pbtm3vaf5b ; systemRan [ 226 ] = ( sysRanDType * ) & rtDW .
ongp1nebfx . fx2esogudmq . fdd1jx2t0b ; systemRan [ 227 ] = ( sysRanDType * )
& rtDW . ongp1nebfx . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 228 ] = (
sysRanDType * ) & rtDW . ongp1nebfx . itcd43cpffs . hjywrjdfvg ; systemRan [
229 ] = ( sysRanDType * ) & rtDW . ongp1nebfx . c3fzgpzsmi ; systemRan [ 230
] = ( sysRanDType * ) & rtDW . dej3sbopt3 . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 231 ] = ( sysRanDType * ) & rtDW . dej3sbopt3 . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 232 ] = ( sysRanDType * ) & rtDW . dej3sbopt3 .
cascrfypvi . hjywrjdfvg ; systemRan [ 233 ] = ( sysRanDType * ) & rtDW .
dej3sbopt3 . pbtm3vaf5b ; systemRan [ 234 ] = ( sysRanDType * ) & rtDW .
bvpxesww1j . fx2esogudmq . fdd1jx2t0b ; systemRan [ 235 ] = ( sysRanDType * )
& rtDW . bvpxesww1j . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 236 ] = (
sysRanDType * ) & rtDW . bvpxesww1j . itcd43cpffs . hjywrjdfvg ; systemRan [
237 ] = ( sysRanDType * ) & rtDW . bvpxesww1j . c3fzgpzsmi ; systemRan [ 238
] = ( sysRanDType * ) & rtDW . gn1qmb4zgp . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 239 ] = ( sysRanDType * ) & rtDW . gn1qmb4zgp . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 240 ] = ( sysRanDType * ) & rtDW . gn1qmb4zgp .
cascrfypvi . hjywrjdfvg ; systemRan [ 241 ] = ( sysRanDType * ) & rtDW .
gn1qmb4zgp . pbtm3vaf5b ; systemRan [ 242 ] = ( sysRanDType * ) & rtDW .
ofq4vda2xo . fx2esogudmq . fdd1jx2t0b ; systemRan [ 243 ] = ( sysRanDType * )
& rtDW . ofq4vda2xo . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 244 ] = (
sysRanDType * ) & rtDW . ofq4vda2xo . itcd43cpffs . hjywrjdfvg ; systemRan [
245 ] = ( sysRanDType * ) & rtDW . ofq4vda2xo . c3fzgpzsmi ; systemRan [ 246
] = ( sysRanDType * ) & rtDW . onwgyzhgir . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 247 ] = ( sysRanDType * ) & rtDW . onwgyzhgir . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 248 ] = ( sysRanDType * ) & rtDW . onwgyzhgir .
cascrfypvi . hjywrjdfvg ; systemRan [ 249 ] = ( sysRanDType * ) & rtDW .
onwgyzhgir . pbtm3vaf5b ; systemRan [ 250 ] = & rtAlwaysEnabled ; systemRan [
251 ] = & rtAlwaysEnabled ; systemRan [ 252 ] = & rtAlwaysEnabled ; systemRan
[ 253 ] = & rtAlwaysEnabled ; systemRan [ 254 ] = & rtAlwaysEnabled ;
systemRan [ 255 ] = & rtAlwaysEnabled ; systemRan [ 256 ] = & rtAlwaysEnabled
; systemRan [ 257 ] = & rtAlwaysEnabled ; systemRan [ 258 ] = &
rtAlwaysEnabled ; systemRan [ 259 ] = & rtAlwaysEnabled ; systemRan [ 260 ] =
& rtAlwaysEnabled ; systemRan [ 261 ] = & rtAlwaysEnabled ; systemRan [ 262 ]
= & rtAlwaysEnabled ; systemRan [ 263 ] = ( sysRanDType * ) & rtDW .
mempnwchre . fx2esogudmq . fdd1jx2t0b ; systemRan [ 264 ] = ( sysRanDType * )
& rtDW . mempnwchre . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 265 ] = (
sysRanDType * ) & rtDW . mempnwchre . itcd43cpffs . hjywrjdfvg ; systemRan [
266 ] = ( sysRanDType * ) & rtDW . mempnwchre . c3fzgpzsmi ; systemRan [ 267
] = ( sysRanDType * ) & rtDW . f2cfptydf3 . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 268 ] = ( sysRanDType * ) & rtDW . f2cfptydf3 . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 269 ] = ( sysRanDType * ) & rtDW . f2cfptydf3 .
cascrfypvi . hjywrjdfvg ; systemRan [ 270 ] = ( sysRanDType * ) & rtDW .
f2cfptydf3 . pbtm3vaf5b ; systemRan [ 271 ] = ( sysRanDType * ) & rtDW .
e05ysccjt5 . fx2esogudmq . fdd1jx2t0b ; systemRan [ 272 ] = ( sysRanDType * )
& rtDW . e05ysccjt5 . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 273 ] = (
sysRanDType * ) & rtDW . e05ysccjt5 . itcd43cpffs . hjywrjdfvg ; systemRan [
274 ] = ( sysRanDType * ) & rtDW . e05ysccjt5 . c3fzgpzsmi ; systemRan [ 275
] = ( sysRanDType * ) & rtDW . ceydu0zdw3 . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 276 ] = ( sysRanDType * ) & rtDW . ceydu0zdw3 . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 277 ] = ( sysRanDType * ) & rtDW . ceydu0zdw3 .
cascrfypvi . hjywrjdfvg ; systemRan [ 278 ] = ( sysRanDType * ) & rtDW .
ceydu0zdw3 . pbtm3vaf5b ; systemRan [ 279 ] = ( sysRanDType * ) & rtDW .
ffo1fpyqgq . fx2esogudmq . fdd1jx2t0b ; systemRan [ 280 ] = ( sysRanDType * )
& rtDW . ffo1fpyqgq . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 281 ] = (
sysRanDType * ) & rtDW . ffo1fpyqgq . itcd43cpffs . hjywrjdfvg ; systemRan [
282 ] = ( sysRanDType * ) & rtDW . ffo1fpyqgq . c3fzgpzsmi ; systemRan [ 283
] = ( sysRanDType * ) & rtDW . kbnljt2vok . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 284 ] = ( sysRanDType * ) & rtDW . kbnljt2vok . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 285 ] = ( sysRanDType * ) & rtDW . kbnljt2vok .
cascrfypvi . hjywrjdfvg ; systemRan [ 286 ] = ( sysRanDType * ) & rtDW .
kbnljt2vok . pbtm3vaf5b ; systemRan [ 287 ] = ( sysRanDType * ) & rtDW .
nepgqug5c0 . fx2esogudmq . fdd1jx2t0b ; systemRan [ 288 ] = ( sysRanDType * )
& rtDW . nepgqug5c0 . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 289 ] = (
sysRanDType * ) & rtDW . nepgqug5c0 . itcd43cpffs . hjywrjdfvg ; systemRan [
290 ] = ( sysRanDType * ) & rtDW . nepgqug5c0 . c3fzgpzsmi ; systemRan [ 291
] = ( sysRanDType * ) & rtDW . nrt14kntrk . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 292 ] = ( sysRanDType * ) & rtDW . nrt14kntrk . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 293 ] = ( sysRanDType * ) & rtDW . nrt14kntrk .
cascrfypvi . hjywrjdfvg ; systemRan [ 294 ] = ( sysRanDType * ) & rtDW .
nrt14kntrk . pbtm3vaf5b ; systemRan [ 295 ] = & rtAlwaysEnabled ; systemRan [
296 ] = & rtAlwaysEnabled ; systemRan [ 297 ] = & rtAlwaysEnabled ; systemRan
[ 298 ] = & rtAlwaysEnabled ; systemRan [ 299 ] = & rtAlwaysEnabled ;
systemRan [ 300 ] = & rtAlwaysEnabled ; systemRan [ 301 ] = & rtAlwaysEnabled
; systemRan [ 302 ] = & rtAlwaysEnabled ; systemRan [ 303 ] = &
rtAlwaysEnabled ; systemRan [ 304 ] = & rtAlwaysEnabled ; systemRan [ 305 ] =
& rtAlwaysEnabled ; systemRan [ 306 ] = & rtAlwaysEnabled ; systemRan [ 307 ]
= & rtAlwaysEnabled ; systemRan [ 308 ] = ( sysRanDType * ) & rtDW .
di10rsm5nz . fx2esogudmq . fdd1jx2t0b ; systemRan [ 309 ] = ( sysRanDType * )
& rtDW . di10rsm5nz . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 310 ] = (
sysRanDType * ) & rtDW . di10rsm5nz . itcd43cpffs . hjywrjdfvg ; systemRan [
311 ] = ( sysRanDType * ) & rtDW . di10rsm5nz . c3fzgpzsmi ; systemRan [ 312
] = ( sysRanDType * ) & rtDW . a1srf3dmr2 . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 313 ] = ( sysRanDType * ) & rtDW . a1srf3dmr2 . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 314 ] = ( sysRanDType * ) & rtDW . a1srf3dmr2 .
cascrfypvi . hjywrjdfvg ; systemRan [ 315 ] = ( sysRanDType * ) & rtDW .
a1srf3dmr2 . pbtm3vaf5b ; systemRan [ 316 ] = ( sysRanDType * ) & rtDW .
hgwrktvi4c . fx2esogudmq . fdd1jx2t0b ; systemRan [ 317 ] = ( sysRanDType * )
& rtDW . hgwrktvi4c . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 318 ] = (
sysRanDType * ) & rtDW . hgwrktvi4c . itcd43cpffs . hjywrjdfvg ; systemRan [
319 ] = ( sysRanDType * ) & rtDW . hgwrktvi4c . c3fzgpzsmi ; systemRan [ 320
] = ( sysRanDType * ) & rtDW . pry2hnvt4q . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 321 ] = ( sysRanDType * ) & rtDW . pry2hnvt4q . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 322 ] = ( sysRanDType * ) & rtDW . pry2hnvt4q .
cascrfypvi . hjywrjdfvg ; systemRan [ 323 ] = ( sysRanDType * ) & rtDW .
pry2hnvt4q . pbtm3vaf5b ; systemRan [ 324 ] = ( sysRanDType * ) & rtDW .
arqjd0gbjl . fx2esogudmq . fdd1jx2t0b ; systemRan [ 325 ] = ( sysRanDType * )
& rtDW . arqjd0gbjl . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 326 ] = (
sysRanDType * ) & rtDW . arqjd0gbjl . itcd43cpffs . hjywrjdfvg ; systemRan [
327 ] = ( sysRanDType * ) & rtDW . arqjd0gbjl . c3fzgpzsmi ; systemRan [ 328
] = ( sysRanDType * ) & rtDW . apyjjt13q1 . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 329 ] = ( sysRanDType * ) & rtDW . apyjjt13q1 . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 330 ] = ( sysRanDType * ) & rtDW . apyjjt13q1 .
cascrfypvi . hjywrjdfvg ; systemRan [ 331 ] = ( sysRanDType * ) & rtDW .
apyjjt13q1 . pbtm3vaf5b ; systemRan [ 332 ] = ( sysRanDType * ) & rtDW .
pg41d4mdss . fx2esogudmq . fdd1jx2t0b ; systemRan [ 333 ] = ( sysRanDType * )
& rtDW . pg41d4mdss . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 334 ] = (
sysRanDType * ) & rtDW . pg41d4mdss . itcd43cpffs . hjywrjdfvg ; systemRan [
335 ] = ( sysRanDType * ) & rtDW . pg41d4mdss . c3fzgpzsmi ; systemRan [ 336
] = ( sysRanDType * ) & rtDW . e1cruf0x3a . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 337 ] = ( sysRanDType * ) & rtDW . e1cruf0x3a . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 338 ] = ( sysRanDType * ) & rtDW . e1cruf0x3a .
cascrfypvi . hjywrjdfvg ; systemRan [ 339 ] = ( sysRanDType * ) & rtDW .
e1cruf0x3a . pbtm3vaf5b ; systemRan [ 340 ] = & rtAlwaysEnabled ; systemRan [
341 ] = & rtAlwaysEnabled ; systemRan [ 342 ] = & rtAlwaysEnabled ; systemRan
[ 343 ] = & rtAlwaysEnabled ; systemRan [ 344 ] = & rtAlwaysEnabled ;
systemRan [ 345 ] = & rtAlwaysEnabled ; systemRan [ 346 ] = & rtAlwaysEnabled
; systemRan [ 347 ] = & rtAlwaysEnabled ; systemRan [ 348 ] = &
rtAlwaysEnabled ; systemRan [ 349 ] = & rtAlwaysEnabled ; systemRan [ 350 ] =
& rtAlwaysEnabled ; systemRan [ 351 ] = & rtAlwaysEnabled ; systemRan [ 352 ]
= & rtAlwaysEnabled ; systemRan [ 353 ] = ( sysRanDType * ) & rtDW .
grby3szfkd . fx2esogudmq . fdd1jx2t0b ; systemRan [ 354 ] = ( sysRanDType * )
& rtDW . grby3szfkd . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 355 ] = (
sysRanDType * ) & rtDW . grby3szfkd . itcd43cpffs . hjywrjdfvg ; systemRan [
356 ] = ( sysRanDType * ) & rtDW . grby3szfkd . c3fzgpzsmi ; systemRan [ 357
] = ( sysRanDType * ) & rtDW . dhpm1tyfmn . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 358 ] = ( sysRanDType * ) & rtDW . dhpm1tyfmn . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 359 ] = ( sysRanDType * ) & rtDW . dhpm1tyfmn .
cascrfypvi . hjywrjdfvg ; systemRan [ 360 ] = ( sysRanDType * ) & rtDW .
dhpm1tyfmn . pbtm3vaf5b ; systemRan [ 361 ] = ( sysRanDType * ) & rtDW .
dzcv3as4ol . fx2esogudmq . fdd1jx2t0b ; systemRan [ 362 ] = ( sysRanDType * )
& rtDW . dzcv3as4ol . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 363 ] = (
sysRanDType * ) & rtDW . dzcv3as4ol . itcd43cpffs . hjywrjdfvg ; systemRan [
364 ] = ( sysRanDType * ) & rtDW . dzcv3as4ol . c3fzgpzsmi ; systemRan [ 365
] = ( sysRanDType * ) & rtDW . nvltfn14rj . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 366 ] = ( sysRanDType * ) & rtDW . nvltfn14rj . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 367 ] = ( sysRanDType * ) & rtDW . nvltfn14rj .
cascrfypvi . hjywrjdfvg ; systemRan [ 368 ] = ( sysRanDType * ) & rtDW .
nvltfn14rj . pbtm3vaf5b ; systemRan [ 369 ] = ( sysRanDType * ) & rtDW .
jdzdt1ap5b . fx2esogudmq . fdd1jx2t0b ; systemRan [ 370 ] = ( sysRanDType * )
& rtDW . jdzdt1ap5b . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 371 ] = (
sysRanDType * ) & rtDW . jdzdt1ap5b . itcd43cpffs . hjywrjdfvg ; systemRan [
372 ] = ( sysRanDType * ) & rtDW . jdzdt1ap5b . c3fzgpzsmi ; systemRan [ 373
] = ( sysRanDType * ) & rtDW . jtynvhttl4 . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 374 ] = ( sysRanDType * ) & rtDW . jtynvhttl4 . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 375 ] = ( sysRanDType * ) & rtDW . jtynvhttl4 .
cascrfypvi . hjywrjdfvg ; systemRan [ 376 ] = ( sysRanDType * ) & rtDW .
jtynvhttl4 . pbtm3vaf5b ; systemRan [ 377 ] = ( sysRanDType * ) & rtDW .
f41hg5wei1 . fx2esogudmq . fdd1jx2t0b ; systemRan [ 378 ] = ( sysRanDType * )
& rtDW . f41hg5wei1 . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 379 ] = (
sysRanDType * ) & rtDW . f41hg5wei1 . itcd43cpffs . hjywrjdfvg ; systemRan [
380 ] = ( sysRanDType * ) & rtDW . f41hg5wei1 . c3fzgpzsmi ; systemRan [ 381
] = ( sysRanDType * ) & rtDW . fvh2zdqaau . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 382 ] = ( sysRanDType * ) & rtDW . fvh2zdqaau . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 383 ] = ( sysRanDType * ) & rtDW . fvh2zdqaau .
cascrfypvi . hjywrjdfvg ; systemRan [ 384 ] = ( sysRanDType * ) & rtDW .
fvh2zdqaau . pbtm3vaf5b ; systemRan [ 385 ] = & rtAlwaysEnabled ; systemRan [
386 ] = & rtAlwaysEnabled ; systemRan [ 387 ] = & rtAlwaysEnabled ; systemRan
[ 388 ] = ( sysRanDType * ) & rtDW . c5rp3exjwz . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 389 ] = ( sysRanDType * ) & rtDW . c5rp3exjwz . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 390 ] = ( sysRanDType * ) & rtDW . c5rp3exjwz .
itcd43cpffs . hjywrjdfvg ; systemRan [ 391 ] = ( sysRanDType * ) & rtDW .
c5rp3exjwz . c3fzgpzsmi ; systemRan [ 392 ] = ( sysRanDType * ) & rtDW .
pflqpv2odc . hirdn25ewp . fdd1jx2t0b ; systemRan [ 393 ] = ( sysRanDType * )
& rtDW . pflqpv2odc . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 394 ] = (
sysRanDType * ) & rtDW . pflqpv2odc . cascrfypvi . hjywrjdfvg ; systemRan [
395 ] = ( sysRanDType * ) & rtDW . pflqpv2odc . pbtm3vaf5b ; systemRan [ 396
] = ( sysRanDType * ) & rtDW . dsdu0sw4bh . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 397 ] = ( sysRanDType * ) & rtDW . dsdu0sw4bh . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 398 ] = ( sysRanDType * ) & rtDW . dsdu0sw4bh .
itcd43cpffs . hjywrjdfvg ; systemRan [ 399 ] = ( sysRanDType * ) & rtDW .
dsdu0sw4bh . c3fzgpzsmi ; systemRan [ 400 ] = ( sysRanDType * ) & rtDW .
ifezniamwz . hirdn25ewp . fdd1jx2t0b ; systemRan [ 401 ] = ( sysRanDType * )
& rtDW . ifezniamwz . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 402 ] = (
sysRanDType * ) & rtDW . ifezniamwz . cascrfypvi . hjywrjdfvg ; systemRan [
403 ] = ( sysRanDType * ) & rtDW . ifezniamwz . pbtm3vaf5b ; systemRan [ 404
] = ( sysRanDType * ) & rtDW . nmmyyafynb . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 405 ] = ( sysRanDType * ) & rtDW . nmmyyafynb . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 406 ] = ( sysRanDType * ) & rtDW . nmmyyafynb .
itcd43cpffs . hjywrjdfvg ; systemRan [ 407 ] = ( sysRanDType * ) & rtDW .
nmmyyafynb . c3fzgpzsmi ; systemRan [ 408 ] = ( sysRanDType * ) & rtDW .
cssuh33ty1 . hirdn25ewp . fdd1jx2t0b ; systemRan [ 409 ] = ( sysRanDType * )
& rtDW . cssuh33ty1 . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 410 ] = (
sysRanDType * ) & rtDW . cssuh33ty1 . cascrfypvi . hjywrjdfvg ; systemRan [
411 ] = ( sysRanDType * ) & rtDW . cssuh33ty1 . pbtm3vaf5b ; systemRan [ 412
] = & rtAlwaysEnabled ; systemRan [ 413 ] = ( sysRanDType * ) & rtDW .
ck45m3cw5y . fx2esogudmq . fdd1jx2t0b ; systemRan [ 414 ] = ( sysRanDType * )
& rtDW . ck45m3cw5y . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 415 ] = (
sysRanDType * ) & rtDW . ck45m3cw5y . itcd43cpffs . hjywrjdfvg ; systemRan [
416 ] = ( sysRanDType * ) & rtDW . ck45m3cw5y . c3fzgpzsmi ; systemRan [ 417
] = ( sysRanDType * ) & rtDW . l0mam0lcui . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 418 ] = ( sysRanDType * ) & rtDW . l0mam0lcui . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 419 ] = ( sysRanDType * ) & rtDW . l0mam0lcui .
cascrfypvi . hjywrjdfvg ; systemRan [ 420 ] = ( sysRanDType * ) & rtDW .
l0mam0lcui . pbtm3vaf5b ; systemRan [ 421 ] = ( sysRanDType * ) & rtDW .
h330fuij3a . fx2esogudmq . fdd1jx2t0b ; systemRan [ 422 ] = ( sysRanDType * )
& rtDW . h330fuij3a . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 423 ] = (
sysRanDType * ) & rtDW . h330fuij3a . itcd43cpffs . hjywrjdfvg ; systemRan [
424 ] = ( sysRanDType * ) & rtDW . h330fuij3a . c3fzgpzsmi ; systemRan [ 425
] = ( sysRanDType * ) & rtDW . o3bwas2b4l . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 426 ] = ( sysRanDType * ) & rtDW . o3bwas2b4l . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 427 ] = ( sysRanDType * ) & rtDW . o3bwas2b4l .
cascrfypvi . hjywrjdfvg ; systemRan [ 428 ] = ( sysRanDType * ) & rtDW .
o3bwas2b4l . pbtm3vaf5b ; systemRan [ 429 ] = ( sysRanDType * ) & rtDW .
lsja5jhqu0 . fdd1jx2t0b ; systemRan [ 430 ] = ( sysRanDType * ) & rtDW .
bckzp5laog . kq4t2lkn4n ; systemRan [ 431 ] = ( sysRanDType * ) & rtDW .
pqmqn4c0xq . hjywrjdfvg ; systemRan [ 432 ] = & rtAlwaysEnabled ; systemRan [
433 ] = & rtAlwaysEnabled ; systemRan [ 434 ] = & rtAlwaysEnabled ; systemRan
[ 435 ] = ( sysRanDType * ) & rtDW . o2355kd0aj . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 436 ] = ( sysRanDType * ) & rtDW . o2355kd0aj . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 437 ] = ( sysRanDType * ) & rtDW . o2355kd0aj .
itcd43cpffs . hjywrjdfvg ; systemRan [ 438 ] = ( sysRanDType * ) & rtDW .
o2355kd0aj . c3fzgpzsmi ; systemRan [ 439 ] = ( sysRanDType * ) & rtDW .
cdulylvwwz . hirdn25ewp . fdd1jx2t0b ; systemRan [ 440 ] = ( sysRanDType * )
& rtDW . cdulylvwwz . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 441 ] = (
sysRanDType * ) & rtDW . cdulylvwwz . cascrfypvi . hjywrjdfvg ; systemRan [
442 ] = ( sysRanDType * ) & rtDW . cdulylvwwz . pbtm3vaf5b ; systemRan [ 443
] = ( sysRanDType * ) & rtDW . h4jqcza024 . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 444 ] = ( sysRanDType * ) & rtDW . h4jqcza024 . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 445 ] = ( sysRanDType * ) & rtDW . h4jqcza024 .
itcd43cpffs . hjywrjdfvg ; systemRan [ 446 ] = ( sysRanDType * ) & rtDW .
h4jqcza024 . c3fzgpzsmi ; systemRan [ 447 ] = ( sysRanDType * ) & rtDW .
j1qputxmt3 . hirdn25ewp . fdd1jx2t0b ; systemRan [ 448 ] = ( sysRanDType * )
& rtDW . j1qputxmt3 . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 449 ] = (
sysRanDType * ) & rtDW . j1qputxmt3 . cascrfypvi . hjywrjdfvg ; systemRan [
450 ] = ( sysRanDType * ) & rtDW . j1qputxmt3 . pbtm3vaf5b ; systemRan [ 451
] = ( sysRanDType * ) & rtDW . b1csqxlk14 . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 452 ] = ( sysRanDType * ) & rtDW . b1csqxlk14 . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 453 ] = ( sysRanDType * ) & rtDW . b1csqxlk14 .
itcd43cpffs . hjywrjdfvg ; systemRan [ 454 ] = ( sysRanDType * ) & rtDW .
b1csqxlk14 . c3fzgpzsmi ; systemRan [ 455 ] = ( sysRanDType * ) & rtDW .
h510twish3 . hirdn25ewp . fdd1jx2t0b ; systemRan [ 456 ] = ( sysRanDType * )
& rtDW . h510twish3 . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 457 ] = (
sysRanDType * ) & rtDW . h510twish3 . cascrfypvi . hjywrjdfvg ; systemRan [
458 ] = ( sysRanDType * ) & rtDW . h510twish3 . pbtm3vaf5b ; systemRan [ 459
] = & rtAlwaysEnabled ; systemRan [ 460 ] = ( sysRanDType * ) & rtDW .
pshtvwhcvn . fx2esogudmq . fdd1jx2t0b ; systemRan [ 461 ] = ( sysRanDType * )
& rtDW . pshtvwhcvn . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 462 ] = (
sysRanDType * ) & rtDW . pshtvwhcvn . itcd43cpffs . hjywrjdfvg ; systemRan [
463 ] = ( sysRanDType * ) & rtDW . pshtvwhcvn . c3fzgpzsmi ; systemRan [ 464
] = ( sysRanDType * ) & rtDW . cqgqlcfbrn . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 465 ] = ( sysRanDType * ) & rtDW . cqgqlcfbrn . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 466 ] = ( sysRanDType * ) & rtDW . cqgqlcfbrn .
cascrfypvi . hjywrjdfvg ; systemRan [ 467 ] = ( sysRanDType * ) & rtDW .
cqgqlcfbrn . pbtm3vaf5b ; systemRan [ 468 ] = ( sysRanDType * ) & rtDW .
pxbtpesdlg . fx2esogudmq . fdd1jx2t0b ; systemRan [ 469 ] = ( sysRanDType * )
& rtDW . pxbtpesdlg . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 470 ] = (
sysRanDType * ) & rtDW . pxbtpesdlg . itcd43cpffs . hjywrjdfvg ; systemRan [
471 ] = ( sysRanDType * ) & rtDW . pxbtpesdlg . c3fzgpzsmi ; systemRan [ 472
] = ( sysRanDType * ) & rtDW . cwhxhlnmc3 . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 473 ] = ( sysRanDType * ) & rtDW . cwhxhlnmc3 . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 474 ] = ( sysRanDType * ) & rtDW . cwhxhlnmc3 .
cascrfypvi . hjywrjdfvg ; systemRan [ 475 ] = ( sysRanDType * ) & rtDW .
cwhxhlnmc3 . pbtm3vaf5b ; systemRan [ 476 ] = ( sysRanDType * ) & rtDW .
nceplkru4a . fdd1jx2t0b ; systemRan [ 477 ] = ( sysRanDType * ) & rtDW .
pxgzwnihte . kq4t2lkn4n ; systemRan [ 478 ] = ( sysRanDType * ) & rtDW .
jfvcadr0il . hjywrjdfvg ; systemRan [ 479 ] = & rtAlwaysEnabled ; systemRan [
480 ] = & rtAlwaysEnabled ; systemRan [ 481 ] = & rtAlwaysEnabled ; systemRan
[ 482 ] = & rtAlwaysEnabled ; systemRan [ 483 ] = ( sysRanDType * ) & rtDW .
g2vaojwvzg . fx2esogudmq . fdd1jx2t0b ; systemRan [ 484 ] = ( sysRanDType * )
& rtDW . g2vaojwvzg . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 485 ] = (
sysRanDType * ) & rtDW . g2vaojwvzg . itcd43cpffs . hjywrjdfvg ; systemRan [
486 ] = ( sysRanDType * ) & rtDW . g2vaojwvzg . c3fzgpzsmi ; systemRan [ 487
] = ( sysRanDType * ) & rtDW . muhd0m2ek3 . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 488 ] = ( sysRanDType * ) & rtDW . muhd0m2ek3 . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 489 ] = ( sysRanDType * ) & rtDW . muhd0m2ek3 .
cascrfypvi . hjywrjdfvg ; systemRan [ 490 ] = ( sysRanDType * ) & rtDW .
muhd0m2ek3 . pbtm3vaf5b ; systemRan [ 491 ] = ( sysRanDType * ) & rtDW .
fey4zxdyka . fx2esogudmq . fdd1jx2t0b ; systemRan [ 492 ] = ( sysRanDType * )
& rtDW . fey4zxdyka . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 493 ] = (
sysRanDType * ) & rtDW . fey4zxdyka . itcd43cpffs . hjywrjdfvg ; systemRan [
494 ] = ( sysRanDType * ) & rtDW . fey4zxdyka . c3fzgpzsmi ; systemRan [ 495
] = ( sysRanDType * ) & rtDW . iosuhjjule . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 496 ] = ( sysRanDType * ) & rtDW . iosuhjjule . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 497 ] = ( sysRanDType * ) & rtDW . iosuhjjule .
cascrfypvi . hjywrjdfvg ; systemRan [ 498 ] = ( sysRanDType * ) & rtDW .
iosuhjjule . pbtm3vaf5b ; systemRan [ 499 ] = ( sysRanDType * ) & rtDW .
anq402mwuh . fx2esogudmq . fdd1jx2t0b ; systemRan [ 500 ] = ( sysRanDType * )
& rtDW . anq402mwuh . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 501 ] = (
sysRanDType * ) & rtDW . anq402mwuh . itcd43cpffs . hjywrjdfvg ; systemRan [
502 ] = ( sysRanDType * ) & rtDW . anq402mwuh . c3fzgpzsmi ; systemRan [ 503
] = ( sysRanDType * ) & rtDW . j0x4gfzwjd . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 504 ] = ( sysRanDType * ) & rtDW . j0x4gfzwjd . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 505 ] = ( sysRanDType * ) & rtDW . j0x4gfzwjd .
cascrfypvi . hjywrjdfvg ; systemRan [ 506 ] = ( sysRanDType * ) & rtDW .
j0x4gfzwjd . pbtm3vaf5b ; systemRan [ 507 ] = & rtAlwaysEnabled ; systemRan [
508 ] = & rtAlwaysEnabled ; systemRan [ 509 ] = & rtAlwaysEnabled ; systemRan
[ 510 ] = ( sysRanDType * ) & rtDW . feptx2ayjl . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 511 ] = ( sysRanDType * ) & rtDW . feptx2ayjl . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 512 ] = ( sysRanDType * ) & rtDW . feptx2ayjl .
itcd43cpffs . hjywrjdfvg ; systemRan [ 513 ] = ( sysRanDType * ) & rtDW .
feptx2ayjl . c3fzgpzsmi ; systemRan [ 514 ] = ( sysRanDType * ) & rtDW .
l2r2uh0izk . hirdn25ewp . fdd1jx2t0b ; systemRan [ 515 ] = ( sysRanDType * )
& rtDW . l2r2uh0izk . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 516 ] = (
sysRanDType * ) & rtDW . l2r2uh0izk . cascrfypvi . hjywrjdfvg ; systemRan [
517 ] = ( sysRanDType * ) & rtDW . l2r2uh0izk . pbtm3vaf5b ; systemRan [ 518
] = ( sysRanDType * ) & rtDW . nvnct41vnp . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 519 ] = ( sysRanDType * ) & rtDW . nvnct41vnp . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 520 ] = ( sysRanDType * ) & rtDW . nvnct41vnp .
itcd43cpffs . hjywrjdfvg ; systemRan [ 521 ] = ( sysRanDType * ) & rtDW .
nvnct41vnp . c3fzgpzsmi ; systemRan [ 522 ] = ( sysRanDType * ) & rtDW .
labvjdxfi3 . hirdn25ewp . fdd1jx2t0b ; systemRan [ 523 ] = ( sysRanDType * )
& rtDW . labvjdxfi3 . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 524 ] = (
sysRanDType * ) & rtDW . labvjdxfi3 . cascrfypvi . hjywrjdfvg ; systemRan [
525 ] = ( sysRanDType * ) & rtDW . labvjdxfi3 . pbtm3vaf5b ; systemRan [ 526
] = ( sysRanDType * ) & rtDW . g2dmi2bhhz . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 527 ] = ( sysRanDType * ) & rtDW . g2dmi2bhhz . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 528 ] = ( sysRanDType * ) & rtDW . g2dmi2bhhz .
itcd43cpffs . hjywrjdfvg ; systemRan [ 529 ] = ( sysRanDType * ) & rtDW .
g2dmi2bhhz . c3fzgpzsmi ; systemRan [ 530 ] = ( sysRanDType * ) & rtDW .
kxnpw5jl23 . hirdn25ewp . fdd1jx2t0b ; systemRan [ 531 ] = ( sysRanDType * )
& rtDW . kxnpw5jl23 . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 532 ] = (
sysRanDType * ) & rtDW . kxnpw5jl23 . cascrfypvi . hjywrjdfvg ; systemRan [
533 ] = ( sysRanDType * ) & rtDW . kxnpw5jl23 . pbtm3vaf5b ; systemRan [ 534
] = ( sysRanDType * ) & rtDW . dzkeyq1gmc . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 535 ] = ( sysRanDType * ) & rtDW . dzkeyq1gmc . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 536 ] = ( sysRanDType * ) & rtDW . dzkeyq1gmc .
itcd43cpffs . hjywrjdfvg ; systemRan [ 537 ] = ( sysRanDType * ) & rtDW .
dzkeyq1gmc . c3fzgpzsmi ; systemRan [ 538 ] = ( sysRanDType * ) & rtDW .
jzgpron3co . hirdn25ewp . fdd1jx2t0b ; systemRan [ 539 ] = ( sysRanDType * )
& rtDW . jzgpron3co . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 540 ] = (
sysRanDType * ) & rtDW . jzgpron3co . cascrfypvi . hjywrjdfvg ; systemRan [
541 ] = ( sysRanDType * ) & rtDW . jzgpron3co . pbtm3vaf5b ; systemRan [ 542
] = & rtAlwaysEnabled ; systemRan [ 543 ] = & rtAlwaysEnabled ; systemRan [
544 ] = & rtAlwaysEnabled ; systemRan [ 545 ] = ( sysRanDType * ) & rtDW .
a503xmz3wd . fdd1jx2t0b ; systemRan [ 546 ] = ( sysRanDType * ) & rtDW .
ftiqhc2zan . kq4t2lkn4n ; systemRan [ 547 ] = ( sysRanDType * ) & rtDW .
b0usl1ftx3 . hjywrjdfvg ; systemRan [ 548 ] = ( sysRanDType * ) & rtDW .
ktoyym5lv5 . fdd1jx2t0b ; systemRan [ 549 ] = ( sysRanDType * ) & rtDW .
er0egiqxa1 . kq4t2lkn4n ; systemRan [ 550 ] = ( sysRanDType * ) & rtDW .
mzl2ofouzz . hjywrjdfvg ; systemRan [ 551 ] = & rtAlwaysEnabled ; systemRan [
552 ] = & rtAlwaysEnabled ; systemRan [ 553 ] = ( sysRanDType * ) & rtDW .
nmew4qgpmi . fx2esogudmq . fdd1jx2t0b ; systemRan [ 554 ] = ( sysRanDType * )
& rtDW . nmew4qgpmi . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 555 ] = (
sysRanDType * ) & rtDW . nmew4qgpmi . itcd43cpffs . hjywrjdfvg ; systemRan [
556 ] = ( sysRanDType * ) & rtDW . nmew4qgpmi . c3fzgpzsmi ; systemRan [ 557
] = ( sysRanDType * ) & rtDW . ox1j1fxqy2 . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 558 ] = ( sysRanDType * ) & rtDW . ox1j1fxqy2 . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 559 ] = ( sysRanDType * ) & rtDW . ox1j1fxqy2 .
cascrfypvi . hjywrjdfvg ; systemRan [ 560 ] = ( sysRanDType * ) & rtDW .
ox1j1fxqy2 . pbtm3vaf5b ; systemRan [ 561 ] = ( sysRanDType * ) & rtDW .
brhbedften . fx2esogudmq . fdd1jx2t0b ; systemRan [ 562 ] = ( sysRanDType * )
& rtDW . brhbedften . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 563 ] = (
sysRanDType * ) & rtDW . brhbedften . itcd43cpffs . hjywrjdfvg ; systemRan [
564 ] = ( sysRanDType * ) & rtDW . brhbedften . c3fzgpzsmi ; systemRan [ 565
] = ( sysRanDType * ) & rtDW . outjvmmxn3 . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 566 ] = ( sysRanDType * ) & rtDW . outjvmmxn3 . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 567 ] = ( sysRanDType * ) & rtDW . outjvmmxn3 .
cascrfypvi . hjywrjdfvg ; systemRan [ 568 ] = ( sysRanDType * ) & rtDW .
outjvmmxn3 . pbtm3vaf5b ; systemRan [ 569 ] = ( sysRanDType * ) & rtDW .
ozvscdoxcm . fx2esogudmq . fdd1jx2t0b ; systemRan [ 570 ] = ( sysRanDType * )
& rtDW . ozvscdoxcm . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 571 ] = (
sysRanDType * ) & rtDW . ozvscdoxcm . itcd43cpffs . hjywrjdfvg ; systemRan [
572 ] = ( sysRanDType * ) & rtDW . ozvscdoxcm . c3fzgpzsmi ; systemRan [ 573
] = ( sysRanDType * ) & rtDW . pwnmvwir3d . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 574 ] = ( sysRanDType * ) & rtDW . pwnmvwir3d . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 575 ] = ( sysRanDType * ) & rtDW . pwnmvwir3d .
cascrfypvi . hjywrjdfvg ; systemRan [ 576 ] = ( sysRanDType * ) & rtDW .
pwnmvwir3d . pbtm3vaf5b ; systemRan [ 577 ] = ( sysRanDType * ) & rtDW .
k5ecxmrj0a . fx2esogudmq . fdd1jx2t0b ; systemRan [ 578 ] = ( sysRanDType * )
& rtDW . k5ecxmrj0a . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 579 ] = (
sysRanDType * ) & rtDW . k5ecxmrj0a . itcd43cpffs . hjywrjdfvg ; systemRan [
580 ] = ( sysRanDType * ) & rtDW . k5ecxmrj0a . c3fzgpzsmi ; systemRan [ 581
] = ( sysRanDType * ) & rtDW . dyyk14ao4t . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 582 ] = ( sysRanDType * ) & rtDW . dyyk14ao4t . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 583 ] = ( sysRanDType * ) & rtDW . dyyk14ao4t .
cascrfypvi . hjywrjdfvg ; systemRan [ 584 ] = ( sysRanDType * ) & rtDW .
dyyk14ao4t . pbtm3vaf5b ; systemRan [ 585 ] = & rtAlwaysEnabled ; systemRan [
586 ] = & rtAlwaysEnabled ; systemRan [ 587 ] = & rtAlwaysEnabled ; systemRan
[ 588 ] = & rtAlwaysEnabled ; systemRan [ 589 ] = & rtAlwaysEnabled ;
systemRan [ 590 ] = & rtAlwaysEnabled ; systemRan [ 591 ] = & rtAlwaysEnabled
; systemRan [ 592 ] = & rtAlwaysEnabled ; systemRan [ 593 ] = &
rtAlwaysEnabled ; systemRan [ 594 ] = & rtAlwaysEnabled ; systemRan [ 595 ] =
& rtAlwaysEnabled ; systemRan [ 596 ] = & rtAlwaysEnabled ; systemRan [ 597 ]
= & rtAlwaysEnabled ; systemRan [ 598 ] = & rtAlwaysEnabled ; systemRan [ 599
] = & rtAlwaysEnabled ; systemRan [ 600 ] = ( sysRanDType * ) & rtDW .
d2nv3h1jhg . fdd1jx2t0b ; systemRan [ 601 ] = ( sysRanDType * ) & rtDW .
b5umakrf53 . kq4t2lkn4n ; systemRan [ 602 ] = ( sysRanDType * ) & rtDW .
a4ebhrt5bq . hjywrjdfvg ; systemRan [ 603 ] = ( sysRanDType * ) & rtDW .
ep3fctwraa . fdd1jx2t0b ; systemRan [ 604 ] = ( sysRanDType * ) & rtDW .
cnueqx4gvt . kq4t2lkn4n ; systemRan [ 605 ] = ( sysRanDType * ) & rtDW .
grl2yxc1of . hjywrjdfvg ; systemRan [ 606 ] = ( sysRanDType * ) & rtDW .
kk13i4dykq . fdd1jx2t0b ; systemRan [ 607 ] = ( sysRanDType * ) & rtDW .
ku2xodfghq . kq4t2lkn4n ; systemRan [ 608 ] = ( sysRanDType * ) & rtDW .
k050vsxh1j . hjywrjdfvg ; systemRan [ 609 ] = & rtAlwaysEnabled ; systemRan [
610 ] = & rtAlwaysEnabled ; systemRan [ 611 ] = & rtAlwaysEnabled ; systemRan
[ 612 ] = & rtAlwaysEnabled ; systemRan [ 613 ] = & rtAlwaysEnabled ;
systemRan [ 614 ] = & rtAlwaysEnabled ; systemRan [ 615 ] = & rtAlwaysEnabled
; systemRan [ 616 ] = & rtAlwaysEnabled ; systemRan [ 617 ] = &
rtAlwaysEnabled ; systemRan [ 618 ] = & rtAlwaysEnabled ; systemRan [ 619 ] =
& rtAlwaysEnabled ; systemRan [ 620 ] = & rtAlwaysEnabled ; systemRan [ 621 ]
= & rtAlwaysEnabled ; systemRan [ 622 ] = & rtAlwaysEnabled ; systemRan [ 623
] = & rtAlwaysEnabled ; systemRan [ 624 ] = & rtAlwaysEnabled ; systemRan [
625 ] = & rtAlwaysEnabled ; systemRan [ 626 ] = & rtAlwaysEnabled ; systemRan
[ 627 ] = & rtAlwaysEnabled ; systemRan [ 628 ] = & rtAlwaysEnabled ;
systemRan [ 629 ] = & rtAlwaysEnabled ; systemRan [ 630 ] = & rtAlwaysEnabled
; systemRan [ 631 ] = & rtAlwaysEnabled ; systemRan [ 632 ] = &
rtAlwaysEnabled ; systemRan [ 633 ] = & rtAlwaysEnabled ; systemRan [ 634 ] =
& rtAlwaysEnabled ; systemRan [ 635 ] = & rtAlwaysEnabled ; systemRan [ 636 ]
= & rtAlwaysEnabled ; systemRan [ 637 ] = & rtAlwaysEnabled ; systemRan [ 638
] = & rtAlwaysEnabled ; systemRan [ 639 ] = & rtAlwaysEnabled ; systemRan [
640 ] = & rtAlwaysEnabled ; systemRan [ 641 ] = & rtAlwaysEnabled ; systemRan
[ 642 ] = & rtAlwaysEnabled ; systemRan [ 643 ] = & rtAlwaysEnabled ;
systemRan [ 644 ] = & rtAlwaysEnabled ; systemRan [ 645 ] = & rtAlwaysEnabled
; systemRan [ 646 ] = & rtAlwaysEnabled ; systemRan [ 647 ] = &
rtAlwaysEnabled ; systemRan [ 648 ] = & rtAlwaysEnabled ; systemRan [ 649 ] =
& rtAlwaysEnabled ; systemRan [ 650 ] = & rtAlwaysEnabled ; systemRan [ 651 ]
= & rtAlwaysEnabled ; systemRan [ 652 ] = & rtAlwaysEnabled ; systemRan [ 653
] = & rtAlwaysEnabled ; systemRan [ 654 ] = & rtAlwaysEnabled ; systemRan [
655 ] = & rtAlwaysEnabled ; systemRan [ 656 ] = & rtAlwaysEnabled ; systemRan
[ 657 ] = & rtAlwaysEnabled ; systemRan [ 658 ] = & rtAlwaysEnabled ;
systemRan [ 659 ] = & rtAlwaysEnabled ; systemRan [ 660 ] = & rtAlwaysEnabled
; systemRan [ 661 ] = & rtAlwaysEnabled ; systemRan [ 662 ] = &
rtAlwaysEnabled ; systemRan [ 663 ] = & rtAlwaysEnabled ; systemRan [ 664 ] =
& rtAlwaysEnabled ; systemRan [ 665 ] = & rtAlwaysEnabled ; systemRan [ 666 ]
= & rtAlwaysEnabled ; systemRan [ 667 ] = & rtAlwaysEnabled ; systemRan [ 668
] = & rtAlwaysEnabled ; systemRan [ 669 ] = & rtAlwaysEnabled ; systemRan [
670 ] = & rtAlwaysEnabled ; systemRan [ 671 ] = & rtAlwaysEnabled ; systemRan
[ 672 ] = & rtAlwaysEnabled ; systemRan [ 673 ] = & rtAlwaysEnabled ;
systemRan [ 674 ] = & rtAlwaysEnabled ; systemRan [ 675 ] = & rtAlwaysEnabled
; systemRan [ 676 ] = & rtAlwaysEnabled ; systemRan [ 677 ] = &
rtAlwaysEnabled ; systemRan [ 678 ] = & rtAlwaysEnabled ; systemRan [ 679 ] =
& rtAlwaysEnabled ; systemRan [ 680 ] = & rtAlwaysEnabled ; systemRan [ 681 ]
= & rtAlwaysEnabled ; systemRan [ 682 ] = & rtAlwaysEnabled ; systemRan [ 683
] = & rtAlwaysEnabled ; systemRan [ 684 ] = & rtAlwaysEnabled ; systemRan [
685 ] = & rtAlwaysEnabled ; systemRan [ 686 ] = & rtAlwaysEnabled ; systemRan
[ 687 ] = & rtAlwaysEnabled ; systemRan [ 688 ] = & rtAlwaysEnabled ;
systemRan [ 689 ] = & rtAlwaysEnabled ; systemRan [ 690 ] = & rtAlwaysEnabled
; systemRan [ 691 ] = & rtAlwaysEnabled ; systemRan [ 692 ] = &
rtAlwaysEnabled ; systemRan [ 693 ] = & rtAlwaysEnabled ; systemRan [ 694 ] =
& rtAlwaysEnabled ; systemRan [ 695 ] = & rtAlwaysEnabled ; systemRan [ 696 ]
= & rtAlwaysEnabled ; systemRan [ 697 ] = & rtAlwaysEnabled ; systemRan [ 698
] = & rtAlwaysEnabled ; systemRan [ 699 ] = & rtAlwaysEnabled ; systemRan [
700 ] = & rtAlwaysEnabled ; systemRan [ 701 ] = ( sysRanDType * ) & rtDW .
boy5y1bmz3 . fx2esogudmq . fdd1jx2t0b ; systemRan [ 702 ] = ( sysRanDType * )
& rtDW . boy5y1bmz3 . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 703 ] = (
sysRanDType * ) & rtDW . boy5y1bmz3 . itcd43cpffs . hjywrjdfvg ; systemRan [
704 ] = ( sysRanDType * ) & rtDW . boy5y1bmz3 . c3fzgpzsmi ; systemRan [ 705
] = ( sysRanDType * ) & rtDW . jvoggbn2yd . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 706 ] = ( sysRanDType * ) & rtDW . jvoggbn2yd . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 707 ] = ( sysRanDType * ) & rtDW . jvoggbn2yd .
cascrfypvi . hjywrjdfvg ; systemRan [ 708 ] = ( sysRanDType * ) & rtDW .
jvoggbn2yd . pbtm3vaf5b ; systemRan [ 709 ] = ( sysRanDType * ) & rtDW .
ojnin3nhvw . fx2esogudmq . fdd1jx2t0b ; systemRan [ 710 ] = ( sysRanDType * )
& rtDW . ojnin3nhvw . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 711 ] = (
sysRanDType * ) & rtDW . ojnin3nhvw . itcd43cpffs . hjywrjdfvg ; systemRan [
712 ] = ( sysRanDType * ) & rtDW . ojnin3nhvw . c3fzgpzsmi ; systemRan [ 713
] = ( sysRanDType * ) & rtDW . ithg0f1jiq . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 714 ] = ( sysRanDType * ) & rtDW . ithg0f1jiq . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 715 ] = ( sysRanDType * ) & rtDW . ithg0f1jiq .
cascrfypvi . hjywrjdfvg ; systemRan [ 716 ] = ( sysRanDType * ) & rtDW .
ithg0f1jiq . pbtm3vaf5b ; systemRan [ 717 ] = ( sysRanDType * ) & rtDW .
bhbl51zywo . fx2esogudmq . fdd1jx2t0b ; systemRan [ 718 ] = ( sysRanDType * )
& rtDW . bhbl51zywo . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 719 ] = (
sysRanDType * ) & rtDW . bhbl51zywo . itcd43cpffs . hjywrjdfvg ; systemRan [
720 ] = ( sysRanDType * ) & rtDW . bhbl51zywo . c3fzgpzsmi ; systemRan [ 721
] = ( sysRanDType * ) & rtDW . p2xsu5tknt . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 722 ] = ( sysRanDType * ) & rtDW . p2xsu5tknt . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 723 ] = ( sysRanDType * ) & rtDW . p2xsu5tknt .
cascrfypvi . hjywrjdfvg ; systemRan [ 724 ] = ( sysRanDType * ) & rtDW .
p2xsu5tknt . pbtm3vaf5b ; systemRan [ 725 ] = & rtAlwaysEnabled ; systemRan [
726 ] = ( sysRanDType * ) & rtDW . allgw5ptgk . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 727 ] = ( sysRanDType * ) & rtDW . allgw5ptgk . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 728 ] = ( sysRanDType * ) & rtDW . allgw5ptgk .
itcd43cpffs . hjywrjdfvg ; systemRan [ 729 ] = ( sysRanDType * ) & rtDW .
allgw5ptgk . c3fzgpzsmi ; systemRan [ 730 ] = ( sysRanDType * ) & rtDW .
hvibrw2awu . hirdn25ewp . fdd1jx2t0b ; systemRan [ 731 ] = ( sysRanDType * )
& rtDW . hvibrw2awu . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 732 ] = (
sysRanDType * ) & rtDW . hvibrw2awu . cascrfypvi . hjywrjdfvg ; systemRan [
733 ] = ( sysRanDType * ) & rtDW . hvibrw2awu . pbtm3vaf5b ; systemRan [ 734
] = ( sysRanDType * ) & rtDW . br1i12egyj . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 735 ] = ( sysRanDType * ) & rtDW . br1i12egyj . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 736 ] = ( sysRanDType * ) & rtDW . br1i12egyj .
itcd43cpffs . hjywrjdfvg ; systemRan [ 737 ] = ( sysRanDType * ) & rtDW .
br1i12egyj . c3fzgpzsmi ; systemRan [ 738 ] = ( sysRanDType * ) & rtDW .
f5lqeh5sbp . hirdn25ewp . fdd1jx2t0b ; systemRan [ 739 ] = ( sysRanDType * )
& rtDW . f5lqeh5sbp . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 740 ] = (
sysRanDType * ) & rtDW . f5lqeh5sbp . cascrfypvi . hjywrjdfvg ; systemRan [
741 ] = ( sysRanDType * ) & rtDW . f5lqeh5sbp . pbtm3vaf5b ; systemRan [ 742
] = & rtAlwaysEnabled ; systemRan [ 743 ] = ( sysRanDType * ) & rtDW .
dewlul2b2r . fx2esogudmq . fdd1jx2t0b ; systemRan [ 744 ] = ( sysRanDType * )
& rtDW . dewlul2b2r . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 745 ] = (
sysRanDType * ) & rtDW . dewlul2b2r . itcd43cpffs . hjywrjdfvg ; systemRan [
746 ] = ( sysRanDType * ) & rtDW . dewlul2b2r . c3fzgpzsmi ; systemRan [ 747
] = ( sysRanDType * ) & rtDW . lf152u5jm4 . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 748 ] = ( sysRanDType * ) & rtDW . lf152u5jm4 . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 749 ] = ( sysRanDType * ) & rtDW . lf152u5jm4 .
cascrfypvi . hjywrjdfvg ; systemRan [ 750 ] = ( sysRanDType * ) & rtDW .
lf152u5jm4 . pbtm3vaf5b ; systemRan [ 751 ] = ( sysRanDType * ) & rtDW .
kd31jkgljk . fx2esogudmq . fdd1jx2t0b ; systemRan [ 752 ] = ( sysRanDType * )
& rtDW . kd31jkgljk . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 753 ] = (
sysRanDType * ) & rtDW . kd31jkgljk . itcd43cpffs . hjywrjdfvg ; systemRan [
754 ] = ( sysRanDType * ) & rtDW . kd31jkgljk . c3fzgpzsmi ; systemRan [ 755
] = ( sysRanDType * ) & rtDW . ivzhqxkykj . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 756 ] = ( sysRanDType * ) & rtDW . ivzhqxkykj . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 757 ] = ( sysRanDType * ) & rtDW . ivzhqxkykj .
cascrfypvi . hjywrjdfvg ; systemRan [ 758 ] = ( sysRanDType * ) & rtDW .
ivzhqxkykj . pbtm3vaf5b ; systemRan [ 759 ] = & rtAlwaysEnabled ; systemRan [
760 ] = ( sysRanDType * ) & rtDW . d5dfks3vhb . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 761 ] = ( sysRanDType * ) & rtDW . d5dfks3vhb . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 762 ] = ( sysRanDType * ) & rtDW . d5dfks3vhb .
itcd43cpffs . hjywrjdfvg ; systemRan [ 763 ] = ( sysRanDType * ) & rtDW .
d5dfks3vhb . c3fzgpzsmi ; systemRan [ 764 ] = ( sysRanDType * ) & rtDW .
b13ln335zj . hirdn25ewp . fdd1jx2t0b ; systemRan [ 765 ] = ( sysRanDType * )
& rtDW . b13ln335zj . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 766 ] = (
sysRanDType * ) & rtDW . b13ln335zj . cascrfypvi . hjywrjdfvg ; systemRan [
767 ] = ( sysRanDType * ) & rtDW . b13ln335zj . pbtm3vaf5b ; systemRan [ 768
] = ( sysRanDType * ) & rtDW . jcev1klzwf . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 769 ] = ( sysRanDType * ) & rtDW . jcev1klzwf . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 770 ] = ( sysRanDType * ) & rtDW . jcev1klzwf .
itcd43cpffs . hjywrjdfvg ; systemRan [ 771 ] = ( sysRanDType * ) & rtDW .
jcev1klzwf . c3fzgpzsmi ; systemRan [ 772 ] = ( sysRanDType * ) & rtDW .
d2dryokhth . hirdn25ewp . fdd1jx2t0b ; systemRan [ 773 ] = ( sysRanDType * )
& rtDW . d2dryokhth . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 774 ] = (
sysRanDType * ) & rtDW . d2dryokhth . cascrfypvi . hjywrjdfvg ; systemRan [
775 ] = ( sysRanDType * ) & rtDW . d2dryokhth . pbtm3vaf5b ; systemRan [ 776
] = ( sysRanDType * ) & rtDW . nx03ta252g . fdd1jx2t0b ; systemRan [ 777 ] =
( sysRanDType * ) & rtDW . fh4f1ldkj5 . kq4t2lkn4n ; systemRan [ 778 ] = (
sysRanDType * ) & rtDW . j1rcgjyzg3 . hjywrjdfvg ; systemRan [ 779 ] = (
sysRanDType * ) & rtDW . j33xaeouj5 . fdd1jx2t0b ; systemRan [ 780 ] = (
sysRanDType * ) & rtDW . dymcbszxp2 . kq4t2lkn4n ; systemRan [ 781 ] = (
sysRanDType * ) & rtDW . culgjirjhd . hjywrjdfvg ; systemRan [ 782 ] = (
sysRanDType * ) & rtDW . pk2r3vb4fa . fdd1jx2t0b ; systemRan [ 783 ] = (
sysRanDType * ) & rtDW . mixbw55a4u . kq4t2lkn4n ; systemRan [ 784 ] = (
sysRanDType * ) & rtDW . dxl224bky1 . hjywrjdfvg ; systemRan [ 785 ] = &
rtAlwaysEnabled ; systemRan [ 786 ] = & rtAlwaysEnabled ; systemRan [ 787 ] =
& rtAlwaysEnabled ; systemRan [ 788 ] = & rtAlwaysEnabled ; systemRan [ 789 ]
= & rtAlwaysEnabled ; systemRan [ 790 ] = & rtAlwaysEnabled ; systemRan [ 791
] = ( sysRanDType * ) & rtDW . pj2a3t0x3l . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 792 ] = ( sysRanDType * ) & rtDW . pj2a3t0x3l . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 793 ] = ( sysRanDType * ) & rtDW . pj2a3t0x3l .
itcd43cpffs . hjywrjdfvg ; systemRan [ 794 ] = ( sysRanDType * ) & rtDW .
pj2a3t0x3l . c3fzgpzsmi ; systemRan [ 795 ] = ( sysRanDType * ) & rtDW .
c2rt0zs300 . hirdn25ewp . fdd1jx2t0b ; systemRan [ 796 ] = ( sysRanDType * )
& rtDW . c2rt0zs300 . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 797 ] = (
sysRanDType * ) & rtDW . c2rt0zs300 . cascrfypvi . hjywrjdfvg ; systemRan [
798 ] = ( sysRanDType * ) & rtDW . c2rt0zs300 . pbtm3vaf5b ; systemRan [ 799
] = ( sysRanDType * ) & rtDW . gk4015knwc . fdd1jx2t0b ; systemRan [ 800 ] =
( sysRanDType * ) & rtDW . j0qka4bwqf . kq4t2lkn4n ; systemRan [ 801 ] = (
sysRanDType * ) & rtDW . ctjk4b4i5h . hjywrjdfvg ; systemRan [ 802 ] = &
rtAlwaysEnabled ; systemRan [ 803 ] = & rtAlwaysEnabled ; systemRan [ 804 ] =
& rtAlwaysEnabled ; systemRan [ 805 ] = & rtAlwaysEnabled ; systemRan [ 806 ]
= & rtAlwaysEnabled ; systemRan [ 807 ] = & rtAlwaysEnabled ; systemRan [ 808
] = & rtAlwaysEnabled ; systemRan [ 809 ] = & rtAlwaysEnabled ; systemRan [
810 ] = & rtAlwaysEnabled ; systemRan [ 811 ] = & rtAlwaysEnabled ; systemRan
[ 812 ] = & rtAlwaysEnabled ; systemRan [ 813 ] = & rtAlwaysEnabled ;
systemRan [ 814 ] = & rtAlwaysEnabled ; systemRan [ 815 ] = & rtAlwaysEnabled
; systemRan [ 816 ] = & rtAlwaysEnabled ; systemRan [ 817 ] = &
rtAlwaysEnabled ; systemRan [ 818 ] = & rtAlwaysEnabled ; systemRan [ 819 ] =
& rtAlwaysEnabled ; systemRan [ 820 ] = & rtAlwaysEnabled ; systemRan [ 821 ]
= & rtAlwaysEnabled ; systemRan [ 822 ] = & rtAlwaysEnabled ; systemRan [ 823
] = ( sysRanDType * ) & rtDW . mgkfvqxti2 . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 824 ] = ( sysRanDType * ) & rtDW . mgkfvqxti2 . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 825 ] = ( sysRanDType * ) & rtDW . mgkfvqxti2 .
itcd43cpffs . hjywrjdfvg ; systemRan [ 826 ] = ( sysRanDType * ) & rtDW .
mgkfvqxti2 . c3fzgpzsmi ; systemRan [ 827 ] = ( sysRanDType * ) & rtDW .
hytx1hl5s5 . hirdn25ewp . fdd1jx2t0b ; systemRan [ 828 ] = ( sysRanDType * )
& rtDW . hytx1hl5s5 . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 829 ] = (
sysRanDType * ) & rtDW . hytx1hl5s5 . cascrfypvi . hjywrjdfvg ; systemRan [
830 ] = ( sysRanDType * ) & rtDW . hytx1hl5s5 . pbtm3vaf5b ; systemRan [ 831
] = ( sysRanDType * ) & rtDW . h145orsdu5 . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 832 ] = ( sysRanDType * ) & rtDW . h145orsdu5 . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 833 ] = ( sysRanDType * ) & rtDW . h145orsdu5 .
itcd43cpffs . hjywrjdfvg ; systemRan [ 834 ] = ( sysRanDType * ) & rtDW .
h145orsdu5 . c3fzgpzsmi ; systemRan [ 835 ] = ( sysRanDType * ) & rtDW .
lfrovwpki3 . hirdn25ewp . fdd1jx2t0b ; systemRan [ 836 ] = ( sysRanDType * )
& rtDW . lfrovwpki3 . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 837 ] = (
sysRanDType * ) & rtDW . lfrovwpki3 . cascrfypvi . hjywrjdfvg ; systemRan [
838 ] = ( sysRanDType * ) & rtDW . lfrovwpki3 . pbtm3vaf5b ; systemRan [ 839
] = ( sysRanDType * ) & rtDW . obeddxl4xp . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 840 ] = ( sysRanDType * ) & rtDW . obeddxl4xp . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 841 ] = ( sysRanDType * ) & rtDW . obeddxl4xp .
itcd43cpffs . hjywrjdfvg ; systemRan [ 842 ] = ( sysRanDType * ) & rtDW .
obeddxl4xp . c3fzgpzsmi ; systemRan [ 843 ] = ( sysRanDType * ) & rtDW .
inwm331xkg . hirdn25ewp . fdd1jx2t0b ; systemRan [ 844 ] = ( sysRanDType * )
& rtDW . inwm331xkg . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 845 ] = (
sysRanDType * ) & rtDW . inwm331xkg . cascrfypvi . hjywrjdfvg ; systemRan [
846 ] = ( sysRanDType * ) & rtDW . inwm331xkg . pbtm3vaf5b ; systemRan [ 847
] = ( sysRanDType * ) & rtDW . dlrk53odir . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 848 ] = ( sysRanDType * ) & rtDW . dlrk53odir . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 849 ] = ( sysRanDType * ) & rtDW . dlrk53odir .
itcd43cpffs . hjywrjdfvg ; systemRan [ 850 ] = ( sysRanDType * ) & rtDW .
dlrk53odir . c3fzgpzsmi ; systemRan [ 851 ] = ( sysRanDType * ) & rtDW .
nci0imhwrv . hirdn25ewp . fdd1jx2t0b ; systemRan [ 852 ] = ( sysRanDType * )
& rtDW . nci0imhwrv . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 853 ] = (
sysRanDType * ) & rtDW . nci0imhwrv . cascrfypvi . hjywrjdfvg ; systemRan [
854 ] = ( sysRanDType * ) & rtDW . nci0imhwrv . pbtm3vaf5b ; systemRan [ 855
] = ( sysRanDType * ) & rtDW . llpwcowx1f . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 856 ] = ( sysRanDType * ) & rtDW . llpwcowx1f . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 857 ] = ( sysRanDType * ) & rtDW . llpwcowx1f .
itcd43cpffs . hjywrjdfvg ; systemRan [ 858 ] = ( sysRanDType * ) & rtDW .
llpwcowx1f . c3fzgpzsmi ; systemRan [ 859 ] = ( sysRanDType * ) & rtDW .
crzn5z4zte . hirdn25ewp . fdd1jx2t0b ; systemRan [ 860 ] = ( sysRanDType * )
& rtDW . crzn5z4zte . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 861 ] = (
sysRanDType * ) & rtDW . crzn5z4zte . cascrfypvi . hjywrjdfvg ; systemRan [
862 ] = ( sysRanDType * ) & rtDW . crzn5z4zte . pbtm3vaf5b ; systemRan [ 863
] = ( sysRanDType * ) & rtDW . cq2uqua4yo . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 864 ] = ( sysRanDType * ) & rtDW . cq2uqua4yo . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 865 ] = ( sysRanDType * ) & rtDW . cq2uqua4yo .
itcd43cpffs . hjywrjdfvg ; systemRan [ 866 ] = ( sysRanDType * ) & rtDW .
cq2uqua4yo . c3fzgpzsmi ; systemRan [ 867 ] = ( sysRanDType * ) & rtDW .
lbq0bgvgcl . hirdn25ewp . fdd1jx2t0b ; systemRan [ 868 ] = ( sysRanDType * )
& rtDW . lbq0bgvgcl . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 869 ] = (
sysRanDType * ) & rtDW . lbq0bgvgcl . cascrfypvi . hjywrjdfvg ; systemRan [
870 ] = ( sysRanDType * ) & rtDW . lbq0bgvgcl . pbtm3vaf5b ; systemRan [ 871
] = ( sysRanDType * ) & rtDW . dlwrrwcsx5 . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 872 ] = ( sysRanDType * ) & rtDW . dlwrrwcsx5 . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 873 ] = ( sysRanDType * ) & rtDW . dlwrrwcsx5 .
itcd43cpffs . hjywrjdfvg ; systemRan [ 874 ] = ( sysRanDType * ) & rtDW .
dlwrrwcsx5 . c3fzgpzsmi ; systemRan [ 875 ] = ( sysRanDType * ) & rtDW .
mpydob1efk . hirdn25ewp . fdd1jx2t0b ; systemRan [ 876 ] = ( sysRanDType * )
& rtDW . mpydob1efk . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 877 ] = (
sysRanDType * ) & rtDW . mpydob1efk . cascrfypvi . hjywrjdfvg ; systemRan [
878 ] = ( sysRanDType * ) & rtDW . mpydob1efk . pbtm3vaf5b ; systemRan [ 879
] = & rtAlwaysEnabled ; systemRan [ 880 ] = ( sysRanDType * ) & rtDW .
hh2cmxftpw . fx2esogudmq . fdd1jx2t0b ; systemRan [ 881 ] = ( sysRanDType * )
& rtDW . hh2cmxftpw . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 882 ] = (
sysRanDType * ) & rtDW . hh2cmxftpw . itcd43cpffs . hjywrjdfvg ; systemRan [
883 ] = ( sysRanDType * ) & rtDW . hh2cmxftpw . c3fzgpzsmi ; systemRan [ 884
] = ( sysRanDType * ) & rtDW . lkrts5ohpw . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 885 ] = ( sysRanDType * ) & rtDW . lkrts5ohpw . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 886 ] = ( sysRanDType * ) & rtDW . lkrts5ohpw .
cascrfypvi . hjywrjdfvg ; systemRan [ 887 ] = ( sysRanDType * ) & rtDW .
lkrts5ohpw . pbtm3vaf5b ; systemRan [ 888 ] = ( sysRanDType * ) & rtDW .
dexvjkg0hj . fx2esogudmq . fdd1jx2t0b ; systemRan [ 889 ] = ( sysRanDType * )
& rtDW . dexvjkg0hj . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 890 ] = (
sysRanDType * ) & rtDW . dexvjkg0hj . itcd43cpffs . hjywrjdfvg ; systemRan [
891 ] = ( sysRanDType * ) & rtDW . dexvjkg0hj . c3fzgpzsmi ; systemRan [ 892
] = ( sysRanDType * ) & rtDW . me023aolbi . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 893 ] = ( sysRanDType * ) & rtDW . me023aolbi . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 894 ] = ( sysRanDType * ) & rtDW . me023aolbi .
cascrfypvi . hjywrjdfvg ; systemRan [ 895 ] = ( sysRanDType * ) & rtDW .
me023aolbi . pbtm3vaf5b ; systemRan [ 896 ] = & rtAlwaysEnabled ; systemRan [
897 ] = ( sysRanDType * ) & rtDW . g3bxbyypfb . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 898 ] = ( sysRanDType * ) & rtDW . g3bxbyypfb . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 899 ] = ( sysRanDType * ) & rtDW . g3bxbyypfb .
itcd43cpffs . hjywrjdfvg ; systemRan [ 900 ] = ( sysRanDType * ) & rtDW .
g3bxbyypfb . c3fzgpzsmi ; systemRan [ 901 ] = ( sysRanDType * ) & rtDW .
purgdi1jfk . hirdn25ewp . fdd1jx2t0b ; systemRan [ 902 ] = ( sysRanDType * )
& rtDW . purgdi1jfk . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 903 ] = (
sysRanDType * ) & rtDW . purgdi1jfk . cascrfypvi . hjywrjdfvg ; systemRan [
904 ] = ( sysRanDType * ) & rtDW . purgdi1jfk . pbtm3vaf5b ; systemRan [ 905
] = ( sysRanDType * ) & rtDW . bs5m40ynfy . fx2esogudmq . fdd1jx2t0b ;
systemRan [ 906 ] = ( sysRanDType * ) & rtDW . bs5m40ynfy . cgvj5xjnjkh .
kq4t2lkn4n ; systemRan [ 907 ] = ( sysRanDType * ) & rtDW . bs5m40ynfy .
itcd43cpffs . hjywrjdfvg ; systemRan [ 908 ] = ( sysRanDType * ) & rtDW .
bs5m40ynfy . c3fzgpzsmi ; systemRan [ 909 ] = ( sysRanDType * ) & rtDW .
e4qyv4smfd . hirdn25ewp . fdd1jx2t0b ; systemRan [ 910 ] = ( sysRanDType * )
& rtDW . e4qyv4smfd . cuc3rxn500 . kq4t2lkn4n ; systemRan [ 911 ] = (
sysRanDType * ) & rtDW . e4qyv4smfd . cascrfypvi . hjywrjdfvg ; systemRan [
912 ] = ( sysRanDType * ) & rtDW . e4qyv4smfd . pbtm3vaf5b ; systemRan [ 913
] = & rtAlwaysEnabled ; systemRan [ 914 ] = ( sysRanDType * ) & rtDW .
ngrhemover . fx2esogudmq . fdd1jx2t0b ; systemRan [ 915 ] = ( sysRanDType * )
& rtDW . ngrhemover . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 916 ] = (
sysRanDType * ) & rtDW . ngrhemover . itcd43cpffs . hjywrjdfvg ; systemRan [
917 ] = ( sysRanDType * ) & rtDW . ngrhemover . c3fzgpzsmi ; systemRan [ 918
] = ( sysRanDType * ) & rtDW . lu12zasvyl . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 919 ] = ( sysRanDType * ) & rtDW . lu12zasvyl . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 920 ] = ( sysRanDType * ) & rtDW . lu12zasvyl .
cascrfypvi . hjywrjdfvg ; systemRan [ 921 ] = ( sysRanDType * ) & rtDW .
lu12zasvyl . pbtm3vaf5b ; systemRan [ 922 ] = ( sysRanDType * ) & rtDW .
onemmt3xbs . fx2esogudmq . fdd1jx2t0b ; systemRan [ 923 ] = ( sysRanDType * )
& rtDW . onemmt3xbs . cgvj5xjnjkh . kq4t2lkn4n ; systemRan [ 924 ] = (
sysRanDType * ) & rtDW . onemmt3xbs . itcd43cpffs . hjywrjdfvg ; systemRan [
925 ] = ( sysRanDType * ) & rtDW . onemmt3xbs . c3fzgpzsmi ; systemRan [ 926
] = ( sysRanDType * ) & rtDW . myp2dcpbvg . hirdn25ewp . fdd1jx2t0b ;
systemRan [ 927 ] = ( sysRanDType * ) & rtDW . myp2dcpbvg . cuc3rxn500 .
kq4t2lkn4n ; systemRan [ 928 ] = ( sysRanDType * ) & rtDW . myp2dcpbvg .
cascrfypvi . hjywrjdfvg ; systemRan [ 929 ] = ( sysRanDType * ) & rtDW .
myp2dcpbvg . pbtm3vaf5b ; systemRan [ 930 ] = & rtAlwaysEnabled ; systemRan [
931 ] = & rtAlwaysEnabled ; systemRan [ 932 ] = ( sysRanDType * ) & rtDW .
ivtty1aemy . fdd1jx2t0b ; systemRan [ 933 ] = ( sysRanDType * ) & rtDW .
lrmch0dvn3 . kq4t2lkn4n ; systemRan [ 934 ] = ( sysRanDType * ) & rtDW .
oo1qtpn5j5 . hjywrjdfvg ; systemRan [ 935 ] = ( sysRanDType * ) & rtDW .
mvirue1zit . fdd1jx2t0b ; systemRan [ 936 ] = ( sysRanDType * ) & rtDW .
c2mshkhisd . kq4t2lkn4n ; systemRan [ 937 ] = ( sysRanDType * ) & rtDW .
kuwwc0aum4 . hjywrjdfvg ; systemRan [ 938 ] = ( sysRanDType * ) & rtDW .
f3zha40yok . fdd1jx2t0b ; systemRan [ 939 ] = ( sysRanDType * ) & rtDW .
jxw41swge5 . kq4t2lkn4n ; systemRan [ 940 ] = ( sysRanDType * ) & rtDW .
hwrjddg40d . hjywrjdfvg ; systemRan [ 941 ] = ( sysRanDType * ) & rtDW .
eeuhsl20cs ; systemRan [ 942 ] = ( sysRanDType * ) & rtDW . eeuhsl20cs ;
systemRan [ 943 ] = ( sysRanDType * ) & rtDW . kseqhf442uv . fsmzugaw0h ;
systemRan [ 944 ] = ( sysRanDType * ) & rtDW . cgsqxgexc24 . kdh1m4autw ;
systemRan [ 945 ] = ( sysRanDType * ) & rtDW . eeuhsl20cs ; systemRan [ 946 ]
= & rtAlwaysEnabled ; systemRan [ 947 ] = ( sysRanDType * ) & rtDW .
n3ncmzva14 . fsmzugaw0h ; systemRan [ 948 ] = ( sysRanDType * ) & rtDW .
dx1zpxayg3 . kdh1m4autw ; systemRan [ 949 ] = & rtAlwaysEnabled ; systemRan [
950 ] = & rtAlwaysEnabled ; systemRan [ 951 ] = & rtAlwaysEnabled ; systemRan
[ 952 ] = & rtAlwaysEnabled ; systemRan [ 953 ] = & rtAlwaysEnabled ;
systemRan [ 954 ] = & rtAlwaysEnabled ; systemRan [ 955 ] = & rtAlwaysEnabled
; systemRan [ 956 ] = & rtAlwaysEnabled ; systemRan [ 957 ] = &
rtAlwaysEnabled ; systemRan [ 958 ] = & rtAlwaysEnabled ; systemRan [ 959 ] =
& rtAlwaysEnabled ; systemRan [ 960 ] = & rtAlwaysEnabled ; systemRan [ 961 ]
= & rtAlwaysEnabled ; systemRan [ 962 ] = & rtAlwaysEnabled ; systemRan [ 963
] = & rtAlwaysEnabled ; systemRan [ 964 ] = & rtAlwaysEnabled ; systemRan [
965 ] = & rtAlwaysEnabled ; systemRan [ 966 ] = & rtAlwaysEnabled ; systemRan
[ 967 ] = & rtAlwaysEnabled ; systemRan [ 968 ] = & rtAlwaysEnabled ;
systemRan [ 969 ] = & rtAlwaysEnabled ; systemRan [ 970 ] = & rtAlwaysEnabled
; systemRan [ 971 ] = & rtAlwaysEnabled ; systemRan [ 972 ] = &
rtAlwaysEnabled ; systemRan [ 973 ] = & rtAlwaysEnabled ; systemRan [ 974 ] =
& rtAlwaysEnabled ; systemRan [ 975 ] = & rtAlwaysEnabled ; systemRan [ 976 ]
= & rtAlwaysEnabled ; systemRan [ 977 ] = & rtAlwaysEnabled ; systemRan [ 978
] = & rtAlwaysEnabled ; systemRan [ 979 ] = & rtAlwaysEnabled ; systemRan [
980 ] = & rtAlwaysEnabled ; systemRan [ 981 ] = & rtAlwaysEnabled ; systemRan
[ 982 ] = & rtAlwaysEnabled ; systemRan [ 983 ] = ( sysRanDType * ) & rtDW .
jvoeh2x3far . iwk0frtfal ; systemRan [ 984 ] = ( sysRanDType * ) & rtDW .
dg2ggauitn . iwk0frtfal ; systemRan [ 985 ] = ( sysRanDType * ) & rtDW .
czm0w3hmwho . mjsgo5sz1r ; systemRan [ 986 ] = & rtAlwaysEnabled ; systemRan
[ 987 ] = & rtAlwaysEnabled ; systemRan [ 988 ] = & rtAlwaysEnabled ;
systemRan [ 989 ] = & rtAlwaysEnabled ; systemRan [ 990 ] = & rtAlwaysEnabled
; systemRan [ 991 ] = & rtAlwaysEnabled ; systemRan [ 992 ] = ( sysRanDType *
) & rtDW . no1v55ban3 . iwk0frtfal ; systemRan [ 993 ] = ( sysRanDType * ) &
rtDW . mb5y4fcaem . iwk0frtfal ; systemRan [ 994 ] = ( sysRanDType * ) & rtDW
. pgdbd0g5cb . mjsgo5sz1r ; systemRan [ 995 ] = & rtAlwaysEnabled ; systemRan
[ 996 ] = & rtAlwaysEnabled ; systemRan [ 997 ] = & rtAlwaysEnabled ;
systemRan [ 998 ] = & rtAlwaysEnabled ; systemRan [ 999 ] = & rtAlwaysEnabled
; systemRan [ 1000 ] = & rtAlwaysEnabled ; systemRan [ 1001 ] = &
rtAlwaysEnabled ; systemRan [ 1002 ] = & rtAlwaysEnabled ; systemRan [ 1003 ]
= & rtAlwaysEnabled ; systemRan [ 1004 ] = & rtAlwaysEnabled ; systemRan [
1005 ] = & rtAlwaysEnabled ; systemRan [ 1006 ] = & rtAlwaysEnabled ;
systemRan [ 1007 ] = & rtAlwaysEnabled ; systemRan [ 1008 ] = &
rtAlwaysEnabled ; systemRan [ 1009 ] = & rtAlwaysEnabled ; systemRan [ 1010 ]
= & rtAlwaysEnabled ; systemRan [ 1011 ] = & rtAlwaysEnabled ; systemRan [
1012 ] = & rtAlwaysEnabled ; systemRan [ 1013 ] = & rtAlwaysEnabled ;
systemRan [ 1014 ] = & rtAlwaysEnabled ; systemRan [ 1015 ] = &
rtAlwaysEnabled ; systemRan [ 1016 ] = & rtAlwaysEnabled ; systemRan [ 1017 ]
= & rtAlwaysEnabled ; systemRan [ 1018 ] = & rtAlwaysEnabled ; systemRan [
1019 ] = & rtAlwaysEnabled ; systemRan [ 1020 ] = & rtAlwaysEnabled ;
systemRan [ 1021 ] = & rtAlwaysEnabled ; systemRan [ 1022 ] = &
rtAlwaysEnabled ; systemRan [ 1023 ] = ( sysRanDType * ) & rtDW . dem3so030k
. iwk0frtfal ; systemRan [ 1024 ] = ( sysRanDType * ) & rtDW . p4hmlcjayz .
iwk0frtfal ; systemRan [ 1025 ] = ( sysRanDType * ) & rtDW . mvaehis2rc .
mjsgo5sz1r ; systemRan [ 1026 ] = & rtAlwaysEnabled ; systemRan [ 1027 ] = &
rtAlwaysEnabled ; systemRan [ 1028 ] = & rtAlwaysEnabled ; systemRan [ 1029 ]
= & rtAlwaysEnabled ; systemRan [ 1030 ] = & rtAlwaysEnabled ; systemRan [
1031 ] = & rtAlwaysEnabled ; systemRan [ 1032 ] = ( sysRanDType * ) & rtDW .
khd3qxar1i . iwk0frtfal ; systemRan [ 1033 ] = ( sysRanDType * ) & rtDW .
gjmkpqrmc3 . iwk0frtfal ; systemRan [ 1034 ] = ( sysRanDType * ) & rtDW .
jzsygc4sa4 . mjsgo5sz1r ; systemRan [ 1035 ] = & rtAlwaysEnabled ; systemRan
[ 1036 ] = & rtAlwaysEnabled ; rteiSetModelMappingInfoPtr (
ssGetRTWExtModeInfo ( rtS ) , & ssGetModelMappingInfo ( rtS ) ) ;
rteiSetChecksumsPtr ( ssGetRTWExtModeInfo ( rtS ) , ssGetChecksums ( rtS ) )
; rteiSetTPtr ( ssGetRTWExtModeInfo ( rtS ) , ssGetTPtr ( rtS ) ) ; } rtP .
Saturation_UpperSat = rtInf ; rtP . Integrator_UpperSat = rtInf ; rtP .
Integrator_LowerSat = rtMinusInf ; rtP . Integrator_UpperSat_hkcvjiz0hz =
rtInf ; rtP . Saturation2_UpperSat = rtInf ; return rtS ; }
#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif
const int_T gblParameterTuningTid = 2 ; void MdlOutputsParameterSampleTime (
int_T tid ) { MdlOutputsTID2 ( tid ) ; }