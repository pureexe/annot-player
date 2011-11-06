// core/text.cc
// 6/28/2011

#include "core/text.h"
#include <poppler-qt4.h>
#include <cmath>

namespace Core {

  bool
  detail::rects_in_the_same_line(const QRectF &left, const QRectF &right)
  {
    if (left.isEmpty() || right.isEmpty())
      return 0;

    qreal w0 = left.width(),
          hw = right.height(); // estimated height of word
    qreal dx = right.x() - left.x(),
          dy = right.y() - left.y(),
          dh = right.height() - left.height();

    return (dx > w0 - hw/3)     // x0 + w0 < x1, left and right relation
        && (fabs(dy) < hw/3 || fabs(dy + dh) < hw/3) // same horizontal level
        && (w0 + 2*hw > dx);    // x0 + w0 + delta > x1, horizontally near
  }

  bool
  detail::rects_in_the_same_block(const QRectF &up, const QRectF &down)
  {
    if (up.isEmpty() || down.isEmpty())
      return 0;

    qreal h0 = up.height(),
          hw = down.height(); // estimated height of word
    qreal dx = down.x() - up.x(),
          dy = down.y() - up.y(),
          dw = down.width() - up.width();

    return (dy > h0 - hw/3)    // y0 + h0 < y1, up and down relation
        && (fabs(dx) < hw*0.7 && fabs(dx + dw) < hw*0.7) // same vertical level
        && (h0 + hw > dy);    // y0 + h0 + delta > y1, vertical near
  }

  /**
   * TextRect
   */

  void
  TextRectCatalog::build(const TextList &l)
  {
    clear();

    if (l.empty())
      return;

    foreach (Poppler::TextBox *textbox, l) {
      // Catalog words
      Q_ASSERT(textbox);
      TextRect *word = new TextRect(textbox->boundingBox(), textbox->text());
      words_ << word;

      // Catalog lines
      if (lines_.empty()) {
        TextRect *newline = new TextRect(*word);
        newline->setType(TextRect::t_line);
        newline->children() << word;
        lines_ << newline;
      } else { // lines not empty
        TextRect *line = lines_.last();
        if (detail::rects_in_the_same_line(*line, *word)) {
          line->children() << word;
          line->setRect(line->united(*word));
        } else { // rects not in the same line
          TextRect *newline = new TextRect(*word);
          newline->setType(TextRect::t_line);
          newline->children() << word;
          lines_ << newline;
        }
      }
    }

    // Catalog blocks:
    foreach (auto line, lines_)
      if (blocks_.empty()) {
        TextRect *newblock = new TextRect(*line);
        newblock->setType(TextRect::t_block);
        newblock->children() << line;
        blocks_ << newblock;
      } else { // blocks not empty
        TextRect *block = blocks_.last();
        if (detail::rects_in_the_same_block(*block, *line)) {
          block->children() << line;
          block->setRect(block->united(*line));
        } else { // rects not in the same block
          TextRect *newblock = new TextRect(*line);
          newblock->setType(TextRect::t_block);
          newblock->children() << line;
          blocks_ << newblock;
        }
      }
  }

} // namespace Core

// EOF
