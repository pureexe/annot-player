#ifndef ANNOTATIONDOWNLOADER_H
#define ANNOTATIONDOWNLOADER_H

// annotationdownloader.h
// 7/6/2012

#include <QtCore/QObject>
#include <QtCore/QString>

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class MrlResolver;
class AnnotationDownloader : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationDownloader)
  typedef AnnotationDownloader Self;
  typedef QObject Base;

  QString downloadsLocation_;
  MrlResolver *resolver_;
  QNetworkAccessManager *nam_;

  // - Construction -
public:
  explicit AnnotationDownloader(QObject *parent = 0);

  void setDownloadsLocation(const QString &path) { downloadsLocation_ = path; }

  QString downloadsLocation() const { return downloadsLocation_; }

signals:
  void error(QString msg);
  void warning(QString msg);
  void message(QString msg);

public slots:
  void download(const QString &refurl);
  void download(const QString &url, const QString &refurl, const QString &title);

protected slots:
  void processReply(QNetworkReply *reply);
protected:
  void saveData(const QByteArray &data, const QString &refurl, const QString &title);
  void saveFile(const QString &fileName, const QString &refurl, const QString &title);
  static QString hashFileName(const QString &refurl, const QString &title);
};

#endif // ANNOTATIONDOWNLOADER_H
