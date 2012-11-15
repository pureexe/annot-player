:: deploy (SHIFT-JIS)
:: 8/12/2012
@echo off
setlocal
cd /d %~dp0
cd /d ..\..

set CYGWIN_VERSION=cygwin1

set CYGWIN=nodosfilewarning
set PATH=%CD%\update\%CYGWIN_VERSION%;%PATH%

chmod -R 755 .

if exist Data attrib +h Data
if exist Licenses attrib +h Licenses
if exist Update attrib +h Update
if exist Icon.ico attrib +h Icon.ico

attrib +h *.exe
if exist "[ Browse ].exe" attrib -h "[ Browse ].exe"
if exist "[ Download ].exe" attrib -h "[ Download ].exe"
if exist "[ Play ].exe" attrib -h "[ Play ].exe"
if exist "[ Translate ].exe" attrib -h "[ Translate ].exe"
if exist "[ Update ].exe" attrib -h "[ Update ].exe"

attrib +h *.txt
if exist Changes.txt attrib -h Changes.txt
if exist License.txt attrib -h License.txt
if exist Readme.txt attrib -h Readme.txt

if exist desktop.ini attrib +h +s desktop.ini
  
::if exist Readme.txt attrib +r Readme.txt
::if exist Changes.txt attrib +r Changes.txt
::if exist License.txt attrib +r License.txt
::if exist Update.txt attrib +r Update.txt

attrib +r +s .

:: EOF
