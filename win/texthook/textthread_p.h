#pragma once

// textthreadprivate.h  6/6/2012

#include "wmtimer/wmtimer.h"
#include <QtCore/QByteArray>

class TextThread;
class TextThreadDelegate
{
  Q_DISABLE_COPY(TextThreadDelegate)
  typedef TextThreadDelegate Self;

  TextThread *t_;
  QByteArray buffer_;
  WmTimer flushTimer_; // as QTimer does not work with windows remote thread, use native WM_TIMER instead

  mutable QString provider_; // buffered
  mutable ulong signature_; // buffered

  // - Construction -
public:
  TextThreadDelegate(TextThread *t, qint64 flushInterval, WId parentWindow = 0)
    : t_(t), flushTimer_(parentWindow), signature_(0)
  {
    Q_ASSERT(t);
    Q_ASSERT(flushInterval);
    flushTimer_.setInterval(flushInterval);
    flushTimer_.setCallback(this, &Self::flush);
    flushTimer_.setSingleShot(true);
  }

  ~TextThreadDelegate()
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

protected: // not used by outside
  QString text() const { return QString::fromLocal8Bit(buffer_); }

  ulong processId() const;

  QString provider() const; ///< thread string
  qint64 signature() const;

  QString threadSummary() const; ///< entry string, example: "0006:2908:0x0041A620:0x0041F4F9:0x00000017:MAJIRO"
  ulong threadContext() const;
  ulong threadSubcontext() const;

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
