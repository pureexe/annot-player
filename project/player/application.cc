// application.cc
// 11/18/2011
#include "application.h"
#include "mainwindow.h"
#include "mainwindowprivate.h" // FIXME!
#include "global.h"
#include <QtGui>

// - Constructions -

Application::Application(int &argc, char **argv)
  : Base(argc, argv), w_(0)
{
  setOrganizationDomain(G_DOMAIN);
  setOrganizationName(G_ORGANIZATION);
  setApplicationName(G_APPLICATION);
  setApplicationVersion(G_VERSION);
}

MainWindow*
Application::mainWindow() const
{ return w_; }

void
Application::setMainWindow(MainWindow *w)
{ w_ = w; }

// - Events -

bool
Application::event(QEvent *e)
{
  bool accept = true;
  switch (e->type()) {

    // See: http://www.qtcentre.org/wiki/index.php?title=Opening_documents_in_the_Mac_OS_X_Finder
  case QEvent::FileOpen: {
      //QFileOpenEvent *fe = qobject_cast<QFileOpenEvent*>(e); // Not working on Mac?
      QFileOpenEvent *fe = dynamic_cast<QFileOpenEvent*>(e);
      if (fe && w_) {
        //w_->openSource(fe->file());
        QString url = fe->file();
        MainWindow_slot_::OpenSource *slot = new MainWindow_slot_::OpenSource(url, w_);
        QTimer::singleShot(0, slot, SLOT(openSource()));
      }
    } break;

  default:
    accept = Base::event(e);
  }

  return accept;
}

// EOF
