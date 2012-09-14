#ifndef THREADVIEW_H
#define THREADVIEW_H

// threadview.h
// 10/16/2011

#include "textmessage.h"
#include "processinfo.h"
#include "textthread.h"
#include "lib/qtext/dialog.h"
#include <QtCore/QList>
#include <QtCore/QVector>

QT_BEGIN_NAMESPACE
class QComboBox;
class QTextEdit;
class QToolButton;
QT_END_NAMESPACE

class CheckBoxGrid;
class RadioButtonGrid;
class MessageHandler;
class HookDialog;

class ThreadView : public QtExt::Dialog
{
  Q_OBJECT
  Q_DISABLE_COPY(ThreadView)
  typedef ThreadView Self;
  typedef QtExt::Dialog Base;

  //enum HookIndex {
  //  HI_AllHooks = 0,
  //  HI_Count
  //};

public:
  explicit ThreadView(MessageHandler *h, QWidget *parent = nullptr);

signals:
  void threadsSelected(const TextThreadList &threads);

  void message(QString msg);
  void warning(QString msg);
  void errorMessage(QString msg);
  void notification(QString msg);

  // - Properties -
public:
   bool isActive() const { return active_; }

   const QString &processName() const { return processName_; }
public slots:
   void setProcessName(const QString &name) { processName_ = name; }
   void clearProcessName() { processName_.clear(); }

   void setProcessNameFromProcessInfo(const ProcessInfo &pi)
   {
     if (pi.isValid())
       setProcessName(pi.processName);
     else
       clearProcessName();
   }

protected:
  bool isEmpty() const { return signatures_.isEmpty(); }

  // - Actions -

public slots:
  void clear();
  void reset();
  void save();
  void processMessage(const QByteArray &data, qint64 signature, const QString &provider);

  void setActive(bool active);

  void addMessages(const QList<QByteArray> &l, qint64 signature, const QString &provider);
  void setThreads(const TextThreadList &l);

protected slots:
  void updateText();
  void updateGrid();
  //void invalidateHookCountLabel();
  void invalidateCurrentCharFormat();
  //void invalidateCurrentHook();

  void updateButtons();
  void refresh();
  void refreshEncodingEdit();

  // - Events -
public:
  void setVisible(bool visible) override;

//protected slots:
//  void dragEnterEvent(QDragEnterEvent *event) override;
//  void dragMoveEvent(QDragMoveEvent *event) override;
//  void dragLeaveEvent(QDragLeaveEvent *event) override;
//  void dropEvent(QDropEvent *event) override;
//
//signals:
//  void dragEnterEventReceived(QDragEnterEvent *event);
//  void dragMoveEventReceived(QDragMoveEvent *event);
//  void dragLeaveEventReceived(QDragLeaveEvent *event);
//  void dropEventReceived(QDropEvent *event);

  // - Implementation -
protected:
  //static bool isBetterHook(ulong goodHookId, ulong badHookId);

protected slots:
  void setEncoding(const QString &name);
  void showHook();

private:
  void createLayout();

private:
  bool active_;
  MessageHandler *messageHandler_;
  QString processName_;
  QTextEdit *textEdit_;

  //QSpinBox *hookIndexEdit_;
  //QLabel *hookCountLabel_;

  QComboBox *encodingEdit_;

  QVector<qint64> signatures_;

  QList<TextMessage> messages_;

  QToolButton *saveButton_, *resetButton_, *hookButton_;

  RadioButtonGrid *leadingThreads_;
  CheckBoxGrid *supportThreads_;
  HookDialog *hookDialog_;
};

#endif // THREADVIEW_H
