// commentview.cc
// 10/9/2011

#include "commentview.h"
#include "defines.h"

#include <QtGui>
#include <QtWebKit>

// - Constructions -
CommentView::CommentView(QWidget *parent)
  : Base(parent), tokenId_(0)
{ }

// - Properties -

qint64
CommentView::tokenId() const
{ return tokenId_; }

void
CommentView::setTokenId(qint64 mtid)
{ tokenId_ = mtid; }

QUrl
CommentView::urlForTokenId(qint64 mtid)
{
  //TODO
  //return QUrl(
  //  mtid ? QString( "%1?mtid=%2").arg(G_TOKENVIEW_URL).arg(mtid)
  //       : G_STARTPAGE_URL
  //);
  Q_UNUSED(mtid);
  return QString(G_STARTPAGE_URL);
}

// - Events -

void
CommentView::setVisible(bool visible)
{
  if (visible)
    load(urlForTokenId(tokenId_));
  Base::setVisible(visible);
}

// EOF
