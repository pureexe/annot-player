#ifndef _CORE_GUI_COMBOBOX_H
#define _CORE_GUI_COMBOBOX_H

// core/gui/combobox.h
// 10/7/2011

#include <QComboBox>
#include <QCursor>

namespace Core { namespace Gui {

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

} } // namespace Gui, Core

#endif // _CORE_GUI_COMBOBOX_H
