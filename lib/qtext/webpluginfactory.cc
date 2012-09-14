// qtext/webpluginfactory.cc
// 4/15/2012
// See: http://opensource.apple.com/source/WebKit/WebKit-6531.9/mac/Plugins/WebBasePluginPackage.h
// Use NSBundle or dlopen?
// See: http://blog.csdn.net/jhluroom/article/details/6786768

#include "lib/qtext/webpluginfactory.h"
#include <QtWebKit>

#define DEBUG "qtext::webpluginfactory"
#include "lib/debug/debug.h"

// - Construction -

QtExt::
WebPluginFactory::WebPluginFactory(QObject *parent)
  : Base(parent)
{ updatePlugins(); }

void
QtExt::
WebPluginFactory::updatePlugins()
{
}

QObject*
QtExt::
WebPluginFactory::create(const QString &mimeType, const QUrl &url,
                         const QStringList &argNames, const QStringList &argValues) const
{
  Q_UNUSED(url)
  Q_UNUSED(argNames)
  Q_UNUSED(argValues)
  foreach (const Plugin &plugin, plugins_)
    foreach (const MimeType &t, plugin.mimeTypes)
      if (!t.name.compare(mimeType, Qt::CaseInsensitive))
        return 0;
  return 0;
}

// EOF
