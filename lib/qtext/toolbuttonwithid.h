#ifndef _QTEXT_TOOLBUTTONWITHID_H
#define _QTEXT_TOOLBUTTONWITHID_H

// qtext/toolbutton.h
// 8/6/2011

#include "lib/qtext/toolbutton.h"

namespace QtExt {

class ToolButtonWithId : public ToolButton
{
  Q_OBJECT
  Q_DISABLE_COPY(ToolButtonWithId)
  typedef ToolButtonWithId Self;
  typedef ToolButton Base;

  int id_;

public:
  explicit ToolButtonWithId(int id, QWidget *parent = nullptr);

signals:
  void clickedWithId(int id, bool checked = false);
  void pressedWithId(int id);
  void releasedWithId(int id);
  void toggledWithId(int id, bool checked);

public:
  int id() const { return id_; }
public slots:
  void setId(int id) { id_ = id; }

// - Implementation -
private slots:
  void emit_clickedWithId(bool checked = false) { emit clickedWithId(id_, checked); }
  void emit_pressedWithId() { emit pressedWithId(id_); }
  void emit_releasedWithId() { emit releasedWithId(id_); }
  void emit_toggledWithId(bool checked) { emit toggledWithId(id_, checked); }
};

} // namespace QtExt

#endif // _QTEXT_TOOLBUTTONWITHID_H
