#include "command.h"

#include <QtDebug>
#include <QNetworkReply>
#include <qjson/parser.h>

#include "session.h"

namespace GoogleDrive
{

Command::Command(Session* session)
    : QObject(session)
    , session_(session)
    , autoDelete_(false)
{
}

void Command::emitError(const QString &msg)
{
    emit error(msg);
    emit session()->error(this, msg);
    qDebug() << metaObject()->className() << "error:" << msg;
}

void Command::tryAutoDelete()
{
    if (autoDelete())
        deleteLater();
}

bool Command::checkJsonReplyError(const QVariantMap& map)
{
    const QString cError("error");
    if (!map.contains(cError))
        return false;

    const QVariantMap& errorMap = map[cError].toMap();
    const QString& code = errorMap["code"].toString();
    const QString& msg = errorMap["message"].toString();

    emitError(tr("(%1) %2").arg(code).arg(msg));
    return true;
}

bool Command::parseJsonReply(QNetworkReply* reply, QVariantMap& map)
{
    bool ok = false;
    QVariant res = QJson::Parser().parse(reply->readAll(), &ok);
    if (!ok)
    {
        emitError(tr("Json reply parse error"));
        return false;
    }
    map = res.toMap();
    return true;
}

bool Command::checkInvalidReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
        return false;

    QVariantMap map;
    if (parseJsonReply(reply, map))
    {
        if (checkJsonReplyError(map))
            return true;
    }
    emitError(reply->errorString());
    return true;
}

void Command::emitFinished()
{
    emit session()->finished(this);
}

void Command::emitStarted()
{
    emit session()->started(this);
}

}
