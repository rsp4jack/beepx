@echo off
set _cd=%CD%
call c:\NTDDK\bin\setenv.bat C:\NTDDK checked
cd %_cd%
set _cd=
build
pause
