:: annot-player.update.cmd
:: 11/5/2011
cd /d d:\protected\releases\player || exit 1

set SOURCE=d:\project\annot-build-desktop\build.win
cd annot-player
cd modules
copy /y "%SOURCE%"\*.exe
copy /y "%SOURCE%"\*.dll
del "Annot Player.exe"
cd ..
copy /y "%SOURCE%\Annot Player.exe"
cd ..
call "Delete Caches.cmd"

pause
