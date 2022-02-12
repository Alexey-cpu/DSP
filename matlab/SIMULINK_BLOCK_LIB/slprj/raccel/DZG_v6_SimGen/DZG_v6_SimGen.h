#ifndef RTW_HEADER_DZG_v6_SimGen_h_
#define RTW_HEADER_DZG_v6_SimGen_h_
#include <stddef.h>
#include <float.h>
#include <string.h>
#include "rtw_modelmap.h"
#ifndef DZG_v6_SimGen_COMMON_INCLUDES_
#define DZG_v6_SimGen_COMMON_INCLUDES_
#include <stdlib.h>
#include "rtwtypes.h"
#include "simtarget/slSimTgtSigstreamRTW.h"
#include "simtarget/slSimTgtSlioCoreRTW.h"
#include "simtarget/slSimTgtSlioClientsRTW.h"
#include "simtarget/slSimTgtSlioSdiRTW.h"
#include "sigstream_rtw.h"
#include "simstruc.h"
#include "fixedpoint.h"
#include "raccel.h"
#include "slsv_diagnostic_codegen_c_api.h"
#include "rt_logging.h"
#include "dt_info.h"
#include "ext_work.h"
#endif
#include "DZG_v6_SimGen_types.h"
#include "multiword_types.h"
#include "mwmathutil.h"
#include "rt_look.h"
#include "rt_look1d.h"
#include "rt_defines.h"
#include "rtGetInf.h"
#include "rt_nonfinite.h"
#define MODEL_NAME DZG_v6_SimGen
#define NSAMPLE_TIMES (4) 
#define NINPUTS (0)       
#define NOUTPUTS (0)     
#define NBLOCKIO (281) 
#define NUM_ZC_EVENTS (0) 
#ifndef NCSTATES
#define NCSTATES (4)   
#elif NCSTATES != 4
#error Invalid specification of NCSTATES defined in compiler command
#endif
#ifndef rtmGetDataMapInfo
#define rtmGetDataMapInfo(rtm) (*rt_dataMapInfoPtr)
#endif
#ifndef rtmSetDataMapInfo
#define rtmSetDataMapInfo(rtm, val) (rt_dataMapInfoPtr = &val)
#endif
#ifndef IN_RACCEL_MAIN
#endif
typedef struct { creal_T jzulpokwia ; creal_T dngbcewebp ; real_T o0laqbijxj
; real_T phf1q3qtgy ; real_T adxx2eh1g0 ; real_T fx0kk0lfk3 ; real_T
ps502vbzlw ; real_T pz2b2ixe3g ; real_T mshc0z4y0b [ 6 ] ; real_T kxk50ui10l
[ 36 ] ; real_T i2htmqgs5h [ 6 ] ; real_T d2l3x5dmoq [ 6 ] ; real_T
a3kfgrogpp ; real_T dggh1ark2b ; real_T hleuoxbvtr ; real_T gfcufizx5q [ 3 ]
; real_T ocwtlsstoa [ 2 ] ; real_T kc2wsb45fs ; real_T l2zafpzrzu ; real_T
ohedl35wf1 ; real_T bkf5f3ujyq [ 50 ] ; real_T mcctrqnlij [ 6 ] ; real_T
cuumld5jhq ; real_T czblwzh330 ; real_T ed1n42eefk ; real_T i524ua1zo0 ;
real_T e11hdbixe4 ; real_T f3bj13s54m ; real_T dljdyoojd5 ; real_T jfbplyjjoy
; real_T mcclxrnvnu ; real_T dv5odcc5un ; real_T hu45skyzd5 ; real_T
nxmiliesal ; real_T lz5rdlkvrc ; real_T a2aelbuuxd ; real_T lb3o0vdenr ;
real_T lsnwzrnmnc ; real_T m4o5gfq3hr ; real_T o1nbxtrkf4 ; real_T jpdedqtzey
; real_T jn3gd54hx5 ; real_T amlu2hdzhw ; real_T jh3odldqrg ; real_T
caiqrgrb4h ; real_T b5yrakypqk ; real_T kyvh55ysq1 ; real_T njwnc1thzp ;
real_T fvgxbp0r0b ; real_T h33eecit0h ; real_T hcnu41udwm ; real_T ixvhkcticg
; real_T ihbtz02goz ; real_T bz4ixbddyb ; real_T nzu205zzco ; real_T
bbfgsoogaz ; real_T dkxzgq2elf ; real_T n1sf2oxpy1 ; real_T bsjs3yr3cm ;
real_T omi22g0ht3 ; real_T maqdked4sl ; real_T l0q0zwxfui [ 3 ] ; real_T
oecmwuiwrp ; real_T m15cezxam4 ; real_T brcpuhmr2n ; real_T cpugwweowj [ 3 ]
; real_T ht3pkpouae ; real_T epw4e2nyr2 ; real_T onhlo5lk3i ; real_T
hvccs5ksdj ; real_T mv11qhbyiu ; real_T lxuyvf0kct ; real_T cpurtfy0lf ;
real_T hdfe5utvyn ; real_T gqo15jq4yq ; real_T hwnr1apibg [ 3 ] ; real_T
by0csc0mva [ 2 ] ; real_T l1lpbegscp [ 3 ] ; real_T ck4g14oqxl ; real_T
h5nuuqtufn [ 3 ] ; real_T bdvh4jtcpt [ 3 ] ; real_T l5qxq2mybh ; real_T
do04422dsh [ 2 ] ; real_T kctgawizah [ 2 ] ; real_T g2rmjbbycl ; real_T
ljwiyate1t ; real_T lwg1hzp3ql [ 2 ] ; real_T c4xvwz5dvp ; real_T hv3y4xtyo5
; real_T otvyxcolz0 ; real_T ca1sh32w2e ; real_T ogxs2lrdho ; real_T
n4glgvjxv2 ; real_T dq2eyjw50a ; real_T fyirte2cly ; real_T i2tqhru53h ;
real_T ahl15fuofz ; real_T nnbfzmzsox [ 18 ] ; real_T nddeflp2rr [ 18 ] ;
real_T b14bfbabib ; real_T mwnm3xw2te ; real_T gpq0cumlrp ; real_T ooqvccplqr
; real_T bsib0j2uiv [ 2 ] ; real_T iwzh4cmhgz [ 2 ] ; real_T kbpgzmhgnk ;
real_T kt4yqwu22g ; real_T eekpbkk0js ; real_T bzg42dyv2g ; real_T gilkef2gvf
; real_T i3cpus0vrs ; real_T mnrqebdkci [ 6 ] ; real_T gwy50czbqx ; real_T
hjl4wiy52h [ 6 ] ; real_T gsp2mdotvq ; real_T cuthot4b55 ; real_T m3hbl5oi51
; real_T kr1dizdsxr ; real_T gftya03rfs ; real_T fmoyvyrdin ; real_T
h2yyrby5su ; real_T jyft0oavfh ; real_T ax2dm5rpub ; real_T o3xqnnpyja ;
real_T k33xjpkxfj ; real_T hlupe3wd1s ; real_T ogr2enhepf ; real_T bxfyqcorw3
[ 6 ] ; real_T dn1jsu5b3a ; real_T anv20tflb2 ; real_T eajcmlcqmc ; real_T
ejzz1lck3x ; real_T clkwduumjv ; real_T geos3slncn ; real_T gmd2h4cezq ;
real_T i20vaspyrp ; real_T mzzuyzwyzo ; real_T oj12dzvfb4 ; real_T pu5azi4ouj
; real_T p4d344o4a4 ; real_T pxtshpab0u ; real_T orydyp31zs ; real_T
btppbtrakx ; real_T embtezfk0j [ 3 ] ; real_T icofxpvarr ; real_T n3yespd0xf
; real_T kfeoejnqkx ; real_T i5mjxvb2y1 [ 3 ] ; real_T kalgaozq2e ; real_T
c3mov1tl1f ; real_T mj5ueklf54 ; real_T chk1pd3rlp [ 3 ] ; real_T nfbe1f5aq4
; real_T gpbkfrtopv ; real_T emcqpstfhw ; real_T o04nrxwopl [ 3 ] ; real_T
m315dx4bxe ; real_T lixhx51dy4 ; real_T arirsm2gtb ; real_T aei40fflxj [ 3 ]
; real_T pchaeyb4wc ; real_T geuzk04gqe ; real_T nx2s4jnhlc ; real_T
k3tazd503n [ 3 ] ; real_T c5z4wv1mbn ; real_T ixbmptgikc ; real_T g1pav31o54
; real_T g2dr1qmh1g [ 3 ] ; real_T or1sotd5ok ; real_T oawoasztyv ; real_T
ez3n02akaw ; real_T hm1ran4pnq [ 3 ] ; real_T im0zegy3zl ; real_T ca0k4g2ixd
; real_T g4um4nifj0 ; real_T bv31jmhnur [ 3 ] ; real_T fgeeodibr2 ; real_T
lzor53a1zc ; real_T a4mondaqym ; real_T ikwzxkxyiz [ 3 ] ; real_T e3fyv2i0xt
; real_T pv0jl14wdm ; real_T gkxjm351v3 ; real_T gqlskp40nb [ 3 ] ; real_T
cjgzre1rfd ; real_T mmqgxd3ztq ; real_T kfya4ws5wt ; real_T czdxilbjs1 [ 3 ]
; real_T kflvqeslk3 ; real_T pubftwd23q ; real_T abudmgdmqo ; real_T
jnufx11sbb ; real_T mnz0k5z1ev ; real_T n3cickajoe ; real_T cka3cbvjkk ;
real_T dvmo0qrznq ; real_T i34c3sd5c4 ; real_T jo0ewhen5p ; real_T janqpo5qe4
[ 6 ] ; real_T dx3rmsixh3 [ 36 ] ; real_T n0sluxojte [ 36 ] ; real_T
lyhqkuq1c4 ; real_T ofyfrkcfxb [ 36 ] ; real_T khdzdbpnht [ 36 ] ; real_T
dgwxdhwkdd [ 36 ] ; real_T caz1neybgm [ 36 ] ; real_T gdroql4j2f [ 36 ] ;
real_T g0lz5qytlu [ 36 ] ; real_T cbygkknxsv [ 6 ] ; real_T ncbiuw0cin [ 36 ]
; real_T deid52kk1x [ 36 ] ; real_T c1v5fiqxar [ 6 ] ; real_T cfmayp3lnt [ 6
] ; real_T b2rku1yvfe [ 3 ] ; real_T iqgj54dvfu ; real_T kgqd5i3tmd ; real_T
pmvqbzeo1a ; real_T lnslax3wky ; real_T o5o0z0ts4i ; real_T ke1trlx0xn ;
real_T pfjhtzhhke ; real_T dsgaag2j1a ; real_T ddpigcbdzw ; real_T msc0q3ydkn
; real_T bzlrt5abhc ; real_T mc0c4fprgt [ 36 ] ; real_T g30midesdo [ 36 ] ;
real_T k3cesiw1su [ 36 ] ; real_T mecr0anvzq [ 36 ] ; real_T g5yq3s1oi3 [ 36
] ; real_T gtavo0wfmk ; real_T p4nck3maqs [ 3 ] ; real_T lv1l3120cn ; real_T
pnchbrbzh4 ; real_T h42emyzqm1 ; real_T mw4gdhqybz ; real_T im1q1kmaf4 ;
real_T a0tcaxg1ye ; real_T babh1jhck2 ; real_T op4v4vz1hb ; real_T elxflb4cn2
; real_T enoczvl5ij ; real_T l42zjb2acu ; real_T cy4aoyd4p4 ; real_T
iipz2h3t1h ; real_T jd3k1qt01g ; real_T p2blqb2x40 ; real_T jwcmbwsc34 ;
real_T paxzwpdtyr ; real_T mnmjtwzkhg ; real_T hfxqkjhn3c ; real_T gh0e24cnt4
; real_T oq2fmk4tkh ; real_T guw3ibew5f ; real_T fwobh2ydkx ; real_T
kji2nsjrqr ; real_T frxxgkpfd0 ; real_T if3uw30dno ; real_T i4qgt25dqa ;
real_T ecp5cwxuux ; real_T fyt1uabhvi ; real_T b133gvafim ; real_T prtdictlyg
; real_T bzogwznvbx ; real_T n1w1gaeskg ; real_T euj2mhfzyp ; real_T
jjuwkhbaqb ; real_T ks1ri00jvc ; real_T orqdb4jq0f ; real_T l25oveef2i ;
boolean_T lvq5fe4dss ; boolean_T hswklfu3tm ; boolean_T nhjhuszeqq ;
boolean_T jgzslezpvv ; boolean_T jsc1e3jlwj ; boolean_T kkthzbzxh4 ;
boolean_T f05xiwzeii ; boolean_T hq4dyb3apy ; boolean_T ot21w1gcro ;
boolean_T dv21ecppse ; boolean_T kgpjy2123a ; } B ; typedef struct { real_T
hsrejdggk0 ; real_T g2cesgd0fj [ 6 ] ; real_T pwalsrbpvg [ 6 ] ; real_T
ltyzpyes2j ; real_T koephaho24 ; real_T bjkyi30h4y [ 6 ] ; real_T cszisyhd2i
; real_T ksbtbqie2m ; real_T ezm5irxmvk ; real_T m2oxahajnn ; real_T
jzuea21ukz ; real_T crh5ypw2db ; real_T ia2zs2ii2b ; real_T lormmfrasx ;
real_T ocwfh3bc3h ; real_T dw042k0azu ; real_T jfihwsipvc ; real_T k2r45klkvr
; real_T lawcljffga ; real_T bhgocl4yyi ; volatile real_T ayphut5w0e ; real_T
byyiabbsdb [ 36 ] ; real_T khwhm1vhhp [ 36 ] ; real_T i33eulcryu [ 36 ] ;
real_T a2rewpflwu [ 36 ] ; struct { real_T modelTStart ; } o31z3owz5o ;
struct { real_T modelTStart ; } kcazm4yk20 ; struct { real_T modelTStart ; }
pznl15zy5j ; struct { real_T modelTStart ; } az4xbowotu ; struct { void * AS
; void * BS ; void * CS ; void * DS ; void * DX_COL ; void * BD_COL ; void *
TMP1 ; void * TMP2 ; void * XTMP ; void * SWITCH_STATUS ; void *
SWITCH_STATUS_INIT ; void * SW_CHG ; void * G_STATE ; void * USWLAST ; void *
XKM12 ; void * XKP12 ; void * XLAST ; void * ULAST ; void * IDX_SW_CHG ; void
* Y_SWITCH ; void * SWITCH_TYPES ; void * IDX_OUT_SW ; void *
SWITCH_TOPO_SAVED_IDX ; void * SWITCH_MAP ; } oh3rj3rgch ; struct { void *
TUbufferPtrs [ 2 ] ; } b1o5sd1mu2 ; struct { void * TUbufferPtrs [ 2 ] ; }
cvivv0qvki ; struct { void * TUbufferPtrs [ 2 ] ; } ecv4fwadro ; struct {
void * TUbufferPtrs [ 2 ] ; } ohddfojdjk ; struct { void * LoggedData [ 2 ] ;
} m0scdgvmro ; struct { void * LoggedData [ 2 ] ; } exi0svnnxz ; struct {
void * LoggedData [ 2 ] ; } apk42thrxw ; struct { void * LoggedData [ 2 ] ; }
m44vudu504 ; int32_T mljcxhdcts ; int32_T euaoyfm334 ; int32_T irr1ngkglw ;
int32_T fa0orlv0f5 [ 6 ] ; uint32_T lrbh114wtw ; uint32_T pftimexvc5 ; int_T
lkcbxptdv5 [ 11 ] ; struct { int_T Tail ; int_T Head ; int_T Last ; int_T
CircularBufSize ; int_T MaxNewBufSize ; } drr2cwynj5 ; struct { int_T Tail ;
int_T Head ; int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
fymd3ellif ; struct { int_T Tail ; int_T Head ; int_T Last ; int_T
CircularBufSize ; int_T MaxNewBufSize ; } eyb4fhkbdy ; struct { int_T Tail ;
int_T Head ; int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
ek4mryl130 ; volatile int8_T m11yduvrgb ; int8_T eocn40ibhc ; int8_T
jld3u4fkos ; uint8_T lkdnaytcjn ; } DW ; typedef struct { real_T lpzjbjknwp ;
real_T oi5s2zktut ; real_T pz2xsnrzjc ; real_T hfv4dc3cpb ; } X ; typedef
struct { real_T lpzjbjknwp ; real_T oi5s2zktut ; real_T pz2xsnrzjc ; real_T
hfv4dc3cpb ; } XDot ; typedef struct { boolean_T lpzjbjknwp ; boolean_T
oi5s2zktut ; boolean_T pz2xsnrzjc ; boolean_T hfv4dc3cpb ; } XDis ; typedef
struct { rtwCAPI_ModelMappingInfo mmi ; } DataMapInfo ; struct P_ { real_T
KZTest_FaultA ; real_T KZTest_FaultB ; real_T KZTest_FaultC ; real_T
System1_Frequency ; real_T Q2_SwitchA ; real_T Q2_SwitchB ; real_T Q2_SwitchC
; real_T Gain_Gain ; real_T Gain_Gain_f0nj4ejddp ; real_T
Gain_Gain_ksjae44pgn ; real_T Gain_Gain_kn1p3uhx5m ; real_T LookUpTable_XData
[ 5 ] ; real_T LookUpTable_YData [ 5 ] ; real_T LookUpTable_XData_ouijhmudsj
[ 5 ] ; real_T LookUpTable_YData_ibczk2yoa0 [ 5 ] ; real_T
LookUpTable_XData_j5m2nusfgx [ 5 ] ; real_T LookUpTable_YData_esrnrccjov [ 5
] ; real_T LookUpTable_XData_fglubeidif [ 5 ] ; real_T
LookUpTable_YData_hhayxsnnev [ 5 ] ; real_T Switch_Threshold ; real_T
LookUpTable_XData_o0gg4yk3ay [ 5 ] ; real_T LookUpTable_YData_dujf1r2cza [ 5
] ; real_T Switch1_Threshold ; real_T LookUpTable_XData_pxfayshagn [ 5 ] ;
real_T LookUpTable_YData_cvxjcdpbmq [ 5 ] ; real_T Switch2_Threshold ; real_T
Lmq_sat_Y0 ; real_T Laq_Y0 ; real_T uLl_q_Gain [ 3 ] ; real_T
Lmq_sat_InitialCondition ; real_T uDLookupTable_tableData [ 10 ] ; real_T
uDLookupTable_bp01Data [ 10 ] ; real_T Lmq_Gain ; real_T Constant1_Value ;
real_T u2_Value [ 3 ] ; real_T Linv_Y0 ; real_T RLinv_Y0 ; real_T Lmqd_Y0 ;
real_T Laqd_Y0 ; real_T uLl_d_Gain [ 3 ] ; real_T Lmd_sat_InitialCondition ;
real_T uDLookupTable_tableData_jcoemdiu0h [ 10 ] ; real_T
uDLookupTable_bp01Data_h0gmvbj4n1 [ 10 ] ; real_T Lmd_Gain ; real_T
Constant1_Value_csdxpxx0tx ; real_T u1_Value [ 2 ] ; real_T
u1_Value_oit41rg0le [ 36 ] ; real_T u5_Value [ 36 ] ; real_T
u1_Value_pqt3qesf2j [ 36 ] ; real_T u2_Value_kee1ps1dzy ; real_T u3_Value ;
real_T Switch1_Threshold_aikum5bfzk ; real_T Gain1_Gain ; real_T
wbaseTs2_Gain ; real_T wbaseTs2_Gain_agkgmtch40 ; real_T
Rotorangledthetae_gainval ; real_T Rotorangledthetae_IC ; real_T web2_Gain ;
real_T fluxes_InitialCondition [ 6 ] ; real_T Switch_Threshold_pea0kljvkj ;
real_T changeIqIdcurrentsigns_Gain [ 6 ] ; real_T ib_Gain ; real_T
SineWaveA_Amp ; real_T SineWaveA_Bias ; real_T SineWaveA_Hsin ; real_T
SineWaveA_HCos ; real_T SineWaveA_PSin ; real_T SineWaveA_PCos ; real_T
SineWaveB_Amp ; real_T SineWaveB_Bias ; real_T SineWaveB_Hsin ; real_T
SineWaveB_HCos ; real_T SineWaveB_PSin ; real_T SineWaveB_PCos ; real_T
SineWaveC_Amp ; real_T SineWaveC_Bias ; real_T SineWaveC_Hsin ; real_T
SineWaveC_HCos ; real_T SineWaveC_PSin ; real_T SineWaveC_PCos ; real_T
StateSpace_AS_param [ 36 ] ; real_T StateSpace_BS_param [ 66 ] ; real_T
StateSpace_CS_param [ 300 ] ; real_T StateSpace_DS_param [ 550 ] ; real_T
StateSpace_X0_param [ 6 ] ; real_T integrator_IC ; real_T
TransportDelay_Delay ; real_T TransportDelay_InitOutput ; real_T K1_Value ;
real_T Memory_InitialCondition ; real_T integrator_IC_kbu50edsmc ; real_T
TransportDelay_Delay_gkek32v2fo ; real_T TransportDelay_InitOutput_gepskaykr5
; real_T K1_Value_mjkioortvy ; real_T Memory_InitialCondition_e0ydmt0d1l ;
real_T RadDeg_Gain ; real_T integrator_IC_j24moklbcm ; real_T
TransportDelay_Delay_mvxoy0gg0s ; real_T TransportDelay_InitOutput_a33cxqs542
; real_T K1_Value_k2kyb4ovnj ; real_T Memory_InitialCondition_l14qxrwnwp ;
real_T integrator_IC_lxfy54g0cz ; real_T TransportDelay_Delay_bifxyqg4be ;
real_T TransportDelay_InitOutput_kfzzmtnzlj ; real_T K1_Value_pj1iwyknqz ;
real_T Memory_InitialCondition_m5lga3j4fd ; real_T RadDeg_Gain_mu3nul0msz ;
real_T Gain4_Gain ; real_T Gain_Gain_hktcntwr5c ; real_T
Gain1_Gain_nmin1oira0 ; real_T u_Gain ; real_T u_Gain_prbig4sao4 ; real_T
donotdeletethisgain_Gain ; real_T donotdeletethisgain_Gain_mrsxwkh4tp ;
real_T donotdeletethisgain_Gain_lbvwbky1pa ; real_T Kv1_Gain ; real_T
donotdeletethisgain_Gain_jxggkyb4oj ; real_T
donotdeletethisgain_Gain_p0laf5fkqn ; real_T
donotdeletethisgain_Gain_j5mfsbj1ew ; real_T Kv_Gain ; real_T sinwt_Amp ;
real_T sinwt_Bias ; real_T sinwt_Freq ; real_T sinwt_Phase ; real_T coswt_Amp
; real_T coswt_Bias ; real_T coswt_Freq ; real_T coswt_Phase ; real_T
sinwt_Amp_ggpyeyy0ei ; real_T sinwt_Bias_aiqrlpqukr ; real_T
sinwt_Freq_a0silddums ; real_T sinwt_Phase_pauodsxphx ; real_T
coswt_Amp_mejccdpyws ; real_T coswt_Bias_acdrihp4mg ; real_T
coswt_Freq_dexl0rdjce ; real_T coswt_Phase_gsew12la0i ; real_T
uLl_q_Gain_nj2ycls33u [ 3 ] ; real_T Switch3_Threshold ; real_T
uLl_d_Gain_mutaj0uj3h [ 3 ] ; real_T u_Vb_Gain ; real_T
Switch2_Threshold_auuawawddv ; real_T Gain_Gain_olfnewahm1 ; real_T
Gain_Gain_pycxugwvd0 ; real_T Gain1_Gain_j0bqxbk0lz ; real_T
outputformatting_Gain [ 18 ] ; real_T Gain_Gain_bpzmbe1g2k ; real_T
Gain1_Gain_h3tkw2h2wq ; real_T Gain3_Gain ; real_T u1_Gain [ 2 ] ; real_T
dw_delay_InitialCondition ; real_T F2_Gain ; real_T
dw_predict_InitialCondition ; real_T voltages_InitialCondition ; real_T
IC_Threshold ; real_T u_Pb_Gain ; real_T F_Gain ; real_T u2H_Gain ; real_T
theta_gainval ; real_T theta_IC ; real_T units_Gain ; real_T units1_Gain ;
real_T t_Gain ; real_T units2_Gain ; real_T Rotorspeeddeviationdw_gainval ;
real_T Rotorspeeddeviationdw_IC ; real_T webase_Gain ; real_T web3_Gain ;
real_T C4_Value ; real_T LookUpTable_XData_i4mu3ctko4 [ 6 ] ; real_T
LookUpTable_YData_epertdpbpx [ 6 ] ; real_T Switch3_Threshold_o2qhhfyd2s ;
real_T Switch3_Threshold_k0uagz5gcg ; real_T C4_Value_fxleokhe0y ; real_T
Switch3_Threshold_icm4fmynnh ; real_T C4_Value_eoqp0wijkf ; real_T
Switch3_Threshold_hdegddai5u ; real_T donotdeletethisgain_Gain_g4a3i3bkvy ;
real_T donotdeletethisgain_Gain_p2gxvtukfv ; real_T
donotdeletethisgain_Gain_is2ce52gq3 ; real_T Kv_Gain_b1jtzjwn5z ; real_T
donotdeletethisgain_Gain_h5sheo5agc ; real_T
donotdeletethisgain_Gain_idjvkwlnjp ; real_T
donotdeletethisgain_Gain_iiwqs0rjpv ; real_T Kv1_Gain_fhx1xa0qe5 ; real_T
donotdeletethisgain_Gain_jc3l43jy02 ; real_T
donotdeletethisgain_Gain_ixohazj2yz ; real_T
donotdeletethisgain_Gain_ijcgmen2vo ; real_T Kv_Gain_hbrwnhtc5z ; real_T
donotdeletethisgain_Gain_dftutozx3l ; real_T
donotdeletethisgain_Gain_dvccqwmfov ; real_T
donotdeletethisgain_Gain_b3iqzvyj5x ; real_T Kv1_Gain_bls2zebxwe ; real_T
donotdeletethisgain_Gain_ckgop4gksi ; real_T
donotdeletethisgain_Gain_gex5e0120e ; real_T
donotdeletethisgain_Gain_mnyedn5jj1 ; real_T Kv_Gain_dc42uxbym3 ; real_T
donotdeletethisgain_Gain_nsmmc33j4l ; real_T
donotdeletethisgain_Gain_oc13lni3fq ; real_T
donotdeletethisgain_Gain_ghrwzwq2wl ; real_T Kv1_Gain_js4lqtbwg1 ; real_T
donotdeletethisgain_Gain_inxi5cfmng ; real_T
donotdeletethisgain_Gain_enynbgjy2q ; real_T
donotdeletethisgain_Gain_ghgnnpri0h ; real_T Kv_Gain_gr1trvmmq4 ; real_T
donotdeletethisgain_Gain_fdstflfpdl ; real_T
donotdeletethisgain_Gain_hwxarrm2xf ; real_T
donotdeletethisgain_Gain_jpsgb554cq ; real_T Kv1_Gain_lejhoixvzv ; real_T
donotdeletethisgain_Gain_akjxwy4mzh ; real_T
donotdeletethisgain_Gain_m1o3yt3pmb ; real_T
donotdeletethisgain_Gain_bdqv4z3h20 ; real_T Kv_Gain_dryr11wp4y ; real_T
donotdeletethisgain_Gain_dyxa4y4pb4 ; real_T
donotdeletethisgain_Gain_kgtmebmbqg ; real_T
donotdeletethisgain_Gain_pn4mhzapma ; real_T Kv1_Gain_itak2vpaxj ; real_T
donotdeletethisgain_Gain_lb32xlqutz ; real_T
donotdeletethisgain_Gain_e1t00s2jpf ; real_T
donotdeletethisgain_Gain_kznkprn5og ; real_T Kv_Gain_ccfkg0ba2b ; real_T
donotdeletethisgain_Gain_krlg4qa22u ; real_T
donotdeletethisgain_Gain_hxax2d03xl ; real_T
donotdeletethisgain_Gain_fgel1ns2zy ; real_T Kv1_Gain_oi10siy4fh ; real_T
C4_Value_gbl0dsqzhj ; real_T LookUpTable_XData_bysejxwrcs [ 6 ] ; real_T
LookUpTable_YData_anwymjpndi [ 6 ] ; real_T Switch3_Threshold_js4dz2naas ;
real_T Switch3_Threshold_g44ubd0yky ; real_T C4_Value_exqcfpjqxr ; real_T
Switch3_Threshold_c15c5kf1pp ; real_T C4_Value_ktcw3s24rj ; real_T
Switch3_Threshold_c2ubqk250k ; real_T SwitchCurrents_Value [ 6 ] ; real_T
Pm1_Value ; real_T Constant2_Value ; real_T Vf_Value ; real_T N_Gain ; real_T
Constant1_Value_iglewzfyey ; real_T Constant2_Value_a35hujaypn ; real_T
Constant3_Value ; real_T Constant4_Value [ 36 ] ; real_T Constant5_Value ;
real_T Constant6_Value [ 36 ] ; real_T Constant8_Value ; real_T
Laqd_nosat_Value [ 2 ] ; real_T Lmqd_nosat_Value [ 2 ] ; real_T
u1_Value_cbuqdiipgi [ 36 ] ; real_T u5_Value_ochldviepj [ 36 ] ; real_T
Vkd0Vkq10Vkq20_Value [ 3 ] ; real_T nominalspeed_Value ; real_T VfPP1_Value ;
real_T C4_Value_gcbzcnw2m4 ; real_T com_Value ; real_T C4_Value_ouf3ao4h2o ;
real_T Constant5_Value_eyvmukmjzw ; real_T com_Value_e0niij5lrb ; boolean_T
Constant1_Value_afrrlaacip ; boolean_T Constant2_Value_lxtjw1dx3o ; boolean_T
Constant3_Value_jspriozsq0 ; boolean_T Constant1_Value_nw4shdp34n ; } ;
extern const char * RT_MEMORY_ALLOCATION_ERROR ; extern B rtB ; extern X rtX
; extern DW rtDW ; extern P rtP ; extern const rtwCAPI_ModelMappingStaticInfo
* DZG_v6_SimGen_GetCAPIStaticMap ( void ) ; extern SimStruct * const rtS ;
extern const int_T gblNumToFiles ; extern const int_T gblNumFrFiles ; extern
const int_T gblNumFrWksBlocks ; extern rtInportTUtable * gblInportTUtables ;
extern const char * gblInportFileName ; extern const int_T
gblNumRootInportBlks ; extern const int_T gblNumModelInputs ; extern const
int_T gblInportDataTypeIdx [ ] ; extern const int_T gblInportDims [ ] ;
extern const int_T gblInportComplex [ ] ; extern const int_T
gblInportInterpoFlag [ ] ; extern const int_T gblInportContinuous [ ] ;
extern const int_T gblParameterTuningTid ; extern DataMapInfo *
rt_dataMapInfoPtr ; extern rtwCAPI_ModelMappingInfo * rt_modelMapInfoPtr ;
void MdlOutputs ( int_T tid ) ; void MdlOutputsParameterSampleTime ( int_T
tid ) ; void MdlUpdate ( int_T tid ) ; void MdlTerminate ( void ) ; void
MdlInitializeSizes ( void ) ; void MdlInitializeSampleTimes ( void ) ;
SimStruct * raccel_register_model ( void ) ;
#endif
