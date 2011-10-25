#ifndef UISTYLE_H
#define UISTYLE_H

// uistyle.h
// 7/30/2011

#include <QWidget>

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

  static Self *global_;
public:
  static Self *globalInstance()               { Q_ASSERT(global_); return global_; }
  static void setGlobalInstance(Self *global) { global_ = global; }

public:
  explicit UiStyle(QObject *parent = 0);

public:
  void setMainWindowStyle(QWidget *mw, bool useQss = true);
  void setWindowStyle(QWidget *w, bool useQss = true);
  void setMenuStyle(QMenu *menu, bool useQss = true);
  void setContextMenuStyle(QMenu *menu, bool useQss = true);
  void setToolButtonStyle(QToolButton *button, bool useQss = true);
  void setComboBoxStyle(QComboBox *box, bool useQss = true);

#ifdef USE_WIN_DWM
public slots:
  void setWindowDwmEnabled(QWidget *w, bool t = true);
  void setWindowDwmEnabled(WId wid, bool t = true);
#endif // USE_WIN_DWM
};

#endif // UISTYLE_H
