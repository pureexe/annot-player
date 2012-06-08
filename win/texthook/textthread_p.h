#pragma once

// textthreadprivate.h  6/6/2012

#include "wmtimer/wmtimer.h"
#include <QtCore/QByteArray>

class TextThread;
class TextThreadProperty
{
  Q_DISABLE_COPY(TextThreadProperty)
  typedef TextThreadProperty Self;

  TextThread *t_;
  QByteArray buffer_;
  WmTimer flushTimer_; // as QTimer does not work with windows remote thread, use native WM_TIMER instead

  // - Construction -
public:
  TextThreadProperty(TextThread *t, qint64 flushInterval, WId parentWindow = 0)
    : t_(t), flushTimer_(parentWindow)
  {
    Q_ASSERT(t);
    Q_ASSERT(flushInterval);
    flushTimer_.setInterval(flushInterval);
    flushTimer_.setCallback(this, &Self::flush);
    flushTimer_.setSingleShot(true);
  }

  ~TextThreadProperty()
  {
    if (flushTimer_.isActive())
      flushTimer_.stop();
  }

  // - Properties -
public:
  void append(const QByteArray &data)
  {
    flushTimer_.start();
    buffer_.append(data);
  }

  void append(const char *data, int len)
  {
    flushTimer_.start();
    if (len)
      buffer_.append(data, len);
  }

  QString text() const { return QString::fromLocal8Bit(buffer_); }

  // - Actions -
public:
  void flush()
  {
    if (flushTimer_.isActive())
      flushTimer_.stop();
    if (!buffer_.isEmpty()) {
      send();
      buffer_.clear();
    }
  }
  void clear() { buffer_.clear(); }

protected:
  void send();
};

// EOF
