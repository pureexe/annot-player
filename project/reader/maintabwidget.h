#ifndef TABWIDGET_H
#define TABWIDGET_H

// tabwidget.h
// 7/9/2011

#include <QTabWidget>

class MainTabWidget : public QTabWidget
{
  Q_OBJECT
  typedef MainTabWidget Self;
  typedef QTabWidget Base;

public:
  explicit MainTabWidget(QWidget *parent = 0);
protected:
  void createToolButtons();
  void createStartPage();

signals:
  void newTabRequested();
  void closeAllRequested();

public slots:
  void newTab() { emit newTabRequested(); }
  void closeTab(int index);
  void closeAll() { emit closeAllRequested(); }


};

#endif // TABWIDGET_H
