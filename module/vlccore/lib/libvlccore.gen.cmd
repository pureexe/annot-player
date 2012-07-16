:: libvlccore.gen.cmd
:: 2/28/2012
setlocal
cd /d %~dp0

set DLL=libvlccore.dll
call dll2lib "%DLL%"

move /y libvlccore.exp vlccore.exp
move /y libvlccore.lib vlccore.lib

pause
:: EOF
