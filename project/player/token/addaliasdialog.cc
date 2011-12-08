// addaliasdialog.cc
// 10/13/2011

#include "addaliasdialog.h"
#include "uistyle.h"
#include "tr.h"
#include "global.h"
#include "stylesheet.h"
#include "lineedit.h"
#include "core/htmltag.h"
#include "core/util/datetime.h"
#include "core/gui/toolbutton.h"
#include "core/cloud/traits.h"
#include "core/cloud/alias.h"
#include <QtGui>

using namespace Core::Cloud;

//#define DEBUG

#ifdef DEBUG
  #define DOUT(_msg)    qDebug() << "AddAliasDialog: " << _msg
#else
  #define DOUT(_dummy)
#endif // DEBUG

// - Panel -
AddAliasDialog::AddAliasDialog(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(tr("Add Alias"));
  UiStyle::globalInstance()->setWindowStyle(this);
  setContentsMargins(0, 0, 0, 0);

  // Widgets

  aliasEdit_ = new LineEdit; {
    aliasEdit_->setStyleSheet(SS_LINEEDIT);
    aliasEdit_->setToolTip(TR(T_TOOLTIP_ADDALIAS));
  }
  connect(aliasEdit_, SIGNAL(returnPressed()), SLOT(ok()));

#define MAKE_LABEL(_label, _tr) \
  QLabel *_label = new QLabel; { \
    _label->setStyleSheet(SS_LABEL); \
    _label->setText(QString("%1:").arg(_tr)); \
    _label->setToolTip(_tr); \
  }

  MAKE_LABEL(tagLabel, TR(T_TAG))
  MAKE_LABEL(typeLabel, TR(T_TYPE))
  MAKE_LABEL(languageLabel, TR(T_LANGUAGE))
  MAKE_LABEL(aliasLabel, TR(T_ALIAS))
#undef MAKE_LABEL

#define MAKE_TAG(_id) \
  QToolButton *tag##_id##Button = new Core::Gui::ToolButton; { \
    tag##_id##Button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
    tag##_id##Button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
    tag##_id##Button->setText(#_id); \
    tag##_id##Button->setToolTip(#_id); \
    QFont font = tag##_id##Button->font(); \
    font.setItalic(true); \
    tag##_id##Button->setFont(font); \
  } \
  connect(tag##_id##Button, SIGNAL(clicked()), SLOT(tag##_id()));

  MAKE_TAG(BD)
  MAKE_TAG(DVD)
  MAKE_TAG(TV)
  MAKE_TAG(Web)
#undef MAKE_TAG

#define MAKE_LANGUAGE(_id, _styleid) \
  is##_id##Button_ = new Core::Gui::ToolButton; { \
    is##_id##Button_->setStyleSheet(SS_TOOLBUTTON_TEXT); \
    is##_id##Button_->setToolButtonStyle(Qt::ToolButtonTextOnly); \
    is##_id##Button_->setText(TR(T_##_styleid)); \
    is##_id##Button_->setCheckable(true); \
    is##_id##Button_->setToolTip(TR(T_##_styleid)); \
    QFont font = is##_id##Button_->font(); \
    font.setUnderline(true); \
    is##_id##Button_->setFont(font); \
  } \
  connect(is##_id##Button_, SIGNAL(clicked(bool)), SLOT(invalidateOKButton()));

  MAKE_LANGUAGE(English, ENGLISH)
  MAKE_LANGUAGE(Japanese, JAPANESE)
  MAKE_LANGUAGE(Chinese, CHINESE)
  MAKE_LANGUAGE(Alien, ALIEN)
#undef MAKE_LANGUAGE

#define MAKE_TYPE(_id, _text, _tip) \
  is##_id##Button_ = new Core::Gui::ToolButton; { \
    is##_id##Button_->setStyleSheet(SS_TOOLBUTTON_TEXT); \
    is##_id##Button_->setToolButtonStyle(Qt::ToolButtonTextOnly); \
    is##_id##Button_->setText(_text); \
    is##_id##Button_->setCheckable(true); \
    is##_id##Button_->setToolTip(_tip); \
    QFont font = is##_id##Button_->font(); \
    font.setBold(true); \
    is##_id##Button_->setFont(font); \
  } \
  connect(is##_id##Button_, SIGNAL(clicked(bool)), SLOT(setTypeTo##_id(bool)));

  MAKE_TYPE(Name, tr("name"), tr("Add a name to the token"))
  MAKE_TYPE(Tag, tr("tag"), tr("Add a tag to the token"))
#undef MAKE_TYPE
  isNameButton_->setChecked(true);

#define MAKE_BUTTON(_button, _styleid, _slot) \
  _button = new Core::Gui::ToolButton; { \
    _button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
    _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
    _button->setText(QString("[ %1 ]").arg(TR(T_##_styleid))); \
    _button->setToolTip(TR(T_##_styleid)); \
  } \
  connect(_button, SIGNAL(clicked()), _slot);

  MAKE_BUTTON(okButton_, ADD, SLOT(ok()))
  MAKE_BUTTON(cancelButton_, CANCEL, SLOT(cancel()))
#undef MAKE_BUTTON

  // Layouts
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *row1 = new QHBoxLayout,
                *row2 = new QHBoxLayout,
                *row3 = new QHBoxLayout,
                *row4 = new QHBoxLayout,
                *row5 = new QHBoxLayout;
    rows->addLayout(row1);
    rows->addLayout(row2);
    rows->addLayout(row3);
    rows->addLayout(row4);
    rows->addLayout(row5);

    row1->addWidget(typeLabel);
    row1->addStretch();
    row1->addWidget(isNameButton_);
    row1->addWidget(isTagButton_);

    row2->addWidget(languageLabel);
    row2->addStretch();
    row2->addWidget(isEnglishButton_);
    row2->addWidget(isJapaneseButton_);
    row2->addWidget(isChineseButton_);
    row2->addWidget(isAlienButton_);

    row3->addWidget(tagLabel);
    row3->addStretch();
    row3->addWidget(tagBDButton);
    row3->addWidget(tagDVDButton);
    row3->addWidget(tagTVButton);
    row3->addWidget(tagWebButton);

    row4->addWidget(aliasLabel);
    row4->addWidget(aliasEdit_);

    row5->addWidget(okButton_);
    row5->addWidget(cancelButton_);
  } setLayout(rows);

  // Initial status
  invalidateOKButton();
  aliasEdit_->setFocus();
}

// - Properties -

quint32
AddAliasDialog::languageFlags() const
{
  int ret = 0;
  if (isEnglishButton_->isChecked()) ret |= Traits::English;
  if (isJapaneseButton_->isChecked()) ret |= Traits::Japanese;
  if (isChineseButton_->isChecked()) ret |= Traits::Chinese;
  if (isAlienButton_->isChecked()) ret |= Traits::UnknownLanguage;
  return ret;
}

// - Slots -

void
AddAliasDialog::tag(const QString &key)
{
  QString alias = aliasEdit_->text().trimmed();
  QString t = QString("[%1]").arg(key);
  alias.remove(t, Qt::CaseInsensitive);
  alias = alias.trimmed();

  if (alias.isEmpty())
    alias = t;
  else if (alias.endsWith(']'))
    alias += t;
  else
    alias = QString("%1 %2").arg(alias).arg(t);

  aliasEdit_->setText(alias);
}

// - Events -

void
AddAliasDialog::setTypeToName(bool t)
{
  isNameButton_->setChecked(t);
  isTagButton_->setChecked(!t);
}

void
AddAliasDialog::setTypeToTag(bool t)
{
  isNameButton_->setChecked(!t);
  isTagButton_->setChecked(t);
}

void
AddAliasDialog::invalidateOKButton()
{
  if (languageFlags()) {
    okButton_->setEnabled(true);
    okButton_->setToolTip(TR(T_OK));
  } else {
    okButton_->setEnabled(false);
    okButton_->setToolTip(tr("Please select alias language"));
  }
}

void
AddAliasDialog::ok()
{
  hide();

  QString alias = aliasEdit_->text().trimmed();
  if (alias.isEmpty())
    return;
  quint32 l = languageFlags();
  if (!l)
    return;

  int type = isNameButton_->isChecked() ? Alias::AT_Name : Alias::AT_Tag;
  emit aliasAdded(alias, type, l);
}

void
AddAliasDialog::cancel()
{ hide(); }

// EOF
