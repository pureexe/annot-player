// youkumrlresolver.cc
// 1/24/2012

#include "youkumrlresolver.h"
#include <QtCore>
#include <QtScript>
#include <QtNetwork>
#include <cstdlib>

//#define DEBUG "youkumrlresolver"
#include "module/debug/debug.h"

// See: jd/plugins/hoster/YoukuCom.java
// See: jd/plugins/decrypter/YkCm.java
// See: http://3shi.net/analyze-youku-video-address/
// See: http://www.pocketdigi.com/20111108/572.html

// Examples:
// http://v.youku.com/v_show/id_XMzQ1NzM1NDM2.html
// http://v.youku.com/player/getPlayList/VideoIDS/XMzQ1NzM1NDM2
// http://f.youku.com/player/getFlvPath/sid/132745855314611463047_00/st/flv/fileid/03000108004F1F454D939903BAF2B1D3A1A504-3603-2115-A3B8-8D7F07A4D1FE?K=e305b0075d69ca5c2410e127&hd=2&myp=0&ts=368
// http://f.youku.com/player/getFlvPath/sid/132747676347914796333_06/st/flv/fileid/03000108064F1F454D939903BAF2B1D3A1A504-3603-2115-A3B8-8D7F07A4D1FE?K=11e3f58a96d7bd932410e136&hd=2&myp=0&ts=219
// http://f.youku.com/player/getFlvPath/sid/132747732759415941579_07/st/mp4/fileid/03000808074F1F160B939903BAF2B1D3A1A504-3603-2115-A3B8-8D7F07A4D1FE?K=fb81be0040730165261c2206&hd=1&myp=0&ts=253

// - Construction -

YoukuMrlResolver::YoukuMrlResolver(QObject *parent)
  : Base(parent)
{
  qnam_ = new QNetworkAccessManager(this);
  connect(qnam_, SIGNAL(finished(QNetworkReply*)), SLOT(resolveMedia(QNetworkReply*)));
}

// - Analysis -

bool
YoukuMrlResolver::matchMedia(const QString &href) const
{
  QString pattern("http://v.youku.com/v_show/id_");
  return href.startsWith(pattern, Qt::CaseInsensitive);
}

void
YoukuMrlResolver::resolveMedia(const QString &href)
{
  static const QString errorMessage = tr("failed to resolve URL");

  QRegExp rx("v.youku.com/v_show/id_([a-z0-9]+)", Qt::CaseInsensitive);
  if (rx.indexIn(href) < 0) {
    emit errorReceived(errorMessage + ": " + href);
    return;
  }
  QString id = rx.cap(1);

  QString url = "http://v.youku.com/player/getPlayList/VideoIDS/" + id;
  emit messageReceived(tr("resolving media URL ...") + ": " + url);

  DOUT("url =" << url);
  QNetworkRequest request(url);
  qnam_->get(request);
}

void
YoukuMrlResolver::resolveMedia(QNetworkReply *reply)
{
  DOUT("enter");
  static const QString resolveErrorMessage = tr("failed to resolve URL");

  Q_ASSERT(reply);
  reply->deleteLater();
  if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
    //emit errorReceived(reply->errorString());
    emit errorReceived(tr("network error, failed to resolve media URL") + ": " + reply->url().toString());
    DOUT("exit: error =" << reply->error());
    return;
  }

  QString json(reply->readAll());
  //if (json.isEmpty())
  DOUT("json:" << json);

  // json parser: http://qtwiki.org/Parsing_JSON_with_QT_using_standard_QT_library
  QScriptEngine engine;
  QScriptValue root = engine.evaluate("(" + json + ")");
  //if (!root.isValid())

  // data => ( seed, title, streamfileids,  segs { } )
  QScriptValue data = root.property("data");
  if (!data.isValid())
    emit errorReceived(resolveErrorMessage + ": " + reply->url().toString());
  else {
    QScriptValueIterator it(data);
    it.next();
    if (!it.hasNext())
      emit errorReceived(resolveErrorMessage + ": " + reply->url().toString());
    else while (it.hasNext()) {
      QScriptValue dataValue = it.value();

      QStringList types;
      QScriptValue streamTypes = dataValue.property("streamtypes");
      if (!streamTypes.isArray()) {
        emit errorReceived(resolveErrorMessage + ": " + reply->url().toString());
        continue;
      }
      qScriptValueToSequence(streamTypes, types);

      QScriptValue streamFileIds = dataValue.property("streamfileids");
      long seed = dataValue.property("seed").toUInt32();

      QString title = dataValue.property("title").toString();
      DOUT("title =" << title);

      QString vid = dataValue.property("vidEncoded").toString();

      foreach (QString type, types) {
        //if (types.size() == 1 && type == "flvhd")
        //  type = "flv";
        if (type == "flvhd" && !types.contains("flv"))
          type = "flv";

        if (!preferredType_.isEmpty() &&
            type != preferredType_)
          continue;

        QString hd = "0", st = "flv";
        if (type == ("mp4")) {
          hd = "1";
          st = "mp4";
        } else if (type == "hd2")
          hd = "2";

        QScriptValue segs = dataValue.property("segs");
        QScriptValue seq = segs.property(type);
        if (!seq.isArray()) {
          QString reason = dataValue.property("error").toString();
          if (reason.isEmpty())
            reason = reply->url().toString();
          emit errorReceived(resolveErrorMessage + ": " + reason);
          continue;
        }

        QString fileId = streamFileIds.property(type).toString();
        if (fileId.isEmpty()) {
          QString reason = dataValue.property("error").toString();
          if (reason.isEmpty())
            reason = reply->url().toString();
          emit errorReceived(resolveErrorMessage + ": " + reason);
          continue;
        }
        fileId = decodeFileId(fileId, seed);
        if (fileId.size() < 10) {
          QString reason = dataValue.property("error").toString();
          if (reason.isEmpty())
            reason = reply->url().toString();
          emit errorReceived(resolveErrorMessage + ": " + reason);
          continue;
        }

        QString sid = newSid();

        MediaInfo mi;

        Q_ASSERT(seq.isArray());
        quint32 len = seq.property("length").toUInt32();
        for (quint32 i = 0; i < len; i++) {
          QScriptValue item = seq.property(i);
          QString part = QString::number(i, 16);
          if (part.size() < 2)
            part = "0" + part;

          QString k = item.property("k").toString();

          QString ts = item.property("seconds").toString();
          QString sz = item.property("size").toString();

          QString fid = fileId.left(8) + part.toUpper() + fileId.mid(10);

          QString url = "http://f.youku.com/player/getFlvPath" \
            "/sid/" + sid + "_" + part +
            "/st/" + st + "/fileid/" + fid +
            "?K=" + k + "&hd=" + hd + "&myp=0&ts=" + ts ;

          qint64 duration = ts.toLongLong() * 1000;
          qint64 size = sz.toLongLong();
          mi.mrls.append(MrlInfo(url, duration, size));
        }

        if (mi.mrls.isEmpty()) {
          emit errorReceived(resolveErrorMessage + ": " + reply->url().toString());
          continue;
        }

        DOUT("mrls =" << mi.mrls);

        QString href = "http://v.youku.com/v_show/id_" + vid + ".html";
        DOUT("href =" << href);
        mi.refurl = href;
        mi.title = title;
        // FIXME: for debugging only!
        emit mediaResolved(mi, 0);
        break;
      }

      it.next();
    }
  }

  DOUT("exit");
}

// - Helpers -

QString
YoukuMrlResolver::newSid()
{
  QString fmt("%1%2%3");

  qint64 i1 = QDateTime::currentMSecsSinceEpoch();
  int i2 = 1000 + 999 * ::rand()/RAND_MAX;
  int i3 = 1000 + 9000 * ::rand()/RAND_MAX;

  QString ret = fmt.arg(QString::number(i1))
                   .arg(QString::number(i2))
                   .arg(QString::number(i3));
  return ret;
}

QString
YoukuMrlResolver::decodeFileIdAlphabet(long seed)
{
  static const char alphabet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ/\\:._-1234567890";
  enum { alphabet_count = sizeof(alphabet)/sizeof(*alphabet) };

  QString ret;

  QString source(alphabet);
  for (int i = 0; i < alphabet_count; i++) {
    seed = (seed * 211 + 30031) % 65536;
    int pos = source.size() * seed / 65536;
    ret.append(source[pos]);
    source.remove(pos, 1);
  }
  return ret;
}

QString
YoukuMrlResolver::decodeFileId(QString fileId, long seed)
{
  DOUT("enter: fileId =" << fileId << ", seed =" << seed);
  QString ret;
  QString alphabet = decodeFileIdAlphabet(seed);
  DOUT("alphabet =" << alphabet);
  QStringList ids = fileId.split('*');
  foreach (QString id, ids) {
    if (id.isEmpty())
      continue;
    bool ok;
    int i = id.toInt(&ok);
    Q_ASSERT(ok);
    if (!ok) {
      DOUT("exit: ERROR: invalid id =" << id);
      return QString();
    }
    ret.append(alphabet[i]);
  }
  DOUT("exit: ret =" << ret);
  return ret;
}

// EOF
