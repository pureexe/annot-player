// annotationgraphicsitem.cc
// 7/16/2011

#include "annotationgraphicsitem.h"
#include "annotationgraphicsitemscheduler.h"
#include "annotationgraphicsview.h"
#include "annotationgraphicseffect.h"
#include "annotationsettings.h"
#include "annotationeditor.h"
#include "datamanager.h"
//#include "textformathandler.h"
#include "tr.h"
#include "global.h"
#include "signalhub.h"
#include "src/common/acui.h"
#include "src/common/acrc.h"
#include "lib/annotcloud/annottag.h"
#include "lib/annotcloud/annothtml.h"
#include "lib/translator/translator.h"
#include "lib/searchengine/searchenginefactory.h"
#include "lib/searchengine/searchenginerc.h"
#include "qtx/qxfs.h"
#include "htmlutil/htmltags.h"
#include "lib/textcodec/textcodec.h"
#include <QtGui>
#include <boost/tuple/tuple.hpp>

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wparentheses"    // suggest parentheses around '&&' within '||'
# pragma GCC diagnostic ignored "-Wsequence-point" // operation on variable may be undefined
#endif // __GNUC__
#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wuninitialized"      // variable 't' is unitialized when used
# pragma GCC diagnostic ignored "-Wunused-function"    // unused curve functions
# pragma GCC diagnostic ignored "-Wlogical-op-parentheses" // '&&' within '||'
#endif // __GNUC__

#ifdef __GNUC__
# define NOINLINE      __attribute__((noinline))
#else
# define NOINLINE
#endif // __GNUC__

enum { HoldTimeOut = 1000 }; // in msecs

//#define DEBUG "annotationgraphicsitem"
#include "qtx/qxdebug.h"

using namespace AnnotCloud;

#define BAD_POSF        QPointF(-1,-1)
namespace { namespace detail {
  inline bool isBadPosF(const QPointF &pos) { return pos.x() < 0 || pos.y() < 0; }
} } // anonymous detail

enum { FloatStayTime = 7000 }; // 7 seconds
enum { FadeInDuration = 800, FadeOutDuration = 600,
       SubFadeInDuration = 200, SubFadeOutDuration = 100 };

// - RC -

#ifdef Q_OS_LINUX
# define RC_AVATAR_PREFIX      AVATARDIR
#else
# define RC_AVATAR_PREFIX      QCoreApplication::applicationDirPath() + "/avatars"
#endif // Q_OS_LINUX

#define AVATAR_SIZE             "33" // 100/3

enum { RC_AVATAR_COUNT = AVATAR_USER_COUNT };
//enum { RC_AVATAR_GIF_COUNT = AVATAR_GIF_COUNT };

namespace { namespace detail {
  inline QString rc_avatar_url(qint64 i)
  {
    static QString fmt = RC_AVATAR_PREFIX "/user_%1.jpg";
    return fmt.arg(QString::number(qAbs(i) % RC_AVATAR_COUNT));

    //static QString jpg = RC_AVATAR_PREFIX "/user_%1.jpg",
    //               gif = RC_AVATAR_PREFIX "/user_%1.gif";
    //int hash = qAbs(i) % (RC_AVATAR_GIF_COUNT + RC_AVATAR_JPG_COUNT);
    //return hash < RC_AVATAR_JPG_COUNT ?
    //  jpg.arg(QString::number(hash)) :
    //  gif.arg(QString::number(hash - RC_AVATAR_JPG_COUNT));
  }
} } // anonymous detail

// - Helpers -

#define ANNOTATION_LIFE_SCHEDULE   ((ANNOTATION_LIFE_ViSIBLE + 1) / 4)
#define OPACITY                    AnnotationSettings::globalSettings()->opacity()

namespace { namespace curve_ { // anonymous curves

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
  { return linear_(progress, 0.15, 0.80); }

  qreal NOINLINE flyOpacity(qreal progress)
  { return linear_(progress, 0.15, 0.80); }

  qreal NOINLINE linear(qreal progress)
  { return progress; }

} } // anonymous namespace curve_

int
AnnotationGraphicsItem::nextY(int msecs) const
{
  DOUT("enter: msecs =" << msecs);
  QSizeF itemSize = boundingRect().size();
  int ret;
  switch (style_) {
  case FlyStyle:
  case DriftStyle:
    ret = view_->scheduler()->nextMovingY(itemSize, msecs);
    break;
  case TopStyle:
  case BottomStyle:
    ret = view_->scheduler()->nextStationaryY(itemSize.height(), msecs, style_, isSubtitle());
    break;
  default: Q_ASSERT(0); ret = 0;
  }
  int max = view_->height() - itemSize.height();
  if (ret > max - 5)
    ret = max;
  DOUT("exit: ret =" << ret);
  return ret;
}

// - Constructions -

void
AnnotationGraphicsItem::warmUp()
{
#ifdef Q_OS_MAC
  QGraphicsTextItem dummy;

  QFont
  font = AnnotationSettings::globalSettings()->font();
  font.setWeight(QFont::Light); dummy.setFont(font);
  font.setWeight(QFont::Normal); dummy.setFont(font);
  font.setWeight(QFont::DemiBold); dummy.setFont(font);
  font.setWeight(QFont::Bold); dummy.setFont(font);
  font.setWeight(QFont::Black); dummy.setFont(font);

  font = AnnotationSettings::globalSettings()->japaneseFont();
  font.setWeight(QFont::Light); dummy.setFont(font);
  font.setWeight(QFont::Normal); dummy.setFont(font);
  font.setWeight(QFont::DemiBold); dummy.setFont(font);
  font.setWeight(QFont::Bold); dummy.setFont(font);
  font.setWeight(QFont::Black); dummy.setFont(font);

  font = AnnotationSettings::globalSettings()->chineseFont();
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
  : metaVisible_(false), avatarVisible_(false),
    view_(view), data_(data), hub_(hub), wrap_(AutoWrap), style_(NoStyle), positionResolution_(0),
    removeLaterTimer_(nullptr),
    flyAni_(nullptr), flyOpacityAni_(nullptr), escapeAni_(nullptr), rushAni_(nullptr), appearOpacityAni_(nullptr), fadeAni_(nullptr),
    dragPos_(BAD_POSF), pressTime_(0)
{
  DOUT("enter");
  Q_ASSERT(data_);
  Q_ASSERT(hub_);
  Q_ASSERT(view_);
  scene_ = view_->scene();
  Q_ASSERT(scene_);
  connect(this, SIGNAL(message(QString)), view_, SIGNAL(message(QString)));

  setAcceptHoverEvents(false);
  setAcceptTouchEvents(false);
  setAcceptDrops(false);
  setFlags(QGraphicsItem::ItemClipsToShape | QGraphicsItem::ItemIgnoresParentOpacity);

  //setCacheMode(QGraphicsItem::DeviceCoordinateCache,  QSize(400,40)); // could mess up OCN translation

  //effect_ = new AnnotationGraphicsEffect;
  setGraphicsEffect(effect_ = new AnnotationGraphicsEffect);

  QTextOption opt = document()->defaultTextOption();
  opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
  opt.setAlignment(Qt::AlignCenter);
  document()->setDefaultTextOption(opt);

  DOUT("exit");
}

// - Properties -

void
AnnotationGraphicsItem::setRelativeXSmoothly(qreal xOffset)
{
  qreal newX = origin_.x() + xOffset;
  if (qAbs(newX - x()) > positionResolution_)
    setX(newX);
}

const QString&
AnnotationGraphicsItem::plainText() const
{
  if (plainText_.isEmpty() && !text_.isEmpty())
    plainText_ = document()->toPlainText().simplified(); //QxhtmlToPlainText(text_).simplified();
  return plainText_;
}

void
AnnotationGraphicsItem::setAnnotation(const Annotation &annot)
{
  if (!prefix_.isEmpty())
    prefix_.clear();
  if (!suffix_.isEmpty())
    suffix_.clear();
  annot_ = annot;
  DOUT("text =" << annot_.text());
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

  setScale(view_->scale());

  //effect_->setOpacity(0);
  //resetOutlineColor();
  setOpacity(0);
}

void
AnnotationGraphicsItem::setOutlineColor(const QColor &color)
{ effect_->setColor(color); }

//void
//AnnotationGraphicsItem::resetOutlineColor()
//{ setOutlineColor(AnnotationSettings::globalSettings()->outlineColor()); }

void
AnnotationGraphicsItem::updateOutlineColor()
{
  setOutlineColor(
    //isOwner() ? QColor(ANNOTATION_OUTLINE_COLOR_SELF) :
    isSubtitle() ? AnnotationSettings::globalSettings()->subtitleColor() :
    AnnotationSettings::globalSettings()->outlineColor()
  );
}

void
AnnotationGraphicsItem::setAvatarVisibleAndUpdate(bool t)
{
  if (avatarVisible_ != t) {
    avatarVisible_ = t;
    updateComponents();
  }
}

void
AnnotationGraphicsItem::setMetaVisibleAndUpdate(bool t)
{
  if (isMetaVisible() != t) {
    metaVisible_ = t;
    updateComponents();
  } else
    metaVisible_ = t;
}

void
AnnotationGraphicsItem::updateComponents()
{
  QString t;
  if (isMetaVisible()) {
    if (suffix_.isEmpty())
      updateMeta();
    t = prefix_ + text_ + suffix_;
  } else if (isAvatarVisible()) {
    if (prefix_.isEmpty())
      updateAvatar();
    t = prefix_ + text_;
  } else
    t = text_;

  setText(t);
}

bool
AnnotationGraphicsItem::isAvatarVisible() const
{
  return avatarVisible_ &&
    //annot_.hasUserId() && annot_.userAlias() != "guest" &&
    !isSubtitle();
}

bool
AnnotationGraphicsItem::isMetaVisible() const
{
  return annot_.hasUserId() && (
    metaVisible_ ||
    AnnotationSettings::globalSettings()->isMetaVisible() ||
    hub_->isSignalTokenMode() && !isSubtitle()
  );
}

bool
AnnotationGraphicsItem::isOwner() const
{
  return view_->userId() &&
         (view_->userId() != User::UI_Guest || hub_->isMediaTokenMode()) &&
         annot_.userId() == view_->userId();
}

void
AnnotationGraphicsItem::updateText()
{
  setDefaultStyle();
  updateOutlineColor();
  //updateEffect();

  // TODO: condition "isMediaTokenMode" is to be removed
  //bool isOwner = view_->userId() &&
  //               view_->userId() != User::UI_Guest &&
  //               annot_.userId() == view_->userId();
  bool owner = isOwner();

  QFont font;
  if (annot_.hasLanguage())
    switch (annot_.language()) {
    case Traits::Japanese: font = AnnotationSettings::globalSettings()->defaultJapaneseFont(); break;
    case Traits::TraditionalChinese:
    case Traits::SimplifiedChinese:
      font = AnnotationSettings::globalSettings()->defaultChineseFont(); break;
    default:  font = AnnotationSettings::globalSettings()->font();
    }
  else
    switch (AcSettings::globalSettings()->language()) {
    case QLocale::Japanese: font = AnnotationSettings::globalSettings()->defaultJapaneseFont(); break;
    case QLocale::Chinese:  font = AnnotationSettings::globalSettings()->defaultChineseFont(); break;
    default:  font = AnnotationSettings::globalSettings()->font();
    }

  if (owner)
    font.setUnderline(true);
    //setOutlineColor(ANNOTATION_OUTLINE_COLOR_SELF);
  setFont(font);

  QString text = annot_.text();

  if (annot_.language() == Traits::SimplifiedChinese &&
      AnnotationSettings::globalSettings()->preferTraditionalChinese())
    text = TextCodec::zhs2zht(text);
  if (isSubtitle())
    text = view_->subtitlePrefix() + text;
    //if (!owner)
    //  setOutlineColor(ANNOTATION_OUTLINE_COLOR_SUB);

  QStringList tags;
  boost::tie(text_, tags) = ANNOT_PARSE_CODE(text);
  if (!plainText_.isEmpty())
    plainText_.clear();
  setTags(tags);

  if (!style_)
    style_ = isSubtitle() ?
      (AnnotationSettings::globalSettings()->isSubtitleOnTop() ? TopStyle : BottomStyle) :
      AnnotationSettings::globalSettings()->preferFloat() ? FloatStyle : DriftStyle;

  enum { MinWrapWidth = 300, MinWrapHeight = 50 };

  int textWidth = -1;
  bool wrap = false;
  switch (wrap_) {
  case AlwaysWrap: wrap = true; break;
  case AutoWrap:
    switch (style_) {
    case DriftStyle:
    case FloatStyle:
    case FlyStyle:
      break;
    default: wrap = true;
    } break;
  case NoWrap: default: ;
  }
  if (wrap) {
    int w = view_->width() * 4/5;
    if (w > MinWrapWidth && view_->height() > MinWrapHeight)
      textWidth = w;
    textWidth /= view_->scale();
  }
  setTextWidth(textWidth);

  //if (tags.contains(CORE_CMD_VERBATIM))
  //  setPlainText(text_);
  if (isMetaVisible()) {
    if (suffix_.isEmpty())
      updateMeta();
    setText(prefix_ + text_ + suffix_);
  } else if (isAvatarVisible()) {
    if (prefix_.isEmpty())
      updateAvatar();
    setText(prefix_ + text_);
  } else
    setText(text_);
  //if (owner)
  //  richText_ = CORE_CMD_LATEX_ULINE " " + richText_;
}

void
AnnotationGraphicsItem::updateToolTip()
{
  QString tip;
  qint64 t = annot_.hasUpdateTime() ? annot_.updateTime() : annot_.createTime();
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
AnnotationGraphicsItem::updateAvatar()
{
  prefix_.clear();
  qint64 uid = annot_.userId();
  if (uid) {
    int count = data_->annotationCountForUserId(uid);
    if (count > 1)
      prefix_ =
        "<img"
        " src=\"" + detail::rc_avatar_url(uid) + "\""
        " alt=\"" + annot_.userAlias() + "\""
        " border=\"0\""
        " width=\"" AVATAR_SIZE "\""
        " height=\"" AVATAR_SIZE "\""
        "/> "; // a trailing space at the end
  }
}

void
AnnotationGraphicsItem::updateMeta()
{
  QString ret;
  qint64 t = annot_.hasUpdateTime() ? annot_.updateTime() : annot_.createTime();
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
      ts.append(HTML_SS_OPEN(color:orange) + ds + HTML_SS_CLOSE());
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
      ts.append(HTML_SS_OPEN(font-size:12px;color:magenta) + w + HTML_SS_CLOSE());

      ts.append(t1.toString("h:mm"));
    } else {
      QTime t0 = QTime::currentTime();
      int d;
      if (days == 1)
        ts.append(QString::number((24 + t1.hour() - t0.hour()) % 24))
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
      ts = HTML_SS_OPEN(color:yellow)
          + ts +
          HTML_SS_CLOSE();
      ret.append(ts);
    }
  }
  QString u = annot_.userAlias();
  if (!u.isEmpty()) {
    enum { user_len = 4 };
    u = HTML_SS_OPEN(color:cyan)
        "@" + u.right(user_len) +
        HTML_SS_CLOSE();
    ret.append(u);
  }
  QString avatar;
  qint64 uid = annot_.userId();
  if (uid) {
    int count = data_->annotationCountForUserId(uid);
    if (count > 1) {
      QString c = QString::number(count);
      c = HTML_SS_OPEN(font-size:11px;color:red) "x" + c + HTML_SS_CLOSE();
      ret.append(c);

      avatar =
        "<img"
        " src=\"" + detail::rc_avatar_url(uid) + "\""
        " alt=\"" + annot_.userAlias() + "\""
        " border=\"0\""
        " width=\"" AVATAR_SIZE "\""
        " height=\"" AVATAR_SIZE "\""
        "/> "; // a trailing space at the end
    }
  }
  if (!ret.isEmpty())
    ret = HTML_SS_OPEN(text-decoration:underline;font-size:11px) + ret + HTML_SS_CLOSE();
  prefix_ = avatar;
  suffix_ = ret;
}

// TODO: How to use QTextCharFormat to set advanced format:
// See: http://lists.trolltech.com/qt-interest/2005-12/thread00469-0.html
// See also: http://www.qtcentre.org/threads/26168-Contoured-text-in-editable-QGraphicsTextItem
void
AnnotationGraphicsItem::setText(const QString &text)
{
  DOUT("scene =" << scene() << ", visible =" << isVisible() << ", opacity =" << effect_->opacity() <<
       ", pos =" << pos() << ", size =" << boundingRect().size() <<
       ", html =" << text);

  if (hub_->isSignalTokenMode() && AnnotationSettings::globalSettings()->backgroundOpacityFactor()) {
    QString alpha = QString::number(
      (AnnotationSettings::globalSettings()->backgroundOpacityFactor() * 255) / 100
    );
    //t.prepend(HTML_CSS_OPEN() "background-color:rgba(0,0,0," + alpha + ");" HTML_CSS_CLOSE());
    setHtml(
      "<span style=\"background-color:rgba(0,0,0," + alpha + ")\">"
        + text +
      "</span>"
    );
  } else
    setHtml(text);

  // Increase text contrast with white outline
  QTextCursor tc = textCursor();
  tc.select(QTextCursor::Document);
  QTextCharFormat fmt;
  //fmt.setTextOutline(QPen(AnnotationSettings::globalSettings()->outlineColor(), 0.5)); // QPen(brush, width)
  fmt.setTextOutline(QPen(QColor(0,0,0,100), 0.2)); // QPen(brush, width)
  tc.mergeCharFormat(fmt);

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
      case AnnotCloud::H_Drift: style_ = DriftStyle; break;
      case AnnotCloud::H_Float: style_ = FloatStyle; break;
      case AnnotCloud::H_Fly: style_ = FlyStyle; break;
      case AnnotCloud::H_Top: style_ = TopStyle; break;
      case AnnotCloud::H_Bottom: style_ = BottomStyle; break;

      case AnnotCloud::H_Sub:
      case AnnotCloud::H_Subtitle:
        break;

      // Wrap:
      case AnnotCloud::H_Wrap: wrap_= AlwaysWrap; break;
      case AnnotCloud::H_NoWrap: wrap_ = NoWrap; break;
        break;

      // Effect:
      //case AnnotCloud::H_Transp:
      //case AnnotCloud::H_Transparent: setEffect(TransparentEffect); break;
      //case AnnotCloud::H_Shadow: setEffect(ShadowEffect); break;
      //case AnnotCloud::H_Blur: setEffect(BlurEffect); break;

      default:
        // Warn if the annot is submitted by current user
        if (!annot_.hasUserId() && !annot_.hasUserAlias() ||
            view_->userId() == annot_.userId())
        view_->warn(TR(T_ERROR_UNKNOWN_COMMAND) + ": " + tag);
      }
    }
}

void
AnnotationGraphicsItem::setDefaultStyle()
{
  style_ = NoStyle;
  setFlags(QGraphicsItem::ItemIsMovable); // Doesn't work when view_ is embedded in dock window orz

  wrap_ = AutoWrap;

  //setToolTip(TR(T_TOOLTIP_ANNOTATIONITEM));
  setDefaultTextColor(ANNOTATION_COLOR_DEFAULT);
  //effect_->setColor(AnnotationSettings::outlineColor());
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
//    auto e = qobject_cast<QGraphicsOpacityEffect *>(graphicsEffect()));
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
//#ifdef Q_OS_WIN
//      enum { offset = 1, radius = 18 };
//#else
//      enum { offset = 1, radius = 12 };
//#endif // Q_OS_WIN
//      auto e = qobject_cast<QGraphicsDropShadowEffect *>(graphicsEffect());
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
//      auto e = qobject_cast<QGraphicsOpacityEffect *>(graphicsEffect());
//      if (!e) {
//        e = new QGraphicsOpacityEffect;
//        e->setOpacity(ANNOTATION_OPACITY);
//        setGraphicsEffect(e);
//      }
//    } break;
//  case BlurEffect:
//    {
//      auto e = qobject_cast<QGraphicsBlurEffect *>(graphicsEffect());
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
//      auto e = qobjectcast<AnnotationGraphicsEffect *>(graphicsEffect());
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
//    return HTML_SS(SELF(left), text-decoration:line-through);
//  else if (hash == AnnotCloud::H_Underline
//           || hash == AnnotCloud::H_Uline)
//    return HTML_SS(SELF(left), text-decoration:underline);
//  else if (hash == AnnotCloud::H_Overline)
//    return HTML_SS(SELF(left), text-decoration:overline);
//  else if (hash == AnnotCloud::H_Blink)
//    return HTML_SS(SELF(left), text-decoration:blink);
//
//#define ELIF_TRANSFORM(_trans)
//  else if (hash == AnnotCloud::H_##_trans)
//    return HTML_SS(SELF(left), text-transform:_trans);
//
//  ELIF_TRANSFORM(Uppercase)
//  ELIF_TRANSFORM(Lowercase)
//#undef ELIF_TRANSFORM
//
//#define ELIF_COLOR(_color)
//  else if (hash == (AnnotCloud::H_##_color))
//    return HTML_SS(SELF(left), color:_color);
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
//    return HTML_SS(SELF(left), font-size:_size);
//
//  ELIF_SIZE(Tiny,       ANNOTATION_SIZE_TINY)
//  ELIF_SIZE(Small,      ANNOTATION_SIZE_SMALL)
//  ELIF_SIZE(Normal,     ANNOTATION_SIZE_NORMAL)
//  ELIF_SIZE(Large,      ANNOTATION_SIZE_LARGE)
//  ELIF_SIZE(Huge,       ANNOTATION_SIZE_HUGE)
//#undef ELIF_SIZE
//
//  else {
//    view_->warn(TR(T_ERROR_UNKNOWN_COMMAND) + ": " + text);
//    return text;
//  }
//#undef SELF
//}

// - Add/remove me -

void
AnnotationGraphicsItem::addMe()
{
  if (!scene()) {
    DOUT("enter");
    connect(view_, SIGNAL(paused()), SLOT(pause()));
    connect(view_, SIGNAL(resumed()), SLOT(resume()));
    connect(view_, SIGNAL(scaleChanged(qreal)), SLOT(setScale(qreal)));
    connect(view_, SIGNAL(itemVisibleChanged(bool)), SLOT(setVisible(bool)));
    connect(view_, SIGNAL(itemMetaVisibleChanged(bool)), SLOT(setMetaVisibleAndUpdate(bool)));
    connect(AnnotationSettings::globalSettings(), SIGNAL(metaVisibleChanged(bool)), SLOT(setMetaVisibleAndUpdate(bool)));
    connect(AnnotationSettings::globalSettings(), SIGNAL(avatarVisibleChanged(bool)), SLOT(setAvatarVisibleAndUpdate(bool)));
    connect(AnnotationSettings::globalSettings(), SIGNAL(outlineColorChanged(QColor)), SLOT(updateOutlineColor()));
    connect(AnnotationSettings::globalSettings(), SIGNAL(subtitleColorChanged(QColor)), SLOT(updateOutlineColor()));
    connect(AnnotationSettings::globalSettings(), SIGNAL(highlightColorChanged(QColor)), SLOT(updateOutlineColor()));
    connect(AnnotationSettings::globalSettings(), SIGNAL(positionResolutionChanged(int)), SLOT(setPositionResolution(int)));
    if (hub_->isSignalTokenMode())
      connect(AnnotationSettings::globalSettings(), SIGNAL(backgroundOpacityFactorChanged(int)), SLOT(updateText()));

    if (isSubtitle())
      connect(view_, SIGNAL(removeSubtitlesRequested()), SLOT(disappear()));

    if (isVisible() != view_->isItemVisible())
      setVisible(view_->isItemVisible());
    scene_->addItem(this);

    DOUT("visible =" << isVisible() << effect_->opacity());
    DOUT("exit");
  }
}

void
AnnotationGraphicsItem::disappear()
{
  switch (style_) {
  case TopStyle: case BottomStyle: case FloatStyle:
    fadeOut(); break;

  case FlyStyle: case DriftStyle:
  default:
    if (!removeLaterTimer_ || !removeLaterTimer_->isActive())
      fadeOut();
  }
}

void
AnnotationGraphicsItem::removeMe()
{
  if (scene()) {
    DOUT("enter");
    // Always try to disconnect to avoid segmentation fault
    disconnect(view_, SIGNAL(removeSubtitlesRequested()), this, SLOT(disappear()));
    disconnect(view_, SIGNAL(paused()), this, SLOT(pause()));
    disconnect(view_, SIGNAL(resumed()), this, SLOT(resume()));
    disconnect(view_, SIGNAL(scaleChanged(qreal)), this, SLOT(setScale(qreal)));
    disconnect(view_, SIGNAL(itemVisibleChanged(bool)), this, SLOT(setVisible(bool)));
    disconnect(view_, SIGNAL(itemMetaVisibleChanged(bool)), this, SLOT(setMetaVisibleAndUpdate(bool)));
    disconnect(AnnotationSettings::globalSettings(), SIGNAL(metaVisibleChanged(bool)), this, SLOT(setMetaVisibleAndUpdate(bool)));
    disconnect(AnnotationSettings::globalSettings(), SIGNAL(avatarVisibleChanged(bool)), this, SLOT(setAvatarVisibleAndUpdate(bool)));
    disconnect(AnnotationSettings::globalSettings(), SIGNAL(outlineColorChanged(QColor)), this, SLOT(updateOutlineColor()));
    disconnect(AnnotationSettings::globalSettings(), SIGNAL(subtitleColorChanged(QColor)), this, SLOT(updateOutlineColor()));
    disconnect(AnnotationSettings::globalSettings(), SIGNAL(highlightColorChanged(QColor)), this, SLOT(updateOutlineColor()));
    disconnect(AnnotationSettings::globalSettings(), SIGNAL(positionResolutionChanged(int)), this, SLOT(setPositionResolution(int)));
    if (hub_->isSignalTokenMode())
      disconnect(AnnotationSettings::globalSettings(), SIGNAL(backgroundOpacityFactorChanged(int)), this, SLOT(updateText()));

    scene_->removeItem(this);
    view_->releaseItem(this);
    DOUT("exit");
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
  case TopStyle:
  case BottomStyle:
    stay();
    break;
  case FlyStyle:
  case DriftStyle:
    fly();
    break;
  case FloatStyle:
    if (!AnnotationSettings::globalSettings()->preferFloat())
        //isOwner())
      fly();
    else {
      enum { msecs = FloatStayTime }; //stayTime();
      QPointF pos = view_->scheduler()->nextFloatPos(boundingRect().size(), msecs, isSubtitle());
      if (pos.isNull()) {
        style_ = DriftStyle;
        fly();
      } else if (isSubtitle())
        appear(pos, -1);
      else
        appear(pos, msecs);
    } break;

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
  if (hub_->isSignalTokenMode() &&
      isSubtitle() &&
      !hub_->isStopped())
    return false;

  switch (style_) {
  case DriftStyle:
  case FlyStyle:
    return flyOpacityAni_ && flyOpacityAni_->state() == QAbstractAnimation::Paused;

  case TopStyle:
  case BottomStyle:
  case FloatStyle:
    return appearOpacityAni_ && appearOpacityAni_->state() == QAbstractAnimation::Paused;
  default: Q_ASSERT(0);
  }
  return false;
}

void
AnnotationGraphicsItem::pause()
{
  if (!isMetaVisible())
    setMetaVisibleAndUpdate(true);

  if (hub_->isSignalTokenMode() &&
      isSubtitle() &&
      !hub_->isStopped())
    return;

  if (fadeAni_ && fadeAni_->state() == QAbstractAnimation::Running)
    fadeAni_->pause();

  switch (style_) {
  case DriftStyle:
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
  case FloatStyle:
    if (appearOpacityAni_ && appearOpacityAni_->state() == QAbstractAnimation::Running)
      appearOpacityAni_->pause();
    break;

  default: Q_ASSERT(0);
  //case SubtitleStyle:
  //  if (removeLaterTimer_ && removeLaterTimer_->isActive())
  //    removeLaterTimer_->stop();
  //  break;
  }
  if (!view_->isPaused())
    setOutlineColor(AnnotationSettings::globalSettings()->highlightColor());
}

void
AnnotationGraphicsItem::resume()
{
  updateOutlineColor();
  if (isMetaVisible())
    setMetaVisibleAndUpdate(false);
  if (hub_->isSignalTokenMode() &&
      isSubtitle() &&
      !hub_->isStopped())
    return;

  if (fadeAni_ && fadeAni_->state() == QAbstractAnimation::Paused)
    fadeAni_->resume();

  switch (style_) {
  case DriftStyle:
  case FlyStyle:
    if (flyOpacityAni_ && flyOpacityAni_->state() == QAbstractAnimation::Paused)
      flyOpacityAni_->resume();
    if (flyAni_ && flyAni_->state() == QAbstractAnimation::Paused) {
      origin_ = relativePos();
      flyAni_->resume();
    } break;

  case TopStyle:
  case BottomStyle:
  //case SubtitleStyle:
  case FloatStyle:
    if (appearOpacityAni_ && appearOpacityAni_->state() == QAbstractAnimation::Paused)
      appearOpacityAni_->resume();
    break;
  default:
    Q_ASSERT(0);
  }
}

int
AnnotationGraphicsItem::stayTime() const
{
  enum {
    AnnotationTime = 5000,
    SubtitleTime = 5000
    //MinTime = 1000, // 1 second
    //MaxTime = 10000 // 10 seconds
  };
  return isSubtitle() ? SubtitleTime : AnnotationTime;

  //QRectF r = boundingRect();
  //int t = style_ != SubtitleStyle ? AverageTime : SubtitleTime;
  //int w0 = qMax<int>(view_->width(), 100),
  //    w = qMax<int>(r.width(), 50),
  //    h = qMax<int>(r.height(), 20);
  //qreal f = qreal(w0 + 200)/ (w + 200),
  //      g = qreal(h + 20) / (ANNOTATION_SIZE_DEFAULT + 15);
  //qreal q = hub_->isSignalTokenMode() ? 1.0 : 0.8;
  //int ret = t * qPow(f, 0.3)* g * q + MinTime;
  //return qMin<int>(ret, MaxTime);
}

int
AnnotationGraphicsItem::flyTime() const
{
  enum {
    MovingFactor = 3000, // the larger, the slower
    MinTime = 1000, // 1 second
    MaxTime = 20000 // 20 seconds
  };

  int itemWidth = boundingRect().width(), // around 50~200
      windowWidth = view_->width();       // around 800~1920
  int ret = (MovingFactor *(windowWidth + itemWidth)) / (itemWidth + 200);
  if (hub_->isSignalTokenMode())
    ret *= 2;
  if (style_ == FlyStyle)
    ret /= 5;
  //qDebug() << "time =" << ret << windowWidth << boundingRect().size();
  return qBound<int>(MinTime, ret, MaxTime) / AnnotationSettings::globalSettings()->speedup();
}

void
AnnotationGraphicsItem::stay()
{
  int msecs = stayTime();
  int x = (view_->width() - boundingRect().width() * scale()) / 2,
      y = nextY(msecs);
  //qDebug() << "y =" << y << ", msecs =" << msecs;

  if (hub_->isSignalTokenMode() &&
      isSubtitle() &&
      !hub_->isStopped())
    msecs = -1;
  //if (isSubtitle() || msecs <= 0)
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
  if (msecs <= 0)
    fadeIn();
  else {
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
AnnotationGraphicsItem::fadeIn()
{ fadeIn(isSubtitle() ? SubFadeInDuration : FadeInDuration); }

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
AnnotationGraphicsItem::fadeOut()
{
  int msecs = isSubtitle() ? SubFadeOutDuration : FadeOutDuration;
  fadeOut(msecs);
  removeLater(msecs);
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
  int y = nextY(msecs);

  qreal fromX = view_->width(),
        toX = - boundingRect().width();

  positionResolution_ = AnnotationSettings::globalSettings()->positionResolution();
  fly(fromX, toX, y, msecs);
}

void
AnnotationGraphicsItem::fly(qreal from, qreal to, qreal y, int msecs)
{
  Q_ASSERT(msecs > 0);
  if (!flyAni_) {
    flyAni_ = new QPropertyAnimation(this, "relativeX", this);
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
  origin_ = QPointF(from, y);
  flyAni_->setDuration(msecs);
  flyAni_->setStartValue(qreal(0.0));
  flyAni_->setEndValue(to - from);

  flyOpacityAni_->setDuration(msecs);

  setY(y);
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
  QMenu *searchMenu = new QMenu(tr("Search"), view_);
  //QMenu *translateMenu = new QMenu(tr("Translate"), view_);
  AcUi::globalInstance()->setContextMenuStyle(m, false); // persistent = false
  AcUi::globalInstance()->setContextMenuStyle(searchMenu, false); // persistent = false

  if (!hub_->isLiveTokenMode()) {
    QAction *a = m->addAction(tr("Edit"), this, SLOT(edit()));
    a->setEnabled(isEditable());
    m->addSeparator();
  }
  m->addAction(tr("Copy") + ": " + summary(), this, SLOT(copyToClipboard()));
  m->addAction(tr("Hide"), this, SLOT(disappear()));
  if (paused)
    m->addAction(tr("Release") + " ["+ tr("MButton")+"]", this, SLOT(resume()));
  if (annot_.hasUserId())
    m->addAction(tr("Analytics"), this, SLOT(analyzeMe()));
  m->addSeparator();

  searchMenu->addAction(QIcon(WBRC_IMAGE_GOOGLE), "Google", this, SLOT(searchWithGoogle()));
  searchMenu->addAction(QIcon(WBRC_IMAGE_BING), "Bing", this, SLOT(searchWithBing()));
  m->addMenu(searchMenu);

  //translateMenu->addAction(QIcon(ACRC_IMAGE_ENGLISH), TR(T_ENGLISH), this, SLOT(translateToEnglish()));
  //translateMenu->addSeparator();
  //translateMenu->addAction(QIcon(ACRC_IMAGE_JAPANESE), TR(T_JAPANESE), this, SLOT(translateToJapanese()));
  //translateMenu->addSeparator();
  //translateMenu->addAction(QIcon(ACRC_IMAGE_TRADITIONAL_CHINESE), TR(T_CHINESE), this, SLOT(translateToTraditionalChinese()));
  //translateMenu->addAction(QIcon(ACRC_IMAGE_SIMPLIFIED_CHINESE), TR(T_SIMPLIFIEDCHINESE), this, SLOT(translateToSimplifiedChinese()));
  //translateMenu->addSeparator();
  //translateMenu->addAction(QIcon(ACRC_IMAGE_KOREAN), TR(T_KOREAN), this, SLOT(translateToKorean()));
  //translateMenu->addSeparator();
  //translateMenu->addAction(QIcon(ACRC_IMAGE_FRENCH), TR(T_FRENCH), this, SLOT(translateToFrench()));
  //translateMenu->addAction(QIcon(ACRC_IMAGE_GERMAN), TR(T_GERMAN), this, SLOT(translateToGerman()));
  //translateMenu->addAction(QIcon(ACRC_IMAGE_ITALIAN), TR(T_ITALIAN), this, SLOT(translateToItalian()));
  //translateMenu->addAction(QIcon(ACRC_IMAGE_SPANISH), TR(T_SPANISH), this, SLOT(translateToSpanish()));
  //translateMenu->addAction(QIcon(ACRC_IMAGE_PORTUGUESE), TR(T_PORTUGUESE), this, SLOT(translateToPortuguese()));
  //translateMenu->addAction(QIcon(ACRC_IMAGE_RUSSIAN), TR(T_RUSSIAN), this, SLOT(translateToRussian()));

  m->addAction(tr("Translate"), this, SLOT(translatePlainText()));

    if (annot_.language() == Traits::SimplifiedChinese &&
      !AnnotationSettings::globalSettings()->preferTraditionalChinese())
    m->addAction(QIcon(ACRC_IMAGE_TRADITIONAL_CHINESE), tr("Show Traditional Chinese"), this, SLOT(showTraditionalChinese()));

  m->addSeparator();

  if (annot_.userId() == view_->userId()) {
    if (annot_.hasId() && !hub_->isLiveTokenMode())
      m->addAction(TR(T_DELETE), this, SLOT(deleteMe()));
  } else {
    if (annot_.id() > 0 && !hub_->isLiveTokenMode()) {
      m->addAction(TR(T_BLESS), this, SLOT(blessMe()));
      m->addAction(TR(T_CURSE), this, SLOT(curseMe()));
    }
    if (annot_.hasUserAlias()) {
      m->addAction(TR(T_BLOCK), this, SLOT(blockMe()));
      m->addSeparator();
      m->addAction(TR(T_MENUTEXT_BLOCKUSER) + ": " + annot_.userAlias(), this, SLOT(blockUser()));
    }
  }

  m->exec(event->globalPos());
  delete m;
  delete searchMenu;
  //delete translateMenu;
  event->accept();

  //if (!paused)
  //  resume();
  DOUT("exit");
}

void
AnnotationGraphicsItem::mouseDoubleClickEvent(QMouseEvent *event)
{
  Q_UNUSED(event)
  pressTime_ = 0;
  if (!isPaused())
    pause();

  //analyzeMe();
  if (hub_->isSignalTokenMode() && isSubtitle() && isAnnotated()) {
    int pos = charPositionAtGlobalPos(event->globalPos());
    QTextCursor tc = textCursor();
    tc.setPosition(pos);
    tc.select(QTextCursor::WordUnderCursor);
    if (tc.hasSelection()) {
      view_->translateWord(tc.selectedText());

      QTextCharFormat fmt;
      fmt.setFontUnderline(true);
      tc.mergeCharFormat(fmt);
    }
  } else if (isEditable())
    edit();
  else
    view_->selectItem(this, true); // detail = true

}

bool
AnnotationGraphicsItem::isDragging() const
{ return !detail::isBadPosF(dragPos_); }

void
AnnotationGraphicsItem::mousePressEvent(QMouseEvent *event)
{
  Q_ASSERT(event);
  switch (event->button()) {
  case Qt::LeftButton:
    if (detail::isBadPosF(dragPos_))
      dragPos_ = QPointF(event->globalPos()) -  scenePos();
  case Qt::RightButton:
    if (isPaused())
      pressTime_ = QDateTime::currentMSecsSinceEpoch();
    else {
      pressTime_ = 0;
      pause();
      selectMe();
    }
    break;
  case Qt::MiddleButton:
  default:
    pressTime_ = 0;
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
    QHelpEvent e(QEvent::ToolTip, event->pos(), event->globalPos());
    QCoreApplication::sendEvent(view_, &e);

    if (hub_->isSignalTokenMode() && isSubtitle() && isAnnotated()) {
      int pos = charPositionAtGlobalPos(event->globalPos());
      QTextCursor tc = textCursor();
      tc.setPosition(pos);
      tc.select(QTextCursor::WordUnderCursor);
      if (tc.hasSelection()) {
        QTextCharFormat fmt;
        //fmt.setUnderlineStyle(QTextCharFormat::WaveUnderline);
        //fmt.setUnderlineColor(Qt::red);
        //fmt.setTextOutline(QPen(Qt::red));
        fmt.setFontUnderline(true);
        tc.mergeCharFormat(fmt);
        //QTextCharFormat fmt = tc.charFormat();
        //QFont font = fmt.font();
        //font.setPointSizeF(font.pointSizeF() * 1.3);
        //font.setUnderline(true);
        //fmt.setFont(font);
        //tc.setCharFormat(fmt);
        //QString t = tc.selectedText().trimmed();
      }
    }
  }
  setOutlineColor(AnnotationSettings::globalSettings()->highlightColor());
}

void
AnnotationGraphicsItem::mouseReleaseEvent(QMouseEvent *event)
{
  Q_UNUSED(event)
  //if (dragPaused_ && isPaused() &&
  //    dragPos_ == event->globalPos() -  scenePos().toPoint())
  //  resume();

 if (pressTime_ && pressTime_ + HoldTimeOut > QDateTime::currentMSecsSinceEpoch() &&
     isPaused())
    resume();
  pressTime_ = 0;
  dragPos_ = BAD_POSF;
}

void
AnnotationGraphicsItem::mouseMoveEvent(QMouseEvent *event)
{
  Q_ASSERT(event);
  if ((event->buttons() & Qt::LeftButton) && !detail::isBadPosF(dragPos_)) {
    QPointF newPos = QPointF(event->globalPos()) - dragPos_;
    // use QCoreApplication::postEvent is more elegant but less efficient
    setPos(newPos);
  }

  setOutlineColor(AnnotationSettings::globalSettings()->highlightColor());
#ifdef Q_OS_WIN
  if (toolTip().isEmpty())
    updateToolTip();
  view_->setToolTip(toolTip()); // FIXME: HACK!
  QCoreApplication::sendEvent(view_,
    new QHelpEvent(QEvent::ToolTip, event->pos(), event->globalPos())
  );
#endif // Q_OS_WIN
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
    view_->showMessage(TR(T_SUCCEED_COPIED) + ": " + annot_.text());
  } else
    view_->warn(TR(T_ERROR_CLIPBOARD_UNAVAILABLE));
}

bool
AnnotationGraphicsItem::isEditable() const
{ return !annot_.userId() || annot_.userId() == view_->userId(); }

void
AnnotationGraphicsItem::edit()
{
  if (!isEditable()) {
    view_->warn(tr("cannot edit other's annotation text"));
    return;
  }

  view_->editor()->setText(annot_.text());
  view_->editor()->setId(isAnnotated() ? qint64(-1) : annot_.id());
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
  QString ret = plainText();
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

  qreal len = qSqrt(d.x()*d.x() + d.y()*d.y());
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
  qreal len = qSqrt(d.x()*d.x() + d.y()*d.y());
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

// - Search -

void
AnnotationGraphicsItem::searchWithEngine(int engine)
{
  QString t = plainText();
  if (!t.isEmpty())
    view_->searchText(t, engine);
}

void
AnnotationGraphicsItem::searchWithBing()
{ searchWithEngine(SearchEngineFactory::Bing); }

void
AnnotationGraphicsItem::searchWithGoogle()
{ searchWithEngine(SearchEngineFactory::Google); }

void
AnnotationGraphicsItem::translatePlainText()
{
  QString t = plainText();
  if (!t.isEmpty())
    view_->translateText(t);
}

void
AnnotationGraphicsItem::showTraditionalChinese()
{
  QString t = plainText();
  if (!t.isEmpty())
    view_->showTraditionalChinese(t);
}

//void
//AnnotationGraphicsItem::translateToEnglish()
//{ translate(Translator::English); }
//
//void
//AnnotationGraphicsItem::translateToJapanese()
//{ translate(Translator::Japanese); }
//
//void
//AnnotationGraphicsItem::translateToTraditionalChinese()
//{ translate(Translator::TraditionalChinese); }
//
//void
//AnnotationGraphicsItem::translateToSimplifiedChinese()
//{ translate(Translator::SimplifiedChinese); }
//
//void
//AnnotationGraphicsItem::translateToKorean()
//{ translate(Translator::Korean); }
//
//void
//AnnotationGraphicsItem::translateToFrench()
//{ translate(Translator::French); }
//
//void
//AnnotationGraphicsItem::translateToGerman()
//{ translate(Translator::German); }
//
//void
//AnnotationGraphicsItem::translateToItalian()
//{ translate(Translator::Italian); }
//
//void
//AnnotationGraphicsItem::translateToSpanish()
//{ translate(Translator::Spanish); }
//
//void
//AnnotationGraphicsItem::translateToPortuguese()
//{ translate(Translator::Portuguese); }
//
//void
//AnnotationGraphicsItem::translateToRussian()
//{ translate(Translator::Russian); }

// - Annotated Japanese -

bool
AnnotationGraphicsItem::isAnnotated() const
{ return annot_.text().contains(HTML_TABLE_CLOSE()); }


int
AnnotationGraphicsItem::charPositionAtGlobalPos(const QPoint &gp) const
{
  // See: http://www.qtforum.org/article/31604/figure-out-which-character-the-mouse-is-hovering-over-in-a-qtextedit.html
  Q_ASSERT(document());
  QPointF fp = pos();
  return 1 + document()->documentLayout()->hitTest(
    view_->mapFromGlobal(gp) - QPoint(fp.x(), fp.y()),
    Qt::FuzzyHit
  );
}

// EOF
