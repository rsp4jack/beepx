@echo off
set _cd=%CD%
call c:\NTDDK\bin\setenv.bat C:\NTDDK free
cd %_cd%
set _cd=
build
copy /Y objfre\i386\beepxp.sys ..\..\bin\beepxp\
pause
