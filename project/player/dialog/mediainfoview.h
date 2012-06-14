#ifndef MEDIAINFOVIEW_H
#define MEDIAINFOVIEW_H

// mediainfoview.h
// 4/10/2012:%s

#include "infoview.h"
#include <QtCore/QString>

class Player;
class DataManager;
class SignalHub;

class MediaInfoView : public InfoView
{
  Q_OBJECT
  Q_DISABLE_COPY(MediaInfoView)
  typedef MediaInfoView Self;
  typedef InfoView Base;

  Player *player_;
  DataManager *data_;
  SignalHub *hub_;

public:
  MediaInfoView(Player *player, DataManager *data, SignalHub *hub, QWidget *parent = 0);

public slots:
  void updateText(); ///< \reimp
};

#endif // MEDIAINFOVIEW_H
