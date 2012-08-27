:: update (SHIFT-JIS)
:: 8/12/2012
@echo off
setlocal

set VERSION=0.1.2.3
set TIMESTAMP=20120824
set CYGWIN_VERSION=cygwin1
set ME=Software Update

color 8f
title %ME% (%VERSION%, %TIMESTAMP%)

cd /d %~dp0
cd /d ..\..

set CYGWIN=nodosfilewarning
set PATH=%CD%\update\%CYGWIN_VERSION%;%PATH%

set HOST=210.175.52.140
set SOURCE=/release/annot-player/win32/Annot Stream
set TARGET=.

set EXCLUDE=-X %CYGWIN_VERSION%/ -X %VERSION%/
set EXCLUDE=%EXCLUDE% -X .DS_Store

set MIRROR=mirror
set MIRROR=%MIRROR% --verbose=1
set MIRROR=%MIRROR% --delete
::set MIRROR=%MIRROR% --no-perms --no-umask
::set MIRROR=%MIRROR% --depth-first
set MIRROR=%MIRROR% --use-cache
set MIRROR=%MIRROR% --continue
set MIRROR=%MIRROR% --parallel=8
set MIRROR=%MIRROR% %EXCLUDE% '%SOURCE%' '%TARGET%'

::            1         2         3         4         5         6         7
echo ----------------------------------------------------------------------
echo    Do you want to update the apps? (An Internet access is required)
echo           ソフトを更新しますか？（インターネットが必要です）
echo ----------------------------------------------------------------------
echo.
pause
echo.

echo ----------------------------------------------------------------------
echo                 Please close all apps in Annot Stream!
echo                   全てのAnnotアプリを閉じてください！
echo ----------------------------------------------------------------------
echo.
pause
echo.

title %ME%: Preparing ...

tskill annot-player 2>nul
tskill annot-browser 2>nul
tskill annot-down 2>nul
tskill annot-dict 2>nul

title %ME%: Synchronizing ...

:: see: http://lftp.yar.ru/lftp-man.html
lftp "%HOST%" -e "%MIRROR%; exit"

title %ME%: Deploying ...

pushd update
for /r %%i in (deploy.cmd) do if exist "%%~i" call "%%~i"
popd

title %ME%: Done^!

echo.
echo ----------------------------------------------------------------------
echo                            Update complete!
echo                               更新完了！
echo ----------------------------------------------------------------------
echo.
pause
echo.

echo ----------------------------------------------------------------------
echo                         Press any key to exit♪
echo                              またね～ノシ
echo ----------------------------------------------------------------------
echo.
pause
echo.

if exist Changes.txt explorer Changes.txt

:: EOF
