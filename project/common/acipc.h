#ifndef ACIPC_H
#define ACIPC_H

// acipc.h
// 4/9/2012
#include <QtCore/QObject>
#include <QtCore/QStringList>

class MetaCallFilter;

class AcIpcController : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AcIpcController)
  typedef AcIpcController Self;
  typedef QObject Base;

  uint id_;
  MetaCallFilter *filter_;
public:
  explicit AcIpcController(QObject *parent = 0, uint id = 0)
    : Base(parent), id_(id), filter_(0) { }

signals:
  void arguments(QStringList);

#ifdef WITH_MODULE_METACALL
public:
  bool isActive() const;
public slots:
  void startServer();
  void startClient();
  void stop();

protected slots:
  void invalidateFilter();
#endif // WITH_MODULE_METACALL

public:
  static bool isProcessRunning(const QString &processName);
  static bool open(const QString &app, const QStringList &args = QStringList());
};

#endif // ACIPC_H
