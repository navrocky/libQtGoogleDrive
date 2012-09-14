#ifndef GOOGLE_DRIVE_AUTHORIZED_COMMAND_H
#define GOOGLE_DRIVE_AUTHORIZED_COMMAND_H

#include "command.h"

namespace GoogleDrive
{

/**
 * @brief Base class of all commands for which authorization is necessary.
 */
class AuthorizedCommand : public Command
{
    Q_OBJECT
public:
    AuthorizedCommand(Session* session);

protected:
    void executeQuery();
    virtual void reexecuteQuery() = 0;

    bool checkForRefreshToken(const QVariantMap&);
    bool checkInvalidReplyAndRefreshToken(QNetworkReply* reply);
    void refreshToken();

    AuthorizedCommand(CommandPrivate*, Session*);

private slots:
    void refreshTokenFinished();
};

}

#endif
