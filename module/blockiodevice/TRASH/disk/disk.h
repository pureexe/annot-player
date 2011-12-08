#pragma once

// disk.h
// 8/17/2011

#include <QIODevice>
#include <QByteArray>

class Disk // : public QIODevice - TODO
  : public QObject
{
  Q_OBJECT
  typedef Disk Self;
  typedef QObject Base;

public:
  typedef QIODevice::OpenMode OpenMode;

  // See also MEDIA_TYPE: http://msdn.microsoft.com/en-us/library/aa365231(v=vs.85).aspx
  enum MediaType {
    Unknown          = 0x00,
    F5_1Pt2_512      = 0x01,
    F3_1Pt44_512     = 0x02,
    F3_2Pt88_512     = 0x03,
    F3_20Pt8_512     = 0x04,
    F3_720_512       = 0x05,
    F5_360_512       = 0x06,
    F5_320_512       = 0x07,
    F5_320_1024      = 0x08,
    F5_180_512       = 0x09,
    F5_160_512       = 0x0a,
    RemovableMedia   = 0x0b,
    FixedMedia       = 0x0c,
    F3_120M_512      = 0x0d,
    F3_640_512       = 0x0e,
    F5_640_512       = 0x0f,
    F5_720_512       = 0x10,
    F3_1Pt2_512      = 0x11,
    F3_1Pt23_1024    = 0x12,
    F5_1Pt23_1024    = 0x13,
    F3_128Mb_512     = 0x14,
    F3_230Mb_512     = 0x15,
    F8_256_128       = 0x16,
    F3_200Mb_512     = 0x17,
    F3_240M_512      = 0x18,
    F3_32M_512       = 0x19
  };

  // See also DISK_GEOMETRY: http://msdn.microsoft.com/en-us/library/aa363972(v=vs.85).aspx
  struct Geometry {
    typedef Geometry Self;

    MediaType mediaType;
    quint64 cylinders;
    quint32 tracksPerCylinder;
    quint32 sectorsPerTrack;
    quint32 bytesPerSector;

    Geometry()
      : mediaType(MediaType(0)), cylinders(0), tracksPerCylinder(0), sectorsPerTrack(0), bytesPerSector(0) { }

    qint64 size() const
    { return cylinders * tracksPerCylinder * sectorsPerTrack * bytesPerSector; }

    void clear(); ///< Set all fields to zero
    bool isNull() const; ///< True if all fields are zero
  };

public:
  /**
   * Some examples of accepted valid device names:
   * - X:
   * - \\.\X:
   *
   * See: http://support.microsoft.com/kb/100027
   */
  static QString guessDeviceFileName(const QString &deviceHint);
  static bool isValidDeviceFileName(const QString &fileName);

  static Geometry geometryForHandle(void *handle);

  // - Constructions -
public:
  explicit Disk(const QString &fileName, QObject *parent = 0);
  explicit Disk(QObject *parent = 0);
  virtual ~Disk(); ///< \override

  const QString &fileName() const;
  void setFileName(const QString &fileName);

  Geometry geometry() const;    ///< Not available unless isOpen() or setGeometry() is invoked.
  void setGeometry(const Geometry &geo); ///< Should not be invoked in most cases

  qint64 size() const; ///< Computed from geometry! Not avaiable unless geometry is available.

  // - I/O -
public:
  bool isOpen() const;
  virtual bool open(OpenMode mode, bool lockRemovableMedia = true); ///< Must be ReadOnly
  virtual void close();
  QByteArray read(int maxSize);

  virtual qint64 pos() const;
  virtual bool seek(qint64 pos);

  // - Implementation -
protected:
  bool isRemovableMediaLocked() const;
  void setRemovableMediaLocked(bool t);

private:
  QString fileName_;
  void *handle_; // HANDLE
  void *pmr_; // PREVENT_MEDIA_REMOVAL
  Geometry geometry_;
};


// EOF
