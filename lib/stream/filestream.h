#ifndef FILESTREAM_H
#define FILESTREAM_H

// filestream.h
// 3/16/2012

#include "lib/stream/inputoutputstream.h"
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
  explicit FileStream(QFile *file, QObject *parent = nullptr)
    : Base(parent), file_(file)
  { Q_ASSERT(file); }

  explicit FileStream(const QString &path, QObject *parent = nullptr)
    : Base(parent)
  { file_ = new QFile(path, this); file_->open(QIODevice::ReadWrite); }

  bool isOpen() const { return file_ && file_->isOpen(); }

  // - Read -
public:
  qint64 size() const override { return file_->size(); }
  qint64 pos() const override { return file_->pos(); }

  bool reset() override { return file_->reset(); }

  bool seek(qint64 pos) override { return file_->seek(pos); }

  qint64 skip(qint64 count) override
  { return seek(file_->pos() + count) ? count : 0; }

  qint64 read(char *data, qint64 maxSize) override { return file_->read(data, maxSize); }
  QByteArray readAll() override { return file_->readAll(); }

  bool writeFile(const QString &fileName) override
  { flush(); return QFile::copy(file_->fileName(), fileName); }

  // - Write -
public:
  qint64 availableSize() const override { return file_->size(); }

  qint64 write(const char *data, qint64 maxSize) override
  { return file_->write(data, maxSize); }

  QString fileName() const { return file_->fileName(); }

public slots:
  void flush() override { file_->flush(); }
};

#endif // FILESTREAM_H
