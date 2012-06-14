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
  explicit WebPluginFactory(QObject *parent = 0);

public:
  ///  \reimp;
  virtual QObject *create(const QString &mimeType, const QUrl &url,
                          const QStringList &argNames, const QStringList &argValues) const;

  ///  \reimp;
  virtual QList<Plugin> plugins() const { return plugins_; }

  virtual void refreshPlugins() { } ///< \reimp;

protected slots:
  void updatePlugins();
};

} // namespace QtExt

#endif // _QTEXT_WEBPLUGINFACTORY_H
