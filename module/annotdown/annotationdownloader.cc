// annotationdownloader.cc
// 7/6/2012

#include "module/annotdown/annotationdownloader.h"
#include "module/qtext/network.h"
#include "module/qtext/filesystem.h"
#ifdef WITH_MODULE_COMPRESS
# include "module/compress/qgzip.h"
#endif // WITH_MODULE_COMPRESS
#ifdef WITH_MODULE_ANNOTCACHE
# include "module/annotcache/annotationcachemanager.h"
#else
# error "require module annotcache"
#endif // WITH_MODULE_ANNOTCACHE
#ifdef WITH_MODULE_MRLRESOLVER
# include "module/mrlresolver/luamrlresolver.h"
#else
# error "require module mrlresolver"
#endif // WITH_MODULE_MRLRESOLVER
#ifdef WITH_MODULE_MRLANALYSIS
# include "module/mrlanalysis/mrlanalysis.h"
#endif // WITH_MODULE_MRLANALYSIS
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

//#define DEBUG "annotationdownloader"
#include "module/debug/debug.h"

#define RequestUrlAttribute   QNetworkRequest::UserMax
#define RequestTitleAttribute QNetworkRequest::User

// - Constructions -

AnnotationDownloader::AnnotationDownloader(QObject *parent)
  : Base(parent)
{
  resolver_ = new LuaMrlResolver(this);
  connect(resolver_, SIGNAL(message(QString)), SIGNAL(message(QString)));
  connect(resolver_, SIGNAL(errorMessage(QString)), SIGNAL(errorMessage(QString)));
  connect(resolver_, SIGNAL(subtitleResolved(QString,QString,QString)), SLOT(download(QString,QString,QString)));

  nam_ = new QNetworkAccessManager(this);
  connect(nam_, SIGNAL(finished(QNetworkReply*)), SLOT(processReply(QNetworkReply*)));
}

QString
AnnotationDownloader::hashFileName(const QString &refurl, const QString &title)
{
  QString ret = AnnotationCacheManager::hashFileName(refurl);
  if (ret.isEmpty())
    return ret;
  if (!title.isEmpty())
    ret.prepend('.').prepend(QtExt::escapeFileName(title));
  return ret;
}

// - Request -

void
AnnotationDownloader::download(const QString &url)
{
  DOUT("enter: url =" << url);
  emit message(tr("analyzing") + ": " + url);
#ifdef WITH_MODULE_MRLANALYSIS
  int site = MrlAnalysis::matchSite(url);
  if (site && site < MrlAnalysis::AnnotationSite)
#endif // WITH_MODULE_MRLANALYSIS
  resolver_->resolveSubtitle(url);
  DOUT("exit");
}

void
AnnotationDownloader::download(const QString &url, const QString &refurl, const QString &title)
{
  DOUT("enter: url =" << url << ", refurl =" << refurl << ", title =" << title);
  if (url.isEmpty() || refurl.isEmpty() || title.isEmpty()) {
    DOUT("exit: empty url or refurl or title");
    return;
  }

  QString fileName = QUrl(url).toLocalFile();
  if (!fileName.isEmpty()) {
    saveFile(fileName, refurl, title);
    DOUT("exit: after saveFile");
    return;
  }

  emit message(tr("downloading") + ": " + title + " | " + url);

  QNetworkReply *reply = nam_->get(QNetworkRequest(url));
  auto p = QtExt::PublicNetworkReply::fromReply(reply);
  p->setAttribute(RequestUrlAttribute, refurl);
  p->setAttribute(RequestTitleAttribute, title);
  DOUT("exit");
}


void
AnnotationDownloader::saveFile(const QString &fileName, const QString &refurl, const QString &title)
{
  QFile file(fileName);
  if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
    emit errorMessage(tr("network error, failed to resolve URL") + ": " + refurl);
    return;
  }
  emit message(tr("saving") + ": " + title + " | " + refurl);
  QByteArray data = file.readAll();
  file.close();
  saveData(data, refurl, title);
}

// - Download -

void
AnnotationDownloader::processReply(QNetworkReply *reply)
{
  DOUT("enter");
  Q_ASSERT(reply);
  reply->deleteLater();
  QString url = reply->url().toString();
  if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
    emit errorMessage(tr("network error, failed to resolve URL") + ": " + url);
    DOUT("exit: network error:" << reply->errorString());
    return;
  }

  QString refurl = reply->attribute(RequestUrlAttribute).toString(),
          title = reply->attribute(RequestTitleAttribute).toString();
  if (refurl.isEmpty() || title.isEmpty()) {
    emit errorMessage(tr("failed to resolve URL") + ": " + url);
    DOUT("exit: invalid attribute");
    return;
  }

  QByteArray data = reply->readAll();
#ifdef WITH_MODULE_COMPRESS
  if (!data.isEmpty() && url.contains(".bilibili.tv/", Qt::CaseInsensitive)) {
    QByteArray unzipped = ::gHttpUncompress(data);
    if (!unzipped.isEmpty())
      data = unzipped;
  }
#endif // WITH_MODULE_COMPRESS
  if (data.isEmpty()) {
    emit errorMessage(tr("failed to resolve URL") + ": " + url);
    DOUT("exit: empty data");
    return;
  }
  saveData(data, refurl, title);
  DOUT("exit: after saveData");
}

void
AnnotationDownloader::saveData(const QByteArray &data, const QString &refurl, const QString &title)
{
  DOUT("enter: title =" << title << "refurl =" << refurl);
  if (data.isEmpty()) {
    emit errorMessage(tr("network error, failed to resolve URL") + ": " + refurl);
    DOUT("exit: empty data");
    return;
  }

  QString fileName = hashFileName(refurl, title);
  if (fileName.isEmpty()) {
    emit errorMessage(tr("failed to resolve URL") + ": " + refurl);
    DOUT("exit: failed to hash file name");
    return;
  }
  if (!downloadsLocation_.isEmpty())
    fileName.prepend(QDir::separator()).prepend(downloadsLocation_);
  DOUT("fileName =" << fileName);

  if (AnnotationCacheManager::globalInstance()->saveData(data, refurl)) {
    QString cache = AnnotationCacheManager::globalInstance()->findFile(refurl);
    if (!cache.isEmpty()) {
      QtExt::trashOrRemoveFile(fileName);
      if (QFile::copy(cache, fileName)) {
        emit fileSaved(fileName);
        emit message(tr("file saved") + ": " + fileName);
      } else
        emit errorMessage(tr("failed to save file") + ": " + fileName);
      DOUT("exit: from cache branch");
      return;
    }
  }

  emit warning(tr("failed to cache annotation from URL") + ": " + refurl);
  QFile f(fileName);
  if (!f.open(QIODevice::WriteOnly)) {
    f.write(data);
    if (f.error() == QFile::NoError) {
      emit fileSaved(fileName);
      emit message(tr("file saved") + ": " + fileName);
      DOUT("exit: failed to cache URL:" << refurl);
      return;
    }
  }
  emit errorMessage(tr("failed to save file") + ": " + fileName);
  DOUT("exit: failed to save file:" << fileName);
}

// EOF
