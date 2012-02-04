#ifndef _QTEXT_COMBOBOX_H
#define _QTEXT_COMBOBOX_H

// qtext/combobox.h
// 10/7/2011

#include <QComboBox>
#include <QCursor>

namespace QtExt {

class ComboBox : public QComboBox
{
  Q_OBJECT
  typedef ComboBox Self;
  typedef QComboBox Base;

  bool hover_;
  QCursor lastCursor_;
  QCursor hoverCursor_;

public:
  explicit ComboBox(QWidget *parent = 0);

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

#endif // _QTEXT_COMBOBOX_H
