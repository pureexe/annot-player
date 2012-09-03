// mainconsole.cc
// 7/16/2011

#include "mainconsole.h"
#include "tr.h"
#include "rc.h"
#include "global.h"
#include "project/common/acui.h"
#include "module/qtext/ss.h"
#include <QtGui>

#define DEBUG "mainconsole"
#include "module/debug/debug.h"

enum { LineHeight = 9 };

#define SS_TEXT_BEGIN   "<span style=\"background-color:rgba(0,0,0,16)\">"
#define SS_TEXT_END     "</span>"

// - Constructions -

MainConsole::MainConsole(QWidget *parent)
  : Base(parent), dragPos_(BAD_POS)
{
  //setContentsMargins(0, 9, 0, 0); // l, t, r, d
  setContentsMargins(0, 0, 0, 0);

  { // Font color and size
    setStyleSheet(
      SS_BEGIN(QLabel)
#ifdef Q_OS_MAC
        SS_COLOR(yellow)
#else
        SS_COLOR(snow)
#endif // Q_OS_MAC
        SS_FONT_SIZE(12pt)
      SS_END
    );
  }
  { // Outline font
    QGraphicsEffect *e = AcUi::globalInstance()->makeHaloEffect(Qt::yellow);
    if (e) {
      e->setProperty("opacity", 0.95);
      setGraphicsEffect(e);
    }
  }

  timer_ = new QTimer(this);
  timer_->setSingleShot(true);
  connect(timer_, SIGNAL(timeout()), SLOT(clearAll()));

  connect(this, SIGNAL(restartAutoClearTimerRequested()), SLOT(restartAutoClearTimer()), Qt::QueuedConnection);
  connect(this, SIGNAL(asyncSetText(QString)), SLOT(setText(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(asyncUpdateText()), SLOT(updateText()), Qt::QueuedConnection);

  //createActions();
  //createMenus();
}

// - Screen -

bool
MainConsole::isAreaFull() const
{
  return !messages_.isEmpty() && areaSize_.height() > 0 &&
         messages_.size() > areaSize_.height() / LineHeight;
}

// - Clear -

void
MainConsole::setAutoClearInterval(int msecs)
{ timer_->setInterval(msecs); }

void
MainConsole::restartAutoClearTimer()
{
  if (timer_->isActive())
    timer_->stop();
  timer_->start();
}

void
MainConsole::stopAutoClearTimer()
{
  if (timer_->isActive())
    timer_->stop();
}

bool
MainConsole::isAutoClearTimerActive() const
{ return timer_->isActive(); }

void
MainConsole::clearAll()
{
  clear();
  messages_.clear();
}

// - Text -

void
MainConsole::updateText()
{
  if (messages_.isEmpty())
    clear();
  else
    setText(
      SS_TEXT_BEGIN
        + messages_.join(QString()) +
      SS_TEXT_END
    );
}

void
MainConsole::sendMessage(const QString &t)
{
  QMutexLocker lock(&mutex_);

  emit message(t);
  emit restartAutoClearTimerRequested();
  messages_.append(t);

  if (isAreaFull()) {
    messages_.removeFirst();
    emit asyncUpdateText();
  } else
    emit asyncSetText(text().append(
      SS_TEXT_BEGIN + t + SS_TEXT_END
    ));
}

// EOF

/*
// - Events -

void
MainConsole::mousePressEvent(QMouseEvent *event)
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
MainConsole::mouseMoveEvent(QMouseEvent *event)
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
MainConsole::mouseReleaseEvent(QMouseEvent *event)
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
MainConsole::mouseDoubleClickEvent(QMouseEvent *event)
{
  DOUT("begin");
  Base::mouseDoubleClickEvent(event);
  DOUT("end");
}

void
MainConsole::contextMenuEvent(QContextMenuEvent *event)
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
MainConsole::copyToClipboard() const
{
  QClipboard *clipboard = QApplication::clipboard();
  if (clipboard)
    clipboard->setText(text());
}

void
MainConsole::pasteFromClipboard()
{
  QClipboard *clipboard = QApplication::clipboard();
  if (clipboard)
    setText(clipboard->text());
}

*/
