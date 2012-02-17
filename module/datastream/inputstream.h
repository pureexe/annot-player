#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H

// inputstream.h
// 2/9/2012

#include <QtGlobal>
#include <QList>
#include <QString>

class InputStream
{
public:
  virtual ~InputStream() { }

  bool isEmpty() const { return !size(); }

  bool atEnd() const
  { qint64 s = size(); return s && pos() >= s; }

  virtual void setBufferSize(qint64 size) { Q_UNUSED(size); }

  virtual qint64 size() const { return 0; }
  virtual qint64 pos() const { return 0; }

  virtual qint64 read(char *data, qint64 maxSize) = 0;
  virtual bool reset() = 0;

  virtual bool seek(qint64 pos) { Q_UNUSED(pos); return false; }
  virtual qint64 skip(qint64 count) { Q_UNUSED(count); return 0; }

  QByteArray readAll();
  bool writeToFile(const QString &path);
};

class InputStreamList : public QList<InputStream*>
{
  typedef InputStreamList Self;
  typedef QList<InputStream*> Base;
  typedef InputStream value_type;
public:
  void reset()
  { foreach (value_type *v, *this) v->reset(); }

};

#endif // INPUTSTREAM_H
