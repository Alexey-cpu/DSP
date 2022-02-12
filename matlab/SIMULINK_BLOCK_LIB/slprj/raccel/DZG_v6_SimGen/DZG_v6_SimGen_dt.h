#include "ext_types.h"
static DataTypeInfo rtDataTypeInfoTable [ ] = { { "real_T" , 0 , 8 } , {
"real32_T" , 1 , 4 } , { "int8_T" , 2 , 1 } , { "uint8_T" , 3 , 1 } , {
"int16_T" , 4 , 2 } , { "uint16_T" , 5 , 2 } , { "int32_T" , 6 , 4 } , {
"uint32_T" , 7 , 4 } , { "boolean_T" , 8 , 1 } , { "fcn_call_T" , 9 , 0 } , {
"int_T" , 10 , 4 } , { "pointer_T" , 11 , 8 } , { "action_T" , 12 , 8 } , {
"timer_uint32_pair_T" , 13 , 8 } , { "struct_nxjvkMAWNMdcRzZZdKzTgC" , 14 ,
40 } , { "struct_Hj9CrByDQJdvNo4enmKmZB" , 15 , 24 } , {
"struct_7U2Xy3czgmwuE6XJcblrBG" , 16 , 16 } , {
"struct_owIlFggqKR9lpxBINQsa2B" , 17 , 144 } , {
"struct_pp0NSkH3FV4HpWNYbqxM3D" , 18 , 8008 } } ; static uint_T
rtDataTypeSizes [ ] = { sizeof ( real_T ) , sizeof ( real32_T ) , sizeof (
int8_T ) , sizeof ( uint8_T ) , sizeof ( int16_T ) , sizeof ( uint16_T ) ,
sizeof ( int32_T ) , sizeof ( uint32_T ) , sizeof ( boolean_T ) , sizeof (
fcn_call_T ) , sizeof ( int_T ) , sizeof ( pointer_T ) , sizeof ( action_T )
, 2 * sizeof ( uint32_T ) , sizeof ( struct_nxjvkMAWNMdcRzZZdKzTgC ) , sizeof
( struct_Hj9CrByDQJdvNo4enmKmZB ) , sizeof ( struct_7U2Xy3czgmwuE6XJcblrBG )
, sizeof ( struct_owIlFggqKR9lpxBINQsa2B ) , sizeof (
struct_pp0NSkH3FV4HpWNYbqxM3D ) } ; static const char_T * rtDataTypeNames [ ]
= { "real_T" , "real32_T" , "int8_T" , "uint8_T" , "int16_T" , "uint16_T" ,
"int32_T" , "uint32_T" , "boolean_T" , "fcn_call_T" , "int_T" , "pointer_T" ,
"action_T" , "timer_uint32_pair_T" , "struct_nxjvkMAWNMdcRzZZdKzTgC" ,
"struct_Hj9CrByDQJdvNo4enmKmZB" , "struct_7U2Xy3czgmwuE6XJcblrBG" ,
"struct_owIlFggqKR9lpxBINQsa2B" , "struct_pp0NSkH3FV4HpWNYbqxM3D" } ; static
DataTypeTransition rtBTransitions [ ] = { { ( char_T * ) ( & rtB . jzulpokwia
. re ) , 0 , 1 , 4 } , { ( char_T * ) ( & rtB . o0laqbijxj ) , 0 , 0 , 1015 }
, { ( char_T * ) ( & rtB . lvq5fe4dss ) , 8 , 0 , 11 } , { ( char_T * ) ( &
rtDW . hsrejdggk0 ) , 0 , 0 , 184 } , { ( char_T * ) ( & rtDW . oh3rj3rgch .
AS ) , 11 , 0 , 17 } , { ( char_T * ) ( & rtDW . mljcxhdcts ) , 6 , 0 , 9 } ,
{ ( char_T * ) ( & rtDW . lrbh114wtw ) , 7 , 0 , 2 } , { ( char_T * ) ( &
rtDW . lkcbxptdv5 [ 0 ] ) , 10 , 0 , 15 } , { ( char_T * ) ( & rtDW .
m11yduvrgb ) , 2 , 0 , 3 } , { ( char_T * ) ( & rtDW . lkdnaytcjn ) , 3 , 0 ,
1 } } ; static DataTypeTransitionTable rtBTransTable = { 10U , rtBTransitions
} ; static DataTypeTransition rtPTransitions [ ] = { { ( char_T * ) ( & rtP .
KZTest_FaultA ) , 0 , 0 , 1607 } , { ( char_T * ) ( & rtP .
Constant1_Value_afrrlaacip ) , 8 , 0 , 4 } } ; static DataTypeTransitionTable
rtPTransTable = { 2U , rtPTransitions } ;
