#ifndef SIGNER_H
#define SIGNER_H

// signer.h
// 3/30/2012

#include <QObject>
#include <QMutex>

class ServerAgent;

class Signer : public QObject
{
  Q_OBJECT
  typedef Signer Self;
  typedef QObject Base;

  QMutex m_;
  bool disposed_;
  ServerAgent *server_;

public:
  explicit Signer(QObject *parent = 0);

signals:
  void message(const QString &text);
  void warning(const QString &text);
  void error(const QString &text);

public slots:
  void dispose() { disposed_ = true; }
  void login(bool async = true);
  void signFileWithUrl(const QString &fileName, const QString &url, bool async = true);

protected:
  void login(const QString &userName, const QString &encryptedPassword); // synchronized
  static QString digestForFile(const QString &fileName);
};

#endif // SIGNER_H
