// mainplayer.cc
// 6/30/2011

#include "mainplayer.h"
#include "stylesheet.h"
#include <QtGui>

// + MainPlayerDock +

MainPlayerDock::MainPlayerDock(QWidget *parent)
  : Base(parent)
{
  setContentsMargins(0, 0, 0, 0);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  setTitleBarWidget(new QWidget(this));
  setFeatures(QDockWidget::NoDockWidgetFeatures);
}

void
MainPlayerDock::setVisible(bool visible)
{
  MainPlayerUi *ui = qobject_cast<MainPlayerUi*>(widget());
  if (ui)
    ui->setActive(visible);

  Base::setVisible(visible);
}

// + MainPlayerUi +

MainPlayerUi::MainPlayerUi(SignalHub *hub, Player *player, ServerAgent *server, QWidget *parent)
  : Base(hub, player, server, parent)
{
  setContentsMargins(0, 0, 0, 0);
  createLayout();
}

void
MainPlayerUi::createLayout()
{
  // Reset Ui style
  prefixLineEdit()->setStyleSheet(SS_PREFIXLINEEDIT_MAIN);
  lineEdit()->setStyleSheet(SS_LINEEDIT_MAIN);

  // Set layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *row1 = new QHBoxLayout,
                *row2 = new QHBoxLayout;

    rows->addWidget(positionSlider());
    rows->addLayout(row1);
    rows->addLayout(row2);

    row1->addWidget(playButton());
    row1->addWidget(toggleAnnotationButton());
    row1->addWidget(nextFrameButton());
    row1->addWidget(stopButton());
    row1->addWidget(openButton());
    row1->addWidget(toggleMiniModeButton());
    row1->addWidget(togglePlayModeButton());
    row1->addStretch(1);
    row1->addWidget(volumeSlider());
    row1->addWidget(positionButton());

    row2->addWidget(userButton());
    row2->addWidget(prefixLineEdit());
    row2->addWidget(lineEdit());

    // margins (px): left, top, right, down
    rows->setContentsMargins(9, 0, 9, 0);
    row1->setContentsMargins(0, 0, 0, 9);
    row2->setContentsMargins(0, 0, 0, 9);
  }
  setLayout(rows);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

// EOF

  /*
  // Comment panel
  QDockWidget *bottomDock = new QDockWidget(this);
  {
    QWidget *bottom = new QWidget(bottomDock);
    QHBoxLayout *row = new QHBoxLayout(bottom);
    row->addWidget(userButton());
    row->addWidget(prefixLineEdit());
    row->addWidget(lineEdit());

    bottom->setLayout(row);
    bottom->setContentsMargins(0, 0, 0, 0);
    //row->setContentsMargins(0, 0, 0, 0);
    bottomDock->setContentsMargins(0, 0, 0, 0);
    bottomDock->setTitleBarWidget(new QWidget(bottomDock));
    bottomDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    bottomDock->setWidget(bottom);
    addDockWidget(Qt::BottomDockWidgetArea, bottomDock);
  }
  */
