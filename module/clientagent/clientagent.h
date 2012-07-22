#ifndef CLIENTAGENT_H
#define CLIENTAGENT_H

// module/clientagent/clientagent.h
// 10/6/2011
// Publish client side ClientAgentService for server callback.
// See: http://blog.csdn.net/tingsking18/article/details/5456831
// See: http://hi.baidu.com/2sky2sea/blog/item/40ec5555680279c1b745ae9b.html
//
// Note: since soap timeout is not slow, synchronized approach is used.
// TODO: Deal with offline case here??

#include "clientdelegate.h"
#include "module/annotcloud/user.h"
#include "module/annotcloud/token.h"
#include "module/annotcloud/annotation.h"
#include <QtCore/QObject>

class ClientService;
class ServerAgent;

class ClientAgent: public QObject, public ClientDelegate
{
  Q_OBJECT
  Q_DISABLE_COPY(ClientAgent)
  typedef ClientAgent Self;
  typedef QObject Base;

  typedef Core::Universe::User User;
  typedef Core::Universe::Token Token;
  typedef Core::Universe::TokenList TokenList;
  typedef Core::Universe::Annotation Annotation;
  typedef Core::Universe::AnnotationList AnnotationList;

  ClientService *service_;
  bool authorized_;
  bool connected_;

  long key_;

#ifdef WITH_MODULE_SERVERAGENT
  ServerAgent *server_;
#endif // WITH_MODULE_SERVERAGENT

public:
  explicit ClientAgent(QObject *parent = nullptr);

#ifdef WITH_MODULE_SERVERAGENT
  void setServerAgent(ServerAgent *server);
#endif // WITH_MODULE_SERVERAGENT

  // - Properties -
public:

  long privateKey() const;
  void setPrivateKey(long key);
  void randomizePrivateKey();

  long nextPublicKey() const; ///< get next possible public key

  int port() const;
protected:
  void randomizeServicePort();
  void setPort(int port);

  // - Service -
public:
  bool isReady() const;

  // - Signals -
signals:
  void authorized();
  void deauthorized();
  void authorizationError();
  void chatReceived(const QString text);

  // - Constrols -
public slots:
  void setAuthorized(bool t);
  void setConnected(bool t);

  // - Implementations -
public:
  bool authorize(long key) override;
  void deauthorize() override;
  bool isAuthorized() const override;
  bool isConnected() const override;
  void chat(const QString &text) override;
};

#endif // CLIENTAGENT_H
