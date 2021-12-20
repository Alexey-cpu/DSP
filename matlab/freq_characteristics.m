clc; clear;
directory = "C:\Qt_projects\DigitalFilters_x32\logs";
filename1 = "amp_resp.txt";
filename2 = "ph_resp.txt";
filename3 = "f.txt";
filename4 = "x_t.txt";
filename5 = "y_t.txt";
filename6 = "coeffs.txt";

freq_amp_resp = TEXTFILE_TO_MATFILE(filename1 , directory);
freq_ph_resp  = TEXTFILE_TO_MATFILE(filename2 , directory);
freq          = TEXTFILE_TO_MATFILE(filename3 , directory);
input_signal  = TEXTFILE_TO_MATFILE(filename4 , directory);
output_signal = TEXTFILE_TO_MATFILE(filename5 , directory);
filt_coeffs   = TEXTFILE_TO_MATFILE(filename6 , directory);

clearvars -except freq_amp_resp freq_ph_resp freq input_signal output_signal filt_coeffs;

FFT = fft( filt_coeffs );
ABS_FFT = abs( FFT );
