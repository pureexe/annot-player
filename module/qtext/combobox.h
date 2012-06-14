#ifndef _QTEXT_COMBOBOX_H
#define _QTEXT_COMBOBOX_H

// qtext/combobox.h
// 10/7/2011

#include <QtGui/QComboBox>

namespace QtExt {

class ComboBox : public QComboBox
{
  Q_OBJECT
  Q_DISABLE_COPY(ComboBox)
  typedef ComboBox Self;
  typedef QComboBox Base;

public:
  explicit ComboBox(QWidget *parent = 0);

public slots:
  virtual void showPopup() { Base::showPopup(); } ///< \reimp
  virtual void hidePopup() { clearFocus(); Base::hidePopup(); } ///< \reimp

  void setPopupVisible(bool t) { if (t) showPopup(); else hidePopup(); }

  // - Events -
protected:
  virtual void enterEvent(QEvent *event); ///< \reimp
  virtual void leaveEvent(QEvent *event); ///< \reimp
};

} // namespace QtExt

#endif // _QTEXT_COMBOBOX_H
