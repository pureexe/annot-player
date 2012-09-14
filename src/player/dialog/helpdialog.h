#ifndef HELPDIALOG_H
#define HELPDIALOG_H

// helpdialog.h
// 11/16/2011

#include "src/common/acwebwindow.h"
#include <QtCore/QUrl>

class HelpDialog: public AcWebWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(HelpDialog)
  typedef HelpDialog Self;
  typedef AcWebWindow Base;

  // - Constructions -
public:
  explicit HelpDialog(QWidget *parent = nullptr);

  // - Properties -
protected:
  static QUrl urlForLanguage(int lang);

  // - Events -
public:
  void setVisible(bool visible) override;
};

#endif // HELPDIALOG_H
