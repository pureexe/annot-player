// miniconsole.cc
// 7/18/2012

#include "miniconsole.h"
#include "tr.h"
#include "rc.h"
#include "global.h"
#include "project/common/acui.h"
#include "module/qtext/ss.h"
#include <QtGui>

#define DEBUG "miniconsole"
#include "module/debug/debug.h"

// - Constructions -

MiniConsole::MiniConsole(QWidget *parent)
  : Base(parent), dragPos_(BAD_POS)
{

  { // Font color and size
    setStyleSheet(
      SS_BEGIN(QLabel)
        SS_FONT_SIZE(30pt)
#ifdef Q_OS_MAC
        SS_COLOR(magenta)
#else
        SS_COLOR(white)
        SS_FONT_WEIGHT(bold)
#endif // !Q_OS_MAC
      SS_END
    );
  }
  { // Outline font
    QGraphicsEffect *e = AcUi::globalInstance()->makeHaloEffect(Qt::magenta);
    if (e) {
      e->setProperty("opacity", 0.95);
      setGraphicsEffect(e);
    }
  }

  setAlignment(Qt::AlignRight | Qt::AlignHCenter);
  setContentsMargins(0, 18, 27, 0); // l,t,r,b

  timer_ = new QTimer(this);
  timer_->setSingleShot(true);
  connect(timer_, SIGNAL(timeout()), SLOT(clear()));

  connect(this, SIGNAL(restartAutoClearTimerRequested()), SLOT(restartAutoClearTimer()), Qt::QueuedConnection);
  connect(this, SIGNAL(asyncSetText(QString)), SLOT(setText(QString)), Qt::QueuedConnection);

  //createActions();
  //createMenus();
}

// - Clear -

void
MiniConsole::setAutoClearInterval(int msecs)
{ timer_->setInterval(msecs); }

void
MiniConsole::restartAutoClearTimer()
{
  if (timer_->isActive())
    timer_->stop();
  timer_->start();
}

void
MiniConsole::stopAutoClearTimer()
{
  if (timer_->isActive())
    timer_->stop();
}

bool
MiniConsole::isAutoClearTimerActive() const
{ return timer_->isActive(); }

// - Text -

void
MiniConsole::sendMessage(const QString &text)
{
  QMutexLocker lock(&mutex_);
  emit restartAutoClearTimerRequested();
  emit asyncSetText(text);
}

// EOF

/*
// - Events -

void
MiniConsole::mousePressEvent(QMouseEvent *event)
{
  DOUT("enter");
  restartAutoClearTimer();
  Base::mousePressEvent(event);
  //if (event && event->button() == Qt::LeftButton
  //    && dragPos_ == BAD_POS) {
  //  dragPos_ = event->globalPos() - frameGeometry().topLeft();
  //  event->accept();
  //}
  DOUT("exit");
}

void
MiniConsole::mouseMoveEvent(QMouseEvent *event)
{
  DOUT("enter");
  restartAutoClearTimer();
  Base::mousePressEvent(event);
  //if (event && event->buttons() & Qt::LeftButton
  //    && dragPos_ != BAD_POS) {
  //  QPoint newPos = event->globalPos() - dragPos_;
  //  move(newPos);
  //  event->accept();
  //}
  DOUT("exit");
}

void
MiniConsole::mouseReleaseEvent(QMouseEvent *event)
{
  DOUT("begin");
  restartAutoClearTimer();
  Base::mouseReleaseEvent(event);
  //dragPos_ = BAD_POS;
  //if (event)
  //  event->accept();
  DOUT("end");
}

void
MiniConsole::mouseDoubleClickEvent(QMouseEvent *event)
{
  DOUT("begin");
  Base::mouseDoubleClickEvent(event);
  DOUT("end");
}

void
MiniConsole::contextMenuEvent(QContextMenuEvent *event)
{
  DOUT("enter");
  Base::contextMenuEvent(event);
  //Q_ASSERT(contextMenu_);
  //Q_ASSERT(timer_);
  //if (event) {
  //  contextMenu_->clear();
  //  if (timer_->isActive())
  //    contextMenu_->addAction(disableAutoClearAct_);
  //  else
  //    contextMenu_->addAction(enableAutoClearAct_);
  //  contextMenu_->popup(event->globalPos());
  //  event->accept();
  //}
  DOUT("exit");
}

// - Copy && paste -

void
MiniConsole::copyToClipboard() const
{
  QClipboard *clipboard = QApplication::clipboard();
  if (clipboard)
    clipboard->setText(text());
}

void
MiniConsole::pasteFromClipboard()
{
  QClipboard *clipboard = QApplication::clipboard();
  if (clipboard)
    setText(clipboard->text());
}

*/
