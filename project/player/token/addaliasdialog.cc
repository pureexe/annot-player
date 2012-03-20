// addaliasdialog.cc
// 10/13/2011

#include "addaliasdialog.h"
#include "uistyle.h"
#include "tr.h"
#include "module/annotcloud/traits.h"
#include "module/annotcloud/alias.h"
#include <QtGui>

using namespace AnnotCloud;

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowCloseButtonHint | \
  Qt::WindowStaysOnTopHint )

// - Panel -
AddAliasDialog::AddAliasDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  setWindowTitle(tr("Add Alias"));
  UiStyle::globalInstance()->setWindowStyle(this);

  createLayout();

  // Shortcuts
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence::Close, this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(hide()));

  // Initial status
  invalidateOKButton();
  aliasEdit_->setFocus();
}

void
AddAliasDialog::createLayout()
{
  UiStyle *ui = UiStyle::globalInstance();

  QStringList defvals = QStringList()
      << "DEATHNOTE 01"
      << "デスノート 01"
      << "死亡笔记 01"
      << "集英社"
      << "http://www.youtube.com/watch?v=koeaZ_z1WbI";

  aliasEdit_ = ui->makeComboBox(UiStyle::EditHint, "", TR(T_TOOLTIP_ADDALIAS), defvals);
  connect(aliasEdit_->lineEdit(), SIGNAL(returnPressed()), SLOT(ok()));

  QLabel *tagLabel = ui->makeLabel(UiStyle::BuddyHint, TR(T_TAG)),
         *typeLabel = ui->makeLabel(UiStyle::BuddyHint, TR(T_TYPE)),
         *languageLabel = ui->makeLabel(UiStyle::BuddyHint, TR(T_LANGUAGE)),
         *aliasLabel = ui->makeLabel(UiStyle::BuddyHint, TR(T_ALIAS));

#define MAKE_TAG(_id) \
  QToolButton *tag##_id##Button = ui->makeToolButton(0, #_id, this, SLOT(tag##_id())); { \
    QFont font = tag##_id##Button->font(); \
    font.setItalic(true); \
    tag##_id##Button->setFont(font); \
  }

  MAKE_TAG(01) MAKE_TAG(02) MAKE_TAG(03) MAKE_TAG(04) MAKE_TAG(05)
  MAKE_TAG(OVA)
  MAKE_TAG(OAD)
#undef MAKE_TAG

#define MAKE_LANGUAGE(_id, _styleid) \
  is##_id##Button_ = ui->makeToolButton(0, TR(T_##_styleid), this, SLOT(invalidateOKButton())); { \
    is##_id##Button_->setCheckable(true); \
    QFont font = is##_id##Button_->font(); \
    font.setUnderline(true); \
    is##_id##Button_->setFont(font); \
  }

  MAKE_LANGUAGE(English, ENGLISH)
  MAKE_LANGUAGE(Japanese, JAPANESE)
  MAKE_LANGUAGE(Chinese, CHINESE)
  MAKE_LANGUAGE(Alien, ALIEN)
#undef MAKE_LANGUAGE

#define MAKE_TYPE(_id, _text, _tip) \
  is##_id##Button_ = ui->makeToolButton(0, _text, _tip); { \
    is##_id##Button_->setCheckable(true); \
    QFont font = is##_id##Button_->font(); \
    font.setBold(true); \
    is##_id##Button_->setFont(font); \
  } \
  connect(is##_id##Button_, SIGNAL(clicked(bool)), SLOT(setTypeTo##_id(bool)));

  MAKE_TYPE(Name, tr("name"), tr("Add a name to the token"))
  MAKE_TYPE(Tag, tr("tag"), tr("Add a tag to the token"))
  MAKE_TYPE(Url, tr("URL"), tr("Add source URL to the token"))
#undef MAKE_TYPE
  isNameButton_->setChecked(true);

  okButton_ = ui->makeToolButton(
        UiStyle::PushHint | UiStyle::HighlightHint , TR(T_ADD), this, SLOT(ok()));
  cancelButton_ = ui->makeToolButton(
        UiStyle::PushHint, TR(T_CANCEL), this, SLOT(hide()));
  QToolButton *pasteButton = ui->makeToolButton(
        UiStyle::PushHint, TR(T_PASTE), this, SLOT(paste()));

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
    row1->addWidget(isUrlButton_);

    row2->addWidget(languageLabel);
    row2->addStretch();
    row2->addWidget(isEnglishButton_);
    row2->addWidget(isJapaneseButton_);
    row2->addWidget(isChineseButton_);
    row2->addWidget(isAlienButton_);

    row3->addWidget(tagLabel);
    row3->addStretch();
    row3->addWidget(tag01Button);
    row3->addWidget(tag02Button);
    row3->addWidget(tag03Button);
    row3->addWidget(tag04Button);
    row3->addWidget(tag05Button);
    row3->addWidget(tagOVAButton);
    row3->addWidget(tagOADButton);

    row4->addWidget(aliasLabel);
    row4->addWidget(aliasEdit_);

    row5->addWidget(cancelButton_);
    row5->addWidget(pasteButton);
    row5->addStretch();
    row5->addWidget(okButton_);

    row1->setContentsMargins(0, 0, 0, 0);
    row2->setContentsMargins(0, 0, 0, 0);
    row3->setContentsMargins(0, 0, 0, 0);
    row4->setContentsMargins(0, 0, 0, 0);
    row5->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(6, 6, 6, 6);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(rows);
}

// - Properties -

quint32
AddAliasDialog::languageFlags() const
{
  if (isAlienButton_->isChecked()) return Traits::UnknownLanguage;
  else if (isChineseButton_->isChecked()) return Traits::Chinese;
  else if (isEnglishButton_->isChecked()) return Traits::English;
  else if (isJapaneseButton_->isChecked()) return Traits::Japanese;
  else return Traits::UnknownLanguage;
}

// - Slots -

//void
//AddAliasDialog::postfix(const QString &tag)
//{
//  QString text = aliasEdit_->currentText().trimmed();
//  if (text.isEmpty())
//    text = tag;
//  else {
//    text.remove(QRegExp(" \\d*$"));
//    text.remove(QRegExp(" " + tag + "$"));
//    text = text.trimmed();
//    if (text.isEmpty())
//      text = tag;
//    else
//      text += " " + tag;
//  }
//  aliasEdit_->setEditText(text);
//}

void
AddAliasDialog::tag(const QString &key)
{
  QString alias = aliasEdit_->currentText().trimmed();
  QString t = QString("[%1]").arg(key);
  if (!alias.isEmpty()) {
    alias.remove(QRegExp("\\[\\d+\\]$"));
    alias.remove(t, Qt::CaseInsensitive);
  }
  alias = alias.trimmed();

  if (alias.isEmpty())
    alias = t;
  else if (alias.endsWith(']'))
    alias += t;
  else
    alias = QString("%1 %2").arg(alias).arg(t);

  aliasEdit_->setEditText(alias);
}

// - Events -

void
AddAliasDialog::setTypeToName(bool t)
{
  isNameButton_->setChecked(t);
  isTagButton_->setChecked(!t);
  isUrlButton_->setChecked(!t);
}

void
AddAliasDialog::setTypeToTag(bool t)
{
  isNameButton_->setChecked(!t);
  isTagButton_->setChecked(t);
  isUrlButton_->setChecked(!t);
}

void
AddAliasDialog::setTypeToUrl(bool t)
{
  isNameButton_->setChecked(!t);
  isTagButton_->setChecked(!t);
  isUrlButton_->setChecked(t);
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
AddAliasDialog::paste()
{
  QClipboard *clipboard = QApplication::clipboard();
  if (clipboard)
    aliasEdit_->setEditText(clipboard->text());
}

void
AddAliasDialog::ok()
{
  hide();

  QString alias = aliasEdit_->currentText().trimmed();
  if (alias.isEmpty())
    return;
  qint32 lang = languageFlags();
  if (!lang)
    return;

  int type = isNameButton_->isChecked() ? Alias::AT_Name :
             isUrlButton_->isChecked() ? Alias::AT_Url :
                                         Alias::AT_Tag;
  if (type == Alias::AT_Url)
    lang = Alias::guessUrlLanguage(alias, lang);

  emit aliasAdded(alias, type, lang);
}

// EOF
