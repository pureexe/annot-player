// main.cc
// 5/6/2012
#include "window.h"
#include "lib/download/download.h"
#include <QtGui>
#include <cstdlib>
#include <ctime>

#define DEBUG "main"
#include "lib/debug/debug.h"

// - Main -

int
main(int argc, char *argv[])
{
  DOUT("enter");
  // Set OS encoding to UTF-8 before application starts.
  QTextCodec *codec = QTextCodec::codecForName("UTF-8");
  QTextCodec::setCodecForCStrings(codec);
  QTextCodec::setCodecForTr(codec);

  // Applications
  QApplication a(argc, argv);

  // Seed global random generator.
  time_t now = ::time(0);
  ::srand(now);
  ::qsrand(now);

  Window w;
  Window w2;
  w2.winId_ = w.winId();
  w2.setWindowTitle("TODO");

  //GraphicsRippleEffect *e = new GraphicsRippleEffect;
  //e->setOpacity(0.5);
  //w.setGraphicsEffect(e);
  w.show();
  w2.show();
  ::dlget("C:/Users/jichi/tmp/nico", QUrl("https://secure.nicovideo.jp/secure/login_form"), QString(), false);
  //  w.scene()->addRect(0, 0, 100, 100, QPen(Qt::red));
  //  w.scene()->addText("123");
  DOUT("exit: exec");
  return a.exec();
}

// EOF
