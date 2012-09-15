#include "command_file_list.h"

#include <QtDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <qjson/parser.h>

#include "session.h"
#include "tools.h"
#include "command_private.h"

using namespace GoogleDrive::Internal;

namespace GoogleDrive
{

class CommandFileListPrivate : public CommandPrivate
{
public:
    FileInfoList files;
    QString query;
};

CommandFileList::CommandFileList(Session *session)
    : AuthorizedCommand(new CommandFileListPrivate, session)
{
}

FileInfoList CommandFileList::files() const
{
    Q_D(const CommandFileList);
    return d->files;
}

void CommandFileList::exec(const QString &query)
{
    Q_D(CommandFileList);
    d->query = query;
    executeQuery();
}

void CommandFileList::execForFolder(const QString& folderId)
{
    exec(QString("'%1' in parents"));
}

void CommandFileList::queryFinished()
{
    Q_D(CommandFileList);
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

    d->files.clear();
    foreach (const QVariant& item, map["items"].toList ())
    {
        d->files << FileInfo(item.toMap());
    }

    emitSuccess();
}

void CommandFileList::reexecuteQuery()
{
    Q_D(CommandFileList);

    QUrl url("https://www.googleapis.com/drive/v2/files");
    if (!d->query.isEmpty())
        url.addQueryItem("q", d->query);

    QNetworkRequest request(url);
    setRequestAccessToken(request, session()->accessToken());

    QNetworkReply* reply = session()->networkManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(queryFinished()));
}

}
