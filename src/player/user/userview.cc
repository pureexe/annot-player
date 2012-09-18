// userview.cc
// 8/3/2011

#include "userview.h"
#include "tr.h"
#include "src/common/acss.h"
#include "src/common/acui.h"
#include "qtx/qxbitwise.h"
#include <QtGui>

using namespace AnnotCloud;

// - Constructions -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint )

UserView::UserView(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  setWindowTitle(TR(T_TITLE_USERVIEW));
  createLayout();
}

void
UserView::createLayout()
{
#define MAKE_BUDDY(_name, _text) \
  _name##Label_ = new QLabel; { \
    _name##Label_->setStyleSheet(ACSS_LABEL); \
    _name##Label_->setToolTip(_text); \
  } \
  QLabel *_name##Buddy = new QLabel; { \
    _name##Buddy->setStyleSheet(ACSS_LABEL); \
    _name##Buddy->setBuddy(_name##Label_); \
    _name##Buddy->setText(_text + ":"); \
    _name##Buddy->setToolTip(_text); \
  }

  MAKE_BUDDY(name, TR(T_USERNAME))
  MAKE_BUDDY(nickname, TR(T_NICKNAME))
  MAKE_BUDDY(email, TR(T_EMAIL))
  MAKE_BUDDY(createTime, TR(T_CREATETIME))
  MAKE_BUDDY(loginTime, TR(T_LOGINTIME))
  MAKE_BUDDY(loginIp, TR(T_LOGINIP))
  MAKE_BUDDY(loginCount, TR(T_LOGINCOUNT))
  MAKE_BUDDY(annotCount, TR(T_ANNOTCOUNT))
  MAKE_BUDDY(blessCount, TR(T_BLESSCOUNT))
  MAKE_BUDDY(curseCount, TR(T_CURSECOUNT))
  MAKE_BUDDY(blockCount, TR(T_BLOCKCOUNT))

  // Set layout

  QGridLayout *grid = new QGridLayout; {
    // (row, col, rowspan, colspan, alignment)
    int r, c;
    grid->addWidget(nameBuddy, r=0, c=0);
    grid->addWidget(nameLabel_, r, ++c);

    grid->addWidget(nicknameBuddy, ++r, c=0);
    grid->addWidget(nicknameLabel_, r, ++c);

    grid->addWidget(emailBuddy, ++r, c=0);
    grid->addWidget(emailLabel_, r, ++c);

    grid->addWidget(createTimeBuddy, ++r, c=0);
    grid->addWidget(createTimeLabel_, r, ++c);

    grid->addWidget(loginTimeBuddy, ++r, c=0);
    grid->addWidget(loginTimeLabel_, r, ++c);

    grid->addWidget(loginIpBuddy, ++r, c=0);
    grid->addWidget(loginIpLabel_, r, ++c);

    grid->addWidget(loginCountBuddy, ++r, c=0);
    grid->addWidget(loginCountLabel_, r, ++c);

    grid->addWidget(annotCountBuddy, ++r, c=0);
    grid->addWidget(annotCountLabel_, r, ++c);

    grid->addWidget(blessCountBuddy, ++r, c=0);
    grid->addWidget(blessCountLabel_, r, ++c);

    grid->addWidget(curseCountBuddy, ++r, c=0);
    grid->addWidget(curseCountLabel_, r, ++c);

    grid->addWidget(blockCountBuddy, ++r, c=0);
    grid->addWidget(blockCountLabel_, r, ++c);

    //grid->setContentsMargins(0, 0, 0, 0);
    //setContentsMargins(0, 0, 0, 0);
  } setLayout(grid);

#undef MAKE_BUDDY
}

void
UserView::updateUser()
{
#define FORMAT_TIME(_secs)      QDateTime::fromMSecsSinceEpoch(_secs * 1000).toString(Qt::ISODate).replace('T', ' ')
#define FORMAT_COUNT(_count)    QString::number(_count)
#define FORMAT_IP(_ip)          QxBitwise::BigEndian::long2ip(_ip)
  nameLabel_->setText(user_.name());
  nicknameLabel_->setText(user_.nickname());
  emailLabel_->setText(user_.email());
  createTimeLabel_->setText(FORMAT_TIME(user_.createTime()));
  loginTimeLabel_->setText(FORMAT_TIME(user_.loginTime()));
  loginIpLabel_->setText(FORMAT_IP(user_.loginIp()));
  loginCountLabel_->setText(FORMAT_COUNT(user_.loginCount()));
  annotCountLabel_->setText(FORMAT_COUNT(user_.annotCount()));
  blessCountLabel_->setText(FORMAT_COUNT(user_.blessCount()));
  curseCountLabel_->setText(FORMAT_COUNT(user_.curseCount()));
  blockCountLabel_->setText(FORMAT_COUNT(user_.blockCount()));
#undef FORMAT_TIME
#undef FORMAT_COUNT
#undef FORMAT_IP
}

// EOF
