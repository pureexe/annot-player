#ifndef YAHOOJLPPARSER_H
#define YAHOOJLPPARSER_H

// 9/8/2012
// yahoojlpparser.h

#include <QtCore/QObject>

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class YahooJlpParser : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(YahooJlpParser)
  typedef YahooJlpParser Self;
  typedef QObject Base;

  QNetworkAccessManager *nam_;
  QNetworkReply *reply_;
  bool sync_;

  // - Construction -
public:
  explicit YahooJlpParser(QObject *parent = nullptr)
    : Base(parent), nam_(nullptr), reply_(nullptr), sync_(true) { }

signals:
  void errorMessage(const QString &t);
  void renderedText(const QString &t);

  // - Properties -
public:
  bool isSynchronized() const { return sync_; }
  void setSynchronized(bool t) { sync_ = t; }
protected:
   QNetworkAccessManager *networkAccessManager() const;

  // - Actions -
public slots:
  void renderText(const QString &text);
protected:
  QNetworkReply *createReply(const QString &text);
protected slots:
  void processReply(QNetworkReply *reply);
};

#endif // YAHOOJLPPARSER_H
