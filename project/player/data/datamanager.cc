// datamanager.cc
// 12/5/2011

#include "datamanager.h"
#include <QtGui>
#include <boost/typeof/typeof.hpp>

using namespace AnnotCloud;

// - User -

void
DataManager::setUser(const User &user)
{
  user_ = user;
  emit userChanged(user_);
}

// - Token -

void
DataManager::setToken(const Token &token)
{
  token_ = token;
  emit tokenChanged(token_);
}

// - Alias -

void
DataManager::setAliases(const AliasList &aliases)
{
  aliases_ = aliases;
  emit aliasesChanged(aliases_);
}

void
DataManager::addAlias(const Alias &alias)
{
  aliases_.append(alias);
  emit aliasAdded(alias);
}

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
DataManager::setAnnotations(const AnnotationList &annots)
{
  annots_ = annots;
  emit annotationsChanged(annots_);
}

void
DataManager::addAnnotation(const Annotation &annot)
{
  annots_.append(annot);
  emit annotationAdded(annot);
}

void
DataManager::updateAnnotation(const Annotation &annot)
{
  if (!annots_.isEmpty() && annot.hasId()) {
    BOOST_AUTO(p, annots_.begin());
    while (p != annots_.end())
      if (p->id() == annot.id())
        p = annots_.erase(p);
      else
        ++p;
  }
  annots_.append(annot);
  emit annotationUpdated(annot);
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
