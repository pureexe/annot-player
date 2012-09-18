#!/bin/bash -v
# 11/12/2011
#
# Note: libssl and libcrypto is using the system default at runtime (usually /usr/lib).

PREFIX=$HOME/build
test -e "$PREFIX" || mkdir "$PREFIX"
cd "$PREFIX"  || exit 1

## environment

COLOR=green
VERSION=0.1.9.0
DMG_SIZE=250m

TARGET="Annot Stream"
TARGET_DMG=$TARGET.dmg
TARGET_ZIP=annot-player-$VERSION-mac.zip

APP_SRC=$HOME/Developer/Annot/Player/annot-player
APP_BUILD=$HOME/Developer/Annot/Player/annot-build-desktop/build.mac
#QT_HOME=/Developer/QtSDK/Desktop/Qt/default/gcc
#QT_HOME=/opt/local/share/qt4
QT_HOME=$HOME/opt/qt
VLC_APP=/Applications/VLC.app
VLC_HOME=$VLC_APP/Contents/MacOS
VLC_FRAMEWORKS=$VLC_APP/Contents/Frameworks

export PATH=$QT_HOME/bin:$PATH

APP_NAME="Annot Player"
APP=$APP_NAME.app
APP_CONTENTS=$APP/Contents
APP_MACOS=$APP_CONTENTS/MacOS
APP_FRAMEWORKS=$APP_CONTENTS/Frameworks
APP_PLUGINS=$APP_CONTENTS/PlugIns
APP_INFO=$APP_CONTENTS/Info.plist
APP_BIN=$APP_MACOS/$APP_NAME
APP_RESOURCES=$APP_CONTENTS/Resources

BROWSER_NAME="Annot Browser"
BROWSER_APP=$BROWSER_NAME.app
BROWSER_CONTENTS=$BROWSER_APP/Contents
BROWSER_MACOS=$BROWSER_CONTENTS/MacOS
BROWSER_BIN=$BROWSER_MACOS/$BROWSER_NAME
BROWSER_RESOURCES=$BROWSER_CONTENTS/Resources

DOWNLOADER_NAME="Annot Downloader"
DOWNLOADER_APP=$DOWNLOADER_NAME.app
DOWNLOADER_CONTENTS=$DOWNLOADER_APP/Contents
DOWNLOADER_MACOS=$DOWNLOADER_CONTENTS/MacOS
DOWNLOADER_BIN=$DOWNLOADER_MACOS/$DOWNLOADER_NAME
DOWNLOADER_RESOURCES=$DOWNLOADER_CONTENTS/Resources

TRANSLATOR_NAME="Annot Translator"
TRANSLATOR_APP=$TRANSLATOR_NAME.app
TRANSLATOR_CONTENTS=$TRANSLATOR_APP/Contents
TRANSLATOR_MACOS=$TRANSLATOR_CONTENTS/MacOS
TRANSLATOR_BIN=$TRANSLATOR_MACOS/$TRANSLATOR_NAME
TRANSLATOR_RESOURCES=$TRANSLATOR_CONTENTS/Resources

## copy package

test -e "$TARGET" && finder-remove "$TARGET"
test -e "$TARGET" && exit 1
mkdir -p "$TARGET"
cd "$TARGET" || exit 1

cp -Rv "$APP_SRC"/licenses Licenses || exit 1
cp -v "$APP_SRC"/COPYING COPYING.txt || exit 1
cp -v "$APP_SRC"/ChangeLog ChangeLog.txt || exit 1
cp -v "$APP_SRC"/README "Read Me.txt" || exit 1
dos2unix "Read Me.txt"
dos2unix ChangeLog.txt
dos2unix COPYING.txt

cp -v "$APP_SRC"/src/common/share/apps.icns .VolumeIcon.icns || exit 1

cp -Rv "$APP_BUILD/$APP" "$APP" || exit 1
rm -Rf "$APP_MACOS"/*
cp -v "$APP_BUILD/$APP_BIN" "$APP_BIN" || exit 1

cp -Rv "$APP_BUILD/$BROWSER_APP" . || exit 1
rm -Rf "$BROWSER_MACOS"/*
cp -v "$APP_BUILD/$BROWSER_BIN" "$BROWSER_BIN" || exit 1

cp -Rv "$APP_BUILD/$DOWNLOADER_APP" . || exit 1
rm -Rf "$DOWNLOADER_MACOS"/*
cp -v "$APP_BUILD/$DOWNLOADER_BIN" "$DOWNLOADER_BIN" || exit 1

cp -Rv "$APP_BUILD/$TRANSLATOR_APP" . || exit 1
rm -Rf "$TRANSLATOR_MACOS"/*
cp -v "$APP_BUILD/$TRANSLATOR_BIN" "$TRANSLATOR_BIN" || exit 1

macdeployqt "$APP"
macdeployqt "$BROWSER_APP"
macdeployqt "$DOWNLOADER_APP"
macdeployqt "$TRANSLATOR_APP"

## link apps to APP

cd "$BROWSER_APP"/Contents || exit 1
finder-remove Frameworks
finder-remove PlugIns
ln -s ../../"$APP_FRAMEWORKS" || exit 1
ln -s ../../"$APP_PLUGINS" || exit 1
cd ../..

cd "$DOWNLOADER_APP"/Contents || exit 1
finder-remove Frameworks
finder-remove PlugIns
ln -s ../../"$APP_FRAMEWORKS" || exit 1
ln -s ../../"$APP_PLUGINS" || exit 1
cd ../..

cd "$TRANSLATOR_APP"/Contents || exit 1
finder-remove Frameworks
finder-remove PlugIns
ln -s ../../"$APP_FRAMEWORKS" || exit 1
ln -s ../../"$APP_PLUGINS" || exit 1
cd ../..

cd "$BROWSER_APP"/Contents/MacOS || exit 1
ln -s ../../../"$APP_MACOS"/lua || exit 1
ln -s ../../../"$APP_MACOS"/images || exit 1
ln -s ../../../"$APP_MACOS"/jsf || exit 1
ln -s ../../../"$APP_MACOS"/translations || exit 1
cd ../../..

cd "$DOWNLOADER_APP"/Contents/MacOS || exit 1
ln -s ../../../"$APP_MACOS"/lua || exit 1
ln -s ../../../"$APP_MACOS"/jsf || exit 1
ln -s ../../../"$APP_MACOS"/translations || exit 1
cd ../../..

cd "$TRANSLATOR_APP"/Contents/MacOS || exit 1
ln -s ../../../"$APP_MACOS"/dic || exit 1
ln -s ../../../"$APP_MACOS"/dict || exit 1
ln -s ../../../"$APP_MACOS"/etc || exit 1
ln -s ../../../"$APP_MACOS"/jsf || exit 1
ln -s ../../../"$APP_MACOS"/translations || exit 1
cd ../../..

## translations

mkdir "$APP_MACOS"/translations || exit 1
cp -Rv "$QT_HOME"/translations/qt_{ja,zh_CN,zh_TW}.qm "$APP_MACOS"/translations/ || exit 1

## functions

otool_libs()
{
  local target=$1
  otool -L "$target" | sed '1d' | awk '{ print $1 }' | \
    grep -v "^@" | \
    grep -v "^/usr/lib/" | \
    grep -v "^/usr/X11/lib/" | \
    grep -v "^/System/Library/Frameworks/" | \
    sort | uniq
}

change_macports_qt()
{
  local qt=$1
  local target=$2
  install_name_tool -change \
    /opt/local/lib/$qt.framework/Versions/4/$qt \
    @executable_path/../Frameworks/$qt.framework/Versions/4/$qt \
    "$target"
}

change_macports_lib()
{
  local lib=$1
  local target=$2
  cp -rv /opt/local/lib/$lib "$APP_FRAMEWORKS/$lib"
  install_name_tool -change \
    /opt/local/lib/$lib \
    @executable_path/../Frameworks/$lib \
    "$target"
}

## deploy qt plugins

#mkdir -p "$APP_PLUGINS"/{codecs,imageformats,sqldrivers} | exit 1
#
#for codec in cn jp kr tw; do
#  dylib=codecs/libq${codec}codecs.dylib
#  cp "$QT_HOME"/plugins/$dylib "$APP_PLUGINS/$dylib"
#  change_macports_qt QtCore "$APP_PLUGINS/$dylib"
#done
#
#for fmt in gif ico jpeg mng svg tiff; do
#  dylib=imageformats/libq$fmt.dylib
#  cp "$QT_HOME"/plugins/$dylib "$APP_PLUGINS/$dylib"
#  #install_name_tool -id @executable_path/../PlugIns/$dylib "$APP_PLUGINS/$dylib"
#  change_macports_qt QtCore "$APP_PLUGINS/$dylib"
#  change_macports_qt QtGui "$APP_PLUGINS/$dylib"
#done
#
#dylib=imageformats/libqsvg.dylib
#change_macports_qt QtSvg "$APP_PLUGINS/$dylib"
#change_macports_qt QtXml "$APP_PLUGINS/$dylib"
#
#dylib=imageformats/libqjpeg.dylib
#change_macports_lib libjpeg.8.dylib  "$APP_PLUGINS/$dylib"
#
#dylib=imageformats/libqmng.dylib
#change_macports_lib libmng.1.dylib  "$APP_PLUGINS/$dylib"
#
#dylib=imageformats/libqtiff.dylib
#change_macports_lib libtiff.3.dylib  "$APP_PLUGINS/$dylib"
#
#dylib=sqldrivers/libqsqlite.dylib
#cp "$QT_HOME"/plugins/$dylib "$APP_PLUGINS/$dylib"
##install_name_tool -id @executable_path/../PlugIns/$dylib "$APP_PLUGINS/$dylib"
#change_macports_qt QtCore "$APP_PLUGINS/$dylib"
#change_macports_qt QtSql "$APP_PLUGINS/$dylib"
#change_macports_lib libsqlite3.0.dylib  "$APP_PLUGINS/$dylib"

## alter vlc lib
install_name_tool -change \
  @loader_path/lib/libvlc.5.dylib \
  @executable_path/lib/libvlc.5.dylib \
 "$APP_BIN" || exit 1
install_name_tool -change \
  @loader_path/lib/libvlccore.5.dylib \
  @executable_path/lib/libvlccore.5.dylib \
 "$APP_BIN" || exit 1

## deploy vlc frameworks

#test -e "$APP_FRAMEWORKS" || mkdir "$APP_FRAMEWORKS" || exit 1
cp -Rv "$VLC_FRAMEWORKS"/* "$APP_FRAMEWORKS"/ || exit 1

## deploy vlc plugins

change_vlc_lib()
{
  local target=$1
  for i in $(otool_libs "$target"); do
    cp -v "$i" "$APP_FRAMEWORKS"/ || exit 1
    install_name_tool -change "$i" \
      "@executable_path/../Frameworks/$(basename "$i")" \
      "$target"
  done
}

#rm -Rfv "$APP_MACOS"/{lua,plugins}

cp -Rv "$VLC_HOME"/{lib,plugins,share} "$APP_MACOS"/ || exit 1
rm -f "$APP_MACOS"/plugins/*.dat
rm -f "$APP_MACOS"/share/*.png

#luac -s -o "$APP_MACOS"/lua/playlist/youtube.luac "$APP_SRC"/lib/player/lua/playlist/youtube.lua  || exit 1

#for i in "$APP_MACOS"/plugins/*.dylib; do
#  change_vlc_lib "$i"
#done

## copy luascript

LUAPATH="$APP_MACOS/lua"
test -e "$LUAPATH" || mkdir "$LUAPATH"|| exit 1
cp "$APP_SRC"/lib/luaresolver/lua/luascript.lua "$LUAPATH"/ || exit 1
cp -R "$APP_SRC"/lib/luaresolver/lua/luascript/*.lua "$LUAPATH"/  || exit 1
cp -R "$APP_SRC"/lib/luaresolver/lua/luascript/*/*.lua "$LUAPATH"/  || exit 1

## copy doc
cp -R "$APP_SRC"/lib/webbrowser/doc "$BROWSER_MACOS"/ || exit 1

## copy images
cp -R "$APP_SRC"/lib/qtx/images "$APP_MACOS"/ || exit 1

## copy table
cp -R "$APP_SRC"/lib/textcodec/table "$APP_MACOS"/ || exit 1

## copy mecab
cp -R "$APP_SRC"/lib/mecab/etc "$APP_MACOS"/ || exit 1

mkdir "$APP_MACOS"/dic
cp -R /opt/local/mecab/dic/ipadic-utf8 "$APP_MACOS"/dic/ipadic || exit 1

mkdir "$APP_MACOS"/dict
cp ~/opt/edict/dict/edict2u "$APP_MACOS"/dict/ || exit 1

## copy jsf
cp -R "$APP_SRC"/lib/annotcloud/jsf "$APP_MACOS"/ || exit 1

## copy images
cp -R "$APP_SRC"/src/player/avatars "$APP_MACOS"/ || exit 1

## deploy macports libs

update_all_libs()
{
  local i
  for i in ` \
    otool -L "$APP_FRAMEWORKS"/*.dylib | sed '1d' | awk '{ print $1 }' | \
    grep -v "^@" | \
    grep -v "^/usr/lib/" | \
    grep -v "^/usr/X11/lib/" | \
    grep -v "^/System/Library/Frameworks/" | \
    sort | uniq \
    `; do
    cp -rv "$i" "$APP_FRAMEWORKS"/
  done
}

change_lib()
{
  local target=$1

  install_name_tool -id \
    "@executable_path/../Frameworks/$(basename "$target")" \
    "$target"

  local i
  for i in $(otool_libs "$target"); do
    install_name_tool -change "$i" \
      "@executable_path/../Frameworks/$(basename "$i")" \
      "$target"
  done
}

change_all_libs()
{
  local i
  for i in  "$APP_FRAMEWORKS"/*.dylib; do
    change_lib "$i"
  done
}

#update_all_libs
#update_all_libs
#update_all_libs
#update_all_libs
#update_all_libs
#update_all_libs
#update_all_libs
#update_all_libs
#update_all_libs
#update_all_libs
#
#change_all_libs

## finalize

repair-permissions
remove-thumbnails
chlabel $COLOR "$APP"
#chlabel $COLOR "$BROWSER_APP"
#chlabel $COLOR "$DOWNLOADER_APP"

## Rez
cp "$APP_SRC"/../video.IconRez "$APP_RESOURCES"/ || exit 1
chflags hidden "$APP_RESOURCES"/video.IconRez

cp "$APP_SRC"/../video.IconRez "$DOWNLOADER_RESOURCES"/ || exit 1
chflags hidden "$DOWNLOADER_RESOURCES"/video.IconRez

cp "$APP_SRC"/../video.IconRez "$BROWSER_RESOURCES"/ || exit 1
chflags hidden "$BROWSER_RESOURCES"/video.IconRez

cd ..

chlabel $COLOR "$TARGET"
test -e "$TARGET_DMG" && finder-remove "$TARGET_DMG"
hdiutil create -size $DMG_SIZE -srcdir "$TARGET" "$TARGET_DMG" || exit 1

chlabel $COLOR "$TARGET_DMG"
test -e "$TARGET_ZIP" && finder-remove "$TARGET_ZIP"
zip -9 "$TARGET_ZIP" "$TARGET_DMG" || exit 1

test -e "/Volumes/$TARGET" && {
  pushd /Volumes/
  eject "$TARGET"
  popd
}
#open "$TARGET_DMG"

# EOF
