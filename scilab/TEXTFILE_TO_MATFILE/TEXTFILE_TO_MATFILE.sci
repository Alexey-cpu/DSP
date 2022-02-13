function [%file] = TEXTFILE_TO_MATFILE(filename,directory)

// Output variables initialisation (not found in input variables)
%file=[];

// Display mode
mode(0);

// Display warning for floating point exception
ieee(1);

//Работа с dat - файлом:
//Читаем dat - файл:

// !! L.5: Matlab function strcat not yet converted, original calling sequence used.
// L.5: (Warning name conflict: function name changed from strcat to %strcat).
file_name = %strcat(directory,"\",filename);
fileID = mtlb_fopen(file_name);
// L.7: No simple equivalent, so mtlb_fread() is called.
file = mtlb_fread(fileID,"*char")';
file = evstr(%file);
mclose(fileID);
endfunction
