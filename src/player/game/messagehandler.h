#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

// messagehandler.h
// 10/19/2011

#include "textmessage.h"
#include "textthread.h"
#include "processinfo.h"
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QSet>

class MessageHandler : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(MessageHandler)
  typedef MessageHandler Self;
  typedef QObject Base;

public:
  struct Context {
    qint64 hash;
    int count;

    explicit Context(qint64 h = 0, int c = 0)
      : hash(h), count(c) { }

    void clear() { hash = 0; count = 0; }
    bool isValid() const { return hash && count; }
  };

public:
  explicit MessageHandler(QObject *parent = nullptr);

signals:
  void textChanged(const QString &text, int role);
  void contextChanged(qint64 h1, qint64 h4);

  void message(QString msg);
  void warning(QString msg);
  void errorMessage(QString msg);
  void notification(QString msg);

  // - Properties -
public:
  bool isActive() const { return active_; }
  const ProcessInfo &processInfo() const { return processInfo_; }
  const TextThreadList &threads() const { return threads_;}
  bool hasThreads() const { return !threads_.isEmpty(); }

  //qint64 signature() const { return signature_; }
  //const QString &provider() const { return provider_; }
  //const QVector<QByteArray> &recentMessages() const { return messages_; }

  //int messageCapacity() const { return messageCount_; }

  const Context &currentContext() const { return context_; }
  qint64 currentHash() const { return hash_; }

public slots:
  void setActive(bool active);
  void setThreads(const TextThreadList &l) { threads_ = l; updateSignatures(); }
  void setProcessInfo(const ProcessInfo &pi) { processInfo_ = pi; }

  void clearMessages() { messages_.clear(); context_.clear(); hash_ = 0; }
  void clear() { clearMessages(); threads_.clear(); processInfo_.clear(); }

  // - Actions
protected slots:
  void processMessage(const QByteArray &data, qint64 signature);

protected:
  void updateSignatures();
  void connectTextHook();
  void disconnectTextHook();

  // - Implementations -
//protected:
//  static bool isSubtitle(const QString &text);

private:
  bool active_;
  ProcessInfo processInfo_;

  TextThreadList threads_;
  qint64 leadingSignature_;
  QSet<qint64> supportSignatures_;

  //QList<TextThread> threads_;
  //QList<TextMessage> messages_; // recent messages queue
  QVector<QByteArray> messages_; // recent messages queue

  qint64 hash_;
  Context context_;
};

#endif // MESSAGEHANDLER_H
