#ifndef EMBEDDEDINFOVIEW_H
#define EMBEDDEDINFOVIEW_H

// embeddedinfoview.h
// 4/10/2012:%s

#include <QtGui/QLabel>

class Player;
class DataManager;
class SignalHub;
class AnnotationGraphicsView;

class EmbeddedInfoView : public QLabel
{
  Q_OBJECT
  typedef EmbeddedInfoView Self;
  typedef QLabel Base;

  Player *player_;
  DataManager *data_;
  AnnotationGraphicsView *annot_;
  SignalHub *hub_;

public:
  EmbeddedInfoView(Player *player, DataManager *data, AnnotationGraphicsView *annot, SignalHub *hub, QWidget *parent = 0);

public slots:
  virtual void setVisible(bool t); ///< \override
  void refresh() { updateText(); }
  void setInvisible(bool t) { setVisible(!t); }

protected slots:
  void updateText(); ///< \override

protected:
  QString timeToString(qint64 secs) const;
};

#endif // EMBEDDEDINFOVIEW_H
