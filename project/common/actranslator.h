#ifndef ACTRANSLATOR_H
#define ACTRANSLATOR_H

// actranslator.h
// 8/11/2012

#include "project/common/acipc.h"

class AcTranslatorServer : public AcIpcController
{
  Q_OBJECT
  Q_DISABLE_COPY(AcTranslatorServer)
  typedef AcTranslatorServer Self;
  typedef AcIpcController Base;

  static Self *global_;

  // - Construction -
public:
  explicit AcTranslatorServer(QObject *parent = nullptr);
  ~AcTranslatorServer() { stop(); }

public slots:
  void start();
  void stop();
};

class AcTranslator : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AcTranslator)
  typedef AcTranslator Self;
  typedef QObject Base;
  typedef AcIpcController Delegate;

  Delegate *delegate_;

public:
  explicit AcTranslator(QObject *parent = nullptr);

signals:
  void arguments(const QStringList &args);
  void showRequested();

public:
  bool isRunning() const;

public slots:
  void show();
  void open();
  void openArguments(const QStringList &args);

  void translate(const QString &text) { openArguments(QStringList(text)); }
};

#endif // ACTRANSLATOR_H
