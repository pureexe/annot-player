#ifndef FILEINPUTSTREAM_H
#define FILEINPUTSTREAM_H

// fileinputstream.h
// 2/13/2012

#include "inputstream.h"
#include <QObject>
#include <QFile>

class FileInputStream : public QObject, public InputStream
{
  Q_OBJECT
  typedef FileInputStream Self;
  typedef QObject Base;

  QFile *file_;

public:
  explicit FileInputStream(QFile *file, QObject *parent = 0)
    : Base(parent), file_(file) { Q_ASSERT(file); }

  explicit FileInputStream(const QString &path, QObject *parent = 0)
    : Base(parent)
  { file_ = new QFile(path, this); file_->open(QIODevice::ReadOnly); }

public:
  virtual qint64 size() const { return file_->size(); } ///< \override
  virtual qint64 pos() const { return file_->pos(); } ///< \override

  virtual bool reset()  { return file_->reset(); } ///< \override

  virtual bool seek(qint64 pos) { return file_->seek(pos); } ///< \override

  virtual qint64 skip(qint64 count) ///< \override
  { return seek(file_->pos() + count) ? count : 0; }

  virtual qint64 read(char *data, qint64 maxSize) { return file_->read(data, maxSize); } ///< \override
  virtual QByteArray readAll() { return file_->readAll(); } ///< \override
};

#endif // FILEINPUTSTREAM_H
