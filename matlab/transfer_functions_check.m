logs = "C:\Qt_projects\DigitalFilters_x32\logs";
yt     = TEXTFILE_TO_MATFILE( "yt.txt"    , logs );
dif_y  = TEXTFILE_TO_MATFILE( "dif_y.txt" , logs );
app_y  = TEXTFILE_TO_MATFILE( "app_y.txt" , logs );
leg_y  = TEXTFILE_TO_MATFILE( "leg_y.txt" , logs );
int_y  = TEXTFILE_TO_MATFILE( "int_y.txt" , logs );
lpf_y  = TEXTFILE_TO_MATFILE( "lpf_y.txt" , logs );
hpf_y  = TEXTFILE_TO_MATFILE( "hpf_y.txt" , logs );
bpf_y  = TEXTFILE_TO_MATFILE( "bpf_y.txt" , logs );
bsf1_y = TEXTFILE_TO_MATFILE( "bsf1_y.txt" , logs );
bsf2_y = TEXTFILE_TO_MATFILE( "bsf2_y.txt" , logs );
tt     = TEXTFILE_TO_MATFILE( "tt.txt"    , logs );

%plotting:
%plot(  tt , lpf_y );
%plot( tt , simout( 1 : length(dif_y) ) , tt , dif_y );
%plot( tt , simout( 1 : length(app_y) ) , tt , app_y );
%plot( tt , simout( 1 : length(leg_y) ) , tt , leg_y );
%plot( tt , simout( 1 : length(int_y) ) , tt , int_y );
%plot( tt , simout( 1 : length(lpf_y) ) , tt , lpf_y );
%plot( tt , simout( 1 : length(hpf_y) ) , tt , hpf_y );
%plot( tt , simout( 1 : length(bpf_y) ) , tt , bpf_y );
%plot( tt , simout( 1 : length(bsf1_y) ) , tt , bsf1_y );
plot( tt , simout( 1 : length(bsf2_y) ) , tt , bsf2_y );