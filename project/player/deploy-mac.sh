#!/bin/bash -v
# 11/12/2011

PREFIX=$HOME/build
test -e "$PREFIX" || mkdir "$PREFIX"
cd "$PREFIX"  || exit 1

## environment

COLOR=purple
VERSION=0.1.3.4
DMG_SIZE=200m

TARGET="Annot Player"
TARGET_DMG=$TARGET.dmg
TARGET_ZIP=annot-player-$VERSION-mac.zip

APP_SRC=$HOME/Developer/Annot/Player/annot-player
APP_BUILD=$HOME/Developer/Annot/Player/annot-build-desktop/build.mac
QT_HOME=/opt/local/share/qt4
#QT_HOME=/Developer/QtSDK/Desktop/Qt/default/gcc
VLC_APP=/Applications/VLC.app
VLC_HOME=$VLC_APP/Contents/MacOS
VLC_FRAMEWORKS=$VLC_APP/Contents/Frameworks

APP_NAME="Annot Player"
APP=$APP_NAME.app
APP_CONTENTS=$APP/Contents
APP_MACOS=$APP_CONTENTS/MacOS
APP_FRAMEWORKS=$APP_CONTENTS/Frameworks
APP_PLUGINS=$APP_CONTENTS/PlugIns
APP_INFO=$APP_CONTENTS/Info.plist
APP_BIN=$APP_MACOS/$APP_NAME
APP_RESORUCES=$APP_CONTENTS/Resources

## copy package

test -e "$TARGET" && finder-remove "$TARGET"
test -e "$TARGET" && exit 1
mkdir -p "$TARGET"
cd "$TARGET" || exit 1

cp -Rv "$APP_SRC"/licenses Licenses || exit 1
cp -v "$APP_SRC"/COPYING Licenses/COPYING.txt || exit 1
cp -v "$APP_SRC"/ChangeLog ChangeLog.txt || exit 1
cp -v "$APP_SRC"/README "Read Me.txt" || exit 1
dos2unix "Read Me.txt"
dos2unix ChangeLog.txt

cp -Rv "$APP_BUILD/$APP" "$APP" || exit 1
rm -Rf "$APP_MACOS"/*

#cp -v "$APP_BUILD/$APP_INFO" "$APP_INFO"/ || exit 1

## redeploy qt frameworks

#rm -Rfv "$APP_FRAMEWORKS"
#rm -Rfv "$APP_PLUGINS"
#rm -fv "$APP_RESORUCES"/qt.conf

cp -v "$APP_BUILD/$APP_BIN" "$APP_BIN" || exit 1
macdeployqt "$APP"

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
  install_name_tool -change /opt/local/lib/$qt.framework/Versions/4/$qt @executable_path/../Frameworks/$qt.framework/Versions/4/$qt "$target"
}

change_macports_lib()
{
  local lib=$1
  local target=$2
  cp -rv /opt/local/lib/$lib "$APP_FRAMEWORKS/$lib"
  install_name_tool -change /opt/local/lib/$lib @executable_path/../Frameworks/$lib "$target"
}

## deploy qt plugins

mkdir -p "$APP_PLUGINS"/{imageformats,sqldrivers} | exit 1

dylib=imageformats/libqjpeg.dylib
cp "$QT_HOME"/plugins/$dylib "$APP_PLUGINS/$dylib"
#install_name_tool -id @executable_path/../PlugIns/$dylib "$APP_PLUGINS/$dylib"
change_macports_qt QtCore "$APP_PLUGINS/$dylib"
change_macports_qt QtGui "$APP_PLUGINS/$dylib"
change_macports_lib libjpeg.8.dylib  "$APP_PLUGINS/$dylib"

dylib=imageformats/libqgif.dylib
cp "$QT_HOME"/plugins/$dylib "$APP_PLUGINS/$dylib"
#install_name_tool -id @executable_path/../PlugIns/$dylib "$APP_PLUGINS/$dylib"
change_macports_qt QtCore "$APP_PLUGINS/$dylib"
change_macports_qt QtGui "$APP_PLUGINS/$dylib"
change_macports_lib libgif.4.1.6.dylib  "$APP_PLUGINS/$dylib"

dylib=imageformats/libqmng.dylib
cp "$QT_HOME"/plugins/$dylib "$APP_PLUGINS/$dylib"
#install_name_tool -id @executable_path/../PlugIns/$dylib "$APP_PLUGINS/$dylib"
change_macports_qt QtCore "$APP_PLUGINS/$dylib"
change_macports_qt QtGui "$APP_PLUGINS/$dylib"
change_macports_lib libmng.1.dylib  "$APP_PLUGINS/$dylib"

dylib=imageformats/libqsvg.dylib
cp "$QT_HOME"/plugins/$dylib "$APP_PLUGINS/$dylib"
#install_name_tool -id @executable_path/../PlugIns/$dylib "$APP_PLUGINS/$dylib"
change_macports_qt QtCore "$APP_PLUGINS/$dylib"
change_macports_qt QtGui "$APP_PLUGINS/$dylib"
change_macports_lib libsvg.2.2.dylib  "$APP_PLUGINS/$dylib"

dylib=imageformats/libqtiff.dylib
cp "$QT_HOME"/plugins/$dylib "$APP_PLUGINS/$dylib"
#install_name_tool -id @executable_path/../PlugIns/$dylib "$APP_PLUGINS/$dylib"
change_macports_qt QtCore "$APP_PLUGINS/$dylib"
change_macports_qt QtGui "$APP_PLUGINS/$dylib"
change_macports_lib libtiff.3.dylib  "$APP_PLUGINS/$dylib"

dylib=sqldrivers/libqsqlite.dylib
cp "$QT_HOME"/plugins/$dylib "$APP_PLUGINS/$dylib"
#install_name_tool -id @executable_path/../PlugIns/$dylib "$APP_PLUGINS/$dylib"
change_macports_qt QtCore "$APP_PLUGINS/$dylib"
change_macports_qt QtSql "$APP_PLUGINS/$dylib"
change_macports_lib libsqlite3.0.dylib  "$APP_PLUGINS/$dylib"

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

#luac -s -o "$APP_MACOS"/lua/playlist/youtube.luac "$APP_SRC"/module/player/lua/playlist/youtube.lua  || exit 1

#for i in "$APP_MACOS"/plugins/*.dylib; do
#  change_vlc_lib "$i"
#done

## copy luascript

LUAPATH="$APP_MACOS/lua"
test -e "$LUAPATH" || mkdir "$LUAPATH"|| exit 1
cp "$APP_SRC"/module/luaresolver/lua/luascript.lua "$LUAPATH"/ || exit 1
cp -R "$APP_SRC"/module/luaresolver/lua/luascript/*.lua "$LUAPATH"/  || exit 1
cp -R "$APP_SRC"/module/luaresolver/lua/luascript/*/*.lua "$LUAPATH"/  || exit 1

## copy jsf
cp -R "$APP_SRC"/module/annotcloud/jsf "$APP_MACOS"/ || exit 1

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

update_all_libs
update_all_libs
update_all_libs
update_all_libs
update_all_libs
update_all_libs
update_all_libs
update_all_libs
update_all_libs
update_all_libs

change_all_libs

## finalize

repair-permissions
remove-thumbnails
chlabel $COLOR "$APP"

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
open "$TARGET_DMG"

# EOF
