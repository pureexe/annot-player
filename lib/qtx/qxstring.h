#ifndef QXSTRING_H
#define QXSTRING_H

// qxstring.h
// 3/11/2012

#include "qtx/qtxglobal.h"
#include <QtCore/QString>
#include <QtCore/QRegExp>

QTX_BEGIN_NAMESPACE

int qxMatchLastString(const QString &str, QRegExp &rx);

QString qxIncreaseString(const QString &s);
QString qxDecreaseString(const QString &s);

QTX_END_NAMESPACE

#endif // QXSTRING_H
