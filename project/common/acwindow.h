#ifndef ACWINDOW_H
#define ACWINDOW_H

// acwindow.h
// 5/5/2012

#include "module/qtext/draggablewidget.h"
#include <QtCore/QList>

#define AC_WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowStaysOnTopHint | \
  Qt::WindowCloseButtonHint )

class AcTextView;
class FadeAnimation;
class RippleImageFilter;
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

  bool isRippleEnabled() const { return rippleEnabled_; }
  void setRippleEnabled(bool t);

  // - Events -
public slots:
  virtual void setVisible(bool visible); ///< \reimp
  void fadeOut();
protected:
  virtual void mousePressEvent(QMouseEvent *event); ///< \reimp
  virtual void mouseMoveEvent(QMouseEvent *event); ///< \reimp
  virtual void mouseReleaseEvent(QMouseEvent *event); ///< \reimp
  virtual void paintEvent(QPaintEvent *event); ///< \reimp
  virtual void closeEvent(QCloseEvent *e); ///< \reimp
private:
  FadeAnimation *fadeAni_;
  bool fadeEnabled_;

  bool rippleEnabled_;
  RippleImageFilter *rippleFilter_;
  QTimer *rippleTimer_;
};

#endif // ACWINDOW_H
