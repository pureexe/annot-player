// acconsoledialog.cc
// 2/18/2012

#include "project/common/acconsole.h"
#include "project/common/actextview.h"
#include "project/common/acui.h"
#include "module/qtext/htmltag.h"
#include <QtGui>

// - Constructions -

QList<AcConsole*> AcConsole::instances_;

AcConsole::AcConsole(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(tr("Console"));

  createLayout();

  connect(this, SIGNAL(debugMessageReceived(QString)),
          textView_, SLOT(append(QString)), Qt::QueuedConnection);

  instances_.append(this);
}

AcConsole::~AcConsole()
{ instances_.removeAll(this); }

void
AcConsole::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  textView_ = new AcTextView(this);

  QToolButton *okButton = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, tr("OK"), this, SLOT(fadeOut()));
  QToolButton *clearButton = ui->makeToolButton(
        AcUi::PushHint, tr("Clear"), textView_, SLOT(clear()));

  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *footer = new QHBoxLayout;
    rows->addWidget(textView_);
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

// - Message handler -

// See: http://www.cppblog.com/lauer3912/archive/2011/04/10/143870.html
void
AcConsole::messageHandler(QtMsgType type, const char *msg)
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
      t->emit_debugMessageReceived(text);
  }
#undef TIMESTAMP
}

// EOF
