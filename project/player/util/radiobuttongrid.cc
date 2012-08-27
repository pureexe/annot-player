// radiobuttongrid.cc
// 8/18/2012

#include "radiobuttongrid.h"
#include "project/common/acui.h"
#include "project/common/acss.h"
#include <QtGui/QGridLayout>
#include <QtGui/QRadioButton>

#define DEBUG "buttongrid"
#include "module/debug/debug.h"

// - Construction -

RadioButtonGrid::RadioButtonGrid(int capacity, int col, QObject *parent)
  : Base(parent), capacity_(capacity), column_(col), size_(0)
{
  Q_ASSERT(capacity_ > 0);
  Q_ASSERT(column_ > 0);
  createLayout();
}

RadioButtonGrid::~RadioButtonGrid()
{ delete buttons_; }

void
RadioButtonGrid::createLayout()
{
  layout_ = new QGridLayout;
  buttons_ = new QRadioButton*[capacity_];
  int r = -1;
  for (int i = 0; i < capacity_; i++) {
    auto b = buttons_[i] = new QRadioButton;
    b->hide();
    b->setStyleSheet(ACSS_RADIOBUTTON);
    b->setGraphicsEffect(AcUi::globalInstance()->makeHaloEffect(Qt::yellow));
    connect(b, SIGNAL(clicked()), SLOT(updateValue()));
    int c = i % column_;
    if (!c)
      r++;
    layout_->addWidget(b, r, c);
  }
}

// - Properties -

//int
//RadioButtonGrid::size() const
//{
//  for (int i = 0; i < capacity_ ; i++)
//    if (!buttons_[i]->isVisible())
//      return i;
//  return capacity_;
//}

//bool
//RadioButtonGrid::isEmpty() const
//{ return !buttons_[0]->isVisible(); }

int
RadioButtonGrid::value() const
{
  for (int i = 0; i < size_ ; i++)
    if (buttons_[i]->isChecked())
      return i;
  return -1;
}

void
RadioButtonGrid::setValue(int index)
{
  if (index >= 0 && index < size_)
    buttons_[index]->setChecked(true);
}

// - Actions -

void
RadioButtonGrid::addItem(const QString &title, const QString &tip)
{
  if (isFull()) {
    Q_ASSERT(0);
    emit error("too many buttons");
    return;
  }

  QRadioButton *b = buttons_[size_++];
  b->setText(title);
  b->setToolTip(tip);
  b->show();
}

void
RadioButtonGrid::clear()
{
  if (size_) {
    for (int i = 0; i < size_ ; i++)
      buttons_[i]->hide();
    size_ =  0;
  }
}

void
RadioButtonGrid::updateValue()
{ emit valueChanged(value()); }

// EOF
