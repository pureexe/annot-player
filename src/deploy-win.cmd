:: deploy-win.cmd
:: 11/5/2011
setlocal
cd /d s:/build || exit /b 1

set VERSION=0.2.0.0
set APP=annot-player
set TARGET=Annot Player
set ZIPFILE=%APP%-%VERSION%-win.zip

::set BASENAME=%APP%-%VERSION%-win
::rm -Rf "%BASENAME%" || exit /b 1
::mkdir "%BASENAME%" || exit /b 1
::cd "%BASENAME%" || exit /b 1

set MSVC_HOME=/Volumes/win/Windows/System32
set MSVC_DLLS=msvcp100.dll,msvcr100.dll

set MSVC90_HOME=/Volumes/win/Program Files/Microsoft Visual Studio 9.0
set MSVC90_REDIST=%MSVC90_HOME%/VC/redist/x86/Microsoft.VC90.CRT

set QT_HOME=/Volumes/win/qt/4
set QT_DLLS=QtCore4.dll,QtDeclarative4.dll,QtGui4.dll,QtNetwork4.dll,QtScript4.dll,QtSql4.dll,QtSvg4.dll,QtWebkit4.dll,QtXml4.dll,QtXmlPatterns4.dll,phonon4.dll
set QT_TRANSLATIONS=qt_ja.qm,qt_zh_CN.qm,qt_zh_TW.qm
set QT_PLUGINS=accessible bearer codecs graphicssystems iconengines imageformats sqldrivers

set ITH_HOME=/Volumes/win/dev/ith
::set ITH_DLLS=ITH.dll,ITH_engine.dll
set ITH_DLLS=IHF.dll,IHF_DLL.dll,ITH_Engine.dll

set OPENSSL_HOME=/Volumes/win/dev/openssl/1.0.0j
set OPENSSL_DLLS=libeay32.dll,ssleay32.dll

set MECAB_HOME=/Volumes/win/dev/mecab
set MECAB_DLL=libmecab.dll
set MECAB_DATA=dic

set EDICT_HOME=/Volumes/win/dev/edrdg
set EDICT_DICT=edict2u

set CYGWIN_HOME=
set CYGWIN_VERSION=cygwin1
set CYGWIN_EXES=lftp.exe,chmod.exe
set CYGWIN_DLLS=cyggcc_s-1.dll,cygstdc++-6.dll,cygcrypto-1.0.0.dll,cygncurses-10.dll,cygncursesw-10.dll,cygwin1.dll,cygexpat-1.dll,cygiconv-2.dll,cygintl-8.dll,cygreadline7.dll,cygssl-1.0.0.dll,cygz.dll

::set GPAC_HOME=/Volumes/win/dev/gpac
::set GPAC_DLLS=js32.dll,libgpac.dll,libeay32.dll,ssleay32.dll

::set MP4BOX_HOME=%GPAC_HOME%
::set MP4BOX_EXE=MP4Box.exe

::set MP4V2_HOME=/Volumes/win/dev/mp4v2
::set MP4V2_DLL=libmp4v2.dll

::set LUA_HOME=/Volumes/win/dev/lua
::set LUA_DLL=lua52.dll

set ZLIB_HOME=/Volumes/win/dev/zlib
set ZLIB_DLL=zlib1.dll

::set VLC_HOME=/Volumes/win/Program Files/VideoLAN/VLC
::set VLC_HOME=/Volumes/local/Applications/VideoLAN/VLC
set VLC_HOME=D:/Applications/VideoLAN/VLC
set VLC_DLLS=libvlc.dll,libvlccore.dll
set VLC_DATA=plugins,lua,locale

set BUILD=/Volumes/local/dev/annot-build-desktop/build.win
set SOURCE=/Volumes/local/dev/annot

::set CURL_HOME=/Volumes/win/dev/curl
::set CURL_BIN=curl.exe

::set GZIP_HOME=/Volumes/win/dev/gzip
::set GZIP_BIN=gzip.exe

:: deploy into app dir

rm -Rf "%TARGET%"
mkdir "%TARGET%"
cd "%TARGET%" || exit /b 1

cp -v "%BUILD%/[ Browse ].exe" . || exit /b 1
cp -v "%BUILD%/[ Download ].exe" . || exit /b 1
cp -v "%BUILD%/[ Play ].exe" . || exit /b 1
cp -v "%BUILD%/[ Translate ].exe" . || exit /b 1
cp -v "%BUILD%/[ Update ].exe" . || exit /b 1

cp -v "%SOURCE%/README" Readme.txt || exit /b 1
unix2dos Readme.txt

::cp -v "%SOURCE%/UPDATE" Update.txt || exit /b 1
::unix2dos "Update.txt"

cp -v "%SOURCE%/ChangeLog" Changes.txt || exit /b 1
unix2dos Changes.txt

::rm -Rf licenses
::cp -R "%SOURCE%/licenses" Licenses

cp "%SOURCE%/COPYING" License.txt || exit /b 1
unix2dos License.txt

:: deploy modules
mkdir Library
mkdir Library\Deprecated
pushd Library\Deprecated || exit /b 1

mkdir translations
cp -v "%QT_HOME%"/translations/{%QT_TRANSLATIONS%} translations/ || exit /b 1

for %%i in (%QT_PLUGINS%) do (
  mkdir %%i
  cp -v "%QT_HOME%"/plugins/%%i/*4.dll %%i/ || exit /b 1
  rm -f %%i/*d4.dll
)

rm -f sqldrivers/{qsqlodbc4.dll,qsqlpsql4.dll}
::rm -f graphicssystems/qglgraphicssystem4.dll

cp -v "%QT_HOME%"/bin/{%QT_DLLS%} . || exit /b 1

cp -v "%MSVC_HOME%"/{%MSVC_DLLS%} . || exit /b 1
cp -Rv "%MSVC90_REDIST%" . || exit /b 1
cp -v "%VLC_HOME%"/{%VLC_DLLS%} . || exit /b 1
::cp -v "%ITH_HOME%"/bin/{%ITH_DLLS%} . || exit /b 1
cp -v "%OPENSSL_HOME%"/{%OPENSSL_DLLS%} . || exit /b 1
::cp -v "%GPAC_HOME%"/bin/{%GPAC_DLLS%} . || exit /b 1
::cp -v "%MP4BOX_HOME%"/bin/%MP4BOX_EXE% . || exit /b 1
::cp -v "%MP4V2_HOME%"/bin/%MP4V2_DLL% . || exit /b 1
::cp -v "%LUA_HOME%"/bin/%LUA_DLL% . || exit /b 1
cp -v "%ZLIB_HOME%"/bin/%ZLIB_DLL% . || exit /b 1

::cp -v "%MECAB_HOME%"/bin/%MECAB_DLL% . || exit /b 1
::cp -Rv "%MECAB_HOME%"/%MECAB_DATA% . || exit /b 1

::cp -v "%CURL_HOME%"/bin/%CURL_BIN% . || exit /b 1
::cp -v "%GZIP_HOME%"/bin/%GZIP_BIN% . || exit /b 1

cp -Rv "%VLC_HOME%"/{%VLC_DATA%} . || exit /b 1

::rm -Rfv plugins || exit /b 1
::mkdir plugins || exit /b 1
::cp -Rv "%VLC_HOME%"/plugins/*/*.dll plugins/

rm -fv plugins/*.dat* || exit /b 1

:: FIXME: playlist plugin bug in VLC 2.0.2+
cp -v "%VLC_HOME%"/../VLC-2.0.1/plugins/demux/libplaylist_plugin.dll plugins/demux/ || exit /b 1

:: Remove useless VLC data
rm -Rfv plugins/gui
rm -Rfv lua/http

::cp -v "%BUILD%"/*.{exe,dll} . || exit /b 1
cp -v "%BUILD%"/*.{exe,dll} .

rm -fv "[ Play ].exe" "[ Browse ].exe" "[ Download ].exe" "[ Translate ].exe" "[ Update ].exe"
rm -fv "annot-update.exe"
rm -fv "annot-tester.exe"
rm -fv hook.dll
rm -fv webbrowser.dll

:: compile lua

::luac -s -o luascript.luac "%SOURCE%"/lib/mrlresolver/lua/luascript.lua  || exit /b 1
::cp -Rv "%SOURCE%"/lib/mrlresolver/lua/luascript .  || exit /b 1
::pushd luascript || exit /b 1
::for %%d in (lib sitelist .) do (
::  pushd %%d || exit /b 1
::  for %%i in (*.lua) do luac -s -o %%ic %%i && rm -f %%i
::  popd
::)
::popd
set LUA_PATH=lua/luascript
mkdir "%LUA_PATH%" || exit /b 1
cp "%SOURCE%"/lib/luaresolver/lua/luascript.lua "%LUA_PATH%"/ || exit /b 1
cp -v "%SOURCE%"/lib/luaresolver/lua/luascript/*.lua "%LUA_PATH%"/  || exit /b 1
cp -v "%SOURCE%"/lib/luaresolver/lua/luascript/*/*.lua "%LUA_PATH%"/  || exit /b 1

:: doc
cp -Rv "%SOURCE%"/lib/webbrowser/doc . || exit /b 1

:: etc
cp -Rv "%SOURCE%"/lib/mecab/etc . || exit /b 1

:: edict
mkdir dict
cp -v "%EDICT_HOME%/dict/"%EDICT_DICT% dict/ || exit /b 1

:: images
cp -Rv "%SOURCE%"/lib/qtx/images . || exit /b 1

:: table
cp -Rv "%SOURCE%"/lib/textcodec/table . || exit /b 1

:: jsf
cp -Rv "%SOURCE%"/lib/annotcloud/jsf . || exit /b 1

:: images
cp -Rv "%SOURCE%"/src/player/avatars . || exit /b 1

popd

mkdir Update
cd Update || exit /b 1

cp -Rv "%SOURCE%"/src/updater/scripts/* .

mkdir %CYGWIN_VERSION% || exit /b 1
cp -v "%CYGWIN_HOME%"/bin/{%CYGWIN_EXES%,%CYGWIN_DLLS%} %CYGWIN_VERSION%/ || exit /b 1

cp -v "%BUILD%/annot-update.exe" . || exit /b 1

cd ..

:: Scripts
cp -v "%SOURCE%"/scripts/* . || exit /b 1

:: desktop.ini
cp -v "%SOURCE%"/src/common/share/apps.ico icon.ico || exit /b 1
cp -v "%SOURCE%"/src/common/share/desktop.ini.txt desktop.ini || exit /b 1

:: repair permissions

chown -R jichi .
chmod -R 755 .

attrib +h Data
attrib +h Licenses
attrib +h Update

attrib +r License.txt || exit /b 1
attrib +r Readme.txt || exit /b 1
attrib +r Changes.txt || exit /b 1

:: See: http://msdn.microsoft.com/en-us/library/windows/desktop/cc144102(v=vs.85).aspx
attrib +h icon.ico
attrib +h +s desktop.ini
attrib +r .

:: archive
::call "Delete Caches.cmd"

cd ..

::rm -f "%ZIPFILE%"
::zip -9r "%ZIPFILE%" "%TARGET%" > nul

:: EOF
