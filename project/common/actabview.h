#ifndef ACTABVIEW_H
#define ACTABVIEW_H

// backlogview.h
// 1/1/2012

#include <QtGui/QWidget>
#include <QtGui/QWidgetList>

QT_FORWARD_DECLARE_CLASS(QToolButton)
QT_FORWARD_DECLARE_CLASS(QStackedLayout)

class AcTabView : public QWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(AcTabView)
  typedef AcTabView Self;
  typedef QWidget Base;

public:
  explicit AcTabView(QWidget *parent = nullptr)
  : Base(parent), tabCount_(0), tabIndex_(0)
  { initializeLayout(); }

  int tabCount() const { return tabCount_; }
  QWidget *widget(int index) const { return tabs_[index]; }
  const QWidgetList &widgets() const { return tabs_; }

  QWidget *currentWidget() const
  { return tabIndex_ >= 0 && tabIndex_ < tabs_.size() ? tabs_[tabIndex_] : (QWidget*)0; }

  QSize sizeHint() const override;

signals:
  void tabChanged(int index);

protected slots:
  void setTab(int index);
  void previousTab();
  void nextTab();
  void invalidateTabIndex();

public:
  void addTab(QWidget *tab);
  void finalizeLayout();
private:
  void initializeLayout();

private:
  QStackedLayout *stackLayout_;

  int tabCount_;
  int tabIndex_;
  QList<QToolButton *> tabButtons_;
  QWidgetList tabs_;
};

#endif // ACTABVIEW_H
