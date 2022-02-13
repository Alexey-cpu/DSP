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
typedef struct { real_T B_49_0_0 ; real_T B_49_1_0 ; }
B_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T ; typedef struct {
int32_T Transferfunctionsfordetailedstabilizer_sysIdxToRun ; int8_T
Transferfunctionsfordetailedstabilizer_SubsysRanBC ; char_T
pad_Transferfunctionsfordetailedstabilizer_SubsysRanBC [ 3 ] ; }
DW_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T ; typedef struct {
creal_T B_55_49_0 ; creal_T B_55_68_0 ; creal_T B_55_123_0 ; creal_T
B_55_142_0 ; creal_T B_55_197_0 ; creal_T B_55_216_0 ; creal_T B_55_271_0 ;
creal_T B_55_290_0 ; creal_T B_55_345_0 ; creal_T B_55_364_0 ; creal_T
B_55_419_0 ; creal_T B_55_438_0 ; creal_T B_55_493_0 ; creal_T B_55_512_0 ;
real_T B_55_0_0 [ 12 ] ; real_T B_55_1_0 [ 12 ] ; real_T B_55_2_0 [ 6 ] ;
real_T B_55_2_1 ; real_T B_55_2_2 ; real_T B_55_3_0 ; real_T B_55_4_0 ;
real_T B_55_5_0 ; real_T B_55_6_0 ; real_T B_55_7_0 ; real_T B_55_7_1 ;
real_T B_55_8_0 [ 6 ] ; real_T B_55_9_0 ; real_T B_55_11_0 [ 36 ] ; real_T
B_55_12_0 [ 6 ] ; real_T B_55_13_0 [ 6 ] ; real_T B_55_14_0 ; real_T
B_55_15_0 ; real_T B_55_16_0 ; real_T B_55_17_0 [ 3 ] ; real_T B_55_18_0 [ 2
] ; real_T B_55_19_0 ; real_T B_55_20_0 ; real_T B_55_23_0 ; real_T B_55_24_0
; real_T B_55_25_0 ; real_T B_55_26_0 [ 3 ] ; real_T B_55_27_0 [ 3 ] ; real_T
B_55_28_0 [ 3 ] ; real_T B_55_29_0 ; real_T B_55_30_0 ; real_T B_55_31_0 ;
real_T B_55_32_0 [ 59 ] ; real_T B_55_32_1 [ 9 ] ; real_T B_55_33_0 ; real_T
B_55_34_0 ; real_T B_55_35_0 ; real_T B_55_36_0 ; real_T B_55_38_0 ; real_T
B_55_40_0 ; real_T B_55_41_0 ; real_T B_55_42_0 ; real_T B_55_43_0 ; real_T
B_55_44_0 ; real_T B_55_46_0 ; real_T B_55_48_0 ; real_T B_55_50_0 ; real_T
B_55_50_1 ; real_T B_55_51_0 ; real_T B_55_52_0 ; real_T B_55_53_0 ; real_T
B_55_54_0 ; real_T B_55_55_0 ; real_T B_55_57_0 ; real_T B_55_59_0 ; real_T
B_55_60_0 ; real_T B_55_61_0 ; real_T B_55_62_0 ; real_T B_55_63_0 ; real_T
B_55_65_0 ; real_T B_55_67_0 ; real_T B_55_69_0 ; real_T B_55_69_1 ; real_T
B_55_70_0 ; real_T B_55_71_0 ; real_T B_55_72_0 ; real_T B_55_73_0 ; real_T
B_55_74_0 ; real_T B_55_75_0 ; real_T B_55_76_0 ; real_T B_55_77_0 ; real_T
B_55_78_0 ; real_T B_55_79_0 ; real_T B_55_80_0 ; real_T B_55_84_0 ; real_T
B_55_85_0 ; real_T B_55_86_0 ; real_T B_55_87_0 [ 3 ] ; real_T B_55_88_0 ;
real_T B_55_89_0 ; real_T B_55_90_0 ; real_T B_55_91_0 [ 3 ] ; real_T
B_55_99_0 ; real_T B_55_100_0 ; real_T B_55_101_0 ; real_T B_55_102_0 ;
real_T B_55_103_0 ; real_T B_55_104_0 ; real_T B_55_105_0 ; real_T B_55_106_0
; real_T B_55_107_0 ; real_T B_55_108_0 ; real_T B_55_109_0 ; real_T
B_55_110_0 ; real_T B_55_112_0 ; real_T B_55_114_0 ; real_T B_55_115_0 ;
real_T B_55_116_0 ; real_T B_55_117_0 ; real_T B_55_118_0 ; real_T B_55_120_0
; real_T B_55_122_0 ; real_T B_55_124_0 ; real_T B_55_124_1 ; real_T
B_55_125_0 ; real_T B_55_126_0 ; real_T B_55_127_0 ; real_T B_55_128_0 ;
real_T B_55_129_0 ; real_T B_55_131_0 ; real_T B_55_133_0 ; real_T B_55_134_0
; real_T B_55_135_0 ; real_T B_55_136_0 ; real_T B_55_137_0 ; real_T
B_55_139_0 ; real_T B_55_141_0 ; real_T B_55_143_0 ; real_T B_55_143_1 ;
real_T B_55_144_0 ; real_T B_55_145_0 ; real_T B_55_146_0 ; real_T B_55_147_0
; real_T B_55_148_0 ; real_T B_55_149_0 ; real_T B_55_150_0 ; real_T
B_55_151_0 ; real_T B_55_152_0 ; real_T B_55_153_0 ; real_T B_55_154_0 ;
real_T B_55_158_0 ; real_T B_55_159_0 ; real_T B_55_160_0 ; real_T B_55_161_0
[ 3 ] ; real_T B_55_162_0 ; real_T B_55_163_0 ; real_T B_55_164_0 ; real_T
B_55_165_0 [ 3 ] ; real_T B_55_173_0 ; real_T B_55_174_0 ; real_T B_55_175_0
; real_T B_55_176_0 ; real_T B_55_177_0 ; real_T B_55_178_0 ; real_T
B_55_179_0 ; real_T B_55_180_0 ; real_T B_55_181_0 ; real_T B_55_182_0 ;
real_T B_55_183_0 ; real_T B_55_184_0 ; real_T B_55_186_0 ; real_T B_55_188_0
; real_T B_55_189_0 ; real_T B_55_190_0 ; real_T B_55_191_0 ; real_T
B_55_192_0 ; real_T B_55_194_0 ; real_T B_55_196_0 ; real_T B_55_198_0 ;
real_T B_55_198_1 ; real_T B_55_199_0 ; real_T B_55_200_0 ; real_T B_55_201_0
; real_T B_55_202_0 ; real_T B_55_203_0 ; real_T B_55_205_0 ; real_T
B_55_207_0 ; real_T B_55_208_0 ; real_T B_55_209_0 ; real_T B_55_210_0 ;
real_T B_55_211_0 ; real_T B_55_213_0 ; real_T B_55_215_0 ; real_T B_55_217_0
; real_T B_55_217_1 ; real_T B_55_218_0 ; real_T B_55_219_0 ; real_T
B_55_221_0 ; real_T B_55_222_0 ; real_T B_55_223_0 ; real_T B_55_224_0 ;
real_T B_55_225_0 ; real_T B_55_226_0 ; real_T B_55_227_0 ; real_T B_55_228_0
; real_T B_55_229_0 ; real_T B_55_232_0 ; real_T B_55_233_0 ; real_T
B_55_234_0 ; real_T B_55_235_0 [ 3 ] ; real_T B_55_236_0 ; real_T B_55_237_0
; real_T B_55_238_0 ; real_T B_55_239_0 [ 3 ] ; real_T B_55_247_0 ; real_T
B_55_248_0 ; real_T B_55_249_0 ; real_T B_55_250_0 ; real_T B_55_251_0 ;
real_T B_55_252_0 ; real_T B_55_253_0 ; real_T B_55_254_0 ; real_T B_55_255_0
; real_T B_55_256_0 ; real_T B_55_257_0 ; real_T B_55_258_0 ; real_T
B_55_260_0 ; real_T B_55_262_0 ; real_T B_55_263_0 ; real_T B_55_264_0 ;
real_T B_55_265_0 ; real_T B_55_266_0 ; real_T B_55_268_0 ; real_T B_55_270_0
; real_T B_55_272_0 ; real_T B_55_272_1 ; real_T B_55_273_0 ; real_T
B_55_274_0 ; real_T B_55_275_0 ; real_T B_55_276_0 ; real_T B_55_277_0 ;
real_T B_55_279_0 ; real_T B_55_281_0 ; real_T B_55_282_0 ; real_T B_55_283_0
; real_T B_55_284_0 ; real_T B_55_285_0 ; real_T B_55_287_0 ; real_T
B_55_289_0 ; real_T B_55_291_0 ; real_T B_55_291_1 ; real_T B_55_292_0 ;
real_T B_55_293_0 ; real_T B_55_294_0 ; real_T B_55_295_0 ; real_T B_55_296_0
; real_T B_55_297_0 ; real_T B_55_298_0 ; real_T B_55_299_0 ; real_T
B_55_300_0 ; real_T B_55_301_0 ; real_T B_55_302_0 ; real_T B_55_306_0 ;
real_T B_55_307_0 ; real_T B_55_308_0 ; real_T B_55_309_0 [ 3 ] ; real_T
B_55_310_0 ; real_T B_55_311_0 ; real_T B_55_312_0 ; real_T B_55_313_0 [ 3 ]
; real_T B_55_321_0 ; real_T B_55_322_0 ; real_T B_55_323_0 ; real_T
B_55_324_0 ; real_T B_55_325_0 ; real_T B_55_326_0 ; real_T B_55_327_0 ;
real_T B_55_328_0 ; real_T B_55_329_0 ; real_T B_55_330_0 ; real_T B_55_331_0
; real_T B_55_332_0 ; real_T B_55_334_0 ; real_T B_55_336_0 ; real_T
B_55_337_0 ; real_T B_55_338_0 ; real_T B_55_339_0 ; real_T B_55_340_0 ;
real_T B_55_342_0 ; real_T B_55_344_0 ; real_T B_55_346_0 ; real_T B_55_346_1
; real_T B_55_347_0 ; real_T B_55_348_0 ; real_T B_55_349_0 ; real_T
B_55_350_0 ; real_T B_55_351_0 ; real_T B_55_353_0 ; real_T B_55_355_0 ;
real_T B_55_356_0 ; real_T B_55_357_0 ; real_T B_55_358_0 ; real_T B_55_359_0
; real_T B_55_361_0 ; real_T B_55_363_0 ; real_T B_55_365_0 ; real_T
B_55_365_1 ; real_T B_55_366_0 ; real_T B_55_367_0 ; real_T B_55_369_0 ;
real_T B_55_370_0 ; real_T B_55_371_0 ; real_T B_55_372_0 ; real_T B_55_373_0
; real_T B_55_374_0 ; real_T B_55_375_0 ; real_T B_55_376_0 ; real_T
B_55_377_0 ; real_T B_55_380_0 ; real_T B_55_381_0 ; real_T B_55_382_0 ;
real_T B_55_383_0 [ 3 ] ; real_T B_55_384_0 ; real_T B_55_385_0 ; real_T
B_55_386_0 ; real_T B_55_387_0 [ 3 ] ; real_T B_55_395_0 ; real_T B_55_396_0
; real_T B_55_397_0 ; real_T B_55_398_0 ; real_T B_55_399_0 ; real_T
B_55_400_0 ; real_T B_55_401_0 ; real_T B_55_402_0 ; real_T B_55_403_0 ;
real_T B_55_404_0 ; real_T B_55_405_0 ; real_T B_55_406_0 ; real_T B_55_408_0
; real_T B_55_410_0 ; real_T B_55_411_0 ; real_T B_55_412_0 ; real_T
B_55_413_0 ; real_T B_55_414_0 ; real_T B_55_416_0 ; real_T B_55_418_0 ;
real_T B_55_420_0 ; real_T B_55_420_1 ; real_T B_55_421_0 ; real_T B_55_422_0
; real_T B_55_423_0 ; real_T B_55_424_0 ; real_T B_55_425_0 ; real_T
B_55_427_0 ; real_T B_55_429_0 ; real_T B_55_430_0 ; real_T B_55_431_0 ;
real_T B_55_432_0 ; real_T B_55_433_0 ; real_T B_55_435_0 ; real_T B_55_437_0
; real_T B_55_439_0 ; real_T B_55_439_1 ; real_T B_55_440_0 ; real_T
B_55_441_0 ; real_T B_55_443_0 ; real_T B_55_444_0 ; real_T B_55_445_0 ;
real_T B_55_446_0 ; real_T B_55_447_0 ; real_T B_55_448_0 ; real_T B_55_449_0
; real_T B_55_450_0 ; real_T B_55_451_0 ; real_T B_55_454_0 ; real_T
B_55_455_0 ; real_T B_55_456_0 ; real_T B_55_457_0 [ 3 ] ; real_T B_55_458_0
; real_T B_55_459_0 ; real_T B_55_460_0 ; real_T B_55_461_0 [ 3 ] ; real_T
B_55_469_0 ; real_T B_55_470_0 ; real_T B_55_471_0 ; real_T B_55_472_0 ;
real_T B_55_473_0 ; real_T B_55_474_0 ; real_T B_55_475_0 ; real_T B_55_476_0
; real_T B_55_477_0 ; real_T B_55_478_0 ; real_T B_55_479_0 ; real_T
B_55_480_0 ; real_T B_55_482_0 ; real_T B_55_484_0 ; real_T B_55_485_0 ;
real_T B_55_486_0 ; real_T B_55_487_0 ; real_T B_55_488_0 ; real_T B_55_490_0
; real_T B_55_492_0 ; real_T B_55_494_0 ; real_T B_55_494_1 ; real_T
B_55_495_0 ; real_T B_55_496_0 ; real_T B_55_497_0 ; real_T B_55_498_0 ;
real_T B_55_499_0 ; real_T B_55_501_0 ; real_T B_55_503_0 ; real_T B_55_504_0
; real_T B_55_505_0 ; real_T B_55_506_0 ; real_T B_55_507_0 ; real_T
B_55_509_0 ; real_T B_55_511_0 ; real_T B_55_513_0 ; real_T B_55_513_1 ;
real_T B_55_514_0 ; real_T B_55_515_0 ; real_T B_55_516_0 ; real_T B_55_517_0
; real_T B_55_518_0 ; real_T B_55_519_0 ; real_T B_55_520_0 ; real_T
B_55_521_0 ; real_T B_55_522_0 ; real_T B_55_523_0 ; real_T B_55_524_0 ;
real_T B_55_528_0 ; real_T B_55_529_0 ; real_T B_55_530_0 ; real_T B_55_531_0
[ 3 ] ; real_T B_55_532_0 ; real_T B_55_533_0 ; real_T B_55_534_0 ; real_T
B_55_535_0 [ 3 ] ; real_T B_55_543_0 ; real_T B_55_544_0 ; real_T B_55_545_0
; real_T B_55_546_0 ; real_T B_55_547_0 ; real_T B_55_548_0 ; real_T
B_55_549_0 ; real_T B_55_550_0 ; real_T B_55_551_0 ; real_T B_55_552_0 ;
real_T B_55_553_0 ; real_T B_55_554_0 ; real_T B_55_555_0 ; real_T B_55_556_0
; real_T B_55_557_0 [ 2 ] ; real_T B_55_558_0 [ 2 ] ; real_T B_55_559_0 ;
real_T B_55_560_0 ; real_T B_55_561_0 ; real_T B_55_562_0 ; real_T B_55_563_0
; real_T B_55_564_0 ; real_T B_55_565_0 ; real_T B_55_566_0 [ 6 ] ; real_T
B_55_568_0 ; real_T B_55_569_0 ; real_T B_55_570_0 ; real_T B_55_571_0 ;
real_T B_55_572_0 ; real_T B_55_573_0 ; real_T B_55_574_0 ; real_T B_55_575_0
; real_T B_55_576_0 ; real_T B_55_579_0 ; real_T B_55_580_0 [ 3 ] ; real_T
B_55_581_0 [ 2 ] ; real_T B_55_582_0 [ 3 ] ; real_T B_55_583_0 ; real_T
B_55_584_0 [ 3 ] ; real_T B_55_585_0 [ 3 ] ; real_T B_55_586_0 ; real_T
B_55_587_0 [ 2 ] ; real_T B_55_588_0 [ 2 ] ; real_T B_55_589_0 ; real_T
B_55_590_0 ; real_T B_55_591_0 [ 2 ] ; real_T B_55_592_0 ; real_T B_55_593_0
; real_T B_55_594_0 ; real_T B_55_595_0 ; real_T B_55_596_0 ; real_T
B_55_597_0 ; real_T B_55_598_0 ; real_T B_55_599_0 ; real_T B_55_600_0 ;
real_T B_55_601_0 ; real_T B_55_602_0 [ 18 ] ; real_T B_55_603_0 [ 18 ] ;
real_T B_55_604_0 ; real_T B_55_607_0 ; real_T B_55_609_0 ; real_T B_55_610_0
; real_T B_55_613_0 ; real_T B_55_614_0 ; real_T B_55_615_0 ; real_T
B_55_616_0 [ 6 ] ; real_T B_55_617_0 ; real_T B_55_619_0 [ 6 ] ; real_T
B_55_620_0 ; real_T B_55_621_0 ; real_T B_55_622_0 ; real_T B_55_623_0 ;
real_T B_55_624_0 ; real_T B_55_625_0 ; real_T B_55_626_0 ; real_T B_55_627_0
; real_T B_55_628_0 ; real_T B_55_634_0 ; real_T B_55_635_0 ; real_T
B_55_636_0 ; real_T B_55_637_0 ; real_T B_55_638_0 ; real_T B_55_639_0 ;
real_T B_55_640_0 ; real_T B_55_641_0 ; real_T B_55_642_0 ; real_T B_55_643_0
; real_T B_55_644_0 ; real_T B_55_645_0 ; real_T B_55_646_0 ; real_T
B_55_647_0 ; real_T B_55_648_0 ; real_T B_55_650_0 ; real_T B_55_651_0 ;
real_T B_55_653_0 ; real_T B_55_654_0 ; real_T B_55_655_0 ; real_T B_55_656_0
; real_T B_55_658_0 ; real_T B_55_659_0 ; real_T B_55_661_0 ; real_T
B_55_662_0 ; real_T B_55_663_0 ; real_T B_55_664_0 ; real_T B_55_666_0 ;
real_T B_55_667_0 ; real_T B_55_669_0 ; real_T B_55_670_0 ; real_T B_55_671_0
; real_T B_55_672_0 ; real_T B_55_673_0 ; real_T B_55_674_0 ; real_T
B_55_675_0 ; real_T B_55_676_0 ; real_T B_55_677_0 ; real_T B_55_678_0 ;
real_T B_55_679_0 ; real_T B_55_680_0 ; real_T B_55_681_0 ; real_T B_55_682_0
; real_T B_55_683_0 ; real_T B_55_684_0 ; real_T B_55_686_0 ; real_T
B_55_687_0 ; real_T B_55_690_0 ; real_T B_55_691_0 ; real_T B_55_693_0 ;
real_T B_55_694_0 ; real_T B_55_696_0 ; real_T B_55_697_0 ; real_T B_55_698_0
; real_T B_55_699_0 ; real_T B_55_700_0 ; real_T B_55_701_0 ; real_T
B_55_702_0 ; real_T B_55_703_0 ; real_T B_55_704_0 ; real_T B_55_705_0 ;
real_T B_55_706_0 ; real_T B_55_707_0 ; real_T B_55_708_0 ; real_T B_55_709_0
; real_T B_55_710_0 ; real_T B_55_711_0 ; real_T B_55_712_0 ; real_T
B_55_713_0 ; real_T B_55_714_0 ; real_T B_55_715_0 ; real_T B_55_716_0 ;
real_T B_55_717_0 ; real_T B_55_718_0 ; real_T B_55_719_0 ; real_T B_55_720_0
; real_T B_55_721_0 ; real_T B_55_722_0 ; real_T B_55_723_0 ; real_T
B_55_724_0 ; real_T B_55_725_0 ; real_T B_55_726_0 ; real_T B_55_727_0 ;
real_T B_55_728_0 ; real_T B_55_729_0 ; real_T B_55_730_0 ; real_T B_55_731_0
; real_T B_55_732_0 ; real_T B_55_733_0 ; real_T B_55_734_0 ; real_T
B_55_735_0 ; real_T B_55_736_0 ; real_T B_55_737_0 ; real_T B_55_738_0 ;
real_T B_55_739_0 ; real_T B_55_740_0 ; real_T B_55_741_0 ; real_T B_55_742_0
; real_T B_55_743_0 ; real_T B_55_744_0 ; real_T B_55_745_0 ; real_T
B_55_746_0 ; real_T B_55_748_0 ; real_T B_55_749_0 ; real_T B_55_750_0 ;
real_T B_55_751_0 ; real_T B_55_752_0 ; real_T B_55_754_0 ; real_T B_55_758_0
; real_T B_55_759_0 ; real_T B_55_760_0 ; real_T B_55_762_0 ; real_T
B_55_766_0 ; real_T B_55_767_0 ; real_T B_55_768_0 ; real_T B_55_770_0 ;
real_T B_55_781_0 ; real_T B_55_782_0 ; real_T B_55_783_0 ; real_T B_55_785_0
; real_T B_55_786_0 ; real_T B_55_787_0 ; real_T B_55_789_0 ; real_T
B_55_793_0 ; real_T B_55_794_0 ; real_T B_55_795_0 ; real_T B_55_797_0 ;
real_T B_55_801_0 ; real_T B_55_802_0 ; real_T B_55_803_0 ; real_T B_55_805_0
; real_T B_55_816_0 ; real_T B_55_894_0 ; real_T B_55_895_0 ; real_T
B_55_896_0 ; real_T B_55_897_0 ; real_T B_55_899_0 ; real_T B_55_902_0 ;
real_T B_55_906_0 ; real_T B_55_907_0 ; real_T B_55_910_0 ; real_T B_55_914_0
; real_T B_55_915_0 ; real_T B_55_918_0 ; real_T B_55_1132_0 ; real_T
B_55_1133_0 ; real_T B_55_1134_0 ; real_T B_55_1135_0 [ 3 ] ; real_T
B_55_1136_0 ; real_T B_55_1137_0 ; real_T B_55_1138_0 ; real_T B_55_1139_0 [
3 ] ; real_T B_55_0_0_m [ 9 ] ; real_T B_55_1_0_c ; real_T B_55_2_0_k ;
real_T B_55_3_0_c ; real_T B_55_4_0_b ; real_T B_55_5_0_p ; real_T B_55_6_0_c
; real_T B_55_7_0_f ; real_T B_55_8_0_g ; real_T B_55_10_0 ; real_T
B_55_11_0_g ; real_T B_55_12_0_m ; real_T B_55_13_0_n ; real_T B_55_14_0_p [
36 ] ; real_T B_55_15_0_l ; real_T B_55_16_0_j [ 36 ] ; real_T B_55_17_0_d ;
real_T B_55_18_0_g [ 2 ] ; real_T B_55_19_0_l [ 2 ] ; real_T B_55_20_0_d [ 36
] ; real_T B_55_22_0 [ 36 ] ; real_T B_55_23_0_d [ 3 ] ; real_T B_55_24_0_l ;
real_T B_55_25_0_o ; real_T B_55_26_0_b ; real_T B_55_27_0_n ; real_T
B_55_28_0_b ; real_T B_55_29_0_l ; real_T B_55_30_0_h ; real_T B_55_31_0_b ;
real_T B_55_32_0_d ; real_T B_55_34_0_e ; real_T B_55_35_0_b ; real_T
B_55_36_0_j ; real_T B_55_37_0 ; real_T B_55_38_0_f ; real_T B_55_39_0 ;
real_T B_55_40_0_a ; real_T B_55_42_0_j ; real_T B_55_43_0_j ; real_T
B_55_44_0_o ; real_T B_55_45_0 ; real_T B_55_46_0_n ; real_T B_55_47_0 ;
real_T B_55_48_0_i ; real_T B_55_49_0_o ; real_T B_55_50_0_n ; real_T
B_55_51_0_m ; real_T B_55_52_0_c ; real_T B_55_54_0_m ; real_T B_55_55_0_m ;
real_T B_55_57_0_j ; real_T B_55_88_0_h ; real_T B_55_89_0_c ; real_T
B_55_90_0_c [ 3 ] ; real_T B_55_91_0_p ; real_T B_55_92_0 ; real_T B_55_93_0
[ 4 ] ; real_T B_55_94_0 ; real_T B_55_95_0 [ 4 ] ; real_T B_55_96_0 ; real_T
B_55_97_0 [ 4 ] ; real_T B_55_98_0 ; real_T B_55_99_0_p ; real_T B_55_100_0_a
; real_T B_55_101_0_e ; real_T B_55_105_0_a ; real_T B_48_0_0 [ 6 ] ; real_T
B_48_1_0 [ 36 ] ; real_T B_48_2_0 [ 36 ] ; real_T B_48_3_0 ; real_T B_48_4_0
[ 36 ] ; real_T B_48_5_0 [ 36 ] ; real_T B_48_6_0 [ 36 ] ; real_T B_48_7_0 [
36 ] ; real_T B_48_8_0 [ 36 ] ; real_T B_48_9_0 [ 36 ] ; real_T B_48_10_0 [ 6
] ; real_T B_48_11_0 [ 36 ] ; real_T B_48_12_0 [ 36 ] ; real_T B_48_13_0 [ 6
] ; real_T B_48_14_0 [ 6 ] ; real_T B_47_1_0 [ 3 ] ; real_T B_47_2_0 ; real_T
B_47_3_0 ; real_T B_47_4_0 ; real_T B_47_5_0 ; real_T B_47_6_0 ; real_T
B_47_7_0 ; real_T B_47_8_0 ; real_T B_47_9_0 ; real_T B_47_11_0 ; real_T
B_47_12_0 ; real_T B_47_14_0 ; real_T B_47_15_0 [ 36 ] ; real_T B_47_16_0 [
36 ] ; real_T B_47_17_0 [ 36 ] ; real_T B_47_18_0 [ 36 ] ; real_T B_47_19_0 [
36 ] ; real_T B_47_20_0 ; real_T B_47_3_0_a ; real_T B_47_4_0_i [ 2 ] ;
real_T B_47_6_0_l [ 36 ] ; real_T B_47_7_0_o [ 36 ] ; real_T B_47_8_0_o [ 36
] ; real_T B_47_9_0_i ; real_T B_47_10_0 ; real_T B_46_0_0 [ 3 ] ; real_T
B_46_1_0 ; real_T B_46_2_0 ; real_T B_46_3_0 ; real_T B_46_4_0 ; real_T
B_46_5_0 ; real_T B_46_6_0 ; real_T B_46_7_0 ; real_T B_46_8_0 ; real_T
B_46_10_0 ; real_T B_46_11_0 ; real_T B_46_0_0_f ; real_T B_46_1_0_i [ 3 ] ;
real_T B_45_0_0 ; real_T B_44_0_0 ; real_T B_43_0_0 ; real_T B_43_1_0 ;
real_T B_43_2_0 ; real_T B_43_3_0 ; real_T B_43_4_0 ; real_T B_43_5_0 ;
real_T B_43_6_0 [ 12 ] ; real_T B_43_0_0_f ; real_T B_43_1_0_g ; real_T
B_43_2_0_c ; real_T B_43_3_0_o ; real_T B_43_4_0_l ; real_T B_42_0_0 ; real_T
B_42_1_0 ; real_T B_41_0_0 ; real_T B_41_1_0 ; real_T B_40_0_0 ; real_T
B_40_1_0 ; real_T B_38_0_0 ; real_T B_38_1_0 ; real_T B_37_0_0 ; real_T
B_37_1_0 ; real_T B_36_0_0 ; real_T B_36_1_0 ; real_T B_35_1_0 ; real_T
B_35_2_0 ; real_T B_34_0_0 ; real_T B_33_1_0 ; real_T B_33_2_0 ; real_T
B_32_0_0 ; real_T B_31_1_0 ; real_T B_31_2_0 ; real_T B_30_0_0 ; real_T
B_29_0_0 ; real_T B_29_1_0 ; real_T B_28_0_0 ; real_T B_28_1_0 ; real_T
B_27_0_0 ; real_T B_27_1_0 ; real_T B_26_0_0 ; real_T B_26_1_0 ; real_T
B_25_0_0 ; real_T B_25_1_0 ; real_T B_24_0_0 ; real_T B_24_1_0 ; real_T
B_23_0_0 ; real_T B_23_1_0 ; real_T B_22_0_0 ; real_T B_22_1_0 ; real_T
B_21_0_0 ; real_T B_21_1_0 ; real_T B_20_0_0 ; real_T B_20_1_0 ; real_T
B_19_0_0 ; real_T B_19_1_0 ; real_T B_18_0_0 ; real_T B_18_1_0 ; real_T
B_17_0_0 ; real_T B_17_1_0 ; real_T B_16_0_0 ; real_T B_16_1_0 ; real_T
B_15_0_0 ; real_T B_15_1_0 ; real_T B_14_0_0 ; real_T B_14_1_0 ; real_T
B_13_0_0 ; real_T B_13_1_0 ; real_T B_12_0_0 ; real_T B_12_1_0 ; real_T
B_11_0_0 ; real_T B_11_1_0 ; real_T B_10_0_0 ; real_T B_10_1_0 ; real_T
B_9_0_0 ; real_T B_9_1_0 ; real_T B_8_0_0 ; real_T B_8_1_0 ; real_T B_7_0_0 ;
real_T B_7_1_0 ; real_T B_6_0_0 ; real_T B_6_1_0 ; real_T B_5_0_0 ; real_T
B_5_1_0 ; real_T B_4_0_0 ; real_T B_4_1_0 ; real_T B_3_0_0 ; real_T B_3_1_0 ;
real_T B_2_0_0 ; real_T B_2_1_0 ; real_T B_1_0_0 ; real_T B_0_0_0 ; boolean_T
B_55_37_0_m ; boolean_T B_55_45_0_m ; boolean_T B_55_56_0 ; boolean_T
B_55_64_0 ; boolean_T B_55_111_0 ; boolean_T B_55_119_0 ; boolean_T
B_55_130_0 ; boolean_T B_55_138_0 ; boolean_T B_55_185_0 ; boolean_T
B_55_193_0 ; boolean_T B_55_204_0 ; boolean_T B_55_212_0 ; boolean_T
B_55_259_0 ; boolean_T B_55_267_0 ; boolean_T B_55_278_0 ; boolean_T
B_55_286_0 ; boolean_T B_55_333_0 ; boolean_T B_55_341_0 ; boolean_T
B_55_352_0 ; boolean_T B_55_360_0 ; boolean_T B_55_407_0 ; boolean_T
B_55_415_0 ; boolean_T B_55_426_0 ; boolean_T B_55_434_0 ; boolean_T
B_55_481_0 ; boolean_T B_55_489_0 ; boolean_T B_55_500_0 ; boolean_T
B_55_508_0 ; boolean_T B_55_685_0 ; boolean_T B_55_688_0 ; boolean_T
B_55_689_0 ; boolean_T B_55_692_0 ; boolean_T B_55_695_0 ; boolean_T
B_55_747_0 ; boolean_T B_55_817_0 ; boolean_T B_55_818_0 ; boolean_T
B_55_898_0 ; boolean_T B_55_9_0_c ; boolean_T B_55_33_0_f ; boolean_T
B_55_102_0_p ; boolean_T B_55_103_0_e ; boolean_T B_55_104_0_o ; boolean_T
B_47_0_0 ; boolean_T B_47_0_0_h ; boolean_T B_47_1_0_l ; boolean_T B_47_2_0_h
; boolean_T B_39_0_0 ; boolean_T B_35_0_0 ; boolean_T B_33_0_0 ; boolean_T
B_31_0_0 ; char_T pad_B_31_0_0 [ 6 ] ;
B_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
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
Transferfunctionsfordetailedstabilizer ; } B_DZG_v6_SimGen_T ; typedef struct
{ real_T UnitDelay_DSTATE [ 12 ] ; real_T Rotorangledthetae_DSTATE ; real_T
fluxes_DSTATE [ 6 ] ; real_T StateSpace_DSTATE [ 9 ] ; real_T dw_delay_DSTATE
; real_T dw_predict_DSTATE ; real_T theta_DSTATE ; real_T UnitDelay2_DSTATE ;
real_T voltages_DSTATE [ 6 ] ; real_T Rotorspeeddeviationdw_DSTATE ; real_T
DiscreteStateSpace_DSTATE [ 2 ] ; real_T DiscreteStateSpace_DSTATE_j ; real_T
DiscreteStateSpace_DSTATE_b ; real_T DiscreteStateSpace_DSTATE_o ; real_T
DiscreteStateSpace_DSTATE_f ; real_T DiscreteStateSpace_DSTATE_bk [ 3 ] ;
real_T DiscreteStateSpace_DSTATE_a ; real_T DiscreteStateSpace_DSTATE_l ;
real_T DiscreteStateSpace_DSTATE_i ; real_T UnitDelay1_DSTATE ; real_T
DiscreteStateSpace_DSTATE_h ; real_T DiscreteStateSpace_DSTATE_be ; real_T
DiscreteStateSpace_DSTATE_n ; real_T Lmd_sat_DSTATE ; real_T Lmq_sat_DSTATE ;
real_T lastSin ; real_T lastCos ; real_T lastSin_m ; real_T lastCos_f ;
real_T lastSin_b ; real_T lastCos_d ; real_T Memory_PreviousInput ; real_T
Memory_PreviousInput_p ; real_T Memory_PreviousInput_l ; real_T
Memory_PreviousInput_po ; real_T Memory_PreviousInput_n ; real_T
Memory_PreviousInput_j ; real_T Memory_PreviousInput_g ; real_T
Memory_PreviousInput_e ; real_T Memory_PreviousInput_d ; real_T
Memory_PreviousInput_c ; real_T Memory_PreviousInput_h ; real_T
Memory_PreviousInput_m ; real_T Memory_PreviousInput_nd ; real_T
Memory_PreviousInput_cy ; real_T Memory_PreviousInput_a ; real_T
Memory_PreviousInput_c2 ; real_T Memory_PreviousInput_dh ; real_T
Memory_PreviousInput_i ; real_T Memory_PreviousInput_di ; real_T
Memory_PreviousInput_gk ; real_T Memory_PreviousInput_jh ; real_T
Memory_PreviousInput_il ; real_T Memory_PreviousInput_gc ; real_T
Memory_PreviousInput_o ; real_T Memory_PreviousInput_ct ; real_T
Memory_PreviousInput_ek ; real_T Memory_PreviousInput_mp ; real_T
Memory_PreviousInput_k ; real_T RateTransition1_Buffer0 ; real_T
inversion_DWORK1 [ 36 ] ; real_T inversion_DWORK3 [ 36 ] ; real_T
inversion_DWORK4 [ 36 ] ; real_T inversion_DWORK4_b [ 36 ] ; real_T
StateSpace_RWORK ; struct { real_T modelTStart ; } TransportDelay_RWORK ;
struct { real_T modelTStart ; } TransportDelay_RWORK_h ; struct { real_T
modelTStart ; } TransportDelay_RWORK_d ; struct { real_T modelTStart ; }
TransportDelay_RWORK_b ; struct { real_T modelTStart ; }
TransportDelay_RWORK_a ; struct { real_T modelTStart ; }
TransportDelay_RWORK_l ; struct { real_T modelTStart ; }
TransportDelay_RWORK_f ; struct { real_T modelTStart ; }
TransportDelay_RWORK_fo ; struct { real_T modelTStart ; }
TransportDelay_RWORK_o ; struct { real_T modelTStart ; }
TransportDelay_RWORK_ap ; struct { real_T modelTStart ; }
TransportDelay_RWORK_p ; struct { real_T modelTStart ; }
TransportDelay_RWORK_fy ; struct { real_T modelTStart ; }
TransportDelay_RWORK_i ; struct { real_T modelTStart ; }
TransportDelay_RWORK_e ; struct { real_T modelTStart ; }
TransportDelay_RWORK_k ; struct { real_T modelTStart ; }
TransportDelay_RWORK_n ; struct { real_T modelTStart ; }
TransportDelay_RWORK_g ; struct { real_T modelTStart ; }
TransportDelay_RWORK_ai ; struct { real_T modelTStart ; }
TransportDelay_RWORK_j ; struct { real_T modelTStart ; }
TransportDelay_RWORK_n2 ; struct { real_T modelTStart ; }
TransportDelay_RWORK_gn ; struct { real_T modelTStart ; }
TransportDelay_RWORK_ge ; struct { real_T modelTStart ; }
TransportDelay_RWORK_fb ; struct { real_T modelTStart ; }
TransportDelay_RWORK_pl ; struct { real_T modelTStart ; }
TransportDelay_RWORK_aw ; struct { real_T modelTStart ; }
TransportDelay_RWORK_l2 ; struct { real_T modelTStart ; }
TransportDelay_RWORK_eb ; struct { real_T modelTStart ; }
TransportDelay_RWORK_m ; struct { real_T modelTStart ; }
TransportDelay_RWORK_l0 ; void * StateSpace_PWORK [ 65 ] ; struct { void * AS
; void * BS ; void * CS ; void * DS ; void * DX_COL ; void * BD_COL ; void *
TMP1 ; void * TMP2 ; void * XTMP ; void * SWITCH_STATUS ; void *
SWITCH_STATUS_INIT ; void * SW_CHG ; void * G_STATE ; void * USWLAST ; void *
XKM12 ; void * XKP12 ; void * XLAST ; void * ULAST ; void * IDX_SW_CHG ; void
* Y_SWITCH ; void * SWITCH_TYPES ; void * IDX_OUT_SW ; void *
SWITCH_TOPO_SAVED_IDX ; void * SWITCH_MAP ; } StateSpace_PWORK_n ; struct {
void * TUbufferPtrs [ 2 ] ; } TransportDelay_PWORK ; struct { void *
TUbufferPtrs [ 2 ] ; } TransportDelay_PWORK_k ; struct { void * TUbufferPtrs
[ 2 ] ; } TransportDelay_PWORK_f ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_d ; void * Additional3_PWORK [ 2 ] ; void *
Additional4_PWORK [ 2 ] ; void * EffRangeG1_PWORK [ 2 ] ; void *
InstValG1_PWORK [ 2 ] ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_a ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_l ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_fp ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_g ; void * Additional5_PWORK [ 2 ] ; void *
Additional6_PWORK [ 2 ] ; void * EffRangeG2_PWORK [ 2 ] ; void *
InstValG2_PWORK [ 2 ] ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_j ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_lk ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_e ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_k2 ; void * Additional13_PWORK [ 2 ] ; void *
Additional14_PWORK [ 2 ] ; void * EffRangeG6_PWORK [ 2 ] ; void *
InstValG6_PWORK [ 2 ] ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_i ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_o ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_b ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_c ; void * Additional7_PWORK [ 2 ] ; void *
Additional8_PWORK [ 2 ] ; void * EffRangeG3_PWORK [ 2 ] ; void *
InstValG3_PWORK [ 2 ] ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_fx ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_eg ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_e0 ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_n ; void * Additional10_PWORK [ 2 ] ; void *
Additional9_PWORK [ 2 ] ; void * EffRangeG4_PWORK [ 2 ] ; void *
InstValG4_PWORK [ 2 ] ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_h ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_jr ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_j3 ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_dm ; void * Additional11_PWORK [ 2 ] ; void *
Additional12_PWORK [ 2 ] ; void * EffRangeG5_PWORK [ 2 ] ; void *
InstValG5_PWORK [ 2 ] ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_lg ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_dw ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_m ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_el ; void * Additional_PWORK [ 2 ] ; void *
Additional1_PWORK [ 2 ] ; void * EffRangeG_PWORK [ 2 ] ; void *
InstValG_PWORK [ 2 ] ; void * Additional15_PWORK [ 3 ] ; void *
Additional2_PWORK [ 2 ] ; struct { void * TUbufferPtrs [ 2 ] ; }
TransportDelay_PWORK_aw ; void * ToWorkspace_PWORK ; int32_T systemEnable ;
int32_T systemEnable_m ; int32_T systemEnable_e ; int32_T
TmpAtomicSubsysAtICInport1_sysIdxToRun ; int32_T inversion_DWORK2 [ 6 ] ;
int32_T Saturation_sysIdxToRun ; int32_T Lmq_sat_sysIdxToRun ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_d ; int32_T
Registrator_sysIdxToRun ; int32_T TmpAtomicSubsysAtSwitch3Inport1_sysIdxToRun
; int32_T TmpAtomicSubsysAtSwitch3Inport1_sysIdxToRun_o ; int32_T
TmpAtomicSubsysAtSwitch3Inport1_sysIdxToRun_p ; int32_T
TmpAtomicSubsysAtSwitch3Inport1_sysIdxToRun_f ; int32_T
TmpAtomicSubsysAtSwitch3Inport1_sysIdxToRun_c ; int32_T
TmpAtomicSubsysAtSwitch3Inport1_sysIdxToRun_n ; int32_T
TmpAtomicSubsysAtSwitch3Inport1_sysIdxToRun_d ; int32_T
TmpAtomicSubsysAtSwitch3Inport1_sysIdxToRun_f5 ; int32_T
TmpAtomicSubsysAtSwitch3Inport3_sysIdxToRun ; int32_T
TmpAtomicSubsysAtSwitch3Inport1_sysIdxToRun_l ; int32_T
TmpAtomicSubsysAtSwitch3Inport3_sysIdxToRun_c ; int32_T
TmpAtomicSubsysAtSwitch3Inport1_sysIdxToRun_j ; int32_T
TmpAtomicSubsysAtSwitch3Inport3_sysIdxToRun_f ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_p ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_a ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_j ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_k ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_b ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_c ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_g ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_g3 ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_kj ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_n ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_i ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_b5 ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_h ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_pu ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_o ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_ch ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_e ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_ns ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_hu ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_l ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_f ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_hb ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_lk ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_fb ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_ki ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_ph ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_h4 ; int32_T
TmpAtomicSubsysAtSwitchInport1_sysIdxToRun_do ; int32_T
TmpAtomicSubsysAt1Inport1_sysIdxToRun ; int32_T
TmpAtomicSubsysAt1Inport3_sysIdxToRun ; uint32_T m_bpIndex ; uint32_T
m_bpIndex_o ; int_T StateSpace_IWORK [ 12 ] ; int_T StateSpace_IWORK_a [ 11 ]
; struct { int_T Tail ; int_T Head ; int_T Last ; int_T CircularBufSize ;
int_T MaxNewBufSize ; } TransportDelay_IWORK ; struct { int_T Tail ; int_T
Head ; int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
TransportDelay_IWORK_c ; struct { int_T Tail ; int_T Head ; int_T Last ;
int_T CircularBufSize ; int_T MaxNewBufSize ; } TransportDelay_IWORK_e ;
struct { int_T Tail ; int_T Head ; int_T Last ; int_T CircularBufSize ; int_T
MaxNewBufSize ; } TransportDelay_IWORK_j ; struct { int_T Tail ; int_T Head ;
int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
TransportDelay_IWORK_eq ; struct { int_T Tail ; int_T Head ; int_T Last ;
int_T CircularBufSize ; int_T MaxNewBufSize ; } TransportDelay_IWORK_e5 ;
struct { int_T Tail ; int_T Head ; int_T Last ; int_T CircularBufSize ; int_T
MaxNewBufSize ; } TransportDelay_IWORK_g ; struct { int_T Tail ; int_T Head ;
int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
TransportDelay_IWORK_m ; struct { int_T Tail ; int_T Head ; int_T Last ;
int_T CircularBufSize ; int_T MaxNewBufSize ; } TransportDelay_IWORK_o ;
struct { int_T Tail ; int_T Head ; int_T Last ; int_T CircularBufSize ; int_T
MaxNewBufSize ; } TransportDelay_IWORK_l ; struct { int_T Tail ; int_T Head ;
int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
TransportDelay_IWORK_cm ; struct { int_T Tail ; int_T Head ; int_T Last ;
int_T CircularBufSize ; int_T MaxNewBufSize ; } TransportDelay_IWORK_p ;
struct { int_T Tail ; int_T Head ; int_T Last ; int_T CircularBufSize ; int_T
MaxNewBufSize ; } TransportDelay_IWORK_j1 ; struct { int_T Tail ; int_T Head
; int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
TransportDelay_IWORK_ln ; struct { int_T Tail ; int_T Head ; int_T Last ;
int_T CircularBufSize ; int_T MaxNewBufSize ; } TransportDelay_IWORK_f ;
struct { int_T Tail ; int_T Head ; int_T Last ; int_T CircularBufSize ; int_T
MaxNewBufSize ; } TransportDelay_IWORK_gl ; struct { int_T Tail ; int_T Head
; int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
TransportDelay_IWORK_k ; struct { int_T Tail ; int_T Head ; int_T Last ;
int_T CircularBufSize ; int_T MaxNewBufSize ; } TransportDelay_IWORK_od ;
struct { int_T Tail ; int_T Head ; int_T Last ; int_T CircularBufSize ; int_T
MaxNewBufSize ; } TransportDelay_IWORK_ed ; struct { int_T Tail ; int_T Head
; int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
TransportDelay_IWORK_h ; struct { int_T Tail ; int_T Head ; int_T Last ;
int_T CircularBufSize ; int_T MaxNewBufSize ; } TransportDelay_IWORK_d ;
struct { int_T Tail ; int_T Head ; int_T Last ; int_T CircularBufSize ; int_T
MaxNewBufSize ; } TransportDelay_IWORK_kn ; struct { int_T Tail ; int_T Head
; int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
TransportDelay_IWORK_gj ; struct { int_T Tail ; int_T Head ; int_T Last ;
int_T CircularBufSize ; int_T MaxNewBufSize ; } TransportDelay_IWORK_lu ;
struct { int_T Tail ; int_T Head ; int_T Last ; int_T CircularBufSize ; int_T
MaxNewBufSize ; } TransportDelay_IWORK_n ; struct { int_T Tail ; int_T Head ;
int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
TransportDelay_IWORK_dz ; struct { int_T Tail ; int_T Head ; int_T Last ;
int_T CircularBufSize ; int_T MaxNewBufSize ; } TransportDelay_IWORK_er ;
struct { int_T Tail ; int_T Head ; int_T Last ; int_T CircularBufSize ; int_T
MaxNewBufSize ; } TransportDelay_IWORK_eb ; struct { int_T Tail ; int_T Head
; int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
TransportDelay_IWORK_l4 ; int8_T RateTransition1_semaphoreTaken ; int8_T
Saturation_SubsysRanBC ; int8_T Lmq_sat_SubsysRanBC ; int8_T
Registrator_SubsysRanBC ; uint8_T Rotorangledthetae_NumInitCond ; uint8_T
theta_NumInitCond ; uint8_T Rotorspeeddeviationdw_SYSTEM_ENABLE ; uint8_T
Rotorspeeddeviationdw_NumInitCond ; boolean_T Registrator_MODE ; char_T
pad_Registrator_MODE [ 7 ] ;
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
Transferfunctionsfordetailedstabilizer ; } DW_DZG_v6_SimGen_T ; typedef
struct { real_T integrator_CSTATE ; real_T integrator_CSTATE_l ; real_T
integrator_CSTATE_i ; real_T integrator_CSTATE_f ; real_T
integrator_CSTATE_ly ; real_T integrator_CSTATE_c ; real_T
integrator_CSTATE_j ; real_T integrator_CSTATE_m ; real_T integrator_CSTATE_o
; real_T integrator_CSTATE_d ; real_T integrator_CSTATE_jx ; real_T
integrator_CSTATE_k ; real_T integrator_CSTATE_kg ; real_T
integrator_CSTATE_fi ; real_T integrator_CSTATE_e ; real_T
integrator_CSTATE_a ; real_T integrator_CSTATE_ol ; real_T
integrator_CSTATE_h ; real_T integrator_CSTATE_b ; real_T
integrator_CSTATE_jb ; real_T integrator_CSTATE_df ; real_T
integrator_CSTATE_p ; real_T integrator_CSTATE_he ; real_T
integrator_CSTATE_hj ; real_T integrator_CSTATE_n ; real_T
integrator_CSTATE_iy ; real_T integrator_CSTATE_jz ; real_T
integrator_CSTATE_lg ; real_T Integrator2_CSTATE ; real_T Integrator1_CSTATE
; real_T Integrator3_CSTATE ; real_T IntegratorLimited_CSTATE ; real_T
Integrator1_CSTATE_i ; real_T Integrator1_CSTATE_d ; real_T
Integrator2_CSTATE_f ; real_T Integrator1_CSTATE_c ; real_T
TransferFcn1_CSTATE ; real_T Integrator1_CSTATE_l ; real_T TransferFcn_CSTATE
; } X_DZG_v6_SimGen_T ; typedef struct { real_T integrator_CSTATE ; real_T
integrator_CSTATE_l ; real_T integrator_CSTATE_i ; real_T integrator_CSTATE_f
; real_T integrator_CSTATE_ly ; real_T integrator_CSTATE_c ; real_T
integrator_CSTATE_j ; real_T integrator_CSTATE_m ; real_T integrator_CSTATE_o
; real_T integrator_CSTATE_d ; real_T integrator_CSTATE_jx ; real_T
integrator_CSTATE_k ; real_T integrator_CSTATE_kg ; real_T
integrator_CSTATE_fi ; real_T integrator_CSTATE_e ; real_T
integrator_CSTATE_a ; real_T integrator_CSTATE_ol ; real_T
integrator_CSTATE_h ; real_T integrator_CSTATE_b ; real_T
integrator_CSTATE_jb ; real_T integrator_CSTATE_df ; real_T
integrator_CSTATE_p ; real_T integrator_CSTATE_he ; real_T
integrator_CSTATE_hj ; real_T integrator_CSTATE_n ; real_T
integrator_CSTATE_iy ; real_T integrator_CSTATE_jz ; real_T
integrator_CSTATE_lg ; real_T Integrator2_CSTATE ; real_T Integrator1_CSTATE
; real_T Integrator3_CSTATE ; real_T IntegratorLimited_CSTATE ; real_T
Integrator1_CSTATE_i ; real_T Integrator1_CSTATE_d ; real_T
Integrator2_CSTATE_f ; real_T Integrator1_CSTATE_c ; real_T
TransferFcn1_CSTATE ; real_T Integrator1_CSTATE_l ; real_T TransferFcn_CSTATE
; } XDot_DZG_v6_SimGen_T ; typedef struct { boolean_T integrator_CSTATE ;
boolean_T integrator_CSTATE_l ; boolean_T integrator_CSTATE_i ; boolean_T
integrator_CSTATE_f ; boolean_T integrator_CSTATE_ly ; boolean_T
integrator_CSTATE_c ; boolean_T integrator_CSTATE_j ; boolean_T
integrator_CSTATE_m ; boolean_T integrator_CSTATE_o ; boolean_T
integrator_CSTATE_d ; boolean_T integrator_CSTATE_jx ; boolean_T
integrator_CSTATE_k ; boolean_T integrator_CSTATE_kg ; boolean_T
integrator_CSTATE_fi ; boolean_T integrator_CSTATE_e ; boolean_T
integrator_CSTATE_a ; boolean_T integrator_CSTATE_ol ; boolean_T
integrator_CSTATE_h ; boolean_T integrator_CSTATE_b ; boolean_T
integrator_CSTATE_jb ; boolean_T integrator_CSTATE_df ; boolean_T
integrator_CSTATE_p ; boolean_T integrator_CSTATE_he ; boolean_T
integrator_CSTATE_hj ; boolean_T integrator_CSTATE_n ; boolean_T
integrator_CSTATE_iy ; boolean_T integrator_CSTATE_jz ; boolean_T
integrator_CSTATE_lg ; boolean_T Integrator2_CSTATE ; boolean_T
Integrator1_CSTATE ; boolean_T Integrator3_CSTATE ; boolean_T
IntegratorLimited_CSTATE ; boolean_T Integrator1_CSTATE_i ; boolean_T
Integrator1_CSTATE_d ; boolean_T Integrator2_CSTATE_f ; boolean_T
Integrator1_CSTATE_c ; boolean_T TransferFcn1_CSTATE ; boolean_T
Integrator1_CSTATE_l ; boolean_T TransferFcn_CSTATE ; } XDis_DZG_v6_SimGen_T
; struct P_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T_ { real_T
P_0 ; real_T P_4 ; real_T P_5 ; real_T P_9 ; real_T P_10 ; } ; struct
P_DZG_v6_SimGen_T_ { real_T P_0 ; real_T P_1 ; real_T P_2 ; real_T P_3 ;
real_T P_4 ; real_T P_5 ; real_T P_6 ; real_T P_7 ; real_T P_8 ; real_T P_9 ;
real_T P_10 ; real_T P_11 ; real_T P_12 ; real_T P_13 ; real_T P_14 ; real_T
P_15 ; real_T P_16 ; real_T P_17 ; real_T P_18 ; real_T P_19 ; real_T P_20 ;
real_T P_21 ; real_T P_22 ; real_T P_23 ; real_T P_24 ; real_T P_25 ; real_T
P_26 ; real_T P_27 ; real_T P_28 ; real_T P_29 ; real_T P_30 [ 5 ] ; real_T
P_31 [ 5 ] ; real_T P_32 [ 5 ] ; real_T P_33 [ 5 ] ; real_T P_34 [ 5 ] ;
real_T P_35 [ 5 ] ; real_T P_36 ; real_T P_37 ; real_T P_38 ; real_T P_39 ;
real_T P_40 ; real_T P_41 ; real_T P_42 ; real_T P_43 ; real_T P_44 ; real_T
P_45 ; real_T P_46 ; real_T P_47 ; real_T P_48 ; real_T P_49 ; real_T P_50 ;
real_T P_51 ; real_T P_52 ; real_T P_53 ; real_T P_54 ; real_T P_55 [ 3 ] ;
real_T P_56 ; real_T P_57 [ 2 ] ; real_T P_58 [ 2 ] ; real_T P_59 ; real_T
P_60 ; real_T P_61 [ 3 ] ; real_T P_62 ; real_T P_63 ; real_T P_64 ; real_T
P_65 ; real_T P_66 [ 3 ] ; real_T P_67 ; real_T P_68 [ 2 ] ; real_T P_69 [ 2
] ; real_T P_70 ; real_T P_71 ; real_T P_72 [ 2 ] ; real_T P_73 [ 36 ] ;
real_T P_74 [ 36 ] ; real_T P_75 [ 36 ] ; real_T P_76 ; real_T P_77 ; real_T
P_78 ; real_T P_79 ; real_T P_80 ; real_T P_81 ; real_T P_82 ; real_T P_83 [
2 ] ; real_T P_84 ; real_T P_85 [ 2 ] ; real_T P_87 [ 2 ] ; real_T P_89 [ 2 ]
; real_T P_91 [ 2 ] ; real_T P_92 [ 36 ] ; real_T P_93 [ 2 ] ; real_T P_95 [
2 ] ; real_T P_96 ; real_T P_97 [ 2 ] ; real_T P_98 [ 9 ] ; real_T P_99 [ 2 ]
; real_T P_101 [ 2 ] ; real_T P_102 [ 6 ] ; real_T P_103 [ 2 ] ; real_T P_104
[ 6 ] ; real_T P_105 [ 2 ] ; real_T P_107 [ 2 ] ; real_T P_108 [ 3 ] ; real_T
P_109 [ 2 ] ; real_T P_110 [ 3 ] ; real_T P_111 [ 2 ] ; real_T P_113 [ 2 ] ;
real_T P_114 [ 3 ] ; real_T P_115 [ 2 ] ; real_T P_116 ; real_T P_117 [ 2 ] ;
real_T P_118 ; real_T P_119 [ 2 ] ; real_T P_120 ; real_T P_121 ; real_T
P_122 ; real_T P_123 ; real_T P_124 [ 6 ] ; real_T P_125 ; real_T P_126 [ 6 ]
; real_T P_127 ; real_T P_128 ; real_T P_129 ; real_T P_130 ; real_T P_131 ;
real_T P_132 ; real_T P_133 ; real_T P_134 ; real_T P_135 ; real_T P_136 ;
real_T P_137 ; real_T P_138 ; real_T P_139 ; real_T P_140 ; real_T P_141 ;
real_T P_142 ; real_T P_143 ; real_T P_144 ; real_T P_145 ; real_T P_146 ;
real_T P_147 ; real_T P_148 ; real_T P_149 ; real_T P_150 ; real_T P_151 ;
real_T P_152 ; real_T P_153 ; real_T P_154 ; real_T P_155 ; real_T P_156 ;
real_T P_157 ; real_T P_158 ; real_T P_159 [ 2 ] ; real_T P_160 [ 81 ] ;
real_T P_161 [ 2 ] ; real_T P_162 [ 153 ] ; real_T P_163 [ 2 ] ; real_T P_164
[ 531 ] ; real_T P_165 [ 2 ] ; real_T P_166 [ 1003 ] ; real_T P_167 [ 2 ] ;
real_T P_168 [ 9 ] ; real_T P_169 ; real_T P_170 ; real_T P_171 ; real_T
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
P_272 ; real_T P_273 ; real_T P_274 ; real_T P_275 ; real_T P_276 ; real_T
P_277 ; real_T P_278 ; real_T P_279 ; real_T P_280 ; real_T P_281 ; real_T
P_282 ; real_T P_283 ; real_T P_284 ; real_T P_285 ; real_T P_286 ; real_T
P_287 ; real_T P_288 ; real_T P_289 ; real_T P_290 ; real_T P_291 ; real_T
P_292 ; real_T P_293 ; real_T P_294 ; real_T P_295 ; real_T P_296 ; real_T
P_297 ; real_T P_298 ; real_T P_299 ; real_T P_300 ; real_T P_301 ; real_T
P_302 ; real_T P_303 ; real_T P_304 ; real_T P_305 ; real_T P_306 ; real_T
P_307 ; real_T P_308 ; real_T P_309 ; real_T P_310 ; real_T P_311 ; real_T
P_312 ; real_T P_313 ; real_T P_314 ; real_T P_315 ; real_T P_316 ; real_T
P_317 ; real_T P_318 ; real_T P_319 ; real_T P_320 ; real_T P_321 ; real_T
P_322 ; real_T P_323 ; real_T P_324 ; real_T P_325 ; real_T P_326 ; real_T
P_327 ; real_T P_328 ; real_T P_329 ; real_T P_330 ; real_T P_331 ; real_T
P_332 ; real_T P_333 ; real_T P_334 ; real_T P_335 ; real_T P_336 ; real_T
P_337 ; real_T P_338 ; real_T P_339 ; real_T P_340 ; real_T P_341 ; real_T
P_342 ; real_T P_343 ; real_T P_344 ; real_T P_345 ; real_T P_346 ; real_T
P_347 ; real_T P_348 ; real_T P_349 ; real_T P_350 ; real_T P_351 ; real_T
P_352 ; real_T P_353 ; real_T P_354 ; real_T P_355 ; real_T P_356 ; real_T
P_357 ; real_T P_358 ; real_T P_359 ; real_T P_360 ; real_T P_361 ; real_T
P_362 ; real_T P_363 ; real_T P_364 ; real_T P_365 ; real_T P_366 ; real_T
P_367 ; real_T P_368 ; real_T P_369 ; real_T P_370 ; real_T P_371 ; real_T
P_372 ; real_T P_373 ; real_T P_374 ; real_T P_375 ; real_T P_376 ; real_T
P_377 ; real_T P_378 ; real_T P_379 ; real_T P_380 ; real_T P_381 ; real_T
P_382 ; real_T P_383 ; real_T P_384 ; real_T P_385 ; real_T P_386 ; real_T
P_387 ; real_T P_388 ; real_T P_389 ; real_T P_390 ; real_T P_391 ; real_T
P_392 ; real_T P_393 ; real_T P_394 ; real_T P_395 ; real_T P_396 ; real_T
P_397 ; real_T P_398 ; real_T P_399 ; real_T P_400 ; real_T P_401 ; real_T
P_402 ; real_T P_403 ; real_T P_404 ; real_T P_405 ; real_T P_406 ; real_T
P_407 ; real_T P_408 ; real_T P_409 ; real_T P_410 ; real_T P_411 ; real_T
P_412 ; real_T P_413 ; real_T P_414 ; real_T P_415 ; real_T P_416 ; real_T
P_417 ; real_T P_418 ; real_T P_419 ; real_T P_420 ; real_T P_421 ; real_T
P_422 ; real_T P_423 ; real_T P_424 ; real_T P_425 ; real_T P_426 ; real_T
P_427 ; real_T P_428 ; real_T P_429 ; real_T P_430 ; real_T P_431 ; real_T
P_432 ; real_T P_433 ; real_T P_434 ; real_T P_435 ; real_T P_436 ; real_T
P_437 ; real_T P_438 ; real_T P_439 ; real_T P_440 ; real_T P_441 ; real_T
P_442 ; real_T P_443 ; real_T P_444 ; real_T P_445 ; real_T P_446 ; real_T
P_447 ; real_T P_448 ; real_T P_449 ; real_T P_450 ; real_T P_451 ; real_T
P_452 ; real_T P_453 ; real_T P_454 ; real_T P_455 ; real_T P_456 ; real_T
P_457 ; real_T P_458 ; real_T P_459 ; real_T P_460 ; real_T P_461 ; real_T
P_462 ; real_T P_463 ; real_T P_464 ; real_T P_465 ; real_T P_466 ; real_T
P_467 ; real_T P_468 ; real_T P_469 ; real_T P_470 ; real_T P_471 ; real_T
P_472 ; real_T P_473 ; real_T P_474 ; real_T P_475 ; real_T P_476 ; real_T
P_477 ; real_T P_478 ; real_T P_479 ; real_T P_480 ; real_T P_481 ; real_T
P_482 ; real_T P_483 ; real_T P_484 ; real_T P_485 ; real_T P_486 ; real_T
P_487 ; real_T P_488 ; real_T P_489 ; real_T P_490 ; real_T P_491 ; real_T
P_492 ; real_T P_493 ; real_T P_494 ; real_T P_495 ; real_T P_496 ; real_T
P_497 ; real_T P_498 ; real_T P_499 ; real_T P_500 ; real_T P_501 ; real_T
P_502 ; real_T P_503 ; real_T P_504 ; real_T P_505 ; real_T P_506 ; real_T
P_507 ; real_T P_508 ; real_T P_509 ; real_T P_510 ; real_T P_511 ; real_T
P_512 ; real_T P_513 ; real_T P_514 ; real_T P_515 ; real_T P_516 ; real_T
P_517 ; real_T P_518 ; real_T P_519 ; real_T P_520 ; real_T P_521 ; real_T
P_522 ; real_T P_523 ; real_T P_524 ; real_T P_525 ; real_T P_526 ; real_T
P_527 ; real_T P_528 ; real_T P_529 ; real_T P_530 [ 2 ] ; real_T P_531 ;
real_T P_532 ; real_T P_533 ; real_T P_534 ; real_T P_535 ; real_T P_536 ;
real_T P_537 ; real_T P_538 ; real_T P_539 ; real_T P_540 ; real_T P_541 ;
real_T P_542 ; real_T P_543 ; real_T P_544 [ 3 ] ; real_T P_545 ; real_T
P_546 [ 3 ] ; real_T P_547 ; real_T P_548 ; real_T P_549 ; real_T P_550 ;
real_T P_551 ; real_T P_552 [ 18 ] ; real_T P_553 ; real_T P_554 ; real_T
P_555 ; real_T P_556 ; real_T P_557 ; real_T P_558 ; real_T P_559 ; real_T
P_560 ; real_T P_561 ; real_T P_562 ; real_T P_563 ; real_T P_564 ; real_T
P_565 [ 4 ] ; real_T P_566 [ 2 ] ; real_T P_567 [ 2 ] ; real_T P_568 ; real_T
P_569 ; real_T P_570 ; real_T P_571 ; real_T P_572 ; real_T P_573 ; real_T
P_574 ; real_T P_575 ; real_T P_576 ; real_T P_577 ; real_T P_578 ; real_T
P_579 ; real_T P_580 ; real_T P_581 ; real_T P_582 ; real_T P_583 ; real_T
P_584 ; real_T P_585 ; real_T P_586 ; real_T P_587 ; real_T P_588 ; real_T
P_589 ; real_T P_590 ; real_T P_591 ; real_T P_592 ; real_T P_593 ; real_T
P_594 [ 9 ] ; real_T P_595 [ 3 ] ; real_T P_596 [ 3 ] ; real_T P_597 ; real_T
P_598 ; real_T P_599 ; real_T P_600 ; real_T P_601 ; real_T P_602 ; real_T
P_603 ; real_T P_604 ; real_T P_605 ; real_T P_606 ; real_T P_607 ; real_T
P_608 ; real_T P_609 ; real_T P_610 ; real_T P_611 ; real_T P_612 ; real_T
P_613 ; real_T P_614 ; real_T P_615 ; real_T P_616 ; real_T P_617 ; real_T
P_618 ; real_T P_619 ; real_T P_620 ; real_T P_621 ; real_T P_622 ; real_T
P_623 ; real_T P_624 ; real_T P_625 ; real_T P_626 ; real_T P_627 ; real_T
P_628 ; real_T P_629 ; real_T P_630 ; real_T P_631 ; real_T P_632 ; real_T
P_633 ; real_T P_634 ; real_T P_638 ; real_T P_640 ; real_T P_641 ; real_T
P_642 ; real_T P_643 ; real_T P_644 ; real_T P_645 ; real_T P_646 ; real_T
P_647 ; real_T P_648 ; real_T P_649 ; real_T P_650 ; real_T P_651 ; real_T
P_652 ; real_T P_653 ; real_T P_654 ; real_T P_655 ; real_T P_656 ; real_T
P_657 ; real_T P_658 ; real_T P_659 ; real_T P_660 ; real_T P_661 ; real_T
P_662 ; real_T P_663 ; real_T P_664 ; real_T P_665 ; real_T P_666 ; real_T
P_667 ; real_T P_668 ; real_T P_669 ; real_T P_670 ; real_T P_671 ; real_T
P_672 ; real_T P_673 ; real_T P_674 ; real_T P_675 ; real_T P_676 ; real_T
P_677 ; real_T P_678 ; real_T P_679 ; real_T P_680 ; real_T P_681 ; real_T
P_682 ; real_T P_683 ; real_T P_684 ; real_T P_685 ; real_T P_686 ; real_T
P_687 ; real_T P_688 ; real_T P_689 ; real_T P_690 ; real_T P_691 ; real_T
P_692 ; real_T P_693 ; real_T P_694 ; real_T P_695 ; real_T P_696 ; real_T
P_697 [ 4 ] ; real_T P_698 [ 4 ] ; real_T P_699 ; real_T P_700 [ 5 ] ; real_T
P_701 [ 5 ] ; real_T P_702 ; real_T P_703 ; real_T P_704 [ 5 ] ; real_T P_705
[ 5 ] ; real_T P_706 ; real_T P_707 ; real_T P_708 [ 5 ] ; real_T P_709 [ 5 ]
; real_T P_710 ; real_T P_711 ; real_T P_712 [ 4 ] ; real_T P_713 [ 4 ] ;
real_T P_714 ; real_T P_715 [ 5 ] ; real_T P_716 [ 5 ] ; real_T P_717 ;
real_T P_718 ; real_T P_719 [ 5 ] ; real_T P_720 [ 5 ] ; real_T P_721 ;
real_T P_722 ; real_T P_723 [ 5 ] ; real_T P_724 [ 5 ] ; real_T P_725 ;
real_T P_726 ; real_T P_727 [ 6 ] ; real_T P_728 [ 6 ] ; real_T P_729 ;
real_T P_730 ; real_T P_731 ; real_T P_732 ; real_T P_733 ; real_T P_734 ;
real_T P_735 ; real_T P_736 ; real_T P_737 ; real_T P_738 ; real_T P_739 ;
real_T P_740 ; real_T P_741 ; real_T P_742 ; real_T P_743 [ 9 ] ; real_T
P_744 ; real_T P_745 ; real_T P_746 ; real_T P_747 ; real_T P_748 ; real_T
P_749 ; real_T P_750 ; real_T P_751 ; real_T P_752 ; real_T P_753 ; real_T
P_754 ; real_T P_755 ; real_T P_756 [ 36 ] ; real_T P_757 ; real_T P_758 [ 36
] ; real_T P_759 ; real_T P_760 [ 2 ] ; real_T P_761 [ 2 ] ; real_T P_762 [
36 ] ; real_T P_763 [ 36 ] ; real_T P_764 [ 3 ] ; real_T P_765 ; real_T P_766
; real_T P_767 ; real_T P_768 ; real_T P_769 ; real_T P_770 ; real_T P_771 ;
real_T P_772 ; real_T P_773 ; real_T P_774 ; real_T P_775 ; real_T P_776 ;
real_T P_777 ; real_T P_778 ; real_T P_779 ; real_T P_780 ; real_T P_781 ;
real_T P_782 ; real_T P_783 ; real_T P_784 ; real_T P_785 ; real_T P_786 ;
real_T P_787 ; real_T P_788 ; real_T P_789 ; real_T P_790 ; real_T P_791 ;
real_T P_792 ; real_T P_793 ; real_T P_794 ; real_T P_795 ; real_T P_796 [ 3
] ; real_T P_797 ; real_T P_798 ; real_T P_799 ; real_T P_800 ; real_T P_801
; real_T P_802 ; real_T P_803 ; real_T P_804 ; real_T P_805 ; boolean_T P_806
; boolean_T P_807 ; boolean_T P_808 ; boolean_T P_809 ; uint8_T P_810 ;
uint8_T P_811 ; char_T pad_P_811 [ 2 ] ;
P_Transferfunctionsfordetailedstabilizer_DZG_v6_SimGen_T
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
Transferfunctionsfordetailedstabilizer ; } ; extern P_DZG_v6_SimGen_T
DZG_v6_SimGen_rtDefaultP ;
#endif
