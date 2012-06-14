// metacallfilter.cc
// 4/9/2012
#include "module/metacall/metacallfilter.h"
#include "module/metacall/metacallevent.h"
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtCore/QEvent>
#include <QtCore/QEventLoop>
#include <memory>
#include <utility>

#define DEBUG "metacallfilter"
#include "module/debug/debug.h"

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses
#endif // __GNUC__

#ifdef Q_OS_MAC
#  define unique_ptr  auto_ptr  // as llvm does not support c++0x at the moment
#endif // Q_OS_MAC

// - Construction -

void
MetaCallFilter::setWatchedObject(QObject *watched)
{
  DOUT("enter: watched_ =" << watched_);
  if (watched_)
    watched_->removeEventFilter(this);
  watched_ = watched;
  if (watched_)
    watched_->installEventFilter(this);
  DOUT("exit: watched_ =" << watched_);
}

bool
MetaCallFilter::eventFilter(QObject *watched, QEvent *e)
{
  DOUT("enter: event type =" << e->type());
  Q_ASSERT(e);
  if (watched_ != watched || e->type() !=  QEvent::MetaCall || !running_) {
    DOUT("exit: not running");
    return QObject::eventFilter(watched, e);
  }

  Q_ASSERT(socket_);
  sendMetaEvent(static_cast<QMetaCallEvent *>(e));
  DOUT("exit: ret = true");
  return true;
}

// - Start -

bool
MetaCallFilter::isActive() const
{
  return running_ && (
    server_ && server_->isListening() ||
    socket_ && socket_->state() == QAbstractSocket::ConnectedState
   );
}

void
MetaCallFilter::stop()
{
  if (socket_) {
    if (socket_->isOpen())
      socket_->close();
    if (!server_)
      delete socket_;
    socket_ = 0;
  }
  if (server_) {
    if (server_->isListening())
      server_->close();
    delete server_;
    server_ = 0;
  }
  running_ = false;
}

bool
MetaCallFilter::startServer(const QHostAddress &address, int port)
{
  DOUT("enter: address =" << address.toString() << ", port =" << port);
  if (running_) {
    DOUT("exit: ret = false, already started");
    return false;
  }

  stop();

  server_ = new QTcpServer(this);
  running_ = server_->listen(address, port);
  if (running_) {
    DOUT("server.connect: address =" << server_->serverAddress() << ", port =" << server_->serverPort());
    connect(server_, SIGNAL(newConnection()), SLOT(acceptConnection()));
  } else
    DOUT("error =" << server_->errorString());
  DOUT("exit: ret =" << running_);
  return running_;
}

bool
MetaCallFilter::startClient(const QHostAddress &address, int port, bool async)
{
  DOUT("enter: address =" << address.toString() << ", port =" << port);
  if (isActive()) {
    DOUT("exit: ret = false, already started");
    return false;
  }

  stop();

  socket_ = new QTcpSocket(this);
  connect(socket_, SIGNAL(readyRead()), SLOT(readSocket()));
  connect(socket_, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(dumpSocket()));
#ifdef DEBUG
  connect(socket_, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(dumpSocket()));
#endif // DEBUG

  socket_->connectToHost(address, port);
  running_ = true;
  if (!async &&
      socket_->state() != QAbstractSocket::ConnectedState &&
      socket_->state() != QAbstractSocket::UnconnectedState) {
    QEventLoop l;
    connect(socket_, SIGNAL(stateChanged(QAbstractSocket::SocketState)), &l, SLOT(quit()));
    connect(socket_, SIGNAL(error(QAbstractSocket::SocketError)), &l, SLOT(quit()));
    do l.exec();
    while (socket_->state() == QAbstractSocket::HostLookupState ||
           socket_->state() == QAbstractSocket::ConnectingState);
  }
  DOUT("exit: ret =" << running_);
  return running_;
}

// - Communication -

void
MetaCallFilter::sendMetaEvent(const QMetaCallEvent *m)
{
  DOUT("enter");
  Q_ASSERT(m);
  Q_ASSERT(socket_);
  if (!socket_)
    return;
  DOUT("enter");

  QByteArray message;
  QDataStream out(&message, QIODevice::WriteOnly);

  out << message_size_t(0) // space holder for message size
      << m->id()
      << m->nargs();

  bool ok = true;
  for (int i = 1; i < m->nargs() && ok; i++) {
    out << m->types()[i];
    ok = QMetaType::save(out, m->types()[i], m->args()[i]);
    if (!ok)
      DOUT("warning: unregisted metatype");
  }

  if (ok) {
    out.device()->seek(0);
    out << message_size_t(message.size());
    socket_->write(message);
  }
  DOUT("exit: ok =" << ok);
}

void
MetaCallFilter::acceptConnection()
{
  DOUT("enter");
  if (socket_) {
    if (socket_->isOpen())
      socket_->close();
    delete socket_;
  }
  socket_ = server_->nextPendingConnection();
  dumpSocket();
  if(socket_) {
    connect(socket_, SIGNAL(readyRead()), SLOT(readSocket()));
    connect(socket_, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(dumpSocket()));
#ifdef DEBUG
    connect(socket_, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(dumpSocket()));
#endif // DEBUG
  }
  DOUT("exit: ok =" << bool(socket_));
}

void
MetaCallFilter::readSocket()
{
  DOUT("enter");
  Q_ASSERT(socket_);
  if (!socket_) {
    DOUT("socket is null");
    return;
  }

  QDataStream in(socket_);
  if (!messageSize_) { // Save messageSize_ since none-blocking read is used
    if (socket_->bytesAvailable() < int(sizeof(message_size_t))) {
      DOUT("exit: insufficient messageSize");
      return;
    }
    in >> messageSize_;
  }

  if (socket_->bytesAvailable() < int(messageSize_ - sizeof(message_size_t))) {
    DOUT("exit: insufficient messageSize");
    return;
  }
  DOUT("messageSize =" << messageSize_);

  messageSize_ = 0;

  int m_id;
  in >> m_id;
  DOUT("m_id =" << m_id);

  int m_nargs;
  in >> m_nargs;
  DOUT("m_nargs =" << m_nargs);
  Q_ASSERT(m_nargs > 0);
  if (m_nargs <= 0) {
    DOUT("exit: error, invalid m_nargs =" << m_nargs);
    return;
  }

  int *m_types = new int[m_nargs];
  std::unique_ptr<int> autorelease_types(m_types);

  void **m_args = new void *[m_nargs];
  std::unique_ptr<void *> autorelease_args(m_args);
  m_args[0] = 0;

  bool ok = true;
  for (int i = 1; i < m_nargs && ok; i++) {
    in >> m_types[i];
    m_args[i] = QMetaType::construct(m_types[i]);

    ok = QMetaType::load(in, m_types[i], m_args[i]) && ok;
  }
  if (!ok)
    DOUT("warning: unregisted metatype");

  Q_ASSERT(watched_);
  if (ok && watched_) {
    // See: QObject::event and QQMetaCallEvent implementation in in qobject.cpp
    // Use macros is case QT_NO_EXCEPTIONS is defined
    QT_TRY {
      DOUT("placeMetaCall");
      QMetaObject::metacall(watched_, QMetaObject::InvokeMetaMethod, m_id, m_args);
    } QT_CATCH(...) {
      DOUT("destroy meta types");
      for (int i = 0; i < m_nargs; ++i)
       if (m_types[i] && m_args[i])
         QMetaType::destroy(m_types[i], m_args[i]);
      QT_RETHROW;
    }
  }
  DOUT("exit: ok =" << ok);
}

// - Debug -

void
MetaCallFilter::dumpSocket()
{
  if (socket_)
    DOUT("socket"
         ": localAddress ="<< socket_->localAddress() <<
         ", localPort ="   << socket_->localPort() <<
         ", peerAddress =" << socket_->peerAddress() <<
         ", peerPort ="    << socket_->peerPort() <<
         ", state ="       << socket_->state() <<
         ", error ="       << socket_->errorString());
  else
    DOUT("socket = null");
}
// EOF
