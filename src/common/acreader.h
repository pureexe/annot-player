#ifndef ACREADER_H
#define ACREADER_H

// acreader.h
// 4/9/2012

#include "src/common/acipc.h"

class AcReaderServer : public AcIpcController
{
  Q_OBJECT
  Q_DISABLE_COPY(AcReaderServer)
  typedef AcReaderServer Self;
  typedef AcIpcController Base;

  static Self *global_;

  // - Construction -
public:
  explicit AcReaderServer(QObject *parent = nullptr);
  ~AcReaderServer() { stop(); }

public slots:
  void start();
  void stop();
};

class AcReader : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AcReader)
  typedef AcReader Self;
  typedef QObject Base;
  typedef AcIpcController Delegate;

  Delegate *delegate_;
public:
  explicit AcReader(QObject *parent = nullptr);

signals:
  void arguments(const QStringList &args);

public:
  bool isRunning() const;

public slots:
  void open();
  void openArguments(const QStringList &args);
};

#endif // ACREADER_H
