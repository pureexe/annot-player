// annotationgraphicsitem.cc
// 7/16/2011

#include "annotationgraphicsitem.h"
#include "annotationgraphicsview.h"
#include "annotationgraphicsstyle.h"
#include "annotationeditor.h"
//#include "textformathandler.h"
#include "uistyle.h"
#include "tr.h"
#include "defines.h"
#include "logger.h"
#include "signalhub.h"
#include "module/serveragent/serveragent.h"
#include "core/cmd.h"
#include "core/htmltag.h"
#include "core/annotationparser.h"
#include <boost/tuple/tuple.hpp>
#include <QtGui>
#include <ctime>

#define DEBUG "annotationgraphicsitem"
#include "module/debug/debug.h"

using namespace Core::Cloud;
using namespace Logger;

// - Helpers -

#define ANNOTATION_LIFE_SCHEDULE   ((ANNOTATION_LIFE_VISIBLE + 1) / 4)

namespace { // anonymous, annotation display

  inline QFont
  default_annot_font_()
  {
    QFont font(ANNOTATION_FONT_DEFAULT, ANNOTATION_SIZE_DEFAULT);
    font.setWeight(QFont::DemiBold);

    //font.setStyle(QFont::StyleOblique);
    //font.setStyleStrategy(QFont::PreferAntialias);
    //font.setStyleStrategy(QFont::PreferOutline);
    font.setStyleStrategy((QFont::StyleStrategy)(
      QFont::ForceOutline | QFont::PreferQuality
    ));

    return font;
  }

  // Use std time() rather than QTime::currentTime() to improve performance.
  inline int
  next_y_(int window_height, int visible_time, AnnotationGraphicsItem::Style style, const SignalHub *hub_)
  {
    Q_ASSERT(hub_);
    enum { lane_height = ANNOTATION_SIZE_DEFAULT + ANNOTATION_SIZE_MARGIN * 2 }; // height of a piece of danmu
    enum { lane_count = 100 };                      // number of vertical lanes, large enough

    static time_t last_time_fly_[lane_count],
                  last_time_top_[lane_count],
                  last_time_bottom_[lane_count];

    Q_ASSERT(visible_time > 0);
    //int wait_time = style == AnnotationGraphicsItem::FloatStyle ? (visible_time / 4 + 1) : visible_time;
    int wait_time = visible_time / 4 + 1;

    time_t *last_time_;
    switch (style) {
    case AnnotationGraphicsItem::FloatStyle:    last_time_ = last_time_fly_; break;
    case AnnotationGraphicsItem::TopStyle:      last_time_ = last_time_top_; break;
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

    int window_header = 0;
    if (hub_->isSignalTokenMode() && hub_->isEmbeddedPlayerMode())
      window_header = 40;

    switch (style) {
    case AnnotationGraphicsItem::BottomStyle:   return window_height - (best_lane + 2) * lane_height;
    default:                    return best_lane * lane_height + window_header;
    }
  }

} // anonymous namespace

// - Constructions -

void
AnnotationGraphicsItem::warmUp()
{
#ifdef Q_WS_MAC
  QGraphicsTextItem dummy;
  QFont font = ::default_annot_font_();

  font.setWeight(QFont::Light);
  dummy.setFont(font);

  font.setWeight(QFont::Normal);
  dummy.setFont(font);

  font.setWeight(QFont::DemiBold);
  dummy.setFont(font);

  font.setWeight(QFont::Bold);
  dummy.setFont(font);

  font.setWeight(QFont::Black);
  dummy.setFont(font);
#endif // Q_WS_MAC
}

AnnotationGraphicsItem::AnnotationGraphicsItem(
  const Annotation &annotation,
  SignalHub *hub,
  ServerAgent *server,
  AnnotationGraphicsView *view)
  : view_(view), hub_(hub), server_(server), style_(FloatStyle), dragPos_(BAD_POS)
{
  Q_ASSERT(hub_);
  Q_ASSERT(server_);
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

  QString text = annotation_.text();
  if (text.contains(CORE_CMD_SUB " ") || text.contains(CORE_CMD_SUBTITLE " "))
    text = view_->subtitlePrefix() + text;

  QString code;
  QStringList tags;
  boost::tie(code, tags) = ANNOT_PARSE_CODE(text);
  setTags(tags);
  if (!tags.empty() && tags.contains(CORE_CMD_VERBATIM))
    setPlainText(code);
  else
    setText(code);
}

// TODO: How to use QTextCharFormat to set advanced format:
// See: http://lists.trolltech.com/qt-interest/2005-12/thread00469-0.html
// See also: http://www.qtcentre.org/threads/26168-Contoured-text-in-editable-QGraphicsTextItem
void
AnnotationGraphicsItem::setText(const QString &text)
{
  setHtml(text);

  //Q_ASSERT(document());
  //TextFormatHandler *h = new TextFormatHandler(text, this);
  //document()->documentLayout()->registerHandler(TextFormatHandler::TextFormat, h);
  //QTextCharFormat f;
  //f.setObjectType(TextFormatHandler::TextFormat);
  //QTextCursor tc = textCursor();
  //tc.insertText(QString(QChar::ObjectReplacementCharacter), f);
  //setTextCursor(tc);

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
      case Core::H_Fly:
      case Core::H_Float: setStyle(FloatStyle); break;
      case Core::H_Top: setStyle(TopStyle); break;
      case Core::H_Bottom: setStyle(BottomStyle); break;

      case Core::H_Sub:
      case Core::H_Subtitle: setStyle(SubtitleStyle); break;
#ifdef DEBUG
      default:
        // Warn if the annot is submitted by current user
        if (!annotation_.hasUserId() && !annotation_.hasUserAlias() ||
            server_->isAuthorized() && server_->user().id() == annotation_.id())
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
  QFont font = ::default_annot_font_();

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

  else if (hash == Core::H_Float) {
    setStyle(Float);
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
  case FloatStyle:
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
  case FloatStyle:
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
  case FloatStyle:
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
  case FloatStyle:
    if (ani_->state() == QAbstractAnimation::Paused)
      ani_->resume();
    break;

  case TopStyle:
  case BottomStyle:
  case SubtitleStyle:
    if (!autoRemoveTimer_->isActive())
      autoRemoveTimer_->start(stayTime(style_));
    break;
  }
}

int
AnnotationGraphicsItem::stayTime(Style style) const
{
  switch (style) {
  case SubtitleStyle:   return ANNOTATION_STAY_TIME_SUBTITLE;
  default:              return ANNOTATION_STAY_TIME;
  }
}

int
AnnotationGraphicsItem::flyTime() const
{ return ANNOTATION_FLY_TIME * (qMax(view_->width(), 100) + 200) / (qMax((int)boundingRect().width(), 50) + 200); }

void
AnnotationGraphicsItem::stay(Style style)
{
  int msecs = stayTime(style);

  Style posStyle = style;
  if (style == SubtitleStyle)
    switch (view_->subtitlePosition()) {
    case AnnotationGraphicsView::AP_Top:    posStyle = TopStyle; break;
    case AnnotationGraphicsView::AP_Bottom: posStyle = BottomStyle; break;
    }

  int x = (view_->width() - boundingRect().width()) / 2,
      y = ::next_y_(view_->height(), msecs, posStyle, hub_);

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
  int y = ::next_y_(view_->height(), msecs, FloatStyle, hub_);

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
  DOUT("enter");
  if (event) {
    bool paused = isPaused();
    if (!paused)
      pause();

    QMenu menu;
    UiStyle::globalInstance()->setContextMenuStyle(&menu, false); // persistent = false

    menu.addAction(TR(T_MENUTEXT_EDIT), this, SLOT(edit()));
    menu.addAction(TR(T_MENUTEXT_COPY), this, SLOT(copyToClipboard()));
    menu.addSeparator();
    menu.addAction(TR(T_MENUTEXT_REMOVEANNOTATION), this, SLOT(removeMe()));

    menu.exec(event->globalPos());
    event->accept();

    if (!paused)
      resume();
  }
  DOUT("exit");
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
