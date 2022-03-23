clearvars -except simout1 simout2 simout3 simout4 simout5 simout6 simout7 simout8
logs      = "C:\Qt_projects\DigitalFilters_x32\logs";
yt        = TEXTFILE_TO_MATFILE( "yt.txt"        , logs );
comb_yt   = TEXTFILE_TO_MATFILE( "comb_yt.txt"   , logs );
combeq_yt = TEXTFILE_TO_MATFILE( "combeq_yt.txt" , logs );
diff_yt   = TEXTFILE_TO_MATFILE( "diff_yt.txt"   , logs );
intg_yt   = TEXTFILE_TO_MATFILE( "intg_yt.txt"   , logs );
lead_yt   = TEXTFILE_TO_MATFILE( "lead_yt.txt"   , logs );
lp2_yt   = TEXTFILE_TO_MATFILE( "lp2_yt.txt"   , logs );
hp2_yt   = TEXTFILE_TO_MATFILE( "hp2_yt.txt"   , logs );
bp2_yt   = TEXTFILE_TO_MATFILE( "bp2_yt.txt"   , logs );
bs2_yt   = TEXTFILE_TO_MATFILE( "bs2_yt.txt"   , logs );

aper_yt   = TEXTFILE_TO_MATFILE( "aper_yt.txt"   , logs );
comb_km   = TEXTFILE_TO_MATFILE( "comb_km.txt"   , logs );
combeq_km = TEXTFILE_TO_MATFILE( "combeq_km.txt" , logs );
diff_km   = TEXTFILE_TO_MATFILE( "diff_km.txt"   , logs );
intg_km   = TEXTFILE_TO_MATFILE( "intg_km.txt"   , logs );
lead_km   = TEXTFILE_TO_MATFILE( "lead_km.txt"   , logs );
flt2_km   = TEXTFILE_TO_MATFILE( "flt2_km.txt"   , logs );
aper_km   = TEXTFILE_TO_MATFILE( "aper_km.txt"   , logs );
comb_ph   = TEXTFILE_TO_MATFILE( "comb_km.txt"   , logs );
combeq_ph = TEXTFILE_TO_MATFILE( "combeq_km.txt" , logs );
diff_ph   = TEXTFILE_TO_MATFILE( "diff_km.txt"   , logs );
intg_ph   = TEXTFILE_TO_MATFILE( "intg_km.txt"   , logs );
lead_ph   = TEXTFILE_TO_MATFILE( "lead_km.txt"   , logs );
flt2_ph   = TEXTFILE_TO_MATFILE( "flt2_km.txt"   , logs );
aper_ph   = TEXTFILE_TO_MATFILE( "aper_ph.txt"   , logs );
tt        = TEXTFILE_TO_MATFILE( "tt.txt"        , logs );
ff        = TEXTFILE_TO_MATFILE( "ff.txt"        , logs );

% individual plotting:
%plot( tt , comb_yt );
%plot( tt , combeq_yt );
%plot( tt , diff_yt, tt, simout1(1:length(simout1)-1) );
%plot( tt , aper_yt, tt, simout2(1:length(simout2)-1) );
%plot( tt , lead_yt,  tt, simout3(1:length(simout3)-1) );
%plot( tt , intg_yt,  tt, simout4(1:length(simout4)-1) );
%plot( tt , lp2_yt,  tt, simout5(1:length(simout5)-1) );
%plot( tt , hp2_yt,  tt, simout6(1:length(simout6)-1) );
%plot( tt , bp2_yt,  tt, simout7(1:length(simout7)-1) );
plot( tt , bs2_yt,  tt, simout8(1:length(simout8)-1) );

%plot( ff , comb_km   );
%plot( ff , combeq_km );
%plot( ff , diff_km   );
%plot( ff , intg_km   );
%plot( ff , lead_km   );
%plot( ff , flt2_km   );

