#include "command_download_file.h"

#include <QtDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QByteArray>

#include "session.h"
#include "defines.h"
#include "tools.h"

using namespace GoogleDrive::Internal;

namespace GoogleDrive
{

struct CommandDownloadFile::Impl
{
    Impl()
        : bufferSize(16*1024)
    {}

    qint64 bufferSize;
    QUrl downloadUrl;
    QPointer<QIODevice> out;
};

CommandDownloadFile::CommandDownloadFile(Session* session)
    : AuthorizedCommand(session)
    , d(new Impl)
{
}

CommandDownloadFile::~CommandDownloadFile()
{
    delete d;
}

qint64 CommandDownloadFile::bufferSize() const
{
    return d->bufferSize;
}

void CommandDownloadFile::setBufferSize(qint64 v)
{
    d->bufferSize = v;
}

void CommandDownloadFile::exec(const QUrl &downloadUrl, QIODevice *out)
{
    d->downloadUrl = downloadUrl;
    d->out = out;
    executeQuery();
}

void CommandDownloadFile::reexecuteQuery()
{
    QNetworkRequest request( d->downloadUrl );
    setRequestAccessToken(request, session()->accessToken());
    QNetworkReply* reply = session()->networkManager()->get(request);
    reply->setReadBufferSize(d->bufferSize);
    connect(reply, SIGNAL(finished()), SLOT(requestFinished()));
    connect(reply, SIGNAL(readyRead()), SLOT(readyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), SIGNAL(progress(qint64,qint64)));
}

void CommandDownloadFile::requestFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (!reply)
        return;

    if (reply->error() == QNetworkReply::AuthenticationRequiredError)
    {
        refreshToken();
        return;
    }

    if (checkInvalidReplyAndRefreshToken(reply))
        return;

    emit finished();
    emitFinished();
}

void CommandDownloadFile::readyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (!reply)
        return;

    qint64 sz = reply->bytesAvailable();
    QByteArray ba = reply->read(sz);

    QIODevice* out = d->out;
    out->write(ba);
}

}
