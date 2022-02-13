@echo off
set MATLAB=C:\Program Files\Polyspace\R2020a
"%MATLAB%\bin\win64\gmake" -f DZG_v3.mk  OPTS="-DTID01EQ=1"
