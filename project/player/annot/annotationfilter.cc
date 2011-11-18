// annotationfilter.cc
// 11/15/2011
#include "annotationfilter.h"
#include "core/cloud/annotation.h"
#include "core/cloud/traits.h"
#include <boost/typeof/typeof.hpp>
#include <QtCore>

using namespace Core::Cloud;

// - Constructions -

AnnotationFilter::AnnotationFilter(QObject *parent)
  : Base(parent), enabled_(true), languages_(0)
{ }

// - Properties -

qint64
AnnotationFilter::languages() const
{ return languages_; }

void
AnnotationFilter::setLanguages(qint64 bits)
{ languages_ = bits; }

bool
AnnotationFilter::isEnabled() const
{ return enabled_; }

void
AnnotationFilter::setEnabled(bool enabled)
{
  if (enabled_ != enabled) {
    enabled_ = enabled;
    emit enabledChanged(enabled_);
  }
}

const AnnotationList&
AnnotationFilter::blockedAnnotations() const
{ return blockedAnnotations_; }

const QStringList&
AnnotationFilter::blockedUserAliases() const
{ return blockedUserAliases_; }

const QStringList&
AnnotationFilter::blockedTexts() const
{ return blockedTexts_; }

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
    foreach (Annotation a, blockedAnnotations_)
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

  BOOST_AUTO(p, blockedAnnotations_.begin());
  bool update = false;
  while (p != blockedAnnotations_.end()) {
    if (p->id() == id) {
      p = blockedAnnotations_.erase(p);
      update = true;
    } else
      ++p;
  }

  if (update)
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

  // Language filter
  if (!(languages_ & Traits::AnyLanguageBit)) {
    qint64 l = 1L << input.language();
    if (!(l & (languages_ | Traits::AnyLanguageBit)))
      return true;
  }

  // Annots filter
  if (!blockedAnnotations_.isEmpty() && input.hasId())
    foreach (Annotation a, blockedAnnotations_)
      if (a.id() == input.id())
        return true;

  // Alias filter
  //if (!blockedAliases_.isEmpty())
  //  foreach (Alias a, blockedAliases_)
  //    if (a.tokenId() == input.tokenId())
  //      return true;

  // User filter
  if (!blockedUserAliases_.isEmpty() && input.hasUserAlias())
    foreach (QString n, blockedUserAliases_)
      if (n == input.userAlias())
        return true;

  // Text
  if (!blockedTexts_.isEmpty() && input.hasText())
    foreach (QString k, blockedTexts_)
      if (input.text().contains(k, Qt::CaseInsensitive))
        return true;

  return false;
}

// EOF
