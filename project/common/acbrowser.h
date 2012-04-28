#ifndef ACBROWSER_H
#define ACBROWSER_H

// acbrowser.h
// 4/9/2012

#include <QtCore/QStringList>

class AcBrowser : public QObject
{
  Q_OBJECT
  typedef AcBrowser Self;
  typedef QObject Base;
public:
  explicit AcBrowser(QObject *parent = 0)
    : Base(parent) { }

public:
  bool isRunning() const;

public slots:
  void open();
  void openArguments(const QStringList &args);
  void openUrls(const QStringList &urls) { openArguments(urls); }
  void openUrl(const QString &url) { openUrls(QStringList(url)); }
};

#endif // ACBROWSER_H
