// processfilter.cc
// 8/22/2012
#include "processfilter.h"
#include "gamelibrary.h"
#include "win/qtwin/qtwin.h"
//#ifdef WITH_WIN_TEXTHOOK
//# include "win/texthook/texthook.h"
//#endif // WITH_WIN_TEXTHOOK
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <boost/assign/list_of.hpp>
#include <set>

//#define DEBUG "processfilter"
#include "module/debug/debug.h"
#include <QtCore/QDebug>

// - Query -

bool
ProcessFilter::processNameIsNotGalGame(const QString &procName)
{
#define H(_cstr) qHash(QString(#_cstr))  // use std::set with qHash to improve search performance
  static const std::set<uint> blacklist = boost::assign::list_of
(H(annot-browser))(H(annot-down))(H(annot-player))
(H(Activator))(H(ApMsgFwd))(H(Apntex))(H(Apoint))(H(AppleMobileDeviceService))(H(APSDaemon))(H(armsvc))(H(AutoHotkey))(H(ApplePhotoStreams))
(H(BitComet))(H(BookmarkDAV_client))(H(BoonSutazio))(H(Bootcamp))(H(BtStackServer))(H(BTTray))(H(btwdins))
(H(CamtasiaStudio))(H(chrome))
(H(distnoted))(H(Dropbox))(H(DTLite))
(H(eclipse))(H(Evernote))(H(EvernoteClipper))(H(EvernoteTray))
(H(firefox))(H(foobar2000))
(H(GoogleIMEJaConverter))(H(GoogleIMEJaRenderer))(H(gvim))
(H(Hamana))(H(HidFind))
(H(iCloudServices))(H(IELowutil))(H(IEXPLOR))(H(iTunes))(H(iTunesHelper))
(H(java))(H(javaw))(H(jusched))
(H(KHALMNPR))(H(KMPlayer))
(H(MacDrive))(H(MacDrive8Service))(H(Maxthon))(H(mDNSResponder))(H(MouseGesture))(H(mpclaunch))(H(mspdbsrv))(H(mysql))(H(mysqld))
(H(netdrive))
(H(oacrmonitor))(H(ONENOTEM))(H(opera))
(H(php-cgi))(H(plugin-container))
(H(QQ))(H(qtcreator))
(H(SecureCRT))(H(SetPoint))(H(sidebar))(H(Skype))(H(SogouCloud))(H(SROSVC))(H(sttray))(H(Switcher))(H(SUService))
(H(thunderbird))(H(TSCHelper))(H(TXPlatform))
(H(VIPAppService))(H(volumouse))
(H(WINWORD))(H(wmpnetwk))
  ;
#undef H
  return blacklist.find(qHash(procName)) != blacklist.end();
}

qint64
ProcessFilter::currentGamePid() const
{
  DOUT("enter");
  if (library_->isEmpty()) {
    DOUT("exit: empty game library");
    return 0;
  }
  static const QString windir = QtWin::getWinDirPath();
  static const QString appdata = QtWin::getLocalAppDataPath();

  // Revert search order, as the game usually comes at last
  //foreach (const QtWin::ProcessInfo &pi, QtWin::getProcessesInfo())
  QList<QtWin::ProcessInfo> l = QtWin::getProcessesInfo();
  for (auto p = l.constEnd(); p != l.constBegin();) {
    const QtWin::ProcessInfo &pi = *--p;
    ulong pid = pi.id;

    if (pid == QCoreApplication::applicationPid())
      continue;

//#ifdef WITH_WIN_TEXTHOOK
//    if (TextHook::globalInstance()->containsProcess(pid))
//      return pid;
//#endif // WITH_WIN_TEXTHOOK

    if (pi.name.isEmpty())
      continue;

    QString location = QtWin::getProcessPathById(pid);
    if (location.isEmpty() ||
        !windir.isEmpty() && location.startsWith(windir, Qt::CaseInsensitive) ||
        !appdata.isEmpty() && location.startsWith(appdata, Qt::CaseInsensitive))
      continue;

    DOUT("location =" << location);
    int i = location.lastIndexOf('\\');
    QString fileName = i < 0 ? location : location.mid(i +1);
    DOUT("executable =" << fileName);

    if (library_->containsExecutable(fileName)) {
      qDebug() << "processfilter:currentGamePid: location =" << location;
      DOUT("exit: pid =" << pid);
      return pid;
    }
  }
  DOUT("exit: pid not found");
  return 0;
}

// EOF
