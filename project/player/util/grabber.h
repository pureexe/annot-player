#ifndef GRABBER_H
#define GRABBER_H

// grabber.h
// 10/20/2011

#include <QtGui/QWidget>

class Grabber : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(Grabber)
  typedef Grabber Self;
  typedef QObject Base;

public:
  explicit Grabber(QObject *parent = 0);

  QString savePath_;
  QString baseName_;

signals:
  void message(QString);
  void warning(QString);

public:
  const QString &baseName() const { return baseName_; }
  const QString &savePath() const { return savePath_; }

public slots:
  void setBaseName(const QString &name);
  void setSavePath(const QString &path) { savePath_ = path; }

  void grabWindow(WId winid);
  void grabDesktop();
};

#endif // GRABBER_H
