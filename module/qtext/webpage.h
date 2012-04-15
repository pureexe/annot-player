#ifndef _QTEXT_WEBPAGE_H
#define _QTEXT_WEBPAGE_H

// qtext/webpage.h
// 4/9/2012

#include <QWebView>
#include <QNetworkRequest>

namespace QtExt {

class WebPage: public QWebPage
{
  Q_OBJECT
  typedef WebPage Self;
  typedef QWebPage Base;

  QString hoveredLink_;
  bool hoveredLinkLocked_;

public:
  explicit WebPage(QWidget *parent = 0);

  QString hoveredLink() const { return hoveredLink_; }

private slots:
  void setHoveredLink(const QString &url)
  { hoveredLink_ = url; }

  // - Extensions -
public:
  virtual bool supportsExtension(Extension extension) const; ///< \override
  virtual bool extension(Extension extension, const ExtensionOption *option = 0, ExtensionReturn *output = 0); ///< \override

protected:
  bool errorPageExtension(const ErrorPageExtensionOption *option = 0, ErrorPageExtensionReturn *output = 0);
};

} // namespace QtExt

#endif // _QTEXT_WEBPAGE_H
