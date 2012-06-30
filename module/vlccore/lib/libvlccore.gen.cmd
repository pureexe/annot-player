:: libvlccore.gen.cmd
:: 2/28/2012
@echo echo off
setlocal

cd /d %~dp1

set DLL=libvlccore.dll
call dll2lib "%DLL%"

ren libvlccore.exp vlccore.exp
ren libvlccore.lib vlccore.lib
