#ifndef DOCUMENTWIDGET_H
#define DOCUMENTWIDGET_H

// documentwidget.h
// 6/18/2011

#include "core/document.h"
#include <QMainWindow>
#include <QVector>
#include <boost/algorithm/minmax.hpp>

class RenderThread;
class Renderable;

class CatalogWidget;
class PreviewWidget;
class ThumbnailWidget;

class DocumentAnnotationWidget;
class PageAnnotationWidget;
// DocumentWidget
/**
 *  \brief  Main widget that holds the document and its different views.
 */
class DocumentWidget : public QMainWindow
{
  Q_OBJECT
  typedef DocumentWidget Self;
  typedef QMainWindow Base;

  Core::Document *document_;

  CatalogWidget *catalog_;
  PreviewWidget *preview_;
  ThumbnailWidget *thumbnail_;

  DocumentAnnotationWidget *documentAnnot_;
  PageAnnotationWidget *pageAnnot_;

  RenderThread *renderer_;

public:
  //explicit DocumentWidget(QWidget *parent = nullptr);

  /// Use render thread if \a parallel is true.
  explicit DocumentWidget(const QString &filePath, bool parallel = true, QWidget *parent = nullptr);

  bool bad() const { return !document_ || document_->bad(); }
  Core::Document *document() const { return document_; }

public slots:
  ///  Used for multithreading to be invoked from another render thread
  void renderDevice(Renderable *device, const QImage &image);

protected:
  void createThreads();     ///< Create renderer_
  void createDockWindows(); ///< Create annotation_, catalog_, preview_, and thumbnail_.

  /*
  void createActions();
  void createContextMenu();
  void createThreads();
  void createPages();

  // Properties:
public:
  // Set and render specified document if not null
  void setDocument(Poppler::Document *toRender);

  // clear opened pages
  void clear();

  bool bad() const { return !document_; }

  ///  Re-render all pages in the document in another thread
  //void renderPages(bool parallel = true);

  ///  Zoom rate must be larger than 0, default is 1.1
  //qreal zoomFactor() const { return zoom; }

  ///  Set zoom factor and render the pages
  //void setZoomFactor(qreal rate);

signals:
  //void mouseDoubleClicked(QMouseEvent *event);

public slots:
  //void zoomIn();
  //void zoomOut();

  ///  Set image of page at pageNumber
  //void renderPage(int pageNumber, const QImage& image);

protected:
  ///  Render my document
  //void renderDocument();

  // Events:
protected:
  //virtual void mouseDoubleClickEvent(QMouseEvent *event);

protected:
  QAction *zoomInAct;
  QAction *zoomOutAct;
  */
};

#endif // DOCUMENTWIDGET_H
