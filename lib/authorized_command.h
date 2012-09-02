#ifndef GOOGLE_DRIVE_AUTHORIZED_COMMAND_H
#define GOOGLE_DRIVE_AUTHORIZED_COMMAND_H

#include "command.h"

namespace GoogleDrive
{

class AuthorizedCommand : public Command
{
    Q_OBJECT
public:
    AuthorizedCommand(Session* session);

signals:
    /*! This signal is emitted when full reauthorization is needed.
     *  \param reason - reauthorization reason.
     */
    void reauthorizationNeeded(const QString& reason);

protected:
    void executeQuery();
    virtual void reexecuteQuery() = 0;

    bool checkForRefreshToken(const QVariantMap&);
    bool checkInvalidReplyAndRefreshToken(QNetworkReply* reply);
    void refreshToken();

private slots:
    void refreshTokenFinished();
    void refreshTokenFailed(const QString&);
};

}

#endif
