#ifndef QXTOOLBUTTONWITHID_H
#define QXTOOLBUTTONWITHID_H

// qxtoolbuttonwithid.h
// 8/6/2011

#include "qxtoolbutton.h"

QTX_BEGIN_NAMESPACE

class QxToolButtonWithId : public QxToolButton
{
  Q_OBJECT
  Q_DISABLE_COPY(QxToolButtonWithId)
  typedef QxToolButtonWithId Self;
  typedef QxToolButton Base;

  int id_;

public:
  explicit QxToolButtonWithId(int id, QWidget *parent = nullptr)
    : Base(parent), id_(id) { init(); }
private:
  void init();

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

QTX_END_NAMESPACE

#endif // QXTOOLBUTTONWITHID_H
