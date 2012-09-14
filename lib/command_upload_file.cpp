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
#include "command_private.h"

using namespace GoogleDrive::Internal;

namespace GoogleDrive
{

struct CommandUploadFilePrivate : public CommandPrivate
{
public:
    CommandUploadFilePrivate()
        : uploadType(CommandUploadFile::Multipart)
        , fileData(0)
    {
    }

    CommandUploadFile::UploadType uploadType;
    FileInfo fileInfo;
    FileInfo resultFileInfo;
    QIODevice* fileData;
};

CommandUploadFile::CommandUploadFile(Session* session)
    : AuthorizedCommand(new CommandUploadFilePrivate, session)
{
}

CommandUploadFile::UploadType CommandUploadFile::uploadType() const
{
    Q_D(const CommandUploadFile);
    return d->uploadType;
}

void CommandUploadFile::setUploadType(CommandUploadFile::UploadType val)
{
    Q_D(CommandUploadFile);
    d->uploadType = val;
}

const FileInfo &CommandUploadFile::resultFileInfo() const
{
    Q_D(const CommandUploadFile);
    return d->resultFileInfo;
}

void CommandUploadFile::exec(const FileInfo &fileInfo, QIODevice* fileData)
{
    Q_D(CommandUploadFile);

    d->fileInfo = fileInfo;
    d->fileData = fileData;

    executeQuery();
}

void CommandUploadFile::reexecuteQuery()
{
    Q_D(CommandUploadFile);

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
    Q_D(CommandUploadFile);

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
        emitError(UnknownError, tr("File info parse error"));
        return;
    }
    FileInfo fileInfo(res.toMap());
    d->resultFileInfo = fileInfo;

    emitSuccess();
}

void CommandUploadFile::multipartUpload()
{
    Q_D(CommandUploadFile);

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
    dataPart.setBodyDevice(d->fileData);

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
