#ifndef MRLRESOLVER_H
#define MRLRESOLVER_H

// mrlresolver.h
// 1/24/2011

#include <QObject>
#include <QStringList>

class MrlResolver : public QObject
{
  Q_OBJECT
  typedef MrlResolver Self;
  typedef QObject Base;

public:
  explicit MrlResolver(QObject *parent = 0)
    : Base(parent) { }

signals:
  void errorReceived(QString message);
  void messageReceived(QString message);

  void resolved(QStringList mrls, QString href, QString title);

public:
  virtual bool match(const QString &href) const = 0;
public slots:
  virtual void resolve(const QString &href) = 0;
};

#endif // MRLRESOLVER_H
