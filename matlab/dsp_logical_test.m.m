logs = "C:\Qt_projects\DigitalFilters_x32\logs";
yt    = TEXTFILE_TO_MATFILE( "yt.txt", logs );
ton_y = TEXTFILE_TO_MATFILE( "ton_y.txt", logs );
tof_y = TEXTFILE_TO_MATFILE( "tof_y.txt", logs );
tp_y  = TEXTFILE_TO_MATFILE( "tp_y.txt" , logs );
rs_y  = TEXTFILE_TO_MATFILE( "rs_y.txt" , logs );
sr_y  = TEXTFILE_TO_MATFILE( "sr_y.txt" , logs );
rr_y  = TEXTFILE_TO_MATFILE( "rr_y.txt" , logs );
ff_y  = TEXTFILE_TO_MATFILE( "ff_y.txt" , logs );
tt    = TEXTFILE_TO_MATFILE( "tt.txt"   , logs );

%plotting:
%plot( tt , yt , tt , ton_y  );
%plot( tt , yt , tt , tof_y );
%plot( tt , yt , tt , tp_y  );
%plot( tt , yt , tt , rs_y  );
%plot( tt , yt , tt , sr_y  );
%plot( tt , rr_y  );
plot( tt , ff_y  );
