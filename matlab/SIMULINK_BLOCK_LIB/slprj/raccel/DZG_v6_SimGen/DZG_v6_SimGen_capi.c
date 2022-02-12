#include "rtw_capi.h"
#ifdef HOST_CAPI_BUILD
#include "DZG_v6_SimGen_capi_host.h"
#define sizeof(s) ((size_t)(0xFFFF))
#undef rt_offsetof
#define rt_offsetof(s,el) ((uint16_T)(0xFFFF))
#define TARGET_CONST
#define TARGET_STRING(s) (s)    
#else
#include "builtin_typeid_types.h"
#include "DZG_v6_SimGen.h"
#include "DZG_v6_SimGen_capi.h"
#include "DZG_v6_SimGen_private.h"
#ifdef LIGHT_WEIGHT_CAPI
#define TARGET_CONST                  
#define TARGET_STRING(s)               (NULL)                    
#else
#define TARGET_CONST                   const
#define TARGET_STRING(s)               (s)
#endif
#endif
static const rtwCAPI_Signals rtBlockSignals [ ] = { { 0 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/4" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 1
, 0 , TARGET_STRING ( "DZG_v6_SimGen/System/7" ) , TARGET_STRING ( "" ) , 0 ,
0 , 0 , 0 , 0 } , { 2 , 0 , TARGET_STRING ( "DZG_v6_SimGen/System/Gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 3 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Gain1" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } ,
{ 4 , 0 , TARGET_STRING ( "DZG_v6_SimGen/System/Gain3" ) , TARGET_STRING ( ""
) , 0 , 0 , 0 , 0 , 1 } , { 5 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Gain" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0
} , { 6 , 0 , TARGET_STRING ( "DZG_v6_SimGen/System/Calc/Gain1" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 7 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Gain4" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 ,
0 } , { 8 , 0 , TARGET_STRING ( "DZG_v6_SimGen/System/Calc/3" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 9 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/4" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 }
, { 10 , 0 , TARGET_STRING ( "DZG_v6_SimGen/System/Calc/Add8" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 11 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/1" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 }
, { 12 , 0 , TARGET_STRING ( "DZG_v6_SimGen/System/Calc/2" ) , TARGET_STRING
( "" ) , 0 , 0 , 0 , 0 , 0 } , { 13 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Data Type  Conversion" ) , TARGET_STRING ( "" )
, 0 , 1 , 0 , 0 , 1 } , { 14 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Data Type  Conversion1" ) , TARGET_STRING ( ""
) , 0 , 1 , 0 , 0 , 2 } , { 15 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Data Type  Conversion2" ) , TARGET_STRING ( ""
) , 0 , 1 , 0 , 0 , 2 } , { 16 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Data Type  Conversion3" ) , TARGET_STRING ( ""
) , 0 , 1 , 0 , 0 , 2 } , { 17 , 6 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Data Type  Conversion4" ) , TARGET_STRING ( ""
) , 0 , 0 , 0 , 0 , 1 } , { 18 , 8 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Data Type  Conversion5" ) , TARGET_STRING ( ""
) , 0 , 0 , 0 , 0 , 1 } , { 19 , 10 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Data Type  Conversion6" ) , TARGET_STRING ( ""
) , 0 , 0 , 0 , 0 , 1 } , { 20 , 6 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Logical Operator1" ) , TARGET_STRING ( "" ) , 0
, 1 , 0 , 0 , 1 } , { 21 , 8 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Logical Operator2" ) , TARGET_STRING ( "" ) , 0
, 1 , 0 , 0 , 1 } , { 22 , 10 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Logical Operator3" ) , TARGET_STRING ( "" ) , 0
, 1 , 0 , 0 , 1 } , { 23 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Switch3" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 ,
0 , 1 } , { 24 , 0 , TARGET_STRING ( "DZG_v6_SimGen/System/MeasureGen_G2/Kv"
) , TARGET_STRING ( "" ) , 0 , 0 , 1 , 0 , 1 } , { 25 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureGen_G2/Kv1" ) , TARGET_STRING ( "" ) , 0 , 0 , 1
, 0 , 1 } , { 26 , 0 , TARGET_STRING ( "DZG_v6_SimGen/System/MeasureLine1/Kv"
) , TARGET_STRING ( "" ) , 0 , 0 , 1 , 0 , 1 } , { 27 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine1/Kv1" ) , TARGET_STRING ( "" ) , 0 , 0 , 1
, 0 , 1 } , { 28 , 0 , TARGET_STRING ( "DZG_v6_SimGen/System/MeasureLine2/Kv"
) , TARGET_STRING ( "" ) , 0 , 0 , 1 , 0 , 1 } , { 29 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine2/Kv1" ) , TARGET_STRING ( "" ) , 0 , 0 , 1
, 0 , 1 } , { 30 , 0 , TARGET_STRING ( "DZG_v6_SimGen/System/MeasureLoad/Kv"
) , TARGET_STRING ( "" ) , 0 , 0 , 1 , 0 , 1 } , { 31 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLoad/Kv1" ) , TARGET_STRING ( "" ) , 0 , 0 , 1 ,
0 , 1 } , { 32 , 0 , TARGET_STRING ( "DZG_v6_SimGen/System/MeasureSystem1/Kv"
) , TARGET_STRING ( "" ) , 0 , 0 , 1 , 0 , 1 } , { 33 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureSystem1/Kv1" ) , TARGET_STRING ( "" ) , 0 , 0 ,
1 , 0 , 1 } , { 34 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_G1/Kv" ) , TARGET_STRING ( "" ) , 0 , 0 , 1 ,
0 , 1 } , { 35 , 0 , TARGET_STRING ( "DZG_v6_SimGen/System/MeasureTr_G1/Kv1"
) , TARGET_STRING ( "" ) , 0 , 0 , 1 , 0 , 1 } , { 36 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_H/Kv" ) , TARGET_STRING ( "" ) , 0 , 0 , 1 ,
0 , 1 } , { 37 , 0 , TARGET_STRING ( "DZG_v6_SimGen/System/MeasureTr_H/Kv1" )
, TARGET_STRING ( "" ) , 0 , 0 , 1 , 0 , 1 } , { 38 , 12 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Switch" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1
} , { 39 , 14 , TARGET_STRING ( "DZG_v6_SimGen/System/Q2/Switch1" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 40 , 16 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Switch2" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 ,
1 } , { 41 , 0 , TARGET_STRING ( "DZG_v6_SimGen/System/Q2/Switch3" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 42 , 0 , TARGET_STRING (
"DZG_v6_SimGen/powergui/EquivalentModel1/State-Space" ) , TARGET_STRING ( ""
) , 0 , 0 , 2 , 0 , 1 } , { 43 , 0 , TARGET_STRING (
"DZG_v6_SimGen/powergui/EquivalentModel1/State-Space" ) , TARGET_STRING ( ""
) , 1 , 0 , 3 , 0 , 1 } , { 44 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Complex to Magnitude-Angle" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 45 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Complex to Magnitude-Angle" ) ,
TARGET_STRING ( "" ) , 1 , 0 , 0 , 0 , 0 } , { 46 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Rad->Deg." ) , TARGET_STRING ( "" ) , 0 ,
0 , 0 , 0 , 0 } , { 47 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Product" ) , TARGET_STRING ( "" ) , 0 , 0
, 0 , 0 , 0 } , { 48 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Product1" ) , TARGET_STRING ( "" ) , 0 , 0
, 0 , 0 , 0 } , { 49 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Real-Imag to Complex" ) , TARGET_STRING (
"" ) , 0 , 2 , 0 , 0 , 0 } , { 50 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/cos(wt)" ) , TARGET_STRING ( "" ) , 0 , 0
, 0 , 0 , 0 } , { 51 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/sin(wt)" ) , TARGET_STRING ( "" ) , 0 , 0
, 0 , 0 , 0 } , { 52 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Complex to Magnitude-Angle" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 53 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Complex to Magnitude-Angle" ) ,
TARGET_STRING ( "" ) , 1 , 0 , 0 , 0 , 0 } , { 54 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Rad->Deg." ) , TARGET_STRING ( "" ) , 0 ,
0 , 0 , 0 , 0 } , { 55 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Product" ) , TARGET_STRING ( "" ) , 0 , 0
, 0 , 0 , 0 } , { 56 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Product1" ) , TARGET_STRING ( "" ) , 0 ,
0 , 0 , 0 , 0 } , { 57 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Real-Imag to Complex" ) , TARGET_STRING (
"" ) , 0 , 2 , 0 , 0 , 0 } , { 58 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/cos(wt)" ) , TARGET_STRING ( "" ) , 0 , 0
, 0 , 0 , 0 } , { 59 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/sin(wt)" ) , TARGET_STRING ( "" ) , 0 , 0
, 0 , 0 , 0 } , { 60 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/N" ) ,
TARGET_STRING ( "Vfd" ) , 0 , 0 , 0 , 0 , 2 } , { 61 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Rate Transition1"
) , TARGET_STRING ( "" ) , 0 , 0 , 4 , 0 , 1 } , { 62 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Rate Transition2"
) , TARGET_STRING ( "" ) , 0 , 0 , 4 , 0 , 1 } , { 63 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Rate Transition3"
) , TARGET_STRING ( "" ) , 0 , 0 , 5 , 0 , 1 } , { 64 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Digital Clock"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 65 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Rotor angle dthetae"
) , TARGET_STRING ( "d_theta (rad)" ) , 0 , 0 , 0 , 0 , 1 } , { 66 , 0 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Rotor speed deviation (dw)"
) , TARGET_STRING ( "dw" ) , 0 , 0 , 0 , 0 , 1 } , { 67 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/theta"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 68 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Fcn" )
, TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 69 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/div" )
, TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 70 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/1 ----- 2H"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 71 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/1_Pb"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 72 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/F" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 73 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/t" ) ,
TARGET_STRING ( "theta" ) , 0 , 0 , 0 , 0 , 1 } , { 74 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/units"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 75 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/units1"
) , TARGET_STRING ( "Pe" ) , 0 , 0 , 0 , 0 , 1 } , { 76 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/units2"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 77 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/we base"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 78 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/web2"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 79 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/web3"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 80 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Product"
) , TARGET_STRING ( "Pe" ) , 0 , 0 , 0 , 0 , 1 } , { 81 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Rate Transition1"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 82 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Rate Transition2"
) , TARGET_STRING ( "" ) , 0 , 0 , 3 , 0 , 1 } , { 83 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Sum" )
, TARGET_STRING ( "n" ) , 0 , 0 , 0 , 0 , 1 } , { 84 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Sum2"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 85 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Sum3"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 86 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave A" ) , TARGET_STRING ( "" ) , 0
, 0 , 0 , 0 , 1 } , { 87 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave B" ) , TARGET_STRING ( "" ) , 0
, 0 , 0 , 0 , 1 } , { 88 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave C" ) , TARGET_STRING ( "" ) , 0
, 0 , 0 , 0 , 1 } , { 89 , 6 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault A/Model/Data Type  Conversion" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 90 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault A/Model/Switch3" ) , TARGET_STRING ( "" )
, 0 , 0 , 0 , 0 , 1 } , { 91 , 8 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault B/Model/Data Type  Conversion" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 92 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault B/Model/Switch3" ) , TARGET_STRING ( "" )
, 0 , 0 , 0 , 0 , 1 } , { 93 , 10 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault C/Model/Data Type  Conversion" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 94 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault C/Model/Switch3" ) , TARGET_STRING ( "" )
, 0 , 0 , 0 , 0 , 1 } , { 95 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Stair Generator/Model/Digital Clock" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 96 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Stair Generator/Model/Look-Up Table" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 97 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureGen_G2/Model/I A:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 98 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureGen_G2/Model/I B:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 99 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureGen_G2/Model/I C:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 100 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureGen_G2/Model/U A:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 101 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureGen_G2/Model/U B:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 102 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureGen_G2/Model/U C:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 103 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine1/Model/I A:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 104 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine1/Model/I B:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 105 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine1/Model/I C:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 106 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine1/Model/U A:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 107 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine1/Model/U B:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 108 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine1/Model/U C:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 109 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine2/Model/I A:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 110 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine2/Model/I B:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 111 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine2/Model/I C:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 112 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine2/Model/U A:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 113 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine2/Model/U B:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 114 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine2/Model/U C:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 115 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLoad/Model/I A:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 116 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLoad/Model/I B:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 117 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLoad/Model/I C:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 118 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLoad/Model/U A:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 119 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLoad/Model/U B:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 120 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLoad/Model/U C:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 121 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureSystem1/Model/I A:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 122 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureSystem1/Model/I B:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 123 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureSystem1/Model/I C:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 124 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureSystem1/Model/U A:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 125 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureSystem1/Model/U B:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 126 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureSystem1/Model/U C:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 127 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_G1/Model/I A:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 128 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_G1/Model/I B:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 129 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_G1/Model/I C:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 130 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_G1/Model/U A:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 131 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_G1/Model/U B:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 132 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_G1/Model/U C:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 133 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_H/Model/I A:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 134 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_H/Model/I B:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 135 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_H/Model/I C:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 136 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_H/Model/U A:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 137 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_H/Model/U B:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 138 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_H/Model/U C:/do not delete this gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 139 , 12 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker A/Model/Data Type  Conversion" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 140 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker A/Model/Switch3" ) , TARGET_STRING ( "" ) ,
0 , 0 , 0 , 0 , 1 } , { 141 , 14 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker B/Model/Data Type  Conversion" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 142 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker B/Model/Switch3" ) , TARGET_STRING ( "" ) ,
0 , 0 , 0 , 0 , 1 } , { 143 , 16 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker C/Model/Data Type  Conversion" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 144 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker C/Model/Switch3" ) , TARGET_STRING ( "" ) ,
0 , 0 , 0 , 0 , 1 } , { 145 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Stair Generator/Model/Digital Clock" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 146 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Stair Generator/Model/Look-Up Table" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 147 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/output formatting"
) , TARGET_STRING ( "" ) , 0 , 0 , 5 , 0 , 1 } , { 148 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/Switch"
) , TARGET_STRING ( "ikq2" ) , 0 , 0 , 0 , 0 , 1 } , { 149 , 20 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation"
) , TARGET_STRING ( "" ) , 0 , 0 , 6 , 0 , 1 } , { 150 , 20 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation"
) , TARGET_STRING ( "" ) , 1 , 0 , 6 , 0 , 1 } , { 151 , 20 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation"
) , TARGET_STRING ( "" ) , 2 , 0 , 0 , 0 , 1 } , { 152 , 20 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation"
) , TARGET_STRING ( "Lmsatd" ) , 2 , 0 , 0 , 0 , 1 } , { 153 , 20 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation"
) , TARGET_STRING ( "" ) , 3 , 0 , 0 , 0 , 1 } , { 154 , 20 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation"
) , TARGET_STRING ( "" ) , 3 , 0 , 0 , 0 , 1 } , { 155 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/change Iq Id  current signs"
) , TARGET_STRING ( "" ) , 0 , 0 , 3 , 0 , 1 } , { 156 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Product3"
) , TARGET_STRING ( "I(k)" ) , 0 , 0 , 3 , 0 , 1 } , { 157 , 21 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Sum1"
) , TARGET_STRING ( "A" ) , 0 , 0 , 6 , 0 , 1 } , { 158 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Switch"
) , TARGET_STRING ( "Linv" ) , 0 , 0 , 6 , 0 , 1 } , { 159 , 21 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Switch1"
) , TARGET_STRING ( "" ) , 0 , 0 , 6 , 0 , 1 } , { 160 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Switch2"
) , TARGET_STRING ( "" ) , 0 , 0 , 4 , 0 , 1 } , { 161 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Switch3"
) , TARGET_STRING ( "" ) , 0 , 0 , 4 , 0 , 1 } , { 162 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/abc to qd transformation/Fcn2"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 163 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/abc to qd transformation/Fcn3"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 164 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/abc to qd transformation/1_Vb"
) , TARGET_STRING ( "" ) , 0 , 0 , 4 , 0 , 1 } , { 165 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/abc to qd transformation/Elementary Math"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 166 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/abc to qd transformation/Elementary Math"
) , TARGET_STRING ( "" ) , 1 , 0 , 0 , 0 , 1 } , { 167 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/qd to abc transformation/Fcn"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 168 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/qd to abc transformation/Fcn1"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 169 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/qd to abc transformation/ib"
) , TARGET_STRING ( "" ) , 0 , 0 , 1 , 0 , 1 } , { 170 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/qd to abc transformation/Sum"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 171 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Delay Prediction/F2"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 172 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Delay Prediction/Sum1"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 173 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Delay Prediction/dw_delay"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 174 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Delay Prediction/dw_predict"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 175 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean/Model/Clock" ) , TARGET_STRING ( "" )
, 0 , 0 , 0 , 0 , 0 } , { 176 , 1 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean/Model/Gain" ) , TARGET_STRING ( "" )
, 0 , 0 , 0 , 0 , 0 } , { 177 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean/Model/integrator" ) , TARGET_STRING (
"" ) , 0 , 0 , 0 , 0 , 0 } , { 178 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean/Model/Memory" ) , TARGET_STRING ( ""
) , 0 , 0 , 0 , 0 , 3 } , { 179 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean/Model/Relational Operator" ) ,
TARGET_STRING ( "" ) , 0 , 1 , 0 , 0 , 0 } , { 180 , 1 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean/Model/Sum" ) , TARGET_STRING ( "" ) ,
0 , 0 , 0 , 0 , 0 } , { 181 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean/Model/Switch" ) , TARGET_STRING ( ""
) , 0 , 0 , 0 , 0 , 0 } , { 182 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean/Model/Transport Delay" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 183 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean value1/Model/Clock" ) , TARGET_STRING
( "" ) , 0 , 0 , 0 , 0 , 0 } , { 184 , 2 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean value1/Model/Gain" ) , TARGET_STRING
( "" ) , 0 , 0 , 0 , 0 , 0 } , { 185 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean value1/Model/integrator" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 186 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean value1/Model/Memory" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 3 } , { 187 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean value1/Model/Relational Operator" ) ,
TARGET_STRING ( "" ) , 0 , 1 , 0 , 0 , 0 } , { 188 , 2 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean value1/Model/Sum" ) , TARGET_STRING (
"" ) , 0 , 0 , 0 , 0 , 0 } , { 189 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean value1/Model/Switch" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 190 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean value1/Model/Transport Delay" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 191 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean/Model/Clock" ) , TARGET_STRING ( ""
) , 0 , 0 , 0 , 0 , 0 } , { 192 , 3 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean/Model/Gain" ) , TARGET_STRING ( "" )
, 0 , 0 , 0 , 0 , 0 } , { 193 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean/Model/integrator" ) , TARGET_STRING
( "" ) , 0 , 0 , 0 , 0 , 0 } , { 194 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean/Model/Memory" ) , TARGET_STRING ( ""
) , 0 , 0 , 0 , 0 , 3 } , { 195 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean/Model/Relational Operator" ) ,
TARGET_STRING ( "" ) , 0 , 1 , 0 , 0 , 0 } , { 196 , 3 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean/Model/Sum" ) , TARGET_STRING ( "" )
, 0 , 0 , 0 , 0 , 0 } , { 197 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean/Model/Switch" ) , TARGET_STRING ( ""
) , 0 , 0 , 0 , 0 , 0 } , { 198 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean/Model/Transport Delay" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 199 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean value1/Model/Clock" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 200 , 4 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean value1/Model/Gain" ) , TARGET_STRING
( "" ) , 0 , 0 , 0 , 0 , 0 } , { 201 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean value1/Model/integrator" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 202 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean value1/Model/Memory" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 3 } , { 203 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean value1/Model/Relational Operator" )
, TARGET_STRING ( "" ) , 0 , 1 , 0 , 0 , 0 } , { 204 , 4 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean value1/Model/Sum" ) , TARGET_STRING
( "" ) , 0 , 0 , 0 , 0 , 0 } , { 205 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean value1/Model/Switch" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 206 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean value1/Model/Transport Delay" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 207 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault A/Model/Timer/Digital Clock" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 208 , 5 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault A/Model/Timer/Look-Up Table" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 209 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault B/Model/Timer/Digital Clock" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 210 , 7 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault B/Model/Timer/Look-Up Table" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 211 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault C/Model/Timer/Digital Clock" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 212 , 9 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault C/Model/Timer/Look-Up Table" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 213 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker A/Model/Timer/Digital Clock" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 214 , 11 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker A/Model/Timer/Look-Up Table" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 215 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker B/Model/Timer/Digital Clock" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 216 , 13 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker B/Model/Timer/Look-Up Table" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 217 , 0 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker C/Model/Timer/Digital Clock" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 218 , 15 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker C/Model/Timer/Look-Up Table" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 219 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/PQ/Gain"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 220 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/PQ/Gain1"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 221 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/PQ/Product2"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 222 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/PQ/Product3"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 223 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/PQ/Product4"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 224 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/PQ/Product5"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 225 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/PQ/Sum"
) , TARGET_STRING ( "Peo" ) , 0 , 0 , 0 , 0 , 1 } , { 226 , 0 , TARGET_STRING
(
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/PQ/Sum1"
) , TARGET_STRING ( "Qeo" ) , 0 , 0 , 0 , 0 , 1 } , { 227 , 0 , TARGET_STRING
(
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Electromagnetic Torque/1-1"
) , TARGET_STRING ( "iqs -ids" ) , 0 , 0 , 4 , 0 , 1 } , { 228 , 0 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Electromagnetic Torque/Mult1"
) , TARGET_STRING ( "" ) , 0 , 0 , 4 , 0 , 1 } , { 229 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Electromagnetic Torque/Sum2"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 230 , 21 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Matrix W/W(1,2)=wr"
) , TARGET_STRING ( "" ) , 0 , 0 , 6 , 0 , 1 } , { 231 , 21 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Matrix W/W(2,1)=-wr"
) , TARGET_STRING ( "" ) , 0 , 0 , 6 , 0 , 1 } , { 232 , 21 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Matrix W/Gain1"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 233 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Phimqd/1//Ll_d"
) , TARGET_STRING ( "" ) , 0 , 0 , 1 , 0 , 1 } , { 234 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Phimqd/1//Ll_q"
) , TARGET_STRING ( "" ) , 0 , 0 , 1 , 0 , 1 } , { 235 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Phimqd/Product"
) , TARGET_STRING ( "" ) , 0 , 0 , 1 , 0 , 1 } , { 236 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Phimqd/Product1"
) , TARGET_STRING ( "" ) , 0 , 0 , 1 , 0 , 1 } , { 237 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Phimqd/sum1"
) , TARGET_STRING ( "phimd" ) , 0 , 0 , 0 , 0 , 1 } , { 238 , 0 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Phimqd/sum2"
) , TARGET_STRING ( "phimq" ) , 0 , 0 , 0 , 0 , 1 } , { 239 , 19 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 240 , 19 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat"
) , TARGET_STRING ( "" ) , 1 , 0 , 0 , 0 , 1 } , { 241 , 20 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Product1"
) , TARGET_STRING ( "R*Linv" ) , 0 , 0 , 6 , 0 , 1 } , { 242 , 20 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/inversion"
) , TARGET_STRING ( "Linv" ) , 0 , 0 , 6 , 0 , 1 } , { 243 , 20 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Switch1"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 244 , 20 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Switch2"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 245 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/Digital Clock"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 246 , 21 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/Product1"
) , TARGET_STRING ( "" ) , 0 , 0 , 3 , 0 , 1 } , { 247 , 21 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/Product2"
) , TARGET_STRING ( "" ) , 0 , 0 , 3 , 0 , 1 } , { 248 , 21 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/Ad*x(k-1) + Bd*( u(k-1) + u(k))"
) , TARGET_STRING ( "x(k)" ) , 0 , 0 , 3 , 0 , 1 } , { 249 , 21 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/sum"
) , TARGET_STRING ( "u(k) + u(k-1)" ) , 0 , 0 , 3 , 0 , 1 } , { 250 , 0 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/IC"
) , TARGET_STRING ( "" ) , 0 , 0 , 3 , 0 , 1 } , { 251 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/fluxes"
) , TARGET_STRING ( "x(k-1)" ) , 0 , 0 , 3 , 0 , 1 } , { 252 , 0 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/voltages"
) , TARGET_STRING ( "" ) , 0 , 0 , 3 , 0 , 1 } , { 253 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/Delta angle/Cartesian to Polar/x->r"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 254 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/Delta angle/Cartesian to Polar/x->theta"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 255 , 0 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/Delta angle/Radians to Degrees/Gain"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 256 , 20 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/Abs"
) , TARGET_STRING ( "Phisat" ) , 0 , 0 , 0 , 0 , 1 } , { 257 , 20 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/Lmd"
) , TARGET_STRING ( "Lmsatd" ) , 0 , 0 , 0 , 0 , 1 } , { 258 , 20 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/1-D Lookup Table"
) , TARGET_STRING ( "Ifdsat" ) , 0 , 0 , 0 , 0 , 1 } , { 259 , 17 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/Product"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 260 , 20 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/Switch"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 261 , 20 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/Lmd_sat"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 262 , 19 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/Abs"
) , TARGET_STRING ( "Phisat" ) , 0 , 0 , 0 , 0 , 1 } , { 263 , 19 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/Lmq"
) , TARGET_STRING ( "Lmsatq" ) , 0 , 0 , 0 , 0 , 1 } , { 264 , 19 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/1-D Lookup Table"
) , TARGET_STRING ( "Ifqsat" ) , 0 , 0 , 0 , 0 , 1 } , { 265 , 18 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/Product"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 266 , 19 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/Switch"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 267 , 19 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/Lmq_sat"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 268 , 20 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Update Matrix L/Update Lmd rows[2,3,4] col[2,3,4]"
) , TARGET_STRING ( "" ) , 0 , 0 , 6 , 0 , 1 } , { 269 , 20 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Update Matrix L/Update Lmq rows[1,5, (6)] col[1,5, (6)] "
) , TARGET_STRING ( "" ) , 0 , 0 , 6 , 0 , 1 } , { 270 , 20 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Update Matrix L/Sum2"
) , TARGET_STRING ( "" ) , 0 , 0 , 6 , 0 , 1 } , { 271 , 21 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/Subsystem/wbase*Ts//2"
) , TARGET_STRING ( "" ) , 0 , 0 , 6 , 0 , 1 } , { 272 , 21 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/Subsystem/wbase*Ts//2 "
) , TARGET_STRING ( "" ) , 0 , 0 , 6 , 0 , 1 } , { 273 , 21 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/Subsystem/Product4"
) , TARGET_STRING ( "" ) , 0 , 0 , 6 , 0 , 1 } , { 274 , 21 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/Subsystem/inversion"
) , TARGET_STRING ( "" ) , 0 , 0 , 6 , 0 , 1 } , { 275 , 21 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/Subsystem/Sum1"
) , TARGET_STRING ( "" ) , 0 , 0 , 6 , 0 , 1 } , { 276 , 21 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/Subsystem/Sum5"
) , TARGET_STRING ( "" ) , 0 , 0 , 6 , 0 , 1 } , { 277 , 20 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/Lad/Math Function2"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 278 , 20 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/Lad/Math Function3"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 279 , 20 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/Lad/Sum1"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 280 , 20 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/phimd/1//Ll_d"
) , TARGET_STRING ( "" ) , 0 , 0 , 1 , 0 , 1 } , { 281 , 20 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/phimd/Product4"
) , TARGET_STRING ( "phimd" ) , 0 , 0 , 0 , 0 , 1 } , { 282 , 20 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/phimd/sum2"
) , TARGET_STRING ( "phimd" ) , 0 , 0 , 0 , 0 , 1 } , { 283 , 19 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/Laq/Math Function"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 284 , 19 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/Laq/Math Function1"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 285 , 19 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/Laq/Sum2"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 286 , 19 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/phimq/1//Ll_q"
) , TARGET_STRING ( "" ) , 0 , 0 , 1 , 0 , 1 } , { 287 , 19 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/phimq/Product3"
) , TARGET_STRING ( "phimq" ) , 0 , 0 , 0 , 0 , 1 } , { 288 , 19 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/phimq/sum1"
) , TARGET_STRING ( "phimq" ) , 0 , 0 , 0 , 0 , 1 } , { 0 , 0 , ( NULL ) , (
NULL ) , 0 , 0 , 0 , 0 , 0 } } ; static const rtwCAPI_BlockParameters
rtBlockParameters [ ] = { { 289 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test" ) , TARGET_STRING ( "FaultA" ) , 0 , 0 , 0 } ,
{ 290 , TARGET_STRING ( "DZG_v6_SimGen/System/KZ Test" ) , TARGET_STRING (
"FaultB" ) , 0 , 0 , 0 } , { 291 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test" ) , TARGET_STRING ( "FaultC" ) , 0 , 0 , 0 } ,
{ 292 , TARGET_STRING ( "DZG_v6_SimGen/System/Q2" ) , TARGET_STRING (
"SwitchA" ) , 0 , 0 , 0 } , { 293 , TARGET_STRING ( "DZG_v6_SimGen/System/Q2"
) , TARGET_STRING ( "SwitchB" ) , 0 , 0 , 0 } , { 294 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2" ) , TARGET_STRING ( "SwitchC" ) , 0 , 0 , 0 } , {
295 , TARGET_STRING ( "DZG_v6_SimGen/System/System1" ) , TARGET_STRING (
"Frequency" ) , 0 , 0 , 0 } , { 296 , TARGET_STRING (
"DZG_v6_SimGen/System/Vf" ) , TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 297
, TARGET_STRING ( "DZG_v6_SimGen/System/Vf(PP)1" ) , TARGET_STRING ( "Value"
) , 0 , 0 , 0 } , { 298 , TARGET_STRING ( "DZG_v6_SimGen/System/4" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 299 , TARGET_STRING (
"DZG_v6_SimGen/System/7" ) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 300 ,
TARGET_STRING ( "DZG_v6_SimGen/System/Gain" ) , TARGET_STRING ( "Gain" ) , 0
, 0 , 0 } , { 301 , TARGET_STRING ( "DZG_v6_SimGen/System/Gain1" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 302 , TARGET_STRING (
"DZG_v6_SimGen/System/Gain3" ) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , {
303 , TARGET_STRING ( "DZG_v6_SimGen/System/Calc/Pm1" ) , TARGET_STRING (
"Value" ) , 0 , 0 , 0 } , { 304 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Gain" ) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } ,
{ 305 , TARGET_STRING ( "DZG_v6_SimGen/System/Calc/Gain1" ) , TARGET_STRING (
"Gain" ) , 0 , 0 , 0 } , { 306 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Gain4" ) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 }
, { 307 , TARGET_STRING ( "DZG_v6_SimGen/System/KZ Test/C4" ) , TARGET_STRING
( "Value" ) , 0 , 0 , 0 } , { 308 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/com" ) , TARGET_STRING ( "Value" ) , 0 , 0 , 0
} , { 309 , TARGET_STRING ( "DZG_v6_SimGen/System/KZ Test/Switch3" ) ,
TARGET_STRING ( "Threshold" ) , 0 , 0 , 0 } , { 310 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureGen_G2/Kv" ) , TARGET_STRING ( "Gain" ) , 0 , 0
, 0 } , { 311 , TARGET_STRING ( "DZG_v6_SimGen/System/MeasureGen_G2/Kv1" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 312 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine1/Kv" ) , TARGET_STRING ( "Gain" ) , 0 , 0 ,
0 } , { 313 , TARGET_STRING ( "DZG_v6_SimGen/System/MeasureLine1/Kv1" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 314 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine2/Kv" ) , TARGET_STRING ( "Gain" ) , 0 , 0 ,
0 } , { 315 , TARGET_STRING ( "DZG_v6_SimGen/System/MeasureLine2/Kv1" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 316 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLoad/Kv" ) , TARGET_STRING ( "Gain" ) , 0 , 0 ,
0 } , { 317 , TARGET_STRING ( "DZG_v6_SimGen/System/MeasureLoad/Kv1" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 318 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureSystem1/Kv" ) , TARGET_STRING ( "Gain" ) , 0 , 0
, 0 } , { 319 , TARGET_STRING ( "DZG_v6_SimGen/System/MeasureSystem1/Kv1" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 320 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_G1/Kv" ) , TARGET_STRING ( "Gain" ) , 0 , 0 ,
0 } , { 321 , TARGET_STRING ( "DZG_v6_SimGen/System/MeasureTr_G1/Kv1" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 322 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_H/Kv" ) , TARGET_STRING ( "Gain" ) , 0 , 0 ,
0 } , { 323 , TARGET_STRING ( "DZG_v6_SimGen/System/MeasureTr_H/Kv1" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 324 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/C4" ) , TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , {
325 , TARGET_STRING ( "DZG_v6_SimGen/System/Q2/Constant5" ) , TARGET_STRING (
"Value" ) , 0 , 0 , 0 } , { 326 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/com" ) , TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , {
327 , TARGET_STRING ( "DZG_v6_SimGen/System/Q2/Switch" ) , TARGET_STRING (
"Threshold" ) , 0 , 0 , 0 } , { 328 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Switch1" ) , TARGET_STRING ( "Threshold" ) , 0 , 0 ,
0 } , { 329 , TARGET_STRING ( "DZG_v6_SimGen/System/Q2/Switch2" ) ,
TARGET_STRING ( "Threshold" ) , 0 , 0 , 0 } , { 330 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Switch3" ) , TARGET_STRING ( "Threshold" ) , 0 , 0 ,
0 } , { 331 , TARGET_STRING (
"DZG_v6_SimGen/powergui/EquivalentModel1/State-Space" ) , TARGET_STRING (
"AS_param" ) , 0 , 6 , 0 } , { 332 , TARGET_STRING (
"DZG_v6_SimGen/powergui/EquivalentModel1/State-Space" ) , TARGET_STRING (
"BS_param" ) , 0 , 7 , 0 } , { 333 , TARGET_STRING (
"DZG_v6_SimGen/powergui/EquivalentModel1/State-Space" ) , TARGET_STRING (
"CS_param" ) , 0 , 8 , 0 } , { 334 , TARGET_STRING (
"DZG_v6_SimGen/powergui/EquivalentModel1/State-Space" ) , TARGET_STRING (
"DS_param" ) , 0 , 9 , 0 } , { 335 , TARGET_STRING (
"DZG_v6_SimGen/powergui/EquivalentModel1/State-Space" ) , TARGET_STRING (
"X0_param" ) , 0 , 3 , 0 } , { 336 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Rad->Deg." ) , TARGET_STRING ( "Gain" ) ,
0 , 0 , 0 } , { 337 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/cos(wt)" ) , TARGET_STRING ( "Amplitude" )
, 0 , 0 , 0 } , { 338 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/cos(wt)" ) , TARGET_STRING ( "Bias" ) , 0
, 0 , 0 } , { 339 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/cos(wt)" ) , TARGET_STRING ( "Frequency" )
, 0 , 0 , 0 } , { 340 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/cos(wt)" ) , TARGET_STRING ( "Phase" ) , 0
, 0 , 0 } , { 341 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/sin(wt)" ) , TARGET_STRING ( "Amplitude" )
, 0 , 0 , 0 } , { 342 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/sin(wt)" ) , TARGET_STRING ( "Bias" ) , 0
, 0 , 0 } , { 343 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/sin(wt)" ) , TARGET_STRING ( "Frequency" )
, 0 , 0 , 0 } , { 344 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/sin(wt)" ) , TARGET_STRING ( "Phase" ) , 0
, 0 , 0 } , { 345 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Rad->Deg." ) , TARGET_STRING ( "Gain" ) ,
0 , 0 , 0 } , { 346 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/cos(wt)" ) , TARGET_STRING ( "Amplitude"
) , 0 , 0 , 0 } , { 347 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/cos(wt)" ) , TARGET_STRING ( "Bias" ) , 0
, 0 , 0 } , { 348 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/cos(wt)" ) , TARGET_STRING ( "Frequency"
) , 0 , 0 , 0 } , { 349 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/cos(wt)" ) , TARGET_STRING ( "Phase" ) ,
0 , 0 , 0 } , { 350 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/sin(wt)" ) , TARGET_STRING ( "Amplitude"
) , 0 , 0 , 0 } , { 351 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/sin(wt)" ) , TARGET_STRING ( "Bias" ) , 0
, 0 , 0 } , { 352 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/sin(wt)" ) , TARGET_STRING ( "Frequency"
) , 0 , 0 , 0 } , { 353 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/sin(wt)" ) , TARGET_STRING ( "Phase" ) ,
0 , 0 , 0 } , { 354 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/[ Vkd =0 Vkq1=0  Vkq2=0 ]"
) , TARGET_STRING ( "Value" ) , 0 , 10 , 0 } , { 355 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/N" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 356 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/nominal speed"
) , TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 357 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Rotor angle dthetae"
) , TARGET_STRING ( "gainval" ) , 0 , 0 , 0 } , { 358 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Rotor angle dthetae"
) , TARGET_STRING ( "InitialCondition" ) , 0 , 0 , 0 } , { 359 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Rotor speed deviation (dw)"
) , TARGET_STRING ( "gainval" ) , 0 , 0 , 0 } , { 360 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Rotor speed deviation (dw)"
) , TARGET_STRING ( "InitialCondition" ) , 0 , 0 , 0 } , { 361 ,
TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/theta"
) , TARGET_STRING ( "gainval" ) , 0 , 0 , 0 } , { 362 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/theta"
) , TARGET_STRING ( "InitialCondition" ) , 0 , 0 , 0 } , { 363 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/1 ----- 2H"
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 364 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/1_Pb"
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 365 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/F" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 366 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/t" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 367 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/units"
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 368 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/units1"
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 369 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/units2"
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 370 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/we base"
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 371 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/web2"
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 372 , TARGET_STRING (
"DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/web3"
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 373 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave A" ) , TARGET_STRING (
"Amplitude" ) , 0 , 0 , 0 } , { 374 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave A" ) , TARGET_STRING ( "Bias" )
, 0 , 0 , 0 } , { 375 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave A" ) , TARGET_STRING ( "SinH" )
, 0 , 0 , 0 } , { 376 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave A" ) , TARGET_STRING ( "CosH" )
, 0 , 0 , 0 } , { 377 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave A" ) , TARGET_STRING ( "SinPhi"
) , 0 , 0 , 0 } , { 378 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave A" ) , TARGET_STRING ( "CosPhi"
) , 0 , 0 , 0 } , { 379 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave B" ) , TARGET_STRING (
"Amplitude" ) , 0 , 0 , 0 } , { 380 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave B" ) , TARGET_STRING ( "Bias" )
, 0 , 0 , 0 } , { 381 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave B" ) , TARGET_STRING ( "SinH" )
, 0 , 0 , 0 } , { 382 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave B" ) , TARGET_STRING ( "CosH" )
, 0 , 0 , 0 } , { 383 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave B" ) , TARGET_STRING ( "SinPhi"
) , 0 , 0 , 0 } , { 384 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave B" ) , TARGET_STRING ( "CosPhi"
) , 0 , 0 , 0 } , { 385 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave C" ) , TARGET_STRING (
"Amplitude" ) , 0 , 0 , 0 } , { 386 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave C" ) , TARGET_STRING ( "Bias" )
, 0 , 0 , 0 } , { 387 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave C" ) , TARGET_STRING ( "SinH" )
, 0 , 0 , 0 } , { 388 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave C" ) , TARGET_STRING ( "CosH" )
, 0 , 0 , 0 } , { 389 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave C" ) , TARGET_STRING ( "SinPhi"
) , 0 , 0 , 0 } , { 390 , TARGET_STRING (
"DZG_v6_SimGen/System/System1/Model/Sine Wave C" ) , TARGET_STRING ( "CosPhi"
) , 0 , 0 , 0 } , { 391 , TARGET_STRING (
"DZG_v6_SimGen/powergui/EquivalentModel1/Sources/SwitchCurrents" ) ,
TARGET_STRING ( "Value" ) , 0 , 3 , 0 } , { 392 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault A/Model/C4" ) , TARGET_STRING ( "Value" )
, 0 , 0 , 0 } , { 393 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault A/Model/Switch3" ) , TARGET_STRING (
"Threshold" ) , 0 , 0 , 0 } , { 394 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault B/Model/C4" ) , TARGET_STRING ( "Value" )
, 0 , 0 , 0 } , { 395 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault B/Model/Switch3" ) , TARGET_STRING (
"Threshold" ) , 0 , 0 , 0 } , { 396 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault C/Model/C4" ) , TARGET_STRING ( "Value" )
, 0 , 0 , 0 } , { 397 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault C/Model/Switch3" ) , TARGET_STRING (
"Threshold" ) , 0 , 0 , 0 } , { 398 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Stair Generator/Model/Look-Up Table" ) ,
TARGET_STRING ( "InputValues" ) , 0 , 11 , 0 } , { 399 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Stair Generator/Model/Look-Up Table" ) ,
TARGET_STRING ( "Table" ) , 0 , 11 , 0 } , { 400 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureGen_G2/Model/I A:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 401 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureGen_G2/Model/I B:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 402 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureGen_G2/Model/I C:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 403 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureGen_G2/Model/U A:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 404 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureGen_G2/Model/U B:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 405 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureGen_G2/Model/U C:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 406 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine1/Model/I A:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 407 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine1/Model/I B:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 408 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine1/Model/I C:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 409 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine1/Model/U A:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 410 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine1/Model/U B:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 411 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine1/Model/U C:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 412 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine2/Model/I A:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 413 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine2/Model/I B:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 414 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine2/Model/I C:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 415 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine2/Model/U A:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 416 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine2/Model/U B:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 417 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLine2/Model/U C:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 418 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLoad/Model/I A:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 419 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLoad/Model/I B:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 420 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLoad/Model/I C:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 421 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLoad/Model/U A:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 422 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLoad/Model/U B:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 423 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureLoad/Model/U C:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 424 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureSystem1/Model/I A:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 425 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureSystem1/Model/I B:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 426 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureSystem1/Model/I C:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 427 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureSystem1/Model/U A:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 428 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureSystem1/Model/U B:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 429 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureSystem1/Model/U C:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 430 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_G1/Model/I A:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 431 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_G1/Model/I B:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 432 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_G1/Model/I C:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 433 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_G1/Model/U A:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 434 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_G1/Model/U B:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 435 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_G1/Model/U C:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 436 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_H/Model/I A:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 437 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_H/Model/I B:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 438 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_H/Model/I C:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 439 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_H/Model/U A:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 440 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_H/Model/U B:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 441 , TARGET_STRING (
"DZG_v6_SimGen/System/MeasureTr_H/Model/U C:/do not delete this gain" ) ,
TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 442 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker A/Model/C4" ) , TARGET_STRING ( "Value" ) ,
0 , 0 , 0 } , { 443 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker A/Model/Switch3" ) , TARGET_STRING (
"Threshold" ) , 0 , 0 , 0 } , { 444 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker B/Model/C4" ) , TARGET_STRING ( "Value" ) ,
0 , 0 , 0 } , { 445 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker B/Model/Switch3" ) , TARGET_STRING (
"Threshold" ) , 0 , 0 , 0 } , { 446 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker C/Model/C4" ) , TARGET_STRING ( "Value" ) ,
0 , 0 , 0 } , { 447 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker C/Model/Switch3" ) , TARGET_STRING (
"Threshold" ) , 0 , 0 , 0 } , { 448 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Stair Generator/Model/Look-Up Table" ) ,
TARGET_STRING ( "InputValues" ) , 0 , 11 , 0 } , { 449 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Stair Generator/Model/Look-Up Table" ) ,
TARGET_STRING ( "Table" ) , 0 , 11 , 0 } , { 450 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/Constant1"
) , TARGET_STRING ( "Value" ) , 1 , 0 , 0 } , { 451 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/Constant2"
) , TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 452 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/output formatting"
) , TARGET_STRING ( "Gain" ) , 0 , 12 , 0 } , { 453 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Constant1"
) , TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 454 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Constant2"
) , TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 455 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Constant3"
) , TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 456 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Constant4"
) , TARGET_STRING ( "Value" ) , 0 , 6 , 0 } , { 457 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Constant5"
) , TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 458 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Constant6"
) , TARGET_STRING ( "Value" ) , 0 , 6 , 0 } , { 459 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Constant8"
) , TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 460 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Laqd_nosat"
) , TARGET_STRING ( "Value" ) , 0 , 13 , 0 } , { 461 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Lmqd_nosat"
) , TARGET_STRING ( "Value" ) , 0 , 13 , 0 } , { 462 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/change Iq Id  current signs"
) , TARGET_STRING ( "Gain" ) , 0 , 11 , 0 } , { 463 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Switch"
) , TARGET_STRING ( "Threshold" ) , 0 , 0 , 0 } , { 464 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Switch1"
) , TARGET_STRING ( "Threshold" ) , 0 , 0 , 0 } , { 465 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Switch2"
) , TARGET_STRING ( "Threshold" ) , 0 , 0 , 0 } , { 466 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Switch3"
) , TARGET_STRING ( "Threshold" ) , 0 , 0 , 0 } , { 467 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/abc to qd transformation/1_Vb"
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 468 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/qd to abc transformation/ib"
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 469 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Delay Prediction/F2"
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 470 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Delay Prediction/dw_delay"
) , TARGET_STRING ( "InitialCondition" ) , 0 , 0 , 0 } , { 471 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Mechanical model/Delay Prediction/dw_predict"
) , TARGET_STRING ( "InitialCondition" ) , 0 , 0 , 0 } , { 472 ,
TARGET_STRING ( "DZG_v6_SimGen/System/Calc/Fourier/Mean/Model/K1" ) ,
TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 473 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean/Model/Gain" ) , TARGET_STRING (
"Gain" ) , 0 , 0 , 0 } , { 474 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean/Model/integrator" ) , TARGET_STRING (
"InitialCondition" ) , 0 , 0 , 0 } , { 475 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean/Model/Memory" ) , TARGET_STRING (
"InitialCondition" ) , 0 , 0 , 0 } , { 476 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean/Model/Transport Delay" ) ,
TARGET_STRING ( "DelayTime" ) , 0 , 0 , 0 } , { 477 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean/Model/Transport Delay" ) ,
TARGET_STRING ( "InitialOutput" ) , 0 , 0 , 0 } , { 478 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean value1/Model/K1" ) , TARGET_STRING (
"Value" ) , 0 , 0 , 0 } , { 479 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean value1/Model/Gain" ) , TARGET_STRING
( "Gain" ) , 0 , 0 , 0 } , { 480 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean value1/Model/integrator" ) ,
TARGET_STRING ( "InitialCondition" ) , 0 , 0 , 0 } , { 481 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean value1/Model/Memory" ) ,
TARGET_STRING ( "InitialCondition" ) , 0 , 0 , 0 } , { 482 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean value1/Model/Transport Delay" ) ,
TARGET_STRING ( "DelayTime" ) , 0 , 0 , 0 } , { 483 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier/Mean value1/Model/Transport Delay" ) ,
TARGET_STRING ( "InitialOutput" ) , 0 , 0 , 0 } , { 484 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean/Model/K1" ) , TARGET_STRING (
"Value" ) , 0 , 0 , 0 } , { 485 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean/Model/Gain" ) , TARGET_STRING (
"Gain" ) , 0 , 0 , 0 } , { 486 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean/Model/integrator" ) , TARGET_STRING
( "InitialCondition" ) , 0 , 0 , 0 } , { 487 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean/Model/Memory" ) , TARGET_STRING (
"InitialCondition" ) , 0 , 0 , 0 } , { 488 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean/Model/Transport Delay" ) ,
TARGET_STRING ( "DelayTime" ) , 0 , 0 , 0 } , { 489 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean/Model/Transport Delay" ) ,
TARGET_STRING ( "InitialOutput" ) , 0 , 0 , 0 } , { 490 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean value1/Model/K1" ) , TARGET_STRING (
"Value" ) , 0 , 0 , 0 } , { 491 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean value1/Model/Gain" ) , TARGET_STRING
( "Gain" ) , 0 , 0 , 0 } , { 492 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean value1/Model/integrator" ) ,
TARGET_STRING ( "InitialCondition" ) , 0 , 0 , 0 } , { 493 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean value1/Model/Memory" ) ,
TARGET_STRING ( "InitialCondition" ) , 0 , 0 , 0 } , { 494 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean value1/Model/Transport Delay" ) ,
TARGET_STRING ( "DelayTime" ) , 0 , 0 , 0 } , { 495 , TARGET_STRING (
"DZG_v6_SimGen/System/Calc/Fourier1/Mean value1/Model/Transport Delay" ) ,
TARGET_STRING ( "InitialOutput" ) , 0 , 0 , 0 } , { 496 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault A/Model/Timer/Look-Up Table" ) ,
TARGET_STRING ( "InputValues" ) , 0 , 14 , 0 } , { 497 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault A/Model/Timer/Look-Up Table" ) ,
TARGET_STRING ( "Table" ) , 0 , 14 , 0 } , { 498 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault B/Model/Timer/Look-Up Table" ) ,
TARGET_STRING ( "InputValues" ) , 0 , 14 , 0 } , { 499 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault B/Model/Timer/Look-Up Table" ) ,
TARGET_STRING ( "Table" ) , 0 , 14 , 0 } , { 500 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault C/Model/Timer/Look-Up Table" ) ,
TARGET_STRING ( "InputValues" ) , 0 , 14 , 0 } , { 501 , TARGET_STRING (
"DZG_v6_SimGen/System/KZ Test/Fault C/Model/Timer/Look-Up Table" ) ,
TARGET_STRING ( "Table" ) , 0 , 14 , 0 } , { 502 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker A/Model/Timer/Look-Up Table" ) ,
TARGET_STRING ( "InputValues" ) , 0 , 14 , 0 } , { 503 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker A/Model/Timer/Look-Up Table" ) ,
TARGET_STRING ( "Table" ) , 0 , 14 , 0 } , { 504 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker B/Model/Timer/Look-Up Table" ) ,
TARGET_STRING ( "InputValues" ) , 0 , 14 , 0 } , { 505 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker B/Model/Timer/Look-Up Table" ) ,
TARGET_STRING ( "Table" ) , 0 , 14 , 0 } , { 506 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker C/Model/Timer/Look-Up Table" ) ,
TARGET_STRING ( "InputValues" ) , 0 , 14 , 0 } , { 507 , TARGET_STRING (
"DZG_v6_SimGen/System/Q2/Breaker C/Model/Timer/Look-Up Table" ) ,
TARGET_STRING ( "Table" ) , 0 , 14 , 0 } , { 508 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/PQ/Gain"
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 509 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/PQ/Gain1"
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 510 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Electromagnetic Torque/1-1"
) , TARGET_STRING ( "Gain" ) , 0 , 13 , 0 } , { 511 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Matrix W/u1"
) , TARGET_STRING ( "Value" ) , 0 , 6 , 0 } , { 512 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Matrix W/Gain1"
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 513 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Phimqd/1//Ll_d"
) , TARGET_STRING ( "Gain" ) , 0 , 10 , 0 } , { 514 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Phimqd/1//Ll_q"
) , TARGET_STRING ( "Gain" ) , 0 , 10 , 0 } , { 515 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Linv"
) , TARGET_STRING ( "InitialOutput" ) , 0 , 0 , 0 } , { 516 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/R*Linv"
) , TARGET_STRING ( "InitialOutput" ) , 0 , 0 , 0 } , { 517 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmqd"
) , TARGET_STRING ( "InitialOutput" ) , 0 , 0 , 0 } , { 518 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Laqd"
) , TARGET_STRING ( "InitialOutput" ) , 0 , 0 , 0 } , { 519 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Constant1"
) , TARGET_STRING ( "Value" ) , 1 , 0 , 0 } , { 520 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Constant2"
) , TARGET_STRING ( "Value" ) , 1 , 0 , 0 } , { 521 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Constant3"
) , TARGET_STRING ( "Value" ) , 1 , 0 , 0 } , { 522 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/u1"
) , TARGET_STRING ( "Value" ) , 0 , 6 , 0 } , { 523 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/u2"
) , TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 524 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/u3"
) , TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 525 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/IC"
) , TARGET_STRING ( "Threshold" ) , 0 , 0 , 0 } , { 526 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/fluxes"
) , TARGET_STRING ( "InitialCondition" ) , 0 , 3 , 0 } , { 527 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/voltages"
) , TARGET_STRING ( "InitialCondition" ) , 0 , 0 , 0 } , { 528 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Machine measurements/Delta angle/Radians to Degrees/Gain"
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 529 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/Constant1"
) , TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 530 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/Lmd"
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 531 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/1-D Lookup Table"
) , TARGET_STRING ( "Table" ) , 0 , 15 , 0 } , { 532 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/1-D Lookup Table"
) , TARGET_STRING ( "BreakpointsForDimension1" ) , 0 , 15 , 0 } , { 533 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/Lmd_sat"
) , TARGET_STRING ( "InitialCondition" ) , 0 , 0 , 0 } , { 534 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/Lmq_sat "
) , TARGET_STRING ( "InitialOutput" ) , 0 , 0 , 0 } , { 535 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/Laq_"
) , TARGET_STRING ( "InitialOutput" ) , 0 , 0 , 0 } , { 536 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/Constant1"
) , TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 537 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/Lmq"
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 538 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/1-D Lookup Table"
) , TARGET_STRING ( "Table" ) , 0 , 15 , 0 } , { 539 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/1-D Lookup Table"
) , TARGET_STRING ( "BreakpointsForDimension1" ) , 0 , 15 , 0 } , { 540 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/Lmq_sat"
) , TARGET_STRING ( "InitialCondition" ) , 0 , 0 , 0 } , { 541 ,
TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Update Matrix L/u1"
) , TARGET_STRING ( "Value" ) , 0 , 6 , 0 } , { 542 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Update Matrix L/u5"
) , TARGET_STRING ( "Value" ) , 0 , 6 , 0 } , { 543 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/Subsystem/u5"
) , TARGET_STRING ( "Value" ) , 0 , 6 , 0 } , { 544 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/Subsystem/wbase*Ts//2"
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 545 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/phi/Subsystem/wbase*Ts//2 "
) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } , { 546 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/Lad/u1"
) , TARGET_STRING ( "Value" ) , 0 , 13 , 0 } , { 547 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmd_sat/phimd/1//Ll_d"
) , TARGET_STRING ( "Gain" ) , 0 , 10 , 0 } , { 548 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/Laq/u2"
) , TARGET_STRING ( "Value" ) , 0 , 10 , 0 } , { 549 , TARGET_STRING (
 "DZG_v6_SimGen/System/Synchronous Machine pu Standard/Electrical model/Synchronous Machine Discrete Model/Saturation/Lmq_sat/phimq/1//Ll_q"
) , TARGET_STRING ( "Gain" ) , 0 , 10 , 0 } , { 0 , ( NULL ) , ( NULL ) , 0 ,
0 , 0 } } ; static const rtwCAPI_ModelParameters rtModelParameters [ ] = { {
0 , ( NULL ) , 0 , 0 , 0 } } ;
#ifndef HOST_CAPI_BUILD
static void * rtDataAddrMap [ ] = { & rtB . nzu205zzco , & rtB . n1sf2oxpy1 ,
& rtB . b14bfbabib , & rtB . mwnm3xw2te , & rtB . gpq0cumlrp , & rtB .
ixvhkcticg , & rtB . ihbtz02goz , & rtB . h33eecit0h , & rtB . bz4ixbddyb , &
rtB . dkxzgq2elf , & rtB . fvgxbp0r0b , & rtB . hcnu41udwm , & rtB .
bbfgsoogaz , & rtB . jsc1e3jlwj , & rtB . kkthzbzxh4 , & rtB . f05xiwzeii , &
rtB . hq4dyb3apy , & rtB . ecp5cwxuux , & rtB . frxxgkpfd0 , & rtB .
guw3ibew5f , & rtB . kgpjy2123a , & rtB . dv21ecppse , & rtB . ot21w1gcro , &
rtB . geos3slncn , & rtB . cpugwweowj [ 0 ] , & rtB . l0q0zwxfui [ 0 ] , &
rtB . embtezfk0j [ 0 ] , & rtB . i5mjxvb2y1 [ 0 ] , & rtB . chk1pd3rlp [ 0 ]
, & rtB . o04nrxwopl [ 0 ] , & rtB . aei40fflxj [ 0 ] , & rtB . k3tazd503n [
0 ] , & rtB . g2dr1qmh1g [ 0 ] , & rtB . hm1ran4pnq [ 0 ] , & rtB .
bv31jmhnur [ 0 ] , & rtB . ikwzxkxyiz [ 0 ] , & rtB . gqlskp40nb [ 0 ] , &
rtB . czdxilbjs1 [ 0 ] , & rtB . hfxqkjhn3c , & rtB . jwcmbwsc34 , & rtB .
iipz2h3t1h , & rtB . abudmgdmqo , & rtB . bkf5f3ujyq [ 0 ] , & rtB .
mcctrqnlij [ 0 ] , & rtB . hu45skyzd5 , & rtB . nxmiliesal , & rtB .
lz5rdlkvrc , & rtB . epw4e2nyr2 , & rtB . hvccs5ksdj , & rtB . jzulpokwia .
re , & rtB . onhlo5lk3i , & rtB . ht3pkpouae , & rtB . b5yrakypqk , & rtB .
kyvh55ysq1 , & rtB . njwnc1thzp , & rtB . lxuyvf0kct , & rtB . hdfe5utvyn , &
rtB . dngbcewebp . re , & rtB . cpurtfy0lf , & rtB . mv11qhbyiu , & rtB .
jo0ewhen5p , & rtB . do04422dsh [ 0 ] , & rtB . ocwtlsstoa [ 0 ] , & rtB .
nddeflp2rr [ 0 ] , & rtB . phf1q3qtgy , & rtB . o0laqbijxj , & rtB .
dn1jsu5b3a , & rtB . h2yyrby5su , & rtB . jyft0oavfh , & rtB . m3hbl5oi51 , &
rtB . fmoyvyrdin , & rtB . cuthot4b55 , & rtB . kr1dizdsxr , & rtB .
hlupe3wd1s , & rtB . o3xqnnpyja , & rtB . k33xjpkxfj , & rtB . ogr2enhepf , &
rtB . anv20tflb2 , & rtB . adxx2eh1g0 , & rtB . eajcmlcqmc , & rtB .
ax2dm5rpub , & rtB . gsp2mdotvq , & rtB . bxfyqcorw3 [ 0 ] , & rtB .
i3cpus0vrs , & rtB . gftya03rfs , & rtB . fx0kk0lfk3 , & rtB . kc2wsb45fs , &
rtB . l2zafpzrzu , & rtB . ohedl35wf1 , & rtB . fyt1uabhvi , & rtB .
i20vaspyrp , & rtB . if3uw30dno , & rtB . oj12dzvfb4 , & rtB . fwobh2ydkx , &
rtB . p4d344o4a4 , & rtB . ejzz1lck3x , & rtB . clkwduumjv , & rtB .
oecmwuiwrp , & rtB . m15cezxam4 , & rtB . brcpuhmr2n , & rtB . bsjs3yr3cm , &
rtB . omi22g0ht3 , & rtB . maqdked4sl , & rtB . pxtshpab0u , & rtB .
orydyp31zs , & rtB . btppbtrakx , & rtB . icofxpvarr , & rtB . n3yespd0xf , &
rtB . kfeoejnqkx , & rtB . kalgaozq2e , & rtB . c3mov1tl1f , & rtB .
mj5ueklf54 , & rtB . nfbe1f5aq4 , & rtB . gpbkfrtopv , & rtB . emcqpstfhw , &
rtB . m315dx4bxe , & rtB . lixhx51dy4 , & rtB . arirsm2gtb , & rtB .
pchaeyb4wc , & rtB . geuzk04gqe , & rtB . nx2s4jnhlc , & rtB . c5z4wv1mbn , &
rtB . ixbmptgikc , & rtB . g1pav31o54 , & rtB . or1sotd5ok , & rtB .
oawoasztyv , & rtB . ez3n02akaw , & rtB . im0zegy3zl , & rtB . ca0k4g2ixd , &
rtB . g4um4nifj0 , & rtB . fgeeodibr2 , & rtB . lzor53a1zc , & rtB .
a4mondaqym , & rtB . e3fyv2i0xt , & rtB . pv0jl14wdm , & rtB . gkxjm351v3 , &
rtB . cjgzre1rfd , & rtB . mmqgxd3ztq , & rtB . kfya4ws5wt , & rtB .
gh0e24cnt4 , & rtB . mnz0k5z1ev , & rtB . paxzwpdtyr , & rtB . cka3cbvjkk , &
rtB . jd3k1qt01g , & rtB . i34c3sd5c4 , & rtB . kflvqeslk3 , & rtB .
pubftwd23q , & rtB . nnbfzmzsox [ 0 ] , & rtB . gqo15jq4yq , & rtB .
mecr0anvzq [ 0 ] , & rtB . g5yq3s1oi3 [ 0 ] , & rtB . bzlrt5abhc , & rtB .
msc0q3ydkn , & rtB . gtavo0wfmk , & rtB . o5o0z0ts4i , & rtB . d2l3x5dmoq [ 0
] , & rtB . i2htmqgs5h [ 0 ] , & rtB . khdzdbpnht [ 0 ] , & rtB . kxk50ui10l
[ 0 ] , & rtB . dx3rmsixh3 [ 0 ] , & rtB . lwg1hzp3ql [ 0 ] , & rtB .
by0csc0mva [ 0 ] , & rtB . g2rmjbbycl , & rtB . ljwiyate1t , & rtB .
kctgawizah [ 0 ] , & rtB . ps502vbzlw , & rtB . pz2b2ixe3g , & rtB .
a3kfgrogpp , & rtB . dggh1ark2b , & rtB . gfcufizx5q [ 0 ] , & rtB .
hleuoxbvtr , & rtB . eekpbkk0js , & rtB . gilkef2gvf , & rtB . kt4yqwu22g , &
rtB . bzg42dyv2g , & rtB . jfbplyjjoy , & rtB . l25oveef2i , & rtB .
f3bj13s54m , & rtB . mcclxrnvnu , & rtB . hswklfu3tm , & rtB . orqdb4jq0f , &
rtB . dv5odcc5un , & rtB . dljdyoojd5 , & rtB . ed1n42eefk , & rtB .
ks1ri00jvc , & rtB . cuumld5jhq , & rtB . i524ua1zo0 , & rtB . lvq5fe4dss , &
rtB . jjuwkhbaqb , & rtB . e11hdbixe4 , & rtB . czblwzh330 , & rtB .
amlu2hdzhw , & rtB . euj2mhfzyp , & rtB . jpdedqtzey , & rtB . jh3odldqrg , &
rtB . jgzslezpvv , & rtB . n1w1gaeskg , & rtB . caiqrgrb4h , & rtB .
jn3gd54hx5 , & rtB . lsnwzrnmnc , & rtB . bzogwznvbx , & rtB . a2aelbuuxd , &
rtB . m4o5gfq3hr , & rtB . nhjhuszeqq , & rtB . prtdictlyg , & rtB .
o1nbxtrkf4 , & rtB . lb3o0vdenr , & rtB . gmd2h4cezq , & rtB . b133gvafim , &
rtB . mzzuyzwyzo , & rtB . i4qgt25dqa , & rtB . pu5azi4ouj , & rtB .
kji2nsjrqr , & rtB . jnufx11sbb , & rtB . oq2fmk4tkh , & rtB . n3cickajoe , &
rtB . mnmjtwzkhg , & rtB . dvmo0qrznq , & rtB . p2blqb2x40 , & rtB .
n4glgvjxv2 , & rtB . ahl15fuofz , & rtB . otvyxcolz0 , & rtB . ca1sh32w2e , &
rtB . dq2eyjw50a , & rtB . fyirte2cly , & rtB . ogxs2lrdho , & rtB .
i2tqhru53h , & rtB . bsib0j2uiv [ 0 ] , & rtB . iwzh4cmhgz [ 0 ] , & rtB .
kbpgzmhgnk , & rtB . n0sluxojte [ 0 ] , & rtB . ofyfrkcfxb [ 0 ] , & rtB .
lyhqkuq1c4 , & rtB . h5nuuqtufn [ 0 ] , & rtB . hwnr1apibg [ 0 ] , & rtB .
l1lpbegscp [ 0 ] , & rtB . bdvh4jtcpt [ 0 ] , & rtB . l5qxq2mybh , & rtB .
ck4g14oqxl , & rtB . enoczvl5ij , & rtB . im1q1kmaf4 , & rtB . g5yq3s1oi3 [ 0
] , & rtB . mecr0anvzq [ 0 ] , & rtB . bzlrt5abhc , & rtB . gtavo0wfmk , &
rtB . gwy50czbqx , & rtB . cbygkknxsv [ 0 ] , & rtB . c1v5fiqxar [ 0 ] , &
rtB . cfmayp3lnt [ 0 ] , & rtB . janqpo5qe4 [ 0 ] , & rtB . hjl4wiy52h [ 0 ]
, & rtB . mshc0z4y0b [ 0 ] , & rtB . mnrqebdkci [ 0 ] , & rtB . ooqvccplqr ,
& rtB . c4xvwz5dvp , & rtB . hv3y4xtyo5 , & rtB . pfjhtzhhke , & rtB .
msc0q3ydkn , & rtB . dsgaag2j1a , & rtB . cy4aoyd4p4 , & rtB . ddpigcbdzw , &
rtB . kgqd5i3tmd , & rtB . babh1jhck2 , & rtB . enoczvl5ij , & rtB .
op4v4vz1hb , & rtB . l42zjb2acu , & rtB . elxflb4cn2 , & rtB . pnchbrbzh4 , &
rtB . g30midesdo [ 0 ] , & rtB . mc0c4fprgt [ 0 ] , & rtB . k3cesiw1su [ 0 ]
, & rtB . dgwxdhwkdd [ 0 ] , & rtB . g0lz5qytlu [ 0 ] , & rtB . deid52kk1x [
0 ] , & rtB . gdroql4j2f [ 0 ] , & rtB . caz1neybgm [ 0 ] , & rtB .
ncbiuw0cin [ 0 ] , & rtB . pmvqbzeo1a , & rtB . o5o0z0ts4i , & rtB .
lnslax3wky , & rtB . b2rku1yvfe [ 0 ] , & rtB . ke1trlx0xn , & rtB .
iqgj54dvfu , & rtB . h42emyzqm1 , & rtB . im1q1kmaf4 , & rtB . mw4gdhqybz , &
rtB . p4nck3maqs [ 0 ] , & rtB . a0tcaxg1ye , & rtB . lv1l3120cn , & rtP .
KZTest_FaultA , & rtP . KZTest_FaultB , & rtP . KZTest_FaultC , & rtP .
Q2_SwitchA , & rtP . Q2_SwitchB , & rtP . Q2_SwitchC , & rtP .
System1_Frequency , & rtP . Vf_Value , & rtP . VfPP1_Value , & rtP . u_Gain ,
& rtP . u_Gain_prbig4sao4 , & rtP . Gain_Gain_bpzmbe1g2k , & rtP .
Gain1_Gain_h3tkw2h2wq , & rtP . Gain3_Gain , & rtP . Pm1_Value , & rtP .
Gain_Gain_hktcntwr5c , & rtP . Gain1_Gain_nmin1oira0 , & rtP . Gain4_Gain , &
rtP . C4_Value_gcbzcnw2m4 , & rtP . com_Value , & rtP .
Switch3_Threshold_o2qhhfyd2s , & rtP . Kv_Gain , & rtP . Kv1_Gain , & rtP .
Kv_Gain_b1jtzjwn5z , & rtP . Kv1_Gain_fhx1xa0qe5 , & rtP . Kv_Gain_hbrwnhtc5z
, & rtP . Kv1_Gain_bls2zebxwe , & rtP . Kv_Gain_dc42uxbym3 , & rtP .
Kv1_Gain_js4lqtbwg1 , & rtP . Kv_Gain_gr1trvmmq4 , & rtP .
Kv1_Gain_lejhoixvzv , & rtP . Kv_Gain_dryr11wp4y , & rtP .
Kv1_Gain_itak2vpaxj , & rtP . Kv_Gain_ccfkg0ba2b , & rtP .
Kv1_Gain_oi10siy4fh , & rtP . C4_Value_ouf3ao4h2o , & rtP .
Constant5_Value_eyvmukmjzw , & rtP . com_Value_e0niij5lrb , & rtP .
Switch_Threshold , & rtP . Switch1_Threshold , & rtP . Switch2_Threshold , &
rtP . Switch3_Threshold_js4dz2naas , & rtP . StateSpace_AS_param [ 0 ] , &
rtP . StateSpace_BS_param [ 0 ] , & rtP . StateSpace_CS_param [ 0 ] , & rtP .
StateSpace_DS_param [ 0 ] , & rtP . StateSpace_X0_param [ 0 ] , & rtP .
RadDeg_Gain , & rtP . coswt_Amp , & rtP . coswt_Bias , & rtP . coswt_Freq , &
rtP . coswt_Phase , & rtP . sinwt_Amp , & rtP . sinwt_Bias , & rtP .
sinwt_Freq , & rtP . sinwt_Phase , & rtP . RadDeg_Gain_mu3nul0msz , & rtP .
coswt_Amp_mejccdpyws , & rtP . coswt_Bias_acdrihp4mg , & rtP .
coswt_Freq_dexl0rdjce , & rtP . coswt_Phase_gsew12la0i , & rtP .
sinwt_Amp_ggpyeyy0ei , & rtP . sinwt_Bias_aiqrlpqukr , & rtP .
sinwt_Freq_a0silddums , & rtP . sinwt_Phase_pauodsxphx , & rtP .
Vkd0Vkq10Vkq20_Value [ 0 ] , & rtP . N_Gain , & rtP . nominalspeed_Value , &
rtP . Rotorangledthetae_gainval , & rtP . Rotorangledthetae_IC , & rtP .
Rotorspeeddeviationdw_gainval , & rtP . Rotorspeeddeviationdw_IC , & rtP .
theta_gainval , & rtP . theta_IC , & rtP . u2H_Gain , & rtP . u_Pb_Gain , &
rtP . F_Gain , & rtP . t_Gain , & rtP . units_Gain , & rtP . units1_Gain , &
rtP . units2_Gain , & rtP . webase_Gain , & rtP . web2_Gain , & rtP .
web3_Gain , & rtP . SineWaveA_Amp , & rtP . SineWaveA_Bias , & rtP .
SineWaveA_Hsin , & rtP . SineWaveA_HCos , & rtP . SineWaveA_PSin , & rtP .
SineWaveA_PCos , & rtP . SineWaveB_Amp , & rtP . SineWaveB_Bias , & rtP .
SineWaveB_Hsin , & rtP . SineWaveB_HCos , & rtP . SineWaveB_PSin , & rtP .
SineWaveB_PCos , & rtP . SineWaveC_Amp , & rtP . SineWaveC_Bias , & rtP .
SineWaveC_Hsin , & rtP . SineWaveC_HCos , & rtP . SineWaveC_PSin , & rtP .
SineWaveC_PCos , & rtP . SwitchCurrents_Value [ 0 ] , & rtP . C4_Value , &
rtP . Switch3_Threshold_k0uagz5gcg , & rtP . C4_Value_fxleokhe0y , & rtP .
Switch3_Threshold_icm4fmynnh , & rtP . C4_Value_eoqp0wijkf , & rtP .
Switch3_Threshold_hdegddai5u , & rtP . LookUpTable_XData_i4mu3ctko4 [ 0 ] , &
rtP . LookUpTable_YData_epertdpbpx [ 0 ] , & rtP .
donotdeletethisgain_Gain_jxggkyb4oj , & rtP .
donotdeletethisgain_Gain_p0laf5fkqn , & rtP .
donotdeletethisgain_Gain_j5mfsbj1ew , & rtP . donotdeletethisgain_Gain , &
rtP . donotdeletethisgain_Gain_mrsxwkh4tp , & rtP .
donotdeletethisgain_Gain_lbvwbky1pa , & rtP .
donotdeletethisgain_Gain_g4a3i3bkvy , & rtP .
donotdeletethisgain_Gain_p2gxvtukfv , & rtP .
donotdeletethisgain_Gain_is2ce52gq3 , & rtP .
donotdeletethisgain_Gain_h5sheo5agc , & rtP .
donotdeletethisgain_Gain_idjvkwlnjp , & rtP .
donotdeletethisgain_Gain_iiwqs0rjpv , & rtP .
donotdeletethisgain_Gain_jc3l43jy02 , & rtP .
donotdeletethisgain_Gain_ixohazj2yz , & rtP .
donotdeletethisgain_Gain_ijcgmen2vo , & rtP .
donotdeletethisgain_Gain_dftutozx3l , & rtP .
donotdeletethisgain_Gain_dvccqwmfov , & rtP .
donotdeletethisgain_Gain_b3iqzvyj5x , & rtP .
donotdeletethisgain_Gain_ckgop4gksi , & rtP .
donotdeletethisgain_Gain_gex5e0120e , & rtP .
donotdeletethisgain_Gain_mnyedn5jj1 , & rtP .
donotdeletethisgain_Gain_nsmmc33j4l , & rtP .
donotdeletethisgain_Gain_oc13lni3fq , & rtP .
donotdeletethisgain_Gain_ghrwzwq2wl , & rtP .
donotdeletethisgain_Gain_inxi5cfmng , & rtP .
donotdeletethisgain_Gain_enynbgjy2q , & rtP .
donotdeletethisgain_Gain_ghgnnpri0h , & rtP .
donotdeletethisgain_Gain_fdstflfpdl , & rtP .
donotdeletethisgain_Gain_hwxarrm2xf , & rtP .
donotdeletethisgain_Gain_jpsgb554cq , & rtP .
donotdeletethisgain_Gain_akjxwy4mzh , & rtP .
donotdeletethisgain_Gain_m1o3yt3pmb , & rtP .
donotdeletethisgain_Gain_bdqv4z3h20 , & rtP .
donotdeletethisgain_Gain_dyxa4y4pb4 , & rtP .
donotdeletethisgain_Gain_kgtmebmbqg , & rtP .
donotdeletethisgain_Gain_pn4mhzapma , & rtP .
donotdeletethisgain_Gain_lb32xlqutz , & rtP .
donotdeletethisgain_Gain_e1t00s2jpf , & rtP .
donotdeletethisgain_Gain_kznkprn5og , & rtP .
donotdeletethisgain_Gain_krlg4qa22u , & rtP .
donotdeletethisgain_Gain_hxax2d03xl , & rtP .
donotdeletethisgain_Gain_fgel1ns2zy , & rtP . C4_Value_gbl0dsqzhj , & rtP .
Switch3_Threshold_g44ubd0yky , & rtP . C4_Value_exqcfpjqxr , & rtP .
Switch3_Threshold_c15c5kf1pp , & rtP . C4_Value_ktcw3s24rj , & rtP .
Switch3_Threshold_c2ubqk250k , & rtP . LookUpTable_XData_bysejxwrcs [ 0 ] , &
rtP . LookUpTable_YData_anwymjpndi [ 0 ] , & rtP . Constant1_Value_nw4shdp34n
, & rtP . Constant2_Value , & rtP . outputformatting_Gain [ 0 ] , & rtP .
Constant1_Value_iglewzfyey , & rtP . Constant2_Value_a35hujaypn , & rtP .
Constant3_Value , & rtP . Constant4_Value [ 0 ] , & rtP . Constant5_Value , &
rtP . Constant6_Value [ 0 ] , & rtP . Constant8_Value , & rtP .
Laqd_nosat_Value [ 0 ] , & rtP . Lmqd_nosat_Value [ 0 ] , & rtP .
changeIqIdcurrentsigns_Gain [ 0 ] , & rtP . Switch_Threshold_pea0kljvkj , &
rtP . Switch1_Threshold_aikum5bfzk , & rtP . Switch2_Threshold_auuawawddv , &
rtP . Switch3_Threshold , & rtP . u_Vb_Gain , & rtP . ib_Gain , & rtP .
F2_Gain , & rtP . dw_delay_InitialCondition , & rtP .
dw_predict_InitialCondition , & rtP . K1_Value_mjkioortvy , & rtP . Gain_Gain
, & rtP . integrator_IC_kbu50edsmc , & rtP .
Memory_InitialCondition_e0ydmt0d1l , & rtP . TransportDelay_Delay_gkek32v2fo
, & rtP . TransportDelay_InitOutput_gepskaykr5 , & rtP . K1_Value , & rtP .
Gain_Gain_f0nj4ejddp , & rtP . integrator_IC , & rtP .
Memory_InitialCondition , & rtP . TransportDelay_Delay , & rtP .
TransportDelay_InitOutput , & rtP . K1_Value_pj1iwyknqz , & rtP .
Gain_Gain_ksjae44pgn , & rtP . integrator_IC_lxfy54g0cz , & rtP .
Memory_InitialCondition_m5lga3j4fd , & rtP . TransportDelay_Delay_bifxyqg4be
, & rtP . TransportDelay_InitOutput_kfzzmtnzlj , & rtP . K1_Value_k2kyb4ovnj
, & rtP . Gain_Gain_kn1p3uhx5m , & rtP . integrator_IC_j24moklbcm , & rtP .
Memory_InitialCondition_l14qxrwnwp , & rtP . TransportDelay_Delay_mvxoy0gg0s
, & rtP . TransportDelay_InitOutput_a33cxqs542 , & rtP . LookUpTable_XData [
0 ] , & rtP . LookUpTable_YData [ 0 ] , & rtP . LookUpTable_XData_ouijhmudsj
[ 0 ] , & rtP . LookUpTable_YData_ibczk2yoa0 [ 0 ] , & rtP .
LookUpTable_XData_j5m2nusfgx [ 0 ] , & rtP . LookUpTable_YData_esrnrccjov [ 0
] , & rtP . LookUpTable_XData_fglubeidif [ 0 ] , & rtP .
LookUpTable_YData_hhayxsnnev [ 0 ] , & rtP . LookUpTable_XData_o0gg4yk3ay [ 0
] , & rtP . LookUpTable_YData_dujf1r2cza [ 0 ] , & rtP .
LookUpTable_XData_pxfayshagn [ 0 ] , & rtP . LookUpTable_YData_cvxjcdpbmq [ 0
] , & rtP . Gain_Gain_pycxugwvd0 , & rtP . Gain1_Gain_j0bqxbk0lz , & rtP .
u1_Gain [ 0 ] , & rtP . u1_Value_cbuqdiipgi [ 0 ] , & rtP . Gain1_Gain , &
rtP . uLl_d_Gain_mutaj0uj3h [ 0 ] , & rtP . uLl_q_Gain_nj2ycls33u [ 0 ] , &
rtP . Linv_Y0 , & rtP . RLinv_Y0 , & rtP . Lmqd_Y0 , & rtP . Laqd_Y0 , & rtP
. Constant1_Value_afrrlaacip , & rtP . Constant2_Value_lxtjw1dx3o , & rtP .
Constant3_Value_jspriozsq0 , & rtP . u1_Value_pqt3qesf2j [ 0 ] , & rtP .
u2_Value_kee1ps1dzy , & rtP . u3_Value , & rtP . IC_Threshold , & rtP .
fluxes_InitialCondition [ 0 ] , & rtP . voltages_InitialCondition , & rtP .
Gain_Gain_olfnewahm1 , & rtP . Constant1_Value_csdxpxx0tx , & rtP . Lmd_Gain
, & rtP . uDLookupTable_tableData_jcoemdiu0h [ 0 ] , & rtP .
uDLookupTable_bp01Data_h0gmvbj4n1 [ 0 ] , & rtP . Lmd_sat_InitialCondition ,
& rtP . Lmq_sat_Y0 , & rtP . Laq_Y0 , & rtP . Constant1_Value , & rtP .
Lmq_Gain , & rtP . uDLookupTable_tableData [ 0 ] , & rtP .
uDLookupTable_bp01Data [ 0 ] , & rtP . Lmq_sat_InitialCondition , & rtP .
u1_Value_oit41rg0le [ 0 ] , & rtP . u5_Value [ 0 ] , & rtP .
u5_Value_ochldviepj [ 0 ] , & rtP . wbaseTs2_Gain , & rtP .
wbaseTs2_Gain_agkgmtch40 , & rtP . u1_Value [ 0 ] , & rtP . uLl_d_Gain [ 0 ]
, & rtP . u2_Value [ 0 ] , & rtP . uLl_q_Gain [ 0 ] , } ; static int32_T *
rtVarDimsAddrMap [ ] = { ( NULL ) } ;
#endif
static TARGET_CONST rtwCAPI_DataTypeMap rtDataTypeMap [ ] = { { "double" ,
"real_T" , 0 , 0 , sizeof ( real_T ) , SS_DOUBLE , 0 , 0 , 0 } , {
"unsigned char" , "boolean_T" , 0 , 0 , sizeof ( boolean_T ) , SS_BOOLEAN , 0
, 0 , 0 } , { "struct" , "creal_T" , 0 , 0 , sizeof ( creal_T ) , SS_DOUBLE ,
1 , 0 , 0 } } ;
#ifdef HOST_CAPI_BUILD
#undef sizeof
#endif
static TARGET_CONST rtwCAPI_ElementMap rtElementMap [ ] = { { ( NULL ) , 0 ,
0 , 0 , 0 } , } ; static const rtwCAPI_DimensionMap rtDimensionMap [ ] = { {
rtwCAPI_SCALAR , 0 , 2 , 0 } , { rtwCAPI_VECTOR , 2 , 2 , 0 } , {
rtwCAPI_VECTOR , 4 , 2 , 0 } , { rtwCAPI_VECTOR , 6 , 2 , 0 } , {
rtwCAPI_VECTOR , 8 , 2 , 0 } , { rtwCAPI_VECTOR , 10 , 2 , 0 } , {
rtwCAPI_MATRIX_COL_MAJOR , 12 , 2 , 0 } , { rtwCAPI_MATRIX_COL_MAJOR , 14 , 2
, 0 } , { rtwCAPI_MATRIX_COL_MAJOR , 16 , 2 , 0 } , {
rtwCAPI_MATRIX_COL_MAJOR , 18 , 2 , 0 } , { rtwCAPI_VECTOR , 20 , 2 , 0 } , {
rtwCAPI_VECTOR , 22 , 2 , 0 } , { rtwCAPI_VECTOR , 24 , 2 , 0 } , {
rtwCAPI_VECTOR , 26 , 2 , 0 } , { rtwCAPI_VECTOR , 28 , 2 , 0 } , {
rtwCAPI_VECTOR , 30 , 2 , 0 } } ; static const uint_T rtDimensionArray [ ] =
{ 1 , 1 , 3 , 1 , 50 , 1 , 6 , 1 , 2 , 1 , 18 , 1 , 6 , 6 , 6 , 11 , 50 , 6 ,
50 , 11 , 1 , 3 , 1 , 6 , 1 , 18 , 1 , 2 , 1 , 5 , 1 , 10 } ; static const
real_T rtcapiStoredFloats [ ] = { 0.0 , 5.0E-5 , 1.0E-5 } ; static const
rtwCAPI_FixPtMap rtFixPtMap [ ] = { { ( NULL ) , ( NULL ) ,
rtwCAPI_FIX_RESERVED , 0 , 0 , 0 } , } ; static const rtwCAPI_SampleTimeMap
rtSampleTimeMap [ ] = { { ( const void * ) & rtcapiStoredFloats [ 0 ] , (
const void * ) & rtcapiStoredFloats [ 0 ] , 0 , 0 } , { ( const void * ) &
rtcapiStoredFloats [ 1 ] , ( const void * ) & rtcapiStoredFloats [ 0 ] , 2 ,
0 } , { ( NULL ) , ( NULL ) , 3 , 0 } , { ( const void * ) &
rtcapiStoredFloats [ 2 ] , ( const void * ) & rtcapiStoredFloats [ 0 ] , 1 ,
0 } } ; static rtwCAPI_ModelMappingStaticInfo mmiStatic = { { rtBlockSignals
, 289 , ( NULL ) , 0 , ( NULL ) , 0 } , { rtBlockParameters , 261 ,
rtModelParameters , 0 } , { ( NULL ) , 0 } , { rtDataTypeMap , rtDimensionMap
, rtFixPtMap , rtElementMap , rtSampleTimeMap , rtDimensionArray } , "float"
, { 749349454U , 2771954547U , 2147498699U , 4272243878U } , ( NULL ) , 0 , 0
} ; const rtwCAPI_ModelMappingStaticInfo * DZG_v6_SimGen_GetCAPIStaticMap (
void ) { return & mmiStatic ; }
#ifndef HOST_CAPI_BUILD
void DZG_v6_SimGen_InitializeDataMapInfo ( void ) { rtwCAPI_SetVersion ( ( *
rt_dataMapInfoPtr ) . mmi , 1 ) ; rtwCAPI_SetStaticMap ( ( *
rt_dataMapInfoPtr ) . mmi , & mmiStatic ) ; rtwCAPI_SetLoggingStaticMap ( ( *
rt_dataMapInfoPtr ) . mmi , ( NULL ) ) ; rtwCAPI_SetDataAddressMap ( ( *
rt_dataMapInfoPtr ) . mmi , rtDataAddrMap ) ; rtwCAPI_SetVarDimsAddressMap (
( * rt_dataMapInfoPtr ) . mmi , rtVarDimsAddrMap ) ;
rtwCAPI_SetInstanceLoggingInfo ( ( * rt_dataMapInfoPtr ) . mmi , ( NULL ) ) ;
rtwCAPI_SetChildMMIArray ( ( * rt_dataMapInfoPtr ) . mmi , ( NULL ) ) ;
rtwCAPI_SetChildMMIArrayLen ( ( * rt_dataMapInfoPtr ) . mmi , 0 ) ; }
#else
#ifdef __cplusplus
extern "C" {
#endif
void DZG_v6_SimGen_host_InitializeDataMapInfo (
DZG_v6_SimGen_host_DataMapInfo_T * dataMap , const char * path ) {
rtwCAPI_SetVersion ( dataMap -> mmi , 1 ) ; rtwCAPI_SetStaticMap ( dataMap ->
mmi , & mmiStatic ) ; rtwCAPI_SetDataAddressMap ( dataMap -> mmi , NULL ) ;
rtwCAPI_SetVarDimsAddressMap ( dataMap -> mmi , NULL ) ; rtwCAPI_SetPath (
dataMap -> mmi , path ) ; rtwCAPI_SetFullPath ( dataMap -> mmi , NULL ) ;
rtwCAPI_SetChildMMIArray ( dataMap -> mmi , ( NULL ) ) ;
rtwCAPI_SetChildMMIArrayLen ( dataMap -> mmi , 0 ) ; }
#ifdef __cplusplus
}
#endif
#endif
