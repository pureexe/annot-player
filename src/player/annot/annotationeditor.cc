// annotationeditor.cc
// 10/4/2011

#include "annotationeditor.h"
#include "tr.h"
#include "global.h"
#include "stylesheet.h"
#include "src/common/acui.h"
#include "lib/annotcloud/annottag.h"
#include "lib/annotcloud/annothtml.h"
#include "qtx/qxtoolbutton.h"
#include "qtx/qxcombobox.h"
#include "qtx/qxfontcombobox.h"
#include <QtGui>
#include <boost/tuple/tuple.hpp>

#ifdef Q_OS_MAC
# define K_CTRL        "cmd"
#else
# define K_CTRL        "Ctrl"
#endif // Q_OS_MAC

//#define DEBUG "annotationeditor"
#include "qtx/qxdebug.h"

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint )

#define FONTCOMBOBOX_WIDTH           100
#define FONTSIZECOMBOBOX_WIDTH       60
#define ALIGNCOMBOBOX_WIDTH          80
#define MOVESTYLECOMBOBOX_WIDTH      60
#define RENDEREFFECTCOMBOBOX_WIDTH   60

#ifdef Q_OS_MAC
  #define SAVE_SHORTCUT "CMD+S"
#else
  #define SAVE_SHORTCUT "CTRL+S"
#endif // Q_OS_MAC

#define SS_TOOLBUTTON_TAG       ACSS_TOOLBUTTON_TEXT_(bold, italic, none, white, red, purple, purple, red, gray)
#define SS_TOOLBUTTON_BOLD      ACSS_TOOLBUTTON_TEXT_(bold, italic, none, white, red, purple, purple, red, gray)
#define SS_TOOLBUTTON_ITALIC    ACSS_TOOLBUTTON_TEXT_(bold, italic, none, white, red, purple, purple, red, gray)
#define SS_TOOLBUTTON_UNDERLINE ACSS_TOOLBUTTON_TEXT_(bold, italic, underline, white, red, purple, purple, red, gray)
#define SS_TOOLBUTTON_STRIKE    ACSS_TOOLBUTTON_TEXT_(bold, italic, line-through, white, red, purple, purple, red, gray)

// - Constructions -

AnnotationEditor::AnnotationEditor(QWidget *parent)
  : Base(parent, WINDOW_FLAGS), id_(0)
{
  setWindowTitle(tr("Annotation Editor"));

  textEdit_ = AcUi::globalInstance()->makeTextEdit(0, TR(T_ANNOT));

  createActions();
  createRibons();
  createLayout();

  connect(textEdit_, SIGNAL(cursorPositionChanged()), SLOT(updateAlignment()));
  connect(textEdit_, SIGNAL(currentCharFormatChanged(QTextCharFormat)), SLOT(setFormat(QTextCharFormat)));
  connect(textEdit_, SIGNAL(currentCharFormatChanged(QTextCharFormat)), SLOT(updateCount()));
  connect(textEdit_, SIGNAL(textChanged()), SLOT(updateCount()));

  // Shortcuts
  saveShortcut_ = new QShortcut(QKeySequence::Save, this, SLOT(save()));

  new QShortcut(QKeySequence("CTRL+1"), codeRibonButton_, SLOT(click()));
  new QShortcut(QKeySequence("CTRL+2"), htmlRibonButton_, SLOT(click()));
#ifndef Q_OS_MAC
  new QShortcut(QKeySequence("ALT+1"), codeRibonButton_, SLOT(click()));
  new QShortcut(QKeySequence("ALT+2"), htmlRibonButton_, SLOT(click()));
#endif // Q_OS_MAC

  // Start up states
  setFontType(QApplication::font());
  setColorIconColor(Qt::black);
  setBackgroundColorIconColor(Qt::black);

  setCodeMode();
  updateCount();

  fontSizeComboBox_->setEditText("22");
  textEdit_->setFocus();
}

void
AnnotationEditor::createRibons()
{
  AcUi *ui = AcUi::globalInstance();

#define MAKE_CHECKABLE_BUTTON(_button, _ss, _title, _tip, _slot) \
  _button = new QxToolButton; { \
    _button->setStyleSheet(_ss); \
    _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
    _button->setText(_title); \
    _button->setToolTip(_tip); \
    _button->setCheckable(true); \
    connect(_button, SIGNAL(clicked(bool)), _slot); \
  }

  // Code ribon

  // - verbatimButton_
  verbatimButton_ = ui->makeToolButton(0, tr("verbatim"), TR(T_VERBATIM), this, SLOT(setVerbatim()));

  // - subtitleButton_
  subtitleButton_ = ui->makeToolButton(0, tr("subtitle"), TR(T_SUBTITLE), this, SLOT(setSubtitle()));

  // - moveStyleComboBox_
  moveStyleComboBox_ = ui->makeComboBox(0, "", tr("Style tag")); {
    moveStyleComboBox_->setFixedWidth(MOVESTYLECOMBOBOX_WIDTH);

    // Must be consistent with MoveStyleIndex
    moveStyleComboBox_->addItem(tr("fly"));
    moveStyleComboBox_->addItem(tr("top"));
    moveStyleComboBox_->addItem(tr("bottom"));
  } connect(moveStyleComboBox_, SIGNAL(activated(int)), SLOT(setMoveStyle(int)));

  renderEffectComboBox_ = ui->makeComboBox(0, "", TR(T_ANNOTATIONEFFECT)); {
    renderEffectComboBox_->setFixedWidth(RENDEREFFECTCOMBOBOX_WIDTH);

    // Must be consistent with RenderEffectIndex
    renderEffectComboBox_->addItem(tr("transp"));
    renderEffectComboBox_->addItem(tr("shadow"));
    renderEffectComboBox_->addItem(tr("blur"));
  } connect(renderEffectComboBox_, SIGNAL(activated(int)), SLOT(setRenderEffect(int)));

  // - Code ribon
  QGroupBox *codeRibon = new QGroupBox(this); {
    QVBoxLayout *rows = new QVBoxLayout;
    QHBoxLayout *row0 = new QHBoxLayout,
                *row1 = new QHBoxLayout;
    rows->addLayout(row0);
    rows->addLayout(row1);
    codeRibon->setLayout(rows);

    row0->addWidget(verbatimButton_);
    row0->addWidget(subtitleButton_);
    row0->addStretch();

    row1->addWidget(moveStyleComboBox_);
    row1->addWidget(renderEffectComboBox_);
    row1->addStretch();

    int patch = 0;
    if (!AcUi::isAeroAvailable())
      patch = 4;

    // void setContentsMargins(int left, int top, int right, int bottom);
    rows->setContentsMargins(patch, patch, patch, patch);
    row0->setContentsMargins(0, 0, 0, 0);
    row1->setContentsMargins(0, 0, 0, 0);
    codeRibon->setContentsMargins(0, 0, 0, 0);
  }

  // Html ribon

  // - boldButton_
  MAKE_CHECKABLE_BUTTON(boldButton_, SS_TOOLBUTTON_BOLD, tr("B"), TR(T_BOLD), SLOT(setBold(bool))) {
    QFont font = boldButton_->font();
    font.setBold(true);
    boldButton_->setFont(font);
  }

  // - italicButton_
  MAKE_CHECKABLE_BUTTON(italicButton_, SS_TOOLBUTTON_ITALIC, tr("I"), TR(T_ITALIC), SLOT(setItalic(bool))) {
    QFont font = italicButton_->font();
    font.setItalic(true);
    italicButton_->setFont(font);
  }

  // - underlineButton_
  MAKE_CHECKABLE_BUTTON(underlineButton_, SS_TOOLBUTTON_UNDERLINE, tr("U"), TR(T_UNDERLINE), SLOT(setUnderline(bool))) {
    QFont font = underlineButton_->font();
    font.setUnderline(true);
    underlineButton_->setFont(font);
  }

  // - strikeOutButton_
  MAKE_CHECKABLE_BUTTON(strikeOutButton_, SS_TOOLBUTTON_STRIKE, tr("S"), TR(T_STRIKEOUT), SLOT(setStrikeOut(bool))) {
    QFont font = strikeOutButton_->font();
    font.setStrikeOut(true);
    strikeOutButton_->setFont(font);
  }

  // - colorButton_
  colorButton_ = new QxToolButton; {
    colorButton_->setStyleSheet(SS_TRANSPARENT);
    colorButton_->setToolTip(TR(T_FOREGROUNDCOLOR));
  }
  connect(colorButton_, SIGNAL(clicked()), SLOT(showColorDialog()));

  // - backgroundColorButton_
  backgroundColorButton_ = new QxToolButton; {
    backgroundColorButton_->setStyleSheet(SS_TRANSPARENT);
    backgroundColorButton_->setToolTip(TR(T_BACKGROUNDCOLOR));
  }
  connect(backgroundColorButton_, SIGNAL(clicked()), SLOT(showBackgroundColorDialog()));

  // - alignComboBox_
  alignComboBox_ = ui->makeComboBox(AcUi::ReadOnlyHint, "", tr("Alignment")); {
    alignComboBox_->setFixedWidth(ALIGNCOMBOBOX_WIDTH);

    // Must be consisitent with AlignIndex
    alignComboBox_->addItem(tr("Left"));
    alignComboBox_->addItem(tr("Right"));
    alignComboBox_->addItem(tr("Center"));
    alignComboBox_->addItem(tr("Justify"));
  } connect(alignComboBox_, SIGNAL(activated(int)), SLOT(setAlignment(int)));

  // - fontComboBox_
  fontComboBox_ = new QxFontComboBox; {
    fontComboBox_->setStyleSheet(ACSS_COMBOBOX);
    //fontComboBox_->setEditable(true);
    fontComboBox_->setFixedWidth(FONTCOMBOBOX_WIDTH);
    fontComboBox_->setToolTip(tr("Font Family"));
  }
  connect(fontComboBox_, SIGNAL(activated(QString)), SLOT(setFontFamily(QString)));

  // - sizeComboBox_
  fontSizeComboBox_ = ui->makeComboBox(0, "", tr("Font Size")); {
    fontSizeComboBox_->setFixedWidth(FONTSIZECOMBOBOX_WIDTH);

    QFontDatabase db;
    foreach(int size, db.standardSizes())
      fontSizeComboBox_->addItem(QString::number(size));
  } connect(fontSizeComboBox_, SIGNAL(activated(QString)), SLOT(setFontSize(QString)));

  // - Html ribon
  QGroupBox *htmlRibon = new QGroupBox(this); {
    QVBoxLayout *rows = new QVBoxLayout;
    QHBoxLayout *row0 = new QHBoxLayout,
                *row1 = new QHBoxLayout;
    rows->addLayout(row0);
    rows->addLayout(row1);
    htmlRibon->setLayout(rows);

    row0->addWidget(boldButton_);
    row0->addWidget(italicButton_);
    row0->addWidget(underlineButton_);
    row0->addWidget(strikeOutButton_);
    row0->addWidget(colorButton_);
    row0->addWidget(backgroundColorButton_);
    row0->addStretch();

    row1->addWidget(fontComboBox_);
    row1->addWidget(fontSizeComboBox_);
    row1->addWidget(alignComboBox_);
    row1->addStretch();

    int patch = 0;
    if (!AcUi::isAeroAvailable())
      patch = 4;

    rows->setContentsMargins(patch, patch, patch, patch);
    row0->setContentsMargins(0, 0, 0, 0);
    row1->setContentsMargins(0, 0, 0, 0);
    htmlRibon->setContentsMargins(0, 0, 0, 0);
  }

  // Ribon layout

  ribonStackLayout_ = new QStackedLayout; {
    // Must be consistent with RibonIndex
    ribonStackLayout_->addWidget(codeRibon);
    ribonStackLayout_->addWidget(htmlRibon);
    ribonStackLayout_->setContentsMargins(0, 0, 0, 0);
  }

  // Header

  codeRibonButton_ = ui->makeToolButton(
        AcUi::TabHint, tr("tex"), "", K_CTRL "+1", this, SLOT(setCodeMode()));
  htmlRibonButton_ = ui->makeToolButton(
        AcUi::TabHint, tr("html"), "", K_CTRL "+2", this, SLOT(setHtmlMode()));

  formatButton_ = ui->makeToolButton(AcUi::MenuHint, tr("format"), tr("Format")); {
    QMenu *menu = new QMenu; {
      AcUi::globalInstance()->setContextMenuStyle(menu, false); // persistent = false

      // - Styles
      menu->addAction(boldAct_);
      menu->addAction(italicAct_);
      menu->addAction(underlineAct_);
      menu->addAction(strikeOutAct_);

      // - Colors
      menu->addSeparator();
      menu->addAction(colorAct_);
      menu->addAction(backgroundColorAct_);

      menu->addAction(TR(T_BLACK), this, SLOT(setBlackColor()));
      menu->addAction(TR(T_BLUE), this, SLOT(setBlueColor()));
      menu->addAction(TR(T_BROWN), this, SLOT(setBrownColor()));
      menu->addAction(TR(T_CYAN), this, SLOT(setCyanColor()));
      menu->addAction(TR(T_GRAY), this, SLOT(setGrayColor()));
      menu->addAction(TR(T_MAGENTA), this, SLOT(setMagentaColor()));
      menu->addAction(TR(T_ORANGE), this, SLOT(setOrangeColor()));
      menu->addAction(TR(T_PINK), this, SLOT(setPinkColor()));
      menu->addAction(TR(T_RED), this, SLOT(setRedColor()));
      menu->addAction(TR(T_WHITE), this, SLOT(setWhiteColor()));
      menu->addAction(TR(T_YELLOW), this, SLOT(setYellowColor()));
    }

    formatButton_->setMenu(menu);
  }

  // Footer

  countLabel_ = new QLabel; {
    countLabel_->setStyleSheet(ACSS_LABEL);
    countLabel_->setToolTip(TR(T_WORDCOUNT));
  }

  saveButton_ = ui->makeToolButton(AcUi::PushHint | AcUi::HighlightHint, TR(T_SAVE), TR(T_SAVE), SAVE_SHORTCUT, this, SLOT(save()));
  cancelButton_ = ui->makeToolButton(AcUi::PushHint, TR(T_CANCEL), TR(T_CANCEL), "ESC", this, SLOT(fadeOut()));

  tidyButton_ = ui->makeToolButton(AcUi::CheckHint, tr("tidy"), tr("Tidy HTML"), this, SLOT(setTidyEnabled(bool)));
  tidyButton_->setChecked(true);

#undef MAKE_CHECKABLE_BUTTON
}

void
AnnotationEditor::createActions()
{
#define MAKE_ACTION(_action, _tr, _slot) \
  connect(_action = new QAction(_tr, this), SIGNAL(triggered()), _slot);

#define MAKE_TOGGLE(_action, _tr, _slot) \
  connect(_action = new QAction(_tr, this), SIGNAL(triggered(bool)), _slot); \
  _action->setCheckable(true);

  MAKE_ACTION(colorAct_, TR(T_FOREGROUNDCOLOR), SLOT(showColorDialog()))
  MAKE_ACTION(backgroundColorAct_, TR(T_BACKGROUNDCOLOR), SLOT(showBackgroundColorDialog()))

  MAKE_TOGGLE(boldAct_, TR(T_BOLD), SLOT(setBold(bool)))
  MAKE_TOGGLE(italicAct_, TR(T_ITALIC), SLOT(setItalic(bool)))
  MAKE_TOGGLE(underlineAct_, TR(T_UNDERLINE), SLOT(setUnderline(bool)))
  MAKE_TOGGLE(strikeOutAct_, TR(T_STRIKEOUT), SLOT(setStrikeOut(bool)))

#undef MAKE_ACTION
#undef MAKE_TOGGLE
}

void
AnnotationEditor::createLayout()
{
  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout,
                *footer = new QHBoxLayout;
    rows->addLayout(header);
    rows->addLayout(ribonStackLayout_);
    rows->addWidget(textEdit_);
    rows->addLayout(footer);

    header->addWidget(codeRibonButton_);
    header->addWidget(htmlRibonButton_);
    header->addStretch();
    header->addWidget(formatButton_);

    footer->addWidget(countLabel_);
    footer->addWidget(tidyButton_);
    footer->addStretch();
    footer->addWidget(cancelButton_);
    footer->addWidget(saveButton_);

    // left, top, right, bottom
    int patch = 0;
    if (!AcUi::isAeroAvailable())
      patch = 9;
    header->setContentsMargins(0, 0, 0, 0);
    footer->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(9, patch, 9, patch);

  } setLayout(rows);
}

void
AnnotationEditor::clear()
{ textEdit_->clear(); }

// - Properties -

qint64
AnnotationEditor::id() const
{ return id_; }

void
AnnotationEditor::setId(qint64 id)
{  id_ = id; }

bool
AnnotationEditor::isTidyEnabled() const
{ return tidyButton_->isChecked(); }

void
AnnotationEditor::setTidyEnabled(bool t)
{
  tidyButton_->setChecked(t);
  if (t)
    textEdit_->setPlainText(ANNOT_REDUCE_HTML(textEdit_->toPlainText()));
  updateCount();
}

AnnotationEditor::Mode
AnnotationEditor::mode() const
{ return isCodeMode() ? CodeMode : HtmlMode; }

bool
AnnotationEditor::isCodeMode() const
{ return ribonStackLayout_->currentIndex() == CodeRibonIndex; }

bool
AnnotationEditor::isHtmlMode() const
{ return ribonStackLayout_->currentIndex() == HtmlRibonIndex; }

void
AnnotationEditor::setMode(Mode mode)
{
  switch (mode) {
  case CodeMode: setCodeMode(); break;
  case HtmlMode: setHtmlMode(); break;
  default: Q_ASSERT(0);
  }
}

QString
AnnotationEditor::text() const
{
  switch (mode()) {
  case CodeMode:
    return textEdit_->toPlainText();

  case HtmlMode:
    if (textEdit_->toPlainText().trimmed().isEmpty())
      return "";
    else {
      QString html = textEdit_->toHtml();
      if (isTidyEnabled())
        return ANNOT_REDUCE_HTML(html);
      else
        return html;
    }

  default:
    Q_ASSERT(0);
    return QString();
  }
}

void
AnnotationEditor::setText(const QString &text)
{
  text_ = text;
  if (text.isEmpty() || text.trimmed().isEmpty()) {
    clear();
    return;
  }

  textEdit_->setPlainText(text);
  if (isHtmlMode())
    code2html();
}

void
AnnotationEditor::code2html()
{
  DOUT("enter: text follows");
  DOUT(textEdit_->toPlainText());
  text_ = textEdit_->toPlainText();
  QString html;
  QStringList tags;
  boost::tie(html, tags) = ANNOT_PARSE_CODE(textEdit_->toPlainText());
  if (!tags.empty())
    html = tags.join("") + " " + html.trimmed();

  textEdit_->setHtml(html);
  DOUT("tags follows");
  DOUT(tags);
  DOUT("html follows");
  DOUT(html);
  DOUT("exit: html follows");
}

void
AnnotationEditor::html2code()
{
  textEdit_->setPlainText(text_);
  /*
  DOUT("enter: tidy =" << isTidyEnabled());
  QString html = textEdit_->toHtml();
  DOUT("html follows");
  DOUT(html);
  if (isTidyEnabled())
    textEdit_->setPlainText(ANNOT_REDUCE_HTML(html));
  else
    textEdit_->setPlainText(html);
  DOUT("text follows");
  DOUT(textEdit_->toPlainText());
  clearFormat();
  DOUT("exit");
  */
}

// - Slots -

void
AnnotationEditor::tag(const QString &tag)
{
  removeTag(tag);
  addTag(tag);
}

void
AnnotationEditor::addTag(const QString &tag)
{
  if (isCodeMode())
    textEdit_->setPlainText(QString("%1 %2").arg(tag).arg(textEdit_->toPlainText()).trimmed());
}

void
AnnotationEditor::removeTag(const QString &tag)
{
  if (isCodeMode())
    textEdit_->setPlainText(textEdit_->toPlainText().remove(tag).trimmed());
}

void
AnnotationEditor::setVerbatim()
{ tag(CORE_CMD_VERBATIM); }

void
AnnotationEditor::setSubtitle()
{ tag(CORE_CMD_SUB); }

void
AnnotationEditor::setMoveStyle(int index)
{
  if (index < 0 || index >= MoveStyleCount) {
    QString newTag = moveStyleComboBox_->currentText().trimmed();
    if (!newTag.isEmpty())
      tag(CORE_CMDSTR + newTag);
    return;
  }

  removeTag(CORE_CMD_VIEW_FLY);
  removeTag(CORE_CMD_VIEW_TOP);
  removeTag(CORE_CMD_VIEW_BOTTOM);

  switch (index) {
  case MoveStyleFlyIndex:    addTag(CORE_CMD_VIEW_FLY); break;
  case MoveStyleTopIndex:    addTag(CORE_CMD_VIEW_TOP); break;
  case MoveStyleBottomIndex: addTag(CORE_CMD_VIEW_BOTTOM); break;
  }
}

void
AnnotationEditor::setRenderEffect(int index)
{
  if (index < 0 || index >= RenderEffectCount) {
    QString newTag = renderEffectComboBox_->currentText().trimmed();
    if (!newTag.isEmpty())
      tag(CORE_CMDSTR + newTag);
    return;
  }

  removeTag(CORE_CMD_EFFECT_TRANSP);
  removeTag(CORE_CMD_EFFECT_TRANSPARENT);
  removeTag(CORE_CMD_EFFECT_SHADOW);
  removeTag(CORE_CMD_EFFECT_BLUR);

  switch (index) {
  case TransparentEffectIndex:  addTag(CORE_CMD_EFFECT_TRANSP); break;
  case ShadowEffectIndex:       addTag(CORE_CMD_EFFECT_SHADOW); break;
  case BlurEffectIndex:         addTag(CORE_CMD_EFFECT_BLUR); break;
  }
}

void
AnnotationEditor::save()
{
  fadeOut();

  QString t = text();
  if (!t.isEmpty() && id_ >= 0)
    emit textSaved(t);
}

void
AnnotationEditor::setCodeMode()
{
  if (!isCodeMode())
    html2code();
  textEdit_->setReadOnly(false);

  ribonStackLayout_->setCurrentIndex(CodeRibonIndex);
  htmlRibonButton_->setChecked(false);
  codeRibonButton_->setChecked(true);

  formatButton_->setEnabled(false);
}

void
AnnotationEditor::setHtmlMode()
{
  if (!isHtmlMode())
    code2html();
  textEdit_->setReadOnly(true);

  ribonStackLayout_->setCurrentIndex(HtmlRibonIndex);
  htmlRibonButton_->setChecked(true);
  codeRibonButton_->setChecked(false);

  formatButton_->setEnabled(true);
}


void
AnnotationEditor::setBold(bool t)
{
  boldButton_->setCheckable(t);
  boldAct_->setCheckable(t);

  QTextCharFormat fmt;
  fmt.setFontWeight(t ? QFont::Bold : QFont::Normal);
  mergeFormat(fmt);
}

void
AnnotationEditor::setItalic(bool t)
{
  italicButton_->setCheckable(t);
  italicAct_->setCheckable(t);

  QTextCharFormat fmt;
  fmt.setFontItalic(t);
  mergeFormat(fmt);
}

void
AnnotationEditor::setUnderline(bool t)
{
  underlineButton_->setCheckable(t);
  underlineAct_->setCheckable(t);

  QTextCharFormat fmt;
  fmt.setFontUnderline(t);
  mergeFormat(fmt);
}

void
AnnotationEditor::setStrikeOut(bool t)
{
  strikeOutButton_->setCheckable(t);
  strikeOutAct_->setCheckable(t);

  QTextCharFormat fmt;
  fmt.setFontStrikeOut(t);
  mergeFormat(fmt);
}

void
AnnotationEditor::setFontFamily(const QString &name)
{
  QTextCharFormat fmt;
  fmt.setFontFamily(name);
  if (fmt.isValid())
    mergeFormat(fmt);
}

void
AnnotationEditor::setFontSize(const QString &size)
{
  bool ok;
  qreal fp = size.toFloat(&ok);
  if (ok && fp > 0) {
    QTextCharFormat fmt;
    fmt.setFontPointSize(fp);
    if (fmt.isValid())
      mergeFormat(fmt);
  }
}

void
AnnotationEditor::setColor(const QColor &c)
{
  QTextCharFormat fmt;
  fmt.setForeground(c);
  mergeFormat(fmt);

  setColorIconColor(c);
}

void
AnnotationEditor::setBackgroundColor(const QColor &c)
{
  QTextCharFormat fmt;
  fmt.setBackground(c);
  mergeFormat(fmt);

  setBackgroundColorIconColor(c);
}

void
AnnotationEditor::setFormat(const QTextCharFormat &fmt)
{
  setFontType(fmt.font());
  setColorIconColor(fmt.foreground().color());
  setBackgroundColorIconColor(fmt.background().color());
}

void
AnnotationEditor::clearFormat()
{
  QTextCharFormat fmt;

  setFontType(fmt.font());
  setColorIconColor(fmt.foreground().color());
  setBackgroundColorIconColor(fmt.foreground().color());

  QTextCursor cursor = textEdit_->textCursor();
  cursor.select(QTextCursor::Document);
  cursor.setCharFormat(fmt);
  textEdit_->setCurrentCharFormat(fmt);
}

void
AnnotationEditor::setFontType(const QFont &font)
{
  fontComboBox_->setCurrentIndex(fontComboBox_->findText(QFontInfo(font).family()));
  fontSizeComboBox_->setCurrentIndex(fontSizeComboBox_->findText(QString::number(font.pointSize())));

  boldButton_->setChecked(font.bold());
  italicButton_->setChecked(font.italic());
  underlineButton_->setChecked(font.underline());
}

void
AnnotationEditor::setColorIconColor(const QColor &input)
{
  QColor c = input;
  if (!c.isValid())
    c = Qt::black;
  QPixmap icon(16, 16);
  icon.fill(c);
  colorButton_->setIcon(icon);
  colorAct_->setIcon(icon);
}

void
AnnotationEditor::setBackgroundColorIconColor(const QColor &input)
{
  QColor c = input;
  if (!c.isValid())
    c = Qt::black;
  QPixmap icon(16, 16);
  icon.fill(c);
  backgroundColorButton_->setIcon(icon);
  backgroundColorAct_->setIcon(icon);
}

void
AnnotationEditor::showColorDialog()
{
  QColor c = QColorDialog::getColor(textEdit_->textColor(), this);
  if (c.isValid())
    setColor(c);
}

void
AnnotationEditor::showBackgroundColorDialog()
{
  QColor c = QColorDialog::getColor(textEdit_->textBackgroundColor(), this);
  if (c.isValid())
    setBackgroundColor(c);
}

void
AnnotationEditor::setAlignment(int alignIndex)
{
  Qt::Alignment a = 0;
  switch (alignIndex) {
  case AlignLeftIndex:    a = Qt::AlignLeft | Qt::AlignAbsolute; break;
  case AlignRightIndex:   a = Qt::AlignRight | Qt::AlignAbsolute; break;
  case AlignHCenterIndex: a = Qt::AlignHCenter; break;
  case AlignJustifyIndex: a = Qt::AlignJustify; break;
  }
  if (a)
    textEdit_->setAlignment(a);
}

void
AnnotationEditor::updateAlignment()
{
  Qt::Alignment a = textEdit_->alignment();

  int index = AlignIndexCount;
  if (a & Qt::AlignLeft)
    index = AlignLeftIndex;
  else if (a & Qt::AlignRight)
    index = AlignRightIndex;
  else if (a & Qt::AlignHCenter)
    index = AlignHCenterIndex;
  else if (a & Qt::AlignJustify)
    index = AlignJustifyIndex;

  if (index < AlignIndexCount)
    alignComboBox_->setCurrentIndex(index);
}

void
AnnotationEditor::updateCount()
{
  static QString longWarning = tr("Text is too long") + " >_<";
  static QString idWarning = tr("Text cannot be saved") + " T_T";
  int cur = text().size();
  int max = G_ANNOTATION_MAXSIZE;
  countLabel_->setText(QString("%1/%2").arg(cur).arg(max));

  if (cur > max) {
    countLabel_->setToolTip(longWarning);
    saveButton_->setToolTip(id_ >= 0 ? longWarning : idWarning);
    saveButton_->setEnabled(false);
    saveShortcut_->setEnabled(false);
  } else {
    countLabel_->setToolTip(TR(T_WORDCOUNT));
    if (id_ < 0) {
      saveButton_->setToolTip(idWarning);
      saveButton_->setEnabled(false);
      saveShortcut_->setEnabled(false);
    } else {
      saveButton_->setToolTip(TR(T_SAVE));
      saveButton_->setEnabled(true);
      saveShortcut_->setEnabled(true);
    }
  }
}

// - Helpers -

void
AnnotationEditor::mergeFormat(const QTextCharFormat &format)
{
  QTextCursor cursor = textEdit_->textCursor();
  if (!cursor.hasSelection())
    cursor.select(QTextCursor::WordUnderCursor);
  cursor.mergeCharFormat(format);
  textEdit_->mergeCurrentCharFormat(format);
}

// - Events -

/*
void
AnnotationEditor::contextMenuEvent(QContextMenuEvent *event)
{
  if (!event)
    return;

  event->accept();
}
*/

// EOF

/*
void TextEdit::textStyle(int styleIndex)
{
  QTextCursor cursor = textEdit_->textCursor();

  if (styleIndex != 0) {
    QTextListFormat::Style style = QTextListFormat::ListDisc;

    switch (styleIndex) {
      default:
      case 1:
        style = QTextListFormat::ListDisc;
        break;
      case 2:
        style = QTextListFormat::ListCircle;
        break;
      case 3:
        style = QTextListFormat::ListSquare;
        break;
      case 4:
        style = QTextListFormat::ListDecimal;
        break;
      case 5:
        style = QTextListFormat::ListLowerAlpha;
        break;
      case 6:
        style = QTextListFormat::ListUpperAlpha;
        break;
      case 7:
        style = QTextListFormat::ListLowerRoman;
        break;
      case 8:
        style = QTextListFormat::ListUpperRoman;
        break;
    }

    cursor.beginEditBlock();

    QTextBlockFormat blockFmt = cursor.blockFormat();

    QTextListFormat listFmt;

    if (cursor.currentList()) {
      listFmt = cursor.currentList()->format();
    } else {
      listFmt.setIndent(blockFmt.indent() + 1);
      blockFmt.setIndent(0);
      cursor.setBlockFormat(blockFmt);
    }

    listFmt.setStyle(style);

    cursor.createList(listFmt);

    cursor.endEditBlock();
  } else {
    // ####
    QTextBlockFormat bfmt;
    bfmt.setObjectIndex(-1);
    cursor.mergeBlockFormat(bfmt);
  }
}
*/

