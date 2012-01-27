#ifndef MRLRESOLVERMANAGER_H
#define MRLRESOLVERMANAGER_H

// mrlresolvermanager.h
// 1/25/2011

#include <QObject>
#include <QStringList>
#include <QList>

class MrlResolver;

class MrlResolverManager : public QObject
{
  Q_OBJECT
  typedef MrlResolverManager Self;
  typedef QObject Base;

  QList<MrlResolver*> resolvers_;

public:
  enum Resolver { Youtube = 0, Youku, ResolverCount };

  static Self *globalInstance() { static Self g; return &g; }

protected:
  explicit MrlResolverManager(QObject *parent = 0);

signals:
  void errorReceived(QString message);
  void messageReceived(QString message);

  void resolved(QStringList mrls, QString href, QString title);

public:
  int match(const QString &href) const;

  void resolve(int id, const QString &href);

  bool resolve(const QString &href)
  {
    int r = match(href);
    if (r < 0)
      return false;
    resolve(r, href);
    return true;
  }
};

#endif // MRLRESOLVERMANAGER_H
