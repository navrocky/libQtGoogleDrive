#ifndef GOOGLE_DRIVE_SESSION_H
#define GOOGLE_DRIVE_SESSION_H

#include <QObject>

class QNetworkAccessManager;

namespace GoogleDrive
{

class Command;
class CommandOAuth2;
class CommandRefreshToken;
class SessionPrivate;

class Session : public QObject
{
    Q_OBJECT
public:
    /**
     * Session constructor.
     * @param manager QNetworkAccessManager used to executing network queries.
     *        Session don't own provided manager.
     * @param parent
     */
    Session(QNetworkAccessManager* manager, QObject* parent = 0);
    ~Session();

    /**
     * Returns assigned QNetworkAccessManager
     */
    QNetworkAccessManager* networkManager() const;

    QString clientId() const;
    void setClientId(const QString& v);

    QString clientSecret() const;
    void setClientSecret(const QString& v);

    QString refreshToken() const;
    void setRefreshToken(const QString& v);

    QString accessToken() const;

    /**
     * Returns number of seconds after access_token will be expired.
     * This value is valid after first time authorization or after executing any command.
     */
    int accessTokenExpiresIn() const;

signals:
    /**
     * This signal is emitted when command started.
     * @param command started to execution command.
     */
    void started(GoogleDrive::Command* command);

    /**
     * Signal is emitted when command is finished successfully or failed.
     * @param successCommand a command that execution is finished.
     * @sa Command::error, Command::errorString
     */
    void finished(GoogleDrive::Command* successCommand);

    /**
     * Signal is emitted when command execution is failed.
     * @param successCommand a command that execution is failed.
     * @sa Command::error, Command::errorString
     */
    void error(GoogleDrive::Command* failedCommand);

private:
    friend class Command;
    friend class CommandOAuth2;
    friend class CommandRefreshToken;

    void setAccessToken(const QString& v);
    void setAccessTokenExpiresIn(int);

    SessionPrivate* d_ptr;
    Q_DECLARE_PRIVATE(Session)
};

}

#endif
