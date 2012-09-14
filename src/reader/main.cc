// main.cc
// 6/18/2011

#include "mainwindow.h"
#include "stylesheet.h"
#include <QApplication>
#include <QMenuBar>
//#include <QCleanLooksStyle>
#ifdef Q_OS_WIN
  #include "win/dwm/dwm.h"
#endif

#define SS_MAINWINDOW \
  SS_LINEEDIT \
  /*SS_TABWIDGET*/ \
  SS_TABBAR \
  /*SS_TOOLBUTTON*/

namespace { // anonymous
  inline void
  setWindowStyle(QMainWindow *w)
  {
    Q_ASSERT(w);
#if defined(Q_OS_WIN) && defined(USE_DWM)
    if (Dwm::isCompositionEnabled()) {
      Dwm::enableBlurBehindWindow(w, true);
      Dwm::extendFrameIntoClientArea(w);
      w->setAttribute(Qt::WA_TranslucentBackground);
      w->setAttribute(Qt::WA_NoSystemBackground);

      w->setContentsMargins(0, 0, 0, 0);
    }
#elif defined(Q_OS_LINUX)
    w->setAttribute(Qt::WA_TranslucentBackground);
    w->setAttribute(Qt::WA_NoSystemBackground, false);
    QPalette pal = w->palette();
    QColor bg = pal.window().color();
    bg.setAlpha(180);
    pal.setColor(QPalette::Window, bg);
    w->setPalette(pal);
    w->ensurePolished(); // workaround Oxygen filling the background
    w->setAttribute(Qt::WA_StyledBackground, false);
#endif

    w->setStyleSheet(SS_MAINWINDOW);

    if (w->menuBar())
      w->menuBar()->hide();
  }
} // namespace anonymous

int
main(int argc, char *argv[])
{
  //QApplication::setStyle(new QCleanlooksStyle);

  QApplication a(argc, argv);
  MainWindow w;
  setWindowStyle(&w);

  w.openFile("d:\\i\\touch\\sample.pdf");
  //w.openFile("file:///d:/i/touch/sample.pdf");
  //w.openFile("http://www.openmp.org/mp-documents/OpenMP3.1.pdf");

  w.show();
  return a.exec();
}

// EOF
