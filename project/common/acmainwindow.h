#ifndef ACMAINWINDOW_H
#define ACMAINWINDOW_H

// acmainwindow.h
// 2/17/2012

#include "module/qtext/draggablemainwindow.h"

QT_FORWARD_DECLARE_CLASS(QTimer)

class FadeAnimation;
class RippleImageFilter;

typedef QtExt::DraggableMainWindow AcMainWindowBase;
class AcMainWindow : public AcMainWindowBase
{
  Q_OBJECT
  Q_PROPERTY(qreal windowOpacity READ windowOpacity WRITE setWindowOpacity)
  Q_DISABLE_COPY(AcMainWindow)
  typedef AcMainWindow Self;
  typedef AcMainWindowBase Base;

  // - Construction -
public:
  explicit AcMainWindow(QWidget *parent = 0, Qt::WindowFlags f = 0);

  bool isFadeEnabled() const { return fadeEnabled_; }
  void setFadeEnabled(bool t) { fadeEnabled_ = t; }
  FadeAnimation *fadeAnimation() const { return fadeAni_; }

  bool isRippleEnabled() const { return rippleEnabled_; }
  void setRippleEnabled(bool t);

  //-  Log -
signals:
  void message(const QString &text);
  void errorMessage(const QString &text);
  void warning(const QString &text);
  void notification(const QString &text);

public slots:
  void fadeOut();

protected slots:
  void showMessage(const QString &text);
  void error(const QString &text);
  void warn(const QString &text);
  void notify(const QString &text);

protected:
  bool autoHideMenuBar() const { return autoHideMenuBar_; }
protected slots:
  void setAutoHideMenuBar(bool t) { autoHideMenuBar_ = t; }

  // - Events -
public slots:
  virtual void setVisible(bool visible); ///< \override
protected:
  virtual void mousePressEvent(QMouseEvent *event); ///< \override
  virtual void mouseMoveEvent(QMouseEvent *event); ///< \override
  virtual void mouseReleaseEvent(QMouseEvent *event); ///< \override
  virtual void paintEvent(QPaintEvent *event); ///< \override
  virtual void closeEvent(QCloseEvent *e); ///< \override

private:
  QTimer *messageTimer_;
  bool autoHideMenuBar_;

  FadeAnimation *fadeAni_;
  bool fadeEnabled_;

  bool rippleEnabled_;
  RippleImageFilter *rippleFilter_;
  QTimer *rippleTimer_;
};

#endif // ACMAINWINDOW_H
