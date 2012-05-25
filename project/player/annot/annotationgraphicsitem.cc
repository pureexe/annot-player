// annotationgraphicsitem.cc
// 7/16/2011

#include "annotationgraphicsitem.h"
#include "annotationgraphicsview.h"
#include "annotationgraphicsstyle.h"
#include "annotationgraphicseffect.h"
#include "annotationsettings.h"
#include "annotationeditor.h"
#include "datamanager.h"
//#include "textformathandler.h"
#include "tr.h"
#include "global.h"
#include "logger.h"
#include "signalhub.h"
#include "project/common/acui.h"
#include "module/annotcloud/annottag.h"
#include "module/annotcloud/annothtml.h"
#include "module/qtext/filesystem.h"
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

#define BAD_POSF        QPointF(-1,-1)
namespace { inline bool isBadPosF(const QPointF &pos) { return pos.x() < 0 || pos.y() < 0; } }

// - RC -

#ifdef Q_WS_LINUX
#  define RC_AVATAR_PREFIX      AVATARDIR
#else
#  define RC_AVATAR_PREFIX      QCoreApplication::applicationDirPath() + "/avatars"
#endif // Q_WS_LINUX
#define RC_AVATAR_COUNT         AVATAR_USER_COUNT
#define AVATAR_SIZE             "40"

namespace { // anonymous
  inline QString rc_avatar_url(qint64 i)
  {
    static QString fmt = RC_AVATAR_PREFIX "/user_%1.jpg";
    return fmt.arg(QString::number(qAbs(i) % RC_AVATAR_COUNT));
  }
} // anonymous namespace

// - Helpers -

#define ANNOTATION_LIFE_SCHEDULE   ((ANNOTATION_LIFE_ViSIBLE + 1) / 4)
#define OPACITY         AnnotationGraphicsEffect::defaultOpacity()

namespace { namespace curve_ { // anonymous, curves

  inline qreal linear_(qreal progress, qreal start, qreal stop)
  {
    Q_ASSERT(0 <= start && start <= stop && stop <= 1.0);
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
  { return linear_(progress, 0.2, 0.80); }

  qreal NOINLINE flyOpacity(qreal progress)
  { return linear_(progress, 0.13, 0.80); }

} } // anonymous namespace

namespace { // anonymous, annotation display

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
#ifdef Q_OS_MAC
  QGraphicsTextItem dummy;

  QFont font = AnnotationSettings::globalInstance()->font();
  font.setWeight(QFont::Light); dummy.setFont(font);
  font.setWeight(QFont::Normal); dummy.setFont(font);
  font.setWeight(QFont::DemiBold); dummy.setFont(font);
  font.setWeight(QFont::Bold); dummy.setFont(font);
  font.setWeight(QFont::Black); dummy.setFont(font);
#endif // Q_OS_MAC
}

AnnotationGraphicsItem::AnnotationGraphicsItem(
  DataManager *data,
  SignalHub *hub,
  AnnotationGraphicsView *view)
  : metaVisible_(false),
    view_(view), data_(data), hub_(hub), style_(FloatStyle),
    removeLaterTimer_(0),
    flyAni_(0), flyOpacityAni_(0), escapeAni_(0), rushAni_(0), appearOpacityAni_(0), fadeAni_(0),
    dragPos_(BAD_POSF)
{
  DOUT("enter");
  Q_ASSERT(data_);
  Q_ASSERT(hub_);
  Q_ASSERT(view_);
  scene_ = view_->scene();
  Q_ASSERT(scene_);

  setAcceptHoverEvents(true);
  //setAcceptTouchEvents(true);

  setScale(view_->scale());
  setGraphicsEffect(effect_ = new AnnotationGraphicsEffect);

  DOUT("exit");
}

void
AnnotationGraphicsItem::setAnnotation(const Annotation &annot)
{
  if (!prefix_.isEmpty())
    prefix_.clear();
  if (!suffix_.isEmpty())
    suffix_.clear();
  annot_ = annot;
  setToolTip(QString());
  updateText();
  //updateToolTip();
}

void
AnnotationGraphicsItem::reset()
{
  if (removeLaterTimer_ && removeLaterTimer_->isActive())
    removeLaterTimer_->stop();
  if (flyAni_ && flyAni_->state() != QAbstractAnimation::Stopped)
    flyAni_->stop();
  if (flyOpacityAni_ && flyOpacityAni_->state() != QAbstractAnimation::Stopped)
    flyOpacityAni_->stop();
  if (escapeAni_ && escapeAni_->state() != QAbstractAnimation::Stopped)
    escapeAni_->stop();
  if (rushAni_ && rushAni_->state() != QAbstractAnimation::Stopped)
    rushAni_->stop();
  if (fadeAni_ && fadeAni_->state() != QAbstractAnimation::Stopped)
    fadeAni_->stop();
  if (appearOpacityAni_ && appearOpacityAni_->state() != QAbstractAnimation::Stopped)
    appearOpacityAni_->stop();
  setOpacity(0.0);
}

void
AnnotationGraphicsItem::setMetaVisibleAndUpdate(bool t)
{
  if (isMetaVisible() != t) {
    metaVisible_ = t;
    if (isMetaVisible()) {
      if (suffix_.isEmpty()) {
        updateMeta();
        richText_ = prefix_ + text_ + suffix_;
      }
      setText(richText_);
    } else
      setText(text_);
  }
}

bool
AnnotationGraphicsItem::isMetaVisible() const
{
  return annot_.hasUserId() && (
    metaVisible_ ||
    hub_->isSignalTokenMode() && !isSubtitle()
  );
}

void
AnnotationGraphicsItem::updateText()
{
  setDefaultStyle();
  //updateEffect();

  // TODO: condition "isMediaTokenMode" is to be removed
  //bool isOwner = view_->userId() &&
  //               view_->userId() != User::UI_Guest &&
  //               annot_.userId() == view_->userId();
  bool isOwner = view_->userId() &&
                 (view_->userId() != User::UI_Guest || hub_->isMediaTokenMode()) &&
                 annot_.userId() == view_->userId();

  QFont font = AnnotationSettings::globalInstance()->font();
  if (isOwner)
    font.setUnderline(true);
  setFont(font);

  QString text = annot_.text();
  if (annot_.isSubtitle())
    text = view_->subtitlePrefix() + text;

  QStringList tags;
  boost::tie(text_, tags) = ANNOT_PARSE_CODE(text);
  setTags(tags);
  richText_ = text_;
  if (tags.contains(CORE_CMD_VERBATIM))
    setPlainText(richText_);
  else if (isMetaVisible()) {
    if (suffix_.isEmpty())
      updateMeta();
    richText_ = prefix_ + text_ + suffix_;
    setText(richText_);
  } else
    setText(text_);
  //if (isOwner)
  //  richText_ = CORE_CMD_LATEX_ULINE " " + richText_;
}

void
AnnotationGraphicsItem::updateToolTip()
{
  QString tip;
  qint64 t = annot_.createTime();
  if (t > Traits::MIN_TIME && t < Traits::MAX_TIME) {
    t *= 1000;
    QDateTime ts = QDateTime::fromMSecsSinceEpoch(t);
    QString d = ts.toString("M/d/yyyy");
    QString w;
    switch (ts.date().dayOfWeek()) {
    case 1: w = tr("Mon"); break;
    case 2: w = tr("Tue"); break;
    case 3: w = tr("Wed"); break;
    case 4: w = tr("Thu"); break;
    case 5: w = tr("Fri"); break;
    case 6: w = tr("Sat"); break;
    case 7: w = tr("Sun"); break;
    default: Q_ASSERT(0);
    }

    QString h = ts.toString("h:mm");
    QString s = d + " " + w + " " + h;
    if (tip.isEmpty())
      tip = s;
    else
      tip.append(" " + s);
  }
  qint64 uid = annot_.userId();
  if (uid) {
    int count = data_->annotationCountForUserId(uid);
    if (count > 1) {
      QString c = QString::number(count);
      QString s = "x" + c;
      if (tip.isEmpty())
        tip = s;
      else
        tip.append(" " + s);
    }
  }
  QString u = annot_.userAlias();
  if (!u.isEmpty()) {
    QString s = "@" + u;
    if (tip.isEmpty())
      tip = s;
    else
      tip.append(" " + s);
  }
  setToolTip(tip);
}

void
AnnotationGraphicsItem::updateMeta()
{
  QString ret;
  qint64 t = annot_.createTime();
  if (t > Traits::MIN_TIME && t < Traits::MAX_TIME) {
    t *= 1000;
    QString ts;

    //QDateTime dt0 = QDateTime::currentDateTime();
    //QTime t0 = dt0.time();
    QDate d0 = QDate::currentDate();

    QDateTime dt1 = QDateTime::fromMSecsSinceEpoch(t);
    QDate d1 = dt1.date();
    QTime t1 = dt1.time();

    int days = d1.daysTo(d0);
    if (days > 1) {
      QString ds =
        days < 0     ? d1.toString("M/d/yyyy") :
        days < 15    ? QString::number(days) + tr("days") :
        days < 7*5   ? QString::number(days/7) + tr("weeks") :
        days < 30*12 ? QString::number((12+d0.month()-d1.month()) % 12) + tr("months") :
                       d1.toString("M/d/yyyy");
      ts.append(HTML_STYLE_OPEN(color:orange) + ds + HTML_STYLE_CLOSE());
      QString w;
      switch (d1.dayOfWeek()) {
      case 1: w = tr("Mon"); break;
      case 2: w = tr("Tue"); break;
      case 3: w = tr("Wed"); break;
      case 4: w = tr("Thu"); break;
      case 5: w = tr("Fri"); break;
      case 6: w = tr("Sat"); break;
      case 7: w = tr("Sun"); break;
      default: Q_ASSERT(0);
      }
      ts.append(HTML_STYLE_OPEN(font-size:12px;color:magenta) + w + HTML_STYLE_CLOSE());

      ts.append(t1.toString("h:mm"));
    } else {
      QTime t0 = QTime::currentTime();
      int d;
      if (days == 1)
        ts.append(QString::number(24 + t1.hour() - t0.hour()))
          .append(tr("hr"));
      else if (d = t1.hour() - t0.hour())
        ts.append(QString::number((24 + d) % 24))
          .append(tr("hr"));
      else if (d = t1.minute() - t0.minute())
        ts.append(QString::number((60 + d) % 60))
          .append(tr("min"));
      else if (d = t1.second() - t0.second())
        ts.append(QString::number((60 + d) % 60))
          .append(tr("sec"));

    }
    if (!ts.isEmpty()) {
      ts = HTML_STYLE_OPEN(color:yellow)
          + ts +
          HTML_STYLE_CLOSE();
      ret.append(ts);
    }
  }
  QString u = annot_.userAlias();
  if (!u.isEmpty()) {
    enum { user_len = 4 };
    u = HTML_STYLE_OPEN(color:cyan)
        "@" + u.right(user_len) +
        HTML_STYLE_CLOSE();
    ret.append(u);
  }
  QString avatar;
  qint64 uid = annot_.userId();
  if (uid) {
    int count = data_->annotationCountForUserId(uid);
    if (count > 1) {
      QString c = QString::number(count);
      c = HTML_STYLE_OPEN(font-size:11px;color:red) "x" + c + HTML_STYLE_CLOSE();
      ret.append(c);

      avatar =
        "<img"
        " src=\"" + ::rc_avatar_url(uid) + "\""
        " alt=\"" + annot_.userAlias() + "\""
        " border=\"0\""
        " width=\"" AVATAR_SIZE "\""
        " height=\"" AVATAR_SIZE "\""
        "/> "; // a trailing space at the end
    }
  }
  if (!ret.isEmpty())
    ret = HTML_STYLE_OPEN(text-decoration:underline;font-size:11px) + ret + HTML_STYLE_CLOSE();
  prefix_ = avatar;
  suffix_ = ret;
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
      //case AnnotCloud::H_Transparent: setEffect(TransparentEffect); break;
      //case AnnotCloud::H_Shadow: setEffect(ShadowEffect); break;
      //case AnnotCloud::H_Blur: setEffect(BlurEffect); break;

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

  //setToolTip(TR(T_TOOLTIP_ANNOTATIONITEM));
  setDefaultTextColor(ANNOTATION_COLOR_DEFAULT);
}

qreal
AnnotationGraphicsItem::opacity() const
{ return effect_->opacity(); }

void
AnnotationGraphicsItem::setOpacity(qreal opacity)
{
  //qreal offset = qFuzzyCompare(opacity, OPACITY) ? 1.0 : 0.0;
  //effect_->setOffset(offset);
  effect_->setOpacity(opacity);
}

//void
//AnnotationGraphicsItem::updateOpacity()
//{
//  if (qFuzzyCompare(opacity_, 1))
//    updateEffect();
//  else {
//    BOOST_AUTO(e, qobject_cast<QGraphicsOpacityEffect *>(graphicsEffect()));
//    if (!e)
//      e = new QGraphicsOpacityEffect;
//    e->setOpacity(opacity_);
//    if (e != graphicsEffect())
//      setGraphicsEffect(e);
//  }
//}

//void
//AnnotationGraphicsItem::updateEffect()
//{
//  Effect e;
//  switch (view_->renderHint()) {
//  case AnnotationGraphicsView::TransparentHint: e = TransparentEffect; break;
//  case AnnotationGraphicsView::ShadowHint: e = ShadowEffect; break;
//  case AnnotationGraphicsView::BlurHint: e = BlurEffect; break;
//  case AnnotationGraphicsView::DefaultRenderHint:
//  default: e = DefaultEffect;
//  }
//  setEffect(e);
//}

//void
//AnnotationGraphicsItem::setEffect(Effect e)
//{
//  switch (e) {
//  case ShadowEffect:
//    {
//#ifdef Q_WS_WIN
//      enum { offset = 1, radius = 18 };
//#else
//      enum { offset = 1, radius = 12 };
//#endif // Q_WS_WIN
//      BOOST_AUTO(e, qobject_cast<QGraphicsDropShadowEffect *>(graphicsEffect()));
//      if (!e) {
//        e = new QGraphicsDropShadowEffect;
//        e->setBlurRadius(radius); // in pixels
//        e->setOffset(offset); // in pixels
//        e->setColor(Qt::black);
//        setGraphicsEffect(e);
//      }
//    } break;
//  case TransparentEffect:
//    {
//      BOOST_AUTO(e, qobject_cast<QGraphicsOpacityEffect *>(graphicsEffect()));
//      if (!e) {
//        e = new QGraphicsOpacityEffect;
//        e->setOpacity(ANNOTATION_OPACITY);
//        setGraphicsEffect(e);
//      }
//    } break;
//  case BlurEffect:
//    {
//      BOOST_AUTO(e, qobject_cast<QGraphicsBlurEffect *>(graphicsEffect()));
//      if (!e) {
//        e = new QGraphicsBlurEffect;
//        e->setBlurHints(QGraphicsBlurEffect::PerformanceHint);
//        e->setBlurRadius(2.5);
//        setGraphicsEffect(e);
//      }
//    } break;
//  case DefaultEffect:
//  default:
//    {
//      BOOST_AUTO(e, dynamic_cast<AnnotationGraphicsEffect *>(graphicsEffect()));
//      if (!e) {
//        e = new AnnotationGraphicsEffect;
//        setGraphicsEffect(e);
//      }
//    }
//  }
//}
//

//QString
//AnnotationGraphicsItem::parse(const QString &input)
//{
//#define SELF(_text) parse(_text)
//  QString text = input.trimmed();
//  if (text.isEmpty())
//    return QString();
//
//  if (text[0] != CORE_CMDCH)
//    return text;
//
//  QString tag, left;
//  boost::tie(tag, left) = Core::parseLeadingTag(text);
//  uint hash = qHash(tag);
//
//  if (hash == AnnotCloud::H_Verbatim)
//    return text;
//
//  else if (hash == AnnotCloud::H_Float) {
//    setStyle(Float);
//    return SELF(left);
//  } else if (hash == AnnotCloud::H_Top) {
//    setStyle(Top);
//    return SELF(left);
//  } else if (hash == AnnotCloud::H_Bottom) {
//    setStyle(Bottom);
//    return SELF(left);
//  }
//
//  else if (hash == AnnotCloud::H_Em || hash == AnnotCloud::H_Emph || hash == AnnotCloud::H_TextIt)
//    return HTML_EM(SELF(left));
//  else if (hash == AnnotCloud::H_Bf || hash == AnnotCloud::H_Strong || hash == AnnotCloud::H_TextBf)
//    return HTML_STRONG(SELF(left));
//
//  else if (hash == AnnotCloud::H_LineThrough
//           || hash == AnnotCloud::H_Strike
//           || hash == AnnotCloud::H_StrikeOut
//           || hash == AnnotCloud::H_Sout)
//    return HTML_STYLE(SELF(left), text-decoration:line-through);
//  else if (hash == AnnotCloud::H_Underline
//           || hash == AnnotCloud::H_Uline)
//    return HTML_STYLE(SELF(left), text-decoration:underline);
//  else if (hash == AnnotCloud::H_Overline)
//    return HTML_STYLE(SELF(left), text-decoration:overline);
//  else if (hash == AnnotCloud::H_Blink)
//    return HTML_STYLE(SELF(left), text-decoration:blink);
//
//#define ELIF_TRANSFORM(_trans)
//  else if (hash == AnnotCloud::H_##_trans)
//    return HTML_STYLE(SELF(left), text-transform:_trans);
//
//  ELIF_TRANSFORM(Uppercase)
//  ELIF_TRANSFORM(Lowercase)
//#undef ELIF_TRANSFORM
//
//#define ELIF_COLOR(_color)
//  else if (hash == (AnnotCloud::H_##_color))
//    return HTML_STYLE(SELF(left), color:_color);
//
//  ELIF_COLOR(Black)
//  ELIF_COLOR(Blue)
//  ELIF_COLOR(Brown)
//  ELIF_COLOR(Cyan)
//  ELIF_COLOR(Green)
//  ELIF_COLOR(Gray)
//  ELIF_COLOR(Grey)
//  ELIF_COLOR(Magenta)
//  ELIF_COLOR(Orange)
//  ELIF_COLOR(Pink)
//  ELIF_COLOR(Purple)
//  ELIF_COLOR(Red)
//  ELIF_COLOR(White)
//  ELIF_COLOR(Yellow)
//#undef ELIF_COLOR
//
//#define ELIF_SIZE(_id, _size)
//  else if (hash == (AnnotCloud::H_##_id))
//    return HTML_STYLE(SELF(left), font-size:_size);
//
//  ELIF_SIZE(Tiny,       ANNOTATION_SIZE_TINY)
//  ELIF_SIZE(Small,      ANNOTATION_SIZE_SMALL)
//  ELIF_SIZE(Normal,     ANNOTATION_SIZE_NORMAL)
//  ELIF_SIZE(Large,      ANNOTATION_SIZE_LARGE)
//  ELIF_SIZE(Huge,       ANNOTATION_SIZE_HUGE)
//#undef ELIF_SIZE
//
//  else {
//    warn(TR(T_ERROR_UNKNOWN_COMMAND) + ": " + text);
//    return text;
//  }
//#undef SELF
//}

// - Add/remove me -

void
AnnotationGraphicsItem::addMe()
{
  if (!scene()) {
    connect(view_, SIGNAL(paused()), SLOT(pause()));
    connect(view_, SIGNAL(resumed()), SLOT(resume()));
    connect(view_, SIGNAL(scaleChanged(qreal)), SLOT(setScale(qreal)));
    connect(view_, SIGNAL(itemVisibleChanged(bool)), SLOT(setVisible(bool)));
    connect(view_, SIGNAL(itemMetaVisibleChanged(bool)), SLOT(setMetaVisibleAndUpdate(bool)));

    if (style_ == SubtitleStyle &&
        hub_->isSignalTokenMode())
      connect(view_, SIGNAL(removeItemRequested()), SLOT(disappear()));

    if (isVisible() != view_->isItemVisible())
      setVisible(view_->isItemVisible());
    scene_->addItem(this);
  }
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
    if (!removeLaterTimer_ || !removeLaterTimer_->isActive()) {
      fadeOut(timeout);
      removeLater(timeout);
    }
  }
}

void
AnnotationGraphicsItem::removeMe()
{
  if (scene()) {
    // Always try to disconnect to avoid segmentation fault
    //if (style_ == SubtitleStyle &&
    //    hub_->isSignalTokenMode() &&
    //    !hub_->isStopped())
    if (hub_->isSignalTokenMode())
      disconnect(view_, SIGNAL(removeItemRequested()), this, SLOT(disappear()));
    disconnect(view_, SIGNAL(paused()), this, SLOT(pause()));
    disconnect(view_, SIGNAL(resumed()), this, SLOT(resume()));
    disconnect(view_, SIGNAL(scaleChanged(qreal)), this, SLOT(setScale(qreal)));
    disconnect(view_, SIGNAL(itemVisibleChanged(bool)), this, SLOT(setVisible(bool)));
    disconnect(view_, SIGNAL(itemMetaVisibleChanged(bool)), this, SLOT(setMetaVisibleAndUpdate(bool)));

    scene_->removeItem(this);
    view_->releaseItem(this);
  }
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
AnnotationGraphicsItem::analyzeMe()
{
  view_->selectItem(this, true); // detail = true
}

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
  case SubtitleStyle:
    return appearOpacityAni_ && appearOpacityAni_->state() == QAbstractAnimation::Paused;
  }
  return false;
}

void
AnnotationGraphicsItem::pause()
{
  if (!isMetaVisible())
    setMetaVisibleAndUpdate(true);

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
  if (isMetaVisible())
    setMetaVisibleAndUpdate(false);
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
  case SubtitleStyle:
    if (appearOpacityAni_ && appearOpacityAni_->state() == QAbstractAnimation::Paused)
      appearOpacityAni_->resume();
    break;
  }
}

int
AnnotationGraphicsItem::stayTime(Style style) const
{
  int t = style == SubtitleStyle ? ANNOTATION_STAY_TIME_SUBTITLE
                                 : ANNOTATION_STAY_TIME;
  int w0 = qMax<int>(view_->width(), 100),
      w = qMax<int>(boundingRect().width(), 50),
      h = qMax<int>(boundingRect().height(), 20);
  qreal f = qreal(w0 + 200)/ (w + 200),
        g = qreal(h + 20) / (ANNOTATION_SIZE_DEFAULT + 15);
  qreal q = hub_->isSignalTokenMode() ? 1.0 : 0.8;
  int ret = t * ::pow(f, 0.3)* g * q + ANNOTATION_STAY_TIME_MIN;
  return qMin(ret, ANNOTATION_STAY_TIME_MAX);
}

int
AnnotationGraphicsItem::flyTime() const
{
  int w0 = qMax<int>(view_->width(), 100),
      w = qMax<int>(boundingRect().width(), 50);
  qreal f = qreal(w0 + 200) / (w + 200);
  int ret = ANNOTATION_FLY_TIME * ::pow(f, 0.2) + ANNOTATION_FLY_TIME_MIN;
  ret = qMin(ret, ANNOTATION_FLY_TIME_MAX);
  if (style_ == FlyStyle)
    ret /= 5;
  //if (view_->width() > 640)
  //  ret = ret * view_->width() / 640;
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
  //if (style == SubtitleStyle || msecs <= 0)
  //  stay(QPointF(x, y), msecs);
  //else
  appear(QPointF(x, y), msecs);
}

//void
//AnnotationGraphicsItem::stay(const QPointF &pos, int msecs)
//{
//  updateEffect();
//  addMe();
//  setPos(pos);
//  if (hub_->isMediaTokenMode() ||
//      !hub_->isStopped() && msecs >= 0)
//    removeLater(msecs);
//}

void
AnnotationGraphicsItem::appear(const QPointF &pos, int msecs)
{
  setOpacity(0.0);
  setPos(pos);
  if (msecs <= 0) {
    enum { FadeDuration = 800 };
    fadeIn(FadeDuration);
  } else {
    if (!appearOpacityAni_) {
      appearOpacityAni_ = new QPropertyAnimation(this, "opacity", this);
      QEasingCurve curve;
      curve.setCustomType(curve_::appearOpacity);
      appearOpacityAni_->setEasingCurve(curve);
      connect(appearOpacityAni_, SIGNAL(finished()), SLOT(removeMe()));
    } else if (appearOpacityAni_->state() != QAbstractAnimation::Stopped)
      appearOpacityAni_->stop();
    appearOpacityAni_->setStartValue(0.0);
    appearOpacityAni_->setEndValue(OPACITY);
    appearOpacityAni_->setDuration(msecs);
    appearOpacityAni_->start();
  }
  addMe();
}

void
AnnotationGraphicsItem::fadeIn(int msecs)
{
  if (!fadeAni_) {
    fadeAni_ = new QPropertyAnimation(this, "opacity", this);
    fadeAni_->setEasingCurve(QEasingCurve::Linear);
  } else if (fadeAni_->state() != QAbstractAnimation::Stopped)
    fadeAni_->stop();
  setOpacity(0.0);
  fadeAni_->setStartValue(0.0);
  fadeAni_->setEndValue(OPACITY);
  fadeAni_->setDuration(msecs);
  fadeAni_->start();
}

void
AnnotationGraphicsItem::fadeOut(int msecs)
{
  if (!fadeAni_) {
    fadeAni_ = new QPropertyAnimation(this, "opacity", this);
    fadeAni_->setEasingCurve(QEasingCurve::Linear);
  } else if (fadeAni_->state() != QAbstractAnimation::Stopped)
    fadeAni_->stop();
  setOpacity(OPACITY);
  fadeAni_->setStartValue(OPACITY);
  fadeAni_->setEndValue(0.0);
  fadeAni_->setDuration(msecs);
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
    flyAni_ = new QPropertyAnimation(this, "relativePos", this);
    flyAni_->setEasingCurve(QEasingCurve::Linear);
  } else if (flyAni_->state() != QAbstractAnimation::Stopped)
    flyAni_->stop();
  if (!flyOpacityAni_) {
    flyOpacityAni_ = new QPropertyAnimation(this, "opacity", this);
    QEasingCurve curve;
    curve.setCustomType(curve_::flyOpacity);
    flyOpacityAni_->setEasingCurve(curve);
    flyOpacityAni_->setStartValue(0.0);
    flyOpacityAni_->setEndValue(OPACITY);
    connect(flyOpacityAni_, SIGNAL(finished()), SLOT(removeMe()));
  } else if (flyOpacityAni_->state() != QAbstractAnimation::Stopped)
    flyOpacityAni_->stop();

  setOpacity(0.0);
  origin_ = from;
  flyAni_->setDuration(msecs);
  flyAni_->setStartValue(QPointF());
  flyAni_->setEndValue(to - from);

  flyOpacityAni_->setDuration(msecs);

  flyAni_->start();
  flyOpacityAni_->start();
  addMe();
}

// - Events -

void
AnnotationGraphicsItem::contextMenuEvent(QContextMenuEvent *event)
{
  DOUT("enter");
  Q_ASSERT(event);

  bool paused = isPaused();
  if (!paused)
    pause();

  QMenu *m = new QMenu(view_);
  AcUi::globalInstance()->setContextMenuStyle(m, false); // persistent = false

  if (!hub_->isLiveTokenMode())
    m->addAction(tr("Edit"), this, SLOT(edit()));
  m->addAction(tr("Copy"), this, SLOT(copyToClipboard()));
  m->addAction(tr("Hide"), this, SLOT(disappear()));
  if (paused)
    m->addAction(tr("Release") + " ["+ tr("MButton")+"]", this, SLOT(resume()));
  if (annot_.hasUserId())
    m->addAction(tr("Analytics") + " ["+ tr("DoubleClick")+"]", this, SLOT(analyzeMe()));
  m->addSeparator();

  if (annot_.userId() == view_->userId()) {
    if (annot_.hasId() && !hub_->isLiveTokenMode())
      m->addAction(TR(T_DELETE), this, SLOT(deleteMe()));
  } else {
    QString text = summary();
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

  //if (!paused)
  //  resume();
  DOUT("exit");
}

void
AnnotationGraphicsItem::mouseDoubleClickEvent(QMouseEvent *event)
{
  Q_UNUSED(event);
  if (!isPaused())
    pause();
  analyzeMe();
}

bool
AnnotationGraphicsItem::isDragging() const
{ return !isBadPosF(dragPos_); }

void
AnnotationGraphicsItem::mousePressEvent(QMouseEvent *event)
{
  Q_ASSERT(event);
  switch (event->button()) {
  case Qt::LeftButton:
    if (::isBadPosF(dragPos_))
      dragPos_ = QPointF(event->globalPos()) -  scenePos();
  case Qt::RightButton:
    if (!isPaused()) {
      pause();
      selectMe();
    }
    break;
  case Qt::MiddleButton:
  default:
    if (isPaused())
      resume();
    break;
  }

  if (event->button() == Qt::LeftButton) {
    if (toolTip().isEmpty())
      updateToolTip();
    // FIXME: HACK!
    // Alternatively, use scene()->sendEvent or
    // intercept QEvent::ToolTip and show tooltip using QToolTip::showText
    // See: http://qt-project.org/doc/qt-4.8/widgets-tooltips.html
    //scene_->sendEvent(this, &toolTipEvent);
    view_->setToolTip(toolTip());
    QCoreApplication::sendEvent(view_,
      new QHelpEvent(QEvent::ToolTip, event->pos(), event->globalPos())
    );
  }
}

void
AnnotationGraphicsItem::mouseReleaseEvent(QMouseEvent *event)
{
  Q_UNUSED(event);
  //if (dragPaused_ && isPaused() &&
  //    dragPos_ == event->globalPos() -  scenePos().toPoint())
  //  resume();
  dragPos_ = BAD_POSF;
}

void
AnnotationGraphicsItem::mouseMoveEvent(QMouseEvent *event)
{
  Q_ASSERT(event);
  if ((event->buttons() & Qt::LeftButton) && !::isBadPosF(dragPos_)) {
    QPointF newPos = QPointF(event->globalPos()) - dragPos_;
    // use QCoreApplication::postEvent is more elegant but less efficient
    setPos(newPos);
  }

#ifdef Q_WS_WIN
  if (toolTip().isEmpty())
    updateToolTip();
  view_->setToolTip(toolTip()); // FIXME: HACK!
  QCoreApplication::sendEvent(view_,
    new QHelpEvent(QEvent::ToolTip, event->pos(), event->globalPos())
  );
#endif // Q_WS_WIN
}

void
AnnotationGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
  Q_ASSERT(event);
  QPoint pos(event->pos().x(), event->pos().y());
  QContextMenuEvent e(QContextMenuEvent::Keyboard, pos, event->screenPos());
  contextMenuEvent(&e);
}

#define GRAPHICS_SCENE_MOUSE_EVENT(_handler) \
  void \
  AnnotationGraphicsItem::_handler(QGraphicsSceneMouseEvent *event) \
  { \
    Q_ASSERT(event); \
    QMouseEvent e(event->type(), event->pos().toPoint(), event->screenPos(), event->button(), event->buttons(), event->modifiers()); \
    _handler(&e); \
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
AnnotationGraphicsItem::summary() const
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
  if (len < 0.0) {
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
  int msecs = qMax<int>(len/v, 100);
  escapeTo(reflected(to), msecs);
}

void
AnnotationGraphicsItem::escapeTo(const QPointF &to, int msecs)
{
  if (!isPaused())
    pause();
  if (rushAni_ && rushAni_->state() != QAbstractAnimation::Stopped)
    rushAni_->stop();
  if (!escapeAni_) {
    escapeAni_ = new QPropertyAnimation(this, "pos", this);
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
AnnotationGraphicsItem::rushTo(const QPointF &center)
{
  // 300: half of (800, 600) in annotationgraphicsview
  // 400: half of 1366x768
  int radius = hub_->isFullScreenWindowMode() ? 400 : 300;
  qreal rx = qrand() / qreal(RAND_MAX),
        ry = qrand() / qreal(RAND_MAX);
  //QPointF now = boundingRect().center(); // use center, so that annot with diff length will have diff speed
  QPointF now = pos();
  QPointF d = now - center;
  qreal len = ::sqrt(d.x()*d.x() + d.y()*d.y());
  if (len < 2) {
    QPointF to = now + radius * QPointF(0.5+rx, 0.5-ry);
    enum { msecs = 2000 };
    rushTo(reflected(to), msecs);
    return;
  }
  qreal dist = qMax<qreal>(len*0.2, radius);
  qreal delta = qAbs(len - dist);
  if (delta < 2)
    return;

  qreal v = 1/(delta/60.0 + 1) + 0.1;
  enum { min_msecs = 800 };
  int msecs = qMax<int>(len/v, min_msecs);

  QPointF to = now - d * (delta/len) + radius * QPointF(0.5-rx, 0.5-ry);
  rushTo(reflected(to), msecs);
}

void
AnnotationGraphicsItem::rushTo(const QPointF &to, int msecs)
{
  if (!isPaused())
    pause();
  if (escapeAni_ && escapeAni_->state() != QAbstractAnimation::Stopped)
    escapeAni_->stop();
  if (!rushAni_) {
    rushAni_ = new QPropertyAnimation(this, "pos", this);
    rushAni_->setEasingCurve(QEasingCurve::OutQuart);
    //connect(rushAni_, SIGNAL(finished()), SLOT(resume()));
  } else if (rushAni_->state() != QAbstractAnimation::Stopped)
    rushAni_->stop();

  rushAni_->setDuration(msecs);
  rushAni_->setStartValue(pos());
  rushAni_->setEndValue(to);
  rushAni_->start();
}

// - Helpers -

QPointF
AnnotationGraphicsItem::boundedToScene(const QPointF &pos) const
{
  QRectF rect = view_->sceneRect();
  return QPointF(
    qBound(rect.left(), pos.x(), rect.right()),
    qBound(rect.top(), pos.y(), rect.bottom())
  );
}

QPointF
AnnotationGraphicsItem::reflected(const QPointF &pos) const
{
  QRectF rect = view_->sceneRect();
  qreal x =
    pos.x() < rect.left()  ? rect.left() * 2 - pos.x() :
    pos.x() > rect.right() ? rect.right() * 2 - pos.x() :
                             pos.x();
  qreal y =
    pos.y() < rect.top()   ? rect.top() * 2 - pos.y() :
    pos.y() > rect.bottom()? rect.bottom() * 2 - pos.y() :
                             pos.y();
  return QPointF(x, y);
}

// EOF
