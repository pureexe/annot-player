#ifndef _CORE_DATABASE_H
#define _CORE_DATABASE_H

// core/database.h
// 7/24/2011

#include <QtCore/QObject>
#include <QtCore/QSet>

namespace Core {

  class Comment;

  class DatabaseConnection : public QObject
  {
    Q_OBJECT
    Q_DISABLE_COPY(DatabaseConnection)
    typedef DatabaseConnection Self;
    typedef QObject Base;

    QSet<const Comment*> comments_;

    // Constructions
  public:
    explicit DatabaseConnection(QObject *parent = nullptr);
    ~DatabaseConnection();

  protected:
    void deleteCache();      ///< Remove cached comments.
  public slots:
    void invalidateCache();

    // -- Properties --
  public:
    //void setUser();
    // setMedia
    //void setLogin();
    // ... TODO, implement this using SSL

    bool isActive() const;
  public slots:
    void start();
    void stop();

    // -- Streaming --
  public:
    //void setStreamInterval;
  signals:

  public:
    // -- Submission --

    ///  \p comment must be allocated on the heap.
    bool postComment(const Comment *comment);

    // -- Queries --
  public:
    //queryComments(qint64 time, qint64 delta);

  };

} // namespace Core

#endif // _CORE_DATABASE_H
