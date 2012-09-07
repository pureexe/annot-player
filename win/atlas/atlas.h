#pragma once

// atlas.h
// 9/6/2012

#include <QtCore/QObject>
#include <QtCore/QString>

QT_FORWARD_DECLARE_CLASS(QTextDecoder)
QT_FORWARD_DECLARE_CLASS(QTextEncoder)

#define ATLAS_DOWNLOAD_URL  "http://software.fujitsu.com/jp/atlas/downloads/trial/"
class Atle;
class Atlas : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(Atlas)
  typedef Atlas Self;
  typedef QObject Base;

  Atle *atle_;
  QString location_;

  QTextEncoder *encoder_;
  QTextDecoder *decoder_;

public:
  explicit Atlas(QObject *parent = nullptr);

  QString translate(const QString &text);

signals:
  void locationChanged(const QString &path);

public:
  static bool isValidLocation(const QString &path);
  static QString findLocation();

  void setLocation(const QString &path);
  QString location() { return location_; }

protected:
  Atle *atle();
  QString decodeText(const QByteArray &text) const;
  QByteArray encodeText(const QString &text) const;
};

// EOF
