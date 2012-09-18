// qxebpluginfactory.cc
// 4/15/2012
// See: http://opensource.apple.com/source/WebKit/WebKit-6531.9/mac/Plugins/WebBasePluginPackage.h
// Use NSBundle or dlopen?
// See: http://blog.csdn.net/jhluroom/article/details/6786768

#include "qtx/qxwebpluginfactory.h"
#include <QtWebKit>

#define DEBUG "qtx::webpluginfactory"
#include "qtx/qxdebug.h"

QTX_BEGIN_NAMESPACE

// - Construction -

QxWebPluginFactory::QxWebPluginFactory(QObject *parent)
  : Base(parent)
{ updatePlugins(); }

void
QxWebPluginFactory::updatePlugins()
{
}

QObject*
QxWebPluginFactory::create(const QString &mimeType, const QUrl &url,
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

QTX_END_NAMESPACE
