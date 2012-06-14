#ifndef HELPDIALOG_H
#define HELPDIALOG_H

// helpdialog.h
// 11/16/2011

#include "project/common/acwebwindow.h"
#include <QtCore/QLocale>
#include <QtCore/QUrl>

class HelpDialog: public AcWebWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(HelpDialog)
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
  virtual void setVisible(bool visible); ///< \reimp
};

#endif // HELPDIALOG_H
