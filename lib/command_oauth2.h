#ifndef GOOGLE_DRIVE_COMMAND_OAUTH2_H
#define GOOGLE_DRIVE_COMMAND_OAUTH2_H

#include <QString>
#include <QUrl>

#include "command.h"

namespace GoogleDrive
{

/*! \brief First-time authorization
 *
 * Authenticating users with OAuth 2.0
 *
 * \see https://developers.google.com/drive/about-auth
 */
class CommandOAuth2 : public Command
{
    Q_OBJECT
public:
    /*! GD Access scope.
     * \see https://developers.google.com/drive/scopes
     */
    enum AccessScope
    {
        /*! Per-file access to files created or opened by the app */
        FileScope = 1,

        /*! Allows apps read-only access to the list of Drive apps a user
         *  has installed. */
        AppsReadonyScope = 1 << 1,

        /*! Allows read-only access to file metadata and file content */
        ReadonlyScope = 1 << 2,

        /*! Allows read-only access to file metadata, but does not allow any
         *  access to read or download file content */
        ReadonlyMetadataScope = 1 << 3,

        /*! Full access to user drive files. */
        FullAccessScope = 1 << 4
    };
    Q_DECLARE_FLAGS(AccessScopes, AccessScope)

    CommandOAuth2(Session*);

    QString clientId() const {return clientId_;}
    void setClientId(const QString& v) {clientId_ = v;}

    QString clientSecret() const {return clientSecret_;}
    void setClientSecret(const QString& v) {clientSecret_ = v;}

    QString scope() const {return scope_;}
    void setScope(const QString& v) {scope_ = v;}
    void setScope(AccessScopes);

    /*! Returns OAuth2 dialog url. You have to open this url in the web browser
     *  for further authorization steps.
        \sa QDesktopServices::openUrl() */
    QUrl getLoginUrl() const;

signals:
    /*! Signal emitted when requestAuthToken() query returns valid refresh token
     *  and access token.
        \sa requestAuthToken() */
    void finished();

public slots:
    /*! Requesting refresh and access tokens using authorization code retrieved
        from browser. Signal finished() emitting when request finished successfully
        otherwise error() emitted.
        \sa emit(), Command::error()*/
    void requestAuthToken(const QString& authCode);

private slots:
    void requestAuthTokenFinished();

private:
    QString clientId_;
    QString clientSecret_;
    QString scope_;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(GoogleDrive::CommandOAuth2::AccessScopes)

#endif
