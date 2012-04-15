#ifndef TEXTEDITTABVIEW_H
#define TEXTEDITTABVIEW_H

// backlogview.h
// 1/1/2012

#include <QWidget>
#include <QList>

QT_FORWARD_DECLARE_CLASS(QToolButton)
QT_FORWARD_DECLARE_CLASS(QStackedLayout)

class AcTextView;

class TextEditTabView : public QWidget
{
  Q_OBJECT
  typedef TextEditTabView Self;
  typedef QWidget Base;

public:
  explicit TextEditTabView(QWidget *parent = 0)
  : Base(parent), tabCount_(0), tabIndex_(0)
  { initializeLayout(); }

  int tabCount() const { return tabCount_; }

public slots:
  void appendText(const QString &text, int tab);
  void clear(int tab);

protected slots:
  void setTab(int index);
  void previousTab();
  void nextTab();
  void invalidateTabIndex();

public:
  void addTab(const QString &name);
  void finalizeLayout();
private:
  void initializeLayout();

private:
  QStackedLayout *stackLayout_;

  int tabCount_;
  int tabIndex_;
  QList<QToolButton *> tabButtons_;
  QList<AcTextView *> tabViews_;
};

#endif // TEXTEDITTABVIEW_H
