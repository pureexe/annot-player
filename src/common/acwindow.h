#ifndef ACWINDOW_H
#define ACWINDOW_H

// acwindow.h
// 5/5/2012

#include "lib/qtext/draggablewidget.h"
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
  explicit AcWindow(QWidget *parent = nullptr, Qt::WindowFlags f = 0);

  bool isFadeEnabled() const { return fadeEnabled_; }
  void setFadeEnabled(bool t) { fadeEnabled_ = t; }
  FadeAnimation *fadeAnimation() const { return fadeAni_; }

  bool isRippleEnabled() const { return rippleEnabled_; }
  void setRippleEnabled(bool t);

  bool isWindowOnTop() const { return windowFlags() & Qt::WindowStaysOnTopHint; }
public slots:
  void setWindowOnTop(bool t);

  // - Events -
public slots:
  void setVisible(bool visible) override;
  void fadeOut();
protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void closeEvent(QCloseEvent *e) override;
private:
  FadeAnimation *fadeAni_;
  bool fadeEnabled_;

  bool rippleEnabled_;
  RippleImageFilter *rippleFilter_;
  QTimer *rippleTimer_;
};

#endif // ACWINDOW_H
