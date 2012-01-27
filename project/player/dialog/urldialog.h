#ifndef USERDIALOG_H
#define USERDIALOG_H

// urldialog.h
// 1/24/2012

#include "core/gui/dialog.h"

QT_FORWARD_DECLARE_CLASS(QLineEdit)
QT_FORWARD_DECLARE_CLASS(QLabel)

typedef Core::Gui::Dialog UrlDialogBase;

class UrlDialog : public UrlDialogBase
{
  Q_OBJECT
  typedef UrlDialog Self;
  typedef UrlDialogBase Base;

  QLineEdit *edit_;

public:
  explicit UrlDialog(QWidget *parent = 0);

signals:
  void urlEntered(const QString &url);

public slots:
  void open();
  void paste();

protected:
  static QString autoCompleteUrl(const QString &url);

  QString exampleUrl() const;
protected slots:
  void showExampleUrl();

private:
  void createLayout();
};

#endif // URLRDIALOG_H
