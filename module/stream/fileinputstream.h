#ifndef FILEINPUTSTREAM_H
#define FILEINPUTSTREAM_H

// fileinputstream.h
// 2/13/2012

#include "module/stream/inputstream.h"
#include <QtCore/QFile>
#include <QtCore/QObject>

class FileInputStream : public QObject, public InputStream
{
  Q_OBJECT
  Q_DISABLE_COPY(FileInputStream)
  typedef FileInputStream Self;
  typedef QObject Base;

  QFile *file_;

public:
  explicit FileInputStream(QFile *file, QObject *parent = 0)
    : Base(parent), file_(file) { Q_ASSERT(file); }

  explicit FileInputStream(const QString &path, QObject *parent = 0)
    : Base(parent)
  { file_ = new QFile(path, this); file_->open(QIODevice::ReadOnly); }

  bool isOpen() const { return file_ && file_->isOpen(); }

public:
  virtual qint64 size() const { return file_->size(); } ///< \reimp
  virtual qint64 pos() const { return file_->pos(); } ///< \reimp

  virtual bool reset()  { return file_->reset(); } ///< \reimp

  virtual bool seek(qint64 pos) { return file_->seek(pos); } ///< \reimp

  virtual qint64 skip(qint64 count) ///< \reimp
  { return seek(file_->pos() + count) ? count : 0; }

  virtual qint64 read(char *data, qint64 maxSize) { return file_->read(data, maxSize); } ///< \reimp
  virtual QByteArray readAll() { return file_->readAll(); } ///< \reimp

  virtual bool writeFile(const QString &fileName) ///< \reimp
  { return QFile::copy(file_->fileName(), fileName); }
};

#endif // FILEINPUTSTREAM_H
