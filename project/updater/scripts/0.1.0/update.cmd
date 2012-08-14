:: update (SHIFT-JIS)
:: 8/12/2012
@echo off
setlocal
cd /d %~dp0
cd /d ..\..

set PATH=%~dp0;%PATH%

set VERSION=0.1.0

set HOST=210.175.52.140
set SOURCE=/release/annot-player/win32/Annot Stream
set TARGET=.
set EXCLUDE=%VERSION%
set THREADS=4

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

:: see: http://lftp.yar.ru/lftp-man.html
lftp "%HOST%" -e "mirror --verbose=3 --delete --no-perms --no-umask --depth-first --parallel=%THREADS% --exclude '%EXCLUDE%' '%SOURCE%' '%TARGET%'; exit"

pushd update
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

if exist "ChangeLog.txt" explorer "ChangeLog.txt"

:: EOF
