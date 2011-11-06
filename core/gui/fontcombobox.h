#ifndef _CORE_GUI_FONTCOMBOBOX_H
#define _CORE_GUI_FONTCOMBOBOX_H

// core/gui/combobox.h
// 10/11/2011

#include <QFontComboBox>
#include <QCursor>

namespace Core { namespace Gui {

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

} } // namespace Gui, Core

#endif // _CORE_GUI_FONTCOMBOBOX_H
