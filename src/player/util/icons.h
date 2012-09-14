#ifndef ICONS_H
#define ICONS_H

// icons.h
// 8/6/2012

#include <QtGui/QFileIconProvider>
#include <QtGui/QIcon>
#include <QtCore/QFileInfo>

QT_FORWARD_DECLARE_CLASS(QFileInfo)

class Icons
{
  static QFileIconProvider iconProvider;

  Icons() { }

public:
 static QIcon iconForFile(const QString &fileName) { return iconForFile(QFileInfo(fileName)); }
 static QIcon iconForFile(const QFileInfo &fi);
 static QIcon iconForSuffix(const QString &suffix);
 static QIcon iconForUrl(const QString &url);
 static QIcon fileIcon();
 static QIcon folderIcon();
 static QIcon driveIcon();
 static QIcon networkIcon();
 static QIcon trashIcon();
 static QIcon desktopIcon();
 static QIcon computerIcon();

 static QIcon gameIcon();
 static QIcon videoIcon();
 static QIcon audioIcon();
 static QIcon imageIcon();

};

#endif // ICONS_H
