#ifndef QXFS_H
#define QXFS_H

// qxfs.h
// 3/16/2012

#include "qtx/qtxglobal.h"
#include <QtCore/QString>

#ifdef Q_OS_WIN
# define FILE_PATH_SEP      "\\"
#else
# define FILE_PATH_SEP      "/"
#endif // Q_OS_WIN

QTX_BEGIN_NAMESPACE

QString qxEscapeFileName(const QString &name);

bool qxTouchFile(const QString &fileName);

bool qxTrashFile(const QString &fileName);
bool qxTrashOrRemoveFile(const QString &fileName);

QTX_END_NAMESPACE

#endif // QXFS_H
