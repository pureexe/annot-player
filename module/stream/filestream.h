#ifndef FILESTREAM_H
#define FILESTREAM_H

// filestream.h
// 3/16/2012

#include "inputoutputstream.h"
#include <QtCore/QObject>
#include <QtCore/QFile>

class FileStream : public QObject, public InputOutputStream
{
  Q_OBJECT
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
  virtual qint64 size() const { return file_->size(); } ///< \override
  virtual qint64 pos() const { return file_->pos(); } ///< \override

  virtual bool reset()  { return file_->reset(); } ///< \override

  virtual bool seek(qint64 pos) { return file_->seek(pos); } ///< \override

  virtual qint64 skip(qint64 count) ///< \override
  { return seek(file_->pos() + count) ? count : 0; }

  virtual qint64 read(char *data, qint64 maxSize) { return file_->read(data, maxSize); } ///< \override
  virtual QByteArray readAll() { return file_->readAll(); } ///< \override

  virtual bool writeFile(const QString &fileName) ///< \override
  { flush(); return QFile::copy(file_->fileName(), fileName); }

  // - Write -
public:
  virtual qint64 availableSize() const { return file_->size(); } ///< \override

  virtual qint64 write(const char *data, qint64 maxSize) ///< \override
  { return file_->write(data, maxSize); }

  QString fileName() const { return file_->fileName(); }

public slots:
  virtual void flush() { file_->flush(); } ///< \override
};

#endif // FILESTREAM_H
