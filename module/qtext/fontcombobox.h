#ifndef _QTEXT_FONTCOMBOBOX_H
#define _QTEXT_FONTCOMBOBOX_H

// qtext/combobox.h
// 10/11/2011

#include <QFontComboBox>
#include <QCursor>

namespace QtExt {

class FontComboBox : public QFontComboBox
{
  Q_OBJECT
  typedef FontComboBox Self;
  typedef QFontComboBox Base;

  bool hover_;
  QCursor lastCursor_;
  QCursor hoverCursor_;

public:
  explicit FontComboBox(QWidget *parent = 0);

  bool isHovered() const;

  QCursor hoverCursor() const;
  void setHoverCursor(const QCursor &cursor);

signals:
  //void hovered();
  //void leaved();

  // - Events -
protected:
  virtual void enterEvent(QEvent *event); ///< \override
  virtual void leaveEvent(QEvent *event); ///< \override
};

} // namespace QtExt

#endif // _QTEXT_FONTCOMBOBOX_H
