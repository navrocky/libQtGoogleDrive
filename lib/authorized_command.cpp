#include "authorized_command.h"

#include <QNetworkReply>

#include "command_refresh_token.h"
#include "session.h"

namespace GoogleDrive
{

AuthorizedCommand::AuthorizedCommand(Session* session)
    : Command(session)
{
}

bool AuthorizedCommand::checkForRefreshToken(const QVariantMap& map)
{
    const QString cError("error");
    if (!map.contains(cError))
        return false;

    const QVariantMap& errorMap = map[cError].toMap();
    const QString& code = errorMap["code"].toString();
    const QString& message = errorMap["message"].toString();

    if (code == "403")
    {
//        refreshToken();
//        return true;
        // full reauthorization is needed
        refreshTokenFailed(message);
        return true;
    }

    if (code != "401")
        return false;

    refreshToken();
    return true;
}

bool AuthorizedCommand::checkInvalidReplyAndRefreshToken(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
        return false;

    QVariantMap map;
    if (parseJsonReply(reply, map))
    {
        if (checkForRefreshToken(map))
            return true;

        if (checkJsonReplyError(map))
            return true;
    }
    emitError(reply->errorString());
    return true;
}

void AuthorizedCommand::refreshToken()
{
    CommandRefreshToken* cmd = new CommandRefreshToken(session());
    connect(cmd, SIGNAL(finished()), SLOT(refreshTokenFinished()));
    connect(cmd, SIGNAL(error(QString)), SLOT(refreshTokenFailed(QString)));
    cmd->exec();
}

void AuthorizedCommand::refreshTokenFinished()
{
    reexecuteQuery();
}

void AuthorizedCommand::refreshTokenFailed(const QString& reason)
{
    emit session()->reauthorizationNeeded(this, reason);
    emit reauthorizationNeeded(reason);
}

void AuthorizedCommand::executeQuery()
{
    emitStarted();
    if (session()->accessToken().isEmpty())
    {
        if (session()->refreshToken().isEmpty())
            refreshTokenFailed(tr("Refresh token is empty, you should to process authorization."));
        else
            refreshToken();
    }
    else
        reexecuteQuery();
}

}
