#ifndef CONSOLEDIALOG_H
#define CONSOLEDIALOG_H

// consoledialog.h
// 2/18/2012

#include "src/common/acwindow.h"
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
  explicit ConsoleDialog(QWidget *parent = nullptr);
  ~ConsoleDialog();

  static void messageHandler(QtMsgType type, const char *msg);

signals:
  void asyncLogText(const QString &text);
  void asyncDebugText(const QString &text);
public slots:
  void sendLogText(const QString &text) { emit asyncLogText(text); }
  void sendDebugText(const QString &text) { emit asyncDebugText(text); }

  void appendLogText(const QString &text);
  void appendDebugText(const QString &text);

  void clear();
private:
  void createLayout();

private:
  TextEditTabView *tabView_;
};

#endif // CONSOLEDIALOG_H
