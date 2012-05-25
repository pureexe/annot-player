// preferences_p.cc
// 5/25/2012

#include "preferences_p.h"
#include "annotationsettings.h"
#include "global.h"
#include "tr.h"
#include "project/common/acss.h"
#include "project/common/acui.h"
#include "module/qtext/fontcombobox.h"
#include <QtGui>

//enum { FONTCOMBOBOX_WIDTH = 100, FONTSIZECOMBOBOX_WIDTH = 60 };

// - Construction -

AnnotationPreferencesTab::AnnotationPreferencesTab(AnnotationSettings *settings, QWidget *parent)
  : Base(parent), settings_(settings)
{
  Q_ASSERT(settings_);
  setWindowTitle(TR(T_ANNOTATION));
  createLayout();

  connect(settings_, SIGNAL(offsetChanged(int)), SLOT(loadOffsetIfVisible()));
}

void
AnnotationPreferencesTab::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  fontEdit_ = new QtExt::FontComboBox; {
    fontEdit_->setStyleSheet(SS_COMBOBOX);
    //fontEdit_->setMaximumWidth(FONTCOMBOBOX_WIDTH);
    //fontEdit_->setMinimumWidth(FONTCOMBOBOX_WIDTH);
    fontEdit_->setToolTip(tr("Font Family"));
  }
  connect(fontEdit_, SIGNAL(activated(QString)), SLOT(saveFontFamily()));

  QLabel *fontLabel = ui->makeLabel(AcUi::BuddyHint, tr("Font"), tr("Font Family"), fontEdit_);
  QToolButton *fontReset = ui->makeToolButton(AcUi::PushHint, TR(T_RESET), this, SLOT(resetFontFamily()));

  QStringList defoffsets = QStringList()
    << "0" << "10" << "30" << "60" << "3000"
           << "-10" << "-30" << "-60" << "-3000";
  offsetEdit_ = ui->makeComboBox(AcUi::EditHint, "", TR(T_SECOND), "", defoffsets);
  offsetEdit_->lineEdit()->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  connect(offsetEdit_, SIGNAL(editTextChanged(QString)), SLOT(saveOffset()));

  offsetEditStyleSheet_ = offsetEdit_->styleSheet();

  QLabel *offsetLabel = ui->makeLabel(AcUi::BuddyHint, tr("Offset"), TR(T_SECOND), offsetEdit_);
  QToolButton *offsetReset = ui->makeToolButton(AcUi::PushHint, TR(T_RESET), this, SLOT(resetOffset()));

  // Layouts
  QGridLayout *grid = new QGridLayout; {
    // (row, col, rowspan, colspan, alignment)
    int r, c;
    grid->addWidget(fontLabel, r=0, c=0, Qt::AlignRight);
    grid->addWidget(fontEdit_, r, ++c);
    grid->addWidget(fontReset, r, ++c);

    //grid->addWidget(offsetLabel, ++r, c=0);
    grid->addWidget(offsetLabel, r, ++c, Qt::AlignRight);
    grid->addWidget(offsetEdit_, r, ++c);
    grid->addWidget(offsetReset, r, ++c);

    //grid->addWidget(cancelButton, ++r, c=0, Qt::AlignHCenter);
    //grid->addWidget(loginButton, r, ++c, Qt::AlignHCenter);

    grid->setContentsMargins(9, 9, 9, 9);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(grid);
}

// - Save/Load -

bool
AnnotationPreferencesTab::save()
{
  saveFontFamily();
  saveOffset();
  return true;
}

void
AnnotationPreferencesTab::load()
{
  loadFontFamily();
  loadOffset();
}

void
AnnotationPreferencesTab::loadFontFamily()
{ fontEdit_->setCurrentFont(QFont(settings_->fontFamily())); }

void
AnnotationPreferencesTab::saveFontFamily()
{
  QString font = fontEdit_->currentFont().family();
  settings_->setFontFamily(font);
  emit message(tr("font saved"));
}

void
AnnotationPreferencesTab::resetFontFamily()
{
  settings_->resetFontFamily();
  loadFontFamily();
}

void
AnnotationPreferencesTab::loadOffset()
{ offsetEdit_->setEditText(QString::number(settings_->offset())); }

void
AnnotationPreferencesTab::saveOffset()
{
  bool ok;
  int offset = offsetEdit_->currentText().trimmed().toInt(&ok);
  if (ok) {
    settings_->setOffset(offset);
    emit message(tr("offset saved"));
  } else
    emit warning(tr("invalid offset"));
  offsetEdit_->setStyleSheet(offsetEditStyleSheet_ + (ok ?
    SS_BEGIN(QComboBox)
      SS_COLOR(black)
    SS_END
    :
    SS_BEGIN(QComboBox)
      SS_COLOR(red)
    SS_END
  ));
}

void
AnnotationPreferencesTab::resetOffset()
{
  settings_->resetOffset();
  loadOffset();
}

// EOF
