:: deploy (SHIFT-JIS)
:: 8/12/2012
@echo off
setlocal
cd /d %~dp0
cd /d ..\..

set PATH=%~dp0;%PATH%

chmod -R 755 .

if exist Data attrib +h Data
if exist Licenses attrib +h Licenses
if exist Update attrib +h Update
if exist Icon.ico attrib +h Icon.ico

if exist desktop.ini attrib +h +s desktop.ini
  
attrib +r +s .

:: EOF
