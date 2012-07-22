#ifndef ACABOUT_H
#define ACABOUT_H

// acabout.h
// 11/13/2011

#include "project/common/acwindow.h"
#include <QtCore/QString>

QT_FORWARD_DECLARE_CLASS(QTextEdit)

class AcAbout : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(AcAbout)
  typedef AcAbout Self;
  typedef AcWindow Base;

  QTextEdit *textEdit_;
  QString app_, version_;

public:
  explicit AcAbout(QWidget *parent = nullptr)
    : Base(parent) { init(); }

  explicit AcAbout(const QString &appName, QWidget *parent = nullptr)
    : Base(parent), app_(appName) { init(); }

  AcAbout(const QString &appName, const QString &version, QWidget *parent = nullptr)
    : Base(parent), app_(appName), version_(version) { init(); }

  virtual QString text() const
  { return templateText().arg(app_).arg(version_); }

public slots:
  void setVisible(bool visible) override;

  void setVersion(const QString v) { version_ = v; }
  void setApplicationName(const QString n) { app_ = n; }

protected:
  QString templateText() const; ///< %1:app

private:
  void init();
  void createLayout();
};

#endif // ACABOUT_H
