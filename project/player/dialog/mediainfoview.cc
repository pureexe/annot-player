// mediainfoview.cc
// 4/10/2012

#include "mediainfoview.h"
#include "datamanager.h"
#include "signalhub.h"
#include "tr.h"
#include "project/common/actextview.h"
#include "module/player/player.h"
#include "module/qtext/htmltag.h"
#include <QtGui>

#define DEBUG "mediainfoview"
#include "module/debug/debug.h"

MediaInfoView::MediaInfoView(Player *player, DataManager *data, SignalHub *hub, QWidget *parent)
  : Base(parent), player_(player), data_(data), hub_(hub)
{
  Q_ASSERT(player_);
  Q_ASSERT(data_);
  Q_ASSERT(hub_);
  connect(this, SIGNAL(refreshRequested()), SLOT(updateText()));
}

void
MediaInfoView::updateText()
{
  DOUT("enter");
  AcTextView *t = textView();
  t->clear();

  // Player
  if (hub_->isMediaTokenMode() && player_->hasMedia()) {
    // Media
    QSize sz = player_->videoDimension();
    qint64 size = player_->mediaSize();
    QString sizeField;
    if (size < 1024)
      sizeField = QString::number(size) + "B ";
    else if (size < 1024 * 1024)
      sizeField = QString::number(size / 1014) + "KB ";
    else
      sizeField = QString::number(size / (1024.0 * 1024), 'f', 1) + "MB ";
    t->append(QString("- %1 -").arg(tr("Codec")), Qt::red);
    t->append(QString("%1: %2, %3kbps")
      .arg(TR(T_DATA))
      .arg(sizeField)
      .arg(QString::number(player_->bitrate() / 1000, 'f', 1))
    );
    t->append(QString("%1: %2, %3x%4, %5fps")
      .arg(TR(T_VIDEO))
      .arg(player_->videoCodecName().toUpper())
      .arg(QString::number(sz.width()))
      .arg(QString::number(sz.height()))
      .arg(QString::number(player_->fps(), 'f', 1))
    );
    t->append(QString("%1: %2, %3%4 x %5%6")
      .arg(TR(T_AUDIO))
      .arg(player_->audioCodecName().toUpper())
      .arg(QString::number(player_->audioRate())).arg(tr("Hz"))
      .arg(QString::number(player_->audioChannelCount())).arg(tr("ch"))
    );

    // Render
    qreal contrast = player_->contrast();
    qreal brightness = player_->brightness();
    int hue = player_->hue();
    qreal saturation = player_->saturation();
    qreal gamma = player_->gamma();

    t->append(QString("- %1 -").arg(tr("Render")), Qt::red);
    t->append(QString(
         "%1: %2" HTML_BR()
         "%3: %4" HTML_BR()
         "%5: %6" HTML_BR()
         "%7: %8" HTML_BR()
         "%9: %10" HTML_BR()
       HTML_BR())
       .arg(TR(T_SATURATION)).arg(QString::number(saturation))
       .arg(TR(T_GAMMA)).arg(QString::number(gamma))
       .arg(TR(T_HUE)).arg(QString::number(hue))
       .arg(TR(T_CONTRAST)).arg(QString::number(contrast))
       .arg(TR(T_BRIGHTNESS)).arg(QString::number(brightness))
    );

    // Meta
    t->append(QString("- %1 -").arg(tr("Meta")), Qt::red);
#define META(_Name) \
    t->append(#_Name ": " + player_->meta##_Name());

    META(Title)
    META(Artist)
    META(Genre)
    META(Copyright)
    META(Album)
    META(TrackNumber)
    META(Description)
    META(Rating)
    META(Date)
    META(Setting)
    META(URL)
    META(Language)
    META(NowPlaying)
    META(Publisher)
    META(EncodedBy)
    META(ArtworkURL)
    META(TrackID)
#undef META
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
    t->append(QString("- %1 -").arg(TR(T_URL)), Qt::red);
    foreach (const QString &url, urls)
      t->append(url);
  }

  //static const size_t tail = qstrlen(HTML_BR());

  if (t->isEmpty())
    t->append(tr("No information"));
  DOUT("exit");
}

// EOF
