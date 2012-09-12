#include "command_file_list.h"

#include <QtDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <qjson/parser.h>

#include "session.h"
#include "tools.h"

using namespace GoogleDrive::Internal;

namespace GoogleDrive
{

CommandFileList::CommandFileList(Session *session)
    : AuthorizedCommand(session)
{
}

void CommandFileList::exec(const QString &query)
{
    query_ = query;
    executeQuery();
}

void CommandFileList::execForFolder(const QString& folderId)
{
    exec(QString("'%1' in parents"));
}

void CommandFileList::queryFinished()
{
    tryAutoDelete();
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;
    reply->deleteLater();

    if (checkInvalidReplyAndRefreshToken(reply))
        return;

    QVariantMap map;
    if (!parseJsonReply(reply, map))
        return;

    files_.clear();
    foreach (const QVariant& item, map["items"].toList ())
    {
        files_ << FileInfo(item.toMap());
    }

    emit finished(files_);
    emitFinished();
}

void CommandFileList::reexecuteQuery()
{
    QString queryStr = !query_.isEmpty() ? QString("?q='%1'").arg(query_) : QString();
    QString url = QString("https://www.googleapis.com/drive/v2/files%1")
            .arg(queryStr);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                       "application/x-www-form-urlencoded");
    setRequestAccessToken(request, session()->accessToken());

    QNetworkReply* reply = session()->networkManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(queryFinished()));
}

}
