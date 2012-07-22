#ifndef ANNOTATIONFILTER_H
#define ANNOTATIONFILTER_H
// annotationfilter.h
// 11/15/2011

#include "module/annotcloud/annotation.h"
#include <QtCore/QObject>
#include <QtCore/QStringList>

class DataManager;

class AnnotationFilter : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationFilter)
  typedef AnnotationFilter Self;
  typedef QObject Base;

  typedef AnnotCloud::Annotation Annotation;
  typedef AnnotCloud::AnnotationList AnnotationList;

  // - Constructions -
public:
  explicit AnnotationFilter(DataManager *dm, QObject *parent = nullptr)
    : Base(parent), dm_(dm), enabled_(false), languages_(0), annotationCountHint_(0)
  { Q_ASSERT(dm_); }

  ///  Return true if the annotation is supposed to be blocked.
  bool filter(const Annotation &a) const;

  bool maybeSignificant(const Annotation &a) const;

  // - Properties -

signals:
  void enabledChanged(bool t);

  void blockedAnnotationsChanged(const AnnotationList &blacklist);
  void blockedUserAliasesChanged(const QStringList &blacklist);
  void blockedTextsChanged(const QStringList &blacklist);

public:
  bool isEnabled() const { return enabled_; }
  qint64 languages() const { return languages_; } ///< Allowed languages

  const AnnotationList &blockedAnnotations() const { return blockedAnnotations_; }
  const QStringList &blockedUserAliases() const { return blockedUserAliases_; }
  const QStringList &blockedTexts() const { return blockedTexts_; }

  int annotationCountHint() const { return annotationCountHint_; }

public slots:
  void setEnabled(bool enabled);
  void setLanguages(qint64 bits) { languages_ = bits; }

  void setAnnotationCountHint(int count = 0)
  { annotationCountHint_ = count > 0 ? count : 0; }

  void clearBlockedAnnotations();
  void clearBlockedUserAliases();
  void clearBlockedTexts();
  void clear(); ///< Clear all

  void addBlockedAnnotation(const Annotation &a);
  void addBlockedUserAlias(const QString &name);
  void addBlockedText(const QString &k);
  void removeBlockedAnnotationWithId(qint64 id);
  void removeBlockedUserAlias(const QString &name);
  void removeBlockedText(const QString &k);

  void setBlockedAnnotations(const AnnotationList &l);
  void setBlockedUserAliases(const QStringList &l);
  void setBlockedTexts(const QStringList &l);

private:
  DataManager *dm_;
  bool enabled_;
  qint64 languages_;

  int annotationCountHint_;

  // Blacklists
  QList<Annotation> blockedAnnotations_; // NG annots
  //QList<Alias> blockedAliases_;          // NG aliases
  QStringList blockedUserAliases_;    // NG users
  QStringList blockedTexts_;          // NG text keywords
};

#endif // ANNOTATIONFILTER_H
