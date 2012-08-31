// gameview.cc
// 8/18/2012

#include "gameview.h"
#include "gamelibrary.h"
#include "tr.h"
#include "global.h"
#include "project/common/acui.h"
#include "module/qtext/layoutwidget.h"
#include <QtGui>

// - Construction -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint )

GameView::GameView(GameLibrary *lib, QWidget *parent)
  : Base(parent, WINDOW_FLAGS), library_(lib)
{
  createLayout();
  updateTitle();
}

void
GameView::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  QToolButton *refreshButton = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_REFRESH), this, SLOT(refresh()));

  encodingLabel_ = ui->makeLabel(AcUi::DefaultHint, QString());
  //functionLabel_ = ui->makeLabel(AcUi::DefaultHint, QString());
  QLabel *encodingBuddy = ui->makeLabel(AcUi::BuddyHint, tr("Encoding"), encodingLabel_);
         //*functionBuddy = ui->makeLabel(AcUi::BuddyHint, tr("Function"), functionLabel_);

  // Layouts
  QGridLayout *grid = new QGridLayout; {
    // (row, col, rowspan, colspan, alignment)
    int r, c;
    grid->addWidget(encodingBuddy, r=0, c=0);
    grid->addWidget(encodingLabel_, r, ++c);

    //grid->addWidget(functionBuddy, ++r, c=0);
    //grid->addWidget(functionLabel_, r, ++c);

    ++r, c=0;
    grid->addWidget(refreshButton, r, ++c);

    grid->setContentsMargins(9, 9, 9, 9);
    setContentsMargins(0, 0, 0, 0);
  } setCentralWidget(new LayoutWidget(grid, this));
}

// - Properties -

void
GameView::setDigest(const QString &digest)
{
  //if (digest_ == digest);
  //  return;
  digest_ = digest;
  refresh();
}

void
GameView::updateGame()
{ game_ = library_->findGameByDigest(digest_); }

void
GameView::updateLabels()
{
  encodingLabel_->setText(game_.hasEncoding() ? game_.encoding() : QString("-"));
  //functionLabel_->setText(game_.hasFunction() ? game_.function() : QString("-"));
}

void
GameView::updateTitle()
{
  setWindowTitle(game_.hasTitle() ? game_.title() : tr("Game Preferences"));
}

// - Actions -

void
GameView::refresh()
{
  updateGame();
  updateLabels();
  updateTitle();
  if (game_.hasDigest())
    showMessage(tr("found 1 game"));
  else
    warn(tr("game not found"));
}

// EOF
