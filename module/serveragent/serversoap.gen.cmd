:: Generate serveragent client side source code
:: Since 9/10/2011
:: See: http://blog.csdn.net/tingsking18/article/details/5456831
::echo off
setlocal

cd /d %~dp0 || exit 1

set SRC=serversoap
set BASE=ss
set HEADER=_serversoap.h
set PREFIX=serverSoap
::set WSDL_URL=http://localhost/services/ServerAgentPort?wsdl
::set WSDL_URL=http://annot.me/services/ServerAgentPort?wsdl
set WSDL_URL=http://annotcloud.com/services/ServerAgentPort?wsdl
set TYPEMAP_URL=%~dp0\wsdl\typemap.dat
set GSOAP_HOME=c:/dev/gsoap
set GSOAP_INCLUDE=%GSOAP_HOME%/include
set NAMESPACE=ServerSoap

if exist "%SRC%~" rd /s /q "%SRC%~"
if exist "%SRC%" ren "%SRC%" "%SRC%~"
if not exist "%SRC%" mkdir "%SRC%"

pushd "%SRC%"
copy "%TYPEMAP_URL%"

:: wsdl2h
:: -n%BASE%: default is 'ns'
:: -q%NAMESPACE%
::
:: soapcpp2
:: -C: generate client-side code only
:: -L: do not generate soapClientLib.cpp
:: -n: use service name to rename function tables
:: -p%PREFIX%
:: -q%NAMESPACE%
:: -i: C++ style class methods, implying -L
:: -j: C style global functions, this is default, opposite of -i
:: -1: generate SOAP 1.1 bindings
:: -2: generate SOAP 1.2 bindings
wsdl2h -q%NAMESPACE% -o"%HEADER%" "%WSDL_URL%" && soapcpp2 -C -i -1 -I"%GSOAP_INCLUDE%" "%HEADER%"

echo %ERRORLEVEL%

dos2unix * 2>nul
popd
pause
