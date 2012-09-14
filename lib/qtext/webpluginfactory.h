#ifndef _QTEXT_WEBPLUGINFACTORY_H
#define _QTEXT_WEBPLUGINFACTORY_H

// qtext/webpluginfactory.h
// 4/15/2012

#include <QtWebKit/QWebPluginFactory>

namespace QtExt {

class WebPluginFactory: public QWebPluginFactory
{
  Q_OBJECT
  Q_DISABLE_COPY(WebPluginFactory)
  typedef WebPluginFactory Self;
  typedef QWebPluginFactory Base;

   QList<Plugin> plugins_;

public:
  static Self *globalInstance() { static Self g; return &g; };

protected:
  explicit WebPluginFactory(QObject *parent = nullptr);

public:
  QObject *create(const QString &mimeType, const QUrl &url,
                  const QStringList &argNames, const QStringList &argValues) const override;

  QList<Plugin> plugins() const override { return plugins_; }

  void refreshPlugins() override { }

protected slots:
  void updatePlugins();
};

} // namespace QtExt

#endif // _QTEXT_WEBPLUGINFACTORY_H
