#ifndef METACALLPROPAGATOR_H
#define METACALLPROPAGATOR_H

// 4/9/2012
// metacallpropagator.h

#include <QtCore/QObject>

QT_FORWARD_DECLARE_CLASS(QHostAddress)

class MetaCallFilter;

/**
 *  All signals in this class will be propagated to remote object.
 *  DO NOT USE LOCAL SIGNALS
 */
class MetaCallPropagator : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(MetaCallPropagator)
  typedef MetaCallPropagator Self;
  typedef QObject Base;
  typedef MetaCallFilter Filter;

  Filter *filter_;

  // - Construction -
public:
protected:
  explicit MetaCallPropagator(QObject *parent = nullptr)
    : Base(parent), filter_(nullptr) { }

  bool isActive() const;

  bool startServer(const QHostAddress &address, int port);
  bool startClient(const QHostAddress &address, int port);

  bool isServer() const;
  bool isClient() const;

public slots:
  void stop();

protected slots:
  void installMetaCallFilter();
};

#endif // METACALLPROPAGATOR_H
