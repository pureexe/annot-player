// annothtml_thread.cc
// 3/20/2012
#include "module/annotcloud/annothtml.h"
#include "module/annotcloud/annotpaint.h"
#include "module/qtext/datetime.h"
#include "module/qtext/os.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <boost/tuple/tuple.hpp>

#define FORMAT_TIME(_secs)        QDateTime::fromMSecsSinceEpoch(_secs * 1000).toString("M/d/yyyy ddd h:mm:ss")
#define FORMAT_POS(_msecs)        ((_msecs) <= 0 ? QString() : QtExt::msecs2time(_msecs).toString())

// - Resources -

#ifdef Q_OS_LINUX
# define RC_AVATAR_PREFIX      AVATARDIR
#else
# define RC_AVATAR_PREFIX      QCoreApplication::applicationDirPath() + "/avatars"
#endif // Q_OS_LINUX

#define AVATAR_WIDTH            "40"
#define AVATAR_HEIGHT           "40"

#ifdef AVATAR_USER_COUNT
enum { RC_AVATAR_COUNT = AVATAR_USER_COUNT };
#else
enum { RC_AVATAR_COUNT = 10 };
#endif // AVATAR_USER_COUNT

//#ifdef AVATAR_GIF_COUNT
//enum { RC_AVATAR_GIF_COUNT = AVATAR_GIF_COUNT };
//#else
//enum { RC_AVATAR_GIF_COUNT = 10 };
//#endif // AVATAR_GIF_COUNT

namespace { namespace detail {
  inline QString rc_avatar_url(qint64 i)
  {
    static QString fmt =
#ifdef Q_OS_WIN
      QString("file:///" + RC_AVATAR_PREFIX "/user_%1.jpg").replace('\\', '/')
#else
      "file://" + QString(RC_AVATAR_PREFIX) + "/user_%1.jpg"
#endif  // Q_OS_WIN
    ;
    return fmt.arg(QString::number(qAbs(i) % RC_AVATAR_COUNT));

//#ifdef Q_OS_WIN
//    static QString jpg = QString("file:///" + RC_AVATAR_PREFIX "/user_%1.jpg").replace('\\', '/'),
//                   gif = QString("file:///" + RC_AVATAR_PREFIX "/user_%1.gif").replace('\\', '/');
//#else
//    static QString jpg = "file://" + RC_AVATAR_PREFIX + "/user_%1.jpg",
//                   gif = "file://" + RC_AVATAR_PREFIX + "/user_%1.gif";
//#endif // Q_OS_WIN
//    int hash = qAbs(i) % (RC_AVATAR_GIF_COUNT + RC_AVATAR_JPG_COUNT);
//    return hash < RC_AVATAR_JPG_COUNT ?
//      jpg.arg(QString::number(hash)) :
//      gif.arg(QString::number(hash - RC_AVATAR_JPG_COUNT));
  }
} } // anonymous detail


#ifdef Q_OS_LINUX
# define RC_PREFIX     JSFDIR "/"
#else
# define RC_PREFIX     QCoreApplication::applicationDirPath() + "/jsf/"
#endif // Q_OS_LINUX

#define RC_JSF_T        RC_PREFIX "t.xhtml"
#define RC_JSF_A        RC_PREFIX "a.xhtml"
#define RC_JSF_I        RC_PREFIX "i.xhtml"

#define EL_TITLE        "#{title}"
#define EL_T            "#{t}"
#define EL_H            "#{h}"

#define EL_A_USER       "#{a_user}"
#define EL_A_POS        "#{a_pos}"
#define EL_A_CREATETIME "#{a_ts}"
#define EL_A_TEXT       "#{a_content}"

#define EL_I_SRC        "#{i_src}"
#define EL_I_TITLE      "#{i_title}"
#define EL_I_WIDTH      "#{i_width}"
#define EL_I_HEIGHT     "#{i_height}"

#define EL_AVATAR_SRC    "#{avatar_src}"
#define EL_AVATAR_TITLE  "#{avatar_title}"
#define EL_AVATAR_WIDTH  "#{avatar_width}"
#define EL_AVATAR_HEIGHT "#{avatar_height}"

namespace { namespace detail {

  inline const QByteArray &rc_jsf_t()
  {
    static QByteArray ret;
    if (ret.isEmpty()) {
      QFile f(RC_JSF_T);
      if (f.open(QIODevice::ReadOnly))
        ret = f.readAll();
    }
    return ret;
  }

  inline const QByteArray &rc_jsf_a()
  {
    static QByteArray ret;
    if (ret.isEmpty()) {
      QFile f(RC_JSF_A);
      if (f.open(QIODevice::ReadOnly))
        ret = f.readAll();
    }
    return ret;
  }

  inline const QByteArray &rc_jsf_i()
  {
    static QByteArray ret;
    if (ret.isEmpty()) {
      QFile f(RC_JSF_I);
      if (f.open(QIODevice::ReadOnly))
        ret = f.readAll();
    }
    return ret;
  }

} } // anonymous detail

// - API -

QString
AnnotCloud::
AnnotationHtmlParser::toHtml(const AnnotationList &l, const QString &title, bool ignorePos) const
{
  QString ret = detail::rc_jsf_t();
  ret.replace(EL_TITLE, title);

  // Threads
  QString t;
  foreach (const Annotation &a, l) {
    QString c = detail::rc_jsf_a();
    c.replace(EL_AVATAR_WIDTH, AVATAR_WIDTH)
     .replace(EL_AVATAR_HEIGHT, AVATAR_HEIGHT)
     .replace(EL_AVATAR_TITLE, a.userAlias())
     .replace(EL_AVATAR_SRC, detail::rc_avatar_url(a.userId()));
    c.replace(EL_A_POS, FORMAT_POS(a.pos()))
     .replace(EL_A_CREATETIME, FORMAT_TIME(a.createTime()))
     .replace(EL_A_USER, a.userAlias());

    QString text; {
      QStringList tags;
      boost::tie(text, tags) = toHtml(a.text());
      if (!tags.isEmpty())
        text.prepend(tags.join("") + " ");
    }
    c.replace(EL_A_TEXT, text);

    t.append(c);
  }
  ret.replace(EL_T, t);

  // Images

  enum { img_width = 500, img_height = 200 };

  QString h;
  Annotation::Field fields[] = { Annotation::Pos, Annotation::CreateTime, Annotation::UserId };
  QString titles[] = { tr("Time - Count"), tr("Date - Count"), tr("User - Count") };
  enum { ImageCount = sizeof(titles) / sizeof(*titles) };
  for (int i = 0; i < ImageCount; i++) {
    if (ignorePos && fields[i] == Annotation::Pos)
      continue;
    QString img = detail::rc_jsf_i();
    QString img_title = titles[i];
    QString img_file = QtExt::mktemp(".svg");
    QString img_src;
#ifdef Q_OS_WIN
    img_src = "file:///" + img_file;
    img_src.replace('\\', '/');
#else
    img_src = "file://" + img_file;
#endif // Q_OS_WIN
    img.replace(EL_I_WIDTH, QString::number(img_width))
       .replace(EL_I_HEIGHT, QString::number(img_height))
       .replace(EL_I_TITLE, img_title)
       .replace(EL_I_SRC, img_src);

    AnnotCloud::AnnotationPainter::globalInstance()->
        saveHistogramAsFile(img_file, l, fields[i], img_width, img_height, img_title);
    h.append(img);
  }

  ret.replace(EL_H, h);
  return ret;
}

// EOF
