#ifndef EMBEDDEDINFOVIEW_H
#define EMBEDDEDINFOVIEW_H

// embeddedinfoview.h
// 4/10/2012:%s

#include <QtGui/QLabel>

class Player;
class DataManager;
class SignalHub;

class EmbeddedInfoView : public QLabel
{
  Q_OBJECT
  Q_DISABLE_COPY(EmbeddedInfoView)
  typedef EmbeddedInfoView Self;
  typedef QLabel Base;

  Player *player_;
  DataManager *data_;
  SignalHub *hub_;

public:
  EmbeddedInfoView(Player *player, DataManager *data, SignalHub *hub, QWidget *parent = 0);

public slots:
  virtual void setVisible(bool t); ///< \reimp
  void refresh() { updateText(); }
  void setInvisible(bool t) { setVisible(!t); }

protected slots:
  void updateText(); ///< \reimp

protected:
  static QString timeToString(qint64 secs);
  static QString weekToString(int dayOfWeek);
};

#endif // EMBEDDEDINFOVIEW_H
