#!/bin/bash -v
# 4/1/2012

VERSION=0.1.9.0
APP=annot-player
ZIPFILE=$APP-$VERSION-debian.zip

SRC_PREFIX=/Volumes/local/dev/annot
BUILD_PREFIX=$HOME/build

export DEBFULLNAME=$PACKAGE_AUTHOR

## copy

rm -fv "$BUILD_PREFIX"/annot-*_$VERSION-*.deb

cd `dirname "$0"`
for i in browser down player; do
  name=annot-$i
  ${i}*/deploy-debian.sh
  mv "$BUILD_PREFIX/$name"/*.deb "$BUILD_PREFIX"/ || exit 1
  rm -Rf "$BUILD_PREFIX/$name"
done

cd "$BUILD_PREFIX" || exit 1
rm -fv "$ZIPFILE"
zip -9 "$ZIPFILE" annot-*_$VERSION-*.deb || exit 1

# EOF
