#ifndef URLDIALOG_H
#define URLDIALOG_H

// urldialog.h
// 2/7/2012

#include "src/common/acmainwindow.h"

QT_FORWARD_DECLARE_CLASS(QToolButton)
QT_FORWARD_DECLARE_CLASS(QComboBox)

class UrlDialog : public AcMainWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(UrlDialog)
  typedef UrlDialog Self;
  typedef AcMainWindow Base;

  QComboBox *edit_;
  QToolButton *urlButton_,
              *openButton_;
              //*saveButton_;
  QString editStyleSheet_;

public:
  explicit UrlDialog(QWidget *parent = nullptr);

  QString text() const;
  bool isEmpty() const;

signals:
  void urlEntered(const QString &url);

public slots:
  void addHistory(const QString &url);
  void open();
  void paste();
  void setExampleUrl(const QString &text);
  void setText(const QString &url);
  //void setSave(bool t);
  void increase();
  void decrease();

  void setVisible(bool visible) override;

protected:
  static QString normalizeUrl(const QString &url);

protected slots:
  void showExampleUrl();
  void verifyEditText();

private:
  void createLayout();
};

#endif // URLDIALOG_H
