#ifndef FILEOUTPUTSTREAM_H
#define FILEOUTPUTSTREAM_H

// fileoutputstream.h
// 2/13/2012

#include "module/stream/outputstream.h"
#include <QtCore/QFile>
#include <QtCore/QObject>

class FileOutputStream : public QObject, public OutputStream
{
  Q_OBJECT
  Q_DISABLE_COPY(FileOutputStream)
  typedef FileOutputStream Self;
  typedef QObject Base;

  QFile *file_;

public:
  explicit FileOutputStream(QObject *parent = 0)
    : Base(parent), file_(0) { file_ = new QFile(this); }
  explicit FileOutputStream(QFile *file, QObject *parent = 0)
    : Base(parent), file_(file) { if (!file_) file_ = new QFile(this); }
  explicit FileOutputStream(const QString &fileName, QObject *parent = 0)
    : Base(parent) { file_ = new QFile(fileName, this); }

  bool isOpen() const { return file_ && file_->isOpen(); }

public:
  virtual qint64 availableSize() const { return file_->size(); } ///< \override

  virtual qint64 write(const char *data, qint64 maxSize) ///< \override
  { return file_->write(data, maxSize); }

  QString fileName() const { return file_->fileName(); }

  bool open() { return file_->open(QIODevice::WriteOnly); }
public slots:
  virtual void flush() { file_->flush(); } ///< \override
  void close() { file_->close(); }

  void setFileName(const QString &path)
  { file_->setFileName(path); }
};

#endif // FILEOUTPUTSTREAM_H
