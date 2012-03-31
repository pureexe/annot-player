#ifndef _AC_ACUI_H
#define _AC_ACUI_H

// ac/acui.h
// 7/30/2011

#include <QWidget>
#include <QWidgetList>

QT_BEGIN_NAMESPACE
class QLabel;
class QDialog;
class QMenu;
class QComboBox;
class QToolButton;
class QRadioButton;
class QLineEdit;
class QStatusBar;
class QTextEdit;
QT_END_NAMESPACE

// - DWM -
#ifdef USE_WIN_DWM
  #define DWM_ENABLE_AERO_WIDGET(_w) \
  { \
    Q_ASSERT(_w); \
    if (Dwm::isCompositionEnabled()) { \
      Dwm::enableBlurBehindWindow((_w)); \
      Dwm::extendFrameIntoClientArea((_w)); \
      (_w)->setAttribute(Qt::WA_TranslucentBackground); \
      (_w)->setAttribute(Qt::WA_NoSystemBackground); \
    } \
  }

  #define DWM_DISABLE_AERO_WIDGET(_w) \
  { \
    Q_ASSERT(_w); \
    if (Dwm::isCompositionEnabled()) { \
      Dwm::enableBlurBehindWindow((_w), false); \
      Dwm::extendFrameIntoClientArea((_w), 0, 0, 0, 0); \
      (_w)->setAttribute(Qt::WA_TranslucentBackground, false); \
      (_w)->setAttribute(Qt::WA_NoSystemBackground, false); \
    } \
  }

  #define DWM_ENABLE_ONETIME_AERO_WIDGET(_w) \
  { \
    Q_ASSERT(_w); \
    if (Dwm::isCompositionEnabled()) { \
      Dwm::enableBlurBehindWindow((_w)->winId()); \
      Dwm::extendFrameIntoClientArea((_w)->winId()); \
      (_w)->setAttribute(Qt::WA_TranslucentBackground); \
      (_w)->setAttribute(Qt::WA_NoSystemBackground); \
    } \
  }

#else
  #define DWM_ENABLE_AERO_WIDGET(_w)            ({ })
  #define DWM_DISABLE_AERO_WIDGET(_w)           ({ })
  #define DWM_ENABLE_ONETIME_AERO_WIDGET(_w) ({ })
#endif // USE_WIN_DWM


#define AC_CONTEXTMENU_OPACITY           0.8
#define AC_CONTEXTMENU_COLOR             "orange"
#define AC_CONTEXTMENU_COLOR_STRENGTH    1       // 0 = no color, 1 = full color (default)
#define AC_TOOLBUTTON_BLUR_RADIUS        1.2     // By default, the blur radius is 5 pixels.

#define AC_DIALOG_COLOR                  "blue"
#define AC_DIALOG_COLOR_STRENGTH         1       // 0 = no color, 1 = full color (default)

#define AC_WINDOW_ALPHA                  240
#define AC_WINDOW_OPACITY                0.95


class AcUi : public QObject
{
  Q_OBJECT
  typedef AcUi Self;
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

protected:
  explicit AcUi(QObject *parent = 0);

signals:
  void aeroEnabledChanged(bool t);
  void menuEnabledChanged(bool t);

public slots:
  void setAeroEnabled(bool t) { aero_ = t; emit aeroEnabledChanged(t); }
  void setMenuEnabled(bool t) { menu_ = t; emit menuEnabledChanged(t); }
  void setTheme(int tid);
public:
  bool isAeroEnabled() const { return aero_ && isAeroAvailable(); }
  bool isMenuEnabled() const { return menu_; }
  Theme theme() const { return theme_; }

  // - Component maker -

public:
  enum RenderHint {
    NoHint = 0x0,
    PushHint = 0x1,             // [ push button ]
    CheckHint = 0x1 << 1,       // / checkable button /
    TabHint = 0x1 << 2,         // - tab button -
    HighlightHint = 0x1 << 3,   // with highlighted text
    InvertHint = 0x1 << 4,      // with inverted color
    UrlHint = 0x1 << 5,         // as URL
    BuddyHint = 0x1 << 6,       // label as buddy
    ReadOnlyHint = 0x1 << 7,    // read-only or editable
    EditHint = 0x1 << 8,        // used as line edit
    PasswordHint = 0x1 << 9,
    MenuHint = 0x1 << 10
  };

  QTextEdit *makeTextEdit(ulong hints = 0, const QString &tip = QString());

  QLabel *makeLabel(ulong hints = 0,
      const QString &title = QString(), const QString &tip = QString(), QWidget *buddy = 0);
  QLabel *makeLabel(ulong hints, const QString &title, QWidget *buddy)
  { return makeLabel(hints, title, QString(), buddy); }

  QComboBox *makeComboBox(ulong hints = 0,
      const QString &text = QString(), const QString &tip = QString(), const QStringList &items = QStringList());
  QLineEdit *makeLineEdit(ulong hints = 0,
      const QString &text = QString(), const QString &tip = QString());
  QRadioButton *makeRadioButton(ulong hints = 0,
      const QString &title = QString(), const QString &tip = QString());

  QToolButton *makeToolButton(ulong hints = 0,
      const QString &title = QString(), const QString &tip = QString(), const QString &key = QString(),
      QObject *receiver = 0, const char *slot = 0,  Qt::ConnectionType type = Qt::AutoConnection);
  QToolButton *makeToolButton(ulong hints,
      const QString &title, const QString &tip,
      QObject *receiver, const char *slot = 0,  Qt::ConnectionType type = Qt::AutoConnection)
  { return makeToolButton(hints, title, tip, QString(), receiver, slot, type); }
  QToolButton *makeToolButton(ulong hints,
      const QString &title,
      QObject *receiver, const char *slot = 0,  Qt::ConnectionType type = Qt::AutoConnection)
  { return makeToolButton(hints, title, QString(), QString(), receiver, slot, type); }

  // - Stylers -

public:
  void setMainWindowStyle(QWidget *w);
  void setWindowStyle(QWidget *w, bool persistent = true);
  //void setMenuStyle(QMenu *menu);
  void setContextMenuStyle(QMenu *menu, bool persistent);
  void setToolButtonStyle(QToolButton *button);

  void setStatusBarStyle(QStatusBar *w);

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
  bool aero_, menu_;
  Theme theme_;
  QWidgetList windows_; // windows with customized background
  QList<QMenu*> menus_; // menus with customized background
};

#endif // _AC_ACUI_H
