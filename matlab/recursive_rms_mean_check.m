logs = "C:\Qt_projects\DigitalFilters_x32\logs";
yt   = TEXTFILE_TO_MATFILE( "yt.txt"  , logs );
rms  = TEXTFILE_TO_MATFILE( "rms.txt"  , logs );
tt   = TEXTFILE_TO_MATFILE( "tt.txt"  , logs );

%plotting:
plot( tt , yt , tt , rms );
%plot( tt , phs );
%plot( tt , df  );
%plot( tt , ff );