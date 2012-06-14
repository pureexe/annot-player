#ifndef _QTEXT_FONTCOMBOBOX_H
#define _QTEXT_FONTCOMBOBOX_H

// qtext/combobox.h
// 10/11/2011

#include <QtGui/QFontComboBox>

namespace QtExt {

class FontComboBox : public QFontComboBox
{
  Q_OBJECT
  Q_DISABLE_COPY(FontComboBox)
  typedef FontComboBox Self;
  typedef QFontComboBox Base;

  QString styleSheet_;

public:
  explicit FontComboBox(QWidget *parent = 0);

public slots:
  bool verifyCurrentText();

  // - Events -
protected:
  virtual void enterEvent(QEvent *event); ///< \reimp
  virtual void leaveEvent(QEvent *event); ///< \reimp
};

} // namespace QtExt

#endif // _QTEXT_FONTCOMBOBOX_H
