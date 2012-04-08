#ifndef ANNOTATIONTHREADVIEW_H
#define ANNOTATIONTHREADVIEW_H

// annotationthreadview.h
// 11/16/2011

#include "ac/acwebwindow.h"
#include "module/annotcloud/annotation.h"
#include <QUrl>

class DataManager;

class AnnotationThreadView: public AcWebWindow
{
  Q_OBJECT
  typedef AnnotationThreadView Self;
  typedef AcWebWindow Base;

  typedef AnnotCloud::AnnotationList AnnotationList;

  // - Constructions -
public:
  explicit AnnotationThreadView(DataManager *data, QWidget *parent = 0);

signals:
  void contentChanged(const QString &html);
  void windowTitleChanged(const QString &title);
  void urlChanged(const QUrl &url);
  void visibleChanged(bool visible);

public slots:
  void refresh();

  // - Events -
public:
  //virtual void setVisible(bool visible); ///< \override

protected slots:
  void setContent(const QString &html);
  void setUrl(const QUrl &url);

  // - Implementations
public slots:
  void invalidateAnnotations(bool async = true);
private:
  void setupActions();

private:
  DataManager *data_;
  bool refreshing_;
};

#endif // ANNOTATIONTHREADVIEW_H
