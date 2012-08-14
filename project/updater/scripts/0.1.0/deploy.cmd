:: deploy (SHIFT-JIS)
:: 8/12/2012
@echo off
setlocal
cd /d "%~dp0"

set ROOT=..\..

if exist "%ROOT%\Data" attrib +h "%ROOT%\Data"
if exist "%ROOT%\Licenses" attrib +h "%ROOT%\Licenses"
if exist "%ROOT%\Update" attrib +h "%ROOT%\Update"
if exist "%ROOT%\Icon.ico" attrib +h "%ROOT%\Icon.ico"

if exist "%ROOT%\desktop.ini" attrib +h +s "%ROOT%\desktop.ini."
  
attrib +r "%ROOT%"

:: EOF
