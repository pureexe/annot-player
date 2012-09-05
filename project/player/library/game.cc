// game.cc
// 8/18/2012

#include "game.h"
#include <QtCore/QFile>
#include <QtCore/QHash>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QXmlStreamWriter>

//#define DEBUG "game"
#include "module/debug/debug.h"

namespace { // anonymous
  struct init_ { init_() {
    qRegisterMetaType<Game>("Game");
  } };
  init_ static_init_;
} // anonymois namespace

#define FMT_VERSION "0.1"
#define TAG_ROOT    "library"
#define TAG_VALUE   "game"

//#define TAG_ANCHOR      "anchor"
//#define TAG_FUNCTION    "function"
#define TAG_ENCODING    "encoding"
#define TAG_ENABLED     "enabled"
#define TAG_TOKENID     "tokenId"
#define TAG_DIGEST      "digest"
#define TAG_LOCATION    "location"
#define TAG_TITLE       "title"
#define TAG_VISITTIME   "visitTime"
#define TAG_VISITCOUNT  "visitCount"

#define TAG_THREADS     "threads"
#define TAG_THREAD      "thread"
#define TAG_ROLE        "role"
#define TAG_SIGNATURE   "signature"
#define TAG_PROVIDER    "provider"

#define TAG_HOOK        "hook"

  //ulong functionId_;
  //void setEncoding(const QString &value) { encoding_ = value; }
  //bool enabled_;

enum Hash {
  //H_Anchor = 109354850,
  //H_Function = 206218750,

  H_Encoding = 77964743,
  H_Enabled = 206017140,
  H_TokenId = 190956052,
  H_Digest = 112188580,
  H_Location = 93889086,
  H_Title = 8063781,
  H_VisitCount = 9030932,
  H_VisitTime = 168384053,

  H_Threads = 184072019,
  H_Thread = 128945012,
  H_Role = 497189,
  H_Signature = 239619941,
  H_Provider = 158135330,

  H_Hook = 456283
};

// - Properties -

bool
Game::isValidHook(const QString &code)
{ return code.size() >= 8 && code.startsWith("/H"); }

// - Read-

QList<Game>
Game::readList(const QString &fileName)
{
  DOUT("enter: fileName =" << fileName);
  QList<Game> ret;

  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text))
    return ret;

  QDomDocument doc;
  doc.setContent(file.readAll());
  if (doc.isNull()) {
    DOUT("exit: invalid document root");
    return ret;
  }
  QDomElement root = doc.firstChildElement(TAG_ROOT);
  if (root.isNull()) {
    DOUT("exit: invalid root element");
    return ret;
  }

  QDomElement e = root.firstChildElement(TAG_VALUE);
  while (!e.isNull()) {
    QDomElement c = e.firstChildElement();
    Self v;
    while (!c.isNull()) {
      switch (qHash(c.tagName())) {
      case H_Encoding:   v.setEncoding(c.text()); break;
      case H_Enabled:    v.setEnabled(c.text() == "true"); break;
      case H_TokenId:    v.setTokenId(c.text().toLongLong()); break;
      case H_Digest:     v.setDigest(c.text()); break;
      case H_Hook:       v.setHook(c.text()); break;
      case H_Location:   v.setLocation(c.text()); break;
      case H_Title:      v.setTitle(c.text()); break;
      case H_VisitTime:  v.setVisitTime(c.text().toLongLong()); break;
      case H_VisitCount: v.setVisitCount(c.text().toInt()); break;

      case H_Threads:
        {
          QDomElement te = c.firstChildElement(TAG_THREAD);
          while (!te.isNull()) {
            QDomElement tc = te.firstChildElement();
            TextThread t;
            while (!tc.isNull()) {
              switch (qHash(tc.tagName())) {
              case H_Provider:  t.setProvider(tc.text()); break;
              case H_Role:      t.setRole(tc.text().toInt()); break;
              case H_Signature: t.setSignature(tc.text().toLongLong()); break;
              default: DOUT("warning: unknown thread tag:" << tc.tagName());
              }
              tc = tc.nextSiblingElement();
            }
            if (t.hasSignature() && t.hasRole())
              v.threads().append(t);
            te = te.nextSiblingElement(TAG_THREAD);
          }
        } break;
      default: DOUT("warning: unknown tag:" << c.tagName());
      }
      c = c.nextSiblingElement();
    }

    if (v.hasThreads())
      ret.append(v);
    e = e.nextSiblingElement(TAG_VALUE);
  }
  DOUT("exit: size =" << ret.size());
  return ret;
}

QHash<QString, Game>
Game::readHash(const QString &fileName)
{
  DOUT("enter: fileName =" << fileName);
  QHash<QString, Game> ret;

  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text))
    return ret;

  QDomDocument doc;
  doc.setContent(file.readAll());
  if (doc.isNull()) {
    DOUT("exit: invalid document root");
    return ret;
  }
  QDomElement root = doc.firstChildElement(TAG_ROOT);
  if (root.isNull()) {
    DOUT("exit: invalid root element");
    return ret;
  }

  QDomElement e = root.firstChildElement(TAG_VALUE);
  while (!e.isNull()) {
    QDomElement c = e.firstChildElement();
    Self v;
    while (!c.isNull()) {
      switch (qHash(c.tagName())) {
      case H_Encoding:   v.setEncoding(c.text()); break;
      case H_Enabled:    v.setEnabled(c.text() == "true"); break;
      case H_TokenId:    v.setTokenId(c.text().toLongLong()); break;
      case H_Digest:     v.setDigest(c.text()); break;
      case H_Hook:       v.setHook(c.text()); break;
      case H_Location:   v.setLocation(c.text()); break;
      case H_Title:      v.setTitle(c.text()); break;
      case H_VisitTime:  v.setVisitTime(c.text().toLongLong()); break;
      case H_VisitCount: v.setVisitCount(c.text().toInt()); break;

      case H_Threads:
        {
          QDomElement te = c.firstChildElement(TAG_THREAD);
          while (!te.isNull()) {
            QDomElement tc = te.firstChildElement();
            TextThread t;
            while (!tc.isNull()) {
              switch (qHash(tc.tagName())) {
              case H_Provider:  t.setProvider(tc.text()); break;
              case H_Role:      t.setRole(tc.text().toInt()); break;
              case H_Signature: t.setSignature(tc.text().toLongLong()); break;
              default: DOUT("warning: unknown thread tag:" << tc.tagName());
              }
              tc = tc.nextSiblingElement();
            }
            if (t.hasSignature() && t.hasRole())
              v.threads().append(t);
            te = te.nextSiblingElement(TAG_THREAD);
          }
        } break;
      default: DOUT("warning: unknown tag:" << c.tagName());
      }
      c = c.nextSiblingElement();
    }

    QString k = v.key();
    if (!k.isEmpty() && v.hasThreads())
      ret[k] = v;
    e = e.nextSiblingElement(TAG_VALUE);
  }
  DOUT("exit: size =" << ret.size());
  return ret;
}


// - Write -

template <typename L>
bool
Game::write(const L &l, const QString &fileName)
{
  DOUT("enter: count =" << l.size() << ", fileName =" << fileName);
  //if (l.isEmpty()) {
  //  if (QFile::exists(fileName))
  //    QFile::remove(fileName);
  //  DOUT("exit: empty list");
  //  return;
  //}
  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly)) {
    DOUT("exit: ret = false, cannot  write to file");
    return false;
  }

  QXmlStreamWriter writer(&file);
  writer.setAutoFormatting(true);

  writer.writeStartDocument();
  writer.writeStartElement(TAG_ROOT);
  writer.writeAttribute("version", FMT_VERSION);

  foreach (const Self &v, l) {
    writer.writeStartElement(TAG_VALUE);

    writer.writeTextElement(TAG_TOKENID, QString::number(v.tokenId()));
    writer.writeTextElement(TAG_DIGEST, v.digest());
    writer.writeTextElement(TAG_ENCODING, v.encoding());
    writer.writeTextElement(TAG_LOCATION, v.location());
    writer.writeTextElement(TAG_ENABLED, v.isEnabled() ? "true" : "false");
    if (v.hasTitle())
      writer.writeTextElement(TAG_TITLE, v.title());
    if (v.hasHook())
    writer.writeTextElement(TAG_HOOK, v.hook());

    writer.writeTextElement(TAG_VISITTIME, QString::number(v.visitTime()));
    writer.writeTextElement(TAG_VISITCOUNT, QString::number(v.visitCount()));


    //writer.writeTextElement(TAG_ANCHOR, QString::number(v.anchor()));
    //writer.writeTextElement(TAG_FUNCTION, v.function());
    if (v.hasThreads()) {
      writer.writeStartElement(TAG_THREADS);
      foreach (const TextThread &t, v.threads()) {
        writer.writeStartElement(TAG_THREAD);
        writer.writeTextElement(TAG_PROVIDER, t.provider());
        writer.writeTextElement(TAG_SIGNATURE, QString::number(t.signature()));
        writer.writeTextElement(TAG_ROLE, QString::number(t.role()));
        writer.writeEndElement();
      }
      writer.writeEndElement();
    }

    writer.writeEndElement();
  }
  writer.writeEndElement();
  writer.writeEndDocument();

  file.close();

  bool ret = !writer.hasError() && file.error() == QFile::NoError;
  DOUT("exit: ret =" << ret);
  return ret;
}

// - Instantiate Templates -

template
bool
Game::write(const QList<Game> &l, const QString &fileName);

template
bool
Game::write(const QHash<QString, Game> &l, const QString &fileName);

// EOF
