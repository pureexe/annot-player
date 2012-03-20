#ifndef UISTYLE_H
#define UISTYLE_H

// uistyle.h
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
class QTextEdit;
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
  void setMenuEnabled(bool t);
public:
  bool isAeroEnabled() const { return aero_ && isAeroAvailable(); }
  bool isMenuEnabled() const { return menu_; }

public:
  explicit UiStyle(QObject *parent = 0);

  Theme theme() const;
  void setTheme(int tid);

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

#endif // UISTYLE_H
