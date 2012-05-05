// annotationgraphicsitem.cc
// 7/16/2011

#include "annotationgraphicsitem.h"
#include "annotationgraphicsview.h"
#include "annotationgraphicsstyle.h"
#include "annotationgraphicseffect.h"
#include "annotationeditor.h"
//#include "textformathandler.h"
#include "tr.h"
#include "global.h"
#include "logger.h"
#include "signalhub.h"
#include "project/common/acui.h"
#include "module/annotcloud/annottag.h"
#include "module/annotcloud/annothtml.h"
#include "module/qtext/htmltag.h"
#include <boost/tuple/tuple.hpp>
#include <boost/typeof/typeof.hpp>
#include <QtGui>
#include <ctime>
#include <cmath>

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses around && within ||
#endif // __GNUC__

#ifdef __GNUC__
#  define NOINLINE      __attribute__((noinline))
#else
#  define NOINLINE
#endif // __GNUC__

//#define DEBUG "annotationgraphicsitem"
#include "module/debug/debug.h"

using namespace AnnotCloud;
using namespace Logger;

// - Helpers -

#define ANNOTATION_LIFE_SCHEDULE   ((ANNOTATION_LIFE_ViSIBLE + 1) / 4)

namespace { namespace curve_ { // anonymous, curves

  inline qreal linear_(qreal progress, qreal start, qreal stop)
  {
    return  progress < start ? progress/start :
            progress < stop  ? 1.0 :
                               (1.0-progress)/(1.0-stop);
  }
  inline qreal in2_(qreal progress, qreal start, qreal stop)
  {
    qreal t; // quadro
    return  progress < start ? (t=progress/start) * t :
            progress < stop  ? 1.0 :
                               (t=(1.0-progress)/(1.0-stop)) * t;
  }
  inline qreal in3_(qreal progress, qreal start, qreal stop)
  {
    qreal t; // quadro
    return  progress < start ? (t=progress/start) * t*t :
            progress < stop  ? 1.0 :
                               (t=(1.0-progress)/(1.0-stop)) * t*t;
  }
  inline qreal out2_(qreal progress, qreal start, qreal stop)
  {
    qreal t; // quadro
    return  progress < start ? 1.0 - (t=(start-progress)/start) * t :
            progress < stop  ? 1.0 :
                               1.0 - (t=(progress-stop)/(1-stop)) * t;
  }
  inline qreal out3_(qreal progress, qreal start, qreal stop)
  {
    qreal t; // quadro
    return  progress < start ? 1.0 - (t=(start-progress)/start) * t*t:
            progress < stop  ? 1.0 :
                               1.0 - (t=(progress-stop)/(1-stop)) * t*t;
  }

  qreal NOINLINE appearOpacity(qreal progress)
  { return linear_(progress, 0.2, 0.85); }

  qreal NOINLINE flyOpacity(qreal progress)
  { return linear_(progress, 0.13, 0.85); }

} } // anonymous namespace

namespace { // anonymous, annotation display

  inline QFont
  default_annot_font_(int language)
  {
    enum { FontSize =
#ifdef Q_WS_WIN
     int(ANNOTATION_SIZE_DEFAULT * 0.9)
#else
     ANNOTATION_SIZE_DEFAULT
#endif // Q_WS_WIN
    };
    if (language == Traits::Chinese) {
      QFont font(ANNOTATION_FONT_CHINESE, FontSize);
      font.setWeight(QFont::DemiBold);
      font.setStyleStrategy((QFont::StyleStrategy)(
        QFont::ForceOutline | QFont::PreferQuality
      ));
      return font;

    } else {
      QFont font(ANNOTATION_FONT_JAPANESE, FontSize);
      font.setWeight(QFont::DemiBold);
      font.setStyleStrategy((QFont::StyleStrategy)(
        QFont::ForceOutline | QFont::PreferQuality
      ));
      return font;
    }
  }

  // Use std time() rather than QTime::currentTime() to improve performance.
  inline int
  next_y_(int window_height, int visible_time, qreal scale, AnnotationGraphicsItem::Style style, const SignalHub *hub_)
  {
    Q_ASSERT(hub_);
    enum { LaneHeight = ANNOTATION_SIZE_DEFAULT + ANNOTATION_SIZE_MARGIN * 2 + 6 }; // height of a piece of danmu
    enum { LaneCount = 100 };                      // number of vertical lanes, large enough

    int laneHeight = LaneHeight * scale;

    static time_t last_time_fly_[LaneCount],
                  last_time_top_[LaneCount],
                  last_time_bottom_[LaneCount];

    Q_ASSERT(visible_time > 0);
    int wait_time = 500;
    switch (style) {
    case AnnotationGraphicsItem::FloatStyle:
    case AnnotationGraphicsItem::FlyStyle: wait_time += visible_time / 2; break;
    default: wait_time += visible_time;
    }

    time_t *last_time_;
    switch (style) {
    case AnnotationGraphicsItem::FloatStyle:
    case AnnotationGraphicsItem::FlyStyle:      last_time_ = last_time_fly_; break;
    case AnnotationGraphicsItem::TopStyle:      last_time_ = last_time_top_; break;
    case AnnotationGraphicsItem::BottomStyle:
    case AnnotationGraphicsItem::SubtitleStyle: last_time_ = last_time_bottom_; break;
    default : Q_ASSERT(0);      last_time_ = last_time_fly_;
    }

    if (window_height <= laneHeight * 2) // Do not schedule when window size is so small
      return 0;

    time_t current_time = ::time(0);
    int count = window_height / laneHeight;
    if (count > LaneCount)
      count = LaneCount;

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
    case AnnotationGraphicsItem::BottomStyle:
    case AnnotationGraphicsItem::SubtitleStyle:
      {
        int window_footer = !hub_->isNormalPlayerMode() ? int(laneHeight * 1.5)   : 0;
        return window_height - (best_lane + 2) * laneHeight - window_footer;
      }
    default:
      {
        int window_header = !hub_->isNormalPlayerMode() && !hub_->isMediaTokenMode() ? 50 : 0;
        return best_lane * laneHeight + window_header;
      }
    }
  }

} // anonymous namespace

int
AnnotationGraphicsItem::nextY(int msecs, Style style) const
{
  int ret = ::next_y_(view_->height(), msecs, view_->scale(), style, hub_);
  int max = view_->height() - boundingRect().height() ;
  if (ret > max - 5)
    ret = max;
  return ret;
}

// - Constructions -

void
AnnotationGraphicsItem::warmUp()
{
  //qDebug() << ::default_annot_font_(Traits::Japanese).rawName();
  //qDebug() << ::default_annot_font_(Traits::Japanese).family();

#ifdef Q_OS_MAC
  QGraphicsTextItem dummy;

  QFont j = ::default_annot_font_(Traits::Japanese),
        c = ::default_annot_font_(Traits::Chinese);

  j.setWeight(QFont::Light); dummy.setFont(j);
  c.setWeight(QFont::Light); dummy.setFont(c);

  j.setWeight(QFont::Normal); dummy.setFont(j);
  c.setWeight(QFont::Normal); dummy.setFont(c);

  j.setWeight(QFont::DemiBold); dummy.setFont(j);
  c.setWeight(QFont::DemiBold); dummy.setFont(c);

  j.setWeight(QFont::Bold); dummy.setFont(j);
  c.setWeight(QFont::Bold); dummy.setFont(c);

  j.setWeight(QFont::Black); dummy.setFont(j);
  c.setWeight(QFont::Black); dummy.setFont(c);
#endif // Q_OS_MAC
}

AnnotationGraphicsItem::AnnotationGraphicsItem(
  SignalHub *hub,
  AnnotationGraphicsView *view)
  : autoDelete_(true), view_(view), hub_(hub), style_(FloatStyle),
    removeLaterTimer_(0),
    flyAni_(0), flyOpacityAni_(0), escapeAni_(0), rushAni_(0), appearOpacityAni_(0), fadeAni_(0),
    dragPos_(BAD_POS), dragPaused_(false), opacity_(1)
{
  DOUT("enter");
  Q_ASSERT(hub_);
  Q_ASSERT(view_);
  scene_ = view_->scene();
  Q_ASSERT(scene_);

  //setAcceptHoverEvents(true); // doesn't work, as the events are filtered by the firt responder
  //setAcceptTouchEvents(true);

  setScale(view_->scale());

  DOUT("exit");
}

void
AnnotationGraphicsItem::setAnnotation(const Annotation &annot)
{
  annot_ = annot;
  invalidateAnnotation();
}

bool
AnnotationGraphicsItem::isSubtitle(const QString &text)
{
  // FIXME: deal with subtitle
  //if (text.contains(CORE_CMD_SUB) || text.contains(CORE_CMD_SUBTITLE))
  return text.contains(CORE_CMD_SUB);
}

void
AnnotationGraphicsItem::reset()
{ setOpacity(1); }

void
AnnotationGraphicsItem::invalidateAnnotation()
{
  setDefaultStyle();
  //updateEffect();

  bool isOwner = view_->userId() &&
                 view_->userId() != User::UI_Guest &&
                 annot_.userId() == view_->userId();

  QFont font = ::default_annot_font_(annot_.language());
  if (isOwner)
    font.setUnderline(true);
  setFont(font);

  QString text = annot_.text();
  if (isSubtitle(text))
    text = view_->subtitlePrefix() + text;

  QString code;
  QStringList tags;
  boost::tie(code, tags) = ANNOT_PARSE_CODE(text);
  setTags(tags);
  if (tags.contains(CORE_CMD_VERBATIM))
    setPlainText(richText_ = code);
  else {
    setText(richText_ = code);
    if (isOwner)
      richText_ = CORE_CMD_LATEX_ULINE " " + richText_;
  }
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
      // Style:
      case AnnotCloud::H_Verbatim: continue;
      case AnnotCloud::H_Float: setStyle(FloatStyle); break;
      case AnnotCloud::H_Fly: setStyle(FlyStyle); break;
      case AnnotCloud::H_Top: setStyle(TopStyle); break;
      case AnnotCloud::H_Bottom: setStyle(BottomStyle); break;
      case AnnotCloud::H_Sub:
      case AnnotCloud::H_Subtitle: setStyle(SubtitleStyle); break;

      // Effect:
      case AnnotCloud::H_Transp:
      case AnnotCloud::H_Transparent: setEffect(TransparentEffect); break;
      case AnnotCloud::H_Shadow: setEffect(ShadowEffect); break;
      case AnnotCloud::H_Blur: setEffect(BlurEffect); break;

      default:
        // Warn if the annot is submitted by current user
        if (!annot_.hasUserId() && !annot_.hasUserAlias() ||
            view_->userId() == annot_.userId())
        warn(TR(T_ERROR_UNKNOWN_COMMAND) + ": " + tag);
      }
    }
}

void
AnnotationGraphicsItem::setDefaultStyle()
{
  setStyle(FloatStyle);
  setFlags(QGraphicsItem::ItemIsMovable); // Doesn't work when view_ is embedded in dock window orz

  setToolTip(TR(T_TOOLTIP_ANNOTATIONITEM)); // TODO: Make this dynamically determined.

  setDefaultTextColor(ANNOTATION_COLOR_DEFAULT);
}

void
AnnotationGraphicsItem::setOpacity(qreal value)
{
  if (!qFuzzyCompare(opacity_, value)) {
    opacity_ = value;
    updateOpacity();
  }
}

void
AnnotationGraphicsItem::updateOpacity()
{
  if (qFuzzyCompare(opacity_, 1))
    updateEffect();
  else {
    BOOST_AUTO(e, qobject_cast<QGraphicsOpacityEffect *>(graphicsEffect()));
    if (!e)
      e = new QGraphicsOpacityEffect;
    e->setOpacity(opacity_);
    if (e != graphicsEffect())
      setGraphicsEffect(e);
  }
}

void
AnnotationGraphicsItem::updateEffect()
{
  Effect e;
  switch (view_->renderHint()) {
  case AnnotationGraphicsView::TransparentHint: e = TransparentEffect; break;
  case AnnotationGraphicsView::ShadowHint: e = ShadowEffect; break;
  case AnnotationGraphicsView::BlurHint: e = BlurEffect; break;
  case AnnotationGraphicsView::DefaultRenderHint:
  default: e = DefaultEffect;
  }
  setEffect(e);
}

void
AnnotationGraphicsItem::setEffect(Effect e)
{
  switch (e) {
  case ShadowEffect:
    {
#ifdef Q_WS_WIN
      enum { offset = 1, radius = 18 };
#else
      enum { offset = 1, radius = 12 };
#endif // Q_WS_WIN
      BOOST_AUTO(e, qobject_cast<QGraphicsDropShadowEffect *>(graphicsEffect()));
      if (!e) {
        e = new QGraphicsDropShadowEffect;
        e->setBlurRadius(radius); // in pixels
        e->setOffset(offset); // in pixels
        e->setColor(Qt::black);
        setGraphicsEffect(e);
      }
    } break;
  case TransparentEffect:
    {
      BOOST_AUTO(e, qobject_cast<QGraphicsOpacityEffect *>(graphicsEffect()));
      if (!e) {
        e = new QGraphicsOpacityEffect;
        e->setOpacity(ANNOTATION_OPACITY);
        setGraphicsEffect(e);
      }
    } break;
  case BlurEffect:
    {
      BOOST_AUTO(e, qobject_cast<QGraphicsBlurEffect *>(graphicsEffect()));
      if (!e) {
        e = new QGraphicsBlurEffect;
        e->setBlurHints(QGraphicsBlurEffect::PerformanceHint);
        e->setBlurRadius(2.5);
        setGraphicsEffect(e);
      }
    } break;
  case DefaultEffect:
  default:
    {
      BOOST_AUTO(e, dynamic_cast<AnnotationGraphicsEffect *>(graphicsEffect()));
      if (!e) {
        e = new AnnotationGraphicsEffect;
        setGraphicsEffect(e);
      }
    }
  }
}

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

  if (hash == AnnotCloud::H_Verbatim)
    return text;

  else if (hash == AnnotCloud::H_Float) {
    setStyle(Float);
    return SELF(left);
  } else if (hash == AnnotCloud::H_Top) {
    setStyle(Top);
    return SELF(left);
  } else if (hash == AnnotCloud::H_Bottom) {
    setStyle(Bottom);
    return SELF(left);
  }

  else if (hash == AnnotCloud::H_Em || hash == AnnotCloud::H_Emph || hash == AnnotCloud::H_TextIt)
    return HTML_EM(SELF(left));
  else if (hash == AnnotCloud::H_Bf || hash == AnnotCloud::H_Strong || hash == AnnotCloud::H_TextBf)
    return HTML_STRONG(SELF(left));

  else if (hash == AnnotCloud::H_LineThrough
           || hash == AnnotCloud::H_Strike
           || hash == AnnotCloud::H_StrikeOut
           || hash == AnnotCloud::H_Sout)
    return HTML_STYLE(SELF(left), text-decoration:line-through);
  else if (hash == AnnotCloud::H_Underline
           || hash == AnnotCloud::H_Uline)
    return HTML_STYLE(SELF(left), text-decoration:underline);
  else if (hash == AnnotCloud::H_Overline)
    return HTML_STYLE(SELF(left), text-decoration:overline);
  else if (hash == AnnotCloud::H_Blink)
    return HTML_STYLE(SELF(left), text-decoration:blink);

#define ELIF_TRANSFORM(_trans) \
  else if (hash == AnnotCloud::H_##_trans) \
    return HTML_STYLE(SELF(left), text-transform:_trans); \

  ELIF_TRANSFORM(Uppercase)
  ELIF_TRANSFORM(Lowercase)
#undef ELIF_TRANSFORM

#define ELIF_COLOR(_color) \
  else if (hash == (AnnotCloud::H_##_color)) \
    return HTML_STYLE(SELF(left), color:_color);

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
  else if (hash == (AnnotCloud::H_##_id)) \
    return HTML_STYLE(SELF(left), font-size:_size);

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
  connect(view_, SIGNAL(itemVisibleChanged(bool)), SLOT(setVisible(bool)));
  connect(view_, SIGNAL(paused()), SLOT(pause()));
  connect(view_, SIGNAL(resumed()), SLOT(resume()));
  connect(view_, SIGNAL(scaleChanged(qreal)), SLOT(setScale(qreal)));

  if (style_ == SubtitleStyle &&
      hub_->isSignalTokenMode() &&
      !hub_->isStopped())
    connect(view_, SIGNAL(removeItemRequested()), SLOT(removeMe()));

  setVisible(view_->isItemVisible());
  scene_->addItem(this);
  autoDelete_ = false;
}

void
AnnotationGraphicsItem::disappear()
{
  enum { timeout = 1000 };

  switch (style_) {
  case TopStyle:
  case BottomStyle:
    //removeMe();
    fadeOut(timeout);
    removeLater(timeout);
    break;
  default:
    fadeOut(timeout);
    removeLater(timeout);
  }
}

void
AnnotationGraphicsItem::removeMe()
{
  // Always try to disconnect to avoid segmentation fault
  //if (style_ == SubtitleStyle &&
  //    hub_->isSignalTokenMode() &&
  //    !hub_->isStopped())
  if (hub_->isSignalTokenMode())
    disconnect(view_, SIGNAL(removeItemRequested()), this, SLOT(removeMe()));
  disconnect(view_, SIGNAL(itemVisibleChanged(bool)), this, SLOT(setVisible(bool)));
  disconnect(view_, SIGNAL(paused()), this, SLOT(pause()));
  disconnect(view_, SIGNAL(resumed()), this, SLOT(resume()));

  if (scene() == scene_)
    scene_->removeItem(this);
  view_->releaseItem(this);
  autoDelete_ = true;
  //QTimer::singleShot(0, this, SLOT(deleteLater()));
}

void
AnnotationGraphicsItem::deleteMe()
{
  if (annot_.hasId())
    view_->removeAnnotationWithId(annot_.id(), true); // deleteAnnot = true
  else
    disappear();
}

void
AnnotationGraphicsItem::selectMe()
{ view_->selectItem(this); }

void
AnnotationGraphicsItem::showMe()
{
  switch (style_) {
  case FloatStyle:
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
AnnotationGraphicsItem::removeLater(int msecs)
{
  if (!removeLaterTimer_) {
    removeLaterTimer_ = new QTimer(this);
    removeLaterTimer_->setSingleShot(true);
    connect(removeLaterTimer_, SIGNAL(timeout()), SLOT(removeMe()));
  }
  removeLaterTimer_->start(msecs);
}

// - Show up -

bool
AnnotationGraphicsItem::isPaused() const
{
  if (style_ == SubtitleStyle &&
      hub_->isSignalTokenMode() &&
      !hub_->isStopped())
    return false;

  switch (style_) {
  case FloatStyle:
  case FlyStyle:
    return flyOpacityAni_ && flyOpacityAni_->state() == QAbstractAnimation::Paused;

  case TopStyle:
  case BottomStyle:
    return appearOpacityAni_ && appearOpacityAni_->state() == QAbstractAnimation::Paused;

  case SubtitleStyle:
    return removeLaterTimer_ && !removeLaterTimer_->isActive();
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

  if (fadeAni_ && fadeAni_->state() == QAbstractAnimation::Running)
    fadeAni_->pause();

  switch (style_) {
  case FloatStyle:
  case FlyStyle:
    if (fadeAni_ && fadeAni_->state() == QAbstractAnimation::Running)
      fadeAni_->pause();
    if (flyOpacityAni_ && flyOpacityAni_->state() == QAbstractAnimation::Running)
      flyOpacityAni_->pause();
    if (flyAni_ && flyAni_->state() == QAbstractAnimation::Running) {
      origin_ = relativePos();
      flyAni_->pause();
    } break;

  case TopStyle:
  case BottomStyle:
    if (appearOpacityAni_ && appearOpacityAni_->state() == QAbstractAnimation::Running)
      appearOpacityAni_->pause();
    break;

  case SubtitleStyle:
    if (removeLaterTimer_ && removeLaterTimer_->isActive())
      removeLaterTimer_->stop();
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

  if (fadeAni_ && fadeAni_->state() == QAbstractAnimation::Paused)
    fadeAni_->resume();

  switch (style_) {
  case FloatStyle:
  case FlyStyle:
    if (flyOpacityAni_ && flyOpacityAni_->state() == QAbstractAnimation::Paused)
      flyOpacityAni_->resume();
    if (flyAni_ && flyAni_->state() == QAbstractAnimation::Paused) {
      origin_ = relativePos();
      flyAni_->resume();
    } break;

  case TopStyle:
  case BottomStyle:
    if (appearOpacityAni_ && appearOpacityAni_->state() == QAbstractAnimation::Paused)
      appearOpacityAni_->resume();
    break;

  case SubtitleStyle:
    if (!removeLaterTimer_ || !removeLaterTimer_->isActive()) {
      //int timeout = stayTime(style_);
      enum { timeout = 2000 };
      removeLater(timeout);
    }
    break;
  }
}

int
AnnotationGraphicsItem::stayTime(Style style) const
{
  int t = style == SubtitleStyle ? ANNOTATION_STAY_TIME_SUBTITLE
                                 : ANNOTATION_STAY_TIME;
  int w0 = qMax(view_->width(), 100),
      w = qMax(int(boundingRect().width()), 50),
      h = qMax(int(boundingRect().height()), 20);
  qreal f = qreal(w0 + 200)/ (w + 200),
        g = qreal(h + 20) / (ANNOTATION_SIZE_DEFAULT + 15);
  qreal q = hub_->isSignalTokenMode() ? 1.0 : 0.8;
  int ret = t * ::pow(f, 0.3)* g * q + ANNOTATION_STAY_TIME_MIN;
  return qMin(ret, ANNOTATION_STAY_TIME_MAX);
}

int
AnnotationGraphicsItem::flyTime() const
{
  int w0 = qMax(view_->width(), 100),
      w = qMax(int(boundingRect().width()), 50);
  qreal f = qreal(w0 + 200) / (w + 200);
  int ret = ANNOTATION_FLY_TIME * ::pow(f, 0.2) + ANNOTATION_FLY_TIME_MIN;
  ret = qMin(ret, ANNOTATION_FLY_TIME_MAX);
  if (style_ == FlyStyle) {
    ret /= 5;
    if (view_->width() > 640)
      ret = ret * view_->width() / 640;
  }
  return qMax(ret, ANNOTATION_FLY_TIME_MIN);
}

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
      y = nextY(msecs, posStyle);

  if (style_ == SubtitleStyle &&
      hub_->isSignalTokenMode() &&
      !hub_->isStopped())
    msecs = -1;
  if (style == SubtitleStyle || msecs <= 0)
    stay(QPointF(x, y), msecs);
  else
    appear(QPointF(x, y), msecs);
}

void
AnnotationGraphicsItem::stay(const QPointF &pos, int msecs)
{
  updateEffect();
  addMe();
  setPos(pos);
  if (hub_->isMediaTokenMode() ||
      !hub_->isStopped() && msecs >= 0)
    removeLater(msecs);
}

void
AnnotationGraphicsItem::appear(const QPointF &pos, int msecs)
{
  if (!appearOpacityAni_) {
    appearOpacityAni_ = new QPropertyAnimation(this, "opacity");
    QEasingCurve curve;
    curve.setCustomType(curve_::appearOpacity);
    appearOpacityAni_->setEasingCurve(curve);
    connect(appearOpacityAni_, SIGNAL(finished()), SLOT(removeMe()));
  } else if (appearOpacityAni_->state() != QAbstractAnimation::Stopped)
    appearOpacityAni_->stop();
  setOpacity(0);
  addMe();
  setPos(pos);
  appearOpacityAni_->setDuration(msecs);
  appearOpacityAni_->setStartValue(0.0);
  appearOpacityAni_->setEndValue(1.0);
  appearOpacityAni_->start();
}

void
AnnotationGraphicsItem::fadeIn(int msecs)
{
  if (!fadeAni_) {
    fadeAni_ = new QPropertyAnimation(this, "opacity");
    fadeAni_->setEasingCurve(QEasingCurve::Linear);
  } else if (fadeAni_->state() != QAbstractAnimation::Stopped)
    fadeAni_->stop();
  fadeAni_->setDuration(msecs);
  fadeAni_->setStartValue(0.0);
  fadeAni_->setEndValue(1.0);
  fadeAni_->start();
}

void
AnnotationGraphicsItem::fadeOut(int msecs)
{
  if (!fadeAni_) {
    fadeAni_ = new QPropertyAnimation(this, "opacity");
    fadeAni_->setEasingCurve(QEasingCurve::Linear);
  } else if (fadeAni_->state() != QAbstractAnimation::Stopped)
    fadeAni_->stop();
  fadeAni_->setDuration(msecs);
  fadeAni_->setStartValue(0.99);
  fadeAni_->setEndValue(0.0);
  fadeAni_->start();
}

void
AnnotationGraphicsItem::fly()
{
  Q_ASSERT(view_);
  int msecs = flyTime();
  int y = nextY(msecs, style_);

  QPoint from(view_->width(), y);
  QPoint to(- boundingRect().width(), y);
  fly(from, to, msecs);
}

void
AnnotationGraphicsItem::fly(const QPointF &from, const QPointF &to, int msecs)
{
  Q_ASSERT(msecs > 0);
  if (!flyAni_) {
    flyAni_ = new QPropertyAnimation(this, "relativePos");
    flyAni_->setEasingCurve(QEasingCurve::Linear);
  } else if (flyAni_->state() != QAbstractAnimation::Stopped)
    flyAni_->stop();
  if (!flyOpacityAni_) {
    flyOpacityAni_ = new QPropertyAnimation(this, "opacity");
    QEasingCurve curve;
    curve.setCustomType(curve_::flyOpacity);
    flyOpacityAni_->setEasingCurve(curve);
    connect(flyOpacityAni_, SIGNAL(finished()), SLOT(removeMe()));
  } else if (flyOpacityAni_->state() != QAbstractAnimation::Stopped)
    flyOpacityAni_->stop();

  setOpacity(0);
  addMe();
  origin_ = from;
  flyAni_->setDuration(msecs);
  flyAni_->setStartValue(QPointF());
  flyAni_->setEndValue(to - from);

  flyOpacityAni_->setDuration(msecs);
  flyOpacityAni_->setStartValue(0.0);
  flyOpacityAni_->setEndValue(1.0);

  flyAni_->start();
  flyOpacityAni_->start();
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

    QMenu *m = new QMenu(view_);
    AcUi::globalInstance()->setContextMenuStyle(m, false); // persistent = false

    if (!hub_->isLiveTokenMode())
      m->addAction(TR(T_MENUTEXT_EDIT), this, SLOT(edit()));
    m->addAction(TR(T_MENUTEXT_COPY), this, SLOT(copyToClipboard()));
    m->addAction(TR(T_MENUTEXT_REMOVEANNOTATION), this, SLOT(disappear()));
    m->addSeparator();

    if (annot_.userId() == view_->userId()) {
      if (annot_.hasId() && !hub_->isLiveTokenMode())
        m->addAction(TR(T_MENUTEXT_DELETETHISANNOT), this, SLOT(deleteMe()));

    } else {
      QString text = abstract();
      if (annot_.id() > 0 && !hub_->isLiveTokenMode()) {
        m->addAction(TR(T_BLESS) + ": " + text, this, SLOT(blessMe()));
        m->addAction(TR(T_CURSE) + ": " + text, this, SLOT(curseMe()));
      }
      m->addAction(TR(T_BLOCK) + ": " + text, this, SLOT(blockMe()));
      if (annot_.hasUserAlias()) {
        m->addSeparator();
        m->addAction(TR(T_MENUTEXT_BLOCKUSER) + ": " + annot_.userAlias(), this, SLOT(blockUser()));
      }
    }

    m->exec(event->globalPos());
    delete m;
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
  if (dragPaused_ && isPaused())
    resume();
}

void
AnnotationGraphicsItem::mousePressEvent(QMouseEvent *event)
{
  dragPaused_ = isPaused();
  if (!dragPaused_) {
    pause();
    selectMe();
  }
  if (event && event->button() == Qt::LeftButton && dragPos_ == BAD_POS)
    dragPos_ = event->globalPos() -  QPoint(scenePos().x(), scenePos().y());
}

void
AnnotationGraphicsItem::mouseReleaseEvent(QMouseEvent *event)
{
  Q_UNUSED(event);
  dragPos_ = BAD_POS;
  //if (dragPaused_ != isPaused()) {
  //  if (dragPaused_)
  //    pause();
  //  else
  //    resume();
  //}
}

void
AnnotationGraphicsItem::mouseMoveEvent(QMouseEvent *event)
{
  if (event && (event->buttons() & Qt::LeftButton) && dragPos_ != BAD_POS) {
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
    clipboard->setText(annot_.text());
    log(TR(T_SUCCEED_COPIED) + ": " + annot_.text());
  } else
    warn(TR(T_ERROR_CLIPBOARD_UNAVAILABLE));
}

void
AnnotationGraphicsItem::edit()
{
  if (annot_.userId() != view_->userId()) {
    warn(tr("cannot edit other's annotation text"));
    return;
  }

  view_->editor()->setText(annot_.text());
  view_->editor()->setId(annot_.id());
  view_->editor()->show();
}

void
AnnotationGraphicsItem::blessMe()
{
  if (annot_.id() > 0 && !hub_->isLiveTokenMode())
    view_->blessAnnotationWithId(annot_.id());
  if (annot_.userId() > 0)
    view_->blessUserWithId(annot_.userId());
}

void
AnnotationGraphicsItem::curseMe()
{
  if (annot_.id() > 0 && !hub_->isLiveTokenMode())
    view_->curseAnnotationWithId(annot_.id());
  if (annot_.userId() > 0)
    view_->curseUserWithId(annot_.userId());
}

void
AnnotationGraphicsItem::blockMe()
{
  hide();
  if (annot_.hasText())
    view_->blockAnnotationWithText(annot_.text());
  if (annot_.id() > 0 && !hub_->isLiveTokenMode())
    view_->blockAnnotationWithId(annot_.id());
}

void
AnnotationGraphicsItem::blockUser()
{
  hide();
  if (annot_.hasUserAlias())
    view_->blockUserWithAlias(annot_.userAlias());
  if (annot_.hasUserId())
    view_->blockUserWithId(annot_.userId());
}

QString
AnnotationGraphicsItem::abstract() const
{
  enum { length = 11 } ;
  QString ret = annot_.text();
  if (ret.size() > length)
    ret = ret.left(length - 6) + "..." + ret.right(3);
  return ret;
}

// - Escape -

void
AnnotationGraphicsItem::escapeFrom(const QPointF &from)
{
  //enum { rmin = 300, rmax = 600 };
  enum { rmin = 200, rmax = 400 }; // radius

  qreal x1, y1, x2, y2; {
    QRectF r = boundingRect();
    r.moveTo(pos());
    r.getCoords(&x1, &y1, &x2, &y2);
  }
  // 1 2 3
  // 4 5 6
  // 7 8 9
  QPointF d =
    from.x() < x1 && from.y() < y1 ? QPointF(x1, y1)               : // 1
    from.x() < x2 && from.y() < y1 ? QPointF((x1+x2)/2, y1)        : // 2
                     from.y() < y1 ? QPointF(x2, y1)               : // 3
    from.x() < x1 && from.y() < y2 ? QPointF(x1, (y1+y2)/2)        : // 4
    from.x() < x2 && from.y() < y2 ? QPointF((x1+x2)/2, (y1+y2)/2) : // 5
                     from.y() < y2 ? QPointF(x2, (y1+y2)/2)        : // 6
    from.x() < x1                  ? QPointF(x1, y2)               : // 7
    from.x() < x2                  ? QPointF((x1+x2)/2, y2)        : // 8
                                     QPointF(x2, y2)               ; // 9
  d -= from;

  qreal len = ::sqrt(d.x()*d.x() + d.y()*d.y());
  if (len < 0.01) {
    d.rx() += rmax;
    len = rmax;
  }
  else {
    qreal l = rmin + (rmax - rmin) /(len+1);
    d *= l/len;
    len = l;
  }
  QPointF to = pos() + d;

  qreal v = 1/(len/30.0 + 1) + 0.02;
  int msecs = qMax(int(len / v), 100);
  escapeTo(to, msecs);
}

void
AnnotationGraphicsItem::escapeTo(const QPointF &to, int msecs)
{
  if (!isPaused())
    pause();
  if (rushAni_ && rushAni_->state() != QAbstractAnimation::Stopped)
    rushAni_->stop();
  if (!escapeAni_) {
    escapeAni_ = new QPropertyAnimation(this, "pos");
    //escapeAni_->setEasingCurve(QEasingCurve::OutInCubic);
    escapeAni_->setEasingCurve(QEasingCurve::OutQuart);
    connect(escapeAni_, SIGNAL(finished()), SLOT(resume()));
  } else if (escapeAni_->state() != QAbstractAnimation::Stopped)
    escapeAni_->stop();

  escapeAni_->setDuration(msecs);
  escapeAni_->setStartValue(pos());
  escapeAni_->setEndValue(to);
  escapeAni_->start();
}

void
AnnotationGraphicsItem::rushTo(const QPointF &to)
{

  QPointF now = boundingRect().center();
  QPointF d = now - to;
  qreal len = ::sqrt(d.x()*d.x() + d.y()*d.y());
  if (len < 0.01)
    return;

  qreal v = 1/(len/60.0 + 1) + 0.1;
  int msecs = qMax(int(len / v), 100);
  rushTo(to, msecs);
}

void
AnnotationGraphicsItem::rushTo(const QPointF &to, int msecs)
{
  if (!isPaused())
    pause();
  if (escapeAni_ && escapeAni_->state() != QAbstractAnimation::Stopped)
    escapeAni_->stop();
  if (!rushAni_) {
    rushAni_ = new QPropertyAnimation(this, "pos");
    rushAni_->setEasingCurve(QEasingCurve::OutQuart);
  } else if (rushAni_->state() != QAbstractAnimation::Stopped)
    rushAni_->stop();

  rushAni_->setDuration(msecs);
  rushAni_->setStartValue(pos());
  rushAni_->setEndValue(to);
  rushAni_->start();
}

// EOF
