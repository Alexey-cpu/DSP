function file = TEXTFILE_TO_MATFILE(filename , directory)
%������ � dat - ������:
%������ dat - ����:

file_name      = strcat(directory , '\', filename );
fileID         = fopen( file_name );
file           = fread(fileID,'*char')';
file           = str2num(file);
fclose(fileID);


end