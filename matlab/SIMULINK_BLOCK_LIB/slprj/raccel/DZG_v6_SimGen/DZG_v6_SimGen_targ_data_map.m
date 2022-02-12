  function targMap = targDataMap(),

  ;%***********************
  ;% Create Parameter Map *
  ;%***********************
      
    nTotData      = 0; %add to this count as we go
    nTotSects     = 2;
    sectIdxOffset = 0;
    
    ;%
    ;% Define dummy sections & preallocate arrays
    ;%
    dumSection.nData = -1;  
    dumSection.data  = [];
    
    dumData.logicalSrcIdx = -1;
    dumData.dtTransOffset = -1;
    
    ;%
    ;% Init/prealloc paramMap
    ;%
    paramMap.nSections           = nTotSects;
    paramMap.sectIdxOffset       = sectIdxOffset;
      paramMap.sections(nTotSects) = dumSection; %prealloc
    paramMap.nTotData            = -1;
    
    ;%
    ;% Auto data (rtP)
    ;%
      section.nData     = 257;
      section.data(257)  = dumData; %prealloc
      
	  ;% rtP.KZTest_FaultA
	  section.data(1).logicalSrcIdx = 0;
	  section.data(1).dtTransOffset = 0;
	
	  ;% rtP.KZTest_FaultB
	  section.data(2).logicalSrcIdx = 1;
	  section.data(2).dtTransOffset = 1;
	
	  ;% rtP.KZTest_FaultC
	  section.data(3).logicalSrcIdx = 2;
	  section.data(3).dtTransOffset = 2;
	
	  ;% rtP.System1_Frequency
	  section.data(4).logicalSrcIdx = 3;
	  section.data(4).dtTransOffset = 3;
	
	  ;% rtP.Q2_SwitchA
	  section.data(5).logicalSrcIdx = 4;
	  section.data(5).dtTransOffset = 4;
	
	  ;% rtP.Q2_SwitchB
	  section.data(6).logicalSrcIdx = 5;
	  section.data(6).dtTransOffset = 5;
	
	  ;% rtP.Q2_SwitchC
	  section.data(7).logicalSrcIdx = 6;
	  section.data(7).dtTransOffset = 6;
	
	  ;% rtP.Gain_Gain
	  section.data(8).logicalSrcIdx = 7;
	  section.data(8).dtTransOffset = 7;
	
	  ;% rtP.Gain_Gain_f0nj4ejddp
	  section.data(9).logicalSrcIdx = 8;
	  section.data(9).dtTransOffset = 8;
	
	  ;% rtP.Gain_Gain_ksjae44pgn
	  section.data(10).logicalSrcIdx = 9;
	  section.data(10).dtTransOffset = 9;
	
	  ;% rtP.Gain_Gain_kn1p3uhx5m
	  section.data(11).logicalSrcIdx = 10;
	  section.data(11).dtTransOffset = 10;
	
	  ;% rtP.LookUpTable_XData
	  section.data(12).logicalSrcIdx = 11;
	  section.data(12).dtTransOffset = 11;
	
	  ;% rtP.LookUpTable_YData
	  section.data(13).logicalSrcIdx = 12;
	  section.data(13).dtTransOffset = 16;
	
	  ;% rtP.LookUpTable_XData_ouijhmudsj
	  section.data(14).logicalSrcIdx = 13;
	  section.data(14).dtTransOffset = 21;
	
	  ;% rtP.LookUpTable_YData_ibczk2yoa0
	  section.data(15).logicalSrcIdx = 14;
	  section.data(15).dtTransOffset = 26;
	
	  ;% rtP.LookUpTable_XData_j5m2nusfgx
	  section.data(16).logicalSrcIdx = 15;
	  section.data(16).dtTransOffset = 31;
	
	  ;% rtP.LookUpTable_YData_esrnrccjov
	  section.data(17).logicalSrcIdx = 16;
	  section.data(17).dtTransOffset = 36;
	
	  ;% rtP.LookUpTable_XData_fglubeidif
	  section.data(18).logicalSrcIdx = 17;
	  section.data(18).dtTransOffset = 41;
	
	  ;% rtP.LookUpTable_YData_hhayxsnnev
	  section.data(19).logicalSrcIdx = 18;
	  section.data(19).dtTransOffset = 46;
	
	  ;% rtP.Switch_Threshold
	  section.data(20).logicalSrcIdx = 19;
	  section.data(20).dtTransOffset = 51;
	
	  ;% rtP.LookUpTable_XData_o0gg4yk3ay
	  section.data(21).logicalSrcIdx = 20;
	  section.data(21).dtTransOffset = 52;
	
	  ;% rtP.LookUpTable_YData_dujf1r2cza
	  section.data(22).logicalSrcIdx = 21;
	  section.data(22).dtTransOffset = 57;
	
	  ;% rtP.Switch1_Threshold
	  section.data(23).logicalSrcIdx = 22;
	  section.data(23).dtTransOffset = 62;
	
	  ;% rtP.LookUpTable_XData_pxfayshagn
	  section.data(24).logicalSrcIdx = 23;
	  section.data(24).dtTransOffset = 63;
	
	  ;% rtP.LookUpTable_YData_cvxjcdpbmq
	  section.data(25).logicalSrcIdx = 24;
	  section.data(25).dtTransOffset = 68;
	
	  ;% rtP.Switch2_Threshold
	  section.data(26).logicalSrcIdx = 25;
	  section.data(26).dtTransOffset = 73;
	
	  ;% rtP.Lmq_sat_Y0
	  section.data(27).logicalSrcIdx = 26;
	  section.data(27).dtTransOffset = 74;
	
	  ;% rtP.Laq_Y0
	  section.data(28).logicalSrcIdx = 27;
	  section.data(28).dtTransOffset = 75;
	
	  ;% rtP.uLl_q_Gain
	  section.data(29).logicalSrcIdx = 28;
	  section.data(29).dtTransOffset = 76;
	
	  ;% rtP.Lmq_sat_InitialCondition
	  section.data(30).logicalSrcIdx = 29;
	  section.data(30).dtTransOffset = 79;
	
	  ;% rtP.uDLookupTable_tableData
	  section.data(31).logicalSrcIdx = 30;
	  section.data(31).dtTransOffset = 80;
	
	  ;% rtP.uDLookupTable_bp01Data
	  section.data(32).logicalSrcIdx = 31;
	  section.data(32).dtTransOffset = 90;
	
	  ;% rtP.Lmq_Gain
	  section.data(33).logicalSrcIdx = 32;
	  section.data(33).dtTransOffset = 100;
	
	  ;% rtP.Constant1_Value
	  section.data(34).logicalSrcIdx = 33;
	  section.data(34).dtTransOffset = 101;
	
	  ;% rtP.u2_Value
	  section.data(35).logicalSrcIdx = 34;
	  section.data(35).dtTransOffset = 102;
	
	  ;% rtP.Linv_Y0
	  section.data(36).logicalSrcIdx = 35;
	  section.data(36).dtTransOffset = 105;
	
	  ;% rtP.RLinv_Y0
	  section.data(37).logicalSrcIdx = 36;
	  section.data(37).dtTransOffset = 106;
	
	  ;% rtP.Lmqd_Y0
	  section.data(38).logicalSrcIdx = 37;
	  section.data(38).dtTransOffset = 107;
	
	  ;% rtP.Laqd_Y0
	  section.data(39).logicalSrcIdx = 38;
	  section.data(39).dtTransOffset = 108;
	
	  ;% rtP.uLl_d_Gain
	  section.data(40).logicalSrcIdx = 39;
	  section.data(40).dtTransOffset = 109;
	
	  ;% rtP.Lmd_sat_InitialCondition
	  section.data(41).logicalSrcIdx = 40;
	  section.data(41).dtTransOffset = 112;
	
	  ;% rtP.uDLookupTable_tableData_jcoemdiu0h
	  section.data(42).logicalSrcIdx = 41;
	  section.data(42).dtTransOffset = 113;
	
	  ;% rtP.uDLookupTable_bp01Data_h0gmvbj4n1
	  section.data(43).logicalSrcIdx = 42;
	  section.data(43).dtTransOffset = 123;
	
	  ;% rtP.Lmd_Gain
	  section.data(44).logicalSrcIdx = 43;
	  section.data(44).dtTransOffset = 133;
	
	  ;% rtP.Constant1_Value_csdxpxx0tx
	  section.data(45).logicalSrcIdx = 44;
	  section.data(45).dtTransOffset = 134;
	
	  ;% rtP.u1_Value
	  section.data(46).logicalSrcIdx = 45;
	  section.data(46).dtTransOffset = 135;
	
	  ;% rtP.u1_Value_oit41rg0le
	  section.data(47).logicalSrcIdx = 46;
	  section.data(47).dtTransOffset = 137;
	
	  ;% rtP.u5_Value
	  section.data(48).logicalSrcIdx = 47;
	  section.data(48).dtTransOffset = 173;
	
	  ;% rtP.u1_Value_pqt3qesf2j
	  section.data(49).logicalSrcIdx = 48;
	  section.data(49).dtTransOffset = 209;
	
	  ;% rtP.u2_Value_kee1ps1dzy
	  section.data(50).logicalSrcIdx = 49;
	  section.data(50).dtTransOffset = 245;
	
	  ;% rtP.u3_Value
	  section.data(51).logicalSrcIdx = 50;
	  section.data(51).dtTransOffset = 246;
	
	  ;% rtP.Switch1_Threshold_aikum5bfzk
	  section.data(52).logicalSrcIdx = 51;
	  section.data(52).dtTransOffset = 247;
	
	  ;% rtP.Gain1_Gain
	  section.data(53).logicalSrcIdx = 52;
	  section.data(53).dtTransOffset = 248;
	
	  ;% rtP.wbaseTs2_Gain
	  section.data(54).logicalSrcIdx = 53;
	  section.data(54).dtTransOffset = 249;
	
	  ;% rtP.wbaseTs2_Gain_agkgmtch40
	  section.data(55).logicalSrcIdx = 54;
	  section.data(55).dtTransOffset = 250;
	
	  ;% rtP.Rotorangledthetae_gainval
	  section.data(56).logicalSrcIdx = 55;
	  section.data(56).dtTransOffset = 251;
	
	  ;% rtP.Rotorangledthetae_IC
	  section.data(57).logicalSrcIdx = 56;
	  section.data(57).dtTransOffset = 252;
	
	  ;% rtP.web2_Gain
	  section.data(58).logicalSrcIdx = 57;
	  section.data(58).dtTransOffset = 253;
	
	  ;% rtP.fluxes_InitialCondition
	  section.data(59).logicalSrcIdx = 58;
	  section.data(59).dtTransOffset = 254;
	
	  ;% rtP.Switch_Threshold_pea0kljvkj
	  section.data(60).logicalSrcIdx = 59;
	  section.data(60).dtTransOffset = 260;
	
	  ;% rtP.changeIqIdcurrentsigns_Gain
	  section.data(61).logicalSrcIdx = 60;
	  section.data(61).dtTransOffset = 261;
	
	  ;% rtP.ib_Gain
	  section.data(62).logicalSrcIdx = 61;
	  section.data(62).dtTransOffset = 267;
	
	  ;% rtP.SineWaveA_Amp
	  section.data(63).logicalSrcIdx = 62;
	  section.data(63).dtTransOffset = 268;
	
	  ;% rtP.SineWaveA_Bias
	  section.data(64).logicalSrcIdx = 63;
	  section.data(64).dtTransOffset = 269;
	
	  ;% rtP.SineWaveA_Hsin
	  section.data(65).logicalSrcIdx = 64;
	  section.data(65).dtTransOffset = 270;
	
	  ;% rtP.SineWaveA_HCos
	  section.data(66).logicalSrcIdx = 65;
	  section.data(66).dtTransOffset = 271;
	
	  ;% rtP.SineWaveA_PSin
	  section.data(67).logicalSrcIdx = 66;
	  section.data(67).dtTransOffset = 272;
	
	  ;% rtP.SineWaveA_PCos
	  section.data(68).logicalSrcIdx = 67;
	  section.data(68).dtTransOffset = 273;
	
	  ;% rtP.SineWaveB_Amp
	  section.data(69).logicalSrcIdx = 68;
	  section.data(69).dtTransOffset = 274;
	
	  ;% rtP.SineWaveB_Bias
	  section.data(70).logicalSrcIdx = 69;
	  section.data(70).dtTransOffset = 275;
	
	  ;% rtP.SineWaveB_Hsin
	  section.data(71).logicalSrcIdx = 70;
	  section.data(71).dtTransOffset = 276;
	
	  ;% rtP.SineWaveB_HCos
	  section.data(72).logicalSrcIdx = 71;
	  section.data(72).dtTransOffset = 277;
	
	  ;% rtP.SineWaveB_PSin
	  section.data(73).logicalSrcIdx = 72;
	  section.data(73).dtTransOffset = 278;
	
	  ;% rtP.SineWaveB_PCos
	  section.data(74).logicalSrcIdx = 73;
	  section.data(74).dtTransOffset = 279;
	
	  ;% rtP.SineWaveC_Amp
	  section.data(75).logicalSrcIdx = 74;
	  section.data(75).dtTransOffset = 280;
	
	  ;% rtP.SineWaveC_Bias
	  section.data(76).logicalSrcIdx = 75;
	  section.data(76).dtTransOffset = 281;
	
	  ;% rtP.SineWaveC_Hsin
	  section.data(77).logicalSrcIdx = 76;
	  section.data(77).dtTransOffset = 282;
	
	  ;% rtP.SineWaveC_HCos
	  section.data(78).logicalSrcIdx = 77;
	  section.data(78).dtTransOffset = 283;
	
	  ;% rtP.SineWaveC_PSin
	  section.data(79).logicalSrcIdx = 78;
	  section.data(79).dtTransOffset = 284;
	
	  ;% rtP.SineWaveC_PCos
	  section.data(80).logicalSrcIdx = 79;
	  section.data(80).dtTransOffset = 285;
	
	  ;% rtP.StateSpace_AS_param
	  section.data(81).logicalSrcIdx = 80;
	  section.data(81).dtTransOffset = 286;
	
	  ;% rtP.StateSpace_BS_param
	  section.data(82).logicalSrcIdx = 81;
	  section.data(82).dtTransOffset = 322;
	
	  ;% rtP.StateSpace_CS_param
	  section.data(83).logicalSrcIdx = 82;
	  section.data(83).dtTransOffset = 388;
	
	  ;% rtP.StateSpace_DS_param
	  section.data(84).logicalSrcIdx = 83;
	  section.data(84).dtTransOffset = 688;
	
	  ;% rtP.StateSpace_X0_param
	  section.data(85).logicalSrcIdx = 84;
	  section.data(85).dtTransOffset = 1238;
	
	  ;% rtP.integrator_IC
	  section.data(86).logicalSrcIdx = 85;
	  section.data(86).dtTransOffset = 1244;
	
	  ;% rtP.TransportDelay_Delay
	  section.data(87).logicalSrcIdx = 86;
	  section.data(87).dtTransOffset = 1245;
	
	  ;% rtP.TransportDelay_InitOutput
	  section.data(88).logicalSrcIdx = 87;
	  section.data(88).dtTransOffset = 1246;
	
	  ;% rtP.K1_Value
	  section.data(89).logicalSrcIdx = 88;
	  section.data(89).dtTransOffset = 1247;
	
	  ;% rtP.Memory_InitialCondition
	  section.data(90).logicalSrcIdx = 89;
	  section.data(90).dtTransOffset = 1248;
	
	  ;% rtP.integrator_IC_kbu50edsmc
	  section.data(91).logicalSrcIdx = 90;
	  section.data(91).dtTransOffset = 1249;
	
	  ;% rtP.TransportDelay_Delay_gkek32v2fo
	  section.data(92).logicalSrcIdx = 91;
	  section.data(92).dtTransOffset = 1250;
	
	  ;% rtP.TransportDelay_InitOutput_gepskaykr5
	  section.data(93).logicalSrcIdx = 92;
	  section.data(93).dtTransOffset = 1251;
	
	  ;% rtP.K1_Value_mjkioortvy
	  section.data(94).logicalSrcIdx = 93;
	  section.data(94).dtTransOffset = 1252;
	
	  ;% rtP.Memory_InitialCondition_e0ydmt0d1l
	  section.data(95).logicalSrcIdx = 94;
	  section.data(95).dtTransOffset = 1253;
	
	  ;% rtP.RadDeg_Gain
	  section.data(96).logicalSrcIdx = 95;
	  section.data(96).dtTransOffset = 1254;
	
	  ;% rtP.integrator_IC_j24moklbcm
	  section.data(97).logicalSrcIdx = 96;
	  section.data(97).dtTransOffset = 1255;
	
	  ;% rtP.TransportDelay_Delay_mvxoy0gg0s
	  section.data(98).logicalSrcIdx = 97;
	  section.data(98).dtTransOffset = 1256;
	
	  ;% rtP.TransportDelay_InitOutput_a33cxqs542
	  section.data(99).logicalSrcIdx = 98;
	  section.data(99).dtTransOffset = 1257;
	
	  ;% rtP.K1_Value_k2kyb4ovnj
	  section.data(100).logicalSrcIdx = 99;
	  section.data(100).dtTransOffset = 1258;
	
	  ;% rtP.Memory_InitialCondition_l14qxrwnwp
	  section.data(101).logicalSrcIdx = 100;
	  section.data(101).dtTransOffset = 1259;
	
	  ;% rtP.integrator_IC_lxfy54g0cz
	  section.data(102).logicalSrcIdx = 101;
	  section.data(102).dtTransOffset = 1260;
	
	  ;% rtP.TransportDelay_Delay_bifxyqg4be
	  section.data(103).logicalSrcIdx = 102;
	  section.data(103).dtTransOffset = 1261;
	
	  ;% rtP.TransportDelay_InitOutput_kfzzmtnzlj
	  section.data(104).logicalSrcIdx = 103;
	  section.data(104).dtTransOffset = 1262;
	
	  ;% rtP.K1_Value_pj1iwyknqz
	  section.data(105).logicalSrcIdx = 104;
	  section.data(105).dtTransOffset = 1263;
	
	  ;% rtP.Memory_InitialCondition_m5lga3j4fd
	  section.data(106).logicalSrcIdx = 105;
	  section.data(106).dtTransOffset = 1264;
	
	  ;% rtP.RadDeg_Gain_mu3nul0msz
	  section.data(107).logicalSrcIdx = 106;
	  section.data(107).dtTransOffset = 1265;
	
	  ;% rtP.Gain4_Gain
	  section.data(108).logicalSrcIdx = 107;
	  section.data(108).dtTransOffset = 1266;
	
	  ;% rtP.Gain_Gain_hktcntwr5c
	  section.data(109).logicalSrcIdx = 108;
	  section.data(109).dtTransOffset = 1267;
	
	  ;% rtP.Gain1_Gain_nmin1oira0
	  section.data(110).logicalSrcIdx = 109;
	  section.data(110).dtTransOffset = 1268;
	
	  ;% rtP.u_Gain
	  section.data(111).logicalSrcIdx = 110;
	  section.data(111).dtTransOffset = 1269;
	
	  ;% rtP.u_Gain_prbig4sao4
	  section.data(112).logicalSrcIdx = 111;
	  section.data(112).dtTransOffset = 1270;
	
	  ;% rtP.donotdeletethisgain_Gain
	  section.data(113).logicalSrcIdx = 112;
	  section.data(113).dtTransOffset = 1271;
	
	  ;% rtP.donotdeletethisgain_Gain_mrsxwkh4tp
	  section.data(114).logicalSrcIdx = 113;
	  section.data(114).dtTransOffset = 1272;
	
	  ;% rtP.donotdeletethisgain_Gain_lbvwbky1pa
	  section.data(115).logicalSrcIdx = 114;
	  section.data(115).dtTransOffset = 1273;
	
	  ;% rtP.Kv1_Gain
	  section.data(116).logicalSrcIdx = 115;
	  section.data(116).dtTransOffset = 1274;
	
	  ;% rtP.donotdeletethisgain_Gain_jxggkyb4oj
	  section.data(117).logicalSrcIdx = 116;
	  section.data(117).dtTransOffset = 1275;
	
	  ;% rtP.donotdeletethisgain_Gain_p0laf5fkqn
	  section.data(118).logicalSrcIdx = 117;
	  section.data(118).dtTransOffset = 1276;
	
	  ;% rtP.donotdeletethisgain_Gain_j5mfsbj1ew
	  section.data(119).logicalSrcIdx = 118;
	  section.data(119).dtTransOffset = 1277;
	
	  ;% rtP.Kv_Gain
	  section.data(120).logicalSrcIdx = 119;
	  section.data(120).dtTransOffset = 1278;
	
	  ;% rtP.sinwt_Amp
	  section.data(121).logicalSrcIdx = 120;
	  section.data(121).dtTransOffset = 1279;
	
	  ;% rtP.sinwt_Bias
	  section.data(122).logicalSrcIdx = 121;
	  section.data(122).dtTransOffset = 1280;
	
	  ;% rtP.sinwt_Freq
	  section.data(123).logicalSrcIdx = 122;
	  section.data(123).dtTransOffset = 1281;
	
	  ;% rtP.sinwt_Phase
	  section.data(124).logicalSrcIdx = 123;
	  section.data(124).dtTransOffset = 1282;
	
	  ;% rtP.coswt_Amp
	  section.data(125).logicalSrcIdx = 124;
	  section.data(125).dtTransOffset = 1283;
	
	  ;% rtP.coswt_Bias
	  section.data(126).logicalSrcIdx = 125;
	  section.data(126).dtTransOffset = 1284;
	
	  ;% rtP.coswt_Freq
	  section.data(127).logicalSrcIdx = 126;
	  section.data(127).dtTransOffset = 1285;
	
	  ;% rtP.coswt_Phase
	  section.data(128).logicalSrcIdx = 127;
	  section.data(128).dtTransOffset = 1286;
	
	  ;% rtP.sinwt_Amp_ggpyeyy0ei
	  section.data(129).logicalSrcIdx = 128;
	  section.data(129).dtTransOffset = 1287;
	
	  ;% rtP.sinwt_Bias_aiqrlpqukr
	  section.data(130).logicalSrcIdx = 129;
	  section.data(130).dtTransOffset = 1288;
	
	  ;% rtP.sinwt_Freq_a0silddums
	  section.data(131).logicalSrcIdx = 130;
	  section.data(131).dtTransOffset = 1289;
	
	  ;% rtP.sinwt_Phase_pauodsxphx
	  section.data(132).logicalSrcIdx = 131;
	  section.data(132).dtTransOffset = 1290;
	
	  ;% rtP.coswt_Amp_mejccdpyws
	  section.data(133).logicalSrcIdx = 132;
	  section.data(133).dtTransOffset = 1291;
	
	  ;% rtP.coswt_Bias_acdrihp4mg
	  section.data(134).logicalSrcIdx = 133;
	  section.data(134).dtTransOffset = 1292;
	
	  ;% rtP.coswt_Freq_dexl0rdjce
	  section.data(135).logicalSrcIdx = 134;
	  section.data(135).dtTransOffset = 1293;
	
	  ;% rtP.coswt_Phase_gsew12la0i
	  section.data(136).logicalSrcIdx = 135;
	  section.data(136).dtTransOffset = 1294;
	
	  ;% rtP.uLl_q_Gain_nj2ycls33u
	  section.data(137).logicalSrcIdx = 136;
	  section.data(137).dtTransOffset = 1295;
	
	  ;% rtP.Switch3_Threshold
	  section.data(138).logicalSrcIdx = 137;
	  section.data(138).dtTransOffset = 1298;
	
	  ;% rtP.uLl_d_Gain_mutaj0uj3h
	  section.data(139).logicalSrcIdx = 138;
	  section.data(139).dtTransOffset = 1299;
	
	  ;% rtP.u_Vb_Gain
	  section.data(140).logicalSrcIdx = 139;
	  section.data(140).dtTransOffset = 1302;
	
	  ;% rtP.Switch2_Threshold_auuawawddv
	  section.data(141).logicalSrcIdx = 140;
	  section.data(141).dtTransOffset = 1303;
	
	  ;% rtP.Gain_Gain_olfnewahm1
	  section.data(142).logicalSrcIdx = 141;
	  section.data(142).dtTransOffset = 1304;
	
	  ;% rtP.Gain_Gain_pycxugwvd0
	  section.data(143).logicalSrcIdx = 142;
	  section.data(143).dtTransOffset = 1305;
	
	  ;% rtP.Gain1_Gain_j0bqxbk0lz
	  section.data(144).logicalSrcIdx = 143;
	  section.data(144).dtTransOffset = 1306;
	
	  ;% rtP.outputformatting_Gain
	  section.data(145).logicalSrcIdx = 144;
	  section.data(145).dtTransOffset = 1307;
	
	  ;% rtP.Gain_Gain_bpzmbe1g2k
	  section.data(146).logicalSrcIdx = 145;
	  section.data(146).dtTransOffset = 1325;
	
	  ;% rtP.Gain1_Gain_h3tkw2h2wq
	  section.data(147).logicalSrcIdx = 146;
	  section.data(147).dtTransOffset = 1326;
	
	  ;% rtP.Gain3_Gain
	  section.data(148).logicalSrcIdx = 147;
	  section.data(148).dtTransOffset = 1327;
	
	  ;% rtP.u1_Gain
	  section.data(149).logicalSrcIdx = 148;
	  section.data(149).dtTransOffset = 1328;
	
	  ;% rtP.dw_delay_InitialCondition
	  section.data(150).logicalSrcIdx = 149;
	  section.data(150).dtTransOffset = 1330;
	
	  ;% rtP.F2_Gain
	  section.data(151).logicalSrcIdx = 150;
	  section.data(151).dtTransOffset = 1331;
	
	  ;% rtP.dw_predict_InitialCondition
	  section.data(152).logicalSrcIdx = 151;
	  section.data(152).dtTransOffset = 1332;
	
	  ;% rtP.voltages_InitialCondition
	  section.data(153).logicalSrcIdx = 152;
	  section.data(153).dtTransOffset = 1333;
	
	  ;% rtP.IC_Threshold
	  section.data(154).logicalSrcIdx = 153;
	  section.data(154).dtTransOffset = 1334;
	
	  ;% rtP.u_Pb_Gain
	  section.data(155).logicalSrcIdx = 154;
	  section.data(155).dtTransOffset = 1335;
	
	  ;% rtP.F_Gain
	  section.data(156).logicalSrcIdx = 155;
	  section.data(156).dtTransOffset = 1336;
	
	  ;% rtP.u2H_Gain
	  section.data(157).logicalSrcIdx = 156;
	  section.data(157).dtTransOffset = 1337;
	
	  ;% rtP.theta_gainval
	  section.data(158).logicalSrcIdx = 157;
	  section.data(158).dtTransOffset = 1338;
	
	  ;% rtP.theta_IC
	  section.data(159).logicalSrcIdx = 158;
	  section.data(159).dtTransOffset = 1339;
	
	  ;% rtP.units_Gain
	  section.data(160).logicalSrcIdx = 159;
	  section.data(160).dtTransOffset = 1340;
	
	  ;% rtP.units1_Gain
	  section.data(161).logicalSrcIdx = 160;
	  section.data(161).dtTransOffset = 1341;
	
	  ;% rtP.t_Gain
	  section.data(162).logicalSrcIdx = 161;
	  section.data(162).dtTransOffset = 1342;
	
	  ;% rtP.units2_Gain
	  section.data(163).logicalSrcIdx = 162;
	  section.data(163).dtTransOffset = 1343;
	
	  ;% rtP.Rotorspeeddeviationdw_gainval
	  section.data(164).logicalSrcIdx = 163;
	  section.data(164).dtTransOffset = 1344;
	
	  ;% rtP.Rotorspeeddeviationdw_IC
	  section.data(165).logicalSrcIdx = 164;
	  section.data(165).dtTransOffset = 1345;
	
	  ;% rtP.webase_Gain
	  section.data(166).logicalSrcIdx = 165;
	  section.data(166).dtTransOffset = 1346;
	
	  ;% rtP.web3_Gain
	  section.data(167).logicalSrcIdx = 166;
	  section.data(167).dtTransOffset = 1347;
	
	  ;% rtP.C4_Value
	  section.data(168).logicalSrcIdx = 167;
	  section.data(168).dtTransOffset = 1348;
	
	  ;% rtP.LookUpTable_XData_i4mu3ctko4
	  section.data(169).logicalSrcIdx = 168;
	  section.data(169).dtTransOffset = 1349;
	
	  ;% rtP.LookUpTable_YData_epertdpbpx
	  section.data(170).logicalSrcIdx = 169;
	  section.data(170).dtTransOffset = 1355;
	
	  ;% rtP.Switch3_Threshold_o2qhhfyd2s
	  section.data(171).logicalSrcIdx = 170;
	  section.data(171).dtTransOffset = 1361;
	
	  ;% rtP.Switch3_Threshold_k0uagz5gcg
	  section.data(172).logicalSrcIdx = 171;
	  section.data(172).dtTransOffset = 1362;
	
	  ;% rtP.C4_Value_fxleokhe0y
	  section.data(173).logicalSrcIdx = 172;
	  section.data(173).dtTransOffset = 1363;
	
	  ;% rtP.Switch3_Threshold_icm4fmynnh
	  section.data(174).logicalSrcIdx = 173;
	  section.data(174).dtTransOffset = 1364;
	
	  ;% rtP.C4_Value_eoqp0wijkf
	  section.data(175).logicalSrcIdx = 174;
	  section.data(175).dtTransOffset = 1365;
	
	  ;% rtP.Switch3_Threshold_hdegddai5u
	  section.data(176).logicalSrcIdx = 175;
	  section.data(176).dtTransOffset = 1366;
	
	  ;% rtP.donotdeletethisgain_Gain_g4a3i3bkvy
	  section.data(177).logicalSrcIdx = 176;
	  section.data(177).dtTransOffset = 1367;
	
	  ;% rtP.donotdeletethisgain_Gain_p2gxvtukfv
	  section.data(178).logicalSrcIdx = 177;
	  section.data(178).dtTransOffset = 1368;
	
	  ;% rtP.donotdeletethisgain_Gain_is2ce52gq3
	  section.data(179).logicalSrcIdx = 178;
	  section.data(179).dtTransOffset = 1369;
	
	  ;% rtP.Kv_Gain_b1jtzjwn5z
	  section.data(180).logicalSrcIdx = 179;
	  section.data(180).dtTransOffset = 1370;
	
	  ;% rtP.donotdeletethisgain_Gain_h5sheo5agc
	  section.data(181).logicalSrcIdx = 180;
	  section.data(181).dtTransOffset = 1371;
	
	  ;% rtP.donotdeletethisgain_Gain_idjvkwlnjp
	  section.data(182).logicalSrcIdx = 181;
	  section.data(182).dtTransOffset = 1372;
	
	  ;% rtP.donotdeletethisgain_Gain_iiwqs0rjpv
	  section.data(183).logicalSrcIdx = 182;
	  section.data(183).dtTransOffset = 1373;
	
	  ;% rtP.Kv1_Gain_fhx1xa0qe5
	  section.data(184).logicalSrcIdx = 183;
	  section.data(184).dtTransOffset = 1374;
	
	  ;% rtP.donotdeletethisgain_Gain_jc3l43jy02
	  section.data(185).logicalSrcIdx = 184;
	  section.data(185).dtTransOffset = 1375;
	
	  ;% rtP.donotdeletethisgain_Gain_ixohazj2yz
	  section.data(186).logicalSrcIdx = 185;
	  section.data(186).dtTransOffset = 1376;
	
	  ;% rtP.donotdeletethisgain_Gain_ijcgmen2vo
	  section.data(187).logicalSrcIdx = 186;
	  section.data(187).dtTransOffset = 1377;
	
	  ;% rtP.Kv_Gain_hbrwnhtc5z
	  section.data(188).logicalSrcIdx = 187;
	  section.data(188).dtTransOffset = 1378;
	
	  ;% rtP.donotdeletethisgain_Gain_dftutozx3l
	  section.data(189).logicalSrcIdx = 188;
	  section.data(189).dtTransOffset = 1379;
	
	  ;% rtP.donotdeletethisgain_Gain_dvccqwmfov
	  section.data(190).logicalSrcIdx = 189;
	  section.data(190).dtTransOffset = 1380;
	
	  ;% rtP.donotdeletethisgain_Gain_b3iqzvyj5x
	  section.data(191).logicalSrcIdx = 190;
	  section.data(191).dtTransOffset = 1381;
	
	  ;% rtP.Kv1_Gain_bls2zebxwe
	  section.data(192).logicalSrcIdx = 191;
	  section.data(192).dtTransOffset = 1382;
	
	  ;% rtP.donotdeletethisgain_Gain_ckgop4gksi
	  section.data(193).logicalSrcIdx = 192;
	  section.data(193).dtTransOffset = 1383;
	
	  ;% rtP.donotdeletethisgain_Gain_gex5e0120e
	  section.data(194).logicalSrcIdx = 193;
	  section.data(194).dtTransOffset = 1384;
	
	  ;% rtP.donotdeletethisgain_Gain_mnyedn5jj1
	  section.data(195).logicalSrcIdx = 194;
	  section.data(195).dtTransOffset = 1385;
	
	  ;% rtP.Kv_Gain_dc42uxbym3
	  section.data(196).logicalSrcIdx = 195;
	  section.data(196).dtTransOffset = 1386;
	
	  ;% rtP.donotdeletethisgain_Gain_nsmmc33j4l
	  section.data(197).logicalSrcIdx = 196;
	  section.data(197).dtTransOffset = 1387;
	
	  ;% rtP.donotdeletethisgain_Gain_oc13lni3fq
	  section.data(198).logicalSrcIdx = 197;
	  section.data(198).dtTransOffset = 1388;
	
	  ;% rtP.donotdeletethisgain_Gain_ghrwzwq2wl
	  section.data(199).logicalSrcIdx = 198;
	  section.data(199).dtTransOffset = 1389;
	
	  ;% rtP.Kv1_Gain_js4lqtbwg1
	  section.data(200).logicalSrcIdx = 199;
	  section.data(200).dtTransOffset = 1390;
	
	  ;% rtP.donotdeletethisgain_Gain_inxi5cfmng
	  section.data(201).logicalSrcIdx = 200;
	  section.data(201).dtTransOffset = 1391;
	
	  ;% rtP.donotdeletethisgain_Gain_enynbgjy2q
	  section.data(202).logicalSrcIdx = 201;
	  section.data(202).dtTransOffset = 1392;
	
	  ;% rtP.donotdeletethisgain_Gain_ghgnnpri0h
	  section.data(203).logicalSrcIdx = 202;
	  section.data(203).dtTransOffset = 1393;
	
	  ;% rtP.Kv_Gain_gr1trvmmq4
	  section.data(204).logicalSrcIdx = 203;
	  section.data(204).dtTransOffset = 1394;
	
	  ;% rtP.donotdeletethisgain_Gain_fdstflfpdl
	  section.data(205).logicalSrcIdx = 204;
	  section.data(205).dtTransOffset = 1395;
	
	  ;% rtP.donotdeletethisgain_Gain_hwxarrm2xf
	  section.data(206).logicalSrcIdx = 205;
	  section.data(206).dtTransOffset = 1396;
	
	  ;% rtP.donotdeletethisgain_Gain_jpsgb554cq
	  section.data(207).logicalSrcIdx = 206;
	  section.data(207).dtTransOffset = 1397;
	
	  ;% rtP.Kv1_Gain_lejhoixvzv
	  section.data(208).logicalSrcIdx = 207;
	  section.data(208).dtTransOffset = 1398;
	
	  ;% rtP.donotdeletethisgain_Gain_akjxwy4mzh
	  section.data(209).logicalSrcIdx = 208;
	  section.data(209).dtTransOffset = 1399;
	
	  ;% rtP.donotdeletethisgain_Gain_m1o3yt3pmb
	  section.data(210).logicalSrcIdx = 209;
	  section.data(210).dtTransOffset = 1400;
	
	  ;% rtP.donotdeletethisgain_Gain_bdqv4z3h20
	  section.data(211).logicalSrcIdx = 210;
	  section.data(211).dtTransOffset = 1401;
	
	  ;% rtP.Kv_Gain_dryr11wp4y
	  section.data(212).logicalSrcIdx = 211;
	  section.data(212).dtTransOffset = 1402;
	
	  ;% rtP.donotdeletethisgain_Gain_dyxa4y4pb4
	  section.data(213).logicalSrcIdx = 212;
	  section.data(213).dtTransOffset = 1403;
	
	  ;% rtP.donotdeletethisgain_Gain_kgtmebmbqg
	  section.data(214).logicalSrcIdx = 213;
	  section.data(214).dtTransOffset = 1404;
	
	  ;% rtP.donotdeletethisgain_Gain_pn4mhzapma
	  section.data(215).logicalSrcIdx = 214;
	  section.data(215).dtTransOffset = 1405;
	
	  ;% rtP.Kv1_Gain_itak2vpaxj
	  section.data(216).logicalSrcIdx = 215;
	  section.data(216).dtTransOffset = 1406;
	
	  ;% rtP.donotdeletethisgain_Gain_lb32xlqutz
	  section.data(217).logicalSrcIdx = 216;
	  section.data(217).dtTransOffset = 1407;
	
	  ;% rtP.donotdeletethisgain_Gain_e1t00s2jpf
	  section.data(218).logicalSrcIdx = 217;
	  section.data(218).dtTransOffset = 1408;
	
	  ;% rtP.donotdeletethisgain_Gain_kznkprn5og
	  section.data(219).logicalSrcIdx = 218;
	  section.data(219).dtTransOffset = 1409;
	
	  ;% rtP.Kv_Gain_ccfkg0ba2b
	  section.data(220).logicalSrcIdx = 219;
	  section.data(220).dtTransOffset = 1410;
	
	  ;% rtP.donotdeletethisgain_Gain_krlg4qa22u
	  section.data(221).logicalSrcIdx = 220;
	  section.data(221).dtTransOffset = 1411;
	
	  ;% rtP.donotdeletethisgain_Gain_hxax2d03xl
	  section.data(222).logicalSrcIdx = 221;
	  section.data(222).dtTransOffset = 1412;
	
	  ;% rtP.donotdeletethisgain_Gain_fgel1ns2zy
	  section.data(223).logicalSrcIdx = 222;
	  section.data(223).dtTransOffset = 1413;
	
	  ;% rtP.Kv1_Gain_oi10siy4fh
	  section.data(224).logicalSrcIdx = 223;
	  section.data(224).dtTransOffset = 1414;
	
	  ;% rtP.C4_Value_gbl0dsqzhj
	  section.data(225).logicalSrcIdx = 224;
	  section.data(225).dtTransOffset = 1415;
	
	  ;% rtP.LookUpTable_XData_bysejxwrcs
	  section.data(226).logicalSrcIdx = 225;
	  section.data(226).dtTransOffset = 1416;
	
	  ;% rtP.LookUpTable_YData_anwymjpndi
	  section.data(227).logicalSrcIdx = 226;
	  section.data(227).dtTransOffset = 1422;
	
	  ;% rtP.Switch3_Threshold_js4dz2naas
	  section.data(228).logicalSrcIdx = 227;
	  section.data(228).dtTransOffset = 1428;
	
	  ;% rtP.Switch3_Threshold_g44ubd0yky
	  section.data(229).logicalSrcIdx = 228;
	  section.data(229).dtTransOffset = 1429;
	
	  ;% rtP.C4_Value_exqcfpjqxr
	  section.data(230).logicalSrcIdx = 229;
	  section.data(230).dtTransOffset = 1430;
	
	  ;% rtP.Switch3_Threshold_c15c5kf1pp
	  section.data(231).logicalSrcIdx = 230;
	  section.data(231).dtTransOffset = 1431;
	
	  ;% rtP.C4_Value_ktcw3s24rj
	  section.data(232).logicalSrcIdx = 231;
	  section.data(232).dtTransOffset = 1432;
	
	  ;% rtP.Switch3_Threshold_c2ubqk250k
	  section.data(233).logicalSrcIdx = 232;
	  section.data(233).dtTransOffset = 1433;
	
	  ;% rtP.SwitchCurrents_Value
	  section.data(234).logicalSrcIdx = 233;
	  section.data(234).dtTransOffset = 1434;
	
	  ;% rtP.Pm1_Value
	  section.data(235).logicalSrcIdx = 234;
	  section.data(235).dtTransOffset = 1440;
	
	  ;% rtP.Constant2_Value
	  section.data(236).logicalSrcIdx = 235;
	  section.data(236).dtTransOffset = 1441;
	
	  ;% rtP.Vf_Value
	  section.data(237).logicalSrcIdx = 236;
	  section.data(237).dtTransOffset = 1442;
	
	  ;% rtP.N_Gain
	  section.data(238).logicalSrcIdx = 237;
	  section.data(238).dtTransOffset = 1443;
	
	  ;% rtP.Constant1_Value_iglewzfyey
	  section.data(239).logicalSrcIdx = 238;
	  section.data(239).dtTransOffset = 1444;
	
	  ;% rtP.Constant2_Value_a35hujaypn
	  section.data(240).logicalSrcIdx = 239;
	  section.data(240).dtTransOffset = 1445;
	
	  ;% rtP.Constant3_Value
	  section.data(241).logicalSrcIdx = 240;
	  section.data(241).dtTransOffset = 1446;
	
	  ;% rtP.Constant4_Value
	  section.data(242).logicalSrcIdx = 241;
	  section.data(242).dtTransOffset = 1447;
	
	  ;% rtP.Constant5_Value
	  section.data(243).logicalSrcIdx = 242;
	  section.data(243).dtTransOffset = 1483;
	
	  ;% rtP.Constant6_Value
	  section.data(244).logicalSrcIdx = 243;
	  section.data(244).dtTransOffset = 1484;
	
	  ;% rtP.Constant8_Value
	  section.data(245).logicalSrcIdx = 244;
	  section.data(245).dtTransOffset = 1520;
	
	  ;% rtP.Laqd_nosat_Value
	  section.data(246).logicalSrcIdx = 245;
	  section.data(246).dtTransOffset = 1521;
	
	  ;% rtP.Lmqd_nosat_Value
	  section.data(247).logicalSrcIdx = 246;
	  section.data(247).dtTransOffset = 1523;
	
	  ;% rtP.u1_Value_cbuqdiipgi
	  section.data(248).logicalSrcIdx = 247;
	  section.data(248).dtTransOffset = 1525;
	
	  ;% rtP.u5_Value_ochldviepj
	  section.data(249).logicalSrcIdx = 248;
	  section.data(249).dtTransOffset = 1561;
	
	  ;% rtP.Vkd0Vkq10Vkq20_Value
	  section.data(250).logicalSrcIdx = 249;
	  section.data(250).dtTransOffset = 1597;
	
	  ;% rtP.nominalspeed_Value
	  section.data(251).logicalSrcIdx = 250;
	  section.data(251).dtTransOffset = 1600;
	
	  ;% rtP.VfPP1_Value
	  section.data(252).logicalSrcIdx = 251;
	  section.data(252).dtTransOffset = 1601;
	
	  ;% rtP.C4_Value_gcbzcnw2m4
	  section.data(253).logicalSrcIdx = 252;
	  section.data(253).dtTransOffset = 1602;
	
	  ;% rtP.com_Value
	  section.data(254).logicalSrcIdx = 253;
	  section.data(254).dtTransOffset = 1603;
	
	  ;% rtP.C4_Value_ouf3ao4h2o
	  section.data(255).logicalSrcIdx = 254;
	  section.data(255).dtTransOffset = 1604;
	
	  ;% rtP.Constant5_Value_eyvmukmjzw
	  section.data(256).logicalSrcIdx = 255;
	  section.data(256).dtTransOffset = 1605;
	
	  ;% rtP.com_Value_e0niij5lrb
	  section.data(257).logicalSrcIdx = 256;
	  section.data(257).dtTransOffset = 1606;
	
      nTotData = nTotData + section.nData;
      paramMap.sections(1) = section;
      clear section
      
      section.nData     = 4;
      section.data(4)  = dumData; %prealloc
      
	  ;% rtP.Constant1_Value_afrrlaacip
	  section.data(1).logicalSrcIdx = 257;
	  section.data(1).dtTransOffset = 0;
	
	  ;% rtP.Constant2_Value_lxtjw1dx3o
	  section.data(2).logicalSrcIdx = 258;
	  section.data(2).dtTransOffset = 1;
	
	  ;% rtP.Constant3_Value_jspriozsq0
	  section.data(3).logicalSrcIdx = 259;
	  section.data(3).dtTransOffset = 2;
	
	  ;% rtP.Constant1_Value_nw4shdp34n
	  section.data(4).logicalSrcIdx = 260;
	  section.data(4).dtTransOffset = 3;
	
      nTotData = nTotData + section.nData;
      paramMap.sections(2) = section;
      clear section
      
    
      ;%
      ;% Non-auto Data (parameter)
      ;%
    

    ;%
    ;% Add final counts to struct.
    ;%
    paramMap.nTotData = nTotData;
    


  ;%**************************
  ;% Create Block Output Map *
  ;%**************************
      
    nTotData      = 0; %add to this count as we go
    nTotSects     = 3;
    sectIdxOffset = 0;
    
    ;%
    ;% Define dummy sections & preallocate arrays
    ;%
    dumSection.nData = -1;  
    dumSection.data  = [];
    
    dumData.logicalSrcIdx = -1;
    dumData.dtTransOffset = -1;
    
    ;%
    ;% Init/prealloc sigMap
    ;%
    sigMap.nSections           = nTotSects;
    sigMap.sectIdxOffset       = sectIdxOffset;
      sigMap.sections(nTotSects) = dumSection; %prealloc
    sigMap.nTotData            = -1;
    
    ;%
    ;% Auto data (rtB)
    ;%
      section.nData     = 2;
      section.data(2)  = dumData; %prealloc
      
	  ;% rtB.jzulpokwia
	  section.data(1).logicalSrcIdx = 0;
	  section.data(1).dtTransOffset = 0;
	
	  ;% rtB.dngbcewebp
	  section.data(2).logicalSrcIdx = 1;
	  section.data(2).dtTransOffset = 1;
	
      nTotData = nTotData + section.nData;
      sigMap.sections(1) = section;
      clear section
      
      section.nData     = 268;
      section.data(268)  = dumData; %prealloc
      
	  ;% rtB.o0laqbijxj
	  section.data(1).logicalSrcIdx = 2;
	  section.data(1).dtTransOffset = 0;
	
	  ;% rtB.phf1q3qtgy
	  section.data(2).logicalSrcIdx = 3;
	  section.data(2).dtTransOffset = 1;
	
	  ;% rtB.adxx2eh1g0
	  section.data(3).logicalSrcIdx = 4;
	  section.data(3).dtTransOffset = 2;
	
	  ;% rtB.fx0kk0lfk3
	  section.data(4).logicalSrcIdx = 5;
	  section.data(4).dtTransOffset = 3;
	
	  ;% rtB.ps502vbzlw
	  section.data(5).logicalSrcIdx = 6;
	  section.data(5).dtTransOffset = 4;
	
	  ;% rtB.pz2b2ixe3g
	  section.data(6).logicalSrcIdx = 7;
	  section.data(6).dtTransOffset = 5;
	
	  ;% rtB.mshc0z4y0b
	  section.data(7).logicalSrcIdx = 8;
	  section.data(7).dtTransOffset = 6;
	
	  ;% rtB.kxk50ui10l
	  section.data(8).logicalSrcIdx = 9;
	  section.data(8).dtTransOffset = 12;
	
	  ;% rtB.i2htmqgs5h
	  section.data(9).logicalSrcIdx = 10;
	  section.data(9).dtTransOffset = 48;
	
	  ;% rtB.d2l3x5dmoq
	  section.data(10).logicalSrcIdx = 11;
	  section.data(10).dtTransOffset = 54;
	
	  ;% rtB.a3kfgrogpp
	  section.data(11).logicalSrcIdx = 12;
	  section.data(11).dtTransOffset = 60;
	
	  ;% rtB.dggh1ark2b
	  section.data(12).logicalSrcIdx = 13;
	  section.data(12).dtTransOffset = 61;
	
	  ;% rtB.hleuoxbvtr
	  section.data(13).logicalSrcIdx = 14;
	  section.data(13).dtTransOffset = 62;
	
	  ;% rtB.gfcufizx5q
	  section.data(14).logicalSrcIdx = 15;
	  section.data(14).dtTransOffset = 63;
	
	  ;% rtB.ocwtlsstoa
	  section.data(15).logicalSrcIdx = 16;
	  section.data(15).dtTransOffset = 66;
	
	  ;% rtB.kc2wsb45fs
	  section.data(16).logicalSrcIdx = 17;
	  section.data(16).dtTransOffset = 68;
	
	  ;% rtB.l2zafpzrzu
	  section.data(17).logicalSrcIdx = 18;
	  section.data(17).dtTransOffset = 69;
	
	  ;% rtB.ohedl35wf1
	  section.data(18).logicalSrcIdx = 19;
	  section.data(18).dtTransOffset = 70;
	
	  ;% rtB.bkf5f3ujyq
	  section.data(19).logicalSrcIdx = 20;
	  section.data(19).dtTransOffset = 71;
	
	  ;% rtB.mcctrqnlij
	  section.data(20).logicalSrcIdx = 21;
	  section.data(20).dtTransOffset = 121;
	
	  ;% rtB.cuumld5jhq
	  section.data(21).logicalSrcIdx = 22;
	  section.data(21).dtTransOffset = 127;
	
	  ;% rtB.czblwzh330
	  section.data(22).logicalSrcIdx = 23;
	  section.data(22).dtTransOffset = 128;
	
	  ;% rtB.ed1n42eefk
	  section.data(23).logicalSrcIdx = 24;
	  section.data(23).dtTransOffset = 129;
	
	  ;% rtB.i524ua1zo0
	  section.data(24).logicalSrcIdx = 25;
	  section.data(24).dtTransOffset = 130;
	
	  ;% rtB.e11hdbixe4
	  section.data(25).logicalSrcIdx = 26;
	  section.data(25).dtTransOffset = 131;
	
	  ;% rtB.f3bj13s54m
	  section.data(26).logicalSrcIdx = 27;
	  section.data(26).dtTransOffset = 132;
	
	  ;% rtB.dljdyoojd5
	  section.data(27).logicalSrcIdx = 28;
	  section.data(27).dtTransOffset = 133;
	
	  ;% rtB.jfbplyjjoy
	  section.data(28).logicalSrcIdx = 29;
	  section.data(28).dtTransOffset = 134;
	
	  ;% rtB.mcclxrnvnu
	  section.data(29).logicalSrcIdx = 30;
	  section.data(29).dtTransOffset = 135;
	
	  ;% rtB.dv5odcc5un
	  section.data(30).logicalSrcIdx = 31;
	  section.data(30).dtTransOffset = 136;
	
	  ;% rtB.hu45skyzd5
	  section.data(31).logicalSrcIdx = 32;
	  section.data(31).dtTransOffset = 137;
	
	  ;% rtB.nxmiliesal
	  section.data(32).logicalSrcIdx = 33;
	  section.data(32).dtTransOffset = 138;
	
	  ;% rtB.lz5rdlkvrc
	  section.data(33).logicalSrcIdx = 34;
	  section.data(33).dtTransOffset = 139;
	
	  ;% rtB.a2aelbuuxd
	  section.data(34).logicalSrcIdx = 35;
	  section.data(34).dtTransOffset = 140;
	
	  ;% rtB.lb3o0vdenr
	  section.data(35).logicalSrcIdx = 36;
	  section.data(35).dtTransOffset = 141;
	
	  ;% rtB.lsnwzrnmnc
	  section.data(36).logicalSrcIdx = 37;
	  section.data(36).dtTransOffset = 142;
	
	  ;% rtB.m4o5gfq3hr
	  section.data(37).logicalSrcIdx = 38;
	  section.data(37).dtTransOffset = 143;
	
	  ;% rtB.o1nbxtrkf4
	  section.data(38).logicalSrcIdx = 39;
	  section.data(38).dtTransOffset = 144;
	
	  ;% rtB.jpdedqtzey
	  section.data(39).logicalSrcIdx = 40;
	  section.data(39).dtTransOffset = 145;
	
	  ;% rtB.jn3gd54hx5
	  section.data(40).logicalSrcIdx = 41;
	  section.data(40).dtTransOffset = 146;
	
	  ;% rtB.amlu2hdzhw
	  section.data(41).logicalSrcIdx = 42;
	  section.data(41).dtTransOffset = 147;
	
	  ;% rtB.jh3odldqrg
	  section.data(42).logicalSrcIdx = 43;
	  section.data(42).dtTransOffset = 148;
	
	  ;% rtB.caiqrgrb4h
	  section.data(43).logicalSrcIdx = 44;
	  section.data(43).dtTransOffset = 149;
	
	  ;% rtB.b5yrakypqk
	  section.data(44).logicalSrcIdx = 45;
	  section.data(44).dtTransOffset = 150;
	
	  ;% rtB.kyvh55ysq1
	  section.data(45).logicalSrcIdx = 46;
	  section.data(45).dtTransOffset = 151;
	
	  ;% rtB.njwnc1thzp
	  section.data(46).logicalSrcIdx = 47;
	  section.data(46).dtTransOffset = 152;
	
	  ;% rtB.fvgxbp0r0b
	  section.data(47).logicalSrcIdx = 48;
	  section.data(47).dtTransOffset = 153;
	
	  ;% rtB.h33eecit0h
	  section.data(48).logicalSrcIdx = 49;
	  section.data(48).dtTransOffset = 154;
	
	  ;% rtB.hcnu41udwm
	  section.data(49).logicalSrcIdx = 50;
	  section.data(49).dtTransOffset = 155;
	
	  ;% rtB.ixvhkcticg
	  section.data(50).logicalSrcIdx = 51;
	  section.data(50).dtTransOffset = 156;
	
	  ;% rtB.ihbtz02goz
	  section.data(51).logicalSrcIdx = 52;
	  section.data(51).dtTransOffset = 157;
	
	  ;% rtB.bz4ixbddyb
	  section.data(52).logicalSrcIdx = 53;
	  section.data(52).dtTransOffset = 158;
	
	  ;% rtB.nzu205zzco
	  section.data(53).logicalSrcIdx = 54;
	  section.data(53).dtTransOffset = 159;
	
	  ;% rtB.bbfgsoogaz
	  section.data(54).logicalSrcIdx = 55;
	  section.data(54).dtTransOffset = 160;
	
	  ;% rtB.dkxzgq2elf
	  section.data(55).logicalSrcIdx = 56;
	  section.data(55).dtTransOffset = 161;
	
	  ;% rtB.n1sf2oxpy1
	  section.data(56).logicalSrcIdx = 57;
	  section.data(56).dtTransOffset = 162;
	
	  ;% rtB.bsjs3yr3cm
	  section.data(57).logicalSrcIdx = 58;
	  section.data(57).dtTransOffset = 163;
	
	  ;% rtB.omi22g0ht3
	  section.data(58).logicalSrcIdx = 59;
	  section.data(58).dtTransOffset = 164;
	
	  ;% rtB.maqdked4sl
	  section.data(59).logicalSrcIdx = 60;
	  section.data(59).dtTransOffset = 165;
	
	  ;% rtB.l0q0zwxfui
	  section.data(60).logicalSrcIdx = 61;
	  section.data(60).dtTransOffset = 166;
	
	  ;% rtB.oecmwuiwrp
	  section.data(61).logicalSrcIdx = 62;
	  section.data(61).dtTransOffset = 169;
	
	  ;% rtB.m15cezxam4
	  section.data(62).logicalSrcIdx = 63;
	  section.data(62).dtTransOffset = 170;
	
	  ;% rtB.brcpuhmr2n
	  section.data(63).logicalSrcIdx = 64;
	  section.data(63).dtTransOffset = 171;
	
	  ;% rtB.cpugwweowj
	  section.data(64).logicalSrcIdx = 65;
	  section.data(64).dtTransOffset = 172;
	
	  ;% rtB.ht3pkpouae
	  section.data(65).logicalSrcIdx = 66;
	  section.data(65).dtTransOffset = 175;
	
	  ;% rtB.epw4e2nyr2
	  section.data(66).logicalSrcIdx = 67;
	  section.data(66).dtTransOffset = 176;
	
	  ;% rtB.onhlo5lk3i
	  section.data(67).logicalSrcIdx = 68;
	  section.data(67).dtTransOffset = 177;
	
	  ;% rtB.hvccs5ksdj
	  section.data(68).logicalSrcIdx = 69;
	  section.data(68).dtTransOffset = 178;
	
	  ;% rtB.mv11qhbyiu
	  section.data(69).logicalSrcIdx = 70;
	  section.data(69).dtTransOffset = 179;
	
	  ;% rtB.lxuyvf0kct
	  section.data(70).logicalSrcIdx = 71;
	  section.data(70).dtTransOffset = 180;
	
	  ;% rtB.cpurtfy0lf
	  section.data(71).logicalSrcIdx = 72;
	  section.data(71).dtTransOffset = 181;
	
	  ;% rtB.hdfe5utvyn
	  section.data(72).logicalSrcIdx = 73;
	  section.data(72).dtTransOffset = 182;
	
	  ;% rtB.gqo15jq4yq
	  section.data(73).logicalSrcIdx = 74;
	  section.data(73).dtTransOffset = 183;
	
	  ;% rtB.hwnr1apibg
	  section.data(74).logicalSrcIdx = 75;
	  section.data(74).dtTransOffset = 184;
	
	  ;% rtB.by0csc0mva
	  section.data(75).logicalSrcIdx = 76;
	  section.data(75).dtTransOffset = 187;
	
	  ;% rtB.l1lpbegscp
	  section.data(76).logicalSrcIdx = 77;
	  section.data(76).dtTransOffset = 189;
	
	  ;% rtB.ck4g14oqxl
	  section.data(77).logicalSrcIdx = 78;
	  section.data(77).dtTransOffset = 192;
	
	  ;% rtB.h5nuuqtufn
	  section.data(78).logicalSrcIdx = 79;
	  section.data(78).dtTransOffset = 193;
	
	  ;% rtB.bdvh4jtcpt
	  section.data(79).logicalSrcIdx = 80;
	  section.data(79).dtTransOffset = 196;
	
	  ;% rtB.l5qxq2mybh
	  section.data(80).logicalSrcIdx = 81;
	  section.data(80).dtTransOffset = 199;
	
	  ;% rtB.do04422dsh
	  section.data(81).logicalSrcIdx = 82;
	  section.data(81).dtTransOffset = 200;
	
	  ;% rtB.kctgawizah
	  section.data(82).logicalSrcIdx = 83;
	  section.data(82).dtTransOffset = 202;
	
	  ;% rtB.g2rmjbbycl
	  section.data(83).logicalSrcIdx = 84;
	  section.data(83).dtTransOffset = 204;
	
	  ;% rtB.ljwiyate1t
	  section.data(84).logicalSrcIdx = 85;
	  section.data(84).dtTransOffset = 205;
	
	  ;% rtB.lwg1hzp3ql
	  section.data(85).logicalSrcIdx = 86;
	  section.data(85).dtTransOffset = 206;
	
	  ;% rtB.c4xvwz5dvp
	  section.data(86).logicalSrcIdx = 87;
	  section.data(86).dtTransOffset = 208;
	
	  ;% rtB.hv3y4xtyo5
	  section.data(87).logicalSrcIdx = 88;
	  section.data(87).dtTransOffset = 209;
	
	  ;% rtB.otvyxcolz0
	  section.data(88).logicalSrcIdx = 89;
	  section.data(88).dtTransOffset = 210;
	
	  ;% rtB.ca1sh32w2e
	  section.data(89).logicalSrcIdx = 90;
	  section.data(89).dtTransOffset = 211;
	
	  ;% rtB.ogxs2lrdho
	  section.data(90).logicalSrcIdx = 91;
	  section.data(90).dtTransOffset = 212;
	
	  ;% rtB.n4glgvjxv2
	  section.data(91).logicalSrcIdx = 92;
	  section.data(91).dtTransOffset = 213;
	
	  ;% rtB.dq2eyjw50a
	  section.data(92).logicalSrcIdx = 93;
	  section.data(92).dtTransOffset = 214;
	
	  ;% rtB.fyirte2cly
	  section.data(93).logicalSrcIdx = 94;
	  section.data(93).dtTransOffset = 215;
	
	  ;% rtB.i2tqhru53h
	  section.data(94).logicalSrcIdx = 95;
	  section.data(94).dtTransOffset = 216;
	
	  ;% rtB.ahl15fuofz
	  section.data(95).logicalSrcIdx = 96;
	  section.data(95).dtTransOffset = 217;
	
	  ;% rtB.nnbfzmzsox
	  section.data(96).logicalSrcIdx = 97;
	  section.data(96).dtTransOffset = 218;
	
	  ;% rtB.nddeflp2rr
	  section.data(97).logicalSrcIdx = 98;
	  section.data(97).dtTransOffset = 236;
	
	  ;% rtB.b14bfbabib
	  section.data(98).logicalSrcIdx = 99;
	  section.data(98).dtTransOffset = 254;
	
	  ;% rtB.mwnm3xw2te
	  section.data(99).logicalSrcIdx = 100;
	  section.data(99).dtTransOffset = 255;
	
	  ;% rtB.gpq0cumlrp
	  section.data(100).logicalSrcIdx = 101;
	  section.data(100).dtTransOffset = 256;
	
	  ;% rtB.ooqvccplqr
	  section.data(101).logicalSrcIdx = 102;
	  section.data(101).dtTransOffset = 257;
	
	  ;% rtB.bsib0j2uiv
	  section.data(102).logicalSrcIdx = 103;
	  section.data(102).dtTransOffset = 258;
	
	  ;% rtB.iwzh4cmhgz
	  section.data(103).logicalSrcIdx = 104;
	  section.data(103).dtTransOffset = 260;
	
	  ;% rtB.kbpgzmhgnk
	  section.data(104).logicalSrcIdx = 105;
	  section.data(104).dtTransOffset = 262;
	
	  ;% rtB.kt4yqwu22g
	  section.data(105).logicalSrcIdx = 106;
	  section.data(105).dtTransOffset = 263;
	
	  ;% rtB.eekpbkk0js
	  section.data(106).logicalSrcIdx = 107;
	  section.data(106).dtTransOffset = 264;
	
	  ;% rtB.bzg42dyv2g
	  section.data(107).logicalSrcIdx = 108;
	  section.data(107).dtTransOffset = 265;
	
	  ;% rtB.gilkef2gvf
	  section.data(108).logicalSrcIdx = 109;
	  section.data(108).dtTransOffset = 266;
	
	  ;% rtB.i3cpus0vrs
	  section.data(109).logicalSrcIdx = 110;
	  section.data(109).dtTransOffset = 267;
	
	  ;% rtB.mnrqebdkci
	  section.data(110).logicalSrcIdx = 111;
	  section.data(110).dtTransOffset = 268;
	
	  ;% rtB.gwy50czbqx
	  section.data(111).logicalSrcIdx = 112;
	  section.data(111).dtTransOffset = 274;
	
	  ;% rtB.hjl4wiy52h
	  section.data(112).logicalSrcIdx = 113;
	  section.data(112).dtTransOffset = 275;
	
	  ;% rtB.gsp2mdotvq
	  section.data(113).logicalSrcIdx = 114;
	  section.data(113).dtTransOffset = 281;
	
	  ;% rtB.cuthot4b55
	  section.data(114).logicalSrcIdx = 115;
	  section.data(114).dtTransOffset = 282;
	
	  ;% rtB.m3hbl5oi51
	  section.data(115).logicalSrcIdx = 116;
	  section.data(115).dtTransOffset = 283;
	
	  ;% rtB.kr1dizdsxr
	  section.data(116).logicalSrcIdx = 117;
	  section.data(116).dtTransOffset = 284;
	
	  ;% rtB.gftya03rfs
	  section.data(117).logicalSrcIdx = 118;
	  section.data(117).dtTransOffset = 285;
	
	  ;% rtB.fmoyvyrdin
	  section.data(118).logicalSrcIdx = 119;
	  section.data(118).dtTransOffset = 286;
	
	  ;% rtB.h2yyrby5su
	  section.data(119).logicalSrcIdx = 120;
	  section.data(119).dtTransOffset = 287;
	
	  ;% rtB.jyft0oavfh
	  section.data(120).logicalSrcIdx = 121;
	  section.data(120).dtTransOffset = 288;
	
	  ;% rtB.ax2dm5rpub
	  section.data(121).logicalSrcIdx = 122;
	  section.data(121).dtTransOffset = 289;
	
	  ;% rtB.o3xqnnpyja
	  section.data(122).logicalSrcIdx = 123;
	  section.data(122).dtTransOffset = 290;
	
	  ;% rtB.k33xjpkxfj
	  section.data(123).logicalSrcIdx = 124;
	  section.data(123).dtTransOffset = 291;
	
	  ;% rtB.hlupe3wd1s
	  section.data(124).logicalSrcIdx = 125;
	  section.data(124).dtTransOffset = 292;
	
	  ;% rtB.ogr2enhepf
	  section.data(125).logicalSrcIdx = 126;
	  section.data(125).dtTransOffset = 293;
	
	  ;% rtB.bxfyqcorw3
	  section.data(126).logicalSrcIdx = 127;
	  section.data(126).dtTransOffset = 294;
	
	  ;% rtB.dn1jsu5b3a
	  section.data(127).logicalSrcIdx = 128;
	  section.data(127).dtTransOffset = 300;
	
	  ;% rtB.anv20tflb2
	  section.data(128).logicalSrcIdx = 129;
	  section.data(128).dtTransOffset = 301;
	
	  ;% rtB.eajcmlcqmc
	  section.data(129).logicalSrcIdx = 130;
	  section.data(129).dtTransOffset = 302;
	
	  ;% rtB.ejzz1lck3x
	  section.data(130).logicalSrcIdx = 131;
	  section.data(130).dtTransOffset = 303;
	
	  ;% rtB.clkwduumjv
	  section.data(131).logicalSrcIdx = 132;
	  section.data(131).dtTransOffset = 304;
	
	  ;% rtB.geos3slncn
	  section.data(132).logicalSrcIdx = 133;
	  section.data(132).dtTransOffset = 305;
	
	  ;% rtB.gmd2h4cezq
	  section.data(133).logicalSrcIdx = 134;
	  section.data(133).dtTransOffset = 306;
	
	  ;% rtB.i20vaspyrp
	  section.data(134).logicalSrcIdx = 135;
	  section.data(134).dtTransOffset = 307;
	
	  ;% rtB.mzzuyzwyzo
	  section.data(135).logicalSrcIdx = 136;
	  section.data(135).dtTransOffset = 308;
	
	  ;% rtB.oj12dzvfb4
	  section.data(136).logicalSrcIdx = 137;
	  section.data(136).dtTransOffset = 309;
	
	  ;% rtB.pu5azi4ouj
	  section.data(137).logicalSrcIdx = 138;
	  section.data(137).dtTransOffset = 310;
	
	  ;% rtB.p4d344o4a4
	  section.data(138).logicalSrcIdx = 139;
	  section.data(138).dtTransOffset = 311;
	
	  ;% rtB.pxtshpab0u
	  section.data(139).logicalSrcIdx = 140;
	  section.data(139).dtTransOffset = 312;
	
	  ;% rtB.orydyp31zs
	  section.data(140).logicalSrcIdx = 141;
	  section.data(140).dtTransOffset = 313;
	
	  ;% rtB.btppbtrakx
	  section.data(141).logicalSrcIdx = 142;
	  section.data(141).dtTransOffset = 314;
	
	  ;% rtB.embtezfk0j
	  section.data(142).logicalSrcIdx = 143;
	  section.data(142).dtTransOffset = 315;
	
	  ;% rtB.icofxpvarr
	  section.data(143).logicalSrcIdx = 144;
	  section.data(143).dtTransOffset = 318;
	
	  ;% rtB.n3yespd0xf
	  section.data(144).logicalSrcIdx = 145;
	  section.data(144).dtTransOffset = 319;
	
	  ;% rtB.kfeoejnqkx
	  section.data(145).logicalSrcIdx = 146;
	  section.data(145).dtTransOffset = 320;
	
	  ;% rtB.i5mjxvb2y1
	  section.data(146).logicalSrcIdx = 147;
	  section.data(146).dtTransOffset = 321;
	
	  ;% rtB.kalgaozq2e
	  section.data(147).logicalSrcIdx = 148;
	  section.data(147).dtTransOffset = 324;
	
	  ;% rtB.c3mov1tl1f
	  section.data(148).logicalSrcIdx = 149;
	  section.data(148).dtTransOffset = 325;
	
	  ;% rtB.mj5ueklf54
	  section.data(149).logicalSrcIdx = 150;
	  section.data(149).dtTransOffset = 326;
	
	  ;% rtB.chk1pd3rlp
	  section.data(150).logicalSrcIdx = 151;
	  section.data(150).dtTransOffset = 327;
	
	  ;% rtB.nfbe1f5aq4
	  section.data(151).logicalSrcIdx = 152;
	  section.data(151).dtTransOffset = 330;
	
	  ;% rtB.gpbkfrtopv
	  section.data(152).logicalSrcIdx = 153;
	  section.data(152).dtTransOffset = 331;
	
	  ;% rtB.emcqpstfhw
	  section.data(153).logicalSrcIdx = 154;
	  section.data(153).dtTransOffset = 332;
	
	  ;% rtB.o04nrxwopl
	  section.data(154).logicalSrcIdx = 155;
	  section.data(154).dtTransOffset = 333;
	
	  ;% rtB.m315dx4bxe
	  section.data(155).logicalSrcIdx = 156;
	  section.data(155).dtTransOffset = 336;
	
	  ;% rtB.lixhx51dy4
	  section.data(156).logicalSrcIdx = 157;
	  section.data(156).dtTransOffset = 337;
	
	  ;% rtB.arirsm2gtb
	  section.data(157).logicalSrcIdx = 158;
	  section.data(157).dtTransOffset = 338;
	
	  ;% rtB.aei40fflxj
	  section.data(158).logicalSrcIdx = 159;
	  section.data(158).dtTransOffset = 339;
	
	  ;% rtB.pchaeyb4wc
	  section.data(159).logicalSrcIdx = 160;
	  section.data(159).dtTransOffset = 342;
	
	  ;% rtB.geuzk04gqe
	  section.data(160).logicalSrcIdx = 161;
	  section.data(160).dtTransOffset = 343;
	
	  ;% rtB.nx2s4jnhlc
	  section.data(161).logicalSrcIdx = 162;
	  section.data(161).dtTransOffset = 344;
	
	  ;% rtB.k3tazd503n
	  section.data(162).logicalSrcIdx = 163;
	  section.data(162).dtTransOffset = 345;
	
	  ;% rtB.c5z4wv1mbn
	  section.data(163).logicalSrcIdx = 164;
	  section.data(163).dtTransOffset = 348;
	
	  ;% rtB.ixbmptgikc
	  section.data(164).logicalSrcIdx = 165;
	  section.data(164).dtTransOffset = 349;
	
	  ;% rtB.g1pav31o54
	  section.data(165).logicalSrcIdx = 166;
	  section.data(165).dtTransOffset = 350;
	
	  ;% rtB.g2dr1qmh1g
	  section.data(166).logicalSrcIdx = 167;
	  section.data(166).dtTransOffset = 351;
	
	  ;% rtB.or1sotd5ok
	  section.data(167).logicalSrcIdx = 168;
	  section.data(167).dtTransOffset = 354;
	
	  ;% rtB.oawoasztyv
	  section.data(168).logicalSrcIdx = 169;
	  section.data(168).dtTransOffset = 355;
	
	  ;% rtB.ez3n02akaw
	  section.data(169).logicalSrcIdx = 170;
	  section.data(169).dtTransOffset = 356;
	
	  ;% rtB.hm1ran4pnq
	  section.data(170).logicalSrcIdx = 171;
	  section.data(170).dtTransOffset = 357;
	
	  ;% rtB.im0zegy3zl
	  section.data(171).logicalSrcIdx = 172;
	  section.data(171).dtTransOffset = 360;
	
	  ;% rtB.ca0k4g2ixd
	  section.data(172).logicalSrcIdx = 173;
	  section.data(172).dtTransOffset = 361;
	
	  ;% rtB.g4um4nifj0
	  section.data(173).logicalSrcIdx = 174;
	  section.data(173).dtTransOffset = 362;
	
	  ;% rtB.bv31jmhnur
	  section.data(174).logicalSrcIdx = 175;
	  section.data(174).dtTransOffset = 363;
	
	  ;% rtB.fgeeodibr2
	  section.data(175).logicalSrcIdx = 176;
	  section.data(175).dtTransOffset = 366;
	
	  ;% rtB.lzor53a1zc
	  section.data(176).logicalSrcIdx = 177;
	  section.data(176).dtTransOffset = 367;
	
	  ;% rtB.a4mondaqym
	  section.data(177).logicalSrcIdx = 178;
	  section.data(177).dtTransOffset = 368;
	
	  ;% rtB.ikwzxkxyiz
	  section.data(178).logicalSrcIdx = 179;
	  section.data(178).dtTransOffset = 369;
	
	  ;% rtB.e3fyv2i0xt
	  section.data(179).logicalSrcIdx = 180;
	  section.data(179).dtTransOffset = 372;
	
	  ;% rtB.pv0jl14wdm
	  section.data(180).logicalSrcIdx = 181;
	  section.data(180).dtTransOffset = 373;
	
	  ;% rtB.gkxjm351v3
	  section.data(181).logicalSrcIdx = 182;
	  section.data(181).dtTransOffset = 374;
	
	  ;% rtB.gqlskp40nb
	  section.data(182).logicalSrcIdx = 183;
	  section.data(182).dtTransOffset = 375;
	
	  ;% rtB.cjgzre1rfd
	  section.data(183).logicalSrcIdx = 184;
	  section.data(183).dtTransOffset = 378;
	
	  ;% rtB.mmqgxd3ztq
	  section.data(184).logicalSrcIdx = 185;
	  section.data(184).dtTransOffset = 379;
	
	  ;% rtB.kfya4ws5wt
	  section.data(185).logicalSrcIdx = 186;
	  section.data(185).dtTransOffset = 380;
	
	  ;% rtB.czdxilbjs1
	  section.data(186).logicalSrcIdx = 187;
	  section.data(186).dtTransOffset = 381;
	
	  ;% rtB.kflvqeslk3
	  section.data(187).logicalSrcIdx = 188;
	  section.data(187).dtTransOffset = 384;
	
	  ;% rtB.pubftwd23q
	  section.data(188).logicalSrcIdx = 189;
	  section.data(188).dtTransOffset = 385;
	
	  ;% rtB.abudmgdmqo
	  section.data(189).logicalSrcIdx = 190;
	  section.data(189).dtTransOffset = 386;
	
	  ;% rtB.jnufx11sbb
	  section.data(190).logicalSrcIdx = 191;
	  section.data(190).dtTransOffset = 387;
	
	  ;% rtB.mnz0k5z1ev
	  section.data(191).logicalSrcIdx = 192;
	  section.data(191).dtTransOffset = 388;
	
	  ;% rtB.n3cickajoe
	  section.data(192).logicalSrcIdx = 193;
	  section.data(192).dtTransOffset = 389;
	
	  ;% rtB.cka3cbvjkk
	  section.data(193).logicalSrcIdx = 194;
	  section.data(193).dtTransOffset = 390;
	
	  ;% rtB.dvmo0qrznq
	  section.data(194).logicalSrcIdx = 195;
	  section.data(194).dtTransOffset = 391;
	
	  ;% rtB.i34c3sd5c4
	  section.data(195).logicalSrcIdx = 196;
	  section.data(195).dtTransOffset = 392;
	
	  ;% rtB.jo0ewhen5p
	  section.data(196).logicalSrcIdx = 197;
	  section.data(196).dtTransOffset = 393;
	
	  ;% rtB.janqpo5qe4
	  section.data(197).logicalSrcIdx = 198;
	  section.data(197).dtTransOffset = 394;
	
	  ;% rtB.dx3rmsixh3
	  section.data(198).logicalSrcIdx = 199;
	  section.data(198).dtTransOffset = 400;
	
	  ;% rtB.n0sluxojte
	  section.data(199).logicalSrcIdx = 200;
	  section.data(199).dtTransOffset = 436;
	
	  ;% rtB.lyhqkuq1c4
	  section.data(200).logicalSrcIdx = 201;
	  section.data(200).dtTransOffset = 472;
	
	  ;% rtB.ofyfrkcfxb
	  section.data(201).logicalSrcIdx = 202;
	  section.data(201).dtTransOffset = 473;
	
	  ;% rtB.khdzdbpnht
	  section.data(202).logicalSrcIdx = 203;
	  section.data(202).dtTransOffset = 509;
	
	  ;% rtB.dgwxdhwkdd
	  section.data(203).logicalSrcIdx = 204;
	  section.data(203).dtTransOffset = 545;
	
	  ;% rtB.caz1neybgm
	  section.data(204).logicalSrcIdx = 205;
	  section.data(204).dtTransOffset = 581;
	
	  ;% rtB.gdroql4j2f
	  section.data(205).logicalSrcIdx = 206;
	  section.data(205).dtTransOffset = 617;
	
	  ;% rtB.g0lz5qytlu
	  section.data(206).logicalSrcIdx = 207;
	  section.data(206).dtTransOffset = 653;
	
	  ;% rtB.cbygkknxsv
	  section.data(207).logicalSrcIdx = 208;
	  section.data(207).dtTransOffset = 689;
	
	  ;% rtB.ncbiuw0cin
	  section.data(208).logicalSrcIdx = 209;
	  section.data(208).dtTransOffset = 695;
	
	  ;% rtB.deid52kk1x
	  section.data(209).logicalSrcIdx = 210;
	  section.data(209).dtTransOffset = 731;
	
	  ;% rtB.c1v5fiqxar
	  section.data(210).logicalSrcIdx = 211;
	  section.data(210).dtTransOffset = 767;
	
	  ;% rtB.cfmayp3lnt
	  section.data(211).logicalSrcIdx = 212;
	  section.data(211).dtTransOffset = 773;
	
	  ;% rtB.b2rku1yvfe
	  section.data(212).logicalSrcIdx = 213;
	  section.data(212).dtTransOffset = 779;
	
	  ;% rtB.iqgj54dvfu
	  section.data(213).logicalSrcIdx = 214;
	  section.data(213).dtTransOffset = 782;
	
	  ;% rtB.kgqd5i3tmd
	  section.data(214).logicalSrcIdx = 215;
	  section.data(214).dtTransOffset = 783;
	
	  ;% rtB.pmvqbzeo1a
	  section.data(215).logicalSrcIdx = 216;
	  section.data(215).dtTransOffset = 784;
	
	  ;% rtB.lnslax3wky
	  section.data(216).logicalSrcIdx = 217;
	  section.data(216).dtTransOffset = 785;
	
	  ;% rtB.o5o0z0ts4i
	  section.data(217).logicalSrcIdx = 218;
	  section.data(217).dtTransOffset = 786;
	
	  ;% rtB.ke1trlx0xn
	  section.data(218).logicalSrcIdx = 219;
	  section.data(218).dtTransOffset = 787;
	
	  ;% rtB.pfjhtzhhke
	  section.data(219).logicalSrcIdx = 220;
	  section.data(219).dtTransOffset = 788;
	
	  ;% rtB.dsgaag2j1a
	  section.data(220).logicalSrcIdx = 221;
	  section.data(220).dtTransOffset = 789;
	
	  ;% rtB.ddpigcbdzw
	  section.data(221).logicalSrcIdx = 222;
	  section.data(221).dtTransOffset = 790;
	
	  ;% rtB.msc0q3ydkn
	  section.data(222).logicalSrcIdx = 223;
	  section.data(222).dtTransOffset = 791;
	
	  ;% rtB.bzlrt5abhc
	  section.data(223).logicalSrcIdx = 224;
	  section.data(223).dtTransOffset = 792;
	
	  ;% rtB.mc0c4fprgt
	  section.data(224).logicalSrcIdx = 225;
	  section.data(224).dtTransOffset = 793;
	
	  ;% rtB.g30midesdo
	  section.data(225).logicalSrcIdx = 226;
	  section.data(225).dtTransOffset = 829;
	
	  ;% rtB.k3cesiw1su
	  section.data(226).logicalSrcIdx = 227;
	  section.data(226).dtTransOffset = 865;
	
	  ;% rtB.mecr0anvzq
	  section.data(227).logicalSrcIdx = 228;
	  section.data(227).dtTransOffset = 901;
	
	  ;% rtB.g5yq3s1oi3
	  section.data(228).logicalSrcIdx = 229;
	  section.data(228).dtTransOffset = 937;
	
	  ;% rtB.gtavo0wfmk
	  section.data(229).logicalSrcIdx = 230;
	  section.data(229).dtTransOffset = 973;
	
	  ;% rtB.p4nck3maqs
	  section.data(230).logicalSrcIdx = 231;
	  section.data(230).dtTransOffset = 974;
	
	  ;% rtB.lv1l3120cn
	  section.data(231).logicalSrcIdx = 232;
	  section.data(231).dtTransOffset = 977;
	
	  ;% rtB.pnchbrbzh4
	  section.data(232).logicalSrcIdx = 233;
	  section.data(232).dtTransOffset = 978;
	
	  ;% rtB.h42emyzqm1
	  section.data(233).logicalSrcIdx = 234;
	  section.data(233).dtTransOffset = 979;
	
	  ;% rtB.mw4gdhqybz
	  section.data(234).logicalSrcIdx = 235;
	  section.data(234).dtTransOffset = 980;
	
	  ;% rtB.im1q1kmaf4
	  section.data(235).logicalSrcIdx = 236;
	  section.data(235).dtTransOffset = 981;
	
	  ;% rtB.a0tcaxg1ye
	  section.data(236).logicalSrcIdx = 237;
	  section.data(236).dtTransOffset = 982;
	
	  ;% rtB.babh1jhck2
	  section.data(237).logicalSrcIdx = 238;
	  section.data(237).dtTransOffset = 983;
	
	  ;% rtB.op4v4vz1hb
	  section.data(238).logicalSrcIdx = 239;
	  section.data(238).dtTransOffset = 984;
	
	  ;% rtB.elxflb4cn2
	  section.data(239).logicalSrcIdx = 240;
	  section.data(239).dtTransOffset = 985;
	
	  ;% rtB.enoczvl5ij
	  section.data(240).logicalSrcIdx = 241;
	  section.data(240).dtTransOffset = 986;
	
	  ;% rtB.l42zjb2acu
	  section.data(241).logicalSrcIdx = 242;
	  section.data(241).dtTransOffset = 987;
	
	  ;% rtB.cy4aoyd4p4
	  section.data(242).logicalSrcIdx = 243;
	  section.data(242).dtTransOffset = 988;
	
	  ;% rtB.iipz2h3t1h
	  section.data(243).logicalSrcIdx = 244;
	  section.data(243).dtTransOffset = 989;
	
	  ;% rtB.jd3k1qt01g
	  section.data(244).logicalSrcIdx = 245;
	  section.data(244).dtTransOffset = 990;
	
	  ;% rtB.p2blqb2x40
	  section.data(245).logicalSrcIdx = 246;
	  section.data(245).dtTransOffset = 991;
	
	  ;% rtB.jwcmbwsc34
	  section.data(246).logicalSrcIdx = 247;
	  section.data(246).dtTransOffset = 992;
	
	  ;% rtB.paxzwpdtyr
	  section.data(247).logicalSrcIdx = 248;
	  section.data(247).dtTransOffset = 993;
	
	  ;% rtB.mnmjtwzkhg
	  section.data(248).logicalSrcIdx = 249;
	  section.data(248).dtTransOffset = 994;
	
	  ;% rtB.hfxqkjhn3c
	  section.data(249).logicalSrcIdx = 250;
	  section.data(249).dtTransOffset = 995;
	
	  ;% rtB.gh0e24cnt4
	  section.data(250).logicalSrcIdx = 251;
	  section.data(250).dtTransOffset = 996;
	
	  ;% rtB.oq2fmk4tkh
	  section.data(251).logicalSrcIdx = 252;
	  section.data(251).dtTransOffset = 997;
	
	  ;% rtB.guw3ibew5f
	  section.data(252).logicalSrcIdx = 253;
	  section.data(252).dtTransOffset = 998;
	
	  ;% rtB.fwobh2ydkx
	  section.data(253).logicalSrcIdx = 254;
	  section.data(253).dtTransOffset = 999;
	
	  ;% rtB.kji2nsjrqr
	  section.data(254).logicalSrcIdx = 255;
	  section.data(254).dtTransOffset = 1000;
	
	  ;% rtB.frxxgkpfd0
	  section.data(255).logicalSrcIdx = 256;
	  section.data(255).dtTransOffset = 1001;
	
	  ;% rtB.if3uw30dno
	  section.data(256).logicalSrcIdx = 257;
	  section.data(256).dtTransOffset = 1002;
	
	  ;% rtB.i4qgt25dqa
	  section.data(257).logicalSrcIdx = 258;
	  section.data(257).dtTransOffset = 1003;
	
	  ;% rtB.ecp5cwxuux
	  section.data(258).logicalSrcIdx = 259;
	  section.data(258).dtTransOffset = 1004;
	
	  ;% rtB.fyt1uabhvi
	  section.data(259).logicalSrcIdx = 260;
	  section.data(259).dtTransOffset = 1005;
	
	  ;% rtB.b133gvafim
	  section.data(260).logicalSrcIdx = 261;
	  section.data(260).dtTransOffset = 1006;
	
	  ;% rtB.prtdictlyg
	  section.data(261).logicalSrcIdx = 262;
	  section.data(261).dtTransOffset = 1007;
	
	  ;% rtB.bzogwznvbx
	  section.data(262).logicalSrcIdx = 263;
	  section.data(262).dtTransOffset = 1008;
	
	  ;% rtB.n1w1gaeskg
	  section.data(263).logicalSrcIdx = 264;
	  section.data(263).dtTransOffset = 1009;
	
	  ;% rtB.euj2mhfzyp
	  section.data(264).logicalSrcIdx = 265;
	  section.data(264).dtTransOffset = 1010;
	
	  ;% rtB.jjuwkhbaqb
	  section.data(265).logicalSrcIdx = 266;
	  section.data(265).dtTransOffset = 1011;
	
	  ;% rtB.ks1ri00jvc
	  section.data(266).logicalSrcIdx = 267;
	  section.data(266).dtTransOffset = 1012;
	
	  ;% rtB.orqdb4jq0f
	  section.data(267).logicalSrcIdx = 268;
	  section.data(267).dtTransOffset = 1013;
	
	  ;% rtB.l25oveef2i
	  section.data(268).logicalSrcIdx = 269;
	  section.data(268).dtTransOffset = 1014;
	
      nTotData = nTotData + section.nData;
      sigMap.sections(2) = section;
      clear section
      
      section.nData     = 11;
      section.data(11)  = dumData; %prealloc
      
	  ;% rtB.lvq5fe4dss
	  section.data(1).logicalSrcIdx = 270;
	  section.data(1).dtTransOffset = 0;
	
	  ;% rtB.hswklfu3tm
	  section.data(2).logicalSrcIdx = 271;
	  section.data(2).dtTransOffset = 1;
	
	  ;% rtB.nhjhuszeqq
	  section.data(3).logicalSrcIdx = 272;
	  section.data(3).dtTransOffset = 2;
	
	  ;% rtB.jgzslezpvv
	  section.data(4).logicalSrcIdx = 273;
	  section.data(4).dtTransOffset = 3;
	
	  ;% rtB.jsc1e3jlwj
	  section.data(5).logicalSrcIdx = 274;
	  section.data(5).dtTransOffset = 4;
	
	  ;% rtB.kkthzbzxh4
	  section.data(6).logicalSrcIdx = 275;
	  section.data(6).dtTransOffset = 5;
	
	  ;% rtB.f05xiwzeii
	  section.data(7).logicalSrcIdx = 276;
	  section.data(7).dtTransOffset = 6;
	
	  ;% rtB.hq4dyb3apy
	  section.data(8).logicalSrcIdx = 277;
	  section.data(8).dtTransOffset = 7;
	
	  ;% rtB.ot21w1gcro
	  section.data(9).logicalSrcIdx = 278;
	  section.data(9).dtTransOffset = 8;
	
	  ;% rtB.dv21ecppse
	  section.data(10).logicalSrcIdx = 279;
	  section.data(10).dtTransOffset = 9;
	
	  ;% rtB.kgpjy2123a
	  section.data(11).logicalSrcIdx = 280;
	  section.data(11).dtTransOffset = 10;
	
      nTotData = nTotData + section.nData;
      sigMap.sections(3) = section;
      clear section
      
    
      ;%
      ;% Non-auto Data (signal)
      ;%
    

    ;%
    ;% Add final counts to struct.
    ;%
    sigMap.nTotData = nTotData;
    


  ;%*******************
  ;% Create DWork Map *
  ;%*******************
      
    nTotData      = 0; %add to this count as we go
    nTotSects     = 7;
    sectIdxOffset = 3;
    
    ;%
    ;% Define dummy sections & preallocate arrays
    ;%
    dumSection.nData = -1;  
    dumSection.data  = [];
    
    dumData.logicalSrcIdx = -1;
    dumData.dtTransOffset = -1;
    
    ;%
    ;% Init/prealloc dworkMap
    ;%
    dworkMap.nSections           = nTotSects;
    dworkMap.sectIdxOffset       = sectIdxOffset;
      dworkMap.sections(nTotSects) = dumSection; %prealloc
    dworkMap.nTotData            = -1;
    
    ;%
    ;% Auto data (rtDW)
    ;%
      section.nData     = 29;
      section.data(29)  = dumData; %prealloc
      
	  ;% rtDW.hsrejdggk0
	  section.data(1).logicalSrcIdx = 0;
	  section.data(1).dtTransOffset = 0;
	
	  ;% rtDW.g2cesgd0fj
	  section.data(2).logicalSrcIdx = 1;
	  section.data(2).dtTransOffset = 1;
	
	  ;% rtDW.pwalsrbpvg
	  section.data(3).logicalSrcIdx = 2;
	  section.data(3).dtTransOffset = 7;
	
	  ;% rtDW.ltyzpyes2j
	  section.data(4).logicalSrcIdx = 3;
	  section.data(4).dtTransOffset = 13;
	
	  ;% rtDW.koephaho24
	  section.data(5).logicalSrcIdx = 4;
	  section.data(5).dtTransOffset = 14;
	
	  ;% rtDW.bjkyi30h4y
	  section.data(6).logicalSrcIdx = 5;
	  section.data(6).dtTransOffset = 15;
	
	  ;% rtDW.cszisyhd2i
	  section.data(7).logicalSrcIdx = 6;
	  section.data(7).dtTransOffset = 21;
	
	  ;% rtDW.ksbtbqie2m
	  section.data(8).logicalSrcIdx = 7;
	  section.data(8).dtTransOffset = 22;
	
	  ;% rtDW.ezm5irxmvk
	  section.data(9).logicalSrcIdx = 8;
	  section.data(9).dtTransOffset = 23;
	
	  ;% rtDW.m2oxahajnn
	  section.data(10).logicalSrcIdx = 9;
	  section.data(10).dtTransOffset = 24;
	
	  ;% rtDW.jzuea21ukz
	  section.data(11).logicalSrcIdx = 10;
	  section.data(11).dtTransOffset = 25;
	
	  ;% rtDW.crh5ypw2db
	  section.data(12).logicalSrcIdx = 11;
	  section.data(12).dtTransOffset = 26;
	
	  ;% rtDW.ia2zs2ii2b
	  section.data(13).logicalSrcIdx = 12;
	  section.data(13).dtTransOffset = 27;
	
	  ;% rtDW.lormmfrasx
	  section.data(14).logicalSrcIdx = 13;
	  section.data(14).dtTransOffset = 28;
	
	  ;% rtDW.ocwfh3bc3h
	  section.data(15).logicalSrcIdx = 14;
	  section.data(15).dtTransOffset = 29;
	
	  ;% rtDW.dw042k0azu
	  section.data(16).logicalSrcIdx = 15;
	  section.data(16).dtTransOffset = 30;
	
	  ;% rtDW.jfihwsipvc
	  section.data(17).logicalSrcIdx = 16;
	  section.data(17).dtTransOffset = 31;
	
	  ;% rtDW.k2r45klkvr
	  section.data(18).logicalSrcIdx = 17;
	  section.data(18).dtTransOffset = 32;
	
	  ;% rtDW.lawcljffga
	  section.data(19).logicalSrcIdx = 18;
	  section.data(19).dtTransOffset = 33;
	
	  ;% rtDW.bhgocl4yyi
	  section.data(20).logicalSrcIdx = 19;
	  section.data(20).dtTransOffset = 34;
	
	  ;% rtDW.ayphut5w0e
	  section.data(21).logicalSrcIdx = 20;
	  section.data(21).dtTransOffset = 35;
	
	  ;% rtDW.byyiabbsdb
	  section.data(22).logicalSrcIdx = 21;
	  section.data(22).dtTransOffset = 36;
	
	  ;% rtDW.khwhm1vhhp
	  section.data(23).logicalSrcIdx = 22;
	  section.data(23).dtTransOffset = 72;
	
	  ;% rtDW.i33eulcryu
	  section.data(24).logicalSrcIdx = 23;
	  section.data(24).dtTransOffset = 108;
	
	  ;% rtDW.a2rewpflwu
	  section.data(25).logicalSrcIdx = 24;
	  section.data(25).dtTransOffset = 144;
	
	  ;% rtDW.o31z3owz5o.modelTStart
	  section.data(26).logicalSrcIdx = 25;
	  section.data(26).dtTransOffset = 180;
	
	  ;% rtDW.kcazm4yk20.modelTStart
	  section.data(27).logicalSrcIdx = 26;
	  section.data(27).dtTransOffset = 181;
	
	  ;% rtDW.pznl15zy5j.modelTStart
	  section.data(28).logicalSrcIdx = 27;
	  section.data(28).dtTransOffset = 182;
	
	  ;% rtDW.az4xbowotu.modelTStart
	  section.data(29).logicalSrcIdx = 28;
	  section.data(29).dtTransOffset = 183;
	
      nTotData = nTotData + section.nData;
      dworkMap.sections(1) = section;
      clear section
      
      section.nData     = 9;
      section.data(9)  = dumData; %prealloc
      
	  ;% rtDW.oh3rj3rgch.AS
	  section.data(1).logicalSrcIdx = 29;
	  section.data(1).dtTransOffset = 0;
	
	  ;% rtDW.b1o5sd1mu2.TUbufferPtrs
	  section.data(2).logicalSrcIdx = 30;
	  section.data(2).dtTransOffset = 1;
	
	  ;% rtDW.cvivv0qvki.TUbufferPtrs
	  section.data(3).logicalSrcIdx = 31;
	  section.data(3).dtTransOffset = 3;
	
	  ;% rtDW.ecv4fwadro.TUbufferPtrs
	  section.data(4).logicalSrcIdx = 32;
	  section.data(4).dtTransOffset = 5;
	
	  ;% rtDW.ohddfojdjk.TUbufferPtrs
	  section.data(5).logicalSrcIdx = 33;
	  section.data(5).dtTransOffset = 7;
	
	  ;% rtDW.m0scdgvmro.LoggedData
	  section.data(6).logicalSrcIdx = 34;
	  section.data(6).dtTransOffset = 9;
	
	  ;% rtDW.exi0svnnxz.LoggedData
	  section.data(7).logicalSrcIdx = 35;
	  section.data(7).dtTransOffset = 11;
	
	  ;% rtDW.apk42thrxw.LoggedData
	  section.data(8).logicalSrcIdx = 36;
	  section.data(8).dtTransOffset = 13;
	
	  ;% rtDW.m44vudu504.LoggedData
	  section.data(9).logicalSrcIdx = 37;
	  section.data(9).dtTransOffset = 15;
	
      nTotData = nTotData + section.nData;
      dworkMap.sections(2) = section;
      clear section
      
      section.nData     = 4;
      section.data(4)  = dumData; %prealloc
      
	  ;% rtDW.mljcxhdcts
	  section.data(1).logicalSrcIdx = 38;
	  section.data(1).dtTransOffset = 0;
	
	  ;% rtDW.euaoyfm334
	  section.data(2).logicalSrcIdx = 39;
	  section.data(2).dtTransOffset = 1;
	
	  ;% rtDW.irr1ngkglw
	  section.data(3).logicalSrcIdx = 40;
	  section.data(3).dtTransOffset = 2;
	
	  ;% rtDW.fa0orlv0f5
	  section.data(4).logicalSrcIdx = 41;
	  section.data(4).dtTransOffset = 3;
	
      nTotData = nTotData + section.nData;
      dworkMap.sections(3) = section;
      clear section
      
      section.nData     = 2;
      section.data(2)  = dumData; %prealloc
      
	  ;% rtDW.lrbh114wtw
	  section.data(1).logicalSrcIdx = 42;
	  section.data(1).dtTransOffset = 0;
	
	  ;% rtDW.pftimexvc5
	  section.data(2).logicalSrcIdx = 43;
	  section.data(2).dtTransOffset = 1;
	
      nTotData = nTotData + section.nData;
      dworkMap.sections(4) = section;
      clear section
      
      section.nData     = 5;
      section.data(5)  = dumData; %prealloc
      
	  ;% rtDW.lkcbxptdv5
	  section.data(1).logicalSrcIdx = 44;
	  section.data(1).dtTransOffset = 0;
	
	  ;% rtDW.drr2cwynj5.Tail
	  section.data(2).logicalSrcIdx = 45;
	  section.data(2).dtTransOffset = 11;
	
	  ;% rtDW.fymd3ellif.Tail
	  section.data(3).logicalSrcIdx = 46;
	  section.data(3).dtTransOffset = 12;
	
	  ;% rtDW.eyb4fhkbdy.Tail
	  section.data(4).logicalSrcIdx = 47;
	  section.data(4).dtTransOffset = 13;
	
	  ;% rtDW.ek4mryl130.Tail
	  section.data(5).logicalSrcIdx = 48;
	  section.data(5).dtTransOffset = 14;
	
      nTotData = nTotData + section.nData;
      dworkMap.sections(5) = section;
      clear section
      
      section.nData     = 3;
      section.data(3)  = dumData; %prealloc
      
	  ;% rtDW.m11yduvrgb
	  section.data(1).logicalSrcIdx = 49;
	  section.data(1).dtTransOffset = 0;
	
	  ;% rtDW.eocn40ibhc
	  section.data(2).logicalSrcIdx = 50;
	  section.data(2).dtTransOffset = 1;
	
	  ;% rtDW.jld3u4fkos
	  section.data(3).logicalSrcIdx = 51;
	  section.data(3).dtTransOffset = 2;
	
      nTotData = nTotData + section.nData;
      dworkMap.sections(6) = section;
      clear section
      
      section.nData     = 1;
      section.data(1)  = dumData; %prealloc
      
	  ;% rtDW.lkdnaytcjn
	  section.data(1).logicalSrcIdx = 52;
	  section.data(1).dtTransOffset = 0;
	
      nTotData = nTotData + section.nData;
      dworkMap.sections(7) = section;
      clear section
      
    
      ;%
      ;% Non-auto Data (dwork)
      ;%
    

    ;%
    ;% Add final counts to struct.
    ;%
    dworkMap.nTotData = nTotData;
    


  ;%
  ;% Add individual maps to base struct.
  ;%

  targMap.paramMap  = paramMap;    
  targMap.signalMap = sigMap;
  targMap.dworkMap  = dworkMap;
  
  ;%
  ;% Add checksums to base struct.
  ;%


  targMap.checksum0 = 749349454;
  targMap.checksum1 = 2771954547;
  targMap.checksum2 = 2147498699;
  targMap.checksum3 = 4272243878;

