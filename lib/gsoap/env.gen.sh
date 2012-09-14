#!/bin/sh -v
# Generate global common environment including serialization codes.
# 7/23/2012
# See: http://www.cs.fsu.edu/~engelen/soapdoc2.html#tth_sEc19.33

require()
{
  local i
  for i; do
    which "$i" >/dev/null 2>&1 || \
      die "$ME: cannot find '$i' in PATH, ABORT"
  done
}
require soapcpp2

SRC=env
SRC_bak=${SRC}~
HEADER=env.h

rm -Rfv "$SRC_bak"
test -e "$SRC" && mv "$SRC" "${SRC_bak}"
mkdir -p "$SRC"

cd "$SRC" || exit 1
rm -fv "$HEADER"
touch "$HEADER"

soapcpp2 -penv "$HEADER"

dos2unix *
