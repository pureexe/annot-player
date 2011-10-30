# core.pri
# Since 6/28/2011

INCLUDEPATH += $$PWD

HEADERS += \
  $$PWD/annotation.h \
  $$PWD/user.h \
  $$PWD/text.h \
  $$PWD/catalog.h \
  $$PWD/page.h \
  $$PWD/document.h

SOURCES += \
  $$PWD/annotation.cc \
  $$PWD/user.cc \
  $$PWD/catalog.cc \
  $$PWD/text.cc \
  $$PWD/page.cc \
  $$PWD/document.cc

QT      += core gui xml
LIBS    += -lpoppler-qt4
