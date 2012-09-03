#include "command_download_file.h"

#include <QtDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QByteArray>

#include "session.h"

namespace GoogleDrive
{

struct CommandDownloadFile::Impl
{
    Impl()
        : bufferSize(16*1024)
        , received(0)
    {}

    qint64 bufferSize;
    qint64 received;
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

qint64 CommandDownloadFile::received() const
{
    return d->received;
}

void CommandDownloadFile::exec(const QUrl &downloadUrl, QIODevice *out)
{
    d->downloadUrl = downloadUrl;
    d->out = out;
    executeQuery();
}

void CommandDownloadFile::reexecuteQuery()
{
    QString str = QString("%1&access_token=%2")
            .arg(d->downloadUrl.toString())
            .arg(session()->accessToken());

    QUrl url(str);

    QNetworkRequest request( url );
    QNetworkReply* reply = session()->networkManager()->get(request);
    reply->setReadBufferSize(d->bufferSize);
    connect(reply, SIGNAL(finished()), SLOT(requestFinished()));
    connect(reply, SIGNAL(readyRead()), SLOT(readyRead()));
}

void CommandDownloadFile::requestFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (!reply)
        return;

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
    d->received += sz;
    emit progress(d->received);
}

}
