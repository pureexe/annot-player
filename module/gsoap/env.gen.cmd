:: Generate global common environment including serialization codes.
:: 10/7/2011
:: See: http://www.cs.fsu.edu/~engelen/soapdoc2.html#tth_sEc19.33
::echo off
setlocal

set SRC=env
set HEADER=env.h

if exist "%SRC%~" rd /s /q "%SRC%~"
if exist "%SRC%" ren "%SRC%" "%SRC%~"
if not exist "%SRC%" mkdir "%SRC%"

pushd "%SRC%"
rm -f "%HEADER%"
touch "%HEADER%"

soapcpp2 -penv "%HEADER%"

echo %ERRORLEVEL%

dos2unix *
popd
pause
