// gui/wbsearchedit.cc
// 3/31/2012

#include "gui/wbsearchedit.h"
#include "module/searchengine/searchengine.h"
#include <QtGui>

//#define DEBUG "wbsearchedit"
#include "module/debug/debug.h"

#define MIN_WIDTH       150
#define MAX_RECENT      15

// - Construction -

void
WbSearchEdit::createActions()
{
  pasteAndGoAct->setText(tr("Paste and go"));
  pasteAndGoAct->setStatusTip(tr("Paste and go"));

  submitAct->setText(tr("Search"));
  submitAct->setStatusTip(tr("Search"));
}

void
WbSearchEdit::createConnections()
{
  connect(this, SIGNAL(editTextChanged(QString)), SLOT(updateText(QString)));
  connect(this, SIGNAL(activated(int)), SLOT(setEngineByIndex(int)));
}

// - Recent -

QStringList
WbSearchEdit::recent() const
{
  QStringList ret;
  for (int i = 0; i < recentCount(); i++) {
    QString t = itemText(i).trimmed();
    if (!t.isEmpty())
      ret.append(t);
  }
  return ret;
}

void
WbSearchEdit::addRecent(const QString &text)
{
  QString t = text.trimmed();
  if (!t.isEmpty() && text != currentText()) {
    removeRecent(text);
    insertItem(0, t);
    if (recentCount() > MAX_RECENT)
      removeItem(recentCount() -1);
  }
}

void
WbSearchEdit::removeRecent(const QString &text)
{
  int i = findText(text);
  if (i >= 0 && i < recentCount()) {
    //int engine = -1;
    //if (i == recentCount() -1)
    //  engine = engine_;
    removeItem(i);
    //if (engine >= 0)
    //  setEngine(engine);
  }
}

void
WbSearchEdit::updateText(const QString &text)
{
  lastText_ = currentText_;
  currentText_ = text;
}

// - Engines -

void
WbSearchEdit::invalidateEngines()
{
  QStringList items,
              icons;

  items.append("アニメ"); icons.append(QString());

  foreach (const SearchEngine *e, engines_) {
    Q_ASSERT(e);
    items.append(e->name());
    icons.append(e->icon());
  }

  setDefaultItems(items, icons);
}

void
WbSearchEdit::setEngine(int engine)
{
  if (engine_ != engine) {
    engine_ = engine;
    const SearchEngine *e = engines_[engine_];
    QString tip = e->name();
    if (e->hasAcronyms()) {
      const QStringList &a = e->acronyms();
      tip.append(" [");
      for (int i = 0; i < qMin(5, a.size()); i++) {
        if (i)
          tip.append(",");
        tip.append(a[i]);
      }
      tip.append("]");
    }
    lineEdit()->setPlaceholderText(e->name());
    lineEdit()->setToolTip(tip);
    lineEdit()->setStatusTip(e->queryUrl());
    setIcon(e->icon());
    emit engineChanged(engine_);
  }
}

void
WbSearchEdit::setText(const QString &text)
{
  if (recentCount())
    setCurrentIndex(0);
  setEditText(text);
  lineEdit()->setCursorPosition(0);
}

void
WbSearchEdit::setEngineByIndex(int index)
{
  DOUT("enter: index =" << index);
  int count = recentCount();
  int engine = index - count;
  if (engine >= 0 && engine < engines_.size()) {
    QString text = lastText_;
    setEngine(engine);
    clearText();
    clearFocus();
    setText(text);
  }
  DOUT("exit");
}

// - Submit -

void
WbSearchEdit::submitText()
{
  DOUT("enter: index =" << currentIndex() << ", text =" << currentText());
  if (currentIndex() < recentCount()) {
    QString t = currentText().trimmed();
    if (!t.isEmpty())
      emit textEntered(t);
    else {
      clearText();
      clearFocus();
    }
  }
  DOUT("exit");
}

// EOF
