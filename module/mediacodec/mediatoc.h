#ifndef MEDIATOC_H
#define MEDIATOC_H

// mediatoc.h
// 2/13/2012

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QList>

class MediaToc : public QObject
{
  Q_OBJECT
  typedef MediaToc Self;
  typedef QObject Base;

  QList<qint64> offsets_;
  QList<qint64> timestamps_;
  mutable QMutex m_;

  qint64 baseTimestamp_;

public:
  explicit MediaToc(QObject *parent = 0)
    : Base(parent), baseTimestamp_(0) { }

public:
  qint64 roundOffset(qint64 pos) const;
  qint64 roundTimestamp(qint64 msecs) const;

  qint64 offsetByTime(qint64 msecs) const;

  int size() const { return offsets_.size(); }

public slots:
  void setBaseTimestamp(qint64 msecs) { baseTimestamp_ = msecs; }

  void append(qint64 pos, qint64 msecs)
  {
    m_.lock();
    offsets_.append(pos);
    timestamps_.append(baseTimestamp_ + msecs);
    m_.unlock();
  }

  void clear()
  {
    m_.lock();
    offsets_.clear();
    timestamps_.clear();
    m_.unlock();
  }
};

#endif // MEDIATOC_H
