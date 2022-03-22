logs      = "C:\Qt_projects\DigitalFilters_x32\logs";
yt        = TEXTFILE_TO_MATFILE( "yt.txt"        , logs );
comb_yt   = TEXTFILE_TO_MATFILE( "comb_yt.txt"   , logs );
combeq_yt = TEXTFILE_TO_MATFILE( "combeq_yt.txt" , logs );
diff_yt   = TEXTFILE_TO_MATFILE( "diff_yt.txt"   , logs );
intg_yt   = TEXTFILE_TO_MATFILE( "intg_yt.txt"   , logs );
lead_yt   = TEXTFILE_TO_MATFILE( "lead_yt.txt"   , logs );
flt2_yt   = TEXTFILE_TO_MATFILE( "flt2_yt.txt"   , logs );
comb_km   = TEXTFILE_TO_MATFILE( "comb_km.txt"   , logs );
combeq_km = TEXTFILE_TO_MATFILE( "combeq_km.txt" , logs );
diff_km   = TEXTFILE_TO_MATFILE( "diff_km.txt"   , logs );
intg_km   = TEXTFILE_TO_MATFILE( "intg_km.txt"   , logs );
lead_km   = TEXTFILE_TO_MATFILE( "lead_km.txt"   , logs );
flt2_km   = TEXTFILE_TO_MATFILE( "flt2_km.txt"   , logs );
comb_ph   = TEXTFILE_TO_MATFILE( "comb_km.txt"   , logs );
combeq_ph = TEXTFILE_TO_MATFILE( "combeq_km.txt" , logs );
diff_ph   = TEXTFILE_TO_MATFILE( "diff_km.txt"   , logs );
intg_ph   = TEXTFILE_TO_MATFILE( "intg_km.txt"   , logs );
lead_ph   = TEXTFILE_TO_MATFILE( "lead_km.txt"   , logs );
flt2_ph   = TEXTFILE_TO_MATFILE( "flt2_km.txt"   , logs );
tt        = TEXTFILE_TO_MATFILE( "tt.txt"        , logs );
ff        = TEXTFILE_TO_MATFILE( "ff.txt"        , logs );

% individual plotting:
%plot( tt , comb_yt );
%plot( tt , combeq_yt );
%plot( tt , diff_yt   );
%plot( tt , intg_yt   );
%plot( tt , lead_yt   );
%plot( tt , flt2_yt   );

%plot( ff , comb_km   );
%plot( ff , combeq_km );
%plot( ff , diff_km   );
%plot( ff , intg_km   );
%plot( ff , lead_km   );
%plot( ff , flt2_km   );

