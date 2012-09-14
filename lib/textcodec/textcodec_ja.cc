// textcodec_ja.cc
// 6/16/2012
//
// See: http://d.hatena.ne.jp/mohayonao/20091129/1259505966

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

#define RC_TABLE    RC_PREFIX "hirakata.txt"

// - Converters -

QString
TextCodec::hira2kata(const QString &input)
{
  static std::map<ushort, ushort> map;
  if (map.empty()) {
    QFile file(RC_TABLE);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      DOUT("error: utftable does not exist");
      return QString();
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");
    for (QString line = in.readLine(); !line.isNull(); line = in.readLine()) {
        if (line.size() != 2 || line[0] == '#')
        continue;
      QChar h = line[0],
            k = line[1];
      map[h.unicode()] = k.unicode();
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
TextCodec::kata2hira(const QString &input)
{
  static std::map<ushort, ushort> map;
  if (map.empty()) {
    QFile file(RC_TABLE);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      DOUT("error: utftable does not exist");
      return QString();
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");
    for (QString line = in.readLine(); !line.isNull(); line = in.readLine()) {
        if (line.size() != 2 || line[0] == '#')
        continue;
      QChar h = line[0],
            k = line[1];
      map[k.unicode()] = h.unicode();
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
