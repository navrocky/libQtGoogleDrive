#ifndef GOOGLE_DRIVE_COMMAND_OAUTH2_H
#define GOOGLE_DRIVE_COMMAND_OAUTH2_H

#include <QString>
#include <QUrl>

#include "command.h"

namespace GoogleDrive
{

class CommandOAuth2Private;

/**
 * @brief First-time OAuth2 authorization for installed applications
 *
 * Authenticating users with OAuth 2.0. At the moment only supported scenario
 * for installed applications.
 *
 * @sa https://developers.google.com/drive/about-auth
 * @sa https://developers.google.com/accounts/docs/OAuth2InstalledApp
 */
class CommandOAuth2 : public Command
{
    Q_OBJECT
public:
    /**
     * Access scope.
     * @sa https://developers.google.com/drive/scopes
     */
    enum AccessScope
    {
        /**
         * Per-file access to files created or opened by the app.
         */
        FileScope = 1,

        /**
         * Allows apps read-only access to the list of Drive apps a user
         * has installed.
         */
        AppsReadonyScope = 1 << 1,

        /**
         * Allows read-only access to file metadata and file content
         */
        ReadonlyScope = 1 << 2,

        /**
         * Allows read-only access to file metadata, but does not allow any
         * access to read or download file content
         */
        ReadonlyMetadataScope = 1 << 3,

        /**
         * Full access to user drive files.
         */
        FullAccessScope = 1 << 4
    };
    Q_DECLARE_FLAGS(AccessScopes, AccessScope)

    CommandOAuth2(Session*);

    QString scope() const;
    void setScope(const QString& v);
    void setScope(AccessScopes);

    /**
     * Returns OAuth2 dialog url. You have to open this url in the web browser
     * for further authorization steps.
     * @sa QDesktopServices::openUrl
     */
    QUrl getLoginUrl() const;

public slots:
    /**
     * Requesting refresh and access tokens using authorization code retrieved
     * from browser.
     * @sa finished, Command::error()
     */
    void requestAccessToken(const QString& authCode);

private slots:
    void requestAccessTokenFinished();

private:
    Q_DECLARE_PRIVATE(CommandOAuth2)
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(GoogleDrive::CommandOAuth2::AccessScopes)

#endif
