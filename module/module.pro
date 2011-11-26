# module.pro
# Since 7/31/2011

include(aiml/aiml.pri)
include(debug/debug.pri)
include(clientagent/clientagent.pri)
include(doll/doll.pri)
include(gsoap/gsoap.pri)
include(player/player.pri)
include(serveragent/serveragent.pri)
include(translator/translator.pri)

TEMPLATE = subdirs

!unix: SUBDIRS = webbrowser
unix:include(webbrowser/webbrowser_static.pri)

# EOF
