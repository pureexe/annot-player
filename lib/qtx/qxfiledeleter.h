#ifndef QXFILEDELETER_H
#define QXFILEDELETER_H

// qxfiledeleter.h
// 2/6/2012

#include "qtx/qtxglobal.h"
#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QStringList>

QTX_BEGIN_NAMESPACE

class QxFileDeleter : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(QxFileDeleter)
  typedef QxFileDeleter Self;
  typedef QObject Base;

  QMutex mutex_;
  QStringList files_;

public:
  static Self *globalInstance() { static Self g; return &g; }

protected:
  explicit QxFileDeleter(QObject *parent = nullptr)
    : Base(parent) { }

  ~QxFileDeleter() { deleteAll(); }

public:
  bool isEmpty() const { return files_.isEmpty(); }

public slots:
  void deleteFileLater(const QString &path, bool async = true);
  void deleteAll();
};

QTX_END_NAMESPACE

#endif // QXFILEDELETER_H
