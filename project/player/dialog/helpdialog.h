#ifndef HELPDIALOG_H
#define HELPDIALOG_H

// helpdialog.h
// 11/16/2011

#include "ac/acwebwindow.h"
#include <QLocale>
#include <QUrl>

class HelpDialog: public AcWebWindow
{
  Q_OBJECT
  typedef HelpDialog Self;
  typedef AcWebWindow Base;

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
