#ifndef QXWEBPLUGINFACTORY_H
#define QXWEBPLUGINFACTORY_H

// qxwebpluginfactory.h
// 4/15/2012

#include "qtx/qtxglobal.h"
#include <QtWebKit/QWebPluginFactory>

QTX_BEGIN_NAMESPACE

class QxWebPluginFactory: public QWebPluginFactory
{
  Q_OBJECT
  Q_DISABLE_COPY(QxWebPluginFactory)
  typedef QxWebPluginFactory Self;
  typedef QWebPluginFactory Base;

   QList<Plugin> plugins_;

public:
  static Self *globalInstance() { static Self g; return &g; };

protected:
  explicit QxWebPluginFactory(QObject *parent = nullptr);

public:
  QObject *create(const QString &mimeType, const QUrl &url,
                  const QStringList &argNames, const QStringList &argValues) const override;

  QList<Plugin> plugins() const override { return plugins_; }

  void refreshPlugins() override { }

protected slots:
  void updatePlugins();
};

QTX_END_NAMESPACE

#endif // QXWEBPLUGINFACTORY_H

