// qxfontcombobox.cc
// 10/11/2011

#include "qtx/qxfontcombobox.h"
#include "htmlutil/sstags.h"

QTX_BEGIN_NAMESPACE

// - Construction -

QxFontComboBox::QxFontComboBox(QWidget *parent)
  : Base(parent)
{
  setEditable(true);
  connect(this, SIGNAL(editTextChanged(QString)), SLOT(verifyCurrentText()));
}

// - Actions -

bool
QxFontComboBox::verifyCurrentText()
{
  QString t = currentText().trimmed();
  if (t.isEmpty())
    return false;
  if (styleSheet_.isEmpty())
    styleSheet_ = styleSheet();
  bool ok = findText(t, Qt::MatchExactly) >= 0; // case-insensitive match
  setStyleSheet(styleSheet_ + (ok ?
    SS_BEGIN(QFontComboBox)
      SS_COLOR(black)
    SS_END
    :
    SS_BEGIN(QFontComboBox)
      SS_COLOR(red)
    SS_END
  ));
  return ok;
}

// - Events -

void
QxFontComboBox::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QxFontComboBox::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

QTX_END_NAMESPACE
