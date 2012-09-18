#ifndef QXFONTCOMBOBOX_H
#define QXFONTCOMBOBOX_H

// qxcombobox.h
// 10/11/2011

#include "qtx/qtxglobal.h"
#include <QtGui/QFontComboBox>

QTX_BEGIN_NAMESPACE

class QxFontComboBox : public QFontComboBox
{
  Q_OBJECT
  Q_DISABLE_COPY(QxFontComboBox)
  typedef QxFontComboBox Self;
  typedef QFontComboBox Base;

  QString styleSheet_;

public:
  explicit QxFontComboBox(QWidget *parent = nullptr);

public slots:
  bool verifyCurrentText();

  // - Events -
protected:
  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;
};

QTX_END_NAMESPACE

#endif // QXFONTCOMBOBOX_H
