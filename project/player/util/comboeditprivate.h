#ifndef COMBOEDITPRIVATE_H
#define COMBOEDITPRIVATE_H

// comboeditprivate.h
// 12/15/2011

#include <QLineEdit>
#include <QKeyEvent>

class ComboBoxLineEdit_ : public QLineEdit
{
  Q_OBJECT
  typedef ComboBoxLineEdit_ Self;
  typedef QLineEdit Base;

public:
  explicit ComboBoxLineEdit_(QWidget *parent = 0)
    : Base(parent) { }

protected:
  virtual void keyPressEvent(QKeyEvent *event) ///< \override
  {
    switch (event->key()) {
    case Qt::Key_Escape:
      clearFocus();    // FIXME: after clear focus, which window get new focus?
      event->accept();
      return;

    case Qt::Key_Return:
      event->accept();
      emit returnPressed();
      return;

    //case Qt::Key_Up:
    //  previous();
    //  event->accept();
    //  return;
    //case Qt::Key_Down:
    //  next();
    //  event->accept();
    //  return;
    }

    Base::keyPressEvent(event);
  }
};

#endif // COMBOEDITPRIVATE_H
