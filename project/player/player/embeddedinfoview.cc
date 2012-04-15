// embeddedinfoview.cc
// 4/10/2012

#include "embeddedinfoview.h"
#include "datamanager.h"
#include "signalhub.h"
#include "tr.h"
#include "annotationgraphicsview.h"
#include "module/player/player.h"
#include "module/qtext/htmltag.h"
#include "module/qtext/ss.h"
#include "module/annotcloud/alias.h"
#include <QtGui>

using namespace AnnotCloud;

//#define DEBUG "embeddedinfoview"
#include "module/debug/debug.h"

#define STYLESHEET \
  SS_BEGIN(QLabel) \
    SS_COLOR(cyan) \
  SS_END

#define MAX_WIDTH       300

#ifdef Q_WS_WIN
#  define FONT_SIZE     8 // 8 em
#else
#  define FONT_SIZE     10 // 10 em
#endif

// - Construction -

EmbeddedInfoView::EmbeddedInfoView(Player *player, DataManager *data, AnnotationGraphicsView *annot, SignalHub *hub, QWidget *parent)
  : Base(parent), player_(player), data_(data), annot_(annot), hub_(hub)
{
  Q_ASSERT(player_);
  Q_ASSERT(data_);
  Q_ASSERT(hub_);
  //connect(this, SIGNAL(refreshRequested()), SLOT(invalidateText()));

  setStyleSheet(STYLESHEET);

  setMaximumWidth(MAX_WIDTH);

  QFont f = font();
  f.setPointSize(FONT_SIZE);
  setFont(f);

  connect(player_, SIGNAL(hueChanged(int)), SLOT(refresh()));
  connect(player_, SIGNAL(contrastChanged(qreal)), SLOT(refresh()));
  connect(player_, SIGNAL(saturationChanged(qreal)), SLOT(refresh()));
  connect(player_, SIGNAL(brightnessChanged(qreal)), SLOT(refresh()));
  connect(player_, SIGNAL(gammaChanged(qreal)), SLOT(refresh()));
  connect(annot_, SIGNAL(scaleChanged(qreal)), SLOT(refresh()));
  connect(annot_, SIGNAL(rotationChanged(qreal)), SLOT(refresh()));
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
EmbeddedInfoView::invalidateText()
{
  DOUT("enter");
  QString t;

  // Player
  if (hub_->isMediaTokenMode() && player_->hasMedia()) {
    // Render
    qreal contrast = player_->contrast();
    qreal brightness = player_->brightness();
    int hue = player_->hue();
    qreal saturation = player_->saturation();
    qreal gamma = player_->gamma();

    t.append(
      HTML_STYLE_OPEN(color:red)
      + QString("- %1 -").arg(tr("Render")) +
      HTML_STYLE_CLOSE() \
      HTML_BR()
      +
      QString(
        "%1: %2" HTML_BR() \
        "%3: %4" HTML_BR() \
        "%5: %6" HTML_BR() \
        "%7: %8" HTML_BR() \
        "%9: %10" HTML_BR() \
      )
      .arg(TR(T_SATURATION)).arg(QString::number(saturation, 'f', 2))
      .arg(TR(T_GAMMA)).arg(QString::number(gamma, 'f', 2))
      .arg(TR(T_HUE)).arg(QString::number(hue))
      .arg(TR(T_CONTRAST)).arg(QString::number(contrast, 'f', 2))
      .arg(TR(T_BRIGHTNESS)).arg(QString::number(brightness, 'f', 2))
    );

    // Media
    QSize sz = player_->videoDimension();
    qreal fps = player_->fps();
    QString fpsField;
    if (!qFuzzyCompare(1.0, 1 + fps))
      fpsField = QString::number(player_->fps(), 'f', 1) + "fps ";
    t.append(
      HTML_STYLE_OPEN(color:red)
      + QString("- %1 -").arg(tr("Codec")) +
      HTML_STYLE_CLOSE() \
      HTML_BR()
      +
      QString("%1: %2 %3x%4 %5%6bps" HTML_BR())
      .arg(TR(T_VIDEO))
      .arg(player_->videoCodecName().toUpper())
      .arg(QString::number(sz.width()))
      .arg(QString::number(sz.height()))
      .arg(fpsField)
      .arg(QString::number(player_->bitrate(), 'f', 1))
      +
      QString("%1: %2 %3%4 %5%6" HTML_BR())
      .arg(TR(T_AUDIO))
      .arg(player_->audioCodecName().toUpper())
      .arg(QString::number(player_->audioRate())).arg(tr("Hz"))
      .arg(QString::number(player_->audioChannels())).arg(tr("ch"))

    );
  };

  // Annotation
  if (data_->hasAnnotations()) {
    t.append(
      HTML_STYLE_OPEN(color:red)
      + QString("- %1 -").arg(TR(T_ANNOTATION)) +
      HTML_STYLE_CLOSE() \
      HTML_BR()
      +
      QString(
        "%1: %2" HTML_BR() \
        "%3: %4" HTML_BR()
      )
      .arg(tr("Scale")).arg(QString::number(annot_->scale(), 'f', 2))
      .arg(tr("Rotation")).arg(QString::number(annot_->rotation(), 'f', 1))
    );
  }

  // Data
  QStringList urls;
  QString src = data_->token().source();
  if (!src.isEmpty())
    urls.append(src);
  foreach (const Alias &a, data_->aliases())
    if (a.type() == Alias::AT_Url)
      urls.append(a.text());
  if (!urls.isEmpty()) {
    t.append(
      HTML_STYLE_OPEN(color:red)
      + QString("- %1 -").arg(TR(T_URL)) +
      HTML_STYLE_CLOSE() \
      HTML_BR()
    );
    foreach (const QString &url, urls)
      t.append(url + HTML_BR());
  }

  //static const int tail = ::strlen(HTML_BR());
  //if (t.size() >= tail)
  //  t.chop(tail);

  setText(t);
  DOUT("exit");
}

// EOF
