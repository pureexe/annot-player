# webbrowser_shared.pri
# 10/9/2011

INCLUDEPATH += $$PWD

DEFINES += WITH_MODULE_WEBBROWSER

HEADERS += \
    $$PWD/webbrowser_config.h \
    $$PWD/webbrowser.h

!mac: LIBS  += -lwebbrowser
mac:  LIBS  += -framework WebBrowser

# EOF
