#!/bin/bash -v
# 4/4/2012

VERSION=0.1.4.4
APP=annot-player
ZIPFILE=$APP-$VERSION-alpha-fedora.zip

SRC_PREFIX=/Volumes/local/devel/annot
BUILD_PREFIX=$HOME/rpmbuild
RELEASE_PREFIX=$HOME/build

export DEBFULLNAME=$PACKAGE_AUTHOR

## copy

test -d "$RELEASE_PREFIX" || mkdir -p "$RELEASE_PREFIX" || exit 1

cd `dirname "$0"`
for i in browser downloader player; do
  name=annot-$i
  $i/deploy-fedora.sh
  mv "$BUILD_PREFIX"/RPMS/*/$name-*.rpm "$RELEASE_PREFIX"/ || exit 1
  rm -Rf "$BUILD_PREFIX"/*
done

cd "$RELEASE_PREFIX" || exit 1
rm -f "$ZIPFILE"
zip -9 "$ZIPFILE" annot-*-$VERSION-*.rpm || exit 1

# EOF
