// qtext/webpage.cc
// 4/9/2012

#include "module/qtext/webpage.h"
#include <QtWebKit/QWebFrame>
#include <QtGui>

//#define HOMEPAGE_URL    "http://annot.me"
//#define HOMEPAGE_URL    "http://" ANNOT_HOST_IP
//#define BASE_URL        HOMEPAGE_URL "/api/player/"
//#define BASE_URL        ""

#define DEBUG "qtext::webpage"
#include "module/debug/debug.h"

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses around assignment
#endif // __GNUC__

// - RC -

#ifdef Q_WS_X11
# ifndef JSFDIR
#  define JSFDIR      "/usr/share/annot"
# endif // JSFDIR
# define RC_PREFIX     JSFDIR "/"
#else
# define RC_PREFIX     QCoreApplication::applicationDirPath() + "/jsf/"
#endif // Q_WS_X11

//#define RC_HTML_ERROR   RC_PREFIX "error.html"
#define RC_JSF_ERROR    RC_PREFIX "error.xhtml"

namespace { // anonymous
  //inline QByteArray rc_html_error_()
  //{
  //  QFile f(RC_HTML_ERROR);
  //  return f.open(QIODevice::ReadOnly) ? f.readAll() : QByteArray();
  //}

  inline QByteArray rc_jsf_error_()
  {
    static QByteArray ret;
    if (ret.isEmpty()) {
      QFile f(RC_JSF_ERROR);
      if (f.open(QIODevice::ReadOnly))
        ret = f.readAll();
    }
    return ret;
  }

  inline QByteArray rc_jsf_error_(int error, const QString &reason, const QUrl &url)
  {
    return QString(rc_jsf_error_())
      .replace("#{error}", QString::number(error))
      .replace("#{reason}", reason)
      .replace("#{url}", url.toString())
      .toUtf8();
  }
} // anonymous namespace

// - Construction -

QtExt::
WebPage::WebPage(QWidget *parent)
  : Base(parent)
{ connect(this, SIGNAL(linkHovered(QString,QString,QString)), SLOT(setHoveredLink(QString))); }

// - Events -

bool
QtExt::
WebPage::event(QEvent *event)
{
  Q_ASSERT(event);
  if (event->type() == QEvent::MouseButtonRelease) {
    auto e = static_cast<QMouseEvent *>(event);
    if (e->button() == Qt::LeftButton && e->modifiers() == Qt::ControlModifier)  {
      if (!hoveredLink_.isEmpty())
        emit openLinkRequested(hoveredLink_);
      e->accept();
      return true;
    }
  }
  return Base::event(event);
}


// - Extensions -

bool
QtExt::
WebPage::supportsExtension(Extension extension) const
{
  switch (extension) {
  case ErrorPageExtension: return true;
  default: return Base::supportsExtension(extension);
  }
}

bool
QtExt::
WebPage::extension(Extension extension, const ExtensionOption *option, ExtensionReturn *output)
{
  switch (extension) {
  case ErrorPageExtension:
    return errorPageExtension(static_cast<const ErrorPageExtensionOption *>(option),
                              static_cast<ErrorPageExtensionReturn *>(output))
        || Base::extension(extension, option, output);
  default:
    return Base::extension(extension, option, output);
  }
}

bool
QtExt::
WebPage::errorPageExtension(const ErrorPageExtensionOption *option, ErrorPageExtensionReturn *output)
{
  if (!option || !output)
    return false;
  DOUT("enter: error =" << option->error << ", message =" << option->errorString);
  output->baseUrl = option->url;
  output->content = ::rc_jsf_error_(option->error, option->errorString, option->url);
  //output->contentType = "text/html"; // automaticly detected
  output->encoding = "UTF-8";
  DOUT("exit");
  return true;
}

// - Scroll -

void
QtExt::
WebPage::scrollTop()
{ mainFrame()->setScrollBarValue(Qt::Vertical, 0); }

void
QtExt::
WebPage::scrollBottom()
{ mainFrame()->setScrollBarValue(Qt::Vertical, mainFrame()->scrollBarMaximum(Qt::Vertical)); }

void
QtExt::
WebPage::scrollLeft()
{ mainFrame()->setScrollBarValue(Qt::Horizontal, 0); }

void
QtExt::
WebPage::scrollRight()
{ mainFrame()->setScrollBarValue(Qt::Horizontal, mainFrame()->scrollBarMaximum(Qt::Horizontal)); }

// - User Agent -

// See: WebKit/qt/Api/qwebpage.cpp
// Example: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/534.34 (KHTML, like Gecko) MYAPP/MYVERSION Safari/534.34
QString
QtExt::
WebPage::userAgentForUrl(const QUrl &url) const
{
  static QString ret;
  if (ret.isEmpty())
    ret = Base::userAgentForUrl(url)
        .replace(QRegExp(" \\S+ Safari/"), " Safari/");
  return ret;
}

// EOF

/*
QObject*
QtExt::
WebPage::createPlugin(const QString &classid, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues)
{
  DOUT("classId =" << classid << ", url =" << url.toString() << ", paramNames =" << paramNames << ", paramValues =" << paramValues);
  return Base::createPlugin(classid, url, paramNames, paramValues);
}
*/
