:: Generate clientagent client side service source code
:: Since 9/10/2011
:: See: http://www.cs.fsu.edu/~engelen/soapdoc2.html#tth_sEc19.33
:: See: http://hi.baidu.com/2sky2sea/blog/item/40ec5555680279c1b745ae9b.html
:: See: http://blog.csdn.net/tingsking18/article/details/5456831
::echo off
setlocal

set SRC=clientsoap
set BASE=cs
set HEADER=_clientsoap.h
set PREFIX=clientSoap
set NAMESPACE=ClientSoap
set WSDL_URL=%~dp0\wsdl\clientagent.wsdl
set TYPEMAP_URL=%~dp0\wsdl\typemap.dat
set GSOAP_HOME=c:/dev/gsoap
set GSOAP_INCLUDE=%GSOAP_HOME%/include

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
:: -S: generate server slide code only
:: -T: generate tester as well
:: -L: do not generate soapServerLib.cpp
:: -n: use service name to rename function tables
:: -p%PREFIX%
:: -q%NAMESPACE%
:: -i: C++ style class methods, implying -L
:: -j: C style global functions, this is default, opposite of -i
:: -1: generate SOAP 1.1 bindings
:: -2: generate SOAP 1.2 bindings
wsdl2h -q%NAMESPACE% -o"%HEADER%" "%WSDL_URL%" && soapcpp2 -S -T -i -1 -I"%GSOAP_INCLUDE%" "%HEADER%"

echo %ERRORLEVEL%

dos2unix *
popd
pause
