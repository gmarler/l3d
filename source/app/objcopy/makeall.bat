rem TCC DOS ****************************************************
rem set up path so that "make" is TCC's make, as well as drive T:
set oldpath=%path%
call settcdos.bat

make -f makeflot.tcc
make -f makefix.tcc

rem restore path
set path=%oldpath%

rem TCC WIN ****************************************************
rem set up path and drive T:
set oldpath=%path%
call settcwin.bat

call maktcw_f.bat
pause
call maktcw_x.bat
pause

rem restore path
set path=%oldpath%

rem DJGPP DOS ****************************************************
rem set up path so that "make" is DJGPP's make
set oldpath=%path%
call setdjgpp.bat
make -f makeflot.djg
make -f makefix.djg

rem restore path
set path=%oldpath%

rem CYGNUS GCC WIN ****************************************************

rem set up path so that "make" is Cygnus's make
set oldpath=%path%
call setcyg.bat

make -f makeflot.cyg 
make -f makefix.cyg

rem restore path
set path=%oldpath%

