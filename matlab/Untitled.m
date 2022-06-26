directory = "C:\Qt_projects\DigitalFilters_x32\logs";
xt = TEXTFILE_TO_MATFILE("xt.txt" , directory);
yt = TEXTFILE_TO_MATFILE("yt.txt" , directory);
dt = TEXTFILE_TO_MATFILE("dt.txt" , directory);
Km = TEXTFILE_TO_MATFILE("Km.txt" , directory);
pH = TEXTFILE_TO_MATFILE("pH.txt" , directory);

% plotting
plot(Km)
%plot(dt, xt, dt, yt)