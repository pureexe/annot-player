#ifndef _QTEXT_FILEDELETER_H
#define _QTEXT_FILEDELETER_H

// qtext/autodeleter.h
// 2/6/2012

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QStringList>

// Note: Due to Qt4 bug, namespace would cause crash QMutex on Mac OS X.
//namespace QtExt {

class FileDeleter : public QObject
{
  Q_OBJECT
  typedef FileDeleter Self;
  typedef QObject Base;

  QMutex mutex_;
  QStringList files_;

public:
  static Self *globalInstance() { static Self g; return &g; }

protected:
  explicit FileDeleter(QObject *parent = 0)
    : Base(parent) { }

  ~FileDeleter() { deleteAll(); }

public:
  bool isEmpty() const { return files_.isEmpty(); }

public slots:
  void deleteFileLater(const QString &path, bool async = true);
  void deleteAll();
};

//} // namespace QtExt

#endif // _QTEXT_FILEDELETER_H
