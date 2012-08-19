#ifndef ACUPDATER_H
#define ACUPDATER_H

// acupdater.h
// 8/16/2012

#include <QtCore/QStringList>

class AcUpdater : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AcUpdater)
  typedef AcUpdater Self;
  typedef QObject Base;
public:
  explicit AcUpdater(QObject *parent = nullptr)
    : Base(parent) { }

public:
  bool isRunning() const;

public slots:
  void open();
  void openArguments(const QStringList &args);
};

#endif // ACBROWSER_H
