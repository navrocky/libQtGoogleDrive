#include "command_file_list.h"

#include <QtDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <qjson/parser.h>

#include "session.h"

namespace GoogleDrive
{

CommandFileList::CommandFileList(Session *session)
    : Command(session)
{
}

void CommandFileList::exec(const QString &query)
{
    query_ = query;
    execute();
}

void CommandFileList::queryFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;

    reply->deleteLater();

    bool ok = false;
    QVariant res = QJson::Parser().parse (reply->readAll (), &ok);
    if (!ok)
    {
        throwError(tr("parse error"));
        return;
    }

    QVariantMap resMap = res.toMap();

    if (resMap.contains ("error"))
    {
        // TODO: parse error
//        parseError (res.toMap ());
        return;
    }

    files_.clear();
    foreach (const QVariant& item, resMap["items"].toList ())
    {
        files_ << FileInfo(item.toMap());
    }

    emit finished(files_);
}

void CommandFileList::execute()
{
    QString queryStr = !query_.isEmpty() ? QString("?q='%1'").arg(query_) : QString();
    QString url = QString("https://www.googleapis.com/drive/v2/files?access_token=%1%2")
            .arg(session()->accessToken())
            .arg(queryStr);

    QNetworkRequest request(url);
    request.setHeader (QNetworkRequest::ContentTypeHeader,
                       "application/x-www-form-urlencoded");

    QNetworkReply *reply = session()->networkManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(queryFinished()));
}

}
