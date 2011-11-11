// osdconsole.cc
// 7/16/2011

#include "osdconsole.h"
#include "tr.h"
#include "rc.h"
#include "global.h"
#include "uistyle.h"
#include <QtGui>

#define DEBUG "OSDConsole"
#include "module/debug/debug.h"

// - Constructions -

OSDConsole *OSDConsole::global_ = 0;

OSDConsole*
OSDConsole::globalInstance()
{ return global_; }

void
OSDConsole::setGlobalInstance(Self *global)
{ global_ = global; }

OSDConsole::OSDConsole(QWidget *parent)
  : Base(parent), dragPos_(BAD_POS)
{
  setContentsMargins(0, 0, 0, 0);

  timer_ = new QTimer(this);
  timer_->setSingleShot(true);
  connect(timer_, SIGNAL(timeout()), SLOT(clear()));

  connect(this, SIGNAL(restartAutoClearTimerRequested()), SLOT(restartAutoClearTimer()), Qt::QueuedConnection);
  connect(this, SIGNAL(asyncSetText(QString)), SLOT(setText(QString)), Qt::QueuedConnection);

  createActions();
  createMenus();
}

void
OSDConsole::createActions()
{
#define MAKE_ACTION(_action, _styleid, _slot) \
  _action = new QAction(QIcon(RC_IMAGE_##_styleid), TR(T_MENUTEXT_##_styleid), this); \
  _action->setStatusTip(TR(T_STATUSTIP_##_styleid)); \
  connect(_action, SIGNAL(triggered()), _slot);

  MAKE_ACTION(enableAutoClearAct_, ENABLEAUTOCLEARCONSOLE, SLOT(restartAutoClearTimer()))
  MAKE_ACTION(enableAutoClearAct_, DISABLEAUTOCLEARCONSOLE, SLOT(stopAutoClearTimer()))

#undef MAKE_ACTION
}

void
OSDConsole::createMenus()
{
  contextMenu_ = new QMenu(this);
  UiStyle::globalInstance()->setContextMenuStyle(contextMenu_, true); // persistent = true
}

void
OSDConsole::setAutoClearInterval(int msecs)
{ timer_->setInterval(msecs); }

void
OSDConsole::restartAutoClearTimer()
{
  if (timer_->isActive())
    timer_->stop();
  timer_->start();
}

void
OSDConsole::stopAutoClearTimer()
{
  if (timer_->isActive())
    timer_->stop();
}

bool
OSDConsole::isAutoClearTimerActive() const
{ return timer_->isActive(); }

// - Output -

void
OSDConsole::append(const QString &t)
{
  mutex_.lock();
  emit restartAutoClearTimerRequested();
  //setText(text() + append);
  emit asyncSetText(text() + t);
  mutex_.unlock();
}

OSDConsole&
OSDConsole::operator<<(const QString &text)
{ append(text); return *this; }

// - Events -

/*
void
OSDConsole::mousePressEvent(QMouseEvent *event)
{
  DOUT("mousePressEvent: enter");
  restartAutoClearTimer();
  Base::mousePressEvent(event);
  //if (event && event->button() == Qt::LeftButton
  //    && dragPos_ == BAD_POS) {
  //  dragPos_ = event->globalPos() - frameGeometry().topLeft();
  //  event->accept();
  //}
  DOUT("mousePressEvent: exit");
}

void
OSDConsole::mouseMoveEvent(QMouseEvent *event)
{
  DOUT("mouseMoveEvent: enter");
  restartAutoClearTimer();
  Base::mousePressEvent(event);
  //if (event && event->buttons() & Qt::LeftButton
  //    && dragPos_ != BAD_POS) {
  //  QPoint newPos = event->globalPos() - dragPos_;
  //  move(newPos);
  //  event->accept();
  //}
  DOUT("mouseMoveEvent: exit");
}

void
OSDConsole::mouseReleaseEvent(QMouseEvent *event)
{
  DOUT("mouseReleaseEvent: begin");
  restartAutoClearTimer();
  Base::mouseReleaseEvent(event);
  //dragPos_ = BAD_POS;
  //if (event)
  //  event->accept();
  DOUT("mouseReleaseEvent: end");
}

void
OSDConsole::mouseDoubleClickEvent(QMouseEvent *event)
{
  DOUT("mouseDoubleClickEvent: begin");
  Base::mouseDoubleClickEvent(event);
  DOUT("mouseDoubleClickEvent: end");
}

void
OSDConsole::contextMenuEvent(QContextMenuEvent *event)
{
  DOUT("contexrMenuEvent: enter");
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
  DOUT("contexrMenuEvent: exit");
}
*/

// - Copy && paste -

void
OSDConsole::copyToClipboard() const
{
  QClipboard *clipboard = QApplication::clipboard();
  if (clipboard)
    clipboard->setText(text());
}

void
OSDConsole::pasteFromClipboard()
{
  QClipboard *clipboard = QApplication::clipboard();
  if (clipboard)
    setText(clipboard->text());
}

// EOF
