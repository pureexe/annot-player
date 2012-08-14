:: update (SHIFT-JIS)
:: 8/12/2012
@echo off
setlocal
cd /d %~dp0

set VERSION=0.1.0
set ROOT=..\..

set HOST=210.175.52.140
set SOURCE=release/annot-player/win32/Annot Stream
set TARGET=../..
set EXCLUDE=%VERSION%

echo --------------------------------------------------------------------
echo   Do you want to update the apps? (An Internet access is required)
echo          ソフトを更新しますか？（インターネットが必要です）
echo --------------------------------------------------------------------
echo.
pause
echo.

echo --------------------------------------------------------------------
echo            Please close all apps in Annot Stream!
echo              全てのAnnotアプリを閉じてください！
echo --------------------------------------------------------------------
echo.
pause
echo.

lftp "%HOST%" -e "mirror --verbose=3 --delete --exclude '%EXCLUDE%' '%SOURCE%' '%TARGET%'; exit"

pushd ..
for /r %%i in (deploy.cmd) do if exist "%%~i" call "%%~i"
popd

echo.
echo --------------------------------------------------------------------
echo                      Update complete!
echo                          更新完了！
echo --------------------------------------------------------------------
echo.
pause
echo.

if exist "%ROOT%\ChangeLog.txt" explorer "%ROOT%\ChangeLog.txt"

:: EOF
