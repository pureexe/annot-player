#ifndef FILESTREAM_H
#define FILESTREAM_H

// filestream.h
// 3/16/2012

#include "module/stream/inputoutputstream.h"
#include <QtCore/QFile>
#include <QtCore/QObject>

class FileStream : public QObject, public InputOutputStream
{
  Q_OBJECT
  Q_DISABLE_COPY(FileStream)
  typedef FileStream Self;
  typedef QObject Base;

  QFile *file_;

public:
  explicit FileStream(QFile *file, QObject *parent = 0)
    : Base(parent), file_(file)
  { Q_ASSERT(file); }

  explicit FileStream(const QString &path, QObject *parent = 0)
    : Base(parent)
  { file_ = new QFile(path, this); file_->open(QIODevice::ReadWrite); }

  bool isOpen() const { return file_ && file_->isOpen(); }

  // - Read -
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
  { flush(); return QFile::copy(file_->fileName(), fileName); }

  // - Write -
public:
  virtual qint64 availableSize() const { return file_->size(); } ///< \reimp

  virtual qint64 write(const char *data, qint64 maxSize) ///< \reimp
  { return file_->write(data, maxSize); }

  QString fileName() const { return file_->fileName(); }

public slots:
  virtual void flush() { file_->flush(); } ///< \reimp
};

#endif // FILESTREAM_H
