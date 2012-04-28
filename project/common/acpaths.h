#ifndef ACPATHS_H
#define ACPATHS_H

// ac/acpaths.h
// 4/23/2012
#include <QtCore/QObject>
#include <QtCore/QString>

// - Resources -

#define AC_PATH_LUA     ""      // TODO
#define AC_PATH_JSF     ""      // TODO
#define AC_PATH_DOC     ""      // TODO
#define AC_PATH_IMAGES  ""      // TODO
#define AC_PATH_CACHES  ""      // TODO
#define AC_PATH_LOGS    ""      // TODO
#define AC_PATH_DOWNLOADS       AcLocationManager::globalInstance()->downloadsLocation()

// Class to enable tr()
class AcLocationManager : public QObject
{
  Q_OBJECT
  typedef AcLocationManager Self;
  typedef QObject Base;

public:
  static Self *globalInstance() { static Self g; return &g; }
protected:
  explicit AcLocationManager(QObject *parent = 0) : Base(parent) { }

public:
  enum StorageLocation {
    DococumentsLocation,// doc/
    ImagesLocation,     // images/
    JsfLocation,        // jsf/
    LuaScriptsLocation, // lua/
    ReadOnlyLocation,   // dummy
    LogsLocation,       // OS dependendent
    CachesLocation,     // OS dependendent
    DownloadsLocation,  // language dependendent
    ReadWriteLocation   // dummy
  };

  QString storageLocation(StorageLocation l) const
  {
    switch(l) {
    case DownloadsLocation:     return downloadsLocation();
    case CachesLocation:        return AC_PATH_CACHES;
    case LogsLocation:          return AC_PATH_LOGS;
    case LuaScriptsLocation:    return AC_PATH_LUA;
    case ImagesLocation:        return AC_PATH_IMAGES;
    case DococumentsLocation:   return AC_PATH_DOC;
    case JsfLocation:           return AC_PATH_JSF;
    default: Q_ASSERT(0);       return QString();
    }
  }

  QString downloadsLocation() const;

public slots:
  void createDownloadsLocation();
};


#endif // ACPATHS_H
