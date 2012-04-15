#ifndef TRANSLATOR_H
#define TRANSLATOR_H

// translator.h
// 11/2/2011

#include <QObject>
#include <QUrl>

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class Translator : public QObject
{
  Q_OBJECT
  typedef Translator Self;
  typedef QObject Base;

  // - Constructions -
public:
  static const char *English;
  static const char *Japanese;
  static const char *Korean;
  static const char *SimplifiedChinese;
  static const char *TraditionalChinese;

public:
  explicit Translator(QObject *parent = 0);

signals:
  void networkError(const QString &reason);
  void translated(const QString &text); ///< Requested translation received

public slots:
  ///  Request translation. Autodetect source language when \param from is empty.
  void translate(const QString &text, const QString &to, const QString &from = QString::null) const;

protected:
  static QUrl translationQuery(const QString &text, const QString &to, const QString &from = QString::null);
protected slots:
  void processNetworkReply(QNetworkReply *reply);

private:
  QNetworkAccessManager *qnam_;
};

#endif // TRANSLATOR_H
