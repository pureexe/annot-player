#ifndef YAHOOJLPHIGHLIGHTER_H
#define YAHOOJLPHIGHLIGHTER_H

// 9/8/2012
// yahoojlphighlighter.h

#include <QtGui/QSyntaxHighlighter>

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class YahooJlpHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT
  Q_DISABLE_COPY(YahooJlpHighlighter)
  typedef YahooJlpHighlighter Self;
  typedef QSyntaxHighlighter Base;

  QNetworkAccessManager *nam_;
  QNetworkReply *reply_;
  bool sync_;

  // - Construction -
public:
  explicit YahooJlpHighlighter(QObject *parent)
    : Base(parent), nam_(nullptr), reply_(nullptr), sync_(true) { }
  explicit YahooJlpHighlighter(QTextDocument *parent)
    : Base(parent), nam_(nullptr), reply_(nullptr), sync_(true) { }
  explicit YahooJlpHighlighter(QTextEdit *parent)
    : Base(parent), nam_(nullptr), reply_(nullptr), sync_(true) { }

signals:
  void errorMessage(const QString &t);

  // - Properties -
public:
  bool isSynchronized() const { return sync_; }
  void setSynchronized(bool t) { sync_ = t; }
protected:
   QNetworkAccessManager *networkAccessManager() const;

  // - Actions -
protected:
  QNetworkReply *createReply(const QString &text);
  void highlightBlock(const QString &text) override;

protected slots:
  void processReply(QNetworkReply *reply);
};

#endif // YAHOOJLPHIGHLIGHTER_H
