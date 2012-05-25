// aimlbot.cc
// 7/31/2011

#include "aimlbot.h"
#include "aimlparser.h"
#include "aiml_config.h"

#include <QtCore>
#include <QProgressDialog>

// - Constructions -
AimlBot::AimlBot(bool showProgress)
  : parser_(0)
{
  // TODO: async!
  AimlParser *parser = new AimlParser;

  parser->loadVars(AIML_PATH_VARS, false);
  parser->loadVars(AIML_PATH_BOT, true);
  parser->loadSubs(AIML_PATH_SUBS);
  QString dirname = AIML_PATH_SET;
  QDir dir(dirname);
  QStringList files = dir.entryList(QStringList()<<"*.aiml");

  QProgressDialog *pd = 0;
  if (showProgress) {
    pd = new QProgressDialog(this);
    pd->setCancelButton(0);
    pd->setMaximum(files.count());
  }

  int count = 0;
  foreach (const QString &file, files) {
    if (pd) {
      pd->setLabelText("parsing " + file + "...");
      pd->setValue(count);
    }

    qApp->processEvents();
    if (pd && pd->wasCanceled())
      break;
    parser->loadAiml(dirname + "/" + file);
    count++;
  }

  parser_ = parser;
}

AimlBot::~AimlBot()
{
  if (parser_) {
    //parser_->saveVars("settings/vars.xml");
    delete parser_;
  }
}

AimlParser*
AimlBot::parser() const
{ return parser_; }

void
AimlBot::setParser(AimlParser *parser)
{  parser_ = parser; }

// - Chat -

QString
AimlBot::chat(const QString &text)
{
  if (!parser_)
    return QString();

  return parser_->getResponse(text);
}

// EOF
