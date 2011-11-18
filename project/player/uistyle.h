#ifndef UISTYLE_H
#define UISTYLE_H

// uistyle.h
// 7/30/2011

#include <QWidget>
#include <QList>

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
    BlackTheme1, BlackTheme2,
    BlueTheme1, BlueTheme2,
    BrownTheme1, BrownTheme2,
    GreenTheme1, GreenTheme2,
    LightBlueTheme1, LightBlueTheme2,
    OrangeTheme1, OrangeTheme2,
    PinkTheme1, PinkTheme2,
    PurpleTheme1, PurpleTheme2,
    RedTheme1, RedTheme2,
    YellowTheme1, YellowTheme2,
    WindowsTheme1, WindowsTheme2,
    ThemeCount
  };

public:
  static Self *globalInstance()               { Q_ASSERT(global_); return global_; }
  static void setGlobalInstance(Self *global) { global_ = global; }
  static bool isAeroAvailable();
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

public slots:
  void invalidateBackground();

#ifdef USE_WIN_DWM
public slots:
  void setWindowDwmEnabled(QWidget *w, bool t = true);
  void setWindowDwmEnabled(WId wid, bool t = true);
#endif // USE_WIN_DWM

  // - Implementations -

protected:
  const char *backgroundImagePath() const; // rc for current theme

private:
  static Self *global_;
  Theme theme_;
  QList<QWidget*> windows_; // windows with customized background
  QList<QMenu*> menus_; // menus with customized background
};

#endif // UISTYLE_H
