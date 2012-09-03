// icons.cc
// 8/6/2012

#include "icons.h"
#include "module/searchengine/searchenginerc.h"
#include "module/mrlanalysis/mrlanalysis.h"

// - Icon Provider -

QFileIconProvider Icons::iconProvider;

QIcon
Icons::fileIcon()
{ return iconProvider.icon(QFileIconProvider::File); }

QIcon
Icons::folderIcon()
{ return iconProvider.icon(QFileIconProvider::Folder); }

QIcon
Icons::driveIcon()
{ return iconProvider.icon(QFileIconProvider::Drive); }

QIcon
Icons::networkIcon()
{ return iconProvider.icon(QFileIconProvider::Network); }

QIcon
Icons::trashIcon()
{ return iconProvider.icon(QFileIconProvider::Trashcan); }

QIcon
Icons::desktopIcon()
{ return iconProvider.icon(QFileIconProvider::Desktop); }

QIcon
Icons::computerIcon()
{ return iconProvider.icon(QFileIconProvider::Computer); }

QIcon
Icons::iconForFile(const QFileInfo &fi)
{
  QIcon ret = iconProvider.icon(fi);
  return ret.isNull() ? fileIcon() : ret;
}

QIcon
Icons::iconForSuffix(const QString &suffix)
{
  Q_UNUSED(suffix)
  // TODO
  // CHECKPOINT
  return fileIcon();
}

QIcon
Icons::iconForUrl(const QString &url)
{
  return url.contains(MA_EIGEN_NICOVIDEO) ? QIcon(WBRC_IMAGE_NICOVIDEO) :
         url.contains(MA_EIGEN_BILIBILI) ? QIcon(WBRC_IMAGE_BILIBILI) :
         url.contains(MA_EIGEN_ACFUN) ? QIcon(WBRC_IMAGE_ACFUN) :
         url.contains(MA_EIGEN_YOUKU) ? QIcon(WBRC_IMAGE_YOUKU) :
         url.contains(MA_EIGEN_YOUTUBE) ? QIcon(WBRC_IMAGE_YOUTUBE) :
         networkIcon();
}

// - Theme Icons -

QIcon
Icons::gameIcon()
{ return fileIcon(); }

QIcon
Icons::videoIcon()
{ return fileIcon(); }

QIcon
Icons::audioIcon()
{ return fileIcon(); }

QIcon
Icons::imageIcon()
{ return fileIcon(); }

// EOF
