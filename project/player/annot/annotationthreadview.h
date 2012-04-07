#ifndef ANNOTATIONTHREADVIEW_H
#define ANNOTATIONTHREADVIEW_H

// annotationthreadview.h
// 11/16/2011

#include "ac/acwebwindow.h"
#include "module/annotcloud/annotation.h"
#include <QLocale>

class AnnotationThreadView: public AcWebWindow
{
  Q_OBJECT
  typedef AnnotationThreadView Self;
  typedef AcWebWindow Base;

  typedef AnnotCloud::AnnotationList AnnotationList;

  int mode_;
  AnnotationList annots_;

  // - Constructions -
public:
  explicit AnnotationThreadView(QWidget *parent = 0);

signals:
  void annotationsRequested();

public slots:
  void setAnnotations(const AnnotationList &l) { annots_ = l; }
  void setMode(int tokenMode) { mode_ = tokenMode; }
  void refresh();

  // - Events -
public:
  virtual void setVisible(bool visible); ///< \override

private:
  void setupActions();
};

#endif // ANNOTATIONTHREADVIEW_H
