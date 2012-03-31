// annothtmlthread.cc
// 3/20/2012
#include "module/annotcloud/annothtml.h"
#include "module/qtext/datetime.h"
#include <QtCore>
#include <boost/tuple/tuple.hpp>

#define FORMAT_TIME(_secs)        QDateTime::fromMSecsSinceEpoch(_secs * 1000).toString(Qt::ISODate) \
                                  .replace('T', ' ')
#define FORMAT_POS(_msecs)        ((_msecs) <= 0 ? QString() : QtExt::msecs2time(_msecs).toString())

// - Resources -

#define RC_PREFIX       "jsf/"

#define RC_JSF_T        RC_PREFIX "t.xhtml"
#define RC_JSF_A        RC_PREFIX "a.xhtml"

#define EL_TITLE        "#{title}"
#define EL_T            "#{t}"
#define EL_A_USER       "#{user}"
#define EL_A_POS        "#{pos}"
#define EL_A_CREATETIME "#{ts}"
#define EL_A_TEXT       "#{content}"

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

} // anonymous namespace

// - API -

QString
AnnotCloud::
AnnotationHtmlParser::toHtml(const AnnotationList &l, const QString &title) const
{
  QString ret = rc_jsf_t_();
  ret.replace(EL_TITLE, title);

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
  return ret;
}

// EOF
