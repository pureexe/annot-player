#ifndef ACWINDOW_H
#define ACWINDOW_H

// backlogview.h
// 2/18/2012

#include "module/qtext/draggablewidget.h"
#include <QtCore/QList>

class AcTextView;
class FadeAnimation;
typedef QtExt::DraggableWidget AcWindowBase;
class AcWindow : public AcWindowBase
{
  Q_OBJECT
  Q_PROPERTY(qreal windowOpacity READ windowOpacity WRITE setWindowOpacity)
  Q_DISABLE_COPY(AcWindow)
  typedef AcWindow Self;
  typedef AcWindowBase Base;

public:
  explicit AcWindow(QWidget *parent = 0, Qt::WindowFlags f = 0);

  bool isFadeEnabled() const { return fadeEnabled_; }
  void setFadeEnabled(bool t) { fadeEnabled_ = t; }
  FadeAnimation *fadeAnimation() const { return fadeAni_; }

  // - Events -
public slots:
  virtual void setVisible(bool visible); ///< \override
  void fadeOut();
protected:
  virtual void closeEvent(QCloseEvent *e); ///< \override
private:
  FadeAnimation *fadeAni_;
  bool fadeEnabled_;
};

#endif // ACWINDOW_H
