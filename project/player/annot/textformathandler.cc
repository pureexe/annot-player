// textformathandler.cc
// 11/8/2011

#include "textformathandler.h"
#include <QtGui>

 // - Paint -

void
TextFormatHandler::drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
  Q_ASSERT(painter);
  Q_UNUSED(doc);
  Q_UNUSED(posInDocument);
  Q_UNUSED(format);

  Qt::PenStyle outlinePen = Qt::SolidLine;
  QColor textColor("white");

  painter->setPen(outlinePen);
  painter->setBrush(textColor);

  QPainterPath path;
  path.addText(rect.bottomLeft(), doc->defaultFont(), text_);

  painter->drawPath(path);
}

QSizeF
TextFormatHandler::intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
  Q_ASSERT(doc);
  Q_UNUSED(posInDocument);
  Q_UNUSED(format);

  const QFontMetrics metrics(doc->defaultFont());
  const QSize size = metrics.size(Qt::TextSingleLine, text_);
  return size;
}

// EOF
