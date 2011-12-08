#ifndef MESSAGEVIEW_H
#define MESSAGEVIEW_H

// messageview.h
// 10/16/2011

#include "core/gui/dialog.h"
#include <QTextCharFormat>
#include <QColor>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QTextEdit;
class QToolButton;
QT_END_NAMESPACE

typedef Core::Gui::Dialog MessageViewBase;
class MessageView : public MessageViewBase
{
  Q_OBJECT
  typedef MessageView Self;
  typedef MessageViewBase Base;

  enum HookIndex {
    HI_AllHooks = 0,
    HI_Count
  };

public:
  explicit MessageView(QWidget *parent = 0);

signals:
  void hookSelected(int hookId);

  // - Properties -
public:
   bool isActive() const;

protected:
  int currentHookId() const;
  int currentIndex() const;

  // - Actions -

public slots:
  void clear();
  void selectCurrentHook();
  void processHookedText(const QString &text, int hookId);

  void setActive(bool active);

  void addMessages(QStringList &messages, int hookId);
  void setCurrentIndex(int index);

protected slots:
  void setTextList(const QStringList& text);
  void selectHookIndex(int index);
  void invalidateHookCountLabel();
  void invalidateCurrentCharFormat();
  void invalidateSelectButton();

  // - Events -
public:
  virtual void setVisible(bool visible); ///< \override

protected slots:
  virtual void dragEnterEvent(QDragEnterEvent *event); ///< \override
  virtual void dragMoveEvent(QDragMoveEvent *event); ///< \override
  virtual void dragLeaveEvent(QDragLeaveEvent *event); ///< \override
  virtual void dropEvent(QDropEvent *event); ///< \override

signals:
  void dragEnterEventReceived(QDragEnterEvent *event);
  void dragMoveEventReceived(QDragMoveEvent *event);
  void dragLeaveEventReceived(QDragLeaveEvent *event);
  void dropEventReceived(QDropEvent *event);

private:
  bool active_;
  QComboBox *hookComboBox_;
  QTextEdit *textEdit_;
  QLabel *hookCountLabel_;

  QVector<int> hooks_;
  QVector<QStringList> texts_;

  QToolButton *selectButton_;
};

#endif // MESSAGEVIEW_H
