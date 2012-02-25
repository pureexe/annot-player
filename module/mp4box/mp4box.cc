// mp4box.cc
// 2/15/2012

#include "mp4box.h"
#include "gpac.h"
#include <QtCore>
#include <cstring>

#ifdef _MSC_VER
  #pragma warning(disable:4996) // C4996: unsafe function or variable used such as strncpy
#endif // _MSC_VER

#define DEBUG "mp4box"
#include "module/debug/debug.h"

// - Helper -
namespace { // anonymous

  char **allocate_argv(const QStringList &args)
  {
    char **argv = new char*[args.size() + 1];
    char **p = argv;
    foreach (QString s, args) {
      QByteArray c = s.toLocal8Bit();
      *p = new char[c.size() + 1];
      strncpy(*p, c.data(), c.size() + 1);
      p++;
    }
    p[0] = 0;
    return argv;
  }

  void release_argv(char **argv)
  {
    Q_ASSERT(argv);
    char **p = argv;
    while (*p)
      delete[] *p++;
    delete[] argv;
  }
} // anonymous namespace

bool
Mp4Box::muxMp4File(const QString &mp4, const QStringList &tracks)
{
  DOUT("enter: mp4 =" << mp4 << ", tracks =" << tracks);

  QFile f(mp4);
  if (f.exists()) {
    bool ok = f.remove();
    if (!ok) {
      DOUT("exit: failed to remove existed old file");
      return false;
    }
  }

  QStringList args;
  args.append("annot-player");

  foreach (QString t, tracks) {
    args.append("-add");
    args.append(t);
  }
  args.append(mp4);

  DOUT("args =" << args);

  int argc = args.size();
  char **argv = allocate_argv(args);

  int err = mp4boxMain(argc, argv);
  DOUT("err =" << err);

  release_argv(argv);

  bool ret = !err && QFileInfo(mp4).exists();
  DOUT("exit: ret =" << ret);
  return ret;
}

// EOF
