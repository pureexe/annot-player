#ifndef CLIENTDELEGATE_H
#define CLIENTDELEGATE_H

// module/clientagent/clientdelegate.h
// 10/6/2011

#include <QString>

class ClientDelegate
{
public:
  virtual ~ClientDelegate() { }

  virtual bool authorize(long key) = 0;

  virtual void deauthorize() = 0;

  virtual bool isAuthorized() const = 0;

  virtual bool isConnected() const = 0;

  virtual void chat(const QString &text) = 0;
};

#endif // CLIENTDELEGATE_H
