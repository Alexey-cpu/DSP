directory = "C:\Qt_projects\DigitalFilters_x32\logs";
yt = TEXTFILE_TO_MATFILE("yt.txt" , directory);
re = TEXTFILE_TO_MATFILE("re.txt" , directory);
im = TEXTFILE_TO_MATFILE("im.txt" , directory);
am = TEXTFILE_TO_MATFILE("am.txt" , directory);
dt = TEXTFILE_TO_MATFILE("dt.txt" , directory);
Km = TEXTFILE_TO_MATFILE("Km.txt" , directory);
pH = TEXTFILE_TO_MATFILE("pH.txt" , directory);

%plot(dt, re, dt, im, dt, am)

%plot(dt, re)
plot(Km)