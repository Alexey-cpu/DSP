logs = "C:\Qt_projects\DigitalFilters_x32\logs";
Km   = TEXTFILE_TO_MATFILE( "Km.txt" , logs );
pH   = TEXTFILE_TO_MATFILE( "pH.txt" , logs );
FF   = TEXTFILE_TO_MATFILE( "FF.txt" , logs );
yt   = TEXTFILE_TO_MATFILE( "yt.txt" , logs );
ft   = TEXTFILE_TO_MATFILE( "ft.txt" , logs );
tt   = TEXTFILE_TO_MATFILE( "tt.txt" , logs );

plot(  tt , ft )
%plot(  tt , ft , tt , simout( 1 : length( simout )-1 ) );
%plot(  FF , Km );