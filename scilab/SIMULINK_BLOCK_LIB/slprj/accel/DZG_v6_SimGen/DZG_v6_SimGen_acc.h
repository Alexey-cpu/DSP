#ifndef RTW_HEADER_DZG_v6_SimGen_acc_h_
#define RTW_HEADER_DZG_v6_SimGen_acc_h_
#include <stddef.h>
#include <string.h>
#include <float.h>
#ifndef DZG_v6_SimGen_acc_COMMON_INCLUDES_
#define DZG_v6_SimGen_acc_COMMON_INCLUDES_
#include <stdlib.h>
#define S_FUNCTION_NAME simulink_only_sfcn 
#define S_FUNCTION_LEVEL 2
#define RTW_GENERATED_S_FUNCTION
#include "rtwtypes.h"
#include "simstruc.h"
#include "fixedpoint.h"
#endif
#include "DZG_v6_SimGen_acc_types.h"
#include "multiword_types.h"
#include "mwmathutil.h"
#include "rt_look.h"
#include "rt_look1d.h"
#include "rt_defines.h"
#include "rt_nonfinite.h"
typedef struct { creal_T B_16_0_0 [ 3 ] ; creal_T B_16_1_0 ; creal_T B_16_2_0
; } B_NegSeqComputation_DZG_v6_SimGen_T ; typedef struct { int32_T
NegSeqComputation_sysIdxToRun ; int8_T NegSeqComputation_SubsysRanBC ;
boolean_T NegSeqComputation_MODE ; char_T pad_NegSeqComputation_MODE [ 2 ] ;
} DW_NegSeqComputation_DZG_v6_SimGen_T ; typedef struct { creal_T B_18_0_0 ;
creal_T B_18_1_0 ; } B_ZeroSeqComputation_DZG_v6_SimGen_T ; typedef struct {
int32_T ZeroSeqComputation_sysIdxToRun ; int8_T
ZeroSeqComputation_SubsysRanBC ; boolean_T ZeroSeqComputation_MODE ; char_T
pad_ZeroSeqComputation_MODE [ 2 ] ; } DW_ZeroSeqComputation_DZG_v6_SimGen_T ;
typedef struct { real_T B_39_0_0 ; real_T B_39_1_0 ; }
B_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T ; typedef struct {
int32_T Transferfunctionsfordetailedstabilizer_sysIdxToRun ; int8_T
Transferfunctionsfordetailedstabilizer_SubsysRanBC ; char_T
pad_Transferfunctionsfordetailedstabilizer_SubsysRanBC [ 3 ] ; }
DW_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T ; typedef struct {
creal_T B_45_36_0 ; creal_T B_45_55_0 ; creal_T B_45_98_0 ; creal_T
B_45_102_0 ; creal_T B_45_119_0 ; creal_T B_45_123_0 ; creal_T B_45_140_0 ;
creal_T B_45_144_0 ; creal_T B_45_168_0 ; creal_T B_45_172_0 ; creal_T
B_45_189_0 ; creal_T B_45_193_0 ; creal_T B_45_210_0 ; creal_T B_45_214_0 ;
real_T B_45_0_0 ; real_T B_45_1_0 ; real_T B_45_2_0 ; real_T B_45_3_0 ;
real_T B_45_4_0 ; real_T B_45_4_1 ; real_T B_45_5_0 [ 6 ] ; real_T B_45_6_0 ;
real_T B_45_8_0 [ 36 ] ; real_T B_45_9_0 [ 6 ] ; real_T B_45_10_0 [ 6 ] ;
real_T B_45_11_0 ; real_T B_45_12_0 ; real_T B_45_13_0 ; real_T B_45_14_0 [ 3
] ; real_T B_45_15_0 [ 2 ] ; real_T B_45_16_0 ; real_T B_45_17_0 ; real_T
B_45_18_0 ; real_T B_45_19_0 [ 50 ] ; real_T B_45_19_1 [ 6 ] ; real_T
B_45_20_0 ; real_T B_45_21_0 ; real_T B_45_22_0 ; real_T B_45_23_0 ; real_T
B_45_25_0 ; real_T B_45_27_0 ; real_T B_45_28_0 ; real_T B_45_29_0 ; real_T
B_45_30_0 ; real_T B_45_31_0 ; real_T B_45_33_0 ; real_T B_45_35_0 ; real_T
B_45_37_0 ; real_T B_45_37_1 ; real_T B_45_38_0 ; real_T B_45_39_0 ; real_T
B_45_40_0 ; real_T B_45_41_0 ; real_T B_45_42_0 ; real_T B_45_44_0 ; real_T
B_45_46_0 ; real_T B_45_47_0 ; real_T B_45_48_0 ; real_T B_45_49_0 ; real_T
B_45_50_0 ; real_T B_45_52_0 ; real_T B_45_54_0 ; real_T B_45_56_0 ; real_T
B_45_56_1 ; real_T B_45_57_0 ; real_T B_45_58_0 ; real_T B_45_59_0 ; real_T
B_45_60_0 ; real_T B_45_61_0 ; real_T B_45_62_0 ; real_T B_45_63_0 ; real_T
B_45_64_0 ; real_T B_45_65_0 ; real_T B_45_66_0 ; real_T B_45_67_0 ; real_T
B_45_71_0 ; real_T B_45_72_0 ; real_T B_45_73_0 ; real_T B_45_74_0 [ 3 ] ;
real_T B_45_75_0 ; real_T B_45_76_0 ; real_T B_45_77_0 ; real_T B_45_78_0 [ 3
] ; real_T B_45_82_0 ; real_T B_45_83_0 ; real_T B_45_84_0 ; real_T B_45_85_0
; real_T B_45_87_0 ; real_T B_45_89_0 ; real_T B_45_90_0 ; real_T B_45_91_0 ;
real_T B_45_92_0 ; real_T B_45_93_0 ; real_T B_45_95_0 ; real_T B_45_97_0 ;
real_T B_45_99_0 ; real_T B_45_99_1 ; real_T B_45_100_0 ; real_T B_45_101_0 ;
real_T B_45_103_0 ; real_T B_45_104_0 ; real_T B_45_105_0 ; real_T B_45_106_0
; real_T B_45_108_0 ; real_T B_45_110_0 ; real_T B_45_111_0 ; real_T
B_45_112_0 ; real_T B_45_113_0 ; real_T B_45_114_0 ; real_T B_45_116_0 ;
real_T B_45_118_0 ; real_T B_45_120_0 ; real_T B_45_120_1 ; real_T B_45_121_0
; real_T B_45_122_0 ; real_T B_45_124_0 ; real_T B_45_125_0 ; real_T
B_45_126_0 ; real_T B_45_127_0 ; real_T B_45_129_0 ; real_T B_45_131_0 ;
real_T B_45_132_0 ; real_T B_45_133_0 ; real_T B_45_134_0 ; real_T B_45_135_0
; real_T B_45_137_0 ; real_T B_45_139_0 ; real_T B_45_141_0 ; real_T
B_45_141_1 ; real_T B_45_142_0 ; real_T B_45_143_0 ; real_T B_45_145_0 ;
real_T B_45_147_0 ; real_T B_45_149_0 ; real_T B_45_151_0 [ 3 ] ; real_T
B_45_151_1 [ 3 ] ; real_T B_45_152_0 ; real_T B_45_153_0 ; real_T B_45_154_0
; real_T B_45_155_0 ; real_T B_45_157_0 ; real_T B_45_159_0 ; real_T
B_45_160_0 ; real_T B_45_161_0 ; real_T B_45_162_0 ; real_T B_45_163_0 ;
real_T B_45_165_0 ; real_T B_45_167_0 ; real_T B_45_169_0 ; real_T B_45_169_1
; real_T B_45_170_0 ; real_T B_45_171_0 ; real_T B_45_173_0 ; real_T
B_45_174_0 ; real_T B_45_175_0 ; real_T B_45_176_0 ; real_T B_45_178_0 ;
real_T B_45_180_0 ; real_T B_45_181_0 ; real_T B_45_182_0 ; real_T B_45_183_0
; real_T B_45_184_0 ; real_T B_45_186_0 ; real_T B_45_188_0 ; real_T
B_45_190_0 ; real_T B_45_190_1 ; real_T B_45_191_0 ; real_T B_45_192_0 ;
real_T B_45_194_0 ; real_T B_45_195_0 ; real_T B_45_196_0 ; real_T B_45_197_0
; real_T B_45_199_0 ; real_T B_45_201_0 ; real_T B_45_202_0 ; real_T
B_45_203_0 ; real_T B_45_204_0 ; real_T B_45_205_0 ; real_T B_45_207_0 ;
real_T B_45_209_0 ; real_T B_45_211_0 ; real_T B_45_211_1 ; real_T B_45_212_0
; real_T B_45_213_0 ; real_T B_45_215_0 ; real_T B_45_217_0 ; real_T
B_45_219_0 ; real_T B_45_221_0 [ 3 ] ; real_T B_45_221_1 [ 3 ] ; real_T
B_45_222_0 ; real_T B_45_223_0 ; real_T B_45_224_0 ; real_T B_45_225_0 ;
real_T B_45_226_0 ; real_T B_45_227_0 ; real_T B_45_228_0 ; real_T B_45_228_1
; real_T B_45_229_0 ; real_T B_45_232_0 ; real_T B_45_237_0 ; real_T
B_45_238_0 ; real_T B_45_239_0 ; real_T B_45_240_0 ; real_T B_45_241_0 ;
real_T B_45_242_0 ; real_T B_45_243_0 ; real_T B_45_244_0 ; real_T B_45_245_0
; real_T B_45_246_0 ; real_T B_45_247_0 ; real_T B_45_248_0 ; real_T
B_45_249_0 ; real_T B_45_250_0 ; real_T B_45_251_0 ; real_T B_45_252_0 ;
real_T B_45_253_0 ; real_T B_45_254_0 ; real_T B_45_255_0 ; real_T B_45_256_0
; real_T B_45_257_0 ; real_T B_45_258_0 ; real_T B_45_259_0 ; real_T
B_45_260_0 ; real_T B_45_261_0 ; real_T B_45_262_0 ; real_T B_45_263_0 ;
real_T B_45_264_0 ; real_T B_45_265_0 ; real_T B_45_266_0 ; real_T B_45_267_0
; real_T B_45_268_0 ; real_T B_45_269_0 ; real_T B_45_270_0 [ 3 ] ; real_T
B_45_271_0 [ 2 ] ; real_T B_45_272_0 [ 3 ] ; real_T B_45_273_0 ; real_T
B_45_274_0 [ 3 ] ; real_T B_45_275_0 [ 3 ] ; real_T B_45_276_0 ; real_T
B_45_277_0 [ 2 ] ; real_T B_45_278_0 [ 2 ] ; real_T B_45_279_0 ; real_T
B_45_280_0 ; real_T B_45_281_0 [ 2 ] ; real_T B_45_282_0 ; real_T B_45_283_0
; real_T B_45_284_0 ; real_T B_45_285_0 ; real_T B_45_286_0 ; real_T
B_45_287_0 ; real_T B_45_288_0 ; real_T B_45_289_0 ; real_T B_45_290_0 ;
real_T B_45_291_0 ; real_T B_45_292_0 [ 18 ] ; real_T B_45_293_0 [ 18 ] ;
real_T B_45_294_0 ; real_T B_45_297_0 ; real_T B_45_299_0 ; real_T B_45_301_0
; real_T B_45_302_0 [ 2 ] ; real_T B_45_303_0 [ 2 ] ; real_T B_45_304_0 ;
real_T B_45_305_0 ; real_T B_45_306_0 ; real_T B_45_307_0 ; real_T B_45_308_0
; real_T B_45_309_0 ; real_T B_45_310_0 [ 6 ] ; real_T B_45_311_0 ; real_T
B_45_313_0 [ 6 ] ; real_T B_45_314_0 ; real_T B_45_315_0 ; real_T B_45_316_0
; real_T B_45_317_0 ; real_T B_45_318_0 ; real_T B_45_319_0 ; real_T
B_45_320_0 ; real_T B_45_321_0 ; real_T B_45_322_0 ; real_T B_45_323_0 ;
real_T B_45_324_0 ; real_T B_45_325_0 ; real_T B_45_326_0 ; real_T B_45_327_0
[ 6 ] ; real_T B_45_328_0 ; real_T B_45_329_0 ; real_T B_45_330_0 ; real_T
B_45_336_0 ; real_T B_45_337_0 ; real_T B_45_338_0 ; real_T B_45_339_0 ;
real_T B_45_340_0 ; real_T B_45_341_0 ; real_T B_45_342_0 ; real_T B_45_343_0
; real_T B_45_344_0 ; real_T B_45_345_0 ; real_T B_45_346_0 ; real_T
B_45_347_0 ; real_T B_45_348_0 ; real_T B_45_349_0 ; real_T B_45_350_0 ;
real_T B_45_352_0 ; real_T B_45_353_0 ; real_T B_45_355_0 ; real_T B_45_356_0
; real_T B_45_357_0 ; real_T B_45_358_0 ; real_T B_45_360_0 ; real_T
B_45_361_0 ; real_T B_45_363_0 ; real_T B_45_364_0 ; real_T B_45_365_0 ;
real_T B_45_366_0 ; real_T B_45_368_0 ; real_T B_45_369_0 ; real_T B_45_371_0
; real_T B_45_372_0 ; real_T B_45_373_0 ; real_T B_45_374_0 ; real_T
B_45_375_0 ; real_T B_45_376_0 ; real_T B_45_377_0 ; real_T B_45_378_0 ;
real_T B_45_379_0 ; real_T B_45_380_0 ; real_T B_45_381_0 ; real_T B_45_382_0
; real_T B_45_383_0 ; real_T B_45_384_0 ; real_T B_45_385_0 ; real_T
B_45_386_0 ; real_T B_45_387_0 ; real_T B_45_388_0 ; real_T B_45_390_0 ;
real_T B_45_391_0 ; real_T B_45_394_0 ; real_T B_45_395_0 ; real_T B_45_397_0
; real_T B_45_398_0 ; real_T B_45_400_0 ; real_T B_45_401_0 ; real_T
B_45_402_0 ; real_T B_45_403_0 ; real_T B_45_404_0 ; real_T B_45_405_0 ;
real_T B_45_406_0 ; real_T B_45_407_0 ; real_T B_45_408_0 ; real_T B_45_409_0
; real_T B_45_410_0 ; real_T B_45_411_0 ; real_T B_45_412_0 ; real_T
B_45_413_0 ; real_T B_45_414_0 ; real_T B_45_415_0 ; real_T B_45_416_0 ;
real_T B_45_417_0 ; real_T B_45_418_0 ; real_T B_45_419_0 ; real_T B_45_420_0
; real_T B_45_421_0 ; real_T B_45_422_0 ; real_T B_45_423_0 ; real_T
B_45_424_0 ; real_T B_45_425_0 ; real_T B_45_426_0 ; real_T B_45_427_0 ;
real_T B_45_428_0 ; real_T B_45_429_0 ; real_T B_45_430_0 ; real_T B_45_431_0
; real_T B_45_432_0 ; real_T B_45_433_0 ; real_T B_45_434_0 ; real_T
B_45_435_0 ; real_T B_45_436_0 ; real_T B_45_437_0 ; real_T B_45_438_0 ;
real_T B_45_439_0 ; real_T B_45_440_0 ; real_T B_45_441_0 ; real_T B_45_442_0
; real_T B_45_443_0 ; real_T B_45_444_0 ; real_T B_45_445_0 ; real_T
B_45_446_0 ; real_T B_45_447_0 ; real_T B_45_448_0 ; real_T B_45_449_0 ;
real_T B_45_450_0 ; real_T B_45_451_0 ; real_T B_45_452_0 ; real_T B_45_453_0
; real_T B_45_454_0 ; real_T B_45_455_0 ; real_T B_45_456_0 ; real_T
B_45_457_0 ; real_T B_45_458_0 ; real_T B_45_460_0 ; real_T B_45_463_0 ;
real_T B_45_467_0 ; real_T B_45_468_0 ; real_T B_45_471_0 ; real_T B_45_475_0
; real_T B_45_476_0 ; real_T B_45_479_0 ; real_T B_45_523_0 ; real_T
B_45_524_0 ; real_T B_45_525_0 ; real_T B_45_526_0 [ 3 ] ; real_T B_45_527_0
; real_T B_45_528_0 ; real_T B_45_529_0 ; real_T B_45_530_0 [ 3 ] ; real_T
B_45_565_0 ; real_T B_45_566_0 ; real_T B_45_567_0 ; real_T B_45_568_0 [ 3 ]
; real_T B_45_569_0 ; real_T B_45_570_0 ; real_T B_45_571_0 ; real_T
B_45_572_0 [ 3 ] ; real_T B_45_607_0 ; real_T B_45_608_0 ; real_T B_45_609_0
; real_T B_45_610_0 [ 3 ] ; real_T B_45_611_0 ; real_T B_45_612_0 ; real_T
B_45_613_0 ; real_T B_45_614_0 [ 3 ] ; real_T B_45_649_0 ; real_T B_45_650_0
; real_T B_45_651_0 ; real_T B_45_652_0 [ 3 ] ; real_T B_45_653_0 ; real_T
B_45_654_0 ; real_T B_45_655_0 ; real_T B_45_656_0 [ 3 ] ; real_T B_45_691_0
; real_T B_45_692_0 ; real_T B_45_693_0 ; real_T B_45_694_0 [ 3 ] ; real_T
B_45_695_0 ; real_T B_45_696_0 ; real_T B_45_697_0 ; real_T B_45_698_0 [ 3 ]
; real_T B_45_733_0 ; real_T B_45_734_0 ; real_T B_45_735_0 ; real_T
B_45_736_0 [ 3 ] ; real_T B_45_737_0 ; real_T B_45_738_0 ; real_T B_45_739_0
; real_T B_45_740_0 [ 3 ] ; real_T B_45_775_0 ; real_T B_45_776_0 ; real_T
B_45_777_0 ; real_T B_45_778_0 ; real_T B_45_779_0 ; real_T B_45_782_0 ;
real_T B_45_786_0 ; real_T B_45_787_0 ; real_T B_45_790_0 ; real_T B_45_794_0
; real_T B_45_795_0 ; real_T B_45_798_0 ; real_T B_45_0_0_m [ 6 ] ; real_T
B_45_1_0_c ; real_T B_45_2_0_k ; real_T B_45_3_0_c ; real_T B_45_4_0_b ;
real_T B_45_5_0_p ; real_T B_45_6_0_c ; real_T B_45_7_0 ; real_T B_45_10_0_f
; real_T B_45_11_0_g ; real_T B_45_12_0_g ; real_T B_45_13_0_m ; real_T
B_45_14_0_n ; real_T B_45_15_0_p ; real_T B_45_16_0_l [ 36 ] ; real_T
B_45_17_0_j ; real_T B_45_18_0_d [ 36 ] ; real_T B_45_19_0_g ; real_T
B_45_20_0_l [ 2 ] ; real_T B_45_21_0_d [ 2 ] ; real_T B_45_22_0_d [ 36 ] ;
real_T B_45_24_0 [ 36 ] ; real_T B_45_25_0_l [ 3 ] ; real_T B_45_26_0 ;
real_T B_45_27_0_o ; real_T B_45_28_0_b ; real_T B_45_29_0_n ; real_T
B_45_30_0_b ; real_T B_45_31_0_l ; real_T B_45_32_0 ; real_T B_45_33_0_h ;
real_T B_45_34_0 ; real_T B_45_35_0_b ; real_T B_45_37_0_d ; real_T
B_45_38_0_e ; real_T B_45_39_0_b ; real_T B_45_40_0_j ; real_T B_45_41_0_f ;
real_T B_45_42_0_a ; real_T B_45_43_0 ; real_T B_45_73_0_j ; real_T
B_45_74_0_j ; real_T B_45_75_0_o ; real_T B_45_76_0_n ; real_T B_45_80_0 ;
real_T B_45_81_0 ; real_T B_45_82_0_i ; real_T B_45_83_0_o ; real_T
B_45_84_0_n ; real_T B_45_85_0_m ; real_T B_45_86_0 ; real_T B_38_0_0 [ 6 ] ;
real_T B_38_1_0 [ 36 ] ; real_T B_38_2_0 [ 36 ] ; real_T B_38_3_0 ; real_T
B_38_4_0 [ 36 ] ; real_T B_38_5_0 [ 36 ] ; real_T B_38_6_0 [ 36 ] ; real_T
B_38_7_0 [ 36 ] ; real_T B_38_8_0 [ 36 ] ; real_T B_38_9_0 [ 36 ] ; real_T
B_38_10_0 [ 6 ] ; real_T B_38_11_0 [ 36 ] ; real_T B_38_12_0 [ 36 ] ; real_T
B_38_13_0 [ 6 ] ; real_T B_38_14_0 [ 6 ] ; real_T B_37_1_0 [ 3 ] ; real_T
B_37_2_0 ; real_T B_37_3_0 ; real_T B_37_4_0 ; real_T B_37_5_0 ; real_T
B_37_6_0 ; real_T B_37_7_0 ; real_T B_37_8_0 ; real_T B_37_9_0 ; real_T
B_37_11_0 ; real_T B_37_12_0 ; real_T B_37_14_0 ; real_T B_37_15_0 [ 36 ] ;
real_T B_37_16_0 [ 36 ] ; real_T B_37_17_0 [ 36 ] ; real_T B_37_18_0 [ 36 ] ;
real_T B_37_19_0 [ 36 ] ; real_T B_37_20_0 ; real_T B_37_3_0_c ; real_T
B_37_4_0_m [ 2 ] ; real_T B_37_6_0_m [ 36 ] ; real_T B_37_7_0_j [ 36 ] ;
real_T B_37_8_0_h [ 36 ] ; real_T B_37_9_0_c ; real_T B_37_10_0 ; real_T
B_36_0_0 [ 3 ] ; real_T B_36_1_0 ; real_T B_36_2_0 ; real_T B_36_3_0 ; real_T
B_36_4_0 ; real_T B_36_5_0 ; real_T B_36_6_0 ; real_T B_36_7_0 ; real_T
B_36_8_0 ; real_T B_36_10_0 ; real_T B_36_11_0 ; real_T B_36_0_0_c ; real_T
B_36_1_0_p [ 3 ] ; real_T B_35_0_0 ; real_T B_34_0_0 ; real_T B_33_0_0 ;
real_T B_33_1_0 ; real_T B_32_0_0 ; real_T B_31_0_0 ; real_T B_31_1_0 ;
real_T B_30_0_0 ; real_T B_29_0_0 ; real_T B_29_1_0 ; real_T B_28_0_0 ;
real_T B_24_0_0 ; real_T B_24_1_0 ; real_T B_23_0_0 ; real_T B_23_1_0 ;
real_T B_22_0_0 ; real_T B_22_1_0 ; real_T B_21_0_0 ; real_T B_21_1_0 ;
real_T B_20_0_0 ; real_T B_20_1_0 ; real_T B_19_0_0 ; real_T B_19_1_0 ;
real_T B_15_0_0 ; real_T B_15_1_0 ; real_T B_14_0_0 ; real_T B_14_1_0 ;
real_T B_13_0_0 ; real_T B_13_1_0 ; real_T B_12_0_0 ; real_T B_12_1_0 ;
real_T B_11_0_0 ; real_T B_11_1_0 ; real_T B_10_0_0 ; real_T B_10_1_0 ;
real_T B_9_1_0 ; real_T B_9_2_0 ; real_T B_8_0_0 ; real_T B_7_1_0 ; real_T
B_7_2_0 ; real_T B_6_0_0 ; real_T B_5_1_0 ; real_T B_5_2_0 ; real_T B_4_0_0 ;
real_T B_3_0_0 ; real_T B_3_1_0 ; real_T B_2_0_0 ; real_T B_2_1_0 ; real_T
B_1_0_0 ; real_T B_1_1_0 ; real_T B_0_0_0 ; real_T B_0_1_0 ; boolean_T
B_45_24_0_p ; boolean_T B_45_32_0_a ; boolean_T B_45_43_0_e ; boolean_T
B_45_51_0 ; boolean_T B_45_86_0_a ; boolean_T B_45_94_0 ; boolean_T
B_45_107_0 ; boolean_T B_45_115_0 ; boolean_T B_45_128_0 ; boolean_T
B_45_136_0 ; boolean_T B_45_156_0 ; boolean_T B_45_164_0 ; boolean_T
B_45_177_0 ; boolean_T B_45_185_0 ; boolean_T B_45_198_0 ; boolean_T
B_45_206_0 ; boolean_T B_45_389_0 ; boolean_T B_45_392_0 ; boolean_T
B_45_393_0 ; boolean_T B_45_396_0 ; boolean_T B_45_399_0 ; boolean_T
B_45_459_0 ; boolean_T B_45_9_0_a ; boolean_T B_45_36_0_i ; boolean_T
B_45_77_0_l ; boolean_T B_45_78_0_o ; boolean_T B_45_79_0 ; boolean_T
B_37_0_0 ; boolean_T B_37_0_0_o ; boolean_T B_37_1_0_i ; boolean_T B_37_2_0_f
; boolean_T B_9_0_0 ; boolean_T B_7_0_0 ; boolean_T B_5_0_0 ; char_T
pad_B_5_0_0 [ 6 ] ; B_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
Transferfunctionsfordetailedstabilizer1_g ;
B_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
Transferfunctionsfordetailedstabilizer_j ;
B_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
Transferfunctionsfordetailedstabilizer1_n ;
B_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
Transferfunctionsfordetailedstabilizer_e ;
B_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
Transferfunctionsfordetailedstabilizer1 ;
B_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
Transferfunctionsfordetailedstabilizer ; B_ZeroSeqComputation_DZG_v6_SimGen_T
ZeroSeqComputation_g ; B_NegSeqComputation_DZG_v6_SimGen_T
PosSeqComputation_d ; B_NegSeqComputation_DZG_v6_SimGen_T NegSeqComputation_c
; B_ZeroSeqComputation_DZG_v6_SimGen_T ZeroSeqComputation ;
B_NegSeqComputation_DZG_v6_SimGen_T PosSeqComputation ;
B_NegSeqComputation_DZG_v6_SimGen_T NegSeqComputation ; } B_DZG_v6_SimGen_T ;
typedef struct { real_T Rotorangledthetae_DSTATE ; real_T fluxes_DSTATE [ 6 ]
; real_T StateSpace_DSTATE [ 6 ] ; real_T dw_delay_DSTATE ; real_T
dw_predict_DSTATE ; real_T voltages_DSTATE [ 6 ] ; real_T theta_DSTATE ;
real_T Rotorspeeddeviationdw_DSTATE ; real_T DiscreteStateSpace_DSTATE [ 2 ]
; real_T DiscreteStateSpace_DSTATE_j ; real_T DiscreteStateSpace_DSTATE_b ;
real_T DiscreteStateSpace_DSTATE_o ; real_T DiscreteStateSpace_DSTATE_f ;
real_T DiscreteStateSpace_DSTATE_bk [ 3 ] ; real_T
DiscreteStateSpace_DSTATE_a ; real_T DiscreteStateSpace_DSTATE_l ; real_T
UnitDelay2_DSTATE ; real_T DiscreteStateSpace_DSTATE_i ; real_T
UnitDelay1_DSTATE ; real_T DiscreteStateSpace_DSTATE_h ; real_T
DiscreteStateSpace_DSTATE_be ; real_T DiscreteStateSpace_DSTATE_n ; real_T
Lmd_sat_DSTATE ; real_T Lmq_sat_DSTATE ; real_T lastSin ; real_T lastCos ;
real_T lastSin_m ; real_T lastCos_f ; real_T lastSin_b ; real_T lastCos_d ;
real_T Memory_PreviousInput ; real_T Memory_PreviousInput_e ; real_T
Memory_PreviousInput_m ; real_T Memory_PreviousInput_k ; real_T
Memory_PreviousInput_l ; real_T Memory_PreviousInput_c ; real_T
Memory_PreviousInput_lq ; real_T Memory_PreviousInput_cg ; real_T
Memory_PreviousInput_n ; real_T Memory_PreviousInput_c0 ; real_T
Memory_PreviousInput_i ; real_T Memory_PreviousInput_ei ; real_T
Memory_PreviousInput_a ; real_T Memory_PreviousInput_in ; real_T
Memory_PreviousInput_f ; real_T Memory_PreviousInput_iw ; real_T
RateTransition1_Buffer0 ; real_T inversion_DWORK1 [ 36 ] ; real_T
inversion_DWORK3 [ 36 ] ; real_T inversion_DWORK4 [ 36 ] ; real_T
inversion_DWORK4_b [ 36 ] ; struct { real_T modelTStart ; }
TransportDelay_RWORK ; struct { real_T modelTStart ; } TransportDelay_RWORK_l
; struct { real_T modelTStart ; } TransportDelay_RWORK_e ; struct { real_T
modelTStart ; } TransportDelay_RWORK_m ; struct { real_T modelTStart ; }
TransportDelay_RWORK_j ; struct { real_T modelTStart ; }
TransportDelay_RWORK_i ; struct { real_T modelTStart ; }
TransportDelay_RWORK_jw ; struct { real_T modelTStart ; }
TransportDelay_RWORK_b ; struct { real_T modelTStart ; }
TransportDelay_RWORK_ia ; struct { real_T modelTStart ; }
TransportDelay_RWORK_in ; struct { real_T modelTStart ; }
TransportDelay_RWORK_h ; struct { real_T modelTStart ; }
TransportDelay_RWORK_p ; struct { real_T modelTStart ; }
TransportDelay_RWORK_mk ; struct { real_T modelTStart ; }
TransportDelay_RWORK_g ; struct { real_T modelTStart ; }
TransportDelay_RWORK_l4 ; struct { real_T modelTStart ; }
TransportDelay_RWORK_mk0 ; struct { real_T modelTStart ; }
TransportDelay_RWORK_l0 ; struct { void * AS ; void * BS ; void * CS ; void *
DS ; void * DX_COL ; void * BD_COL ; void * TMP1 ; void * TMP2 ; void * XTMP
; void * SWITCH_STATUS ; void * SWITCH_STATUS_INIT ; void * SW_CHG ; void *
G_STATE ; void * USWLAST ; void * XKM12 ; void * XKP12 ; void * XLAST ; void
* ULAST ; void * IDX_SW_CHG ; void * Y_SWITCH ; void * SWITCH_TYPES ; void *
IDX_OUT_SW ; void * SWITCH_TOPO_SAVED_IDX ; void * SWITCH_MAP ; }
StateSpace_PWORK ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_d ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_m ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_e ; void * Additional_PWORK [ 2 ] ; void *
Additional1_PWORK [ 2 ] ; void * EffRangeG_PWORK [ 2 ] ; void *
InstValG_PWORK [ 2 ] ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_g ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_j ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_k ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_kj ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_ks ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_l ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_h ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_a ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_ar ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_o ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_i ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_lb ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_aw ; int32_T systemEnable ; int32_T systemEnable_m ;
int32_T systemEnable_e ; int32_T TmpAtomicSubsysAtICInport1_sysIdxToRun ;
int32_T inversion_DWORK2 [ 6 ] ; int32_T Saturation_sysIdxToRun ; int32_T
Lmq_sat_sysIdxToRun ; int32_T TmpAtomicSubsysAtSwitchInport1_sysIdxToRun ;
int32_T TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_d ; int32_T
TmpAtomicSubsysAtSwitch3Inport1_sysIdxToRun ; int32_T
TmpAtomicSubsysAtSwitch3Inport3_sysIdxToRun ; int32_T
TmpAtomicSubsysAtSwitch3Inport1_sysIdxToRun_n ; int32_T
TmpAtomicSubsysAtSwitch3Inport3_sysIdxToRun_m ; int32_T
TmpAtomicSubsysAtSwitch3Inport1_sysIdxToRun_d ; int32_T
TmpAtomicSubsysAtSwitch3Inport3_sysIdxToRun_k ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_b ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_f ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_i ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_b4 ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_l ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_m ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_by ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_bp ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_dy ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_c ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_mn ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_e ; int32_T
TmpAtomicSubsysAtSwitch3Inport1_sysIdxToRun_f ; int32_T
TmpAtomicSubsysAtSwitch3Inport3_sysIdxToRun_g ; int32_T
TmpAtomicSubsysAtSwitch3Inport1_sysIdxToRun_l ; int32_T
TmpAtomicSubsysAtSwitch3Inport3_sysIdxToRun_c ; int32_T
TmpAtomicSubsysAtSwitch3Inport1_sysIdxToRun_j ; int32_T
TmpAtomicSubsysAtSwitch3Inport3_sysIdxToRun_f ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_k ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_p ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_h ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_do ; uint32_T m_bpIndex ; uint32_T
m_bpIndex_o ; int_T StateSpace_IWORK [ 11 ] ; struct { int_T Tail ; int_T
Head ; int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
TransportDelay_IWORK ; struct { int_T Tail ; int_T Head ; int_T Last ; int_T
CircularBufSize ; int_T MaxNewBufSize ; } TransportDelay_IWORK_d ; struct {
int_T Tail ; int_T Head ; int_T Last ; int_T CircularBufSize ; int_T
MaxNewBufSize ; } TransportDelay_IWORK_e ; struct { int_T Tail ; int_T Head ;
int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
TransportDelay_IWORK_eb ; struct { int_T Tail ; int_T Head ; int_T Last ;
int_T CircularBufSize ; int_T MaxNewBufSize ; } TransportDelay_IWORK_f ;
struct { int_T Tail ; int_T Head ; int_T Last ; int_T CircularBufSize ; int_T
MaxNewBufSize ; } TransportDelay_IWORK_h ; struct { int_T Tail ; int_T Head ;
int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
TransportDelay_IWORK_o ; struct { int_T Tail ; int_T Head ; int_T Last ;
int_T CircularBufSize ; int_T MaxNewBufSize ; } TransportDelay_IWORK_dv ;
struct { int_T Tail ; int_T Head ; int_T Last ; int_T CircularBufSize ; int_T
MaxNewBufSize ; } TransportDelay_IWORK_fq ; struct { int_T Tail ; int_T Head
; int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
TransportDelay_IWORK_hg ; struct { int_T Tail ; int_T Head ; int_T Last ;
int_T CircularBufSize ; int_T MaxNewBufSize ; } TransportDelay_IWORK_a ;
struct { int_T Tail ; int_T Head ; int_T Last ; int_T CircularBufSize ; int_T
MaxNewBufSize ; } TransportDelay_IWORK_n ; struct { int_T Tail ; int_T Head ;
int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
TransportDelay_IWORK_b ; struct { int_T Tail ; int_T Head ; int_T Last ;
int_T CircularBufSize ; int_T MaxNewBufSize ; } TransportDelay_IWORK_hs ;
struct { int_T Tail ; int_T Head ; int_T Last ; int_T CircularBufSize ; int_T
MaxNewBufSize ; } TransportDelay_IWORK_ed ; struct { int_T Tail ; int_T Head
; int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
TransportDelay_IWORK_ns ; struct { int_T Tail ; int_T Head ; int_T Last ;
int_T CircularBufSize ; int_T MaxNewBufSize ; } TransportDelay_IWORK_l ;
int8_T RateTransition1_semaphoreTaken ; int8_T Saturation_SubsysRanBC ;
int8_T Lmq_sat_SubsysRanBC ; uint8_T Rotorangledthetae_NumInitCond ; uint8_T
theta_NumInitCond ; uint8_T Rotorspeeddeviationdw_SYSTEM_ENABLE ; uint8_T
Rotorspeeddeviationdw_NumInitCond ; char_T
pad_Rotorspeeddeviationdw_NumInitCond [ 1 ] ;
DW_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
Transferfunctionsfordetailedstabilizer1_g ;
DW_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
Transferfunctionsfordetailedstabilizer_j ;
DW_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
Transferfunctionsfordetailedstabilizer1_n ;
DW_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
Transferfunctionsfordetailedstabilizer_e ;
DW_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
Transferfunctionsfordetailedstabilizer1 ;
DW_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
Transferfunctionsfordetailedstabilizer ;
DW_ZeroSeqComputation_DZG_v6_SimGen_T ZeroSeqComputation_g ;
DW_NegSeqComputation_DZG_v6_SimGen_T PosSeqComputation_d ;
DW_NegSeqComputation_DZG_v6_SimGen_T NegSeqComputation_c ;
DW_ZeroSeqComputation_DZG_v6_SimGen_T ZeroSeqComputation ;
DW_NegSeqComputation_DZG_v6_SimGen_T PosSeqComputation ;
DW_NegSeqComputation_DZG_v6_SimGen_T NegSeqComputation ; } DW_DZG_v6_SimGen_T
; typedef struct { real_T integrator_CSTATE ; real_T integrator_CSTATE_i ;
real_T integrator_CSTATE_j ; real_T integrator_CSTATE_l ; real_T
integrator_CSTATE_m ; real_T integrator_CSTATE_g ; real_T integrator_CSTATE_a
; real_T integrator_CSTATE_d ; real_T integrator_CSTATE_l2 ; real_T
integrator_CSTATE_if ; real_T integrator_CSTATE_h ; real_T
integrator_CSTATE_b ; real_T integrator_CSTATE_o ; real_T
integrator_CSTATE_lx ; real_T integrator_CSTATE_k ; real_T
integrator_CSTATE_mu ; real_T IntegratorLimited_CSTATE ; real_T
Integrator1_CSTATE ; real_T Integrator1_CSTATE_d ; real_T Integrator2_CSTATE
; real_T Integrator1_CSTATE_c ; real_T TransferFcn1_CSTATE ; real_T
Integrator1_CSTATE_l ; real_T TransferFcn_CSTATE ; real_T
Integrator2_CSTATE_n ; real_T Integrator1_CSTATE_h ; real_T
Integrator3_CSTATE ; } X_DZG_v6_SimGen_T ; typedef struct { real_T
integrator_CSTATE ; real_T integrator_CSTATE_i ; real_T integrator_CSTATE_j ;
real_T integrator_CSTATE_l ; real_T integrator_CSTATE_m ; real_T
integrator_CSTATE_g ; real_T integrator_CSTATE_a ; real_T integrator_CSTATE_d
; real_T integrator_CSTATE_l2 ; real_T integrator_CSTATE_if ; real_T
integrator_CSTATE_h ; real_T integrator_CSTATE_b ; real_T integrator_CSTATE_o
; real_T integrator_CSTATE_lx ; real_T integrator_CSTATE_k ; real_T
integrator_CSTATE_mu ; real_T IntegratorLimited_CSTATE ; real_T
Integrator1_CSTATE ; real_T Integrator1_CSTATE_d ; real_T Integrator2_CSTATE
; real_T Integrator1_CSTATE_c ; real_T TransferFcn1_CSTATE ; real_T
Integrator1_CSTATE_l ; real_T TransferFcn_CSTATE ; real_T
Integrator2_CSTATE_n ; real_T Integrator1_CSTATE_h ; real_T
Integrator3_CSTATE ; } XDot_DZG_v6_SimGen_T ; typedef struct { boolean_T
integrator_CSTATE ; boolean_T integrator_CSTATE_i ; boolean_T
integrator_CSTATE_j ; boolean_T integrator_CSTATE_l ; boolean_T
integrator_CSTATE_m ; boolean_T integrator_CSTATE_g ; boolean_T
integrator_CSTATE_a ; boolean_T integrator_CSTATE_d ; boolean_T
integrator_CSTATE_l2 ; boolean_T integrator_CSTATE_if ; boolean_T
integrator_CSTATE_h ; boolean_T integrator_CSTATE_b ; boolean_T
integrator_CSTATE_o ; boolean_T integrator_CSTATE_lx ; boolean_T
integrator_CSTATE_k ; boolean_T integrator_CSTATE_mu ; boolean_T
IntegratorLimited_CSTATE ; boolean_T Integrator1_CSTATE ; boolean_T
Integrator1_CSTATE_d ; boolean_T Integrator2_CSTATE ; boolean_T
Integrator1_CSTATE_c ; boolean_T TransferFcn1_CSTATE ; boolean_T
Integrator1_CSTATE_l ; boolean_T TransferFcn_CSTATE ; boolean_T
Integrator2_CSTATE_n ; boolean_T Integrator1_CSTATE_h ; boolean_T
Integrator3_CSTATE ; } XDis_DZG_v6_SimGen_T ; struct
P_NegSeqComputation_DZG_v6_SimGen_T_ { real_T P_0 ; real_T P_1 ; creal_T P_2
[ 3 ] ; } ; struct P_ZeroSeqComputation_DZG_v6_SimGen_T_ { real_T P_0 ;
real_T P_1 ; } ; struct
P_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T_ { real_T P_0 ;
real_T P_4 ; real_T P_5 ; real_T P_9 ; real_T P_10 ; } ; struct
P_DZG_v6_SimGen_T_ { real_T P_0 ; real_T P_1 ; real_T P_2 ; real_T P_3 ;
real_T P_4 [ 5 ] ; real_T P_5 [ 5 ] ; real_T P_6 [ 5 ] ; real_T P_7 [ 5 ] ;
real_T P_8 [ 5 ] ; real_T P_9 [ 5 ] ; real_T P_10 ; real_T P_11 ; real_T P_12
; real_T P_13 ; real_T P_14 ; real_T P_15 ; real_T P_16 ; real_T P_17 ;
real_T P_18 ; real_T P_19 ; real_T P_20 ; real_T P_21 ; real_T P_22 [ 5 ] ;
real_T P_23 [ 5 ] ; real_T P_24 ; real_T P_25 [ 5 ] ; real_T P_26 [ 5 ] ;
real_T P_27 ; real_T P_28 [ 5 ] ; real_T P_29 [ 5 ] ; real_T P_30 ; real_T
P_31 ; real_T P_32 ; real_T P_33 [ 3 ] ; real_T P_34 ; real_T P_35 [ 10 ] ;
real_T P_36 [ 10 ] ; real_T P_37 ; real_T P_38 ; real_T P_39 [ 3 ] ; real_T
P_40 ; real_T P_41 ; real_T P_42 ; real_T P_43 ; real_T P_44 [ 3 ] ; real_T
P_45 ; real_T P_46 [ 10 ] ; real_T P_47 [ 10 ] ; real_T P_48 ; real_T P_49 ;
real_T P_50 [ 2 ] ; real_T P_51 [ 36 ] ; real_T P_52 [ 36 ] ; real_T P_53 [
36 ] ; real_T P_54 ; real_T P_55 ; real_T P_56 ; real_T P_57 ; real_T P_58 ;
real_T P_59 ; real_T P_60 ; real_T P_61 ; real_T P_62 ; real_T P_63 [ 6 ] ;
real_T P_64 ; real_T P_65 [ 6 ] ; real_T P_66 ; real_T P_67 ; real_T P_68 ;
real_T P_69 ; real_T P_70 ; real_T P_71 ; real_T P_72 ; real_T P_73 ; real_T
P_74 ; real_T P_75 ; real_T P_76 ; real_T P_77 ; real_T P_78 ; real_T P_79 ;
real_T P_80 ; real_T P_81 ; real_T P_82 ; real_T P_83 ; real_T P_84 ; real_T
P_85 ; real_T P_86 ; real_T P_87 ; real_T P_88 [ 2 ] ; real_T P_89 [ 36 ] ;
real_T P_90 [ 2 ] ; real_T P_91 [ 66 ] ; real_T P_92 [ 2 ] ; real_T P_93 [
300 ] ; real_T P_94 [ 2 ] ; real_T P_95 [ 550 ] ; real_T P_96 [ 2 ] ; real_T
P_97 [ 6 ] ; real_T P_98 ; real_T P_99 ; real_T P_100 ; real_T P_101 ; real_T
P_102 ; real_T P_103 ; real_T P_104 ; real_T P_105 ; real_T P_106 ; real_T
P_107 ; real_T P_108 ; real_T P_109 ; real_T P_110 ; real_T P_111 ; real_T
P_112 ; real_T P_113 ; real_T P_114 ; real_T P_115 ; real_T P_116 ; real_T
P_117 ; real_T P_118 ; real_T P_119 ; real_T P_120 ; real_T P_121 ; real_T
P_122 ; real_T P_123 ; real_T P_124 ; real_T P_125 ; real_T P_126 ; real_T
P_127 ; real_T P_128 ; real_T P_129 ; real_T P_130 ; real_T P_131 ; real_T
P_132 ; real_T P_133 ; real_T P_134 ; real_T P_135 ; real_T P_136 ; real_T
P_137 ; real_T P_138 ; real_T P_139 ; real_T P_140 ; real_T P_141 ; real_T
P_142 ; real_T P_143 ; real_T P_144 ; real_T P_145 ; real_T P_146 ; real_T
P_147 ; real_T P_148 ; real_T P_149 ; real_T P_150 ; real_T P_151 ; real_T
P_152 ; real_T P_153 ; real_T P_154 ; real_T P_155 ; real_T P_156 ; real_T
P_157 ; real_T P_158 ; real_T P_159 ; real_T P_160 ; real_T P_161 ; real_T
P_162 ; real_T P_163 ; real_T P_164 ; real_T P_165 ; real_T P_166 ; real_T
P_167 ; real_T P_168 ; real_T P_169 ; real_T P_170 ; real_T P_171 ; real_T
P_172 ; real_T P_173 ; real_T P_174 ; real_T P_175 ; real_T P_176 ; real_T
P_177 ; real_T P_178 ; real_T P_179 ; real_T P_180 ; real_T P_181 ; real_T
P_182 ; real_T P_183 ; real_T P_184 ; real_T P_185 ; real_T P_186 ; real_T
P_187 ; real_T P_188 ; real_T P_189 ; real_T P_190 ; real_T P_191 ; real_T
P_192 ; real_T P_193 ; real_T P_194 ; real_T P_195 ; real_T P_196 ; real_T
P_197 ; real_T P_198 ; real_T P_199 ; real_T P_200 ; real_T P_201 ; real_T
P_202 ; real_T P_203 ; real_T P_204 ; real_T P_205 ; real_T P_206 ; real_T
P_207 ; real_T P_208 ; real_T P_209 ; real_T P_210 ; real_T P_211 ; real_T
P_212 ; real_T P_213 ; real_T P_214 ; real_T P_215 ; real_T P_216 ; real_T
P_217 ; real_T P_218 ; real_T P_219 ; real_T P_220 ; real_T P_221 ; real_T
P_222 ; real_T P_223 ; real_T P_224 ; real_T P_225 ; real_T P_226 ; real_T
P_227 ; real_T P_228 ; real_T P_229 ; real_T P_230 ; real_T P_231 ; real_T
P_232 ; real_T P_233 ; real_T P_234 ; real_T P_235 ; real_T P_236 ; real_T
P_237 ; real_T P_238 ; real_T P_239 ; real_T P_240 ; real_T P_241 ; real_T
P_242 ; real_T P_243 ; real_T P_244 ; real_T P_245 ; real_T P_246 ; real_T
P_247 ; real_T P_248 ; real_T P_249 ; real_T P_250 ; real_T P_251 ; real_T
P_252 ; real_T P_253 ; real_T P_254 ; real_T P_255 ; real_T P_256 ; real_T
P_257 ; real_T P_258 ; real_T P_259 ; real_T P_260 ; real_T P_261 ; real_T
P_262 ; real_T P_263 ; real_T P_264 ; real_T P_265 ; real_T P_266 ; real_T
P_267 ; real_T P_268 ; real_T P_269 ; real_T P_270 ; real_T P_271 ; real_T
P_272 ; real_T P_273 [ 3 ] ; real_T P_274 ; real_T P_275 [ 3 ] ; real_T P_276
; real_T P_277 ; real_T P_278 ; real_T P_279 ; real_T P_280 ; real_T P_281 [
18 ] ; real_T P_282 ; real_T P_283 ; real_T P_284 ; real_T P_285 [ 2 ] ;
real_T P_286 ; real_T P_287 ; real_T P_288 ; real_T P_289 ; real_T P_290 ;
real_T P_291 ; real_T P_292 ; real_T P_293 ; real_T P_294 ; real_T P_295 ;
real_T P_296 ; real_T P_297 ; real_T P_298 ; real_T P_299 ; real_T P_300 ;
real_T P_301 ; real_T P_302 ; real_T P_303 ; real_T P_304 [ 4 ] ; real_T
P_305 [ 2 ] ; real_T P_306 [ 2 ] ; real_T P_307 ; real_T P_308 ; real_T P_309
; real_T P_310 ; real_T P_311 ; real_T P_312 ; real_T P_313 ; real_T P_314 ;
real_T P_315 ; real_T P_316 ; real_T P_317 ; real_T P_318 ; real_T P_319 ;
real_T P_320 ; real_T P_321 ; real_T P_322 ; real_T P_323 ; real_T P_324 ;
real_T P_325 ; real_T P_326 ; real_T P_327 ; real_T P_328 ; real_T P_329 ;
real_T P_330 ; real_T P_331 ; real_T P_332 ; real_T P_333 [ 9 ] ; real_T
P_334 [ 3 ] ; real_T P_335 [ 3 ] ; real_T P_336 ; real_T P_337 ; real_T P_338
; real_T P_339 ; real_T P_340 ; real_T P_341 ; real_T P_342 ; real_T P_343 ;
real_T P_344 ; real_T P_345 ; real_T P_346 ; real_T P_347 ; real_T P_348 ;
real_T P_349 ; real_T P_350 ; real_T P_351 ; real_T P_352 ; real_T P_353 ;
real_T P_354 ; real_T P_355 ; real_T P_356 ; real_T P_357 ; real_T P_358 ;
real_T P_359 ; real_T P_360 ; real_T P_361 ; real_T P_362 ; real_T P_363 ;
real_T P_364 ; real_T P_365 ; real_T P_366 ; real_T P_367 ; real_T P_368 ;
real_T P_369 ; real_T P_370 ; real_T P_371 ; real_T P_372 ; real_T P_373 ;
real_T P_374 ; real_T P_375 ; real_T P_379 ; real_T P_381 ; real_T P_382 ;
real_T P_383 ; real_T P_384 ; real_T P_385 ; real_T P_386 ; real_T P_387 ;
real_T P_388 ; real_T P_389 ; real_T P_390 ; real_T P_391 ; real_T P_392 ;
real_T P_393 ; real_T P_394 ; real_T P_395 ; real_T P_396 ; real_T P_397 ;
real_T P_398 ; real_T P_399 ; real_T P_400 ; real_T P_401 ; real_T P_402 ;
real_T P_403 ; real_T P_404 ; real_T P_405 ; real_T P_406 ; real_T P_407 ;
real_T P_408 ; real_T P_409 ; real_T P_410 ; real_T P_411 ; real_T P_412 ;
real_T P_413 ; real_T P_414 ; real_T P_415 ; real_T P_416 ; real_T P_417 ;
real_T P_418 ; real_T P_419 ; real_T P_420 ; real_T P_421 ; real_T P_422 ;
real_T P_423 ; real_T P_424 ; real_T P_425 ; real_T P_426 ; real_T P_427 ;
real_T P_428 ; real_T P_429 ; real_T P_430 ; real_T P_431 ; real_T P_432 ;
real_T P_433 ; real_T P_434 ; real_T P_435 ; real_T P_436 ; real_T P_437 ;
real_T P_438 ; real_T P_439 ; real_T P_440 ; real_T P_441 [ 6 ] ; real_T
P_442 [ 6 ] ; real_T P_443 ; real_T P_444 ; real_T P_445 ; real_T P_446 ;
real_T P_447 ; real_T P_448 ; real_T P_449 ; real_T P_450 ; real_T P_451 ;
real_T P_452 ; real_T P_453 ; real_T P_454 ; real_T P_455 ; real_T P_456 ;
real_T P_457 ; real_T P_458 ; real_T P_459 ; real_T P_460 ; real_T P_461 ;
real_T P_462 ; real_T P_463 ; real_T P_464 ; real_T P_465 ; real_T P_466 ;
real_T P_467 ; real_T P_468 ; real_T P_469 ; real_T P_470 ; real_T P_471 ;
real_T P_472 ; real_T P_473 ; real_T P_474 ; real_T P_475 ; real_T P_476 ;
real_T P_477 ; real_T P_478 ; real_T P_479 ; real_T P_480 ; real_T P_481 ;
real_T P_482 ; real_T P_483 ; real_T P_484 ; real_T P_485 ; real_T P_486 ;
real_T P_487 ; real_T P_488 ; real_T P_489 ; real_T P_490 ; real_T P_491 ;
real_T P_492 ; real_T P_493 ; real_T P_494 ; real_T P_495 ; real_T P_496 ;
real_T P_497 ; real_T P_498 [ 6 ] ; real_T P_499 [ 6 ] ; real_T P_500 ;
real_T P_501 ; real_T P_502 ; real_T P_503 ; real_T P_504 ; real_T P_505 ;
real_T P_506 [ 6 ] ; real_T P_507 ; real_T P_508 ; real_T P_509 ; real_T
P_510 ; real_T P_511 ; real_T P_512 ; real_T P_513 ; real_T P_514 ; real_T
P_515 ; real_T P_516 ; real_T P_517 ; real_T P_518 ; real_T P_519 ; real_T
P_520 [ 36 ] ; real_T P_521 ; real_T P_522 [ 36 ] ; real_T P_523 ; real_T
P_524 [ 2 ] ; real_T P_525 [ 2 ] ; real_T P_526 [ 36 ] ; real_T P_527 [ 36 ]
; real_T P_528 [ 3 ] ; real_T P_529 ; real_T P_530 ; real_T P_531 ; real_T
P_532 ; real_T P_533 ; real_T P_534 ; real_T P_535 ; real_T P_536 ; real_T
P_537 ; real_T P_538 ; real_T P_539 ; real_T P_540 ; real_T P_541 ; real_T
P_542 ; real_T P_543 ; real_T P_544 ; real_T P_545 ; real_T P_546 ; real_T
P_547 ; real_T P_548 ; real_T P_549 ; real_T P_550 ; real_T P_551 ; real_T
P_552 ; real_T P_553 ; real_T P_554 ; real_T P_555 ; boolean_T P_556 ;
boolean_T P_557 ; boolean_T P_558 ; boolean_T P_559 ; char_T pad_P_559 [ 4 ]
; P_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
Transferfunctionsfordetailedstabilizer1_g ;
P_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
Transferfunctionsfordetailedstabilizer_j ;
P_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
Transferfunctionsfordetailedstabilizer1_n ;
P_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
Transferfunctionsfordetailedstabilizer_e ;
P_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
Transferfunctionsfordetailedstabilizer1 ;
P_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
Transferfunctionsfordetailedstabilizer ; P_ZeroSeqComputation_DZG_v6_SimGen_T
ZeroSeqComputation_g ; P_NegSeqComputation_DZG_v6_SimGen_T
PosSeqComputation_d ; P_NegSeqComputation_DZG_v6_SimGen_T NegSeqComputation_c
; P_ZeroSeqComputation_DZG_v6_SimGen_T ZeroSeqComputation ;
P_NegSeqComputation_DZG_v6_SimGen_T PosSeqComputation ;
P_NegSeqComputation_DZG_v6_SimGen_T NegSeqComputation ; } ; extern
P_DZG_v6_SimGen_T DZG_v6_SimGen_rtDefaultP ;
#endif
