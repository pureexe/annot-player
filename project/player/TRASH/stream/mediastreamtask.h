#ifndef MEDIASTREAMTASK_H
#define MEDIASTREAMTASK_H

// mediastreamtask.h
// 2/16/2016

#include "mediastreamer.h"
#include <QRunnable>

class InputStreamList;

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)

class MediaStreamTask : public QObject, public QRunnable
{
  Q_OBJECT
  typedef MediaStreamTask Self;
  typedef QObject Base;

  MediaStreamInfo msi_;
  QNetworkAccessManager *nam_;
  InputStreamList *ins_;

public:
  explicit MediaStreamTask(const MediaStreamInfo &msi, QNetworkAccessManager *nam = 0,
                           QObject *parent = 0)
    : Base(parent), msi_(msi), nam_(nam), ins_(0) {  }

  ~MediaStreamTask() { stop(); }

public slots:
  void stop();
  virtual void run(); ///< \override

private:
  void debug();
};

#endif // MEDIASTREAMTASK_H
