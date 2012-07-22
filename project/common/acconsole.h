#ifndef ACCONSOLE_H
#define ACCONSOLE_H

// acconsole.h
// 2/18/2012

#include "project/common/acwindow.h"
#include <QtCore/QList>

class AcTextView;
class AcConsole : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(AcConsole)
  typedef AcConsole Self;
  typedef AcWindow Base;

  static QList<Self *> instances_;

public:
  explicit AcConsole(QWidget *parent = nullptr);
  ~AcConsole();

  static void messageHandler(QtMsgType type, const char *msg);

  // - Implementation -
signals:
  void debugMessageReceived(QString msg); ///< \internal
private slots:
  void emit_debugMessageReceived(const QString &msg) { emit debugMessageReceived(msg); }

private:
  void createLayout();

private:
  AcTextView *textView_;
};

#endif // ACCONSOLE_H
