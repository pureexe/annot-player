#ifndef ACBROWSER_H
#define ACBROWSER_H

// acbrowser.h
// 4/9/2012

#include <QtCore/QStringList>

#define ACSCHEME_BROWSER_SEARCH "http://_/browser/search/"

class AcBrowser : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AcBrowser)
  typedef AcBrowser Self;
  typedef QObject Base;
public:
  explicit AcBrowser(QObject *parent = nullptr)
    : Base(parent) { }

public:
  bool isRunning() const;

public slots:
  void open();
  void openArguments(const QStringList &args);
  void openUrls(const QStringList &urls) { openArguments(urls); }
  void openUrl(const QString &url) { openUrls(QStringList(url)); }

  void search(const QString &engine, const QString &key);
};

#endif // ACBROWSER_H
