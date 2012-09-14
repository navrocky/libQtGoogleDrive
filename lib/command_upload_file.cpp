#include "command_upload_file.h"

#include <QIODevice>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QHttpMultiPart>
#include <QtDebug>
#include <qjson/serializer.h>
#include <qjson/parser.h>

#include "session.h"
#include "tools.h"

using namespace GoogleDrive::Internal;

namespace GoogleDrive
{

struct CommandUploadFile::Impl
{
    Impl()
        : uploadType(CommandUploadFile::Multipart)
        , fileData(0)
    {
    }

    CommandUploadFile::UploadType uploadType;
    FileInfo fileInfo;
    QIODevice* fileData;
};

CommandUploadFile::CommandUploadFile(Session* session)
    : AuthorizedCommand(session)
    , d(new Impl)
{
}

CommandUploadFile::~CommandUploadFile()
{
    delete d;
}

CommandUploadFile::UploadType CommandUploadFile::uploadType() const
{
    return d->uploadType;
}

void CommandUploadFile::setUploadType(CommandUploadFile::UploadType val)
{
    d->uploadType = val;
}

void CommandUploadFile::exec(const FileInfo &fileInfo, QIODevice* fileData)
{
    d->fileInfo = fileInfo;
    d->fileData = fileData;

    qDebug() << d->fileInfo.id();
    executeQuery();
}

void CommandUploadFile::reexecuteQuery()
{
    switch (d->uploadType)
    {
        case Multipart: multipartUpload(); break;
        case Resumable:
        {
            Q_ASSERT(false && "Not realized");
        }
    }
}

void CommandUploadFile::requestFinished()
{
    tryAutoDelete();
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;
    reply->deleteLater();

    if (checkInvalidReplyAndRefreshToken(reply))
        return;

    bool ok;
    QVariant res = QJson::Parser().parse(reply, &ok);
    if (!ok)
    {
        emitError(tr("File info parse error"));
        return;
    }
    FileInfo fileInfo(res.toMap());

    emit finished(fileInfo);
    emitFinished();
}

void CommandUploadFile::multipartUpload()
{
    // prepare url
    QString id = !d->fileInfo.id().isEmpty() ?
                QString("/%1").arg(d->fileInfo.id()) :
                QString();

    QString urlStr = QString("https://www.googleapis.com/upload/drive/v2/files%1?uploadType=multipart")
            .arg(id);

    QUrl url(urlStr);

    // prepare multipart body
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::RelatedType);

    QHttpPart metaPart;
    metaPart.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=UTF-8");
    QVariantMap fileInfoMap = d->fileInfo.rawData();
    QByteArray ba = QJson::Serializer().serialize(fileInfoMap);
    metaPart.setBody(ba);

    QHttpPart dataPart;
    QByteArray fileData = d->fileData->readAll();
    dataPart.setBody(fileData);

    multiPart->append(metaPart);
    multiPart->append(dataPart);

    QNetworkRequest request( url );
    setRequestAccessToken(request, session()->accessToken());

    // send request
    QNetworkReply* reply = session()->networkManager()->post(request, multiPart);
    multiPart->setParent(reply);

    connect(reply, SIGNAL(finished()), SLOT(requestFinished()));
}

}
