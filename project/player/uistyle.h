#ifndef UISTYLE_H
#define UISTYLE_H

// uistyle.h
// 7/30/2011

#include <QWidget>
#include <QWidgetList>

QT_BEGIN_NAMESPACE
class QDialog;
class QMenu;
class QToolButton;
class QComboBox;
QT_END_NAMESPACE

class UiStyle : public QObject
{
  Q_OBJECT
  typedef UiStyle Self;
  typedef QObject Base;

  // - Constructions -

public:
  enum Theme {
    DefaultTheme = 0, RandomTheme,
    DarkTheme,
    BlackTheme,
    BlueTheme,
    BrownTheme,
    CyanTheme,
    GrayTheme,
    GreenTheme,
    PinkTheme,
    PurpleTheme,
    RedTheme,
    WhiteTheme,
    YellowTheme,
    ThemeCount
  };

public:
  static Self *globalInstance() { static Self g; return &g; }
  static bool isAeroAvailable();
public slots:
  void setAeroEnabled(bool t);
  void setAeroDisabled(bool t) { setAeroEnabled(!t); }
public:
  bool isAeroEnabled() const { return aero_ && isAeroAvailable(); }

public:
  explicit UiStyle(QObject *parent = 0);

  Theme theme() const;
  void setTheme(int tid);

  // - Stylers -

public:
  void setMainWindowStyle(QWidget *w);
  void setWindowStyle(QWidget *w, bool persistent = true);
  //void setMenuStyle(QMenu *menu);
  void setContextMenuStyle(QMenu *menu, bool persistent);
  void setToolButtonStyle(QToolButton *button);
  void setComboBoxStyle(QComboBox *box);

  void setWindowBackground(QWidget *w, bool persistent); ///< If the widget is persistant
  void setMenuBackground(QMenu *m, bool persistent); ///< If the widget is persistant
  void setBlackBackground(QWidget *w);

public slots:
  void invalidateBackground();

#ifdef USE_WIN_DWM
public slots:
  void setWindowDwmEnabled(QWidget *w, bool t = true);
  void setWindowDwmEnabled(WId wid, bool t = true);

  void setDwmEnabled(bool t); ///< global
private:
  QWidgetList dwmEnabledWindows_;
#endif // USE_WIN_DWM

  // - Implementations -

protected:
  const char *backgroundImagePath() const; // rc for current theme

private:
  bool aero_;
  Theme theme_;
  QWidgetList windows_; // windows with customized background
  QList<QMenu*> menus_; // menus with customized background
};

#endif // UISTYLE_H
