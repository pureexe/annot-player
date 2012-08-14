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

signals:
  void message(const QString &text);
  void errorMessage(const QString &text);
  void warning(const QString &text);
  void notification(const QString &text);
protected slots:
  void showMessage(const QString &text);
  void showError(const QString &text);
  void warn(const QString &text);
  void notify(const QString &text);

  // - Construction -
public:
  explicit AcMainWindow(QWidget *parent = nullptr, Qt::WindowFlags f = 0);

  bool isFadeEnabled() const { return fadeEnabled_; }
  void setFadeEnabled(bool t) { fadeEnabled_ = t; }
  FadeAnimation *fadeAnimation() const { return fadeAni_; }

  bool isRippleEnabled() const { return rippleEnabled_; }
  void setRippleEnabled(bool t);

  bool isWindowOnTop() const { return windowFlags() & Qt::WindowStaysOnTopHint; }
public slots:
  void setWindowOnTop(bool t);

public slots:
  void fadeOut();

protected:
  bool autoHideMenuBar() const { return autoHideMenuBar_; }
protected slots:
  void setAutoHideMenuBar(bool t) { autoHideMenuBar_ = t; }

  // - Events -
public slots:
  void setVisible(bool visible) override;
protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void closeEvent(QCloseEvent *e) override;

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
