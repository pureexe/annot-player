// annotationfilter.cc
// 11/15/2011
#include "annotationfilter.h"
#include "datamanager.h"
#include "module/annotcloud/annotation.h"
#include "module/annotcloud/traits.h"
#include <QtCore>
#include <ctime>

using namespace AnnotCloud;

// - Properties -

void
AnnotationFilter::setEnabled(bool enabled)
{
  if (enabled_ != enabled) {
    enabled_ = enabled;
    emit enabledChanged(enabled_);
  }
}

void
AnnotationFilter::clear()
{
  clearBlockedAnnotations();
  clearBlockedUserAliases();
  clearBlockedTexts();
}

void
AnnotationFilter::clearBlockedAnnotations()
{ blockedAnnotations_.clear(); }

void
AnnotationFilter::clearBlockedUserAliases()
{ blockedUserAliases_.clear(); }

void
AnnotationFilter::clearBlockedTexts()
{ blockedTexts_.clear(); }

void
AnnotationFilter::setBlockedAnnotations(const AnnotationList &l)
{
  blockedAnnotations_ = l;
  emit blockedAnnotationsChanged(blockedAnnotations_);
}

void
AnnotationFilter::setBlockedUserAliases(const QStringList &l)
{
  blockedUserAliases_ = l;
  emit blockedUserAliasesChanged(blockedUserAliases_);
}

void
AnnotationFilter::setBlockedTexts(const QStringList &l)
{
  blockedTexts_ = l;
  emit blockedTextsChanged(blockedTexts_);
}

void
AnnotationFilter::addBlockedAnnotation(const Annotation &input)
{
  if (!blockedAnnotations_.isEmpty())
    foreach (const Annotation &a, blockedAnnotations_)
      if (a.id() == input.id())
        return;

  blockedAnnotations_.append(input);
  emit blockedAnnotationsChanged(blockedAnnotations_);
}

void
AnnotationFilter::addBlockedUserAlias(const QString &name)
{
  if (!blockedUserAliases_.contains(name)) {
    blockedUserAliases_.append(name);
    emit blockedUserAliasesChanged(blockedUserAliases_);
  }
}

void
AnnotationFilter::addBlockedText(const QString &k)
{
  if (!blockedTexts_.contains(k)) {
    blockedTexts_.append(k);
    emit blockedTextsChanged(blockedTexts_);
  }
}

void
AnnotationFilter::removeBlockedAnnotationWithId(qint64 id)
{
  if (blockedAnnotations_.isEmpty())
    return;

  int size = blockedAnnotations_.size();
  QMutableListIterator<Annotation> i(blockedAnnotations_);
  while (i.hasNext())
    if (i.next().id() == id)
      i.remove();

  if (size != blockedAnnotations_.size())
    emit blockedAnnotationsChanged(blockedAnnotations_);
}

void
AnnotationFilter::removeBlockedUserAlias(const QString &name)
{
  if (blockedUserAliases_.removeAll(name))
    emit blockedUserAliasesChanged(blockedUserAliases_);
}

void
AnnotationFilter::removeBlockedText(const QString &k)
{
  if (blockedTexts_.removeAll(k))
    emit blockedTextsChanged(blockedTexts_);
}

// - Filter -

bool
AnnotationFilter::filter(const Annotation &input) const
{
  if (!enabled_)
    return false;
  if (dm_->user().id() == input.userId())
    return false;

  // Language filter
  if (!languages_.isEmpty() && input.language() &&
      languages_.contains(input.language()))
    return true;

  // Annots filter
  if (!blockedAnnotations_.isEmpty() && input.hasId())
    foreach (const Annotation &a, blockedAnnotations_)
      if (a.id() == input.id())
        return true;

  // Alias filter
  //if (!blockedAliases_.isEmpty())
  //  foreach (const Alias &a, blockedAliases_)
  //    if (a.tokenId() == input.tokenId())
  //      return true;

  // User filter
  if (!blockedUserAliases_.isEmpty() && input.hasUserAlias())
    foreach (const QString &n, blockedUserAliases_)
      if (n == input.userAlias())
        return true;

  // Text
  if (!blockedTexts_.isEmpty() && input.hasText())
    foreach (const QString &k, blockedTexts_)
      if (input.text().contains(k, Qt::CaseInsensitive))
        return true;

  if (annotationCountHint_)
    return !maybeSignificant(input);

  return false;
}

bool
AnnotationFilter::maybeSignificant(const Annotation &a) const
{
  int total = dm_->annotations().count();
  if (total < annotationCountHint_)
    return true;
  if (a.isBlocked())
    return false;
  if (a.isBlessed() || a.isCursed())
    return true;
  if (annotationCountHint_ <= 0)
    return true;

  int r = qrand();
  // TODO: make time to increase probability here.
  //qint64 t = a.createTime() / 1000; // seconds
  //if (t > 0) {
  //  qint64 current = ::time(0);
  //  if (t >= current)
  //    return true;
  //
  //  enum { week = 86400 * 7 };
  //  double p = (double)t/current;
  //  int delta = total - annotationCountHint_;
  //  total -=
  //}
  if (r % total < annotationCountHint_)
    return false;

  return true;
}

// EOF
