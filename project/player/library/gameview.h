#ifndef GAMEVIEW_H
#define GAMEVIEW_H

// gameview.h
// 8/18/2012

#include "game.h"
#include "project/common/acmainwindow.h"

QT_FORWARD_DECLARE_CLASS(QLabel)

class GameLibrary;
class GameView : public AcMainWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(GameView)
  typedef GameView Self;
  typedef AcMainWindow Base;

  QLabel *encodingLabel_, *functionLabel_;

  GameLibrary *library_;
  QString digest_;
  Game game_;

  // - Construction -
public:
  explicit GameView(GameLibrary *lib, QWidget *parent = nullptr);

  // - Properties -
public slots:
  void setDigest(const QString &digest);

protected:
  void updateGame();
  void updateLabels();
  void updateTitle();

protected slots:
  void refresh();

private:
  void createLayout();
};

#endif // GAMEVIEW_H
