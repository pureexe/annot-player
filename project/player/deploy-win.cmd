:: annot-player.update.cmd
:: 11/5/2011
setlocal
cd /d d:\project\publish\releases\player || exit 1
test -e "Delete Caches.cmd" || exit 1

set BUILD=/Volumes/local/project/annot-build-desktop/build.win
set SOURCE=/Volumes/local/project/annot

cd annot-player || exit 1
cd modules || exit 1

cp -v "%BUILD%"/*.{exe,dll} .

rm -fv "Annot Player.exe"
rm -fv hook.dll

cd ..

cp -v "%BUILD%/Annot Player.exe" .

cp -v "%SOURCE%/README" "Read Me.txt"
unix2dos "Read Me.txt"

rm -Rf licenses
cp -R "%SOURCE%/licenses" .
cp "%SOURCE%/COPYING" licenses/COPYING.txt
cp "%SOURCE%/ChangeLog" licenses/ChangeLog.txt

unix2dos licenses/ChangeLog.txt

cd ..
call "Delete Caches.cmd"

:: EOF
