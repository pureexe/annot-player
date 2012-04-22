#ifndef _QTEXT_FONTCOMBOBOX_H
#define _QTEXT_FONTCOMBOBOX_H

// qtext/combobox.h
// 10/11/2011

#include <QtGui/QFontComboBox>
#include <QtGui/QCursor>

namespace QtExt {

class FontComboBox : public QFontComboBox
{
  Q_OBJECT
  typedef FontComboBox Self;
  typedef QFontComboBox Base;

  bool hovered_;
  QCursor hoverCursor_,
          lastCursor_;

public:
  explicit FontComboBox(QWidget *parent = 0)
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

#endif // _QTEXT_FONTCOMBOBOX_H
