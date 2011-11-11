#ifndef QTSTEP_H
#define QTSTEP_H

// qtstep.h
// 7/30/2011

// - NSView -
struct nsview_t { };
nsview_t *nsview_new();
void nsview_release(nsview_t *handle);
void nsview_set_hidden(nsview_t *handle, bool hidden);
bool nsview_is_hidden(const nsview_t *handle);

#endif // QTSTEP_H
