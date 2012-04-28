// datamanager.cc
// 12/5/2011

#include "datamanager.h"
#include <QtGui>
#include <boost/typeof/typeof.hpp>
#include <climits>

using namespace AnnotCloud;

// - Construction -

DataManager::DataManager(QObject *parent)
  : Base(parent), minAnnotTime_(LLONG_MAX), maxAnnotTime_(0)
{ }

// - Alias -

void
DataManager::updateAlias(const Alias &alias)
{
  if (!aliases_.isEmpty() && alias.hasId()) {
    BOOST_AUTO(p, aliases_.begin());
    while (p != aliases_.end())
      if (p->id() == alias.id())
        p = aliases_.erase(p);
      else
        ++p;
  }
  aliases_.append(alias);
  emit aliasUpdated(alias);
}

void
DataManager::removeAliasWithId(qint64 id)
{
  if (!aliases_.isEmpty() && id) {
    BOOST_AUTO(p, aliases_.begin());
    while (p != aliases_.end())
      if (p->id() == id)
        p = aliases_.erase(p);
      else
        ++p;
  }
  emit aliasRemovedWithId(id);
}

// - Annotation -

void
DataManager::invalidateAnnotations()
{
  maxAnnotTime_ = 0;
  minAnnotTime_ = LLONG_MAX;

  userAnnotCount_.clear();
  foreach (const Annotation &a, annots_) {
    ++userAnnotCount_[a.userId()];
    qint64 t = a.createTime();
    if (t > Traits::MIN_TIME && t < Traits::MAX_TIME) {
      if (t > maxAnnotTime_)
        maxAnnotTime_ = t;
      if (t < minAnnotTime_)
        minAnnotTime_ = t;
    }
  }

  emit annotationsChanged(annots_);
}

void
DataManager::addAnnotation(const Annotation &a)
{
  annots_.append(a);
  ++userAnnotCount_[a.userId()];

  qint64 t = a.createTime();
  if (t > Traits::MIN_TIME
#ifndef Q_WS_WIN
      && t < Traits::MAX_TIME // FIXME
#endif // Q_WS_WIN
      ) {
    if (t > maxAnnotTime_)
      maxAnnotTime_ = t;
    if (t < minAnnotTime_)
      minAnnotTime_ = t;
  }
  emit annotationAdded(a);
}

void
DataManager::updateAnnotation(const Annotation &a)
{
  if (!annots_.isEmpty() && a.hasId()) {
    BOOST_AUTO(p, annots_.begin());
    while (p != annots_.end())
      if (p->id() == a.id())
        p = annots_.erase(p);
      else
        ++p;
  }
  annots_.append(a);
  emit annotationUpdated(a);
}

void
DataManager::removeAnnotationWithId(qint64 id)
{
  if (!annots_.isEmpty() && id) {
    BOOST_AUTO(p, annots_.begin());
    while (p != annots_.end())
      if (p->id() == id)
        p = annots_.erase(p);
      else
        ++p;
  }
  emit annotationRemovedWithId(id);
}

// - Update -

void
DataManager::updateAnnotationTextWithId(const QString &text, qint64 id)
{
  if (!annots_.isEmpty() && id) {
    BOOST_AUTO(p, annots_.begin());
    while (p != annots_.end())
      if (p->id() == id) {
        p->setText(text);
        p->setUpdateTime(QDateTime::currentMSecsSinceEpoch() / 1000);
        break;
      } else
        ++p;

    emit annotationTextUpdatedWithId(text, id);
  }
}

bool
DataManager::aliasConflicts(const Alias &input) const
{
  if (aliases_.isEmpty() || !input.hasText())
    return false;

  foreach (const Alias &a, aliases_)
    if (a.type() == input.type() &&
        !a.text().compare(input.text(), Qt::CaseInsensitive))
      return true;
  return false;
}

// EOF
