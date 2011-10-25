// annotationgraphicsitem.cc
// 7/16/2011

#include "annotationgraphicsitem.h"
#include "annotationgraphicsview.h"
#include "annotationgraphicsstyle.h"
#include "annotationeditor.h"
#include "uistyle.h"
#include "tr.h"
#include "global.h"
#include "logger.h"
#include "signalhub.h"
#include "core/cmd.h"
#include "core/htmltag.h"
#include "core/annotationparser.h"
#include <boost/tuple/tuple.hpp>
#include <QtGui>
#include <ctime>

//#define DEBUG "AnnotationGraphicsItem"
#include "module/debug/debug.h"

using namespace Core::Data;
using namespace Logger;

// - Helpers -

#define ANNOTATION_LIFE_SCHEDULE   ((ANNOTATION_LIFE_VISIBLE + 1) / 4)

namespace { // anonymous, annotation display

  // Use std time() rather than QTime::currentTime() to improve performance.
  inline int next_y_(int window_height, int visible_time, AnnotationGraphicsItem::Style style)
  {
    enum { lane_height = ANNOTATION_SIZE_DEFAULT + ANNOTATION_SIZE_MARGIN }; // height of a piece of danmu
    enum { lane_count = 100 };                      // number of vertical lanes, large enough

    static time_t last_time_fly_[lane_count],
                  last_time_top_[lane_count],
                  last_time_bottom_[lane_count];

    Q_ASSERT(visible_time > 0);
    int wait_time = visible_time / 4 + 1;

    time_t *last_time_;
    switch (style) {
    case AnnotationGraphicsItem::FlyStyle:      last_time_ = last_time_fly_; break;
    case AnnotationGraphicsItem::TopStyle:      last_time_ = last_time_top_; break;
    case AnnotationGraphicsItem::SubtitleStyle:
    case AnnotationGraphicsItem::BottomStyle:   last_time_ = last_time_bottom_; break;
    default : Q_ASSERT(0);      last_time_ = last_time_fly_;
    }

    if (window_height <= lane_height * 2) // Do not schedule when window size is so small
      return 0;

    time_t current_time = ::time(0);
    int count = window_height / lane_height;
    if (count > lane_count)
      count = lane_count;

    int best_lane = 0;
    time_t max_time = current_time;
    for (int lane = 0; lane < count; lane++) {
      time_t last_time = last_time_[lane]; // difftime is in seconds, while wait_time is in msecs
      if (::difftime(current_time, last_time) * 1000 > wait_time) {
        best_lane = lane;
        break;
      } else if (max_time > last_time) {
        max_time = last_time;
        best_lane = lane;
      }
    }

    //qDebug() << best_lane;

    last_time_[best_lane] = current_time;

    switch (style) {
    case AnnotationGraphicsItem::SubtitleStyle:
    case AnnotationGraphicsItem::BottomStyle:   return window_height - (best_lane + 2) * lane_height;
    default:                    return best_lane * lane_height;
    }
  }

} // anonymous namespace

// - Constructions -

AnnotationGraphicsItem::AnnotationGraphicsItem(const Annotation &annotation, SignalHub *hub, AnnotationGraphicsView *view)
  : view_(view), hub_(hub), style_(FlyStyle), dragPos_(BAD_POS)
{
  Q_ASSERT(hub_);
  Q_ASSERT(view_);
  scene_ = view_->scene();
  Q_ASSERT(scene_);

  autoRemoveTimer_ = new QTimer(this);
  autoRemoveTimer_->setSingleShot(true);
  connect(autoRemoveTimer_, SIGNAL(timeout()), SLOT(removeMe()));

  ani_ = new QPropertyAnimation(this, "pos");
  connect(ani_, SIGNAL(finished()), SLOT(removeMe()));

  setDefaultStyle();
  setAnnotation(annotation);
}

const Annotation&
AnnotationGraphicsItem::annotation() const
{ return annotation_; }

void
AnnotationGraphicsItem::setAnnotation(const Annotation &annot)
{
  annotation_ = annot;

  QString text;
  QStringList tags;
  boost::tie(text, tags) = ANNOT_PARSE_CODE(annotation_.text());
  setTags(tags);
  if (!tags.empty() && tags.contains(CORE_CMD_VERBATIM))
    setPlainText(text);
  else
    setText(text);
}



// TODO: How to use QTextCharFormat to set advanced format:
// See: http://lists.trolltech.com/qt-interest/2005-12/thread00469-0.html
// See also: http://www.qtcentre.org/threads/26168-Contoured-text-in-editable-QGraphicsTextItem
void
AnnotationGraphicsItem::setText(const QString &text)
{
  setHtml(text);

  //QTextCharFormat format;
  //format.setTextOutline(QPen(Qt::blue, 3));
  //format.setFontPointSize(100);
  //QLinearGradient gradient(0, 0, 100, 100);
  //gradient.setColorAt(0, Qt::red);
  //gradient.setColorAt(1, Qt::green);
  //format.setForeground(QBrush(gradient));
  //textCursor().insertText(text, format);
}

void
AnnotationGraphicsItem::setTags(const QStringList &tags)
{
  if (!tags.empty())
    foreach (const QString &tag, tags) {
      switch (qHash(tag)) {
      case Core::H_Verbatim: continue;
      case Core::H_Fly: setStyle(FlyStyle); break;
      case Core::H_Top: setStyle(TopStyle); break;
      case Core::H_Bottom: setStyle(BottomStyle); break;

      case Core::H_Sub:
      case Core::H_Subtitle: setStyle(SubtitleStyle); break;
#ifdef DEBUG
      default:
        warn(TR(T_ERROR_UNKNOWN_COMMAND) + ": " + tag);
#endif // DEBUG
      }
    }
}

void
AnnotationGraphicsItem::setDefaultStyle()
{
  setFlags(QGraphicsItem::ItemIsMovable); // Doesn't work when view_ is embedded in dock window orz

  setToolTip(TR(T_TOOLTIP_ANNOTATIONITEM)); // TODO: Make this dynamically determined.

  setDefaultTextColor(ANNOTATION_COLOR_DEFAULT);



  //QGraphicsDropShadowEffect *shadow = QGraphicsDropShadowEffect(this));
  //setGraphicsEffect(shadow);

  //QGraphicsBlurEffect *blur = new QGraphicsBlurEffect(this);
  //blur->setBlurHints(QGraphicsBlurEffect::PerformanceHint);
  //blur->setBlurRadius(1.2);
  //setGraphicsEffect(blur);

//#if ANNOTATION_OPACITY < 1
  QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);
  effect->setOpacity(ANNOTATION_OPACITY);
  setGraphicsEffect(effect);
//#endif // ANNOTATION_OPACITY

  // Font
  QFont font(ANNOTATION_FONT_DEFAULT, ANNOTATION_SIZE_DEFAULT);
  //font.setStyle(QFont::StyleOblique);
  font.setStyleStrategy(QFont::PreferAntialias);
  font.setWeight(QFont::DemiBold);

  // if the annotation is made by user or doll, add underline
  //font.setUnderline(true);

  setFont(font);
}

AnnotationGraphicsItem::Style
AnnotationGraphicsItem::style() const
{ return style_; }

void
AnnotationGraphicsItem::setStyle(Style style)
{ style_ = style; }

/*
QString
AnnotationGraphicsItem::parse(const QString &input)
{
#define SELF(_text) parse(_text)
  QString text = input.trimmed();
  if (text.isEmpty())
    return QString();

  if (text[0] != CORE_CMDCH)
    return text;

  QString tag, left;
  boost::tie(tag, left) = Core::parseLeadingTag(text);
  uint hash = qHash(tag);

  if (hash == Core::H_Verbatim)
    return text;

  else if (hash == Core::H_Fly) {
    setStyle(Fly);
    return SELF(left);
  } else if (hash == Core::H_Top) {
    setStyle(Top);
    return SELF(left);
  } else if (hash == Core::H_Bottom) {
    setStyle(Bottom);
    return SELF(left);
  }

  else if (hash == Core::H_Em || hash == Core::H_Emph || hash == Core::H_TextIt)
    return CORE_HTML_EM(SELF(left));
  else if (hash == Core::H_Bf || hash == Core::H_Strong || hash == Core::H_TextBf)
    return CORE_HTML_STRONG(SELF(left));

  else if (hash == Core::H_LineThrough
           || hash == Core::H_Strike
           || hash == Core::H_StrikeOut
           || hash == Core::H_Sout)
    return CORE_HTML_STYLE(SELF(left), text-decoration:line-through);
  else if (hash == Core::H_Underline
           || hash == Core::H_Uline)
    return CORE_HTML_STYLE(SELF(left), text-decoration:underline);
  else if (hash == Core::H_Overline)
    return CORE_HTML_STYLE(SELF(left), text-decoration:overline);
  else if (hash == Core::H_Blink)
    return CORE_HTML_STYLE(SELF(left), text-decoration:blink);

#define ELIF_TRANSFORM(_trans) \
  else if (hash == Core::H_##_trans) \
    return CORE_HTML_STYLE(SELF(left), text-transform:_trans); \

  ELIF_TRANSFORM(Uppercase)
  ELIF_TRANSFORM(Lowercase)
#undef ELIF_TRANSFORM

#define ELIF_COLOR(_color) \
  else if (hash == (Core::H_##_color)) \
    return CORE_HTML_STYLE(SELF(left), color:_color);

  ELIF_COLOR(Black)
  ELIF_COLOR(Blue)
  ELIF_COLOR(Brown)
  ELIF_COLOR(Cyan)
  ELIF_COLOR(Green)
  ELIF_COLOR(Gray)
  ELIF_COLOR(Grey)
  ELIF_COLOR(Magenta)
  ELIF_COLOR(Orange)
  ELIF_COLOR(Pink)
  ELIF_COLOR(Purple)
  ELIF_COLOR(Red)
  ELIF_COLOR(White)
  ELIF_COLOR(Yellow)
#undef ELIF_COLOR

#define ELIF_SIZE(_id, _size) \
  else if (hash == (Core::H_##_id)) \
    return CORE_HTML_STYLE(SELF(left), font-size:_size);

  ELIF_SIZE(Tiny,       ANNOTATION_SIZE_TINY)
  ELIF_SIZE(Small,      ANNOTATION_SIZE_SMALL)
  ELIF_SIZE(Normal,     ANNOTATION_SIZE_NORMAL)
  ELIF_SIZE(Large,      ANNOTATION_SIZE_LARGE)
  ELIF_SIZE(Huge,       ANNOTATION_SIZE_HUGE)
#undef ELIF_SIZE

  else {
    warn(TR(T_ERROR_UNKNOWN_COMMAND) + ": " + text);
    return text;
  }
#undef SELF
}

*/

// - Add/remove me -

void
AnnotationGraphicsItem::addMe()
{
  connect(view_, SIGNAL(paused()), SLOT(pause()));
  connect(view_, SIGNAL(resumed()), SLOT(resume()));

  if (style_ == SubtitleStyle &&
      hub_->isSignalTokenMode() &&
      !hub_->isStopped())
    connect(view_, SIGNAL(posChanged()), SLOT(removeMe()));
  scene_->addItem(this);
}

void
AnnotationGraphicsItem::removeMe()
{
  if (style_ == SubtitleStyle &&
      hub_->isSignalTokenMode() &&
      !hub_->isStopped())
    disconnect(view_, SIGNAL(posChanged()), this, SLOT(removeMe()));
  disconnect(view_, SIGNAL(paused()), this, SLOT(pause()));
  disconnect(view_, SIGNAL(resumed()), this, SLOT(resume()));
  scene_->removeItem(this);
}

void AnnotationGraphicsItem::showMe()
{
  switch (style_) {
  case FlyStyle:
    fly(); break;

  case TopStyle:
  case BottomStyle:
  case SubtitleStyle:
    stay(style_); break;

  default:
    Q_ASSERT(0); fly();
  }
}

void
AnnotationGraphicsItem::autoRemove(int msecs)
{ autoRemoveTimer_->start(msecs); }


// - Show up -

bool
AnnotationGraphicsItem::isPaused() const
{
  if (style_ == SubtitleStyle &&
      hub_->isSignalTokenMode() &&
      !hub_->isStopped())
    return false;

  Q_ASSERT(ani_);
  Q_ASSERT(autoRemoveTimer_);
  switch (style_) {
  case FlyStyle:
    return ani_->state() == QAbstractAnimation::Paused;

  case TopStyle:
  case BottomStyle:
  case SubtitleStyle:
    return !autoRemoveTimer_->isActive();
  }
  return false;
}

void
AnnotationGraphicsItem::pause()
{
  if (style_ == SubtitleStyle &&
      hub_->isSignalTokenMode() &&
      !hub_->isStopped())
    return;

  Q_ASSERT(ani_);
  Q_ASSERT(autoRemoveTimer_);
  switch (style_) {
  case FlyStyle:
    if (ani_->state() == QAbstractAnimation::Running)
      ani_->pause();
    break;

  case TopStyle:
  case BottomStyle:
  case SubtitleStyle:
    if (autoRemoveTimer_->isActive())
      autoRemoveTimer_->stop();
    break;
  }
}

void
AnnotationGraphicsItem::resume()
{
  if (style_ == SubtitleStyle &&
      hub_->isSignalTokenMode() &&
      !hub_->isStopped())
    return;

  Q_ASSERT(ani_);
  Q_ASSERT(autoRemoveTimer_);
  switch (style_) {
  case FlyStyle:
    if (ani_->state() == QAbstractAnimation::Paused)
      ani_->resume();
    break;

  case TopStyle:
  case BottomStyle:
  case SubtitleStyle:
    if (!autoRemoveTimer_->isActive())
      autoRemoveTimer_->start(stayTime());
    break;
  }
}

int
AnnotationGraphicsItem::stayTime() const
{ return ANNOTATION_STAY_TIME; }

int
AnnotationGraphicsItem::flyTime() const
{ return (view_->width() + boundingRect().width()) * 1000 / ANNOTATION_FLY_SPEED; }

void
AnnotationGraphicsItem::stay(Style style)
{
  int msecs = stayTime();
  int x = (view_->width() - boundingRect().width()) / 2,
      y = ::next_y_(view_->height(), msecs, style);

  if (style_ == SubtitleStyle &&
      hub_->isSignalTokenMode() &&
      !hub_->isStopped())
    msecs = -1;
  stay(QPointF(x, y), msecs);
}

void
AnnotationGraphicsItem::stay(const QPointF &pos, int msecs)
{
  setPos(pos);
  addMe();
  if (hub_->isMediaTokenMode() ||
      hub_->isSignalTokenMode() && !hub_->isStopped() && msecs >= 0)
    autoRemove(msecs);
}

void
AnnotationGraphicsItem::fly()
{
  Q_ASSERT(view_);
  int msecs = flyTime();
  int y = ::next_y_(view_->height(), msecs, FlyStyle);

  QPoint from(view_->width(), y);
  QPoint to(- boundingRect().width(), y);
  fly(from, to, msecs);
}

void
AnnotationGraphicsItem::fly(const QPointF &from, const QPointF &to, int msecs, QEasingCurve::Type type)
{
  Q_ASSERT(msecs > 0);
  Q_ASSERT(ani_);
  if (ani_->state() != QAbstractAnimation::Stopped)
    ani_->stop();
  else
    addMe();

  ani_->setDuration(msecs);
  ani_->setStartValue(from);
  ani_->setEndValue(to);
  ani_->setEasingCurve(type);

  ani_->start();
}

// - Events -

void
AnnotationGraphicsItem::contextMenuEvent(QContextMenuEvent *event)
{
  DOUT("contexrMenuEvent: enter");
  if (event) {
    bool paused = isPaused();
    if (!paused)
      pause();

    QMenu menu;
    UiStyle::globalInstance()->setContextMenuStyle(&menu);

    menu.addAction(TR(T_MENUTEXT_EDIT), this, SLOT(edit()));
    menu.addAction(TR(T_MENUTEXT_COPY), this, SLOT(copyToClipboard()));
    menu.addSeparator();
    menu.addAction(TR(T_MENUTEXT_REMOVEANNOTATION), this, SLOT(removeMe()));

    menu.exec(event->globalPos());
    event->accept();

    if (!paused)
      resume();
  }
  DOUT("contexrMenuEvent: exit");
}

void
AnnotationGraphicsItem::mouseDoubleClickEvent(QMouseEvent *event)
{
  Q_UNUSED(event);
  if (isPaused())
    resume();
  else
    pause();
}

void
AnnotationGraphicsItem::mousePressEvent(QMouseEvent *event)
{
  if (!event|| dragPos_ != BAD_POS)
    return;

  if (event->button() == Qt::LeftButton)
    dragPos_ = event->globalPos() -  QPoint(scenePos().x(), scenePos().y());
}

void
AnnotationGraphicsItem::mouseReleaseEvent(QMouseEvent *event)
{
  dragPos_ = BAD_POS;
  Q_UNUSED(event);
}

void
AnnotationGraphicsItem::mouseMoveEvent(QMouseEvent *event)
{
  if (event && event->buttons() & Qt::LeftButton
      && dragPos_ != BAD_POS) {
    QPoint newPos = event->globalPos() - dragPos_;
    // use QApplication::postEvent is more elegant but less efficient
    setPos(newPos);
  }
}

void
AnnotationGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
  if (event) {
    QPoint pos(event->pos().x(), event->pos().y());
    QContextMenuEvent e(QContextMenuEvent::Keyboard, pos, event->screenPos());
    contextMenuEvent(&e);
  }
}

#define GRAPHICS_SCENE_MOUSE_EVENT(_handler) \
  void \
  AnnotationGraphicsItem::_handler(QGraphicsSceneMouseEvent *event) \
  { \
    if (event) { \
      QPoint pos(event->pos().x(), event->pos().y()); \
      QMouseEvent e(event->type(), pos, event->screenPos(), event->button(), event->buttons(), event->modifiers()); \
      _handler(&e); \
    } \
  }

  GRAPHICS_SCENE_MOUSE_EVENT(mouseDoubleClickEvent)
  GRAPHICS_SCENE_MOUSE_EVENT(mousePressEvent)
  GRAPHICS_SCENE_MOUSE_EVENT(mouseReleaseEvent)
  GRAPHICS_SCENE_MOUSE_EVENT(mouseMoveEvent)
#undef GRAPHICS_SCENE_MOUSE_EVENT

// - Copy && paste -

void
AnnotationGraphicsItem::copyToClipboard() const
{
  QClipboard *clipboard = QApplication::clipboard();
  if (clipboard) {
    clipboard->setText(annotation_.text());
    log(TR(T_SUCCEED_ANNOTATION_COPIED) + ": " + annotation_.text());
  } else
    warn(TR(T_ERROR_CLIPBOARD_UNAVAILABLE));
}

void
AnnotationGraphicsItem::edit()
{
  view_->editor()->setText(annotation_.text());
  view_->editor()->setId(annotation_.id());
  view_->editor()->show();
}

// EOF
