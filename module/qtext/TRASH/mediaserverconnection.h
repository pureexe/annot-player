#ifndef _CORE_MEDIASERVERCONNECTION_H
#define _CORE_MEDIASERVERCONNECTION_H

// core/mediaserverconnection.h
// 8/4/2011

#include "core/universe/mediatoken.h"
#include "core/universe/mediaannotation.h"
#include "core/universe/user.h"

#include <QtXml/QDomDocument>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
QT_END_NAMESPACE

namespace Core {

  class MediaServerConnection : public QObject
  {
    Q_OBJECT
    Q_DISABLE_COPY(MediaServerConnection)
    typedef MediaServerConnection Self;
    typedef QObject Base;

    QNetworkAccessManager *qnam_;

    Universe::User user_;
    bool login_;

    // -- Types --
  public:
    typedef Universe::User User;
    typedef Universe::UserList UserList;
    typedef Universe::MediaToken MediaToken;
    typedef Universe::MediaTokenList MediaTokenList;
    typedef Universe::MediaAnnotation MediaAnnotation;
    typedef Universe::MediaAnnotationList MediaAnnotationList;

    // -- Errors --
  public:
    ///  Conventions: 0: succeed, <0: local error, >0: remote error
    enum Error { NoError = 0, TimeOutError = -1 };

    QString errorToString(int error) const;

    // -- Constructions --
  public:
    explicit MediaServerConnection(QObject *parent = nullptr);
    ~MediaServerConnection();

    // -- Login --
  public:
    const User &user() const; ///< Logged in user
    void setUser(const User &user); ///< \internal

    qint64 userId() const;
    const QString &userName() const;
    const QString &userPassword() const;

    bool isLoggedIn() const;
  signals:
    void loginRequested();
    void loginFailed();
    void loginSucceeded();

    void logoutRequested();
    void logoutFinished();

  public slots:
    void login(const QString &userName, const QString &password);
    void login(); // Login using last userName and password
    void logout();

    // -- Query/submit --

    // Maybe, it is better to use another Reply class like QNetworkReply!
  signals:
    void tokensReceived(const MediaTokenList &tokens);
    void annotationsReceived(const MediaAnnotationList &annots);
    void errorReceived(int error);
  public:
    // TODO: Make source insertion into user API!!!!
    void submitToken(const MediaToken &token, const QString &source);
    void queryClusteredTokensByTokenId(qint64 tid);
    //void queryClusteredTokensByHash(const QString &hash);
    void submitAnnotation(const MediaAnnotation &annot);
    void queryAnnotationsByTokenId(qint64 tid);

  public slots:
    void processServerReply(QNetworkReply *reply); // TO BE REFINED!!!!

  protected:
    static MediaTokenList parseTokens(const QDomDocument &doc);
    static MediaAnnotationList parseAnnotations(const QDomDocument &doc);
    static UserList parseUsers(const QDomDocument &doc);
    static int parseError(const QDomDocument &doc);

    // -- Helpers --
  public:
    static bool isValidUserName(const QString &userName);
    static bool isValidUserPassword(const QString &password);
  };

} // namespace Core

#endif // _CORE_MEDIASERVERCONNECTION_H
