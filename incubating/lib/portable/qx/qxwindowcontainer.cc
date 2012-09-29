// qxwindowcontainer.cc
// 9/22/2012
//
// See: http://qt-project.org/forums/viewthread/16869
// See: https://bugreports.qt-project.org/browse/QTBG-25643
// See: http://code.google.com/p/h-qt-exercise/

#include "qx/qxwindowcontainer.h"
#include <QtCore/QCoreApplication>
#include <QtGui/QWindow>

QX_BEGIN_NAMESPACE

// - Construction -

QxWindowContainer::QxWindowContainer(QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f), delegate_(nullptr)
{ init(); }

QxWindowContainer::QxWindowContainer(QWindow *delegate, QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f), delegate_(delegate)
{ init(); }

void
QxWindowContainer::init()
{
  setAttribute(Qt::WA_NativeWindow);
  setAttribute(Qt::WA_DontCreateNativeAncestors);
  setAttribute(Qt::WA_NoBackground);
  if (delegate_)
    setDelegateWindow(delegate_);
}

// - Properties -

QWindow*
QxWindowContainer::delegateWindow() const
{ return delegate_; }

void
QxWindowContainer::setDelegateWindow(QWindow *value)
{
  if (delegate_)
    delegate_->removeEventFilter(this);
  delegate_ = value;
  if (delegate_) {
    delegate_->installEventFilter(this);
    if (!(delegate_->windowFlags() & Qt::FramelessWindowHint))
      delegate_->setWindowFlags(delegate_->windowFlags() | Qt::FramelessWindowHint);
    delegate_->setParent(windowHandle());
    delegate_->resize(size());
    delegate_->setVisible(isVisible());
  }
}

// - Events -

void
QxWindowContainer::setVisible(bool visible)
{
  Base::setVisible(visible);
  if (delegate_)
    delegate_->setVisible( visible);
}

bool
QxWindowContainer::eventFilter(QObject *obj, QEvent *ev)
{
  return obj == delegate_ &&
         size().isEmpty() && ev->type() == QEvent::Expose;
}

bool
QxWindowContainer::event(QEvent *ev)
{
  bool ret = Base::event(ev);
  if (delegate_)
    switch (ev->type()) {
    //case QEvent::UpdateRequest:
    //  return true;
    case QEvent::Wheel:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
      QCoreApplication::sendEvent(delegate_, ev);
      break;
    case QEvent::Show:
      if (!delegate_->parent())
        delegate_->setParent(windowHandle());
      break;
    case QEvent::Hide:
       windowHandle()->close();
       break;
    case QEvent::Resize:
      delegate_->resize(size());
      break;
    case QEvent::Move:
      break;
    default: ;
    }
  return ret;
}

QX_END_NAMESPACE
