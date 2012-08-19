// mainplayer_dock.cc
// 6/30/2011

#include "mainplayer.h"

// - Dock -

MainPlayerDock::MainPlayerDock(QWidget *parent)
  : Base(parent)
{
  setContentsMargins(0, 0, 0, 0);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  setTitleBarWidget(new QWidget(this));
  setFeatures(QDockWidget::NoDockWidgetFeatures);
}

void
MainPlayerDock::setVisible(bool visible)
{
  if (auto w = qobject_cast<MainPlayerUi *>(widget()))
    w->setActive(visible);

  Base::setVisible(visible);
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
