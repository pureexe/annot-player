# macdeployqt
# 11/11/2011
TEMPLATE = app
TARGET = macdeployqt
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += shared.h
SOURCES += main.cc shared.cc
CONFIG += qt warn_on
CONFIG -= app_bundle

# EOF
