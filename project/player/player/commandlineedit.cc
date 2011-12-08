// commandlineedit.cc
// 7/16/2011

#include "commandlineedit.h"
#include "annotationeditor.h"
#include "uistyle.h"
#include "rc.h"
#include "tr.h"
#include "annotationeditor.h"
#include <QtGui>

#define DEBUG "CommandLineEdit"
#include "module/debug/debug.h"

// - Constructions -

CommandLineEdit::CommandLineEdit(QWidget *parent)
  : Base(parent), current_(0), editor_(0)
{
  createActions();

  connect(this, SIGNAL(returnPressed()), SLOT(addCurrentTextToHistory()));
}

void
CommandLineEdit::createActions()
{
#define MAKE_ACTION(_action, _styleid, _slot) \
  _action = new QAction(QIcon(RC_IMAGE_##_styleid), TR(T_MENUTEXT_##_styleid), this); \
  _action->setToolTip(TR(T_TOOLTIP_##_styleid)); \
  connect(_action, SIGNAL(triggered()), _slot);

  MAKE_ACTION(editAct_,  EDIT, SLOT(edit()))

#undef MAKE_ACTION

  // Create menus
  contextMenu_ = new QMenu(TR(T_TITLE_ANNOTATIONBROWSER), this);
  UiStyle::globalInstance()->setContextMenuStyle(contextMenu_, true); // persistent = true
}


// - Properties -

AnnotationEditor*
CommandLineEdit::editor() const
{
  if (!editor_) {
    Self *self = const_cast<Self*>(this);
    AnnotationEditor *ret = new AnnotationEditor(self);
    connect(ret, SIGNAL(textSaved(QString)), SLOT(setText(QString)));
    self->editor_ = ret;
  }
  return editor_;
}

void
CommandLineEdit::edit()
{
  editor()->setText(text());
  editor()->show();
}


// - Events -

void
CommandLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
  if (!event)
    return;

  contextMenu_->clear();

  contextMenu_->addAction(editAct_);
  contextMenu_->addSeparator();

  QMenu *scm = createStandardContextMenu();
  contextMenu_->addActions(scm->actions());

  contextMenu_->exec(event->globalPos());
  delete scm;
  event->accept();
}

void
CommandLineEdit::keyPressEvent(QKeyEvent *event)
{
  Q_ASSERT(event);
  // Do not pass escape key to parent.
  switch (event->key()) {
  case Qt::Key_Escape:
    DOUT("keyPressEvent: Key_Escape");
    clearFocus();    // FIXME: after clear focus, which window get new focus?
    event->accept();
    return;

  case Qt::Key_Return:
    DOUT("keyPressEvent: Key_Return");
    event->accept();
    emit returnPressed();
    return;

  case Qt::Key_Up:
    DOUT("keyPressEvent: Key_Up");
    previous();
    event->accept();
    return;

  case Qt::Key_Down:
    DOUT("keyPressEvent: Key_Down");
    next();
    event->accept();
    return;
  }

  Base::keyPressEvent(event);
}

// - History -

const QVector<QString>&
CommandLineEdit::history() const
{ return history_; }

void
CommandLineEdit::clearHistory()
{ history_.clear(); }

void
CommandLineEdit::addCurrentTextToHistory()
{
  QString t = text();
  if (!t.isEmpty()) {
    if (!history_.empty() && t == history_.back())
      return;

    history_.push_back(t);
    current_ = (size_t)history_.size();
  }
}

void
CommandLineEdit::previous()
{
  if (current_ == 0)
    return;

  if (current_ == (size_t)history_.size()) {
    QString t = text();
    if (!t.isEmpty() && t != history_.back())
      if (history_.empty() || t != history_.back())
        history_.push_back(t);
  }

  current_--;
  Q_ASSERT(current_ < (size_t)history_.size());
  setText(history_[current_]);
}

void CommandLineEdit::next()
{
  if (current_ == (size_t)history_.size())
    return;

  current_++;

  if (current_ == (size_t)history_.size()) {
    clear();
    return;
  }

  if (current_ >= (size_t)history_.size()) {
    qDebug() << "current:" << current_ << ", history:" << history_.size();
    Q_ASSERT(0);
    return;
  }

  setText(history_[current_]);
}

// EOF
