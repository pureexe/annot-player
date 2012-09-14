// mousegesture.cc
// 4/25/2012

#include "lib/mousegesture/mousegesture.h"
#include <QtCore/QStringList>

#define DEBUG "mousegesture"
#include "lib/debug/debug.h"

void
MouseGesture::trigger()
{
  DOUT("enter: directions =" << directionsToString(directions_));
  QString m = tr("mouse gesture") + ": " + directionsToString(directions_);
  if (!description_.isEmpty())
    m.append( "  | " + description_);
  emit message(m);
  emit triggered();
  DOUT("exit");
}

QString
MouseGesture::directionToString(Direction direction)
{
  switch (direction) {
#ifdef _MSC_VER
  case NoMatch: return "\xce\xa6";
  case Up: return "\xe2\x86\x91";
  case Down: return "\xe2\x86\x93";
  case Left: return "\xe2\x86\x90";
  case Right: return "\xe2\x86\x92";
#else
  case NoMatch: return "Φ";
  case Up: return "↑";
  case Down: return "↓";
  case Left: return "←";
  case Right: return "→";
#endif // _MSC_VER
  case Horizontal: return "-";
  case Vertical: return "|";
  default: return QString();
  }
}

QString
MouseGesture::directionsToString(const DirectionList &l, const QString &sep)
{
  QStringList ret;
  foreach (Direction d, l)
    ret.append(directionToString(d));
  return ret.join(sep);
}

// EOF
