// prefixlineedit.cc
// 9/26/2011
//
// 10/11/2011: This class is currently reserved for future specialization.
// For instance, add "\chat" into popup context menu.

#include "prefixlineedit.h"
#include <QtGui>

//#define DEBUG
#ifdef DEBUG
  #define DOUT(_msg)    qDebug() << "PrefixLineEdit:" << _msg
#else
  #define DOUT(_dummy)  ;
#endif // DEBUG

PrefixLineEdit::PrefixLineEdit(QWidget *parent)
  : Base(parent)
{ }


// EOF
