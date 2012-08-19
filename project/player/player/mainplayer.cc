// mainplayer.cc
// 6/30/2011

#include "mainplayer.h"
#include "positionslider.h"
#include "module/qtext/ss.h"
#include "module/qtext/overlaylayout.h"
#include <QtGui>

//#define VOLUMN_SLIDE_MAX_WIDTH 30
enum { PLAY_BUTTON_SIZE = 30 };

// - Construction -

MainPlayerUi::MainPlayerUi(SignalHub *hub, Player *player, AnnotationServerAgent *server, QWidget *parent)
  : Base(hub, player, server, parent)
{
  setContentsMargins(0, 0, 0, 0);
  createLayout();
  setTabOrder(inputComboBox(), prefixComboBox());
}

void
MainPlayerUi::createLayout()
{
  playButton()->setProperty("radius", int(PLAY_BUTTON_SIZE));

  //volumeSlider()->setMaximumWidth(VOLUMN_SLIDE_MAX_WIDTH);
  // Set layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *row1 = new QHBoxLayout,
                *row2 = new QHBoxLayout;

    rows->addWidget(positionSlider());
    rows->addLayout(row1);
    rows->addLayout(row2);

    //row1->addWidget(menuButton());
    row1->addWidget(openButton());
    row1->addWidget(playButton());
    //row1->addWidget(toggleAnnotationButton());
    row1->addWidget(nextFrameButton());
    row1->addWidget(fastForwardButton());
    //row1->addWidget(fastFastForwardButton());
    row1->addWidget(stopButton());
    row1->addWidget(previousButton());
    row1->addWidget(nextButton());
    row1->addWidget(toggleEmbedModeButton());
    row1->addWidget(toggleMiniModeButton());
    row1->addWidget(toggleFullScreenModeButton());
    row1->addWidget(toggleTraceWindowButton());
    row1->addStretch();
    row1->addWidget(volumeSlider());
    row1->addWidget(positionButton());
    row1->addWidget(libraryButton());

    row2->addWidget(userButton());
    row2->addWidget(prefixComboBox());

    OverlayLayout *input = new OverlayLayout;
    input->addWidget(inputComboBox());
    input->addWidget(inputCountButton(), Qt::AlignRight);
    row2->addLayout(input);

    row2->addWidget(networkButton());

    // margins (px): left, top, right, down
    rows->setContentsMargins(9, 0, 9, 0);
    row1->setContentsMargins(0, 0, 0, 1);
    row2->setContentsMargins(0, 1, 0, 9);
  }
  setLayout(rows);
  menuButton()->hide();
  menuButton()->resize(QSize());
  progressButton()->hide();
  progressButton()->resize(QSize());
  //fastFastForwardButton()->hide();
  //fastFastForwardButton()->resize(QSize());
#ifndef WITH_WIN_PICKER
  toggleTraceWindowButton()->hide();
  toggleTraceWindowButton()->resize(QSize());
#endif // WITH_WIN_PICKER
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  setStyleSheet(styleSheet() +
    SS_BEGIN(QToolButton)
      SS_NO_BORDER
    SS_END
  );
}

// EOF
