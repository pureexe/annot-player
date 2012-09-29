// main.cc
// 9/22/2012 jichi

#include "application.h"
#include "global.h"
#include "mainobject.h"
#include <QtCore/QDir>
#include <qt_windows.h>

#define DEBUG "main"
#include "qx/qxdebug.h"

int
main(int argc, char *argv[])
{
  DOUT("enter");

  Application a(argc,argv);
  QDir::setCurrent(QCoreApplication::applicationDirPath());
  a.setQuitOnLastWindowClosed(false);

//#ifdef WITH_WIN_DWM
//  if (Dwm::isCompositionEnabled()) {
//    Dwm::ColorizationParameters cp;
//    qMemSet(&cp, 0, sizeof(cp));
//    if (Dwm::getColorizationParameters(&cp)) {
//      qDebug()<<11111<<QColor(cp.color).name();
//      qDebug()<<22222<<QColor(cp.afterglow).name();
//      qMemSet(&cp, 0, sizeof(cp));
//      //cp.color = cp.afterglow = 0x001129;
//      Dwm::setColorizationParameters(cp);
//    }
//  }
//#endif WITH_WIN_DWM

  //CopyCursor(::LoadCursor(NULL, DC_ARROW ));
  // See: http://msdn.microsoft.com/en-us/library/windows/desktop/ms648071%28v=vs.85%29.aspx
  //HCURSOR hcur = LoadCursorFromFileW(L"d:\\blue_B\\01.cur");
  //SetSystemCursor(hcur,32512); OCR_

  DOUT("creating main object");
  MainObject object;
  object.launch(a.arguments());


  DOUT("app exec");
  int ret = a.exec();
  DOUT("exit: ret =" << ret);
  return ret;
}

// EOF
