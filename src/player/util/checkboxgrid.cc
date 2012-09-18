// checkboxgrid.cc
// 8/28/2012

#include "checkboxgrid.h"
#include "src/common/acui.h"
#include "src/common/acss.h"
#include <QtGui/QGridLayout>
#include <QtGui/QCheckBox>

#define DEBUG "buttongrid"
#include "qtx/qxdebug.h"

// - Construction -

CheckBoxGrid::CheckBoxGrid(int capacity, int col, QObject *parent)
  : Base(parent), capacity_(capacity), column_(col), size_(0)
{
  Q_ASSERT(capacity_ > 0);
  Q_ASSERT(column_ > 0);
  createLayout();
}

CheckBoxGrid::~CheckBoxGrid()
{ delete buttons_; }

void
CheckBoxGrid::createLayout()
{
  layout_ = new QGridLayout;
  buttons_ = new QCheckBox*[capacity_];
  int r = -1;
  for (int i = 0; i < capacity_; i++) {
    auto b = buttons_[i] = new QCheckBox;
    b->hide();
    b->setStyleSheet(ACSS_CHECKBOX);
    b->setGraphicsEffect(AcUi::globalInstance()->makeHaloEffect(Qt::yellow));
    connect(b, SIGNAL(clicked()), SIGNAL(selectionChanged()));
    int c = i % column_;
    if (!c)
      r++;
    layout_->addWidget(b, r, c);
  }
}

// - Properties -

//int
//CheckBoxGrid::size() const
//{
//  for (int i = 0; i < capacity_ ; i++)
//    if (!buttons_[i]->isVisible())
//      return i;
//  return capacity_;
//}

//bool
//CheckBoxGrid::isEmpty() const
//{ return !buttons_[0]->isVisible(); }

QString
CheckBoxGrid::itemText(int index) const
{
  return index < 0 || index >= size_ ? QString() :
         buttons_[index]->text();
}

bool
CheckBoxGrid::isItemEnabled(int index) const
{ return index >= 0 && index < size_ && buttons_[index]->isEnabled(); }

void
CheckBoxGrid::setItemText(int index, const QString &text)
{
  if (index >= 0 && index < size_)
    buttons_[index]->setText(text);
}

void
CheckBoxGrid::setItemsEnabled(bool t)
{
  for (int i = 0; i < size_; i++)
    buttons_[i]->setEnabled(t);
}

void
CheckBoxGrid::setItemEnabled(int index, bool enabled)
{
  if (index >= 0 && index < size_)
    buttons_[index]->setEnabled(enabled);
}

bool
CheckBoxGrid::contains(int index) const
{ return index >= 0 && index < size_ && buttons_[index]->isChecked() && buttons_[index]->isEnabled(); }

bool
CheckBoxGrid::hasSelection() const
{
  for (int i = 0; i < size_ ; i++)
    if (buttons_[i]->isChecked() && buttons_[i]->isEnabled())
      return true;
  return false;
}

int
CheckBoxGrid::itemCount() const
{
  int ret = 0;
  for (int i = 0; i < size_ ; i++)
    if (buttons_[i]->isChecked() && buttons_[i]->isEnabled())
      ret++;
  return ret;
}

QList<int>
CheckBoxGrid::currentIndices() const
{
  QList<int> ret;
  for (int i = 0; i < size_ ; i++)
    if (buttons_[i]->isChecked() && buttons_[i]->isEnabled())
      ret.append(i);
  return ret;
}

void
CheckBoxGrid::setCurrentIndices(const QList<int> &indices)
{
  if (indices.isEmpty())
    for (int i = 0; i < size_; i++)
      buttons_[i]->setChecked(false);

  else
    for (int i = 0; i < size_; i++) {
      auto b = buttons_[i];
      b->setChecked(indices.contains(i));
      if (b->isChecked() && !b->isEnabled())
        b->setEnabled(true);
    }
}

// - Actions -

void
CheckBoxGrid::setItemChecked(int index, bool t)
{
  if (index >= 0 && index < size_) {
    auto b = buttons_[index];
    b->setChecked(t);
    if (b->isChecked() && !b->isEnabled())
      b->setEnabled(true);
  }
}

void
CheckBoxGrid::addItem(const QString &text)
{
  if (isFull()) {
    Q_ASSERT(0);
    emit error("too many buttons");
    return;
  }

  QCheckBox *b = buttons_[size_++];
  b->setText(text);
  //b->setToolTip(text);
  b->setChecked(false);
  b->setEnabled(true);
  b->show();
}

void
CheckBoxGrid::clear()
{
  if (size_) {
    for (int i = 0; i < size_ ; i++)
      buttons_[i]->hide();
    size_ =  0;
    //emit itemsChanged();
  }
}

// EOF
