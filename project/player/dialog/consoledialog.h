#ifndef CONSOLEDIALOG_H
#define CONSOLEDIALOG_H

// consoledialog.h
// 2/18/2012

#include "project/common/acwindow.h"
#include <QtCore/QList>

class TextEditTabView;

class ConsoleDialog : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(ConsoleDialog)
  typedef ConsoleDialog Self;
  typedef AcWindow Base;

  static QList<Self *> instances_;

  enum TabIndex {
    DebugTabIndex = 0,
    LogTabIndex,
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
