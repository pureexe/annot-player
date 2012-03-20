#ifndef INPUTOUTPUTSTREAM_H
#define INPUTOUTPUTSTREAM_H

// inputoutputstream.h
// 3/18/2012

#include "inputstream.h"
#include "outputstream.h"

class InputOutputStream : public InputStream, public OutputStream
{ };

class InputOutputStreamList : public QList<InputOutputStream*>
{
public:
  void reset() const
  { foreach (value_type v, *this) v->reset(); }
};

#endif // INPUTOUTPUTSTREAM_H
