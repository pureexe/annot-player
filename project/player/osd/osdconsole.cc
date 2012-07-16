// osdconsole.cc
// 7/16/2011

#include "osdconsole.h"
#include "tr.h"
#include "rc.h"
#include "global.h"
#include "project/common/acui.h"
#include "module/qtext/ss.h"
#include <QtGui>

#define DEBUG "osdconsole"
#include "module/debug/debug.h"

// - Constructions -

OsdConsole *OsdConsole::global_ = 0;

OsdConsole::OsdConsole(QWidget *parent)
  : Base(parent), dragPos_(BAD_POS)
{
  setContentsMargins(0, 0, 0, 0);

  { // Font color and size
    setStyleSheet(
      SS_BEGIN(QLabel)
        SS_COLOR(white)
        SS_FONT_SIZE(12pt)
      SS_END
    );
  }
  { // Outline font
    QGraphicsEffect *e = AcUi::globalInstance()->makeHaloEffect(Qt::yellow);
    e->setProperty("opacity", 0.95);
    setGraphicsEffect(e);
  }

  timer_ = new QTimer(this);
  timer_->setSingleShot(true);
  connect(timer_, SIGNAL(timeout()), SLOT(clear()));

  connect(this, SIGNAL(restartAutoClearTimerRequested()), SLOT(restartAutoClearTimer()), Qt::QueuedConnection);
  connect(this, SIGNAL(asyncSetText(QString)), SLOT(setText(QString)), Qt::QueuedConnection);

  //createActions();
  //createMenus();
}

//void
//OsdConsole::createActions()
//{
//  connect(enableAutoClearAct_ = new QAction(TR(T_MENUTEXT_ENABLEAUTOCLEARCONSOLE), this),
//          SIGNAL(triggered()), SLOT(restartAutoClearTimer()));
//  connect(disableAutoClearAct_ = new QAction(TR(T_MENUTEXT_DISABLEAUTOCLEARCONSOLE), this),
//          SIGNAL(triggered()), SLOT(stopAutoClearTimer()));
//}

//void
//OsdConsole::createMenus()
//{
//  contextMenu_ = new QMenu(this);
//  AcUi::globalInstance()->setContextMenuStyle(contextMenu_, true); // persistent = true
//}

void
OsdConsole::setAutoClearInterval(int msecs)
{ timer_->setInterval(msecs); }

void
OsdConsole::restartAutoClearTimer()
{
  if (timer_->isActive())
    timer_->stop();
  timer_->start();
}

void
OsdConsole::stopAutoClearTimer()
{
  if (timer_->isActive())
    timer_->stop();
}

bool
OsdConsole::isAutoClearTimerActive() const
{ return timer_->isActive(); }

// - Output -

void
OsdConsole::append(const QString &t)
{
  mutex_.lock();
  emit restartAutoClearTimerRequested();
  //setText(text() + append);
  emit asyncSetText(text().append(QString(t)));
  mutex_.unlock();
}

OsdConsole&
OsdConsole::operator<<(const QString &text)
{ append(text); return *this; }

// EOF

/*
// - Events -

void
OsdConsole::mousePressEvent(QMouseEvent *event)
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
OsdConsole::mouseMoveEvent(QMouseEvent *event)
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
OsdConsole::mouseReleaseEvent(QMouseEvent *event)
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
OsdConsole::mouseDoubleClickEvent(QMouseEvent *event)
{
  DOUT("begin");
  Base::mouseDoubleClickEvent(event);
  DOUT("end");
}

void
OsdConsole::contextMenuEvent(QContextMenuEvent *event)
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
OsdConsole::copyToClipboard() const
{
  QClipboard *clipboard = QApplication::clipboard();
  if (clipboard)
    clipboard->setText(text());
}

void
OsdConsole::pasteFromClipboard()
{
  QClipboard *clipboard = QApplication::clipboard();
  if (clipboard)
    setText(clipboard->text());
}

*/
