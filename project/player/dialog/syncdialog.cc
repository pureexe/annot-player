// syncdialog.cc
// 10/7/2011

#include "syncdialog.h"
#include "uistyle.h"
#include "tr.h"
#include "stylesheet.h"
#include "module/qtext/toolbutton.h"
#include "module/qtext/combobox.h"
#include <QtGui>

#define SLOTLINE_MAXWIDTH 150

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowCloseButtonHint | \
  Qt::WindowStaysOnTopHint )

// - Constructions -

SyncDialog::SyncDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS), timeSlotIndex_(0)
{
//#ifdef Q_WS_MAC
//  setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
//#endif // Q_WS_MAC
  setWindowTitle(TR(T_TITLE_SYNC));
  setContentsMargins(0, 0, 0, 0);
  UiStyle::globalInstance()->setWindowStyle(this);

  // Widgets

  comboBox_ = new QtExt::ComboBox; {
    UiStyle::globalInstance()->setComboBoxStyle(comboBox_);
    comboBox_->setMaximumWidth(SLOTLINE_MAXWIDTH);
  }

  QToolButton *okButton = new QtExt::ToolButton; {
    okButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
    okButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    okButton->setText(QString("[ %1 ]").arg(TR(T_OK)));
    okButton->setToolTip(TR(T_OK));
  }

  QToolButton *cancelButton = new QtExt::ToolButton; {
    cancelButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
    cancelButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    cancelButton->setText(QString("[ %1 ]").arg(TR(T_CANCEL)));
    cancelButton->setToolTip(TR(T_CANCEL));
  }

  QLabel *comboBoxLabel = new QLabel; {
    comboBoxLabel->setBuddy(comboBox_);
    comboBoxLabel->setStyleSheet(SS_LABEL);
    comboBoxLabel->setText(TR(T_LABEL_TIMESLOT));
    comboBoxLabel->setToolTip(TR(T_TOOLTIP_TIMESLOT));
  }

  // Layouts
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *row1 = new QHBoxLayout,
                *row2 = new QHBoxLayout;
    rows->addLayout(row1);
    rows->addLayout(row2);

    row1->addWidget(comboBoxLabel);
    row1->addWidget(comboBox_);
    row2->addWidget(okButton);
    row2->addWidget(cancelButton);
  } setLayout(rows);

  /*
  QGridLayout *layout = new QGridLayout; {
    layout->addWidget(userNameLabel, 0, 0);
    layout->addWidget(userNameEdit_, 0, 1);
    layout->addWidget(passwordLabel, 1, 0);
    layout->addWidget(passwordEdit_, 1, 1);
    layout->addWidget(okButton, 2, 0);
    layout->addWidget(cancelButton, 2, 1);
  }
  setLayout(layout);
  */

  // Connections
  connect(cancelButton, SIGNAL(clicked()), SLOT(cancel()));
  connect(okButton, SIGNAL(clicked()), SLOT(ok()));

  connect(comboBox_, SIGNAL(currentIndexChanged(int)), SLOT(setTimeSlotIndex(int)));

  // Focus
  comboBox_->setFocus();
}

void
SyncDialog::setVisible(bool visible)
{
  if (visible)
    updateComboBox();
  Base::setVisible(visible);
}

void
SyncDialog::updateComboBox()
{
  comboBox_->clear();

  for (int i = 0; i < timeSlots_.size(); i++) {
    qint64 secs = timeSlots_[i];
    qint64 msecs = secs * 1000;
    QTime t = QDateTime::fromMSecsSinceEpoch(msecs).time();
    QString ts = t.toString();

    int count = 0;
    if (i < activeCounts_.size())
      count = activeCounts_[i];

    QString text;
    if (count)
      text = QString("%1 (%2)").arg(ts).arg(count);
    else
      text = ts;
    comboBox_->addItem(text);
  }
}

// - Properties -

int
SyncDialog::timeSlotIndex() const
{ return timeSlotIndex_; }

void
SyncDialog::setTimeSlotIndex(int index)
{ timeSlotIndex_ = index; }

const QList<qint64>&
SyncDialog::timeSlots() const
{ return timeSlots_; }

void
SyncDialog::setTimeSlots(const QList<qint64> &timeSlots)
{ timeSlots_ = timeSlots; }

const QList<int>&
SyncDialog::activeCounts() const
{ return activeCounts_; }

void
SyncDialog::setActiveCounts(const QList<int> &counts)
{ activeCounts_ = counts; }

// - Slots -

void
SyncDialog::ok()
{
  if (timeSlotIndex_ >= 0 && timeSlotIndex_ < timeSlots_.size()) {
    qint64 slot = timeSlots_[timeSlotIndex_];
    emit timeSlotSelected(slot);
  }
}

void
SyncDialog::cancel()
{ hide(); }

// EOF
