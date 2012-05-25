#ifndef URLDIALOG_H
#define URLDIALOG_H

// urldialog.h
// 2/7/2012

#include "project/common/acwindow.h"

QT_FORWARD_DECLARE_CLASS(QToolButton)
QT_FORWARD_DECLARE_CLASS(QComboBox)

class UrlDialog : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(UrlDialog)
  typedef UrlDialog Self;
  typedef AcWindow Base;

  QComboBox *edit_;
  QToolButton *urlButton_,
              *openButton_,
              *saveButton_;
  QString editStyleSheet_;

public:
  explicit UrlDialog(QWidget *parent = 0);

  QString text() const;
  bool isEmpty() const;

signals:
  void urlEntered(const QString &url, bool save);

public slots:
  void addHistory(const QString &url);
  void open();
  void paste();
  void setExampleUrl(const QString &text);
  void setText(const QString &url);
  void setSave(bool t);
  void increase();
  void decrease();

  virtual void setVisible(bool visible); ///< \override

protected:
  static QString autoCompleteUrl(const QString &url);

protected slots:
  void showExampleUrl();
  void verifyEditText();

private:
  void createLayout();
};

#endif // URLDIALOG_H
