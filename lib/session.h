#ifndef GOOGLE_DRIVE_SESSION_H
#define GOOGLE_DRIVE_SESSION_H

#include <QObject>
#include <QPointer>

class QNetworkAccessManager;

/*!  */
namespace GoogleDrive
{

class Command;
class AuthorizedCommand;

class Session : public QObject
{
    Q_OBJECT
public:
    Session(QNetworkAccessManager* manager, QObject* parent = 0);

    /*! Returns assigned QNetworkAccessManager. */
    QNetworkAccessManager* networkManager() const;

    QString clientId() const {return clientId_;}
    void setClientId(const QString& v) {clientId_ = v;}

    QString clientSecret() const {return clientSecret_;}
    void setClientSecret(const QString& v) {clientSecret_ = v;}

    QString accessToken() const {return accessToken_;}
    void setAccessToken(const QString& v) {accessToken_ = v;}

    QString refreshToken() const {return refreshToken_;}
    void setRefreshToken(const QString& v) {refreshToken_ = v;}

signals:
    void started(GoogleDrive::Command* command);

    /*! Signal is emitted when command is finished successfully.
        \param successCommand - a command that execution is finished.*/
    void finished(GoogleDrive::Command* successCommand);

    /*! Signal is emitted when command execution is failed.
        \param successCommand - a command that execution is failed.
        \param msg - error message.*/
    void error(GoogleDrive::Command* failedCommand, const QString& msg);

    /*! This signal is emitted when reauthorization is needed.
     *  \param failedCommand - a command that has not been executed due to lack
     *         of authorization.
     *  \param msg - reauthorization reason. */
    void reauthorizationNeeded(GoogleDrive::Command* failedCommand, const QString& msg);

private:
    friend class Command;
    friend class AuthorizedCommand;

    QPointer<QNetworkAccessManager> manager_;
    QString clientId_;
    QString clientSecret_;
    QString accessToken_;
    QString refreshToken_;
};

}

#endif
