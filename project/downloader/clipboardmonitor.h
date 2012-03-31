#ifndef CLIPBOARDMONITOR_H
#define CLIPBOARDMONITOR_H

// clipboardmonitor.h
// 3/10/2012

#include <QObject>

class ClipboardMonitor : public QObject
{
  Q_OBJECT
  typedef ClipboardMonitor Self;
  typedef QObject Base;

  bool enabled_;

  // - Constructions -
public:
  explicit ClipboardMonitor(QObject *parent = 0);

  bool isEnabled() const { return enabled_; }

signals:
  void message(const QString &text);
  void warning(const QString &text);
  void error(const QString &text);

  void mediaUrlEntered(const QString &url);
  void annotationUrlEntered(const QString &url);

public slots:
  void setEnabled(bool t) { enabled_ = t; }
  void checkClipboard() { invalidateClipboard(); }

protected slots:
  void invalidateClipboard();
  void parseUrl(const QString &text);

protected:
  static bool isSupportedMediaUrl(const QString &url);
  static bool isSupportedAnnotationUrl(const QString &url);
};

#endif // CLIPBOARDMONITOR_H
