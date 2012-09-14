// textcodec_zh.cc
// 6/16/2012
//
// See also:
// http://www.cppblog.com/noswimfish/archive/2010/07/30/121663.html
// http://topic.csdn.net/t/20051011/14/4319278.html
// http://blog.csdn.net/i_like_cpp/article/details/320606
// http://blog.csdn.net/hydonlee/article/details/82636

#include "lib/textcodec/textcodec.h"
#include <QtCore>
#include <map>

//#define DEBUG "textcodec"
#include "lib/debug/debug.h"

#ifdef Q_OS_LINUX
# ifndef TABLEDIR
#  define TABLEDIR   "/usr/share/annot"
# endif // DOCDIR
# define RC_PREFIX   TABLEDIR "/"
#else
# define RC_PREFIX   QCoreApplication::applicationDirPath() + "/table/"
#endif // Q_OS_LINUX

#define RC_TABLE_ZHS_ZHT RC_PREFIX "zhszht.txt"
#define RC_TABLE_ZHT_ZHS RC_PREFIX "zhtzhs.txt"

// - Converters -

QString
TextCodec::zhs2zht(const QString &input)
{
  static std::map<ushort, ushort> map;
  if (map.empty()) {
    QFile file(RC_TABLE_ZHS_ZHT);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      DOUT("error: utftable does not exist");
      return QString();
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");
    for (QString line = in.readLine(); !line.isNull(); line = in.readLine()) {
        if (line.size() != 2 || line[0] == '#')
        continue;
      QChar s = line[0],
            t = line[1];
      if (s != t)
        map[s.unicode()] = t.unicode();
    }
  }
  if (input.isEmpty())
    return QString();
  QString ret;
  ret.reserve(input.size());
  foreach (const QChar &c, input) {
    auto p = map.find(c.unicode());
    ret.append(p == map.end() ? c : QChar(p->second));
  }
  return ret;
}

QString
TextCodec::zht2zhs(const QString &input)
{
  static std::map<ushort, ushort> map;
  if (map.empty()) {
    QFile file(RC_TABLE_ZHT_ZHS);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      DOUT("error: utftable does not exist");
      return QString();
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");
    for (QString line = in.readLine(); !line.isNull(); line = in.readLine()) {
      if (line.size() != 2 || line[0] == '#')
        continue;
      QChar t = line[0],
            s = line[1];
      if (s != t)
        map[t.unicode()] = s.unicode();
    }
  }
  if (input.isEmpty())
    return QString();
  QString ret;
  ret.reserve(input.size());
  foreach (const QChar &c, input) {
    auto p = map.find(c.unicode());
    ret.append(p == map.end() ? c : QChar(p->second));
  }
  return ret;
}

// EOF
