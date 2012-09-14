#include "command.h"

#include <QtDebug>
#include <QNetworkReply>
#include <qjson/parser.h>

#include "session.h"
#include "command_private.h"

namespace GoogleDrive
{

Command::Command(Session* session)
    : QObject(session)
    , d_ptr(new CommandPrivate)
{
    init();
    Q_D(Command);
    d->session = session;
}

Command::~Command()
{
    delete d_ptr;
}

bool Command::autoDelete() const
{
    Q_D(const Command);
    return d->autoDelete;
}

void Command::setAutoDelete(bool v)
{
    Q_D(Command);
    d->autoDelete = v;
}

Command::Error Command::error() const
{
    Q_D(const Command);
    return d->error;
}

QString Command::errorString() const
{
    Q_D(const Command);
    return d->errorString;
}

void Command::emitError(Error code, const QString &msg)
{
    Q_D(Command);
    d->error = code;
    d->errorString = msg;

    emit error(code);
    emit session()->error(this);
    emit session()->finished(this);
    emit finished();
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

    emitError(UnknownError, tr("(%1) %2").arg(code).arg(msg));
    return true;
}

bool Command::parseJsonReply(QNetworkReply* reply, QVariantMap& map)
{
    bool ok = false;
    QVariant res = QJson::Parser().parse(reply->readAll(), &ok);
    if (!ok)
    {
        emitError(UnknownError, tr("Json reply parse error"));
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
    emitError(UnknownError, reply->errorString());
    return true;
}

Command::Command(CommandPrivate* d, Session *session)
    : QObject(session)
    , d_ptr(d)
{
    init();
    d->session = session;
}

void Command::init()
{
    Q_D(Command);
    d->session = 0;
    d->autoDelete = false;
}

void Command::emitSuccess()
{
    Q_D(Command);
    d->error = NoError;
    d->errorString = QString();
    emit finished();
    emit session()->finished(this);
}

void Command::emitStarted()
{
    emit session()->started(this);
}

Session *Command::session() const
{
    Q_D(const Command);
    return d->session;
}

}
