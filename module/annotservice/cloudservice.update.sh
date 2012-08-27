#!/bin/sh -v
# Generate cloud client side source code
# 9/10/2011
# See: http://blog.csdn.net/tingsking18/article/details/5456831

require()
{
  local i
  for i; do
    which "$i" >/dev/null 2>&1 || \
      die "$ME: cannot find '$i' in PATH, ABORT"
  done
}
require wsdl2h soapcpp2

SRC=cloudservice
SRC_bak=${SRC}~
BASE=ss
HEADER=cloudservice.h
#PREFIX=annot
HOST=210.175.54.32
#HOST=localhost
#HOST=localhost:8080
#HOST=annot.me
#HOST=annotcloud.com
WSDL_URL=http://$HOST/service/cloud?wsdl
TYPEMAP_URL=$PWD/wsdl/typemap.dat
GSOAP_HOME=$HOME/opt/src/gsoap
GSOAP_INCLUDE=$GSOAP_HOME/gsoap/import
NAMESPACE=Annot

rm -Rfv "${SRC_bak}"
test -e "$SRC" && mv "$SRC" "$SRC_bak"
mkdir "$SRC"

cd "$SRC" || exit 1
cp -v "$TYPEMAP_URL" . || exit 1

# wsdl2h
# -n%BASE%: default is 'ns'
# -q%NAMESPACE%
# -g enable globals, do not define WITH_NOGLOBAL
#
# soapcpp2
# -C: generate client-side code only
# -L: do not generate soapClientLib.cpp
# -n: use service name to rename function tables
# -p%PREFIX%
# -q%NAMESPACE%
# -i: C++ style class methods, implying -L
# -j: C style global functions, this is default, opposite of -i
# -1: generate SOAP 1.1 bindings
# -2: generate SOAP 1.2 bindings

wsdl2h -q$NAMESPACE -o"$HEADER" "$WSDL_URL" && \
  soapcpp2 -C -i -1 -I"$GSOAP_INCLUDE" "$HEADER"

dos2unix *

cd ..
touch *.cc *.h
