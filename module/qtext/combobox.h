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

  bool hovered_;
  QCursor hoverCursor_,
          lastCursor_;

public:
  explicit ComboBox(QWidget *parent = 0)
    : Base(parent), hovered_(false), hoverCursor_(Qt::PointingHandCursor) { }
signals:
  void hovered();
  void leaved();

public:
  bool isHovered() const { return hovered_; }
  QCursor hoverCursor() const { return hoverCursor_; }
public slots:
  void setHoverCursor(const QCursor &cursor) { hoverCursor_ = cursor; }

  // - Events -
protected:
  virtual void enterEvent(QEvent *event); ///< \override
  virtual void leaveEvent(QEvent *event); ///< \override
};

} // namespace QtExt

#endif // _QTEXT_COMBOBOX_H
