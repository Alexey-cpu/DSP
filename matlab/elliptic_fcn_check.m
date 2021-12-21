clc; clear;

logs = "C:\Qt_projects\DigitalFilters_x32\logs";

% main elliptic functions computation:
M = 0.7;
U = -5:0.01:5;
tic
[ SN , CN , DN ] = ellipj( U , M );
toc

% depended elliptic functions computation:
CD = CN ./ DN;
SD = SN ./ DN;
ND = 1  ./ DN;
DC = DN ./ CN;
NC = 1  ./ CN;
SC = SN ./ CN;
NS = 1  ./ SN;
DS = DN ./ SN;
CS = CN ./ SN;

% uploading C++ function answer:
SN_CPP = TEXTFILE_TO_MATFILE( "sn.txt" , logs );
CN_CPP = TEXTFILE_TO_MATFILE( "cn.txt" , logs );
DN_CPP = TEXTFILE_TO_MATFILE( "dn.txt" , logs );
CD_CPP = TEXTFILE_TO_MATFILE( "cd.txt" , logs );
SD_CPP = TEXTFILE_TO_MATFILE( "sd.txt" , logs );
ND_CPP = TEXTFILE_TO_MATFILE( "nd.txt" , logs );
DC_CPP = TEXTFILE_TO_MATFILE( "dc.txt" , logs );
NC_CPP = TEXTFILE_TO_MATFILE( "nc.txt" , logs );
SC_CPP = TEXTFILE_TO_MATFILE( "sc.txt" , logs );
NS_CPP = TEXTFILE_TO_MATFILE( "ns.txt" , logs );
DS_CPP = TEXTFILE_TO_MATFILE( "ds.txt" , logs );
CS_CPP = TEXTFILE_TO_MATFILE( "cs.txt" , logs );
UU_CPP = TEXTFILE_TO_MATFILE( "uu.txt" , logs );

%plotting:
%plot( U , SN , transpose(UU_CPP) , transpose( SN_CPP ) )
%plot( U , CN , transpose(UU_CPP) , transpose( CN_CPP ) )
%plot( U , DN , transpose(UU_CPP) , transpose( DN_CPP ) )
%plot( U , CD , transpose(UU_CPP) , transpose( CD_CPP ) )
%plot( U , SD , transpose(UU_CPP) , transpose( SD_CPP ) )
%plot( U , ND , transpose(UU_CPP) , transpose( ND_CPP ) )
%plot( U , DC , transpose(UU_CPP) , transpose( DC_CPP ) )
%plot( U , NC , transpose(UU_CPP) , transpose( NC_CPP ) )
%plot( U , SC , transpose(UU_CPP) , transpose( SC_CPP ) )
%plot( U , NS , transpose(UU_CPP) , transpose( NS_CPP ) )
%plot( U , DS , transpose(UU_CPP) , transpose( DS_CPP ) )
%plot( U , CS , transpose(UU_CPP) , transpose( CS_CPP ) )