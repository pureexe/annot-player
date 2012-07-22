// mp4box.cc
// 2/15/2012

#include "mp4box.h"
#include "module/mediacodec/mp4codec.h"
#ifdef WITH_GPAC
# include "gpac/gpac.h"
#endif // WITH_GPAC
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <cstdio>
#include <cstring>

#define MP4BOX_BIN      QCoreApplication::applicationDirPath() + "/" "MP4Box"

#ifdef _MSC_VER
  #pragma warning(disable:4996) // C4996: unsafe function or variable used such as strncpy
#endif // _MSC_VER

#define DEBUG "mp4box"
#include "module/debug/debug.h"

// - Helper -
namespace { namespace detail {

  inline
  char **allocate_argv(const QStringList &args)
  {
    char **argv = new char*[args.size() + 1];
    char **p = argv;
    foreach (const QString &s, args) {
      QByteArray c = s.toLocal8Bit();
      *p = new char[c.size() + 1];
      strncpy(*p, c.data(), c.size() + 1);
      p++;
    }
    p[0] = 0;
    return argv;
  }

  inline
  void release_argv(char **argv)
  {
    Q_ASSERT(argv);
    char **p = argv;
    while (*p)
      delete[] *p++;
    delete[] argv;
  }

  inline QString fix_win_path(const QString &path)
  { return QString(path).replace("/", "\\"); }

#ifdef Q_OS_WIN
  class FileMangler
  {
    struct Pair {
      QString originalPath, mangledPath;
      Pair(const QString &orig, const QString &mangled)
        : originalPath(orig), mangledPath(mangled) { }
    };
    QList<Pair> store_;
  public:
    ~FileMangler() { restore(); }

    QString mangle(const QString &path)
    {
      QFileInfo fi(path);
      QString ret = fi.absolutePath() + "/" + ::tmpnam(nullptr) + "." + fi.suffix();
      //QString ret = ::tmpnam(nullptr) + ("." + fi.suffix());
      if (!QFile::rename(path, ret))
        return QString();
      store_.append(Pair(path, ret));
      return ret;
    }

    QString mangleLater(const QString &path)
    {
      QFileInfo fi(path);
      QString ret = fi.absolutePath() + "/" + ::tmpnam(nullptr) + "." + fi.suffix();
      //QString ret = ::tmpnam(nullptr) + ("." + fi.suffix());
      store_.append(Pair(path, ret));
      return ret;
    }

    void restore()
    {
      foreach (const Pair &p, store_)
        QFile::rename(p.mangledPath, p.originalPath);
    }
  };
#endif // Q_OS_WIN

} } // anonymous detail

bool
Mp4Box::muxMp4File(const QString &mp4, const QStringList &tracks,
                   Mp4Info *info)
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

  QStringList args(detail::fix_win_path(MP4BOX_BIN));

#ifdef Q_OS_WIN
  detail::FileMangler m;
  #define _qs(_path)      detail::fix_win_path(m.mangle(_path))
  #define _qt(_path)      detail::fix_win_path(m.mangleLater(_path))
#else
  #define _qs(_path)      (_path)
  #define _qt(_path)      (_path)
#endif // Q_OS_WIN

  foreach (const QString &t, tracks) {
    QString path = _qs(t);
    if (path.isEmpty()) {
      DOUT("exit: invalid track path");
      return false;
    }
    args.append("-add");
    args.append(path);
  }
  QString outputFile = _qt(mp4);
  if (outputFile.isEmpty()) {
    DOUT("exit: invalid mp4 path");
    return false;
  }
  args.append(outputFile);

  DOUT("args =" << args);

  int err;
#ifdef WITH_GPAC
  {
    int argc = args.size();
    char **argv = detail::allocate_argv(args);
    err = mp4boxMain(argc, argv);
    detail::release_argv(argv);
  }
#else
  {
    QString program = args.first();
    args.removeFirst();
    QProcess proc;
    proc.start(program, args, QIODevice::ReadOnly | QIODevice::Text);
    proc.waitForFinished(-1);
    err = proc.exitCode();
    if (err)
      DOUT(proc.readAll());
  }
#endif // WITH_GPAC
  DOUT("err =" << err);

  bool ret = !err && QFile::exists(outputFile);
  if (ret && info) {
    QPair<int, int> dim = Mp4Codec::fileDimension(outputFile);
    info->width = dim.first;
    info->height = dim.second;
  }
  DOUT("exit: ret =" << ret);
  return ret;
}

// EOF
