// consoledialog.cc
// 2/18/2012

#include "consoledialog.h"
#include "textedittabview.h"
#include "tr.h"
#include "project/common/acui.h"
#include "module/qtext/htmltag.h"
#include <QtGui>

// - Constructions -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowStaysOnTopHint | \
  Qt::WindowCloseButtonHint )

QList<ConsoleDialog*> ConsoleDialog::instances_;

ConsoleDialog::ConsoleDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  setWindowTitle(TR(T_TITLE_CONSOLE));

  createLayout();

  connect(this, SIGNAL(asyncLogText(QString)),
          SLOT(appendLogText(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(asyncDebugText(QString)),
          SLOT(appendDebugText(QString)), Qt::QueuedConnection);

  instances_.append(this);
}

ConsoleDialog::~ConsoleDialog()
{ instances_.removeAll(this); }

void
ConsoleDialog::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  tabView_ = new TextEditTabView(this);
  tabView_->addTab(tr("Debug"));
  tabView_->addTab(tr("Log"));
  tabView_->finalizeLayout();

  QToolButton *okButton = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_OK), this, SLOT(fadeOut()));
  QToolButton *clearButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_CLEAR), this, SLOT(clear()));

  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *footer = new QHBoxLayout;
    rows->addWidget(tabView_);
    rows->addLayout(footer);

    footer->addWidget(clearButton);
    footer->addWidget(okButton);

    // left, top, right, bottom
    int patch = 0;
    if (!AcUi::isAeroAvailable())
      patch = 9;
    footer->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(9, patch, 9, 9);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(rows);
}

// - Properties -

void
ConsoleDialog::appendLogText(const QString &text)
{ tabView_->appendText(text, LogTabIndex); }

void
ConsoleDialog::appendDebugText(const QString &text)
{ tabView_->appendText(text, DebugTabIndex); }

void
ConsoleDialog::clear()
{
  for (int i = 0; i < TabIndexCount; i++)
    tabView_->clear(i);
}

// - Message handler -

// See: http://www.cppblog.com/lauer3912/archive/2011/04/10/143870.html
void
ConsoleDialog::messageHandler(QtMsgType type, const char *msg)
{
#define TIMESTAMP QDateTime::currentDateTime().toString("MM:dd: hh:mm:ss")
  if (!instances_.isEmpty()) {
    QString text;
    switch (type) {
    case QtDebugMsg:    text = QString("%1: %2").arg(TIMESTAMP).arg(msg); break;
    case QtWarningMsg:  text = QString(HTML_STYLE_OPEN(color:orange) "%1: warning: %2" HTML_STYLE_CLOSE()).arg(TIMESTAMP).arg(msg); break;
    case QtCriticalMsg: text = QString(HTML_STYLE_OPEN(color:red) "%1: critical: %2" HTML_STYLE_CLOSE()).arg(TIMESTAMP).arg(msg); break;
    case QtFatalMsg:    text = QString(HTML_STYLE_OPEN(color:red) "%1: fatal: %2" HTML_STYLE_CLOSE()).arg(TIMESTAMP).arg(msg); break;
    default: return;
    }

    foreach (Self *t, instances_)
      t->sendDebugText(text);
  }
#undef TIMESTAMP
}

// EOF
