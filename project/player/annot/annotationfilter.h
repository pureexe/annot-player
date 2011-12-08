#ifndef ANNOTATIONFILTER_H
#define ANNOTATIONFILTER_H
// annotationfilter.h
// 11/15/2011

#include "core/cloud/annotation.h"
#include <QObject>
#include <QStringList>

class AnnotationFilter : public QObject
{
  Q_OBJECT
  typedef AnnotationFilter Self;
  typedef QObject Base;

  typedef Core::Cloud::Annotation Annotation;
  typedef Core::Cloud::AnnotationList AnnotationList;

  // - Constructions -
public:
  explicit AnnotationFilter(QObject *parent = 0);

  ///  Return true if the annotation is supposed to be blocked.
  bool filter(const Annotation &a) const;

  // - Properties -

signals:
  void enabledChanged(bool t);

  void blockedAnnotationsChanged(const AnnotationList &blacklist);
  void blockedUserAliasesChanged(const QStringList &blacklist);
  void blockedTextsChanged(const QStringList &blacklist);

public:
  bool isEnabled() const;
  qint64 languages() const; ///< Allowed languages

  const AnnotationList &blockedAnnotations() const;
  const QStringList &blockedUserAliases() const;
  const QStringList &blockedTexts() const;

public slots:
  void setEnabled(bool enabled);
  void setLanguages(qint64 bits);

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
  bool enabled_;
  qint64 languages_;

  // Blacklists
  QList<Annotation> blockedAnnotations_; // NG annots
  //QList<Alias> blockedAliases_;          // NG aliases
  QStringList blockedUserAliases_;    // NG users
  QStringList blockedTexts_;          // NG text keywords
};

#endif // ANNOTATIONFILTER_H
