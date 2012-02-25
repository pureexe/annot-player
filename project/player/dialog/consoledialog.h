#ifndef CONSOLEDIALOG_H
#define CONSOLEDIALOG_H

// backlogview.h
// 2/18/2012

#include "module/qtext/dialog.h"
#include <QList>

class TextEditTabView;

typedef QtExt::Dialog ConsoleDialogBase;
class ConsoleDialog : public ConsoleDialogBase
{
  Q_OBJECT
  typedef ConsoleDialog Self;
  typedef ConsoleDialogBase Base;

  static QList<Self*> instances_;

  enum TabIndex {
    LogTabIndex = 0,
    DebugTabIndex,
    TabIndexCount
  };

public:
  explicit ConsoleDialog(QWidget *parent = 0);
  ~ConsoleDialog();

  static void messageHandler(QtMsgType type, const char *msg);

public slots:
  void appendLogText(const QString &text);
  void appendDebugText(const QString &text);
  void clear();

  // - Implementation -
signals:
  void debugMessageReceived(QString msg); ///< \internal
private slots:
  void emit_debugMessageReceived(const QString &msg) { emit debugMessageReceived(msg); }

private:
  void createLayout();

private:
  TextEditTabView *tabView_;
};

#endif // CONSOLEDIALOG_H
