logs = "C:\Qt_projects\DigitalFilters_x32\logs";
yt    = TEXTFILE_TO_MATFILE( "yt.txt"    , logs );
lp_yt = TEXTFILE_TO_MATFILE( "lp_yt.txt" , logs );
hp_yt = TEXTFILE_TO_MATFILE( "hp_yt.txt" , logs );
bp_yt = TEXTFILE_TO_MATFILE( "bp_yt.txt" , logs );
bs_yt = TEXTFILE_TO_MATFILE( "bs_yt.txt" , logs );
lp_km = TEXTFILE_TO_MATFILE( "lp_km.txt" , logs );
hp_km = TEXTFILE_TO_MATFILE( "hp_km.txt" , logs );
bp_km = TEXTFILE_TO_MATFILE( "bp_km.txt" , logs );
bs_km = TEXTFILE_TO_MATFILE( "bs_km.txt" , logs );
lp_ph = TEXTFILE_TO_MATFILE( "lp_ph.txt" , logs );
hp_ph = TEXTFILE_TO_MATFILE( "hp_ph.txt" , logs );
bp_ph = TEXTFILE_TO_MATFILE( "bp_ph.txt" , logs );
bs_ph = TEXTFILE_TO_MATFILE( "bs_ph.txt" , logs );
ff    = TEXTFILE_TO_MATFILE( "ff.txt"    , logs );
tt    = TEXTFILE_TO_MATFILE( "tt.txt"    , logs );

%group plotting:
plot( ff , lp_km , ff , hp_km , ff , bp_km , ff , bs_km );
%plot( tt , lp_yt , tt , hp_yt , tt , bp_yt , tt , bs_yt );

%individual plotting:
%plot(ff , lp_km);
%plot(ff , hp_km);
%plot(ff , bp_km);
%plot(ff , bs_km);

%plot( lp_yt);
%plot(tt , hp_yt);
%plot(tt , bp_yt);
%plot(tt , bs_yt);