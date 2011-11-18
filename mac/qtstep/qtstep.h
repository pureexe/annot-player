#ifndef QTSTEP_H
#define QTSTEP_H

// qtstep.h
// 7/30/2011

#include <QPoint>

// - NSView -
struct nsview_t { };
nsview_t *nsview_new();
void nsview_release(nsview_t *view);
void nsview_set_hidden(nsview_t *view, bool hidden);
bool nsview_is_hidden(const nsview_t *view);

// - QtStep -
namespace QtStep {

  // Send mouse click event
  void mouseClickEvent(nsview_t *view, const QPoint &pos);

} // namespace QtStep

#endif // QTSTEP_H
