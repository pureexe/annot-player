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
  explicit FileInputStream(QFile *file, QObject *parent = nullptr)
    : Base(parent), file_(file) { Q_ASSERT(file); }

  explicit FileInputStream(const QString &path, QObject *parent = nullptr)
    : Base(parent)
  { file_ = new QFile(path, this); file_->open(QIODevice::ReadOnly); }

  bool isOpen() const { return file_ && file_->isOpen(); }

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
  { return QFile::copy(file_->fileName(), fileName); }
};

#endif // FILEINPUTSTREAM_H
