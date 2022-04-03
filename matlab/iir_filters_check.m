logs = "C:\Qt_projects\DigitalFilters_x32\logs";
yt      = TEXTFILE_TO_MATFILE ( "yt.txt"        , logs );
cheb1_y = TEXTFILE_TO_MATFILE ( "cheb1_y.txt"   , logs );
cheb2_y = TEXTFILE_TO_MATFILE ( "cheb2_y.txt"   , logs );
buttf_y = TEXTFILE_TO_MATFILE ( "buttf_y.txt"   , logs );
ellip_y = TEXTFILE_TO_MATFILE ( "ellip_y.txt"   , logs );
cheb1_km = TEXTFILE_TO_MATFILE( "cheb1_km.txt"  , logs );
cheb2_km = TEXTFILE_TO_MATFILE( "cheb2_km.txt"  , logs );
buttf_km = TEXTFILE_TO_MATFILE( "buttf_km.txt"  , logs );
ellip_km = TEXTFILE_TO_MATFILE( "ellip_km.txt"  , logs );
cheb1_ph = TEXTFILE_TO_MATFILE( "cheb1_ph.txt"  , logs );
cheb2_ph = TEXTFILE_TO_MATFILE( "cheb2_ph.txt"  , logs );
buttf_ph = TEXTFILE_TO_MATFILE( "buttf_ph.txt"  , logs );
ellip_ph = TEXTFILE_TO_MATFILE( "ellip_ph.txt"  , logs );
ff       = TEXTFILE_TO_MATFILE ( "ff.txt"       , logs );
tt       = TEXTFILE_TO_MATFILE ( "tt.txt"       , logs );

% group plotting:
%plot( ff , cheb1_km , ff , cheb2_km , ff , buttf_km , ff , ellip_km );
plot( tt , cheb1_y , tt , cheb2_y , tt , buttf_y , tt , ellip_y );

%plot( tt , cheb1_y , tt , buttf_y );

%plot( ff , buttf_km )

%plotting:
%plot( tt , cheb1_y )
%plot( tt , cheb2_y )
%plot( tt , buttf_y )
%plot( tt , ellip_y )