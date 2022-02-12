#include "rt_logging_mmi.h"
#include "DZG_v6_SimGen_capi.h"
#include <math.h>
#include "DZG_v6_SimGen.h"
#include "DZG_v6_SimGen_private.h"
#include "DZG_v6_SimGen_dt.h"
extern void * CreateDiagnosticAsVoidPtr_wrapper ( const char * id , int nargs
, ... ) ; RTWExtModeInfo * gblRTWExtModeInfo = NULL ; extern boolean_T
gblExtModeStartPktReceived ; void raccelForceExtModeShutdown ( ) { if ( !
gblExtModeStartPktReceived ) { boolean_T stopRequested = false ;
rtExtModeWaitForStartPkt ( gblRTWExtModeInfo , 3 , & stopRequested ) ; }
rtExtModeShutdown ( 3 ) ; }
#include "slsv_diagnostic_codegen_c_api.h"
const int_T gblNumToFiles = 0 ; const int_T gblNumFrFiles = 0 ; const int_T
gblNumFrWksBlocks = 0 ;
#ifdef RSIM_WITH_SOLVER_MULTITASKING
boolean_T gbl_raccel_isMultitasking = 1 ;
#else
boolean_T gbl_raccel_isMultitasking = 0 ;
#endif
boolean_T gbl_raccel_tid01eq = 1 ; int_T gbl_raccel_NumST = 4 ; const char_T
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
"slprj\\raccel\\DZG_v6_SimGen\\DZG_v6_SimGen_Jpattern.mat" ; const int_T
gblNumRootInportBlks = 0 ; const int_T gblNumModelInputs = 0 ; extern
rtInportTUtable * gblInportTUtables ; extern const char * gblInportFileName ;
extern void * gblAperiodicPartitionHitTimes ; const int_T
gblInportDataTypeIdx [ ] = { - 1 } ; const int_T gblInportDims [ ] = { - 1 }
; const int_T gblInportComplex [ ] = { - 1 } ; const int_T
gblInportInterpoFlag [ ] = { - 1 } ; const int_T gblInportContinuous [ ] = {
- 1 } ; int_T enableFcnCallFlag [ ] = { 1 , 1 , 1 , 1 } ; const char *
raccelLoadInputsAndAperiodicHitTimes ( const char * inportFileName , int *
matFileFormat ) { return rt_RapidReadInportsMatFile ( inportFileName ,
matFileFormat , 1 ) ; }
#include "simstruc.h"
#include "fixedpoint.h"
B rtB ; X rtX ; DW rtDW ; static SimStruct model_S ; SimStruct * const rtS =
& model_S ;
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
real_T look1_pbinlxpw ( real_T u0 , const real_T bp0 [ ] , const real_T table
[ ] , uint32_T prevIndex [ ] , uint32_T maxIndex ) { real_T frac ; uint32_T
iRght ; uint32_T iLeft ; uint32_T bpIdx ; uint32_T found ; if ( u0 <= bp0 [
0U ] ) { bpIdx = 0U ; frac = ( u0 - bp0 [ 0U ] ) / ( bp0 [ 1U ] - bp0 [ 0U ]
) ; } else if ( u0 < bp0 [ maxIndex ] ) { bpIdx = prevIndex [ 0U ] ; iLeft =
0U ; iRght = maxIndex ; found = 0U ; while ( found == 0U ) { if ( u0 < bp0 [
bpIdx ] ) { iRght = bpIdx - 1U ; bpIdx = ( iRght + iLeft ) >> 1U ; } else if
( u0 < bp0 [ bpIdx + 1U ] ) { found = 1U ; } else { iLeft = bpIdx + 1U ;
bpIdx = ( iRght + iLeft ) >> 1U ; } } frac = ( u0 - bp0 [ bpIdx ] ) / ( bp0 [
bpIdx + 1U ] - bp0 [ bpIdx ] ) ; } else { bpIdx = maxIndex - 1U ; frac = ( u0
- bp0 [ maxIndex - 1U ] ) / ( bp0 [ maxIndex ] - bp0 [ maxIndex - 1U ] ) ; }
prevIndex [ 0U ] = bpIdx ; return ( table [ bpIdx + 1U ] - table [ bpIdx ] )
* frac + table [ bpIdx ] ; }
#ifndef __RTW_UTFREE__  
extern void utFree ( void * ) ;
#endif
void rt_TDelayFreeBuf ( void * buf ) { utFree ( buf ) ; } void rt_invd6x6_snf
( const real_T u [ 36 ] , real_T y [ 36 ] ) { real_T u_p [ 36 ] ; int8_T p [
6 ] ; int8_T ipiv [ 6 ] ; int32_T ipk ; int32_T pipk ; int32_T jm1 ; int32_T
mmj ; int32_T jj ; int32_T jp1j ; int32_T jpiv_offset ; int32_T Aoffset ;
int32_T b ; real_T x [ 36 ] ; int32_T ONE ; int32_T ix ; real_T smax ; real_T
s ; real_T u_e ; for ( ix = 0 ; ix < 36 ; ix ++ ) { u_e = u [ ix ] ; y [ ix ]
= 0.0 ; u_p [ ix ] = u_e ; } for ( ix = 0 ; ix < 6 ; ix ++ ) { ipiv [ ix ] =
( int8_T ) ( ix + 1 ) ; } for ( ipk = 0 ; ipk < 5 ; ipk ++ ) { pipk = ipk + 1
; jm1 = pipk - 1 ; mmj = 6 - pipk ; Aoffset = jm1 * 7 ; ONE = 1 ; jj =
Aoffset + 1 ; jp1j = jj + 1 ; Aoffset = mmj + 1 ; for ( ix = 0 ; ix < 36 ; ix
++ ) { u_e = u_p [ ix ] ; x [ ix ] = u_e ; } ix = jj - 1 ; u_e = x [ jj - 1 ]
; s = muDoubleScalarAbs ( u_e ) ; smax = s ; for ( jpiv_offset = 2 ;
jpiv_offset <= Aoffset ; jpiv_offset ++ ) { ix ++ ; u_e = x [ ix ] ; s =
muDoubleScalarAbs ( u_e ) ; if ( s > smax ) { ONE = jpiv_offset ; smax = s ;
} } jpiv_offset = ONE - 1 ; ONE = ( jj + jpiv_offset ) - 1 ; if ( u_p [ ONE ]
!= 0.0 ) { if ( jpiv_offset != 0 ) { Aoffset = pipk + jpiv_offset ; ipiv [
pipk - 1 ] = ( int8_T ) Aoffset ; ONE = jm1 + jpiv_offset ; for ( jpiv_offset
= 0 ; jpiv_offset < 6 ; jpiv_offset ++ ) { s = u_p [ jm1 ] ; u_p [ jm1 ] =
u_p [ ONE ] ; u_p [ ONE ] = s ; jm1 += 6 ; ONE += 6 ; } } Aoffset = mmj - 1 ;
jm1 = jp1j + Aoffset ; for ( b = jp1j ; b <= jm1 ; b ++ ) { u_e = u_p [ b - 1
] ; s = u_p [ jj - 1 ] ; smax = u_e / s ; u_p [ b - 1 ] = smax ; } } Aoffset
= 5 - pipk ; jpiv_offset = jj + 5 ; pipk = jj + 7 ; ONE = pipk - 1 ; for (
pipk = 0 ; pipk <= Aoffset ; pipk ++ ) { s = u_p [ jpiv_offset ] ; if ( s !=
0.0 ) { s = - s ; ix = jp1j - 1 ; jm1 = ONE + 1 ; jj = mmj + ONE ; for ( b =
jm1 ; b <= jj ; b ++ ) { u_p [ b - 1 ] += u_p [ ix ] * s ; ix ++ ; } }
jpiv_offset += 6 ; ONE += 6 ; } } for ( ix = 0 ; ix < 6 ; ix ++ ) { p [ ix ]
= ( int8_T ) ( ix + 1 ) ; } for ( jpiv_offset = 0 ; jpiv_offset < 5 ;
jpiv_offset ++ ) { smax = ( real_T ) jpiv_offset + 1.0 ; ipk = ipiv [ (
int32_T ) smax - 1 ] - 1 ; if ( ipk + 1 > ( int32_T ) smax ) { pipk = p [ ipk
] ; p [ ipk ] = p [ ( int32_T ) smax - 1 ] ; p [ ( int32_T ) smax - 1 ] = (
int8_T ) pipk ; } } for ( jpiv_offset = 0 ; jpiv_offset < 6 ; jpiv_offset ++
) { pipk = jpiv_offset ; Aoffset = p [ pipk ] - 1 ; y [ pipk + 6 * Aoffset ]
= 1.0 ; for ( ipk = pipk + 1 ; ipk < 7 ; ipk ++ ) { if ( y [ ( 6 * Aoffset +
ipk ) - 1 ] != 0.0 ) { jm1 = ipk + 1 ; for ( b = jm1 ; b < 7 ; b ++ ) { y [ (
b + 6 * Aoffset ) - 1 ] -= u_p [ ( ( ipk - 1 ) * 6 + b ) - 1 ] * y [ ( 6 *
Aoffset + ipk ) - 1 ] ; } } } } for ( ipk = 0 ; ipk < 6 ; ipk ++ ) { pipk =
ipk ; pipk *= 6 ; for ( jpiv_offset = 5 ; jpiv_offset >= 0 ; jpiv_offset -- )
{ ix = 6 * jpiv_offset ; if ( y [ jpiv_offset + pipk ] != 0.0 ) { y [
jpiv_offset + pipk ] /= u_p [ jpiv_offset + ix ] ; jm1 = jpiv_offset - 1 ;
for ( b = 0 ; b <= jm1 ; b ++ ) { y [ b + pipk ] -= y [ jpiv_offset + pipk ]
* u_p [ b + ix ] ; } } } } } void MdlInitialize ( void ) { int32_T i ; rtDW .
hsrejdggk0 = rtP . Rotorangledthetae_IC ; for ( i = 0 ; i < 6 ; i ++ ) { rtDW
. g2cesgd0fj [ i ] = rtP . fluxes_InitialCondition [ i ] ; } { int32_T i , j
; real_T * As = ( real_T * ) rtDW . oh3rj3rgch . AS ; real_T * Bs = ( real_T
* ) rtDW . oh3rj3rgch . BS ; real_T * Cs = ( real_T * ) rtDW . oh3rj3rgch .
CS ; real_T * Ds = ( real_T * ) rtDW . oh3rj3rgch . DS ; real_T * X0 = (
real_T * ) & rtDW . pwalsrbpvg [ 0 ] ; for ( i = 0 ; i < 6 ; i ++ ) { X0 [ i
] = ( rtP . StateSpace_X0_param [ i ] ) ; } for ( i = 0 ; i < 6 ; i ++ ) {
for ( j = 0 ; j < 6 ; j ++ ) As [ i * 6 + j ] = ( rtP . StateSpace_AS_param [
i + j * 6 ] ) ; for ( j = 0 ; j < 11 ; j ++ ) Bs [ i * 11 + j ] = ( rtP .
StateSpace_BS_param [ i + j * 6 ] ) ; } for ( i = 0 ; i < 50 ; i ++ ) { for (
j = 0 ; j < 6 ; j ++ ) Cs [ i * 6 + j ] = ( rtP . StateSpace_CS_param [ i + j
* 50 ] ) ; } for ( i = 0 ; i < 50 ; i ++ ) { for ( j = 0 ; j < 11 ; j ++ ) Ds
[ i * 11 + j ] = ( rtP . StateSpace_DS_param [ i + j * 50 ] ) ; } { int_T *
switch_status = ( int_T * ) rtDW . oh3rj3rgch . SWITCH_STATUS ; int_T *
gState = ( int_T * ) rtDW . oh3rj3rgch . G_STATE ; real_T * yswitch = (
real_T * ) rtDW . oh3rj3rgch . Y_SWITCH ; int_T * switchTypes = ( int_T * )
rtDW . oh3rj3rgch . SWITCH_TYPES ; int_T * idxOutSw = ( int_T * ) rtDW .
oh3rj3rgch . IDX_OUT_SW ; int_T * switch_status_init = ( int_T * ) rtDW .
oh3rj3rgch . SWITCH_STATUS_INIT ; switch_status [ 0 ] = 0 ;
switch_status_init [ 0 ] = 0 ; gState [ 0 ] = ( int_T ) 1.0 ; yswitch [ 0 ] =
1 / 1.4641 ; switchTypes [ 0 ] = ( int_T ) 2.0 ; idxOutSw [ 0 ] = ( ( int_T )
0.0 ) - 1 ; switch_status [ 1 ] = 0 ; switch_status_init [ 1 ] = 0 ; gState [
1 ] = ( int_T ) 1.0 ; yswitch [ 1 ] = 1 / 1.4641 ; switchTypes [ 1 ] = (
int_T ) 2.0 ; idxOutSw [ 1 ] = ( ( int_T ) 0.0 ) - 1 ; switch_status [ 2 ] =
0 ; switch_status_init [ 2 ] = 0 ; gState [ 2 ] = ( int_T ) 1.0 ; yswitch [ 2
] = 1 / 1.4641 ; switchTypes [ 2 ] = ( int_T ) 2.0 ; idxOutSw [ 2 ] = ( (
int_T ) 0.0 ) - 1 ; switch_status [ 3 ] = 0 ; switch_status_init [ 3 ] = 0 ;
gState [ 3 ] = ( int_T ) 0.0 ; yswitch [ 3 ] = 1 / 1.0E-6 ; switchTypes [ 3 ]
= ( int_T ) 2.0 ; idxOutSw [ 3 ] = ( ( int_T ) 0.0 ) - 1 ; switch_status [ 4
] = 0 ; switch_status_init [ 4 ] = 0 ; gState [ 4 ] = ( int_T ) 0.0 ; yswitch
[ 4 ] = 1 / 1.0E-6 ; switchTypes [ 4 ] = ( int_T ) 2.0 ; idxOutSw [ 4 ] = ( (
int_T ) 0.0 ) - 1 ; switch_status [ 5 ] = 0 ; switch_status_init [ 5 ] = 0 ;
gState [ 5 ] = ( int_T ) 0.0 ; yswitch [ 5 ] = 1 / 1.0E-6 ; switchTypes [ 5 ]
= ( int_T ) 2.0 ; idxOutSw [ 5 ] = ( ( int_T ) 0.0 ) - 1 ; } } rtX .
lpzjbjknwp = rtP . integrator_IC ; rtDW . jfihwsipvc = rtP .
Memory_InitialCondition ; rtX . oi5s2zktut = rtP . integrator_IC_kbu50edsmc ;
rtDW . k2r45klkvr = rtP . Memory_InitialCondition_e0ydmt0d1l ; rtX .
pz2xsnrzjc = rtP . integrator_IC_j24moklbcm ; rtDW . lawcljffga = rtP .
Memory_InitialCondition_l14qxrwnwp ; rtX . hfv4dc3cpb = rtP .
integrator_IC_lxfy54g0cz ; rtDW . bhgocl4yyi = rtP .
Memory_InitialCondition_m5lga3j4fd ; rtDW . ltyzpyes2j = rtP .
dw_delay_InitialCondition ; rtDW . koephaho24 = rtP .
dw_predict_InitialCondition ; for ( i = 0 ; i < 6 ; i ++ ) { rtDW .
bjkyi30h4y [ i ] = rtP . voltages_InitialCondition ; } rtDW . cszisyhd2i =
rtP . theta_IC ; rtDW . ksbtbqie2m = rtP . Rotorspeeddeviationdw_IC ; rtDW .
ezm5irxmvk = rtP . Lmd_sat_InitialCondition ; rtDW . m2oxahajnn = rtP .
Lmq_sat_InitialCondition ; rtB . enoczvl5ij = rtP . Lmq_sat_Y0 ; rtB .
im1q1kmaf4 = rtP . Laq_Y0 ; for ( i = 0 ; i < 36 ; i ++ ) { rtB . mecr0anvzq
[ i ] = rtP . Linv_Y0 ; rtB . g5yq3s1oi3 [ i ] = rtP . RLinv_Y0 ; } rtB .
bzlrt5abhc = rtP . Lmqd_Y0 ; rtB . msc0q3ydkn = rtP . Lmqd_Y0 ; rtB .
gtavo0wfmk = rtP . Laqd_Y0 ; rtB . o5o0z0ts4i = rtP . Laqd_Y0 ; } void
MdlEnable ( void ) { rtDW . mljcxhdcts = 1 ; rtDW . euaoyfm334 = 1 ; rtDW .
irr1ngkglw = 1 ; rtDW . lkdnaytcjn = 1U ; } void MdlStart ( void ) { real_T
tmp ; { void * * slioCatalogueAddr = rt_slioCatalogueAddr ( ) ; void * r2 = (
NULL ) ; void * * pOSigstreamManagerAddr = ( NULL ) ; const int
maxErrorBufferSize = 16384 ; char errMsgCreatingOSigstreamManager [ 16384 ] ;
bool errorCreatingOSigstreamManager = false ; const char *
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
externalInputIsInDatasetFormat ) { } } { RTWLogSignalInfo rt_ScopeSignalInfo
; static int_T rt_ScopeSignalWidths [ ] = { 1 , 1 } ; static int_T
rt_ScopeSignalNumDimensions [ ] = { 1 , 1 } ; static int_T
rt_ScopeSignalDimensions [ ] = { 1 , 1 } ; static void * rt_ScopeCurrSigDims
[ ] = { ( NULL ) , ( NULL ) } ; static int_T rt_ScopeCurrSigDimsSize [ ] = {
4 , 4 } ; static const char_T * rt_ScopeSignalLabels [ ] = { "" , "" } ;
static char_T rt_ScopeSignalTitles [ ] = "" ; static int_T
rt_ScopeSignalTitleLengths [ ] = { 0 , 0 } ; static boolean_T
rt_ScopeSignalIsVarDims [ ] = { 0 , 0 } ; static int_T
rt_ScopeSignalPlotStyles [ ] = { 0 , 0 } ; BuiltInDTypeId dTypes [ 2 ] = {
SS_DOUBLE , SS_DOUBLE } ; static char_T rt_ScopeBlockName [ ] =
"DZG_v6_SimGen/System/Additional" ; static int_T rt_ScopeFrameData [ ] = { 0
, 0 } ; static RTWPreprocessingFcnPtr
rt_ScopeSignalLoggingPreprocessingFcnPtrs [ ] = { ( NULL ) , ( NULL ) } ;
rt_ScopeSignalInfo . numSignals = 2 ; rt_ScopeSignalInfo . numCols =
rt_ScopeSignalWidths ; rt_ScopeSignalInfo . numDims =
rt_ScopeSignalNumDimensions ; rt_ScopeSignalInfo . dims =
rt_ScopeSignalDimensions ; rt_ScopeSignalInfo . isVarDims =
rt_ScopeSignalIsVarDims ; rt_ScopeSignalInfo . currSigDims =
rt_ScopeCurrSigDims ; rt_ScopeSignalInfo . currSigDimsSize =
rt_ScopeCurrSigDimsSize ; rt_ScopeSignalInfo . dataTypes = dTypes ;
rt_ScopeSignalInfo . complexSignals = ( NULL ) ; rt_ScopeSignalInfo .
frameData = rt_ScopeFrameData ; rt_ScopeSignalInfo . preprocessingPtrs =
rt_ScopeSignalLoggingPreprocessingFcnPtrs ; rt_ScopeSignalInfo . labels .
cptr = rt_ScopeSignalLabels ; rt_ScopeSignalInfo . titles =
rt_ScopeSignalTitles ; rt_ScopeSignalInfo . titleLengths =
rt_ScopeSignalTitleLengths ; rt_ScopeSignalInfo . plotStyles =
rt_ScopeSignalPlotStyles ; rt_ScopeSignalInfo . blockNames . cptr = ( NULL )
; rt_ScopeSignalInfo . stateNames . cptr = ( NULL ) ; rt_ScopeSignalInfo .
crossMdlRef = ( NULL ) ; rt_ScopeSignalInfo . dataTypeConvert = ( NULL ) ;
rtDW . m0scdgvmro . LoggedData [ 0 ] = rt_CreateStructLogVar (
ssGetRTWLogInfo ( rtS ) , ssGetTStart ( rtS ) , ssGetTFinal ( rtS ) , 0.0 , (
& ssGetErrorStatus ( rtS ) ) , "ScopeData1" , 1 , 1215752192 , 2 , 1.0E-5 , &
rt_ScopeSignalInfo , rt_ScopeBlockName ) ; if ( rtDW . m0scdgvmro .
LoggedData [ 0 ] == ( NULL ) ) return ; } { RTWLogSignalInfo
rt_ScopeSignalInfo ; static int_T rt_ScopeSignalWidths [ ] = { 1 , 1 } ;
static int_T rt_ScopeSignalNumDimensions [ ] = { 1 , 1 } ; static int_T
rt_ScopeSignalDimensions [ ] = { 1 , 1 } ; static void * rt_ScopeCurrSigDims
[ ] = { ( NULL ) , ( NULL ) } ; static int_T rt_ScopeCurrSigDimsSize [ ] = {
4 , 4 } ; static const char_T * rt_ScopeSignalLabels [ ] = { "dg" , "fg" } ;
static char_T rt_ScopeSignalTitles [ ] = "dgfg" ; static int_T
rt_ScopeSignalTitleLengths [ ] = { 2 , 2 } ; static boolean_T
rt_ScopeSignalIsVarDims [ ] = { 0 , 0 } ; static int_T
rt_ScopeSignalPlotStyles [ ] = { 0 , 0 } ; BuiltInDTypeId dTypes [ 2 ] = {
SS_DOUBLE , SS_DOUBLE } ; static char_T rt_ScopeBlockName [ ] =
"DZG_v6_SimGen/System/Additional1" ; static int_T rt_ScopeFrameData [ ] = { 0
, 0 } ; static RTWPreprocessingFcnPtr
rt_ScopeSignalLoggingPreprocessingFcnPtrs [ ] = { ( NULL ) , ( NULL ) } ;
rt_ScopeSignalInfo . numSignals = 2 ; rt_ScopeSignalInfo . numCols =
rt_ScopeSignalWidths ; rt_ScopeSignalInfo . numDims =
rt_ScopeSignalNumDimensions ; rt_ScopeSignalInfo . dims =
rt_ScopeSignalDimensions ; rt_ScopeSignalInfo . isVarDims =
rt_ScopeSignalIsVarDims ; rt_ScopeSignalInfo . currSigDims =
rt_ScopeCurrSigDims ; rt_ScopeSignalInfo . currSigDimsSize =
rt_ScopeCurrSigDimsSize ; rt_ScopeSignalInfo . dataTypes = dTypes ;
rt_ScopeSignalInfo . complexSignals = ( NULL ) ; rt_ScopeSignalInfo .
frameData = rt_ScopeFrameData ; rt_ScopeSignalInfo . preprocessingPtrs =
rt_ScopeSignalLoggingPreprocessingFcnPtrs ; rt_ScopeSignalInfo . labels .
cptr = rt_ScopeSignalLabels ; rt_ScopeSignalInfo . titles =
rt_ScopeSignalTitles ; rt_ScopeSignalInfo . titleLengths =
rt_ScopeSignalTitleLengths ; rt_ScopeSignalInfo . plotStyles =
rt_ScopeSignalPlotStyles ; rt_ScopeSignalInfo . blockNames . cptr = ( NULL )
; rt_ScopeSignalInfo . stateNames . cptr = ( NULL ) ; rt_ScopeSignalInfo .
crossMdlRef = ( NULL ) ; rt_ScopeSignalInfo . dataTypeConvert = ( NULL ) ;
rtDW . exi0svnnxz . LoggedData [ 0 ] = rt_CreateStructLogVar (
ssGetRTWLogInfo ( rtS ) , ssGetTStart ( rtS ) , ssGetTFinal ( rtS ) , 0.0 , (
& ssGetErrorStatus ( rtS ) ) , "ScopeData2" , 1 , 1215752192 , 2 , 1.0E-5 , &
rt_ScopeSignalInfo , rt_ScopeBlockName ) ; if ( rtDW . exi0svnnxz .
LoggedData [ 0 ] == ( NULL ) ) return ; } { RTWLogSignalInfo
rt_ScopeSignalInfo ; static int_T rt_ScopeSignalWidths [ ] = { 1 , 1 } ;
static int_T rt_ScopeSignalNumDimensions [ ] = { 1 , 1 } ; static int_T
rt_ScopeSignalDimensions [ ] = { 1 , 1 } ; static void * rt_ScopeCurrSigDims
[ ] = { ( NULL ) , ( NULL ) } ; static int_T rt_ScopeCurrSigDimsSize [ ] = {
4 , 4 } ; static const char_T * rt_ScopeSignalLabels [ ] = { "U" , "I" } ;
static char_T rt_ScopeSignalTitles [ ] = "UI" ; static int_T
rt_ScopeSignalTitleLengths [ ] = { 1 , 1 } ; static boolean_T
rt_ScopeSignalIsVarDims [ ] = { 0 , 0 } ; static int_T
rt_ScopeSignalPlotStyles [ ] = { 0 , 0 } ; BuiltInDTypeId dTypes [ 2 ] = {
SS_DOUBLE , SS_DOUBLE } ; static char_T rt_ScopeBlockName [ ] =
"DZG_v6_SimGen/System/EffRangeG" ; static int_T rt_ScopeFrameData [ ] = { 0 ,
0 } ; static RTWPreprocessingFcnPtr rt_ScopeSignalLoggingPreprocessingFcnPtrs
[ ] = { ( NULL ) , ( NULL ) } ; rt_ScopeSignalInfo . numSignals = 2 ;
rt_ScopeSignalInfo . numCols = rt_ScopeSignalWidths ; rt_ScopeSignalInfo .
numDims = rt_ScopeSignalNumDimensions ; rt_ScopeSignalInfo . dims =
rt_ScopeSignalDimensions ; rt_ScopeSignalInfo . isVarDims =
rt_ScopeSignalIsVarDims ; rt_ScopeSignalInfo . currSigDims =
rt_ScopeCurrSigDims ; rt_ScopeSignalInfo . currSigDimsSize =
rt_ScopeCurrSigDimsSize ; rt_ScopeSignalInfo . dataTypes = dTypes ;
rt_ScopeSignalInfo . complexSignals = ( NULL ) ; rt_ScopeSignalInfo .
frameData = rt_ScopeFrameData ; rt_ScopeSignalInfo . preprocessingPtrs =
rt_ScopeSignalLoggingPreprocessingFcnPtrs ; rt_ScopeSignalInfo . labels .
cptr = rt_ScopeSignalLabels ; rt_ScopeSignalInfo . titles =
rt_ScopeSignalTitles ; rt_ScopeSignalInfo . titleLengths =
rt_ScopeSignalTitleLengths ; rt_ScopeSignalInfo . plotStyles =
rt_ScopeSignalPlotStyles ; rt_ScopeSignalInfo . blockNames . cptr = ( NULL )
; rt_ScopeSignalInfo . stateNames . cptr = ( NULL ) ; rt_ScopeSignalInfo .
crossMdlRef = ( NULL ) ; rt_ScopeSignalInfo . dataTypeConvert = ( NULL ) ;
rtDW . apk42thrxw . LoggedData [ 0 ] = rt_CreateStructLogVar (
ssGetRTWLogInfo ( rtS ) , ssGetTStart ( rtS ) , ssGetTFinal ( rtS ) , 0.0 , (
& ssGetErrorStatus ( rtS ) ) , "ScopeData3" , 1 , - 727379968 , 2 , 1.0E-5 ,
& rt_ScopeSignalInfo , rt_ScopeBlockName ) ; if ( rtDW . apk42thrxw .
LoggedData [ 0 ] == ( NULL ) ) return ; } { RTWLogSignalInfo
rt_ScopeSignalInfo ; static int_T rt_ScopeSignalWidths [ ] = { 3 , 3 } ;
static int_T rt_ScopeSignalNumDimensions [ ] = { 1 , 1 } ; static int_T
rt_ScopeSignalDimensions [ ] = { 3 , 3 } ; static void * rt_ScopeCurrSigDims
[ ] = { ( NULL ) , ( NULL ) } ; static int_T rt_ScopeCurrSigDimsSize [ ] = {
4 , 4 } ; static const char_T * rt_ScopeSignalLabels [ ] = { "" , "" } ;
static char_T rt_ScopeSignalTitles [ ] = "" ; static int_T
rt_ScopeSignalTitleLengths [ ] = { 0 , 0 } ; static boolean_T
rt_ScopeSignalIsVarDims [ ] = { 0 , 0 } ; static int_T
rt_ScopeSignalPlotStyles [ ] = { 1 , 1 , 1 , 1 , 1 , 1 } ; BuiltInDTypeId
dTypes [ 2 ] = { SS_DOUBLE , SS_DOUBLE } ; static char_T rt_ScopeBlockName [
] = "DZG_v6_SimGen/System/InstValG" ; static int_T rt_ScopeFrameData [ ] = {
0 , 0 } ; static RTWPreprocessingFcnPtr
rt_ScopeSignalLoggingPreprocessingFcnPtrs [ ] = { ( NULL ) , ( NULL ) } ;
rt_ScopeSignalInfo . numSignals = 2 ; rt_ScopeSignalInfo . numCols =
rt_ScopeSignalWidths ; rt_ScopeSignalInfo . numDims =
rt_ScopeSignalNumDimensions ; rt_ScopeSignalInfo . dims =
rt_ScopeSignalDimensions ; rt_ScopeSignalInfo . isVarDims =
rt_ScopeSignalIsVarDims ; rt_ScopeSignalInfo . currSigDims =
rt_ScopeCurrSigDims ; rt_ScopeSignalInfo . currSigDimsSize =
rt_ScopeCurrSigDimsSize ; rt_ScopeSignalInfo . dataTypes = dTypes ;
rt_ScopeSignalInfo . complexSignals = ( NULL ) ; rt_ScopeSignalInfo .
frameData = rt_ScopeFrameData ; rt_ScopeSignalInfo . preprocessingPtrs =
rt_ScopeSignalLoggingPreprocessingFcnPtrs ; rt_ScopeSignalInfo . labels .
cptr = rt_ScopeSignalLabels ; rt_ScopeSignalInfo . titles =
rt_ScopeSignalTitles ; rt_ScopeSignalInfo . titleLengths =
rt_ScopeSignalTitleLengths ; rt_ScopeSignalInfo . plotStyles =
rt_ScopeSignalPlotStyles ; rt_ScopeSignalInfo . blockNames . cptr = ( NULL )
; rt_ScopeSignalInfo . stateNames . cptr = ( NULL ) ; rt_ScopeSignalInfo .
crossMdlRef = ( NULL ) ; rt_ScopeSignalInfo . dataTypeConvert = ( NULL ) ;
rtDW . m44vudu504 . LoggedData [ 0 ] = rt_CreateStructLogVar (
ssGetRTWLogInfo ( rtS ) , ssGetTStart ( rtS ) , ssGetTFinal ( rtS ) , 0.0 , (
& ssGetErrorStatus ( rtS ) ) , "ScopeData4" , 1 , 100000000 , 2 , 5.0E-5 , &
rt_ScopeSignalInfo , rt_ScopeBlockName ) ; if ( rtDW . m44vudu504 .
LoggedData [ 0 ] == ( NULL ) ) return ; } tmp = 6.2831853071795862 * rtP .
System1_Frequency ; rtP . SineWaveA_Hsin = muDoubleScalarSin ( tmp * 5.0E-5 )
; tmp = 6.2831853071795862 * rtP . System1_Frequency ; rtP . SineWaveA_HCos =
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
muDoubleScalarCos ( 2.0943951023931953 - tmp * 5.0E-5 ) ; { rtDW . oh3rj3rgch
. AS = ( real_T * ) calloc ( 6 * 6 , sizeof ( real_T ) ) ; rtDW . oh3rj3rgch
. BS = ( real_T * ) calloc ( 6 * 11 , sizeof ( real_T ) ) ; rtDW . oh3rj3rgch
. CS = ( real_T * ) calloc ( 50 * 6 , sizeof ( real_T ) ) ; rtDW . oh3rj3rgch
. DS = ( real_T * ) calloc ( 50 * 11 , sizeof ( real_T ) ) ; rtDW .
oh3rj3rgch . DX_COL = ( real_T * ) calloc ( 50 , sizeof ( real_T ) ) ; rtDW .
oh3rj3rgch . TMP2 = ( real_T * ) calloc ( 11 , sizeof ( real_T ) ) ; rtDW .
oh3rj3rgch . BD_COL = ( real_T * ) calloc ( 6 , sizeof ( real_T ) ) ; rtDW .
oh3rj3rgch . TMP1 = ( real_T * ) calloc ( 6 , sizeof ( real_T ) ) ; rtDW .
oh3rj3rgch . XTMP = ( real_T * ) calloc ( 6 , sizeof ( real_T ) ) ; rtDW .
oh3rj3rgch . SWITCH_STATUS = ( int_T * ) calloc ( 6 , sizeof ( int_T ) ) ;
rtDW . oh3rj3rgch . SW_CHG = ( int_T * ) calloc ( 6 , sizeof ( int_T ) ) ;
rtDW . oh3rj3rgch . G_STATE = ( int_T * ) calloc ( 6 , sizeof ( int_T ) ) ;
rtDW . oh3rj3rgch . Y_SWITCH = ( real_T * ) calloc ( 6 , sizeof ( real_T ) )
; rtDW . oh3rj3rgch . SWITCH_TYPES = ( int_T * ) calloc ( 6 , sizeof ( int_T
) ) ; rtDW . oh3rj3rgch . IDX_OUT_SW = ( int_T * ) calloc ( 6 , sizeof (
int_T ) ) ; rtDW . oh3rj3rgch . SWITCH_STATUS_INIT = ( int_T * ) calloc ( 6 ,
sizeof ( int_T ) ) ; rtDW . oh3rj3rgch . USWLAST = ( real_T * ) calloc ( 6 ,
sizeof ( real_T ) ) ; } { { real_T * pBuffer = ( real_T * )
rt_TDelayCreateBuf ( 2 , 8192 , sizeof ( real_T ) ) ; if ( pBuffer == ( NULL
) ) { ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ;
} rtDW . drr2cwynj5 . Tail = 0 ; rtDW . drr2cwynj5 . Head = 0 ; rtDW .
drr2cwynj5 . Last = 0 ; rtDW . drr2cwynj5 . CircularBufSize = 8192 ; pBuffer
[ 0 ] = rtP . TransportDelay_InitOutput ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . b1o5sd1mu2 . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
b1o5sd1mu2 . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . fymd3ellif . Tail = 0
; rtDW . fymd3ellif . Head = 0 ; rtDW . fymd3ellif . Last = 0 ; rtDW .
fymd3ellif . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_gepskaykr5 ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . cvivv0qvki . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
cvivv0qvki . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . eyb4fhkbdy . Tail = 0
; rtDW . eyb4fhkbdy . Head = 0 ; rtDW . eyb4fhkbdy . Last = 0 ; rtDW .
eyb4fhkbdy . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_a33cxqs542 ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . ecv4fwadro . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
ecv4fwadro . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } } { {
real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } rtDW . ek4mryl130 . Tail = 0
; rtDW . ek4mryl130 . Head = 0 ; rtDW . ek4mryl130 . Last = 0 ; rtDW .
ek4mryl130 . CircularBufSize = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_kfzzmtnzlj ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
rtDW . ohddfojdjk . TUbufferPtrs [ 0 ] = ( void * ) & pBuffer [ 0 ] ; rtDW .
ohddfojdjk . TUbufferPtrs [ 1 ] = ( void * ) & pBuffer [ 8192 ] ; } }
MdlInitialize ( ) ; MdlEnable ( ) ; } void MdlOutputs ( int_T tid ) { real_T
tmp [ 6 ] ; real_T tmp_p [ 6 ] ; real_T tmp_e [ 36 ] ; real_T tmp_i [ 36 ] ;
int32_T i ; int32_T i_p ; int32_T i_e ; real_T tmpForInput_idx_2 ; real_T
tmpForInput_idx_3 ; real_T tmpForInput_idx_0 ; real_T tmpForInput_idx_1 ;
real_T tmpForInput_idx_0_p ; int8_T tmp_m ; static const int8_T tmp_g [ 3 ] =
{ 0 , 4 , 5 } ; srClearBC ( rtDW . jld3u4fkos ) ; srClearBC ( rtDW .
eocn40ibhc ) ; if ( ssIsSampleHit ( rtS , 2 , 0 ) ) { rtB . o0laqbijxj = rtDW
. hsrejdggk0 ; rtB . phf1q3qtgy = ssGetTaskTime ( rtS , 2 ) ; rtB .
adxx2eh1g0 = rtP . web2_Gain * rtB . phf1q3qtgy ; rtB . fx0kk0lfk3 = rtB .
o0laqbijxj + rtB . adxx2eh1g0 ; muDoubleScalarSinCos ( rtB . fx0kk0lfk3 , &
rtB . ps502vbzlw , & rtB . pz2b2ixe3g ) ; for ( i = 0 ; i < 6 ; i ++ ) { rtB
. mshc0z4y0b [ i ] = rtDW . g2cesgd0fj [ i ] ; } if ( rtP .
Constant1_Value_iglewzfyey > 0.0 ) { rtB . b2rku1yvfe [ 0 ] = rtP .
uLl_d_Gain [ 0 ] * rtB . mshc0z4y0b [ 1 ] ; rtB . b2rku1yvfe [ 1 ] = rtP .
uLl_d_Gain [ 1 ] * rtB . mshc0z4y0b [ 2 ] ; rtB . b2rku1yvfe [ 2 ] = rtP .
uLl_d_Gain [ 2 ] * rtB . mshc0z4y0b [ 3 ] ; tmpForInput_idx_0 = rtB .
b2rku1yvfe [ 0 ] ; tmpForInput_idx_0 += rtB . b2rku1yvfe [ 1 ] ;
tmpForInput_idx_0 += rtB . b2rku1yvfe [ 2 ] ; rtB . iqgj54dvfu =
tmpForInput_idx_0 ; rtB . kgqd5i3tmd = rtDW . ezm5irxmvk ; tmpForInput_idx_0
= rtB . kgqd5i3tmd ; rtB . pmvqbzeo1a = 1.0 / tmpForInput_idx_0 ;
tmpForInput_idx_0 = rtP . u1_Value [ 0 ] ; tmpForInput_idx_1 = rtP . u1_Value
[ 1 ] ; tmpForInput_idx_2 = rtB . pmvqbzeo1a ; tmpForInput_idx_0 +=
tmpForInput_idx_1 ; tmpForInput_idx_0 += tmpForInput_idx_2 ; rtB . lnslax3wky
= tmpForInput_idx_0 ; tmpForInput_idx_0 = rtB . lnslax3wky ; rtB . o5o0z0ts4i
= 1.0 / tmpForInput_idx_0 ; rtB . ke1trlx0xn = rtB . iqgj54dvfu * rtB .
o5o0z0ts4i ; rtB . pfjhtzhhke = muDoubleScalarAbs ( rtB . ke1trlx0xn ) ; rtB
. dsgaag2j1a = look1_pbinlxpw ( rtB . pfjhtzhhke , rtP .
uDLookupTable_bp01Data_h0gmvbj4n1 , rtP . uDLookupTable_tableData_jcoemdiu0h
, & rtDW . lrbh114wtw , 9U ) ; if ( rtB . dsgaag2j1a != 0.0 ) { rtB .
cy4aoyd4p4 = rtB . pfjhtzhhke / rtB . dsgaag2j1a ; rtB . ddpigcbdzw = rtB .
cy4aoyd4p4 ; } else { rtB . ddpigcbdzw = rtP . Constant1_Value_csdxpxx0tx ; }
rtB . msc0q3ydkn = rtP . Lmd_Gain * rtB . ddpigcbdzw ; if ( rtP .
Constant1_Value_afrrlaacip ) { rtB . p4nck3maqs [ 0 ] = rtP . uLl_q_Gain [ 0
] * rtB . mshc0z4y0b [ 0 ] ; rtB . p4nck3maqs [ 1 ] = rtP . uLl_q_Gain [ 1 ]
* rtB . mshc0z4y0b [ 4 ] ; rtB . p4nck3maqs [ 2 ] = rtP . uLl_q_Gain [ 2 ] *
rtB . mshc0z4y0b [ 5 ] ; rtB . pnchbrbzh4 = rtDW . m2oxahajnn ;
tmpForInput_idx_0 = rtB . p4nck3maqs [ 0 ] ; tmpForInput_idx_0_p = rtP .
u2_Value [ 0 ] ; tmpForInput_idx_0 += rtB . p4nck3maqs [ 1 ] ;
tmpForInput_idx_1 = rtP . u2_Value [ 1 ] ; tmpForInput_idx_0 += rtB .
p4nck3maqs [ 2 ] ; tmpForInput_idx_2 = rtP . u2_Value [ 2 ] ; rtB .
lv1l3120cn = tmpForInput_idx_0 ; tmpForInput_idx_0 = rtB . pnchbrbzh4 ; rtB .
h42emyzqm1 = 1.0 / tmpForInput_idx_0 ; tmpForInput_idx_3 = rtB . h42emyzqm1 ;
tmpForInput_idx_0 = tmpForInput_idx_0_p ; tmpForInput_idx_0 +=
tmpForInput_idx_1 ; tmpForInput_idx_0 += tmpForInput_idx_2 ;
tmpForInput_idx_0 += tmpForInput_idx_3 ; rtB . mw4gdhqybz = tmpForInput_idx_0
; tmpForInput_idx_0 = rtB . mw4gdhqybz ; rtB . im1q1kmaf4 = 1.0 /
tmpForInput_idx_0 ; rtB . a0tcaxg1ye = rtB . lv1l3120cn * rtB . im1q1kmaf4 ;
rtB . babh1jhck2 = muDoubleScalarAbs ( rtB . a0tcaxg1ye ) ; rtB . op4v4vz1hb
= look1_pbinlxpw ( rtB . babh1jhck2 , rtP . uDLookupTable_bp01Data , rtP .
uDLookupTable_tableData , & rtDW . pftimexvc5 , 9U ) ; if ( rtB . op4v4vz1hb
!= 0.0 ) { rtB . l42zjb2acu = rtB . babh1jhck2 / rtB . op4v4vz1hb ; rtB .
elxflb4cn2 = rtB . l42zjb2acu ; } else { rtB . elxflb4cn2 = rtP .
Constant1_Value ; } rtB . enoczvl5ij = rtP . Lmq_Gain * rtB . elxflb4cn2 ;
srUpdateBC ( rtDW . jld3u4fkos ) ; } if ( rtP . Constant2_Value_lxtjw1dx3o )
{ rtB . bzlrt5abhc = rtB . enoczvl5ij ; } else { rtB . bzlrt5abhc = rtP .
u3_Value ; } memcpy ( & rtB . mc0c4fprgt [ 0 ] , & rtP . u1_Value_oit41rg0le
[ 0 ] , 36U * sizeof ( real_T ) ) ; for ( i_p = 0 ; i_p < 3 ; i_p ++ ) {
tmp_m = tmp_g [ i_p ] ; rtB . mc0c4fprgt [ 6 * tmp_m ] = rtB . bzlrt5abhc ;
rtB . mc0c4fprgt [ 4 + 6 * tmp_m ] = rtB . bzlrt5abhc ; rtB . mc0c4fprgt [ 5
+ 6 * tmp_m ] = rtB . bzlrt5abhc ; } memcpy ( & rtB . g30midesdo [ 0 ] , &
rtB . mc0c4fprgt [ 0 ] , 36U * sizeof ( real_T ) ) ; for ( i_p = 0 ; i_p < 3
; i_p ++ ) { rtB . g30midesdo [ 6 * ( i_p + 1 ) + 1 ] = rtB . msc0q3ydkn ;
rtB . g30midesdo [ 6 * ( i_p + 1 ) + 2 ] = rtB . msc0q3ydkn ; rtB .
g30midesdo [ 6 * ( i_p + 1 ) + 3 ] = rtB . msc0q3ydkn ; } for ( i = 0 ; i <
36 ; i ++ ) { rtB . k3cesiw1su [ i ] = rtB . g30midesdo [ i ] + rtP .
u5_Value [ i ] ; } rt_invd6x6_snf ( rtB . k3cesiw1su , rtB . mecr0anvzq ) ;
memcpy ( & tmp_e [ 0 ] , & rtP . u1_Value_pqt3qesf2j [ 0 ] , 36U * sizeof (
real_T ) ) ; memcpy ( & tmp_i [ 0 ] , & rtB . mecr0anvzq [ 0 ] , 36U * sizeof
( real_T ) ) ; for ( i_p = 0 ; i_p < 6 ; i_p ++ ) { for ( i = 0 ; i < 6 ; i
++ ) { rtB . g5yq3s1oi3 [ i + 6 * i_p ] = 0.0 ; for ( i_e = 0 ; i_e < 6 ; i_e
++ ) { rtB . g5yq3s1oi3 [ i + 6 * i_p ] += tmp_e [ 6 * i_e + i ] * tmp_i [ 6
* i_p + i_e ] ; } } } if ( rtP . Constant3_Value_jspriozsq0 ) { rtB .
gtavo0wfmk = rtB . im1q1kmaf4 ; } else { rtB . gtavo0wfmk = rtP .
u2_Value_kee1ps1dzy ; } if ( ssIsMajorTimeStep ( rtS ) ) { srUpdateBC ( rtDW
. eocn40ibhc ) ; } } for ( i = 0 ; i < 36 ; i ++ ) { if ( rtP .
Constant3_Value >= rtP . Switch_Threshold_pea0kljvkj ) { rtB . kxk50ui10l [ i
] = rtB . mecr0anvzq [ i ] ; } else { rtB . kxk50ui10l [ i ] = rtP .
Constant4_Value [ i ] ; } tmp_e [ i ] = rtB . kxk50ui10l [ i ] ; } for ( i =
0 ; i < 6 ; i ++ ) { tmp [ i ] = rtB . mshc0z4y0b [ i ] ; } for ( i = 0 ; i <
6 ; i ++ ) { tmp_p [ i ] = 0.0 ; for ( i_p = 0 ; i_p < 6 ; i_p ++ ) {
tmpForInput_idx_0 = tmp_p [ i ] ; tmpForInput_idx_0 += tmp_e [ 6 * i_p + i ]
* tmp [ i_p ] ; tmp_p [ i ] = tmpForInput_idx_0 ; } rtB . i2htmqgs5h [ i ] =
tmp_p [ i ] ; rtB . d2l3x5dmoq [ i ] = rtP . changeIqIdcurrentsigns_Gain [ i
] * rtB . i2htmqgs5h [ i ] ; } rtB . a3kfgrogpp = rtB . d2l3x5dmoq [ 0 ] *
rtB . pz2b2ixe3g + rtB . d2l3x5dmoq [ 1 ] * rtB . ps502vbzlw ; rtB .
dggh1ark2b = ( ( - rtB . d2l3x5dmoq [ 0 ] - 1.7320508075688772 * rtB .
d2l3x5dmoq [ 1 ] ) * rtB . pz2b2ixe3g + ( 1.7320508075688772 * rtB .
d2l3x5dmoq [ 0 ] - rtB . d2l3x5dmoq [ 1 ] ) * rtB . ps502vbzlw ) * 0.5 ; rtB
. hleuoxbvtr = ( 0.0 - rtB . a3kfgrogpp ) - rtB . dggh1ark2b ; rtB .
gfcufizx5q [ 0 ] = rtP . ib_Gain * rtB . a3kfgrogpp ; rtB . gfcufizx5q [ 1 ]
= rtP . ib_Gain * rtB . dggh1ark2b ; rtB . gfcufizx5q [ 2 ] = rtP . ib_Gain *
rtB . hleuoxbvtr ; if ( ssIsSampleHit ( rtS , 2 , 0 ) ) { rtB . ocwtlsstoa [
0 ] = rtB . gfcufizx5q [ 0 ] ; rtB . ocwtlsstoa [ 1 ] = rtB . gfcufizx5q [ 1
] ; } if ( rtDW . mljcxhdcts != 0 ) { tmpForInput_idx_0 = 6.2831853071795862
* rtP . System1_Frequency ; rtDW . jzuea21ukz = muDoubleScalarSin (
tmpForInput_idx_0 * ssGetTaskTime ( rtS , 2 ) ) ; tmpForInput_idx_0 =
6.2831853071795862 * rtP . System1_Frequency ; rtDW . crh5ypw2db =
muDoubleScalarCos ( tmpForInput_idx_0 * ssGetTaskTime ( rtS , 2 ) ) ; rtDW .
mljcxhdcts = 0 ; } rtB . kc2wsb45fs = ( ( rtDW . jzuea21ukz * rtP .
SineWaveA_PCos + rtDW . crh5ypw2db * rtP . SineWaveA_PSin ) * rtP .
SineWaveA_HCos + ( rtDW . crh5ypw2db * rtP . SineWaveA_PCos - rtDW .
jzuea21ukz * rtP . SineWaveA_PSin ) * rtP . SineWaveA_Hsin ) * rtP .
SineWaveA_Amp + rtP . SineWaveA_Bias ; if ( rtDW . euaoyfm334 != 0 ) {
tmpForInput_idx_0 = 6.2831853071795862 * rtP . System1_Frequency ; rtDW .
ia2zs2ii2b = muDoubleScalarSin ( tmpForInput_idx_0 * ssGetTaskTime ( rtS , 2
) ) ; tmpForInput_idx_0 = 6.2831853071795862 * rtP . System1_Frequency ; rtDW
. lormmfrasx = muDoubleScalarCos ( tmpForInput_idx_0 * ssGetTaskTime ( rtS ,
2 ) ) ; rtDW . euaoyfm334 = 0 ; } rtB . l2zafpzrzu = ( ( rtDW . ia2zs2ii2b *
rtP . SineWaveB_PCos + rtDW . lormmfrasx * rtP . SineWaveB_PSin ) * rtP .
SineWaveB_HCos + ( rtDW . lormmfrasx * rtP . SineWaveB_PCos - rtDW .
ia2zs2ii2b * rtP . SineWaveB_PSin ) * rtP . SineWaveB_Hsin ) * rtP .
SineWaveB_Amp + rtP . SineWaveB_Bias ; if ( rtDW . irr1ngkglw != 0 ) {
tmpForInput_idx_0 = 6.2831853071795862 * rtP . System1_Frequency ; rtDW .
ocwfh3bc3h = muDoubleScalarSin ( tmpForInput_idx_0 * ssGetTaskTime ( rtS , 2
) ) ; tmpForInput_idx_0 = 6.2831853071795862 * rtP . System1_Frequency ; rtDW
. dw042k0azu = muDoubleScalarCos ( tmpForInput_idx_0 * ssGetTaskTime ( rtS ,
2 ) ) ; rtDW . irr1ngkglw = 0 ; } rtB . ohedl35wf1 = ( ( rtDW . ocwfh3bc3h *
rtP . SineWaveC_PCos + rtDW . dw042k0azu * rtP . SineWaveC_PSin ) * rtP .
SineWaveC_HCos + ( rtDW . dw042k0azu * rtP . SineWaveC_PCos - rtDW .
ocwfh3bc3h * rtP . SineWaveC_PSin ) * rtP . SineWaveC_Hsin ) * rtP .
SineWaveC_Amp + rtP . SineWaveC_Bias ; { real_T accum ; int_T * switch_status
= ( int_T * ) rtDW . oh3rj3rgch . SWITCH_STATUS ; int_T * switch_status_init
= ( int_T * ) rtDW . oh3rj3rgch . SWITCH_STATUS_INIT ; int_T * SwitchChange =
( int_T * ) rtDW . oh3rj3rgch . SW_CHG ; int_T * gState = ( int_T * ) rtDW .
oh3rj3rgch . G_STATE ; real_T * yswitch = ( real_T * ) rtDW . oh3rj3rgch .
Y_SWITCH ; int_T * switchTypes = ( int_T * ) rtDW . oh3rj3rgch . SWITCH_TYPES
; int_T * idxOutSw = ( int_T * ) rtDW . oh3rj3rgch . IDX_OUT_SW ; real_T *
DxCol = ( real_T * ) rtDW . oh3rj3rgch . DX_COL ; real_T * tmp2 = ( real_T *
) rtDW . oh3rj3rgch . TMP2 ; real_T * BDcol = ( real_T * ) rtDW . oh3rj3rgch
. BD_COL ; real_T * tmp1 = ( real_T * ) rtDW . oh3rj3rgch . TMP1 ; real_T *
uswlast = ( real_T * ) rtDW . oh3rj3rgch . USWLAST ; int_T newState ; int_T
swChanged = 0 ; int loopsToDo = 20 ; real_T temp ; memcpy (
switch_status_init , switch_status , 6 * sizeof ( int_T ) ) ; memcpy (
uswlast , & rtB . bkf5f3ujyq [ 0 ] , 6 * sizeof ( real_T ) ) ; do { if (
loopsToDo == 1 ) { swChanged = 0 ; { int_T i1 ; for ( i1 = 0 ; i1 < 6 ; i1 ++
) { swChanged = ( ( SwitchChange [ i1 ] = switch_status_init [ i1 ] -
switch_status [ i1 ] ) != 0 ) ? 1 : swChanged ; switch_status [ i1 ] =
switch_status_init [ i1 ] ; } } } else { real_T * Cs = ( real_T * ) rtDW .
oh3rj3rgch . CS ; real_T * Ds = ( real_T * ) rtDW . oh3rj3rgch . DS ; { int_T
i1 ; real_T * y0 = & rtB . bkf5f3ujyq [ 0 ] ; for ( i1 = 0 ; i1 < 50 ; i1 ++
) { accum = 0.0 ; { int_T i2 ; real_T * xd = & rtDW . pwalsrbpvg [ 0 ] ; for
( i2 = 0 ; i2 < 6 ; i2 ++ ) { accum += * ( Cs ++ ) * xd [ i2 ] ; } } { int_T
i2 ; const real_T * u0 = & rtP . SwitchCurrents_Value [ 0 ] ; for ( i2 = 0 ;
i2 < 6 ; i2 ++ ) { accum += * ( Ds ++ ) * u0 [ i2 ] ; } accum += * ( Ds ++ )
* rtB . ocwtlsstoa [ 0 ] ; accum += * ( Ds ++ ) * rtB . ocwtlsstoa [ 1 ] ;
accum += * ( Ds ++ ) * rtB . kc2wsb45fs ; accum += * ( Ds ++ ) * rtB .
l2zafpzrzu ; accum += * ( Ds ++ ) * rtB . ohedl35wf1 ; } y0 [ i1 ] = accum ;
} } swChanged = 0 ; { int_T i1 ; real_T * y0 = & rtB . bkf5f3ujyq [ 0 ] ; for
( i1 = 0 ; i1 < 6 ; i1 ++ ) { newState = ( gState [ i1 ] > 0 ) ? 1 : ( ( y0 [
i1 ] * uswlast [ i1 ] < 0.0 ) ? 0 : switch_status [ i1 ] ) ; swChanged = ( (
SwitchChange [ i1 ] = newState - switch_status [ i1 ] ) != 0 ) ? 1 :
swChanged ; switch_status [ i1 ] = newState ; } } } if ( swChanged ) { real_T
* As = ( real_T * ) rtDW . oh3rj3rgch . AS ; real_T * Cs = ( real_T * ) rtDW
. oh3rj3rgch . CS ; real_T * Bs = ( real_T * ) rtDW . oh3rj3rgch . BS ;
real_T * Ds = ( real_T * ) rtDW . oh3rj3rgch . DS ; real_T a1 ; { int_T i1 ;
for ( i1 = 0 ; i1 < 6 ; i1 ++ ) { if ( SwitchChange [ i1 ] != 0 ) { a1 =
yswitch [ i1 ] * SwitchChange [ i1 ] ; temp = 1 / ( 1 - Ds [ i1 * 12 ] * a1 )
; { int_T i2 ; for ( i2 = 0 ; i2 < 50 ; i2 ++ ) { DxCol [ i2 ] = Ds [ i2 * 11
+ i1 ] * temp * a1 ; } } DxCol [ i1 ] = temp ; { int_T i2 ; for ( i2 = 0 ; i2
< 6 ; i2 ++ ) { BDcol [ i2 ] = Bs [ i2 * 11 + i1 ] * a1 ; } } memcpy ( tmp1 ,
& Cs [ i1 * 6 ] , 6 * sizeof ( real_T ) ) ; memset ( & Cs [ i1 * 6 ] , '\0' ,
6 * sizeof ( real_T ) ) ; memcpy ( tmp2 , & Ds [ i1 * 11 ] , 11 * sizeof (
real_T ) ) ; memset ( & Ds [ i1 * 11 ] , '\0' , 11 * sizeof ( real_T ) ) ; {
int_T i2 ; for ( i2 = 0 ; i2 < 50 ; i2 ++ ) { a1 = DxCol [ i2 ] ; { int_T i3
; for ( i3 = 0 ; i3 < 6 ; i3 ++ ) { Cs [ i2 * 6 + i3 ] += a1 * tmp1 [ i3 ] ;
} } { int_T i3 ; for ( i3 = 0 ; i3 < 11 ; i3 ++ ) { Ds [ i2 * 11 + i3 ] += a1
* tmp2 [ i3 ] ; } } } } { int_T i2 ; for ( i2 = 0 ; i2 < 6 ; i2 ++ ) { a1 =
BDcol [ i2 ] ; { int_T i3 ; for ( i3 = 0 ; i3 < 6 ; i3 ++ ) { As [ i2 * 6 +
i3 ] += a1 * Cs [ i1 * 6 + i3 ] ; } } { int_T i3 ; for ( i3 = 0 ; i3 < 11 ;
i3 ++ ) { Bs [ i2 * 11 + i3 ] += a1 * Ds [ i1 * 11 + i3 ] ; } } } } } } } } }
while ( swChanged > 0 && -- loopsToDo > 0 ) ; if ( loopsToDo == 0 ) { real_T
* Cs = ( real_T * ) rtDW . oh3rj3rgch . CS ; real_T * Ds = ( real_T * ) rtDW
. oh3rj3rgch . DS ; { int_T i1 ; real_T * y0 = & rtB . bkf5f3ujyq [ 0 ] ; for
( i1 = 0 ; i1 < 50 ; i1 ++ ) { accum = 0.0 ; { int_T i2 ; real_T * xd = &
rtDW . pwalsrbpvg [ 0 ] ; for ( i2 = 0 ; i2 < 6 ; i2 ++ ) { accum += * ( Cs
++ ) * xd [ i2 ] ; } } { int_T i2 ; const real_T * u0 = & rtP .
SwitchCurrents_Value [ 0 ] ; for ( i2 = 0 ; i2 < 6 ; i2 ++ ) { accum += * (
Ds ++ ) * u0 [ i2 ] ; } accum += * ( Ds ++ ) * rtB . ocwtlsstoa [ 0 ] ; accum
+= * ( Ds ++ ) * rtB . ocwtlsstoa [ 1 ] ; accum += * ( Ds ++ ) * rtB .
kc2wsb45fs ; accum += * ( Ds ++ ) * rtB . l2zafpzrzu ; accum += * ( Ds ++ ) *
rtB . ohedl35wf1 ; } y0 [ i1 ] = accum ; } } } { int_T i1 ; real_T * y1 = &
rtB . mcctrqnlij [ 0 ] ; for ( i1 = 0 ; i1 < 6 ; i1 ++ ) { y1 [ i1 ] = (
real_T ) switch_status [ i1 ] ; } } } } rtB . cuumld5jhq = rtX . lpzjbjknwp ;
{ real_T * * uBuffer = ( real_T * * ) & rtDW . b1o5sd1mu2 . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & rtDW . b1o5sd1mu2 . TUbufferPtrs [
1 ] ; real_T simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay ; rtB . czblwzh330 = rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , rtDW . drr2cwynj5 . CircularBufSize , & rtDW
. drr2cwynj5 . Last , rtDW . drr2cwynj5 . Tail , rtDW . drr2cwynj5 . Head ,
rtP . TransportDelay_InitOutput , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( rtS
) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
ed1n42eefk = ssGetT ( rtS ) ; rtB . lvq5fe4dss = ( rtB . ed1n42eefk >= rtP .
K1_Value ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . i524ua1zo0 = rtDW .
jfihwsipvc ; } if ( rtB . lvq5fe4dss ) { rtB . jjuwkhbaqb = rtB . cuumld5jhq
- rtB . czblwzh330 ; rtB . ks1ri00jvc = rtP . Gain_Gain_f0nj4ejddp * rtB .
jjuwkhbaqb ; rtB . e11hdbixe4 = rtB . ks1ri00jvc ; } else { rtB . e11hdbixe4
= rtB . i524ua1zo0 ; } rtB . f3bj13s54m = rtX . oi5s2zktut ; { real_T * *
uBuffer = ( real_T * * ) & rtDW . cvivv0qvki . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & rtDW . cvivv0qvki . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_gkek32v2fo ; rtB . dljdyoojd5 = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . fymd3ellif .
CircularBufSize , & rtDW . fymd3ellif . Last , rtDW . fymd3ellif . Tail ,
rtDW . fymd3ellif . Head , rtP . TransportDelay_InitOutput_gepskaykr5 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . jfbplyjjoy = ssGetT ( rtS ) ; rtB . hswklfu3tm
= ( rtB . jfbplyjjoy >= rtP . K1_Value_mjkioortvy ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . mcclxrnvnu = rtDW . k2r45klkvr ; } if ( rtB .
hswklfu3tm ) { rtB . orqdb4jq0f = rtB . f3bj13s54m - rtB . dljdyoojd5 ; rtB .
l25oveef2i = rtP . Gain_Gain * rtB . orqdb4jq0f ; rtB . dv5odcc5un = rtB .
l25oveef2i ; } else { rtB . dv5odcc5un = rtB . mcclxrnvnu ; } rtB .
jzulpokwia . re = rtB . e11hdbixe4 ; rtB . jzulpokwia . im = rtB . dv5odcc5un
; rtB . hu45skyzd5 = muDoubleScalarHypot ( rtB . jzulpokwia . re , rtB .
jzulpokwia . im ) ; rtB . nxmiliesal = muDoubleScalarAtan2 ( rtB . jzulpokwia
. im , rtB . jzulpokwia . re ) ; rtB . lz5rdlkvrc = rtP . RadDeg_Gain * rtB .
nxmiliesal ; rtB . a2aelbuuxd = rtX . pz2xsnrzjc ; { real_T * * uBuffer = (
real_T * * ) & rtDW . ecv4fwadro . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & rtDW . ecv4fwadro . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; real_T tMinusDelay = simTime - rtP .
TransportDelay_Delay_mvxoy0gg0s ; rtB . lb3o0vdenr = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , rtDW . eyb4fhkbdy .
CircularBufSize , & rtDW . eyb4fhkbdy . Last , rtDW . eyb4fhkbdy . Tail ,
rtDW . eyb4fhkbdy . Head , rtP . TransportDelay_InitOutput_a33cxqs542 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) ==
ssGetT ( rtS ) ) ) ) ; } rtB . lsnwzrnmnc = ssGetT ( rtS ) ; rtB . nhjhuszeqq
= ( rtB . lsnwzrnmnc >= rtP . K1_Value_k2kyb4ovnj ) ; if ( ssIsSampleHit (
rtS , 1 , 0 ) ) { rtB . m4o5gfq3hr = rtDW . lawcljffga ; } if ( rtB .
nhjhuszeqq ) { rtB . prtdictlyg = rtB . a2aelbuuxd - rtB . lb3o0vdenr ; rtB .
bzogwznvbx = rtP . Gain_Gain_kn1p3uhx5m * rtB . prtdictlyg ; rtB . o1nbxtrkf4
= rtB . bzogwznvbx ; } else { rtB . o1nbxtrkf4 = rtB . m4o5gfq3hr ; } rtB .
jpdedqtzey = rtX . hfv4dc3cpb ; { real_T * * uBuffer = ( real_T * * ) & rtDW
. ohddfojdjk . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
rtDW . ohddfojdjk . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ;
real_T tMinusDelay = simTime - rtP . TransportDelay_Delay_bifxyqg4be ; rtB .
jn3gd54hx5 = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, rtDW . ek4mryl130 . CircularBufSize , & rtDW . ek4mryl130 . Last , rtDW .
ek4mryl130 . Tail , rtDW . ek4mryl130 . Head , rtP .
TransportDelay_InitOutput_kfzzmtnzlj , 0 , ( boolean_T ) ( ssIsMinorTimeStep
( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; } rtB .
amlu2hdzhw = ssGetT ( rtS ) ; rtB . jgzslezpvv = ( rtB . amlu2hdzhw >= rtP .
K1_Value_pj1iwyknqz ) ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB .
jh3odldqrg = rtDW . bhgocl4yyi ; } if ( rtB . jgzslezpvv ) { rtB . n1w1gaeskg
= rtB . jpdedqtzey - rtB . jn3gd54hx5 ; rtB . euj2mhfzyp = rtP .
Gain_Gain_ksjae44pgn * rtB . n1w1gaeskg ; rtB . caiqrgrb4h = rtB . euj2mhfzyp
; } else { rtB . caiqrgrb4h = rtB . jh3odldqrg ; } rtB . dngbcewebp . re =
rtB . o1nbxtrkf4 ; rtB . dngbcewebp . im = rtB . caiqrgrb4h ; rtB .
b5yrakypqk = muDoubleScalarHypot ( rtB . dngbcewebp . re , rtB . dngbcewebp .
im ) ; rtB . kyvh55ysq1 = muDoubleScalarAtan2 ( rtB . dngbcewebp . im , rtB .
dngbcewebp . re ) ; rtB . njwnc1thzp = rtP . RadDeg_Gain_mu3nul0msz * rtB .
kyvh55ysq1 ; rtB . fvgxbp0r0b = rtB . lz5rdlkvrc - rtB . njwnc1thzp ; rtB .
h33eecit0h = rtP . Gain4_Gain * rtB . fvgxbp0r0b ; rtB . hcnu41udwm =
muDoubleScalarCos ( rtB . h33eecit0h ) ; rtB . ixvhkcticg = rtP .
Gain_Gain_hktcntwr5c * rtB . hu45skyzd5 ; rtB . ihbtz02goz = rtP .
Gain1_Gain_nmin1oira0 * rtB . b5yrakypqk ; rtB . bz4ixbddyb = rtP . Pm1_Value
* rtB . hcnu41udwm * rtB . ixvhkcticg * rtB . ihbtz02goz ; rtB . nzu205zzco =
rtP . u_Gain * rtB . bz4ixbddyb ; rtB . bbfgsoogaz = muDoubleScalarSin ( rtB
. h33eecit0h ) ; rtB . dkxzgq2elf = rtP . Pm1_Value * rtB . bbfgsoogaz * rtB
. ixvhkcticg * rtB . ihbtz02goz ; rtB . n1sf2oxpy1 = rtP . u_Gain_prbig4sao4
* rtB . dkxzgq2elf ; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { if (
ssGetLogOutput ( rtS ) ) { StructLogVar * svar = ( StructLogVar * ) rtDW .
m0scdgvmro . LoggedData [ 0 ] ; LogVar * var = svar -> signals . values ; {
double locTime = ssGetTaskTime ( rtS , 1 ) ; ; rt_UpdateLogVar ( ( LogVar * )
svar -> time , & locTime , 0 ) ; } { real_T up0 [ 1 ] ; up0 [ 0 ] = rtB .
nzu205zzco ; rt_UpdateLogVar ( ( LogVar * ) var , up0 , 0 ) ; var = var ->
next ; } { real_T up1 [ 1 ] ; up1 [ 0 ] = rtB . n1sf2oxpy1 ; rt_UpdateLogVar
( ( LogVar * ) var , up1 , 0 ) ; } } if ( ssGetLogOutput ( rtS ) ) {
StructLogVar * svar = ( StructLogVar * ) rtDW . exi0svnnxz . LoggedData [ 0 ]
; LogVar * var = svar -> signals . values ; { double locTime = ssGetTaskTime
( rtS , 1 ) ; ; rt_UpdateLogVar ( ( LogVar * ) svar -> time , & locTime , 0 )
; } { real_T up0 [ 1 ] ; up0 [ 0 ] = rtB . lz5rdlkvrc ; rt_UpdateLogVar ( (
LogVar * ) var , up0 , 0 ) ; var = var -> next ; } { real_T up1 [ 1 ] ; up1 [
0 ] = rtB . fvgxbp0r0b ; rt_UpdateLogVar ( ( LogVar * ) var , up1 , 0 ) ; } }
if ( ssGetLogOutput ( rtS ) ) { StructLogVar * svar = ( StructLogVar * ) rtDW
. apk42thrxw . LoggedData [ 0 ] ; LogVar * var = svar -> signals . values ; {
double locTime = ssGetTaskTime ( rtS , 1 ) ; ; rt_UpdateLogVar ( ( LogVar * )
svar -> time , & locTime , 0 ) ; } { real_T up0 [ 1 ] ; up0 [ 0 ] = rtB .
ixvhkcticg ; rt_UpdateLogVar ( ( LogVar * ) var , up0 , 0 ) ; var = var ->
next ; } { real_T up1 [ 1 ] ; up1 [ 0 ] = rtB . ihbtz02goz ; rt_UpdateLogVar
( ( LogVar * ) var , up1 , 0 ) ; } } } if ( ssIsSampleHit ( rtS , 2 , 0 ) ) {
rtB . bsjs3yr3cm = rtP . donotdeletethisgain_Gain * rtB . bkf5f3ujyq [ 23 ] ;
rtB . omi22g0ht3 = rtP . donotdeletethisgain_Gain_mrsxwkh4tp * rtB .
bkf5f3ujyq [ 24 ] ; rtB . maqdked4sl = rtP .
donotdeletethisgain_Gain_lbvwbky1pa * rtB . bkf5f3ujyq [ 25 ] ; rtB .
l0q0zwxfui [ 0 ] = rtP . Kv1_Gain * rtB . bsjs3yr3cm ; rtB . l0q0zwxfui [ 1 ]
= rtP . Kv1_Gain * rtB . omi22g0ht3 ; rtB . l0q0zwxfui [ 2 ] = rtP . Kv1_Gain
* rtB . maqdked4sl ; rtB . oecmwuiwrp = rtP .
donotdeletethisgain_Gain_jxggkyb4oj * rtB . bkf5f3ujyq [ 44 ] ; rtB .
m15cezxam4 = rtP . donotdeletethisgain_Gain_p0laf5fkqn * rtB . bkf5f3ujyq [
45 ] ; rtB . brcpuhmr2n = rtP . donotdeletethisgain_Gain_j5mfsbj1ew * rtB .
bkf5f3ujyq [ 46 ] ; rtB . cpugwweowj [ 0 ] = rtP . Kv_Gain * rtB . oecmwuiwrp
; rtB . cpugwweowj [ 1 ] = rtP . Kv_Gain * rtB . m15cezxam4 ; rtB .
cpugwweowj [ 2 ] = rtP . Kv_Gain * rtB . brcpuhmr2n ; if ( ssGetLogOutput (
rtS ) ) { StructLogVar * svar = ( StructLogVar * ) rtDW . m44vudu504 .
LoggedData [ 0 ] ; LogVar * var = svar -> signals . values ; { double locTime
= ssGetTaskTime ( rtS , 2 ) ; ; rt_UpdateLogVar ( ( LogVar * ) svar -> time ,
& locTime , 0 ) ; } { real_T up0 [ 3 ] ; up0 [ 0 ] = rtB . l0q0zwxfui [ 0 ] ;
up0 [ 1 ] = rtB . l0q0zwxfui [ 1 ] ; up0 [ 2 ] = rtB . l0q0zwxfui [ 2 ] ;
rt_UpdateLogVar ( ( LogVar * ) var , up0 , 0 ) ; var = var -> next ; } {
real_T up1 [ 3 ] ; up1 [ 0 ] = rtB . cpugwweowj [ 0 ] ; up1 [ 1 ] = rtB .
cpugwweowj [ 1 ] ; up1 [ 2 ] = rtB . cpugwweowj [ 2 ] ; rt_UpdateLogVar ( (
LogVar * ) var , up1 , 0 ) ; } } if ( rtP . Constant1_Value_nw4shdp34n ) {
rtB . gqo15jq4yq = rtB . d2l3x5dmoq [ 5 ] ; } else { rtB . gqo15jq4yq = rtP .
Constant2_Value ; } rtB . hwnr1apibg [ 0 ] = rtP . uLl_q_Gain_nj2ycls33u [ 0
] * rtB . mshc0z4y0b [ 0 ] ; rtB . hwnr1apibg [ 1 ] = rtP .
uLl_q_Gain_nj2ycls33u [ 1 ] * rtB . mshc0z4y0b [ 4 ] ; rtB . hwnr1apibg [ 2 ]
= rtP . uLl_q_Gain_nj2ycls33u [ 2 ] * rtB . mshc0z4y0b [ 5 ] ; if ( rtP .
Constant8_Value >= rtP . Switch3_Threshold ) { rtB . by0csc0mva [ 0 ] = rtB .
gtavo0wfmk ; rtB . by0csc0mva [ 1 ] = rtB . o5o0z0ts4i ; } else { rtB .
by0csc0mva [ 0 ] = rtP . Laqd_nosat_Value [ 0 ] ; rtB . by0csc0mva [ 1 ] =
rtP . Laqd_nosat_Value [ 1 ] ; } rtB . l1lpbegscp [ 0 ] = rtB . hwnr1apibg [
0 ] * rtB . by0csc0mva [ 0 ] ; rtB . l1lpbegscp [ 1 ] = rtB . hwnr1apibg [ 1
] * rtB . by0csc0mva [ 0 ] ; rtB . l1lpbegscp [ 2 ] = rtB . hwnr1apibg [ 2 ]
* rtB . by0csc0mva [ 0 ] ; tmpForInput_idx_0 = rtB . l1lpbegscp [ 0 ] ; rtB .
h5nuuqtufn [ 0 ] = rtP . uLl_d_Gain_mutaj0uj3h [ 0 ] * rtB . mshc0z4y0b [ 1 ]
; rtB . bdvh4jtcpt [ 0 ] = rtB . h5nuuqtufn [ 0 ] * rtB . by0csc0mva [ 1 ] ;
tmpForInput_idx_0 += rtB . l1lpbegscp [ 1 ] ; rtB . h5nuuqtufn [ 1 ] = rtP .
uLl_d_Gain_mutaj0uj3h [ 1 ] * rtB . mshc0z4y0b [ 2 ] ; rtB . bdvh4jtcpt [ 1 ]
= rtB . h5nuuqtufn [ 1 ] * rtB . by0csc0mva [ 1 ] ; tmpForInput_idx_0 += rtB
. l1lpbegscp [ 2 ] ; rtB . h5nuuqtufn [ 2 ] = rtP . uLl_d_Gain_mutaj0uj3h [ 2
] * rtB . mshc0z4y0b [ 3 ] ; rtB . bdvh4jtcpt [ 2 ] = rtB . h5nuuqtufn [ 2 ]
* rtB . by0csc0mva [ 1 ] ; rtB . ck4g14oqxl = tmpForInput_idx_0 ;
tmpForInput_idx_0 = rtB . bdvh4jtcpt [ 0 ] ; tmpForInput_idx_0 += rtB .
bdvh4jtcpt [ 1 ] ; tmpForInput_idx_0 += rtB . bdvh4jtcpt [ 2 ] ; rtB .
l5qxq2mybh = tmpForInput_idx_0 ; if ( ssIsSampleHit ( rtS , 2 , 0 ) ) { rtB .
do04422dsh [ 0 ] = rtB . bkf5f3ujyq [ 6 ] ; rtB . do04422dsh [ 1 ] = rtB .
bkf5f3ujyq [ 7 ] ; } rtB . kctgawizah [ 0 ] = rtP . u_Vb_Gain * rtB .
do04422dsh [ 0 ] ; rtB . kctgawizah [ 1 ] = rtP . u_Vb_Gain * rtB .
do04422dsh [ 1 ] ; rtB . g2rmjbbycl = ( ( 2.0 * rtB . kctgawizah [ 0 ] + rtB
. kctgawizah [ 1 ] ) * rtB . pz2b2ixe3g + 1.7320508075688772 * rtB .
kctgawizah [ 1 ] * rtB . ps502vbzlw ) * 0.33333333333333331 ; rtB .
ljwiyate1t = ( ( 2.0 * rtB . kctgawizah [ 0 ] + rtB . kctgawizah [ 1 ] ) *
rtB . ps502vbzlw + - 1.7320508075688772 * rtB . kctgawizah [ 1 ] * rtB .
pz2b2ixe3g ) * 0.33333333333333331 ; if ( rtP . Constant5_Value >= rtP .
Switch2_Threshold_auuawawddv ) { rtB . lwg1hzp3ql [ 0 ] = rtB . bzlrt5abhc ;
rtB . lwg1hzp3ql [ 1 ] = rtB . msc0q3ydkn ; } else { rtB . lwg1hzp3ql [ 0 ] =
rtP . Lmqd_nosat_Value [ 0 ] ; rtB . lwg1hzp3ql [ 1 ] = rtP .
Lmqd_nosat_Value [ 1 ] ; } rtB . c4xvwz5dvp = muDoubleScalarAtan2 ( rtB .
ljwiyate1t , rtB . g2rmjbbycl ) ; rtB . hv3y4xtyo5 = rtP .
Gain_Gain_olfnewahm1 * rtB . c4xvwz5dvp ; rtB . otvyxcolz0 = rtB . g2rmjbbycl
* rtB . d2l3x5dmoq [ 0 ] ; rtB . ca1sh32w2e = rtB . ljwiyate1t * rtB .
d2l3x5dmoq [ 1 ] ; rtB . ogxs2lrdho = rtB . otvyxcolz0 + rtB . ca1sh32w2e ;
rtB . n4glgvjxv2 = rtP . Gain_Gain_pycxugwvd0 * rtB . ogxs2lrdho ; rtB .
dq2eyjw50a = rtB . g2rmjbbycl * rtB . d2l3x5dmoq [ 1 ] ; rtB . fyirte2cly =
rtB . ljwiyate1t * rtB . d2l3x5dmoq [ 0 ] ; rtB . i2tqhru53h = rtB .
dq2eyjw50a - rtB . fyirte2cly ; rtB . ahl15fuofz = rtP .
Gain1_Gain_j0bqxbk0lz * rtB . i2tqhru53h ; rtB . nnbfzmzsox [ 0 ] = rtP .
outputformatting_Gain [ 0 ] * rtB . a3kfgrogpp ; rtB . nnbfzmzsox [ 1 ] = rtP
. outputformatting_Gain [ 1 ] * rtB . dggh1ark2b ; rtB . nnbfzmzsox [ 2 ] =
rtP . outputformatting_Gain [ 2 ] * rtB . hleuoxbvtr ; rtB . nnbfzmzsox [ 3 ]
= rtP . outputformatting_Gain [ 3 ] * rtB . d2l3x5dmoq [ 0 ] ; rtB .
nnbfzmzsox [ 4 ] = rtP . outputformatting_Gain [ 4 ] * rtB . d2l3x5dmoq [ 1 ]
; rtB . nnbfzmzsox [ 5 ] = rtP . outputformatting_Gain [ 5 ] * rtB .
d2l3x5dmoq [ 2 ] ; rtB . nnbfzmzsox [ 6 ] = rtP . outputformatting_Gain [ 6 ]
* rtB . d2l3x5dmoq [ 4 ] ; rtB . nnbfzmzsox [ 7 ] = rtP .
outputformatting_Gain [ 7 ] * rtB . gqo15jq4yq ; rtB . nnbfzmzsox [ 8 ] = rtP
. outputformatting_Gain [ 8 ] * rtB . d2l3x5dmoq [ 3 ] ; rtB . nnbfzmzsox [ 9
] = rtP . outputformatting_Gain [ 9 ] * rtB . ck4g14oqxl ; rtB . nnbfzmzsox [
10 ] = rtP . outputformatting_Gain [ 10 ] * rtB . l5qxq2mybh ; rtB .
nnbfzmzsox [ 11 ] = rtP . outputformatting_Gain [ 11 ] * rtB . g2rmjbbycl ;
rtB . nnbfzmzsox [ 12 ] = rtP . outputformatting_Gain [ 12 ] * rtB .
ljwiyate1t ; rtB . nnbfzmzsox [ 13 ] = rtP . outputformatting_Gain [ 13 ] *
rtB . lwg1hzp3ql [ 0 ] ; rtB . nnbfzmzsox [ 14 ] = rtP .
outputformatting_Gain [ 14 ] * rtB . lwg1hzp3ql [ 1 ] ; rtB . nnbfzmzsox [ 15
] = rtP . outputformatting_Gain [ 15 ] * rtB . hv3y4xtyo5 ; rtB . nnbfzmzsox
[ 16 ] = rtP . outputformatting_Gain [ 16 ] * rtB . n4glgvjxv2 ; rtB .
nnbfzmzsox [ 17 ] = rtP . outputformatting_Gain [ 17 ] * rtB . ahl15fuofz ;
if ( ssIsSampleHit ( rtS , 2 , 0 ) ) { memcpy ( & rtB . nddeflp2rr [ 0 ] , &
rtB . nnbfzmzsox [ 0 ] , 18U * sizeof ( real_T ) ) ; } rtB . b14bfbabib = rtP
. Gain_Gain_bpzmbe1g2k * rtB . nddeflp2rr [ 16 ] ; rtB . mwnm3xw2te = rtP .
Gain1_Gain_h3tkw2h2wq * rtB . nddeflp2rr [ 17 ] ; rtB . gpq0cumlrp = rtP .
Gain3_Gain * rtB . nddeflp2rr [ 5 ] ; rtB . ooqvccplqr = muDoubleScalarHypot
( rtB . g2rmjbbycl , rtB . ljwiyate1t ) ; rtB . bsib0j2uiv [ 0 ] = rtP .
u1_Gain [ 0 ] * rtB . d2l3x5dmoq [ 0 ] ; rtB . bsib0j2uiv [ 1 ] = rtP .
u1_Gain [ 1 ] * rtB . d2l3x5dmoq [ 1 ] ; rtB . iwzh4cmhgz [ 0 ] = rtB .
mshc0z4y0b [ 1 ] * rtB . bsib0j2uiv [ 0 ] ; rtB . iwzh4cmhgz [ 1 ] = rtB .
mshc0z4y0b [ 0 ] * rtB . bsib0j2uiv [ 1 ] ; tmpForInput_idx_0 = rtB .
iwzh4cmhgz [ 0 ] ; tmpForInput_idx_0 += rtB . iwzh4cmhgz [ 1 ] ; rtB .
kbpgzmhgnk = tmpForInput_idx_0 ; rtB . kt4yqwu22g = rtDW . ltyzpyes2j ; rtB .
eekpbkk0js = rtP . F2_Gain * rtB . kt4yqwu22g ; rtB . bzg42dyv2g = rtDW .
koephaho24 ; rtB . gilkef2gvf = rtB . eekpbkk0js - rtB . bzg42dyv2g ; rtB .
i3cpus0vrs = rtP . nominalspeed_Value + rtB . gilkef2gvf ; for ( i = 0 ; i <
6 ; i ++ ) { rtB . mnrqebdkci [ i ] = rtDW . bjkyi30h4y [ i ] ; } rtB .
gwy50czbqx = ssGetTaskTime ( rtS , 2 ) ; if ( rtB . gwy50czbqx >= rtP .
IC_Threshold ) { rtB . janqpo5qe4 [ 0 ] = rtB . g2rmjbbycl + rtB . mnrqebdkci
[ 0 ] ; rtB . janqpo5qe4 [ 1 ] = rtB . ljwiyate1t + rtB . mnrqebdkci [ 1 ] ;
rtB . janqpo5qe4 [ 2 ] = rtB . jo0ewhen5p + rtB . mnrqebdkci [ 2 ] ; rtB .
janqpo5qe4 [ 3 ] = rtP . Vkd0Vkq10Vkq20_Value [ 0 ] + rtB . mnrqebdkci [ 3 ]
; rtB . janqpo5qe4 [ 4 ] = rtP . Vkd0Vkq10Vkq20_Value [ 1 ] + rtB .
mnrqebdkci [ 4 ] ; rtB . janqpo5qe4 [ 5 ] = rtP . Vkd0Vkq10Vkq20_Value [ 2 ]
+ rtB . mnrqebdkci [ 5 ] ; if ( rtP . Constant2_Value_a35hujaypn >= rtP .
Switch1_Threshold_aikum5bfzk ) { memcpy ( & rtB . dx3rmsixh3 [ 0 ] , & rtB .
g5yq3s1oi3 [ 0 ] , 36U * sizeof ( real_T ) ) ; } else { memcpy ( & rtB .
dx3rmsixh3 [ 0 ] , & rtP . Constant6_Value [ 0 ] , 36U * sizeof ( real_T ) )
; } for ( i = 0 ; i < 36 ; i ++ ) { rtB . n0sluxojte [ i ] = rtP .
u1_Value_cbuqdiipgi [ i ] ; } rtB . n0sluxojte [ 6 ] = rtB . i3cpus0vrs ; rtB
. lyhqkuq1c4 = rtP . Gain1_Gain * rtB . i3cpus0vrs ; memcpy ( & rtB .
ofyfrkcfxb [ 0 ] , & rtB . n0sluxojte [ 0 ] , 36U * sizeof ( real_T ) ) ; rtB
. ofyfrkcfxb [ 1 ] = rtB . lyhqkuq1c4 ; for ( i = 0 ; i < 36 ; i ++ ) { rtB .
khdzdbpnht [ i ] = ( 0.0 - rtB . ofyfrkcfxb [ i ] ) - rtB . dx3rmsixh3 [ i ]
; rtB . dgwxdhwkdd [ i ] = rtP . wbaseTs2_Gain * rtB . khdzdbpnht [ i ] ; rtB
. caz1neybgm [ i ] = rtP . u5_Value_ochldviepj [ i ] - rtB . dgwxdhwkdd [ i ]
; } rt_invd6x6_snf ( rtB . caz1neybgm , rtB . gdroql4j2f ) ; for ( i = 0 ; i
< 36 ; i ++ ) { rtB . g0lz5qytlu [ i ] = rtP . wbaseTs2_Gain_agkgmtch40 * rtB
. gdroql4j2f [ i ] ; tmp_e [ i ] = rtB . g0lz5qytlu [ i ] ; } for ( i = 0 ; i
< 6 ; i ++ ) { tmp [ i ] = rtB . janqpo5qe4 [ i ] ; } for ( i_p = 0 ; i_p < 6
; i_p ++ ) { tmp_p [ i_p ] = 0.0 ; for ( i = 0 ; i < 6 ; i ++ ) {
tmpForInput_idx_0 = tmp_p [ i_p ] ; tmpForInput_idx_0 += tmp_e [ 6 * i + i_p
] * tmp [ i ] ; tmp_p [ i_p ] = tmpForInput_idx_0 ; } rtB . cbygkknxsv [ i_p
] = tmp_p [ i_p ] ; } for ( i = 0 ; i < 36 ; i ++ ) { rtB . ncbiuw0cin [ i ]
= rtP . u5_Value_ochldviepj [ i ] + rtB . dgwxdhwkdd [ i ] ; tmp_e [ i ] =
rtB . gdroql4j2f [ i ] ; tmp_i [ i ] = rtB . ncbiuw0cin [ i ] ; } for ( i_p =
0 ; i_p < 6 ; i_p ++ ) { for ( i = 0 ; i < 6 ; i ++ ) { rtB . deid52kk1x [ i
+ 6 * i_p ] = 0.0 ; for ( i_e = 0 ; i_e < 6 ; i_e ++ ) { rtB . deid52kk1x [ i
+ 6 * i_p ] += tmp_e [ 6 * i_e + i ] * tmp_i [ 6 * i_p + i_e ] ; } } } memcpy
( & tmp_e [ 0 ] , & rtB . deid52kk1x [ 0 ] , 36U * sizeof ( real_T ) ) ; for
( i = 0 ; i < 6 ; i ++ ) { tmp [ i ] = rtB . mshc0z4y0b [ i ] ; } for ( i = 0
; i < 6 ; i ++ ) { tmp_p [ i ] = 0.0 ; for ( i_p = 0 ; i_p < 6 ; i_p ++ ) {
tmpForInput_idx_0 = tmp_p [ i ] ; tmpForInput_idx_0 += tmp_e [ 6 * i_p + i ]
* tmp [ i_p ] ; tmp_p [ i ] = tmpForInput_idx_0 ; } rtB . c1v5fiqxar [ i ] =
tmp_p [ i ] ; rtB . cfmayp3lnt [ i ] = rtB . c1v5fiqxar [ i ] + rtB .
cbygkknxsv [ i ] ; } for ( i = 0 ; i < 6 ; i ++ ) { rtB . hjl4wiy52h [ i ] =
rtB . cfmayp3lnt [ i ] ; } } else { for ( i = 0 ; i < 6 ; i ++ ) { rtB .
hjl4wiy52h [ i ] = rtB . mshc0z4y0b [ i ] ; } } } rtB . ht3pkpouae =
muDoubleScalarSin ( rtP . sinwt_Freq * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase ) * rtP . sinwt_Amp + rtP . sinwt_Bias ; rtB . epw4e2nyr2 = rtB .
l0q0zwxfui [ 0 ] * rtB . ht3pkpouae ; rtB . onhlo5lk3i = muDoubleScalarSin (
rtP . coswt_Freq * ssGetTaskTime ( rtS , 0 ) + rtP . coswt_Phase ) * rtP .
coswt_Amp + rtP . coswt_Bias ; rtB . hvccs5ksdj = rtB . l0q0zwxfui [ 0 ] *
rtB . onhlo5lk3i ; rtB . mv11qhbyiu = muDoubleScalarSin ( rtP .
sinwt_Freq_a0silddums * ssGetTaskTime ( rtS , 0 ) + rtP .
sinwt_Phase_pauodsxphx ) * rtP . sinwt_Amp_ggpyeyy0ei + rtP .
sinwt_Bias_aiqrlpqukr ; rtB . lxuyvf0kct = rtB . cpugwweowj [ 0 ] * rtB .
mv11qhbyiu ; rtB . cpurtfy0lf = muDoubleScalarSin ( rtP .
coswt_Freq_dexl0rdjce * ssGetTaskTime ( rtS , 0 ) + rtP .
coswt_Phase_gsew12la0i ) * rtP . coswt_Amp_mejccdpyws + rtP .
coswt_Bias_acdrihp4mg ; rtB . hdfe5utvyn = rtB . cpugwweowj [ 0 ] * rtB .
cpurtfy0lf ; if ( ssIsSampleHit ( rtS , 1 , 0 ) && ( rtDW . m11yduvrgb == 0 )
) { rtDW . ayphut5w0e = rtP . VfPP1_Value ; } if ( ssIsSampleHit ( rtS , 2 ,
0 ) ) { rtDW . m11yduvrgb = 1 ; rtB . gsp2mdotvq = rtDW . ayphut5w0e ; rtDW .
m11yduvrgb = 0 ; rtB . cuthot4b55 = rtP . u_Pb_Gain * rtB . gsp2mdotvq ; rtB
. m3hbl5oi51 = rtB . cuthot4b55 / rtB . i3cpus0vrs ; rtB . kr1dizdsxr = rtP .
F_Gain * rtB . i3cpus0vrs ; rtB . gftya03rfs = ( rtB . m3hbl5oi51 - rtB .
kbpgzmhgnk ) - rtB . kr1dizdsxr ; rtB . fmoyvyrdin = rtP . u2H_Gain * rtB .
gftya03rfs ; rtB . h2yyrby5su = rtDW . cszisyhd2i ; tmpForInput_idx_0 = rtB .
h2yyrby5su ; tmpForInput_idx_0 = muDoubleScalarRem ( tmpForInput_idx_0 ,
6.2831853071795862 ) ; rtB . jyft0oavfh = tmpForInput_idx_0 ; rtB .
ax2dm5rpub = rtB . i3cpus0vrs * rtB . kbpgzmhgnk ; rtB . o3xqnnpyja = rtP .
units_Gain * rtB . i3cpus0vrs ; rtB . k33xjpkxfj = rtP . units1_Gain * rtB .
ax2dm5rpub ; rtB . hlupe3wd1s = rtP . t_Gain * rtB . jyft0oavfh ; rtB .
ogr2enhepf = rtP . units2_Gain * rtB . kbpgzmhgnk ; if ( ssIsSampleHit ( rtS
, 2 , 0 ) ) { rtB . bxfyqcorw3 [ 0 ] = rtB . o0laqbijxj ; rtB . bxfyqcorw3 [
1 ] = rtB . o3xqnnpyja ; rtB . bxfyqcorw3 [ 2 ] = rtB . k33xjpkxfj ; rtB .
bxfyqcorw3 [ 3 ] = rtB . gilkef2gvf ; rtB . bxfyqcorw3 [ 4 ] = rtB .
hlupe3wd1s ; rtB . bxfyqcorw3 [ 5 ] = rtB . ogr2enhepf ; } if ( rtDW .
lkdnaytcjn != 0 ) { rtB . dn1jsu5b3a = rtDW . ksbtbqie2m ; } else { rtB .
dn1jsu5b3a = rtP . Rotorspeeddeviationdw_gainval * rtB . fmoyvyrdin + rtDW .
ksbtbqie2m ; } rtB . anv20tflb2 = rtP . webase_Gain * rtB . gilkef2gvf ; rtB
. eajcmlcqmc = rtP . web3_Gain * rtB . i3cpus0vrs ; rtB . ejzz1lck3x =
ssGetTaskTime ( rtS , 2 ) ; rtB . clkwduumjv = rt_Lookup ( rtP .
LookUpTable_XData_i4mu3ctko4 , 6 , rtB . ejzz1lck3x , rtP .
LookUpTable_YData_epertdpbpx ) ; if ( rtP . C4_Value_gcbzcnw2m4 >= rtP .
Switch3_Threshold_o2qhhfyd2s ) { rtB . geos3slncn = rtP . com_Value ; } else
{ rtB . geos3slncn = rtB . clkwduumjv ; } rtB . jsc1e3jlwj = ( rtB .
geos3slncn != 0.0 ) ; rtB . gmd2h4cezq = ssGetTaskTime ( rtS , 2 ) ; if ( rtP
. C4_Value >= rtP . Switch3_Threshold_k0uagz5gcg ) { rtB . kgpjy2123a = ( rtB
. jsc1e3jlwj && rtB . hq4dyb3apy ) ; rtB . ecp5cwxuux = rtB . kgpjy2123a ;
rtB . fyt1uabhvi = rtB . ecp5cwxuux ; rtB . i20vaspyrp = rtB . fyt1uabhvi ; }
else { rtB . b133gvafim = rt_Lookup ( rtP . LookUpTable_XData , 5 , rtB .
gmd2h4cezq , rtP . LookUpTable_YData ) ; rtB . i20vaspyrp = rtB . b133gvafim
; } rtB . mzzuyzwyzo = ssGetTaskTime ( rtS , 2 ) ; if ( rtP .
C4_Value_fxleokhe0y >= rtP . Switch3_Threshold_icm4fmynnh ) { rtB .
dv21ecppse = ( rtB . jsc1e3jlwj && rtB . f05xiwzeii ) ; rtB . frxxgkpfd0 =
rtB . dv21ecppse ; rtB . if3uw30dno = rtB . frxxgkpfd0 ; rtB . oj12dzvfb4 =
rtB . if3uw30dno ; } else { rtB . i4qgt25dqa = rt_Lookup ( rtP .
LookUpTable_XData_ouijhmudsj , 5 , rtB . mzzuyzwyzo , rtP .
LookUpTable_YData_ibczk2yoa0 ) ; rtB . oj12dzvfb4 = rtB . i4qgt25dqa ; } rtB
. pu5azi4ouj = ssGetTaskTime ( rtS , 2 ) ; if ( rtP . C4_Value_eoqp0wijkf >=
rtP . Switch3_Threshold_hdegddai5u ) { rtB . ot21w1gcro = ( rtB . jsc1e3jlwj
&& rtB . kkthzbzxh4 ) ; rtB . guw3ibew5f = rtB . ot21w1gcro ; rtB .
fwobh2ydkx = rtB . guw3ibew5f ; rtB . p4d344o4a4 = rtB . fwobh2ydkx ; } else
{ rtB . kji2nsjrqr = rt_Lookup ( rtP . LookUpTable_XData_j5m2nusfgx , 5 , rtB
. pu5azi4ouj , rtP . LookUpTable_YData_esrnrccjov ) ; rtB . p4d344o4a4 = rtB
. kji2nsjrqr ; } rtB . pxtshpab0u = rtP . donotdeletethisgain_Gain_g4a3i3bkvy
* rtB . bkf5f3ujyq [ 35 ] ; rtB . orydyp31zs = rtP .
donotdeletethisgain_Gain_p2gxvtukfv * rtB . bkf5f3ujyq [ 36 ] ; rtB .
btppbtrakx = rtP . donotdeletethisgain_Gain_is2ce52gq3 * rtB . bkf5f3ujyq [
37 ] ; rtB . embtezfk0j [ 0 ] = rtP . Kv_Gain_b1jtzjwn5z * rtB . pxtshpab0u ;
rtB . embtezfk0j [ 1 ] = rtP . Kv_Gain_b1jtzjwn5z * rtB . orydyp31zs ; rtB .
embtezfk0j [ 2 ] = rtP . Kv_Gain_b1jtzjwn5z * rtB . btppbtrakx ; rtB .
icofxpvarr = rtP . donotdeletethisgain_Gain_h5sheo5agc * rtB . bkf5f3ujyq [
14 ] ; rtB . n3yespd0xf = rtP . donotdeletethisgain_Gain_idjvkwlnjp * rtB .
bkf5f3ujyq [ 15 ] ; rtB . kfeoejnqkx = rtP .
donotdeletethisgain_Gain_iiwqs0rjpv * rtB . bkf5f3ujyq [ 16 ] ; rtB .
i5mjxvb2y1 [ 0 ] = rtP . Kv1_Gain_fhx1xa0qe5 * rtB . icofxpvarr ; rtB .
i5mjxvb2y1 [ 1 ] = rtP . Kv1_Gain_fhx1xa0qe5 * rtB . n3yespd0xf ; rtB .
i5mjxvb2y1 [ 2 ] = rtP . Kv1_Gain_fhx1xa0qe5 * rtB . kfeoejnqkx ; rtB .
kalgaozq2e = rtP . donotdeletethisgain_Gain_jc3l43jy02 * rtB . bkf5f3ujyq [
32 ] ; rtB . c3mov1tl1f = rtP . donotdeletethisgain_Gain_ixohazj2yz * rtB .
bkf5f3ujyq [ 33 ] ; rtB . mj5ueklf54 = rtP .
donotdeletethisgain_Gain_ijcgmen2vo * rtB . bkf5f3ujyq [ 34 ] ; rtB .
chk1pd3rlp [ 0 ] = rtP . Kv_Gain_hbrwnhtc5z * rtB . kalgaozq2e ; rtB .
chk1pd3rlp [ 1 ] = rtP . Kv_Gain_hbrwnhtc5z * rtB . c3mov1tl1f ; rtB .
chk1pd3rlp [ 2 ] = rtP . Kv_Gain_hbrwnhtc5z * rtB . mj5ueklf54 ; rtB .
nfbe1f5aq4 = rtP . donotdeletethisgain_Gain_dftutozx3l * rtB . bkf5f3ujyq [
11 ] ; rtB . gpbkfrtopv = rtP . donotdeletethisgain_Gain_dvccqwmfov * rtB .
bkf5f3ujyq [ 12 ] ; rtB . emcqpstfhw = rtP .
donotdeletethisgain_Gain_b3iqzvyj5x * rtB . bkf5f3ujyq [ 13 ] ; rtB .
o04nrxwopl [ 0 ] = rtP . Kv1_Gain_bls2zebxwe * rtB . nfbe1f5aq4 ; rtB .
o04nrxwopl [ 1 ] = rtP . Kv1_Gain_bls2zebxwe * rtB . gpbkfrtopv ; rtB .
o04nrxwopl [ 2 ] = rtP . Kv1_Gain_bls2zebxwe * rtB . emcqpstfhw ; rtB .
m315dx4bxe = rtP . donotdeletethisgain_Gain_ckgop4gksi * rtB . bkf5f3ujyq [
38 ] ; rtB . lixhx51dy4 = rtP . donotdeletethisgain_Gain_gex5e0120e * rtB .
bkf5f3ujyq [ 39 ] ; rtB . arirsm2gtb = rtP .
donotdeletethisgain_Gain_mnyedn5jj1 * rtB . bkf5f3ujyq [ 40 ] ; rtB .
aei40fflxj [ 0 ] = rtP . Kv_Gain_dc42uxbym3 * rtB . m315dx4bxe ; rtB .
aei40fflxj [ 1 ] = rtP . Kv_Gain_dc42uxbym3 * rtB . lixhx51dy4 ; rtB .
aei40fflxj [ 2 ] = rtP . Kv_Gain_dc42uxbym3 * rtB . arirsm2gtb ; rtB .
pchaeyb4wc = rtP . donotdeletethisgain_Gain_nsmmc33j4l * rtB . bkf5f3ujyq [
17 ] ; rtB . geuzk04gqe = rtP . donotdeletethisgain_Gain_oc13lni3fq * rtB .
bkf5f3ujyq [ 18 ] ; rtB . nx2s4jnhlc = rtP .
donotdeletethisgain_Gain_ghrwzwq2wl * rtB . bkf5f3ujyq [ 19 ] ; rtB .
k3tazd503n [ 0 ] = rtP . Kv1_Gain_js4lqtbwg1 * rtB . pchaeyb4wc ; rtB .
k3tazd503n [ 1 ] = rtP . Kv1_Gain_js4lqtbwg1 * rtB . geuzk04gqe ; rtB .
k3tazd503n [ 2 ] = rtP . Kv1_Gain_js4lqtbwg1 * rtB . nx2s4jnhlc ; rtB .
c5z4wv1mbn = rtP . donotdeletethisgain_Gain_inxi5cfmng * rtB . bkf5f3ujyq [
29 ] ; rtB . ixbmptgikc = rtP . donotdeletethisgain_Gain_enynbgjy2q * rtB .
bkf5f3ujyq [ 30 ] ; rtB . g1pav31o54 = rtP .
donotdeletethisgain_Gain_ghgnnpri0h * rtB . bkf5f3ujyq [ 31 ] ; rtB .
g2dr1qmh1g [ 0 ] = rtP . Kv_Gain_gr1trvmmq4 * rtB . c5z4wv1mbn ; rtB .
g2dr1qmh1g [ 1 ] = rtP . Kv_Gain_gr1trvmmq4 * rtB . ixbmptgikc ; rtB .
g2dr1qmh1g [ 2 ] = rtP . Kv_Gain_gr1trvmmq4 * rtB . g1pav31o54 ; rtB .
or1sotd5ok = rtP . donotdeletethisgain_Gain_fdstflfpdl * rtB . bkf5f3ujyq [ 8
] ; rtB . oawoasztyv = rtP . donotdeletethisgain_Gain_hwxarrm2xf * rtB .
bkf5f3ujyq [ 9 ] ; rtB . ez3n02akaw = rtP .
donotdeletethisgain_Gain_jpsgb554cq * rtB . bkf5f3ujyq [ 10 ] ; rtB .
hm1ran4pnq [ 0 ] = rtP . Kv1_Gain_lejhoixvzv * rtB . or1sotd5ok ; rtB .
hm1ran4pnq [ 1 ] = rtP . Kv1_Gain_lejhoixvzv * rtB . oawoasztyv ; rtB .
hm1ran4pnq [ 2 ] = rtP . Kv1_Gain_lejhoixvzv * rtB . ez3n02akaw ; rtB .
im0zegy3zl = rtP . donotdeletethisgain_Gain_akjxwy4mzh * rtB . bkf5f3ujyq [
47 ] ; rtB . ca0k4g2ixd = rtP . donotdeletethisgain_Gain_m1o3yt3pmb * rtB .
bkf5f3ujyq [ 48 ] ; rtB . g4um4nifj0 = rtP .
donotdeletethisgain_Gain_bdqv4z3h20 * rtB . bkf5f3ujyq [ 49 ] ; rtB .
bv31jmhnur [ 0 ] = rtP . Kv_Gain_dryr11wp4y * rtB . im0zegy3zl ; rtB .
bv31jmhnur [ 1 ] = rtP . Kv_Gain_dryr11wp4y * rtB . ca0k4g2ixd ; rtB .
bv31jmhnur [ 2 ] = rtP . Kv_Gain_dryr11wp4y * rtB . g4um4nifj0 ; rtB .
fgeeodibr2 = rtP . donotdeletethisgain_Gain_dyxa4y4pb4 * rtB . bkf5f3ujyq [
26 ] ; rtB . lzor53a1zc = rtP . donotdeletethisgain_Gain_kgtmebmbqg * rtB .
bkf5f3ujyq [ 27 ] ; rtB . a4mondaqym = rtP .
donotdeletethisgain_Gain_pn4mhzapma * rtB . bkf5f3ujyq [ 28 ] ; rtB .
ikwzxkxyiz [ 0 ] = rtP . Kv1_Gain_itak2vpaxj * rtB . fgeeodibr2 ; rtB .
ikwzxkxyiz [ 1 ] = rtP . Kv1_Gain_itak2vpaxj * rtB . lzor53a1zc ; rtB .
ikwzxkxyiz [ 2 ] = rtP . Kv1_Gain_itak2vpaxj * rtB . a4mondaqym ; rtB .
e3fyv2i0xt = rtP . donotdeletethisgain_Gain_lb32xlqutz * rtB . bkf5f3ujyq [
41 ] ; rtB . pv0jl14wdm = rtP . donotdeletethisgain_Gain_e1t00s2jpf * rtB .
bkf5f3ujyq [ 42 ] ; rtB . gkxjm351v3 = rtP .
donotdeletethisgain_Gain_kznkprn5og * rtB . bkf5f3ujyq [ 43 ] ; rtB .
gqlskp40nb [ 0 ] = rtP . Kv_Gain_ccfkg0ba2b * rtB . e3fyv2i0xt ; rtB .
gqlskp40nb [ 1 ] = rtP . Kv_Gain_ccfkg0ba2b * rtB . pv0jl14wdm ; rtB .
gqlskp40nb [ 2 ] = rtP . Kv_Gain_ccfkg0ba2b * rtB . gkxjm351v3 ; rtB .
cjgzre1rfd = rtP . donotdeletethisgain_Gain_krlg4qa22u * rtB . bkf5f3ujyq [
20 ] ; rtB . mmqgxd3ztq = rtP . donotdeletethisgain_Gain_hxax2d03xl * rtB .
bkf5f3ujyq [ 21 ] ; rtB . kfya4ws5wt = rtP .
donotdeletethisgain_Gain_fgel1ns2zy * rtB . bkf5f3ujyq [ 22 ] ; rtB .
czdxilbjs1 [ 0 ] = rtP . Kv1_Gain_oi10siy4fh * rtB . cjgzre1rfd ; rtB .
czdxilbjs1 [ 1 ] = rtP . Kv1_Gain_oi10siy4fh * rtB . mmqgxd3ztq ; rtB .
czdxilbjs1 [ 2 ] = rtP . Kv1_Gain_oi10siy4fh * rtB . kfya4ws5wt ; rtB .
kflvqeslk3 = ssGetTaskTime ( rtS , 2 ) ; rtB . pubftwd23q = rt_Lookup ( rtP .
LookUpTable_XData_bysejxwrcs , 6 , rtB . kflvqeslk3 , rtP .
LookUpTable_YData_anwymjpndi ) ; if ( rtP . C4_Value_ouf3ao4h2o >= rtP .
Switch3_Threshold_js4dz2naas ) { rtB . abudmgdmqo = rtP .
com_Value_e0niij5lrb ; } else { rtB . abudmgdmqo = rtB . pubftwd23q ; } rtB .
jnufx11sbb = ssGetTaskTime ( rtS , 2 ) ; if ( rtP . C4_Value_gbl0dsqzhj >=
rtP . Switch3_Threshold_g44ubd0yky ) { if ( rtP . Q2_SwitchA >= rtP .
Switch_Threshold ) { rtB . hfxqkjhn3c = rtB . abudmgdmqo ; } else { rtB .
hfxqkjhn3c = rtP . Constant5_Value_eyvmukmjzw ; } rtB . gh0e24cnt4 = rtB .
hfxqkjhn3c ; rtB . mnz0k5z1ev = rtB . gh0e24cnt4 ; } else { rtB . oq2fmk4tkh
= rt_Lookup ( rtP . LookUpTable_XData_fglubeidif , 5 , rtB . jnufx11sbb , rtP
. LookUpTable_YData_hhayxsnnev ) ; rtB . mnz0k5z1ev = rtB . oq2fmk4tkh ; }
rtB . n3cickajoe = ssGetTaskTime ( rtS , 2 ) ; if ( rtP . C4_Value_exqcfpjqxr
>= rtP . Switch3_Threshold_c15c5kf1pp ) { if ( rtP . Q2_SwitchB >= rtP .
Switch1_Threshold ) { rtB . jwcmbwsc34 = rtB . abudmgdmqo ; } else { rtB .
jwcmbwsc34 = rtP . Constant5_Value_eyvmukmjzw ; } rtB . paxzwpdtyr = rtB .
jwcmbwsc34 ; rtB . cka3cbvjkk = rtB . paxzwpdtyr ; } else { rtB . mnmjtwzkhg
= rt_Lookup ( rtP . LookUpTable_XData_o0gg4yk3ay , 5 , rtB . n3cickajoe , rtP
. LookUpTable_YData_dujf1r2cza ) ; rtB . cka3cbvjkk = rtB . mnmjtwzkhg ; }
rtB . dvmo0qrznq = ssGetTaskTime ( rtS , 2 ) ; if ( rtP . C4_Value_ktcw3s24rj
>= rtP . Switch3_Threshold_c2ubqk250k ) { if ( rtP . Q2_SwitchC >= rtP .
Switch2_Threshold ) { rtB . iipz2h3t1h = rtB . abudmgdmqo ; } else { rtB .
iipz2h3t1h = rtP . Constant5_Value_eyvmukmjzw ; } rtB . jd3k1qt01g = rtB .
iipz2h3t1h ; rtB . i34c3sd5c4 = rtB . jd3k1qt01g ; } else { rtB . p2blqb2x40
= rt_Lookup ( rtP . LookUpTable_XData_pxfayshagn , 5 , rtB . dvmo0qrznq , rtP
. LookUpTable_YData_cvxjcdpbmq ) ; rtB . i34c3sd5c4 = rtB . p2blqb2x40 ; } }
UNUSED_PARAMETER ( tid ) ; } void MdlOutputsTID3 ( int_T tid ) { rtB .
jo0ewhen5p = rtP . N_Gain * rtP . Vf_Value ; rtB . kkthzbzxh4 = ( rtP .
KZTest_FaultC != 0.0 ) ; rtB . f05xiwzeii = ( rtP . KZTest_FaultB != 0.0 ) ;
rtB . hq4dyb3apy = ( rtP . KZTest_FaultA != 0.0 ) ; UNUSED_PARAMETER ( tid )
; } void MdlUpdate ( int_T tid ) { real_T HoldSine ; real_T HoldCosine ;
int32_T i ; if ( ssIsSampleHit ( rtS , 2 , 0 ) ) { rtDW . hsrejdggk0 += rtP .
Rotorangledthetae_gainval * rtB . anv20tflb2 ; for ( i = 0 ; i < 6 ; i ++ ) {
rtDW . g2cesgd0fj [ i ] = rtB . hjl4wiy52h [ i ] ; } if ( rtP .
Constant1_Value_iglewzfyey > 0.0 ) { rtDW . ezm5irxmvk = rtB . msc0q3ydkn ;
if ( rtP . Constant1_Value_afrrlaacip ) { rtDW . m2oxahajnn = rtB .
enoczvl5ij ; } } HoldSine = rtDW . jzuea21ukz ; HoldCosine = rtDW .
crh5ypw2db ; rtDW . jzuea21ukz = HoldSine * rtP . SineWaveA_HCos + HoldCosine
* rtP . SineWaveA_Hsin ; rtDW . crh5ypw2db = HoldCosine * rtP .
SineWaveA_HCos - HoldSine * rtP . SineWaveA_Hsin ; HoldSine = rtDW .
ia2zs2ii2b ; HoldCosine = rtDW . lormmfrasx ; rtDW . ia2zs2ii2b = HoldSine *
rtP . SineWaveB_HCos + HoldCosine * rtP . SineWaveB_Hsin ; rtDW . lormmfrasx
= HoldCosine * rtP . SineWaveB_HCos - HoldSine * rtP . SineWaveB_Hsin ;
HoldSine = rtDW . ocwfh3bc3h ; HoldCosine = rtDW . dw042k0azu ; rtDW .
ocwfh3bc3h = HoldSine * rtP . SineWaveC_HCos + HoldCosine * rtP .
SineWaveC_Hsin ; rtDW . dw042k0azu = HoldCosine * rtP . SineWaveC_HCos -
HoldSine * rtP . SineWaveC_Hsin ; { const real_T * As = ( real_T * ) rtDW .
oh3rj3rgch . AS ; const real_T * Bs = ( real_T * ) rtDW . oh3rj3rgch . BS ;
real_T * xtmp = ( real_T * ) rtDW . oh3rj3rgch . XTMP ; real_T accum ; {
int_T i1 ; real_T * xd = & rtDW . pwalsrbpvg [ 0 ] ; for ( i1 = 0 ; i1 < 6 ;
i1 ++ ) { accum = 0.0 ; { int_T i2 ; real_T * xd = & rtDW . pwalsrbpvg [ 0 ]
; for ( i2 = 0 ; i2 < 6 ; i2 ++ ) { accum += * ( As ++ ) * xd [ i2 ] ; } } {
int_T i2 ; const real_T * u0 = rtP . SwitchCurrents_Value ; for ( i2 = 0 ; i2
< 6 ; i2 ++ ) { accum += * ( Bs ++ ) * u0 [ i2 ] ; } accum += * ( Bs ++ ) *
rtB . ocwtlsstoa [ 0 ] ; accum += * ( Bs ++ ) * rtB . ocwtlsstoa [ 1 ] ;
accum += * ( Bs ++ ) * rtB . kc2wsb45fs ; accum += * ( Bs ++ ) * rtB .
l2zafpzrzu ; accum += * ( Bs ++ ) * rtB . ohedl35wf1 ; } xtmp [ i1 ] = accum
; } } { int_T i1 ; real_T * xd = & rtDW . pwalsrbpvg [ 0 ] ; for ( i1 = 0 ;
i1 < 6 ; i1 ++ ) { xd [ i1 ] = xtmp [ i1 ] ; } } { int_T * gState = ( int_T *
) rtDW . oh3rj3rgch . G_STATE ; * ( gState ++ ) = ( int_T ) rtB . i34c3sd5c4
; * ( gState ++ ) = ( int_T ) rtB . cka3cbvjkk ; * ( gState ++ ) = ( int_T )
rtB . mnz0k5z1ev ; * ( gState ++ ) = ( int_T ) rtB . i20vaspyrp ; * ( gState
++ ) = ( int_T ) rtB . oj12dzvfb4 ; * ( gState ++ ) = ( int_T ) rtB .
p4d344o4a4 ; } } } { real_T * * uBuffer = ( real_T * * ) & rtDW . b1o5sd1mu2
. TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & rtDW .
b1o5sd1mu2 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( rtS ) ; rtDW .
drr2cwynj5 . Head = ( ( rtDW . drr2cwynj5 . Head < ( rtDW . drr2cwynj5 .
CircularBufSize - 1 ) ) ? ( rtDW . drr2cwynj5 . Head + 1 ) : 0 ) ; if ( rtDW
. drr2cwynj5 . Head == rtDW . drr2cwynj5 . Tail ) { if ( !
rt_TDelayUpdateTailOrGrowBuf ( & rtDW . drr2cwynj5 . CircularBufSize , & rtDW
. drr2cwynj5 . Tail , & rtDW . drr2cwynj5 . Head , & rtDW . drr2cwynj5 . Last
, simTime - rtP . TransportDelay_Delay , tBuffer , uBuffer , ( NULL ) , (
boolean_T ) 0 , false , & rtDW . drr2cwynj5 . MaxNewBufSize ) ) {
ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } } (
* tBuffer ) [ rtDW . drr2cwynj5 . Head ] = simTime ; ( * uBuffer ) [ rtDW .
drr2cwynj5 . Head ] = rtB . cuumld5jhq ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . jfihwsipvc = rtB . e11hdbixe4 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . cvivv0qvki . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . cvivv0qvki . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . fymd3ellif . Head = ( ( rtDW . fymd3ellif . Head < (
rtDW . fymd3ellif . CircularBufSize - 1 ) ) ? ( rtDW . fymd3ellif . Head + 1
) : 0 ) ; if ( rtDW . fymd3ellif . Head == rtDW . fymd3ellif . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . fymd3ellif . CircularBufSize , &
rtDW . fymd3ellif . Tail , & rtDW . fymd3ellif . Head , & rtDW . fymd3ellif .
Last , simTime - rtP . TransportDelay_Delay_gkek32v2fo , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . fymd3ellif . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . fymd3ellif . Head ] = simTime ; ( * uBuffer ) [ rtDW .
fymd3ellif . Head ] = rtB . f3bj13s54m ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . k2r45klkvr = rtB . dv5odcc5un ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . ecv4fwadro . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . ecv4fwadro . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . eyb4fhkbdy . Head = ( ( rtDW . eyb4fhkbdy . Head < (
rtDW . eyb4fhkbdy . CircularBufSize - 1 ) ) ? ( rtDW . eyb4fhkbdy . Head + 1
) : 0 ) ; if ( rtDW . eyb4fhkbdy . Head == rtDW . eyb4fhkbdy . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . eyb4fhkbdy . CircularBufSize , &
rtDW . eyb4fhkbdy . Tail , & rtDW . eyb4fhkbdy . Head , & rtDW . eyb4fhkbdy .
Last , simTime - rtP . TransportDelay_Delay_mvxoy0gg0s , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . eyb4fhkbdy . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . eyb4fhkbdy . Head ] = simTime ; ( * uBuffer ) [ rtDW .
eyb4fhkbdy . Head ] = rtB . a2aelbuuxd ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . lawcljffga = rtB . o1nbxtrkf4 ; } { real_T * * uBuffer = ( real_T
* * ) & rtDW . ohddfojdjk . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & rtDW . ohddfojdjk . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( rtS ) ; rtDW . ek4mryl130 . Head = ( ( rtDW . ek4mryl130 . Head < (
rtDW . ek4mryl130 . CircularBufSize - 1 ) ) ? ( rtDW . ek4mryl130 . Head + 1
) : 0 ) ; if ( rtDW . ek4mryl130 . Head == rtDW . ek4mryl130 . Tail ) { if (
! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . ek4mryl130 . CircularBufSize , &
rtDW . ek4mryl130 . Tail , & rtDW . ek4mryl130 . Head , & rtDW . ek4mryl130 .
Last , simTime - rtP . TransportDelay_Delay_bifxyqg4be , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & rtDW . ek4mryl130 . MaxNewBufSize ) )
{ ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } }
( * tBuffer ) [ rtDW . ek4mryl130 . Head ] = simTime ; ( * uBuffer ) [ rtDW .
ek4mryl130 . Head ] = rtB . jpdedqtzey ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . bhgocl4yyi = rtB . caiqrgrb4h ; } if ( ssIsSampleHit ( rtS , 2 , 0
) ) { rtDW . ltyzpyes2j = rtB . dn1jsu5b3a ; rtDW . koephaho24 = rtB .
kt4yqwu22g ; rtDW . bjkyi30h4y [ 0 ] = rtB . g2rmjbbycl ; rtDW . bjkyi30h4y [
1 ] = rtB . ljwiyate1t ; rtDW . bjkyi30h4y [ 2 ] = rtB . jo0ewhen5p ; rtDW .
bjkyi30h4y [ 3 ] = rtP . Vkd0Vkq10Vkq20_Value [ 0 ] ; rtDW . bjkyi30h4y [ 4 ]
= rtP . Vkd0Vkq10Vkq20_Value [ 1 ] ; rtDW . bjkyi30h4y [ 5 ] = rtP .
Vkd0Vkq10Vkq20_Value [ 2 ] ; rtDW . cszisyhd2i += rtP . theta_gainval * rtB .
eajcmlcqmc ; rtDW . lkdnaytcjn = 0U ; rtDW . ksbtbqie2m = rtP .
Rotorspeeddeviationdw_gainval * rtB . fmoyvyrdin + rtB . dn1jsu5b3a ; }
UNUSED_PARAMETER ( tid ) ; } void MdlUpdateTID3 ( int_T tid ) {
UNUSED_PARAMETER ( tid ) ; } void MdlDerivatives ( void ) { XDot * _rtXdot ;
_rtXdot = ( ( XDot * ) ssGetdX ( rtS ) ) ; _rtXdot -> lpzjbjknwp = rtB .
epw4e2nyr2 ; _rtXdot -> oi5s2zktut = rtB . hvccs5ksdj ; _rtXdot -> pz2xsnrzjc
= rtB . lxuyvf0kct ; _rtXdot -> hfv4dc3cpb = rtB . hdfe5utvyn ; } void
MdlProjection ( void ) { } void MdlTerminate ( void ) { { free ( rtDW .
oh3rj3rgch . AS ) ; free ( rtDW . oh3rj3rgch . BS ) ; free ( rtDW .
oh3rj3rgch . CS ) ; free ( rtDW . oh3rj3rgch . DS ) ; free ( rtDW .
oh3rj3rgch . DX_COL ) ; free ( rtDW . oh3rj3rgch . TMP2 ) ; free ( rtDW .
oh3rj3rgch . BD_COL ) ; free ( rtDW . oh3rj3rgch . TMP1 ) ; free ( rtDW .
oh3rj3rgch . XTMP ) ; free ( rtDW . oh3rj3rgch . G_STATE ) ; free ( rtDW .
oh3rj3rgch . SWITCH_STATUS ) ; free ( rtDW . oh3rj3rgch . SW_CHG ) ; free (
rtDW . oh3rj3rgch . SWITCH_STATUS_INIT ) ; } rt_TDelayFreeBuf ( rtDW .
b1o5sd1mu2 . TUbufferPtrs [ 0 ] ) ; rt_TDelayFreeBuf ( rtDW . cvivv0qvki .
TUbufferPtrs [ 0 ] ) ; rt_TDelayFreeBuf ( rtDW . ecv4fwadro . TUbufferPtrs [
0 ] ) ; rt_TDelayFreeBuf ( rtDW . ohddfojdjk . TUbufferPtrs [ 0 ] ) ; if (
rt_slioCatalogue ( ) != ( NULL ) ) { void * * slioCatalogueAddr =
rt_slioCatalogueAddr ( ) ; rtwSaveDatasetsToMatFile (
rtwGetPointerFromUniquePtr ( rt_slioCatalogue ( ) ) ,
rt_GetMatSigstreamLoggingFileName ( ) ) ; rtwTerminateSlioCatalogue (
slioCatalogueAddr ) ; * slioCatalogueAddr = NULL ; } } void
MdlInitializeSizes ( void ) { ssSetNumContStates ( rtS , 4 ) ;
ssSetNumPeriodicContStates ( rtS , 0 ) ; ssSetNumY ( rtS , 0 ) ; ssSetNumU (
rtS , 0 ) ; ssSetDirectFeedThrough ( rtS , 0 ) ; ssSetNumSampleTimes ( rtS ,
3 ) ; ssSetNumBlocks ( rtS , 686 ) ; ssSetNumBlockIO ( rtS , 281 ) ;
ssSetNumBlockParams ( rtS , 1611 ) ; } void MdlInitializeSampleTimes ( void )
{ ssSetSampleTime ( rtS , 0 , 0.0 ) ; ssSetSampleTime ( rtS , 1 , 1.0E-5 ) ;
ssSetSampleTime ( rtS , 2 , 5.0E-5 ) ; ssSetOffsetTime ( rtS , 0 , 0.0 ) ;
ssSetOffsetTime ( rtS , 1 , 0.0 ) ; ssSetOffsetTime ( rtS , 2 , 0.0 ) ; }
void raccel_set_checksum ( ) { ssSetChecksumVal ( rtS , 0 , 749349454U ) ;
ssSetChecksumVal ( rtS , 1 , 2771954547U ) ; ssSetChecksumVal ( rtS , 2 ,
2147498699U ) ; ssSetChecksumVal ( rtS , 3 , 4272243878U ) ; }
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
, & dtInfo ) ; dtInfo . numDataTypes = 19 ; dtInfo . dataTypeSizes = &
rtDataTypeSizes [ 0 ] ; dtInfo . dataTypeNames = & rtDataTypeNames [ 0 ] ;
dtInfo . BTransTable = & rtBTransTable ; dtInfo . PTransTable = &
rtPTransTable ; dtInfo . dataTypeInfoTable = rtDataTypeInfoTable ; }
DZG_v6_SimGen_InitializeDataMapInfo ( ) ; ssSetIsRapidAcceleratorActive ( rtS
, true ) ; ssSetRootSS ( rtS , rtS ) ; ssSetVersion ( rtS ,
SIMSTRUCT_VERSION_LEVEL2 ) ; ssSetModelName ( rtS , "DZG_v6_SimGen" ) ;
ssSetPath ( rtS , "DZG_v6_SimGen" ) ; ssSetTStart ( rtS , 0.0 ) ; ssSetTFinal
( rtS , 60.000000000000007 ) ; ssSetStepSize ( rtS , 1.0E-5 ) ;
ssSetFixedStepSize ( rtS , 1.0E-5 ) ; { static RTWLogInfo rt_DataLoggingInfo
; rt_DataLoggingInfo . loggingInterval = NULL ; ssSetRTWLogInfo ( rtS , &
rt_DataLoggingInfo ) ; } { { static int_T rt_LoggedStateWidths [ ] = { 1 , 1
, 1 , 1 , 1 , 6 , 6 , 1 , 1 , 6 , 1 , 1 , 1 , 1 } ; static int_T
rt_LoggedStateNumDimensions [ ] = { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
, 1 , 1 , 1 } ; static int_T rt_LoggedStateDimensions [ ] = { 1 , 1 , 1 , 1 ,
1 , 6 , 6 , 1 , 1 , 6 , 1 , 1 , 1 , 1 } ; static boolean_T
rt_LoggedStateIsVarDims [ ] = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
, 0 , 0 } ; static BuiltInDTypeId rt_LoggedStateDataTypeIds [ ] = { SS_DOUBLE
, SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE } ; static int_T rt_LoggedStateComplexSignals [ ] = { 0 , 0 , 0 , 0
, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ; static RTWPreprocessingFcnPtr
rt_LoggingStatePreprocessingFcnPtrs [ ] = { ( NULL ) , ( NULL ) , ( NULL ) ,
( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) ,
( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) } ; static const char_T *
rt_LoggedStateLabels [ ] = { "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" ,
"DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" ,
"DSTATE" , "DSTATE" , "DSTATE" } ; static const char_T *
rt_LoggedStateBlockNames [ ] = {
"DZG_v6_SimGen/System/Calc/Fourier/Mean value1/Model/integrator" ,
"DZG_v6_SimGen/System/Calc/Fourier/Mean/Model/integrator" ,
"DZG_v6_SimGen/System/Calc/Fourier1/Mean value1/Model/integrator" ,
"DZG_v6_SimGen/System/Calc/Fourier1/Mean/Model/integrator" ,
 "DZG_v6_SimGen/System/Synchronous Machine\npu Standard/Mechanical model/Rotor angle\ndthetae"
,
 "DZG_v6_SimGen/System/Synchronous Machine\npu Standard/Electrical model/Synchronous Machine\nDiscrete Model/phi/fluxes"
, "DZG_v6_SimGen/powergui/EquivalentModel1/State-Space" ,
 "DZG_v6_SimGen/System/Synchronous Machine\npu Standard/Mechanical model/Delay\nPrediction/dw_delay"
,
 "DZG_v6_SimGen/System/Synchronous Machine\npu Standard/Mechanical model/Delay\nPrediction/dw_predict"
,
 "DZG_v6_SimGen/System/Synchronous Machine\npu Standard/Electrical model/Synchronous Machine\nDiscrete Model/phi/voltages"
,
"DZG_v6_SimGen/System/Synchronous Machine\npu Standard/Mechanical model/theta"
,
 "DZG_v6_SimGen/System/Synchronous Machine\npu Standard/Mechanical model/Rotor speed\ndeviation (dw)"
,
 "DZG_v6_SimGen/System/Synchronous Machine\npu Standard/Electrical model/Synchronous Machine\nDiscrete Model/Saturation/Lmd_sat/Lmd_sat"
,
 "DZG_v6_SimGen/System/Synchronous Machine\npu Standard/Electrical model/Synchronous Machine\nDiscrete Model/Saturation/Lmq_sat/Lmq_sat"
} ; static const char_T * rt_LoggedStateNames [ ] = { "" , "" , "" , "" ,
"DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" ,
"DSTATE" , "DSTATE" , "DSTATE" } ; static boolean_T rt_LoggedStateCrossMdlRef
[ ] = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ; static
RTWLogDataTypeConvert rt_RTWLogDataTypeConvert [ ] = { { 0 , SS_DOUBLE ,
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
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } } ; static
RTWLogSignalInfo rt_LoggedStateSignalInfo = { 14 , rt_LoggedStateWidths ,
rt_LoggedStateNumDimensions , rt_LoggedStateDimensions ,
rt_LoggedStateIsVarDims , ( NULL ) , ( NULL ) , rt_LoggedStateDataTypeIds ,
rt_LoggedStateComplexSignals , ( NULL ) , rt_LoggingStatePreprocessingFcnPtrs
, { rt_LoggedStateLabels } , ( NULL ) , ( NULL ) , ( NULL ) , {
rt_LoggedStateBlockNames } , { rt_LoggedStateNames } ,
rt_LoggedStateCrossMdlRef , rt_RTWLogDataTypeConvert } ; static void *
rt_LoggedStateSignalPtrs [ 14 ] ; rtliSetLogXSignalPtrs ( ssGetRTWLogInfo (
rtS ) , ( LogSignalPtrsType ) rt_LoggedStateSignalPtrs ) ;
rtliSetLogXSignalInfo ( ssGetRTWLogInfo ( rtS ) , & rt_LoggedStateSignalInfo
) ; rt_LoggedStateSignalPtrs [ 0 ] = ( void * ) & rtX . lpzjbjknwp ;
rt_LoggedStateSignalPtrs [ 1 ] = ( void * ) & rtX . oi5s2zktut ;
rt_LoggedStateSignalPtrs [ 2 ] = ( void * ) & rtX . pz2xsnrzjc ;
rt_LoggedStateSignalPtrs [ 3 ] = ( void * ) & rtX . hfv4dc3cpb ;
rt_LoggedStateSignalPtrs [ 4 ] = ( void * ) & rtDW . hsrejdggk0 ;
rt_LoggedStateSignalPtrs [ 5 ] = ( void * ) rtDW . g2cesgd0fj ;
rt_LoggedStateSignalPtrs [ 6 ] = ( void * ) rtDW . pwalsrbpvg ;
rt_LoggedStateSignalPtrs [ 7 ] = ( void * ) & rtDW . ltyzpyes2j ;
rt_LoggedStateSignalPtrs [ 8 ] = ( void * ) & rtDW . koephaho24 ;
rt_LoggedStateSignalPtrs [ 9 ] = ( void * ) rtDW . bjkyi30h4y ;
rt_LoggedStateSignalPtrs [ 10 ] = ( void * ) & rtDW . cszisyhd2i ;
rt_LoggedStateSignalPtrs [ 11 ] = ( void * ) & rtDW . ksbtbqie2m ;
rt_LoggedStateSignalPtrs [ 12 ] = ( void * ) & rtDW . ezm5irxmvk ;
rt_LoggedStateSignalPtrs [ 13 ] = ( void * ) & rtDW . m2oxahajnn ; }
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
static ssSolverInfo slvrInfo ; static boolean_T contStatesDisabled [ 4 ] ;
ssSetSolverInfo ( rtS , & slvrInfo ) ; ssSetSolverName ( rtS , "ode3" ) ;
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
0 ) ; ssSetContStateDisabled ( rtS , contStatesDisabled ) ; }
ssSetChecksumVal ( rtS , 0 , 749349454U ) ; ssSetChecksumVal ( rtS , 1 ,
2771954547U ) ; ssSetChecksumVal ( rtS , 2 , 2147498699U ) ; ssSetChecksumVal
( rtS , 3 , 4272243878U ) ; { static const sysRanDType rtAlwaysEnabled =
SUBSYS_RAN_BC_ENABLE ; static RTWExtModeInfo rt_ExtModeInfo ; static const
sysRanDType * systemRan [ 22 ] ; gblRTWExtModeInfo = & rt_ExtModeInfo ;
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
systemRan [ 16 ] = & rtAlwaysEnabled ; systemRan [ 17 ] = ( sysRanDType * ) &
rtDW . eocn40ibhc ; systemRan [ 18 ] = ( sysRanDType * ) & rtDW . jld3u4fkos
; systemRan [ 19 ] = ( sysRanDType * ) & rtDW . jld3u4fkos ; systemRan [ 20 ]
= ( sysRanDType * ) & rtDW . eocn40ibhc ; systemRan [ 21 ] = &
rtAlwaysEnabled ; rteiSetModelMappingInfoPtr ( ssGetRTWExtModeInfo ( rtS ) ,
& ssGetModelMappingInfo ( rtS ) ) ; rteiSetChecksumsPtr ( ssGetRTWExtModeInfo
( rtS ) , ssGetChecksums ( rtS ) ) ; rteiSetTPtr ( ssGetRTWExtModeInfo ( rtS
) , ssGetTPtr ( rtS ) ) ; } return rtS ; }
#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif
const int_T gblParameterTuningTid = 3 ; void MdlOutputsParameterSampleTime (
int_T tid ) { MdlOutputsTID3 ( tid ) ; }
