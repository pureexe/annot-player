#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

// messagehandler.h
// 10/19/2011

#include "processinfo.h"
#include <QObject>
#include <QStringList>
#include <QList>
#include <QVector>

class MessageHandler : public QObject
{
  Q_OBJECT
  typedef MessageHandler Self;
  typedef QObject Base;

public:
  struct MessageHash {
    qint64 hash;
    int count;

    explicit MessageHash(qint64 h = 0, int c = 0)
      : hash(h), count(c) { }

    void clear() { hash = 0; count = 0; }
    bool isValid() const { return hash && count; }
  };

public:
  explicit MessageHandler(QObject *parent = 0);

signals:
  void messageReceived(qint64 pos);

  // - Properties -
public:
  bool isActive() const;
  int hookId() const;
  const ProcessInfo &processInfo() const;

  ///  Most recent on the top.
  const QStringList &recentMessages() const;
  int recentMessageCapacity() const;

  const MessageHash &lastMessageHash() const;

public slots:
  void setActive(bool active);
  void setHookId(int hid);
  void setProcessInfo(const ProcessInfo &pi);

  void clearRecentMessages();

  // - Actions
protected slots:
  void processTextMessage(const QString &text, int hookId, qint64 tsMsecs);

  void connectTextHook();
  void disconnectTextHook();

  // - Implementations -
protected:
  static bool isSubtitle(const QString &text);

private:
  bool active_;
  int hookId_;
  ProcessInfo pi_;

  QStringList messages_; // recent messages queue
  int messageCount_;     // recent message capacity

  MessageHash lastMessageHash_;
};

#endif // MESSAGEHANDLER_H
