#ifndef ACUI_H
#define ACUI_H

// ac/acui.h
// 7/30/2011

#include <QtGui/QColor>
#include <QtGui/QWidget>
#include <QtGui/QWidgetList>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QDialog;
class QGraphicsEffect;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QRadioButton;
class QStatusBar;
class QTextBrowser;
class QTextEdit;
class QToolButton;
QT_END_NAMESPACE

// - DWM -
#ifdef WITH_WIN_DWM
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
# define DWM_ENABLE_AERO_WIDGET(_w)            ({ })
# define DWM_DISABLE_AERO_WIDGET(_w)           ({ })
# define DWM_ENABLE_ONETIME_AERO_WIDGET(_w) ({ })
#endif // WITH_WIN_DWM

//#define AC_CONTEXTMENU_OPACITY           0.8
//#define AC_CONTEXTMENU_COLOR             Qt::white
//#define AC_CONTEXTMENU_COLOR_STRENGTH    1       // 0 = no color, 1 = full color (default)
#define AC_TOOLBUTTON_BLUR_RADIUS        1.2     // By default, the blur radius is 5 pixels.

#define AC_DIALOG_COLOR                  "blue"
#define AC_DIALOG_COLOR_STRENGTH         1       // 0 = no color, 1 = full color (default)

#define AC_WINDOW_ALPHA                  240
#define AC_WINDOW_OPACITY                0.95


class AcUi : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AcUi)
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
  explicit AcUi(QObject *parent = nullptr);

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
    DefaultHint = 0x1,
    PushHint = 0x1 << 1,        // [ push button ]
    CheckHint = 0x1 << 2,       // / checkable button /
    TabHint = 0x1 << 3,         // - tab button -
    HighlightHint = 0x1 << 4,   // with highlighted text
    InvertHint = 0x1 << 5,      // with inverted color
    UrlHint = 0x1 << 6,         // as URL
    BuddyHint = 0x1 << 7,       // label as buddy
    ReadOnlyHint = 0x1 << 8,    // read-only or editable
    EditHint = 0x1 << 9,        // used as line edit
    PasswordHint = 0x1 << 10,
    MenuHint = 0x1 << 11
  };

  QTextEdit *makeTextEdit(ulong hints = 0, const QString &tip = QString());
  QTextBrowser *makeTextBrowser(ulong hints = 0, const QString &tip = QString());

  QLabel *makeLabel(ulong hints = 0,
      const QString &title = QString(), const QString &tip = QString(), QWidget *buddy = nullptr);
  QLabel *makeLabel(ulong hints, const QString &title, QWidget *buddy)
  { return makeLabel(hints, title, QString(), buddy); }

  QComboBox *makeComboBox(ulong hints = 0,
      const QString &text = QString(), const QString &tip = QString(), const QString &holder = QString(), const QStringList &items = QStringList());
  QLineEdit *makeLineEdit(ulong hints = 0,
      const QString &text = QString(), const QString &tip = QString(), const QString &holder = QString());
  QGroupBox *makeGroupBox(ulong hints = 0,
      const QString &text = QString(), const QString &tip = QString());

  QGraphicsEffect *makeHaloEffect(const QColor &c);
  QGraphicsEffect *makeHaloEffect(Qt::GlobalColor c) { return makeHaloEffect(QColor(c)); }
  QGraphicsEffect *makeHaloEffect(ulong hints = DefaultHint);

  QCheckBox *makeCheckBox(ulong hints = 0,
      const QString &text = QString(), const QString &tip = QString(), const QString &key = QString(),
      QObject *receiver = nullptr, const char *slot = nullptr,  Qt::ConnectionType type = Qt::AutoConnection);
  QCheckBox *makeCheckBox(ulong hints, const QString &text, const QString &tip,
      QObject *receiver , const char *slot = nullptr,  Qt::ConnectionType type = Qt::AutoConnection)
  { return makeCheckBox(hints, text, tip, QString(), receiver, slot, type); }
  QCheckBox *makeCheckBox(ulong hints, const QString &text,
      QObject *receiver, const char *slot = nullptr,  Qt::ConnectionType type = Qt::AutoConnection)
  { return makeCheckBox(hints, text, QString(), QString(), receiver, slot, type); }

  QRadioButton *makeRadioButton(ulong hints = 0,
      const QString &text = QString(), const QString &tip = QString(), const QString &key = QString(),
      QObject *receiver = nullptr, const char *slot = nullptr,  Qt::ConnectionType type = Qt::AutoConnection);
  QRadioButton *makeRadioButton(ulong hints, const QString &text, const QString &tip,
      QObject *receiver, const char *slot = nullptr,  Qt::ConnectionType type = Qt::AutoConnection)
  { return makeRadioButton(hints, text, tip, QString(), receiver, slot, type); }
  QRadioButton *makeRadioButton(ulong hints, const QString &text,
      QObject *receiver, const char *slot = nullptr,  Qt::ConnectionType type = Qt::AutoConnection)
  { return makeRadioButton(hints, text, QString(), QString(), receiver, slot, type); }

  QToolButton *makeToolButton(ulong hints = 0,
      const QString &title = QString(), const QString &tip = QString(), const QString &key = QString(),
      QObject *receiver = nullptr, const char *slot = nullptr,  Qt::ConnectionType type = Qt::AutoConnection);
  QToolButton *makeToolButton(ulong hints, const QString &title, const QString &tip,
      QObject *receiver, const char *slot = nullptr,  Qt::ConnectionType type = Qt::AutoConnection)
  { return makeToolButton(hints, title, tip, QString(), receiver, slot, type); }
  QToolButton *makeToolButton(ulong hints, const QString &title,
      QObject *receiver, const char *slot = nullptr,  Qt::ConnectionType type = Qt::AutoConnection)
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
  void removeWindowBackground(QWidget *w);
  void setMenuBackground(QMenu *m, bool persistent); ///< If the widget is persistant
  void setBlackBackground(QWidget *w);

  const char *backgroundImage() const; // rc for current theme

public slots:
  void updateBackground();

#ifdef WITH_WIN_DWM
public slots:
  void setWindowDwmEnabled(QWidget *w, bool t = true);
  void setWindowDwmEnabled(WId wid, bool t = true);

  void setDwmEnabled(bool t); ///< global
private:
  QWidgetList dwmEnabledWindows_;
#endif // WITH_WIN_DWM

private:
  bool aero_, menu_;
  Theme theme_;
  QWidgetList windows_; // windows with customized background
  QList<QMenu*> menus_; // menus with customized background
};

#endif // ACUI_H
