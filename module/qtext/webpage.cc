// qtext/webpage.cc
// 4/9/2012

#include "module/qtext/webpage.h"
#include <QtWebkit>

//#define HOMEPAGE_URL    "http://annot.me"
//#define HOMEPAGE_URL    "http://" ANNOT_HOST_IP
//#define BASE_URL        HOMEPAGE_URL "/api/player/"
//#define BASE_URL        ""

#define DEBUG "qtext::webpage"
#include "module/debug/debug.h"

// - RC -

#ifdef Q_OS_LINUX
#  define RC_PREFIX     DOCDIR "/"
#else
#  define RC_PREFIX     QCoreApplication::applicationDirPath() + "/doc/"
#endif // Q_OS_LINUX

#define RC_HTML_ERROR   RC_PREFIX "error.html"

namespace { // anonymous
  inline QByteArray rc_html_error_()
  {
    QFile f(RC_HTML_ERROR);
    if (f.open(QIODevice::ReadOnly))
      return f.readAll();
    else
      return QByteArray();
  }
} // anonymous namespace

// - Construction -

QtExt::
WebPage::WebPage(QWidget *parent)
  : Base(parent)
{ connect(this, SIGNAL(linkHovered(QString,QString,QString)), SLOT(setHoveredLink(QString))); }

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
  output->content = rc_html_error_();
  output->contentType = "text/html";
  output->encoding = "UTF-8";
  DOUT("exit");
  return true;
}

// EOF
