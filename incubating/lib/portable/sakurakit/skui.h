#ifndef SKUI_H
#define SKUI_H

// skui.h
// 9/22/2012

#include "sakurakit/skglobal.h"
#include <QtCore/QString>

QT_FORWARD_DECLARE_CLASS(QObject)
QT_FORWARD_DECLARE_CLASS(QToolButton)
QT_FORWARD_DECLARE_CLASS(QWindow)
QT_FORWARD_DECLARE_CLASS(QWidget)

SK_BEGIN_NAMESPACE

namespace SkUi
{

  void setAeroEffect(QWindow *w, bool enable = true);
  void setAeroEffect(QWidget *w, bool enable = true);

  /*
  enum StyleHint {
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

  inline const char *styleClass(ulong hints) { return nullptr; }

  QToolButton *createToolButton(ulong hints = 0,
      const QString &title = QString(), const QString &tip = QString(), const QString &key = QString(),
      QObject *receiver = nullptr, const char *slot = nullptr,  Qt::ConnectionType type = Qt::AutoConnection);
  QToolButton *createToolButton(ulong hints, const QString &title, const QString &tip,
      QObject *receiver, const char *slot = nullptr,  Qt::ConnectionType type = Qt::AutoConnection)
  { return createToolButton(hints, title, tip, QString(), receiver, slot, type); }
  QToolButton *createToolButton(ulong hints, const QString &title,
      QObject *receiver, const char *slot = nullptr,  Qt::ConnectionType type = Qt::AutoConnection)
  { return createToolButton(hints, title, QString(), QString(), receiver, slot, type); }
  */

} // namespace SkUi

SK_END_NAMESPACE

#endif // SKDECLARATIVEWINDOW_H
