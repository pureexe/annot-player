#ifndef TEXTFORMATHANDLER_H
#define TEXTFORMATHANDLER_H

// textformathandler.h
// 11/8/2011
// See: http://www.qtcentre.org/threads/26168-Contoured-text-in-editable-QGraphicsTextItem

#include <QtCore/QObject>
#include <QtGui/QTextObjectInterface>
#include <QtGui/QTextFormat>

class TextFormatHandler : public QObject, public QTextObjectInterface
{
  Q_OBJECT
  Q_INTERFACES(QTextObjectInterface)
  Q_DISABLE_COPY(TextFormatHandler)
  typedef TextFormatHandler Self;
  typedef QObject Base;

  QString text_;

public:
  enum { TextFormat = QTextFormat::UserFormat + 1 };

public:
  explicit TextFormatHandler(const QString &text, QObject *parent = nullptr)
    : Base(parent), text_(text) { }

  // - Overridings -
public:
  void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format) override;

  QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format) override;
};

#endif // TEXTFORMATHANDLER_H
