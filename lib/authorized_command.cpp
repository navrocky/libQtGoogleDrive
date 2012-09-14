#include "authorized_command.h"

#include <QNetworkReply>

#include "command_refresh_token.h"
#include "session.h"

Q_DECLARE_METATYPE(GoogleDrive::CommandRefreshToken*)

namespace GoogleDrive
{

namespace
{
const char* cRefreshCommand = "RefreshCommand";
}

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
        // full reauthorization is needed
        emitError(AuthorizationNeeded, message);
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
    emitError(UnknownError, reply->errorString());
    return true;
}

void AuthorizedCommand::refreshToken()
{
    CommandRefreshToken* cmd = session()->property(cRefreshCommand).value<CommandRefreshToken*>();
    if (!cmd)
    {
        cmd = new CommandRefreshToken(session());
        cmd->exec();
        session()->setProperty(cRefreshCommand, QVariant::fromValue(cmd));
    }
    connect(cmd, SIGNAL(finished()), SLOT(refreshTokenFinished()));
}

AuthorizedCommand::AuthorizedCommand(CommandPrivate* d, Session* s)
    : Command(d, s)
{
}

void AuthorizedCommand::refreshTokenFinished()
{
    CommandRefreshToken* cmd = qobject_cast<CommandRefreshToken*>(sender());
    if (cmd->error() == NoError)
        reexecuteQuery();
    else
        emitError(AuthorizationNeeded, cmd->errorString());
}

void AuthorizedCommand::executeQuery()
{
    emitStarted();
    if (session()->accessToken().isEmpty())
    {
        if (session()->refreshToken().isEmpty())
            emitError(AuthorizationNeeded, tr("Refresh token is empty, "
                                              "you should to process authorization."));
        else
            refreshToken();
    }
    else
        reexecuteQuery();
}

}
