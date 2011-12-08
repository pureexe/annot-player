// main.cpp
// 11/11/2011

#include "shared.h"

int main(int argc, char **argv)
{
  QString appBundlePath;
  if (argc > 1)
    appBundlePath = QString::fromLocal8Bit(argv[1]);

  if (argc < 2 || appBundlePath.startsWith("-")) {
    qDebug() << "Usage: macdeployqt app-bundle [-no-plugins] [-dmg]";
    qDebug() << "";
    qDebug() << "macdeployqt creates self-contained application bundles that";
    qDebug() << "contains the Qt frameworks and plugins used by the application.";
    qDebug() << "";
    qDebug() << "Only frameworks in use are copied into the bundle.";
    qDebug() << "Plugins related to a framework are copied in with the";
    qDebug() << "framework. The accessibilty, image formats, and text codec";
    qDebug() << "plugins are always copied, unless \"-no-plugins\" is specified.";
    qDebug() << "";
    qDebug() << "See the \"Deploying an Application on Qt/Mac\" typic in the";
    qDebug() << "documentation for more information about deployment on Mac OS X.";

    return 0;
  }
  
  if (appBundlePath.endsWith("/"))
    appBundlePath.chop(1);
  
  DeploymentInfo deploymentInfo  = deployQtFrameworks(appBundlePath);
  
  bool plugins = true;
  bool dmg = false;
    
  for (int i = 2; i < argc; ++i) {
    QByteArray argument = QByteArray(argv[i]);
    if (argument == QByteArray("-no-plugins"))
      plugins = false;
    if (argument == QByteArray("-dmg"))
      dmg = true;
  }

  if (plugins) {
    if (deploymentInfo.qtPath.isEmpty())
      deploymentInfo.pluginPath = "/Developer/Applications/Qt/plugins"; // Assume binary package.
    else
      deploymentInfo.pluginPath = deploymentInfo.qtPath + "/plugins";

    qDebug() << "";
    qDebug() << "Deploying plugins from" << deploymentInfo.pluginPath;
    deployPlugins(appBundlePath, deploymentInfo);
    createQtConf(appBundlePath);
  }

  if (dmg) {
    qDebug() << "Creating disk image (.dmg) for" << appBundlePath;
    createDiskImage(appBundlePath);
  }
}

// EOF
