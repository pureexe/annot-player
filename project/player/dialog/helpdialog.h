#ifndef HELPDIALOG_H
#define HELPDIALOG_H

// helpdialog.h
// 11/16/2011

#include "core/gui/webdialog.h"
#include <QLocale>

typedef Core::Gui::WebDialog HelpDialogBase;

class HelpDialog: public HelpDialogBase
{
  Q_OBJECT
  typedef HelpDialog Self;
  typedef WebDialog Base;

  // - Constructions -
public:
  explicit HelpDialog(QWidget *parent = 0);

  // - Properties -
protected:
  static QUrl urlForLanguage(QLocale::Language lang);

  // - Events -
public:
  virtual void setVisible(bool visible); ///< \override
};

#endif // HELPDIALOG_H
