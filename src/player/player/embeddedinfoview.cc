// embeddedinfoview.cc
// 4/10/2012

#include "embeddedinfoview.h"
#include "datamanager.h"
#include "signalhub.h"
#include "tr.h"
#include "src/common/acsettings.h"
#include "src/common/acui.h"
#include "annotationsettings.h"
#include "lib/annotcloud/alias.h"
#include "lib/player/player.h"
#include "lib/qtext/datetime.h"
#include "lib/qtext/htmltag.h"
#include "lib/qtext/ss.h"
#include <QtGui>

using namespace AnnotCloud;

//#define DEBUG "embeddedinfoview"
#include "lib/debug/debug.h"

#ifdef __clang__
# pragma clang diagnostic ignored "-Wlogical-op-parentheses" // '&&' within '||'
#endif // __clang__

#define MAX_WIDTH       300

#define SS_TEXT_BEGIN   "<span style=\"background-color:rgba(0,0,0,16)\">"
#define SS_TEXT_END     "</span>"

// - Construction -

EmbeddedInfoView::EmbeddedInfoView(Player *player, DataManager *data, SignalHub *hub, QWidget *parent)
  : Base(parent), player_(player), data_(data), hub_(hub)
{
  Q_ASSERT(player_);
  Q_ASSERT(data_);
  Q_ASSERT(hub_);

  setStyleSheet(
    SS_BEGIN(QLabel)
      SS_COLOR(snow)
      SS_FONT_SIZE(9pt)
      //SS_FONT_WEIGHT(bold)
    SS_END
  );

  setMaximumWidth(MAX_WIDTH);

  { // Outline font
    QGraphicsEffect *e = AcUi::globalInstance()->makeHaloEffect(QColor("orange"));
    if (e) {
      e->setProperty("opacity", 0.95);
      setGraphicsEffect(e);
    }
  }

  connect(player_, SIGNAL(hueChanged(int)), SLOT(refresh()));
  connect(player_, SIGNAL(contrastChanged(qreal)), SLOT(refresh()));
  connect(player_, SIGNAL(saturationChanged(qreal)), SLOT(refresh()));
  connect(player_, SIGNAL(brightnessChanged(qreal)), SLOT(refresh()));
  connect(player_, SIGNAL(gammaChanged(qreal)), SLOT(refresh()));
  connect(player_, SIGNAL(positionChanged()), SLOT(refresh()));
  connect(player_, SIGNAL(audioDelayChanged(qint64)), SLOT(refresh()));
  connect(player_, SIGNAL(audioChannelChanged(int)), SLOT(refresh()));
  connect(AnnotationSettings::globalSettings(), SIGNAL(scaleChanged(qreal)), SLOT(refresh()));
  connect(AnnotationSettings::globalSettings(), SIGNAL(speedupChanged(qreal)), SLOT(refresh()));
  connect(AnnotationSettings::globalSettings(), SIGNAL(rotationChanged(qreal)), SLOT(refresh()));
  connect(AnnotationSettings::globalSettings(), SIGNAL(offsetChanged(int)), SLOT(refresh()));
}

// - Events -

void
EmbeddedInfoView::setVisible(bool t)
{
  bool visible = t && hub_->isMediaTokenMode();
  if (visible != isVisible())
    Base::setVisible(visible);
}

void
EmbeddedInfoView::updateText()
{
  DOUT("enter");
  QString t;

  // Player
  if (hub_->isMediaTokenMode() && player_->hasMedia()) {
    // Media
    QSize sz = player_->videoDimension();
    QString szField;
    if (!sz.isEmpty())
      szField = QString("%1x%2 ")
        .arg(QString::number(sz.width()))
        .arg(QString::number(sz.height()));
    QString nch;
    switch (player_->audioChannelCount()) {
    case 1: nch = tr("1"); break;
    case 2: nch = tr("2"); break;
    default: nch = QString::number(player_->audioChannelCount());
    }
    qreal fps = player_->fps();
    QString fpsField;
    if (!qFuzzyCompare(1.0, 1 + fps))
      fpsField = QString::number(player_->fps(), 'f', 1) + "fps";
    qint64 size = player_->mediaSize();
    QString sizeField;
    if (size) {
      if (size < 1024)
        sizeField = QString::number(size) + "B" " ";
      else if (size < 1024 * 1024)
        sizeField = QString::number(size / 1014) + " KB" " ";
      else
        sizeField = QString::number(size / (1024.0 * 1024), 'f', 1) + " MB" " ";
    }
    qreal kbrate = player_->bitrate() / 1000;
    QString kbrateField;
    if (kbrate > 0.0)
      kbrateField = QString(HTML_SS_OPEN(color:red) "%3" HTML_SS_CLOSE() " kbps")
        .arg(QString::number(kbrate, 'f', 1));
    t.append(
      HTML_SS_OPEN(color:red)
      + QString("- %1 -").arg(tr("Codec")) +
      HTML_SS_CLOSE()
      HTML_BR()
      +
      QString("%1: %2" "%3" HTML_BR())
      .arg(TR(T_DATA))
      .arg(sizeField)
      .arg(kbrateField)
      +
      QString(
        "%1: %2" " "
        "%3" "%4" HTML_BR()
       )
      .arg(tr("Video"))
      .arg(player_->videoCodecName().toUpper())
      .arg(szField).arg(fpsField)
      +
      QString(
         "%1: %2" " "
         "%3%4" " "
         "%5%6" HTML_BR()
       )
      .arg(tr("Audio"))
      .arg(player_->audioCodecName().toUpper())
      .arg(QString::number(player_->audioRate())).arg(tr("Hz"))
      .arg(nch).arg(tr("ch"))
    );

    // Video
    qreal saturation = player_->saturation();
    qreal gamma = player_->gamma();
    int hue = player_->hue();
    qreal contrast = player_->contrast();
    qreal brightness = player_->brightness();

    //if (hue || !qFuzzyCompare(saturation, 1) || !qFuzzyCompare(gamma, 1) ||
    //    !qFuzzyCompare(contrast, 1) || !qFuzzyCompare(brightness, 1)) {
      t.append(
        HTML_SS_OPEN(color:red)
        + QString("- %1 -").arg(tr("Video")) +
        HTML_SS_CLOSE()
        HTML_BR()
      );

      //if (!qFuzzyCompare(saturation, 1))
        t.append(QString("%1(Q):%2" " ")
          .arg(TR(T_SATURATION))
          .arg(QString::number(saturation, 'f', 2))
        );
      //if (!qFuzzyCompare(gamma, 1))
        t.append(QString("%1(W):%2" " ")
          .arg(TR(T_GAMMA))
          .arg(QString::number(gamma, 'f', 2))
        );
      //if (hue)
        t.append(QString("%1(E):%2" " ")
          .arg(TR(T_HUE))
          .arg(QString::number(hue))
        );
      t.append(HTML_BR());
      //if (!qFuzzyCompare(contrast, 1))
        t.append(QString("%1(R):%2" " ")
          .arg(TR(T_CONTRAST))
          .arg(QString::number(contrast, 'f', 2))
        );
      //if (!qFuzzyCompare(brightness, 1))
        t.append(QString("%1(T):%2" " ")
          .arg(TR(T_BRIGHTNESS))
          .arg(QString::number(brightness, 'f', 2))
        );
      t.append(HTML_BR());
    //}

    qint64 delay = player_->audioDelay() / 1000;
    Player::AudioChannel ch = player_->audioChannel();
    if (delay ||
        ch && ch != Player::StereoChannel) {
      t.append(
        HTML_SS_OPEN(color:red)
        + QString("- %1 -").arg(tr("Audio")) +
        HTML_SS_CLOSE()
        HTML_BR()
      );

      QString chstr;
      switch (ch) {
      case Player::LeftChannel:     chstr = tr("Left"); break;
      case Player::RightChannel:    chstr = tr("Right"); break;
      case Player::StereoChannel:   chstr = tr("Stereo"); break;
      case Player::ReverseStereoChannel: chstr = tr("Reverse"); break;
      case Player::DolbysChannel:   chstr = tr("Dolby's"); break;
      case Player::NoChannel:
      default: chstr = tr("None");
      }
      if (ch != Player::StereoChannel)
        chstr = HTML_SS_OPEN(color:orange) + chstr + HTML_SS_CLOSE();
      t.append(QString("%1: %2")
        .arg(tr("Channel")).arg(chstr)
      );

      if (delay)
        t.append(QString(" %1: " HTML_SS_OPEN(color:red) "%2" "%3" HTML_SS_CLOSE())
          .arg(tr("Delay")).arg(QString::number(delay)).arg(tr(" sec."))
        );

      t.append(HTML_BR());
    }
  }

  // Annotation
  if (data_->hasAnnotations()) {
    t.append(
      HTML_SS_OPEN(color:red)
      + QString("- %1 -").arg(TR(T_ANNOTATION)) +
      HTML_SS_CLOSE()
      HTML_BR()
    );
    qint64 minT = data_->minAnnotationCreateTime(),
           maxT = data_->maxAnnotationCreateTime();
    if (minT < maxT) {
      QString latest = timeToString(maxT),
              oldest = timeToString(minT);
      if (!latest.isEmpty() && !oldest.isEmpty())
        t.append(
          QString(
            "%1: %2%3" " - "
            "%4%5" HTML_BR()
          )
          .arg(TR(T_DATE))
          .arg(oldest).arg(tr(" ago"))
          .arg(latest).arg(tr(" ago"))
        );
    }
    int userCount = data_->userCount();
    if (!userCount)
      userCount = 1;
    t.append(
      QString(
        "%1:" HTML_SS_OPEN(color:red) "%2" HTML_SS_CLOSE() " "
        "%3:" HTML_SS_OPEN(color:red) "%4" HTML_SS_CLOSE() " "
        "%5:" HTML_SS_OPEN(color:red) "%6" HTML_SS_CLOSE() "/%7" HTML_BR()
      )
      .arg(tr("Annotations")).arg(QString::number(data_->annotations().size()))
      .arg(tr("Users")).arg(QString::number(userCount))
      .arg(tr("Average"))
        .arg(QString::number(data_->annotations().size() / qreal(userCount), 'f', 1))
        .arg(tr("user"))
    );
    bool newline = false;;
    qreal scale = hub_->isFullScreenWindowMode() ? AnnotationSettings::globalSettings()->fullscreenScale()
                                                 : AnnotationSettings::globalSettings()->scale();
    if (!qFuzzyCompare(scale, 1)) {
      QString s = QString::number(scale, 'f', 2);
      t.append(tr("Scale") + ":" + s + " ");
      newline = true;
    }
    if (!qFuzzyCompare(AnnotationSettings::globalSettings()->speedup(), 1)) {
      QString speed = QString::number(AnnotationSettings::globalSettings()->speedup(), 'f', 2);
      t.append(tr("Speed") + ":" + speed + " ");
      newline = true;
    }
    if (!qFuzzyCompare(AnnotationSettings::globalSettings()->rotation() +1, 1)) {
      QString rotation = QString::number(AnnotationSettings::globalSettings()->rotation(), 'f', 1);
      rotation = HTML_SS_OPEN(color:red) + rotation + HTML_SS_CLOSE();
      t.append(tr("Rotation") + ":" + rotation + " ");
      newline = true;
    }
    if (AnnotationSettings::globalSettings()->offset()) {
      QString offset = QString::number(AnnotationSettings::globalSettings()->offset());
      offset = HTML_SS_OPEN(color:red) + offset + HTML_SS_CLOSE();
      t.append(tr("Offset") + ":" + offset + tr(" sec"));
      newline = true;
    }
    if (newline)
      t.append(HTML_BR());
  }

  // Data
  QStringList urls;
  QString src = data_->token().url();
  if (!src.isEmpty())
    urls.append(src);
  foreach (const Alias &a, data_->aliases())
    if (a.type() == Alias::AT_Url)
      urls.append(a.text());
  if (!urls.isEmpty()) {
    t.append(
      HTML_SS_OPEN(color:red)
      + QString("- %1 -").arg(TR(T_URL)) +
      HTML_SS_CLOSE()
      HTML_BR()
    );
    foreach (const QString &url, urls)
      t.append(url + HTML_BR());
  }

  // Time
  {
    static int lang = 0;
    if (!lang)
      lang = AcSettings::globalSettings()->language();
    t.append(
      HTML_SS_OPEN(color:red)
      + QString("- %1 -").arg(TR(T_TIME)) +
      HTML_SS_CLOSE()
      HTML_BR()
    );
    QDateTime utc = QDateTime::currentDateTimeUtc();
    QDateTime local = utc.toLocalTime();
    int currentTimeZone = utc.secsTo(local) / 3600;
    bool showJapanTime = currentTimeZone != QtExt::JapanTimeZone,
         showChinaTime = currentTimeZone != QtExt::ChinaTimeZone && lang == QLocale::Chinese;
    static const QString fmt = "h:mm";
    t.append(
      tr("Local") + ": "
      HTML_SS_OPEN(color:red)
      + local.time().toString(fmt) + " "
      + weekToString(local.date().dayOfWeek()) +
      HTML_SS_CLOSE()
    );
    if (showJapanTime) {
      QTime jst = utc.time().addSecs(3600 * QtExt::JapanTimeZone);
      t.append(QString(" %1:%2")
        .arg(tr("Japan")).arg(jst.toString(fmt))
      );
    }
    if (showChinaTime) {
      QTime cst = utc.time().addSecs(3600 * QtExt::ChinaTimeZone);
      t.append(QString(" %1:%2")
        .arg(tr("China")).arg(cst.toString(fmt))
      );
    }
    t.append(HTML_BR());
  }

  //static const size_t tail = qstrlen(HTML_BR());
  //if (t.size() >= tail)
  //  t.chop(tail);

  setText(SS_TEXT_BEGIN + t + SS_TEXT_END);
  DOUT("exit");
}

// - Format -

QString
EmbeddedInfoView::weekToString(int dayOfWeek)
{
  switch (dayOfWeek) {
  case 1: return tr("Mon");
  case 2: return tr("Tue");
  case 3: return tr("Wed");
  case 4: return tr("Thu");
  case 5: return tr("Fri");
  case 6: return tr("Sat");
  case 7: return tr("Sun");
  default: return QString();
  }
}

QString
EmbeddedInfoView::timeToString(qint64 secs)
{
  QString ret;
  QDateTime earlier = QDateTime::fromMSecsSinceEpoch(secs * 1000);
  QDateTime now = QDateTime::currentDateTime();
  qint64 days = earlier.daysTo(now);
  if (days > 0) {
    if (days >= 365 * 2)
      ret = QString::number(days / 365) + tr(" years");
    else if (days >= 365)
      ret = QString::number(days / 365) + tr(" year");
    else if (days >= 30 * 2)
      ret = QString::number(days / 30) + tr(" months");
    else if (days >= 30)
      ret = QString::number(days / 30) + tr(" month");
    else if (days >= 7 * 2)
      ret = QString::number(days / 7) + tr(" weeks");
    else if (days >= 7)
      ret = QString::number(days / 7) + tr(" week");
    else if (days >= 1 * 2)
      ret = QString::number(days) + tr(" days");
    else if (days >= 1)
      ret = QString::number(days) + tr(" day");
  } else {
    qint64 msecs = earlier.msecsTo(now);
    QTime t = QtExt::msecs2time(msecs);
    if (t.hour() > 0)
      ret = QString::number(t.hour()) + tr(" hr.");
    else if (t.minute() > 0)
      ret = QString::number(t.minute()) + tr(" min.");
    else if (t.second() > 0)
      ret = QString::number(t.second()) + tr(" sec.");
  }
  return ret;
}

// EOF
