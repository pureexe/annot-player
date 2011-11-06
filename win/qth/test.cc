// main.cc
// 10/14/2011

#include "qth.h"
#include "ith/main.h"
#include <QtGui>

int
main(int argc, char *argv[])
{
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

  QApplication a(argc, argv);
  QMainWindow w;

  Qth qth(w.winId(), &w);
  Qth::setGlobalInstance(&qth);

  DWORD pid = PIDByName(L"malie.exe");
  qDebug() << pid;
  qDebug() << Qth::globalInstance()->attachProcess(pid);

  w.show();
  return a.exec();
}


// EOF
