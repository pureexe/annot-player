# webbrowser.pri
# Since 10/9/2011

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/webbrowser_config.h \
    $$PWD/webbrowser.h

!mac: LIBS  += -lwebbrowser
mac:  LIBS  += -framework WebBrowser

# EOF
