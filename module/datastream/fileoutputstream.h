#ifndef FILEOUTPUTSTREAM_H
#define FILEOUTPUTSTREAM_H

// fileoutputstream.h
// 2/13/2012

#include "outputstream.h"
#include <QObject>
#include <QFile>

class FileOutputStream : public QObject, public OutputStream
{
  Q_OBJECT
  typedef FileOutputStream Self;
  typedef QObject Base;

  QFile *file_;

public:
  explicit FileOutputStream(QFile *file, QObject *parent = 0)
    : Base(parent), file_(file) { Q_ASSERT(file); }

  virtual qint64 realSize() const { return file_->size(); } ///< \override

  virtual qint64 write(const char *data, qint64 maxSize) ///< \override
  { return file_->write(data, maxSize); }

  virtual void flush() { file_->flush(); } ///< \override
};

#endif // FILEOUTPUTSTREAM_H
