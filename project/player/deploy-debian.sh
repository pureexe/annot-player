#!/bin/bash -v
# 11/25/2011
# See: http://wiki.maemo.org/Packaging_a_Qt_application
# See: https://wiki.kubuntu.org/PackagingGuide/QtApplication
## environment PACKAGE_AUTHOR="Annot Cloud"
PACKAGE_EMAIL=annotcloud@gmail.com
PACKAGE_LICENSE=gpl
PACKAGE_NAME=annot-player
PACKAGE_VERSION=0.1.0.2
PACKAGE_ARCH=i386
PACKAGE_SECTION=video
PACKAGE_PRIORITY=optional

SRC_PREFIX=/Volumes/local/project/annot
SRC_RULES="$SRC_PREFIX/project/player/debian.rules"
SRC_CONTROL="$SRC_PREFIX/project/player/debian.control"
BUILD_PREFIX=$HOME/tmp/annot-player
BUILD_SRC="$BUILD_PREFIX/$PACKAGE_NAME-$PACKAGE_VERSION"
BUILD_DEBIAN="$BUILD_SRC/debian"

export DEBFULLNAME=$PACKAGE_AUTHOR

## copy

rm -Rf "$BUILD_PREFIX"
test -e "$BUILD_PREFIX" || mkdir -p "$BUILD_PREFIX"
test -e "$BUILD_PREFIX" || exit 1

cp -Rv "$SRC_PREFIX" "$BUILD_SRC"

# build
cd "$BUILD_SRC" || exit 1
#qmake-qt4 -config release PREFIX=/usr ../annot.pro
echo | dh_make --createorig --single -e "$PACKAGE_EMAIL" -c "$PACKAGE_LICENSE"
rm -f "$BUILD_DEBIAN"/*.{ex,EX}
cp "$SRC_RULES" "$BUILD_SRC"/debian/rules
cp "$SRC_CONTROL" "$BUILD_SRC"/debian/control
#test -e "$BUILD_SRC"/debian/files || cat > "$BUILD_SRC"/debian/files << EOF
#${PACKAGE_NAME}_${PACKAGE_VERSION}-1_${PACKAGE_ARCH}.deb $PACKAGE_SECTION $PACKAGE_PRIORITY
#EOF

repair-permissions

debuild -i -us -uc -b | $HOME/private/bin/colormake

# EOF
