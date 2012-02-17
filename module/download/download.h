#ifndef DOWNLOAD_H
#define DOWNLOAD_H

// download.h
// 2/4/2012

#include <QString>
#include <QUrl>

bool dlget(const QString &path, const QUrl &url,
           const QString &header = QString(),
           bool zipped = false,  bool async = true,
           int retries = 3);

bool dlpost(const QString &path, const QUrl &url, const QByteArray &data = QByteArray(),
            const QString &header = QString(),
            bool zipped = false,  bool async = true,
            int retries = 3);

#endif // DOWNLOAD_H
