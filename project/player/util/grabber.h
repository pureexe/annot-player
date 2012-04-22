#ifndef GRABBER_H
#define GRABBER_H

// grabber.h
// 10/20/2011

#include <QtGui/QWidget>

class Grabber : public QObject
{
  Q_OBJECT
  typedef Grabber Self;
  typedef QObject Base;

public:
  explicit Grabber(QObject *parent = 0);

public:
  const QString &baseName() const;
  const QString &savePath() const;

public slots:
  void setBaseName(const QString &name);
  void setSavePath(const QString &path);

  void grabWindow(WId winid);
  void grabDesktop();

private:
  QString savePath_;
  QString baseName_;
};

#endif // GRABBER_H
