:: libvlc.gen.cmd
:: Since 6/30/2011
:: http://wiki.videolan.org/GenerateLibFromDll
:: dump: libvlc.dll => libvlc.def => libvlc.lib
setlocal
cd /d %~dp0

set VLC_DLL=libvlc.dll
set VLC_DEF=libvlc.def
set VLC_LIB=vlc.lib
set VLC_NAME=libvlc

echo EXPORTS > "%VLC_DEF%"
for /f "usebackq tokens=4,* delims=_ " %%i in (`dumpbin /exports "%VLC_DLL%"`) do if %%i==%VLC_NAME% echo %%i_%%j >> "%VLC_DEF%"

lib /def:"%VLC_DEF%" /out:"%VLC_LIB%" /machine:x86

pause
:: EOF
