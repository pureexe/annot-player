:: dll2lib.cmd
:: Since 6/30/2011
:: http://wiki.videolan.org/GenerateLibFromDll
:: dump: libvlc.dll => libvlc.def => libvlc.lib
:: 
:: TODO: Alternatively, use impdef
:: See: http://www.cygwin.com/faq/faq.programming.html#faq.programming.msvs-mingw
:: Example:
::   impdef cygwin1.dll > cygwin1.def
::   lib /def=cygwin1.def /out=cygwin1.lib
@echo off
setlocal

set DLL=%~1
set NAME=%~dpn1
set DLL_DEF=%NAME%.def
set DLL_LIB=%NAME%.lib

if not exist "%DLL%" echo Could not find dll, exit && exit /b 1

::for /f "usebackq tokens=4,* delims=_ " %%i in (`dumpbin /exports "%DLL%"`) do if %%i==libvlc echo %%i_%%j >> "%DLL_DEF%"
echo EXPORTS > "%DLL_DEF%"
dumpbin /exports "%DLL%" |grep "^ \+ [0-9]\+ \+[0-9A-F]\+ \+[0-9A-F]\+" | sed "s/ (.*//" | sed "s/^ \+[0-9]\+ \+[0-9A-F]\+ \+[0-9A-F]\+//" >> "%DLL_DEF%" 

lib /def:"%DLL_DEF%" /out:"%DLL_LIB%" /machine:x86
