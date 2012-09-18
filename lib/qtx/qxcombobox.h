#ifndef QXCOMBOBOX_H
#define QXCOMBOBOX_H

// qxcombobox.h
// 10/7/2011

#include "qtx/qtxglobal.h"
#include <QtGui/QComboBox>

QTX_BEGIN_NAMESPACE

class QxComboBox : public QComboBox
{
  Q_OBJECT
  Q_DISABLE_COPY(QxComboBox)
  typedef QxComboBox Self;
  typedef QComboBox Base;

public:
  explicit QxComboBox(QWidget *parent = nullptr);

public slots:
  void showPopup() override { Base::showPopup(); }
  void hidePopup() override { clearFocus(); Base::hidePopup(); }

  void setPopupVisible(bool t) { if (t) showPopup(); else hidePopup(); }

  // - Events -
protected:
  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;
};

QTX_END_NAMESPACE

#endif // QXCOMBOBOX_H
