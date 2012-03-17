#ifndef URLDIALOG_H
#define URLDIALOG_H

// urldialog.h
// 1/24/2012

#include "module/qtext/dialog.h"

QT_FORWARD_DECLARE_CLASS(QToolButton)
QT_FORWARD_DECLARE_CLASS(QComboBox)

typedef QtExt::Dialog UrlDialogBase;
class UrlDialog : public UrlDialogBase
{
  Q_OBJECT
  typedef UrlDialog Self;
  typedef UrlDialogBase Base;

  QComboBox *edit_;
  QToolButton *urlButton_;

public:
  explicit UrlDialog(QWidget *parent = 0);
  QString text() const;

signals:
  void urlEntered(const QString &url);

public slots:
  void open();
  void paste();
  void setExampleUrl(const QString &text);
  void setText(const QString &url);
  void increase();
  void decrease();

protected:
  static QString autoCompleteUrl(const QString &url);

protected slots:
  void showExampleUrl();

private:
  void createLayout();
};

#endif // URLDIALOG_H
