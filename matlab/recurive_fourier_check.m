logs = "C:\Qt_projects\DigitalFilters_x32\logs";
yt   = TEXTFILE_TO_MATFILE( "yt.txt"  , logs );
re   = TEXTFILE_TO_MATFILE( "re.txt"  , logs );
im   = TEXTFILE_TO_MATFILE( "im.txt"  , logs );
abs  = TEXTFILE_TO_MATFILE( "abs.txt" , logs );
phs  = TEXTFILE_TO_MATFILE( "phs.txt" , logs );
tt   = TEXTFILE_TO_MATFILE( "tt.txt"  , logs );

%plotting:
plot( tt , yt , tt , abs * sqrt(2) );
%plot( tt , phs );
%plot( tt , df  );
%plot( tt , ff );