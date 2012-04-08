// annothtmlthread.cc
// 3/20/2012
#include "module/annotcloud/annothtml.h"
#include "module/annotcloud/annotpaint.h"
#include "module/qtext/datetime.h"
#include "module/qtext/os.h"
#include <QFile>
#include <QDesktopServices>
#include <boost/tuple/tuple.hpp>

#define FORMAT_TIME(_secs)        QDateTime::fromMSecsSinceEpoch(_secs * 1000).toString(Qt::ISODate) \
                                  .replace('T', ' ')
#define FORMAT_POS(_msecs)        ((_msecs) <= 0 ? QString() : QtExt::msecs2time(_msecs).toString())

// - Resources -

#ifdef Q_OS_LINUX
#  define RC_PREFIX     JSFDIR "/"
#else
#  define RC_PREFIX     "jsf/"
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

namespace { // anonymous

  inline QString rc_jsf_t_()
  {
    static QString ret;
    if (ret.isEmpty()) {
      QFile f(RC_JSF_T);
      if (f.open(QIODevice::ReadOnly))
        ret = f.readAll();
    }
    return ret;
  }

  inline QString rc_jsf_a_()
  {
    static QString ret;
    if (ret.isEmpty()) {
      QFile f(RC_JSF_A);
      if (f.open(QIODevice::ReadOnly))
        ret = f.readAll();
    }
    return ret;
  }

  inline QString rc_jsf_i_()
  {
    static QString ret;
    if (ret.isEmpty()) {
      QFile f(RC_JSF_I);
      if (f.open(QIODevice::ReadOnly))
        ret = f.readAll();
    }
    return ret;
  }

} // anonymous namespace

// - API -

QString
AnnotCloud::
AnnotationHtmlParser::toHtml(const AnnotationList &l, const QString &title) const
{
  QString ret = rc_jsf_t_();
  ret.replace(EL_TITLE, title);

  // Threads
  QString t;
  foreach (const Annotation &a, l) {
    QString c = rc_jsf_a_();
    c.replace(EL_A_POS, FORMAT_POS(a.pos()));
    c.replace(EL_A_CREATETIME, FORMAT_TIME(a.createTime()));
    c.replace(EL_A_USER, a.userAlias());

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
    QString img = rc_jsf_i_();
    QString img_title = titles[i];
    QString img_file = QtExt::mktemp(".svg");
    QString img_src;
#ifdef Q_WS_WIN
    img_src = "file:///" + img_file;
    img_src.replace('\\', '/');
#else
    img_src = "file://" + img_file;
#endif // Q_WS_WIN
    img.replace(EL_I_SRC, img_src);
    img.replace(EL_I_TITLE, img_title);
    img.replace(EL_I_WIDTH, QString::number(img_width));
    img.replace(EL_I_HEIGHT, QString::number(img_height));

    AnnotCloud::AnnotationPainter::globalInstance()->
        saveHistogramAsFile(img_file, l, fields[i], img_width, img_height, img_title);
    h.append(img);
  }

  ret.replace(EL_H, h);
  return ret;
}

// EOF
