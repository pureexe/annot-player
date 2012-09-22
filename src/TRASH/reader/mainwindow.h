#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// mainwindow.h
// 6/18/2011

#include <QMainWindow>
#include <QVector>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

class DocumentWidget;
class MainTabWidget;

/*
namespace Ui {
  class MainWindow;
}
*/

// MainWindow
/**
 *  \brief  Main window that holds the document widgets
 */
class MainWindow : public QMainWindow
{
  Q_OBJECT

  typedef MainWindow Self;
  typedef QMainWindow Base;

  //Ui::MainWindow *ui;
  QVector<DocumentWidget*> documents_;
  MainTabWidget *tabWidget_;
  QPoint dragPos_;

  // Constructions:
private:
  MainWindow(const Self&);
public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

public slots:
  void openFile();
  bool openFile(const QString &filePath);
  void about();

  void newTab();
  void closeAll();

  // Override event handlers:
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);

protected:
  void createDocuments();
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void createTabs();
  void createDockWindows();

  //QToolBar *fileToolBar;
  //QToolBar *editToolBar;
  QMenu *fileMenu;
  //QMenu *editMenu;
  QMenu *viewMenu;
  QMenu *windowMenu;
  QMenu *helpMenu;

  QAction *openAct;
  QAction *quitAct;
  QAction *aboutAct;
};

#endif // MAINWINDOW_H
