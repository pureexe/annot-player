#ifndef ANNOTATIONWIDGET_H
#define ANNOTATIONWIDGET_H

// annotationwidget.h
// 7/4/2011

#include <QColumnView>

// class AnnotationWidget
/**
 *  \brief  Twitter like view for annotations.
 *
 *  Suppose to be base class for all annotation views.
 */
class AnnotationWidget : public QWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationWidget)
  typedef AnnotationWidget Self;
  typedef QWidget Base;

public:
  explicit AnnotationWidget(QWidget *parent)
    : Base(parent)
  {
    setWindowTitle(tr("Annotations"));
  }
};

// class DocumentAnnotationWidget
///  \brief  All anntations for entire document.
class DocumentAnnotationWidget : public QColumnView
{
  Q_OBJECT
  typedef DocumentAnnotationWidget Self;
  typedef QColumnView Base;

public:
  explicit DocumentAnnotationWidget(QWidget *parent)
    : Base(parent)
  {
    setWindowTitle(tr("All annotations"));
  }
};

// class PageAnnotationWidget
///  \brief  All anntations for current page.
class PageAnnotationWidget : public QWidget
{
  Q_OBJECT
  typedef PageAnnotationWidget Self;
  typedef QWidget Base;

public:
  explicit PageAnnotationWidget(QWidget *parent)
    : Base(parent)
  {
    setWindowTitle(tr("Page # - TODO"));
  }
};

#endif // ANNOTATIONWIDGET_H
