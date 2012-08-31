// annotationprefs.cc
// 5/25/2012

#include "annotationprefs.h"
#include "annotationsettings.h"
#include "global.h"
#include "tr.h"
#include "project/common/acss.h"
#include "project/common/acui.h"
#include "module/qtext/doublespinbox.h"
#include "module/qtext/spinbox.h"
#include "module/qtext/fontcombobox.h"
#include "module/qtext/toolbutton.h"
#include <QtGui>

enum { FONTEDIT_WIDTH = 100 };
enum { SPINEDIT_WIDTH = 60 };

enum { SPEED_FACTOR = 10 };
enum { SCALE_FACTOR = 10 };

#define ICON_SIZE QSize(32, 16)

// - Construction -

void
AnnotationPreferencesTab::init()
{
  if (!settings_)
    settings_ = AnnotationSettings::globalSettings();
  setWindowTitle(TR(T_ANNOTATION));
  createLayout();

  connect(settings_, SIGNAL(offsetChanged(int)), SLOT(loadOffsetIfVisible()));
  connect(settings_, SIGNAL(speedupChanged(qreal)), SLOT(loadSpeedIfVisible()));
}

void
AnnotationPreferencesTab::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  annotColorButton_ = new QtExt::ToolButton; {
    annotColorButton_->setStyleSheet(SS_TRANSPARENT);
    annotColorButton_->setToolTip(tr("Annotation Color"));
  }
  connect(annotColorButton_, SIGNAL(clicked()), SLOT(promptAnnotationColor()));

  QLabel *annotColorLabel = ui->makeLabel(AcUi::BuddyHint, TR(T_ANNOTATION), tr("Halo Color"), annotColorButton_);
  QToolButton *annotColorReset = ui->makeToolButton(AcUi::PushHint, TR(T_DEFAULT), this, SLOT(resetAnnotationColor()));

  subtitleColorButton_ = new QtExt::ToolButton; {
    subtitleColorButton_->setStyleSheet(SS_TRANSPARENT);
    subtitleColorButton_->setToolTip(tr("Subtitle Color"));
  }
  connect(subtitleColorButton_, SIGNAL(clicked()), SLOT(promptSubtitleColor()));

  QLabel *subtitleColorLabel = ui->makeLabel(AcUi::BuddyHint, TR(T_SUBTITLE), tr("Halo Color"), subtitleColorButton_);
  QToolButton *subtitleColorReset = ui->makeToolButton(AcUi::PushHint, TR(T_DEFAULT), this, SLOT(resetSubtitleColor()));

  highlightColorButton_ = new QtExt::ToolButton; {
    highlightColorButton_->setStyleSheet(SS_TRANSPARENT);
    highlightColorButton_->setToolTip(tr("Highlight Color"));
  } connect(highlightColorButton_, SIGNAL(clicked()), SLOT(promptHighlightColor()));
  QLabel *highlightColorLabel = ui->makeLabel(AcUi::BuddyHint, tr("Highlight"), tr("Halo Color"), highlightColorButton_);
  QToolButton *highlightColorReset = ui->makeToolButton(AcUi::PushHint, TR(T_DEFAULT), this, SLOT(resetHighlightColor()));

  fontEdit_ = new QtExt::FontComboBox; {
    fontEdit_->setStyleSheet(ACSS_COMBOBOX);
    fontEdit_->setFixedWidth(FONTEDIT_WIDTH);
    fontEdit_->setToolTip(tr("Font Family"));
  } connect(fontEdit_, SIGNAL(activated(QString)), SLOT(saveFont()));
  QLabel *fontLabel = ui->makeLabel(AcUi::BuddyHint, TR(T_DEFAULT), tr("Font Family"), fontEdit_);
  QToolButton *fontReset = ui->makeToolButton(AcUi::PushHint, TR(T_DEFAULT), this, SLOT(resetFont()));

  japaneseFontEdit_ = new QtExt::FontComboBox; {
    japaneseFontEdit_->setStyleSheet(ACSS_COMBOBOX);
    japaneseFontEdit_->setFixedWidth(FONTEDIT_WIDTH);
    japaneseFontEdit_->setToolTip(tr("Font Family"));
  } connect(japaneseFontEdit_, SIGNAL(activated(QString)), SLOT(saveJapaneseFont()));
  QLabel *japaneseFontLabel = ui->makeLabel(AcUi::BuddyHint, TR(T_JAPANESE), tr("Font Family"), japaneseFontEdit_);
  QToolButton *japaneseFontReset = ui->makeToolButton(AcUi::PushHint, TR(T_DEFAULT), this, SLOT(resetJapaneseFont()));

  chineseFontEdit_ = new QtExt::FontComboBox; {
    chineseFontEdit_->setStyleSheet(ACSS_COMBOBOX);
    chineseFontEdit_->setFixedWidth(FONTEDIT_WIDTH);
    chineseFontEdit_->setToolTip(tr("Font Family"));
  } connect(chineseFontEdit_, SIGNAL(activated(QString)), SLOT(saveChineseFont()));
  QLabel *chineseFontLabel = ui->makeLabel(AcUi::BuddyHint, TR(T_CHINESE), tr("Font Family"), chineseFontEdit_);
  QToolButton *chineseFontReset = ui->makeToolButton(AcUi::PushHint, TR(T_DEFAULT), this, SLOT(resetChineseFont()));

  offsetEdit_ = new QtExt::SpinBox; {
    enum { TimeRange = 3600 * 3 }; // 3 houre
    offsetEdit_->setSuffix(tr(" sec."));
    offsetEdit_->setMaximum(TimeRange);
    offsetEdit_->setMinimum(-TimeRange);
    offsetEdit_->setToolTip(TR(T_SECOND));
    offsetEdit_->setFixedWidth(SPINEDIT_WIDTH);
    offsetEdit_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  } connect(offsetEdit_, SIGNAL(valueChanged(int)), SLOT(saveOffset()));
  QLabel *offsetLabel = ui->makeLabel(AcUi::BuddyHint, tr("Delay Time"), tr("Delay time"), offsetEdit_);
  QToolButton *offsetReset = ui->makeToolButton(AcUi::PushHint, TR(T_RESET), this, SLOT(resetOffset()));

  opacityEdit_ = new QtExt::SpinBox; {
    enum { Min = 50, Max = 100 };
    opacityEdit_->setSuffix(" %");
    opacityEdit_->setMaximum(Max);
    opacityEdit_->setMinimum(Min);
    opacityEdit_->setToolTip(tr("Font Opacity"));
    opacityEdit_->setFixedWidth(SPINEDIT_WIDTH);
    opacityEdit_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  } connect(opacityEdit_, SIGNAL(valueChanged(int)), SLOT(saveOpacity()));
  QLabel *opacityLabel = ui->makeLabel(AcUi::BuddyHint, tr("Font Opacity"), tr("Annotation Transparency"), opacityEdit_);
  QToolButton *opacityReset = ui->makeToolButton(AcUi::PushHint, TR(T_DEFAULT), this, SLOT(resetOpacity()));

  backgroundOpacityEdit_ = new QtExt::SpinBox; {
    enum { Min = 0, Max = 80 };
    backgroundOpacityEdit_->setSuffix(" %");
    backgroundOpacityEdit_->setMaximum(Max);
    backgroundOpacityEdit_->setMinimum(Min);
    backgroundOpacityEdit_->setToolTip(tr("Background Opacity") + " (" + tr("Galgame Only") + ")");
    backgroundOpacityEdit_->setFixedWidth(SPINEDIT_WIDTH);
    backgroundOpacityEdit_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  } connect(backgroundOpacityEdit_, SIGNAL(valueChanged(int)), SLOT(saveBackgroundOpacity()));
  QLabel *backgroundOpacityLabel = ui->makeLabel(AcUi::BuddyHint, tr("Background Opacity"), tr("Background Opacity") + " (" + tr("Galgame Only") + ")", backgroundOpacityEdit_);
  QToolButton *backgroundOpacityReset = ui->makeToolButton(AcUi::PushHint, TR(T_DEFAULT), this, SLOT(resetBackgroundOpacity()));

  speedEdit_ = new QtExt::DoubleSpinBox; {
    Q_ASSERT(MinSpeed);
    speedEdit_->setPrefix("x ");
    speedEdit_->setMaximum(1.0 * SPEED_FACTOR);
    speedEdit_->setMinimum(1.0 / SPEED_FACTOR);
    speedEdit_->setSingleStep(1.0 / (SPEED_FACTOR * 2));
    speedEdit_->setToolTip(tr("Speedup") + " x" + QString::number(SPEED_FACTOR));
    speedEdit_->setFixedWidth(SPINEDIT_WIDTH);
    speedEdit_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  } connect(speedEdit_, SIGNAL(valueChanged(double)), SLOT(saveSpeed()));
  QLabel *speedLabel = ui->makeLabel(AcUi::BuddyHint, tr("Moving Speed"), tr("Moving speed, the larger the faster"), speedEdit_);
  QToolButton *speedReset = ui->makeToolButton(AcUi::PushHint, TR(T_DEFAULT), this, SLOT(resetSpeed()));

  scaleEdit_ = new QtExt::DoubleSpinBox; {
    //enum { MinSize = SCALE_FACTOR/10, MaxSize = SCALE_FACTOR*10 };
    scaleEdit_->setPrefix("x ");
    scaleEdit_->setMaximum(1.0 * SCALE_FACTOR);
    scaleEdit_->setMinimum(1.0 / SCALE_FACTOR);
    scaleEdit_->setSingleStep(1.0 / (SCALE_FACTOR * 2));
    scaleEdit_->setToolTip(tr("Scale"));
    scaleEdit_->setFixedWidth(SPINEDIT_WIDTH);
    scaleEdit_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  } connect(scaleEdit_, SIGNAL(valueChanged(double)), SLOT(saveScale()));
  QLabel *scaleLabel = ui->makeLabel(AcUi::BuddyHint, tr("Normal Size"), tr("Annotation size"), scaleEdit_);
  QToolButton *scaleReset = ui->makeToolButton(AcUi::PushHint, TR(T_DEFAULT), this, SLOT(resetScale()));

  fullscreenScaleEdit_ = new QtExt::DoubleSpinBox; {
    //enum { MinSize = SCALE_FACTOR/10, MaxSize = SCALE_FACTOR*10 };
    fullscreenScaleEdit_->setPrefix("x ");
    fullscreenScaleEdit_->setMaximum(1.0 * SCALE_FACTOR);
    fullscreenScaleEdit_->setMinimum(1.0 / SCALE_FACTOR);
    fullscreenScaleEdit_->setSingleStep(1.0 / (SCALE_FACTOR * 2));
    fullscreenScaleEdit_->setToolTip(tr("Scale"));
    fullscreenScaleEdit_->setFixedWidth(SPINEDIT_WIDTH);
    fullscreenScaleEdit_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  } connect(fullscreenScaleEdit_, SIGNAL(valueChanged(double)), SLOT(saveFullscreenScale()));
  QLabel *fullscreenScaleLabel = ui->makeLabel(AcUi::BuddyHint, tr("Fullscreen Size"), tr("Annotation size in fullscreen"), fullscreenScaleEdit_);
  QToolButton *fullscreenScaleReset = ui->makeToolButton(AcUi::PushHint, TR(T_DEFAULT), this, SLOT(resetFullscreenScale()));

  // See: http://qt-project.org/doc/qt-4.8/stylesheet-examples.html#customizing-qspinbox
  //offsetEdit_->setStyleSheet(
  //  SS_BEGIN(QSpinBox)
  //    SS_BORDER_IMAGE_URL(ACRC_IMAGE_BACKGROUND)
  //    SS_BORDER_WIDTH(4px)
  //    SS_BORDER_RADIUS(3px)
  //  SS_END
  //);

  // Layouts
  QGridLayout *grid = new QGridLayout; {
    // (row, col, rowspan, colspan, alignment)
    int r, c;
    grid->addWidget(japaneseFontLabel, r=0, c=0, Qt::AlignRight);
    grid->addWidget(japaneseFontEdit_, r, ++c);
    grid->addWidget(japaneseFontReset, r, ++c);

    grid->addWidget(annotColorLabel, r, ++c, Qt::AlignRight);
    grid->addWidget(annotColorButton_, r, ++c);
    grid->addWidget(annotColorReset, r, ++c);

    grid->addWidget(chineseFontLabel, ++r, c=0, Qt::AlignRight);
    grid->addWidget(chineseFontEdit_, r, ++c);
    grid->addWidget(chineseFontReset, r, ++c);

    grid->addWidget(subtitleColorLabel, r, ++c, Qt::AlignRight);
    grid->addWidget(subtitleColorButton_, r, ++c);
    grid->addWidget(subtitleColorReset, r, ++c);

    grid->addWidget(fontLabel, ++r, c=0, Qt::AlignRight);
    grid->addWidget(fontEdit_, r, ++c);
    grid->addWidget(fontReset, r, ++c);

    grid->addWidget(highlightColorLabel, r, ++c, Qt::AlignRight);
    grid->addWidget(highlightColorButton_, r, ++c);
    grid->addWidget(highlightColorReset, r, ++c);

    grid->addWidget(speedLabel, ++r, c=0, Qt::AlignRight);
    grid->addWidget(speedEdit_, r, ++c);
    grid->addWidget(speedReset, r, ++c);

    grid->addWidget(offsetLabel, r, ++c, Qt::AlignRight);
    grid->addWidget(offsetEdit_, r, ++c);
    grid->addWidget(offsetReset, r, ++c);

    grid->addWidget(scaleLabel, ++r, c=0, Qt::AlignRight);
    grid->addWidget(scaleEdit_, r, ++c);
    grid->addWidget(scaleReset, r, ++c);

    grid->addWidget(fullscreenScaleLabel, r, ++c, Qt::AlignRight);
    grid->addWidget(fullscreenScaleEdit_, r, ++c);
    grid->addWidget(fullscreenScaleReset, r, ++c);

    grid->addWidget(opacityLabel, ++r, c=0, Qt::AlignRight);
    grid->addWidget(opacityEdit_, r, ++c);
    grid->addWidget(opacityReset, r, ++c);

#ifdef AC_ENABLE_GAME
    grid->addWidget(backgroundOpacityLabel, r, ++c, Qt::AlignRight);
    grid->addWidget(backgroundOpacityEdit_, r, ++c);
    grid->addWidget(backgroundOpacityReset, r, ++c);
#endif // AC_ENABLE_GAME

    //grid->addWidget(cancelButton, ++r, c=0, Qt::AlignHCenter);
    //grid->addWidget(loginButton, r, ++c, Qt::AlignHCenter);

    //grid->setAlignment(Qt::AlignRight);
    grid->setContentsMargins(9, 9, 9, 9);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(grid);
}

// - Save/Load -

bool
AnnotationPreferencesTab::save()
{
  saveFont();
  saveChineseFont();
  saveJapaneseFont();
  saveOffset();
  saveSpeed();
  saveBackgroundOpacity();
  saveOpacity();

  //saveScale(); // avoid loosing precision
  //saveFullscreenScale(); // avoid loosing precision

  //saveAnnotationColor();
  //saveSubtitleColor();
  //saveHighlightColor();
  return true;
}

void
AnnotationPreferencesTab::load()
{
  loadFont();
  loadChineseFont();
  loadJapaneseFont();
  loadOffset();
  loadSpeed();
  loadBackgroundOpacity();
  loadOpacity();
  loadScale();
  loadFullscreenScale();
  loadAnnotationColor();
  loadSubtitleColor();
  loadHighlightColor();
}

void
AnnotationPreferencesTab::loadFont()
{
  // Has to load twice. no idea why ...
  fontEdit_->setCurrentFont(QFont(settings_->fontFamily()));
  fontEdit_->setCurrentFont(QFont(settings_->fontFamily()));
}

void
AnnotationPreferencesTab::saveFont()
{
  QString ff = fontEdit_->currentFont().family();
  settings_->setFontFamily(ff);
  emit message(tr("font saved"));
}

void
AnnotationPreferencesTab::resetFont()
{
  settings_->resetFont();
  loadFont();
  emit message(tr("font saved"));
}

void
AnnotationPreferencesTab::loadJapaneseFont()
{ japaneseFontEdit_->setCurrentFont(QFont(settings_->japaneseFontFamily())); }

void
AnnotationPreferencesTab::saveJapaneseFont()
{
  QString ff = japaneseFontEdit_->currentFont().family();
  settings_->setJapaneseFontFamily(ff);
  emit message(tr("Japanese font saved"));
}

void
AnnotationPreferencesTab::resetJapaneseFont()
{
  settings_->resetJapaneseFont();
  loadJapaneseFont();
  emit message(tr("Japanese font saved"));
}

void
AnnotationPreferencesTab::loadChineseFont()
{ chineseFontEdit_->setCurrentFont(QFont(settings_->chineseFontFamily())); }

void
AnnotationPreferencesTab::saveChineseFont()
{
  QString ff = chineseFontEdit_->currentFont().family();
  settings_->setChineseFontFamily(ff);
  emit message(tr("Chinese font saved"));
}

void
AnnotationPreferencesTab::resetChineseFont()
{
  settings_->resetChineseFont();
  loadChineseFont();
  emit message(tr("Chinese font saved"));
}

void
AnnotationPreferencesTab::loadOffset()
{ offsetEdit_->setValue(settings_->offset()); }

void
AnnotationPreferencesTab::saveOffset()
{
  settings_->setOffset(offsetEdit_->value());
  emit message(tr("offset saved"));
}

void
AnnotationPreferencesTab::resetOffset()
{
  settings_->resetOffset();
  loadOffset();
  emit message(tr("offset saved"));
}

void
AnnotationPreferencesTab::loadScale()
{ scaleEdit_->setValue(settings_->scale()); }

void
AnnotationPreferencesTab::saveScale()
{
  settings_->setScale(scaleEdit_->value());
  emit message(tr("annotation size saved"));
}

void
AnnotationPreferencesTab::resetScale()
{
  settings_->resetScale();
  loadScale();
  emit message(tr("annotation size saved"));
}

void
AnnotationPreferencesTab::loadFullscreenScale()
{ fullscreenScaleEdit_->setValue(settings_->fullscreenScale()); }

void
AnnotationPreferencesTab::saveFullscreenScale()
{
  settings_->setFullscreenScale(fullscreenScaleEdit_->value());
  emit message(tr("annotation size saved"));
}

void
AnnotationPreferencesTab::resetFullscreenScale()
{
  settings_->resetFullscreenScale();
  loadFullscreenScale();
  emit message(tr("annotation size saved"));
}

void
AnnotationPreferencesTab::loadSpeed()
{ speedEdit_->setValue(settings_->speedup()); }

void
AnnotationPreferencesTab::saveSpeed()
{
  settings_->setSpeedup(speedEdit_->value());
  emit message(tr("annotation speed saved"));
}

void
AnnotationPreferencesTab::resetSpeed()
{
  settings_->resetSpeedup();
  loadSpeed();
  emit message(tr("annotation speed saved"));
}

void
AnnotationPreferencesTab::loadOpacity()
{ opacityEdit_->setValue(settings_->opacityFactor()); }

void
AnnotationPreferencesTab::saveOpacity()
{
  settings_->setOpacityFactor(opacityEdit_->value());
  emit message(tr("font opacity saved"));
}

void
AnnotationPreferencesTab::resetOpacity()
{
  settings_->resetOpacityFactor();
  loadOpacity();
  emit message(tr("font opacity saved"));
}

void
AnnotationPreferencesTab::loadBackgroundOpacity()
{ backgroundOpacityEdit_->setValue(settings_->backgroundOpacityFactor()); }

void
AnnotationPreferencesTab::saveBackgroundOpacity()
{
  settings_->setBackgroundOpacityFactor(backgroundOpacityEdit_->value());
  emit message(tr("background opacity saved"));
}

void
AnnotationPreferencesTab::resetBackgroundOpacity()
{
  settings_->resetBackgroundOpacityFactor();
  loadBackgroundOpacity();
  emit message(tr("background opacity saved"));
}

// - Color -

void
AnnotationPreferencesTab::promptAnnotationColor()
{
  QColor c = QColorDialog::getColor(settings_->outlineColor(), this, tr("Annotation Color"));
  if (c.isValid() &&
      c != settings_->outlineColor()) {
    settings_->setOutlineColor(c);
    loadAnnotationColor();
    emit message(tr("annotation color saved"));
  }
}

void
AnnotationPreferencesTab::loadAnnotationColor()
{
  QPixmap icon(ICON_SIZE);
  icon.fill(settings_->outlineColor());
  annotColorButton_->setIcon(icon);
  annotColorButton_->setIconSize(icon.size());
}

void
AnnotationPreferencesTab::resetAnnotationColor()
{
  settings_->resetOutlineColor();
  loadAnnotationColor();
  emit message(tr("annotation color saved"));
}

void
AnnotationPreferencesTab::promptSubtitleColor()
{
  QColor c = QColorDialog::getColor(settings_->subtitleColor(), this, tr("Subtitle Color"));
  if (c.isValid() &&
      c != settings_->subtitleColor()) {
    settings_->setSubtitleColor(c);
    loadSubtitleColor();
    emit message(tr("subtitle color saved"));
  }
}

void
AnnotationPreferencesTab::loadSubtitleColor()
{
  QPixmap icon(ICON_SIZE);
  icon.fill(settings_->subtitleColor());
  subtitleColorButton_->setIcon(icon);
  subtitleColorButton_->setIconSize(icon.size());
}

void
AnnotationPreferencesTab::resetSubtitleColor()
{
  settings_->resetSubtitleColor();
  loadSubtitleColor();
  emit message(tr("subtitle color saved"));
}

void
AnnotationPreferencesTab::promptHighlightColor()
{
  QColor c = QColorDialog::getColor(settings_->highlightColor(), this, tr("Highlight Color"));
  if (c.isValid() &&
      c != settings_->highlightColor()) {
    settings_->setHighlightColor(c);
    loadHighlightColor();
    emit message(tr("highlight color saved"));
  }
}

void
AnnotationPreferencesTab::loadHighlightColor()
{
  QPixmap icon(ICON_SIZE);
  icon.fill(settings_->highlightColor());
  highlightColorButton_->setIcon(icon);
  highlightColorButton_->setIconSize(icon.size());
}

void
AnnotationPreferencesTab::resetHighlightColor()
{
  settings_->resetHighlightColor();
  loadHighlightColor();
  emit message(tr("highlight color saved"));
}

// EOF
