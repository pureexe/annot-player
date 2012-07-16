#ifndef _QTEXT_BYTEARRAY_H
#define _QTEXT_BYTEARRAY_H

// qtext/bytearray.h
// 7/13/2012
// See also: <corelib/tools/qbytearray.cpp>

#include <QtCore/QByteArray>

QT_BEGIN_NAMESPACE

static inline char toHexHelper(char c)
{
  static const char hexnumbers[] = "0123456789ABCDEF";
  return hexnumbers[c & 0xf];
}

inline bool q_strchr(const char str[], char chr)
{
  if (!str) return false;

  const char *ptr = str;
  char c;
  while ((c = *ptr++))
    if (c == chr)
      return true;
  return false;
}


inline void q_fromPercentEncoding(QByteArray *ba, char percent)
{
  if (ba->isEmpty())
    return;

  char *data = ba->data();
  const char *inputPtr = data;

  int i = 0;
  int len = ba->count();
  int outlen = 0;
  int a, b;
  char c;
  while (i < len) {
    c = inputPtr[i];
    if (c == percent && i + 2 < len) {
      a = inputPtr[++i];
      b = inputPtr[++i];

      if (a >= '0' && a <= '9') a -= '0';
      else if (a >= 'a' && a <= 'f') a = a - 'a' + 10;
      else if (a >= 'A' && a <= 'F') a = a - 'A' + 10;

      if (b >= '0' && b <= '9') b -= '0';
      else if (b >= 'a' && b <= 'f') b  = b - 'a' + 10;
      else if (b >= 'A' && b <= 'F') b  = b - 'A' + 10;

      *data++ = (char)((a << 4) | b);
    } else {
      *data++ = c;
    }

    ++i;
    ++outlen;
  }

  if (outlen != len)
    ba->truncate(outlen);
}

inline void q_fromPercentEncoding(QByteArray *ba)
{ q_fromPercentEncoding(ba, '%'); }

inline void q_toPercentEncoding(QByteArray *ba, const char *dontEncode, const char *alsoEncode, char percent)
{
  if (ba->isEmpty())
    return;

  QByteArray input = *ba;
  int len = input.count();
  const char *inputData = input.constData();
  char *output = 0;
  int length = 0;

  for (int i = 0; i < len; ++i) {
    unsigned char c = *inputData++;
    if (((c >= 0x61 && c <= 0x7A) // ALPHA
       || (c >= 0x41 && c <= 0x5A) // ALPHA
       || (c >= 0x30 && c <= 0x39) // DIGIT
       || c == 0x2D // -
       || c == 0x2E // .
       || c == 0x5F // _
       || c == 0x7E // ~
       || q_strchr(dontEncode, c))
      && !q_strchr(alsoEncode, c)) {
      if (output)
        output[length] = c;
      ++length;
    } else {
      if (!output) {
        // detach now
        ba->resize(len*3); // worst case
        output = ba->data();
      }
      output[length++] = percent;
      output[length++] = toHexHelper((c & 0xf0) >> 4);
      output[length++] = toHexHelper(c & 0xf);
    }
  }
  if (output)
    ba->truncate(length);
}

inline void q_toPercentEncoding(QByteArray *ba, const char *exclude = "", const char *include = "")
{ q_toPercentEncoding(ba, exclude, include, '%'); }

inline void q_normalizePercentEncoding(QByteArray *ba, const char *exclude = "")
{
  q_fromPercentEncoding(ba, '%');
  q_toPercentEncoding(ba, exclude, 0, '%');
}

QT_END_NAMESPACE

#endif // _QTEXT_STRING_H
