// metacallfilter.cc
// 4/9/2012
#include "metacallfilter.h"
#include "metacallevent.h"
#include <QtCore>
#include <QtNetwork>
#include <utility>

#define DEBUG "metacallfilter"
#include "module/debug/debug.h"

// - Event -

bool
MetaCallFilter::eventFilter(QObject *watched, QEvent *e)
{
  Q_ASSERT(e);
  Q_ASSERT(socket_);
  if (watched_ != watched || e->type() !=  QEvent::MetaCall || !running_)
    return QObject::eventFilter(watched, e);

  sendMetaEvent(static_cast<QMetaCallEvent *>(e));
  return true;
}

// - Start -

void
MetaCallFilter::stop()
{
  if (server_)
    server_->close();
  if (socket_)
    socket_->close();
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

  server_ = new QTcpServer(this);
  running_ = server_->listen(address, port);
  if (running_) {
    DOUT("server.connect: address =" << server_->serverAddress() << ", port =" << server_->serverPort());
    connect(server_, SIGNAL(newConnection()), SLOT(acceptConnection()));
  } else
    DOUT("error =" << server_->errorString());
  DOUT("exit: ret =" << running_);
  return ok;
}

bool
MetaCallFilter::startClient(const QHostAddress &address, int port)
{
  DOUT("enter: address =" << address.toString() << ", port =" << port);
  if (running_) {
    DOUT("exit: ret = false, already started");
    return false;
  }

  socket_ = new QTcpSocket(this);
  connect(socket_, SIGNAL(readyRead()), SLOT(readSocket()));
  connect(socket_, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(dumpSocket()));
#ifdef DEBUG
  connect(socket_, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(dumpSocket()));
#endif // DEBUG

  socket_->connectToHost(address, port);
  dumpSocket();

  running_ = true;
  DOUT("exit: ret =" << running_);
  return running_;
}

// - Communication -

void
MetaCallFilter::sendMetaEvent(const QMetaCallEvent *m)
{
  Q_ASSERT(m);
  Q_ASSERT(socket_);
  if (!socket_)
    return;
  DOUT("enter");

  QByteArray message;
  QDataStream out(&message, QIODevice::WriteOnly);

  out << (message_size_t)0 // space holder for message size
#ifdef MCE_HAS_STATIC_CALL
      << m->methodOffset()
      << m->methodRelative()
#else
      << m->id()
#endif // MCE_HAS_STATIC_CALL
      << m->signalId()
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
    out << (message_size_t)message.size();
    socket_->write(message);
  }
  DOUT("exit: ok =" << ok);
}

void
MetaCallFilter::acceptConnection()
{
  DOUT("enter");
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
    if (socket_->bytesAvailable() < sizeof(message_size_t)) {
      DOUT("exit: insufficient messageSize");
      return;
    }
    in >> messageSize_;
  }

  if (socket_->bytesAvailable() < messageSize_ - sizeof(message_size_t)) {
    DOUT("exit: insufficient messageSize");
    return;
  }

  messageSize_ = 0;

#ifdef MCE_HAS_STATIC_CALL
  ushort m_methodOffset;
  in >> m_methodOffset;
  ushort m_methodRelative;
  in >> m_methodRelative;
#else
  int m_id;
  in >> m_id;
#endif // MCE_HAS_STATIC_CALL
  int m_signalId;
  in >> m_signalId;

  int m_nargs;
  in >> m_nargs;
  Q_ASSERT(m_nargs > 0);
  if (m_nargs <= 0) {
    DOUT("exit: error, invalid m_nargs =" << m_nargs);
    return;
  }

  // Use auto-release pool to prevent memory leak on exceptions
  int *m_types = new int[m_nargs];  // FIXME: never released
  std::auto_ptr<int> autorelease_types(m_types);
  Q_UNUSED(autorelease_types);

  void **m_args = new void*[m_nargs]; // FIXME: never released
  std::auto_ptr<void *> autorelease_args(m_args);
  Q_UNUSED(autorelease_args);

  m_args[0] = 0;


  bool ok = true;
  for (int i = 1; i < m_nargs && ok; i++) {
    in >> m_types[i];
    m_args[i] = QMetaType::construct(m_types[i]);

    ok = QMetaType::load(in, m_types[i], m_args[i]);
    if (!ok)
      DOUT("warning: unregisted metatype");
  }
  if (ok) {
#ifdef MCE_HAS_STATIC_CALL
    QMetaCallEvent m(m_methodOffset, m_methodRelative, 0, this, m_signalId, m_nargs, m_types, m_args);
#else
    QMetaCallEvent m(m_id, this, m_signalId, m_nargs, m_types, m_args);
#endif // MCE_HAS_STATIC_CALL
    DOUT("placeMetaCall");
    m.placeMetaCall(this); // might throw exceptions
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
