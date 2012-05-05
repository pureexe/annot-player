#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

// searchengine.h
// 4/25/2012

#include <QtCore/QObject>
#include <QtCore/QStringList>

class SearchEngine : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(SearchEngine)
  typedef SearchEngine Self;
  typedef QObject Base;

  int id_;
  QString query_;
  QString name_;
  QString icon_;
  QStringList acronyms_;

public:
  SearchEngine(int id, const QString &queryUrl, const QString &name, const QString &icon, const QStringList &acronyms, QObject *parent = 0)
    : Base(parent), id_(id), query_(queryUrl), name_(name), icon_(icon), acronyms_(acronyms) { }

  int id() const { return id_; }
  void setId(int id) { id_ = id; }

  const QString &name() const { return name_; }
  void setName(const QString &name) { name_ = name; }

  const QString &queryUrl() const { return query_; }
  void setQueryUrl(const QString &url) { query_ = url; }

  const QString &icon() const { return icon_; }
  void setIcon(const QString &url) { icon_ = url; }

  const QStringList &acronyms() const { return acronyms_; }
  void setAcronyms(const QStringList &l) { acronyms_ = l; }
  bool hasAcronyms() const { return !acronyms_.isEmpty(); }

  QString search(const QString &key) const { return query_.arg(key); }
};

#endif // WBSEARCHENGINE_H
