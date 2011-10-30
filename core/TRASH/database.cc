// core/database.cc
// 7/24/2011

#include "core/database.h"
#include "core/comment.h"
#include <QtCore>

namespace Core {

  // - Constructions -

  DatabaseConnection::DatabaseConnection(QObject *parent)
    : Base(parent) { }

  DatabaseConnection::~DatabaseConnection() { deleteCache();}

  void
  DatabaseConnection::invalidateCache()
  { deleteCache(); }

  void
  DatabaseConnection::deleteCache()
  {
    if (!comments_.isEmpty()) {
      foreach (auto p, comments_)
        delete p;
      comments_.clear();
    }
  }

  // - Connections - TODO: not implemented yet

  bool
  DatabaseConnection::isActive() const
  { return true;  }

  void
  DatabaseConnection::start()
  { }

  void
  DatabaseConnection::stop()
  { }

  // - Submitions -

  bool
  DatabaseConnection::postComment(const Comment *comment)
  {
    if (!comment || !comment->isValid())
      return false;

    comments_.insert(comment);
    return true;
  }


} // namespace Core

// EOF
