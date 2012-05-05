#ifndef ACCOMBOEDIT_P_H
#define ACCOMBOEDIT_P_H

// accomboedit_p.h
// 12/15/2011

#include <QtGui/QLineEdit>
#include <QtGui/QKeyEvent>

class AcComboBoxLineEdit : public QLineEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(AcComboBoxLineEdit)
  typedef AcComboBoxLineEdit Self;
  typedef QLineEdit Base;

public:
  explicit AcComboBoxLineEdit(QWidget *parent = 0)
    : Base(parent) { }

public slots:
  void enterText(const QString &text)
  {
    setText(text);
    emit returnPressed();
  }

protected:
  virtual void keyPressEvent(QKeyEvent *event) ///< \override
  {
    switch (event->key()) {
    case Qt::Key_Escape:
      clearFocus();
      event->accept();
      return;
    case Qt::Key_Return:
      event->accept();
      emit returnPressed();
      return;
    }
    Base::keyPressEvent(event);
  }
};

#endif // ACCOMBOEDIT_P_H
