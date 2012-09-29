// application.cc
// 9/22/2012 jichi

#include "application.h"

#define DEBUG "application"
#include "qx/qxdebug.h"

// - Construction -

Application::Application(int &argc, char **argv)
  : Base(argc, argv, GuiClient)
{
}

// EOF
