#ifndef ACABOUTDIALOG_H
#define ACABOUTDIALOG_H

// acaboutdialog.h
// 11/13/2011

#include "module/qtext/dialog.h"
#include <QtCore/QString>

QT_FORWARD_DECLARE_CLASS(QTextEdit)

typedef QtExt::Dialog AcAboutDialogBase;
class AcAboutDialog : public AcAboutDialogBase
{
  Q_OBJECT
  typedef AcAboutDialog Self;
  typedef AcAboutDialogBase Base;

  QTextEdit *textEdit_;
  QString app_, version_;

public:
  explicit AcAboutDialog(QWidget *parent = 0)
    : Base(parent) { init(); }

  explicit AcAboutDialog(const QString &appName, QWidget *parent = 0)
    : Base(parent), app_(appName) { init(); }

  AcAboutDialog(const QString &appName, const QString &version, QWidget *parent = 0)
    : Base(parent), app_(appName), version_(version) { init(); }

  virtual QString text() const
  { return templateText().arg(app_).arg(version_); }

public slots:
  virtual void setVisible(bool visible); ///< \override

  void setVersion(const QString v) { version_ = v; }
  void setApplicationName(const QString n) { app_ = n; }

protected:
  QString templateText() const; ///< %1:app

private:
  void init();
  void createLayout();
};

#endif // ACABOUTDIALOG_H
