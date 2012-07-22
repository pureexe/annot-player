#ifndef BLOCKIODEVICE_H
#define BLOCKIODEVICE_H

// blockiodevice.h
// 12/3/2011

#include <QtCore/QIODevice>

///  Read/write/seek pos must be blocked.
class BlockIODevice : public QIODevice
{
  Q_OBJECT
  Q_DISABLE_COPY(BlockIODevice)
  typedef BlockIODevice Self;
  typedef QIODevice Base;

  // - Constructions -
public:
  explicit BlockIODevice(QObject *parent = nullptr)
    : Base(parent), fd_((fd_t)-1), blockSize_(0) { }

  explicit BlockIODevice(const QString &fileName, QObject *parent = nullptr)
    : Base(parent), fd_((fd_t)-1), blockSize_(0), fileName_(fileName) { }

  ~BlockIODevice() { if (isOpen()) close(); } ///< \reimp

  // - Properties -
public:
  const QString &fileName() const { return fileName_; }
  void setBlockIODeviceName(const QString &fileName) { fileName_ = fileName; }

  ///< Return sector size if the file is a block device.
  int blockSize() const { return blockSize_; }

  bool isAudioCD() const;

  // - Overriden -
public:
  bool open(OpenMode mode) override; ///< Only read mode is implemented.
  void close() override;

  /*
  bool atEnd() const override;
  qint64 bytesAvailable() const override;
  qint64 bytesToWrite() const override;
  bool isSequential() const override;
  qint64 pos() const override;
  bool reset() override;
  bool seek( qint64 pos ) override;
  qint64 size() const override;
  bool waitForBytesWritten( int msecs ) override;
  bool waitForReadyRead( int msecs ) override;
  */

  bool canReadLine() const override ///< Not implemented.
  { return false; }

protected:
  //qint64 readLineData( char * data, qint64 maxSize) override;
  qint64 readData(char *data, qint64 maxSize ) override;
  qint64 writeData(const char *data, qint64 maxSize) override;

  // - Implementations -
public:
#ifdef Q_OS_WIN
  typedef void *fd_t; // file handle
#else
  typedef int fd_t; // file descriptor
#endif // Q_OS_WIN
private:
  fd_t fd_;
  int blockSize_;
  QString fileName_;

};

#endif // BLOCKIODEVICE_H
