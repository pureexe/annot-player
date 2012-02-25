#ifndef _VLCCORE_HTTP_H
#define _VLCCORE_HTTP_H

// vlccore/http.h
// 2/21/2012

#ifdef _MSC_VER
  #pragma warning (disable:4819)       // C4819: The file contains a character that cannot be represented in the current code page.
  #pragma warning (disable:4996)       // C4996: MS' deprecated std functions orz.
#endif // _MSC_VER

#include <QNetworkCookie>
#include <QList>

namespace vlccore {

  void load_http_plugin();

  void set_http_cookies(const QList<QNetworkCookie> &cookies);
  void clear_http_cookies();

} // namespace vlccore

#endif // _VLCCORE_VIDEO_H
