#ifndef MESSAGEVIEW_H
#define MESSAGEVIEW_H

// messageview.h
// 10/16/2011

#include "processinfo.h"
#include "module/qtext/dialog.h"
#include <QtCore/QVector>

QT_BEGIN_NAMESPACE
class QComboBox;
class QTextEdit;
class QToolButton;
QT_END_NAMESPACE

class RadioButtonGrid;

class MessageView : public QtExt::Dialog
{
  Q_OBJECT
  Q_DISABLE_COPY(MessageView)
  typedef MessageView Self;
  typedef QtExt::Dialog Base;

  //enum HookIndex {
  //  HI_AllHooks = 0,
  //  HI_Count
  //};

public:
  explicit MessageView(QWidget *parent = nullptr);

signals:
  void channelSelected(ulong anchor, const QString &function);

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
  ulong currentAnchor() const;
  QString currentFunction() const;
  int currentIndex() const;
  bool isEmpty() const;

  // - Actions -

public slots:
  void clear();
  void select();
  //void processHookedText(const QString &text, ulong id);
  void processMessage(const QByteArray &data, ulong anchor, const QString &function);

  void setActive(bool active);

  void addMessages(const QList<QByteArray> &l, ulong id, const QString &function);
  void setCurrentIndex(int index);

protected slots:
  void setData(const QList<QByteArray> &l);
  void setCurrentText(int index);
  //void invalidateHookCountLabel();
  void invalidateCurrentCharFormat();
  void invalidateSelectButton();
  //void invalidateCurrentHook();

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

private:
  void createLayout();

private:
  bool active_;
  QString processName_;
  QTextEdit *textEdit_;

  //QSpinBox *hookIndexEdit_;
  //QLabel *hookCountLabel_;

  QComboBox *encodingEdit_;

  QVector<ulong> anchors_;
  QVector<QString> functions_;
  QVector<QList<QByteArray> > messages_;

  QToolButton *selectButton_;

  RadioButtonGrid *channelGrid_;
};

#endif // MESSAGEVIEW_H
