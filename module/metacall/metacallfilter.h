#ifndef METACALLFILTER_H
#define METACALLFILTER_H

// 4/9/2012
// metacallfilter.h

#include <QtNetwork/QAbstractSocket>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE
class QMetaCallEvent;
class QHostAddress;
class QTcpServer;
class QTcpSocket;
QT_END_NAMESPACE

/**
 *  All signals in this class will be propagated to remote object.
 *  DO NOT USE LOCAL SIGNALS
 */
class MetaCallFilter : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(MetaCallFilter)
  typedef MetaCallFilter Self;
  typedef QObject Base;

  // - Construction -
public:
  explicit MetaCallFilter(QObject *parent = nullptr)
    : Base(parent), running_(false), watched_(0), server_(nullptr), socket_(nullptr), messageSize_(0)
  { }

public:
  //bool isRunning() const { return running_; }
  bool isActive() const;

  QObject *watchedObject() const { return watched_; }
  void setWatchedObject(QObject *watched);

  bool startServer(const QHostAddress &address, int port);
  bool startClient(const QHostAddress &address, int port, bool async = true);

  bool isServer() const { return server_; }
  bool isClient() const { return !server_ && socket_; }

public slots:
  void stop();

  // - Implementation -
public:
  bool eventFilter(QObject *watched, QEvent *e) override;
private slots:
  void acceptConnection(); ///< server creates connection
  void readSocket();  ///< receive and invoke meta call from connected socket
  void sendMetaEvent(const QMetaCallEvent *e);

  void dumpSocket(); ///< only for debug
private:
  bool running_;
  QObject *watched_;

  QTcpServer *server_; // for receiver
  QTcpSocket *socket_; // for both sender and receiver

  typedef quint16 message_size_t;
  message_size_t messageSize_;
};

#endif // METACALLFILTER_H
